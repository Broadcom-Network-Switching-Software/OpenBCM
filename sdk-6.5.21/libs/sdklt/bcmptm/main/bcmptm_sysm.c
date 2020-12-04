/*! \file bcmptm_sysm.c
 *
 * Interface functions to the system manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bcmdrd_config.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmptm/bcmptm_sysm.h>
#include "bcmptm/bcmptm_internal.h"
#include "bcmptm/bcmptm_chip_internal.h"
#include <bcmptm/bcmlrd_ptrm.h>
#include "bcmptm/bcmptm_ptcache_internal.h"
#include "bcmptm/bcmptm_scor_internal.h"
#include "bcmptm/bcmptm_wal_internal.h"
#include "bcmptm/bcmptm_cci_internal.h"
#include "bcmptm/bcmptm_ser_internal.h"
#include "bcmptm/bcmptm_ser_cth_internal.h"
#include <bcmcfg/comp/bcmcfg_feature_ctl.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MISC


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */
static bcmmgmt_sysm_comp_id_t init_comp_list[] =
    {BCMMGMT_BD_COMP_ID,
     BCMMGMT_CFG_COMP_ID,
     BCMMGMT_DRD_COMP_ID,
     BCMMGMT_LRD_COMP_ID,
     BCMMGMT_LTD_COMP_ID,
     BCMMGMT_IMM_FE_COMP_ID,
     BCMMGMT_EVM_COMP_ID,
     BCMMGMT_MAX_COMP_ID}; /* end of list indicator */

/* Following ensures that TRM, LTM, CTH (clients of PTM)
 * are no longer sending commands to PTM when we attempt to do PTM stop */
static bcmmgmt_sysm_comp_id_t cleanup_comp_list[] =
    {BCMMGMT_TRM_COMP_ID,
     BCMMGMT_RM_HASH_COMP_ID,
     BCMMGMT_RM_TCAM_COMP_ID,
     BCMMGMT_RM_ALPM_COMP_ID,
     BCMMGMT_UFT_COMP_ID,
     BCMMGMT_LTM_COMP_ID,
     BCMMGMT_DI_COMP_ID,
     BCMMGMT_SER_COMP_ID,
     BCMMGMT_MAX_COMP_ID}; /* end of list indicator */

static shr_sysm_states_t instance_state[BCMDRD_CONFIG_MAX_UNITS];

/* accum_init_seq == TRUE:
 *      - This can be asserted only for coldboot && enable_new_init case.
 *      - New init sequence - WAL will store the init_seq ops during comp_config
 *        and send these accumulated ops to HW during pre_config.
 *
 * accum_init_seq == FALSE:
 *      - Legacy init sequence - write_ops from different components are sent
 *        directly to HW without going through accumulation in WAL.
 */
static bool accum_init_seq[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private Functions
 */
static bool
ptm_is_blocked(shr_sysm_categories_t instance_category, int unit,
               bcmmgmt_sysm_comp_id_t *comp_list,
               uint32_t *blocking_component)
{
    int i;
    for (i = 0; ; i++) {
        if (comp_list[i] == BCMMGMT_MAX_COMP_ID) {
            break;
        }
        if (!shr_sysm_is_comp_complete(comp_list[i], instance_category, unit)) {
            *blocking_component = comp_list[i];
            return TRUE;
        }
    }
    return FALSE;
}

/*
 * Within PTM the sequence for init:
 *      WALreader,
 *      WALwriter,
 *      PTcache + CCI,
 *      scor,
 *      ireq,
 *      mreq
 */
static shr_sysm_rv_t
ptm_init(int unit, bool warm)
{
    bcmcfg_feature_ctl_config_t feature_conf;
    int rv = SHR_E_NONE;
    bool enable_legacy_init;

    rv = bcmcfg_feature_ctl_config_get(&feature_conf);
    if (SHR_FAILURE(rv) || (feature_conf.enable_legacy_init == 0)) {
        enable_legacy_init = FALSE;
    } else {
        enable_legacy_init = TRUE;
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "enable_legacy_init is %s\n"),
         enable_legacy_init ?  "True" : "False"));

    /* WAL init
     * - WAL reader must be enabled as soon as possible to allow access to
     *   HW through modeled path.
     *   Rsrc_mgr in PTM makes access to HW through modeled path in comp_config
     *   stage.
     *   Currently WAL reader is initialized before WAL writer.
     *
     *   Note: init of PTM assumes config vars are available.
     *   This is ensured by putting dependency on CFG component.
     *   (bcmcfg is ready only in comp_config phase)
     */
    if (SHR_FAILURE(bcmptm_walr_init(unit))) {
        return SHR_SYSM_RV_ERROR;
    }
    if (SHR_FAILURE(bcmptm_wal_init(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }
    if (!warm && !enable_legacy_init) { /* Coldboot && enable_new_init */
        int tmp_rv = bcmptm_wal_init_seq(unit,
                                         TRUE,   /* record */
                                         FALSE,  /* send */
                                         FALSE   /* accumulate */
                                        );
        if (SHR_FAILURE(tmp_rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Recording of init_seq failed (rv = %0d)"
                            "...\n"),
                 tmp_rv));
            return SHR_SYSM_RV_ERROR;
        } else {
            accum_init_seq[unit] = TRUE;
        }
    }

    /* cci + ptcache */
    if (SHR_FAILURE(bcmptm_cci_init(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }
    if (SHR_FAILURE(bcmptm_ptcache_init(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }
    if (SHR_FAILURE(bcmptm_cci_comp_config(unit, warm, BCMPTM_SUB_PHASE_3))) {
        return SHR_SYSM_RV_ERROR;
    }

    /* scor */
    if (SHR_FAILURE(bcmptm_scor_init(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }
    if (accum_init_seq[unit]) {
        if (SHR_FAILURE(bcmptm_scor_mode_set(unit,
                                             TRUE,  /* hw_wr_en */
                                             TRUE,  /* hw_wr_forward */
                                             FALSE  /* hw_rd_enable */
                                            ))) {
            return SHR_SYSM_RV_ERROR;
        }
    }

    /* ireq */
    if (SHR_FAILURE(bcmptm_ireq_init(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }
    if (accum_init_seq[unit]) {
        if (SHR_FAILURE(bcmptm_ireq_mode_set(unit,
                                             TRUE,  /* hw_wr_en */
                                             TRUE,  /* hw_wr_forward */
                                             FALSE  /* hw_rd_enable */
                                            ))) {
            return SHR_SYSM_RV_ERROR;
        }
    }

    /* mreq */
    if (SHR_FAILURE(bcmptm_mreq_init(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }
    if (SHR_FAILURE(bcmlrd_lt_mreq_info_init(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    /* Now PTM is ready to accept mreq, ireq API calls.
     */
    bcmptm_ptm_ready_set(unit, TRUE);

    /* Success */
    return SHR_SYSM_RV_DONE;
}

/* Phase: init
 * Expect Devices will be attached, BD, DRD, LRD will happen here.
 * We can do config-independent init here
 */
static shr_sysm_rv_t
bcmptm_unit_init(shr_sysm_categories_t instance_category,
                 int unit,
                 void *comp_data,
                 bool warm,
                 uint32_t *blocking_component)
{
    accum_init_seq[unit] = FALSE;

    if (ptm_is_blocked(instance_category, unit, init_comp_list,
                       blocking_component)) {
        return SHR_SYSM_RV_BLOCKED;
    }

    if (SHR_FAILURE(bcmptm_fnptr_init(unit))) {
        return SHR_SYSM_RV_ERROR;
    }
    instance_state[unit] = SHR_SYSM_INIT;
    return SHR_SYSM_RV_DONE;
}

/* Phase: comp_config
 */
static shr_sysm_rv_t
bcmptm_unit_comp_config(shr_sysm_categories_t instance_category,
                        int unit,
                        void *comp_data,
                        bool warm,
                        uint32_t *blocking_component)
{
    shr_sysm_rv_t rv;
    if (ptm_is_blocked(instance_category, unit, init_comp_list,
                       blocking_component)) {
        return SHR_SYSM_RV_BLOCKED;
    }

    if (SHR_FAILURE(bcmptm_var_data_init(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    rv = ptm_init(unit, warm);
    if (rv == SHR_SYSM_RV_DONE) {
        instance_state[unit] = SHR_SYSM_COMP_CONFIG;
    }
    return rv;
}

/* Phase: pre_config
 * Expect Cfg writes.
 * unit-dependent config will happen with LT writes
 */
static shr_sysm_rv_t
bcmptm_unit_pre_config(shr_sysm_categories_t instance_category,
                       int unit,
                       void *comp_data,
                       bool warm,
                       uint32_t *blocking_component)
{
    int tmp_rv;
    if (accum_init_seq[unit]) {
        /* Can be here only for coldboot && enable_new_init case */

        /* Disable all HW access via scor, interactive paths */
        if (SHR_FAILURE(bcmptm_scor_mode_set(unit,
                                             FALSE,  /* hw_wr_en */
                                             FALSE,  /* hw_wr_forward */
                                             FALSE   /* hw_rd_enable */
                                            ))) {
            return SHR_SYSM_RV_ERROR;
        }
        if (SHR_FAILURE(bcmptm_ireq_mode_set(unit,
                                             FALSE,  /* hw_wr_en */
                                             FALSE,  /* hw_wr_forward */
                                             FALSE   /* hw_rd_enable */
                                            ))) {
            return SHR_SYSM_RV_ERROR;
        }

        /* Send recorded ops in WAL to HW. */
        tmp_rv = bcmptm_wal_init_seq(unit,
                                     FALSE,  /* record */
                                     TRUE,   /* send */
                                     FALSE   /* accumulate */
                                    );
        if (SHR_FAILURE(tmp_rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Playback of init_seq failed (rv = %0d)...\n"),
                 tmp_rv));
            return SHR_SYSM_RV_ERROR;
        }

        /* Resume normal HW access via scor, interactive paths */
        if (SHR_FAILURE(bcmptm_scor_mode_set(unit,
                                             TRUE,  /* hw_wr_en */
                                             FALSE, /* hw_wr_forward */
                                             TRUE   /* hw_rd_enable */
                                            ))) {
            return SHR_SYSM_RV_ERROR;
        }
        if (SHR_FAILURE(bcmptm_ireq_mode_set(unit,
                                             TRUE,  /* hw_wr_en */
                                             FALSE, /* hw_wr_forward */
                                             TRUE   /* hw_rd_enable */
                                            ))) {
            return SHR_SYSM_RV_ERROR;
        }
    }
    instance_state[unit] = SHR_SYSM_PRE_CONFIG;
    return SHR_SYSM_RV_DONE;
}

/* Phase: run
 * 1. Start all threads as configured.
 */
static void
bcmptm_unit_run(shr_sysm_categories_t instance_category,
                int unit,
                void *comp_data)
{
    int tmp_rv = 0;

    tmp_rv = bcmptm_cci_run(unit);
    if (SHR_FAILURE(tmp_rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "bcmptm_cci_unit_run FAILED(rv = %0d)\n"),
             tmp_rv));
    }
    instance_state[unit] = SHR_SYSM_RUN;
}

/* Phase: stop
 */
static shr_sysm_rv_t
bcmptm_unit_stop(shr_sysm_categories_t instance_category,
                 int unit,
                 void *comp_data,
                 bool graceful,
                 uint32_t *blocking_component)
{
    if (ptm_is_blocked(instance_category, unit, cleanup_comp_list,
                       blocking_component)) {
        return SHR_SYSM_RV_BLOCKED;
    }

    /* PTM stop: ireq - nothing to do */
    if (instance_state[unit] < SHR_SYSM_COMP_CONFIG) {
       /* Nothing to do since we never initialized */
       instance_state[unit] = SHR_SYSM_STOP;
       return SHR_SYSM_RV_DONE;
    }

    /* PTM stop: cci */
    if (SHR_FAILURE(bcmptm_cci_stop(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    /* PTM stop: modeled path - wait for WAL drain */
    if (SHR_FAILURE(bcmptm_mreq_stop(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    instance_state[unit] = SHR_SYSM_STOP;
    return SHR_SYSM_RV_DONE;
}

/* Phase: shutdown */
static shr_sysm_rv_t
bcmptm_unit_shutdown(shr_sysm_categories_t instance_category,
                     int unit,
                     void *comp_data,
                     bool graceful,
                     uint32_t *blocking_component)
{
    if (ptm_is_blocked(instance_category, unit, cleanup_comp_list,
                       blocking_component)) {
        return SHR_SYSM_RV_BLOCKED;
    }
    if (SHR_FAILURE(bcmptm_ptcache_cleanup(unit))) {
        return SHR_SYSM_RV_ERROR;
    }
    if (SHR_FAILURE(bcmptm_cci_cleanup(unit))) {
        return SHR_SYSM_RV_ERROR;
    }
    if (SHR_FAILURE(bcmptm_mreq_cleanup(unit))) {
        return SHR_SYSM_RV_ERROR;
    }
    if (SHR_FAILURE(bcmptm_scor_cleanup(unit))) {
        return SHR_SYSM_RV_ERROR;
    }
    if (SHR_FAILURE(bcmptm_wal_cleanup(unit))) {
        /* must be after bcmptm_mreq_cleanup to ensure that mreq_cleanup
         * sees correct wal_mode !! */
        return SHR_SYSM_RV_ERROR;
    }
    if (SHR_FAILURE(bcmptm_walr_cleanup(unit))) {
        /* must be after bcmptm_wal_cleanup to ensure that writer is idle */
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmlrd_lt_mreq_info_cleanup(unit))) {
        return SHR_SYSM_RV_ERROR;
    }


    /* Always last
     * - Allow sub-components to use chip-specific functions for cleanup
     */
    if (SHR_FAILURE(bcmptm_var_data_cleanup(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmptm_fnptr_cleanup(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    instance_state[unit] = SHR_SYSM_SHUTDOWN;
    return SHR_SYSM_RV_DONE;
}


/*******************************************************************************
 * Public Functions
 */
int
bcmptm_start(void)
{
    shr_sysm_cb_func_set_t funcs;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Register unit-specific functions */
    sal_memset(&funcs, 0, sizeof(funcs));
    funcs.init = bcmptm_unit_init;
    funcs.comp_config = bcmptm_unit_comp_config;
    funcs.pre_config = bcmptm_unit_pre_config;
    funcs.run = bcmptm_unit_run;
    funcs.stop = bcmptm_unit_stop;
    funcs.shutdown = bcmptm_unit_shutdown;
    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_PTM_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &funcs,
                           NULL));
exit:
    SHR_FUNC_EXIT();
}
