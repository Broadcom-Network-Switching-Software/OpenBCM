/*! \file bcmmgmt_sysm.c
 *
 * Invokes SDK components to register with the System Manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_sem.h>
#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd.h>
#include <bcmmgmt/bcmmgmt_comp.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmlt/bcmlt.h>
#include <bcmmgmt/bcmmgmt_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMMGMT_SYSM

/*******************************************************************************
 * Local variables
 */

/* Semaphore to signal main function from callback function */
static sal_sem_t sync_sem;

/* System Manager status received via callback function */
static int generic_sysm_status;

/*******************************************************************************
 * Local functions
 */

/*
 * Use this function to signal that the generic instance of system manager
 * had completed its initialization.
 */
static void
generic_comp_start_cb(shr_sysm_categories_t instance_cat, int unit, int status)
{
    generic_sysm_status = status;
    sal_sem_give(sync_sem);
}

/*******************************************************************************
 * Public functions
 */

int
bcmmgmt_sysm_components_start(bool warm, const bcmmgmt_comp_list_t *comp_list)
{
    uint32_t idx;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    shr_sysm_comp_name_init(bcmmgmt_comp_name_from_id);

    SHR_IF_ERR_EXIT
        (shr_sysm_category_define(SHR_SYSM_CAT_GENERIC, 1));

    SHR_IF_ERR_EXIT
        (shr_sysm_category_define(SHR_SYSM_CAT_UNIT, BCMDRD_CONFIG_MAX_UNITS));

    SHR_IF_ERR_EXIT
        (shr_sysm_init(BCMMGMT_MAX_COMP_ID, true));

    for (idx = 0; idx < comp_list->num_comp; idx++) {
        if (comp_list->comp_start[idx] == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_EXIT
            (comp_list->comp_start[idx]());
    }

    sync_sem = sal_sem_create("", SAL_SEM_BINARY, 0);
    SHR_NULL_CHECK(sync_sem, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (shr_sysm_instance_new(SHR_SYSM_CAT_GENERIC, 0, warm, false,
                               generic_comp_start_cb));

    /* Block until the generic instance has initialized */
    sal_sem_take(sync_sem, SAL_SEM_FOREVER);

    /* Return status of starting the generic instance */
    SHR_IF_ERR_EXIT(generic_sysm_status);

exit:
    SHR_FUNC_EXIT();
}

int
bcmmgmt_sysm_components_stop(bool graceful)
{
    int rv;

    if (sync_sem == NULL) {
        /* Not running */
        return SHR_E_NONE;
    }

    rv = shr_sysm_instance_stop(SHR_SYSM_CAT_GENERIC, 0, graceful,
                                generic_comp_start_cb);
    if (SHR_SUCCESS(rv)) {
        sal_sem_take(sync_sem, SAL_SEM_FOREVER);
        rv = generic_sysm_status;
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Failed to stop generic SYSM instance erro=%d\n"),
                   rv));
    }
    sal_sem_destroy(sync_sem);
    sync_sem = NULL;

    if (SHR_FAILURE(rv) && rv != SHR_E_PARTIAL) {
        return rv;
    }
    shr_sysm_instance_delete(SHR_SYSM_CAT_GENERIC, 0);

    /* Delete the system manager */
    shr_sysm_delete();

    return rv;
}
