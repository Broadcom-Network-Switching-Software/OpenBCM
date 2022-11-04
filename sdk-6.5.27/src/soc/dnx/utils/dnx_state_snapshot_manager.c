
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>

#include <soc/dnx/dbal/dbal_journal.h>
#include <soc/dnx/recovery/rollback_journal.h>
#include <soc/dnx/swstate/dnx_sw_state_journal.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_err_recovery_manager_common.h>
#include <soc/dnxc/dnxc_wb_test_utils.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <soc/dnx/recovery/generic_state_journal.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_ERRORRECOVERY

static uint32
dnx_state_snapshot_manager_entry_seq_id_get(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    dnx_er_db->dnx_state_snapshot_manager->entry_counter++;
    return dnx_er_db->dnx_state_snapshot_manager->entry_counter;
}

static uint8
dnx_state_snapshot_manager_is_on(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    return dnx_err_recovery_is_on(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON);
}

static uint8
dnx_state_snapshot_sw_state_journal_is_on(
    int unit,
    dnx_er_db_t * dnx_er_db)
{

    return (dnx_state_snapshot_manager_is_on(unit, dnx_er_db)
            && !dnx_err_recovery_is_dbal_access_region(unit, dnx_er_db));
}

shr_error_e
dnx_state_snapshot_manager_init(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    dnx_rollback_journal_cbs_t swstate_cbs;
    dnx_rollback_journal_cbs_t dbal_cbs;
    dnx_rollback_journal_cbs_t state_cbs;

    SHR_FUNC_INIT_VARS(unit);

    dnx_er_db->dnx_state_snapshot_manager->is_on_counter = 0;
    dnx_err_recovery_flag_clear(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);

    dnx_er_db->dnx_state_snapshot_manager->entry_counter = 0;
    dnx_er_db->dnx_state_snapshot_manager->is_suppressed_counter = 0;
    dnx_err_recovery_flag_set(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                              DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);

    dnx_err_recovery_flag_set(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                              DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED);

    swstate_cbs.is_on = &dnx_state_snapshot_sw_state_journal_is_on;
    swstate_cbs.seq_id_get = &dnx_state_snapshot_manager_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnx_sw_state_journal_initialize(unit, dnx_er_db, swstate_cbs, NULL, TRUE));

    dbal_cbs.is_on = &dnx_state_snapshot_manager_is_on;
    dbal_cbs.seq_id_get = &dnx_state_snapshot_manager_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnx_dbal_journal_initialize(unit, dnx_er_db, dbal_cbs, NULL, TRUE));

    state_cbs.is_on = &dnx_state_snapshot_manager_is_on;
    state_cbs.seq_id_get = &dnx_state_snapshot_manager_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnx_generic_state_journal_initialize(unit, dnx_er_db, state_cbs, TRUE));

    dnx_err_recovery_flag_set(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                              DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_state_snapshot_manager_deinit(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    dnx_er_db->dnx_state_snapshot_manager->is_on_counter = 0;
    dnx_err_recovery_flag_clear(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);

    dnx_er_db->dnx_state_snapshot_manager->is_suppressed_counter = 0;
    dnx_err_recovery_flag_set(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                              DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);

    dnx_err_recovery_flag_set(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                              DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED);

    rv = dnx_sw_state_journal_destroy(unit, dnx_er_db, TRUE);
    if (rv != _SHR_E_NONE)
    {
        SHR_SET_CURRENT_ERR(rv);
    }

    rv = dnx_dbal_journal_destroy(unit, dnx_er_db, TRUE);
    if (rv != _SHR_E_NONE)
    {
        SHR_SET_CURRENT_ERR(rv);
    }

    rv = dnx_generic_state_journal_destroy(unit, dnx_er_db, TRUE);
    if (rv != _SHR_E_NONE)
    {
        SHR_SET_CURRENT_ERR(rv);
    }

    dnx_err_recovery_flag_clear(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED);

    dnx_er_db->dnx_state_snapshot_manager->entry_counter = 0;

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_state_comparison_start(
    int unit)
{
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_er_db_get(unit, &dnx_er_db));

    SHR_IF_ERR_EXIT(dnx_er_transaction_register(unit));

    SHR_NULL_CHECK(dnx_er_db->dnx_state_snapshot_manager, _SHR_E_INTERNAL, "dnx_state_snapshot_manager");

    dnx_er_db->dnx_state_snapshot_manager->is_on_counter++;
    dnx_err_recovery_flag_set(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                              DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);

#ifdef BCM_WARM_BOOT_SUPPORT

    if (dnx_er_db->dnx_state_snapshot_manager->is_on_counter == 1)
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
    uint8 should_clean = TRUE;
    uint8 differences_exist = FALSE;
    shr_error_e rollback_rv = _SHR_E_NONE;
    dnx_er_db_t *dnx_er_db = NULL;

    uint32 state_entry_seq_id = 0;
    uint32 sw_state_entry_seq_id = 0;
    uint32 dbal_entry_seq_id = 0;

    uint32 seq_id = 1;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        should_clean = FALSE;
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "state snapshot manager ERROR: cannot end and compare comparison journal in transactions where error recovery is NOT_NEEDED/NO_SUPPORT.\n");
    }

    if (dnx_err_recovery_is_validation_enabled(unit))
    {
        if (0 == dnx_er_db->dnx_state_snapshot_manager->is_on_counter)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "snapshot manager ERROR: end called without manager being started .\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
    }

    dnx_er_db->dnx_state_snapshot_manager->is_on_counter--;

    if (0 == dnx_er_db->dnx_state_snapshot_manager->is_on_counter)
    {
        if (dnx_err_recovery_is_validation_enabled(unit))
        {
            if (0 < dnx_er_db->dnx_state_snapshot_manager->is_suppressed_counter)
            {
                DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                            "snapshot manager ERROR: end called while temporary suppressions are on.\n%s%s%s",
                                                            EMPTY, EMPTY, EMPTY);
            }
        }

        dnx_err_recovery_flag_clear(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                    DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);

        while (seq_id <= dnx_er_db->dnx_state_snapshot_manager->entry_counter)
        {

            SHR_IF_ERR_EXIT(dnx_sw_state_journal_get_head_seq_id(unit, dnx_er_db, TRUE, &sw_state_entry_seq_id));
            SHR_IF_ERR_EXIT(dnx_generic_state_journal_get_head_seq_id(unit, dnx_er_db, TRUE, &state_entry_seq_id));
            SHR_IF_ERR_EXIT(dnx_dbal_journal_get_head_seq_id(unit, dnx_er_db, TRUE, &dbal_entry_seq_id));

            if (sw_state_entry_seq_id == seq_id)
            {
                rollback_rv = dnx_sw_state_journal_roll_back(unit, dnx_er_db, TRUE, TRUE);
                if (_SHR_E_NONE != rollback_rv)
                {
                    differences_exist = TRUE;
                }
            }
            else if (state_entry_seq_id == seq_id)
            {
                rollback_rv = dnx_generic_state_journal_roll_back(unit, dnx_er_db, TRUE, TRUE);
                if (_SHR_E_NONE != rollback_rv)
                {
                    differences_exist = TRUE;
                }
            }
            else if (dbal_entry_seq_id == seq_id)
            {
                rollback_rv = dnx_dbal_journal_roll_back(unit, dnx_er_db, TRUE, TRUE);
                if (_SHR_E_NONE != rollback_rv)
                {
                    differences_exist = TRUE;
                }

            }
            seq_id++;
        }

        dnx_er_db->dnx_state_snapshot_manager->entry_counter = 0;

        SHR_IF_ERR_EXIT(dnx_dbal_journal_exclude_table_clear(unit, dnx_er_db, TRUE));
        SHR_IF_ERR_EXIT(dnx_sw_state_journal_exclude_clear(unit, dnx_er_db, TRUE));

#ifdef BCM_WARM_BOOT_SUPPORT

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
        dnx_sw_state_journal_clear(unit, dnx_er_db, TRUE);
        dnx_generic_state_journal_clear(unit, dnx_er_db, TRUE);
        dnx_dbal_journal_clear(unit, dnx_er_db, TRUE);

        dnx_dbal_journal_exclude_table_clear(unit, dnx_er_db, TRUE);
        dnx_sw_state_journal_exclude_clear(unit, dnx_er_db, TRUE);
    }
    SHR_IF_ERR_EXIT(dnx_er_transaction_unregister(unit));
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_state_comparison_suppress(
    int unit,
    uint8 is_suppressed)
{
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        SHR_EXIT();
    }

    if (is_suppressed)
    {
        dnx_er_db->dnx_state_snapshot_manager->is_suppressed_counter++;

        dnx_err_recovery_flag_clear(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                    DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);
    }
    else
    {
        if (0 == dnx_er_db->dnx_state_snapshot_manager->is_suppressed_counter)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "snapshot manager ERROR: attempting to unsuppress without any suppressions done\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);

        }

        dnx_er_db->dnx_state_snapshot_manager->is_suppressed_counter--;

        if (0 == dnx_er_db->dnx_state_snapshot_manager->is_suppressed_counter)
        {
            dnx_err_recovery_flag_set(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                      DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);
        }

    }

exit:
    SHR_FUNC_EXIT;
}

uint8
dnx_state_comparison_is_suppressed(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    if (dnx_er_db->dnx_state_snapshot_manager->is_suppressed_counter != 0)
    {
        return TRUE;
    }
    return FALSE;
}

shr_error_e
dnx_state_comparison_exclude_by_stamp(
    int unit,
    char *stamp,
    int is_swstate)
{
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "state snapshot manager ERROR: cannot create exclusions in transactions where error recovery is NOT_NEEDED/NO_SUPPORT.\n");
    }

    if (is_swstate)
    {
        SHR_IF_ERR_EXIT(dnx_sw_state_journal_exclude_by_stamp(unit, dnx_er_db, stamp, TRUE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_dbal_journal_exclude_table_by_stamp(unit, dnx_er_db, stamp, TRUE));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_state_comparison_print_journal(
    int unit,
    dnx_rollback_journal_subtype_e type)
{
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        SHR_EXIT();
    }

    switch (type)
    {
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE:
            SHR_IF_ERR_EXIT(dnx_sw_state_journal_print(unit, dnx_er_db, TRUE));
            break;
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL:
            SHR_IF_ERR_EXIT(dnx_dbal_journal_print(unit, dnx_er_db, TRUE));
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
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        SHR_EXIT();
    }

    switch (type)
    {
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE:
            SHR_IF_ERR_EXIT(dnx_sw_state_journal_logger_state_change(unit, dnx_er_db, TRUE));
            break;
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL:
            SHR_IF_ERR_EXIT(dnx_dbal_journal_logger_state_change(unit, dnx_er_db, TRUE));
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

#ifdef BCM_WARM_BOOT_SUPPORT
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

#ifdef BCM_WARM_BOOT_SUPPORT
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

#ifdef BCM_WARM_BOOT_SUPPORT
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

#ifdef BCM_WARM_BOOT_SUPPORT
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
#ifdef BCM_WARM_BOOT_SUPPORT
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
