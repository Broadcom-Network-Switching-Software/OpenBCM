
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

#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_err_recovery_manager_common.h>
#include <soc/dnx/dbal/dbal_journal.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnx/swstate/auto_generated/access/switch_access.h>
#include <soc/dnx/recovery/generic_state_journal.h>
#include <soc/dnx/recovery/rollback_journal.h>
#include <soc/dnx/recovery/rollback_journal_diag.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/recovery/rollback_journal_utils.h>
#include <soc/dnx/dnx_er_threading.h>
#include <soc/dnxc/dnxc_wb_test_utils.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_ERRORRECOVERY

extern dnx_err_recovery_common_root_t dnx_err_recovery_common_root[SOC_MAX_NUM_DEVICES];

#define DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(_unit)\
do{                                                          \
    if (!dnx_err_recovery_are_prerequisites_met(_unit)) {    \
        SHR_EXIT();                                          \
    }                                                        \
}while(0)

#define DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN(_unit, _er_thread_id)           \
do {                                                                             \
    if(0 == dnx_err_recovery_manager[_unit][_er_thread_id].transaction_counter) {\
        SHR_EXIT();                                                              \
    }                                                                            \
}                                                                                \
while(0)

#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC

#define DNX_ERR_RECOVERY_EXIT_IF_NO_VALIDATION(_unit)           \
do {                                                            \
    if (!dnx_err_recovery_common_is_validation_enabled(_unit)) {\
        SHR_EXIT();                                             \
    }                                                           \
} while(0)
#endif

#define DNX_ERR_RECOVERY_EXIT_IF_INVALIDATED(_unit, _er_thread_id)                                                                                        \
do {                                                                                                                                       \
    if(!dnx_err_recovery_common_flag_is_on(_unit, _er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK, DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED)){\
        SHR_EXIT();                                                                                                                        \
    }                                                                                                                                      \
}                                                                                                                                          \
while(0)

err_recovery_manager_t dnx_err_recovery_manager[SOC_MAX_NUM_DEVICES][DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION] =
    { {{0}} };

static uint8 dnx_err_recovery_is_on(
    int unit,
    int er_thread_id);

static void
dnx_err_recovery_transaction_bookmark_set(
    int unit,
    int er_thread_id,
    uint32 idx,
    uint32 seq_id)
{
    dnx_err_recovery_manager[unit][er_thread_id].trans_bookmarks[idx] = seq_id;
}

static uint32
dnx_err_recovery_transaction_bookmark_get(
    int unit,
    int er_thread_id,
    uint32 idx)
{
    return dnx_err_recovery_manager[unit][er_thread_id].trans_bookmarks[idx];
}

static shr_error_e
dnx_err_recovery_transaction_clear(
    int unit,
    int er_thread_id)
{
    uint32 idx = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_clear(unit, er_thread_id, FALSE, FALSE));
    SHR_IF_ERR_EXIT(dnx_dbal_journal_clear(unit, er_thread_id, FALSE, FALSE));
    SHR_IF_ERR_EXIT(dnx_generic_state_journal_clear(unit, er_thread_id, FALSE));

    for (idx = 0; idx < dnx_err_recovery_manager[unit][er_thread_id].transaction_counter; idx++)
    {
        dnx_err_recovery_transaction_bookmark_set(unit, er_thread_id, idx, 0);
    }

    dnx_err_recovery_manager[unit][er_thread_id].entry_counter = 0;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_err_recovery_commit_unsafe(
    int unit,
    int er_thread_id)
{
    uint32 state_entry_seq_id = 0;
    uint32 sw_state_entry_seq_id = 0;
    uint32 dbal_entry_seq_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    while (dnx_err_recovery_manager[unit][er_thread_id].entry_counter > 0)
    {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_get_head_seq_id(unit, er_thread_id, FALSE, &sw_state_entry_seq_id));
        SHR_IF_ERR_EXIT(dnx_generic_state_journal_get_head_seq_id(unit, er_thread_id, &state_entry_seq_id));
        SHR_IF_ERR_EXIT(dnx_dbal_journal_get_head_seq_id(unit, er_thread_id, FALSE, &dbal_entry_seq_id));

#ifdef DNX_ERR_RECOVERY_VALIDATION

        if ((sw_state_entry_seq_id != dnx_err_recovery_manager[unit][er_thread_id].entry_counter)
            && (state_entry_seq_id != dnx_err_recovery_manager[unit][er_thread_id].entry_counter)
            && (dbal_entry_seq_id != dnx_err_recovery_manager[unit][er_thread_id].entry_counter))
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: entry sequence id not found in journals.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
#endif

        if (sw_state_entry_seq_id == dnx_err_recovery_manager[unit][er_thread_id].entry_counter)
        {
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_clear(unit, er_thread_id, TRUE, FALSE));
        }
        else if (state_entry_seq_id == dnx_err_recovery_manager[unit][er_thread_id].entry_counter)
        {
            SHR_IF_ERR_EXIT(dnx_generic_state_journal_clear(unit, er_thread_id, TRUE));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_dbal_journal_clear(unit, er_thread_id, TRUE, FALSE));
        }

        dnx_err_recovery_manager[unit][er_thread_id].entry_counter--;
    }

    dnx_err_recovery_transaction_bookmark_set(unit, er_thread_id,
                                              dnx_err_recovery_manager[unit][er_thread_id].transaction_counter, 0);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (dnx_err_recovery_manager[unit][er_thread_id].entry_counter != 0)
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "err recovery ERROR: entry count non-zero after journals cleared.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_err_recovery_abort_unsafe(
    int unit,
    int er_thread_id)
{
    uint32 state_entry_seq_id = 0;
    uint32 sw_state_entry_seq_id = 0;
    uint32 dbal_entry_seq_id = 0;

    uint32 rollback_entry_bound = 0;
    uint8 rollback_error = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    rollback_entry_bound =
        dnx_err_recovery_transaction_bookmark_get(unit, er_thread_id,
                                                  dnx_err_recovery_manager[unit][er_thread_id].transaction_counter);

    while (dnx_err_recovery_manager[unit][er_thread_id].entry_counter > rollback_entry_bound)
    {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_get_head_seq_id(unit, er_thread_id, FALSE, &sw_state_entry_seq_id));
        SHR_IF_ERR_EXIT(dnx_generic_state_journal_get_head_seq_id(unit, er_thread_id, &state_entry_seq_id));
        SHR_IF_ERR_EXIT(dnx_dbal_journal_get_head_seq_id(unit, er_thread_id, FALSE, &dbal_entry_seq_id));

#ifdef DNX_ERR_RECOVERY_VALIDATION

        if ((sw_state_entry_seq_id != dnx_err_recovery_manager[unit][er_thread_id].entry_counter)
            && (state_entry_seq_id != dnx_err_recovery_manager[unit][er_thread_id].entry_counter)
            && (dbal_entry_seq_id != dnx_err_recovery_manager[unit][er_thread_id].entry_counter))
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: entry sequence id not found in journals.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
#endif

        if (sw_state_entry_seq_id == dnx_err_recovery_manager[unit][er_thread_id].entry_counter)
        {
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_roll_back(unit, er_thread_id, TRUE, FALSE));
        }
        else if (state_entry_seq_id == dnx_err_recovery_manager[unit][er_thread_id].entry_counter)
        {
            SHR_IF_ERR_EXIT(dnx_generic_state_journal_roll_back(unit, er_thread_id, TRUE));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_dbal_journal_roll_back(unit, er_thread_id, TRUE, FALSE));
        }

        dnx_err_recovery_manager[unit][er_thread_id].entry_counter--;
    }

    dnx_err_recovery_transaction_bookmark_set(unit, er_thread_id,
                                              dnx_err_recovery_manager[unit][er_thread_id].transaction_counter, 0);

    rollback_error = FALSE;

exit:
    if (rollback_error)
    {
        dnx_err_recovery_transaction_clear(unit, er_thread_id);
    }
    SHR_FUNC_EXIT;
}

static uint32
dnx_err_recovery_new_journal_entry_seq_id_get(
    int unit,
    int er_thread_id)
{
    dnx_err_recovery_manager[unit][er_thread_id].entry_counter++;
    return dnx_err_recovery_manager[unit][er_thread_id].entry_counter;
}

uint8
dnx_err_recovery_is_dbal_access_region(
    int unit,
    int er_thread_id)
{
    return (dnx_err_recovery_manager[unit][er_thread_id].dbal_access_counter > 0);
}

shr_error_e
dnx_err_recovery_dbal_access_region_start(
    int unit)
{
    int er_thread_id = dnx_er_threading_transaction_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    assert(er_thread_id != -1);

    dnx_err_recovery_manager[unit][er_thread_id].dbal_access_counter++;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_err_recovery_dbal_access_region_end(
    int unit)
{
    int er_thread_id = dnx_er_threading_transaction_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    assert(er_thread_id != -1);

    if (0 == dnx_err_recovery_manager[unit][er_thread_id].dbal_access_counter)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "err recovery manager ERROR: attempted to end dbal access region without start.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    dnx_err_recovery_manager[unit][er_thread_id].dbal_access_counter--;

exit:
    SHR_FUNC_EXIT;
}

static uint8
dnx_err_recovery_is_sw_state_journal_on(
    int unit,
    int er_thread_id)
{

    return (dnx_err_recovery_is_on(unit, er_thread_id) && !dnx_err_recovery_is_dbal_access_region(unit, er_thread_id));
}

static inline void
dnx_err_recovery_transaction_counters_reset(
    int unit,
    int er_thread_id)
{
    dnx_err_recovery_manager[unit][er_thread_id].transaction_counter = 0;
    dnx_err_recovery_common_flag_clear(unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                       DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);

    dnx_err_recovery_manager[unit][er_thread_id].is_suppressed_counter = 0;
    dnx_err_recovery_common_flag_set(unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                     DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);

    dnx_err_recovery_common_flag_set(unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                     DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED);
}

static inline void
dnx_err_recovery_init_counters_reset(
    int unit,
    int er_thread_id)
{
    dnx_err_recovery_manager[unit][er_thread_id].entry_counter = 0;

    dnx_err_recovery_common_flag_clear(unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                       DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED);

    dnx_err_recovery_transaction_counters_reset(unit, er_thread_id);
}

shr_error_e
dnx_err_recovery_is_init_check_bypass(
    int unit,
    uint8 is_on)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, is_on));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_err_recovery_initialize(
    int unit)
{
    dnx_rollback_journal_cbs_t swstate_cbs;
    dnx_rollback_journal_cbs_t dbal_cbs;
    dnx_rollback_journal_cbs_t state_cbs;

    dnxc_sw_state_journal_access_cb swstate_access_cb = NULL;
    dnx_dbal_journal_access_cb dbal_access_cb = NULL;
    int er_thread_id = 0;

    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC
#ifdef DNX_ERR_RECOVERY_VALIDATION
    dnx_err_recovery_common_root[unit].validation_enabled = TRUE;
#else
    dnx_err_recovery_common_root[unit].validation_enabled = FALSE;
#endif
#endif

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING

    SHR_IF_ERR_EXIT(dnx_er_regress_init(unit));
    if (dnx_er_regress_testing_api_test_mode_get(unit))
    {
        swstate_access_cb = dnx_er_regress_swstate_access_cb;
        dbal_access_cb = dnx_er_regress_dbal_access_cb;
        dnx_err_recovery_common_root[unit].validation_enabled = TRUE;
    }
#endif

    swstate_cbs.is_on = &dnx_err_recovery_is_sw_state_journal_on;
    swstate_cbs.seq_id_get = &dnx_err_recovery_new_journal_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_initialize(unit, swstate_cbs, swstate_access_cb, FALSE));

    dbal_cbs.is_on = &dnx_err_recovery_is_on;
    dbal_cbs.seq_id_get = &dnx_err_recovery_new_journal_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnx_dbal_journal_initialize(unit, dbal_cbs, dbal_access_cb, FALSE));

    state_cbs.is_on = &dnx_err_recovery_is_on;
    state_cbs.seq_id_get = &dnx_err_recovery_new_journal_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnx_generic_state_journal_initialize(unit, state_cbs));

    for (er_thread_id = 0; er_thread_id < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; er_thread_id++)
    {
        dnx_err_recovery_init_counters_reset(unit, er_thread_id);

        if (dnx_err_recovery_common_flag_is_on
            (unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK, DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED))
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "err recovery ERROR: already initialized .\n%s%s%s", EMPTY,
                                     EMPTY, EMPTY);
        }

        dnx_err_recovery_common_flag_set(unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                         DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED);

#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC
        dnx_err_recovery_manager[unit][er_thread_id].api_counter = 0;
        sal_memset(dnx_err_recovery_manager[unit][er_thread_id].api_to_trans_map, 0x0,
                   sizeof(dnx_err_recovery_manager[unit][er_thread_id].api_to_trans_map));
#endif
    }

    SHR_IF_ERR_EXIT(dnx_er_threading_initialize(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_err_recovery_deinitialize(
    int unit)
{
    int er_thread_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_err_recovery_common_flag_is_on
        (unit, 0, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK, DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED))
    {
        SHR_EXIT();
    }

#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC
#ifdef DNX_ERR_RECOVERY_VALIDATION
    dnx_err_recovery_common_root[unit].validation_enabled = TRUE;
#else
    dnx_err_recovery_common_root[unit].validation_enabled = FALSE;
#endif
#endif

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING

    SHR_IF_ERR_EXIT(dnx_er_regress_deinit(unit));

    if (dnx_er_regress_testing_api_test_mode_get(unit))
    {
        dnx_err_recovery_common_root[unit].validation_enabled = TRUE;
    }
#endif

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_destroy(unit, FALSE));

    SHR_IF_ERR_EXIT(dnx_dbal_journal_destroy(unit, FALSE));

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_destroy(unit, FALSE));

    SHR_IF_ERR_EXIT(dnx_er_threading_deinitialize(unit));
    for (er_thread_id = 0; er_thread_id < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; er_thread_id++)
    {
        dnx_err_recovery_init_counters_reset(unit, er_thread_id);

#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC

        dnx_err_recovery_manager[unit][er_thread_id].api_counter = 0;
        sal_memset(dnx_err_recovery_manager[unit][er_thread_id].api_to_trans_map, 0x0,
                   sizeof(dnx_err_recovery_manager[unit][er_thread_id].api_to_trans_map));
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnx_err_recovery_module_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_err_recovery_common_init(unit));
    SHR_IF_ERR_EXIT(dnx_err_recovery_initialize(unit));
    SHR_IF_ERR_EXIT(dnx_state_snapshot_manager_init(unit));

exit:
    SHR_FUNC_EXIT;
}

int
dnx_err_recovery_module_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_state_snapshot_manager_deinit(unit));
    SHR_IF_ERR_EXIT(dnx_err_recovery_deinitialize(unit));
exit:
    SHR_FUNC_EXIT;
}

#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC

shr_error_e
dnx_err_recovery_api_counter_inc(
    int unit)
{
    int er_thread_id = dnx_er_threading_transaction_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);

    DNX_ERR_RECOVERY_EXIT_IF_NO_VALIDATION(unit);

    if (er_thread_id == -1)
    {
        SHR_EXIT();
    }

    if (dnx_err_recovery_manager[unit][er_thread_id].api_counter + 1 > DNX_ERR_RECOVERY_MAX_NUM_NESTED_APIS)
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "err recovery ERROR: API counter cannot be more than 3.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }

    dnx_err_recovery_manager[unit][er_thread_id].
        api_to_trans_map[dnx_err_recovery_manager[unit][er_thread_id].api_counter] =
        dnx_err_recovery_manager[unit][er_thread_id].transaction_counter;

    dnx_err_recovery_manager[unit][er_thread_id].api_counter++;

    if (1 == dnx_err_recovery_manager[unit][er_thread_id].api_counter)
    {
#ifdef DNX_ERR_RECOVERY_VALIDATION
        dnx_err_recovery_manager[unit][er_thread_id].api_status = DNX_ERR_RECOVERY_API_STATUS_UNKNOWN;
        dnx_err_recovery_manager[unit][er_thread_id].api_status_count = 0;
#endif

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
        SHR_IF_ERR_EXIT(dnx_er_regress_top_level_api_flags_reset(unit, er_thread_id));
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

#ifdef DNX_ERR_RECOVERY_VALIDATION

static uint8
dnx_err_recovery_api_status_is_valid(
    int unit,
    int er_thread_id)
{
    int api_count_floor = 0;

#ifdef BCM_WARM_BOOT_SUPPORT

    api_count_floor = 1;
#endif

    if (dnx_er_threading_is_tmp_excluded(unit))
    {
        return TRUE;
    }
    else
    {
        return (dnx_err_recovery_manager[unit][er_thread_id].api_status != DNX_ERR_RECOVERY_API_STATUS_UNKNOWN &&
                dnx_err_recovery_manager[unit][er_thread_id].api_status_count > api_count_floor);
    }

}
#endif

shr_error_e
dnx_err_recovery_api_counter_dec(
    int unit,
    uint8 bcm_api_error)
{
    uint8 reset_flags = FALSE;
    int er_thread_id = dnx_er_threading_transaction_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);

    DNX_ERR_RECOVERY_EXIT_IF_NO_VALIDATION(unit);

    if (er_thread_id == -1)
    {
        SHR_EXIT();
    }

    if (0 == dnx_err_recovery_manager[unit][er_thread_id].api_counter)
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "err recovery ERROR: API counter cannot be less than 0.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }

    reset_flags = TRUE;

    if (1 == dnx_err_recovery_manager[unit][er_thread_id].api_counter)
    {

#ifdef DNX_ERR_RECOVERY_VALIDATION

        if (!dnx_err_recovery_api_status_is_valid(unit, er_thread_id) && !bcm_api_error)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: DNX does not use DNX_ERR_RECOVERY_START or DNX_ERR_RECOVERY_NOT_NEEDED.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
        dnx_err_recovery_manager[unit][er_thread_id].api_status = DNX_ERR_RECOVERY_API_STATUS_UNKNOWN;
        dnx_err_recovery_manager[unit][er_thread_id].api_status_count = 0;
#endif

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
        SHR_IF_ERR_EXIT(dnx_er_regress_top_level_api_flags_reset(unit, er_thread_id));
#endif
    }

    dnx_err_recovery_manager[unit][er_thread_id].api_counter--;

    if (dnx_err_recovery_manager[unit][er_thread_id].api_to_trans_map
        [dnx_err_recovery_manager[unit][er_thread_id].api_counter] !=
        dnx_err_recovery_manager[unit][er_thread_id].transaction_counter)
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "err recovery ERROR: API opens a transaction but never closes it.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }

    reset_flags = FALSE;

exit:
    if (reset_flags)
    {
        if (1 == dnx_err_recovery_manager[unit][er_thread_id].api_counter)
        {
#ifdef DNX_ERR_RECOVERY_VALIDATION
            dnx_err_recovery_manager[unit][er_thread_id].api_status = DNX_ERR_RECOVERY_API_STATUS_UNKNOWN;
            dnx_err_recovery_manager[unit][er_thread_id].api_status_count = 0;
#endif

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
            dnx_er_regress_top_level_api_flags_reset(unit, er_thread_id);
#endif
        }
        dnx_err_recovery_manager[unit][er_thread_id].api_counter--;
    }
    SHR_FUNC_EXIT;
}
#endif

#ifdef DNX_ERR_RECOVERY_VALIDATION

shr_error_e
dnx_err_recovery_not_needed(
    int unit)
{
    int er_thread_id = dnx_er_threading_transaction_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);

    if (er_thread_id == -1)
    {
        SHR_EXIT();
    }

    if (dnx_err_recovery_manager[unit][er_thread_id].api_status == DNX_ERR_RECOVERY_API_STATUS_UNKNOWN)
    {
        dnx_err_recovery_manager[unit][er_thread_id].api_status = DNX_ERR_RECOVERY_API_NOT_NEEDED;
    }

    dnx_err_recovery_manager[unit][er_thread_id].api_status_count++;

exit:
    SHR_FUNC_EXIT;
}

#endif

uint8
dnx_err_recovery_are_prerequisites_met(
    int unit)
{

    return (SOC_UNIT_VALID(unit) &&
            dnx_err_recovery_common_is_enabled(unit) &&
            dnx_rollback_journal_is_done_init(unit) &&
            !SOC_IS_DETACHING(unit) &&
            dnx_err_recovery_common_flag_is_on(unit, 0, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                               DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED));
}

static uint8
dnx_err_recovery_is_on(
    int unit,
    int er_thread_id)
{
    return dnx_err_recovery_common_is_on(unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK);
}

uint8
dnx_err_recovery_is_on_test(
    int unit)
{
    int er_thread_id = dnx_er_threading_transaction_get(unit);

    return dnx_err_recovery_is_on(unit, er_thread_id);
}

shr_error_e
dnx_err_recovery_tmp_allow(
    int unit,
    uint8 is_on)
{
    int er_thread_id = dnx_er_threading_transaction_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);

    if (er_thread_id == -1)
    {
        SHR_EXIT();
    }

    DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN(unit, er_thread_id);
    DNX_ERR_RECOVERY_EXIT_IF_INVALIDATED(unit, er_thread_id);

    if (!is_on)
    {
#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
        SHR_IF_ERR_EXIT(dnx_er_regress_comparison_tmp_allow(unit, er_thread_id, is_on));
#endif

        dnx_err_recovery_manager[unit][er_thread_id].is_suppressed_counter++;
        dnx_err_recovery_common_flag_clear(unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                           DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);
    }
    else
    {
        if (0 == dnx_err_recovery_manager[unit][er_thread_id].is_suppressed_counter)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "err recovery ERROR: Suppression counter cannot be less than 0.\n%s%s%s", EMPTY,
                                     EMPTY, EMPTY);
        }

        dnx_err_recovery_manager[unit][er_thread_id].is_suppressed_counter--;
        if (0 == dnx_err_recovery_manager[unit][er_thread_id].is_suppressed_counter)
        {
            dnx_err_recovery_common_flag_set(unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                             DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);
        }

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
        SHR_IF_ERR_EXIT(dnx_er_regress_comparison_tmp_allow(unit, er_thread_id, is_on));
#endif

    }

exit:
    SHR_FUNC_EXIT;
}

#ifdef DNX_ERR_RECOVERY_VALIDATION

uint8
dnx_err_recovery_hw_access_is_valid(
    int unit)
{
    int er_thread_id = dnx_er_threading_transaction_get(unit);

    if (er_thread_id == -1)
    {
        return TRUE;
    }

    if (!dnx_err_recovery_is_on(unit, er_thread_id))
    {
        return TRUE;
    }

    if (dnx_dbal_journal_is_tmp_suppressed_unsafe(unit, er_thread_id, FALSE))
    {
        return TRUE;
    }

    return dnx_err_recovery_is_dbal_access_region(unit, er_thread_id);
}
#endif

shr_error_e
dnx_err_recovery_unsupported_dbal_unsup_tbls_bypass(
    int unit,
    uint8 is_on)
{
    int er_thread_id = dnx_er_threading_transaction_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    if (er_thread_id == -1)
    {
        SHR_EXIT();
    }

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);
    DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN(unit, er_thread_id);
    DNX_ERR_RECOVERY_EXIT_IF_INVALIDATED(unit, er_thread_id);

    SHR_IF_ERR_EXIT(dnx_dbal_journal_unsupported_tables_bypass(unit, er_thread_id, is_on));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_err_recovery_transaction_no_support_counter_inc(
    int unit)
{
    int er_thread_id = dnx_er_threading_transaction_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_err_recovery_are_prerequisites_met(unit))
    {
        return _SHR_E_NONE;
    }

    SHR_IF_ERR_EXIT(dnx_er_threading_tmp_excluded_add(unit));

    if (er_thread_id == -1)
    {
        SHR_EXIT();
    }

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);

    if (dnx_err_recovery_manager[unit][er_thread_id].transaction_counter != 0)
    {
        SHR_IF_ERR_EXIT(dnx_generic_state_journal_all_tmp_suppress_unsafe(unit, er_thread_id, TRUE));
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_all_tmp_suppress_unsafe(unit, er_thread_id, TRUE));
        SHR_IF_ERR_EXIT(dnx_dbal_journal_all_tmp_suppress_unsafe(unit, er_thread_id, TRUE));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_err_recovery_transaction_no_support_counter_dec(
    int unit)
{
    int er_thread_id = dnx_er_threading_transaction_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_err_recovery_are_prerequisites_met(unit))
    {
        return _SHR_E_NONE;
    }

    assert(er_thread_id != -1);

    SHR_IF_ERR_EXIT(dnx_er_threading_tmp_excluded_remove(unit));

    if (dnx_err_recovery_manager[unit][er_thread_id].transaction_counter != 0)
    {
        DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);
        SHR_IF_ERR_EXIT(dnx_generic_state_journal_all_tmp_suppress_unsafe(unit, er_thread_id, FALSE));
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_all_tmp_suppress_unsafe(unit, er_thread_id, FALSE));
        SHR_IF_ERR_EXIT(dnx_dbal_journal_all_tmp_suppress_unsafe(unit, er_thread_id, FALSE));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_err_recovery_transaction_start_internal(
    int unit)
{
    int er_thread_id = dnx_er_threading_transaction_get(unit);
    uint8 is_top_level_transaction = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    if (er_thread_id == -1)
    {
        is_top_level_transaction = TRUE;
        SHR_IF_ERR_EXIT(dnx_er_threading_transaction_register(unit));
        er_thread_id = dnx_er_threading_transaction_get(unit);
        assert(er_thread_id != -1);
    }
    else if (0 == dnx_err_recovery_manager[unit][er_thread_id].transaction_counter)
    {
        is_top_level_transaction = TRUE;
    }

#ifdef DNX_ERR_RECOVERY_VALIDATION

    if (is_top_level_transaction)
    {
        if (!dnx_err_recovery_common_flag_is_on
            (unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK, DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED))
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: transaction cannot be invalidated before it has been started.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }

        if (0 != dnx_err_recovery_manager[unit][er_thread_id].is_suppressed_counter)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: transaction cannot be suppressed before it has been started.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }

        if (dnx_dbal_journal_are_unsupported_tables_bypassed(unit, er_thread_id))
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: transaction cannot be started with temporary bypass of unsupported dbal tables set.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }

        if (0 != dnx_err_recovery_manager[unit][er_thread_id].entry_counter)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: entries found prior to transaction start.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
    }
#endif

    dnx_err_recovery_manager[unit][er_thread_id].transaction_counter++;
    dnx_err_recovery_common_flag_set(unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                     DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);

    dnx_err_recovery_transaction_bookmark_set(unit, er_thread_id,
                                              dnx_err_recovery_manager[unit][er_thread_id].transaction_counter,
                                              dnx_err_recovery_manager[unit][er_thread_id].entry_counter);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (dnx_err_recovery_manager[unit][er_thread_id].api_status == DNX_ERR_RECOVERY_API_STATUS_UNKNOWN)
    {
        dnx_err_recovery_manager[unit][er_thread_id].api_status = DNX_ERR_RECOVERY_API_OPTS_IN;
    }

    dnx_err_recovery_manager[unit][er_thread_id].api_status_count++;
#endif

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    SHR_IF_ERR_EXIT(dnx_er_regress_transaction_start(unit, er_thread_id, is_top_level_transaction));
#endif

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_err_recovery_transaction_start(
    int unit,
    bcm_switch_module_t module_id)
{
    uint8 enabled = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);

    if (dnx_er_threading_is_tmp_excluded(unit))
    {
        SHR_EXIT();
    }

    if (module_id != bcmModuleCount)
    {
        if (bcmModuleCount > module_id)
        {
            SHR_IF_ERR_EXIT(switch_db.module_error_recovery.get(unit, module_id, &enabled));
            if (!enabled)
            {
                SHR_EXIT();
            }
        }
        else
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "err recovery ERROR: invalid module_id %d. bcmModuleCount is %d.\n%s",
                                     module_id, bcmModuleCount, EMPTY);
        }
    }

    SHR_IF_ERR_EXIT(dnx_err_recovery_transaction_start_internal(unit));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_err_recovery_transaction_end_internal(
    int unit,
    int er_thread_id,
    int api_result)
{
    uint8 reset_flags = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    if (1 == (dnx_err_recovery_manager[unit][er_thread_id].transaction_counter))
    {
#ifdef DNX_ERR_RECOVERY_VALIDATION

        if (0 != dnx_err_recovery_manager[unit][er_thread_id].is_suppressed_counter)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: temporary suppression is not disabled at the end of the transaction.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }

        if (dnx_dbal_journal_are_unsupported_tables_bypassed(unit, er_thread_id))
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: temporary bypass of unsupported dbal table is not disabled at the end of transaction.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }

        if (!dnx_err_recovery_common_flag_is_on
            (unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK, DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED))
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: unsupported features were accessed during the transaction.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
#endif

        if (SHR_FAILURE(api_result)
#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
            || dnx_er_regress_transaction_end_should_abort(unit, er_thread_id)
#endif
            )
        {
            SHR_IF_ERR_EXIT(dnx_err_recovery_abort_unsafe(unit, er_thread_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_err_recovery_commit_unsafe(unit, er_thread_id));
        }
    }
    else
    {

        if (SHR_FAILURE(api_result)
#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
            || dnx_er_regress_transaction_end_should_abort(unit, er_thread_id)
#endif
            )
        {
            SHR_IF_ERR_EXIT(dnx_err_recovery_abort_unsafe(unit, er_thread_id));
        }
    }

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    SHR_IF_ERR_EXIT(dnx_er_regress_transaction_end(unit, er_thread_id));
#endif

    dnx_err_recovery_manager[unit][er_thread_id].transaction_counter--;

    if (0 == dnx_err_recovery_manager[unit][er_thread_id].transaction_counter)
    {
        dnx_err_recovery_transaction_counters_reset(unit, er_thread_id);
    }

    reset_flags = FALSE;
exit:
    if (reset_flags)
    {
        dnx_err_recovery_manager[unit][er_thread_id].transaction_counter--;
        if (0 == dnx_err_recovery_manager[unit][er_thread_id].transaction_counter)
        {
            dnx_err_recovery_transaction_counters_reset(unit, er_thread_id);
        }
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_err_recovery_transaction_end(
    int unit,
    bcm_switch_module_t module_id,
    int api_result)
{
    uint8 enabled = FALSE;

    int er_thread_id = dnx_er_threading_transaction_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);

    assert(er_thread_id != -1);

    DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN(unit, er_thread_id);

    if (dnx_er_threading_is_tmp_excluded(unit))
    {
        SHR_EXIT();
    }

    if (bcmModuleCount != module_id)
    {
        switch_db.module_error_recovery.get(unit, module_id, &enabled);
        if (!enabled)
        {
            SHR_EXIT();
        }
    }

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    if (dnx_er_regress_transaction_fail_should_return_error(unit, er_thread_id))
    {
        SHR_IF_ERR_EXIT(dnx_err_recovery_transaction_end_internal(unit, er_thread_id, api_result));
    }
    else
#endif
    {
        dnx_err_recovery_transaction_end_internal(unit, er_thread_id, api_result);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_rollback_journal_start(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);

    if (dnx_er_threading_is_tmp_excluded(unit))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_err_recovery_transaction_start_internal(unit));

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    SHR_IF_ERR_EXIT(dnx_er_regress_journal_start(unit));
#endif

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_rollback_journal_end(
    int unit,
    uint8 rollback_journal)
{
    shr_error_e error_type = _SHR_E_NONE;

    int er_thread_id = dnx_er_threading_transaction_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    assert(er_thread_id != -1);

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);
    DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN(unit, er_thread_id);

    if (rollback_journal)
    {

        error_type = _SHR_E_INTERNAL;
    }

    SHR_IF_ERR_EXIT(dnx_err_recovery_transaction_end_internal(unit, er_thread_id, error_type));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_err_recovery_transaction_invalidate(
    int unit)
{
    int er_thread_id = dnx_er_threading_transaction_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    if (er_thread_id == -1)
    {
        SHR_EXIT();
    }

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);
    DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN(unit, er_thread_id);
    DNX_ERR_RECOVERY_EXIT_IF_INVALIDATED(unit, er_thread_id);

    dnx_err_recovery_common_flag_clear(unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                       DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED);

    SHR_IF_ERR_EXIT(dnx_err_recovery_transaction_clear(unit, er_thread_id));

    SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "err recovery ERROR: feature not supported.\n%s%s%s", EMPTY, EMPTY, EMPTY);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_rollback_journal_appl_init_state_change(
    int unit,
    uint8 is_init)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    SHR_IF_ERR_EXIT(dnx_er_regress_appl_init_state_change(unit, is_init));
#endif

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_rollback_journal_print_journal(
    int unit,
    dnx_rollback_journal_subtype_e type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE:
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_print(unit, FALSE));
            break;
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL:
            SHR_IF_ERR_EXIT(dnx_dbal_journal_print(unit, FALSE));
            break;
        default:
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: unrecognized journal type.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_rollback_journal_logger_state_change(
    int unit,
    dnx_rollback_journal_subtype_e type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE:
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_logger_state_change(unit, FALSE));
            break;
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL:
            SHR_IF_ERR_EXIT(dnx_dbal_journal_logger_state_change(unit, FALSE));
            break;
        default:
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: unrecognized journal type.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnx_tcl_rollback_journal_start(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 1));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 0));
exit:
    SHR_FUNC_EXIT;
}

int
dnx_tcl_rollback_journal_end(
    int unit,
    uint8 rollback_journal)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 1));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, rollback_journal));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 0));
exit:
    SHR_FUNC_EXIT;

}

int
dnx_tcl_rollback_journal_suppress(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 1));
    SHR_IF_ERR_EXIT(dnx_err_recovery_tmp_allow(unit, FALSE));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 0));
exit:
    SHR_FUNC_EXIT;
}

int
dnx_tcl_rollback_journal_unsuppress(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 1));
    SHR_IF_ERR_EXIT(dnx_err_recovery_tmp_allow(unit, TRUE));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 0));
exit:
    SHR_FUNC_EXIT;
}

int
dnx_tcl_rollback_journal_is_initialized(
    int unit)
{
    int ret = 0;
    assert(dnx_rollback_journal_is_init_check_bypass(unit, 1) == _SHR_E_NONE);

    ret = dnx_err_recovery_common_flag_is_on(unit, 0, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                             DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED);
    assert(dnx_rollback_journal_is_init_check_bypass(unit, 0) == _SHR_E_NONE);
    return ret;
}

int
dnx_tcl_rollback_journal_diagnostics_test_end(
    int unit,
    char *testName)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 1));
#ifdef DNX_ROLLBACK_JOURNAL_DIAGNOSTICS
    dnx_rollback_journal_diag_print(unit, testName);
    SHR_IF_ERR_EXIT(dnx_rollback_journal_diag_clean(unit));
#endif
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 0));
exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
