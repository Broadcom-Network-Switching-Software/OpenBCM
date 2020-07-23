/* \file cosq.c
 *
 * TM COSQ Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/control.h>

#include <bcm_int/ltsw/mbcm/cosq.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/tm/cosq.h>
#include <bcm_int/ltsw/cosq_int.h>
#include <bcm_int/ltsw/cosq_thd.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/init.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/profile_mgmt.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COSQ

#define MAX_TM_SCHEDULER_NODE 12
#define COSQ_PROFILE_INDEX_INVALID 0xffffffff
#define COSQ_DEFAULT_PROFILE_ID 0
#define COSQ_NUM_INT_PRI        16

#define DEVICE_RX_MAX_CPU_COS 128

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/* WRED drop percentage. */
typedef enum cosq_wred_drop_percentage_e {
    /* Zero drop rate. */
    COSQ_WRED_DROP_PERCENTAGE_0 = 0,

    /* Drop rate 1%. */
    COSQ_WRED_DROP_PERCENTAGE_1 = 1,

    /* Drop rate 2%. */
    COSQ_WRED_DROP_PERCENTAGE_2 = 2,

    /* Drop rate 3%. */
    COSQ_WRED_DROP_PERCENTAGE_3 = 3,

    /* Drop rate 4%. */
    COSQ_WRED_DROP_PERCENTAGE_4 = 4,

    /* Drop rate 5%. */
    COSQ_WRED_DROP_PERCENTAGE_5 = 5,

    /* Drop rate 6%. */
    COSQ_WRED_DROP_PERCENTAGE_6 = 6,

    /* Drop rate 7%. */
    COSQ_WRED_DROP_PERCENTAGE_7 = 7,

    /* Drop rate 8%. */
    COSQ_WRED_DROP_PERCENTAGE_8 = 8,

    /* Drop rate 9%. */
    COSQ_WRED_DROP_PERCENTAGE_9 = 9,

    /* Drop rate 10%. */
    COSQ_WRED_DROP_PERCENTAGE_10 = 10,

    /* Drop rate 25%. */
    COSQ_WRED_DROP_PERCENTAGE_25 = 25,

    /* Drop rate 50%. */
    COSQ_WRED_DROP_PERCENTAGE_50 = 50,

    /* Drop rate 75%. */
    COSQ_WRED_DROP_PERCENTAGE_75 = 75,

    /* Drop rate 100%. */
    COSQ_WRED_DROP_PERCENTAGE_100 = 100,
} cosq_wred_drop_percentage_t;

/*
 * \brief WRED drop curve profile structure.
 *
 * This data structure is used to identify WRED drop curve profile structure.
 */
typedef struct cosq_wred_drop_curve_profile_s {
    /* responsive drop green minimum threshold. */
    uint32_t                     responsive_green_drop_min_thd_cells;

    /* responsive drop green maximum threshold. */
    uint32_t                     responsive_green_drop_max_thd_cells;

    /* responsive drop green drop percentage. */
    cosq_wred_drop_percentage_t  responsive_green_drop_percentage;

    /* responsive drop yellow minimum threshold. */
    uint32_t                     responsive_yellow_drop_min_thd_cells;

    /* responsive drop yellow maximum threshold. */
    uint32_t                     responsive_yellow_drop_max_thd_cells;

    /* responsive drop yellow drop percentage. */
    cosq_wred_drop_percentage_t  responsive_yellow_drop_percentage;

    /* responsive drop red minimum threshold. */
    uint32_t                     responsive_red_drop_min_thd_cells;

    /* responsive drop red maximum threshold. */
    uint32_t                     responsive_red_drop_max_thd_cells;

    /* responsive drop red drop percentage. */
    cosq_wred_drop_percentage_t  responsive_red_drop_percentage;

    /* non-responsive drop green minimum threshold. */
    uint32_t                     non_responsive_green_drop_min_thd_cells;

    /* non-responsive drop green maximum threshold. */
    uint32_t                     non_responsive_green_drop_max_thd_cells;

    /* non-responsive drop green drop percentage. */
    cosq_wred_drop_percentage_t  non_responsive_green_drop_percentage;

    /* non-responsive drop yellow minimum threshold. */
    uint32_t                     non_responsive_yellow_drop_min_thd_cells;

    /* non-responsive drop yellow maximum threshold. */
    uint32_t                     non_responsive_yellow_drop_max_thd_cells;

    /* non-responsive drop yellow drop percentage. */
    cosq_wred_drop_percentage_t  non_responsive_yellow_drop_percentage;

    /* non-responsive drop red minimum threshold. */
    uint32_t                     non_responsive_red_drop_min_thd_cells;

    /* non-responsive drop red maximum threshold. */
    uint32_t                     non_responsive_red_drop_max_thd_cells;

    /* non-responsive drop red drop percentage. */
    cosq_wred_drop_percentage_t  non_responsive_red_drop_percentage;

    /* ecn marking green minimum threshold. */
    uint32_t                     ecn_green_drop_min_thd_cells;

    /* ecn marking green maximum threshold. */
    uint32_t                     ecn_green_drop_max_thd_cells;

    /* ecn marking green drop percentage. */
    cosq_wred_drop_percentage_t  ecn_green_drop_percentage;

    /* ecn marking yellow minimum threshold. */
    uint32_t                     ecn_yellow_drop_min_thd_cells;

    /* ecn marking yellow maximum threshold. */
    uint32_t                     ecn_yellow_drop_max_thd_cells;

    /* ecn marking yellow drop percentage. */
    cosq_wred_drop_percentage_t  ecn_yellow_drop_percentage;

    /* ecn marking red minimum threshold. */
    uint32_t                     ecn_red_drop_min_thd_cells;

    /* ecn marking red maximum threshold. */
    uint32_t                     ecn_red_drop_max_thd_cells;

    /* ecn marking red drop percentage. */
    cosq_wred_drop_percentage_t  ecn_red_drop_percentage;
} cosq_wred_drop_curve_profile_t;

typedef struct cosq_wred_drop_curve_field_s {
    /* Field name. */
    const char      *min_name;
    /* Field name. */
    const char      *max_name;
    /* Field name. */
    const char      *rate_name;
} cosq_wred_drop_curve_field_t;

/* Added for Queue scheduler mapping */
typedef struct cosq_cfg_scheduler_profile_s {
    int num_ucq;            /* Number of unicast queues. Valid values are 0-2 */
    int num_mcq;            /* Number of multicast queues. Valid values are 0-1 */
    int strict_priority;    /* Schedudling priority is strict. 0: Not strict, 1:
                               strict */
    int fc_is_uc_only;      /* Flow control is set for uc only packets */
} cosq_scheduler_profile_t;

static tm_ltsw_cosq_chip_driver_t *tm_cosq_chip_driver_cb[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Private functions
 */

/*
 * \brief MC queue mode get.
 *
 * \param [in]  unit          Unit number.
 * \param [out] mc_q_mode     mc_q_mode.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_mcq_mode_get(int unit, int *mc_q_mode)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {NUM_MC_Qs, BCMI_LT_FIELD_F_GET | BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    rv = bcmi_lt_entry_get(unit, TM_SCHEDULER_CONFIGs,
                           &lt_entry, NULL, NULL);

    if (rv == SHR_E_NONE) {
        if (sal_strcasecmp(fields[0].u.sym_val, NUM_MC_Q_0s) == 0) {
            *mc_q_mode = 0;
        } else if (sal_strcasecmp(fields[0].u.sym_val, NUM_MC_Q_2s) == 0) {
            *mc_q_mode = 1;
        } else if (sal_strcasecmp(fields[0].u.sym_val, NUM_MC_Q_4s) == 0) {
            *mc_q_mode = 2;
        } else if (sal_strcasecmp(fields[0].u.sym_val, NUM_MC_Q_6s) == 0) {
            *mc_q_mode = 3;
        } else {
            *mc_q_mode = 2;
        }
    } else if (rv == SHR_E_NOT_FOUND) {
        *mc_q_mode = 2;
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit,
            "Fetch mcq mode failed! Using default value 2.\n")));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
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
 * \brief Check port type.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  port          PORT ID.
 *
 * \retval TRUE               Port is rdb port.
 * \retval FALSE              Port is non rdb port.
 */
static int
is_rdb_port(int unit, bcm_port_t lport)
{
    return bcmi_ltsw_port_is_type(unit, lport,  BCMI_LTSW_PORT_TYPE_RDB);
}


/*
 * \brief Retrieve the map state of a specified LT field.
 *
 * This function retrieves the map state of the specified
 * LT field on a particular device.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  tbl_name      LT name.
 * \param [in]  fld_name      LT field name.
 * \param [out] valid         valid.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
lt_field_validation(int unit, const char *tbl_name,
                    const char *fld_name, bool *valid)
{
    int rv = SHR_E_NONE;
    uint32_t ltid;
    uint32_t lfid;

    SHR_FUNC_ENTER(unit);

    rv = bcmlt_table_field_id_get_by_name(unit, tbl_name, fld_name,
                                          &ltid, &lfid);
    if (rv == SHR_E_NOT_FOUND) {
        *valid = FALSE;
        SHR_EXIT();
    } else {
        *valid = TRUE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get port map information.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lport         Logic port number.
 * \param [out] info          ltsw_cosq_port_map_info_t.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_port_map_info_get(int unit, bcm_port_t lport,
                          bcmi_ltsw_cosq_port_map_info_t *info)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t value;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    if (info == NULL){
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, lport, &lport));

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_PORT_MAP_INFOs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_IDs,
                               lport));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, PC_PHYS_PORT_IDs,
                               &value));
    info->phy_port = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, TM_PIPEs,
                               &value));
    info->pipe = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, TM_LOCAL_PORTs,
                               &value));
    info->local_port = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, TM_SPARSE_GLOBAL_PORTs,
                               &value));
    info->mmu_port = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, TM_COMPACT_GLOBAL_PORTs,
                               &value));
    info->global_port = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, BASE_UC_Qs,
                               &value));
    info->uc_base = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, NUM_UC_Qs,
                               &value));
    info->num_uc_q = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, BASE_MC_Qs,
                               &value));
    info->mc_base = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, NUM_MC_Qs,
                               &value));
    info->num_mc_q = value;

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Get device specific TM information.
 *
 * \param [in]   unit         Unit number.
 * \param [out]  info         bcmi_ltsw_cosq_device_info_t.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_device_info_get(int unit, bcmi_ltsw_cosq_device_info_t *info)
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
        (bcmlt_entry_allocate(dunit, TM_DEVICE_INFOs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, MAX_PKT_SIZEs,
                               &value));
    info->max_pkt_byte = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, PKT_HDR_SIZEs,
                               &value));
    info->mmu_hdr_byte = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, JUMBO_PKT_SIZEs,
                               &value));
    info->jumbo_pkt_size = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, DEFAULT_MTUs,
                               &value));
    info->default_mtu_size = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, CELL_SIZEs,
                               &value));
    info->mmu_cell_size = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, NUM_CELLSs,
                               &value));
    info->mmu_total_cell = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, NUM_PORT_PRI_GRPs,
                               &value));
    info->num_pg = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, NUM_SERVICE_POOLs,
                               &value));
    info->num_service_pool = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, NUM_Qs,
                               &value));
    info->num_queue = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, NUM_CPU_Qs,
                               &value));
    info->num_cpu_queue = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, NUM_PIPEs,
                               &value));
    info->num_pipe = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, NUM_BUFFER_POOLs,
                               &value));
    info->num_itm = value;

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Set cut-through mode for a specific port
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] enable Enable to use cut-through mode.
 * \param [in] flags 0: init, 1: set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_tm_port_ct_mode_set(
    int unit,
    bcm_port_t port,
    int enable,
    uint32_t flags)
{
    bcmi_lt_entry_t lt_entry;
    int rv = SHR_E_NONE;
    int rv1 = SHR_E_NONE;
    bcmi_lt_field_t fields[] =
    {
        /* 0 */ {PORT_IDs,           BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 1 */ {CUT_THROUGHs,       0, 0, {0}},
        /* 2 */ {OPERATIONAL_STATEs, BCMI_LT_FIELD_F_GET |
                                     BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    if ((enable != 1) && (enable != 0)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = port;

    if (flags == 1) {
        fields[1].flags = BCMI_LT_FIELD_F_SET;
        fields[1].u.val = enable ? TRUE : FALSE;
    }

    rv = bcmi_lt_entry_set(unit, TM_CUT_THROUGH_PORTs, &lt_entry, NULL);

    fields[1].flags = 0;

    rv1 = bcmi_lt_entry_get(unit, TM_CUT_THROUGH_PORTs,
                           &lt_entry, NULL, NULL);

    if (rv1 == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FAIL);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv1);
    }

    if (sal_strcasecmp(fields[2].u.sym_val, VALIDs) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else if (sal_strcasecmp(fields[2].u.sym_val, PORT_INFO_UNAVAILABLEs) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FAIL);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}


/*
 * \brief Get cut-through mode for a specific port
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [out] enable Enable to use cut-through mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_tm_port_ct_mode_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /* 0 */ {PORT_IDs,         BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 1 */ {CUT_THROUGHs,     BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = port;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_lt_entry_get(unit, TM_CUT_THROUGH_PORTs,
                           &lt_entry, NULL, NULL), SHR_E_NOT_FOUND);

    if (enable != NULL) {
        *enable = fields[1].u.val;
    }

exit:
    SHR_FUNC_EXIT();
}



/*
 * \brief Function to Set Parent(L0) for a given L1 queue of the CPU port
 *        Note: Only child @ L1 level is supported.
 *        Because only L1 level queues can be attached to different
 *        parent (L0.0-L0.9) of the CPU port.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  child_index   child node index.
 * \param [in]  child_level   child node level.
 * \param [in]  parent_index  parent node index.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_cpu_parent_set(int unit, int child_index, int child_level,
                    int parent_index)
{
    bcmi_lt_entry_t lt_entry;
    tm_ltsw_cosq_chip_driver_t *drv = NULL;
    const char *sym_val = NULL;
    bcmi_lt_field_t fields[] =
    {
        /* 0 */ {SCHED_NODEs,                    BCMI_LT_FIELD_F_SET |
                                                 BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /* 1 */ {TM_SCHEDULER_SHAPER_CPU_NODE_IDs,        BCMI_LT_FIELD_F_SET,
                                                          0, {0}},
        /* 2 */ {PARENT_TM_SCHEDULER_SHAPER_CPU_NODE_IDs, BCMI_LT_FIELD_F_SET,
                                                          0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    if (child_level != LTSW_COSQ_NODE_LVL_MC) {
        /* Only child at L1 are allowed to move */
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_cosq_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);

    if (drv->cosq_sched_node_type_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (drv->cosq_sched_node_type_get(unit, child_level, &sym_val));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    fields[0].u.sym_val = sym_val;
    fields[1].u.val = child_index;
    fields[2].u.val = parent_index;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_SCHEDULER_SHAPER_CPU_NODEs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Function to Get Parent(L0) for a given L1 queue of the CPU port
 *        Note: Only child @ L1 level is supported.
 *        Because only L1 level queues can be attached to different
 *        parent (L0.0-L0.9) of the CPU port.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  child_index   child node index.
 * \param [in]  child_level   child node level.
 * \param [out] parent_index  parent node index.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_cpu_parent_get(int unit, int child_index, int child_level,
                       int *parent_index)
{
    bcmi_lt_entry_t lt_entry;
    tm_ltsw_cosq_chip_driver_t *drv = NULL;
    const char *sym_val = NULL;
    int rv;
    bcmi_lt_field_t fields[] =
    {
        /* 0 */ {SCHED_NODEs,                   BCMI_LT_FIELD_F_SET |
                                                BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /* 1 */ {TM_SCHEDULER_SHAPER_CPU_NODE_IDs,        BCMI_LT_FIELD_F_SET,
                                                          0, {0}},
        /* 2 */ {PARENT_TM_SCHEDULER_SHAPER_CPU_NODE_IDs, BCMI_LT_FIELD_F_GET,
                                                          0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    if (child_level != LTSW_COSQ_NODE_LVL_MC) {
        /* Only child at L1 are allowed to move */
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_cosq_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);

    if (drv->cosq_sched_node_type_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (drv->cosq_sched_node_type_get(unit, child_level, &sym_val));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    fields[0].u.sym_val = sym_val;
    fields[1].u.val = child_index;

    rv = bcmi_lt_entry_get(unit, TM_SCHEDULER_SHAPER_CPU_NODEs,
                           &lt_entry, NULL, NULL);

    *parent_index = fields[2].u.val;

    if (rv == SHR_E_NOT_FOUND) {
        *parent_index = -1;
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Retrieve a cos to CMC channel state.
 *
 * \param [in] unit           Unit number.
 * \param [in] rx_q           channel id.
 * \param [in] input_cos      input_cos.
 * \param [out] enable        enable.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_rx_queue_get(int unit, int rx_q, int input_cos, bool *enable)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_field_def_t field_def;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint32_t r_elem_cnt = 0, i = 0;
    uint64_t cos[DEVICE_RX_MAX_CPU_COS];
    uint64_t qid;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit, DEVICE_PKT_RX_Qs, COSs, &field_def));
    if (input_cos > field_def.elements) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    qid = rx_q;
    *enable = FALSE;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DEVICE_PKT_RX_Qs, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, RX_Qs, qid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_get(eh, COSs, 0, cos,
                                     DEVICE_RX_MAX_CPU_COS, &r_elem_cnt));

    for (i = 0; i < DEVICE_RX_MAX_CPU_COS; i++) {
        if (input_cos == i) {
            if (cos[i] != 0) {
                *enable = TRUE;
            } else {
                *enable = FALSE;
            }

            break;
        }
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Associate a cos to CMC channel.
 *
 * \param [in] unit           Unit number.
 * \param [in] rx_q           channel id.
 * \param [in] input_cos      input_cos.
 * \param [in] enable         enable.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_rx_queue_set(int unit, int rx_q, int input_cos, bool enable)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_field_def_t field_def;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t data = enable;
    uint64_t qid;

    SHR_FUNC_ENTER(unit);

    qid = rx_q;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit, DEVICE_PKT_RX_Qs, COSs, &field_def));
    if (input_cos > field_def.elements) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DEVICE_PKT_RX_Qs, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, RX_Qs, qid));

    /* Data field. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_add(eh, COSs, input_cos, &data, 1));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

    (void)bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Associate a cos to CMC channel.
 *
 * \param [in] unit           Unit number.
 * \param [in] input_cos      input_cos.
 * \param [in] parent_cos     parent_cos.
 * \param [in] enable         enable.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_rx_queue_channel_set(int unit, int input_cos, int channel_id, bool enable)
{
    uint64_t min_qid, max_qid;
    int rx_q, qid;

    SHR_FUNC_ENTER(unit);

    rx_q = channel_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, DEVICE_PKT_RX_Qs, RX_Qs,
                                       &min_qid, &max_qid));

    for (qid = min_qid; qid <= max_qid; qid++) {

        if (enable && (qid == rx_q)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_tm_rx_queue_set(unit, qid, input_cos, TRUE));
        } else {
            /* Clear all other rx_qs */
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_tm_rx_queue_set(unit, qid, input_cos, FALSE));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Function to Set node sched mode and weight of the CPU port.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  node_id       node index.
 * \param [in]  node_type     node type.
 * \param [in]  sched_mode    sched mode.
 * \param [in]  weight        weight.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_cpu_node_mode_set(int unit, int node_id, int node_type,
                          int sched_mode, int weight)
{
    bcmi_lt_entry_t lt_entry;
    tm_ltsw_cosq_chip_driver_t *drv = NULL;
    const char *sym_val = NULL;
    const char *sym_val1 = NULL;
    bcmi_lt_field_t fields[] =
    {
        /* 0 */ {SCHED_NODEs,                  BCMI_LT_FIELD_F_SET |
                                               BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /* 1 */ {TM_SCHEDULER_SHAPER_CPU_NODE_IDs,  BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 2 */ {SCHED_MODEs,                  BCMI_LT_FIELD_F_SET |
                                               BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /* 3 */ {WEIGHTs,                      BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_cosq_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);

    if (drv->cosq_sched_node_type_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (drv->cosq_sched_node_type_get(unit, node_type, &sym_val));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    fields[0].u.sym_val = sym_val;
    fields[1].u.val = node_id;

    if (drv->cosq_sched_mode_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (drv->cosq_sched_mode_get(unit, &sched_mode, &sym_val1, true));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }
    fields[2].u.sym_val = sym_val1;
    fields[3].u.val = weight;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_SCHEDULER_SHAPER_CPU_NODEs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Function to Get node sched mode and weight of the CPU port.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  node_id       node index.
 * \param [in]  node_type     node type.
 * \param [out] sched_mode    sched mode.
 * \param [out] weight        weight.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_cpu_node_mode_get(int unit, int node_id, int node_type,
                          int *sched_mode, int *weight)
{
    bcmi_lt_entry_t lt_entry;
    tm_ltsw_cosq_chip_driver_t *drv = NULL;
    const char *sym_val = NULL;
    bcmi_lt_field_t fields[] =
    {
        /* 0 */ {SCHED_NODEs,                   BCMI_LT_FIELD_F_SET |
                                                BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /* 1 */ {TM_SCHEDULER_SHAPER_CPU_NODE_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 2 */ {SCHED_MODEs,                   BCMI_LT_FIELD_F_GET |
                                                BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /* 3 */ {WEIGHTs,                       BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_cosq_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);

    if (drv->cosq_sched_node_type_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (drv->cosq_sched_node_type_get(unit, node_type, &sym_val));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    fields[0].u.sym_val = sym_val;
    fields[1].u.val = node_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_SCHEDULER_SHAPER_CPU_NODEs,
                           &lt_entry, NULL, NULL));

    if (drv->cosq_sched_mode_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (drv->cosq_sched_mode_get(unit, sched_mode,
                                      &(fields[2].u.sym_val), false));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    *weight = fields[3].u.val;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Function to Set cpu port sched mode
 *
 * \param [in]  unit          Unit number.
 * \param [in]  en_wrr        enable wrr.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_cpu_schedule_set(int unit, int en_wrr)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /* 0 */ {WRRs,     BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = en_wrr;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_SCHEDULER_CPU_PORTs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Function to Set cpu port sched mode
 *
 * \param [in]  unit          Unit number.
 * \param [out] en_wrr        wrr.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_cpu_schedule_get(int unit, int *en_wrr)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /* 0 */ {WRRs,     BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_SCHEDULER_CPU_PORTs,
                           &lt_entry, NULL, NULL));
    *en_wrr = fields[0].u.val;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_SCHEDULER_SP_PROFILE get.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  profile_id    Profile id.
 * \param [in]  node_id       node_id.
 * \param [out] sp            strict priority.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_schedule_sp_profile_get(int unit, int profile_id, int node_id, int *sp)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    union {
        /* scalar data. */
        uint64_t    array[MAX_TM_SCHEDULER_NODE];
        /* symbol data. */
        const char  *sym_val;
    } fval;
    uint32_t r_elem_cnt;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    if (node_id >= MAX_TM_SCHEDULER_NODE) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_SCHEDULER_SP_PROFILEs, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, TM_SCHEDULER_PROFILE_IDs,
                               profile_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_get(eh, STRICT_PRIORITY_OPERs, 0, fval.array,
                                     MAX_TM_SCHEDULER_NODE, &r_elem_cnt));

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "lt %s lookup %s=%d %s[%d]=%d\n"),
            TM_SCHEDULER_SP_PROFILEs,
            TM_SCHEDULER_PROFILE_IDs, profile_id,
            STRICT_PRIORITY_OPERs, node_id, (int)fval.array[node_id]));

    *sp = fval.array[node_id];

exit:
    if (BCMLT_INVALID_HDL != eh) {
        (void) bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}


/*
 * \brief TM_SCHEDULER_SP_PROFILE set.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  profile_id    Profile id.
 * \param [in]  node_id       node_id.
 * \param [in]  sp            strict priority.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_schedule_sp_profile_set(int unit, int profile_id, int node_id, int sp)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t data = sp;

    SHR_FUNC_ENTER(unit);

    if (node_id >= MAX_TM_SCHEDULER_NODE) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_SCHEDULER_SP_PROFILEs, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, TM_SCHEDULER_PROFILE_IDs, profile_id));

    /* Data field. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_add(eh, STRICT_PRIORITYs, node_id, &data, 1));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "lt %s update %s=%d %s[%d]=%d\n"),
            TM_SCHEDULER_SP_PROFILEs,
            TM_SCHEDULER_PROFILE_IDs, profile_id,
            STRICT_PRIORITYs, node_id, sp));

    (void)bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief port schedule profile set.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lport         Logic port number.
 * \param [in]  profile_id    Profile id.
 * \param [in]  en_wrr        enable wrr.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_schedule_port_profile_set(int unit, bcm_port_t lport,
                                  int profile_id, int en_wrr)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /* 0 */ {PORT_IDs,                     BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 1 */ {TM_SCHEDULER_PROFILE_IDs,     0, 0, {0}},
        /* 2 */ {WRRs,                         0, 0, {0}},
        /* 3 */ {OPERATIONAL_STATEs,           BCMI_LT_FIELD_F_GET |
                                               BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = lport;
    if (profile_id != -1) {
        fields[1].flags = BCMI_LT_FIELD_F_SET;
        fields[1].u.val = profile_id;
    }

    if (en_wrr != -1) {
        fields[2].flags = BCMI_LT_FIELD_F_SET;
        fields[2].u.val = en_wrr ? TRUE : FALSE;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_SCHEDULER_PORT_PROFILEs,
                           &lt_entry, NULL));

    fields[1].flags = 0;
    fields[2].flags = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_SCHEDULER_PORT_PROFILEs,
                           &lt_entry, NULL, NULL));

    if (sal_strcasecmp(fields[3].u.sym_val, VALIDs) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NONE);
    } else if (sal_strcasecmp(fields[3].u.sym_val, PROFILE_INVALIDs) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}


/*
 * \brief Get port profile id.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lport         Logic port number.
 * \param [out] profile_id    Profile id.
 * \param [out] en_wrr        wrr.
 * \param [out] state         entry state.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_schedule_port_profile_get(int unit, bcm_port_t lport,
                                  int *profile_id, int *en_wrr)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /* 0 */ {PORT_IDs,                   BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 1 */ {TM_SCHEDULER_PROFILE_IDs,   BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 2 */ {WRRs,                       BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 3 */ {OPERATIONAL_STATEs,         BCMI_LT_FIELD_F_GET |
                                             BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    fields[0].u.val = lport;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_SCHEDULER_PORT_PROFILEs,
                           &lt_entry, NULL, NULL));

    if (profile_id != NULL) {
        *profile_id = fields[1].u.val;
    }

    if (en_wrr != NULL) {
        *en_wrr = fields[2].u.val;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief schedule node state check.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_state op_state.
 * \param [in] lport logic port.
 * \param [in] node_id node id.
 * \param [in] node_type node type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_tm_schedule_node_state_check(int unit, const char *op_state,
                                  bcm_port_t lport, int node_id,
                                  const char *node_type)
{
    SHR_FUNC_ENTER(unit);

    if (sal_strcasecmp(op_state, VALIDs) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NONE);
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "port %d %s : %d update error: %s\n"),
                lport, node_type, node_id, op_state));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief node schedule profile get.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lport         Logic port number.
 * \param [in]  node_id       Node id.
 * \param [in]  node_type     node type.
 * \param [out] sched_mode    sched mode.
 * \param [out] weight        weight.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_schedule_node_get(int unit, bcm_port_t lport, int node_id,
                          int node_type, int *sched_mode, int *weight,
                          bool check_state)
{
    bcmi_lt_entry_t lt_entry;
    tm_ltsw_cosq_chip_driver_t *drv = NULL;
    const char *sym_val = NULL;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,               BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {SCHED_NODEs,            BCMI_LT_FIELD_F_SET |
                                          BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
         /* 2 */ {TM_SCHEDULER_NODE_IDs,  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 3 */ {SCHED_MODEs,            BCMI_LT_FIELD_F_GET |
                                          BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
         /* 4 */ {WEIGHTs,                BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 5 */ {OPERATIONAL_STATEs,     BCMI_LT_FIELD_F_GET |
                                          BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_cosq_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);

    if (drv->cosq_sched_node_type_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (drv->cosq_sched_node_type_get(unit, node_type, &sym_val));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    fields[0].u.val = lport;
    fields[1].u.sym_val = sym_val;
    fields[2].u.val = node_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_SCHEDULER_NODEs,
                           &lt_entry, NULL, NULL));


    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "lt %s lookup %s=%d %s=%s %s=%d %s=%s %s=%d %s=%s\n"),
            TM_SCHEDULER_NODEs,
            fields[0].fld_name, (int)fields[0].u.val,
            fields[1].fld_name, fields[1].u.sym_val,
            fields[2].fld_name, (int)fields[2].u.val,
            fields[3].fld_name, fields[3].u.sym_val,
            fields[4].fld_name, (int)fields[4].u.val,
            fields[5].fld_name, fields[5].u.sym_val));

    if (!check_state) {
        if (sched_mode != NULL) {
            if (drv->cosq_sched_mode_get) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (drv->cosq_sched_mode_get(unit, sched_mode,
                                              &(fields[3].u.sym_val), false));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
            }
        }
        if (weight != NULL) {
            *weight = fields[4].u.val;
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_schedule_node_state_check(unit, fields[5].u.sym_val,
                                               lport, node_id,
                                               fields[1].u.sym_val));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief node schedule profile set.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lport         Logic port number.
 * \param [in]  node_id       Node id.
 * \param [in]  node_type     node type.
 * \param [in]  sched_mode    sched mode.
 * \param [in]  weight        weight.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_schedule_node_set(int unit, bcm_port_t lport, int node_id,
                          int node_type, int sched_mode, int weight)
{
    bcmi_lt_entry_t lt_entry;
    tm_ltsw_cosq_chip_driver_t *drv = NULL;
    const char *sym_val = NULL;
    const char *sym_val1 = NULL;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,               BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {SCHED_NODEs,            BCMI_LT_FIELD_F_SET |
                                          BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
         /* 2 */ {TM_SCHEDULER_NODE_IDs,  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 3 */ {SCHED_MODEs,            BCMI_LT_FIELD_F_SET |
                                          BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
         /* 4 */ {WEIGHTs,                BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_cosq_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);

    if (drv->cosq_sched_node_type_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (drv->cosq_sched_node_type_get(unit, node_type, &sym_val));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    fields[0].u.val = lport;
    fields[1].u.sym_val = sym_val;
    fields[2].u.val = node_id;
    if (drv->cosq_sched_mode_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (drv->cosq_sched_mode_get(unit, &sched_mode, &sym_val1, true));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }
    fields[3].u.sym_val = sym_val1;
    if (weight == -1) {
        /* ignore WEIGHTs set */
        fields[4].flags &= ~BCMI_LT_FIELD_F_SET;
    } else {
        fields[4].u.val = weight;
    }

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "lt %s update %s=%d %s=%s %s=%d %s=%s %s=%d\n"),
            TM_SCHEDULER_NODEs,
            fields[0].fld_name, (int)fields[0].u.val,
            fields[1].fld_name, fields[1].u.sym_val,
            fields[2].fld_name, (int)fields[2].u.val,
            fields[3].fld_name, fields[3].u.sym_val,
            fields[4].fld_name, (int)fields[4].u.val));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_SCHEDULER_NODEs, &lt_entry, NULL));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_schedule_node_get(unit, lport, node_id, node_type,
                                   NULL, NULL, true));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief cosq wred service pool set.
 *
 * \param [in]  unit                        Unit number.
 * \param [in]  pool_id                     pool_id.
 * \param [in]  current_q_size              current_q_size.
 * \param [in]  weight                      weight.
 * \param [in]  wred                        wred.
 * \param [in]  time_profile_idx            time_profile_idx.
 * \param [in]  drop_curve_profile_idx      drop_curve_profile_idx.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_wred_service_pool_set(int unit, int pool_id, bool current_q_size,
                              int weight, bool wred, int time_profile_idx,
                              int drop_curve_profile_idx)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {TM_WRED_SERVICE_POOL_IDs,       BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {CURRENT_Q_SIZEs,                BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {WEIGHTs,                        BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 3 */ {WREDs,                          BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 4 */ {TM_WRED_TIME_PROFILE_IDs,       BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 5 */ {TM_WRED_DROP_CURVE_SET_PROFILE_IDs, 0, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = pool_id;
    fields[1].u.val = current_q_size;
    fields[2].u.val = weight;
    fields[3].u.val = wred;
    fields[4].u.val = time_profile_idx;
    fields[5].u.val = drop_curve_profile_idx;
    if (drop_curve_profile_idx != -1) {
        fields[5].flags = BCMI_LT_FIELD_F_SET;
        fields[5].u.val = drop_curve_profile_idx;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_WRED_SERVICE_POOLs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief cosq wred service pool get.
 *
 * \param [in]  unit                        Unit number.
 * \param [in]  pool_id                     pool_id.
 * \param [out] current_q_size              current_q_size.
 * \param [out] weight                      weight.
 * \param [out] wred                        wred.
 * \param [out] time_profile_idx            time_profile_idx.
 * \param [out] drop_curve_profile_idx      drop_curve_profile_idx.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_wred_service_pool_get(int unit, int pool_id, bool *current_q_size,
                              int *weight, bool *wred, int *time_profile_idx,
                              int *drop_curve_profile_idx)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {TM_WRED_SERVICE_POOL_IDs,       BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {CURRENT_Q_SIZEs,                BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 2 */ {WEIGHTs,                        BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 3 */ {WREDs,                          BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 4 */ {TM_WRED_TIME_PROFILE_IDs,       BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 5 */ {TM_WRED_DROP_CURVE_SET_PROFILE_IDs, BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = pool_id;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_lt_entry_get(unit, TM_WRED_SERVICE_POOLs,
                           &lt_entry, NULL, NULL), SHR_E_NOT_FOUND);

    *current_q_size         = fields[1].u.val;
    *weight                 = fields[2].u.val;
    *wred                   = fields[3].u.val;
    *time_profile_idx       = fields[4].u.val;
    *drop_curve_profile_idx = fields[5].u.val;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief cosq wred port service pool set.
 *
 * \param [in]  unit                        Unit number.
 * \param [in]  lport                       Logic port number.
 * \param [in]  pool_id                     pool_id.
 * \param [in]  current_q_size              current_q_size.
 * \param [in]  weight                      weight.
 * \param [in]  wred                        wred.
 * \param [in]  time_profile_idx            time_profile_idx.
 * \param [in]  drop_curve_profile_idx      drop_curve_profile_idx.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_wred_port_service_pool_set(int unit, bcm_port_t lport, int pool_id,
                                   bool current_q_size, int weight,
                                   bool wred, int time_profile_idx,
                                   int drop_curve_profile_idx)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,                       BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {TM_WRED_PORT_SERVICE_POOL_IDs,  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {CURRENT_Q_SIZEs,                BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 3 */ {WEIGHTs,                        BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 4 */ {WREDs,                          BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 5 */ {TM_WRED_TIME_PROFILE_IDs,       BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 6 */ {TM_WRED_DROP_CURVE_SET_PROFILE_IDs, 0, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = lport;
    fields[1].u.val = pool_id;
    fields[2].u.val = current_q_size;
    fields[3].u.val = weight;
    fields[4].u.val = wred;
    fields[5].u.val = time_profile_idx;
    if (drop_curve_profile_idx != -1) {
        fields[6].flags = BCMI_LT_FIELD_F_SET;
        fields[6].u.val = drop_curve_profile_idx;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_WRED_PORT_SERVICE_POOLs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief cosq wred port service pool get.
 *
 * \param [in]  unit                        Unit number.
 * \param [in]  lport                       Logic port number.
 * \param [in]  pool_id                     pool_id.
 * \param [out] current_q_size              current_q_size.
 * \param [out] weight                      weight.
 * \param [out] wred                        wred.
 * \param [out] time_profile_idx            time_profile_idx.
 * \param [out] drop_curve_profile_idx      drop_curve_profile_idx.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_wred_port_service_pool_get(int unit, bcm_port_t lport, int pool_id,
                                   bool *current_q_size, int *weight,
                                   bool *wred, int *time_profile_idx,
                                   int *drop_curve_profile_idx)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,                       BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {TM_WRED_PORT_SERVICE_POOL_IDs,  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {CURRENT_Q_SIZEs,                BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 3 */ {WEIGHTs,                        BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 4 */ {WREDs,                          BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 5 */ {TM_WRED_TIME_PROFILE_IDs,       BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 6 */ {TM_WRED_DROP_CURVE_SET_PROFILE_IDs, BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = lport;
    fields[1].u.val = pool_id;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_lt_entry_get(unit, TM_WRED_PORT_SERVICE_POOLs,
                           &lt_entry, NULL, NULL), SHR_E_NOT_FOUND);

    *current_q_size         = fields[2].u.val;
    *weight                 = fields[3].u.val;
    *wred                   = fields[4].u.val;
    *time_profile_idx       = fields[5].u.val;
    *drop_curve_profile_idx = fields[6].u.val;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief cosq wred uc queue set.
 *
 * \param [in]  unit                        Unit number.
 * \param [in]  lport                       Logic port number.
 * \param [in]  queue_id                    queue_id.
 * \param [in]  weight                      weight.
 * \param [in]  current_q_size              current_q_size.
 * \param [in]  wred                        wred.
 * \param [in]  ecn                         ecn.
 * \param [in]  time_profile_idx            time_profile_idx.
 * \param [in]  mark_ecn_profile_idx        mark_ecn_profile_idx.
 * \param [in]  drop_ecn_profile_idx        drop_ecn_profile_idx.
 * \param [in]  drop_curve_profile_idx      drop_curve_profile_idx.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_wred_uc_queue_set(int unit, bcm_port_t lport, int queue_id,
                          int weight, bool current_q_size, bool wred,
                          bool ecn, int time_profile_idx,
                          int mark_ecn_profile_idx,
                          int drop_ecn_profile_idx,
                          int drop_curve_profile_idx)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,                         BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {TM_UC_Q_IDs,                      BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {WEIGHTs,                          BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 3 */ {CURRENT_Q_SIZEs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 4 */ {WREDs,                            BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 5 */ {ECNs,                             BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 6 */ {TM_WRED_TIME_PROFILE_IDs,         BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 7 */ {MARK_TM_WRED_CNG_NOTIFICATION_PROFILE_IDs,
                                                    BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 8 */ {DROP_TM_WRED_CNG_NOTIFICATION_PROFILE_IDs,
                                                    BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 9 */ {TM_WRED_DROP_CURVE_SET_PROFILE_IDs,
                                                    0, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = lport;
    fields[1].u.val = queue_id;
    fields[2].u.val = weight;
    fields[3].u.val = current_q_size;
    fields[4].u.val = wred;
    fields[5].u.val = ecn;
    fields[6].u.val = time_profile_idx;
    fields[7].u.val = mark_ecn_profile_idx;
    fields[8].u.val = drop_ecn_profile_idx;
    if (drop_curve_profile_idx != -1) {
        fields[9].flags = BCMI_LT_FIELD_F_SET;
        fields[9].u.val = drop_curve_profile_idx;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_WRED_UC_Qs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief cosq wred uc queue get.
 *
 * \param [in]  unit                        Unit number.
 * \param [in]  lport                       Logic port number.
 * \param [in]  queue_id                    queue_id.
 * \param [out] weight                      weight.
 * \param [out] current_q_size              current_q_size.
 * \param [out] wred                        wred.
 * \param [out] ecn                         ecn.
 * \param [out] time_profile_idx            time_profile_idx.
 * \param [out] mark_ecn_profile_idx        mark_ecn_profile_idx.
 * \param [out] drop_ecn_profile_idx        drop_ecn_profile_idx.
 * \param [out] drop_curve_profile_idx      drop_curve_profile_idx.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_wred_uc_queue_get(int unit, bcm_port_t lport, int queue_id,
                          int *weight, bool *current_q_size, bool *wred,
                          bool *ecn, int *time_profile_idx,
                          int *mark_ecn_profile_idx,
                          int *drop_ecn_profile_idx,
                          int *drop_curve_profile_idx)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,                        BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {TM_UC_Q_IDs,                     BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {WEIGHTs,                         BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 3 */ {CURRENT_Q_SIZEs,                 BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 4 */ {WREDs,                           BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 5 */ {ECNs,                            BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 6 */ {TM_WRED_TIME_PROFILE_IDs,        BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 7 */ {MARK_TM_WRED_CNG_NOTIFICATION_PROFILE_IDs,
                                                   BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 8 */ {DROP_TM_WRED_CNG_NOTIFICATION_PROFILE_IDs,
                                                   BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 9 */ {TM_WRED_DROP_CURVE_SET_PROFILE_IDs,
                                                   BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = lport;
    fields[1].u.val = queue_id;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_lt_entry_get(unit, TM_WRED_UC_Qs, &lt_entry, NULL, NULL),
         SHR_E_NOT_FOUND);

    *weight                  = fields[2].u.val;
    *current_q_size          = fields[3].u.val;
    *wred                    = fields[4].u.val;
    *ecn                     = fields[5].u.val;
    *time_profile_idx        = fields[6].u.val;
    *mark_ecn_profile_idx    = fields[7].u.val;
    *drop_ecn_profile_idx    = fields[8].u.val;
    *drop_curve_profile_idx  = fields[9].u.val;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief cosq wred cng profile set.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  profile_idx   profile_idx.
 * \param [in]  profile       profile.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_wred_cng_profile_set(int unit, int profile_idx,
                             tm_ltsw_cosq_wred_cng_profile_t profile)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {TM_WRED_CNG_NOTIFICATION_PROFILE_IDs,
                                               BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {Q_AVGs,                      BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {Q_MINs,                      BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 3 */ {SERVICE_POOLs,               BCMI_LT_FIELD_F_SET |
                                               BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
         /* 4 */ {ACTIONs,                     BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = profile_idx;
    fields[1].u.val = profile.q_avg;
    fields[2].u.val = profile.q_min;
    fields[3].u.sym_val = profile.sp == 0 ? LOW_CONGESTIONs :
                         (profile.sp == 1 ? MEDIUM_CONGESTIONs :
                                            HIGH_CONGESTIONs);
    fields[4].u.val = profile.action;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_update(unit, TM_WRED_CNG_NOTIFICATION_PROFILEs,
                              &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief cosq wred cng profile get.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  profile_idx   profile_idx.
 * \param [out] profile       profile.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_wred_cng_profile_get(int unit, int profile_idx,
                             tm_ltsw_cosq_wred_cng_profile_t *profile)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {TM_WRED_CNG_NOTIFICATION_PROFILE_IDs,
                                              BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {Q_AVGs,                     BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {Q_MINs,                     BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 3 */ {SERVICE_POOLs,              BCMI_LT_FIELD_F_SET |
                                              BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
         /* 4 */ {ACTIONs,                    BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = profile_idx;
    fields[1].u.val = profile->q_avg;
    fields[2].u.val = profile->q_min;
    fields[3].u.sym_val = profile->sp == 0 ? LOW_CONGESTIONs :
                         (profile->sp == 1 ? MEDIUM_CONGESTIONs :
                                             HIGH_CONGESTIONs);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_WRED_CNG_NOTIFICATION_PROFILEs,
                              &lt_entry, NULL, NULL));
    profile->action = fields[4].u.val;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get shaper configure from LT TM_SHAPER_CONFIG.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   itu mode        itu mode enable.
 * \param [in]   shaper          shaper enable.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_shaper_config_get(int unit, int *itu_mode, int *shaper)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {ITU_MODEs,                BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 1 */ {SHAPERs,                  BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_SHAPER_CONFIGs, &lt_entry, NULL, NULL));

    if (itu_mode != NULL) {
        *itu_mode = fields[0].u.val;
    }

    if (shaper != NULL) {
        *shaper = fields[1].u.val;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Set shaper configure to LT TM_SCHEDULER_SHAPER_CPU_NODE.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   node_id         node_id.
 * \param [in]   node_type       node_type.
 * \param [in]   shaper_mode     shaper_mode.
 * \param [in]   kbits_sec_min   kbits_sec_min.
 * \param [in]   kbits_sec_max   kbits_sec_max.
 * \param [in]   burst_size_auto Enables automatic calculation of burst size.
 * \param [in]   kbits_burst_min Configured size of maximum burst traffic.
 * \param [in]   kbits_burst_max Configured size of maximum burst traffic.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_cpu_shaper_node_set(int unit, int node_type, int node_id,
                            int shaper_mode, uint32_t kbits_sec_min,
                            uint32_t kbits_sec_max, bool burst_size_auto,
                            uint32_t burst_min, uint32_t burst_max)
{
    bcmi_lt_entry_t lt_entry;
    tm_ltsw_cosq_chip_driver_t *drv = NULL;
    const char *sym_val = NULL;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {SCHED_NODEs,              BCMI_LT_FIELD_F_SET |
                                            BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
         /* 1 */ {TM_SCHEDULER_SHAPER_CPU_NODE_IDs,  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {SHAPING_MODEs,            BCMI_LT_FIELD_F_SET |
                                            BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
         /* 3 */ {BURST_SIZE_AUTOs,         BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 4 */ {MIN_BANDWIDTH_KBPSs,      BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 5 */ {MAX_BANDWIDTH_KBPSs,      BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 6 */ {MIN_BURST_SIZE_KBITSs,    BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 7 */ {MAX_BURST_SIZE_KBITSs,    BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_cosq_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);

    if (drv->cosq_sched_node_type_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (drv->cosq_sched_node_type_get(unit, node_type, &sym_val));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    fields[0].u.sym_val = sym_val;
    fields[1].u.val = node_id;
    fields[2].u.sym_val = (shaper_mode == 0) ? BYTE_MODEs : PACKET_MODEs;
    fields[3].u.val = burst_size_auto;
    fields[4].u.val = kbits_sec_min;
    fields[5].u.val = kbits_sec_max;
    fields[6].u.val = burst_min;
    fields[7].u.val = burst_max;

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "lt %s update %s=%s %s=%d %s=%s %s=%d %s=%d %s=%d %s=%d %s=%d\n"),
            TM_SCHEDULER_SHAPER_CPU_NODEs,
            fields[0].fld_name, fields[0].u.sym_val,
            fields[1].fld_name, (int)fields[1].u.val,
            fields[2].fld_name, fields[2].u.sym_val,
            fields[3].fld_name, (int)fields[3].u.val,
            fields[4].fld_name, (int)fields[4].u.val,
            fields[5].fld_name, (int)fields[5].u.val,
            fields[6].fld_name, (int)fields[6].u.val,
            fields[7].fld_name, (int)fields[7].u.val));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_SCHEDULER_SHAPER_CPU_NODEs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief get shaper configure to LT TM_SCHEDULER_SHAPER_CPU_NODE.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   node_id         node_id.
 * \param [out]  node_type       node_type.
 * \param [out]  shaper_mode     shaper_mode.
 * \param [out]  kbits_sec_min   kbits_sec_min.
 * \param [out]  kbits_sec_max   kbits_sec_max.
 * \param [in]   burst_size_auto Enables automatic calculation of burst size.
 * \param [out]  kbits_burst_min Configured size of maximum burst traffic.
 * \param [out]  kbits_burst_max Configured size of maximum burst traffic.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_cpu_shaper_node_get(int unit, int node_type, int node_id,
                            int *shaper_mode, uint32_t *kbits_sec_min,
                            uint32_t *kbits_sec_max, bool burst_size_auto,
                            uint32_t *burst_min, uint32_t *burst_max)
{
    bcmi_lt_entry_t lt_entry;
    tm_ltsw_cosq_chip_driver_t *drv = NULL;
    const char *sym_val = NULL;
    bcmi_lt_field_t fields[] =
    {
        /* 0 */ {SCHED_NODEs,              BCMI_LT_FIELD_F_SET |
                                           BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /* 1 */ {TM_SCHEDULER_SHAPER_CPU_NODE_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 2 */ {SHAPING_MODEs,            BCMI_LT_FIELD_F_GET |
                                           BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /* 3 */ {BURST_SIZE_AUTOs,         BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 4 */ {MIN_BANDWIDTH_KBPS_OPERs,      BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 5 */ {MAX_BANDWIDTH_KBPS_OPERs,      BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 6 */ {MIN_BURST_SIZE_KBITS_OPERs,    BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 7 */ {MAX_BURST_SIZE_KBITS_OPERs,    BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_cosq_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);

    if (drv->cosq_sched_node_type_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (drv->cosq_sched_node_type_get(unit, node_type, &sym_val));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    fields[0].u.sym_val = sym_val;
    fields[1].u.val = node_id;
    fields[3].u.val = burst_size_auto;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_SCHEDULER_SHAPER_CPU_NODEs,
                           &lt_entry, NULL, NULL));

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "lt %s lookup %s=%s %s=%d %s=%s %s=%d %s=%d %s=%d %s=%d %s=%d\n"),
            TM_SCHEDULER_SHAPER_CPU_NODEs,
            fields[0].fld_name, fields[0].u.sym_val,
            fields[1].fld_name, (int)fields[1].u.val,
            fields[2].fld_name, fields[2].u.sym_val,
            fields[3].fld_name, (int)fields[3].u.val,
            fields[4].fld_name, (int)fields[4].u.val,
            fields[5].fld_name, (int)fields[5].u.val,
            fields[6].fld_name, (int)fields[6].u.val,
            fields[7].fld_name, (int)fields[7].u.val));

    if (sal_strcasecmp(fields[2].u.sym_val, BYTE_MODEs) == 0) {
        *shaper_mode = 0;
    } else {
        *shaper_mode = 1;
    }

    *kbits_sec_min = fields[4].u.val;
    *kbits_sec_max = fields[5].u.val;
    *burst_min = fields[6].u.val;
    *burst_max = fields[7].u.val;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief shaper node state check.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_state op_state.
 * \param [in] lport logic port.
 * \param [in] node_id node id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_tm_shaper_node_state_check(int unit, const char *op_state,
                                bcm_port_t lport, int node_id)
{
    SHR_FUNC_ENTER(unit);

    if (sal_strcasecmp(op_state, VALIDs) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NONE);
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "port %d l0 : %d update error: %s\n"),
                lport, node_id, op_state));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get shaper configure from LT TM_SHAPER_NODE.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   lport           Port number.
 * \param [in]   node_id         node_id.
 * \param [out]  node_type       node_type.
 * \param [out]  shaper_mode     shaper_mode.
 * \param [out]  kbits_sec_min   kbits_sec_min.
 * \param [out]  kbits_sec_max   kbits_sec_max.
 * \param [in]   burst_size_auto Enables automatic calculation of burst size.
 * \param [out]  kbits_burst_min Configured size of maximum burst traffic.
 * \param [out]  kbits_burst_max Configured size of maximum burst traffic.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_shaper_node_get(int unit, bcm_port_t lport, int node_id,
                        int node_type, int *shaper_mode,
                        uint32_t *kbits_sec_min, uint32_t *kbits_sec_max,
                        bool burst_size_auto, uint32_t *burst_min,
                        uint32_t *burst_max, bool check_state)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,               BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {TM_SCHEDULER_NODE_IDs,  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {SHAPING_MODEs,          BCMI_LT_FIELD_F_GET |
                                          BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
         /* 3 */ {BURST_SIZE_AUTOs,       BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 4 */ {MIN_BANDWIDTH_KBPS_OPERs,    BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 5 */ {MAX_BANDWIDTH_KBPS_OPERs,    BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 6 */ {MIN_BURST_SIZE_KBITS_OPERs,  BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 7 */ {MAX_BURST_SIZE_KBITS_OPERs,  BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 8 */ {OPERATIONAL_STATEs,     BCMI_LT_FIELD_F_GET |
                                          BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    if (node_type != 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = lport;
    fields[1].u.val = node_id;
    fields[3].u.val = burst_size_auto;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_SHAPER_NODEs,
                           &lt_entry, NULL, NULL));


    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "lt %s lookup %s=%d %s=%d %s=%s %s=%d %s=%d %s=%d %s=%d %s=%d %s=%s\n"),
            TM_SHAPER_NODEs,
            fields[0].fld_name, (int)fields[0].u.val,
            fields[1].fld_name, (int)fields[1].u.val,
            fields[2].fld_name, fields[2].u.sym_val,
            fields[3].fld_name, (int)fields[3].u.val,
            fields[4].fld_name, (int)fields[4].u.val,
            fields[5].fld_name, (int)fields[5].u.val,
            fields[6].fld_name, (int)fields[6].u.val,
            fields[7].fld_name, (int)fields[7].u.val,
            fields[8].fld_name, fields[8].u.sym_val));

    if (!check_state) {
        if (shaper_mode != NULL) {
            if (sal_strcasecmp(fields[2].u.sym_val, BYTE_MODEs) == 0) {
                *shaper_mode = 0;
            } else {
                *shaper_mode = 1;
            }
        }

        if (kbits_sec_min != NULL) {
            *kbits_sec_min = fields[4].u.val;
        }

        if (kbits_sec_max != NULL) {
            *kbits_sec_max = fields[5].u.val;
        }

        if (burst_min != NULL) {
            *burst_min = fields[6].u.val;
        }

        if (burst_max != NULL) {
            *burst_max = fields[7].u.val;
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_shaper_node_state_check(unit, fields[8].u.sym_val,
                                             lport, node_id));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Set shaper configure from LT TM_SHAPER_NODE.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   lport           Port number.
 * \param [in]   node_id         node_id.
 * \param [in]   node_type       node_type.
 * \param [in]   shaper_mode     shaper_mode.
 * \param [in]   kbits_sec_min   kbits_sec_min.
 * \param [in]   kbits_sec_max   kbits_sec_max.
 * \param [in]   burst_size_auto Enables automatic calculation of burst size.
 * \param [in]   kbits_burst_min Configured size of maximum burst traffic.
 * \param [in]   kbits_burst_max Configured size of maximum burst traffic.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_shaper_node_set(int unit, bcm_port_t lport, int node_id,
                        int node_type, int shaper_mode,
                        uint32_t kbits_sec_min, uint32_t kbits_sec_max,
                        bool burst_size_auto, uint32_t burst_min,
                        uint32_t burst_max)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,               BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {TM_SCHEDULER_NODE_IDs,  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {SHAPING_MODEs,          BCMI_LT_FIELD_F_SET |
                                          BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
         /* 3 */ {BURST_SIZE_AUTOs,       BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 4 */ {MIN_BANDWIDTH_KBPSs,    BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 5 */ {MAX_BANDWIDTH_KBPSs,    BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 6 */ {MIN_BURST_SIZE_KBITSs,  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 7 */ {MAX_BURST_SIZE_KBITSs,  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 8 */ {OPERATIONAL_STATEs,     BCMI_LT_FIELD_F_GET |
                                          BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    if (node_type != 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = lport;
    fields[1].u.val = node_id;
    fields[2].u.sym_val = (shaper_mode == 0) ? BYTE_MODEs : PACKET_MODEs;
    fields[3].u.val = burst_size_auto;
    fields[4].u.val = kbits_sec_min;
    fields[5].u.val = kbits_sec_max;
    fields[6].u.val = burst_min;
    fields[7].u.val = burst_max;

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "lt %s update %s=%d %s=%d %s=%s %s=%d %s=%d %s=%d %s=%d %s=%d\n"),
            TM_SHAPER_NODEs,
            fields[0].fld_name, (int)fields[0].u.val,
            fields[1].fld_name, (int)fields[1].u.val,
            fields[2].fld_name, fields[2].u.sym_val,
            fields[3].fld_name, (int)fields[3].u.val,
            fields[4].fld_name, (int)fields[4].u.val,
            fields[5].fld_name, (int)fields[5].u.val,
            fields[6].fld_name, (int)fields[6].u.val,
            fields[7].fld_name, (int)fields[7].u.val));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_SHAPER_NODEs, &lt_entry, NULL));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_shaper_node_get(unit, lport, node_id, node_type, NULL, NULL,
                                 NULL, burst_size_auto, NULL, NULL, true));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Delete Schedule profile entry.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  profile_id    Profile id.
 * \param [in]  node_id       Cos.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_schedule_profile_entry_delete(int unit, int profile_id, int node_id)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /* 0 */ {TM_SCHEDULER_PROFILE_IDs,        BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 1 */ {TM_SCHEDULER_NODE_IDs,           BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = profile_id;
    fields[1].u.val = node_id;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_lt_entry_delete(unit, TM_SCHEDULER_PROFILEs, &lt_entry, NULL),
         SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Schedule profile set.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  profile_id    Profile id.
 * \param [in]  node_id       Cos.
 * \param [in]  num_uc        Number of uc.
 * \param [in]  num_mc        Number of mc.
 * \param [in]  fc_uc         Flow control is set for uc.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_schedule_profile_entry_set(int unit, int profile_id, int node_id,
                                   int num_uc, int num_mc, int fc_uc,
                                   int *op_status)
{
    bcmi_lt_entry_t lt_entry;
    int rv = SHR_E_NONE;
    int rv1 = SHR_E_NONE;
    bcmi_lt_field_t fields[] =
    {
        /* 0 */ {TM_SCHEDULER_PROFILE_IDs,  BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 1 */ {TM_SCHEDULER_NODE_IDs,     BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 2 */ {NUM_UC_Qs,                 BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 3 */ {NUM_MC_Qs,                 BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 4 */ {FLOW_CTRL_UCs,             BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 5 */ {OPERATIONAL_STATEs,        BCMI_LT_FIELD_F_GET |
                                            BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = profile_id;
    fields[1].u.val = node_id;
    fields[2].u.val = num_uc;
    fields[3].u.val = num_mc;
    fields[4].u.val = fc_uc;
    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "lt %s update %s=%d %s=%d %s=%d %s=%d %s=%d\n"),
            TM_SCHEDULER_PROFILEs,
            fields[0].fld_name, (int)fields[0].u.val,
            fields[1].fld_name, (int)fields[1].u.val,
            fields[2].fld_name, (int)fields[2].u.val,
            fields[3].fld_name, (int)fields[3].u.val,
            fields[4].fld_name, (int)fields[4].u.val));

    rv = bcmi_lt_entry_set(unit, TM_SCHEDULER_PROFILEs, &lt_entry, NULL);

    fields[2].flags = 0;
    fields[3].flags = 0;
    fields[4].flags = 0;

    rv1 = bcmi_lt_entry_get(unit, TM_SCHEDULER_PROFILEs,
                           &lt_entry, NULL, NULL);

    if (rv1 == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FAIL);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv1);
    }

    *op_status = 0;

    if (sal_strcasecmp(fields[5].u.sym_val, VALIDs) == 0) {
        if (rv == SHR_E_NONE) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FAIL);
        }
    } else {
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "lt TM_SCHEDULER_PROFILE lookup %s=%d %s=%d %s=%s\n"),
            fields[0].fld_name, (int)fields[0].u.val,
            fields[1].fld_name, (int)fields[1].u.val,
            fields[5].fld_name, fields[5].u.sym_val));
        *op_status = 1;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Schedule profile get.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  profile_id    Profile id.
 * \param [in]  node_id       Cos.
 * \param [out]  num_uc        Number of uc.
 * \param [out]  num_mc        Number of mc.
 * \param [out]  fc_uc         Flow control is set for uc.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_schedule_profile_entry_get(int unit, int profile_id, int node_id,
                                   int *num_uc, int *num_mc, int *fc_uc)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /* 0 */ {TM_SCHEDULER_PROFILE_IDs,  BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 1 */ {TM_SCHEDULER_NODE_IDs,     BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 2 */ {NUM_UC_Qs,                 BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 3 */ {NUM_MC_Qs,                 BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 4 */ {FLOW_CTRL_UCs,             BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 5 */ {OPERATIONAL_STATEs,        BCMI_LT_FIELD_F_GET |
                                            BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = profile_id;
    fields[1].u.val = node_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_SCHEDULER_PROFILEs,
                           &lt_entry, NULL, NULL));

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "lt TM_SCHEDULER_PROFILE lookup %s=%d %s=%d %s=%d %s=%d %s=%d %s=%s\n"),
        fields[0].fld_name, (int)fields[0].u.val,
        fields[1].fld_name, (int)fields[1].u.val,
        fields[2].fld_name, (int)fields[2].u.val,
        fields[3].fld_name, (int)fields[3].u.val,
        fields[4].fld_name, (int)fields[4].u.val,
        fields[5].fld_name, fields[5].u.sym_val));

    if (sal_strcasecmp(fields[5].u.sym_val, VALIDs) == 0) {
        *num_uc = fields[2].u.val;
        *num_mc = fields[3].u.val;
        *fc_uc  = fields[4].u.val;
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NONE);
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "lt TM_SCHEDULER_PROFILE lookup %s=%d %s=%d %s=%d %s=%d %s=%d %s=%s\n"),
            fields[0].fld_name, (int)fields[0].u.val,
            fields[1].fld_name, (int)fields[1].u.val,
            fields[2].fld_name, (int)fields[2].u.val,
            fields[3].fld_name, (int)fields[3].u.val,
            fields[4].fld_name, (int)fields[4].u.val,
            fields[5].fld_name, fields[5].u.sym_val));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Schedule profile queue info get.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  profile_id    Profile id.
 * \param [in]  node_id       Cos.
 * \param [out]  num_uc        Number of uc.
 * \param [out]  num_mc        Number of mc.
 * \param [out]  mcq_id        mc queue id.
 * \param [out]  ucq_id0       uc queue id.
 * \param [out]  ucq_id1       uc queue id.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_tm_schedule_profile_queue_info_get(int unit, int profile_id, int node_id,
                                        tm_ltsw_cosq_scheduler_profile_q_info_t *profile)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /* 0 */ {TM_SCHEDULER_PROFILE_IDs,  BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 1 */ {TM_SCHEDULER_NODE_IDs,     BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 2 */ {NUM_TM_UC_Qs,              BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 3 */ {NUM_TM_MC_Qs,              BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 4 */ {TM_MC_Q_IDs,               BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 5 */ {TM_UC_Q_IDs,               BCMI_LT_FIELD_F_ARRAY |
                                            BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 6 */ {TM_UC_Q_IDs,               BCMI_LT_FIELD_F_ARRAY |
                                            BCMI_LT_FIELD_F_GET, 1, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = profile_id;
    fields[1].u.val = node_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_SCHEDULER_PROFILE_Q_INFOs,
                           &lt_entry, NULL, NULL));

    profile->num_ucq = fields[2].u.val;
    profile->num_mcq = fields[3].u.val;
    profile->mcq_id = fields[4].u.val;
    profile->ucq_id[0] = fields[5].u.val;
    profile->ucq_id[1] = fields[6].u.val;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Add an entry into LT PORT_COS_Q_MAP.
 *
 * This function is used to add an entry into LT PORT_COS_Q_MAP.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  cos_map_entries      cos_map_entries.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  prio                 Internal priority.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_cos_map_profile_entry_add(int unit,
                               tm_ltsw_cosq_cos_map_profile_t *profile,
                               int profile_idx, int prio)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    bool cpu_cos_valid = FALSE;
    bool eflow_cos_valid = FALSE;
    int mc_q_mode;

    SHR_FUNC_ENTER(unit);

    /* check CPU_COS field. */
    SHR_IF_ERR_VERBOSE_EXIT
        (lt_field_validation(unit, PORT_COS_Q_MAPs,
                             CPU_COSs, &cpu_cos_valid));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_field_validation(unit, PORT_COS_Q_MAPs,
                             UC_ELEPHANT_COSs, &eflow_cos_valid));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_mcq_mode_get(unit, &mc_q_mode));

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Add an entry into LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_COS_Q_MAPs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_COS_Q_MAP_IDs,
                               profile_idx));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, INT_PRIs, prio));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, UC_COSs,
                               profile->uc_cos));

    if (eflow_cos_valid) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, UC_ELEPHANT_COSs,
                                   profile->uc_elephant_cos));
    }

    if (mc_q_mode != 0) {
        if (profile->mc_cos != -1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, MC_COSs,
                                       profile->mc_cos));
        }
    }

    if (profile->rqe_cos != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, RQE_COSs,
                                   profile->rqe_cos));
    }

    if (cpu_cos_valid) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, CPU_COSs,
                                   profile->cpu_cos));
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "lt %s update %s=%d %s=%d %s=%d %s=%d %s=%d %s=%d\n"),
                PORT_COS_Q_MAPs,
                PORT_COS_Q_MAP_IDs, profile_idx,
                INT_PRIs, prio,
                UC_COSs, profile->uc_cos,
                MC_COSs, profile->mc_cos,
                RQE_COSs, profile->rqe_cos,
                CPU_COSs, profile->cpu_cos));
    }

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "lt %s update %s=%d %s=%d %s=%d %s=%d %s=%d\n"),
            PORT_COS_Q_MAPs,
            PORT_COS_Q_MAP_IDs, profile_idx,
            INT_PRIs, prio,
            UC_COSs, profile->uc_cos,
            MC_COSs, profile->mc_cos,
            RQE_COSs, profile->rqe_cos));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Add an entry into LT PORT_COS_Q_MAP.
 *
 * This function is used to add an entry into LT PORT_COS_Q_MAP.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  cos_map_entries      cos_map_entries.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_cos_map_profile_add(int unit,
                         tm_ltsw_cosq_cos_map_profile_t *cos_map_entries,
                         int *profile_idx)
{
    int i;
    int rv = SHR_E_NONE;
    int entries_per_set = COSQ_NUM_INT_PRI;

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_allocate(unit, BCMI_LTSW_PROFILE_COS_MAP,
                                          cos_map_entries, 0,
                                          entries_per_set, profile_idx);

    /* Get an existing entry and return the profile index directly */
    if (rv == SHR_E_EXISTS) {
        SHR_EXIT();
    }

    if (rv == SHR_E_FULL) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    for (i  = 0; i < COSQ_NUM_INT_PRI; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_cos_map_profile_entry_add(unit, &cos_map_entries[i],
                                            *profile_idx / COSQ_NUM_INT_PRI, i));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get a entry from LT PORT_COS_Q_MAP.
 *
 * This function is used to get a entry from LT PORT_COS_Q_MAP.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  prio                 Internal priority.
 * \param [out] profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_cos_map_profile_entry_get(int unit, int profile_idx, int prio,
                               tm_ltsw_cosq_cos_map_profile_t *profile)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t value;
    int dunit = 0;
    bool cpu_cos_valid = FALSE;
    bool eflow_cos_valid = FALSE;

    SHR_FUNC_ENTER(unit);

    /* check CPU_COS field. */
    SHR_IF_ERR_VERBOSE_EXIT
        (lt_field_validation(unit, PORT_COS_Q_MAPs,
                             CPU_COSs, &cpu_cos_valid));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_field_validation(unit, PORT_COS_Q_MAPs,
                             UC_ELEPHANT_COSs, &eflow_cos_valid));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_COS_Q_MAPs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_COS_Q_MAP_IDs,
                               profile_idx));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, INT_PRIs, prio));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, UC_COSs,
                               &value));

    profile->uc_cos = value;

    if (eflow_cos_valid) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, UC_ELEPHANT_COSs,
                                   &value));

        profile->uc_elephant_cos = value;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, MC_COSs,
                               &value));

    profile->mc_cos = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, RQE_COSs,
                               &value));
    profile->rqe_cos = value;

    if (cpu_cos_valid) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, CPU_COSs,
                                   &value));

        profile->cpu_cos = value;
    }

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Get a entries from LT PORT_COS_Q_MAP.
 *
 * This function is used to get a entry from LT PORT_COS_Q_MAP.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] profile_idx           Profile index.
 * \param [out] cos_map_entries      Profile entries.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_cos_map_profile_get(int unit,
                         int profile_idx,
                         tm_ltsw_cosq_cos_map_profile_t *cos_map_entries)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i  = 0; i < COSQ_NUM_INT_PRI; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_cos_map_profile_entry_get(unit, profile_idx / COSQ_NUM_INT_PRI,
                                            i, &cos_map_entries[i]));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Delete a entry from LT PORT_COS_Q_MAP.
 *
 * This function is used to delete a entry from LT PORT_COS_Q_MAP.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  prio                 Internal priority.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_cos_map_profile_entry_delete(int unit, int profile_idx, int prio)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_COS_Q_MAPs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_COS_Q_MAP_IDs,
                               profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, INT_PRIs, prio));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Delete a entries from LT PORT_COS_Q_MAP.
 *
 * This function is used to delete a entry from LT PORT_COS_Q_MAP.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_cos_map_profile_delete(int unit, int profile_idx)
{
    int rv = SHR_E_NONE;
    int i;

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_free(unit, BCMI_LTSW_PROFILE_COS_MAP,
                                      profile_idx);

    if (rv == SHR_E_BUSY) {
        /* Profile index is still used by others and return directly */
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    for (i  = 0; i < COSQ_NUM_INT_PRI; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_cos_map_profile_entry_delete(unit,
                                               profile_idx / COSQ_NUM_INT_PRI,
                                               i));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Calculate hash signature for LT PORT_COS_Q_MAP.
 *
 * This function is used to calculate hash signature for LT PORT_COS_Q_MAP.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              PORT_COS_Q_MAP profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [out] hash                 Hash signature.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_cos_map_profile_hash_cb(int unit, void *entries,
                     int entries_per_set, uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (entries_per_set != COSQ_NUM_INT_PRI) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *hash = shr_crc32(0, entries,
                      entries_per_set * sizeof(tm_ltsw_cosq_cos_map_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Compare profile set of LT PORT_COS_Q_MAP.
 *
 * This function is used to compare profile set of LT PORT_COS_Q_MAP.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              PORT_COS_Q_MAP profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [in]  index                Entry index of profile table.
 * \param [out] cmp                  Result of comparison.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_cos_map_profile_cmp_cb(int unit, void *entries,
                    int entries_per_set, int index, int *cmp)
{
    tm_ltsw_cosq_cos_map_profile_t cos_map_entries[COSQ_NUM_INT_PRI] = {{0}};

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != COSQ_NUM_INT_PRI) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_cos_map_profile_get(unit, index, &cos_map_entries[0]));

    *cmp = sal_memcmp(entries, (void *)cos_map_entries,
                     (entries_per_set * sizeof(tm_ltsw_cosq_cos_map_profile_t)));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Add default profile.
 *
 * \param [in]   unit         Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_cos_map_default_profile_add(int unit, int numq)
{
    int count;
    int cos;
    int prio = 0;
    int num_ucq, num_mcq, num_rqe;
    tm_ltsw_cosq_cos_map_profile_t cos_map_entries[COSQ_NUM_INT_PRI] = {{0}};
    int profile_idx;
    bcm_port_t local_port;
    bcm_pbmp_t pbmp;
    bool eflow_cos_valid = FALSE;
    int bcm_init = bcmi_ltsw_init_state_get(unit);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_num_queue_get(unit, &num_ucq, &num_mcq));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_field_validation(unit, PORT_COS_Q_MAPs,
                             UC_ELEPHANT_COSs, &eflow_cos_valid));

    num_rqe = num_mcq;

    for (cos = 0; cos < numq; cos++) {
        cos_map_entries[prio].uc_cos = MIN(cos, num_ucq - 1);
        cos_map_entries[prio].uc_elephant_cos = eflow_cos_valid ?
                                                MIN(cos, num_ucq - 1) : 0;
        cos_map_entries[prio].mc_cos = MIN(cos, num_mcq ? num_mcq - 1 : 0);
        cos_map_entries[prio].rqe_cos = MIN(cos, num_rqe ? num_rqe - 1 : 0);
        prio++;
    }

    for (; prio < COSQ_NUM_INT_PRI; prio++) {
        if (prio < numq) {
            cos_map_entries[prio].uc_cos = MIN(prio, num_ucq - 1);
            cos_map_entries[prio].uc_elephant_cos = eflow_cos_valid ?
                                                    MIN(prio, num_ucq - 1) : 0;
            cos_map_entries[prio].mc_cos = MIN(prio, num_mcq ? num_mcq - 1 : 0);
            cos_map_entries[prio].rqe_cos = MIN(prio, num_rqe ? num_rqe - 1 : 0);
        } else {
            cos_map_entries[prio].uc_cos = MIN(numq - 1, num_ucq - 1);
            cos_map_entries[prio].uc_elephant_cos = eflow_cos_valid ?
                                                    MIN(numq - 1, num_ucq - 1) : 0;
            cos_map_entries[prio].mc_cos = MIN(numq - 1, num_mcq ? num_mcq - 1 :  0);
            cos_map_entries[prio].rqe_cos =  MIN(numq - 1, num_rqe ? num_rqe - 1 : 0);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_cos_map_profile_add(unit, cos_map_entries, &profile_idx));

    count = 0;

    SHR_IF_ERR_VERBOSE_EXIT
         (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &pbmp));

    BCM_PBMP_ITER(pbmp, local_port) {
        if (!bcm_init) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set(unit, local_port, BCMI_PT_PORT_COS_QUEUE_MAP_ID,
                                        profile_idx / COSQ_NUM_INT_PRI));
        }
        count++;
    }

    if (count > 1) {
        /* increment reference count if more than one ref_count */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_ref_count_increase(unit, BCMI_LTSW_PROFILE_COS_MAP,
                                 COSQ_NUM_INT_PRI, profile_idx, (count - 1)));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Register LT PORT_COS_Q_MAP.
 *
 * This function is used to register LT PORT_COS_Q_MAP.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_cos_map_profile_register(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_COS_MAP;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, PORT_COS_Q_MAPs,
                                       PORT_COS_Q_MAP_IDs,
                                       &idx_min, &idx_max));
    ent_idx_min = (int)idx_min;
    ent_idx_max = ((int)idx_max + 1) * COSQ_NUM_INT_PRI - 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    cosq_cos_map_profile_hash_cb,
                                    cosq_cos_map_profile_cmp_cb));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Recover hash and reference count of LT PORT_COS_Q_MAP.
 *
 * This function is used to Recover hash and reference count of
 * LT PORT_COS_Q_MAP profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] index                 Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_cos_map_profile_recover(int unit, int index)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_COS_MAP;
    tm_ltsw_cosq_cos_map_profile_t cos_map_entries[COSQ_NUM_INT_PRI] = {{0}};
    int entries_per_set = COSQ_NUM_INT_PRI;
    uint32_t ref_count = 0;
    int profile_id;

    SHR_FUNC_ENTER(unit);

    profile_id = index * entries_per_set;

    /* Update profile hash on the first usage.  */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl, profile_id, &ref_count));

    if (ref_count == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_cos_map_profile_get(unit, profile_id, &cos_map_entries[0]));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update(unit, profile_hdl, cos_map_entries,
                                           entries_per_set, profile_id));
    }

    /* Increment reference count. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, profile_hdl,
                                              entries_per_set, profile_id, 1));
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Recover LT PORT_COS_Q_MAP.
 *
 * This function is used to Recover hash and reference count of
 * LT PORT_COS_Q_MAP profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] index                 Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_cos_map_recover(int unit)
{
    bcm_port_t local_port;
    bcm_pbmp_t pbmp;
    int index;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &pbmp));

    BCM_PBMP_ITER(pbmp, local_port) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_get(unit, local_port, BCMI_PT_PORT_COS_QUEUE_MAP_ID,
                                    &index));

        /* Recover ref_cnt for profile entry. */
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_cos_map_profile_recover(unit, index));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Initialize the LT PORT_COS_Q_MAP.
 *
 * This function is used to initialize the LT PORT_COS_Q_MAP
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_cos_map_profile_init(int unit, int numq)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_COS_MAP;
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_profile_unregister(unit, profile_hdl), SHR_E_NOT_FOUND);

    /* Register profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_cos_map_profile_register(unit));

    if(!warm) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, PORT_COS_Q_MAPs));

        /* Add default profile. */
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_cos_map_default_profile_add(unit, numq));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_cos_map_recover(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Add an entry into LT FP_ING_COS_Q_MAPs.
 *
 * This function is used to add an entry into LT FP_ING_COS_Q_MAPs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  cos_map_entries      cos_map_entries.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  prio                 Internal priority.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ifp_cos_map_hi_profile_add(int unit, int profile_idx)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* Add an entry into LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, FP_ING_COS_Q_MAPs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FP_ING_COS_Q_MAP_IDs,
                               profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief update an entry into LT FP_ING_COS_Q_MAPs.
 *
 * This function is used to update an entry into LT FP_ING_COS_Q_MAPs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ifp_cos_map_hi_profile_set(int unit, int profile_idx,
                                tm_ltsw_cosq_cos_map_profile_t *profile)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    bool cpu_cos_valid = FALSE;

    SHR_FUNC_ENTER(unit);

    /* check CPU_COS field. */
    SHR_IF_ERR_VERBOSE_EXIT
        (lt_field_validation(unit, FP_ING_COS_Q_MAPs,
                             CPU_COSs, &cpu_cos_valid));

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* Add an entry into LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, FP_ING_COS_Q_MAPs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FP_ING_COS_Q_MAP_IDs,
                               profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, UC_COSs,
                               profile->uc_cos));

    if (profile->mc_cos != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, MC_COSs,
                                   profile->mc_cos));
    }

    if (profile->rqe_cos != -1) {
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_add(entry_hdl, RQE_COSs,
                                  profile->rqe_cos));
    }

    if (cpu_cos_valid) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, CPU_COSs,
                                   profile->cpu_cos));

        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "lt %s update %s=%d %s=%d %s=%d %s=%d %s=%d\n"),
                FP_ING_COS_Q_MAPs,
                FP_ING_COS_Q_MAP_IDs, profile_idx,
                UC_COSs, profile->uc_cos,
                MC_COSs, profile->mc_cos,
                RQE_COSs, profile->rqe_cos,
                CPU_COSs, profile->cpu_cos));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Get a entry from LT FP_ING_COS_Q_MAPs.
 *
 * This function is used to get a entry from LT FP_ING_COS_Q_MAPs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [out] profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ifp_cos_map_hi_profile_get(int unit, int profile_idx,
                                tm_ltsw_cosq_cos_map_profile_t *profile)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t value;
    int dunit = 0;
    bool cpu_cos_valid = FALSE;

    SHR_FUNC_ENTER(unit);

    /* check CPU_COS field. */
    SHR_IF_ERR_VERBOSE_EXIT
        (lt_field_validation(unit, FP_ING_COS_Q_MAPs,
                             CPU_COSs, &cpu_cos_valid));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, FP_ING_COS_Q_MAPs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FP_ING_COS_Q_MAP_IDs,
                               profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, UC_COSs,
                               &value));
    profile->uc_cos = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, MC_COSs,
                               &value));
    profile->mc_cos = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, RQE_COSs,
                               &value));

    profile->rqe_cos = value;

    if (cpu_cos_valid) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, CPU_COSs,
                                   &value));

        profile->cpu_cos = value;
    }

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Delete a entry from LT FP_ING_COS_Q_MAPs.
 *
 * This function is used to delete a entry from LT FP_ING_COS_Q_MAPs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  prio                 Internal priority.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ifp_cos_map_hi_profile_delete(int unit, int profile_idx)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, FP_ING_COS_Q_MAPs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FP_ING_COS_Q_MAP_IDs,
                               profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Add an entry into LT FP_ING_COS_Q_INT_PRI_MAPs.
 *
 * This function is used to add an entry into LT FP_ING_COS_Q_INT_PRI_MAPs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  cos_map_entries      cos_map_entries.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  prio                 Internal priority.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ifp_cos_map_profile_entry_add(int unit,
                                   tm_ltsw_cosq_cos_map_profile_t *profile,
                                   int profile_idx, int prio)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    bool cpu_cos_valid = FALSE;

    SHR_FUNC_ENTER(unit);

    /* check CPU_COS field. */
    SHR_IF_ERR_VERBOSE_EXIT
        (lt_field_validation(unit, FP_ING_COS_Q_INT_PRI_MAPs,
                             CPU_COSs, &cpu_cos_valid));

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* Add an entry into LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, FP_ING_COS_Q_INT_PRI_MAPs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FP_ING_COS_Q_INT_PRI_MAP_IDs,
                               profile_idx));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, INT_PRIs, prio));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, UC_COSs,
                               profile->uc_cos));

    if (profile->mc_cos != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, MC_COSs,
                                   profile->mc_cos));
    }

    if (profile->rqe_cos != -1) {
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_add(entry_hdl, RQE_COSs,
                                  profile->rqe_cos));
    }

    if (cpu_cos_valid) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, CPU_COSs,
                                   profile->cpu_cos));
    }

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "lt %s update %s=%d %s=%d %s=%d %s=%d %s=%d\n"),
            FP_ING_COS_Q_INT_PRI_MAPs,
            FP_ING_COS_Q_INT_PRI_MAP_IDs, profile_idx,
            INT_PRIs, prio,
            UC_COSs, profile->uc_cos,
            MC_COSs, profile->mc_cos,
            RQE_COSs, profile->rqe_cos));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Add an entry into LT FP_ING_COS_Q_INT_PRI_MAPs.
 *
 * This function is used to add an entry into LT FP_ING_COS_Q_INT_PRI_MAPs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  cos_map_entries      cos_map_entries.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ifp_cos_map_profile_add(int unit,
                             tm_ltsw_cosq_cos_map_profile_t *cos_map_entries,
                             int *profile_idx)
{
    int i;
    int rv = SHR_E_NONE;
    int entries_per_set = COSQ_NUM_INT_PRI;

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_allocate(unit, BCMI_LTSW_PROFILE_IFP_COS_MAP,
                                          cos_map_entries, 0,
                                          entries_per_set, profile_idx);

    /* Get an existing entry and return the profile index directly */
    if (rv == SHR_E_EXISTS) {
        SHR_EXIT();
    }

    if (rv == SHR_E_FULL) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    for (i  = 0; i < COSQ_NUM_INT_PRI; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ifp_cos_map_profile_entry_add(unit, &cos_map_entries[i],
                                                *profile_idx / COSQ_NUM_INT_PRI,
                                                i));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get a entry from LT FP_ING_COS_Q_INT_PRI_MAPs.
 *
 * This function is used to get a entry from LT FP_ING_COS_Q_INT_PRI_MAPs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  prio                 Internal priority.
 * \param [out] profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ifp_cos_map_profile_entry_get(int unit, int profile_idx, int prio,
                                   tm_ltsw_cosq_cos_map_profile_t *profile)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t value;
    int dunit = 0;
    bool cpu_cos_valid = FALSE;

    SHR_FUNC_ENTER(unit);

    /* check CPU_COS field. */
    SHR_IF_ERR_VERBOSE_EXIT
        (lt_field_validation(unit, FP_ING_COS_Q_INT_PRI_MAPs,
                             CPU_COSs, &cpu_cos_valid));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, FP_ING_COS_Q_INT_PRI_MAPs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FP_ING_COS_Q_INT_PRI_MAP_IDs,
                               profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, INT_PRIs, prio));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, UC_COSs,
                               &value));
    profile->uc_cos = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, MC_COSs,
                               &value));
    profile->mc_cos = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, RQE_COSs,
                               &value));

    profile->rqe_cos = value;

    if (cpu_cos_valid) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, CPU_COSs,
                                   &value));

        profile->cpu_cos = value;
    }

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Get a entries from LT FP_ING_COS_Q_INT_PRI_MAPs.
 *
 * This function is used to get a entry from LT FP_ING_COS_Q_INT_PRI_MAPs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile index.
 * \param [out] cos_map_entries      Profile entries.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ifp_cos_map_profile_get(int unit,
                             int profile_idx,
                             tm_ltsw_cosq_cos_map_profile_t *cos_map_entries)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i  = 0; i < COSQ_NUM_INT_PRI; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ifp_cos_map_profile_entry_get(unit,
                                                profile_idx / COSQ_NUM_INT_PRI,
                                                i, &cos_map_entries[i]));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Delete a entry from LT FP_ING_COS_Q_INT_PRI_MAPs.
 *
 * This function is used to delete a entry from LT FP_ING_COS_Q_INT_PRI_MAPs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  prio                 Internal priority.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ifp_cos_map_profile_entry_delete(int unit, int profile_idx, int prio)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, FP_ING_COS_Q_INT_PRI_MAPs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FP_ING_COS_Q_INT_PRI_MAP_IDs,
                               profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, INT_PRIs, prio));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Delete a entries from LT FP_ING_COS_Q_INT_PRI_MAPs.
 *
 * This function is used to delete a entry from LT FP_ING_COS_Q_INT_PRI_MAPs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ifp_cos_map_profile_delete(int unit, int profile_idx)
{
    int rv = SHR_E_NONE;
    int i;

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_free(unit, BCMI_LTSW_PROFILE_IFP_COS_MAP,
                                      profile_idx);
    if (rv == SHR_E_BUSY) {
        /* Profile index is still used by others and return directly */
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    for (i  = 0; i < COSQ_NUM_INT_PRI; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ifp_cos_map_profile_entry_delete(unit,
                                           profile_idx / COSQ_NUM_INT_PRI, i));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Calculate hash signature for LT FP_ING_COS_Q_INT_PRI_MAPs.
 *
 * This function is used to calculate hash signature for LT FP_ING_COS_Q_INT_PRI_MAPs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              PORT_COS_Q_MAP profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [out] hash                 Hash signature.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ifp_cos_map_profile_hash_cb(int unit, void *entries,
                                 int entries_per_set, uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (entries_per_set != COSQ_NUM_INT_PRI) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *hash = shr_crc32(0, entries,
                      entries_per_set * sizeof(tm_ltsw_cosq_cos_map_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Compare profile set of LT FP_ING_COS_Q_INT_PRI_MAPs.
 *
 * This function is used to compare profile set of LT FP_ING_COS_Q_INT_PRI_MAPs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              PORT_COS_Q_MAP profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [in]  index                Entry index of profile table.
 * \param [out] cmp                  Result of comparison.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ifp_cos_map_profile_cmp_cb(int unit, void *entries,
                                int entries_per_set, int index, int *cmp)
{
    tm_ltsw_cosq_cos_map_profile_t cos_map_entries[COSQ_NUM_INT_PRI] = {{0}};

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != COSQ_NUM_INT_PRI) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ifp_cos_map_profile_get(unit, index, &cos_map_entries[0]));

    *cmp = sal_memcmp(entries, (void *)cos_map_entries,
                     (entries_per_set * sizeof(tm_ltsw_cosq_cos_map_profile_t)));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Add default profile.
 *
 * \param [in]   unit         Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_ifp_cos_map_default_profile_add(int unit)
{
    tm_ltsw_cosq_cos_map_profile_t cos_map_entries[COSQ_NUM_INT_PRI] = {{0}};
    int profile_idx;
    int num_ucq, num_mcq;
    int cos, prio = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_num_queue_get(unit, &num_ucq, &num_mcq));

    for (cos = 0; cos < COSQ_NUM_INT_PRI; cos++) {
        cos_map_entries[prio].uc_cos = 0;
        cos_map_entries[prio].uc_elephant_cos = 0;
        cos_map_entries[prio].mc_cos = num_mcq ? 0 : -1;
        cos_map_entries[prio].rqe_cos = num_mcq ? 0 : -1;
        prio++;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ifp_cos_map_profile_add(unit, cos_map_entries, &profile_idx));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Register LT FP_ING_COS_Q_MAP.
 *
 * This function is used to register LT FP_ING_COS_Q_MAP.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ifp_cos_map_profile_register(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_IFP_COS_MAP;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, FP_ING_COS_Q_INT_PRI_MAPs,
                                       FP_ING_COS_Q_INT_PRI_MAP_IDs,
                                       &idx_min, &idx_max));
    ent_idx_min = (int)idx_min;
    ent_idx_max = ((int)idx_max + 1) * COSQ_NUM_INT_PRI - 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    cosq_ifp_cos_map_profile_hash_cb,
                                    cosq_ifp_cos_map_profile_cmp_cb));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Recover hash and reference count of LT FP_ING_COS_Q_INT_PRI_MAPs.
 *
 * This function is used to Recover hash and reference count of
 * LT FP_ING_COS_Q_INT_PRI_MAPs profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] index                 Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ifp_cos_map_profile_recover(int unit, int index)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_IFP_COS_MAP;
    tm_ltsw_cosq_cos_map_profile_t cos_map_entries[COSQ_NUM_INT_PRI] = {{0}};
    int entries_per_set = COSQ_NUM_INT_PRI;
    uint32_t ref_count = 0;
    int profile_id;

    SHR_FUNC_ENTER(unit);

    profile_id = index * entries_per_set;

    /* Update profile hash on the first usage.  */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl, profile_id, &ref_count));

    if (ref_count == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ifp_cos_map_profile_get(unit, profile_id, &cos_map_entries[0]));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update(unit, profile_hdl, cos_map_entries,
                                           entries_per_set, profile_id));

        /* Increment reference count. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_ref_count_increase(unit, profile_hdl,
                                                  entries_per_set, profile_id, 1));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Recover LT FP_ING_COS_Q_INT_PRI_MAPs.
 *
 * This function is used to Recover hash and reference count of
 * LT FP_ING_COS_Q_INT_PRI_MAPs profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] index                 Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ifp_cos_map_recover(int unit)
{
    tm_ltsw_cosq_cos_map_profile_t cos_map_entries[COSQ_NUM_INT_PRI] = {{0}};
    int rv, index;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, FP_ING_COS_Q_INT_PRI_MAPs,
                                       FP_ING_COS_Q_INT_PRI_MAP_IDs,
                                       &idx_min, &idx_max));
    ent_idx_max = (int)idx_max + 1;

    for (index = 0; index < ent_idx_max; index++) {
        rv = cosq_ifp_cos_map_profile_get(unit, index * COSQ_NUM_INT_PRI,
                                          &cos_map_entries[0]);
        if (rv == SHR_E_NONE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_ifp_cos_map_profile_recover(unit, index));
        } else if (rv == SHR_E_NOT_FOUND) {
            rv = SHR_E_NONE;
            continue;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:

    SHR_FUNC_EXIT();
}

/*
 * \brief Initialize the LT FP_ING_COS_Q_INT_PRI_MAPs.
 *
 * This function is used to initialize the LT FP_ING_COS_Q_INT_PRI_MAPs
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ifp_cos_map_profile_init(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_IFP_COS_MAP;
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_profile_unregister(unit, profile_hdl), SHR_E_NOT_FOUND);

    /* Register profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ifp_cos_map_profile_register(unit));

    if (!warm) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, FP_ING_COS_Q_INT_PRI_MAPs));

        /* Add default profile. */
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ifp_cos_map_default_profile_add(unit));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ifp_cos_map_recover(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Create a cosq classifier.
 *
 * \param [in]  unit           Unit number.
 * \param [out] classifier_id  Classifier ID.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
cosq_field_classifier_id_create(int unit, int *classifier_id)
{
    uint32_t ref_count = 0;
    int ent_per_set = COSQ_NUM_INT_PRI;
    int i;
    uint32_t size;

    SHR_FUNC_ENTER(unit);

    if (NULL == classifier_id) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, FP_ING_COS_Q_INT_PRI_MAPs, &size));

    for (i = 0; i < (size - ent_per_set); i += ent_per_set) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_ref_count_get(unit, BCMI_LTSW_PROFILE_IFP_COS_MAP,
                                             i, &ref_count));
        if (0 == ref_count) {
            break;
        }
    }

    if (i >= size && ref_count != 0) {
        *classifier_id = 0;
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
    }

    BCMINT_COSQ_CLASSIFIER_FIELD_SET(*classifier_id, (i / ent_per_set));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Free resource associated with this field classifier id.
 *
 * \param [in]  unit           Unit number.
 * \param [in]  classifier_id  Classifier ID.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
cosq_field_classifier_id_destroy(int unit, int classifier_id)
{
    return SHR_E_NONE;
}

/*
 * \brief Delete an internal priority to ingress field processor CoS queue
 *        override mapping profile set.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   classifier_id       Classifier ID.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_field_classifier_map_clear(int unit, int classifier_id)
{
    uint32_t index;

    SHR_FUNC_ENTER(unit);

    /* Get profile table entry set base index. */
    index = BCMINT_COSQ_CLASSIFIER_FIELD_GET(classifier_id);

    if (index == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Delete the profile entries set. */
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ifp_cos_map_profile_delete(unit, index * COSQ_NUM_INT_PRI));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Add an entry into LT TM_WRED_TIME_PROFILE.
 *
 * This function is used to add an entry into LT TM_WRED_TIME_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  time_domain          time_domain.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_wred_time_profile_entry_add(int unit,
                                 int *time_domain,
                                 int *profile_idx)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    tm_ltsw_cosq_chip_driver_t *drv = NULL;
    int rv = SHR_E_NONE;
    int entries_per_set = 1;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_allocate(unit, BCMI_LTSW_PROFILE_WRED_TIME,
                                          time_domain, 0,
                                          entries_per_set, profile_idx);
    /* Get an existing entry and return the profile index directly */
    if (rv == SHR_E_EXISTS) {
        SHR_EXIT();
    }

    if (rv == SHR_E_FULL) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_cosq_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_wred_us_to_time_domain, SHR_E_INIT);

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* Add an entry into LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_WRED_TIME_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TM_WRED_TIME_PROFILE_IDs,
                               *profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, TIME_DOMAINs,
              drv->cosq_wred_us_to_time_domain(*time_domain)));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Get a entry from LT TM_WRED_TIME_PROFILE.
 *
 * This function is used to get a entry from LT TM_WRED_TIME_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx           Profile entry index.
 * \param [out] time_domain          time_domain.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_wred_time_profile_entry_get(int unit,
                                 int profile_idx,
                                 int *time_domain)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    tm_ltsw_cosq_chip_driver_t *drv = NULL;
    const char *sym_val;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_cosq_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_wred_time_domain_to_us, SHR_E_INIT);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_WRED_TIME_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TM_WRED_TIME_PROFILE_IDs,
                               profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, TIME_DOMAINs,
                               &sym_val));
    *time_domain = drv->cosq_wred_time_domain_to_us(sym_val);

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Delete a entry from LT TM_WRED_TIME_PROFILE.
 *
 * This function is used to delete a entry from LT TM_WRED_TIME_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_wred_time_profile_entry_delete(int unit, int profile_idx)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv = SHR_E_NONE;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_free(unit, BCMI_LTSW_PROFILE_WRED_TIME,
                                      profile_idx);
    if (rv == SHR_E_BUSY) {
        /* Profile index is still used by others and return directly */
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_WRED_TIME_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TM_WRED_TIME_PROFILE_IDs,
                               profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));
exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Calculate hash signature for LT TM_WRED_TIME_PROFILE.
 *
 * This function is used to calculate hash signature for LT TM_WRED_TIME_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              TM_WRED_TIME_PROFILE profile.
 * \param [in]  entries_per_set      Number of entries in the TIME profile set.
 * \param [out] hash                 Hash signature.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_wred_time_profile_hash_cb(int unit, void *entries,
                               int entries_per_set, uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *hash = shr_crc32(0, entries,
                      entries_per_set * sizeof(int));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Compare profile set of LT TM_WRED_TIME_PROFILE.
 *
 * This function is used to compare profile set of LT TM_WRED_TIME_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              TM_WRED_TIME_PROFILE profile.
 * \param [in]  entries_per_set      Number of entries in the TIME profile set.
 * \param [in]  index                Entry index of profile table.
 * \param [out] cmp                  Result of comparison.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_wred_time_profile_cmp_cb(int unit, void *entries,
                              int entries_per_set, int index, int *cmp)
{
    int time_domain = 0;

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_wred_time_profile_entry_get(unit, index, &time_domain));

    if (time_domain == *(int*)(entries)) {
        *cmp = 0;
    } else {
        *cmp = 1;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Add default profile.
 *
 * \param [in]   unit         Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_wred_time_default_profile_add(int unit)
{
    int time_profile_idx;
    int time_domain = 0;
    uint32_t ref_count = 0;
    uint32_t entry_num = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_wred_time_profile_entry_add(unit, &time_domain, &time_profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, TM_WRED_SERVICE_POOLs, &entry_num));
    ref_count += entry_num;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, TM_WRED_PORT_SERVICE_POOLs, &entry_num));
    ref_count += entry_num;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, TM_WRED_UC_Qs, &entry_num));
    ref_count += entry_num;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, BCMI_LTSW_PROFILE_WRED_TIME,
                                              1, time_profile_idx, ref_count));
exit:
    SHR_FUNC_EXIT();
}
/*
 * \brief Register LT PORT_COS_Q_MAP.
 *
 * This function is used to register LT PORT_COS_Q_MAP.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_wred_time_profile_register(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_WRED_TIME;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, TM_WRED_TIME_PROFILEs,
                                       TM_WRED_TIME_PROFILE_IDs,
                                       &idx_min, &idx_max));

    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    cosq_wred_time_profile_hash_cb,
                                    cosq_wred_time_profile_cmp_cb));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Recover hash and reference count of LT PORT_COS_Q_MAP.
 *
 * This function is used to Recover hash and reference count of
 * LT PORT_COS_Q_MAP profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] index                 Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_wred_time_profile_recover(int unit, int index)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_WRED_TIME;
    int time_domain = 0;
    int entries_per_set = 1;
    uint32_t ref_count = 0;
    int profile_id;
    uint32_t count = 0;
    uint32_t entry_num = 0;

    SHR_FUNC_ENTER(unit);

    profile_id = index * entries_per_set;

    /* Update profile hash on the first usage.  */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl, profile_id,
                                         &ref_count));

    if (ref_count == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_time_profile_entry_get(unit, profile_id, &time_domain));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update(unit, profile_hdl, &time_domain,
                                           entries_per_set, profile_id));

        if (profile_id == COSQ_DEFAULT_PROFILE_ID) {
            /* Increment default profile reference count to max. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_capacity_get(unit, TM_WRED_SERVICE_POOLs, &entry_num));
            count += entry_num;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_capacity_get(unit, TM_WRED_PORT_SERVICE_POOLs, &entry_num));
            count += entry_num;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_capacity_get(unit, TM_WRED_UC_Qs, &entry_num));
            count += entry_num;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_ref_count_increase
                    (unit, profile_hdl, entries_per_set, profile_id, count + 1));
        }
    }

    if (profile_id != COSQ_DEFAULT_PROFILE_ID) {
        /* Increment reference count. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_ref_count_increase(unit, profile_hdl,
                                                  entries_per_set, profile_id, 1));
        /* Decrease default profile reference count. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_ref_count_decrease(unit, profile_hdl,
                                                  entries_per_set,
                                                  COSQ_DEFAULT_PROFILE_ID, 1));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Recover LT PORT_COS_Q_MAP.
 *
 * This function is used to Recover hash and reference count of
 * LT PORT_COS_Q_MAP profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] index                 Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_wred_time_recover(int unit)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv, dunit, index;
    uint64_t data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_wred_time_profile_recover(unit, COSQ_DEFAULT_PROFILE_ID));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_WRED_SERVICE_POOLs, &entry_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, TM_WRED_TIME_PROFILE_IDs,
                                   &data));

        index = data;

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_time_profile_recover(unit, index));
    }

    if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (entry_hdl != BCMLT_INVALID_HDL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_free(entry_hdl));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_WRED_PORT_SERVICE_POOLs, &entry_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, TM_WRED_TIME_PROFILE_IDs,
                                   &data));

        index = data;

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_time_profile_recover(unit, index));
    }

    if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (entry_hdl != BCMLT_INVALID_HDL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_free(entry_hdl));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_WRED_UC_Qs, &entry_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, TM_WRED_TIME_PROFILE_IDs,
                                   &data));

        index = data;

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_time_profile_recover(unit, index));
    }

    if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Initialize the LT TM_WRED_TIME_PROFILE.
 *
 * This function is used to initialize the LT TM_WRED_TIME_PROFILE
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vd                   Default TM_WRED_TIME_PROFILE information.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_wred_time_profile_init(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_WRED_TIME;
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_profile_unregister(unit, profile_hdl), SHR_E_NOT_FOUND);

    /* Register profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_wred_time_profile_register(unit));

    if(!warm) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, TM_WRED_TIME_PROFILEs));

        /* Add default profile. */
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_time_default_profile_add(unit));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_time_recover(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Displays time domain profile reference count.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_time_domain_dump(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_WRED_TIME;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;
    int ii;
    uint32_t ref_count = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, TM_WRED_TIME_PROFILEs,
                                       TM_WRED_TIME_PROFILE_IDs,
                                       &idx_min, &idx_max));

    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;

    for (ii = ent_idx_min; ii <=  ent_idx_max; ii++) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl, ii,
                                             &ref_count));
        LOG_CLI((BSL_META_U(unit,
                            "\nTIME_DOMAIN[%d] - refcnt %d\n"), ii,
                            ref_count));
    }

    LOG_CLI((BSL_META_U(unit,
                        "=========== \n")));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Add an entry into LT TM_WRED_DROP_CURVE_SET_PROFILE.
 *
 * This function is used to add an entry into LT TM_WRED_DROP_CURVE_SET_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile              Profile entry.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_wred_drop_curve_profile_entry_add(int unit,
                                       cosq_wred_drop_curve_profile_t *profile,
                                       int *profile_idx)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    tm_ltsw_cosq_chip_driver_t *drv = NULL;
    int rv = SHR_E_NONE;
    int entries_per_set = 1;
    int dunit = 0;
    const char *rate;
    cosq_wred_drop_curve_field_t fields[] =
    {
        /*0 */ {RESPONSIVE_GREEN_DROP_MIN_THD_CELLSs,
                RESPONSIVE_GREEN_DROP_MAX_THD_CELLSs,
                RESPONSIVE_GREEN_DROP_PERCENTAGEs},
        /*1 */ {RESPONSIVE_YELLOW_DROP_MIN_THD_CELLSs,
                RESPONSIVE_YELLOW_DROP_MAX_THD_CELLSs,
                RESPONSIVE_YELLOW_DROP_PERCENTAGEs},
        /*2 */ {RESPONSIVE_RED_DROP_MIN_THD_CELLSs,
                RESPONSIVE_RED_DROP_MAX_THD_CELLSs,
                RESPONSIVE_RED_DROP_PERCENTAGEs},
        /*3 */ {NON_RESPONSIVE_GREEN_DROP_MIN_THD_CELLSs,
                NON_RESPONSIVE_GREEN_DROP_MAX_THD_CELLSs,
                NON_RESPONSIVE_GREEN_DROP_PERCENTAGEs},
        /*4 */ {NON_RESPONSIVE_YELLOW_DROP_MIN_THD_CELLSs,
                NON_RESPONSIVE_YELLOW_DROP_MAX_THD_CELLSs,
                NON_RESPONSIVE_YELLOW_DROP_PERCENTAGEs},
        /*5 */ {NON_RESPONSIVE_RED_DROP_MIN_THD_CELLSs,
                NON_RESPONSIVE_RED_DROP_MAX_THD_CELLSs,
                NON_RESPONSIVE_RED_DROP_PERCENTAGEs},
        /*6 */ {ECN_GREEN_DROP_MIN_THD_CELLSs,
                ECN_GREEN_DROP_MAX_THD_CELLSs,
                ECN_GREEN_DROP_PERCENTAGEs},
        /*7 */ {ECN_YELLOW_DROP_MIN_THD_CELLSs,
                ECN_YELLOW_DROP_MAX_THD_CELLSs,
                ECN_YELLOW_DROP_PERCENTAGEs},
        /*8 */ {ECN_RED_DROP_MIN_THD_CELLSs,
                ECN_RED_DROP_MAX_THD_CELLSs,
                ECN_RED_DROP_PERCENTAGEs},
    };

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_allocate(unit, BCMI_LTSW_PROFILE_WRED_DROP_CURVE,
                                          profile, 0,
                                          entries_per_set, profile_idx);
    /* Get an existing entry and return the profile index directly */
    if (rv == SHR_E_EXISTS) {
        SHR_EXIT();
    }

    if (rv == SHR_E_FULL) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_cosq_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_wred_percent_to_drop_prob, SHR_E_INIT);

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* Add an entry into LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_WRED_DROP_CURVE_SET_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TM_WRED_DROP_CURVE_SET_PROFILE_IDs,
                               *profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, fields[0].min_name,
                               profile->responsive_green_drop_min_thd_cells));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, fields[0].max_name,
                               profile->responsive_green_drop_max_thd_cells));

    rate = drv->cosq_wred_percent_to_drop_prob(
                               profile->responsive_green_drop_percentage);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, fields[0].rate_name, rate));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, fields[1].min_name,
                               profile->responsive_yellow_drop_min_thd_cells));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, fields[1].max_name,
                               profile->responsive_yellow_drop_max_thd_cells));

    rate = drv->cosq_wred_percent_to_drop_prob(
                               profile->responsive_yellow_drop_percentage);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, fields[1].rate_name, rate));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, fields[2].min_name,
                               profile->responsive_red_drop_min_thd_cells));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, fields[2].max_name,
                               profile->responsive_red_drop_max_thd_cells));

    rate = drv->cosq_wred_percent_to_drop_prob(
                               profile->responsive_red_drop_percentage);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, fields[2].rate_name, rate));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, fields[3].min_name,
                               profile->non_responsive_green_drop_min_thd_cells));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, fields[3].max_name,
                               profile->non_responsive_green_drop_max_thd_cells));

    rate = drv->cosq_wred_percent_to_drop_prob(
                               profile->non_responsive_green_drop_percentage);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, fields[3].rate_name, rate));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, fields[4].min_name,
                               profile->non_responsive_yellow_drop_min_thd_cells));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, fields[4].max_name,
                               profile->non_responsive_yellow_drop_max_thd_cells));

    rate = drv->cosq_wred_percent_to_drop_prob(
                               profile->non_responsive_yellow_drop_percentage);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, fields[4].rate_name, rate));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, fields[5].min_name,
                               profile->non_responsive_red_drop_min_thd_cells));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, fields[5].max_name,
                               profile->non_responsive_red_drop_max_thd_cells));

    rate = drv->cosq_wred_percent_to_drop_prob(
                               profile->non_responsive_red_drop_percentage);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, fields[5].rate_name, rate));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, fields[6].min_name,
                               profile->ecn_green_drop_min_thd_cells));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, fields[6].max_name,
                               profile->ecn_green_drop_max_thd_cells));

    rate = drv->cosq_wred_percent_to_drop_prob(
                               profile->ecn_green_drop_percentage);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, fields[6].rate_name, rate));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, fields[7].min_name,
                               profile->ecn_yellow_drop_min_thd_cells));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, fields[7].max_name,
                               profile->ecn_yellow_drop_max_thd_cells));

    rate = drv->cosq_wred_percent_to_drop_prob(
                               profile->ecn_yellow_drop_percentage);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, fields[7].rate_name, rate));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, fields[8].min_name,
                               profile->ecn_red_drop_min_thd_cells));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, fields[8].max_name,
                               profile->ecn_red_drop_max_thd_cells));

    rate = drv->cosq_wred_percent_to_drop_prob(
                               profile->ecn_red_drop_percentage);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, fields[8].rate_name, rate));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Get a entry from LT TM_WRED_DROP_CURVE_SET_PROFILE.
 *
 * This function is used to get a entry from LT TM_WRED_DROP_CURVE_SET_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile              Profile entry.
 * \param [in] profile_idx           Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_wred_drop_curve_profile_entry_get(int unit, int profile_idx,
                                       cosq_wred_drop_curve_profile_t *profile)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    tm_ltsw_cosq_chip_driver_t *drv = NULL;
    union {
        /* scalar data. */
        uint64_t    val;
        /* symbol data. */
        const char  *sym_val;
    } value;
    int dunit = 0;
    cosq_wred_drop_curve_field_t fields[] =
    {
        /*0 */ {RESPONSIVE_GREEN_DROP_MIN_THD_CELLSs,
                RESPONSIVE_GREEN_DROP_MAX_THD_CELLSs,
                RESPONSIVE_GREEN_DROP_PERCENTAGEs},
        /*1 */ {RESPONSIVE_YELLOW_DROP_MIN_THD_CELLSs,
                RESPONSIVE_YELLOW_DROP_MAX_THD_CELLSs,
                RESPONSIVE_YELLOW_DROP_PERCENTAGEs},
        /*2 */ {RESPONSIVE_RED_DROP_MIN_THD_CELLSs,
                RESPONSIVE_RED_DROP_MAX_THD_CELLSs,
                RESPONSIVE_RED_DROP_PERCENTAGEs},
        /*3 */ {NON_RESPONSIVE_GREEN_DROP_MIN_THD_CELLSs,
                NON_RESPONSIVE_GREEN_DROP_MAX_THD_CELLSs,
                NON_RESPONSIVE_GREEN_DROP_PERCENTAGEs},
        /*4 */ {NON_RESPONSIVE_YELLOW_DROP_MIN_THD_CELLSs,
                NON_RESPONSIVE_YELLOW_DROP_MAX_THD_CELLSs,
                NON_RESPONSIVE_YELLOW_DROP_PERCENTAGEs},
        /*5 */ {NON_RESPONSIVE_RED_DROP_MIN_THD_CELLSs,
                NON_RESPONSIVE_RED_DROP_MAX_THD_CELLSs,
                NON_RESPONSIVE_RED_DROP_PERCENTAGEs},
        /*6 */ {ECN_GREEN_DROP_MIN_THD_CELLSs,
                ECN_GREEN_DROP_MAX_THD_CELLSs,
                ECN_GREEN_DROP_PERCENTAGEs},
        /*7 */ {ECN_YELLOW_DROP_MIN_THD_CELLSs,
                ECN_YELLOW_DROP_MAX_THD_CELLSs,
                ECN_YELLOW_DROP_PERCENTAGEs},
        /*8 */ {ECN_RED_DROP_MIN_THD_CELLSs,
                ECN_RED_DROP_MAX_THD_CELLSs,
                ECN_RED_DROP_PERCENTAGEs},
    };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_cosq_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_wred_drop_prob_to_percent, SHR_E_INIT);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_WRED_DROP_CURVE_SET_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TM_WRED_DROP_CURVE_SET_PROFILE_IDs,
                               profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, fields[0].min_name,
                               &(value.val)));

    profile->responsive_green_drop_min_thd_cells = value.val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, fields[0].max_name,
                               &(value.val)));
    profile->responsive_green_drop_max_thd_cells = value.val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, fields[0].rate_name,
                               &(value.sym_val)));

    profile->responsive_green_drop_percentage =
             drv->cosq_wred_drop_prob_to_percent(value.sym_val);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, fields[1].min_name,
                               &(value.val)));

    profile->responsive_yellow_drop_min_thd_cells = value.val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, fields[1].max_name,
                               &(value.val)));
    profile->responsive_yellow_drop_max_thd_cells = value.val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, fields[1].rate_name,
                               &(value.sym_val)));

    profile->responsive_yellow_drop_percentage =
             drv->cosq_wred_drop_prob_to_percent(value.sym_val);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, fields[2].min_name,
                               &(value.val)));

    profile->responsive_red_drop_min_thd_cells = value.val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, fields[2].max_name,
                               &(value.val)));

    profile->responsive_red_drop_max_thd_cells = value.val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, fields[2].rate_name,
                               &(value.sym_val)));

    profile->responsive_red_drop_percentage =
             drv->cosq_wred_drop_prob_to_percent(value.sym_val);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, fields[3].min_name,
                               &(value.val)));
    profile->non_responsive_green_drop_min_thd_cells = value.val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, fields[3].max_name,
                               &(value.val)));

    profile->non_responsive_green_drop_max_thd_cells = value.val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, fields[3].rate_name,
                               &(value.sym_val)));

    profile->non_responsive_green_drop_percentage =
             drv->cosq_wred_drop_prob_to_percent(value.sym_val);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, fields[4].min_name,
                               &(value.val)));

    profile->non_responsive_yellow_drop_min_thd_cells = value.val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, fields[4].max_name,
                               &(value.val)));

    profile->non_responsive_yellow_drop_max_thd_cells = value.val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, fields[4].rate_name,
                               &(value.sym_val)));

    profile->non_responsive_yellow_drop_percentage =
             drv->cosq_wred_drop_prob_to_percent(value.sym_val);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, fields[5].min_name,
                               &(value.val)));

    profile->non_responsive_red_drop_min_thd_cells = value.val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, fields[5].max_name,
                               &(value.val)));

    profile->non_responsive_red_drop_max_thd_cells = value.val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, fields[5].rate_name,
                               &(value.sym_val)));

    profile->non_responsive_red_drop_percentage =
             drv->cosq_wred_drop_prob_to_percent(value.sym_val);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, fields[6].min_name,
                               &(value.val)));

    profile->ecn_green_drop_min_thd_cells = value.val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, fields[6].max_name,
                               &(value.val)));

    profile->ecn_green_drop_max_thd_cells = value.val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, fields[6].rate_name,
                               &(value.sym_val)));

    profile->ecn_green_drop_percentage =
             drv->cosq_wred_drop_prob_to_percent(value.sym_val);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, fields[7].min_name,
                               &(value.val)));

    profile->ecn_yellow_drop_min_thd_cells = value.val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, fields[7].max_name,
                               &(value.val)));

    profile->ecn_yellow_drop_max_thd_cells = value.val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, fields[7].rate_name,
                               &(value.sym_val)));

    profile->ecn_yellow_drop_percentage =
             drv->cosq_wred_drop_prob_to_percent(value.sym_val);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, fields[8].min_name,
                               &(value.val)));

    profile->ecn_red_drop_min_thd_cells = value.val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, fields[8].max_name,
                               &(value.val)));

    profile->ecn_red_drop_max_thd_cells = value.val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, fields[8].rate_name,
                               &(value.sym_val)));

    profile->ecn_red_drop_percentage =
             drv->cosq_wred_drop_prob_to_percent(value.sym_val);

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Delete a entry from LT TM_WRED_DROP_CURVE_SET_PROFILE.
 *
 * This function is used to delete a entry from LT TM_WRED_DROP_CURVE_SET_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_wred_drop_curve_profile_entry_delete(int unit, int profile_idx)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv = SHR_E_NONE;
    int dunit = 0;
    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_free(unit, BCMI_LTSW_PROFILE_WRED_DROP_CURVE,
                                      profile_idx);
    if (rv == SHR_E_BUSY) {
        /* Profile index is still used by others and return directly */
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_WRED_DROP_CURVE_SET_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TM_WRED_DROP_CURVE_SET_PROFILE_IDs,
                               profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));
exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Calculate hash signature for LT TM_WRED_DROP_CURVE_SET_PROFILE.
 *
 * This function is used to calculate hash signature for
 * LT TM_WRED_DROP_CURVE_SET_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              TM_WRED_DROP_CURVE_SET_PROFILE profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [out] hash                 Hash signature.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_wred_drop_curve_profile_hash_cb(int unit, void *entries,
                     int entries_per_set, uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *hash = shr_crc32(0, entries,
                      entries_per_set * sizeof(cosq_wred_drop_curve_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Compare profile set of LT TM_WRED_DROP_CURVE_SET_PROFILE.
 *
 * This function is used to compare profile set of LT
 * TM_WRED_DROP_CURVE_SET_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              TM_WRED_DROP_CURVE_SET_PROFILE profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [in]  index                Entry index of profile table.
 * \param [out] cmp                  Result of comparison.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_wred_drop_curve_profile_cmp_cb(int unit, void *entries,
                    int entries_per_set, int index, int *cmp)
{
    cosq_wred_drop_curve_profile_t profile;

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&profile, 0, sizeof(cosq_wred_drop_curve_profile_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_wred_drop_curve_profile_entry_get(unit, index, &profile));

    *cmp = sal_memcmp(entries, (void *)&profile,
                      sizeof(cosq_wred_drop_curve_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Setup profile with wred_config.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  wred_config   wred_config.
 * \param [out] profile       profile.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_wred_drop_curve_profile_fill(int unit, tm_ltsw_cosq_wred_config_t wred_config,
                                  cosq_wred_drop_curve_profile_t *profile)
{
    int rate, new_rate;
    const char *drop_prob;
    uint32_t wred_flags;
    int min_thresh;
    int max_thresh;
    tm_ltsw_cosq_chip_driver_t *drv = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_cosq_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_wred_percent_to_drop_prob, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_wred_drop_prob_to_percent, SHR_E_INIT);

    wred_flags = wred_config.flags;
    min_thresh = wred_config.min_thresh;
    max_thresh = wred_config.max_thresh;
    rate = wred_config.drop_probability;
    if (!(wred_flags & (BCM_COSQ_DISCARD_COLOR_GREEN |
                   BCM_COSQ_DISCARD_COLOR_YELLOW |
                   BCM_COSQ_DISCARD_COLOR_RED))) {
        wred_flags |= BCM_COSQ_DISCARD_COLOR_ALL;
    }

    if (!(wred_flags & (BCM_COSQ_DISCARD_NONTCP |
                        BCM_COSQ_DISCARD_NON_RESPONSIVE_DROP |
                        BCM_COSQ_DISCARD_ECT_MARKED)) ||
         ((wred_flags & BCM_COSQ_DISCARD_TCP) ||
         (wred_flags & BCM_COSQ_DISCARD_RESPONSIVE_DROP))) {
        if (wred_flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
            profile->responsive_green_drop_min_thd_cells = min_thresh;
            profile->responsive_green_drop_max_thd_cells = max_thresh;
            drop_prob = drv->cosq_wred_percent_to_drop_prob(rate);
            new_rate = drv->cosq_wred_drop_prob_to_percent(drop_prob);
            profile->responsive_green_drop_percentage = new_rate;
        }
        if (wred_flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            profile->responsive_yellow_drop_min_thd_cells = min_thresh;
            profile->responsive_yellow_drop_max_thd_cells = max_thresh;
            drop_prob = drv->cosq_wred_percent_to_drop_prob(rate);
            new_rate = drv->cosq_wred_drop_prob_to_percent(drop_prob);
            profile->responsive_yellow_drop_percentage = new_rate;
        }
        if (wred_flags & BCM_COSQ_DISCARD_COLOR_RED) {
            profile->responsive_red_drop_min_thd_cells = min_thresh;
            profile->responsive_red_drop_max_thd_cells = max_thresh;
            drop_prob = drv->cosq_wred_percent_to_drop_prob(rate);
            new_rate = drv->cosq_wred_drop_prob_to_percent(drop_prob);
            profile->responsive_red_drop_percentage = new_rate;
        }
    }
    if ((wred_flags & BCM_COSQ_DISCARD_NONTCP) ||
        (wred_flags & BCM_COSQ_DISCARD_NON_RESPONSIVE_DROP)){
        if (wred_flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
            profile->non_responsive_green_drop_min_thd_cells = min_thresh;
            profile->non_responsive_green_drop_max_thd_cells = max_thresh;
            drop_prob = drv->cosq_wred_percent_to_drop_prob(rate);
            new_rate = drv->cosq_wred_drop_prob_to_percent(drop_prob);
            profile->non_responsive_green_drop_percentage = new_rate;
        }
        if (wred_flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            profile->non_responsive_yellow_drop_min_thd_cells = min_thresh;
            profile->non_responsive_yellow_drop_max_thd_cells = max_thresh;
            drop_prob = drv->cosq_wred_percent_to_drop_prob(rate);
            new_rate = drv->cosq_wred_drop_prob_to_percent(drop_prob);
            profile->non_responsive_yellow_drop_percentage = new_rate;
        }
        if (wred_flags & BCM_COSQ_DISCARD_COLOR_RED) {
            profile->non_responsive_red_drop_min_thd_cells = min_thresh;
            profile->non_responsive_red_drop_max_thd_cells = max_thresh;
            drop_prob = drv->cosq_wred_percent_to_drop_prob(rate);
            new_rate = drv->cosq_wred_drop_prob_to_percent(drop_prob);
            profile->non_responsive_red_drop_percentage = new_rate;
        }
    }
    if (wred_flags & BCM_COSQ_DISCARD_ECT_MARKED) {
        if (wred_flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
            profile->ecn_green_drop_min_thd_cells = min_thresh;
            profile->ecn_green_drop_max_thd_cells = max_thresh;
            drop_prob = drv->cosq_wred_percent_to_drop_prob(rate);
            new_rate = drv->cosq_wred_drop_prob_to_percent(drop_prob);
            profile->ecn_green_drop_percentage = new_rate;
        }
        if (wred_flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            profile->ecn_yellow_drop_min_thd_cells = min_thresh;
            profile->ecn_yellow_drop_max_thd_cells = max_thresh;
            drop_prob = drv->cosq_wred_percent_to_drop_prob(rate);
            new_rate = drv->cosq_wred_drop_prob_to_percent(drop_prob);
            profile->ecn_yellow_drop_percentage = new_rate;
        }
        if (wred_flags & BCM_COSQ_DISCARD_COLOR_RED) {
            profile->ecn_red_drop_min_thd_cells = min_thresh;
            profile->ecn_red_drop_max_thd_cells = max_thresh;
            drop_prob = drv->cosq_wred_percent_to_drop_prob(rate);
            new_rate = drv->cosq_wred_drop_prob_to_percent(drop_prob);
            profile->ecn_red_drop_percentage = new_rate;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Setup wred_config with profile.
 *
 * \param [in]  unit          Unit number.
 * \param [out] wred_config   wred_config.
 * \param [in]  profile_index profile_index.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_wred_drop_curve_profile_get(int unit, int profile_index,
                                 tm_ltsw_cosq_wred_config_t *wred_config)
{
    cosq_wred_drop_curve_profile_t profile;
    int rate;
    int min_thresh;
    int max_thresh;
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = cosq_wred_drop_curve_profile_entry_get(unit, profile_index, &profile);
    if (rv == SHR_E_NOT_FOUND) {
        sal_memset(&profile, 0, sizeof(cosq_wred_drop_curve_profile_t));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (!(wred_config->flags & (BCM_COSQ_DISCARD_NONTCP |
                        BCM_COSQ_DISCARD_NON_RESPONSIVE_DROP |
                        BCM_COSQ_DISCARD_ECT_MARKED))) {
        /* By default we will assume
           BCM_COSQ_DISCARD_TCP */
        if (wred_config->flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            min_thresh = profile.responsive_yellow_drop_min_thd_cells;
            max_thresh = profile.responsive_yellow_drop_max_thd_cells;
            rate       = profile.responsive_yellow_drop_percentage;
        } else if (wred_config->flags & BCM_COSQ_DISCARD_COLOR_RED) {
            min_thresh = profile.responsive_red_drop_min_thd_cells;
            max_thresh = profile.responsive_red_drop_max_thd_cells;
            rate       = profile.responsive_red_drop_percentage;
        } else {
            /* By default we will assume
               BCM_COSQ_DISCARD_COLOR_GREEN */
            min_thresh = profile.responsive_green_drop_min_thd_cells;
            max_thresh = profile.responsive_green_drop_max_thd_cells;
            rate       = profile.responsive_green_drop_percentage;
        }
    } else if ((wred_config->flags & BCM_COSQ_DISCARD_NONTCP) ||
               (wred_config->flags & BCM_COSQ_DISCARD_NON_RESPONSIVE_DROP)) {
        if (wred_config->flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            min_thresh = profile.non_responsive_yellow_drop_min_thd_cells;
            max_thresh = profile.non_responsive_yellow_drop_max_thd_cells;
            rate       = profile.non_responsive_yellow_drop_percentage;
        } else if (wred_config->flags & BCM_COSQ_DISCARD_COLOR_RED) {
            min_thresh = profile.non_responsive_red_drop_min_thd_cells;
            max_thresh = profile.non_responsive_red_drop_max_thd_cells;
            rate       = profile.non_responsive_red_drop_percentage;
        } else {
            /* By default we will assume
               BCM_COSQ_DISCARD_COLOR_GREEN */
            min_thresh = profile.non_responsive_green_drop_min_thd_cells;
            max_thresh = profile.non_responsive_green_drop_max_thd_cells;
            rate       = profile.non_responsive_green_drop_percentage;
        }
    } else {
        if (wred_config->flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            min_thresh = profile.ecn_yellow_drop_min_thd_cells;
            max_thresh = profile.ecn_yellow_drop_max_thd_cells;
            rate       = profile.ecn_yellow_drop_percentage;
        } else if (wred_config->flags & BCM_COSQ_DISCARD_COLOR_RED) {
            min_thresh = profile.ecn_red_drop_min_thd_cells;
            max_thresh = profile.ecn_red_drop_max_thd_cells;
            rate       = profile.ecn_red_drop_percentage;
        } else {
            /* By default we will assume
               BCM_COSQ_DISCARD_COLOR_GREEN */
            min_thresh = profile.ecn_green_drop_min_thd_cells;
            max_thresh = profile.ecn_green_drop_max_thd_cells;
            rate       = profile.ecn_green_drop_percentage;
        }
    }

    wred_config->min_thresh = min_thresh;
    wred_config->max_thresh = max_thresh;
    wred_config->drop_probability = rate;

exit:
    SHR_FUNC_EXIT();
}

/*
 *
 * This function is used to retrieve default value from logic table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out]  min_thresh          min_thresh.
 * \param [out]  max_thresh          max_thresh.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_wred_drop_default_threshold_get(int unit, int *min_thresh, int *max_thresh)
{
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, TM_WRED_DROP_CURVE_SET_PROFILEs,
                                       RESPONSIVE_GREEN_DROP_MIN_THD_CELLSs,
                                       &idx_min, &idx_max));
    *min_thresh = (int)idx_max;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, TM_WRED_DROP_CURVE_SET_PROFILEs,
                                       RESPONSIVE_GREEN_DROP_MAX_THD_CELLSs,
                                       &idx_min, &idx_max));
    *max_thresh = (int)idx_max;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Add default profile.
 *
 * \param [in]   unit         Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_wred_drop_curve_default_profile_add(int unit)
{
    cosq_wred_drop_curve_profile_t profile;
    tm_ltsw_cosq_wred_config_t wred_config;
    int profile_idx;
    uint32_t ref_count = 0;
    uint32_t entry_num = 0;
    int min_thresh = 0, max_thresh = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&profile, 0, sizeof(cosq_wred_drop_curve_profile_t));

    sal_memset(&wred_config, 0, sizeof(tm_ltsw_cosq_wred_config_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_wred_drop_default_threshold_get(unit, &min_thresh, &max_thresh));

    wred_config.min_thresh = min_thresh;

    wred_config.max_thresh = max_thresh;

    wred_config.flags = BCM_COSQ_DISCARD_COLOR_ALL |
                        BCM_COSQ_DISCARD_NONTCP |
                        BCM_COSQ_DISCARD_NON_RESPONSIVE_DROP |
                        BCM_COSQ_DISCARD_ECT_MARKED |
                        BCM_COSQ_DISCARD_TCP;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_wred_drop_curve_profile_fill(unit, wred_config, &profile));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_wred_drop_curve_profile_entry_add(unit, &profile, &profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, TM_WRED_SERVICE_POOLs, &entry_num));
    ref_count += entry_num;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, TM_WRED_PORT_SERVICE_POOLs, &entry_num));
    ref_count += entry_num;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, TM_WRED_UC_Qs, &entry_num));
    ref_count += entry_num;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, BCMI_LTSW_PROFILE_WRED_DROP_CURVE,
                                              1, profile_idx, ref_count));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Register LT PORT_COS_Q_MAP.
 *
 * This function is used to register LT PORT_COS_Q_MAP.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_wred_drop_curve_profile_register(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_WRED_DROP_CURVE;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, TM_WRED_DROP_CURVE_SET_PROFILEs,
                                       TM_WRED_DROP_CURVE_SET_PROFILE_IDs,
                                       &idx_min, &idx_max));
    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    cosq_wred_drop_curve_profile_hash_cb,
                                    cosq_wred_drop_curve_profile_cmp_cb));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Recover hash and reference count of LT PORT_COS_Q_MAP.
 *
 * This function is used to Recover hash and reference count of
 * LT PORT_COS_Q_MAP profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] index                 Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_wred_drop_curve_profile_recover(int unit, int index)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_WRED_DROP_CURVE;
    cosq_wred_drop_curve_profile_t profile;
    int entries_per_set = 1;
    uint32_t ref_count = 0;
    int profile_id;
    uint32_t count = 0;
    uint32_t entry_num = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&profile, 0, sizeof(cosq_wred_drop_curve_profile_t));

    profile_id = index * entries_per_set;

    /* Update profile hash on the first usage.  */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl, profile_id,
                                         &ref_count));

    if (ref_count == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_drop_curve_profile_entry_get(unit, index, &profile));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update(unit, profile_hdl, &profile,
                                           entries_per_set, profile_id));

        if (profile_id == COSQ_DEFAULT_PROFILE_ID) {
            /* Increment default profile reference count to max. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_capacity_get(unit, TM_WRED_SERVICE_POOLs, &entry_num));
            count += entry_num;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_capacity_get(unit, TM_WRED_PORT_SERVICE_POOLs, &entry_num));
            count += entry_num;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_capacity_get(unit, TM_WRED_UC_Qs, &entry_num));
            count += entry_num;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_ref_count_increase
                    (unit, profile_hdl, entries_per_set, profile_id, count + 1));
        }
    }

    if (profile_id != COSQ_DEFAULT_PROFILE_ID) {
        /* Increment reference count. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_ref_count_increase(unit, profile_hdl,
                                                  entries_per_set, profile_id, 1));
        /* Decrease default profile reference count. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_ref_count_decrease(unit, profile_hdl,
                                                  entries_per_set,
                                                  COSQ_DEFAULT_PROFILE_ID, 1));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Recover LT PORT_COS_Q_MAP.
 *
 * This function is used to Recover hash and reference count of
 * LT PORT_COS_Q_MAP profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] index                 Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_wred_drop_curve_recover(int unit)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv, dunit, index;
    uint64_t data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_wred_drop_curve_profile_recover(unit, COSQ_DEFAULT_PROFILE_ID));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_WRED_SERVICE_POOLs, &entry_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, TM_WRED_DROP_CURVE_SET_PROFILE_IDs,
                                   &data));

        index = data;

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_drop_curve_profile_recover(unit, index));
    }

    if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_free(entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_WRED_PORT_SERVICE_POOLs, &entry_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, TM_WRED_DROP_CURVE_SET_PROFILE_IDs,
                                   &data));

        index = data;

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_drop_curve_profile_recover(unit, index));
    }

    if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_free(entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_WRED_UC_Qs, &entry_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, TM_WRED_DROP_CURVE_SET_PROFILE_IDs,
                                   &data));

        index = data;

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_drop_curve_profile_recover(unit, index));
    }

    if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Initialize the LT TM_WRED_DROP_CURVE_SET_PROFILE.
 *
 * This function is used to initialize the LT TM_WRED_DROP_CURVE_SET_PROFILE
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_wred_drop_curve_profile_init(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_WRED_DROP_CURVE;
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_profile_unregister(unit, profile_hdl), SHR_E_NOT_FOUND);

    /* Register profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_wred_drop_curve_profile_register(unit));

    if(!warm) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, TM_WRED_DROP_CURVE_SET_PROFILEs));

        /* Add default profile. */
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_drop_curve_default_profile_add(unit));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_drop_curve_recover(unit));
    }

exit:
    SHR_FUNC_EXIT();

}

/*
 * \brief Setup wred_config.
 *
 * \param [in]  unit                  Unit number.
 * \param [out] wred_config           wred_config.
 * \param [in]  current_q_size        current_q_size.
 * \param [in]  gain                  gain.
 * \param [in]  wred                  wred.
 * \param [in]  ecn                   ecn.
 * \param [in]  profile_index         profile_index.
 * \param [in]  time_profile_idx      time_profile_idx.
 * \param [in]  mark_ecn_profile_idx  mark_ecn_profile_idx.
 * \param [in]  drop_ecn_profile_idx  drop_ecn_profile_idx.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_wred_config_fill(int unit, tm_ltsw_cosq_wred_config_t *wred_config,
                      bool current_q_size, int gain, bool wred, bool ecn,
                      uint32_t profile_index, uint32_t time_profile_idx,
                      uint32_t mark_ecn_profile_idx, uint32_t drop_ecn_profile_idx)
{
    int time_domain = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_wred_drop_curve_profile_get(unit, profile_index, wred_config));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_wred_time_profile_entry_get(unit, time_profile_idx, &time_domain));

    wred_config->refresh_time = time_domain + 1;
    wred_config->gain = gain;

    /* Get flags */
    if (wred_config->flags) {
        wred_config->flags &= ~(BCM_COSQ_DISCARD_CAP_AVERAGE |
                                BCM_COSQ_DISCARD_ENABLE |
                                BCM_COSQ_DISCARD_MARK_CONGESTION);
        if (current_q_size) {
            wred_config->flags |= BCM_COSQ_DISCARD_CAP_AVERAGE;
        }
        if (wred) {
            wred_config->flags |= BCM_COSQ_DISCARD_ENABLE;
        }
        if (ecn) {
            wred_config->flags |= BCM_COSQ_DISCARD_MARK_CONGESTION;
        }
    }

    /* Get resolution table index */
    if (ecn) {
        wred_config->profile_id = mark_ecn_profile_idx;
    } else {
        wred_config->profile_id = drop_ecn_profile_idx;
    }

    /* Get use_queue_group */

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief get new_drop_index and new_time_index.
 *
 * \param [in]  unit                  Unit number.
 * \param [out] wred_config           wred_config.
 * \param [in]  old_drop_index        old_drop_index.
 * \param [in]  new_drop_index        new_drop_index.
 * \param [in]  old_time_index        old_time_index.
 * \param [in]  new_time_index        new_time_index.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_wred_profile_index_get(int unit, tm_ltsw_cosq_wred_config_t wred_config,
                            int old_drop_index, int *new_drop_index,
                            int old_time_index, int *new_time_index)
{
    cosq_wred_drop_curve_profile_t profile;
    int time_domain;

    SHR_FUNC_ENTER(unit);

    *new_drop_index = COSQ_PROFILE_INDEX_INVALID;
    *new_time_index = COSQ_PROFILE_INDEX_INVALID;

    if (wred_config.need_profile) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_drop_curve_profile_entry_get(unit, old_drop_index,
                                                    &profile));

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_drop_curve_profile_fill(unit, wred_config, &profile));

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_drop_curve_profile_entry_add(unit, &profile,
                                                    new_drop_index));
    }

    time_domain = wred_config.refresh_time - 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_wred_time_profile_entry_add(unit, &time_domain, new_time_index));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief cosq wred service pool set.
 *
 * \param [in]  unit                  Unit number.
 * \param [in]  port                  port.
 * \param [in]  cosq                  cosq.
 * \param [in]  wred_config           wred_config.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_wred_service_pool_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           tm_ltsw_cosq_wred_config_t wred_config)
{
    int spid;
    int profile_index, time_profile_idx;
    int old_profile_index, old_time_profile_idx;
    int from_spid, to_spid;
    bool current_q_size;
    bool wred;
    int gain;
    int rv;

    SHR_FUNC_ENTER(unit);

    if (port == BCM_GPORT_INVALID) {
        /* (port == -1) For all Global SPs */
        from_spid = 0;
        to_spid = 3;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &port));

        if (cosq != -1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_thd_port_uc_spid_get(unit, port, cosq, &spid, NULL));
            from_spid = to_spid = spid;
        } else {
            /* (cosq == -1) For all Global SPs */
            from_spid = 0;
            to_spid = 3;
        }
    }

    for (spid = from_spid; spid <= to_spid; spid++ ) {
        rv = cosq_tm_wred_service_pool_get(unit, spid,
                                           &current_q_size, &gain,
                                           &wred,
                                           &old_time_profile_idx,
                                           &old_profile_index);
        if (rv == SHR_E_NOT_FOUND) {
            old_time_profile_idx = 0;
            old_profile_index = 0;
            current_q_size = 0;
            wred = 0;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }

        if (!wred_config.ignore_enable_flags) {
            current_q_size = wred_config.flags &
                             BCM_COSQ_DISCARD_CAP_AVERAGE ? 1 : 0;
            wred           = wred_config.flags &
                             BCM_COSQ_DISCARD_ENABLE ? 1 : 0;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_profile_index_get(unit, wred_config,
                                         old_profile_index, &profile_index,
                                         old_time_profile_idx,
                                         &time_profile_idx));

        if (wred_config.need_profile) {
            SHR_IF_ERR_CONT
                (cosq_tm_wred_service_pool_set(unit, spid, current_q_size,
                                               wred_config.gain, wred,
                                               time_profile_idx, profile_index));

            if (SHR_FUNC_ERR()) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_wred_drop_curve_profile_entry_delete(unit, profile_index));

                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_wred_time_profile_entry_delete(unit, time_profile_idx));

                SHR_EXIT();
            }

            if (old_profile_index != profile_index) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_wred_drop_curve_profile_entry_delete(unit, old_profile_index));
            }
        } else {
            SHR_IF_ERR_CONT
                (cosq_tm_wred_service_pool_set(unit, spid, current_q_size,
                                               wred_config.gain, wred,
                                               time_profile_idx, -1));
        }

        if (old_time_profile_idx != time_profile_idx) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_wred_time_profile_entry_delete(unit, old_time_profile_idx));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief cosq wred service pool get.
 *
 * \param [in]  unit                  Unit number.
 * \param [in]  port                  port.
 * \param [in]  cosq                  cosq.
 * \param [out] wred_config           wred_config.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_wred_service_pool_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           tm_ltsw_cosq_wred_config_t *wred_config)
{
    int spid;
    int profile_index, time_profile_idx;
    bool current_q_size;
    bool wred;
    int gain;

    SHR_FUNC_ENTER(unit);

    if (port == BCM_GPORT_INVALID) {
        /* (port == -1) For all Global SPs */
        spid = 0;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &port));
        if (cosq != -1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_thd_port_uc_spid_get(unit, port, cosq, &spid, NULL));
        } else {
            /* (cosq == -1) For all Global SPs */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_thd_port_uc_spid_get(unit, port, 0, &spid, NULL));
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_wred_service_pool_get(unit, spid,
                                       &current_q_size, &gain,
                                       &wred,
                                       &time_profile_idx,
                                       &profile_index));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_wred_config_fill(unit, wred_config, current_q_size, gain, wred, 0,
                               profile_index, time_profile_idx, 0, 0));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief cosq wred port service pool set.
 *
 * \param [in]  unit                  Unit number.
 * \param [in]  port                  port.
 * \param [in]  cosq                  cosq.
 * \param [in]  wred_config           wred_config.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_wred_port_service_pool_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                                tm_ltsw_cosq_wred_config_t wred_config)
{
    int spid;
    int profile_index, time_profile_idx;
    int old_profile_index, old_time_profile_idx;
    int from_spid, to_spid;
    bool current_q_size;
    bool wred;
    int gain;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (cosq != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_thd_port_uc_spid_get(unit, port, cosq, &spid, NULL));
        from_spid = to_spid = spid;
    } else {
        /* (cosq == -1) For all Global SPs */
        from_spid = 0;
        to_spid = 3;
    }

    for (spid = from_spid; spid <= to_spid; spid++ ) {
        rv = cosq_tm_wred_port_service_pool_get(unit, port, spid,
                                                &current_q_size, &gain,
                                                &wred,
                                                &old_time_profile_idx,
                                                &old_profile_index);
        if (rv == SHR_E_NOT_FOUND) {
            old_time_profile_idx = 0;
            old_profile_index = 0;
            current_q_size = 0;
            wred = 0;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }

        if (!wred_config.ignore_enable_flags) {
            current_q_size = wred_config.flags &
                             BCM_COSQ_DISCARD_CAP_AVERAGE ? 1 : 0;
            wred = wred_config.flags &
                             BCM_COSQ_DISCARD_ENABLE ? 1 : 0;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_profile_index_get(unit, wred_config,
                                         old_profile_index, &profile_index,
                                         old_time_profile_idx,
                                         &time_profile_idx));

        if (wred_config.need_profile) {
            SHR_IF_ERR_CONT
                (cosq_tm_wred_port_service_pool_set(unit, port, spid,
                                                    current_q_size,
                                                    wred_config.gain,
                                                    wred, time_profile_idx,
                                                    profile_index));

            if (SHR_FUNC_ERR()) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_wred_drop_curve_profile_entry_delete(unit, profile_index));

                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_wred_time_profile_entry_delete(unit, time_profile_idx));

                SHR_EXIT();
            }

            if (old_profile_index != profile_index) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_wred_drop_curve_profile_entry_delete(unit,
                                                               old_profile_index));
            }
        } else {
            SHR_IF_ERR_CONT
                (cosq_tm_wred_port_service_pool_set(unit, port, spid,
                                                    current_q_size,
                                                    wred_config.gain,
                                                    wred, time_profile_idx,
                                                    -1));
        }

        if (old_time_profile_idx != time_profile_idx) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_wred_time_profile_entry_delete(unit, old_time_profile_idx));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief cosq wred port service pool get.
 *
 * \param [in]  unit                  Unit number.
 * \param [in]  port                  port.
 * \param [in]  cosq                  cosq.
 * \param [out] wred_config           wred_config.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_wred_port_service_pool_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                                tm_ltsw_cosq_wred_config_t *wred_config)
{
    int spid = 0;
    int profile_index, time_profile_idx;
    bool current_q_size;
    bool wred;
    int gain;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (cosq != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_thd_port_uc_spid_get(unit, port, cosq, &spid, NULL));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_wred_port_service_pool_get(unit, port, spid, &current_q_size,
                                            &gain, &wred, &time_profile_idx,
                                            &profile_index));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_wred_config_fill(unit, wred_config, current_q_size, gain, wred, 0,
                               profile_index, time_profile_idx, 0, 0));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief cosq wred uc service pool set.
 *
 * \param [in]  unit                  Unit number.
 * \param [in]  port                  port.
 * \param [in]  cosq                  cosq.
 * \param [in]  wred_config           wred_config.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_wred_uc_queue_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                       tm_ltsw_cosq_wred_config_t wred_config)
{
    int queue;
    int profile_index, time_profile_idx;
    int old_profile_index, old_time_profile_idx;
    int mark_ecn_profile_idx, drop_ecn_profile_idx;
    int old_mark_ecn_profile_idx, old_drop_ecn_profile_idx;
    int from_cosq, to_cosq;
    bool current_q_size;
    bool wred;
    bool ecn;
    int gain;
    int num_ucq;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_num_queue_get(unit, &num_ucq, NULL));

    from_cosq = to_cosq = cosq;

    if (cosq == -1){
        /* (cosq == -1) For all Queues of the given port */
        from_cosq = 0;
        to_cosq = num_ucq - 1;
    }

    for (queue = from_cosq; queue <= to_cosq; queue++ ) {
        rv = cosq_tm_wred_uc_queue_get(unit, port, queue,
                                       &gain, &current_q_size,
                                       &wred, &ecn, &old_time_profile_idx,
                                       &old_mark_ecn_profile_idx,
                                       &old_drop_ecn_profile_idx,
                                       &old_profile_index);

        if (rv == SHR_E_NOT_FOUND) {
            old_time_profile_idx = 0;
            old_mark_ecn_profile_idx = 0;
            old_drop_ecn_profile_idx = 0;
            old_profile_index = 0;
            current_q_size = 0;
            wred = 0;
            ecn = 0;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_profile_index_get(unit, wred_config,
                                         old_profile_index, &profile_index,
                                         old_time_profile_idx,
                                         &time_profile_idx));

        if (!wred_config.ignore_enable_flags) {
            current_q_size = wred_config.flags &
                             BCM_COSQ_DISCARD_CAP_AVERAGE ? 1 : 0;
            wred = wred_config.flags &
                             BCM_COSQ_DISCARD_ENABLE ? 1 : 0;
            ecn = wred_config.flags &
                             BCM_COSQ_DISCARD_MARK_CONGESTION ? 1 : 0;
        }

        if (wred_config.flags & BCM_COSQ_DISCARD_MARK_CONGESTION) {
            mark_ecn_profile_idx = wred_config.profile_id;
            drop_ecn_profile_idx = old_drop_ecn_profile_idx;
        } else {
            drop_ecn_profile_idx = wred_config.profile_id;
            mark_ecn_profile_idx = old_mark_ecn_profile_idx;
        }

        if (wred_config.need_profile) {
            SHR_IF_ERR_CONT
                (cosq_tm_wred_uc_queue_set(unit, port, queue, wred_config.gain,
                                           current_q_size, wred, ecn,
                                           time_profile_idx, mark_ecn_profile_idx,
                                           drop_ecn_profile_idx, profile_index));
            if (SHR_FUNC_ERR()) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_wred_drop_curve_profile_entry_delete(unit, profile_index));

                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_wred_time_profile_entry_delete(unit, time_profile_idx));

                SHR_EXIT();
            }

            if (old_profile_index != profile_index) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_wred_drop_curve_profile_entry_delete(unit, old_profile_index));
            }
        } else {
            SHR_IF_ERR_CONT
                (cosq_tm_wred_uc_queue_set(unit, port, queue, wred_config.gain,
                                           current_q_size, wred, ecn,
                                           time_profile_idx, mark_ecn_profile_idx,
                                           drop_ecn_profile_idx, -1));
        }

        if (old_time_profile_idx != time_profile_idx) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_wred_time_profile_entry_delete(unit, old_time_profile_idx));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief cosq wred uc service pool get.
 *
 * \param [in]  unit                  Unit number.
 * \param [in]  port                  port.
 * \param [in]  cosq                  cosq.
 * \param [out] wred_config           wred_config.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_wred_uc_queue_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                       tm_ltsw_cosq_wred_config_t *wred_config)
{
    int queue;
    int profile_index, time_profile_idx;
    int mark_ecn_profile_idx, drop_ecn_profile_idx;
    bool current_q_size;
    bool wred;
    bool ecn;
    int gain;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    queue = cosq;

    if (cosq == -1){
        queue = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_wred_uc_queue_get(unit, port, queue, &gain, &current_q_size,
                                   &wred, &ecn, &time_profile_idx,
                                   &mark_ecn_profile_idx,
                                   &drop_ecn_profile_idx, &profile_index));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_wred_config_fill(unit, wred_config, current_q_size, gain, wred, ecn,
                               profile_index, time_profile_idx, mark_ecn_profile_idx,
                               drop_ecn_profile_idx));

exit:
    SHR_FUNC_EXIT();
}


/*
 * \brief cosq wred set.
 *
 * \param [in]  unit                  Unit number.
 * \param [in]  port                  port.
 * \param [in]  cosq                  cosq.
 * \param [in]  wred_config           wred_config.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_wred_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
              tm_ltsw_cosq_wred_config_t wred_config)
{
    bcm_pbmp_t pbmp;
    bcm_port_t port_num;
    int device_setting = 0;
    int portsp_setting = 0;
    int spid_need = FALSE;
    int num_ucq, queue, spid, spid_n;
    SHR_FUNC_ENTER(unit);

    if (wred_config.flags & BCM_COSQ_DISCARD_DEVICE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_service_pool_set(unit, port, cosq, wred_config));
        device_setting = 1;
        if (port != -1 && cosq != -1) {
            spid_need = TRUE;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_thd_port_uc_spid_get(unit, port, cosq, &spid, NULL));
        }
    } else if (wred_config.flags & BCM_COSQ_DISCARD_PORT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_port_service_pool_set(unit, port, cosq, wred_config));
        portsp_setting = 1;
        if (cosq != -1) {
            spid_need = TRUE;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_thd_port_uc_spid_get(unit, port, cosq, &spid, NULL));
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_uc_queue_set(unit, port, cosq, wred_config));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_PORT, &pbmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_num_queue_get(unit, &num_ucq, NULL));

    if (device_setting) {
        wred_config.flags &= ~(BCM_COSQ_DISCARD_DEVICE);
        BCM_PBMP_ITER(pbmp, port_num) {
            if (spid_need) {
                for (queue = 0; queue < num_ucq; queue++) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_ltsw_cosq_thd_port_uc_spid_get(unit, port_num, queue,
                                                             &spid_n, NULL));
                    if (spid == spid_n) {
                        LOG_INFO(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                            "cosq_wred_set: device_setting calling wred_queue"
                            " with port: %d cosq: %d\n"), port_num, queue));
                        SHR_IF_ERR_VERBOSE_EXIT
                            (cosq_wred_set(unit, port_num, queue, wred_config));
                    }
                }
            } else {
                LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                    "cosq_wred_set: device_setting calling wred_queue"
                    " with port: %d cosq: -1\n"), port_num));
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_wred_set(unit, port_num, -1, wred_config));
            }
        }
    }
    if (portsp_setting) {
        wred_config.flags &= ~(BCM_COSQ_DISCARD_PORT);
        if (spid_need) {
            for (queue = 0; queue < num_ucq; queue++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_cosq_thd_port_uc_spid_get(unit, port, queue,
                                                         &spid_n, NULL));
                if (spid == spid_n) {
                    LOG_INFO(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                        "cosq_wred_set: portsp_setting calling wred_queue"
                        " with port: %d cosq: %d\n"), port, queue));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (cosq_wred_set(unit, port, queue, wred_config));
                }
            }
        } else {
            LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "cosq_wred_set: portsp_setting calling wred_queue"
                " with port: %d cosq: %d\n"), port, cosq));
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_wred_set(unit, port, cosq, wred_config));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief cosq wred get.
 *
 * \param [in]  unit                  Unit number.
 * \param [in]  port                  port.
 * \param [in]  cosq                  cosq.
 * \param [out] wred_config           wred_config.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_wred_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
              tm_ltsw_cosq_wred_config_t *wred_config)
{
    SHR_FUNC_ENTER(unit);

    if (wred_config->flags & BCM_COSQ_DISCARD_DEVICE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_service_pool_get(unit, port, cosq, wred_config));
    } else if (wred_config->flags & BCM_COSQ_DISCARD_PORT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_port_service_pool_get(unit, port, cosq, wred_config));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_uc_queue_get(unit, port, cosq, wred_config));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Clear wred config.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_wred_default_clear(int unit)
{
    (void)bcmi_lt_clear(unit, TM_WRED_SERVICE_POOLs);

    (void)bcmi_lt_clear(unit, TM_WRED_PORT_SERVICE_POOLs);

    (void)bcmi_lt_clear(unit, TM_WRED_UC_Qs);

    (void)bcmi_lt_clear(unit, TM_WRED_TIME_PROFILEs);

    (void)bcmi_lt_clear(unit, TM_WRED_DROP_CURVE_SET_PROFILEs);

    return SHR_E_NONE;
}

/*
 * \brief Clear Default Scheduler Configuration for all the ports.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_schedule_default_clear(int unit)
{
    (void)bcmi_lt_clear(unit, TM_SCHEDULER_CPU_PORTs);

    (void)bcmi_lt_clear(unit, TM_SCHEDULER_SHAPER_CPU_NODEs);

    (void)bcmi_lt_clear(unit, TM_SCHEDULER_NODEs);

    (void)bcmi_lt_clear(unit, TM_SHAPER_NODEs);

    return SHR_E_NONE;
}

/*
 * \brief L0 node schedule mode set.
 *
 * \param [in] unit           Unit number.
 * \param [in] port_num       PORT ID.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_port_schedule_node_set(int unit, int port, int profile_id)
{
    int idx;
    int sched_mode = 1;
    int id, ucq_id;
    bcmi_ltsw_cosq_device_info_t device_info = {0};
    tm_ltsw_cosq_scheduler_profile_q_info_t profile;
    int num_node;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    for (idx = 0; idx < device_info.num_sched_node; idx++) {

        rv = cosq_tm_schedule_profile_queue_info_get(unit, profile_id, idx,
                                                     &profile);
        num_node = profile.num_mcq + profile.num_ucq;

        if (rv == SHR_E_NOT_FOUND || num_node == 0) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT(rv);

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_schedule_node_set(unit, port, idx, 0, sched_mode, -1));

        for (ucq_id = 0; ucq_id < profile.num_ucq; ucq_id++) {
            id = profile.ucq_id[ucq_id];
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_tm_schedule_node_set(unit, port, id, 1, sched_mode, -1));
        }

        if (profile.num_mcq == 1) {
            id = profile.mcq_id;
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_tm_schedule_node_set(unit, port, id, 2, sched_mode, -1));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Init default schedule profile.
 *
 * \param [in] unit           Unit number.
 * \param [in] profile        cosq_scheduler_profile_t.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_scheduler_default_profile_init(int unit, cosq_scheduler_profile_t *profile)
{
    int cosq_idx;
    int num_ucq;
    int warm = bcmi_warmboot_get(unit);
    int rv = SHR_E_NONE;
    int profile_id = 0;
    int op_status = 0;
    bcmi_ltsw_cosq_device_info_t device_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_num_queue_get(unit, &num_ucq, NULL));
    /*
     * Default scheduler profile 0 is setup with default values
     * or override via bcm config
     */
    for (cosq_idx = 0; cosq_idx < device_info.num_sched_node; cosq_idx++) {
        rv = cosq_tm_schedule_profile_entry_get(unit, profile_id, cosq_idx,
                                        &profile[cosq_idx].num_ucq,
                                        &profile[cosq_idx].num_mcq,
                                        &profile[cosq_idx].fc_is_uc_only);

        if (rv != SHR_E_NONE) {
            if (cosq_idx < num_ucq) {
                profile[cosq_idx].num_ucq= 1;
                profile[cosq_idx].num_mcq= 0;
            } else {
                profile[cosq_idx].num_ucq = 0;
                profile[cosq_idx].num_mcq = 1;
            }

            profile[cosq_idx].strict_priority = 0;
            profile[cosq_idx].fc_is_uc_only = 0;

            if(!warm) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_tm_schedule_profile_entry_set(
                        unit, profile_id, cosq_idx,
                        profile[cosq_idx].num_ucq,
                        profile[cosq_idx].num_mcq,
                        profile[cosq_idx].fc_is_uc_only,
                        &op_status));

                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_tm_schedule_sp_profile_set(unit, profile_id, cosq_idx,
                                                     profile[cosq_idx].strict_priority));
            }
        }
    }

    if (op_status != 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Retrieve a profile.
 *
 * \param [in] unit             Unit number.
 * \param [in] profile_id       profile id.
 * \param [in] profile          profile.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_scheduler_profile_get(int unit, int profile_id, int array_max,
                           cosq_scheduler_profile_t *profile, int *size)
{
    int cos, number_cos, rv;
    int sp = 0;
    bcmi_ltsw_cosq_device_info_t device_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    if (profile_id < 0 || profile_id >= device_info.num_sched_profile) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (array_max < 0 || array_max > MAX_TM_SCHEDULER_NODE) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (profile == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(profile, 0,
              (sizeof(cosq_scheduler_profile_t) * MAX_TM_SCHEDULER_NODE));

    number_cos = 0;

    for (cos = 0; cos < array_max; cos++) {
        rv = cosq_tm_schedule_profile_entry_get(unit, profile_id, cos,
                                                &profile[cos].num_ucq,
                                                &profile[cos].num_mcq,
                                                &profile[cos].fc_is_uc_only);

        if (rv == SHR_E_NOT_FOUND) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT(rv);

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_schedule_sp_profile_get(unit, profile_id, cos, &sp));

        profile[cos].strict_priority = sp;
        number_cos++;
    }

    if (size) {
        *size = number_cos;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Copy data from profile to cosq_mapping_arr.
 *
 * \param [in] unit             Unit number.
 * \param [in] profile          profile.
 * \param [in] cosq_mapping_arr cosq_mapping_arr.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static void
cosq_mapping_arr_set(int unit, cosq_scheduler_profile_t *profile,
                     bcm_cosq_mapping_t *cosq_mapping_arr, int size)
{
    int cosq_idx;

    for (cosq_idx = 0; cosq_idx < size; cosq_idx++) {
        cosq_mapping_arr[cosq_idx].cosq = cosq_idx;
        cosq_mapping_arr[cosq_idx].num_ucq = profile[cosq_idx].num_ucq;
        cosq_mapping_arr[cosq_idx].num_mcq = profile[cosq_idx].num_mcq;
        cosq_mapping_arr[cosq_idx].strict_priority =
                                             profile[cosq_idx].strict_priority;
        cosq_mapping_arr[cosq_idx].fc_is_uc_only =
                                             profile[cosq_idx].fc_is_uc_only;
    }
}

/*
 * \brief Set a profile.
 *
 * \param [in] unit             Unit number.
 * \param [in] profile_id       profile id.
 * \param [in] profile          profile.
 * \param [in] size             size.
 * \param [in] cosq_mapping_arr cosq_mapping_arr.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_scheduler_profile_set(int unit, int profile_id,
                           cosq_scheduler_profile_t *profile,
                           int size, bcm_cosq_mapping_t *cosq_mapping_arr)
{
    int cos;
    int cosq_idx;
    int op_status = 0;
    bcmi_ltsw_cosq_device_info_t device_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    if (profile_id < 0 || profile_id >= device_info.num_sched_profile) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (cosq_mapping_arr == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (size < 0 || size > MAX_TM_SCHEDULER_NODE) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    for (cosq_idx = size; cosq_idx < MAX_TM_SCHEDULER_NODE; cosq_idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_schedule_profile_entry_delete(unit, profile_id, cosq_idx));
    }

    for (cosq_idx = 0; cosq_idx < size; cosq_idx++) {
        cos = cosq_mapping_arr[cosq_idx].cosq;
        profile[cos].num_ucq = cosq_mapping_arr[cosq_idx].num_ucq;
        profile[cos].num_mcq = cosq_mapping_arr[cosq_idx].num_mcq;

        profile[cos].strict_priority =
                               cosq_mapping_arr[cosq_idx].strict_priority;
        profile[cos].fc_is_uc_only =
                               cosq_mapping_arr[cosq_idx].fc_is_uc_only;

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_schedule_profile_entry_set(unit, profile_id, cos,
                                                profile[cos].num_ucq,
                                                profile[cos].num_mcq,
                                                profile[cos].fc_is_uc_only,
                                                &op_status));

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_schedule_sp_profile_set(unit, profile_id, cosq_idx,
                                             profile[cos].strict_priority));
    }

    if (op_status != 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Wrapper function to set L0->schedq->MMUQ map for a specific profile.
 *
 * \param [in] unit           Unit number.
 * \param [in] port_base      PORT ID.
 * \param [in] port           PORT ID.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_cpu_hierarchy_init(int unit)
{
    int queue_num;
    int num_queue_pci, num_queue_uc0, start_q, end_q;
    bcmi_ltsw_cosq_device_info_t device_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    num_queue_pci = bcmi_ltsw_property_get(unit, BCMI_CPN_NUM_QUEUES_PCI,
                                           device_info.num_cpu_queue);
    num_queue_uc0 = bcmi_ltsw_property_get(unit, BCMI_CPN_NUM_QUEUES_UC0,
                                           0);
    start_q = num_queue_pci;
    end_q = num_queue_pci + num_queue_uc0;

    if (start_q > device_info.num_cpu_queue) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
    }

    if (end_q > device_info.num_cpu_queue) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
    }

    for (queue_num = 0; queue_num < num_queue_pci; queue_num++) {
        /* All pci queues attach to L0.0 by default. */
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_cpu_parent_set(unit, queue_num, LTSW_COSQ_NODE_LVL_MC, 0));

        /*
         * Channel 0 is used for tx, rx begine with channel 1.
         * All pci queues mapped to channel 1 by default.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_rx_queue_channel_set(unit, queue_num, 1, TRUE));
    }

    for (queue_num = start_q; queue_num < end_q; queue_num++) {
        /* All arm queues attach to L0.7 by default. */
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_cpu_parent_set(unit, queue_num, LTSW_COSQ_NODE_LVL_MC, 7));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_cpu_schedule_set(unit, 1));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Wrapper function to set L0->schedq->MMUQ map for a specific profile.
 *
 * \param [in] unit           Unit number.
 * \param [in] port_base      PORT ID.
 * \param [in] port           PORT ID.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_hierarchy_init(int unit)
{
    bcm_pbmp_t pbmp;
    int port;
    bcmi_ltsw_cosq_port_map_info_t port_info;
    int default_profile_id = 0;
    int profile_id = -1;
    int wrr = 1;
    cosq_scheduler_profile_t default_sched_profile[MAX_TM_SCHEDULER_NODE] = {{0}};
    cosq_scheduler_profile_t sched_profile[MAX_TM_SCHEDULER_NODE] = {{0}};
    bcm_cosq_mapping_t cosq_mapping_arr[MAX_TM_SCHEDULER_NODE] = {{0}};
    int warm = bcmi_warmboot_get(unit);
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &pbmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_scheduler_default_profile_init(unit, default_sched_profile));

    BCM_PBMP_ITER(pbmp, port) {
        rv = cosq_tm_port_map_info_get(unit, port, &port_info);
        if (rv == SHR_E_NOT_FOUND) {
            continue;
        }

        /* RDB ports donot have queue hierarchy. */
        if (is_rdb_port(unit, port)) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT(rv);

        if (is_cpu_port(unit, port) && !warm) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_cpu_hierarchy_init(unit));
            continue;
        }

        rv = cosq_tm_schedule_port_profile_get(unit, port, &profile_id, &wrr);
        if (rv == SHR_E_NONE) {
            rv = cosq_scheduler_profile_get(unit, profile_id, MAX_TM_SCHEDULER_NODE,
                                            sched_profile, NULL);

            if (rv != SHR_E_NONE) {
                LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                    "Override failed Resetting scheduler profile to default init\n")));

                cosq_mapping_arr_set(unit, default_sched_profile,
                                     cosq_mapping_arr, MAX_TM_SCHEDULER_NODE);

                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_scheduler_profile_set(unit, profile_id, sched_profile,
                                                MAX_TM_SCHEDULER_NODE, cosq_mapping_arr));
            }

            if(!warm) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_port_schedule_node_set(unit, port, profile_id));
            }
        } else {
            if(!warm) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_tm_schedule_port_profile_set(unit, port,
                                                       default_profile_id, wrr));

                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_port_schedule_node_set(unit, port, default_profile_id));
            }
        }

        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Port scheduler mapping setup %d\n"), port));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Wrapper function to set L0->schedq->MMUQ map for a specific profile.
 *
 * \param [in] unit           Unit number.
 * \param [in] port_base      PORT ID.
 * \param [in] port           PORT ID.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_hierarchy_setup(int unit, int profile_index)
{
    int profile_in_use = -1;
    bcm_pbmp_t pbmp;
    int port;
    bcmi_ltsw_cosq_port_map_info_t port_info;
    int wrr = 1;
    bcmi_ltsw_cosq_device_info_t device_info = {0};
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    if (profile_index < 0 || profile_index >= device_info.num_sched_profile) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &pbmp));

    BCM_PBMP_ITER(pbmp, port) {
        rv = cosq_tm_port_map_info_get(unit, port, &port_info);
        if (rv == SHR_E_NOT_FOUND) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT(rv);

        if (is_cpu_port(unit, port)) {
            continue;
        }
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (cosq_tm_schedule_port_profile_get(unit, port, &profile_in_use,
                                               &wrr), SHR_E_NOT_FOUND);

        if (profile_index == profile_in_use) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_tm_schedule_port_profile_set(unit, port, profile_index,
                                                   wrr));
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_port_schedule_node_set(unit, port, profile_index));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Init cut-through mode for a specific port
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] enable Enable to use cut-through mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_port_ct_mode_init(int unit)
{
    bcm_pbmp_t pbmp;
    int port;
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    if(!warm) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_PORT, &pbmp));

        BCM_PBMP_ITER(pbmp, port) {
            if (bcmi_ltsw_port_is_type(unit, port,
                                       BCMI_LTSW_PORT_TYPE_CPU |
                                       BCMI_LTSW_PORT_TYPE_MGMT |
                                       BCMI_LTSW_PORT_TYPE_RDB |
                                       BCMI_LTSW_PORT_TYPE_LB)) {
                continue;
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_tm_port_ct_mode_set(unit, port, 0, 0));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*
 * \brief Init cut-through mode for a specific port
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] enable Enable to use cut-through mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_port_ct_mode_clear(int unit)
{
    (void)bcmi_lt_clear(unit, TM_CUT_THROUGH_PORTs);

    return SHR_E_NONE;
}

/******************************************************************************
 * Public Internal HSDK functions
 */

int
tm_ltsw_cosq_chip_driver_register(int unit, tm_ltsw_cosq_chip_driver_t *drv)
{
    tm_cosq_chip_driver_cb[unit] = drv;

    return SHR_E_NONE;
}

int
tm_ltsw_cosq_chip_driver_deregister(int unit)
{
    tm_cosq_chip_driver_cb[unit] = NULL;

    return SHR_E_NONE;
}

int
tm_ltsw_cosq_chip_driver_get(int unit, tm_ltsw_cosq_chip_driver_t **drv)
{
    *drv = tm_cosq_chip_driver_cb[unit];

    return SHR_E_NONE;
}

int
tm_ltsw_cosq_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_wred_drop_curve_profile_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_wred_time_profile_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_port_ct_mode_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_schedule_default_clear(unit));

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_wred_default_clear(unit));

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_port_ct_mode_clear(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_hierarchy_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_hierarchy_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_schedule_profile_queue_info_get(
    int unit,
    int profile_id,
    int node_id,
    tm_ltsw_cosq_scheduler_profile_q_info_t *profile)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_schedule_profile_queue_info_get(unit, profile_id, node_id,
                                                 profile));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_schedq_mapping_set(
    int unit,
    int profile_index,
    int size,
    bcm_cosq_mapping_t *cosq_mapping_arr)
{
    cosq_scheduler_profile_t sched_profile[MAX_TM_SCHEDULER_NODE];
    bcmi_ltsw_cosq_device_info_t device_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    if ((profile_index < 0 || profile_index >= device_info.num_sched_profile) ||
        (cosq_mapping_arr == NULL) ||
        (size < 0 || size > MAX_TM_SCHEDULER_NODE)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(sched_profile, 0,
              (sizeof(cosq_scheduler_profile_t) * MAX_TM_SCHEDULER_NODE));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_scheduler_profile_set(unit, profile_index, sched_profile,
                                    size, cosq_mapping_arr));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_hierarchy_setup(unit, profile_index));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_schedq_mapping_get(
    int unit,
    int profile_index,
    int array_max,
    bcm_cosq_mapping_t *cosq_mapping_arr,
    int *size)
{
    cosq_scheduler_profile_t sched_profile[MAX_TM_SCHEDULER_NODE];
    bcmi_ltsw_cosq_device_info_t device_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    if ((profile_index < 0 || profile_index >= device_info.num_sched_profile) ||
        (array_max < 0 || array_max > MAX_TM_SCHEDULER_NODE) ||
        (cosq_mapping_arr == NULL) ||
        (size == NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_scheduler_profile_get(unit, profile_index, array_max,
                                    sched_profile, size));

    cosq_mapping_arr_set(unit, sched_profile, cosq_mapping_arr, *size);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_port_scheduler_profile_set(
    int unit,
    bcm_port_t port,
    int profile_id)
{
    cosq_scheduler_profile_t sched_profile[MAX_TM_SCHEDULER_NODE];
    int rv = SHR_E_NONE;
    bcmi_ltsw_cosq_device_info_t device_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    if (profile_id < 0 || profile_id >= device_info.num_sched_profile) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    rv = cosq_scheduler_profile_get(unit, profile_id, MAX_TM_SCHEDULER_NODE, sched_profile,
                                    NULL);

    if (rv == SHR_E_FAIL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_schedule_port_profile_set(unit, port, profile_id, -1));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_port_schedule_node_set(unit, port, profile_id));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_port_scheduler_profile_get(
    int unit,
    bcm_port_t port,
    int *profile_id)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (profile_id == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    rv = cosq_tm_schedule_port_profile_get(unit, port, profile_id, NULL);

    if (rv == SHR_E_NOT_FOUND) {
        *profile_id = 0;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_cpu_parent_set(
    int unit,
    int child_index,
    int child_level,
    int parent_index)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_cpu_parent_set(unit, child_index, child_level, parent_index));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_cpu_parent_get(
    int unit,
    int child_index,
    int child_level,
    int *parent_index)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_cpu_parent_get(unit, child_index, child_level, parent_index));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_rx_queue_channel_get(
    int unit,
    int input_cos,
    int channel_id,
    bool *enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_rx_queue_get(unit, channel_id, input_cos, enable));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_rx_queue_channel_set(
    int unit,
    int input_cos,
    int channel_id,
    bool enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_rx_queue_channel_set(unit, input_cos, channel_id, enable));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_sched_set(
    int unit,
    int lport,
    int node_id,
    int node_type,
    int sched_mode,
    int weight)
{
    SHR_FUNC_ENTER(unit);

    if (is_cpu_port(unit, lport)) {
        if (sched_mode == LTSW_SCHED_MODE_WRR) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_tm_cpu_schedule_set(unit, 1));

        } else if (sched_mode == LTSW_SCHED_MODE_WERR) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_tm_cpu_schedule_set(unit, 0));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_cpu_node_mode_set(unit, node_id, node_type,
                                       sched_mode, weight));
    } else {
        if (sched_mode == LTSW_SCHED_MODE_WRR) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_tm_schedule_port_profile_set(unit, lport, -1, 1));

        } else if (sched_mode == LTSW_SCHED_MODE_WERR) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_tm_schedule_port_profile_set(unit, lport, -1, 0));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_schedule_node_set(unit, lport, node_id, node_type,
                                       sched_mode, weight));
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_sched_get(
    int unit,
    int lport,
    int node_id,
    int node_type,
    int *sched_mode,
    int *weight)
{
    int rv = SHR_E_NONE;
    int wrr = 0;
    int lwts = 0;

    SHR_FUNC_ENTER(unit);

    if (is_cpu_port(unit, lport)) {
        rv = cosq_tm_cpu_node_mode_get(unit, node_id, node_type,
                                       sched_mode, &lwts);
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_cpu_schedule_get(unit, &wrr));
    } else {
        rv = cosq_tm_schedule_node_get(unit, lport, node_id, node_type,
                                       sched_mode, &lwts, false);
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (cosq_tm_schedule_port_profile_get(unit, lport, NULL, &wrr),
             SHR_E_NOT_FOUND);
    }

    if (rv == SHR_E_NOT_FOUND) {
        *sched_mode = 0;
        *weight = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (*sched_mode == LTSW_SCHED_MODE_WRR){
        *sched_mode = wrr ? LTSW_SCHED_MODE_WRR : LTSW_SCHED_MODE_WERR;
    }

    *weight = lwts;

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_wred_set(
    int unit,
    bcm_port_t port,
    int cosq,
    tm_ltsw_cosq_wred_config_t wred_config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_wred_set(unit, port, cosq, wred_config));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_wred_get(
    int unit,
    bcm_port_t port,
    int cosq,
    tm_ltsw_cosq_wred_config_t *wred_config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_wred_get(unit, port, cosq, wred_config));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_wred_cng_profile_set(
    int unit,
    int profile_idx,
    tm_ltsw_cosq_wred_cng_profile_t profile)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_wred_cng_profile_set(unit, profile_idx, profile));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_wred_cng_profile_get(
    int unit,
    int profile_idx,
    tm_ltsw_cosq_wred_cng_profile_t *profile)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_wred_cng_profile_get(unit, profile_idx, profile));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_wred_cng_profile_reset(
    int unit,
    int profile_idx)
{
    tm_ltsw_cosq_wred_cng_profile_t profile;
    int idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(&profile, 0, sizeof(tm_ltsw_cosq_wred_cng_profile_t));

    for (idx = 1; idx < COSQ_WRED_CNG_ENTRY_PER_PROFILE; idx++) {
        profile.q_avg = (idx & 0x1) >> COSQ_WRED_CNG_Q_AVG_OFFSET;

        profile.q_min = (idx & (1 << COSQ_WRED_CNG_Q_MIN_OFFSET)) >>
                         COSQ_WRED_CNG_Q_MIN_OFFSET;

        profile.sp = idx >> COSQ_WRED_CNG_SERVICE_POOL_OFFSET;

        if (profile.q_avg) {
            /* Default value for action is 1 when Q_AVG is enabled. */
            profile.action = 1;
        } else {
            profile.action = 0;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_wred_cng_profile_set(unit, profile_idx, profile));
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_time_domain_dump(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_time_domain_dump(unit));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_cos_map_profile_add(
    int unit,
    tm_ltsw_cosq_cos_map_profile_t *cos_map_entries,
    int *profile_idx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_cos_map_profile_add(unit, cos_map_entries, profile_idx));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_cos_map_profile_get(
    int unit,
    int profile_idx,
    tm_ltsw_cosq_cos_map_profile_t *cos_map_entries)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_cos_map_profile_get(unit, profile_idx, cos_map_entries));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_cos_map_profile_delete(
    int unit,
    int profile_idx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_cos_map_profile_delete(unit, profile_idx));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_cos_map_profile_init(
    int unit,
    int numq)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_cos_map_profile_init(unit, numq));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_shaper_config_get(
    int unit,
    int *refresh_time,
    int *itu_mode,
    int *shaper)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_shaper_config_get(unit, itu_mode, shaper));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_shaper_node_set(
    int unit,
    bcm_port_t port,
    int node_id,
    int node_type,
    int shaper_mode,
    uint32_t kbits_sec_min,
    uint32_t kbits_sec_max,
    bool burst_size_auto,
    uint32_t burst_min,
    uint32_t burst_max)
{
    SHR_FUNC_ENTER(unit);

    if (is_cpu_port(unit, port)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_cpu_shaper_node_set(unit, node_type, node_id,
                                         shaper_mode, kbits_sec_min, kbits_sec_max,
                                         burst_size_auto, burst_min, burst_max));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_shaper_node_set(unit, port, node_id, node_type,
                                     shaper_mode, kbits_sec_min, kbits_sec_max,
                                     burst_size_auto, burst_min, burst_max));
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_shaper_node_get(
    int unit,
    bcm_port_t port,
    int node_id,
    int node_type,
    int *shaper_mode,
    uint32_t *kbits_sec_min,
    uint32_t *kbits_sec_max,
    bool burst_size_auto,
    uint32_t *burst_min,
    uint32_t *burst_max)
{
    SHR_FUNC_ENTER(unit);

    if (is_cpu_port(unit, port)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_cpu_shaper_node_get(unit, node_type, node_id, shaper_mode,
                                         kbits_sec_min, kbits_sec_max,
                                         burst_size_auto, burst_min, burst_max));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_shaper_node_get(unit, port, node_id, node_type, shaper_mode,
                                     kbits_sec_min, kbits_sec_max,
                                     burst_size_auto, burst_min, burst_max, false));
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_ifp_cos_map_profile_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ifp_cos_map_profile_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_field_classifier_id_create(
    int unit,
    int *classifier_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_field_classifier_id_create(unit, classifier_id));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_field_classifier_id_destroy(
    int unit,
    int classifier_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_field_classifier_id_destroy(unit, classifier_id));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_ifp_cos_map_profile_add(
    int unit,
    tm_ltsw_cosq_cos_map_profile_t *cos_map_entries,
    int *profile_idx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ifp_cos_map_profile_add(unit, cos_map_entries, profile_idx));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_ifp_cos_map_profile_get(
    int unit,
    int profile_idx,
    tm_ltsw_cosq_cos_map_profile_t *cos_map_entries)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ifp_cos_map_profile_get(unit, profile_idx, cos_map_entries));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_ifp_cos_map_profile_delete(
    int unit,
    int profile_idx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ifp_cos_map_profile_delete(unit, profile_idx));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_field_classifier_map_clear(
    int unit,
    int classifier_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_field_classifier_map_clear(unit, classifier_id));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_field_hi_classifier_id_create(
    int unit,
    int *classifier_id)
{
    int i, rv;
    bool found = FALSE;
    tm_ltsw_cosq_cos_map_profile_t profile;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    if (NULL == classifier_id) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, FP_ING_COS_Q_MAPs,
                                       FP_ING_COS_Q_MAP_IDs,
                                       &idx_min, &idx_max));

    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;

    for (i = ent_idx_min; i <= ent_idx_max; i++) {
        rv = cosq_ifp_cos_map_hi_profile_get(unit, i, &profile);
        if ((SHR_E_NOT_FOUND == rv) && (0 != i % 16)) {
            found = TRUE;
            break;
        }
    }

    if (i > ent_idx_max && !found) {
        *classifier_id = 0;
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ifp_cos_map_hi_profile_add(unit, i));

    BCMINT_COSQ_CLASSIFIER_FIELD_HI_SET(*classifier_id, i);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_field_hi_classifier_id_destroy(
    int unit,
    int classifier_id)
{
    int profile_id;
    SHR_FUNC_ENTER(unit);

    /* Get profile table entry set base index. */
    profile_id = BCMINT_COSQ_CLASSIFIER_FIELD_HI_GET(classifier_id);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ifp_cos_map_hi_profile_delete(unit, profile_id));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_ifp_cos_map_hi_profile_set(
    int unit,
    int profile_idx,
    tm_ltsw_cosq_cos_map_profile_t *cos_map_entries)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ifp_cos_map_hi_profile_set(unit, profile_idx, cos_map_entries));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_ifp_cos_map_hi_profile_get(
    int unit,
    int profile_idx,
    tm_ltsw_cosq_cos_map_profile_t *cos_map_entries)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ifp_cos_map_hi_profile_get(unit, profile_idx, cos_map_entries));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_ifp_cos_map_hi_clear(
    int unit)
{
    (void)bcmi_lt_clear(unit, FP_ING_COS_Q_MAPs);

    return SHR_E_NONE;
}

int
tm_ltsw_cosq_mcq_mode_get(
    int unit,
    int *mc_q_mode)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_mcq_mode_get(unit, mc_q_mode));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_port_map_info_get(
    int unit,
    bcm_port_t port,
    bcmi_ltsw_cosq_port_map_info_t *info)
{
    int rv = SHR_E_NONE;
    int num_ucq, num_mcq;
    bcmi_ltsw_cosq_device_info_t device_info = {0};

    SHR_FUNC_ENTER(unit);

    rv = cosq_tm_port_map_info_get(unit, port, info);

    if (rv == SHR_E_NOT_FOUND) {
        if (bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_CPU)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
            info->num_uc_q = 0;
            info->num_mc_q = device_info.num_cpu_queue;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_num_queue_get(unit, &num_ucq, &num_mcq));
            info->num_uc_q = num_ucq;
            info->num_mc_q = num_mcq;
        }

        LOG_DEBUG(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "port %d is not available!\n"), port));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_device_info_get(
    int unit,
    bcmi_ltsw_cosq_device_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_device_info_get(unit, info));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_port_ct_mode_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_port_ct_mode_set(unit, port, enable, 1));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_port_ct_mode_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_port_ct_mode_get(unit, port, enable));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_tm_cut_through_port_info_get(
    int unit,
    int lport,
    tm_ltsw_cut_through_port_info_t *info)
{
    bcmi_lt_entry_t lt_entry;
    tm_ltsw_cosq_chip_driver_t *drv = NULL;
    int i;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,               BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {SRC_PORT_MAX_SPEEDs,    BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 2 */ {CUT_THROUGH_CLASSs,     BCMI_LT_FIELD_F_GET |
                                          BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
         /* 3 */ {MAX_CREDIT_CELLSs,      BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 4 */ {EGR_XMIT_START_COUNT_BYTESs, BCMI_LT_FIELD_F_GET |
                                               BCMI_LT_FIELD_F_ARRAY, 0, {0}},
         /* 5 */ {EGR_XMIT_START_COUNT_BYTESs, BCMI_LT_FIELD_F_GET |
                                               BCMI_LT_FIELD_F_ARRAY, 1, {0}},
         /* 6 */ {EGR_XMIT_START_COUNT_BYTESs, BCMI_LT_FIELD_F_GET |
                                               BCMI_LT_FIELD_F_ARRAY, 2, {0}},
         /* 7 */ {EGR_XMIT_START_COUNT_BYTESs, BCMI_LT_FIELD_F_GET |
                                               BCMI_LT_FIELD_F_ARRAY, 3, {0}},
         /* 8 */ {EGR_XMIT_START_COUNT_BYTESs, BCMI_LT_FIELD_F_GET |
                                               BCMI_LT_FIELD_F_ARRAY, 4, {0}},
         /* 9 */ {EGR_XMIT_START_COUNT_BYTESs, BCMI_LT_FIELD_F_GET |
                                               BCMI_LT_FIELD_F_ARRAY, 5, {0}},
         /* 10 */ {EGR_XMIT_START_COUNT_BYTESs, BCMI_LT_FIELD_F_GET |
                                               BCMI_LT_FIELD_F_ARRAY, 6, {0}},
         /* 11 */ {EGR_XMIT_START_COUNT_BYTESs, BCMI_LT_FIELD_F_GET |
                                               BCMI_LT_FIELD_F_ARRAY, 7, {0}},
         /* 12 */ {FIFO_THD_CELLSs,        BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_cosq_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->port_ct_mode_to_class, SHR_E_INIT);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = lport;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_CUT_THROUGH_PORT_INFOs,
                           &lt_entry, NULL, NULL));

    info->src_port_max_speed = fields[1].u.val;
    info->cut_through_class = drv->port_ct_mode_to_class(fields[2].u.sym_val);
    info->max_credit_cells = fields[3].u.val;

    for (i = 0; i < 8; i++) {
        info->egr_xmit_start_count_bytes[i] = fields[i+4].u.val;
    }

    info->fifo_thd_cells = fields[12].u.val;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Init PORT_COS_Q_STRENGTH_PROFILE table.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
tm_ltsw_port_cosq_strength_profile_init(
    int unit,
    tm_ltsw_cosq_strength_profile_t *profile)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {PORT_COS_Q_STRENGTH_PROFILE_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/ {UC_COS_STRENGTHs,                BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2*/ {MC_COS_STRENGTHs,                BCMI_LT_FIELD_F_SET, 0, {0}},
        /*3*/ {RQE_COS_STRENGTHs,               BCMI_LT_FIELD_F_SET, 0, {0}},
        /*4*/ {CPU_COS_STRENGTHs,               BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    field[0].u.val = profile->strength_profile_id;
    field[1].u.val = profile->uc_cos_strength;
    field[2].u.val = profile->mc_cos_strength;
    field[3].u.val = profile->rqe_cos_strength;
    field[4].u.val = profile->cpu_cos_strength;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, PORT_COS_Q_STRENGTH_PROFILEs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Init FP_ING_COS_Q_STRENGTH_PROFILE table.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
tm_ltsw_ifp_cosq_strength_profile_init(
    int unit,
    tm_ltsw_cosq_strength_profile_t *profile)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {FP_ING_COS_Q_STRENGTH_PROFILE_IDs,
                                                BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/ {UC_COS_STRENGTHs,                BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2*/ {MC_COS_STRENGTHs,                BCMI_LT_FIELD_F_SET, 0, {0}},
        /*3*/ {RQE_COS_STRENGTHs,               BCMI_LT_FIELD_F_SET, 0, {0}},
        /*4*/ {CPU_COS_STRENGTHs,               BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    field[0].u.val = profile->strength_profile_id;
    field[1].u.val = profile->uc_cos_strength;
    field[2].u.val = profile->mc_cos_strength;
    field[3].u.val = profile->rqe_cos_strength;
    field[4].u.val = profile->cpu_cos_strength;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, FP_ING_COS_Q_STRENGTH_PROFILEs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Init FP_DESTINATION_COS_Q_STRENGTH_PROFILE table.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
tm_ltsw_destination_cosq_strength_profile_init(
    int unit,
    tm_ltsw_cosq_strength_profile_t *profile)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {FP_DESTINATION_COS_Q_STRENGTH_PROFILE_IDs,
                                                BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/ {UC_COS_STRENGTHs,                BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2*/ {MC_COS_STRENGTHs,                BCMI_LT_FIELD_F_SET, 0, {0}},
        /*3*/ {RQE_COS_STRENGTHs,               BCMI_LT_FIELD_F_SET, 0, {0}},
        /*4*/ {CPU_COS_STRENGTHs,               BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    field[0].u.val = profile->strength_profile_id;
    field[1].u.val = profile->uc_cos_strength;
    field[2].u.val = profile->mc_cos_strength;
    field[3].u.val = profile->rqe_cos_strength;
    field[4].u.val = profile->cpu_cos_strength;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, FP_DESTINATION_COS_Q_STRENGTH_PROFILEs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Init TM_COS_Q_CPU_STRENGTH_PROFILE table.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
tm_ltsw_cpu_cosq_strength_profile_init(
    int unit,
    tm_ltsw_cosq_strength_profile_t *profile)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {TM_COS_Q_CPU_STRENGTH_PROFILE_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/ {REPL_Q_NUM_STRENGTHs,              BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2*/ {CPU_COS_STRENGTHs,                 BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    field[0].u.val = profile->strength_profile_id;
    field[1].u.val = profile->rqe_cos_strength;
    field[2].u.val = profile->cpu_cos_strength;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_COS_Q_CPU_STRENGTH_PROFILEs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Init TM_Q_ASSIGNMENT_PROFILE table.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
tm_ltsw_queue_assignment_profile_init(
    int unit,
    tm_ltsw_q_assignment_profile_t *profile)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {TM_Q_ASSIGNMENT_PROFILE_IDs,    BCMI_LT_FIELD_F_SET |
                                               BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*1*/ {BASE_PORT_COS_Q_MAP_IDs,        BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2*/ {BASE_FP_ING_COS_Q_MAP_IDs,      BCMI_LT_FIELD_F_SET, 0, {0}},
        /*3*/ {BASE_FP_DESTINATION_COS_Q_MAP_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*4*/ {PORT_COS_Q_STRENGTH_PROFILE_IDs,   BCMI_LT_FIELD_F_SET, 0, {0}},
        /*5*/ {FP_ING_COS_Q_STRENGTH_PROFILE_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*6*/ {TM_COS_Q_CPU_STRENGTH_PROFILE_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*7*/ {FP_DESTINATION_COS_Q_STRENGTH_PROFILE_IDs,
                                                  BCMI_LT_FIELD_F_SET, 0, {0}},
        /*8*/ {MC_COS_MIRRORs,                    BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    field[0].u.sym_val = profile->tm_q_assignment_profile_id;
    field[1].u.val = profile->base_port_cos_q_map_id;
    field[2].u.val = profile->base_fp_ing_cos_q_map_id;
    field[3].u.val = profile->base_fp_destination_cos_q_map_id;
    field[4].u.val = profile->port_cos_q_strength_profile_id;
    field[5].u.val = profile->fp_ing_cos_q_strength_profile_id;
    field[6].u.val = profile->tm_cos_q_cpu_strength_profile_id;
    field[7].u.val = profile->fp_destination_cos_q_strength_profile_id;
    field[8].u.val = profile->mc_cos_mirror;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_Q_ASSIGNMENT_PROFILEs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief tm_ltsw_mc_cos_mirror_set.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
tm_ltsw_mc_cos_mirror_set(
    int unit,
    tm_ltsw_q_assignment_profile_t *profile)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {TM_Q_ASSIGNMENT_PROFILE_IDs,    BCMI_LT_FIELD_F_SET |
                                               BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*1*/ {MC_COS_MIRRORs,                 BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    field[0].u.sym_val = profile->tm_q_assignment_profile_id;
    field[1].u.val = profile->mc_cos_mirror;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_Q_ASSIGNMENT_PROFILEs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief tm_ltsw_mc_cos_mirror_get.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
tm_ltsw_mc_cos_mirror_get(
    int unit,
    tm_ltsw_q_assignment_profile_t *profile)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {TM_Q_ASSIGNMENT_PROFILE_IDs,    BCMI_LT_FIELD_F_SET |
                                               BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*1*/ {MC_COS_MIRRORs,                 BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    field[0].u.sym_val = profile->tm_q_assignment_profile_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_Q_ASSIGNMENT_PROFILEs,
                           &lt_entry, NULL, NULL));

    profile->mc_cos_mirror = field[1].u.val;

exit:
    SHR_FUNC_EXIT();
}

