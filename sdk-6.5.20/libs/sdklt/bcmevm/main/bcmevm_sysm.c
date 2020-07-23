/*! \file bcmevm_sysm.c
 *
 *  Event manager interface to system manager.
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
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmevm/bcmevm_sysm.h>
#include "bcmevm_internal.h"

#define BSL_LOG_MODULE BSL_LS_BCMEVM_EVENT

static shr_sysm_rv_t unit_init(shr_sysm_categories_t instance_category,
                               int unit,
                               void *comp_data,
                               bool warm,
                               uint32_t *blocking_component)
{
    if (bcmevm_unit_init(unit) != SHR_E_NONE) {
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
    if (bcmevm_unit_shutdown(unit) != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }
    return SHR_SYSM_RV_DONE;
}


int bcmevm_start(void)
{
    shr_sysm_cb_func_set_t funcs;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    sal_memset(&funcs, 0, sizeof(funcs));

    funcs.init = unit_init;
    funcs.shutdown = unit_shutdown;
    SHR_IF_ERR_EXIT(shr_sysm_register(BCMMGMT_EVM_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &funcs,
                           NULL));
exit:
    SHR_FUNC_EXIT();
}
