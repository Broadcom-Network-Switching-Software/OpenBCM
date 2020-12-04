
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_ERR_RECOVERY_MANAGER_UTILS_H

#define _DNX_ERR_RECOVERY_MANAGER_UTILS_H

#include <soc/types.h>
#include <soc/error.h>
#include <assert.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/recovery/rollback_journal_utils.h>

#define DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION 20

typedef struct dnx_er_threading_d
{

    uint8 is_initialized;

    sal_mutex_t mtx;

    volatile sal_thread_t transaction_to_thread[DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION];

    volatile int counter[DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION];

    volatile int tmp_excluded_counter[DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION];

} dnx_er_threading_t;

shr_error_e dnx_er_threading_initialize(
    int unit);

shr_error_e dnx_er_threading_deinitialize(
    int unit);

shr_error_e dnx_er_threading_transaction_register(
    int unit);

shr_error_e dnx_er_threading_transaction_unregister(
    int unit);

int dnx_er_threading_transaction_get(
    int unit);

shr_error_e dnx_er_threading_tmp_excluded_add(
    int unit);

shr_error_e dnx_er_threading_tmp_excluded_remove(
    int unit);

int dnx_er_threading_is_tmp_excluded(
    int unit);

#endif
