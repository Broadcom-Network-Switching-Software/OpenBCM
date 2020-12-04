/*! \file trm_sysm.c
 *
 * This module implements the interface to the system manager.
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
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmtrm/trm_api.h>
#include <bcmtrm/trm_sysm.h>
#include <bcmtrm/generated/bcmtrm_ha.h>
#include "trm_internal.h"

#define BSL_LOG_MODULE BSL_LS_BCMLT_TABLE


static shr_sysm_rv_t unit_init(shr_sysm_categories_t instance_category,
                               int unit,
                               void *comp_data,
                               bool warm,
                               uint32_t *blocking_component)
{
    if (!shr_sysm_is_comp_complete(BCMMGMT_ISSU_COMP_ID,
                                   instance_category, unit)) {
        *blocking_component = BCMMGMT_ISSU_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }
    if (bcmtrm_unit_init(unit, warm) != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }
    return SHR_SYSM_RV_DONE;
}

/*
 * This function takes care of the warm boot recovery and HA memory allocation
 */
static shr_sysm_rv_t unit_pre_conf(shr_sysm_categories_t instance_category,
                                   int unit,
                                   void *comp_data,
                                   bool warm,
                                   uint32_t *blocking_component)
{

    if (warm) {
        bcmtrm_recover(unit);
    }

    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t unit_stop(shr_sysm_categories_t instance_category,
                               int unit,
                               void *comp_data,
                               bool graceful,
                               uint32_t *blocking_component)
{
    if (bcmtrm_unit_stop(unit, graceful) != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }
    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t unit_shutdown(shr_sysm_categories_t instance_category,
                                   int unit,
                                   void *comp_data,
                                   bool graceful,
                                   uint32_t *blocking_component)
{
    if (!shr_sysm_is_comp_complete(BCMMGMT_LT_COMP_ID,
                                   instance_category, unit)) {
        *blocking_component = BCMMGMT_LT_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }
    if (bcmtrm_unit_delete(unit) != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }
    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t sys_init(shr_sysm_categories_t instance_category,
                              int unit,
                              void *comp_data,
                              bool warm,
                              uint32_t *blocking_component)
{
    if (!shr_sysm_is_comp_complete(BCMMGMT_CFG_COMP_ID,
                                   instance_category, unit)) {
        *blocking_component = BCMMGMT_CFG_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (bcmtrm_init() != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }
    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t sys_shutdown(shr_sysm_categories_t instance_category,
                                  int unit,
                                  void *comp_data,
                                  bool graceful,
                                  uint32_t *blocking_component)
{
    if (bcmtrm_delete() != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }
    return SHR_SYSM_RV_DONE;
}

int bcmtrm_start(void)
{
    shr_sysm_cb_func_set_t funcs;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    sal_memset(&funcs, 0, sizeof(funcs));

    funcs.init = sys_init;
    funcs.shutdown = sys_shutdown;
    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_TRM_COMP_ID,
                           SHR_SYSM_CAT_GENERIC,
                           &funcs,
                           NULL));

    sal_memset(&funcs, 0, sizeof(funcs));

    funcs.init = unit_init;
    funcs.pre_config = unit_pre_conf;
    funcs.stop = unit_stop;
    funcs.shutdown = unit_shutdown;
    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_TRM_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &funcs,
                           NULL));
exit:
    SHR_FUNC_EXIT();
}
