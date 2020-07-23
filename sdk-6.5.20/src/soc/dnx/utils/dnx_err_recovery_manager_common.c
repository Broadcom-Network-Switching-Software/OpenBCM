
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <soc/drv.h>
#include <soc/dnx/recovery/rollback_journal.h>
#include <soc/dnx/dnx_er_threading.h>
#include <soc/dnx/dnx_err_recovery_manager_common.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dev_init.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_ERRORRECOVERY


static dnx_err_recovery_common_root_t dnx_err_recovery_common_root[SOC_MAX_NUM_DEVICES] = { {0} };


shr_error_e
dnx_err_recovery_common_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    dnx_err_recovery_common_root[unit].is_enable =
        (dnx_data_dev_init.ha.error_recovery_support_get(unit) > 0) ? TRUE : FALSE;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_err_recovery_common_enable_disable(
    int unit,
    uint32 is_enable)
{
    SHR_FUNC_INIT_VARS(unit);
    dnx_err_recovery_common_root[unit].is_enable = is_enable;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


uint8
dnx_err_recovery_common_is_enabled(
    int unit)
{
    return dnx_err_recovery_common_root[unit].is_enable || dnx_rollback_journal_is_error_recovery_bypassed(unit);
}


uint8
dnx_err_recovery_common_is_on(
    int unit,
    dnx_rollback_journal_type_e mngr_id)
{
    return (dnx_err_recovery_common_is_enabled(unit)
            && dnx_rollback_journal_is_done_init(unit)
            && !SOC_IS_DETACHING(unit)
            && !dnx_rollback_journal_is_any_journal_rolling_back(unit)
            && DNX_ERR_RECOVERY_COMMON_IS_ON_ALL == dnx_err_recovery_common_root[unit].managers[mngr_id].bitmap);
}


uint8
dnx_err_recovery_common_flag_is_on(
    int unit,
    dnx_rollback_journal_type_e mngr_id,
    uint32 flag)
{
    return (dnx_err_recovery_common_root[unit].managers[mngr_id].bitmap & flag) ? TRUE : FALSE;
}


uint8
dnx_err_recovery_common_flag_mask_is_on(
    int unit,
    dnx_rollback_journal_type_e mngr_id,
    uint32 mask)
{
    return ((dnx_err_recovery_common_root[unit].managers[mngr_id].bitmap & mask) == mask) ? TRUE : FALSE;
}


shr_error_e
dnx_err_recovery_common_flag_set(
    int unit,
    dnx_rollback_journal_type_e mngr_id,
    uint32 flag)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_err_recovery_common_root[unit].managers[mngr_id].bitmap |= flag;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_err_recovery_common_flag_clear(
    int unit,
    dnx_rollback_journal_type_e mngr_id,
    uint32 flag)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_err_recovery_common_root[unit].managers[mngr_id].bitmap &= ~(flag);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


uint8
dnx_err_recovery_common_is_dbal_access_region(
    int unit)
{
    return (dnx_err_recovery_common_root[unit].dbal_access_counter > 0);
}


shr_error_e
dnx_err_recovery_common_dbal_access_region_start(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_err_recovery_common_root[unit].dbal_access_counter++;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_err_recovery_common_dbal_access_region_end(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (0 == dnx_err_recovery_common_root[unit].dbal_access_counter)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "err recovery manager ERROR: attempted to end dbal access region without start.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    dnx_err_recovery_common_root[unit].dbal_access_counter--;

exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
