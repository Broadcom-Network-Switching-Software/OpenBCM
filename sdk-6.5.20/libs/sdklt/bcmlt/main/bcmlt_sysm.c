/*! \file bcmlt_sysm.c
 *
 * This module implements the interface to the system manager
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
#include <bcmlt/bcmlt.h>
#include <bcmlt/bcmlt_sysm.h>
#include "bcmlt_internal.h"

#define BSL_LOG_MODULE BSL_LS_BCMLT_ENTRY

/*================================================================*
 *     C O M P O N E N T    I N T E R N A L   F U N C T I O N S   *
 *================================================================*/
static void unit_run(shr_sysm_categories_t instance_cat,
                     int unit,
                     void *comp_data)
{
    bcmlt_tables_db_init(unit);
}

static shr_sysm_rv_t unit_shutdown(shr_sysm_categories_t instance_category,
                                   int unit,
                                   void *comp_data,
                                   bool graceful,
                                   uint32_t *blocking_component)
{
    if (bcmlt_allocated_resources(unit) != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }

    bcmlt_tables_db_delete(unit);

    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t sys_init(shr_sysm_categories_t instance_category,
                          int unit,
                          void *comp_data,
                          bool warm,
                          uint32_t *blocking_component)
{
    if (!shr_sysm_is_comp_complete(BCMMGMT_CFG_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_CFG_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (bcmlt_init() != SHR_E_NONE) {
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
    if (bcmlt_close() != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }
    return SHR_SYSM_RV_DONE;
}

int bcmlt_start(void)
{
    shr_sysm_cb_func_set_t funcs;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    sal_memset(&funcs, 0, sizeof(funcs));

    funcs.init = sys_init;
    funcs.shutdown = sys_shutdown;
    SHR_IF_ERR_EXIT(shr_sysm_register(BCMMGMT_LT_COMP_ID,
                           SHR_SYSM_CAT_GENERIC,
                           &funcs,
                           NULL));

    sal_memset(&funcs, 0, sizeof(funcs));
    funcs.run = unit_run;
    funcs.shutdown = unit_shutdown;
    SHR_IF_ERR_EXIT(shr_sysm_register(BCMMGMT_LT_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &funcs,
                           NULL));

exit:
    SHR_FUNC_EXIT();
}

