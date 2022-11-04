
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <soc/defs.h>
#include <soc/dnx/recovery/rollback_journal.h>
#include <soc/dnx/dnx_err_recovery_manager_common.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dev_init.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_ERRORRECOVERY

static uint32 dnx_err_recovery_common_enabled[SOC_MAX_NUM_DEVICES] = { 0 };

shr_error_e
dnx_err_recovery_common_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_err_recovery_common_enabled[unit] = (dnx_data_dev_init.ha.error_recovery_support_get(unit) > 0) ? TRUE : FALSE;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_err_recovery_common_enable_disable_set(
    int unit,
    uint32 is_enable)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_err_recovery_common_enabled[unit] = is_enable;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

uint8
dnx_err_recovery_common_is_enabled(
    int unit)
{
    return dnx_err_recovery_common_enabled[unit] || dnx_rollback_journal_is_error_recovery_bypassed(unit);
}

#undef _ERR_MSG_MODULE_NAME
