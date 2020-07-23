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

static shr_sysm_rv_t
ptm_init(int unit, bool warm)
{
    if (SHR_FAILURE(bcmptm_ptcache_init(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }
    if (SHR_FAILURE(bcmptm_scor_init(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }
    if (SHR_FAILURE(bcmptm_cci_comp_config(unit, warm, BCMPTM_SUB_PHASE_3))) {
        return SHR_SYSM_RV_ERROR;
    }
    if (SHR_FAILURE(bcmptm_mreq_init(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }
    if (SHR_FAILURE(bcmptm_wal_init(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }
    if (SHR_FAILURE(bcmlrd_lt_mreq_info_init(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    /* Now PTM is ready to accept mreq, ireq API calls.
     * For coldboot this will happen during comp_config state.
     * For warmboot this will happen during pre_config state.
     * Must do it here because RMs in PTM will use ireq API.
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
    if (ptm_is_blocked(instance_category, unit, init_comp_list,
                       blocking_component)) {
        return SHR_SYSM_RV_BLOCKED;
    }

    if (SHR_FAILURE(bcmptm_fnptr_init(unit))) {
        return SHR_SYSM_RV_ERROR;
    }
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

    if (ptm_is_blocked(instance_category, unit, init_comp_list,
                       blocking_component)) {
        return SHR_SYSM_RV_BLOCKED;
    }

    if (SHR_FAILURE(bcmptm_var_data_init(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    /* WAL_RDR init
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
    if (SHR_FAILURE(bcmptm_cci_init(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }

    return ptm_init(unit, warm);
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
    if (ptm_is_blocked(instance_category, unit, init_comp_list,
                       blocking_component)) {
        return SHR_SYSM_RV_BLOCKED;
    }

    /* PTM pre_config */
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

    /* PTM stop: cci */
    if (SHR_FAILURE(bcmptm_cci_stop(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    /* PTM stop: modeled path - wait for WAL drain */
    if (SHR_FAILURE(bcmptm_mreq_stop(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

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
