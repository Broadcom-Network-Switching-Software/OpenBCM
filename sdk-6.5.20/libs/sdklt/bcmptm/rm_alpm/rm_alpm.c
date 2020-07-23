/*! \file rm_alpm.c
 *
 * Resource Manager for ALPM
 *
 * This file contains the resource manager for ALPM
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <shr/shr_util.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_cth_alpm_internal.h>
#include <bcmptm/bcmptm_rm_alpm_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmissu/issu_api.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmptm/generated/bcmptm_rm_alpm_ha.h>
#include <bcmcfg/comp/bcmcfg_feature_ctl.h>

#include "rm_alpm.h"
#include "rm_alpm_util.h"
#include "rm_alpm_device.h"
#include "rm_alpm_level1.h"
#include "rm_alpm_leveln.h"
#include "rm_alpm_bucket.h"
#include "rm_alpm_traverse.h"
#include "rm_alpm_ts.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE              BSL_LS_BCMPTM_RMALPM

#define RM_ALPM_CALL_EXEC(u, m, lvls, fn)  alpm_call[u][m][(lvls) - 1].fn

/*! ALPM Cmdproc control fields */
#define ALPM_CMD_CTRL(u, m)        (alpm_info[u][m]->cmd_ctrl)
#define WRITE_HW(u, m)             (ALPM_CMD_CTRL(u, m).write_hw)
#define WRITE_CACHE(u, m)          (ALPM_CMD_CTRL(u, m).write_cache)
#define SET_CACHE_VBIT(u, m)       (ALPM_CMD_CTRL(u, m).set_cache_vbit)
#define REQ_LTID(u, m)             (ALPM_CMD_CTRL(u, m).req_ltid)
#define REQ_FORMAT_ID(u, m)        (ALPM_CMD_CTRL(u, m).req_format_id)
#define REQ_FLAGS(u, m)            (ALPM_CMD_CTRL(u, m).req_flags)
#define RSP_ENTRY_WSIZE(u, m)      (ALPM_CMD_CTRL(u, m).rsp_entry_wsize)
#define RSP_ENTRY_CACHE_VBIT(u, m) (ALPM_CMD_CTRL(u, m).rsp_entry_cache_vbit)
#define RSP_LTID(u, m)             (ALPM_CMD_CTRL(u, m).rsp_ltid)
#define RSP_DFIELD_FORMAT_ID(u, m) (ALPM_CMD_CTRL(u, m).rsp_dfield_format_id)
#define RSP_FLAGS(u, m)            (ALPM_CMD_CTRL(u, m).rsp_flags)
#define INST(u, m)                 (ALPM_CMD_CTRL(u, m).inst)
#define CSEQ_ID(u, m)              (ALPM_CMD_CTRL(u, m).cseq_id)

/*! ALPM INFO fields */
#define ALPM_INFO(u, m)            (alpm_info[u][m])
#define ALPM_CONFIG(u, m)          (alpm_info[u][m]->config)
#define ALPM_MODE(u, m)            (alpm_info[u][m]->config.core.alpm_mode)

#define ALPM_HA(u, m)              (alpm_ha[u][m])
#define ALPM_HA_TRANS_STATE(u, m)  (alpm_ha[u][m]->trans_state)
#define ALPM_HA_TRANS_ID(u, m)     (alpm_ha[u][m]->trans_id)

#define ARG_ALLOC_CHUNK_SIZE 4

#ifdef ALPM_DEBUG
static uint32_t alpm_sanity_step = 1;
static uint32_t alpm_sanity_start = 0xFFFFFFFF;
#endif
/*******************************************************************************
 * Typedefs
 */


typedef struct rm_alpm_calls_s {
     int (*insert) (int u, int m, alpm_arg_t *arg);
     int (*match)  (int u, int m, alpm_arg_t *arg);
     int (*find)   (int u, int m, alpm_arg_t *arg);
     int (*delete) (int u, int m, alpm_arg_t *arg);
} rm_alpm_calls_t;


/*******************************************************************************
 * Private variables
 */
static alpm_info_t *alpm_info[BCMDRD_CONFIG_MAX_UNITS][ALPMS];

static rm_alpm_calls_t alpm_call[BCMDRD_CONFIG_MAX_UNITS][ALPMS][MAX_LEVELS];

extern alpm_db_t *bcmptm_rm_alpm_database[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
extern int bcmptm_rm_alpm_database_cnt[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static bcmptm_rm_alpm_ha_t *alpm_ha[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static bool alpm_supported[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static bool alpm_atomic_trans[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static bool dynamic_alpm_atomic_trans[BCMDRD_CONFIG_MAX_UNITS][ALPMS];

static alpm_arg_t static_arg[BCMDRD_CONFIG_MAX_UNITS];
static bcmltd_sid_t all_alpm_lts[BCMDRD_CONFIG_MAX_UNITS][ALPMS][MAX_ALPM_LTS];
static int all_alpm_lts_count[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static bcmptm_rm_alpm_more_info_t
    all_alpm_lts_info[BCMDRD_CONFIG_MAX_UNITS][ALPMS][MAX_ALPM_LTS];
static const bcmptm_alpm_variant_info_t
*bcmptm_rm_alpm_variant_info[BCMDRD_CONFIG_MAX_UNITS][ALPMS];

/*
 * This is for local usage, i.e., what's allcoated will be freed immediately.
 * Thus not necessary to make it per unit.
 */
shr_lmm_hdl_t bcmptm_rm_alpm_arg_lmem[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
/*******************************************************************************
 * Private Functions
 */

static const char *
req_op_name(int u, int op)
{
    char *name = NULL;

    switch (op) {
    case BCMPTM_OP_NOP:
        name = "Nop";
        break;
    case BCMPTM_OP_LOOKUP:
        name = "LKUP";
        break;
    case BCMPTM_OP_INSERT:
        name = "INST";
        break;
    case BCMPTM_OP_DELETE:
        name = "DELE";
        break;
    case BCMPTM_OP_GET_FIRST:
        name = "FIRST";
        break;
    case BCMPTM_OP_GET_NEXT:
        name = "NEXT";
        break;
    case BCMPTM_OP_WRITE:
        name = "WR";
        break;
    case BCMPTM_OP_READ:
        name = "RD";
        break;
    case BCMPTM_OP_GET_TABLE_INFO:
        name = "TBL_INFO";
        break;
    default:
        name = "Unknown";
        break;
    }
    return name;
}

static int rm_alpm_init(int u, int m, alpm_config_t *config, bool recover,
                        uint8_t boot_type);
static int rm_alpm_cleanup(int u, int m, bool warm);

/*
 * The following sets of routines - single-level & multi-level
 * are ok to co-exist. They don't have to be mutually
 * exclusive for a single run.
 */

/*!
 * \brief ALPM insert operation in single-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
static int
rm_alpm_single_level_insert(int u, int m, alpm_arg_t *arg)
{
    arg->a1.direct_route = 1;
    return bcmptm_rm_alpm_l1_insert(u, m, arg);
}

/*!
 * \brief ALPM match operation in single-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
static int
rm_alpm_single_level_match(int u, int m, alpm_arg_t *arg)
{
    arg->a1.direct_route = 1;
    return bcmptm_rm_alpm_l1_match(u, m, arg);
}

/*!
 * \brief ALPM delete operation in single-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
static int
rm_alpm_single_level_delete(int u, int m, alpm_arg_t *arg)
{
    arg->a1.direct_route = 1;
    return bcmptm_rm_alpm_l1_delete(u, m, arg);
}

/*!
 * \brief ALPM find operation in single-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
static int
rm_alpm_single_level_find(int u, int m, alpm_arg_t *arg)
{
    arg->a1.direct_route = 1;
    return bcmptm_rm_alpm_l1_find(u, m, arg);
}

/*!
 * \brief Check if a given route must fall back to single-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return True/False
 */
static int
rm_alpm_multi_levels_fall_back(int u, int m, alpm_arg_t *arg)
{
    if (arg->key.vt == VRF_OVERRIDE) {
        arg->a1.direct_route = 1;
        return true;
    }
    return false;
}


/*!
 * \brief ALPM insert operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
static int
rm_alpm_multi_levels_insert(int u, int m, alpm_arg_t *arg)
{

    if (rm_alpm_multi_levels_fall_back(u, m, arg)) {
        return bcmptm_rm_alpm_l1_insert(u, m, arg);
    } else {
        return bcmptm_rm_alpm_ln_insert(u, m, arg);
    }
}

/*!
 * \brief ALPM match operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
static int
rm_alpm_multi_levels_match(int u, int m, alpm_arg_t *arg)
{
    if (rm_alpm_multi_levels_fall_back(u, m, arg)) {
        return bcmptm_rm_alpm_l1_match(u, m, arg);
    } else {
        return bcmptm_rm_alpm_ln_match(u, m, arg);
    }
}

/*!
 * \brief ALPM delete operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
static int
rm_alpm_multi_levels_delete(int u, int m, alpm_arg_t *arg)
{
    if (rm_alpm_multi_levels_fall_back(u, m, arg)) {
        return bcmptm_rm_alpm_l1_delete(u, m, arg);
    } else {
        return bcmptm_rm_alpm_ln_delete(u, m, arg);
    }
}

/*!
 * \brief ALPM find operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
static int
rm_alpm_multi_levels_find(int u, int m, alpm_arg_t *arg)
{
    if (rm_alpm_multi_levels_fall_back(u, m, arg)) {
        return bcmptm_rm_alpm_l1_find(u, m, arg);
    } else {
        return bcmptm_rm_alpm_ln_find(u, m, arg);
    }
}

static int
rm_alpm_call_init(int u, int m)
{
    alpm_call[u][m][LEVEL1].insert   = rm_alpm_single_level_insert;
    alpm_call[u][m][LEVEL1].match    = rm_alpm_single_level_match;
    alpm_call[u][m][LEVEL1].delete   = rm_alpm_single_level_delete;
    alpm_call[u][m][LEVEL1].find     = rm_alpm_single_level_find;

    alpm_call[u][m][LEVEL2].insert   = rm_alpm_multi_levels_insert;
    alpm_call[u][m][LEVEL2].match    = rm_alpm_multi_levels_match;
    alpm_call[u][m][LEVEL2].delete   = rm_alpm_multi_levels_delete;
    alpm_call[u][m][LEVEL2].find     = rm_alpm_multi_levels_find;

    alpm_call[u][m][LEVEL3].insert   = rm_alpm_multi_levels_insert;
    alpm_call[u][m][LEVEL3].match    = rm_alpm_multi_levels_match;
    alpm_call[u][m][LEVEL3].delete   = rm_alpm_multi_levels_delete;
    alpm_call[u][m][LEVEL3].find     = rm_alpm_multi_levels_find;
    return SHR_E_NONE;
}

/*!
 * \brief Map alpm control to alpm config.
 *
 * \param [in] u Unit number.
 * \param [in] ctrl ALPM control.
 * \param [out] config ALPM config.
 *
 * \retval nothing.
 */
static void
rm_alpm_config_map(int u, int m,
                   bcmptm_cth_alpm_control_t *ctrl,
                   alpm_config_t *config)
{
    uint8_t i;
    uint8_t ver0_blocks = 4;

    sal_memset(config, 0, sizeof(*config));
    config->core.alpm_dbs = ctrl->alpm_dbs;
    config->core.alpm_mode = ctrl->alpm_mode;
    config->core.db_levels[DB0] = ctrl->db0_levels;
    config->core.db_levels[DB1] = ctrl->db1_levels;
    config->core.db_levels[DB2] = ctrl->db2_levels;
    config->core.db_levels[DB3] = ctrl->db3_levels;
    config->hit_support = ctrl->hit_support;
    config->core.large_vrf = ctrl->large_vrf;
    config->core.comp_key_type = ctrl->comp_key_type;

    sal_memcpy(config->core.l1_key_input, ctrl->l1_key_input,
               sizeof(ctrl->l1_key_input));
    sal_memcpy(config->core.l1_phy_key_input, ctrl->l1_phy_key_input,
               sizeof(ctrl->l1_phy_key_input));
    sal_memcpy(config->core.l1_db, ctrl->l1_db,
               sizeof(ctrl->l1_db));
    sal_memcpy(config->core.l1_phy_db, ctrl->l1_phy_db,
               sizeof(ctrl->l1_phy_db));
    sal_memcpy(config->core.l1_blocks_l2p, ctrl->l1_blocks_l2p,
               sizeof(ctrl->l1_blocks_l2p));
    sal_memcpy(config->core.l1_blocks_p2l, ctrl->l1_blocks_p2l,
               sizeof(ctrl->l1_blocks_p2l));

    config->core.l1_blocks_map_en = ctrl->l1_blocks_map_en;
    config->core.db0 = ctrl->db0;
    config->core.db1 = ctrl->db1;
    config->core.db2 = ctrl->db2;
    config->core.db3 = ctrl->db3;
    config->core.tot = ctrl->tot;

    config->ver0_128_enable = false;
    for (i = 0; i < ver0_blocks; i++) {
        if (config->core.l1_key_input[i] == ALPM_KEY_INPUT_LPM_DST_Q) {
            config->ver0_128_enable = true;
            break;
        }
    }

    config->ipv4_uc_sbr = ctrl->ipv4_uc_sbr;
    config->ipv4_uc_vrf_sbr = ctrl->ipv4_uc_vrf_sbr;
    config->ipv4_uc_override_sbr = ctrl->ipv4_uc_override_sbr;
    config->ipv6_uc_sbr = ctrl->ipv6_uc_sbr;
    config->ipv6_uc_vrf_sbr = ctrl->ipv6_uc_vrf_sbr;
    config->ipv6_uc_override_sbr = ctrl->ipv6_uc_override_sbr;
    config->ipv4_comp_sbr = ctrl->ipv4_comp_sbr;
    config->ipv6_comp_sbr = ctrl->ipv6_comp_sbr;
    config->destination = ctrl->destination;
    config->destination_mask = ctrl->destination_mask;
    config->destination_type_match = ctrl->destination_type_match;
    config->destination_type_non_match = ctrl->destination_type_non_match;
}

/*!
 * \brief Get current alpm configs.
 *
 * \param [in] u Unit number.
 * \param [out] config ALPM config.
 *
 * \retval nothing.
 */
static int
rm_alpm_config_get(int u, int m, alpm_config_t *config)
{
    bcmptm_cth_alpm_control_t ctrl;

    SHR_FUNC_ENTER(u);
    SHR_IF_ERR_EXIT(
        bcmptm_cth_alpm_control_get(u, m, &ctrl));
    rm_alpm_config_map(u, m, &ctrl, config);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize alpm configs.
 *
 * If config=null reset to default, otherwise, override with it.
 *
 * \param [in] u Unit number.
 * \param [in] config ALPM config.
 *
 * \retval SHR_E_NONE, otherwise failure.
 */
static int
rm_alpm_config_init(int u, int m, alpm_config_t *config)
{

    SHR_FUNC_ENTER(u);
    /*
     * If config=null reset to default, this is cold boot case.
     * Otherwise, override with input config, this is warm boot case
     * or resize case due to LT changes.
     */
    if (config == NULL) {
        SHR_IF_ERR_EXIT(
            rm_alpm_config_get(u, m, &ALPM_CONFIG(u, m)));
    } else { /* Else override */
        ALPM_CONFIG(u, m) = *config;
    }
exit:
    SHR_FUNC_EXIT();
}

static void
handle_alpm_control_event(int u, int m, const char *event, uint64_t ev_data)
{
    alpm_config_t config;
    bcmptm_cth_alpm_control_t *ctrl;
    uint32_t entry_in_use_cnt = 0;
    bcmltd_sid_t ltid;
    int i;

    ctrl = (bcmptm_cth_alpm_control_t *)(unsigned long) ev_data;
    /* Abort if others subscribing the same event fails */
    if (!ctrl || SHR_FAILURE(ctrl->rv)) {
        return;
    }
    rm_alpm_config_map(u, m, ctrl, &config);

    /* Just return if no changes */
    if (sal_memcmp(&config, &ALPM_CONFIG(u, m), sizeof(config)) == 0) {
        return;
    }

    if (sal_memcmp(&config.core, &alpm_info[u][m]->config.core,
                   sizeof(config.core)) != 0) {
        for (i = 0; i < all_alpm_lts_count[u][m]; i++) {
            ltid = all_alpm_lts[u][m][i];
            (void) bcmlrd_table_inuse_count_get(u, 0, ltid, &entry_in_use_cnt);
            if (entry_in_use_cnt != 0) {
                ctrl->rv = SHR_E_BUSY;
                return;
            }
        }

        /* Brutally cleanup and re-init. */
        rm_alpm_cleanup(u, m, true);
        ctrl->rv = rm_alpm_init(u, m, &config, false, ALPM_NONE_BOOT);
        if (SHR_FAILURE(ctrl->rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(u, "L3_ALPM_CONTROL event fails.\n")));
            return;
        }
    }

    ALPM_CONFIG(u, m) = config;
}

static void
handle_alpm_control_event_mtop(int u, const char *event, uint64_t ev_data)
{
    handle_alpm_control_event(u, ALPM_1, event, ev_data);
}

static void
handle_alpm_control_event_std(int u, const char *event, uint64_t ev_data)
{
    handle_alpm_control_event(u, ALPM_0, event, ev_data);
}

/*!
 * \brief Initialize ALPM info
 *
 * \param [in] u Device u.
 * \param [in] config Override config if not null.
 *
 * \return SHR_E_XXX
 */
static int
rm_alpm_info_init(int u, int m, alpm_config_t *config)
{
    alpm_dev_info_t *dev_info;

    SHR_FUNC_ENTER(u);

    dev_info = bcmptm_rm_alpm_device_info_get(u, m);
    SHR_NULL_CHECK(dev_info, SHR_E_INIT);

    ALPM_ALLOC(alpm_info[u][m], sizeof(alpm_info_t), "bcmptmRmalpmInfo");
    sal_memset(alpm_info[u][m], 0, sizeof(alpm_info_t));

    SHR_IF_ERR_EXIT(
        rm_alpm_config_init(u, m, config));

    /* Command Control */
    WRITE_HW(u, m) = TRUE;
    WRITE_CACHE(u, m) = TRUE;
    SET_CACHE_VBIT(u, m) = TRUE;

exit:
    SHR_FUNC_EXIT();

}


static void
rm_alpm_call_cleanup(int u, int m)
{
    int i;

    for (i = LEVEL1; i < LEVELS; i++) {
        alpm_call[u][m][i].insert = NULL;
        alpm_call[u][m][i].match  = NULL;
        alpm_call[u][m][i].delete = NULL;
        alpm_call[u][m][i].find   = NULL;
    }
}

static void
rm_alpm_info_cleanup(int u, int m)
{
    SHR_FREE(alpm_info[u][m]);
}

static inline int
subscript_check(int u, int m, int db, int w_vrf, int ipv, int max_levels)
{
    if (db >= ALPM_DB_CNT(u, m) ||
        ipv >= IP_VER_COUNT ||
        w_vrf >= DB_VRF_HDL_CNT(u, m, db) ||
        max_levels <= 0 ||
        max_levels > MAX_LEVELS) {
        return SHR_E_INTERNAL;
    }
    return SHR_E_NONE;
}

static void
rm_alpm_ha_cleanup(int u, int m, bool graceful)
{
    if (graceful) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "Graceful exit: Trans_id %d Trans_state %d \n"),
                     ALPM_HA_TRANS_ID(u, m),
                     ALPM_HA_TRANS_STATE(u, m)));
    }
}

static int
all_alpm_lts_init(int u, int m)
{
    int rv;
    bcmltd_sid_t ltid;
    const lt_mreq_info_t *lrd_info;
    SHR_FUNC_ENTER(u);
    for (ltid = 0; ltid < BCMLTD_TABLE_COUNT; ltid++) {
        lrd_info = NULL;
        rv = bcmlrd_lt_mreq_info_get(u, ltid, &lrd_info);
        if (SHR_SUCCESS(rv) &&
            lrd_info && lrd_info->pt_type == LT_PT_TYPE_LPM) {
            all_alpm_lts[u][m][all_alpm_lts_count[u][m]++] = ltid;
        }
    }
    assert(all_alpm_lts_count[u][m] < MAX_ALPM_LTS);
    SHR_FUNC_EXIT();
}

static int
all_alpm_lts_info_init(int u, int m)
{
    int rv;
    int i;
    bcmltd_sid_t ltid;
    const lt_mreq_info_t *lrd_info;
    const bcmptm_rm_alpm_more_info_t *alpm_more_info;
    SHR_FUNC_ENTER(u);
    for (i = 0; i < all_alpm_lts_count[u][m]; i++) {
        ltid = bcmptm_rm_alpm_lt_get(u, m, i);
        lrd_info = NULL;
        rv = bcmlrd_lt_mreq_info_get(u, ltid, &lrd_info);
        if (SHR_SUCCESS(rv) && lrd_info && lrd_info->rm_more_info) {
            alpm_more_info = lrd_info->rm_more_info;
            all_alpm_lts_info[u][m][i] = *alpm_more_info;
        }
    }
    SHR_FUNC_EXIT();
}

static void
all_alpm_lts_cleanup(int u, int m)
{
    all_alpm_lts_count[u][m] = 0;
}

static int
rm_alpm_variant_init(int u, int m)
{
    bcmltd_sid_t ltid;
    const lt_mreq_info_t *lrd_info;
    const bcmptm_rm_alpm_more_info_t *alpm_more_info;
    SHR_FUNC_ENTER(u);

    if (all_alpm_lts_count[u][m] > 0) {
        ltid = bcmptm_rm_alpm_lt_get(u, m, (all_alpm_lts_count[u][m] - 1));
        SHR_IF_ERR_EXIT(
            bcmlrd_lt_mreq_info_get(u, ltid, &lrd_info));
        alpm_more_info = (bcmptm_rm_alpm_more_info_t *)lrd_info->rm_more_info;
        bcmptm_rm_alpm_variant_info[u][m] = alpm_more_info->variant_info;
    }

exit:
    SHR_FUNC_EXIT();
}

static void
rm_alpm_variant_cleanup(int u, int m)
{
    bcmptm_rm_alpm_variant_info[u][m] = NULL;
}

static int
rm_alpm_cleanup(int u, int m, bool graceful)
{
    SHR_FUNC_ENTER(u);
    bcmptm_rm_alpm_trie_cleanup(u, m);
    rm_alpm_ha_cleanup(u, m, graceful);
    bcmptm_rm_alpm_ln_cleanup(u, m, graceful);
    bcmptm_rm_alpm_l1_cleanup(u, m, graceful);
    bcmptm_rm_alpm_database_cleanup(u, m);
    rm_alpm_call_cleanup(u, m);
    rm_alpm_info_cleanup(u, m);
    all_alpm_lts_cleanup(u, m);
    rm_alpm_variant_cleanup(u, m);
    bcmptm_rm_alpm_device_cleanup(u, m);
    bcmptm_rm_alpm_traverse_cleanup(u, m);
    alpm_supported[u][m] = false;

    if (bcmptm_rm_alpm_arg_lmem[u][m]) {
        shr_lmm_delete(bcmptm_rm_alpm_arg_lmem[u][m]);
        bcmptm_rm_alpm_arg_lmem[u][m] = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
rm_alpm_ha_init(int u, int m, bool recover, uint8_t boot_type)
{
    shr_ha_sub_id sub_id;
    uint32_t req_size = sizeof(bcmptm_rm_alpm_ha_t);
    uint32_t alloc_size = sizeof(bcmptm_rm_alpm_ha_t);

    SHR_FUNC_ENTER(u);

    sub_id = m ? BCMPTM_HA_SUBID_M_RM_ALPM : BCMPTM_HA_SUBID_RM_ALPM;
    ALPM_HA(u, m) = shr_ha_mem_alloc(u,
                                  BCMMGMT_RM_ALPM_COMP_ID,
                                  sub_id,
                                  "bcmptmRmAlpmHa",
                                  &alloc_size);
    SHR_NULL_CHECK(ALPM_HA(u, m), SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    if (recover) {
        ALPM_ASSERT(ALPM_HA(u, m));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "Recover: Trans_id %d Trans_state %d \n"),
                     ALPM_HA_TRANS_ID(u, m),
                     ALPM_HA_TRANS_STATE(u, m)));
    } else {
        ALPM_HA_TRANS_ID(u, m) = 0;
        ALPM_HA_TRANS_STATE(u, m) = RM_ALPM_IDLE;
        if (boot_type == ALPM_WARM_BOOT || boot_type == ALPM_COLD_BOOT) {
            SHR_IF_ERR_EXIT(
                bcmissu_struct_info_report(u, BCMMGMT_RM_ALPM_COMP_ID,
                                           sub_id,
                                           0, /* offset */
                                           req_size, 1,
                                           BCMPTM_RM_ALPM_HA_T_ID));
        }
    }
exit:
    SHR_FUNC_EXIT();
}


static int
rm_alpm_init(int u, int m, alpm_config_t *config, bool recover, uint8_t boot_type)
{
    bcmcfg_feature_ctl_config_t feature_conf;
    int rv;
    SHR_FUNC_ENTER(u);

    /* Atomic trans enable mode */
    rv = bcmcfg_feature_ctl_config_get(&feature_conf);
    if (SHR_FAILURE(rv) || (feature_conf.enable_atomic_trans == 0)) {
        alpm_atomic_trans[u][m] = FALSE;
    } else {
        alpm_atomic_trans[u][m] = TRUE;
    }
    SHR_IF_ERR_EXIT(all_alpm_lts_init(u, m));
    SHR_IF_ERR_EXIT(rm_alpm_variant_init(u, m));
    SHR_IF_ERR_EXIT(all_alpm_lts_info_init(u, m));
    rv = bcmptm_rm_alpm_device_init(u, m);
    if (SHR_SUCCESS(rv)) {
        alpm_dev_info_t *dev_info;
        dev_info = bcmptm_rm_alpm_device_info_get(u, m);
        SHR_NULL_CHECK(dev_info, SHR_E_INIT);
        if (dev_info->alpm_ver != ALPM_VERSION_NONE) {
            alpm_supported[u][m] = true;
        } else {
            alpm_supported[u][m] = false;
            SHR_EXIT();
        }
    } else {
        alpm_supported[u][m] = false;
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(bcmptm_rm_alpm_traverse_init(u, m));
    SHR_IF_ERR_EXIT(rm_alpm_info_init(u, m, config));
    SHR_IF_ERR_EXIT(rm_alpm_call_init(u, m));
    SHR_IF_ERR_EXIT(bcmptm_rm_alpm_database_init(u, m, &ALPM_CONFIG(u, m)));
    SHR_IF_ERR_EXIT(bcmptm_rm_alpm_l1_init(u, m, recover, boot_type));
    SHR_IF_ERR_EXIT(bcmptm_rm_alpm_ln_init(u, m, recover, boot_type));
    SHR_IF_ERR_EXIT(rm_alpm_ha_init(u, m, recover, boot_type));
    SHR_IF_ERR_EXIT(bcmptm_rm_alpm_trie_init(u, m, FALSE));

    ALPM_LMM_INIT(alpm_arg_t,
                  bcmptm_rm_alpm_arg_lmem[u][m],
                  ARG_ALLOC_CHUNK_SIZE,
                  false,
                  rv);
    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
exit:
    if (alpm_supported[u][m] == false || SHR_FUNC_ERR()) {
        rm_alpm_cleanup(u, m, true);
    }
    SHR_FUNC_EXIT();
}

static int
rm_alpm_full_recover(int u, int m)
{
    uint8_t num_banks, num_tcams;
    alpm_dev_info_t *dev_info;
    SHR_FUNC_ENTER(u);

    dev_info = bcmptm_rm_alpm_device_info_get(u, m);
    SHR_NULL_CHECK(dev_info, SHR_E_INIT);

    if (dev_info->l1_tcams_shared) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_uft_banks(u, m, DBS, LEVEL1, &num_tcams, NULL, NULL,
                                     NULL));
        if (num_tcams == 0) {
            SHR_EXIT();
        }
    }

    SHR_IF_ERR_EXIT(bcmptm_rm_alpm_l1_recover(u, m, true));

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_uft_banks(u, m, DBS, LEVEL2, &num_banks, NULL, NULL, NULL));
    if (num_banks == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(bcmptm_rm_alpm_ln_recover_done(u, m, true));
exit:
    SHR_FUNC_EXIT();
}

static bool
rm_alpm_device_is_supported(int u, int m)
{
    return alpm_supported[u][m];
}

static int
rm_alpm_resp_lt_info(int u, int m, bcmltd_sid_t req_ltid,
                     bcmptm_op_type_t req_op,
                     alpm_arg_t *arg,
                     bcmptm_table_info_t *table_info)
{
    bkt_hdl_t *bkt_hdl;
    int db = arg->db, ln;
    alpm_dev_info_t *dev_info;

    SHR_FUNC_ENTER(u);

    ln = alpm_info[u][m]->config.core.db_levels[db] - 1;
    if (ln == LEVEL1 || rm_alpm_multi_levels_fall_back(u, m, arg)) {
        if (bcmptm_rm_alpm_l1_info_get(u, m)) {
            table_info->entry_limit =
                bcmptm_rm_alpm_l1_max_count(u, m, db, arg->key.kt, arg->key.vt);
        } else {
            table_info->entry_limit = 0;
        }
    } else {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_bkt_hdl_get(u, m, db, ln, &bkt_hdl));
        dev_info = bcmptm_rm_alpm_device_info_get(u, m);
        if (bkt_hdl && dev_info) {
            table_info->entry_limit =
                (uint32_t)bkt_hdl->rbkts_total * dev_info->max_base_entries;
        } else {
            table_info->entry_limit = 0;
        }
    }

    /* Disable GlobalLo LT in Combined mode when Large_VRF=1. */
    if (arg->lpm_large_vrf && arg->key.vt == VRF_GLOBAL &&
        arg->key.t.app == APP_LPM) {
        if (!bcmptm_rm_alpm_is_parallel(u, m)) {
            table_info->entry_limit = 0;
        }
    }
exit:
    SHR_FUNC_EXIT();
}
static int
rm_alpm_resp_pt_info(int u, int m, bcmptm_op_type_t req_op,
                     alpm_arg_t *arg,
                     bcmptm_pt_op_info_t  *op_info)
{
    int i;
    uint8_t cnt = 0, cnt_data = 0;
    uint8_t max_levels = DB_LEVELS(u, m, arg->db);
    alpm_sid_index_map_t map_info;
    int l1_index;
    alpm_key_type_t l1_kt;

    SHR_FUNC_ENTER(u);

    if (rm_alpm_multi_levels_fall_back(u, m, arg)) {
        max_levels = 1;
    }
    if (arg->pivot[LEVEL1]) {
        l1_index = arg->pivot[LEVEL1]->index;
        l1_kt = bcmptm_rm_alpm_key_type_get(u, m,
                                            arg->pivot[LEVEL1]->key.t.ipv,
                                            arg->pivot[LEVEL1]->key.t.len,
                                            arg->pivot[LEVEL1]->key.t.app,
                                            arg->pivot[LEVEL1]->key.vt);
    } else if (req_op == BCMPTM_OP_DELETE && max_levels == 1) {
        l1_index = arg->index[LEVEL1];
        l1_kt = arg->key.kt;
    } else {
        l1_index = INVALID_INDEX;
    }
    if (l1_index != INVALID_INDEX) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_l1_sid_hidx_get(u, m, l1_index,
                                           l1_kt,
                                           &op_info->rsp_entry_sid[LEVEL1],
                                           &op_info->rsp_entry_index[LEVEL1],
                                           &op_info->rsp_entry_sid_data[LEVEL1],
                                           &op_info->rsp_entry_index_data[LEVEL1],
                                           &cnt,
                                           &cnt_data));
    }

    for (i = LEVEL2; i < max_levels; i++) {
        if (BCMPTM_OP_DELETE == req_op && i == max_levels - 1) {
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_bucket_entry_hw_idx_get(u, m, arg->db, i,
                              /* delete index*/ arg->orig_index,
                                                &map_info.index));
        } else if (arg->pivot[i]) {
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_bucket_entry_hw_idx_get(u, m, arg->db, i,
                                                arg->pivot[i]->index,
                                                &map_info.index));
        } else {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        op_info->rsp_entry_sid[cnt] = bcmptm_rm_alpm_ln_sid_get(u, m, i);
        op_info->rsp_entry_index[cnt] = map_info.index;
        map_info.wide = i;

        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_uft_banks(u, m, arg->db, i, NULL,
                                     NULL,
                                     NULL,
                                     NULL));
        if (SHR_SUCCESS(bcmptm_rm_alpm_ln_sid_index_map(u, m, &map_info))) {
            op_info->rsp_entry_sid[cnt] = map_info.sid;
            op_info->rsp_entry_index[cnt] = map_info.sid_index;
        }
        cnt++;
    }
    op_info->rsp_entry_index_cnt = cnt;
    op_info->rsp_entry_sid_cnt = cnt;
    op_info->rsp_entry_index_data_cnt = cnt_data;
    op_info->rsp_entry_sid_data_cnt = cnt_data;

exit:
    SHR_FUNC_EXIT();
}


static int
rm_alpm_resp(int u, int m, bcmptm_op_type_t req_op,
             bcmptm_rm_alpm_req_info_t *req,
             alpm_arg_t *arg,
             uint8_t l1v,
             bcmptm_rm_alpm_rsp_info_t *rsp)
{
    const bcmptm_rm_alpm_more_info_t *alpm_more_info;
    SHR_FUNC_ENTER(u);

    alpm_more_info = req->rm_more_info;
    /*
     * PKM can be changed to wide format, need to set pkm as the one
     * from more_info to properly encode LT.
     */
    arg->pkm = alpm_more_info->pkm;
    arg->encode_data_only = 0;
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_l1_encode(u, m, arg, rsp->rsp_edw, rsp->rsp_edw, l1v));
    if (req_op == BCMPTM_OP_GET_FIRST ||
        req_op == BCMPTM_OP_GET_NEXT) {
        sal_memcpy(rsp->rsp_ekw, rsp->rsp_edw, req->pt_words * 4);
    }
exit:
    SHR_FUNC_EXIT();
}


static int
rm_alpm_trans_cmd(int u, int m,
                  uint64_t flags,
                  uint32_t cseq_id,
                  bcmptm_trans_cmd_t trans_cmd)
{

    SHR_FUNC_ENTER(u);


    if (!ALPM_HA(u, m)) {
        SHR_EXIT(); /* RM is not initialized */
    }
    if (RM_ALPM_IDLE == ALPM_HA_TRANS_STATE(u, m)) {
        SHR_EXIT();
    }

    switch (trans_cmd) {
    case BCMPTM_TRANS_CMD_COMMIT:
        if (cseq_id == ALPM_HA_TRANS_ID(u, m)) {
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_commit(u, m, false));
            ALPM_HA_TRANS_STATE(u, m) = RM_ALPM_IDLE;
        }
        break;
    case BCMPTM_TRANS_CMD_ABORT:
        if (cseq_id != ALPM_HA_TRANS_ID(u, m)) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        } else {
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_abort(u, m, true));
            ALPM_HA_TRANS_STATE(u, m) = RM_ALPM_IDLE;
        }
        break;
    case BCMPTM_TRANS_CMD_ATOMIC_TRANS_ENABLE:
        dynamic_alpm_atomic_trans[u][m] = true;
        break;
    case BCMPTM_TRANS_CMD_ATOMIC_TRANS_DISABLE:
        dynamic_alpm_atomic_trans[u][m] = false;
        break;
    default: /* BCMPTM_TRANS_CMD_IDLE_CHECK */
        SHR_ERR_EXIT(SHR_E_FAIL);
    }
exit:
    SHR_FUNC_EXIT();
}
/*******************************************************************************
 * Public Functions
 */

int
bcmptm_rm_alpm_req_arg(int u, bcmltd_sid_t req_ltid,
                       bcmptm_op_type_t req_op,
                       bcmptm_rm_alpm_req_info_t *req,
                       uint8_t *l1v,
                       int *mo,
                       alpm_arg_t *arg)
{
    uint32_t *de, *se;
    uint8_t db;
    uint8_t pkm, app, src = 0, m = 0;
    int ipv6;
    alpm_vrf_type_t vt = VRF_DEFAULT;
    const bcmptm_rm_alpm_more_info_t *alpm_more_info;

    SHR_FUNC_ENTER(u);
    SHR_NULL_CHECK(req, SHR_E_INTERNAL);

    alpm_more_info = req->rm_more_info;
    src = alpm_more_info->is_src;
    app = alpm_more_info->app;
    pkm = alpm_more_info->pkm;
    *l1v = alpm_more_info->l1v;
    ipv6 = alpm_more_info->ipv6;
    vt = alpm_more_info->vt;
    m = alpm_more_info->mtop;

    if (!bcmptm_rm_alpm_l1_info_get(u, m)) {
        SHR_VERBOSE_EXIT(SHR_E_RESOURCE);
    }

    if (req->same_key) {
        /* Append data */
        de = (req)->edw;
        se = de;
        /* bcmptm_rm_alpm_arg_init(u, m, arg); */
        arg->decode_data_only = 1;
        ALPM_ASSERT(arg->pkm == pkm);
        ALPM_ASSERT(arg->key.t.app == app);
        arg->decode_lt_view = 1;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_alpm_arg_fill(u, m, arg, de, se, *l1v));
        arg->decode_lt_view = 0;
    } else {
        bcmptm_rm_alpm_arg_init(u, m, arg);
        arg->pkm = pkm;
        arg->key.t.app = app;

        if (req_op == BCMPTM_OP_GET_TABLE_INFO) {
            arg->key.t.ipv = ipv6;
            arg->key.vt = vt;
            arg->key.kt = bcmptm_rm_alpm_key_type_get(u, m, ipv6, 0, arg->key.t.app, vt);
        } else {
            de = (req)->ekw;
            se = de;
            if (req_op == BCMPTM_OP_GET_FIRST) {
                arg->key.t.ipv = ipv6;
                arg->key.vt = vt;
                arg->key.kt = bcmptm_rm_alpm_key_type_get(u, m, ipv6, 0, arg->key.t.app, vt);
            } else {
                arg->decode_lt_view = 1;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_alpm_arg_fill(u, m, arg, de, se, *l1v));
                arg->decode_lt_view = 0;
            }
            if (arg->key.t.app == APP_LPM || arg->key.t.app == APP_COMP0) {
                switch (arg->key.kt) {
                case KEY_IPV4:
                case KEY_COMP0_V4:
                    arg->key.ip_addr[0] &= arg->key.ip_mask[0];
                    ALPM_ASSERT_ERRMSG(arg->key.ip_mask[1] == 0);
                    ALPM_ASSERT_ERRMSG(arg->key.ip_mask[2] == 0);
                    ALPM_ASSERT_ERRMSG(arg->key.ip_mask[3] == 0);
                    break;
                case KEY_IPV6_SINGLE:
                case KEY_IPV6_DOUBLE:
                    ALPM_ASSERT_ERRMSG(arg->key.ip_mask[0] == 0);
                    ALPM_ASSERT_ERRMSG(arg->key.ip_mask[1] == 0);
                    arg->key.ip_addr[0] &= arg->key.ip_mask[0];
                    arg->key.ip_addr[1] &= arg->key.ip_mask[1];
                    arg->key.ip_addr[2] &= arg->key.ip_mask[2];
                    arg->key.ip_addr[3] &= arg->key.ip_mask[3];
                    break;
                case KEY_IPV6_QUAD:
                case KEY_COMP0_V6:
                default:
                    arg->key.ip_addr[0] &= arg->key.ip_mask[0];
                    arg->key.ip_addr[1] &= arg->key.ip_mask[1];
                    arg->key.ip_addr[2] &= arg->key.ip_mask[2];
                    arg->key.ip_addr[3] &= arg->key.ip_mask[3];
                    break;
                }
            }
        }
    }

    if (SHR_FAILURE
        (bcmptm_rm_alpm_db(u, m, arg->key.t.app, src, arg->key.vt, &db)) ||
        DB_LEVELS(u, m, db) == 0) {
        SHR_VERBOSE_EXIT(SHR_E_RESOURCE);
    }
    arg->db = db;
    arg->mtop = m;
    *mo = m;
exit:
    SHR_FUNC_EXIT();

}

void
bcmptm_rm_alpm_trans_id_incr(int u, int m)
{
    CSEQ_ID(u, m)++;
}

int
bcmptm_rm_alpm_is_large_vrf(int u, int m)
{
    return alpm_info[u][m]->config.core.large_vrf;
}

int
bcmptm_rm_alpm_is_large_l4_port(int u, int m)
{
    return alpm_info[u][m]->config.core.large_l4_port;
}

int
bcmptm_rm_alpm_vrf_len(int u, int m)
{
    alpm_dev_info_t *dev_info;
    dev_info = bcmptm_rm_alpm_device_info_get(u, m);
    if (dev_info) {
        return shr_util_log2(dev_info->max_vrf + 1);
    }
    return 0;
}

int
bcmptm_rm_alpm_comp_key_type(int u, int m)
{
    return alpm_info[u][m]->config.core.comp_key_type;
}

int
bcmptm_rm_alpm_has_4_data_types(int u, int m)
{
    alpm_dev_info_t *dev_info;
    dev_info = bcmptm_rm_alpm_device_info_get(u, m);
    return dev_info->feature_has_4_data_types;
}

int
bcmptm_rm_alpm_is_large_data_type(int u, int m, int db)
{
    alpm_dev_info_t *dev_info;
    dev_info = bcmptm_rm_alpm_device_info_get(u, m);
    return dev_info->feature_large_data_type[db];
}

int
bcmptm_rm_alpm_cache_data_type(int u, int m)
{
    alpm_dev_info_t *dev_info;
    dev_info = bcmptm_rm_alpm_device_info_get(u, m);
    return dev_info->feature_cache_dt;
}

int
bcmptm_rm_alpm_mode(int u, int m)
{
    return ALPM_MODE(u, m);
}

int
bcmptm_rm_alpm_is_parallel(int u, int m)
{
    return (ALPM_MODE(u, m) & ALPM_MODE_PARALLEL) ? 1 : 0;
}


uint8_t
bcmptm_rm_alpm_hit_support(int u, int m)
{
    return alpm_info[u][m]->config.hit_support;
}

bool
bcmptm_rm_alpm_pkm128_is_set(int u, int m)
{
    return alpm_info[u][m]->config.ver0_128_enable;
}

const bcmptm_alpm_variant_info_t *
bcmptm_rm_alpm_variant_get(int u, int m)
{
    return bcmptm_rm_alpm_variant_info[u][m];
}

int
bcmptm_rm_alpm_uft_banks(int u, int m, int db, int ln,
                         uint8_t *num_banks, uint8_t *start_bank,
                         uint8_t *bucket_bits, uint32_t *bank_bitmap)
{
    uint8_t banks, bkt_bits = 0;
    uint32_t bitmap;
    bcmptm_cth_alpm_control_bank_t *p_bank;


    switch (db) {
    case DB0:
        p_bank = &alpm_info[u][m]->config.core.db0;
        break;
    case DB1:
        p_bank = &alpm_info[u][m]->config.core.db1;
        break;
    case DB2:
        p_bank = &alpm_info[u][m]->config.core.db2;
        break;
    case DB3:
        p_bank = &alpm_info[u][m]->config.core.db3;
        break;
    case DBS:
        p_bank = &alpm_info[u][m]->config.core.tot;
        break;
    default:
        return SHR_E_PARAM;

    }
    if (ln == LEVEL1) {
        banks = p_bank->num_l1_banks;
        bitmap = p_bank->l1_bank_bitmap;
    } else if (ln == LEVEL2) {
        banks = p_bank->num_l2_banks;
        bitmap = p_bank->l2_bank_bitmap;
        bkt_bits = p_bank->l2_bucket_bits;
    } else if (ln == LEVEL3) {
        banks = p_bank->num_l3_banks;
        bitmap = p_bank->l3_bank_bitmap;
        bkt_bits = p_bank->l3_bucket_bits;
    } else {
        return SHR_E_PARAM;
    }
    if (num_banks) {
        *num_banks = banks;
    }
    if (bank_bitmap) {
        *bank_bitmap = bitmap;
    }
    if (start_bank) {
        uint8_t i;
        for (i = 0; i < 32; i++) {
            if (bitmap & (1 << i)) {
                *start_bank = i;
                break;
            }
        }
        if (i == 32) {
            return SHR_E_PARAM;
        }
    }
    if (bucket_bits) {
        *bucket_bits = bkt_bits;
    }
    return SHR_E_NONE;
}

alpm_info_t *
bcmptm_rm_alpm_info_get(int u, int m)
{
    return alpm_info[u][m];
}

const alpm_config_t *
bcmptm_rm_alpm_config(int u, int m)
{
    return &(alpm_info[u][m]->config);
}

int
bcmptm_rm_alpm_insert(int u, int m, bcmltd_sid_t ltid, alpm_arg_t *arg, int levels)
{
    int rv;
#ifdef ALPM_DEBUG
    static uint32_t inc = 0;
#endif

    rv = (RM_ALPM_CALL_EXEC(u, m, levels, insert) (u, m, arg));

#ifdef ALPM_DEBUG
    if (inc > alpm_sanity_start &&
        inc % alpm_sanity_step == 0) {
        assert(bcmptm_rm_alpm_sanity(u, m, BCMLTD_SID_INVALID, NULL) == 0);
    }
    inc++;
#endif
    bcmptm_rm_alpm_traverse_cache_invalidate(u, m, ltid, arg, false);
    return rv;
}

int
bcmptm_rm_alpm_match(int u, int m, alpm_arg_t *arg, int levels)
{
    return (RM_ALPM_CALL_EXEC(u, m, levels, match) (u, m, arg));
}


int
bcmptm_rm_alpm_delete(int u, int m, bcmltd_sid_t ltid, alpm_arg_t *arg, int levels)
{
    int rv;
    rv = (RM_ALPM_CALL_EXEC(u, m, levels, delete) (u, m, arg));
    bcmptm_rm_alpm_traverse_cache_invalidate(u, m, ltid, arg, true);
    return rv;
}

int
bcmptm_rm_alpm_find(int u, int m, alpm_arg_t *arg, int levels)
{
    return (RM_ALPM_CALL_EXEC(u, m, levels, find) (u, m, arg));
}

int
bcmptm_rm_alpm_req(int                       u,
                   uint64_t                  flags,
                   uint32_t                  cseq_id,
                   bcmltd_sid_t              req_ltid,
                   bcmbd_pt_dyn_info_t       *pt_dyn_info,
                   bcmptm_op_type_t          req_op,
                   bcmptm_rm_alpm_req_info_t *req,

                   bcmptm_rm_alpm_rsp_info_t *rsp,
                   bcmltd_sid_t              *rsp_ltid,
                   uint32_t                  *rsp_flags)
{
    alpm_arg_t *arg = &static_arg[u];
    uint8_t max_levels;
    uint8_t l1v = 0;
    int m;
    int rv = SHR_E_NONE;
    bool snapshot_en = flags & BCMLT_ENT_ATTR_TRAVERSE_SNAPSHOT;

    /* Log the function entry. */
    SHR_FUNC_ENTER(u);

    rv = bcmptm_rm_alpm_req_arg(u, req_ltid, req_op, req, &l1v, &m, arg);
    if (rv == SHR_E_RESOURCE) {
        switch (req_op) {
            case BCMPTM_OP_LOOKUP:
            case BCMPTM_OP_DELETE:
            case BCMPTM_OP_GET_FIRST:
            case BCMPTM_OP_GET_NEXT:
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
                break;
            case BCMPTM_OP_GET_TABLE_INFO:
                if (rsp->rsp_misc) {
                    ((bcmptm_table_info_t *)rsp->rsp_misc)->entry_limit = 0;
                }
                SHR_EXIT();
                break;
            default:
                SHR_ERR_EXIT(rv);
                break;
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    alpm_info[u][m]->hw_info = req->hw_info;
    INST(u, m) = pt_dyn_info->tbl_inst;
    CSEQ_ID(u, m) = cseq_id;
    REQ_FLAGS(u, m) = flags;
    REQ_LTID(u, m) = req_ltid;
    REQ_FORMAT_ID(u, m) = 0;

    if (LOG_CHECK_DEBUG(BSL_LS_BCMPTM_RMALPM)) {
        shr_pb_t *pb = shr_pb_create();
        shr_pb_printf(pb, "ALPM:%s ", req_op_name(u, req_op));
        if (req_op != BCMPTM_OP_GET_TABLE_INFO) {
            bcmptm_rm_alpm_arg_dump(u, m, arg, true, pb);
        }
        LOG_CLI((BSL_META("%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }

    max_levels = DB_LEVELS(u, m, arg->db);

    SHR_IF_ERR_EXIT(
        subscript_check(u, m, arg->db, arg->key.t.w_vrf, arg->key.t.ipv,
                        max_levels));

    if (req_op == BCMPTM_OP_INSERT ||
        req_op == BCMPTM_OP_DELETE) {
        ALPM_HA_TRANS_ID(u, m) = cseq_id;
    }

    if (flags & BCMLT_ENT_ATTR_TRAVERSE_DONE) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_traverse_done(u, m, req_ltid, snapshot_en, arg));
        SHR_EXIT();
    }

    switch (req_op) {
    case BCMPTM_OP_LOOKUP:
        if (flags & BCMLT_ENT_ATTR_LPM_LOOKUP) {
            rv = bcmptm_rm_alpm_find(u, m, arg, max_levels);
        } else{
            arg->decode_data_only = 1;
            rv = bcmptm_rm_alpm_match(u, m, arg, max_levels);
        }
        break;

    case BCMPTM_OP_INSERT:
        rv = bcmptm_rm_alpm_insert(u, m, req_ltid, arg, max_levels);
        if (rv == SHR_E_RESOURCE) {
            SHR_ERR_EXIT(rv);
        } else {
            SHR_IF_ERR_EXIT(rv);
        }
        break;

    case BCMPTM_OP_DELETE:
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_alpm_delete(u, m, req_ltid, arg, max_levels));
        break;

    case BCMPTM_OP_GET_FIRST:
        rv = bcmptm_rm_alpm_get_first(u, m, req_ltid, snapshot_en, arg, max_levels);
        break;

    case BCMPTM_OP_GET_NEXT:
        rv = bcmptm_rm_alpm_get_next(u, m, req_ltid, snapshot_en, arg, max_levels);
        break;

    case BCMPTM_OP_GET_TABLE_INFO:
        break;
    default:
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
        break;
    }

    switch (req_op) {
    case BCMPTM_OP_LOOKUP:
    case BCMPTM_OP_GET_FIRST:
    case BCMPTM_OP_GET_NEXT:
        if (SHR_SUCCESS(rv)) {
            SHR_IF_ERR_EXIT(req->pt_words > req->rsp_ekw_buf_wsize ?
                            SHR_E_INTERNAL : SHR_E_NONE);
            SHR_IF_ERR_EXIT(req->pt_words > req->rsp_edw_buf_wsize ?
                            SHR_E_INTERNAL : SHR_E_NONE);
            arg->encode_lt_view = 1;
            SHR_IF_ERR_EXIT(
                rm_alpm_resp(u, m, req_op, req, arg, l1v, rsp));
            arg->encode_lt_view = 0;
            if (rsp->rsp_pt_op_info) {
                SHR_IF_ERR_EXIT(
                    rm_alpm_resp_pt_info(u, m, req_op, arg, rsp->rsp_pt_op_info));
            }

        } else if (req_op == BCMPTM_OP_LOOKUP) {
            sal_memset(arg->pivot, 0, sizeof(arg->pivot));
        }
        SHR_ERR_EXIT(rv);
    case BCMPTM_OP_INSERT:
    case BCMPTM_OP_DELETE:
        if (rsp->rsp_pt_op_info) {
            SHR_IF_ERR_EXIT(
                rm_alpm_resp_pt_info(u, m, req_op, arg, rsp->rsp_pt_op_info));
        }

        break;
    case BCMPTM_OP_GET_TABLE_INFO:
    default:
        if (rsp->rsp_misc) {
            SHR_IF_ERR_EXIT(
                rm_alpm_resp_lt_info(u, m, req_ltid, req_op, arg, rsp->rsp_misc));
        }
        break;
    }
exit:
    if (arg->state_changed &&
        (req_op == BCMPTM_OP_INSERT || req_op == BCMPTM_OP_DELETE)) {
        ALPM_HA_TRANS_STATE(u, m) = RM_ALPM_CHANGED;
    }

    /* Log the function exit. */
    SHR_FUNC_EXIT();
}


int
bcmptm_rm_alpm_commit(int u, int m, bool force_sync)
{
    if (alpm_atomic_trans[u][m] ||
        dynamic_alpm_atomic_trans[u][m] || force_sync) {
        bcmptm_rm_alpm_l1_ha_sync(u, m);
    }
    return SHR_E_NONE;
}

int
bcmptm_rm_alpm_abort(int u, int m, bool full)
{
    alpm_config_t config = ALPM_CONFIG(u, m);
#if defined(ALPM_DEBUG) && defined(ALPM_DEBUG_FILE)
    bool write_file = false;
    int db, db_count = 0;
#endif

    SHR_FUNC_ENTER(u);
    SHR_IF_ERR_EXIT(full ? SHR_E_NONE : SHR_E_UNAVAIL);
#if defined(ALPM_DEBUG) && defined(ALPM_DEBUG_FILE)
    if (LOG_CHECK_INFO(BSL_LS_BCMPTM_RMALPM)) {
        write_file = bcmptm_rm_alpm_dump_to_file_get();
        bcmptm_rm_alpm_dump_to_file_set(true);

        db_count = bcmptm_rm_alpm_database_count(u, m);
        for (db = 0; db < db_count; db++) {
            bcmptm_rm_alpm_info_dump(u, m, db, true, false, NULL);
        }
    }
#endif
    SHR_IF_ERR_EXIT(rm_alpm_cleanup(u, m, true));
    SHR_IF_ERR_EXIT(rm_alpm_init(u, m, &config, true, ALPM_NONE_BOOT));
    SHR_IF_ERR_EXIT(rm_alpm_full_recover(u, m));
#if defined(ALPM_DEBUG) && defined(ALPM_DEBUG_FILE)
    if (LOG_CHECK_INFO(BSL_LS_BCMPTM_RMALPM)) {
        for (db = 0; db < db_count; db++) {
            bcmptm_rm_alpm_info_dump(u, m, db, true, false, NULL);
        }
        bcmptm_rm_alpm_dump_to_file_set(write_file);
    }
#endif

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_init(int u, bool warm)
{
    int m = ALPM_0;
    bcmevm_event_cb *evm_cb[ALPMS] = {
        handle_alpm_control_event_std,
        handle_alpm_control_event_mtop
    };
    const char *evm_name[ALPMS] = {
        "bcmptmEvAlpmCtrlUpdate",
        "bcmptmEvMtopAlpmCtrlUpdate"
    };

    SHR_FUNC_ENTER(u);
    do {
        if (warm) {
            SHR_IF_ERR_EXIT(
                rm_alpm_init(u, m, NULL, true, ALPM_WARM_BOOT));
            if (!rm_alpm_device_is_supported(u, m)) {
                SHR_EXIT();
            }
            SHR_IF_ERR_EXIT(rm_alpm_full_recover(u, m));
        } else {
            SHR_IF_ERR_EXIT(
                rm_alpm_init(u, m, NULL, false, ALPM_COLD_BOOT));
            if (!rm_alpm_device_is_supported(u, m)) {
                SHR_EXIT();
            }
        }
        SHR_IF_ERR_EXIT(
            bcmevm_register_published_event(u, evm_name[m], evm_cb[m]));
    } while (++m < ALPMS);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_stop(int u)
{
    int m = ALPM_0;
    bcmevm_event_cb *evm_cb[ALPMS] = {
        handle_alpm_control_event_std,
        handle_alpm_control_event_mtop
    };
    const char *evm_name[ALPMS] = {
        "bcmptmEvAlpmCtrlUpdate",
        "bcmptmEvMtopAlpmCtrlUpdate"
    };

    SHR_FUNC_ENTER(u);
    do {
        if (rm_alpm_device_is_supported(u, m)) {
            SHR_IF_ERR_EXIT(
                bcmevm_unregister_published_event(u, evm_name[m], evm_cb[m]));
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_commit(u, m, true));

        }
    } while (++m < ALPMS);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_cleanup(int u, bool graceful)
{
    int m = ALPM_0;

    SHR_FUNC_ENTER(u);
    do {
        if (rm_alpm_device_is_supported(u, m)) {
            SHR_IF_ERR_EXIT(
                rm_alpm_cleanup(u, m, graceful));
        }
    } while (++m < ALPMS);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_global_init(void)
{
    return SHR_E_NONE;
}

int
bcmptm_rm_alpm_global_cleanup(void)
{
    return SHR_E_NONE;
}

int
bcmptm_rm_alpm_trans_cmd(int u,
                         uint64_t flags,
                         uint32_t cseq_id,
                         bcmptm_trans_cmd_t trans_cmd)
{
    int m = ALPM_0;

    SHR_FUNC_ENTER(u);

    do {
        if (rm_alpm_device_is_supported(u, m)) {
            SHR_IF_ERR_EXIT(
               rm_alpm_trans_cmd(u, m, flags, cseq_id, trans_cmd));
        }
    } while (++m < ALPMS);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_lts_count(int u, int m)
{
    return all_alpm_lts_count[u][m];
}

bcmlrd_sid_t
bcmptm_rm_alpm_lt_get(int u, int m, int i)
{
    if (i >= all_alpm_lts_count[u][m]) {
        return BCMLTD_SID_INVALID;
    }
    return all_alpm_lts[u][m][i];
}

bcmptm_rm_alpm_more_info_t *
bcmptm_rm_alpm_lt_info_get(int u, int m, int i)
{
    if (i >= all_alpm_lts_count[u][m]) {
        return NULL;
    }
    return &all_alpm_lts_info[u][m][i];
}


void
bcmptm_rm_alpm_usage_get(int u, int m, int level, bcmptm_cth_alpm_usage_info_t *info)
{
    if (level == LEVEL1) {
        bcmptm_rm_alpm_l1_usage_get(u, m, info);
    } else {
        bcmptm_rm_alpm_ln_usage_get(u, m, level, info);
    }
}
