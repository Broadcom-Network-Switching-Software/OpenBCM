/* \file cosq_thd.c
 *
 * TM COSQ THD Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/control.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/mbcm/cosq_thd.h>
#include <bcm_int/ltsw/tm/cosq_thd.h>
#include <bcm_int/ltsw/cosq_thd.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/pfc.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COSQ

/*
 * \brief Index style.
 */
typedef enum cosq_thd_index_style_s {

    /* ucast queue. */
    COSQ_THD_INDEX_STYLE_UCAST_QUEUE = 0,

    /* mcast queue. */
    COSQ_THD_INDEX_STYLE_MCAST_QUEUE = 1,

    /* egress pool. */
    COSQ_THD_INDEX_STYLE_EGR_POOL = 2,

    /* max value. */
    COSQ_THD_INDEX_STYLE_COUNT = 3

} cosq_thd_index_style_t;

/*
 * \brief Service pool information.
 */
typedef struct cosq_thd_service_pool_info_s {

    /* Shared buffer limit in cells. */
    int shared_limit;

    /* Static transmit resume limit in cells. */
    int resume_limit;

    /* Shared buffer limit for yellow packets in cells. */
    int yellow_limit;

    /* Transmission resume limit (in cells) for yellow packets. */
    int yellow_resume;

    /* Shared buffer limit for red packets in cells.. */
    int red_limit;

    /* Transmission resume limit (in cells) for red packets. */
    int red_resume;

    /* Enable to drop packets based on color specific shared buffer limits. */
    int enable_color_limit;

} cosq_thd_service_pool_info_t;

#define ITMS_PER_DEV                2
#define MMU_PACKET_HEADER_BYTES     40
#define IPG_SIZE_BYTES              20
#define PFC_FRAME_SIZE_BYTES        64
#define MMU_NUM_PG                  8
#define MMU_NUM_POOL                4
#define MMU_NUM_INT_PRI             16
#define MMU_NUM_MAX_PROFILES        8
#define MMU_NUM_MAX_PACKET_SIZE     3
#define MMU_DEF_HDRM_CABLE_LEN      100

#define UC               (1 << 0)
#define MC               (1 << 1)
#define QGROUP           (1 << 2)
#define PORT_POOL        (1 << 3)
#define IS_SET           (1 << 4)
#define IS_GET           (1 << 5)
/* If in dynamic mode convert field to dynamic field before lookup LT */
#define FIELD_CONVERT    (1 << 6)
#define PORT_PG          (1 << 7)
#define PG_POOL          (1 << 8)
#define ING_PORT_POOL    (1 << 9)
#define ALPHA            (1 << 10)

#define CEIL(x, y)              (((x) + ((y)-1)) / (y))
#define MMU_CFG_MMU_BYTES_TO_CELLS(byte,cellhdr) (((byte) + (cellhdr) - 1) / (cellhdr))
#ifndef MAX
#define MAX(a, b)               (((a)>(b))?(a):(b))
#endif

typedef int (*cosq_thd_field_convert_f)(int unit, uint16_t flags, int port, int index,
                                        bcmi_lt_field_t *field, int *arg);

typedef int (*cosq_thd_field_set_f)(int unit, int flags, int port, int index,
                                    bcmi_lt_field_t *field);

typedef int (*cosq_thd_field_get_f)(int unit, int flags, int port, int index,
                                    bcmi_lt_field_t *field);

typedef struct cosq_control_map_s
{
    int type;
    bcmi_lt_field_t field;
    uint16_t flags;
    cosq_thd_field_convert_f    field_convert;
    cosq_thd_field_set_f        field_set;
    cosq_thd_field_get_f        field_get;
    bcmi_lt_field_t oper_field;
} cosq_control_map_t;

/*
 * \brief Service pool override.
 */
typedef struct cosq_thd_service_pool_override_s {
    int mc_override;
    int mc_spid;
    int cpu_override;
    int cpu_spid;
    int mirror_override;
    int mirror_spid;
} cosq_thd_service_pool_override_t;

typedef struct cosq_thd_lt_convert_s {
    uint32_t        val;
    const char      *sym_val;
} cosq_thd_lt_convert_t;

typedef struct cosq_thd_pfc_speed_attrs_s
{
    int port_speed;
    int pfc_response;   /* PFC response in PQs */
    int tx_rx_delay;    /* Max delay in ns */
    int max_add_buffer; /* Max additional buffer in bytes */
} cosq_thd_pfc_speed_attrs_t;

static tm_ltsw_cosq_thd_chip_driver_t *tm_cosq_thd_chip_driver_cb[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Private functions
 */

/*
 * \brief Convert cosq control type to lt
 *
 * \param [in]  type         bcm_cosq_control_t.
 * \param [in]  array        cosq_control_map_t.
 * \param [in]  nfields      nfields.
 *
 * \retval array              array.
 */
static cosq_control_map_t*
cosq_thd_type_to_field(bcm_cosq_control_t type, cosq_control_map_t *array, int nfields)
{
    int i;
    bool found = false;

    for (i = (nfields - 1); i >= 0; i--) {
      if (type == array[i].type) {
          found = true;
          break;
      }
    }

    return found ? &array[i] : NULL;
}

/*
 * \brief Check port type.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  port          PORT ID.
 *
 * \retval TRUE               Port is cpu port.
 * \retval FALSE              Port is non cpu port.
 */
static int
is_cpu_port(int unit, bcm_port_t lport)
{
    return bcmi_ltsw_port_is_type(unit, lport,  BCMI_LTSW_PORT_TYPE_CPU);
}

/*
 * \brief Validate input parameters of ingress threshold.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  gport         GPORT ID.
 * \param [in]  prigroup      prigroup id.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_thd_port_pg_validate(int unit, bcm_gport_t gport,
                          bcm_cos_t prigroup)
{
    SHR_FUNC_ENTER(unit);

    if ((prigroup < 0) || (prigroup >= MMU_NUM_PG)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Egress service pool state check.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_state op_state.
 * \param [in] buffer_id Buffer id.
 * \param [in] spid Pool id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_egr_service_pool_state_check(int unit, const char *op_state,
                                      int buffer_id, int spid)
{
    SHR_FUNC_ENTER(unit);

    if (sal_strcasecmp(op_state, VALIDs) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NONE);
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "ITM %d EGR service pool %d threshold update error: %s\n"),
                buffer_id, spid, op_state));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Egress service pool get.
 *
 * \param [in] unit Unit Number.
 * \param [in] buffer_id Buffer id.
 * \param [in] spid Pool id.
 * \param [out] info Service pool information.
 * \param [in] check_state check op state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_egr_service_pool_get(int unit, int buffer_id, int spid,
                              cosq_thd_service_pool_info_t *info, bool check_state)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {BUFFER_POOLs,                   BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {TM_EGR_SERVICE_POOL_IDs,        BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {SHARED_LIMIT_CELLSs,            BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 3 */ {SHARED_LIMIT_CELLS_OPERs,       BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 4 */ {SHARED_RESUME_LIMIT_CELLSs,     BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 5 */ {SHARED_RESUME_LIMIT_CELLS_OPERs, BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 6 */ {COLOR_SPECIFIC_LIMITSs,          BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 7 */ {YELLOW_SHARED_LIMIT_CELLSs,      BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 8 */ {YELLOW_SHARED_LIMIT_CELLS_OPERs, BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 9 */ {YELLOW_SHARED_RESUME_LIMIT_CELLSs, BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 10 */ {YELLOW_SHARED_RESUME_LIMIT_CELLS_OPERs, BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 11 */ {RED_SHARED_LIMIT_CELLSs,        BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 12 */ {RED_SHARED_LIMIT_CELLS_OPERs,   BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 13 */ {RED_SHARED_RESUME_LIMIT_CELLSs, BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 14 */ {RED_SHARED_RESUME_LIMIT_CELLS_OPERs, BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 15 */ {OPERATIONAL_STATEs,             BCMI_LT_FIELD_F_GET |
                                                  BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = buffer_id;
    fields[1].u.val = spid;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_EGR_THD_SERVICE_POOLs, &lt_entry, NULL, NULL));

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "lt %s lookup %s=%d %s=%d %s=%d %s=%d %s=%d %s=%d %s=%d %s=%d %s=%d"
            " %s=%d %s=%d %s=%d %s=%d %s=%d %s=%d %s=%s\n"),
            TM_EGR_THD_SERVICE_POOLs,
            fields[0].fld_name, (int)fields[0].u.val,
            fields[1].fld_name, (int)fields[1].u.val,
            fields[2].fld_name, (int)fields[2].u.val,
            fields[3].fld_name, (int)fields[3].u.val,
            fields[4].fld_name, (int)fields[4].u.val,
            fields[5].fld_name, (int)fields[5].u.val,
            fields[6].fld_name, (int)fields[6].u.val,
            fields[7].fld_name, (int)fields[7].u.val,
            fields[8].fld_name, (int)fields[8].u.val,
            fields[9].fld_name, (int)fields[9].u.val,
            fields[10].fld_name, (int)fields[10].u.val,
            fields[11].fld_name, (int)fields[11].u.val,
            fields[12].fld_name, (int)fields[12].u.val,
            fields[13].fld_name, (int)fields[13].u.val,
            fields[14].fld_name, (int)fields[14].u.val,
            fields[15].fld_name, fields[15].u.sym_val));

    if (!check_state) {
        info->shared_limit = fields[3].u.val;
        info->resume_limit = fields[5].u.val;
        info->enable_color_limit = fields[6].u.val;
        info->yellow_limit = fields[8].u.val;
        info->yellow_resume = fields[10].u.val;
        info->red_limit = fields[12].u.val;
        info->red_resume = fields[14].u.val;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_egr_service_pool_state_check(unit, fields[15].u.sym_val,
                                                   buffer_id, spid));
    }

exit:
    SHR_FUNC_EXIT();
}


/*
 * \brief Egress service pool configure.
 *
 * \param [in] unit Unit Number.
 * \param [in] buffer_id Buffer id.
 * \param [in] spid Pool id.
 * \param [in] info Service pool information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_egr_service_pool_set(int unit, int buffer_id, int spid,
                              cosq_thd_service_pool_info_t *info)
{
    bcmi_lt_entry_t lt_entry;
    bool enable = false;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {BUFFER_POOLs,                   BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {TM_EGR_SERVICE_POOL_IDs,        BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {SHARED_LIMIT_CELLSs,            0, 0, {0}},
         /* 3 */ {SHARED_RESUME_LIMIT_CELLSs,     0, 0, {0}},
         /* 4 */ {COLOR_SPECIFIC_LIMITSs,         0, 0, {0}},
         /* 5 */ {YELLOW_SHARED_LIMIT_CELLSs,     0, 0, {0}},
         /* 6 */ {YELLOW_SHARED_RESUME_LIMIT_CELLSs, 0, 0, {0}},
         /* 7 */ {RED_SHARED_LIMIT_CELLSs,        0, 0, {0}},
         /* 8 */ {RED_SHARED_RESUME_LIMIT_CELLSs, 0, 0, {0}},
         /* 9 */ {OPERATIONAL_STATEs,             BCMI_LT_FIELD_F_GET |
                                                  BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = buffer_id;
    fields[1].u.val = spid;
    if (info->shared_limit != -1) {
        fields[2].flags = BCMI_LT_FIELD_F_SET;
        fields[2].u.val = info->shared_limit;
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "lt %s update %s=%d %s=%d %s=%d\n"),
                TM_EGR_THD_SERVICE_POOLs,
                fields[0].fld_name, (int)fields[0].u.val,
                fields[1].fld_name, (int)fields[1].u.val,
                fields[2].fld_name, (int)fields[2].u.val));
    }

    if (info->resume_limit != -1) {
        fields[3].flags = BCMI_LT_FIELD_F_SET;
        fields[3].u.val = info->resume_limit;
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "lt %s update %s=%d %s=%d %s=%d\n"),
                TM_EGR_THD_SERVICE_POOLs,
                fields[0].fld_name, (int)fields[0].u.val,
                fields[1].fld_name, (int)fields[1].u.val,
                fields[3].fld_name, (int)fields[3].u.val));
    }

    if (info->enable_color_limit != -1) {
        enable = info->enable_color_limit ? true : false;

        fields[4].flags = BCMI_LT_FIELD_F_SET;
        fields[4].u.val = enable;
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "lt %s update %s=%d %s=%d %s=%d\n"),
                TM_EGR_THD_SERVICE_POOLs,
                fields[0].fld_name, (int)fields[0].u.val,
                fields[1].fld_name, (int)fields[1].u.val,
                fields[4].fld_name, (int)fields[4].u.val));
    }

    if (info->yellow_limit != -1) {
        fields[5].flags = BCMI_LT_FIELD_F_SET;
        fields[5].u.val = info->yellow_limit;
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "lt %s update %s=%d %s=%d %s=%d\n"),
                TM_EGR_THD_SERVICE_POOLs,
                fields[0].fld_name, (int)fields[0].u.val,
                fields[1].fld_name, (int)fields[1].u.val,
                fields[5].fld_name, (int)fields[5].u.val));
    }

    if (info->yellow_resume != -1) {
        fields[6].flags = BCMI_LT_FIELD_F_SET;
        fields[6].u.val = info->yellow_resume;
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "lt %s update %s=%d %s=%d %s=%d\n"),
                TM_EGR_THD_SERVICE_POOLs,
                fields[0].fld_name, (int)fields[0].u.val,
                fields[1].fld_name, (int)fields[1].u.val,
                fields[6].fld_name, (int)fields[6].u.val));
    }

    if (info->red_limit != -1) {
        fields[7].flags = BCMI_LT_FIELD_F_SET;
        fields[7].u.val = info->red_limit;
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "lt %s update %s=%d %s=%d %s=%d\n"),
                TM_EGR_THD_SERVICE_POOLs,
                fields[0].fld_name, (int)fields[0].u.val,
                fields[1].fld_name, (int)fields[1].u.val,
                fields[7].fld_name, (int)fields[7].u.val));
    }

    if (info->red_resume != -1) {
        fields[8].flags = BCMI_LT_FIELD_F_SET;
        fields[8].u.val = info->red_resume;
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "lt %s update %s=%d %s=%d %s=%d\n"),
                TM_EGR_THD_SERVICE_POOLs,
                fields[0].fld_name, (int)fields[0].u.val,
                fields[1].fld_name, (int)fields[1].u.val,
                fields[8].fld_name, (int)fields[8].u.val));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_update(unit, TM_EGR_THD_SERVICE_POOLs, &lt_entry, NULL));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_egr_service_pool_get(unit, buffer_id, spid, NULL, true));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Port MC service pool id get.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] queue queue id.
 * \param [out] spid pool id.
 * \param [out] use_qgroup_min true/false.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_port_mc_spid_get(int unit, bcm_port_t port, int queue,
                          int *spid, int *use_qgroup_min)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t value;
    int dunit = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_PORT_MC_Q_TO_SERVICE_POOLs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_IDs,
                               port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TM_MC_Q_IDs,
                               queue));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, TM_EGR_SERVICE_POOL_IDs,
                               &value));
    if (spid != NULL) {
        *spid = value;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, USE_QGROUP_MINs,
                               &value));
    if (use_qgroup_min != NULL) {
        *use_qgroup_min = value;
    }

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief Port UC service pool id get.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] queue queue id.
 * \param [out] spid pool id.
 * \param [out] use_qgroup_min true/false.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_port_uc_spid_get(int unit, bcm_port_t port, int queue,
                          int *spid, int *use_qgroup_min)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t value;
    int dunit = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_PORT_UC_Q_TO_SERVICE_POOLs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_IDs,
                               port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TM_UC_Q_IDs,
                               queue));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, TM_EGR_SERVICE_POOL_IDs,
                               &value));
    if (spid != NULL) {
        *spid = value;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, USE_QGROUP_MINs,
                               &value));
    if (use_qgroup_min != NULL) {
        *use_qgroup_min = value;
    }

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}


/*
 * \brief TM db shared limit get.
 *
 * \param [in] unit Unit Number.
 * \param [in] itm itm.
 * \param [in] spid ing service pool id.
 * \param [out] shared_limit shared limit value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_shared_limit_get(int unit, int itm, int spid, int *shared_limit)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_ING_THD_SERVICE_POOLs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, BUFFER_POOLs, itm));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TM_ING_SERVICE_POOL_IDs, spid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, SHARED_LIMIT_CELLSs, &value));

    *shared_limit = value;

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Find hardware index for the given port/cosq.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  port          port number or GPORT identifier.
 * \param [in]  cosq          COS queue number.
 * \param [in]  style         index style (COSQ_THD_INDEX_STYLE_XXX).
 * \param [out] local_port    local port number.
 * \param [out] index         result index.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_thd_index_resolve(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                       cosq_thd_index_style_t style,
                       bcm_port_t *local_port, int *index)
{
    bcm_port_t resolved_port;
    bcmi_ltsw_cosq_port_map_info_t port_info;
    bcmi_ltsw_cosq_device_info_t device_info = {0};
    int resolved_id = -1;
    int resolved_index = -1;
    bool hierarchical = false;
    int startq, numq = 0;
    SHR_FUNC_ENTER(unit);

    if (cosq < -1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    } else if (cosq == -1) {
        startq = 0;
    } else {
        startq = cosq;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport)) {
        hierarchical = true;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &resolved_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_port_map_info_get(unit, resolved_port, &port_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    numq = is_cpu_port(unit, resolved_port) ? device_info.num_cpu_queue :
                                              device_info.num_queue;

    switch (style) {
        case COSQ_THD_INDEX_STYLE_EGR_POOL:
            if (hierarchical) {
                if (BCM_GPORT_IS_SCHEDULER(gport)) { /* scheduler */
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_cosq_gport_port_resolve(unit, gport,
                                                       &resolved_port, &resolved_index));

                if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (cosq_thd_port_mc_spid_get(unit, resolved_port,
                                                          resolved_index,
                                                          &resolved_id, NULL));
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_ltsw_cosq_thd_port_uc_spid_get(unit, resolved_port,
                                                             resolved_index,
                                                             &resolved_id, NULL));
                }
            } else {
                if (startq >= numq) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }

                if (is_cpu_port(unit, resolved_port)) {
                    resolved_index = startq;
                } else {
                    if (startq >= port_info.num_uc_q) {
                        resolved_index = startq - port_info.num_uc_q;
                    } else {
                        resolved_index = startq;
                    }
                }

                if (is_cpu_port(unit, resolved_port) || (startq >= port_info.num_uc_q)) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (cosq_thd_port_mc_spid_get(unit, resolved_port,
                                                          resolved_index,
                                                          &resolved_id, NULL));
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_ltsw_cosq_thd_port_uc_spid_get(unit, resolved_port,
                                                             resolved_index,
                                                             &resolved_id, NULL));
                }
            }

            break;
        case COSQ_THD_INDEX_STYLE_UCAST_QUEUE:
            if (hierarchical) {
                if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_ltsw_cosq_gport_port_resolve(unit, gport,
                                                           &resolved_port,
                                                           &resolved_id));
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
            } else {
                if (is_cpu_port(unit, resolved_port)) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
                if (startq >= port_info.num_uc_q) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
                resolved_id = startq;
            }
            break;
        case COSQ_THD_INDEX_STYLE_MCAST_QUEUE:
            if (hierarchical) {
                if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_ltsw_cosq_gport_port_resolve(unit, gport,
                                                           &resolved_port,
                                                           &resolved_id));
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }

            } else {
                if (is_cpu_port(unit, resolved_port)) {
                    if (startq >= numq) {
                        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                    }
                    resolved_id = startq;
                } else {
                    if ((startq < port_info.num_uc_q) || (startq >= numq)) {
                        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                    }
                    resolved_id = startq - port_info.num_uc_q;
                }
            }
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (index) {
        *index = resolved_id;
    }

    if (local_port) {
        *local_port = resolved_port;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_SERVICE_POOL_OVERRIDEs field set
 *
 * \param [in] unit Unit Number.
 * \param [in] info cosq_thd_service_pool_override_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
cosq_thd_service_pool_override_set(int unit, cosq_thd_service_pool_override_t *info)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bool enable;
    int dunit = 0;
    SHR_FUNC_ENTER(unit);

    if (info == NULL){
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_SERVICE_POOL_OVERRIDEs, &entry_hdl));

    if (info->mc_override != -1) {
        enable = info->mc_override ? true : false;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, MC_OVERRIDEs,
                                   enable));
    }

    if (info->mc_spid != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, MC_SERVICE_POOLs,
                                   info->mc_spid));
    }

    if (info->cpu_override != -1) {
        enable = info->cpu_override ? true : false;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, CPU_OVERRIDEs,
                                   enable));
    }

    if (info->cpu_spid != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, CPU_SERVICE_POOLs,
                                   info->cpu_spid));
    }


    if (info->mirror_override != -1) {
        enable = info->mirror_override ? true : false;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, MIRROR_OVERRIDEs,
                                   enable));
    }

    if (info->mirror_spid != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, MIRROR_SERVICE_POOLs,
                                   info->mirror_spid));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_SERVICE_POOL_OVERRIDEs field get
 *
 * \param [in] unit Unit Number.
 * \param [out] info cosq_thd_service_pool_override_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
cosq_thd_service_pool_override_get(int unit, cosq_thd_service_pool_override_t *info)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t value;
    int dunit = 0;
    SHR_FUNC_ENTER(unit);

    if (info == NULL){
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_SERVICE_POOL_OVERRIDEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, MC_OVERRIDEs,
                               &value));
    info->mc_override = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, MC_SERVICE_POOLs,
                               &value));
    info->mc_spid = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, CPU_OVERRIDEs,
                               &value));
    info->cpu_override = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, CPU_SERVICE_POOLs,
                               &value));
    info->cpu_spid = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, MIRROR_OVERRIDEs,
                               &value));
    info->mirror_override = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, MIRROR_SERVICE_POOLs,
                               &value));
    info->mirror_spid = value;

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief threshold table state check.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_state op_state.
 * \param [in] key_value key value.
 * \param [in] key2_value key2 value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_table_state_check(int unit, const char *op_state,
                           int key_value, int key2_value)
{
    SHR_FUNC_ENTER(unit);

    if (sal_strcasecmp(op_state, VALIDs) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NONE);
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "key1 %d key2 %d update error: %s\n"),
                key_value, key2_value, op_state));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get an entry from LT.
 *
 * This function is used to get an entry from LT.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  lt_val               Logic table name.
 * \param [in]  key_val              key field.
 * \param [in]  key_value            key value.
 * \param [in]  key_val              key 2 field.
 * \param [in]  key_value            key 2 value.
 * \param [out]  field                bcmi_lt_field_t.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_thd_table_get(int unit,
                   const char *lt_val,
                   const char *key_val,
                   int key_value,
                   const char *key2_val,
                   int key2_value,
                   bcmi_lt_field_t *field,
                   bool check_state)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *op_state;
    int rv = SHR_E_NONE;
    int dunit = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_val, &entry_hdl));

    if (key_val != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, key_val,
                                   key_value));
    }

    if (key2_val != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, key2_val,
                                   key2_value));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    if (!check_state) {
        if (field->flags & BCMI_LT_FIELD_F_SYMBOL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_get(entry_hdl, field->fld_name,
                                       &(field->u.sym_val)));
            LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "lt %s lookup %s=%d %s=%d %s=%s\n"),
                    lt_val,
                    (key_val != NULL) ? key_val : " ", key_value,
                    (key2_val != NULL) ? key2_val : " ", key2_value,
                    field->fld_name, field->u.sym_val));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, field->fld_name,
                                       &(field->u.val)));
            LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "lt %s lookup %s=%d %s=%d %s=%d\n"),
                    lt_val,
                    (key_val != NULL) ? key_val : " ", key_value,
                    (key2_val != NULL) ? key2_val : " ", key2_value,
                    field->fld_name, (int)field->u.val));
        }
    } else {
        rv = bcmlt_entry_field_symbol_get(entry_hdl, OPERATIONAL_STATEs,
                                          &op_state);
        if (rv != SHR_E_NOT_FOUND) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_thd_table_state_check(unit, op_state,
                                            (key_val != NULL) ? key_value : -1,
                                            (key2_val != NULL) ? key2_value : -1));
        }
    }

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief Add an entry into LT.
 *
 * This function is used to add an entry into LT.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  lt_val               Logic table name.
 * \param [in]  key_val              key field.
 * \param [in]  key_value            key value.
 * \param [in]  key_val              key 2 field.
 * \param [in]  key_value            key 2 value.
 * \param [in]  field                bcmi_lt_field_t.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_thd_table_set(int unit,
                   const char *lt_val,
                   const char *key_val,
                   int key_value,
                   const char *key2_val,
                   int key2_value,
                   bcmi_lt_field_t *field,
                   int check_state)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_val, &entry_hdl));

    if (key_val != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, key_val,
                                   key_value));
    }

    if (key2_val != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, key2_val,
                                   key2_value));
    }

    if (field->flags & BCMI_LT_FIELD_F_SYMBOL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(entry_hdl,
                                          field->fld_name,
                                          field->u.sym_val));
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "lt %s update %s=%d %s=%d %s=%s\n"),
                lt_val,
                (key_val != NULL) ? key_val : " ", key_value,
                (key2_val != NULL) ? key2_val : " ", key2_value,
                field->fld_name, field->u.sym_val));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl,
                                   field->fld_name,
                                   field->u.val));
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "lt %s update %s=%d %s=%d %s=%d\n"),
                lt_val,
                (key_val != NULL) ? key_val : " ", key_value,
                (key2_val != NULL) ? key2_val : " ", key2_value,
                field->fld_name, (int)field->u.val));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    if (check_state){
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_table_get(unit, lt_val, key_val, key_value,
                                key2_val, key2_value, field, check_state));
    }
exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_THD_U/MC_Qs field set
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index index.
 * \param [in] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */

static int
cosq_thd_egress_queue_field_set(int unit, int flags, int port, int index,
                                bcmi_lt_field_t *field)
{
    const char *tbl_name;
    const char *fld_name;
    SHR_FUNC_ENTER(unit);

    tbl_name = (flags & UC) ? TM_THD_UC_Qs : TM_THD_MC_Qs;
    fld_name = (flags & UC) ? TM_UC_Q_IDs  : TM_MC_Q_IDs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_set(unit, tbl_name, PORT_IDs, port, fld_name, index,
                            field, true));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_THD_U/MC_Qs field get
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index index.
 * \param [out] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_egress_queue_field_get(int unit, int flags, int port, int index,
                                bcmi_lt_field_t *field)
{
    const char *tbl_name;
    const char *fld_name;
    SHR_FUNC_ENTER(unit);

    tbl_name = (flags & UC) ? TM_THD_UC_Qs : TM_THD_MC_Qs;
    fld_name = (flags & UC) ? TM_UC_Q_IDs  : TM_MC_Q_IDs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_get(unit, tbl_name, PORT_IDs, port, fld_name, index,
                            field, false));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_PORT_U/MC_Q_TO_SERVICE_POOLs field set
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index indexs.
 * \param [in] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_egress_pool_field_set(int unit, int flags, int port, int index,
                               bcmi_lt_field_t *field)
{
    const char *tbl_name;
    const char *fld_name;
    SHR_FUNC_ENTER(unit);

    tbl_name = (flags & UC) ? TM_PORT_UC_Q_TO_SERVICE_POOLs : TM_PORT_MC_Q_TO_SERVICE_POOLs;
    fld_name = (flags & UC) ? TM_UC_Q_IDs  : TM_MC_Q_IDs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_set(unit, tbl_name, PORT_IDs, port, fld_name, index,
                            field, true));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_PORT_U/MC_Q_TO_SERVICE_POOLs field get
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index index.
 * \param [out] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_egress_pool_field_get(int unit, int flags, int port, int index,
                               bcmi_lt_field_t *field)
{
    const char *tbl_name;
    const char *fld_name;
    SHR_FUNC_ENTER(unit);

    tbl_name = (flags & UC) ? TM_PORT_UC_Q_TO_SERVICE_POOLs : TM_PORT_MC_Q_TO_SERVICE_POOLs;
    fld_name = (flags & UC) ? TM_UC_Q_IDs  : TM_MC_Q_IDs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_get(unit, tbl_name, PORT_IDs, port, fld_name, index,
                            field, false));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_EGR_THD_U/MC_PORT_SERVICE_POOLs field set
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index index.
 * \param [in] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_egress_port_pool_field_set(int unit, int flags, int port, int index,
                               bcmi_lt_field_t *field)
{
    const char *tbl_name;
    const char *fld_name;
    SHR_FUNC_ENTER(unit);

    tbl_name = (flags & UC) ? TM_EGR_THD_UC_PORT_SERVICE_POOLs : TM_EGR_THD_MC_PORT_SERVICE_POOLs;
    fld_name = TM_EGR_SERVICE_POOL_IDs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_set(unit, tbl_name, PORT_IDs, port, fld_name, index,
                            field, true));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_EGR_THD_U/MC_PORT_SERVICE_POOLs field get
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index index.
 * \param [out] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_egress_port_pool_field_get(int unit, int flags, int port, int index,
                                    bcmi_lt_field_t *field)
{
    const char *tbl_name;
    const char *fld_name;
    SHR_FUNC_ENTER(unit);

    tbl_name = (flags & UC) ? TM_EGR_THD_UC_PORT_SERVICE_POOLs : TM_EGR_THD_MC_PORT_SERVICE_POOLs;
    fld_name = TM_EGR_SERVICE_POOL_IDs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_get(unit, tbl_name, PORT_IDs, port, fld_name, index,
                            field, false));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_THD_Q_GROUPs field set
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index index.
 * \param [in] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_egress_qgroup_set(int unit, int flags, int port, int index,
                           bcmi_lt_field_t *field)
{
    const char *tbl_name;
    const char *fld_name;
    SHR_FUNC_ENTER(unit);

    tbl_name = TM_THD_Q_GRPs;
    fld_name = PORT_IDs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_set(unit, tbl_name, fld_name, port, NULL, index,
                            field, true));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_THD_Q_GROUPs field get
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index index.
 * \param [out] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_egress_qgroup_get(int unit, int flags, int port, int index,
                           bcmi_lt_field_t *field)
{
    const char *tbl_name;
    const char *fld_name;
    SHR_FUNC_ENTER(unit);

    tbl_name = TM_THD_Q_GRPs;
    fld_name = PORT_IDs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_get(unit, tbl_name, fld_name, port, NULL, index,
                            field, false));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_ING_UC_ING_PRI_MAPs field set
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index index.
 * \param [out] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_ing_pri_map_field_set(int unit, int flags, int profile, int index,
                               bcmi_lt_field_t *field)
{
    const char *tbl_name;
    const char *fld_name;
    SHR_FUNC_ENTER(unit);

    tbl_name = (flags & UC) ? TM_ING_UC_ING_PRI_MAPs : TM_ING_NONUC_ING_PRI_MAPs;
    fld_name = (flags & UC) ? TM_ING_UC_ING_PRI_MAP_IDs  : TM_ING_NONUC_ING_PRI_MAP_IDs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_set(unit, tbl_name,
                            ING_PRIs, index,
                            fld_name, profile,
                            field, true));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_ING_UC_ING_PRI_MAPs field get
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index index.
 * \param [out] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_ing_pri_map_field_get(int unit, int flags, int profile, int index,
                               bcmi_lt_field_t *field)
{
    const char *tbl_name;
    const char *fld_name;
    SHR_FUNC_ENTER(unit);

    tbl_name = (flags & UC) ? TM_ING_UC_ING_PRI_MAPs : TM_ING_NONUC_ING_PRI_MAPs;
    fld_name = (flags & UC) ? TM_ING_UC_ING_PRI_MAP_IDs  : TM_ING_NONUC_ING_PRI_MAP_IDs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_get(unit, tbl_name,
                            ING_PRIs, index,
                            fld_name, profile,
                            field, false));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_ING_THD_PORT_PRI_GRPs field set
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index index.
 * \param [in] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_ingress_thd_port_pg_field_set(int unit, int flags, int port, int index,
                                       bcmi_lt_field_t *field)
{
    const char *tbl_name;
    const char *fld_name;
    bcmi_lt_field_t auto_field;
    SHR_FUNC_ENTER(unit);

    tbl_name = TM_ING_THD_PORT_PRI_GRPs;
    fld_name = TM_PRI_GRP_IDs;

    if (sal_strcasecmp(field->fld_name, HEADROOM_LIMIT_CELLSs) == 0) {
        auto_field.flags = 0;
        auto_field.fld_name = HEADROOM_LIMIT_AUTOs;
        auto_field.idx = 0;
        auto_field.u.val = FALSE;
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_table_set(unit, tbl_name, PORT_IDs, port,
                                fld_name, index, &auto_field, true));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_set(unit, tbl_name, PORT_IDs, port, fld_name, index,
                            field, true));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_ING_THD_PORT_PRI_GRPs field get
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index index.
 * \param [out] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_ingress_thd_port_pg_field_get(int unit, int flags, int port, int index,
                                       bcmi_lt_field_t *field)
{
    const char *tbl_name;
    const char *fld_name;
    SHR_FUNC_ENTER(unit);

    tbl_name = TM_ING_THD_PORT_PRI_GRPs;
    fld_name = TM_PRI_GRP_IDs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_get(unit, tbl_name, PORT_IDs, port, fld_name, index,
                            field, false));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_ING_PORT_PRI_GRPs field set
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index index.
 * \param [in] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_ingress_port_pg_field_set(int unit, int flags, int port, int index,
                                   bcmi_lt_field_t *field)
{
    const char *tbl_name;
    const char *fld_name;
    SHR_FUNC_ENTER(unit);

    tbl_name = TM_ING_PORT_PRI_GRPs;
    fld_name = TM_PRI_GRP_IDs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_set(unit, tbl_name, PORT_IDs, port, fld_name, index,
                            field, true));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_ING_PORT_PRI_GRPs field get
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index index.
 * \param [out] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_ingress_port_pg_field_get(int unit, int flags, int port, int index,
                                   bcmi_lt_field_t *field)
{
    const char *tbl_name;
    const char *fld_name;
    SHR_FUNC_ENTER(unit);

    tbl_name = TM_ING_PORT_PRI_GRPs;
    fld_name = TM_PRI_GRP_IDs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_get(unit, tbl_name, PORT_IDs, port, fld_name, index,
                            field, false));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_PRI_GRP_POOL_MAPs field set
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index index.
 * \param [out] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_pg_pool_map_field_set(int unit, int flags, int pool, int index,
                               bcmi_lt_field_t *field)
{
    const char *tbl_name;
    const char *fld_name;
    SHR_FUNC_ENTER(unit);

    tbl_name = TM_PRI_GRP_POOL_MAPs;
    fld_name = TM_PRI_GRP_IDs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_set(unit, tbl_name,
                            TM_PRI_GRP_POOL_MAP_IDs, pool,
                            fld_name, index,
                            field, true));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_PRI_GRP_POOL_MAPs field get
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index index.
 * \param [out] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_pg_pool_map_field_get(int unit, int flags, int pool, int index,
                               bcmi_lt_field_t *field)
{
    const char *tbl_name;
    const char *fld_name;
    SHR_FUNC_ENTER(unit);

    tbl_name = TM_PRI_GRP_POOL_MAPs;
    fld_name = TM_PRI_GRP_IDs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_get(unit, tbl_name,
                            TM_PRI_GRP_POOL_MAP_IDs, pool,
                            fld_name, index,
                            field, false));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_ING_THD_PORT_SERVICE_POOLs field set
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index index.
 * \param [out] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_ingress_port_pool_field_set(int unit, int flags, int port, int index,
                               bcmi_lt_field_t *field)
{
    const char *tbl_name;
    const char *fld_name;
    SHR_FUNC_ENTER(unit);

    tbl_name = TM_ING_THD_PORT_SERVICE_POOLs;
    fld_name = TM_ING_SERVICE_POOL_IDs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_set(unit, tbl_name, PORT_IDs, port, fld_name, index,
                            field, true));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_ING_THD_PORT_SERVICE_POOLs field get
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index index.
 * \param [out] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_ingress_port_pool_field_get(int unit, int flags, int port, int index,
                               bcmi_lt_field_t *field)
{
    const char *tbl_name;
    const char *fld_name;
    SHR_FUNC_ENTER(unit);

    tbl_name = TM_ING_THD_PORT_SERVICE_POOLs;
    fld_name = TM_ING_SERVICE_POOL_IDs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_get(unit, tbl_name, PORT_IDs, port, fld_name, index,
                            field, false));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Set the profile for Input priortty to PG mapping for a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [in] profile_id Profile index.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_port_inppri_profile_set(int unit, bcm_port_t port, int profile_id)
{
    const char *tbl_name;
    bcmi_lt_field_t field;
    SHR_FUNC_ENTER(unit);

    if ((profile_id < 0) || (profile_id >= MMU_NUM_MAX_PROFILES)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&field, 0, sizeof(bcmi_lt_field_t));

    tbl_name = TM_ING_PORTs;
    field.fld_name = ING_PRI_MAP_IDs;
    field.u.val = profile_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_set(unit, tbl_name, PORT_IDs, port, NULL, -1, &field, true));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the profile for Input priortty to PG mapping for a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [out] profile_id Profile index.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_port_inppri_profile_get(int unit, bcm_port_t port, int *profile_id)
{
    const char *tbl_name;
    bcmi_lt_field_t field;
    SHR_FUNC_ENTER(unit);

    if (profile_id == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&field, 0, sizeof(bcmi_lt_field_t));

    tbl_name = TM_ING_PORTs;
    field.fld_name = ING_PRI_MAP_IDs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_get(unit, tbl_name, PORT_IDs, port, NULL, -1, &field, false));

    *profile_id = field.u.val;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Set pause to a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [in] pause pause.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_port_pause_set(int unit, bcm_port_t port, int pause)
{
    const char *tbl_name;
    bcmi_lt_field_t field;

    SHR_FUNC_ENTER(unit);

    sal_memset(&field, 0, sizeof(bcmi_lt_field_t));

    tbl_name = TM_ING_PORTs;
    field.fld_name = PAUSEs;
    field.u.val = pause;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_set(unit, tbl_name, PORT_IDs, port, NULL, -1, &field, true));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the pause state for a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [out] pause pause.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_port_pause_get(int unit, bcm_port_t port, int *pause)
{
    const char *tbl_name;
    bcmi_lt_field_t field;

    SHR_FUNC_ENTER(unit);

    if (pause == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&field, 0, sizeof(bcmi_lt_field_t));

    tbl_name = TM_ING_PORTs;
    field.fld_name = PAUSEs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_get(unit, tbl_name, PORT_IDs, port, NULL, -1, &field, false));

    *pause = field.u.val;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief COS queue alpha convert
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] cosq COS queue.
 * \param [in] field field.
 * \param [in] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_alpha_convert(int unit, uint16_t flags, int port, int index,
                       bcmi_lt_field_t *field, int *arg)
{
    tm_ltsw_cosq_thd_chip_driver_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_cosq_thd_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_thd_int_to_alpha, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_thd_alpha_to_int, SHR_E_INIT);

    if (flags & IS_SET) {
        field->u.sym_val = drv->cosq_thd_int_to_alpha(*arg);
    } else {
        if (flags & IS_GET) {
            *arg = drv->cosq_thd_alpha_to_int(field->u.sym_val);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief alpha field set
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index index.
 * \param [out] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_alpha_field_set(int unit, int flags, int port, int index,
                         bcmi_lt_field_t *field)
{
    SHR_FUNC_ENTER(unit);

    if (flags == PORT_PG) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_ingress_thd_port_pg_field_set(unit, flags, port, index, field));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_egress_queue_field_set(unit, flags, port, index, field));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief alpha field get
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index index.
 * \param [out] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_alpha_field_get(int unit, int flags, int port, int index,
                         bcmi_lt_field_t *field)
{
    SHR_FUNC_ENTER(unit);

    if (flags == PORT_PG) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_ingress_thd_port_pg_field_get(unit, flags, port, index, field));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_egress_queue_field_get(unit, flags, port, index, field));
    }

exit:
    SHR_FUNC_EXIT();
}


/*
 * \brief Set the profile for PG to SPID/HPID/PFCPRI mapping for a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [in] profile_id Profile index.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_port_pg_profile_set(int unit, bcm_port_t port, int profile_id)
{
    const char *tbl_name;
    bcmi_lt_field_t field;
    SHR_FUNC_ENTER(unit);

    if ((profile_id < 0) || (profile_id >= MMU_NUM_MAX_PROFILES)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&field, 0, sizeof(bcmi_lt_field_t));

    tbl_name = TM_ING_PORTs;
    field.fld_name = PRI_GRP_MAP_IDs;
    field.u.val = profile_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_set(unit, tbl_name, PORT_IDs, port, NULL, -1, &field, true));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the profile for PG to SPID/HPID/PFCPRI mapping for a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [out] profile_id Profile index.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_port_pg_profile_get(int unit, bcm_port_t port, int *profile_id)
{
    const char *tbl_name;
    bcmi_lt_field_t field;
    SHR_FUNC_ENTER(unit);

    if (profile_id == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&field, 0, sizeof(bcmi_lt_field_t));

    tbl_name = TM_ING_PORTs;
    field.fld_name = PRI_GRP_MAP_IDs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_get(unit, tbl_name, PORT_IDs, port, NULL, -1, &field, false));

    *profile_id = field.u.val;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Find hardware index for the given port/cosq.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  port          port number or GPORT identifier.
 * \param [in]  cosq          COS queue number.
 * \param [in]  style         index style (COSQ_THD_INDEX_STYLE_XXX).
 * \param [out] local_port    local port number.
 * \param [out] index         result index.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_thd_gport_local_index_resolve(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *flags,
    bcm_port_t *local_port,
    int *index)
{
    bcmi_ltsw_cosq_port_map_info_t port_info;
    int lport;
    int modid, trunk_id, id;
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_index_resolve(unit, gport, cosq,
                                    COSQ_THD_INDEX_STYLE_UCAST_QUEUE,
                                    local_port, index));
        *flags = UC;
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_index_resolve(unit, gport, cosq,
                                    COSQ_THD_INDEX_STYLE_MCAST_QUEUE,
                                    local_port, index));
        *flags = MC;
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    } else  { /* local port */
        if (BCM_GPORT_IS_SET(gport)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_resolve(unit, gport, &modid, &lport,
                                              &trunk_id, &id));
        } else {
            lport = gport;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_port_map_info_get(unit, lport, &port_info));

        if (is_cpu_port(unit, lport) || (cosq >= port_info.num_uc_q)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_thd_index_resolve(unit, lport, cosq,
                                        COSQ_THD_INDEX_STYLE_MCAST_QUEUE,
                                        local_port, index));
            *flags = MC;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_thd_index_resolve(unit, lport, cosq,
                                        COSQ_THD_INDEX_STYLE_UCAST_QUEUE,
                                        local_port, index));
            *flags = UC;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
cosq_thd_dynamic_mode_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int *arg);

/*
 * \brief Find hardware index for the given port/cosq.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  port          port number or GPORT identifier.
 * \param [in]  cosq          COS queue number.
 * \param [in]  style         index style (COSQ_THD_INDEX_STYLE_XXX).
 * \param [out] local_port    local port number.
 * \param [out] index         result index.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_thd_alpha_index_resolve(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *flags,
    bcm_port_t *local_port,
    int *index)
{
    int dynamic_thresh_mode;
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_dynamic_mode_get(unit, gport, cosq,
                                       bcmCosqControlEgressUCSharedDynamicEnable,
                                       &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_index_resolve(unit, gport, cosq,
                                    COSQ_THD_INDEX_STYLE_UCAST_QUEUE,
                                    local_port, index));
        *flags = UC;
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_dynamic_mode_get(unit, gport, cosq,
                                       bcmCosqControlEgressMCSharedDynamicEnable,
                                       &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_index_resolve(unit, gport, cosq,
                                    COSQ_THD_INDEX_STYLE_MCAST_QUEUE,
                                    local_port, index));
        *flags = MC;
    } else if (BCM_GPORT_IS_LOCAL(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_dynamic_mode_get(unit, gport, cosq,
                                       bcmCosqControlIngressPortPGSharedDynamicEnable,
                                       &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_localport_resolve(unit, gport, local_port));
        if ((cosq < 0) || (cosq >= MMU_NUM_PG)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        *index = cosq;
        *flags = PORT_PG;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief BYTES to CELL convert
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] cosq COS queue.
 * \param [in] field field.
 * \param [in] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_bytes_to_cell_convert(int unit, uint16_t flags, int port, int queue,
                               bcmi_lt_field_t *field, int *arg)
{
    int bytes = 0;
    int cells = 0;
    if (flags & IS_SET) {
        (void)bcmi_ltsw_cosq_thd_byte_to_cell(unit, *arg, &cells);
        field->u.val = cells;
    } else {
        if (!(flags & FIELD_CONVERT)) {
            *arg = field->u.val;
            (void)bcmi_ltsw_cosq_thd_cell_to_byte(unit, *arg, &bytes);
            *arg = bytes;
        }
    }

    return SHR_E_NONE;
}

/*
 * \brief COS queue egress color dynamic convert
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] cosq COS queue.
 * \param [in] field field.
 * \param [in] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_limit_percent_convert(int unit, uint16_t flags, int port, int queue,
                               bcmi_lt_field_t *field, int *arg)
{
    bool is_dynamic = false;
    bool is_red = false;
    bool shared_limit = false;
    bool color_specific_limit = false;
    bool color_specific_dynamic_limit = false;
    bcmi_lt_field_t fields;
    tm_ltsw_cosq_thd_chip_driver_t *drv = NULL;
    int cells = 0;
    int bytes = 0;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "port=%d queue=%d flags=%d\n"), port, queue, (int)flags));

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_cosq_thd_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_thd_int_to_percent, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_thd_percent_to_int, SHR_E_INIT);


    sal_memset(&fields, 0, sizeof(bcmi_lt_field_t));

    fields.fld_name = SHARED_LIMITSs;
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_egress_queue_field_get(unit, (flags & UC), port, queue, &fields));
    shared_limit = fields.u.val;

    fields.fld_name = COLOR_SPECIFIC_LIMITSs;
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_egress_queue_field_get(unit, (flags & UC), port, queue, &fields));
    color_specific_limit = fields.u.val;

    if ((flags & IS_SET) && !shared_limit && !color_specific_limit) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    fields.fld_name = COLOR_SPECIFIC_DYNAMIC_LIMITSs;
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_egress_queue_field_get(unit, (flags & UC), port, queue, &fields));
    color_specific_dynamic_limit = fields.u.val;

    /*
     * Shared buffer static limit for yellow and red packets (in 8-cell granularity).
     * Applicable only if SHARED_LIMITS and COLOR_SPECIFIC_LIMITS are
     * enabled, and COLOR_SPECIFIC_DYNAMIC_LIMITS is disabled.
     * If COLOR_SPECIFIC_DYNAMIC_LIMITS are enabled, YELLOW_LIMIT_DYNAMIC and RED_LIMIT_DYNAMIC
     * should be set accordingly.
     */

    if (shared_limit && color_specific_limit && color_specific_dynamic_limit){
        is_dynamic = true;
    }

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "Before convert field %s field_flags=%d dynamic=%d arg=%d\n"),
            field->fld_name, (int)field->flags, is_dynamic, (int)*arg));

    if (flags & IS_SET) {
        if ((sal_strcasecmp(field->fld_name, RED_LIMIT_CELLS_STATICs) == 0) ||
            (sal_strcasecmp(field->fld_name, RED_LIMIT_DYNAMICs) == 0)) {
            is_red = true;
        }

        if (is_dynamic) {
            field->fld_name = is_red ? RED_LIMIT_DYNAMICs : YELLOW_LIMIT_DYNAMICs;
            field->u.sym_val = drv->cosq_thd_int_to_percent(*arg);
            field->flags |= BCMI_LT_FIELD_F_SYMBOL;
            if (*arg < 0 || *arg > 7) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        } else {
            field->fld_name = is_red ? RED_LIMIT_CELLS_STATICs
                              : YELLOW_LIMIT_CELLS_STATICs;
            field->flags &= ~BCMI_LT_FIELD_F_SYMBOL;
            (void)bcmi_ltsw_cosq_thd_byte_to_cell(unit, *arg, &cells);
            field->u.val = cells;
        }
    } else {
        if ((sal_strcasecmp(field->fld_name, RED_LIMIT_CELLS_STATIC_OPERs) == 0) ||
            (sal_strcasecmp(field->fld_name, RED_LIMIT_DYNAMICs) == 0)) {
            is_red = true;
        }

        if (flags & FIELD_CONVERT) {
            /* If in dynamic mode convert field to dynamic field before lookup LT */
            if (is_dynamic) {
                field->fld_name = is_red ? RED_LIMIT_DYNAMICs : YELLOW_LIMIT_DYNAMICs;
                field->flags |= BCMI_LT_FIELD_F_SYMBOL;
            } else {
                field->fld_name = is_red ? RED_LIMIT_CELLS_STATIC_OPERs :
                                  YELLOW_LIMIT_CELLS_STATIC_OPERs;
                field->flags &= ~BCMI_LT_FIELD_F_SYMBOL;
            }
        } else {
            if (is_dynamic) {
                *arg = drv->cosq_thd_percent_to_int(field->u.sym_val);
            } else {
                (void)bcmi_ltsw_cosq_thd_cell_to_byte(unit, field->u.val, &bytes);
                *arg = bytes;
            }
        }
    }

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "After convert field %s field_flags=%d arg=%d\n"),
            field->fld_name, (int)field->flags, (int)*arg));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief COS ADT priority group convert
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] cosq COS queue.
 * \param [in] field field.
 * \param [in] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_adt_pri_group_convert(int unit, uint16_t flags, int port, int index,
                       bcmi_lt_field_t *field, int *arg)
{
    tm_ltsw_cosq_thd_chip_driver_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_cosq_thd_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_thd_int_to_adt_pri_group, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_thd_adt_pri_group_to_int, SHR_E_INIT);

    if (flags & IS_SET) {
        field->u.sym_val = drv->cosq_thd_int_to_adt_pri_group(*arg);
    } else {
        if (flags & IS_GET) {
            *arg = drv->cosq_thd_adt_pri_group_to_int(field->u.sym_val);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief ADT priority group field set
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index index.
 * \param [out] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_adt_pri_group_field_set(int unit, int flags, int port, int index,
                              bcmi_lt_field_t *field)
{
    const char *tbl_name;
    const char *fld_name;
    SHR_FUNC_ENTER(unit);

    tbl_name = TM_THD_UC_Qs;
    fld_name = TM_UC_Q_IDs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_set(unit, tbl_name, PORT_IDs, port, fld_name, index,
                            field, true));

exit:
    SHR_FUNC_EXIT();
}


/*
 * \brief ADT priority group field get
 *
 * \param [in] unit Unit Number.
 * \param [in] flags flags.
 * \param [in] port port ID.
 * \param [in] index index.
 * \param [out] field field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_adt_pri_group_field_get(int unit, int flags, int port, int index,
                         bcmi_lt_field_t *field)
{
    const char *tbl_name;
    const char *fld_name;
    SHR_FUNC_ENTER(unit);

    tbl_name = TM_THD_UC_Qs;
    fld_name = TM_UC_Q_IDs;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_get(unit, tbl_name, PORT_IDs, port, fld_name, index,
                            field, false));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Convert control type to lt field.
 */
static cosq_control_map_t cosq_thd_type_to_fields[] =
{
    {bcmCosqControlEgressPool,
        {TM_EGR_SERVICE_POOL_IDs, 0},         UC|MC,  NULL,
         cosq_thd_egress_pool_field_set, cosq_thd_egress_pool_field_get},
    {bcmCosqControlUCEgressPool,
        {TM_EGR_SERVICE_POOL_IDs, 0},         UC,     NULL,
         cosq_thd_egress_pool_field_set, cosq_thd_egress_pool_field_get},
    {bcmCosqControlMCEgressPool,
        {TM_EGR_SERVICE_POOL_IDs, 0},         MC,     NULL,
         cosq_thd_egress_pool_field_set, cosq_thd_egress_pool_field_get},
    {bcmCosqControlEgressPoolLimitBytes,
        {SHARED_LIMIT_CELLS_STATICs, 0},      UC|MC,  cosq_thd_bytes_to_cell_convert,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get},
    {bcmCosqControlEgressPoolYellowLimitBytes,
        {YELLOW_LIMIT_CELLS_STATICs, 0},      UC|MC,  cosq_thd_bytes_to_cell_convert,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get,
        {YELLOW_LIMIT_CELLS_STATIC_OPERs, 0}},
    {bcmCosqControlEgressPoolRedLimitBytes,
        {RED_LIMIT_CELLS_STATICs, 0},         UC|MC,  cosq_thd_bytes_to_cell_convert,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get,
        {RED_LIMIT_CELLS_STATIC_OPERs, 0}},
    {bcmCosqControlEgressPoolLimitEnable,
        {SHARED_LIMITSs, 0},                  UC|MC,  NULL,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get},
    {bcmCosqControlEgressUCQueueSharedLimitBytes,
        {SHARED_LIMIT_CELLS_STATICs, 0},      UC,     cosq_thd_bytes_to_cell_convert,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get},
    {bcmCosqControlEgressMCQueueSharedLimitBytes,
        {SHARED_LIMIT_CELLS_STATICs, 0},      MC,     cosq_thd_bytes_to_cell_convert,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get},
    {bcmCosqControlEgressUCQueueResumeOffsetBytes,
        {RESUME_OFFSET_CELLSs, 0},            UC,     cosq_thd_bytes_to_cell_convert,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get},
    {bcmCosqControlEgressMCQueueResumeOffsetBytes,
        {RESUME_OFFSET_CELLSs, 0},            MC,     cosq_thd_bytes_to_cell_convert,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get},
    {bcmCosqControlEgressUCQueueMinLimitBytes,
        {MIN_GUARANTEE_CELLSs, 0},            UC,     cosq_thd_bytes_to_cell_convert,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get,
        {MIN_GUARANTEE_CELLS_OPERs, 0}},
    {bcmCosqControlEgressMCQueueMinLimitBytes,
        {MIN_GUARANTEE_CELLSs, 0},            MC,     cosq_thd_bytes_to_cell_convert,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get,
        {MIN_GUARANTEE_CELLS_OPERs, 0}},
    {bcmCosqControlEgressUCSharedDynamicEnable,
        {DYNAMIC_SHARED_LIMITSs, 0},          UC,     NULL,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get},
    {bcmCosqControlEgressMCSharedDynamicEnable,
        {DYNAMIC_SHARED_LIMITSs, 0},          MC,     NULL,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get},
    {bcmCosqControlEgressUCQueueLimitEnable,
        {SHARED_LIMITSs, 0},                  UC,     NULL,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get},
    {bcmCosqControlEgressMCQueueLimitEnable,
        {SHARED_LIMITSs, 0},                  MC,     NULL,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get},
    {bcmCosqControlEgressUCQueueColorLimitDynamicEnable,
        {COLOR_SPECIFIC_DYNAMIC_LIMITSs, 0},  UC,     NULL,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get},
    {bcmCosqControlEgressMCQueueColorLimitDynamicEnable,
        {COLOR_SPECIFIC_DYNAMIC_LIMITSs, 0},  MC,     NULL,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get},
    {bcmCosqControlEgressUCQueueColorLimitEnable,
        {COLOR_SPECIFIC_LIMITSs, 0},          UC,     NULL,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get},
    {bcmCosqControlEgressMCQueueColorLimitEnable,
        {COLOR_SPECIFIC_LIMITSs, 0},          MC,     NULL,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get},
    {bcmCosqControlEgressUCQueueRedLimit,
        {RED_LIMIT_CELLS_STATICs, 0},         UC,     cosq_thd_limit_percent_convert,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get,
        {RED_LIMIT_CELLS_STATIC_OPERs, 0}},
    {bcmCosqControlEgressUCQueueYellowLimit,
        {YELLOW_LIMIT_CELLS_STATICs, 0},      UC,     cosq_thd_limit_percent_convert,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get,
        {YELLOW_LIMIT_CELLS_STATIC_OPERs, 0}},
    {bcmCosqControlEgressMCQueueRedLimit,
        {RED_LIMIT_CELLS_STATICs, 0},         MC,     cosq_thd_limit_percent_convert,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get,
        {RED_LIMIT_CELLS_STATIC_OPERs, 0}},
    {bcmCosqControlEgressMCQueueYellowLimit,
        {YELLOW_LIMIT_CELLS_STATICs, 0},      MC,      cosq_thd_limit_percent_convert,
         cosq_thd_egress_queue_field_set, cosq_thd_egress_queue_field_get,
        {YELLOW_LIMIT_CELLS_STATIC_OPERs, 0}},
    {bcmCosqControlEgressUCQueueGroupMinEnable,
        {USE_QGROUP_MINs, 0},                 UC,     NULL,
         cosq_thd_egress_pool_field_set, cosq_thd_egress_pool_field_get},
    {bcmCosqControlEgressMCQueueGroupMinEnable,
        {USE_QGROUP_MINs, 0},                 MC,     NULL,
         cosq_thd_egress_pool_field_set, cosq_thd_egress_pool_field_get},
    {bcmCosqControlEgressUCQueueGroupMinLimitBytes,
        {UC_Q_GRP_MIN_GUARANTEE_CELLSs, 0}, QGROUP|UC, cosq_thd_bytes_to_cell_convert,
         cosq_thd_egress_qgroup_set,     cosq_thd_egress_qgroup_get,
        {UC_Q_GRP_MIN_GUARANTEE_CELLS_OPERs, 0}},
    {bcmCosqControlEgressMCQueueGroupMinLimitBytes,
        {MC_Q_GRP_MIN_GUARANTEE_CELLSs, 0}, QGROUP|MC, cosq_thd_bytes_to_cell_convert,
         cosq_thd_egress_qgroup_set,     cosq_thd_egress_qgroup_get,
        {MC_Q_GRP_MIN_GUARANTEE_CELLS_OPERs, 0}},
    {bcmCosqControlEgressPortPoolSharedLimitBytes,
        {SHARED_LIMIT_CELLSs, 0},             PORT_POOL, cosq_thd_bytes_to_cell_convert,
         cosq_thd_egress_port_pool_field_set,     cosq_thd_egress_port_pool_field_get},
    {bcmCosqControlEgressPortPoolYellowLimitBytes,
        {YELLOW_SHARED_LIMIT_CELLSs, 0},      PORT_POOL, cosq_thd_bytes_to_cell_convert,
         cosq_thd_egress_port_pool_field_set,     cosq_thd_egress_port_pool_field_get},
    {bcmCosqControlEgressPortPoolRedLimitBytes,
        {RED_SHARED_LIMIT_CELLSs, 0},         PORT_POOL, cosq_thd_bytes_to_cell_convert,
         cosq_thd_egress_port_pool_field_set,     cosq_thd_egress_port_pool_field_get},
    {bcmCosqControlEgressPortPoolSharedResumeBytes,
        {SHARED_RESUME_LIMIT_CELLSs, 0},      PORT_POOL, cosq_thd_bytes_to_cell_convert,
         cosq_thd_egress_port_pool_field_set,     cosq_thd_egress_port_pool_field_get,
        {SHARED_RESUME_LIMIT_CELLS_OPERs, 0}},
    {bcmCosqControlEgressPortPoolYellowResumeBytes,
        {YELLOW_SHARED_RESUME_LIMIT_CELLSs, 0}, PORT_POOL, cosq_thd_bytes_to_cell_convert,
         cosq_thd_egress_port_pool_field_set,     cosq_thd_egress_port_pool_field_get,
        {YELLOW_SHARED_RESUME_LIMIT_CELLS_OPERs, 0}},
    {bcmCosqControlEgressPortPoolRedResumeBytes,
        {RED_SHARED_RESUME_LIMIT_CELLSs, 0},  PORT_POOL, cosq_thd_bytes_to_cell_convert,
         cosq_thd_egress_port_pool_field_set,     cosq_thd_egress_port_pool_field_get,
        {RED_SHARED_RESUME_LIMIT_CELLS_OPERs, 0}},
    {bcmCosqControlIngressPortPGSharedLimitBytes,
        {SHARED_LIMIT_CELLS_STATICs, 0},      PORT_PG, cosq_thd_bytes_to_cell_convert,
         cosq_thd_ingress_thd_port_pg_field_set,     cosq_thd_ingress_thd_port_pg_field_get},
    {bcmCosqControlIngressPortPGMinLimitBytes,
        {MIN_GUARANTEE_CELLSs, 0},            PORT_PG, cosq_thd_bytes_to_cell_convert,
         cosq_thd_ingress_thd_port_pg_field_set,     cosq_thd_ingress_thd_port_pg_field_get,
        {MIN_GUARANTEE_CELLS_OPERs, 0}},
    {bcmCosqControlIngressPortPGHeadroomLimitBytes,
        {HEADROOM_LIMIT_CELLSs, 0},           PORT_PG, cosq_thd_bytes_to_cell_convert,
         cosq_thd_ingress_thd_port_pg_field_set,     cosq_thd_ingress_thd_port_pg_field_get,
        {HEADROOM_LIMIT_CELLS_OPERs, 0}},
    {bcmCosqControlIngressPortPGResetFloorBytes,
        {RESUME_FLOOR_CELLSs, 0},             PORT_PG, cosq_thd_bytes_to_cell_convert,
         cosq_thd_ingress_thd_port_pg_field_set,     cosq_thd_ingress_thd_port_pg_field_get},
    {bcmCosqControlIngressPortPGResetOffsetBytes,
        {RESUME_OFFSET_CELLSs, 0},            PORT_PG, cosq_thd_bytes_to_cell_convert,
         cosq_thd_ingress_thd_port_pg_field_set,     cosq_thd_ingress_thd_port_pg_field_get},
    {bcmCosqControlIngressPortPGSharedDynamicEnable,
        {DYNAMIC_SHARED_LIMITSs, 0},          PORT_PG, NULL,
         cosq_thd_ingress_thd_port_pg_field_set,     cosq_thd_ingress_thd_port_pg_field_get},
    {bcmCosqControlIngressPool,
        {TM_ING_SERVICE_POOL_IDs, 0},         PG_POOL, NULL,
         cosq_thd_pg_pool_map_field_set,     cosq_thd_pg_pool_map_field_get},
    {bcmCosqControlIngressHeadroomPool,
        {TM_HEADROOM_POOL_IDs, 0},            PG_POOL, NULL,
         cosq_thd_pg_pool_map_field_set,     cosq_thd_pg_pool_map_field_get},
    {bcmCosqControlIngressPortPoolMinLimitBytes,
        {MIN_GUARANTEE_CELLSs, 0},            ING_PORT_POOL, cosq_thd_bytes_to_cell_convert,
         cosq_thd_ingress_port_pool_field_set,     cosq_thd_ingress_port_pool_field_get,
        {MIN_GUARANTEE_CELLS_OPERs, 0}},
    {bcmCosqControlIngressPortPoolSharedLimitBytes,
        {SHARED_LIMIT_CELLSs, 0},             ING_PORT_POOL, cosq_thd_bytes_to_cell_convert,
         cosq_thd_ingress_port_pool_field_set,     cosq_thd_ingress_port_pool_field_get},
    {bcmCosqControlIngressPortPoolResumeLimitBytes,
        {RESUME_LIMIT_CELLSs, 0},             ING_PORT_POOL, cosq_thd_bytes_to_cell_convert,
         cosq_thd_ingress_port_pool_field_set,     cosq_thd_ingress_port_pool_field_get,
        {RESUME_LIMIT_CELLS_OPERs, 0}},
    {bcmCosqControlDropLimitAlpha,
        {SHARED_LIMIT_DYNAMICs, BCMI_LT_FIELD_F_SYMBOL}, ALPHA, cosq_thd_alpha_convert,
         cosq_thd_alpha_field_set,            cosq_thd_alpha_field_get},
    {bcmCosqControlAdtPriGroup,
        {DYNAMIC_GROUPs, BCMI_LT_FIELD_F_SYMBOL}, UC, cosq_thd_adt_pri_group_convert,
         cosq_thd_adt_pri_group_field_set,            cosq_thd_adt_pri_group_field_get},
};

/*
 * \brief Convert HW drop probability to percent value
 *
 * \param [in]  type      bcm_cosq_control_t.
 *
 * \retval map           cosq_control_map_t.
 */
static cosq_control_map_t*
cosq_thd_type_to_queue_field(bcm_cosq_control_t type)
{
    int nfields;

    nfields = sizeof(cosq_thd_type_to_fields) / sizeof(cosq_thd_type_to_fields[0]);

    return cosq_thd_type_to_field(type, cosq_thd_type_to_fields, nfields);
}

/*
 * \brief Find hardware index for the given port/cosq.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  port          port number or GPORT identifier.
 * \param [in]  cosq          COS queue number.
 * \param [in]  style         index style (COSQ_THD_INDEX_STYLE_XXX).
 * \param [out] local_port    local port number.
 * \param [out] index         result index.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_thd_map_index_reslove(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    cosq_control_map_t *map,
    int *flags,
    bcm_port_t *local_port,
    int *index)
{
    bcmi_ltsw_cosq_port_map_info_t port_info;
    int profile_id;
    int pool_id;
    SHR_FUNC_ENTER(unit);

    if (map->flags == (UC|MC)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_gport_local_index_resolve(unit, gport, cosq, flags,
                                                local_port, index));
    } else if ((map->flags & MC) == MC) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_index_resolve(unit, gport, cosq,
                                    COSQ_THD_INDEX_STYLE_MCAST_QUEUE,
                                    local_port, index));
        *flags = MC;
    } else if ((map->flags & UC) == UC) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_index_resolve(unit, gport, cosq,
                                    COSQ_THD_INDEX_STYLE_UCAST_QUEUE,
                                    local_port, index));
        *flags = UC;
    } else if (map->flags == (QGROUP|UC)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_index_resolve(unit, gport, cosq,
                                    COSQ_THD_INDEX_STYLE_UCAST_QUEUE,
                                    local_port, index));
    } else if (map->flags == (QGROUP|MC)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_index_resolve(unit, gport, cosq,
                                    COSQ_THD_INDEX_STYLE_MCAST_QUEUE,
                                    local_port, index));
    } else if (map->flags == PORT_POOL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_index_resolve(unit, gport, cosq,
                                    COSQ_THD_INDEX_STYLE_EGR_POOL,
                                    local_port, index));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_port_map_info_get(unit, *local_port, &port_info));

        if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
           (!(BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
              BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) &&
           (cosq >= port_info.num_uc_q))) {
            *flags = MC;
        } else {
            *flags = UC;
        }
    } else if (map->flags == PORT_PG) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_localport_resolve(unit, gport, local_port));
        *index = cosq;
    } else if (map->flags == PG_POOL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_localport_resolve(unit, gport, local_port));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_thd_port_pg_profile_get(unit, *local_port, &profile_id));
        *local_port = profile_id;
        *index = cosq;
    } else if (map->flags == ING_PORT_POOL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_localport_resolve(unit, gport, local_port));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_thd_ing_pg_pool_get(unit, gport, cosq, bcmCosqControlIngressPool,
                                                &pool_id));
        *index = pool_id;
    } else if (map->flags == ALPHA) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_alpha_index_resolve(unit, gport, cosq, flags, local_port, index));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief COS queue egress threshold setting
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] type Feature.
 * \param [in] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_type_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    int index, local_port;
    int flags;
    cosq_control_map_t *map;
    SHR_FUNC_ENTER(unit);

    map = cosq_thd_type_to_queue_field(type);

    if (map == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_map_index_reslove(unit, gport, cosq, map, &flags, &local_port, &index));

    if (map->field_convert != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (map->field_convert(unit, (IS_SET | map->flags),
                                local_port, index, &map->field, &arg));
    } else {
        map->field.u.val = arg;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (map->field_set(unit, flags, local_port, index, &map->field));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief COS queue egress threshold get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] type Feature.
 * \param [out] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_type_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    int index, local_port;
    int flags;
    cosq_control_map_t *map;
    bcmi_lt_field_t field;

    SHR_FUNC_ENTER(unit);

    if (arg == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    map = cosq_thd_type_to_queue_field(type);

    if (map == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_map_index_reslove(unit, gport, cosq, map, &flags, &local_port, &index));

    if (map->oper_field.fld_name != NULL) {
        field = map->oper_field;
    } else {
        field = map->field;
    }

    if (map->field_convert != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (map->field_convert(unit, (FIELD_CONVERT | map->flags),
                                local_port, index, &field, arg));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (map->field_get(unit, flags, local_port, index, &field));

    if (map->field_convert != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (map->field_convert(unit, (IS_GET | map->flags),
                                local_port, index, &field, arg));
    } else {
       *arg = field.u.val;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief COS queue egress dynamic get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] type Feature.
 * \param [in] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_dynamic_mode_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_type_get(unit, gport, cosq, type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Ingress pg service pool get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] prigroup Priority group.
 * \param [in] type Feature.
 * \param [out] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_thd_ing_pg_pool_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t prigroup,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_ENTER(unit);

    if (arg == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_port_pg_validate(unit, gport, prigroup));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_type_get(unit, gport, prigroup, type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get port cos range.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   port                GPORT ID.
 * \param [in]   cosq                COS queue.
 * \param [out]  start_cos           Start cos.
 * \param [out]  end_cos             End cos.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_thd_cos_range_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                       int *start_cos, int *end_cos)
{
    bcm_port_t local_port;
    SHR_FUNC_ENTER(unit);

    if (cosq < -1) {
        return BCM_E_PARAM;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    *start_cos = *end_cos = cosq;

    if (cosq == -1) {
        *start_cos = 0;
        *end_cos = bcmi_ltsw_cosq_port_num_cos(unit, local_port) - 1;
    }

    if (BCM_GPORT_IS_SET(gport)) {
        if ((BCM_GPORT_IS_SCHEDULER(gport)) ||
             BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
             BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            *start_cos = *end_cos = 0;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get COS queue bandwidth burst setting.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   port                GPORT ID.
 * \param [in]   cosq                COS queue.
 * \param [out]  kbits_burst_min     maximum burst, kbits.
 * \param [out]  kbits_burst_max     maximum burst, kbits.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_thd_bucket_get(int unit, bcm_gport_t gport,
                    bcm_cos_queue_t cosq,
                    uint32_t *kbits_burst_min,
                    uint32_t *kbits_burst_max)
{
    uint32_t kbits_sec_min, kbits_sec_max, flags;
    SHR_FUNC_ENTER(unit);

    if (cosq < -1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_bucket_get(unit, gport, cosq == -1 ? 0 : cosq,
                                   &kbits_sec_min, &kbits_sec_max, TRUE,
                                   kbits_burst_min, kbits_burst_max, &flags));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief COS queue bandwidth control bucket setting.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   port                GPORT ID.
 * \param [in]   cosq                COS queue.
 * \param [in]   kbits_burst_min     maximum burst, kbits.
 * \param [in]   kbits_burst_max     maximum burst, kbits.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_thd_bucket_set(int unit, bcm_gport_t gport,
                    bcm_cos_queue_t cosq,
                    uint32_t kbits_burst_min,
                    uint32_t kbits_burst_max)
{
    uint32_t kbits_sec_min, kbits_sec_max, kbits_sec_burst, flags;
    int i, start_cos=0, end_cos=0;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_cos_range_get(unit, gport, cosq, &start_cos, &end_cos));

    for (i = start_cos; i <= end_cos; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_bucket_get(unit, gport, i, &kbits_sec_min,
                                       &kbits_sec_max, FALSE, &kbits_sec_burst,
                                       &kbits_sec_burst, &flags));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_bucket_set(unit, gport, i, kbits_sec_min,
                                       kbits_sec_max, FALSE, kbits_burst_min,
                                       kbits_burst_max, flags));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief wred service pool set.
 *
 * \param [in]  unit                        Unit number.
 * \param [in]  pool_id                     pool_id.
 * \param [in]  high_cng_cells              High congestion limit in cells.
 * \param [in]  low_cng_cells               Low congestion limit in cells.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_thd_wred_service_pool_set(int unit, int pool_id,
                               int high_cng_cells,
                               int low_cng_cells)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {TM_WRED_SERVICE_POOL_IDs,       BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {HIGH_CNG_LIMIT_CELLSs,          0, 0, {0}},
         /* 2 */ {LOW_CNG_LIMIT_CELLSs,           0, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = pool_id;
    if (high_cng_cells != -1) {
        fields[1].flags = BCMI_LT_FIELD_F_SET;
        fields[1].u.val = high_cng_cells;
    }

    if (low_cng_cells != -1) {
        fields[2].flags = BCMI_LT_FIELD_F_SET;
        fields[2].u.val = low_cng_cells;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_WRED_SERVICE_POOLs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief wred service pool get.
 *
 * \param [in]  unit                        Unit number.
 * \param [in]  pool_id                     pool_id.
 * \param [out] high_cng_cells              High congestion limit in cells.
 * \param [out] low_cng_cells               Low congestion limit in cells.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_thd_wred_service_pool_get(int unit, int pool_id,
                               int *high_cng_cells,
                               int *low_cng_cells)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {TM_WRED_SERVICE_POOL_IDs,       BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {HIGH_CNG_LIMIT_CELLSs,          BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 2 */ {LOW_CNG_LIMIT_CELLSs,           BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = pool_id;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_lt_entry_get(unit, TM_WRED_SERVICE_POOLs,
                           &lt_entry, NULL, NULL), SHR_E_NOT_FOUND);

    *high_cng_cells         = fields[1].u.val;
    *low_cng_cells          = fields[2].u.val;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Configure egress service pool ADT high priority queue alpha.
 *
 * \param [in]  unit                        Unit number.
 * \param [in]  buffer                     Buffer ID.
 * \param [in]  spid                       Sevice pool ID.
 * \param [in]  arg                         Cosq control value.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_thd_egr_service_pool_adt_high_pri_alpha_set(
                               int unit, int buffer_id, int spid, int arg)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {TM_EGR_SERVICE_POOL_IDs,       BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {BUFFER_POOLs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {ADAPTIVE_DYNAMICs,             BCMI_LT_FIELD_F_SET
                 | BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_alpha_convert(unit, IS_SET, -1, -1, &fields[2], &arg));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = spid;
    fields[1].u.val = buffer_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_EGR_SERVICE_POOL_DYNAMICs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get egress service pool ADT high priority queue alpha.
 *
 * \param [in]  unit                        Unit number.
 * \param [in]  buffer                     Buffer ID.
 * \param [in]  spid                       Sevice pool ID.
 * \param [out]  arg                       Cosq control value.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_thd_egr_service_pool_adt_high_pri_alpha_get(
                               int unit, int buffer_id, int spid, int *arg)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {TM_EGR_SERVICE_POOL_IDs,       BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {BUFFER_POOLs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {ADAPTIVE_DYNAMICs,             BCMI_LT_FIELD_F_GET
                 | BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = spid;
    fields[1].u.val = buffer_id;

    rv = bcmi_lt_entry_get(unit, TM_EGR_SERVICE_POOL_DYNAMICs,
                           &lt_entry, NULL, NULL);
    if (rv == SHR_E_NOT_FOUND) {
        *arg = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_alpha_convert(unit, IS_GET, -1, -1, &fields[2], arg));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Configure egress service pool ADT margin.
 *
 * \param [in]  unit                        Unit number.
 * \param [in]  buffer                     Buffer ID.
 * \param [in]  spid                       Sevice pool ID.
 * \param [in]  type                       Cosq control type.
 * \param [in]  arg                         Cosq control value.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_thd_egr_service_pool_adt_margin_set(
                               int unit, int buffer_id, int spid,
                               bcm_cosq_control_t type, int arg)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit, i;
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    i = type - bcmCosqControlEgressPoolAdtMargin0;
    value = arg;

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_THD_DYNAMIC_MARGINs, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, TM_EGR_SERVICE_POOL_IDs, spid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, BUFFER_POOLs, buffer_id));

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_array_add(eh, MARGINs, i, &value, 1));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit
            (unit, eh, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief Get egress service pool ADT margin.
 *
 * \param [in]  unit                        Unit number.
 * \param [in]  buffer                     Buffer ID.
 * \param [in]  spid                       Sevice pool ID.
 * \param [in]  type                       Cosq control type.
 * \param [out]  arg                       Cosq control value.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_thd_egr_service_pool_adt_margin_get(
                               int unit, int buffer_id, int spid,
                               bcm_cosq_control_t type, int *arg)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit, i;
    uint32_t cnt;
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    i = type - bcmCosqControlEgressPoolAdtMargin0;

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_THD_DYNAMIC_MARGINs, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, TM_EGR_SERVICE_POOL_IDs, spid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, BUFFER_POOLs, buffer_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit
            (unit, eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_array_get(eh, MARGINs, i, &value, 1, &cnt));

    *arg = value & 0xffffffff;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief Egress service pool ADT configure.
 *
 * \param [in]  unit                        Unit number.
 * \param [in]  buffer                     Buffer ID.
 * \param [in]  spid                       Sevice pool ID.
 * \param [in]  type                       Cosq control type.
 * \param [in]  arg                         Cosq control value.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_thd_egr_service_pool_adt_set(
    int unit,
    bcm_cosq_buffer_id_t buffer,
    int spid,
    bcm_cosq_control_t type,
    int arg)
{
    int itm, from_itm, to_itm;
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    /* These regs are chip unique, buffer = -1 writes on both itms
       No need to do pipe-itm checking */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    if (buffer == BCM_COSQ_BUFFER_ID_INVALID) {
        from_itm = 0;
        to_itm = device_info.num_itm - 1;
    } else if ((buffer >=  0) && (buffer <  device_info.num_itm)) {
        from_itm = to_itm = buffer;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    for (itm = from_itm; itm <= to_itm; itm++ ) {
        switch (type) {
            case bcmCosqControlEgressPoolAdtHighPriAlpha:
                SHR_IF_ERR_VERBOSE_EXIT
                   (cosq_thd_egr_service_pool_adt_high_pri_alpha_set(unit, itm,
                                                                     spid, arg));
                break;
            case bcmCosqControlEgressPoolAdtMargin0:
            case bcmCosqControlEgressPoolAdtMargin1:
            case bcmCosqControlEgressPoolAdtMargin2:
            case bcmCosqControlEgressPoolAdtMargin3:
            case bcmCosqControlEgressPoolAdtMargin4:
            case bcmCosqControlEgressPoolAdtMargin5:
            case bcmCosqControlEgressPoolAdtMargin6:
            case bcmCosqControlEgressPoolAdtMargin7:
            case bcmCosqControlEgressPoolAdtMargin8:
            case bcmCosqControlEgressPoolAdtMargin9:
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_thd_egr_service_pool_adt_margin_set(unit, itm,
                                                              spid, type, arg));
                break;
            default:
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get egress service pool ADT control.
 *
 * \param [in]  unit                        Unit number.
 * \param [in]  buffer                     Buffer ID.
 * \param [in]  spid                       Sevice pool ID.
 * \param [in]  type                       Cosq control type.
 * \param [out]  arg                       Cosq control value.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_thd_egr_service_pool_adt_get(
    int unit,
    bcm_cosq_buffer_id_t buffer,
    int spid,
    bcm_cosq_control_t type,
    int *arg)
{
    int itm;
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    /* These regs are chip unique, retrieve value from itm 0 if buffer == -1 */
    if (buffer == BCM_COSQ_BUFFER_ID_INVALID) {
        itm = 0;
    } else if ((buffer >=  0) && (buffer < device_info.num_itm)) {
        itm = buffer;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    switch (type) {
        case bcmCosqControlEgressPoolAdtHighPriAlpha:
            SHR_IF_ERR_VERBOSE_EXIT
               (cosq_thd_egr_service_pool_adt_high_pri_alpha_get(unit, itm,
                                                                 spid, arg));
            break;
        case bcmCosqControlEgressPoolAdtMargin0:
        case bcmCosqControlEgressPoolAdtMargin1:
        case bcmCosqControlEgressPoolAdtMargin2:
        case bcmCosqControlEgressPoolAdtMargin3:
        case bcmCosqControlEgressPoolAdtMargin4:
        case bcmCosqControlEgressPoolAdtMargin5:
        case bcmCosqControlEgressPoolAdtMargin6:
        case bcmCosqControlEgressPoolAdtMargin7:
        case bcmCosqControlEgressPoolAdtMargin8:
        case bcmCosqControlEgressPoolAdtMargin9:
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_thd_egr_service_pool_adt_margin_get(unit, itm,
                                                          spid, type, arg));
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static cosq_thd_pfc_speed_attrs_t pfc_speed_attrs[7] =
{
    { 10000,    36,  454,   22342},
    { 25000,   110,  454,   22342},
    { 40000,   175,  454,   22342},
    { 50000,   175,  454,   22342},
    {100000,   339,  364,   51166},
    {200000,   693,  359,  101854},
    {400000,  1385,  280,  203708}
};

static int
cosq_thd_pfc_port_speed_attrs_get(int unit, int port_speed,
                                  int *pfc_response, int *tx_rx_delay,
                                  int *max_add_buffer)
{
    int i, speed_found = 0;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < (sizeof(pfc_speed_attrs) /
                sizeof(cosq_thd_pfc_speed_attrs_t)); i++) {
        if (pfc_speed_attrs[i].port_speed == port_speed) {
            *pfc_response = pfc_speed_attrs[i].pfc_response;
            *tx_rx_delay = pfc_speed_attrs[i].tx_rx_delay;
            *max_add_buffer = pfc_speed_attrs[i].max_add_buffer;
            speed_found = 1;
            break;
        }
    }
    if (!speed_found) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
cosq_thd_port_pg_headroom_get(int unit, int port_speed,
                              int cable_length, int mtu,
                              int array_size,
                              bcm_cosq_pkt_size_dist_t *pkt_size_dst,
                              int *pg_hdrm)
{
    uint32_t pfc_tx_dly = 0, pfc_resp_time = 0, max_tx_rx_dly = 0;
    uint32_t last_packet = 0;
    uint64_t pkt_dist_num = 0, pkt_dist_denom = 1;
    int port_pq = 0, i, speed_txrx_dly = 0;
    uint32_t max_pkt_size_dist = 0;
    uint32_t mult_size_dist, packet_bytes;
    uint64_t sum_delays = 0;
    uint64_t cable_rtt = 0, sum_factor = 0;
    uint64_t tot_inflight = 0;
    int addl_buf = 0;
    int total_dist_perc = 0;
    bcmi_ltsw_cosq_device_info_t device_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    /* Check for sum of packet distribution perc to be 100 */
    for (i = 0; i < array_size; i++) {
        total_dist_perc += pkt_size_dst[i].dist_perc;
    }
    if (total_dist_perc != 100) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
            cosq_thd_pfc_port_speed_attrs_get(unit, port_speed, &port_pq,
                &speed_txrx_dly, &addl_buf));

    /* Derive inflight traffic components */

    /* RTT for 10G, 100m is 1250 */
    cable_rtt = CEIL(((1250 * cable_length * (port_speed/10000))/ 100), 1);
    /* PFC mesage tx delay */
    pfc_tx_dly = (PFC_FRAME_SIZE_BYTES + IPG_SIZE_BYTES);
    /* PFC response time - Pause quanta based on port speed */
    pfc_resp_time = port_pq * 64;

    /* Switch delay based on port_speed */
    max_tx_rx_dly = CEIL((speed_txrx_dly * (port_speed / 1000) / 8), 1);

    /* Retrieve max pkt size in the distribution */
    for (i = 0; i < array_size; i++) {
        if (pkt_size_dst[i].dist_perc > 0) {
            max_pkt_size_dist = MAX(max_pkt_size_dist, pkt_size_dst[i].pkt_size);
        }
    }
    /* Last packet in transmission based on packet distribution */
    last_packet = MMU_CFG_MMU_BYTES_TO_CELLS(max_pkt_size_dist +
                                              MMU_PACKET_HEADER_BYTES,
                                              device_info.mmu_cell_size) *
                                              device_info.mmu_cell_size;

    /* Retrieve numerator and denominator of pkt size distrbution factor */
    for (i = 0; i < array_size; i++) {
        if (pkt_size_dst[i].dist_perc > 0) {
            mult_size_dist = pkt_size_dst[i].pkt_size *
                    pkt_size_dst[i].dist_perc;
            packet_bytes = (MMU_CFG_MMU_BYTES_TO_CELLS(
                        pkt_size_dst[i].pkt_size +
                        MMU_PACKET_HEADER_BYTES,
                        device_info.mmu_cell_size) *
                        device_info.mmu_cell_size);
            pkt_dist_num += CEIL((packet_bytes * mult_size_dist) /
                (pkt_size_dst[i].pkt_size + IPG_SIZE_BYTES), 1) ;
            if (pkt_dist_denom == 1) {
                pkt_dist_denom = mult_size_dist;
            } else {
                pkt_dist_denom += mult_size_dist;
            }
        }
    }

    sum_delays = (mtu + cable_rtt + pfc_tx_dly + pfc_resp_time +
                 max_tx_rx_dly);
    sum_factor = (sum_delays * pkt_dist_num) / pkt_dist_denom;
    tot_inflight = sum_factor + addl_buf + last_packet;

    *pg_hdrm = CEIL((tot_inflight / device_info.mmu_cell_size), 1);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "mtu: %"PRId32", cable_rtt: %"PRIu64", pfc_tx_dly : %"PRIu32","
                "pfc_resp_time : %"PRIu32", max_tx_rx_dly : %"PRIu32","
                "addl_buf: %"PRId32", last_packet: %"PRIu32" \n"),
                mtu, cable_rtt, pfc_tx_dly, pfc_resp_time, max_tx_rx_dly, addl_buf,
                last_packet));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "pkt_dist_num: %"PRIu64", pkt_dist_denom: %"PRIu64", sum_delays"
                ": %"PRIu64", sum_factor: %"PRIu64", tot_inflight: %"PRIu64", "
                "pg_hdrm : %"PRId32"\n"),
                pkt_dist_num, pkt_dist_denom, sum_delays, sum_factor, tot_inflight,
                *pg_hdrm));

exit:
    SHR_FUNC_EXIT();
}

static int
cosq_thd_delta_hdrm_service_pools_adjust(int unit, int itm,
                                                    int hdrm_pool,
                                                    int service_pool,
                                                    uint32_t new_hp_limit)
{
    int old_sp_limit[ITMS_PER_DEV];
    uint32_t old_hp_limit[ITMS_PER_DEV] = {0},
             new_sp_limit[ITMS_PER_DEV] = {0};
    const char *tbl_name;
    const char *key_name;
    bcmi_lt_field_t field;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Retrieve old value of headroom pool limit */
    sal_memset(&field, 0, sizeof(bcmi_lt_field_t));
    tbl_name = TM_ING_THD_HEADROOM_POOLs;
    key_name = TM_HEADROOM_POOL_IDs;
    field.fld_name = LIMIT_CELLS_OPERs;
    rv = cosq_thd_table_get(unit, tbl_name, key_name, hdrm_pool,
                            BUFFER_POOLs, itm, &field, false);

    old_hp_limit[itm] = field.u.val;
    if (rv == SHR_E_NOT_FOUND) {
        old_hp_limit[itm] = 0;
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (old_hp_limit[itm] != new_hp_limit) {
        /* Retrieve old value of sevice pool limit */
        sal_memset(&field, 0, sizeof(bcmi_lt_field_t));
        tbl_name = TM_ING_THD_SERVICE_POOLs;
        key_name = TM_ING_SERVICE_POOL_IDs;
        field.fld_name = SHARED_LIMIT_CELLS_OPERs;
        rv = cosq_thd_table_get(unit, tbl_name, key_name, service_pool,
                                BUFFER_POOLs, itm, &field, false);

        old_sp_limit[itm] = field.u.val;
        if (rv == SHR_E_NOT_FOUND) {
            old_sp_limit[itm] = 0;
            rv = SHR_E_NONE;
        }
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        if (old_hp_limit[itm] > new_hp_limit) {
            new_sp_limit[itm] = old_sp_limit[itm] + (old_hp_limit[itm]
                                - new_hp_limit);
        } else {
            if (old_sp_limit[itm] > (new_hp_limit - old_hp_limit[itm])) {
                new_sp_limit[itm] = old_sp_limit[itm] - (new_hp_limit
                                    - old_hp_limit[itm]);
            } else {
                LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Warning: Given input params result"
                                "in Headroom pool limit addition greater"
                                "than available shared space: %d for itm:"
                                "%d. Try reducing the number of lossless"
                                "classes\n"), old_sp_limit[itm], itm));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);;
            }
        }

        /* Update the shared limit for headroom pool */
        sal_memset(&field, 0, sizeof(bcmi_lt_field_t));
        tbl_name = TM_ING_THD_HEADROOM_POOLs;
        key_name = TM_HEADROOM_POOL_IDs;
        field.fld_name = LIMIT_CELLSs;
        field.u.val = new_hp_limit;
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_table_set(unit, tbl_name, key_name, hdrm_pool,
                                BUFFER_POOLs, itm, &field, true));

        /* Update the Service pool shared limit for SP 0 */
        sal_memset(&field, 0, sizeof(bcmi_lt_field_t));
        tbl_name = TM_ING_THD_SERVICE_POOLs;
        key_name = TM_ING_SERVICE_POOL_IDs;
        field.fld_name = SHARED_LIMIT_CELLSs;
        field.u.val = new_sp_limit[itm];
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_table_set(unit, tbl_name, key_name, service_pool,
                                BUFFER_POOLs, itm, &field, true));
    }

exit:
    SHR_FUNC_EXIT();

}

/******************************************************************************
 * Public functions
 */

int
tm_ltsw_cosq_thd_chip_driver_register(int unit, tm_ltsw_cosq_thd_chip_driver_t *drv)
{
    tm_cosq_thd_chip_driver_cb[unit] = drv;

    return SHR_E_NONE;
}

int
tm_ltsw_cosq_thd_chip_driver_deregister(int unit)
{
    tm_cosq_thd_chip_driver_cb[unit] = NULL;

    return SHR_E_NONE;
}

int
tm_ltsw_cosq_thd_chip_driver_get(int unit, tm_ltsw_cosq_thd_chip_driver_t **drv)
{
    *drv = tm_cosq_thd_chip_driver_cb[unit];

    return SHR_E_NONE;
}

/*
 * \brief COS queue bandwidth control bucket setting.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   port                GPORT ID.
 * \param [in]   cosq                COS queue.
 * \param [in]   type                Feature.
 * \param [out]  arg                 Feature value.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
tm_ltsw_cosq_thd_bandwidth_burst_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    uint32_t kburst_tmp = 0;
    uint32_t kbits_burst = 0;
    SHR_FUNC_ENTER(unit);

    switch (type) {
        case bcmCosqControlBandwidthBurstMax:
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_thd_bucket_get(unit, gport, cosq,
                                     &kburst_tmp,
                                     &kbits_burst));
            break;
        case bcmCosqControlBandwidthBurstMin:
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_thd_bucket_get(unit, gport, cosq,
                                     &kbits_burst,
                                     &kburst_tmp));

        default:
            break;
    }

    *arg = kbits_burst & 0x7fffffff;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief COS queue bandwidth control bucket setting.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   port                GPORT ID.
 * \param [in]   cosq                COS queue.
 * \param [in]   type                Feature.
 * \param [in]   arg                 Feature value.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
tm_ltsw_cosq_thd_bandwidth_burst_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    uint32_t kbits_burst_min, kbits_burst_max;
    uint32_t kbits_burst;
    SHR_FUNC_ENTER(unit);

    kbits_burst_min = 0;
    kbits_burst_max = 0;

    COMPILER_REFERENCE(kbits_burst_min);
    COMPILER_REFERENCE(kbits_burst_max);

    /* Convert to uint32_t */
    kbits_burst = arg & 0x7fffffff;
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_bucket_get(unit, gport, cosq,
                             &kbits_burst_min,
                             &kbits_burst_max));

    switch (type) {
        case bcmCosqControlBandwidthBurstMax:
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_thd_bucket_set(unit, gport, cosq,
                                     kbits_burst_min,
                                     kbits_burst));
            break;
        case bcmCosqControlBandwidthBurstMin:
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_thd_bucket_set(unit, gport, cosq,
                                     kbits_burst,
                                     kbits_burst_max));
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_egr_spid_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_ENTER(unit);

    if (type == bcmCosqControlUCEgressPool ||
        type == bcmCosqControlMCEgressPool) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_type_set(unit, gport, cosq, type, arg));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_egr_spid_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    int rv;
    SHR_FUNC_ENTER(unit);

    if (type == bcmCosqControlUCEgressPool ||
        type == bcmCosqControlMCEgressPool) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    rv = cosq_thd_type_get(unit, gport, cosq, type, arg);
    if (rv == SHR_E_NOT_FOUND) {
        *arg = 0;
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_egr_pool_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_buffer_id_t buffer,
    bcm_cosq_control_t type,
    int arg)
{
    int itm, from_itm, to_itm;
    bcm_port_t local_port;
    int pool;
    bcmi_ltsw_cosq_device_info_t device_info;
    cosq_thd_service_pool_info_t pool_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    sal_memset(&pool_info, 0xFF, sizeof(cosq_thd_service_pool_info_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_index_resolve(unit, gport, cosq,
                                COSQ_THD_INDEX_STYLE_EGR_POOL,
                                &local_port, &pool));

    switch (type) {
        case bcmCosqControlEgressPoolSharedLimitBytes:
            pool_info.shared_limit = arg;
            break;
        case bcmCosqControlEgressPoolResumeLimitBytes:
            pool_info.resume_limit = arg;
            break;
        case bcmCosqControlEgressPoolYellowSharedLimitBytes:
            pool_info.yellow_limit = arg;
            break;
        case bcmCosqControlEgressPoolYellowResumeLimitBytes:
            pool_info.yellow_resume = arg;
            break;
        case bcmCosqControlEgressPoolRedSharedLimitBytes:
            pool_info.red_limit = arg;
            break;
        case bcmCosqControlEgressPoolRedResumeLimitBytes:
            pool_info.red_resume = arg;
            break;
        case bcmCosqControlEgressPoolHighCongestionLimitBytes:
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_thd_wred_service_pool_set(unit, pool, arg, -1));
            SHR_EXIT();
        case bcmCosqControlEgressPoolLowCongestionLimitBytes:
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_thd_wred_service_pool_set(unit, pool, -1, arg));
            SHR_EXIT();
        case bcmCosqControlEgressPoolAdtHighPriAlpha:
            if (arg < 0 || arg >= bcmCosqControlDropLimitAlphaCount) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        case bcmCosqControlEgressPoolAdtMargin0:
        case bcmCosqControlEgressPoolAdtMargin1:
        case bcmCosqControlEgressPoolAdtMargin2:
        case bcmCosqControlEgressPoolAdtMargin3:
        case bcmCosqControlEgressPoolAdtMargin4:
        case bcmCosqControlEgressPoolAdtMargin5:
        case bcmCosqControlEgressPoolAdtMargin6:
        case bcmCosqControlEgressPoolAdtMargin7:
        case bcmCosqControlEgressPoolAdtMargin8:
        case bcmCosqControlEgressPoolAdtMargin9:
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_thd_egr_service_pool_adt_set(unit, buffer, pool, type, arg));
            SHR_EXIT();
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    /* These regs are chip unique, buffer = -1 writes on both itms
       No need to do pipe-itm checking */

    if (buffer == BCM_COSQ_BUFFER_ID_INVALID) {
        from_itm = 0;
        to_itm = device_info.num_itm - 1;
    } else if ((buffer >=  0) && (buffer < device_info.num_itm)) {
        from_itm = to_itm = buffer;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    for (itm = from_itm; itm <= to_itm; itm++ ) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_egr_service_pool_set(unit, itm, pool, &pool_info));
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_egr_pool_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_buffer_id_t buffer,
    bcm_cosq_control_t type,
    int *arg)
{
    int itm;
    bcm_port_t local_port;
    int pool;
    cosq_thd_service_pool_info_t pool_info;
    int high_cng_cells, low_cng_cells;
    int rv;
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pool_info, 0, sizeof(cosq_thd_service_pool_info_t));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_index_resolve(unit, gport, cosq,
                                COSQ_THD_INDEX_STYLE_EGR_POOL,
                                &local_port, &pool));

    switch (type) {
        case bcmCosqControlEgressPoolHighCongestionLimitBytes:
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_thd_wred_service_pool_get(unit, pool, &high_cng_cells,
                                                &low_cng_cells));
            *arg = high_cng_cells;
            SHR_EXIT();
        case bcmCosqControlEgressPoolLowCongestionLimitBytes:
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_thd_wred_service_pool_get(unit, pool, &high_cng_cells,
                                                &low_cng_cells));
            *arg = low_cng_cells;
            SHR_EXIT();
        case bcmCosqControlEgressPoolAdtHighPriAlpha:
        case bcmCosqControlEgressPoolAdtMargin0:
        case bcmCosqControlEgressPoolAdtMargin1:
        case bcmCosqControlEgressPoolAdtMargin2:
        case bcmCosqControlEgressPoolAdtMargin3:
        case bcmCosqControlEgressPoolAdtMargin4:
        case bcmCosqControlEgressPoolAdtMargin5:
        case bcmCosqControlEgressPoolAdtMargin6:
        case bcmCosqControlEgressPoolAdtMargin7:
        case bcmCosqControlEgressPoolAdtMargin8:
        case bcmCosqControlEgressPoolAdtMargin9:
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_thd_egr_service_pool_adt_get(unit, buffer, pool, type, arg));
            SHR_EXIT();
        default:
            break;
    }

    /* These regs are chip unique, buffer = -1 writes on both itms
       No need to do pipe-itm checking */
    if (buffer == BCM_COSQ_BUFFER_ID_INVALID) {
        itm = 0;
    } else if ((buffer >=  0) && (buffer <  device_info.num_itm)) {
        itm = buffer;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    rv = cosq_thd_egr_service_pool_get(unit, itm, pool, &pool_info, false);
    if (rv == SHR_E_NOT_FOUND) {
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    switch (type) {
        case bcmCosqControlEgressPoolSharedLimitBytes:
            *arg = pool_info.shared_limit;
            break;
        case bcmCosqControlEgressPoolResumeLimitBytes:
            *arg = pool_info.resume_limit;
            break;
        case bcmCosqControlEgressPoolYellowSharedLimitBytes:
            *arg = pool_info.yellow_limit;
            break;
        case bcmCosqControlEgressPoolYellowResumeLimitBytes:
            *arg = pool_info.yellow_resume;
            break;
        case bcmCosqControlEgressPoolRedSharedLimitBytes:
            *arg = pool_info.red_limit;
            break;
        case bcmCosqControlEgressPoolRedResumeLimitBytes:
            *arg = pool_info.red_resume;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_egr_queue_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_type_set(unit, gport, cosq, type, arg));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_egr_queue_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    int rv;
    SHR_FUNC_ENTER(unit);

    rv = cosq_thd_type_get(unit, gport, cosq, type, arg);
    if (rv == SHR_E_NOT_FOUND) {
        *arg = 0;
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_egr_port_pool_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_type_set(unit, gport, cosq, type, arg));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_egr_port_pool_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    int rv;
    SHR_FUNC_ENTER(unit);

    rv = cosq_thd_type_get(unit, gport, cosq, type, arg);
    if (rv == SHR_E_NOT_FOUND) {
        *arg = 0;
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_ing_pool_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t prigroup,
    bcm_cosq_buffer_id_t buffer,
    bcm_cosq_control_t type,
    int arg)
{
    int itm, local_port;
    int pool_id;
    const char *tbl_name;
    const char *key_name;
    bcmi_lt_field_t field;
    SHR_FUNC_ENTER(unit);

    if (arg < 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_port_pg_validate(unit, gport, prigroup));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, gport, &local_port));

    itm = buffer;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_port_itm_validate(unit, local_port, &itm));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_thd_ing_pg_pool_get(unit, local_port, prigroup,
                                            bcmCosqControlIngressPool,
                                            &pool_id));

    sal_memset(&field, 0, sizeof(bcmi_lt_field_t));

    switch (type) {
        case bcmCosqControlIngressPoolLimitBytes:
            tbl_name = TM_ING_THD_SERVICE_POOLs;
            key_name = TM_ING_SERVICE_POOL_IDs;
            field.fld_name = SHARED_LIMIT_CELLSs;
            break;
        case bcmCosqControlIngressPoolResetOffsetLimitBytes:
            tbl_name = TM_ING_THD_SERVICE_POOLs;
            key_name = TM_ING_SERVICE_POOL_IDs;
            field.fld_name = SHARED_RESUME_OFFSET_CELLSs;
            break;
        case bcmCosqControlIngressHeadroomPoolLimitBytes:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_thd_ing_pg_pool_get(unit, local_port, prigroup,
                                                    bcmCosqControlIngressHeadroomPool,
                                                    &pool_id));
            tbl_name = TM_ING_THD_HEADROOM_POOLs;
            key_name = TM_HEADROOM_POOL_IDs;
            field.fld_name = LIMIT_CELLSs;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    field.u.val = arg;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_table_set(unit, tbl_name, key_name, pool_id,
                            BUFFER_POOLs, itm, &field, true));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_ing_pool_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t prigroup,
    bcm_cosq_buffer_id_t buffer,
    bcm_cosq_control_t type,
    int *arg)
{
    int itm, local_port;
    int pool_id;
    const char *tbl_name;
    const char *key_name;
    bcmi_lt_field_t field;
    int rv;

    SHR_FUNC_ENTER(unit);

    if (arg == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_port_pg_validate(unit, gport, prigroup));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, gport, &local_port));

    itm = buffer;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_port_itm_validate(unit, local_port, &itm));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_thd_ing_pg_pool_get(unit, local_port, prigroup,
                                            bcmCosqControlIngressPool,
                                            &pool_id));

    sal_memset(&field, 0, sizeof(bcmi_lt_field_t));

    switch (type) {
        case bcmCosqControlIngressPoolLimitBytes:
            tbl_name = TM_ING_THD_SERVICE_POOLs;
            key_name = TM_ING_SERVICE_POOL_IDs;
            field.fld_name = SHARED_LIMIT_CELLS_OPERs;
            break;
        case bcmCosqControlIngressPoolResetOffsetLimitBytes:
            tbl_name = TM_ING_THD_SERVICE_POOLs;
            key_name = TM_ING_SERVICE_POOL_IDs;
            field.fld_name = SHARED_RESUME_OFFSET_CELLSs;
            break;
        case bcmCosqControlIngressHeadroomPoolLimitBytes:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_thd_ing_pg_pool_get(unit, local_port, prigroup,
                                                    bcmCosqControlIngressHeadroomPool,
                                                    &pool_id));
            tbl_name = TM_ING_THD_HEADROOM_POOLs;
            key_name = TM_HEADROOM_POOL_IDs;
            field.fld_name = LIMIT_CELLS_OPERs;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    rv = cosq_thd_table_get(unit, tbl_name, key_name, pool_id,
                            BUFFER_POOLs, itm, &field, false);

    *arg = field.u.val;
    if (rv == SHR_E_NOT_FOUND) {
        *arg = 0;
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_ing_port_pool_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t prigroup,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_ENTER(unit);

    if (arg < 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_port_pg_validate(unit, gport, prigroup));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_type_set(unit, gport, prigroup, type, arg));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_ing_port_pool_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t prigroup,
    bcm_cosq_control_t type,
    int *arg)
{
    int rv;
    SHR_FUNC_ENTER(unit);

    if (arg == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_port_pg_validate(unit, gport, prigroup));

    rv = cosq_thd_type_get(unit, gport, prigroup, type, arg);
    if (rv == SHR_E_NOT_FOUND) {
        *arg = 0;
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_ing_pg_pool_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t prigroup,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_ENTER(unit);

    if ((arg < 0) || (arg >= MMU_NUM_POOL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_port_pg_validate(unit, gport, prigroup));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_type_set(unit, gport, prigroup, type, arg));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_ing_pg_pool_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t prigroup,
    bcm_cosq_control_t type,
    int *arg)
{
    int rv;
    SHR_FUNC_ENTER(unit);

    rv = cosq_thd_ing_pg_pool_get(unit, gport, prigroup, type, arg);
    if (rv == SHR_E_NOT_FOUND) {
        *arg = 0;
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_ing_port_pg_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t prigroup,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_ENTER(unit);

    if (arg < 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_port_pg_validate(unit, gport, prigroup));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_type_set(unit, gport, prigroup, type, arg));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_ing_port_pg_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t prigroup,
    bcm_cosq_control_t type,
    int *arg)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    if (arg == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_port_pg_validate(unit, gport, prigroup));

    rv = cosq_thd_type_get(unit, gport, prigroup, type, arg);
    if (rv == SHR_E_NOT_FOUND) {
        *arg = 0;
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_alpha_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_drop_limit_alpha_value_t arg)
{
    SHR_FUNC_ENTER(unit);

    if (arg < 0 || arg >= bcmCosqControlDropLimitAlphaCount) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_type_set(unit, gport, cosq, bcmCosqControlDropLimitAlpha, arg));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_alpha_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_drop_limit_alpha_value_t *arg)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = cosq_thd_type_get(unit, gport, cosq, bcmCosqControlDropLimitAlpha,
                           (int *)arg);
    if (rv == SHR_E_NOT_FOUND) {
        *arg = 0;
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_port_uc_spid_get(
    int unit,
    bcm_port_t port,
    int queue,
    int *spid,
    int *use_qgroup_min)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_port_uc_spid_get(unit, port, queue, spid, use_qgroup_min));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_shared_limit_get(
    int unit,
    int itm,
    int spid,
    int *shared_limit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_shared_limit_get(unit, itm, spid, shared_limit));

exit:
    SHR_FUNC_EXIT()
}

int
tm_ltsw_cosq_thd_port_inppri_profile_set(
    int unit,
    bcm_port_t port,
    int profile_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_port_inppri_profile_set(unit, port, profile_id));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_port_inppri_profile_get(
    int unit,
    bcm_port_t port,
    int *profile_id)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = cosq_thd_port_inppri_profile_get(unit, port, profile_id);
    if (rv == SHR_E_NOT_FOUND) {
        *profile_id = 0;
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_port_pg_profile_set(
    int unit,
    bcm_port_t port,
    int profile_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_port_pg_profile_set(unit, port, profile_id));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_port_pg_profile_get(
    int unit,
    bcm_port_t port,
    int *profile_id)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = cosq_thd_port_pg_profile_get(unit, port, profile_id);
    if (rv == SHR_E_NOT_FOUND) {
        *profile_id = 0;
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_service_pool_set(
    int unit,
    bcm_service_pool_id_t id,
    bcm_cosq_service_pool_t cosq_service_pool)
{
    int itm;
    cosq_thd_service_pool_info_t pool_info;
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    if (id < 0 || (id > MMU_NUM_POOL - 1)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&pool_info, 0xFF, sizeof(cosq_thd_service_pool_info_t));

    switch (cosq_service_pool.type) {
        case bcmCosqServicePoolColorAware:
            pool_info.enable_color_limit = cosq_service_pool.enabled;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    for (itm = 0; itm < device_info.num_itm; itm++ ) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_thd_egr_service_pool_set(unit, itm, id, &pool_info));
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_service_pool_get(
    int unit,
    bcm_service_pool_id_t id,
    bcm_cosq_service_pool_t *cosq_service_pool)
{
    int itm = 0;
    cosq_thd_service_pool_info_t pool_info;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if ((id < 0) ||
        (id > MMU_NUM_POOL - 1) ||
        (cosq_service_pool == NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&pool_info, 0xFF, sizeof(cosq_thd_service_pool_info_t));

    rv = cosq_thd_egr_service_pool_get(unit, itm, id, &pool_info, false);
    if (rv == SHR_E_NOT_FOUND) {
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    switch (cosq_service_pool->type) {
        case bcmCosqServicePoolColorAware:
            cosq_service_pool->enabled = pool_info.enable_color_limit;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_service_pool_override_set(
    int unit,
    bcm_cosq_service_pool_override_type_t type,
    bcm_service_pool_id_t service_pool,
    int enable)
{
    cosq_thd_service_pool_override_t info;
    SHR_FUNC_ENTER(unit);

    if ((service_pool < 0) ||
        (service_pool >= MMU_NUM_POOL) ||
        ((enable < 0) || (enable > 1))) {
       SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&info, 0xFF, sizeof(cosq_thd_service_pool_override_t));

    switch (type) {
        case bcmCosqServicePoolOverrideCpu:
            info.cpu_override = enable;
            info.cpu_spid = service_pool;
            break;
        case bcmCosqServicePoolOverrideMcPkt:
            info.mc_override = enable;
            info.mc_spid = service_pool;
            break;
        case bcmCosqServicePoolOverrideMirror:
            info.mirror_override = enable;
            info.mirror_spid = service_pool;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_service_pool_override_set(unit, &info));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_service_pool_override_get(
    int unit,
    bcm_cosq_service_pool_override_type_t type,
    bcm_service_pool_id_t *service_pool,
    int *enable)
{
    cosq_thd_service_pool_override_t info;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if ((service_pool == NULL) ||
        (enable == NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&info, 0, sizeof(cosq_thd_service_pool_override_t));

    rv = cosq_thd_service_pool_override_get(unit, &info);
    if (rv == SHR_E_NOT_FOUND) {
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    switch (type) {
        case bcmCosqServicePoolOverrideCpu:
            *enable = info.cpu_override;
            *service_pool = info.cpu_spid;
            break;
        case bcmCosqServicePoolOverrideMcPkt:
            *enable = info.mc_override;
            *service_pool = info.mc_spid;
            break;
        case bcmCosqServicePoolOverrideMirror:
            *enable = info.mirror_override;
            *service_pool = info.mirror_spid;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_priority_group_mapping_profile_set(
    int unit,
    int profile_index,
    bcm_cosq_priority_group_mapping_profile_type_t type,
    int array_count,
    int *arg)
{
    int i;
    bcmi_lt_field_t field;
    SHR_FUNC_ENTER(unit);

    if ((profile_index < 0) || (profile_index >= MMU_NUM_MAX_PROFILES)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    sal_memset(&field, 0, sizeof(bcmi_lt_field_t));

    switch (type) {
        case bcmCosqInputPriPriorityGroupUcMapping:
            if ((array_count <= 0) || (array_count > MMU_NUM_INT_PRI)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            for (i = 0; i < array_count; i++) {
                if (arg[i] != -1) {
                    if ((arg[i] < 0) || (arg[i] >= MMU_NUM_PG)) {
                        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                    }
                } else {
                    continue;
                }

                field.fld_name = TM_PRI_GRP_IDs;
                field.u.val = arg[i];
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_thd_ing_pri_map_field_set(unit, UC, profile_index,
                                                    i, &field));
            }
            break;
        case bcmCosqInputPriPriorityGroupMcMapping:
            if ((array_count <= 0) || (array_count > MMU_NUM_INT_PRI)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            for (i = 0; i < array_count; i++) {
                if (arg[i] != -1) {
                    if ((arg[i] < 0) || (arg[i] >= MMU_NUM_PG)) {
                        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                    }
                } else {
                    continue;
                }

                field.fld_name = TM_PRI_GRP_IDs;
                field.u.val = arg[i];
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_thd_ing_pri_map_field_set(unit, MC, profile_index,
                                                    i, &field));
            }
            break;
        case bcmCosqPriorityGroupServicePoolMapping:
            if ((array_count <= 0) || (array_count > MMU_NUM_PG)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            for (i = 0; i < array_count; i++) {
                if (arg[i] != -1) {
                    if ((arg[i] < 0) || (arg[i] >= MMU_NUM_POOL)) {
                        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                    }
                } else {
                    continue;
                }

                field.fld_name = TM_ING_SERVICE_POOL_IDs;
                field.u.val = arg[i];
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_thd_pg_pool_map_field_set(unit, -1, profile_index,
                                                    i, &field));
            }
            break;
        case bcmCosqPriorityGroupHeadroomPoolMapping:
            if ((array_count <= 0) || (array_count > MMU_NUM_PG)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            for (i = 0; i < array_count; i++) {
                if (arg[i] != -1) {
                    if ((arg[i] < 0) || (arg[i] >= MMU_NUM_POOL)) {
                        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                    }
                } else {
                    continue;
                }

                field.fld_name = TM_HEADROOM_POOL_IDs;
                field.u.val = arg[i];
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_thd_pg_pool_map_field_set(unit, -1, profile_index,
                                                    i, &field));
            }
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_priority_group_mapping_profile_get(
    int unit,
    int profile_index,
    bcm_cosq_priority_group_mapping_profile_type_t type,
    int array_max,
    int *arg,
    int *array_count)
{
    int i;
    bcmi_lt_field_t field;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if ((profile_index < 0) || (profile_index >= MMU_NUM_MAX_PROFILES)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (((array_max == 0) && (arg != NULL)) ||
        ((array_max != 0) && (arg == NULL))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(array_count, SHR_E_PARAM);

    sal_memset(&field, 0, sizeof(bcmi_lt_field_t));

    *array_count = 0;
    switch (type) {
        case bcmCosqInputPriPriorityGroupUcMapping:
            for (i = 0; ((i < MMU_NUM_INT_PRI) && (i < array_max)); i++) {
                field.fld_name = TM_PRI_GRP_IDs;
                rv = cosq_thd_ing_pri_map_field_get(unit, UC, profile_index,
                                                    i, &field);
                arg[i] = field.u.val;
                if (rv == SHR_E_NOT_FOUND) {
                    rv = SHR_E_NONE;
                }
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
            *array_count = i;
            break;
        case bcmCosqInputPriPriorityGroupMcMapping:
            for (i = 0; ((i < MMU_NUM_INT_PRI) && (i < array_max)); i++) {
                field.fld_name = TM_PRI_GRP_IDs;
                rv = cosq_thd_ing_pri_map_field_get(unit, MC, profile_index,
                                                    i, &field);
                arg[i] = field.u.val;
                if (rv == SHR_E_NOT_FOUND) {
                    rv = SHR_E_NONE;
                }
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
            *array_count = i;
            break;
        case bcmCosqPriorityGroupServicePoolMapping:
            for (i = 0; ((i < MMU_NUM_PG) && (i < array_max)); i++) {
                field.fld_name = TM_ING_SERVICE_POOL_IDs;
                rv = cosq_thd_pg_pool_map_field_get(unit, -1, profile_index,
                                                    i, &field);
                arg[i] = field.u.val;
                if (rv == SHR_E_NOT_FOUND) {
                    rv = SHR_E_NONE;
                }
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
            *array_count = i;
            break;
        case bcmCosqPriorityGroupHeadroomPoolMapping:
            for (i = 0; ((i < MMU_NUM_PG) && (i < array_max)); i++) {
                field.fld_name = TM_HEADROOM_POOL_IDs;
                rv = cosq_thd_pg_pool_map_field_get(unit, -1, profile_index,
                                                    i, &field);
                arg[i] = field.u.val;
                if (rv == SHR_E_NOT_FOUND) {
                    rv = SHR_E_NONE;
                }
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
            *array_count = i;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_port_priority_group_property_set(
    int unit,
    bcm_port_t port,
    int priority_group_id,
    bcm_cosq_port_prigroup_control_t type,
    int arg)
{
    bcmi_lt_field_t field;
    int pause_enable;

    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PORT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    sal_memset(&field, 0, sizeof(bcmi_lt_field_t));

    switch (type) {
        case bcmCosqPriorityGroupLossless:
            if ((priority_group_id < 0) ||
                (priority_group_id >= MMU_NUM_PG)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            field.fld_name = LOSSLESSs;
            field.u.val = arg ? true : false;

            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_thd_ingress_port_pg_field_set(unit, -1, port, priority_group_id,
                                                    &field));
            break;
        case bcmCosqPauseEnable:
            pause_enable = arg ? true : false;

            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_thd_port_pause_set(unit, port, pause_enable));
            /* Disable PFC if pause is enabled */
            if (pause_enable) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_pfc_port_pfc_disable(unit, port));
            }

            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_port_priority_group_property_get(
    int unit,
    bcm_port_t port,
    int priority_group_id,
    bcm_cosq_port_prigroup_control_t type,
    int *arg)
{
    bcmi_lt_field_t field;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    if (BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PORT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    sal_memset(&field, 0, sizeof(bcmi_lt_field_t));

    switch (type) {
        case bcmCosqPriorityGroupLossless:
            if ((priority_group_id < 0) ||
                (priority_group_id >= MMU_NUM_PG)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            field.fld_name = LOSSLESSs;

            rv = cosq_thd_ingress_port_pg_field_get(unit, -1, port,
                                                    priority_group_id, &field);
            *arg = field.u.val;

            if (rv == SHR_E_NOT_FOUND) {
                rv = SHR_E_NONE;
            }
            SHR_IF_ERR_VERBOSE_EXIT(rv);

            break;
        case bcmCosqPauseEnable:
            rv = cosq_thd_port_pause_get(unit, port, arg);
            if (rv == SHR_E_NOT_FOUND) {
                *arg = 0;
                rv = SHR_E_NONE;
            }

            SHR_IF_ERR_VERBOSE_EXIT(rv);
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_cpu_cosq_enable_set(
    int unit,
    bcm_cos_queue_t cosq,
    tm_ltsw_cosq_cpu_cosq_config_t *cpu_cosq_config)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /* 0 */ {PORT_IDs,                   BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 1 */ {TM_MC_Q_IDs,                BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 2 */ {MIN_GUARANTEE_CELLSs,       BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 3 */ {SHARED_LIMIT_CELLS_STATICs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 4 */ {SHARED_LIMITSs,             BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 5 */ {DYNAMIC_SHARED_LIMITSs,     BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 6 */ {COLOR_SPECIFIC_LIMITSs,     BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = bcmi_ltsw_port_cpu(unit);
    fields[1].u.val = cosq;
    fields[2].u.val = cpu_cosq_config->q_min_limit;
    fields[3].u.val = cpu_cosq_config->q_shared_limit;
    fields[4].u.val = cpu_cosq_config->q_limit_enable ? TRUE : FALSE;
    fields[5].u.val = cpu_cosq_config->q_limit_dynamic ? TRUE : FALSE;
    fields[6].u.val = cpu_cosq_config->q_color_limit_enable ? TRUE : FALSE;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_THD_MC_Qs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_cpu_cosq_enable_get(
    int unit,
    bcm_cos_queue_t cosq,
    tm_ltsw_cosq_cpu_cosq_config_t *cpu_cosq_config)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /* 0 */ {PORT_IDs,                   BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 1 */ {TM_MC_Q_IDs,                BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 2 */ {MIN_GUARANTEE_CELLSs,       BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 3 */ {SHARED_LIMIT_CELLS_STATICs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 4 */ {SHARED_LIMITSs,             BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 5 */ {DYNAMIC_SHARED_LIMITSs,     BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 6 */ {COLOR_SPECIFIC_LIMITSs,     BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = bcmi_ltsw_port_cpu(unit);
    fields[1].u.val = cosq;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_THD_MC_Qs,
                           &lt_entry, NULL, NULL));

    cpu_cosq_config->q_min_limit = fields[2].u.val;
    cpu_cosq_config->q_shared_limit = fields[3].u.val;
    cpu_cosq_config->q_limit_enable = fields[4].u.val;
    cpu_cosq_config->q_limit_dynamic = fields[5].u.val;
    cpu_cosq_config->q_color_limit_enable = fields[6].u.val;

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_adt_pri_group_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int arg)
{
    SHR_FUNC_ENTER(unit);

    if (arg < BCM_COSQ_ADT_PRI_GROUP_MIDDLE
        || arg > BCM_COSQ_ADT_PRI_GROUP_HIGH) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_type_set(unit, gport, cosq, bcmCosqControlAdtPriGroup, arg));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_adt_pri_group_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *arg)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = cosq_thd_type_get(unit, gport, cosq, bcmCosqControlAdtPriGroup,
                           (int *)arg);
    if (rv == SHR_E_NOT_FOUND) {
        *arg = 0;
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_adt_low_pri_mon_pool_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_buffer_id_t buffer,
    bcm_cosq_control_t type,
    int arg)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {BUFFER_POOLs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {TM_EGR_SERVICE_POOL_IDs,       BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    int itm, from_itm, to_itm;
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    if ((gport != -1) || (cosq != -1)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    /* These regs are chip unique, buffer = -1 writes on both itms
       No need to do pipe-itm checking */

    if (buffer == BCM_COSQ_BUFFER_ID_INVALID) {
        from_itm = 0;
        to_itm = device_info.num_itm - 1;
    } else if ((buffer >=  0) && (buffer < device_info.num_itm)) {
        from_itm = to_itm = buffer;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    for (itm = from_itm; itm <= to_itm; itm++ ) {
        lt_entry.fields = fields;
        lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
        lt_entry.attr = 0;

        fields[0].u.val = itm;
        fields[1].u.val = arg;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set(unit, CTR_TM_THD_CONTROLs,
                               &lt_entry, NULL));
    }

exit:
    SHR_FUNC_EXIT();

}

int
tm_ltsw_cosq_thd_adt_low_pri_mon_pool_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_buffer_id_t buffer,
    bcm_cosq_control_t type,
    int *arg)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {BUFFER_POOLs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {TM_EGR_SERVICE_POOL_IDs,       BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    int itm;
    bcmi_ltsw_cosq_device_info_t device_info;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    if ((gport != -1) || (cosq != -1)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* These regs are chip unique, retrieve value from itm 0 if buffer == -1 */
    if (buffer == BCM_COSQ_BUFFER_ID_INVALID) {
        itm = 0;
    } else if ((buffer >=  0) && (buffer < device_info.num_itm)) {
        itm = buffer;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = itm;

    rv = bcmi_lt_entry_get(unit, CTR_TM_THD_CONTROLs,
                           &lt_entry, NULL, NULL);
    if (rv == SHR_E_NOT_FOUND) {
        *arg = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    *arg = fields[1].u.val;

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_adt_low_pri_mon_pool_clear(
    int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, CTR_TM_THD_CONTROLs));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_egr_service_pool_adt_clear(
    int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, TM_EGR_SERVICE_POOL_DYNAMICs));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_hdrm_pool_limit_set(
    int unit,
    int hdrm_pool,
    int num_lossless_class,
    int arr_size,
    bcm_cosq_pkt_size_dist_t *pkt_dist_array)
{
    int pg_hdrm, pool_hdrm, i;
    bcm_cosq_pkt_size_dist_t *size_dist = NULL;
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    int speed = 0, speed_max = 0;
    int mtu = 0, port_mtu = 0;
    int itm;

    SHR_FUNC_ENTER(unit);

    if ((arr_size > MMU_NUM_MAX_PACKET_SIZE) || (hdrm_pool > MMU_NUM_POOL)
        || (num_lossless_class > MMU_NUM_PG) || (pkt_dist_array == NULL) ) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_ALLOC(size_dist, arr_size * sizeof(bcm_cosq_pkt_size_dist_t),
              "bcmCosqPktSizeDist");
    SHR_NULL_CHECK(size_dist, SHR_E_MEMORY);
    sal_memset(size_dist, 0, arr_size * sizeof(bcm_cosq_pkt_size_dist_t));

    for (i = 0; i < arr_size; i++) {
        size_dist[i].pkt_size = pkt_dist_array[i].pkt_size;
        size_dist[i].dist_perc = pkt_dist_array[i].dist_perc;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_PORT, &pbmp));

    BCM_PBMP_ITER(pbmp, port) {
        if (bcmi_ltsw_port_is_type(unit, port,  BCMI_LTSW_PORT_TYPE_LB)) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_speed_max(unit, port, &speed_max));
        speed = MAX(speed, speed_max);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_frame_max_get(unit, port, &port_mtu));
        mtu = MAX(mtu, port_mtu);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_port_pg_headroom_get(unit, speed, MMU_DEF_HDRM_CABLE_LEN,
                                        mtu, arr_size, size_dist, &pg_hdrm));

    pool_hdrm = pg_hdrm * num_lossless_class;

    for (itm = 0; itm < ITMS_PER_DEV; itm ++) {
        SHR_IF_ERR_VERBOSE_EXIT
              (cosq_thd_delta_hdrm_service_pools_adjust(unit, itm, hdrm_pool, 0,
                                                        pool_hdrm));
    }

exit:
    SHR_FREE(size_dist);
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_thd_port_prigrp_hdrm_set(
    int unit,
    bcm_port_t port,
    uint32_t pri_bmp,
    int cable_len,
    int arr_size,
    bcm_cosq_pkt_size_dist_t *pkt_dist_array)
{
    int i;
    int pg_hdrm;
    int pg_hdrm_byte;
    bcm_cosq_pkt_size_dist_t *size_dist = NULL;
    int speed = 0;
    int mtu = 0;
    int pri_pg_prof, pri_i, uc_pg, mc_pg, pg_i;
    bcm_port_resource_t resource;
    uint32_t pg_bmp = 0x0;
    bcm_cosq_control_t type;
    bcmi_lt_field_t field;
    int rv = SHR_E_NONE;
    bcmi_ltsw_cosq_device_info_t device_info = {0};

    SHR_FUNC_ENTER(unit);

    if ((arr_size > MMU_NUM_MAX_PACKET_SIZE) || (pri_bmp > 0xffff)
        || (pkt_dist_array == NULL) ) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    SHR_ALLOC(size_dist, arr_size * sizeof(bcm_cosq_pkt_size_dist_t),
              "bcmCosqPktSizeDist");
    SHR_NULL_CHECK(size_dist, SHR_E_MEMORY);
    sal_memset(size_dist, 0, arr_size * sizeof(bcm_cosq_pkt_size_dist_t));

    for (i = 0; i < arr_size; i++) {
        size_dist[i].pkt_size = pkt_dist_array[i].pkt_size;
        size_dist[i].dist_perc = pkt_dist_array[i].dist_perc;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_port_resource_speed_get(unit, port, &resource));
    speed = resource.speed;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_port_frame_max_get(unit, port, &mtu));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_thd_port_pg_headroom_get(unit, speed, cable_len, mtu, arr_size,
                                       size_dist, &pg_hdrm));

    /* Retrieve internal priority to PG mapping profile */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_thd_port_inppri_profile_get(unit, port, &pri_pg_prof));

    sal_memset(&field, 0, sizeof(bcmi_lt_field_t));
    field.fld_name = TM_PRI_GRP_IDs;
    for (pri_i = 0; pri_i < MMU_NUM_INT_PRI; pri_i++) {
        if (pri_bmp & (1U << pri_i)) {
            rv = cosq_thd_ing_pri_map_field_get(unit, UC, pri_pg_prof,
                                                pri_i, &field);
            uc_pg = field.u.val;
            if (rv == SHR_E_NOT_FOUND) {
                rv = SHR_E_NONE;
            }
            SHR_IF_ERR_VERBOSE_EXIT(rv);

            pg_bmp |= (1U << uc_pg);

            rv = cosq_thd_ing_pri_map_field_get(unit, MC, pri_pg_prof,
                                                pri_i, &field);
            mc_pg = field.u.val;
            if (rv == SHR_E_NOT_FOUND) {
                rv = SHR_E_NONE;
            }
            SHR_IF_ERR_VERBOSE_EXIT(rv);

            pg_bmp |= (1U << mc_pg);
        }
    }

    type = bcmCosqControlIngressPortPGHeadroomLimitBytes;
    for (pg_i = 0; pg_i < MMU_NUM_PG; pg_i++) {
        if (pg_bmp & (1U << pg_i)) {
            /* Set new value to PG headroom limit */
            pg_hdrm_byte = pg_hdrm * device_info.mmu_cell_size;
            SHR_IF_ERR_VERBOSE_EXIT
               (tm_ltsw_cosq_thd_ing_port_pg_set(unit, port, pg_i, type,
                                                 pg_hdrm_byte));
        }
    }

exit:
    SHR_FREE(size_dist);
    SHR_FUNC_EXIT();
}

