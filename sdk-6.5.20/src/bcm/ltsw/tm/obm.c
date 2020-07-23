/* \file obm.c
 *
 * TM OBM Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/types.h>

#include <bcm_int/control.h>

#include <bcm_int/ltsw/tm/obm.h>

#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/switch.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>
#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COSQ

#define OBM_COMPILER_64_TO_32_LO(dst, src)  ((dst) = (uint32_t) (src))
#define OBM_COMPILER_64_TO_32_HI(dst, src)  ((dst) = (uint32_t) ((src) >> 32))
#define OBM_COMPILER_64_ZERO(dst)           ((dst) = 0)
#define OBM_COMPILER_64_COPY(dst, src)      (dst = src)
#define OBM_COMPILER_64_SET(dst, src_hi, src_lo)                \
    ((dst) = (((uint64_t) ((uint32_t)(src_hi))) << 32) | ((uint64_t) ((uint32_t)(src_lo))))

static tm_ltsw_obm_chip_driver_t *tm_obm_chip_driver_cb[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Private functions
 */

/*
 * \brief Set LT TM_OBM_PORT_FLOW_CTRLs.
 *
 * \param [in] unit          Unit Number.
 * \param [in] port          port id.
 * \param [in] lossless_fc   LOSSLESS0_FLOW_CTRL.
 * \param [in] cos_bmap      COS_BMAP_LOSSLESS0.
 * \param [in] max_pri_count max_pri_count.
 * \param [in] priority_list priority_list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
obm_port_flow_ctrl_bmap_set(
    int unit,
    int local_port,
    const char *lossless_fc,
    const char *cos_bmap,
    int max_pri_count,
    int *priority_list)
{
    int array_idx, pri_val;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0*/  {NULL,        BCMI_LT_FIELD_F_ARRAY, 0, {0}},
        /*1*/  {NULL,        BCMI_LT_FIELD_F_ARRAY, 1, {0}},
        /*2*/  {NULL,        BCMI_LT_FIELD_F_ARRAY, 2, {0}},
        /*3*/  {NULL,        BCMI_LT_FIELD_F_ARRAY, 3, {0}},
        /*4*/  {NULL,        BCMI_LT_FIELD_F_ARRAY, 4, {0}},
        /*5*/  {NULL,        BCMI_LT_FIELD_F_ARRAY, 5, {0}},
        /*6*/  {NULL,        BCMI_LT_FIELD_F_ARRAY, 6, {0}},
        /*7*/  {NULL,        BCMI_LT_FIELD_F_ARRAY, 7, {0}},
        /*8*/  {NULL,        BCMI_LT_FIELD_F_SET, 0, {0}},
        /*9*/  {PORT_IDs,    BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    for (array_idx = 0; array_idx < max_pri_count; array_idx++) {
        pri_val = priority_list[array_idx];
        if (pri_val < 0 || pri_val >= 8) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        fields[pri_val].fld_name = cos_bmap;
        fields[pri_val].flags |= BCMI_LT_FIELD_F_SET |
                                 BCMI_LT_FIELD_F_ELE_VALID;
        fields[pri_val].u.val = 1;
    }

    fields[8].fld_name = lossless_fc;
    fields[8].u.val = TRUE;
    fields[9].u.val = local_port;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_OBM_PORT_FLOW_CTRLs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get LT TM_OBM_PORT_FLOW_CTRLs.
 *
 * \param [in] unit          Unit Number.
 * \param [in] port          port id.
 * \param [in] lossless_fc   LOSSLESS0_FLOW_CTRL.
 * \param [in] cos_bmap      COS_BMAP_LOSSLESS0.
 * \param [in] max_pri_count max_pri_count.
 * \param [out] priority_list priority_list.
 * \param [out] pri_count     pri_count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
obm_port_flow_ctrl_bmap_get(
    int unit,
    int local_port,
    const char *cos_bmap,
    int max_pri_count,
    int *priority_list,
    int *pri_count)
{
    int pri_val, array_idx = 0;
    int rv;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0*/  {NULL,        BCMI_LT_FIELD_F_ARRAY, 0, {0}},
        /*1*/  {NULL,        BCMI_LT_FIELD_F_ARRAY, 1, {0}},
        /*2*/  {NULL,        BCMI_LT_FIELD_F_ARRAY, 2, {0}},
        /*3*/  {NULL,        BCMI_LT_FIELD_F_ARRAY, 3, {0}},
        /*4*/  {NULL,        BCMI_LT_FIELD_F_ARRAY, 4, {0}},
        /*5*/  {NULL,        BCMI_LT_FIELD_F_ARRAY, 5, {0}},
        /*6*/  {NULL,        BCMI_LT_FIELD_F_ARRAY, 6, {0}},
        /*7*/  {NULL,        BCMI_LT_FIELD_F_ARRAY, 7, {0}},
        /*8*/  {PORT_IDs,    BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields)/sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[8].u.val = local_port;

    for (pri_val = 0; pri_val < 8; pri_val++) {
        fields[pri_val].fld_name = cos_bmap;
        fields[pri_val].flags |= BCMI_LT_FIELD_F_GET;
    }

    rv = bcmi_lt_entry_get(unit, TM_OBM_PORT_FLOW_CTRLs, &lt_entry, NULL, NULL);

    if (rv == SHR_E_NOT_FOUND) {
        for (pri_val = 0; pri_val < 8 ; pri_val++) {
            if (array_idx == max_pri_count) {
                break;
            }
            priority_list[array_idx++] = 0;
        }
        *pri_count = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    for (pri_val = 0; pri_val < 8 ; pri_val++) {
        if (array_idx == max_pri_count) {
            break;
        }

        if (fields[pri_val].u.val == 1) {
            priority_list[array_idx++] = pri_val;
        }
    }
    *pri_count = array_idx;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Set LT TM_OBM_PORT_FLOW_CTRLs_FLOW_CTRL_TYPEs.
 *
 * \param [in] unit          Unit Number.
 * \param [in] local_port    Port id.
 * \param [in] fc_enable     fc_enable.
 * \param [in] fc_type       fc_type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
obm_port_fc_enable_set(int unit, int local_port, int fc_enable, int fc_type)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,            BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {FLOW_CTRLs,          0, 0, {0}},
         /* 2 */ {FLOW_CTRL_TYPEs,     0, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = local_port;

    if (fc_enable != -1) {
        fields[1].flags = BCMI_LT_FIELD_F_SET;
        fields[1].u.val = fc_enable;
    }

    if (fc_type != -1) {
        fields[2].flags = BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL;
        fields[2].u.sym_val = (fc_type == 0) ? PAUSEs : PFCs;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_OBM_PORT_FLOW_CTRLs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief obm port pkt parse state check.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_state op_state.
 * \param [in] lport logic port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
obm_port_pkt_parse_state_check(int unit, const char *op_state,
                               bcm_port_t lport)
{
    SHR_FUNC_ENTER(unit);

    if (sal_strcasecmp(op_state, VALIDs) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NONE);
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "port %d update error: %s\n"),
                lport, op_state));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_OBM_PORT_PKT_PRI_TC_MAPs state check.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_state op_state.
 * \param [in] lport logic port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
obm_port_pkt_pri_tc_map_state_check(int unit, const char *op_state,
                                    bcm_port_t lport, const char *type,
                                    int pri)
{
    SHR_FUNC_ENTER(unit);

    if (sal_strcasecmp(op_state, VALIDs) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NONE);
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "port %d type = %s priority = %d update error: %s\n"),
                lport, type, pri, op_state));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Internal HSDK functions
 */

int
tm_ltsw_obm_chip_driver_register(int unit, tm_ltsw_obm_chip_driver_t *drv)
{
    tm_obm_chip_driver_cb[unit] = drv;

    return SHR_E_NONE;
}

int
tm_ltsw_obm_chip_driver_deregister(int unit)
{
    tm_obm_chip_driver_cb[unit] = NULL;

    return SHR_E_NONE;
}

int
tm_ltsw_obm_chip_driver_get(int unit, tm_ltsw_obm_chip_driver_t **drv)
{
    *drv = tm_obm_chip_driver_cb[unit];

    return SHR_E_NONE;
}

int
tm_ltsw_obm_port_max_usage_mode_get(
    int unit,
    bcm_port_t port,
    bcm_obm_max_watermark_mode_t *obm_wm_mode)
{
    int local_port;
    int pm_id;
    int rv;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PC_PM_IDs,           BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {MAX_USAGE_MODEs,     BCMI_LT_FIELD_F_GET |
                                       BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(obm_wm_mode, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_pm_id_get(unit, local_port, &pm_id));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = pm_id;

    rv = bcmi_lt_entry_get(unit, TM_OBM_PC_PM_MAX_USAGE_MODEs,
                           &lt_entry, NULL, NULL);

    if (rv == SHR_E_NOT_FOUND) {
        *obm_wm_mode = bcmObmMaxUsageModeAllTraffic;

        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (sal_strcasecmp(fields[1].u.sym_val, OBM_TC_ALLs) == 0) {
        *obm_wm_mode = bcmObmMaxUsageModeAllTraffic;
    } else if (sal_strcasecmp(fields[1].u.sym_val, OBM_TC_LOSSYs) == 0) {
        *obm_wm_mode = bcmObmMaxUsageModeLossy;
    } else if (sal_strcasecmp(fields[1].u.sym_val, OBM_TC_LOSSLESS0s) == 0) {
        *obm_wm_mode = bcmObmMaxUsageModeLossless0;
    } else if (sal_strcasecmp(fields[1].u.sym_val, OBM_TC_LOSSLESS1s) == 0) {
        *obm_wm_mode = bcmObmMaxUsageModeLossless1;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_obm_port_max_usage_mode_set(
    int unit,
    bcm_port_t port,
    bcm_obm_max_watermark_mode_t obm_wm_mode)
{
    int local_port;
    int pm_id;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PC_PM_IDs,           BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {MAX_USAGE_MODEs,     BCMI_LT_FIELD_F_SET |
                                       BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_pm_id_get(unit, local_port, &pm_id));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = pm_id;

    switch(obm_wm_mode) {
        case bcmObmMaxUsageModeAllTraffic:
            fields[1].u.sym_val = OBM_TC_ALLs;
            break;
        case bcmObmMaxUsageModeLossy:
            fields[1].u.sym_val = OBM_TC_LOSSYs;
            break;
        case bcmObmMaxUsageModeLossless0:
            fields[1].u.sym_val = OBM_TC_LOSSLESS0s;
            break;
        case bcmObmMaxUsageModeLossless1:
            fields[1].u.sym_val = OBM_TC_LOSSLESS1s;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_OBM_PC_PM_MAX_USAGE_MODEs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_obm_traffic_class_pfc_priority_mapping_get(
    int unit,
    bcm_port_t port,
    bcm_obm_traffic_class_t obm_traffic_class,
    int max_pri_count,
    int *priority_list,
    int *pri_count)
{
    int local_port;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(priority_list, SHR_E_PARAM);

    SHR_NULL_CHECK(pri_count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &local_port));

    if (max_pri_count <= 0 || max_pri_count > 8) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    switch (obm_traffic_class) {
        case bcmObmClassLossless0:
            SHR_IF_ERR_VERBOSE_EXIT
                (obm_port_flow_ctrl_bmap_get(unit, local_port, COS_BMAP_LOSSLESS0s,
                                             max_pri_count, priority_list,
                                             pri_count));
            break;
        case bcmObmClassLossless1:
            SHR_IF_ERR_VERBOSE_EXIT
                (obm_port_flow_ctrl_bmap_get(unit, local_port, COS_BMAP_LOSSLESS1s,
                                             max_pri_count, priority_list,
                                             pri_count));
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_obm_traffic_class_pfc_priority_mapping_set(
    int unit,
    bcm_port_t port,
    bcm_obm_traffic_class_t obm_traffic_class,
    int max_pri_count,
    int *priority_list)
{
    int local_port;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(priority_list, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &local_port));

    if (max_pri_count <= 0 || max_pri_count > 8) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    switch (obm_traffic_class) {
        case bcmObmClassLossless0:
            SHR_IF_ERR_VERBOSE_EXIT
                (obm_port_flow_ctrl_bmap_set(unit, local_port, LOSSLESS0_FLOW_CTRLs,
                                             COS_BMAP_LOSSLESS0s, max_pri_count,
                                             priority_list));
            break;
        case bcmObmClassLossless1:
            SHR_IF_ERR_VERBOSE_EXIT
                (obm_port_flow_ctrl_bmap_set(unit, local_port, LOSSLESS1_FLOW_CTRLs,
                                             COS_BMAP_LOSSLESS1s, max_pri_count,
                                             priority_list));
            break;
        default:
            return BCM_E_PARAM;
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_obm_init(int unit)
{
    return SHR_E_NONE;
}

int
tm_ltsw_obm_detach(int unit)
{
    int warm = bcmi_warmboot_get(unit);

    if (!warm) {
        (void)bcmi_lt_clear(unit, TM_OBM_PC_PM_MAX_USAGE_MODEs);

        (void)bcmi_lt_clear(unit, TM_OBM_PORT_PKT_PRI_TC_MAPs);

        (void)bcmi_lt_clear(unit, TM_OBM_PORT_FLOW_CTRLs);

        (void)bcmi_lt_clear(unit, TM_OBM_PORT_PKT_PARSEs);
    }

    return SHR_E_NONE;
}

int
tm_ltsw_obm_port_fc_enable_set(
    int unit,
    int lport,
    int fc_type)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, lport, &lport));

    SHR_IF_ERR_VERBOSE_EXIT
        (obm_port_fc_enable_set(unit, lport, -1, fc_type));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_obm_port_pkt_parse_get(
    int unit,
    bcm_port_t port,
    bcmi_ltsw_obm_port_control_t type,
    int *arg,
    int check_state)
{
    int local_port;
    int rv;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,            BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {HEADER_TYPEs,        BCMI_LT_FIELD_F_GET |
                                       BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
         /* 2 */ {DSCP_MAPs,           BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 3 */ {MPLS_MAPs,           BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 4 */ {ETAG_MAPs,           BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 5 */ {INNER_TPIDs,         BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 6 */ {OUTER_TPIDs,         BCMI_LT_FIELD_F_GET |
                                       BCMI_LT_FIELD_F_ARRAY, 0, {0}},
         /* 7 */ {OUTER_TPIDs,         BCMI_LT_FIELD_F_GET |
                                       BCMI_LT_FIELD_F_ARRAY, 1, {0}},
         /* 8 */ {OUTER_TPIDs,         BCMI_LT_FIELD_F_GET |
                                       BCMI_LT_FIELD_F_ARRAY, 2, {0}},
         /* 9 */ {OUTER_TPIDs,         BCMI_LT_FIELD_F_GET |
                                       BCMI_LT_FIELD_F_ARRAY, 3, {0}},
         /* 10 */ {DEFAULT_PKT_PRIs,   BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 11 */ {OPERATIONAL_STATEs, BCMI_LT_FIELD_F_GET |
                                       BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &local_port));

    if (bcmi_ltsw_port_is_type(unit, local_port, BCMI_LTSW_PORT_TYPE_CPU |
                                                 BCMI_LTSW_PORT_TYPE_MGMT |
                                                 BCMI_LTSW_PORT_TYPE_LB)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PORT);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = local_port;

    rv = bcmi_lt_entry_get(unit, TM_OBM_PORT_PKT_PARSEs,
                           &lt_entry, NULL, NULL);

    if (!check_state) {
        SHR_NULL_CHECK(arg, SHR_E_PARAM);

        if (rv == SHR_E_NOT_FOUND) {
            *arg = 0;
            SHR_EXIT();
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }

        switch (type) {
            case bcmiObmPortControlHeaderType:
                if (sal_strcasecmp(fields[1].u.sym_val,
                                   OBM_HEADER_TYPE_ETHERNETs) == 0) {
                    *arg = 0;
                } else {
                    *arg = 1;
                }

                break;
            case bcmiObmPortControlDscpMapEnable:
                *arg = fields[2].u.val;
                break;
            case bcmiObmPortControlMplsMapEnable:
                *arg = fields[3].u.val;
                break;
            case bcmiObmPortControlEtagMapEnable:
                *arg = fields[4].u.val;
                break;
            case bcmiObmPortControlInnerTpidEnable:
                *arg = fields[5].u.val;
                break;
            case bcmiObmPortControlOuterTpid0Enable:
                *arg = fields[6].u.val;
                break;
            case bcmiObmPortControlOuterTpid1Enable:
                *arg = fields[7].u.val;
                break;
            case bcmiObmPortControlOuterTpid2Enable:
                *arg = fields[8].u.val;
                break;
            case bcmiObmPortControlOuterTpid3Enable:
                *arg = fields[9].u.val;
                break;
            case bcmiObmPortControlDefaultPktPri:
                *arg = fields[10].u.val;
                break;
            default:
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        SHR_IF_ERR_VERBOSE_EXIT
            (obm_port_pkt_parse_state_check(unit, fields[11].u.sym_val,
                                            local_port));
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_obm_port_pkt_parse_set(
    int unit,
    bcm_port_t port,
    bcmi_ltsw_obm_port_control_t type,
    int arg)
{
    int local_port;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,            BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {NULL,                0, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &local_port));

    if (bcmi_ltsw_port_is_type(unit, local_port, BCMI_LTSW_PORT_TYPE_CPU |
                                                 BCMI_LTSW_PORT_TYPE_MGMT |
                                                 BCMI_LTSW_PORT_TYPE_LB)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PORT);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = local_port;

    switch (type) {
        case bcmiObmPortControlHeaderType:
            if (arg < 0 || arg > 1) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            fields[1].fld_name = HEADER_TYPEs;
            fields[1].u.sym_val = (arg == 0) ? OBM_HEADER_TYPE_ETHERNETs :
                                  OBM_HEADER_TYPE_GENERIC_STACKING_HEADERs;
            fields[1].flags = BCMI_LT_FIELD_F_SET |
                              BCMI_LT_FIELD_F_SYMBOL;

            break;
        case bcmiObmPortControlDscpMapEnable:
            if (arg < 0 || arg > 1) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            fields[1].fld_name = DSCP_MAPs;
            fields[1].u.val = arg;
            fields[1].flags = BCMI_LT_FIELD_F_SET;

            break;
        case bcmiObmPortControlMplsMapEnable:
            if (arg < 0 || arg > 1) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            fields[1].fld_name = MPLS_MAPs;
            fields[1].u.val = arg;
            fields[1].flags = BCMI_LT_FIELD_F_SET;

            break;
        case bcmiObmPortControlEtagMapEnable:
            if (arg < 0 || arg > 1) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            fields[1].fld_name = ETAG_MAPs;
            fields[1].u.val = arg;
            fields[1].flags = BCMI_LT_FIELD_F_SET;

            break;
        case bcmiObmPortControlInnerTpidEnable:
            if (arg < 0 || arg > 1) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            fields[1].fld_name = INNER_TPIDs;
            fields[1].u.val = arg;
            fields[1].flags = BCMI_LT_FIELD_F_SET;

            break;
        case bcmiObmPortControlOuterTpid0Enable:
            if (arg < 0 || arg > 1) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            fields[1].fld_name = OUTER_TPIDs;
            fields[1].u.val = arg;
            fields[1].flags = BCMI_LT_FIELD_F_SET |
                              BCMI_LT_FIELD_F_ARRAY |
                              BCMI_LT_FIELD_F_ELE_VALID;
            fields[1].idx = 0;

            break;
        case bcmiObmPortControlOuterTpid1Enable:
            if (arg < 0 || arg > 1) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            fields[1].fld_name = OUTER_TPIDs;
            fields[1].u.val = arg;
            fields[1].flags = BCMI_LT_FIELD_F_SET |
                              BCMI_LT_FIELD_F_ARRAY |
                              BCMI_LT_FIELD_F_ELE_VALID;
            fields[1].idx = 1;

            break;
        case bcmiObmPortControlOuterTpid2Enable:
            if (arg < 0 || arg > 1) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            fields[1].fld_name = OUTER_TPIDs;
            fields[1].u.val = arg;
            fields[1].flags = BCMI_LT_FIELD_F_SET |
                              BCMI_LT_FIELD_F_ARRAY |
                              BCMI_LT_FIELD_F_ELE_VALID;
            fields[1].idx = 2;

            break;
        case bcmiObmPortControlOuterTpid3Enable:
            if (arg < 0 || arg > 1) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            fields[1].fld_name = OUTER_TPIDs;
            fields[1].u.val = arg;
            fields[1].flags = BCMI_LT_FIELD_F_SET |
                              BCMI_LT_FIELD_F_ARRAY |
                              BCMI_LT_FIELD_F_ELE_VALID;
            fields[1].idx = 3;

            break;
        case bcmiObmPortControlDefaultPktPri:
            if (arg < 0 || arg > 7) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            fields[1].fld_name = DEFAULT_PKT_PRIs;
            fields[1].u.val = arg;
            fields[1].flags = BCMI_LT_FIELD_F_SET;

            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_OBM_PORT_PKT_PARSEs,
                           &lt_entry, NULL));

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_obm_port_pkt_parse_get(unit, local_port, type, NULL, true));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_obm_pm_pkt_parse_tpid_get(
    int unit,
    int pm_id,
    bcmi_ltsw_obm_port_control_t type,
    int *arg)
{
    int rv;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PC_PM_IDs,           BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {INNER_TPIDs,         BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 2 */ {OUTER_TPIDs,         BCMI_LT_FIELD_F_GET |
                                       BCMI_LT_FIELD_F_ARRAY, 0, {0}},
         /* 3 */ {OUTER_TPIDs,         BCMI_LT_FIELD_F_GET |
                                       BCMI_LT_FIELD_F_ARRAY, 1, {0}},
         /* 4 */ {OUTER_TPIDs,         BCMI_LT_FIELD_F_GET |
                                       BCMI_LT_FIELD_F_ARRAY, 2, {0}},
         /* 5 */ {OUTER_TPIDs,         BCMI_LT_FIELD_F_GET |
                                       BCMI_LT_FIELD_F_ARRAY, 3, {0}},
    };

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = pm_id;

    rv = bcmi_lt_entry_get(unit, TM_OBM_PC_PM_PKT_PARSEs,
                           &lt_entry, NULL, NULL);

    if (rv == SHR_E_NOT_FOUND) {
        *arg = 0;

        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    switch (type) {
        case bcmiObmPortControlInnerTpid:
            *arg = fields[1].u.val;
            break;
        case bcmiObmPortControlOuterTpid0:
            *arg = fields[2].u.val;
            break;
        case bcmiObmPortControlOuterTpid1:
            *arg = fields[3].u.val;
            break;
        case bcmiObmPortControlOuterTpid2:
            *arg = fields[4].u.val;
            break;
        case bcmiObmPortControlOuterTpid3:
            *arg = fields[5].u.val;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_obm_pm_pkt_parse_tpid_set(
    int unit,
    int pm_id,
    bcmi_ltsw_obm_port_control_t type,
    int arg)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PC_PM_IDs,           BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {NULL,                0, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = pm_id;

    switch (type) {
        case bcmiObmPortControlInnerTpid:
            if (arg < 0) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            fields[1].fld_name = INNER_TPIDs;
            fields[1].u.val = arg;
            fields[1].flags = BCMI_LT_FIELD_F_SET;

            break;
        case bcmiObmPortControlOuterTpid0:
            if (arg < 0) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            fields[1].fld_name = OUTER_TPIDs;
            fields[1].u.val = arg;
            fields[1].flags = BCMI_LT_FIELD_F_SET |
                              BCMI_LT_FIELD_F_ARRAY |
                              BCMI_LT_FIELD_F_ELE_VALID;
            fields[1].idx = 0;

            break;
        case bcmiObmPortControlOuterTpid1:
            if (arg < 0) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            fields[1].fld_name = OUTER_TPIDs;
            fields[1].u.val = arg;
            fields[1].flags = BCMI_LT_FIELD_F_SET |
                              BCMI_LT_FIELD_F_ARRAY |
                              BCMI_LT_FIELD_F_ELE_VALID;
            fields[1].idx = 1;

            break;
        case bcmiObmPortControlOuterTpid2:
            if (arg < 0) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            fields[1].fld_name = OUTER_TPIDs;
            fields[1].u.val = arg;
            fields[1].flags = BCMI_LT_FIELD_F_SET |
                              BCMI_LT_FIELD_F_ARRAY |
                              BCMI_LT_FIELD_F_ELE_VALID;
            fields[1].idx = 2;

            break;
        case bcmiObmPortControlOuterTpid3:
            if (arg < 0) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            fields[1].fld_name = OUTER_TPIDs;
            fields[1].u.val = arg;
            fields[1].flags = BCMI_LT_FIELD_F_SET |
                              BCMI_LT_FIELD_F_ARRAY |
                              BCMI_LT_FIELD_F_ELE_VALID;
            fields[1].idx = 3;

            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_OBM_PC_PM_PKT_PARSEs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_obm_pm_pkt_parse_ethertype_get(
    int unit,
    int pm_id,
    bcmi_ltsw_obm_port_control_t type,
    int *arg)
{
    int rv;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PC_PM_IDs,              BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {ETAG_PARSEs,            BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 2 */ {ETAG_ETHERTYPEs,        BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 3 */ {VNTAG_PARSEs,           BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 4 */ {VNTAG_ETHERTYPEs,       BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 5 */ {HIGIG3_ETHERTYPEs,      BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 6 */ {HIGIG3_ETHERTYPE_MASKs, BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = pm_id;

    rv = bcmi_lt_entry_get(unit, TM_OBM_PC_PM_PKT_PARSEs,
                           &lt_entry, NULL, NULL);

    if (rv == SHR_E_NOT_FOUND) {
        *arg = 0;

        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    switch (type) {
        case bcmiObmPortControlEtagEthertype:
            *arg = fields[2].u.val;
            break;
        case bcmiObmPortControlVntagEthertype:
            *arg = fields[4].u.val;
            break;
        case bcmiObmPortControlHigig3Ethertype:
            *arg = fields[5].u.val;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_obm_pm_pkt_parse_ethertype_set(
    int unit,
    int pm_id,
    bcmi_ltsw_obm_port_control_t type,
    int arg)
{
    bool enable = false;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PC_PM_IDs,              BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {ETAG_PARSEs,            0, 0, {0}},
         /* 2 */ {ETAG_ETHERTYPEs,        0, 0, {0}},
         /* 3 */ {VNTAG_PARSEs,           0, 0, {0}},
         /* 4 */ {VNTAG_ETHERTYPEs,       0, 0, {0}},
         /* 5 */ {HIGIG3_ETHERTYPEs,      0, 0, {0}},
         /* 6 */ {HIGIG3_ETHERTYPE_MASKs, 0, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = pm_id;

    switch (type) {
        case bcmiObmPortControlEtagEthertype:
            if (arg < 0) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            enable = (arg == 0) ? false : true;
            fields[1].u.val = enable;
            fields[1].flags = BCMI_LT_FIELD_F_SET;
            fields[2].u.val = arg;
            fields[2].flags = BCMI_LT_FIELD_F_SET;

            break;
        case bcmiObmPortControlVntagEthertype:
            if (arg < 0) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            enable = (arg == 0) ? false : true;
            fields[3].u.val = enable;
            fields[3].flags = BCMI_LT_FIELD_F_SET;
            fields[4].u.val = arg;
            fields[4].flags = BCMI_LT_FIELD_F_SET;

            break;
        case bcmiObmPortControlHigig3Ethertype:
            if (arg < 0) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            enable = (arg == 0) ? false : true;
            fields[5].u.val = arg << 6;
            fields[5].flags = BCMI_LT_FIELD_F_SET;
            fields[6].u.val = 0xFFC0;
            fields[6].flags = BCMI_LT_FIELD_F_SET;

            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_OBM_PC_PM_PKT_PARSEs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_obm_pm_pkt_parse_user_define_ethertype_get(
    int unit,
    bcm_port_t port,
    int index,
    bcm_switch_obm_classifier_t *switch_obm_classifier)
{
    int local_port;
    int pm_id, i;
    uint64_t rval64;
    uint32_t rval_hi, rval_lo;
    int rv;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PC_PM_IDs,              BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {USER_DEFINED_PROTOCOL_MATCHs,
                                          BCMI_LT_FIELD_F_GET |
                                          BCMI_LT_FIELD_F_ARRAY, 0, {0}},
         /* 2 */ {USER_DEFINED_PROTOCOL_ETHERTYPEs,
                                          BCMI_LT_FIELD_F_GET |
                                          BCMI_LT_FIELD_F_ARRAY, 0, {0}},
         /* 3 */ {USER_DEFINED_PROTOCOL_ETHERTYPE_MASKs,
                                          BCMI_LT_FIELD_F_GET |
                                          BCMI_LT_FIELD_F_ARRAY, 0, {0}},
         /* 4 */ {USER_DEFINED_PROTOCOL_DST_MACs,
                                          BCMI_LT_FIELD_F_GET |
                                          BCMI_LT_FIELD_F_ARRAY, 0, {0}},
         /* 5 */ {USER_DEFINED_PROTOCOL_DST_MAC_MASKs,
                                          BCMI_LT_FIELD_F_GET |
                                          BCMI_LT_FIELD_F_ARRAY, 0, {0}},
         /* 6 */ {USER_DEFINED_PROTOCOL_TRAFFIC_CLASSs,
                                          BCMI_LT_FIELD_F_GET |
                                          BCMI_LT_FIELD_F_ARRAY, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(switch_obm_classifier, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_pm_id_get(unit, local_port, &pm_id));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = pm_id;

    for (i = 1; i <= 6; i++) {
        fields[i].idx = index;
    }

    rv = bcmi_lt_entry_get(unit, TM_OBM_PC_PM_PKT_PARSEs,
                           &lt_entry, NULL, NULL);

    if (rv == SHR_E_NOT_FOUND) {
        switch_obm_classifier->obm_destination_mac[0] = 0;
        switch_obm_classifier->obm_destination_mac[1] = 0;
        switch_obm_classifier->obm_destination_mac[2] = 0;
        switch_obm_classifier->obm_destination_mac[3] = 0;
        switch_obm_classifier->obm_destination_mac[4] = 0;
        switch_obm_classifier->obm_destination_mac[5] = 0;
        switch_obm_classifier->obm_destination_mac_mask[0] = 0;
        switch_obm_classifier->obm_destination_mac_mask[1] = 0;
        switch_obm_classifier->obm_destination_mac_mask[2] = 0;
        switch_obm_classifier->obm_destination_mac_mask[3] = 0;
        switch_obm_classifier->obm_destination_mac_mask[4] = 0;
        switch_obm_classifier->obm_destination_mac_mask[5] = 0;
        switch_obm_classifier->obm_ethertype = 0;
        switch_obm_classifier->obm_ethertype_mask = 0;
        switch_obm_classifier->obm_code_point = 0;
        switch_obm_classifier->obm_priority = 0;

        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    switch_obm_classifier->obm_ethertype = fields[2].u.val;
    switch_obm_classifier->obm_ethertype_mask = fields[3].u.val;
    OBM_COMPILER_64_ZERO(rval64);
    OBM_COMPILER_64_COPY(rval64, fields[4].u.val);
    OBM_COMPILER_64_TO_32_HI(rval_hi, rval64);
    OBM_COMPILER_64_TO_32_LO(rval_lo, rval64);
    switch_obm_classifier->obm_destination_mac[0] = (rval_hi >> 8) & 0xff;
    switch_obm_classifier->obm_destination_mac[1] = rval_hi & 0xff;
    switch_obm_classifier->obm_destination_mac[2] = (rval_lo >> 24) & 0xff;
    switch_obm_classifier->obm_destination_mac[3] = (rval_lo >> 16) & 0xff;
    switch_obm_classifier->obm_destination_mac[4] = (rval_lo >> 8) & 0xff;
    switch_obm_classifier->obm_destination_mac[5] = rval_lo & 0xff;
    OBM_COMPILER_64_ZERO(rval64);
    OBM_COMPILER_64_COPY(rval64, fields[5].u.val);
    OBM_COMPILER_64_TO_32_HI(rval_hi, rval64);
    OBM_COMPILER_64_TO_32_LO(rval_lo, rval64);
    switch_obm_classifier->obm_destination_mac_mask[0] = (rval_hi >> 8) & 0xff;
    switch_obm_classifier->obm_destination_mac_mask[1] = rval_hi & 0xff;
    switch_obm_classifier->obm_destination_mac_mask[2] = (rval_lo >> 24) & 0xff;
    switch_obm_classifier->obm_destination_mac_mask[3] = (rval_lo >> 16) & 0xff;
    switch_obm_classifier->obm_destination_mac_mask[4] = (rval_lo >> 8) & 0xff;
    switch_obm_classifier->obm_destination_mac_mask[5] = rval_lo & 0xff;
    switch_obm_classifier->obm_priority = fields[6].u.val;

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_obm_pm_pkt_parse_user_define_ethertype_set(
    int unit,
    bcm_port_t port,
    int index,
    bcm_switch_obm_classifier_t *switch_obm_classifier)
{
    int local_port;
    int pm_id;
    uint64_t rval64;
    uint32_t rval_hi, rval_lo;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PC_PM_IDs,              BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {USER_DEFINED_PROTOCOL_MATCHs,
                                          BCMI_LT_FIELD_F_SET |
                                          BCMI_LT_FIELD_F_ARRAY |
                                          BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
         /* 2 */ {USER_DEFINED_PROTOCOL_ETHERTYPEs,
                                          BCMI_LT_FIELD_F_SET |
                                          BCMI_LT_FIELD_F_ARRAY |
                                          BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
         /* 3 */ {USER_DEFINED_PROTOCOL_ETHERTYPE_MASKs,
                                          BCMI_LT_FIELD_F_SET |
                                          BCMI_LT_FIELD_F_ARRAY |
                                          BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
         /* 4 */ {USER_DEFINED_PROTOCOL_DST_MACs,
                                          BCMI_LT_FIELD_F_SET |
                                          BCMI_LT_FIELD_F_ARRAY |
                                          BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
         /* 5 */ {USER_DEFINED_PROTOCOL_DST_MAC_MASKs,
                                          BCMI_LT_FIELD_F_SET |
                                          BCMI_LT_FIELD_F_ARRAY |
                                          BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
         /* 6 */ {USER_DEFINED_PROTOCOL_TRAFFIC_CLASSs,
                                          BCMI_LT_FIELD_F_SET |
                                          BCMI_LT_FIELD_F_ARRAY |
                                          BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_pm_id_get(unit, local_port, &pm_id));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = pm_id;
    fields[1].u.val = true;
    fields[1].idx = index;
    fields[2].u.val = switch_obm_classifier->obm_ethertype;
    fields[2].idx = index;
    fields[3].u.val = switch_obm_classifier->obm_ethertype_mask;
    fields[3].idx = index;
    OBM_COMPILER_64_ZERO(rval64);
    OBM_COMPILER_64_TO_32_HI(rval_hi, rval64);
    OBM_COMPILER_64_TO_32_LO(rval_lo, rval64);
    rval_hi |= ((switch_obm_classifier->obm_destination_mac[0] & 0xff) << 8);
    rval_hi |= (switch_obm_classifier->obm_destination_mac[1] & 0xff);
    rval_lo |= ((switch_obm_classifier->obm_destination_mac[2] & 0xff) << 24);
    rval_lo |= ((switch_obm_classifier->obm_destination_mac[3] & 0xff) << 16);
    rval_lo |= ((switch_obm_classifier->obm_destination_mac[4] & 0xff) << 8);
    rval_lo |= (switch_obm_classifier->obm_destination_mac[5] & 0xff);
    OBM_COMPILER_64_SET(rval64, rval_hi, rval_lo);
    fields[4].u.val = rval64;
    fields[4].idx = index;
    OBM_COMPILER_64_ZERO(rval64);
    OBM_COMPILER_64_TO_32_HI(rval_hi, rval64);
    OBM_COMPILER_64_TO_32_LO(rval_lo, rval64);
    rval_hi |= ((switch_obm_classifier->obm_destination_mac_mask[0] & 0xff) << 8);
    rval_hi |= (switch_obm_classifier->obm_destination_mac_mask[1] & 0xff);
    rval_lo |= ((switch_obm_classifier->obm_destination_mac_mask[2] & 0xff) << 24);
    rval_lo |= ((switch_obm_classifier->obm_destination_mac_mask[3] & 0xff) << 16);
    rval_lo |= ((switch_obm_classifier->obm_destination_mac_mask[4] & 0xff) << 8);
    rval_lo |= (switch_obm_classifier->obm_destination_mac_mask[5] & 0xff);
    OBM_COMPILER_64_SET(rval64, rval_hi, rval_lo);
    fields[5].u.val = rval64;
    fields[5].idx = index;
    fields[6].u.val = switch_obm_classifier->obm_priority;;
    fields[6].idx = index;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_OBM_PC_PM_PKT_PARSEs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_obm_port_pkt_pri_tc_map_get(
    int unit,
    bcm_port_t port,
    bcm_switch_obm_classifier_type_t piority_type,
    int priority,
    bcm_switch_obm_priority_t *obm_tc,
    int check_state)
{
    int local_port;
    int is_higig_port = 0;
    int rv;
    tm_ltsw_obm_chip_driver_t *drv = NULL;
    const char *sym_val = NULL;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,            BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {PKT_PRI_TYPEs,       BCMI_LT_FIELD_F_SET |
                                       BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
         /* 2 */ {PKT_PRIs,            BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 3 */ {TRAFFIC_CLASSs,      BCMI_LT_FIELD_F_GET |
                                       BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
         /* 4 */ {OPERATIONAL_STATEs,  BCMI_LT_FIELD_F_GET |
                                       BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };


    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &local_port));

    if (priority < 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    is_higig_port = bcmi_ltsw_port_is_type(unit, local_port,
                                           BCMI_LTSW_PORT_TYPE_HG);

    if (is_higig_port) {
        if (piority_type == bcmSwitchObmClassifierVlan) {
            LOG_ERROR(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                     "ERROR: OBM Vlan classifier not allowed on Higig"
                     " port %d\n"), local_port));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    } else {
        if (piority_type == bcmSwitchObmClassifierHigig3) {
            LOG_ERROR(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                     "ERROR: OBM Higig3 classifier not allowed on non Higig"
                     " port %d\n"), local_port));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_obm_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);

    if (drv->pkt_pri_type_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (drv->pkt_pri_type_get(unit, piority_type, &sym_val));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = local_port;
    fields[1].u.sym_val = sym_val;
    fields[2].u.val = priority;

    rv = bcmi_lt_entry_get(unit, TM_OBM_PORT_PKT_PRI_TC_MAPs,
                           &lt_entry, NULL, NULL);

    if (!check_state) {
        SHR_NULL_CHECK(obm_tc, SHR_E_PARAM);

        if (rv == SHR_E_NOT_FOUND) {
            *obm_tc = BCM_SWITCH_OBM_PRIORITY_LOSSY_LOW;
            SHR_EXIT();
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }

        if (sal_strcasecmp(fields[3].u.sym_val, OBM_TC_LOSSY_LOWs) == 0) {
            *obm_tc = BCM_SWITCH_OBM_PRIORITY_LOSSY_LOW;
        } else if (sal_strcasecmp(fields[3].u.sym_val, OBM_TC_LOSSY_HIGHs) == 0) {
            *obm_tc = BCM_SWITCH_OBM_PRIORITY_LOSSY_HIGH;
        } else if (sal_strcasecmp(fields[3].u.sym_val, OBM_TC_LOSSLESS0s) == 0) {
            *obm_tc = BCM_SWITCH_OBM_PRIORITY_LOSSLESS0;
        } else if (sal_strcasecmp(fields[3].u.sym_val, OBM_TC_LOSSLESS1s) == 0) {
            *obm_tc = BCM_SWITCH_OBM_PRIORITY_LOSSLESS1;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
        SHR_IF_ERR_VERBOSE_EXIT
            (obm_port_pkt_pri_tc_map_state_check(unit, fields[4].u.sym_val,
                                                 local_port, fields[1].u.sym_val,
                                                 fields[2].u.val));
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_obm_port_pkt_pri_tc_map_set(
    int unit,
    bcm_port_t port,
    bcm_switch_obm_classifier_type_t piority_type,
    int priority,
    bcm_switch_obm_priority_t obm_tc)
{
    int local_port;
    int is_higig_port = 0;
    tm_ltsw_obm_chip_driver_t *drv = NULL;
    const char *sym_val = NULL;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,            BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {PKT_PRI_TYPEs,       BCMI_LT_FIELD_F_SET |
                                       BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
         /* 2 */ {PKT_PRIs,            BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 3 */ {TRAFFIC_CLASSs,      BCMI_LT_FIELD_F_SET |
                                       BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &local_port));

    if (priority < 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    is_higig_port = bcmi_ltsw_port_is_type(unit, local_port,
                                           BCMI_LTSW_PORT_TYPE_HG);

    if (is_higig_port) {
        if (piority_type == bcmSwitchObmClassifierVlan) {
            LOG_ERROR(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                     "ERROR: OBM Vlan classifier not allowed on Higig"
                     " port %d\n"), local_port));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    } else {
        if (piority_type == bcmSwitchObmClassifierHigig3) {
            LOG_ERROR(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                     "ERROR: OBM Higig3 classifier not allowed on non Higig"
                     " port %d\n"), local_port));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_obm_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);

    if (drv->pkt_pri_type_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (drv->pkt_pri_type_get(unit, piority_type, &sym_val));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = local_port;
    fields[1].u.sym_val = sym_val;
    fields[2].u.val = priority;

    switch(obm_tc) {
        case BCM_SWITCH_OBM_PRIORITY_LOSSY_LOW:
            fields[3].u.sym_val = OBM_TC_LOSSY_LOWs;
            break;
        case BCM_SWITCH_OBM_PRIORITY_LOSSY_HIGH:
            fields[3].u.sym_val = OBM_TC_LOSSY_HIGHs;
            break;
        case BCM_SWITCH_OBM_PRIORITY_LOSSLESS0:
            fields[3].u.sym_val = OBM_TC_LOSSLESS0s;
            break;
        case BCM_SWITCH_OBM_PRIORITY_LOSSLESS1:
            fields[3].u.sym_val = OBM_TC_LOSSLESS1s;
            break;
        case BCM_SWITCH_OBM_PRIORITY_EXPRESS:
            SHR_IF_ERR_VERBOSE_EXIT(_SHR_E_UNAVAIL);
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_OBM_PORT_PKT_PRI_TC_MAPs,
                           &lt_entry, NULL));

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_obm_port_pkt_pri_tc_map_get(unit, local_port, piority_type,
                                             priority, NULL, true));

exit:
    SHR_FUNC_EXIT();
}

