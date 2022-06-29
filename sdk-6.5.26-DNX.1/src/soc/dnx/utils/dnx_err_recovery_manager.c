
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

#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_err_recovery_manager_common.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dbal/dbal_journal.h>
#include <soc/dnx/swstate/dnx_sw_state_journal.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnx/swstate/auto_generated/access/switch_access.h>
#include <soc/dnx/recovery/generic_state_journal.h>
#include <soc/dnx/recovery/rollback_journal.h>
#include <soc/dnx/recovery/rollback_journal_diag.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/recovery/rollback_journal_utils.h>
#include <soc/dnxc/dnxc_wb_test_utils.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_ERRORRECOVERY

#define DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(_unit, dnx_er_db)     \
do{                                                                          \
    if (!dnx_err_recovery_are_prerequisites_met(_unit, dnx_er_db)) {         \
        SHR_EXIT();                                                          \
    }                                                                        \
}while(0)

#define DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN                                 \
do {                                                                             \
    if(0 == dnx_er_db->dnx_err_recovery_manager->transaction_counter) {          \
        SHR_EXIT();                                                              \
    }                                                                            \
}                                                                                \
while(0)

#define DNX_ERR_RECOVERY_EXIT_IF_NO_VALIDATION(_unit)          \
do {                                                           \
    if (!dnx_err_recovery_is_validation_enabled(_unit)) {      \
        SHR_EXIT();                                            \
    }                                                          \
} while(0)

#define DNX_ERR_RECOVERY_EXIT_IF_INVALIDATED(_unit, dnx_er_db)                                                                                 \
do {                                                                                                                                           \
    if(!dnx_err_recovery_flag_is_on(_unit, dnx_er_db , DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK, DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED)){    \
        SHR_EXIT();                                                                                                                            \
    }                                                                                                                                          \
}                                                                                                                                              \
while(0)

uint8
dnx_err_recovery_is_on(
    int unit,
    dnx_er_db_t * dnx_er_db,
    dnx_rollback_journal_type_e mngr_id)
{
    return (dnx_err_recovery_common_is_enabled(unit)
            && dnx_rollback_journal_is_done_init(unit)
            && !SOC_IS_DETACHING(unit)
            && !dnx_rollback_journal_is_any_journal_rolling_back(unit, dnx_er_db)
            && DNX_ERR_RECOVERY_COMMON_IS_ON_ALL == dnx_er_db->dnx_err_recovery_manager->bitmap[mngr_id]);
}

static uint8
dnx_err_recovery_rollback_is_on(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    return dnx_err_recovery_is_on(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK);
}

uint8
dnx_err_recovery_flag_is_on(
    int unit,
    dnx_er_db_t * dnx_er_db,
    dnx_rollback_journal_type_e mngr_id,
    uint32 flag)
{
    return (dnx_er_db->dnx_err_recovery_manager->bitmap[mngr_id] & flag) ? TRUE : FALSE;
}

uint8
dnx_err_recovery_flag_mask_is_on(
    int unit,
    dnx_er_db_t * dnx_er_db,
    dnx_rollback_journal_type_e mngr_id,
    uint32 mask)
{
    return ((dnx_er_db->dnx_err_recovery_manager->bitmap[mngr_id] & mask) == mask) ? TRUE : FALSE;
}

shr_error_e
dnx_err_recovery_flag_set(
    int unit,
    dnx_er_db_t * dnx_er_db,
    dnx_rollback_journal_type_e mngr_id,
    uint32 flag)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_er_db->dnx_err_recovery_manager->bitmap[mngr_id] |= flag;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_err_recovery_flag_clear(
    int unit,
    dnx_er_db_t * dnx_er_db,
    dnx_rollback_journal_type_e mngr_id,
    uint32 flag)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_er_db->dnx_err_recovery_manager->bitmap[mngr_id] &= ~(flag);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_err_recovery_validation_enable_disable_set(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint32 validation_enabled)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_er_db->dnx_err_recovery_manager->validation_enabled = validation_enabled;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

uint8
dnx_err_recovery_is_validation_enabled(
    int unit)
{
    dnx_er_db_t *dnx_er_db = NULL;

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        return FALSE;
    }

    return dnx_er_db->dnx_err_recovery_manager->validation_enabled;
}

static void
dnx_err_recovery_transaction_bookmark_set(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint32 idx,
    uint32 seq_id)
{
    dnx_er_db->dnx_err_recovery_manager->trans_bookmarks[idx] = seq_id;
}

static uint32
dnx_err_recovery_transaction_bookmark_get(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint32 idx)
{
    return dnx_er_db->dnx_err_recovery_manager->trans_bookmarks[idx];
}

static shr_error_e
dnx_err_recovery_transaction_clear(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    uint32 idx = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sw_state_journal_clear(unit, dnx_er_db, FALSE));
    SHR_IF_ERR_EXIT(dnx_dbal_journal_clear(unit, dnx_er_db, FALSE));
    SHR_IF_ERR_EXIT(dnx_generic_state_journal_clear(unit, dnx_er_db, FALSE));

    for (idx = 0; idx < dnx_er_db->dnx_err_recovery_manager->transaction_counter; idx++)
    {
        dnx_err_recovery_transaction_bookmark_set(unit, dnx_er_db, idx, 0);
    }

    dnx_er_db->dnx_err_recovery_manager->entry_counter = 0;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_err_recovery_commit_unsafe(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sw_state_journal_clear(unit, dnx_er_db, FALSE));
    SHR_IF_ERR_EXIT(dnx_generic_state_journal_clear(unit, dnx_er_db, FALSE));
    SHR_IF_ERR_EXIT(dnx_dbal_journal_clear(unit, dnx_er_db, FALSE));

    dnx_er_db->dnx_err_recovery_manager->entry_counter = 0;

    dnx_err_recovery_transaction_bookmark_set(unit, dnx_er_db, dnx_er_db->dnx_err_recovery_manager->transaction_counter,
                                              0);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_err_recovery_abort_unsafe(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    uint32 state_entry_seq_id = 0;
    uint32 sw_state_entry_seq_id = 0;
    uint32 dbal_entry_seq_id = 0;

    uint32 rollback_entry_bound = 0;
    uint8 rollback_error = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    rollback_entry_bound =
        dnx_err_recovery_transaction_bookmark_get(unit, dnx_er_db,
                                                  dnx_er_db->dnx_err_recovery_manager->transaction_counter);

    while (dnx_er_db->dnx_err_recovery_manager->entry_counter > rollback_entry_bound)
    {
        SHR_IF_ERR_EXIT(dnx_sw_state_journal_get_head_seq_id(unit, dnx_er_db, FALSE, &sw_state_entry_seq_id));
        SHR_IF_ERR_EXIT(dnx_generic_state_journal_get_head_seq_id(unit, dnx_er_db, FALSE, &state_entry_seq_id));
        SHR_IF_ERR_EXIT(dnx_dbal_journal_get_head_seq_id(unit, dnx_er_db, FALSE, &dbal_entry_seq_id));

        if (dnx_err_recovery_is_validation_enabled(unit))
        {

            if ((sw_state_entry_seq_id != dnx_er_db->dnx_err_recovery_manager->entry_counter)
                && (state_entry_seq_id != dnx_er_db->dnx_err_recovery_manager->entry_counter)
                && (dbal_entry_seq_id != dnx_er_db->dnx_err_recovery_manager->entry_counter))
            {
                DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                            "err recovery ERROR: entry sequence id not found in journals.\n%s%s%s",
                                                            EMPTY, EMPTY, EMPTY);
            }
        }

        if (sw_state_entry_seq_id == dnx_er_db->dnx_err_recovery_manager->entry_counter)
        {
            SHR_IF_ERR_EXIT(dnx_sw_state_journal_roll_back(unit, dnx_er_db, TRUE, FALSE));
        }
        else if (state_entry_seq_id == dnx_er_db->dnx_err_recovery_manager->entry_counter)
        {
            SHR_IF_ERR_EXIT(dnx_generic_state_journal_roll_back(unit, dnx_er_db, TRUE, FALSE));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_dbal_journal_roll_back(unit, dnx_er_db, TRUE, FALSE));
        }

        dnx_er_db->dnx_err_recovery_manager->entry_counter--;
    }

    dnx_err_recovery_transaction_bookmark_set(unit, dnx_er_db, dnx_er_db->dnx_err_recovery_manager->transaction_counter,
                                              0);

    rollback_error = FALSE;

exit:
    if (rollback_error)
    {
        dnx_err_recovery_transaction_clear(unit, dnx_er_db);
    }
    SHR_FUNC_EXIT;
}

static uint32
dnx_err_recovery_new_journal_entry_seq_id_get(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    dnx_er_db->dnx_err_recovery_manager->entry_counter++;
    return dnx_er_db->dnx_err_recovery_manager->entry_counter;
}

uint8
dnx_err_recovery_is_dbal_access_region(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    return (dnx_er_db->dnx_dbal_journal_manager->dbal_access_counter > 0);
}

shr_error_e
dnx_err_recovery_dbal_access_region_start(
    int unit)
{
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "error recovery manager ERROR: cannot start a dbal access region in transactions where error recovery is NOT_NEEDED/NO_SUPPORT.\n");
    }

    dnx_er_db->dnx_dbal_journal_manager->dbal_access_counter++;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_err_recovery_dbal_access_region_end(
    int unit)
{
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "error recovery manager ERROR: cannot end a dbal access region in transactions where error recovery is NOT_NEEDED/NO_SUPPORT.\n");
    }

    if (0 == dnx_er_db->dnx_dbal_journal_manager->dbal_access_counter)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "err recovery manager ERROR: attempted to end dbal access region without start.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    dnx_er_db->dnx_dbal_journal_manager->dbal_access_counter--;

exit:
    SHR_FUNC_EXIT;
}

static uint8
dnx_err_recovery_is_sw_state_journal_on(
    int unit,
    dnx_er_db_t * dnx_er_db)
{

    return (dnx_err_recovery_rollback_is_on(unit, dnx_er_db)
            && !dnx_err_recovery_is_dbal_access_region(unit, dnx_er_db));
}

static inline void
dnx_err_recovery_transaction_counters_reset(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    dnx_er_db->dnx_err_recovery_manager->transaction_counter = 0;
    dnx_err_recovery_flag_clear(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);

    dnx_er_db->dnx_err_recovery_manager->is_suppressed_counter = 0;
    dnx_err_recovery_flag_set(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                              DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);

    dnx_err_recovery_flag_set(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                              DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED);
}

static inline void
dnx_err_recovery_init_counters_reset(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    dnx_er_db->dnx_err_recovery_manager->entry_counter = 0;

    dnx_err_recovery_flag_clear(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED);

    dnx_err_recovery_transaction_counters_reset(unit, dnx_er_db);
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
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    dnx_rollback_journal_cbs_t swstate_cbs;
    dnx_rollback_journal_cbs_t dbal_cbs;
    dnx_rollback_journal_cbs_t state_cbs;

    dnx_sw_state_journal_access_cb swstate_access_cb = NULL;
    dnx_dbal_journal_access_cb dbal_access_cb = NULL;
    const char *err_recovery_system_name = "ERROR-RECOVERY-REGRESSION";

    SHR_FUNC_INIT_VARS(unit);

    if ((NULL != dnx_data_device.regression.regression_parms_get(unit)->system_mode_name)
        && (sal_strncmp(dnx_data_device.regression.regression_parms_get(unit)->system_mode_name,
                        err_recovery_system_name, sal_strnlen(err_recovery_system_name,
                                                              SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)) == 0))
    {
        dnx_err_recovery_validation_enable_disable_set(unit, dnx_er_db, TRUE);
    }

    dnx_er_regress_init(unit, dnx_er_db);
    if (dnx_er_regress_testing_api_test_mode_get(unit))
    {
        swstate_access_cb = dnx_er_regress_swstate_access_cb;
        dbal_access_cb = dnx_er_regress_dbal_access_cb;
        dnx_err_recovery_validation_enable_disable_set(unit, dnx_er_db, TRUE);
    }

    swstate_cbs.is_on = &dnx_err_recovery_is_sw_state_journal_on;
    swstate_cbs.seq_id_get = &dnx_err_recovery_new_journal_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnx_sw_state_journal_initialize(unit, dnx_er_db, swstate_cbs, swstate_access_cb, FALSE));

    dbal_cbs.is_on = &dnx_err_recovery_rollback_is_on;
    dbal_cbs.seq_id_get = &dnx_err_recovery_new_journal_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnx_dbal_journal_initialize(unit, dnx_er_db, dbal_cbs, dbal_access_cb, FALSE));

    state_cbs.is_on = &dnx_err_recovery_rollback_is_on;
    state_cbs.seq_id_get = &dnx_err_recovery_new_journal_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnx_generic_state_journal_initialize(unit, dnx_er_db, state_cbs, FALSE));

    dnx_err_recovery_init_counters_reset(unit, dnx_er_db);

    if (dnx_err_recovery_flag_is_on
        (unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK, DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED))
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "err recovery ERROR: already initialized .\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }

    dnx_err_recovery_flag_set(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                              DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED);

    dnx_er_db->dnx_err_recovery_manager->api_counter = 0;
    sal_memset(dnx_er_db->dnx_err_recovery_manager->api_to_trans_map, 0x0,
               sizeof(dnx_er_db->dnx_err_recovery_manager->api_to_trans_map));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_err_recovery_deinitialize(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_err_recovery_flag_is_on
        (unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK, DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED))
    {
        SHR_EXIT();
    }

    dnx_er_regress_deinit(unit, dnx_er_db);

    SHR_IF_ERR_EXIT(dnx_sw_state_journal_destroy(unit, dnx_er_db, FALSE));

    SHR_IF_ERR_EXIT(dnx_dbal_journal_destroy(unit, dnx_er_db, FALSE));

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_destroy(unit, dnx_er_db, FALSE));

    dnx_err_recovery_init_counters_reset(unit, dnx_er_db);

    dnx_er_db->dnx_err_recovery_manager->api_counter = 0;
    sal_memset(dnx_er_db->dnx_err_recovery_manager->api_to_trans_map, 0x0,
               sizeof(dnx_er_db->dnx_err_recovery_manager->api_to_trans_map));

exit:
    if (dnx_err_recovery_is_validation_enabled(unit) == TRUE)
    {
        dnx_err_recovery_validation_enable_disable_set(unit, dnx_er_db, FALSE);
    }
    SHR_FUNC_EXIT;
}

int
dnx_err_recovery_module_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_er_db_key_init(unit);
    SHR_IF_ERR_EXIT(dnx_err_recovery_common_init(unit));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int
dnx_err_recovery_module_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_er_db_key_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}

static inline void
dnx_er_transaction_register_internal(
    int is_tmp_excluded_thread,
    dnx_er_db_t * dnx_er_db)
{
    dnx_er_db->dnx_err_recovery_manager->thread_transaction_counter++;
    if (is_tmp_excluded_thread)
    {
        dnx_er_db->dnx_err_recovery_manager->tmp_excluded_thread_transaction_counter++;
    }
}

static inline void
dnx_er_transaction_unregister_internal(
    int is_tmp_excluded_thread,
    dnx_er_db_t * dnx_er_db)
{
    dnx_er_db->dnx_err_recovery_manager->thread_transaction_counter--;

    if (is_tmp_excluded_thread)
    {
        dnx_er_db->dnx_err_recovery_manager->tmp_excluded_thread_transaction_counter--;
    }
}

shr_error_e
dnx_er_transaction_register(
    int unit)
{
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (soc_is(unit, DNXF_DEVICE))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_er_db_get(unit, &dnx_er_db));

    SHR_NULL_CHECK(dnx_er_db, _SHR_E_INIT, "dnx_er_db");

    if (!dnx_err_recovery_are_prerequisites_met(unit, dnx_er_db))
    {
        SHR_EXIT();
    }

    dnx_er_transaction_register_internal(FALSE, dnx_er_db);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_er_transaction_unregister(
    int unit)
{
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (soc_is(unit, DNXF_DEVICE))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_er_db_get(unit, &dnx_er_db));

    SHR_NULL_CHECK(dnx_er_db, _SHR_E_INIT, "dnx_er_db");

    if (!dnx_err_recovery_are_prerequisites_met(unit, dnx_er_db))
    {
        SHR_EXIT();
    }

    dnx_er_transaction_unregister_internal(FALSE, dnx_er_db);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_er_tmp_excluded_add(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_err_recovery_are_prerequisites_met(unit, dnx_er_db))
    {
        SHR_EXIT();
    }

    dnx_er_transaction_register_internal(TRUE, dnx_er_db);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_er_tmp_excluded_remove(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_err_recovery_are_prerequisites_met(unit, dnx_er_db))
    {
        SHR_EXIT();
    }

    dnx_er_transaction_unregister_internal(TRUE, dnx_er_db);

exit:
    SHR_FUNC_EXIT;
}

static uint8
dnx_er_is_tmp_excluded(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    if (!dnx_err_recovery_are_prerequisites_met(unit, dnx_er_db))
    {
        return TRUE;
    }

    return (dnx_er_db->dnx_err_recovery_manager->tmp_excluded_thread_transaction_counter > 0);
}

shr_error_e
dnx_err_recovery_api_counter_inc(
    int unit)
{
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        SHR_EXIT();
    }

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit, dnx_er_db);

    DNX_ERR_RECOVERY_EXIT_IF_NO_VALIDATION(unit);

    if (dnx_er_db->dnx_err_recovery_manager->api_counter + 1 > DNX_ERR_RECOVERY_MAX_NUM_NESTED_APIS)
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "err recovery ERROR: API counter cannot be more than 3.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }

    dnx_er_db->dnx_err_recovery_manager->api_to_trans_map[dnx_er_db->dnx_err_recovery_manager->api_counter] =
        dnx_er_db->dnx_err_recovery_manager->transaction_counter;

    dnx_er_db->dnx_err_recovery_manager->api_counter++;

    if (1 == dnx_er_db->dnx_err_recovery_manager->api_counter)
    {
        dnx_er_db->dnx_err_recovery_manager->api_status = DNX_ERR_RECOVERY_API_STATUS_UNKNOWN;
        dnx_er_db->dnx_err_recovery_manager->api_status_count = 0;

        dnx_er_regress_top_level_api_flags_reset(unit, dnx_er_db);
    }

exit:
    SHR_FUNC_EXIT;
}

static uint8
dnx_err_recovery_api_status_is_valid(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    int api_count_floor = 0;

#ifdef BCM_WARM_BOOT_SUPPORT

    api_count_floor = 1;
#endif

    if (dnx_er_is_tmp_excluded(unit, dnx_er_db))
    {
        return TRUE;
    }
    else
    {
        return (dnx_er_db->dnx_err_recovery_manager->api_status != DNX_ERR_RECOVERY_API_STATUS_UNKNOWN &&
                dnx_er_db->dnx_err_recovery_manager->api_status_count > api_count_floor);
    }

}

shr_error_e
dnx_err_recovery_api_counter_dec(
    int unit,
    uint8 bcm_api_error)
{
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        SHR_EXIT();
    }

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit, dnx_er_db);

    DNX_ERR_RECOVERY_EXIT_IF_NO_VALIDATION(unit);

    if (0 == dnx_er_db->dnx_err_recovery_manager->api_counter)
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "err recovery ERROR: API counter cannot be less than 0.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }

    if (1 == dnx_er_db->dnx_err_recovery_manager->api_counter)
    {

        if (!dnx_err_recovery_api_status_is_valid(unit, dnx_er_db) && !bcm_api_error)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: DNX does not use DNX_ERR_RECOVERY_START or DNX_ERR_RECOVERY_NOT_NEEDED.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
        dnx_er_db->dnx_err_recovery_manager->api_status = DNX_ERR_RECOVERY_API_STATUS_UNKNOWN;
        dnx_er_db->dnx_err_recovery_manager->api_status_count = 0;

        dnx_er_regress_top_level_api_flags_reset(unit, dnx_er_db);
    }

    dnx_er_db->dnx_err_recovery_manager->api_counter--;

    if (dnx_er_db->dnx_err_recovery_manager->api_to_trans_map
        [dnx_er_db->dnx_err_recovery_manager->api_counter] != dnx_er_db->dnx_err_recovery_manager->transaction_counter)
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "err recovery ERROR: API opens a transaction but never closes it.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_err_recovery_not_needed(
    int unit)
{
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        SHR_EXIT();
    }

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit, dnx_er_db);

    if (dnx_er_db->dnx_err_recovery_manager->api_status == DNX_ERR_RECOVERY_API_STATUS_UNKNOWN)
    {
        dnx_er_db->dnx_err_recovery_manager->api_status = DNX_ERR_RECOVERY_API_NOT_NEEDED;
    }

    dnx_er_db->dnx_err_recovery_manager->api_status_count++;

exit:
    SHR_FUNC_EXIT;
}

uint8
dnx_err_recovery_are_prerequisites_met(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    return (SOC_UNIT_VALID(unit) &&
            dnx_err_recovery_common_is_enabled(unit) &&
            dnx_rollback_journal_is_done_init(unit) &&
            !SOC_IS_DETACHING(unit) &&
            dnx_err_recovery_flag_is_on(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                        DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED));
}

uint8
dnx_err_recovery_is_on_test(
    int unit)
{
    dnx_er_db_t *dnx_er_db = NULL;

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        return FALSE;
    }

    return dnx_err_recovery_rollback_is_on(unit, dnx_er_db);
}

shr_error_e
dnx_err_recovery_tmp_allow(
    int unit,
    uint8 is_on)
{
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        SHR_EXIT();
    }

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit, dnx_er_db);

    DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN;
    DNX_ERR_RECOVERY_EXIT_IF_INVALIDATED(unit, dnx_er_db);

    if (!is_on)
    {
        SHR_IF_ERR_EXIT(dnx_er_regress_comparison_tmp_allow(unit, dnx_er_db, is_on));

        dnx_er_db->dnx_err_recovery_manager->is_suppressed_counter++;
        dnx_err_recovery_flag_clear(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                    DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);
    }
    else
    {
        if (0 == dnx_er_db->dnx_err_recovery_manager->is_suppressed_counter)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "err recovery ERROR: Suppression counter cannot be less than 0.\n%s%s%s", EMPTY,
                                     EMPTY, EMPTY);
        }

        dnx_er_db->dnx_err_recovery_manager->is_suppressed_counter--;
        if (0 == dnx_er_db->dnx_err_recovery_manager->is_suppressed_counter)
        {
            dnx_err_recovery_flag_set(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                      DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);
        }

        SHR_IF_ERR_EXIT(dnx_er_regress_comparison_tmp_allow(unit, dnx_er_db, is_on));

    }

exit:
    SHR_FUNC_EXIT;
}

uint8
dnx_err_recovery_hw_access_is_valid(
    int unit)
{
    dnx_er_db_t *dnx_er_db = NULL;

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        return TRUE;
    }

    if (!dnx_err_recovery_rollback_is_on(unit, dnx_er_db))
    {
        return TRUE;
    }

    if (dnx_dbal_journal_is_tmp_suppressed_unsafe(unit, dnx_er_db, FALSE))
    {
        return TRUE;
    }

    return dnx_err_recovery_is_dbal_access_region(unit, dnx_er_db);
}

shr_error_e
dnx_err_recovery_unsupported_dbal_unsup_tbls_bypass(
    int unit,
    uint8 is_on)
{
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        SHR_EXIT();
    }

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit, dnx_er_db);
    DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN;
    DNX_ERR_RECOVERY_EXIT_IF_INVALIDATED(unit, dnx_er_db);

    SHR_IF_ERR_EXIT(dnx_dbal_journal_unsupported_tables_bypass(unit, dnx_er_db, is_on));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_err_recovery_transaction_no_support_counter_inc(
    int unit,
    const char *func_name)
{
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_er_db_get(unit, &dnx_er_db));

    if (!dnx_err_recovery_are_prerequisites_met(unit, dnx_er_db))
    {
        return _SHR_E_NONE;
    }

    SHR_IF_ERR_EXIT(dnx_er_tmp_excluded_add(unit, dnx_er_db));

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit, dnx_er_db);

    if (dnx_er_db->dnx_err_recovery_manager->transaction_counter != 0)
    {
        SHR_IF_ERR_EXIT(dnx_generic_state_journal_all_tmp_suppress_unsafe(unit, dnx_er_db, TRUE));
        SHR_IF_ERR_EXIT(dnx_sw_state_journal_all_tmp_suppress_unsafe(unit, dnx_er_db, TRUE));
        SHR_IF_ERR_EXIT(dnx_dbal_journal_all_tmp_suppress_unsafe(unit, dnx_er_db, TRUE));
    }
    else if (dnx_err_recovery_is_on(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON)
             && !dnx_state_comparison_is_suppressed(unit, dnx_er_db))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "The API - %s - does not support Error Recovery, yet it is being used while comparison journal is enabled.\n Please disable comparison journaling or do not use APIs that does not support ER.",
                     func_name);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_err_recovery_transaction_no_support_counter_dec(
    int unit)
{
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "error recovery manager ERROR: cannot decrement the no support counter in transactions where error recovery is NOT_NEEDED/NO_SUPPORT and the counter wasn't previously incremented.\n");
    }

    if (!dnx_err_recovery_are_prerequisites_met(unit, dnx_er_db))
    {
        return _SHR_E_NONE;
    }

    SHR_IF_ERR_EXIT(dnx_er_tmp_excluded_remove(unit, dnx_er_db));

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit, dnx_er_db);

    if (dnx_er_db->dnx_err_recovery_manager->transaction_counter != 0)
    {
        SHR_IF_ERR_EXIT(dnx_generic_state_journal_all_tmp_suppress_unsafe(unit, dnx_er_db, FALSE));
        SHR_IF_ERR_EXIT(dnx_sw_state_journal_all_tmp_suppress_unsafe(unit, dnx_er_db, FALSE));
        SHR_IF_ERR_EXIT(dnx_dbal_journal_all_tmp_suppress_unsafe(unit, dnx_er_db, FALSE));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_err_recovery_transaction_start_internal(
    int unit,
    dnx_er_db_t * dnx_er_db)
{
    uint8 is_top_level_transaction = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_er_transaction_register(unit));

    if (0 == dnx_er_db->dnx_err_recovery_manager->transaction_counter)
    {
        is_top_level_transaction = TRUE;
    }

    if (dnx_err_recovery_is_validation_enabled(unit))
    {

        if (is_top_level_transaction)
        {
            if (!dnx_err_recovery_flag_is_on
                (unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK, DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED))
            {
                DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                            "err recovery ERROR: transaction cannot be invalidated before it has been started.\n%s%s%s",
                                                            EMPTY, EMPTY, EMPTY);
            }

            if (0 != dnx_er_db->dnx_err_recovery_manager->is_suppressed_counter)
            {
                DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                            "err recovery ERROR: transaction cannot be suppressed before it has been started.\n%s%s%s",
                                                            EMPTY, EMPTY, EMPTY);
            }

            if (dnx_dbal_journal_are_unsupported_tables_bypassed(unit, dnx_er_db))
            {
                DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                            "err recovery ERROR: transaction cannot be started with temporary bypass of unsupported dbal tables set.\n%s%s%s",
                                                            EMPTY, EMPTY, EMPTY);
            }

            if (0 != dnx_er_db->dnx_err_recovery_manager->entry_counter)
            {
                DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                            "err recovery ERROR: entries found prior to transaction start.\n%s%s%s",
                                                            EMPTY, EMPTY, EMPTY);
            }
        }
    }

    dnx_er_db->dnx_err_recovery_manager->transaction_counter++;
    dnx_err_recovery_flag_set(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                              DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);

    dnx_err_recovery_transaction_bookmark_set(unit, dnx_er_db,
                                              dnx_er_db->dnx_err_recovery_manager->transaction_counter,
                                              dnx_er_db->dnx_err_recovery_manager->entry_counter);

    if (dnx_er_db->dnx_err_recovery_manager->api_status == DNX_ERR_RECOVERY_API_STATUS_UNKNOWN)
    {
        dnx_er_db->dnx_err_recovery_manager->api_status = DNX_ERR_RECOVERY_API_OPTS_IN;
    }

    dnx_er_db->dnx_err_recovery_manager->api_status_count++;

    SHR_IF_ERR_EXIT(dnx_er_regress_transaction_start(unit, dnx_er_db, is_top_level_transaction));

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
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_er_db_get(unit, &dnx_er_db));

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit, dnx_er_db);

    if (dnx_er_is_tmp_excluded(unit, dnx_er_db))
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

    SHR_IF_ERR_EXIT(dnx_err_recovery_transaction_start_internal(unit, dnx_er_db));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_err_recovery_transaction_end_internal(
    int unit,
    dnx_er_db_t * dnx_er_db,
    int api_result)
{
    SHR_FUNC_INIT_VARS(unit);

    if (1 == (dnx_er_db->dnx_err_recovery_manager->transaction_counter))
    {

        if (0 != dnx_er_db->dnx_err_recovery_manager->is_suppressed_counter)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: temporary suppression is not disabled at the end of the transaction.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }

        if (dnx_dbal_journal_are_unsupported_tables_bypassed(unit, dnx_er_db))
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: temporary bypass of unsupported dbal table is not disabled at the end of transaction.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }

        if (!dnx_err_recovery_flag_is_on
            (unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK, DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED))
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: unsupported features were accessed during the transaction.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }

        if (SHR_FAILURE(api_result) || dnx_er_regress_transaction_end_should_abort(unit, dnx_er_db))
        {
            SHR_IF_ERR_EXIT(dnx_err_recovery_abort_unsafe(unit, dnx_er_db));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_err_recovery_commit_unsafe(unit, dnx_er_db));
        }
    }
    else
    {

        if (SHR_FAILURE(api_result) || dnx_er_regress_transaction_end_should_abort(unit, dnx_er_db))
        {
            SHR_IF_ERR_EXIT(dnx_err_recovery_abort_unsafe(unit, dnx_er_db));
        }
    }

    SHR_IF_ERR_EXIT(dnx_er_regress_transaction_end(unit, dnx_er_db));

exit:
    dnx_er_db->dnx_err_recovery_manager->transaction_counter--;

    if (0 == dnx_er_db->dnx_err_recovery_manager->transaction_counter)
    {
        dnx_err_recovery_transaction_counters_reset(unit, dnx_er_db);
    }
    dnx_er_transaction_unregister(unit);

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_err_recovery_transaction_end(
    int unit,
    bcm_switch_module_t module_id,
    int api_result)
{
    uint8 enabled = FALSE;

    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "error recovery manager ERROR: cannot end an error recovery transaction that was never started.\n");
    }

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit, dnx_er_db);

    DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN;

    if (dnx_er_is_tmp_excluded(unit, dnx_er_db))
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

    SHR_IF_ERR_EXIT(dnx_err_recovery_transaction_end_internal(unit, dnx_er_db, api_result));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_rollback_journal_start(
    int unit)
{
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_er_db_get(unit, &dnx_er_db));

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit, dnx_er_db);

    if (dnx_er_is_tmp_excluded(unit, dnx_er_db))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_err_recovery_transaction_start_internal(unit, dnx_er_db));

    dnx_er_regress_journal_start(unit);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_rollback_journal_end(
    int unit,
    uint8 rollback_journal)
{
    shr_error_e error_type = _SHR_E_NONE;

    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "error recovery manager ERROR: cannot end an error recovery transaction that was never started.\n");
    }

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit, dnx_er_db);
    DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN;

    if (rollback_journal)
    {

        error_type = _SHR_E_INTERNAL;
    }

    SHR_IF_ERR_EXIT(dnx_err_recovery_transaction_end_internal(unit, dnx_er_db, error_type));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_err_recovery_transaction_invalidate(
    int unit)
{
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        SHR_EXIT();
    }

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit, dnx_er_db);
    DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN;
    DNX_ERR_RECOVERY_EXIT_IF_INVALIDATED(unit, dnx_er_db);

    dnx_err_recovery_flag_clear(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED);

    SHR_IF_ERR_EXIT(dnx_err_recovery_transaction_clear(unit, dnx_er_db));

    SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "err recovery ERROR: feature not supported.\n%s%s%s", EMPTY, EMPTY, EMPTY);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

void
dnx_rollback_journal_appl_init_state_change(
    int unit,
    uint8 is_init)
{
    dnx_er_regress_appl_init_state_change(unit, is_init);
}

shr_error_e
dnx_rollback_journal_print_journal(
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
            SHR_IF_ERR_EXIT(dnx_sw_state_journal_print(unit, dnx_er_db, FALSE));
            break;
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL:
            SHR_IF_ERR_EXIT(dnx_dbal_journal_print(unit, dnx_er_db, FALSE));
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
    dnx_er_db_t *dnx_er_db = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        SHR_EXIT();
    }

    switch (type)
    {
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE:
            SHR_IF_ERR_EXIT(dnx_sw_state_journal_logger_state_change(unit, dnx_er_db, FALSE));
            break;
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL:
            SHR_IF_ERR_EXIT(dnx_dbal_journal_logger_state_change(unit, dnx_er_db, FALSE));
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
    dnx_er_db_t *dnx_er_db = NULL;

    if (dnx_er_db_get_if_initialized(unit, &dnx_er_db) != _SHR_E_NONE)
    {
        return FALSE;
    }

    if (dnx_rollback_journal_is_init_check_bypass(unit, 1) != _SHR_E_NONE)
    {
        return FALSE;
    }

    ret = dnx_err_recovery_flag_is_on(unit, dnx_er_db, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                      DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED);

    if (dnx_rollback_journal_is_init_check_bypass(unit, 0) != _SHR_E_NONE)
    {
        return FALSE;
    }

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

int
dnx_er_leak_verification(
    int unit)
{
    dnx_er_db_t *dnx_er_db = NULL;
    int rv;

    rv = dnx_er_db_get(unit, &dnx_er_db);

    assert(rv == _SHR_E_NONE);

    if ((dnx_er_db->dnx_err_recovery_manager->thread_transaction_counter > 0)
        || (dnx_er_db->dnx_err_recovery_manager->tmp_excluded_thread_transaction_counter > 0))
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META
                                   ("ER is still active at the end of test/command.\n Likely caused by an ER transaction not being closed or left suppressed.\n")));
        return TRUE;
    }
    return FALSE;
}

#undef _ERR_MSG_MODULE_NAME
