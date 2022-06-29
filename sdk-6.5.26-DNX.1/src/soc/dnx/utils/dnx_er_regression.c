
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <soc/drv.h>
#include <soc/dnx/dnx_er_regression.h>
#include <soc/dnx/dnx_err_recovery_manager_common.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnx/recovery/generic_state_journal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnx/dnx_er_db.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dev_init.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_ERRORRECOVERY

#define DNX_ER_DEBUG_MODE 2

dnx_er_regress_t dnx_er_regression[SOC_MAX_NUM_DEVICES] = { {0}
};

static uint8
dnx_er_regress_is_top_level_api(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    return (1 == dnx_er_db->dnx_err_recovery_manager->api_counter);
}

static uint8
dnx_er_regress_is_top_level_api_transaction(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    uint32 trans_num_before_api;

    trans_num_before_api = dnx_er_db->dnx_err_recovery_manager->api_to_trans_map[0];

    return (dnx_er_db->dnx_err_recovery_manager->transaction_counter == (trans_num_before_api + 1));
}

static uint8
dnx_er_regress_is_outside_of_transaction(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    return (0 == dnx_er_db->dnx_err_recovery_manager->transaction_counter);
}

static uint8
dnx_er_regress_testing_conditions_are_met(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    return (SOC_UNIT_VALID(unit)
            && SOC_IS_DONE_INIT(unit)
            && !SOC_IS_DETACHING(unit)
            && dnx_err_recovery_flag_mask_is_on(unit,
                                                dnx_er_db,
                                                DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                                DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED
                                                | DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED
                                                | DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED)
            && !dnx_rollback_journal_is_any_journal_rolling_back(unit, dnx_er_db)
            && dnx_er_regress_is_top_level_api(unit, dnx_er_db)
            && dnx_er_regression[unit].is_appl_init_done
            && dnx_er_regression[unit].is_testing_enabled
            && (!dnx_er_db->dnx_err_recovery_manager->trans_info.is_api_excluded));
}

static void
dnx_er_regress_system_state_modified(
    int unit,
    dnx_er_db_t * dnx_er_db)
{

    if (dnx_er_regress_testing_conditions_are_met(unit, dnx_er_db)
        && dnx_er_regress_is_outside_of_transaction(unit, dnx_er_db))
    {
        dnx_er_db->dnx_err_recovery_manager->trans_info.has_access_outside_trans = TRUE;

        if (0 != dnx_er_db->dnx_err_recovery_manager->trans_info.top_level_trans_in_api_count)
        {
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U
                                      (unit,
                                       "error recovery regression ERROR: hw or swstate access outside of transaction\n")));
            assert(0);
        }
    }
}

static uint8
dnx_err_recovery_regress_is_err_type_suppressed(
    int unit,
    dnx_er_db_t * dnx_er_db,
    dnx_err_recovery_regress_error_t type)
{
    return (dnx_er_db->dnx_err_recovery_manager->trans_info.err_type_suppress_counters[type] > 0);
}

static uint8
dnx_err_recovery_is_top_level_trans_in_top_level_api(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    return (dnx_er_regress_is_top_level_api(unit, dnx_er_db)
            && dnx_er_regress_is_top_level_api_transaction(unit, dnx_er_db));
}

static uint8
dnx_er_regress_is_first_test_iteration(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    return (dnx_er_regress_testing_conditions_are_met(unit, dnx_er_db)
            && dnx_err_recovery_is_top_level_trans_in_top_level_api(unit, dnx_er_db)
            && (!dnx_er_db->dnx_err_recovery_manager->trans_info.is_api_ran));
}

static uint8
dnx_er_regress_is_second_test_iteration(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    return (dnx_er_regress_testing_conditions_are_met(unit, dnx_er_db)
            && dnx_err_recovery_is_top_level_trans_in_top_level_api(unit, dnx_er_db)
            && (dnx_er_db->dnx_err_recovery_manager->trans_info.is_api_ran));
}

static void
dnx_er_regress_print_api_and_iteration(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 is_begin)
{
    if (is_begin)
    {
        if (dnx_er_regress_is_first_test_iteration(unit, dnx_er_db))
        {
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "**** ER BCM API %s **** ERROR SIMULATION RUN ****\n"),
                                      dnx_er_db->dnx_err_recovery_manager->trans_info.dnx_er_regress_testing_api_name));
        }
        else if (dnx_er_regress_is_second_test_iteration(unit, dnx_er_db))
        {
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "**** ER BCM API %s **** NORMAL RUN ****\n"),
                                      dnx_er_db->dnx_err_recovery_manager->trans_info.dnx_er_regress_testing_api_name));
        }
    }
    else
    {
        if (dnx_er_regress_is_second_test_iteration(unit, dnx_er_db))
        {
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "**** ER BCM test finished successfully ****\n")));
        }
    }
}

static void
dnx_er_regress_init_deinit_counters_reset(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    uint32 idx = 0;

    for (idx = 0; idx < DNX_ERR_RECOVERY_REGRESS_ERROR_NOF; idx++)
    {
        dnx_er_db->dnx_err_recovery_manager->trans_info.err_type_suppress_counters[idx] = 0;
    }

    dnx_er_regression[unit].is_appl_init_done = FALSE;

    dnx_er_regression[unit].is_testing_enabled = FALSE;
    dnx_er_db->dnx_err_recovery_manager->trans_info.is_api_excluded = FALSE;
    dnx_er_db->dnx_err_recovery_manager->trans_info.has_access_outside_trans = FALSE;
}

void
dnx_er_regress_init(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    const char *err_recovery_system_name = "ERROR-RECOVERY-REGRESSION";

    dnx_er_regress_init_deinit_counters_reset(unit, dnx_er_db);

    if ((NULL != dnx_data_device.regression.regression_parms_get(unit)->system_mode_name)
        && (sal_strncmp(dnx_data_device.regression.regression_parms_get(unit)->system_mode_name,
                        err_recovery_system_name, sal_strnlen(err_recovery_system_name,
                                                              SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)) == 0))
    {
        dnx_er_regression[unit].is_testing_enabled = TRUE;
    }
}

void
dnx_er_regress_deinit(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    dnx_er_regress_init_deinit_counters_reset(unit, dnx_er_db);
}

void
dnx_er_regress_top_level_api_flags_reset(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    dnx_er_db->dnx_err_recovery_manager->trans_info.top_level_trans_in_api_count = 0;
    dnx_er_db->dnx_err_recovery_manager->trans_info.is_api_excluded = FALSE;
    dnx_er_db->dnx_err_recovery_manager->trans_info.is_api_ran = FALSE;
    dnx_er_db->dnx_err_recovery_manager->trans_info.has_access_outside_trans = FALSE;
}

shr_error_e
dnx_er_regress_comparison_tmp_allow(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 is_on)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_regress_is_first_test_iteration(unit, dnx_er_db))
    {
        SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, !is_on));
    }

exit:
    SHR_FUNC_EXIT;
}

static void
dnx_er_regress_transaction_start_top_level_verify(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    if (dnx_er_db->dnx_err_recovery_manager->trans_info.has_access_outside_trans)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U
                                   (unit, "#### #### #### #### ####"
                                    "#### Error Recovery regression ERROR: hw or swstate access outside of transaction ####\n"
                                    "#### It was detected that the current API opt in to Error Recovery but perform some HW or sw state\n"
                                    "     access before it started the transaction or after it was already terminated\n"
                                    "     most common cause for it is when an API does not opt in for Error Recovery but calls another\n"
                                    "     API that does opt in. to fix it you should either opt in also for the outer API, or alternatively\n"
                                    "     use the macro DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN/END on the outer API, more details in confluence page.\n"
                                    "#### #### #### ##### ####")));
        assert(0);
    }
}

shr_error_e
dnx_er_regress_transaction_start(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 is_top_level_transaction)
{

    SHR_FUNC_INIT_VARS(unit);

    if (is_top_level_transaction && dnx_er_regress_testing_conditions_are_met(unit, dnx_er_db))
    {
        dnx_er_regress_transaction_start_top_level_verify(unit, dnx_er_db);
    }

    if (dnx_er_regress_is_first_test_iteration(unit, dnx_er_db))
    {
        SHR_IF_ERR_EXIT(dnx_state_comparison_start(unit));
    }

    if (dnx_data_dev_init.ha.error_recovery_support_get(unit) == DNX_ER_DEBUG_MODE)
    {
        dnx_er_regress_print_api_and_iteration(unit, dnx_er_db, TRUE);
    }

    if (dnx_err_recovery_is_top_level_trans_in_top_level_api(unit, dnx_er_db))
    {
        dnx_er_db->dnx_err_recovery_manager->trans_info.top_level_trans_in_api_count++;

        if (dnx_er_regress_testing_conditions_are_met(unit, dnx_er_db)
            && (dnx_er_db->dnx_err_recovery_manager->trans_info.top_level_trans_in_api_count > 1))
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U
                                       (unit,
                                        "err recovery ERROR: Multiple top-level transactions in a top level bcm API detected. Either opt-in the API for ER or exclude it from the ER regression algorithm.\n")));
            assert(0);
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

uint8
dnx_er_regress_transaction_end_should_abort(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    return dnx_er_regress_is_first_test_iteration(unit, dnx_er_db);
}

shr_error_e
dnx_er_regress_transaction_end(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_dev_init.ha.error_recovery_support_get(unit) == DNX_ER_DEBUG_MODE)
    {
        dnx_er_regress_print_api_and_iteration(unit, dnx_er_db, FALSE);
    }

    if (dnx_er_regress_is_first_test_iteration(unit, dnx_er_db))
    {
        SHR_IF_ERR_EXIT(dnx_state_comparison_end_and_compare(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

void
dnx_er_regress_journal_start(
    int unit)
{

    dnx_er_regress_testing_api_name_update(unit, "ERROR RECOVERY CLEANUP");
}

void
dnx_er_regress_appl_init_state_change(
    int unit,
    uint8 is_init)
{
    dnx_er_regression[unit].is_appl_init_done = (is_init) ? TRUE : FALSE;
}

void
dnx_er_regress_testing_api_name_update(
    int unit,
    const char *func_name)
{
    dnx_er_db_t *dnx_er_db = NULL;

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        return;
    }

    if (!dnx_err_recovery_are_prerequisites_met(unit, dnx_er_db))
    {
        return;
    }

    if (dnx_er_regress_testing_conditions_are_met(unit, dnx_er_db))
    {
        sal_strncpy(dnx_er_db->dnx_err_recovery_manager->trans_info.dnx_er_regress_testing_api_name, func_name,
                    DNX_ROLLBACK_JOURNAL_API_NAME_MAX_LENGTH - 1);
    }
}

uint8
dnx_er_regress_test_should_loop(
    int unit)
{
    dnx_er_db_t *dnx_er_db = NULL;

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        return FALSE;
    }

    if ((dnx_er_regress_testing_conditions_are_met(unit, dnx_er_db)
         && (1 == dnx_er_db->dnx_err_recovery_manager->trans_info.top_level_trans_in_api_count)
         && (!dnx_er_db->dnx_err_recovery_manager->trans_info.is_api_ran)))
    {

        dnx_er_db->dnx_err_recovery_manager->trans_info.is_api_ran = TRUE;
        dnx_er_db->dnx_err_recovery_manager->trans_info.top_level_trans_in_api_count = 0;

        return TRUE;
    }

    return FALSE;
}

uint8
dnx_er_regress_testing_api_test_mode_get(
    int unit)
{
    return dnx_er_regression[unit].is_testing_enabled;
}

void
dnx_er_regress_testing_api_test_mode_change(
    int unit,
    uint8 is_on)
{
    dnx_er_regression[unit].is_testing_enabled = is_on ? TRUE : FALSE;
}

void
dnx_er_regress_testing_api_exclude(
    int unit)
{
    dnx_er_db_t *dnx_er_db = NULL;

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        return;
    }

    if (dnx_er_regress_testing_conditions_are_met(unit, dnx_er_db)
        && dnx_er_regress_is_outside_of_transaction(unit, dnx_er_db))
    {
        dnx_er_db->dnx_err_recovery_manager->trans_info.is_api_excluded = TRUE;
    }
}

void
dnx_er_regress_sim_skip(
    int unit)
{
    dnx_er_db_t *dnx_er_db = NULL;
    int rv;

    rv = dnx_er_db_get(unit, &dnx_er_db);

    assert((rv == _SHR_E_NONE) && (dnx_er_db != NULL));

    if (dnx_er_regress_testing_conditions_are_met(unit, dnx_er_db))
    {
        dnx_er_db->dnx_err_recovery_manager->trans_info.is_api_excluded = TRUE;
    }
}

shr_error_e
dnx_er_regress_journal_var(
    int unit,
    uint32 size,
    uint8 *ptr)
{
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_er_db_get(unit, &dnx_er_db));

    if (dnx_er_regress_is_first_test_iteration(unit, dnx_er_db))
    {
        SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_memcpy(unit, size, ptr));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_err_recovery_regress_violate_tmp_allow(
    int unit,
    dnx_err_recovery_regress_error_t type,
    uint8 is_on)
{
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        SHR_EXIT();
    }

    if (!dnx_er_regress_testing_conditions_are_met(unit, dnx_er_db))
    {
        SHR_EXIT();
    }

    if (!is_on)
    {
        dnx_er_db->dnx_err_recovery_manager->trans_info.err_type_suppress_counters[type]++;
    }
    else
    {
        if (dnx_er_db->dnx_err_recovery_manager->trans_info.err_type_suppress_counters[type] == 0)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "error recovery regression ERROR: Error suppression counter cannot be less than 0.\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
        }

        dnx_er_db->dnx_err_recovery_manager->trans_info.err_type_suppress_counters[type]--;
    }

exit:
    SHR_FUNC_EXIT;
}

void
dnx_er_regress_swstate_access_cb(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    if (dnx_er_regression[unit].is_testing_enabled
        && !dnx_err_recovery_regress_is_err_type_suppressed(unit, dnx_er_db,
                                                            DNX_ERR_RECOVERY_REGRESS_ERROR_SW_ACCESS_OUTSIDE_TRANS))
    {
        dnx_er_regress_system_state_modified(unit, dnx_er_db);
    }
}

void
dnx_er_regress_dbal_access_cb(
    int unit,
    dnx_er_db_t * dnx_er_db,
    dbal_logical_table_t * table_ptr)
{
    if (dnx_er_regression[unit].is_testing_enabled
        && !dnx_err_recovery_regress_is_err_type_suppressed(unit, dnx_er_db,
                                                            DNX_ERR_RECOVERY_REGRESS_ERROR_DBAL_ACCESS_OUTSIDE_TRANS))
    {
        dnx_er_regress_system_state_modified(unit, dnx_er_db);
    }

#if defined(INCLUDE_KBP)
    if (dnx_er_regress_testing_conditions_are_met(unit, dnx_er_db)
        && table_ptr->access_method == DBAL_ACCESS_METHOD_KBP && dnx_kbp_device_enabled(unit))
    {
        int l3_route_cache_enabled = 0;
        int field_cache_enabled = 0;
        int rv = _SHR_E_NONE;

        rv = bcm_switch_control_get(unit, bcmSwitchL3RouteCache, &l3_route_cache_enabled);

        if (rv != _SHR_E_NONE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error in bcm_switch_control_get.\n")));
        }

        rv = bcm_switch_control_get(unit, bcmSwitchFieldCache, &field_cache_enabled);

        if (rv != _SHR_E_NONE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error in bcm_switch_control_get.\n")));
        }

        if ((l3_route_cache_enabled && DNX_KBP_IS_TABLE_USED_FOR_FWD(table_ptr->table_type))
            || (field_cache_enabled && DNX_KBP_IS_TABLE_USED_FOR_ACL(table_ptr->table_type)))
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U
                                       (unit,
                                        "error recovery regression ERROR: attempted to start a new top-level transaction when KBP caching is on.\n")));
            assert(0);
        }
    }
#endif
}

#undef _ERR_MSG_MODULE_NAME
