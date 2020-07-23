/*! \file bcmptm_cth_alpm_sysm.c
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
#include "bcmptm/bcmptm_internal.h"
#include "bcmptm/bcmptm_chip_internal.h"
#include "bcmptm/bcmptm_cth_alpm_internal.h"
#include "bcmptm/bcmptm_cth_alpm_sysm.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MISC
#define IF_BLOCK_RETURN(cat, unit, comp_list, block_comp)                   \
    if (is_blocked(cat, unit, comp_list, COUNTOF(comp_list), block_comp)) { \
        return SHR_SYSM_RV_BLOCKED;                                         \
    }

/*******************************************************************************
 * Private variables
 */
static bcmmgmt_sysm_comp_id_t init_comp_list[] = {
        BCMMGMT_BD_COMP_ID,
        BCMMGMT_CFG_COMP_ID,
        BCMMGMT_DRD_COMP_ID,
        BCMMGMT_LTD_COMP_ID,
        BCMMGMT_IMM_FE_COMP_ID,
        BCMMGMT_EVM_COMP_ID,
        BCMMGMT_PTM_COMP_ID,
        BCMMGMT_DI_COMP_ID,
        BCMMGMT_SER_COMP_ID,
        BCMMGMT_LRD_COMP_ID,
        BCMMGMT_ALPM_BE_COMP_ID,
        BCMMGMT_UFT_COMP_ID,
    };
static bcmmgmt_sysm_comp_id_t cleanup_comp_list[] = {
        BCMMGMT_TRM_COMP_ID,
        BCMMGMT_LTM_COMP_ID
    };

/*******************************************************************************
 * Private Functions
 */
static bool
is_blocked(shr_sysm_categories_t instance_category,
           int unit,
           bcmmgmt_sysm_comp_id_t *comp_list,
           uint8_t count,
           uint32_t *blocking_component)
{
    int i;
    for (i = 0; i < count; i++) {
        if (!shr_sysm_is_comp_complete(comp_list[i], instance_category, unit)) {
            *blocking_component = comp_list[i];
            return TRUE;
        }
    }
    return FALSE;
}


/* Phase: init
 * Expect Devices will be attached, BD, DRD, LRD will happen here.
 * We can do config-independent init here
 */
static shr_sysm_rv_t
cth_alpm_unit_init(shr_sysm_categories_t instance_category,
                   int unit,
                   void *comp_data,
                   bool warm,
                   uint32_t *blocking_component)
{
    IF_BLOCK_RETURN(instance_category, unit,
                    init_comp_list, blocking_component);

    return SHR_SYSM_RV_DONE;
}

/* Phase: comp_config
 */
static shr_sysm_rv_t
cth_alpm_unit_comp_config(shr_sysm_categories_t instance_category,
                          int unit,
                          void *comp_data,
                          bool warm,
                          uint32_t *blocking_component)
{
    int tmp_rv;
    IF_BLOCK_RETURN(instance_category, unit,
                    init_comp_list, blocking_component);

    tmp_rv = bcmptm_cth_alpm_init(unit, warm);
    if (!warm && SHR_FAILURE(tmp_rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "bcmptm_cth_alpm_init FAILED(rv = %0d)\n"),
             tmp_rv));
        return SHR_SYSM_RV_ERROR;
    }
    return SHR_SYSM_RV_DONE;
}

/* Phase: pre_config
 * Expect Cfg writes.
 * unit-dependent config will happen with LT writes
 */
static shr_sysm_rv_t
cth_alpm_unit_pre_config(shr_sysm_categories_t instance_category,
                         int unit,
                         void *comp_data,
                         bool warm,
                         uint32_t *blocking_component)
{
    IF_BLOCK_RETURN(instance_category, unit,
                    init_comp_list, blocking_component);

    return SHR_SYSM_RV_DONE;
}

/* Phase: run
 * 1. Start all threads as configured.
 */
static void
cth_alpm_unit_run(shr_sysm_categories_t instance_category,
                  int unit,
                  void *comp_data)
{
    return;
}

/* Phase: stop
 */
static shr_sysm_rv_t
cth_alpm_unit_stop(shr_sysm_categories_t instance_category,
                   int unit,
                   void *comp_data,
                   bool graceful,
                   uint32_t *blocking_component)
{
    int tmp_rv;
    IF_BLOCK_RETURN(instance_category, unit,
                    cleanup_comp_list, blocking_component);

    tmp_rv = bcmptm_cth_alpm_stop(unit);
    if (SHR_FAILURE(tmp_rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "bcmptm_cth_alpm_stop FAILED(rv = %0d)\n"),
             tmp_rv));
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/* Phase: shutdown */
static shr_sysm_rv_t
cth_alpm_unit_shutdown(shr_sysm_categories_t instance_category,
                       int unit,
                       void *comp_data,
                       bool graceful,
                       uint32_t *blocking_component)
{
    IF_BLOCK_RETURN(instance_category, unit,
                    cleanup_comp_list, blocking_component);

    if (SHR_FAILURE(bcmptm_cth_alpm_cleanup(unit, graceful))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}


/*******************************************************************************
 * Public Functions
 */
int
bcmptm_cth_alpm_start(void)
{
    shr_sysm_cb_func_set_t funcs;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Register unit-specific functions */
    sal_memset(&funcs, 0, sizeof(funcs));
    funcs.init = cth_alpm_unit_init;
    funcs.comp_config = cth_alpm_unit_comp_config;
    funcs.pre_config = cth_alpm_unit_pre_config;
    funcs.run = cth_alpm_unit_run;
    funcs.stop = cth_alpm_unit_stop;
    funcs.shutdown = cth_alpm_unit_shutdown;
    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_ALPM_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &funcs,
                           NULL));
exit:
    SHR_FUNC_EXIT();
}
