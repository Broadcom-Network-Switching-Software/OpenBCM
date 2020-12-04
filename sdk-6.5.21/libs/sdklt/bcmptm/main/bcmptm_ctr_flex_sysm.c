/*! \file bcmptm_ctr_flex_sysm.c
 *
 * Interface functions to the system manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bcmbd/bcmbd.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmptm/bcmptm_ctr_flex.h>
#include <bcmptm/bcmptm_ctr_flex_sysm.h>

#define BSL_LOG_MODULE BSL_LS_BCMPTM_MISC
#define IF_BLOCK_RETURN(cat, unit, comp_list, block_comp)                   \
    if (is_blocked(cat, unit, comp_list, COUNTOF(comp_list), block_comp)) { \
        return SHR_SYSM_RV_BLOCKED;                                         \
    }

static bcmmgmt_sysm_comp_id_t init_comp_list[] = {
        BCMMGMT_LRD_COMP_ID,
        BCMMGMT_PTM_COMP_ID,
        BCMMGMT_DI_COMP_ID,
        BCMMGMT_SER_COMP_ID
    };
static bcmmgmt_sysm_comp_id_t cleanup_comp_list[] = {
        BCMMGMT_TRM_COMP_ID,
        BCMMGMT_LTM_COMP_ID
    };

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

/*!
 * init phase
 */
static shr_sysm_rv_t
ctr_flex_unit_init(shr_sysm_categories_t instance_category,
                   int unit,
                   void *comp_data,
                   bool warm,
                   uint32_t *blocking_component)
{
    int tmp_rv;

    IF_BLOCK_RETURN(instance_category, unit,
                    init_comp_list, blocking_component);

    tmp_rv = bcmptm_ctr_flex_driver_init(unit, warm);

    if (SHR_FAILURE(tmp_rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "bcmptm_ctr_flex_driver_init FAILED(rv = %0d)\n"),
             tmp_rv));
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * comp_config phase
 */
static shr_sysm_rv_t
ctr_flex_unit_comp_config(shr_sysm_categories_t instance_category,
                          int unit,
                          void *comp_data,
                          bool warm,
                          uint32_t *blocking_component)
{
    int tmp_rv;

    IF_BLOCK_RETURN(instance_category, unit,
                    init_comp_list, blocking_component);

    tmp_rv = bcmptm_ctr_flex_init(unit, warm);
    if (SHR_FAILURE(tmp_rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "bcmptm_ctr_flex_init FAILED(rv = %0d)\n"),
             tmp_rv));
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * shutdown phase
 */
static shr_sysm_rv_t
ctr_flex_unit_shutdown(shr_sysm_categories_t instance_category,
                       int unit,
                       void *comp_data,
                       bool graceful,
                       uint32_t *blocking_component)
{
    IF_BLOCK_RETURN(instance_category, unit,
                    cleanup_comp_list, blocking_component);

    if (SHR_FAILURE(bcmptm_ctr_flex_cleanup(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}


/*!
 * Public Functions
 */
int
bcmptm_ctr_flex_start(void)
{
    shr_sysm_cb_func_set_t funcs;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Register unit-specific functions */
    sal_memset(&funcs, 0, sizeof(funcs));
    funcs.init = ctr_flex_unit_init;
    funcs.pre_config = NULL;
    funcs.comp_config = ctr_flex_unit_comp_config;
    funcs.run = NULL;
    funcs.stop = NULL;
    funcs.shutdown = ctr_flex_unit_shutdown;
    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_CTR_FLEX_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &funcs,
                           NULL));
exit:
    SHR_FUNC_EXIT();
}
