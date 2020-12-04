/*! \file bcmpc_mutex.c
 *
 * The mutex utilities for PC internal use.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>

#include <sal/sal_mutex.h>

#include <bcmpc/bcmpc_util_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_DEV

/*! Mutex for each unit. */
static sal_mutex_t pc_mutex[BCMPC_NUM_UNITS_MAX];


/*******************************************************************************
 * Internal public functions
 */

int
bcmpc_phymod_bus_mutex_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (pc_mutex[unit]) {
        SHR_EXIT();
    }

    pc_mutex[unit] = sal_mutex_create("bcmpcPhyBus");
    SHR_NULL_CHECK(pc_mutex[unit], SHR_E_MEMORY);

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_phymod_bus_mutex_cleanup(int unit)
{
    if (!pc_mutex[unit]) {
        return;
    }

    sal_mutex_destroy(pc_mutex[unit]);
    pc_mutex[unit] = NULL;
}

int
bcmpc_phymod_bus_mutex_take(void *user_acc)
{
    bcmpc_phy_access_data_t *user_data = user_acc;
    int unit;
    sal_mutex_t mutex;

    if (!user_acc) {
        return SHR_E_FAIL;
    }

    unit = user_data->unit;
    mutex = pc_mutex[unit];
    if (!mutex) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Mutex is not allocated.\n")));
        return SHR_E_FAIL;
    }

    return sal_mutex_take(mutex, SAL_MUTEX_FOREVER);
}

int
bcmpc_phymod_bus_mutex_give(void *user_acc)
{
    bcmpc_phy_access_data_t *user_data = user_acc;
    int unit;
    sal_mutex_t mutex;

    if (!user_acc) {
        return SHR_E_FAIL;
    }

    unit = user_data->unit;
    mutex = pc_mutex[unit];
    if (!mutex) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Mutex is not allocated.\n")));
        return SHR_E_FAIL;
    }

    return sal_mutex_give(mutex);
}
