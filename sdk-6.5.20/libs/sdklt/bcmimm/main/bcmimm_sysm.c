/*! \file bcmimm_sysm.c
 *
 * This module implements the interface of the in-memory front-end component
 * to the system manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bcmdrd_config.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmimm/bcmimm_sysm.h>
#include <bcmimm/generated/front_e_ha.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmimm/bcmimm_backend.h>
#include <bcmimm/bcmimm_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMIMM_FRONTEND

static bool bcmimm_warm;

static shr_sysm_rv_t unit_init(shr_sysm_categories_t instance_category,
                               int unit,
                               void *comp_data,
                               bool warm,
                               uint32_t *blocking_component)
{
    if (bcmimm_internal_unit_init(unit) != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }
    bcmimm_warm = warm;
    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t unit_comp_config(shr_sysm_categories_t instance_category,
                                      int unit,
                                      void *comp_data,
                                      bool warm,
                                      uint32_t *blocking_component)
{
    if (bcmimm_unit_init(unit, BCMMGMT_IMM_FE_COMP_ID, warm) == SHR_E_NONE) {
        return SHR_SYSM_RV_DONE;
    }
    return SHR_SYSM_RV_ERROR;
}

static shr_sysm_rv_t unit_shutdown(shr_sysm_categories_t instance_category,
                                  int unit,
                                  void *comp_data,
                                  bool graceful,
                                  uint32_t *blocking_component)
{
    bcmimm_unit_shutdown(unit);
    bcmimm_be_cleanup(unit);
    bcmimm_internal_unit_shutdown(unit);
    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t global_init(shr_sysm_categories_t instance_category,
                                 int unit,
                                 void *comp_data,
                                 bool cold,
                                 uint32_t *blocking_component)
{
    if (bcmimm_internal_init() == SHR_E_NONE)
        return SHR_SYSM_RV_DONE;
    else
        return SHR_SYSM_RV_ERROR;
}

static shr_sysm_rv_t global_shutdown(shr_sysm_categories_t instance_category,
                                     int unit,
                                     void *comp_data,
                                     bool graceful,
                                     uint32_t *blocking_component)
{
    bcmimm_internal_shutdown();
    return SHR_SYSM_RV_DONE;
}

int bcmimm_start(void)
{
    shr_sysm_cb_func_set_t funcs;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    sal_memset(&funcs, 0, sizeof(funcs));

    funcs.init = global_init;
    funcs.shutdown = global_shutdown;
    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_IMM_FE_COMP_ID,
                           SHR_SYSM_CAT_GENERIC,
                           &funcs,
                           NULL));

    sal_memset(&funcs, 0, sizeof(funcs));
    funcs.init = unit_init;
    funcs.comp_config = unit_comp_config;
    funcs.shutdown = unit_shutdown;
    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_IMM_FE_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &funcs,
                           NULL));
exit:
    SHR_FUNC_EXIT();
}

bool bcmimm_is_warm()
{
   return bcmimm_warm;
}
