/**
 * \file dbal_journal.c
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * INFO: This module is one of the components needed for Error Recovery,
 * it is a rollback journal, saving the old values that were overridden
 * by dbal access calls since the beginning of the last transaction.
 */

#include <shared/bsl.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>
#include <soc/dnx/dbal/dbal_journal.h>
#include <soc/dnx/recovery/rollback_journal.h>
#include <soc/dnx/recovery/rollback_journal_utils.h>
#include <shared/shrextend/shrextend_debug.h>
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/dbal/dbal_string_mgmt.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/dnx_er_threading.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

static dnx_dbal_journal_manager_t dnx_dbal_journal_manager[SOC_MAX_NUM_DEVICES] = { {0} };

#define DNX_DBAL_ROLLBACK_JOURNAL_NUM 0
#define DNX_DBAL_COMPARISON_JOURNAL_NUM 1

#define DNX_DBAL_ROLLBACK_JOURNAL_TRANSACTION dnx_dbal_journal_manager[unit].journals[DNX_DBAL_ROLLBACK_JOURNAL_NUM].transaction
#define DNX_DBAL_COMPARISON_JOURNAL_TRANSACTION dnx_dbal_journal_manager[unit].journals[DNX_DBAL_COMPARISON_JOURNAL_NUM].transaction

#define DNX_DBAL_ROLLBACK_JOURNAL_HANDLE dnx_dbal_journal_manager[unit].journals[DNX_DBAL_ROLLBACK_JOURNAL_NUM].handle
#define DNX_DBAL_COMPARISON_JOURNAL_HANDLE dnx_dbal_journal_manager[unit].journals[DNX_DBAL_COMPARISON_JOURNAL_NUM].handle


#define DNX_DBAL_JOURNAL_DEFS\
    SHR_FUNC_INIT_VARS(unit);\


#define DNX_DBAL_FUNC_RETURN\
    SHR_EXIT();\
exit:\
    SHR_FUNC_EXIT

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING

#define DNX_DBAL_JOURNAL_ACCESS_NOTIFY(_unit, _table_ptr)                                                         \
    do {                                                                                                          \
        if(NULL != dnx_dbal_journal_manager[unit].journals[DNX_DBAL_COMPARISON_JOURNAL_NUM].access_cb) {          \
            dnx_dbal_journal_manager[unit].journals[DNX_DBAL_COMPARISON_JOURNAL_NUM].access_cb(_unit, _table_ptr);\
        }                                                                                                         \
        if(NULL != dnx_dbal_journal_manager[unit].journals[DNX_DBAL_ROLLBACK_JOURNAL_NUM].access_cb) {            \
            dnx_dbal_journal_manager[unit].journals[DNX_DBAL_ROLLBACK_JOURNAL_NUM].access_cb(_unit, _table_ptr);  \
        }                                                                                                         \
    } while(0)
#else
#define DNX_DBAL_JOURNAL_ACCESS_NOTIFY(_unit, _table_ptr)
#endif


static uint8
dnx_dbal_journal_is_table_excluded(
    int unit,
    dbal_tables_e table_id,
    uint8 is_comparison)
{
    
    dbal_tables_e comparison_perm_excluded[] = {
        DBAL_TABLE_EMPTY
    };

    
    dbal_tables_e rollback_perm_excluded[] = {
        DBAL_TABLE_EMPTY
    };

    int ii = 0;
    int nof_tables = 0;

    if (!is_comparison)
    {
        
        for (ii = 0; ii < DNX_DBAL_ROLLBACK_JOURNAL_TRANSACTION.excluded_tables_count; ii++)
        {
            if (table_id == DNX_DBAL_ROLLBACK_JOURNAL_TRANSACTION.excluded_tables[ii])
            {
                return TRUE;
            }
        }

        
        nof_tables = sizeof(comparison_perm_excluded) / sizeof(dbal_tables_e);

        for (ii = 0; ii < nof_tables; ii++)
        {
            if (comparison_perm_excluded[ii] == table_id)
            {
                return TRUE;
            }
        }
    }
    else
    {
        
        for (ii = 0; ii < DNX_DBAL_COMPARISON_JOURNAL_TRANSACTION.excluded_tables_count; ii++)
        {
            if (table_id == DNX_DBAL_COMPARISON_JOURNAL_TRANSACTION.excluded_tables[ii])
            {
                return TRUE;
            }
        }

        
        nof_tables = sizeof(rollback_perm_excluded) / sizeof(dbal_tables_e);

        for (ii = 0; ii < nof_tables; ii++)
        {
            if (rollback_perm_excluded[ii] == table_id)
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}


static shr_error_e
dnx_dbal_journal_entry_print_cb(
    int unit,
    uint8 *metadata,
    uint8 *payload)
{
    dnx_dbal_journal_entry_t *ptr_entry = NULL;
    dbal_entry_handle_t *ptr_payload = NULL;

    DNX_DBAL_JOURNAL_DEFS;

    ptr_entry = (dnx_dbal_journal_entry_t *) metadata;
    ptr_payload = (dbal_entry_handle_t *) payload;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == ptr_entry || NULL == ptr_payload)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                    "dbal journal ERROR: null entry found in journal.\n%s%s%s", EMPTY,
                                                    EMPTY, EMPTY);
    }
#endif 

    LOG_CLI((BSL_META("********************************* DBAL Journal *********************************\n")));

    switch (ptr_entry->action)
    {
        case DBAL_ACTION_ENTRY_CLEAR:
            LOG_CLI((BSL_META("Type: CLEAR\n")));
            break;
        case DBAL_ACTION_ENTRY_COMMIT:
            LOG_CLI((BSL_META("Type: COMMIT\n")));
            break;
        default:
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "dbal journal ERROR: unrecognized entry type.\n%s%s%s", EMPTY,
                                     EMPTY, EMPTY);
    }

    LOG_CLI((BSL_META("Table name: %s\n"), ptr_payload->table->table_name));

    LOG_CLI((BSL_META("Data associated with DBAL entry:\n")));

    SHR_IF_ERR_EXIT(dbal_entry_print(unit, ptr_payload, TRUE));

    LOG_CLI((BSL_META("********************************* DBAL Journal End *****************************\n\n")));

    DNX_DBAL_FUNC_RETURN;
}


static shr_error_e
dnx_dbal_journal_rollback_common(
    int unit,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    dnx_dbal_journal_entry_t *ptr_entry = NULL;
    dbal_entry_handle_t *ptr_payload = NULL;
    uint8 is_logging = 0;

    DNX_DBAL_JOURNAL_DEFS;

    ptr_entry = (dnx_dbal_journal_entry_t *) metadata;
    ptr_payload = (dbal_entry_handle_t *) payload;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == ptr_entry || NULL == ptr_payload)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                    "dbal journal ERROR: null entry found in journal.\n%s%s%s", EMPTY,
                                                    EMPTY, EMPTY);
    }
#endif 

    
    ptr_payload->er_flags = 0x0;
    dnx_rollback_journal_is_logging(unit, DNX_DBAL_ROLLBACK_JOURNAL_HANDLE, &is_logging);
    switch (ptr_entry->action)
    {
        case DBAL_ACTION_ENTRY_COMMIT:
            SHR_IF_ERR_EXIT(dbal_actions_access_entry_commit(unit, ptr_payload, NULL));
            break;
        case DBAL_ACTION_ENTRY_CLEAR:
            SHR_IF_ERR_EXIT(dbal_actions_access_entry_clear(unit, ptr_payload));
            break;
        default:
            DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "dbal journal ERROR: unrecognized entry type.\n%s%s%s", EMPTY,
                                     EMPTY, EMPTY);
    }

    DNX_DBAL_FUNC_RETURN;
}


static shr_error_e
dnx_dbal_journal_handle_trigger_value_mask(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_handle_t * get_entry_handle,
    uint8 *ptr_compare_payload)
{
    multi_res_info_t result_set;
    int result_type_id;
    int result_id;
    uint8 contains_comparable_fields = FALSE;
    dbal_table_field_info_t field_info;
    dbal_logical_table_t *table = NULL;
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    DNX_DBAL_JOURNAL_DEFS;

    if (NULL == get_entry_handle)
    {
        SHR_EXIT();
    }

    table = entry_handle->table;

    for (result_type_id = 0; result_type_id < table->nof_result_types; result_type_id++)
    {
        result_set = table->multi_res_info[result_type_id];
        for (result_id = 0; result_id < result_set.nof_result_fields; result_id++)
        {
            field_info = result_set.results_info[result_id];

            if (DBAL_PERMISSION_TRIGGER == field_info.permission)
            {
                
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_val,
                                                               field_info.bits_offset_in_buffer,
                                                               field_info.field_nof_bits,
                                                               &(entry_handle->phy_entry.payload[0])));

                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_val,
                                                               field_info.bits_offset_in_buffer,
                                                               field_info.field_nof_bits,
                                                               &(get_entry_handle->phy_entry.payload[0])));
            }

            
            if (DBAL_PERMISSION_TRIGGER != field_info.permission
                && DBAL_PERMISSION_READONLY != field_info.permission
                && DBAL_PERMISSION_WRITEONLY != field_info.permission)
            {
                contains_comparable_fields = TRUE;
            }
        }
    }

    *ptr_compare_payload = contains_comparable_fields;

    DNX_DBAL_FUNC_RETURN;
}


static shr_error_e
dnx_dbal_comparison_journal_roll_back_entry_cb(
    int unit,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    int rv = _SHR_E_NONE;
    int hl_entry_is_default = 0;
    uint8 entry_not_found = FALSE;
    uint8 is_journaled_entry_default = FALSE;
    uint8 entries_match = TRUE;
    uint8 is_compare_payload = TRUE;
    dnx_dbal_journal_entry_t *ptr_entry = NULL;
    dbal_entry_handle_t *ptr_payload = NULL;
    dbal_entry_handle_t *get_entry_handle = NULL;
    dbal_logical_table_t *table = NULL;

    SHR_FUNC_INIT_VARS(unit);

    ptr_entry = (dnx_dbal_journal_entry_t *) metadata;
    ptr_payload = (dbal_entry_handle_t *) payload;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == ptr_entry || NULL == ptr_payload)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                    "dbal journal ERROR: null entry found in journal.\n%s%s%s", EMPTY,
                                                    EMPTY, EMPTY);
    }
#endif 

    
    ptr_payload->er_flags = 0x0;

    
    SHR_ALLOC_SET_ZERO(get_entry_handle, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n", EMPTY, EMPTY,
                       EMPTY);

    rv = dbal_actions_access_entry_get(unit, ptr_payload, get_entry_handle, 0);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    
    SHR_IF_ERR_EXIT(dnx_dbal_journal_handle_trigger_value_mask
                    (unit, ptr_payload, get_entry_handle, &is_compare_payload));

    
    entry_not_found = (_SHR_E_NOT_FOUND == rv) || (NULL == get_entry_handle);

    if (!entry_not_found && !dbal_tables_is_non_direct(unit, ptr_payload->table_id))
    {
        SHR_IF_ERR_EXIT(dbal_entry_direct_is_default(unit, get_entry_handle, &hl_entry_is_default));
        entry_not_found = (hl_entry_is_default != 0);

        SHR_IF_ERR_EXIT(dbal_entry_direct_is_default(unit, ptr_payload, &hl_entry_is_default));
        is_journaled_entry_default = (hl_entry_is_default != 0);
    }

    table = ptr_payload->table;

    
    if (DBAL_ACTION_ENTRY_CLEAR == ptr_entry->action)
    {
        
        if (!entry_not_found
            && !(table->table_type == DBAL_TABLE_TYPE_TCAM && table->access_method == DBAL_ACCESS_METHOD_MDB))
        {
            LOG_CLI((BSL_META("\n################################################################################\n")));
            LOG_CLI((BSL_META("DBAL entry found when it should not exist for table stamp: %s, core id: %d\n"),
                     table->table_name, (get_entry_handle->core_id)));
            LOG_CLI((BSL_META("Data associated with DBAL entry:\n")));
            SHR_IF_ERR_EXIT(dbal_entry_print(unit, get_entry_handle, TRUE));
            LOG_CLI((BSL_META("################################################################################\n")));

            SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_INTERNAL);
        }
    }
    else if (DBAL_ACTION_ENTRY_COMMIT == ptr_entry->action)
    {
        
        if (entry_not_found && !is_journaled_entry_default)
        {
            LOG_CLI((BSL_META("\n################################################################################\n")));
            LOG_CLI((BSL_META("DBAL entry not found when it should exist for table stamp: %s, core id: %d\n"),
                     table->table_name, (ptr_payload->core_id)));
            LOG_CLI((BSL_META("Data associated with DBAL entry:\n")));
            SHR_IF_ERR_EXIT(dbal_entry_print(unit, ptr_payload, TRUE));
            LOG_CLI((BSL_META("################################################################################\n")));

            SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_INTERNAL);
        }

        
        if (entries_match && is_compare_payload)
        {
            entries_match =
                (sal_memcmp
                 (ptr_payload->phy_entry.payload, get_entry_handle->phy_entry.payload,
                  WORDS2BYTES(BITS2WORDS(table->multi_res_info[ptr_payload->cur_res_type].entry_payload_size))) == 0);
        }

        
        if (DBAL_TABLE_IS_TCAM(table))
        {
            
            if (entries_match)
            {
                entries_match =
                    (sal_memcmp
                     (ptr_payload->phy_entry.key, get_entry_handle->phy_entry.key,
                      DBAL_PHYSICAL_KEY_SIZE_IN_BYTES) == 0);
            }

            
            if (entries_match)
            {
                entries_match =
                    (sal_memcmp
                     (ptr_payload->phy_entry.k_mask, get_entry_handle->phy_entry.k_mask,
                      DBAL_PHYSICAL_KEY_SIZE_IN_BYTES) == 0);
            }

        }

        
        if (!entries_match)
        {
            LOG_CLI((BSL_META("\n################################################################################\n")));
            LOG_CLI((BSL_META
                     ("Found difference in state before and after comparison for table stamp: %s, core id: %d\n"),
                     table->table_name, (get_entry_handle->core_id)));
            LOG_CLI((BSL_META("Entry data before comparison journal start:\n")));
            SHR_IF_ERR_EXIT(dbal_entry_print(unit, ptr_payload, TRUE));
            LOG_CLI((BSL_META("Entry data after comparison journal end:\n")));
            SHR_IF_ERR_EXIT(dbal_entry_print(unit, get_entry_handle, TRUE));
            LOG_CLI((BSL_META("################################################################################\n")));

            SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_INTERNAL);
        }
    }
    else
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "dbal comparison journal ERROR: unrecognized dbal journal entry type.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }

exit:
    if (NULL != get_entry_handle)
    {
        SHR_FREE(get_entry_handle);
    }
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_dbal_comparison_journal_entries_are_distinct(
    int unit,
    uint8 *first_metadata,
    uint8 *first_payload,
    uint8 *second_metadata,
    uint8 *second_payload,
    uint8 *result)
{
    uint8 entries_match = FALSE;

    dbal_entry_handle_t *first_dbal_payload = (dbal_entry_handle_t *) first_payload;
    dbal_entry_handle_t *second_dbal_payload = (dbal_entry_handle_t *) second_payload;

    DNX_DBAL_JOURNAL_DEFS;

    
    if (first_dbal_payload->table_id == second_dbal_payload->table_id)
    {
        entries_match =
            (sal_memcmp
             (first_dbal_payload->phy_entry.key, second_dbal_payload->phy_entry.key,
              DBAL_PHYSICAL_KEY_SIZE_IN_BYTES) == 0);
    }

    
    if (entries_match
        && (first_dbal_payload->core_id == DBAL_CORE_ALL) && (second_dbal_payload->core_id != DBAL_CORE_ALL))
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "dbal comparison journal ERROR: unsupported matching between two dbal journal entries.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    
    *result = !(entries_match
                && ((first_dbal_payload->core_id == second_dbal_payload->core_id)
                    || (second_dbal_payload->core_id == DBAL_CORE_ALL)));
    DNX_DBAL_FUNC_RETURN;
}


static shr_error_e
dnx_dbal_rollback_journal_entry_state_get(
    int unit,
    dbal_logical_table_t * table,
    dnx_rollback_journal_entry_state * entry_state)
{
    DNX_DBAL_JOURNAL_DEFS;

    if ((DBAL_ACCESS_METHOD_HARD_LOGIC == table->access_method)
        || (DBAL_ACCESS_METHOD_TCAM_CS == table->access_method)
        || (DBAL_ACCESS_METHOD_SW_STATE == table->access_method))
    {
        
        *entry_state = DNX_ROLLBACK_JOURNAL_ENTRY_STATE_ALWAYS_VALID;
    }
    else if ((DBAL_ACCESS_METHOD_MDB == table->access_method)
             || (DBAL_ACCESS_METHOD_PEMLA == table->access_method) || (DBAL_ACCESS_METHOD_KBP == table->access_method))
    {
        
        
        *entry_state = DNX_ROLLBACK_JOURNAL_ENTRY_STATE_VALID;
    }
    else
    {
        
        *entry_state = DNX_ROLLBACK_JOURNAL_ENTRY_STATE_INVALID;
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "dbal journal ERROR: Unrecognized access method entry attempted to be inserted in dbal journal.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    DNX_DBAL_FUNC_RETURN;
}


static shr_error_e
dnx_dbal_journal_entry_insert(
    int unit,
    dbal_entry_handle_t * ptr_handle,
    dbal_actions_e action,
    dnx_rollback_journal_rollback_handler rollback_handler,
    uint8 is_er,
    uint8 is_cmp,
    uint8 is_dbal_error)
{
    dnx_dbal_journal_entry_t journal_entry;

    dnx_rollback_journal_entry_state entry_state = DNX_ROLLBACK_JOURNAL_ENTRY_STATE_VALID;

    DNX_DBAL_JOURNAL_DEFS;

    journal_entry.action = action;

    
    SHR_IF_ERR_EXIT(dnx_dbal_rollback_journal_entry_state_get(unit, ptr_handle->table, &entry_state));

    if (is_er
        && !dnx_dbal_journal_is_table_excluded(unit, ptr_handle->table_id, FALSE)
        && (!is_dbal_error || (DNX_ROLLBACK_JOURNAL_ENTRY_STATE_ALWAYS_VALID == entry_state)))
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_new(unit,
                                                       DNX_DBAL_ROLLBACK_JOURNAL_HANDLE,
                                                       (uint8 *) (&journal_entry),
                                                       sizeof(dnx_dbal_journal_entry_t),
                                                       (uint8 *) ptr_handle,
                                                       sizeof(dbal_entry_handle_t),
                                                       entry_state,
                                                       rollback_handler, NULL, dnx_dbal_journal_entry_print_cb));
    }

    if (is_cmp && !dnx_dbal_journal_is_table_excluded(unit, ptr_handle->table_id, TRUE))
    {
        
        SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_new(unit,
                                                       DNX_DBAL_COMPARISON_JOURNAL_HANDLE,
                                                       (uint8 *) (&journal_entry),
                                                       sizeof(dnx_dbal_journal_entry_t),
                                                       (uint8 *) ptr_handle,
                                                       sizeof(dbal_entry_handle_t),
                                                       DNX_ROLLBACK_JOURNAL_ENTRY_STATE_VALID,
                                                       &dnx_dbal_comparison_journal_roll_back_entry_cb,
                                                       NULL, dnx_dbal_journal_entry_print_cb));
    }

    DNX_DBAL_FUNC_RETURN;
}


uint8
dnx_dbal_journal_is_done_init(
    int unit)
{
    return dnx_rollback_journal_is_done_init(unit);
}


shr_error_e
dnx_dbal_journal_initialize(
    int unit,
    dnx_rollback_journal_cbs_t rollback_journal_cbs,
    dnx_dbal_journal_access_cb access_cb,
    uint8 is_comparison)
{
    DNX_DBAL_JOURNAL_DEFS;

    dnx_dbal_journal_manager[unit].unsupported_table_bypass_counter = 0;

    if (NULL == rollback_journal_cbs.is_on)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "dbal journal ERROR: journal is on callback must be set.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    if (is_comparison)
    {
        
        SHR_IF_ERR_EXIT(dnx_rollback_journal_new(unit,
                                                 TRUE,
                                                 DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                                 DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL,
                                                 rollback_journal_cbs, &(DNX_DBAL_COMPARISON_JOURNAL_HANDLE)));

        dnx_dbal_journal_manager[unit].journals[DNX_DBAL_COMPARISON_JOURNAL_NUM].access_cb = access_cb;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_new(unit,
                                                 FALSE,
                                                 DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                                 DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL,
                                                 rollback_journal_cbs, &(DNX_DBAL_ROLLBACK_JOURNAL_HANDLE)));
        dnx_dbal_journal_manager[unit].journals[DNX_DBAL_ROLLBACK_JOURNAL_NUM].access_cb = access_cb;
    }

    
    if (is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_dbal_journal_exclude_table_clear(unit, is_comparison));
    }

    DNX_DBAL_FUNC_RETURN;
}


uint8
dnx_dbal_journal_is_on(
    int unit,
    dnx_err_recovery_journal_e journals)
{
    switch (journals)
    {
        case DNX_DBAL_JOURNAL_ALL:
            return (dnx_rollback_journal_is_on(unit, DNX_DBAL_ROLLBACK_JOURNAL_HANDLE)
                    || dnx_rollback_journal_is_on(unit, DNX_DBAL_COMPARISON_JOURNAL_HANDLE));
        case DNX_DBAL_JOURNAL_ROLLBACK:
            return dnx_rollback_journal_is_on(unit, DNX_DBAL_ROLLBACK_JOURNAL_HANDLE);
        case DNX_DBAL_JOURNAL_COMPARISON:
            return dnx_rollback_journal_is_on(unit, DNX_DBAL_COMPARISON_JOURNAL_HANDLE);
        default:
            return FALSE;
    }
}


static inline shr_error_e
dnx_dbal_journal_destroy_internal(
    int unit,
    uint8 is_comparison)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_destroy(unit, &DNX_DBAL_COMPARISON_JOURNAL_HANDLE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_destroy(unit, &DNX_DBAL_ROLLBACK_JOURNAL_HANDLE));
    }

    dnx_dbal_journal_manager[unit].unsupported_table_bypass_counter = 0;
exit:
    if (dnx_dbal_journal_manager[unit].unsupported_table_bypass_counter > 0)
    {
        dnx_dbal_journal_manager[unit].unsupported_table_bypass_counter = 0;
    }
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dbal_journal_destroy(
    int unit,
    uint8 is_comparison)
{
    DNX_DBAL_JOURNAL_DEFS;

    SHR_IF_ERR_EXIT(dnx_dbal_journal_destroy_internal(unit, is_comparison));

    DNX_DBAL_FUNC_RETURN;
}

static inline shr_error_e
dnx_dbal_journal_clear_internal(
    int unit,
    uint8 only_head_clear,
    uint8 is_comparison)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_clear(unit, only_head_clear, DNX_DBAL_COMPARISON_JOURNAL_HANDLE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_clear(unit, only_head_clear, DNX_DBAL_ROLLBACK_JOURNAL_HANDLE));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dbal_journal_clear(
    int unit,
    uint8 only_head_clear,
    uint8 is_comparison)
{
    DNX_DBAL_JOURNAL_DEFS;

    SHR_IF_ERR_EXIT(dnx_dbal_journal_clear_internal(unit, only_head_clear, is_comparison));

    DNX_DBAL_FUNC_RETURN;
}


shr_error_e
dnx_dbal_journal_roll_back(
    int unit,
    uint8 only_head_rollback,
    uint8 is_comparison)
{
    DNX_DBAL_JOURNAL_DEFS;

    if (is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_distinct
                        (unit, DNX_DBAL_COMPARISON_JOURNAL_HANDLE, &dnx_dbal_comparison_journal_entries_are_distinct));

        SHR_IF_ERR_EXIT_NO_MSG(dnx_rollback_journal_rollback
                               (unit, only_head_rollback, DNX_DBAL_COMPARISON_JOURNAL_HANDLE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_rollback(unit, only_head_rollback, DNX_DBAL_ROLLBACK_JOURNAL_HANDLE));
    }

    DNX_DBAL_FUNC_RETURN;
}


static uint8
dnx_dbal_journal_is_unsupported_table(
    int unit,
    uint32 table_id)
{
    dbal_status_e status;
    dbal_table_status_e table_status;

    
    if (_SHR_E_NONE != dbal_status_get(unit, &status))
    {
        return TRUE;
    }

    DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status);

    if ((table_status == DBAL_TABLE_HW_ERROR) && (status == DBAL_STATUS_DEVICE_INIT_DONE))
    {
        return TRUE;
    }

    return (    
               
               
               
               
               
               (DBAL_TABLE_HCC_HBM_DRAM_CPU_ACCESS == table_id)
               
               || (DBAL_TABLE_HBMC_INTERRUPTS == table_id)
               
               
               
        );
}


uint8
dnx_dbal_journal_should_create_test(
    int unit,
    uint32 table_id)
{
    multi_res_info_t result_set;
    int result_type_id;
    int result_id;
    dbal_table_field_info_t field_info;
    CONST dbal_logical_table_t *table = NULL;

    
    if (dbal_tables_table_get(unit, table_id, &table) != _SHR_E_NONE)
    {
        return FALSE;
    }

    for (result_type_id = 0; result_type_id < table->nof_result_types; result_type_id++)
    {
        result_set = table->multi_res_info[result_type_id];
        for (result_id = 0; result_id < result_set.nof_result_fields; result_id++)
        {
            field_info = result_set.results_info[result_id];

            
            if (DBAL_PERMISSION_TRIGGER == field_info.permission || DBAL_PERMISSION_READONLY == field_info.permission)
            {
                return FALSE;
            }
        }
    }

    
    return !dnx_dbal_journal_is_unsupported_table(unit, table_id);
}


static shr_error_e
dnx_dbal_journal_log_common(
    int unit,
    dbal_entry_handle_t * ptr_handle,
    dbal_actions_e action,
    uint32 er_flags,
    uint8 is_dbal_error)
{
    uint8 is_er = (0 != (er_flags & DNX_DBAL_JOURNAL_ROLLBACK_FLAG));
    uint8 is_cmp = (0 != (er_flags & DNX_DBAL_JOURNAL_COMPARE_FLAG));

    DNX_DBAL_JOURNAL_DEFS;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == ptr_handle)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                    "dbal journal ERROR: NULL data attempted to be inserted to journal.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }
#endif 

    if (dnx_dbal_journal_is_unsupported_table(unit, ptr_handle->table_id))
    {
        
        if (dnx_dbal_journal_manager[unit].unsupported_table_bypass_counter > 0)
        {
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "dbal journal ERROR: Unsupported table access: %s. Error recovery is not supported for this API !\n%s%s",
                                 ptr_handle->table->table_name, EMPTY, EMPTY);
    }

    
    if (ptr_handle->table->table_type == DBAL_TABLE_TYPE_TCAM
        && ptr_handle->table->access_method == DBAL_ACCESS_METHOD_MDB)
    {
        SHR_BITAND_RANGE(ptr_handle->phy_entry.key, ptr_handle->phy_entry.k_mask, 0, ptr_handle->phy_entry.key_size,
                         ptr_handle->phy_entry.key);
    }

    SHR_IF_ERR_EXIT(dnx_dbal_journal_entry_insert
                    (unit, ptr_handle, action, &dnx_dbal_journal_rollback_common, is_er, is_cmp, is_dbal_error));

    DNX_DBAL_FUNC_RETURN;
}

static inline shr_error_e
dnx_dbal_journal_get_head_seq_id_internal(
    int unit,
    uint8 is_comparison,
    uint32 *seq_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_get_head_seq_id(unit, seq_id, DNX_DBAL_COMPARISON_JOURNAL_HANDLE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_get_head_seq_id(unit, seq_id, DNX_DBAL_ROLLBACK_JOURNAL_HANDLE));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dbal_journal_get_head_seq_id(
    int unit,
    uint8 is_comparison,
    uint32 *seq_id)
{
    DNX_DBAL_JOURNAL_DEFS;

    SHR_IF_ERR_EXIT(dnx_dbal_journal_get_head_seq_id_internal(unit, is_comparison, seq_id));

    DNX_DBAL_FUNC_RETURN;
}


shr_error_e
dnx_dbal_journal_all_tmp_suppress_unsafe(
    int unit,
    uint8 is_disable)
{
    DNX_DBAL_JOURNAL_DEFS;

    SHR_IF_ERR_EXIT(dnx_rollback_journal_disabled_counter_change
                    (unit, is_disable, DNX_DBAL_ROLLBACK_JOURNAL_HANDLE, TRUE));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_disabled_counter_change
                    (unit, is_disable, DNX_DBAL_COMPARISON_JOURNAL_HANDLE, TRUE));

    DNX_DBAL_FUNC_RETURN;
}


shr_error_e
dnx_dbal_journal_comparison_tmp_suppress_unsafe(
    int unit,
    uint8 is_disable)
{
    DNX_DBAL_JOURNAL_DEFS;

    SHR_IF_ERR_EXIT(dnx_rollback_journal_disabled_counter_change
                    (unit, is_disable, DNX_DBAL_COMPARISON_JOURNAL_HANDLE, TRUE));

    DNX_DBAL_FUNC_RETURN;
}


uint8
dnx_dbal_journal_is_tmp_suppressed_unsafe(
    int unit,
    uint8 is_comparison)
{
    uint32 counter = 0;

    if (is_comparison)
    {
        dnx_rollback_journal_disabled_counter_get(unit, DNX_DBAL_COMPARISON_JOURNAL_HANDLE, &counter);
    }
    else
    {
        dnx_rollback_journal_disabled_counter_get(unit, DNX_DBAL_ROLLBACK_JOURNAL_HANDLE, &counter);
    }

    return (counter != 0);
}


static inline shr_error_e
dnx_dbal_journal_entry_access_id_update(
    int unit,
    dbal_entry_handle_t * handle,
    int access_id)
{
    DNX_DBAL_JOURNAL_DEFS;

    if ((DBAL_TABLE_IS_TCAM(handle->table)) && handle->table->access_method == DBAL_ACCESS_METHOD_KBP)
    {
#ifdef DNX_ERR_RECOVERY_VALIDATION
        if (handle->phy_entry.entry_hw_id == 0)
        {
            DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                        "dbal journal ERROR: attempted to journal KBP TCAM entry with hw id = 0.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
#endif 

        handle->phy_entry.entry_hw_id = access_id;
        handle->access_id_set = TRUE;
    }

    DNX_DBAL_FUNC_RETURN;
}

static inline shr_error_e
dnx_dbal_journal_log_add_internal(
    int unit,
    dbal_entry_handle_t * get_handle,
    dbal_entry_handle_t * entry_handle,
    uint8 is_dbal_error)
{
    int committed_entry_access_id = entry_handle->phy_entry.entry_hw_id;
    dbal_logical_table_t *table = entry_handle->table;

    SHR_FUNC_INIT_VARS(unit);

    if (get_handle == NULL)
    {
        
        if (!is_dbal_error)
        {
            SHR_IF_ERR_EXIT(dnx_dbal_journal_entry_access_id_update(unit, entry_handle, committed_entry_access_id));
        }

        SHR_IF_ERR_EXIT(dnx_dbal_journal_log_common
                        (unit, entry_handle, DBAL_ACTION_ENTRY_CLEAR, entry_handle->er_flags, is_dbal_error));
    }
    else
    {
        
        if (table->access_method == DBAL_ACCESS_METHOD_MDB && get_handle != entry_handle)
        {
            sal_memcpy(get_handle->phy_entry.p_mask, entry_handle->phy_entry.p_mask, DBAL_PHYSICAL_RES_SIZE_IN_BYTES);
        }
        get_handle->phy_entry.indirect_commit_mode = DBAL_INDIRECT_COMMIT_MODE_UPDATE;
        get_handle->entry_merged = 1;

        
        if (!is_dbal_error)
        {
            SHR_IF_ERR_EXIT(dnx_dbal_journal_entry_access_id_update(unit, get_handle, committed_entry_access_id));
        }

        SHR_IF_ERR_EXIT(dnx_dbal_journal_log_common
                        (unit, get_handle, DBAL_ACTION_ENTRY_COMMIT, entry_handle->er_flags, is_dbal_error));

        
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dbal_journal_log_add(
    int unit,
    dbal_entry_handle_t * get_handle,
    dbal_entry_handle_t * entry_handle,
    uint8 is_dbal_error)
{
    DNX_DBAL_JOURNAL_DEFS;

    DNX_DBAL_JOURNAL_ACCESS_NOTIFY(unit, entry_handle->table);

    if (0 != entry_handle->er_flags)
    {
        SHR_IF_ERR_EXIT(dnx_dbal_journal_log_add_internal(unit, get_handle, entry_handle, is_dbal_error));
    }

    DNX_DBAL_FUNC_RETURN;
}


static inline shr_error_e
dnx_dbal_journal_log_clear_internal(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 is_dbal_error)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_dbal_journal_log_common
                    (unit, entry_handle, DBAL_ACTION_ENTRY_COMMIT, entry_handle->er_flags, is_dbal_error));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dbal_journal_log_clear(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 is_dbal_error)
{
    DNX_DBAL_JOURNAL_DEFS;

    DNX_DBAL_JOURNAL_ACCESS_NOTIFY(unit, entry_handle->table);

    if (0 != entry_handle->er_flags)
    {
        SHR_IF_ERR_EXIT(dnx_dbal_journal_log_clear_internal(unit, entry_handle, is_dbal_error));
    }

    DNX_DBAL_FUNC_RETURN;
}


shr_error_e
dnx_dbal_journal_unsupported_tables_bypass(
    int unit,
    uint8 is_bypassed)
{
    DNX_DBAL_JOURNAL_DEFS;

    if (is_bypassed)
    {
        dnx_dbal_journal_manager[unit].unsupported_table_bypass_counter++;
    }
    else
    {
        if (0 == dnx_dbal_journal_manager[unit].unsupported_table_bypass_counter)
        {
            DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                     "dbal journal ERROR: attempted to end unsupported tables bypass region without start.\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
        }

        dnx_dbal_journal_manager[unit].unsupported_table_bypass_counter--;
    }

    DNX_DBAL_FUNC_RETURN;
}


uint8
dnx_dbal_journal_are_unsupported_tables_bypassed(
    int unit)
{
    return (dnx_dbal_journal_manager[unit].unsupported_table_bypass_counter > 0) ? TRUE : FALSE;
}


shr_error_e
dnx_dbal_journal_exclude_table_by_stamp(
    int unit,
    char *stamp,
    uint8 is_comparison)
{
    dbal_tables_e table_id;
    DNX_DBAL_JOURNAL_DEFS;

    if (!is_comparison)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "dbal journal ERROR: rollback journal table exclusions are not supported.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    if (!dnx_rollback_journal_is_on(unit, DNX_DBAL_COMPARISON_JOURNAL_HANDLE))
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "dbal journal ERROR: comparison journal table exclusions are done only if the dbal comparsion journal is on.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    if (DNX_DBAL_COMPARISON_JOURNAL_TRANSACTION.excluded_tables_count + 1 >= DNX_DBAL_JOURNAL_EXCLUDED_TABLES_MAX_NOF)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "dbal journal ERROR: comparison journal table exclusions exceed the maximum number of allowed.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    
    SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, stamp, &table_id));

    DNX_DBAL_COMPARISON_JOURNAL_TRANSACTION.
        excluded_tables[DNX_DBAL_COMPARISON_JOURNAL_TRANSACTION.excluded_tables_count] = table_id;
    DNX_DBAL_COMPARISON_JOURNAL_TRANSACTION.excluded_tables_count++;

    DNX_DBAL_FUNC_RETURN;
}


shr_error_e
dnx_dbal_journal_exclude_table_clear(
    int unit,
    uint8 is_comparison)
{
    DNX_DBAL_JOURNAL_DEFS;

    if (!is_comparison)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "dbal journal ERROR: rollback journal table exclusions clear are not supported.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    DNX_DBAL_COMPARISON_JOURNAL_TRANSACTION.excluded_tables_count = 0;

    DNX_DBAL_FUNC_RETURN;
}


shr_error_e
dnx_dbal_journal_print(
    int unit,
    uint8 is_comparison)
{
    DNX_DBAL_JOURNAL_DEFS;

    if (!is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_print(unit, DNX_DBAL_ROLLBACK_JOURNAL_HANDLE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_print(unit, DNX_DBAL_COMPARISON_JOURNAL_HANDLE));
    }

    DNX_DBAL_FUNC_RETURN;
}


shr_error_e
dnx_dbal_journal_logger_state_change(
    int unit,
    uint8 is_comparison)
{
    DNX_DBAL_JOURNAL_DEFS;

    if (!is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_logger_state_change_by_type(unit, DNX_DBAL_ROLLBACK_JOURNAL_HANDLE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_logger_state_change_by_type(unit, DNX_DBAL_COMPARISON_JOURNAL_HANDLE));
    }

    DNX_DBAL_FUNC_RETURN;
}

shr_error_e
dnx_dbal_journal_logger_state_get(
    int unit,
    uint8 *is_logging)
{
    DNX_DBAL_JOURNAL_DEFS;

    dnx_rollback_journal_is_logging(unit, DNX_DBAL_ROLLBACK_JOURNAL_HANDLE, is_logging);

    DNX_DBAL_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
