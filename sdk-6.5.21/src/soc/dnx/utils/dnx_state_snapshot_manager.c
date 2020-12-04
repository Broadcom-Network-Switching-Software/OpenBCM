
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

#include <soc/dnx/dbal/dbal_journal.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnx/recovery/rollback_journal.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_er_threading.h>
#include <soc/dnx/dnx_err_recovery_manager_common.h>
#include <soc/dnxc/dnxc_wb_test_utils.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_ERRORRECOVERY

#define DNX_STATE_SNAPSHOT_MANAGER_EXIT_IF_NOT_INITIALIZED(unit)  \
do{                                                               \
    if (TRUE != dnx_state_snapshot_manager[unit].is_initialized) {\
        SHR_EXIT();                                               \
    }                                                             \
}while(0)

dnx_state_snapshot_manager_t dnx_state_snapshot_manager[SOC_MAX_NUM_DEVICES] = { {0}
};

static uint32
dnx_state_snapshot_manager_entry_seq_id_get(
    int unit,
    int er_thread_id)
{
    dnx_state_snapshot_manager[unit].entry_counter[er_thread_id]++;
    return dnx_state_snapshot_manager[unit].entry_counter[er_thread_id];
}

static uint8
dnx_state_snapshot_manager_is_on(
    int unit,
    int er_thread_id)
{
    return dnx_err_recovery_common_is_on(unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON);
}

static uint8
dnx_state_snapshot_sw_state_journal_is_on(
    int unit,
    int er_thread_id)
{

    return (dnx_state_snapshot_manager_is_on(unit, er_thread_id)
            && !dnx_err_recovery_is_dbal_access_region(unit, er_thread_id));
}

shr_error_e
dnx_state_snapshot_manager_init(
    int unit)
{
    dnx_rollback_journal_cbs_t swstate_cbs;
    dnx_rollback_journal_cbs_t dbal_cbs;
    int idx = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (idx = 0; idx < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; idx++)
    {
        dnx_state_snapshot_manager[unit].is_on_counter[idx] = 0;
        dnx_err_recovery_common_flag_clear(unit, idx, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                           DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);

        dnx_state_snapshot_manager[unit].entry_counter[idx] = 0;
        dnx_state_snapshot_manager[unit].is_suppressed_counter[idx] = 0;
        dnx_err_recovery_common_flag_set(unit, idx, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                         DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);

        dnx_err_recovery_common_flag_set(unit, idx, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                         DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED);
    }

    swstate_cbs.is_on = &dnx_state_snapshot_sw_state_journal_is_on;
    swstate_cbs.seq_id_get = &dnx_state_snapshot_manager_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_initialize(unit, swstate_cbs, NULL, TRUE));

    dbal_cbs.is_on = &dnx_state_snapshot_manager_is_on;
    dbal_cbs.seq_id_get = &dnx_state_snapshot_manager_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnx_dbal_journal_initialize(unit, dbal_cbs, NULL, TRUE));

    dnx_state_snapshot_manager[unit].is_initialized = TRUE;

    for (idx = 0; idx < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; idx++)
    {
        dnx_err_recovery_common_flag_set(unit, idx, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                         DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_state_snapshot_manager_deinit(
    int unit)
{
    int idx = 0;
    SHR_FUNC_INIT_VARS(unit);

    DNX_STATE_SNAPSHOT_MANAGER_EXIT_IF_NOT_INITIALIZED(unit);

    for (idx = 0; idx < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; idx++)
    {
        dnx_state_snapshot_manager[unit].is_on_counter[idx] = 0;
        dnx_err_recovery_common_flag_clear(unit, idx, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                           DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);

        dnx_state_snapshot_manager[unit].is_suppressed_counter[idx] = 0;
        dnx_err_recovery_common_flag_set(unit, idx, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                         DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);

        dnx_err_recovery_common_flag_set(unit, idx, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                         DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED);
    }

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_destroy(unit, TRUE));

    SHR_IF_ERR_EXIT(dnx_dbal_journal_destroy(unit, TRUE));

    dnx_state_snapshot_manager[unit].is_initialized = FALSE;
    for (idx = 0; idx < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; idx++)
    {
        dnx_err_recovery_common_flag_clear(unit, idx, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                           DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED);

        dnx_state_snapshot_manager[unit].entry_counter[idx] = 0;
    }
exit:

    if (dnx_state_snapshot_manager[unit].is_initialized)
    {
        dnx_state_snapshot_manager[unit].is_initialized = FALSE;
        for (idx = 0; idx < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; idx++)
        {
            dnx_err_recovery_common_flag_clear(unit, idx, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                               DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED);
        }
    }
    for (idx = 0; idx < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; idx++)
    {
        if (dnx_state_snapshot_manager[unit].entry_counter[idx] != 0)
        {
            dnx_state_snapshot_manager[unit].entry_counter[idx] = 0;
        }
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_state_comparison_start(
    int unit)
{
    int er_thread_id = dnx_er_threading_transaction_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNX_STATE_SNAPSHOT_MANAGER_EXIT_IF_NOT_INITIALIZED(unit);

    if (er_thread_id == -1)
    {
        SHR_IF_ERR_EXIT(dnx_er_threading_transaction_register(unit));
        er_thread_id = dnx_er_threading_transaction_get(unit);
        assert(er_thread_id != -1);
    }

    dnx_state_snapshot_manager[unit].is_on_counter[er_thread_id]++;
    dnx_err_recovery_common_flag_set(unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                     DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);

#ifdef BCM_WARM_BOOT_API_TEST

    if (dnx_state_snapshot_manager[unit].is_on_counter[er_thread_id] == 1)
    {
        dnxc_wb_no_wb_test_set(unit, 1);
    }
#endif

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_state_comparison_end_and_compare(
    int unit)
{
    uint32 seq_id = 0;

    uint8 should_clean = TRUE;
    uint8 differences_exist = FALSE;
    shr_error_e rollback_rv = _SHR_E_NONE;
    int er_thread_id = dnx_er_threading_transaction_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    assert(er_thread_id != -1);

    DNX_STATE_SNAPSHOT_MANAGER_EXIT_IF_NOT_INITIALIZED(unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (0 == dnx_state_snapshot_manager[unit].is_on_counter[er_thread_id])
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "snapshot manager ERROR: end called without manager being started .\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }
#endif

    dnx_state_snapshot_manager[unit].is_on_counter[er_thread_id]--;

    if (0 == dnx_state_snapshot_manager[unit].is_on_counter[er_thread_id])
    {

#ifdef DNX_ERR_RECOVERY_VALIDATION
        if (0 < dnx_state_snapshot_manager[unit].is_suppressed_counter[er_thread_id])
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "snapshot manager ERROR: end called while temporary suppressions are on.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
#endif

        dnx_err_recovery_common_flag_clear(unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                           DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);

        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_get_head_seq_id(unit, er_thread_id, TRUE, &seq_id));

        while (0 != seq_id)
        {
            rollback_rv = dnxc_sw_state_journal_roll_back(unit, er_thread_id, TRUE, TRUE);
            if (_SHR_E_NONE != rollback_rv)
            {
                differences_exist = TRUE;
            }

            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_get_head_seq_id(unit, er_thread_id, TRUE, &seq_id));
        }

        SHR_IF_ERR_EXIT(dnx_dbal_journal_get_head_seq_id(unit, er_thread_id, TRUE, &seq_id));

        while (0 != seq_id)
        {
            rollback_rv = dnx_dbal_journal_roll_back(unit, er_thread_id, TRUE, TRUE);
            if (_SHR_E_NONE != rollback_rv)
            {
                differences_exist = TRUE;
            }

            SHR_IF_ERR_EXIT(dnx_dbal_journal_get_head_seq_id(unit, er_thread_id, TRUE, &seq_id));
        }

        dnx_state_snapshot_manager[unit].entry_counter[er_thread_id] = 0;

        SHR_IF_ERR_EXIT(dnx_dbal_journal_exclude_table_clear(unit, er_thread_id, TRUE));
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_exclude_clear(unit, er_thread_id, TRUE));

#ifdef BCM_WARM_BOOT_API_TEST

        dnxc_wb_no_wb_test_set(unit, 0);
#endif
    }

    should_clean = FALSE;

    if (differences_exist)
    {
        SHR_IF_ERR_EXIT_NO_MSG(SOC_E_INTERNAL);
    }
exit:
    if (should_clean)
    {
        dnxc_sw_state_journal_clear(unit, er_thread_id, FALSE, TRUE);
        dnx_dbal_journal_clear(unit, er_thread_id, FALSE, TRUE);

        dnx_dbal_journal_exclude_table_clear(unit, er_thread_id, TRUE);
        dnxc_sw_state_journal_exclude_clear(unit, er_thread_id, TRUE);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_state_comparison_suppress(
    int unit,
    uint8 is_suppressed)
{
    int er_thread_id = dnx_er_threading_transaction_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    if (er_thread_id == -1)
    {
        SHR_EXIT();
    }

    DNX_STATE_SNAPSHOT_MANAGER_EXIT_IF_NOT_INITIALIZED(unit);

    if (is_suppressed)
    {
        dnx_state_snapshot_manager[unit].is_suppressed_counter[er_thread_id]++;

        dnx_err_recovery_common_flag_clear(unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                           DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);
    }
    else
    {
        if (0 == dnx_state_snapshot_manager[unit].is_suppressed_counter[er_thread_id])
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "snapshot manager ERROR: attempting to unsuppress without any suppressions done\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);

        }

        dnx_state_snapshot_manager[unit].is_suppressed_counter[er_thread_id]--;

        if (0 == dnx_state_snapshot_manager[unit].is_suppressed_counter[er_thread_id])
        {
            dnx_err_recovery_common_flag_set(unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                             DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);
        }

    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_state_comparison_exclude_by_stamp(
    int unit,
    char *stamp,
    int is_swstate)
{
    int er_thread_id = dnx_er_threading_transaction_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    assert(er_thread_id != -1);

    if (is_swstate)
    {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_exclude_by_stamp(unit, er_thread_id, stamp, TRUE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_dbal_journal_exclude_table_by_stamp(unit, er_thread_id, stamp, TRUE));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_state_comparison_print_journal(
    int unit,
    dnx_rollback_journal_subtype_e type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE:
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_print(unit, TRUE));
            break;
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL:
            SHR_IF_ERR_EXIT(dnx_dbal_journal_print(unit, TRUE));
            break;
        default:
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "snapshot manager ERROR:: unrecognized journal type.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_state_comparison_journal_logger_state_change(
    int unit,
    dnx_rollback_journal_subtype_e type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE:
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_logger_state_change(unit, TRUE));
            break;
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL:
            SHR_IF_ERR_EXIT(dnx_dbal_journal_logger_state_change(unit, TRUE));
            break;
        default:
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "snapshot manager ERROR:: unrecognized journal type.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnx_tcl_state_comparison_start(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_WARM_BOOT_API_TEST
    {
        int wb_enable;
        dnxc_wb_test_mode_get(unit, &wb_enable);
        if (wb_enable)
        {
            return 0;
        }
    }
#endif
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 1));
    SHR_IF_ERR_EXIT(dnx_state_comparison_start(unit));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 0));
exit:
    SHR_FUNC_EXIT;
}

int
dnx_tcl_state_comparison_end_and_compare(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_WARM_BOOT_API_TEST
    {
        int wb_enable;
        dnxc_wb_test_mode_get(unit, &wb_enable);
        if (wb_enable)
        {
            return 0;
        }
    }
#endif
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 1));
    SHR_IF_ERR_EXIT(dnx_state_comparison_end_and_compare(unit));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 0));
exit:
    SHR_FUNC_EXIT;
}

int
dnx_tcl_state_comparison_suppress(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_WARM_BOOT_API_TEST
    {
        int wb_enable;
        dnxc_wb_test_mode_get(unit, &wb_enable);
        if (wb_enable)
        {
            return 0;
        }
    }
#endif
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 1));
    SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, TRUE));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 0));
exit:
    SHR_FUNC_EXIT;
}

int
dnx_tcl_state_comparison_unsuppress(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_WARM_BOOT_API_TEST
    {
        int wb_enable;
        dnxc_wb_test_mode_get(unit, &wb_enable);
        if (wb_enable)
        {
            return 0;
        }
    }
#endif
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 1));
    SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, FALSE));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 0));
exit:
    SHR_FUNC_EXIT;
}

int
dnx_tcl_state_comparison_exclude_by_stamp(
    int unit,
    char *stamp,
    int is_swstate)
{

    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_WARM_BOOT_API_TEST
    {
        int wb_enable;
        dnxc_wb_test_mode_get(unit, &wb_enable);
        if (wb_enable)
        {
            return 0;
        }
    }
#endif
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 1));
    SHR_IF_ERR_EXIT(dnx_state_comparison_exclude_by_stamp(unit, stamp, is_swstate));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 0));
exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
