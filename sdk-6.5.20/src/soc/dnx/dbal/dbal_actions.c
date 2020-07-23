/** \file dbal_actions.c
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * System wide Logical Table Manager internal functions.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

#include "dbal_internal.h"
#include <soc/dnx/mdb.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_dbal_api.h>
#include <shared/utilex/utilex_bitstream.h>
#include <sal/appl/sal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dbal/dbal_journal.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>


#define DBAL_ACTIONS_SET_ERR(_expr)              \
do                                               \
{                                                \
  int post_rv = (_expr);                         \
  if (SHR_FAILURE(post_rv)                       \
          && !SHR_FAILURE(SHR_GET_CURRENT_ERR()))\
  {                                              \
    SHR_SET_CURRENT_ERR(post_rv);                \
  }                                              \
} while (0)



#define COMMON_LEGAL_FLAGS  (DBAL_COMMIT_DISABLE_ACTION_PRINTS | DBAL_COMMIT| DBAL_COMMIT_IGNORE_ALLOC_ERROR )
#define PRE_ACCESS_PRINTS_ENABLED   1
#define POST_ACCESS_PRINTS_ENABLED  0


CONST dbal_action_info_t dbal_actions_info[DBAL_NOF_ACTIONS] =
{

    
    {"COMMIT",         PRE_ACCESS_PRINTS_ENABLED, ALL_KEY_FIELDS_MUST_EXIST | RESULT_FIELD_MUST_EXIST | VALIDATE_ALLOCATOR_FIELD, COMMON_LEGAL_FLAGS | DBAL_COMMIT_OVERRIDE_DEFAULT | DBAL_COMMIT_OVERRUN},

    
    {"COMMIT UPDATE",  PRE_ACCESS_PRINTS_ENABLED, ALL_KEY_FIELDS_MUST_EXIST | RESULT_FIELD_MUST_EXIST | VALIDATE_ALLOCATOR_FIELD, COMMON_LEGAL_FLAGS | DBAL_COMMIT_UPDATE | DBAL_COMMIT_OVERRIDE_DEFAULT},

    
    {"COMMIT FORCE",   PRE_ACCESS_PRINTS_ENABLED, ALL_KEY_FIELDS_MUST_EXIST | RESULT_FIELD_MUST_EXIST | VALIDATE_ALLOCATOR_FIELD, COMMON_LEGAL_FLAGS | DBAL_COMMIT_FORCE | DBAL_COMMIT_OVERRIDE_DEFAULT},

    
    {"COMMIT RANGE",   PRE_ACCESS_PRINTS_ENABLED, ALL_KEY_FIELDS_MUST_EXIST | RESULT_FIELD_MUST_EXIST | VALIDATE_ALLOCATOR_FIELD, COMMON_LEGAL_FLAGS | DBAL_COMMIT_OVERRUN},

    
    {"GET",            POST_ACCESS_PRINTS_ENABLED, ALL_KEY_FIELDS_MUST_EXIST | RESULT_FIELD_MUST_EXIST | VALIDATE_ALLOCATOR_FIELD | CORE_ID_MUST_BE_SET_TO_ONE_CORE_ONLY, COMMON_LEGAL_FLAGS | DBAL_COMMIT_VALIDATE_OTHER_CORE},

    
    {"GET ALL FIELDS", POST_ACCESS_PRINTS_ENABLED, ALL_KEY_FIELDS_MUST_EXIST| VALIDATE_ALLOCATOR_FIELD | CORE_ID_MUST_BE_SET_TO_ONE_CORE_ONLY, COMMON_LEGAL_FLAGS | DBAL_GET_ALL_FIELDS},

    
    {"GET ACCESS ID BY KEY", POST_ACCESS_PRINTS_ENABLED, ALL_KEY_FIELDS_MUST_EXIST | VALIDATE_ALLOCATOR_FIELD | CORE_ID_MUST_BE_SET_TO_ONE_CORE_ONLY, COMMON_LEGAL_FLAGS},

    
    {"ITERATOR GET", POST_ACCESS_PRINTS_ENABLED, ALL_KEY_FIELDS_MUST_EXIST| VALIDATE_ALLOCATOR_FIELD, COMMON_LEGAL_FLAGS},

    
    {"ENTRY CLEAR",    PRE_ACCESS_PRINTS_ENABLED, ALL_KEY_FIELDS_MUST_EXIST, COMMON_LEGAL_FLAGS},

    
    {"ENTRY CLEAR RANGE", PRE_ACCESS_PRINTS_ENABLED, ALL_KEY_FIELDS_MUST_EXIST, COMMON_LEGAL_FLAGS},

    
    {"TABLE CLEAR",    PRE_ACCESS_PRINTS_ENABLED, CORE_COULD_BE_NOT_INITIALIZED, COMMON_LEGAL_FLAGS}

};


static shr_error_e dbal_actions_access_commit_consecutive_range(int unit, dbal_entry_handle_t * entry_handle, int num_of_entries);

static shr_error_e dbal_actions_access_clear_consecutive_range(int unit, dbal_entry_handle_t * entry_handle, int num_of_entries);





shr_error_e
dbal_merge_entries(
    int unit,
    dbal_entry_handle_t * prim_entry,
    dbal_entry_handle_t * secondary_entry)
{
    int payload_num_of_uint32;
    uint32 converted_mask[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };
    uint32 converted_payload[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };
    dbal_logical_table_t *table = prim_entry->table;

    SHR_FUNC_INIT_VARS(unit);

    
    if (prim_entry->nof_result_fields != table->multi_res_info[prim_entry->cur_res_type].nof_result_fields)
    {
        if (prim_entry->cur_res_type != secondary_entry->cur_res_type)
        {
            
            if (utilex_bitstream_have_one_in_range(secondary_entry->phy_entry.payload, 0,
                                                   table->multi_res_info[secondary_entry->
                                                                         cur_res_type].entry_payload_size))
            {
                SHR_ERR_EXIT(_SHR_E_EXISTS,
                             "Cannot merge entries of different result types. Primary = %d, Secondary = %d\n",
                             prim_entry->cur_res_type, secondary_entry->cur_res_type);
            }
        }
        prim_entry->entry_merged = 1;

        payload_num_of_uint32 = (table->max_payload_size + 31) / 32;

        
        SHR_IF_ERR_EXIT(utilex_bitstream_and
                        (prim_entry->phy_entry.payload, prim_entry->phy_entry.p_mask, payload_num_of_uint32));

        
        sal_memcpy(converted_mask, prim_entry->phy_entry.p_mask, WORDS2BYTES(payload_num_of_uint32));
        sal_memcpy(converted_payload, secondary_entry->phy_entry.payload, WORDS2BYTES(payload_num_of_uint32));
        SHR_IF_ERR_EXIT(utilex_bitstream_not(converted_mask, payload_num_of_uint32));
        SHR_IF_ERR_EXIT(utilex_bitstream_and(converted_payload, converted_mask, payload_num_of_uint32));

        
        SHR_IF_ERR_EXIT(utilex_bitstream_or(prim_entry->phy_entry.payload, converted_payload, payload_num_of_uint32));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_get_handle_update_value_field(
    int unit,
    dbal_entry_handle_t * handle)
{
    int iter = 0;
    int cur_res_type = handle->cur_res_type;
    dbal_logical_table_t *table = handle->table;

    SHR_FUNC_INIT_VARS(unit);

    if (cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
    {
        SHR_EXIT();
    }

    for (iter = 0; iter < table->multi_res_info[cur_res_type].nof_result_fields; iter++)
    {
        if (table->access_method != DBAL_ACCESS_METHOD_PEMLA
            || table->pemla_mapping.result_fields_mapping[iter] != DBAL_PEMLA_FIELD_MAPPING_INVALID)
        {
            handle->value_field_ids[iter] = table->multi_res_info[cur_res_type].results_info[iter].field_id;
        }
    }

    
    for (; iter < DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE; iter++)
    {
        if (handle->value_field_ids[iter] == DBAL_FIELD_EMPTY)
        {
            break;
        }

        handle->value_field_ids[iter] = DBAL_FIELD_EMPTY;
    }

    
    handle->nof_result_fields = table->multi_res_info[cur_res_type].nof_result_fields;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_entry_non_consecutive_range_action_perform(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int num_of_entries,
    uint32 *key_field_min_values,
    dbal_actions_e action)
{
    int ii, jj;
    uint32 key_field_counter[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS] = { 0 };
    uint32 key_field_orig_range_values[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS] = { 0 };
    uint8 is_field_has_ilegal_value[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS] = { 0 };
    int orig_nof_ranged_fields = 0;

    SHR_FUNC_INIT_VARS(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n***** (non-consecutive) min key value %d nof entries %d ****\n"),
                                 entry_handle->phy_entry.key[0], num_of_entries));

    orig_nof_ranged_fields = entry_handle->nof_ranged_fields;
    entry_handle->nof_ranged_fields = 0;

    for (ii = 0; ii < entry_handle->nof_key_fields; ii++)
    {
        key_field_orig_range_values[ii] = entry_handle->key_field_ranges[ii];
        key_field_counter[ii] = key_field_min_values[ii];
        SHR_IF_ERR_EXIT(dbal_fields_has_illegal_values
                        (unit, entry_handle->key_field_ids[ii], &is_field_has_ilegal_value[ii]));
    }

    
    if (action == DBAL_ACTION_ENTRY_COMMIT_RANGE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit_normal(unit, entry_handle));
    }
    else if (action == DBAL_ACTION_ENTRY_CLEAR_RANGE)
    {
        SHR_IF_ERR_EXIT(dbal_actions_access_entry_clear(unit, entry_handle));
    }

    for (ii = 1; ii < num_of_entries; ii++)
    {
        int no_need_commit = 0;
        for (jj = 0; jj < entry_handle->nof_key_fields; jj++)
        {
            if (key_field_orig_range_values[jj] != 0)
            {
                uint32 max_key_field_value = (key_field_min_values[jj] - 1) + key_field_orig_range_values[jj];
                if (key_field_counter[jj] <= max_key_field_value)
                {
                    if (key_field_counter[jj] == max_key_field_value)
                    {
                        key_field_counter[jj] = key_field_min_values[jj];
                    }
                    else
                    {
                        uint8 is_illegal_value_for_field = TRUE;
                        key_field_counter[jj]++;
                        if (is_field_has_ilegal_value[jj])
                        {
                            SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, entry_handle->key_field_ids[jj],
                                                                         key_field_counter[jj],
                                                                         &is_illegal_value_for_field));
                            if (is_illegal_value_for_field)
                            {
                                no_need_commit = 1;
                            }
                        }
                        break;
                    }
                }
            }
        }

        if (!no_need_commit)
        {
            for (jj = 0; jj < entry_handle->nof_key_fields; jj++)
            {
                if (key_field_orig_range_values[jj] > 0)
                {
                    
                    dbal_entry_key_field32_set(unit, entry_handle->handle_id, entry_handle->key_field_ids[jj],
                                               key_field_counter[jj]);
                }
            }

            if (action == DBAL_ACTION_ENTRY_COMMIT_RANGE)
            {
                SHR_IF_ERR_EXIT(dbal_entry_commit_normal(unit, entry_handle));
            }
            else if (action == DBAL_ACTION_ENTRY_CLEAR_RANGE)
            {
                SHR_IF_ERR_EXIT(dbal_actions_access_entry_clear(unit, entry_handle));
            }

            if (entry_handle->error_info.error_exists)
            {
                break;
            }
        }
    }

    
    for (ii = 0; ii < entry_handle->nof_key_fields; ii++)
    {
        if (key_field_orig_range_values[ii] > 0)
        {
            
            dbal_entry_key_field32_set(unit, entry_handle->handle_id, entry_handle->key_field_ids[ii],
                                       key_field_min_values[ii]);
        }
        entry_handle->key_field_ranges[ii] = key_field_orig_range_values[ii];
    }
    entry_handle->nof_ranged_fields = orig_nof_ranged_fields;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_direct_is_default(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *is_default)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (DBAL_TABLE_IS_NONE_DIRECT(entry_handle->table))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Table is indirect: %s.\n",
                     dbal_logical_table_to_string(unit, entry_handle->table_id));
    }

    (*is_default) = 0;
    if (entry_handle->table->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC)
    {
        SHR_IF_ERR_EXIT(dbal_hl_is_entry_default(unit, entry_handle, is_default));
    }
    else
    {
        if (sal_memcmp
            (zero_buffer_to_compare, entry_handle->phy_entry.payload,
             DBAL_TABLE_BUFFER_IN_BYTES(entry_handle->table)) == 0)
        {
            (*is_default) = 1;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_commit_direct(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int rv = _SHR_E_NONE;
    dbal_entry_handle_t *get_entry_handle = NULL;
    dbal_entry_handle_t *get_entry_copy = NULL; 
    dbal_logical_table_t *table = entry_handle->table;

    SHR_FUNC_INIT_VARS(unit);

    if ((DBAL_HANDLE_IS_MERGE_NEEDED(entry_handle) &&
         dbal_tables_is_merge_entries_supported(unit, entry_handle->table_id)) || entry_handle->er_flags)
    {
        int is_exists = FALSE;
        if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_ITERATOR_OPTIMIZED) &&
            !SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_DEFAULT_NON_STANDARD))
        {
            SHR_IF_ERR_EXIT(dbal_actions_optimized_is_exists(unit, entry_handle, &is_exists));
            if (!is_exists)
            {
                rv = _SHR_E_NOT_FOUND;
            }
        }

        if (is_exists || (rv == _SHR_E_NONE))
        {
            
            SHR_ALLOC_SET_ZERO(get_entry_handle, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n",
                               EMPTY, EMPTY, EMPTY);

            rv = dbal_actions_access_entry_get(unit, entry_handle, get_entry_handle, 0);
        }
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        
        if ((rv != _SHR_E_NOT_FOUND) && entry_handle->er_flags)
        {
            int is_default = 0;

            
            SHR_IF_ERR_EXIT(dbal_entry_direct_is_default(unit, get_entry_handle, &is_default));
            if (!is_default)
            {
                get_entry_copy = get_entry_handle;
            }
        }

        
        if (DBAL_HANDLE_IS_MERGE_NEEDED(entry_handle))
        {
            if (dbal_tables_is_merge_entries_supported(unit, entry_handle->table_id))
            {
                if (rv == _SHR_E_NONE)
                {
                    
                    SHR_IF_ERR_EXIT(dbal_merge_entries(unit, entry_handle, get_entry_handle));
                }
            }
        }
    }

    
    SHR_IF_ERR_EXIT(dbal_actions_access_entry_commit(unit, entry_handle, get_entry_copy));

exit:
    SHR_FREE(get_entry_handle);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_commit_normal(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int rv = _SHR_E_NONE;
    dbal_entry_handle_t *get_entry_handle = NULL;
    int is_sw_get = 1;
    SHR_FUNC_INIT_VARS(unit);

    entry_handle->phy_entry.indirect_commit_mode = DBAL_INDIRECT_COMMIT_MODE_NORMAL;

    if (DBAL_TABLE_IS_NONE_DIRECT(entry_handle->table))
    {
        
        if (!SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_NO_VALIDATIONS)
            && (!SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_NONE_DIRECT_OPTIMIZED)
                || entry_handle->er_flags))
        {
            SHR_ALLOC_SET_ZERO(get_entry_handle, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n",
                               EMPTY, EMPTY, EMPTY);
            if (entry_handle->table_id == DBAL_TABLE_GLOBAL_RIF_EM)
            {
                
                int is_exists = FALSE;
                SHR_IF_ERR_EXIT(dbal_actions_optimized_is_exists(unit, entry_handle, &is_exists));
                if (!is_exists)
                {
                    rv = _SHR_E_NOT_FOUND;
                }
            }
            else
            {
                
                if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_LEARNING_EN))
                {
                    is_sw_get = 0;
                }
                
                rv = dbal_actions_access_entry_get(unit, entry_handle, get_entry_handle, is_sw_get);
                
                get_entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_ALL;
                SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
            }

            
            if (rv == _SHR_E_NONE)
            {
                
                SHR_ERR_EXIT(_SHR_E_EXISTS, "Cannot commit entry - already exists in table %s\n",
                             entry_handle->table->table_name);
            }

            if (entry_handle->entry_merged)
            {
                
                SHR_IF_ERR_EXIT(dbal_merge_entries(unit, entry_handle, get_entry_handle));
            }
        }
        SHR_IF_ERR_EXIT(dbal_actions_access_entry_commit(unit, entry_handle, NULL));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit_direct(unit, entry_handle));
    }

exit:
    SHR_FREE(get_entry_handle);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_commit_update(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int rv;
    dbal_entry_handle_t *get_entry_handle = NULL;
    int is_sw_get = 1;

    SHR_FUNC_INIT_VARS(unit);

    entry_handle->phy_entry.indirect_commit_mode = DBAL_INDIRECT_COMMIT_MODE_UPDATE;

    
    if (!(SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_NONE_DIRECT_OPTIMIZED) &&
          !(DBAL_HANDLE_IS_MERGE_NEEDED(entry_handle))) || entry_handle->er_flags)
    {
        SHR_ALLOC_SET_ZERO(get_entry_handle, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n",
                           EMPTY, EMPTY, EMPTY);

        
        if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_LEARNING_EN))
        {
            is_sw_get = 0;
        }
        rv = dbal_actions_access_entry_get(unit, entry_handle, get_entry_handle, is_sw_get);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NOT_FOUND)
        {
            
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot update entry - entry does not exist in table %s\n",
                         entry_handle->table->table_name);
        }

        
        get_entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_ALL;

        
        SHR_IF_ERR_EXIT(dbal_merge_entries(unit, entry_handle, get_entry_handle));
    }

    SHR_IF_ERR_EXIT(dbal_actions_access_entry_commit(unit, entry_handle, get_entry_handle));

exit:
    SHR_FREE(get_entry_handle);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_commit_force(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_entry_handle_t *get_entry_handle = NULL;
    int rv;
    int is_sw_get = 0;

    SHR_FUNC_INIT_VARS(unit);

    
    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_NONE_DIRECT_OPTIMIZED) &&
        !(DBAL_HANDLE_IS_MERGE_NEEDED(entry_handle)) && !(entry_handle->er_flags))
    {
        entry_handle->phy_entry.indirect_commit_mode = DBAL_INDIRECT_COMMIT_MODE_FORCE;
    }
    else
    {
        SHR_ALLOC_SET_ZERO(get_entry_handle, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n", EMPTY, EMPTY,
                           EMPTY);

        if ((entry_handle->table->access_method == DBAL_ACCESS_METHOD_MDB)
            && dbal_logical_table_is_mact(unit, entry_handle->table_id))
        {
            if ((dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_mact_use_refresh_on_insert))
                && (!entry_handle->er_flags))
            {
                is_sw_get = 1;
            }
        }
        rv = dbal_actions_access_entry_get(unit, entry_handle, get_entry_handle, is_sw_get);

        if (rv == _SHR_E_NONE)
        {
            
            entry_handle->phy_entry.indirect_commit_mode = DBAL_INDIRECT_COMMIT_MODE_UPDATE;

            
            SHR_IF_ERR_EXIT(dbal_merge_entries(unit, entry_handle, get_entry_handle));

        }
        else if (rv == _SHR_E_NOT_FOUND)
        {
            if (entry_handle->entry_merged)
            {
                
                SHR_IF_ERR_EXIT(dbal_merge_entries(unit, entry_handle, get_entry_handle));
            }

            
            entry_handle->phy_entry.indirect_commit_mode = DBAL_INDIRECT_COMMIT_MODE_NORMAL;
            SHR_FREE(get_entry_handle);
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
    }

    SHR_IF_ERR_EXIT(dbal_actions_access_entry_commit(unit, entry_handle, get_entry_handle));

exit:
    SHR_FREE(get_entry_handle);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_range_action_process(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_actions_e action)
{
    dbal_logical_table_t *table;
    int num_of_entries = 1, ii, jj;
    uint32 max_key_handle_id;
    dbal_entry_handle_t *max_key_handle;
    uint32 key_field_min_values[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    table = entry_handle->table;

    
    for (ii = 0; ii < entry_handle->nof_key_fields; ii++)
    {
        if (entry_handle->key_field_ranges[ii] != 0)
        {
            num_of_entries = num_of_entries * entry_handle->key_field_ranges[ii];
            SHR_IF_ERR_EXIT(dbal_field_from_buffer_get(unit, &table->keys_info[ii], entry_handle->key_field_ids[ii],
                                                       entry_handle->phy_entry.key, &key_field_min_values[ii]));

        }
    }

    if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RANGE_SET_SUPPORTED) && entry_handle->er_flags)
    {
        
        uint8 is_illegal_value_exists = FALSE;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, entry_handle->table_id, &max_key_handle_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, max_key_handle_id, &max_key_handle));

        max_key_handle->phy_entry.key[0] = entry_handle->phy_entry.key[0];
        for (jj = 0; jj < entry_handle->nof_key_fields; jj++)
        {
            uint8 curr_field_illegal_value = FALSE;
            SHR_IF_ERR_EXIT(dbal_fields_has_illegal_values
                            (unit, entry_handle->key_field_ids[jj], &curr_field_illegal_value));
            is_illegal_value_exists |= curr_field_illegal_value;
            if (entry_handle->key_field_ranges[jj] > 0)
            {
                uint32 max_key_field_value = (key_field_min_values[ii] - 1) + entry_handle->key_field_ranges[jj];
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\nSetting max value %d for field %s \n"),
                                             key_field_min_values[ii] + entry_handle->key_field_ranges[jj],
                                             dbal_field_to_string(unit, entry_handle->key_field_ids[jj])));
                dbal_entry_key_field32_set(unit, max_key_handle_id, entry_handle->key_field_ids[jj],
                                           max_key_field_value);
                if (max_key_handle->error_info.error_exists)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, " field %s out of range min val = %d range = %d \n",
                                 dbal_field_to_string(unit, max_key_handle->error_info.field_id),
                                 key_field_min_values[ii], entry_handle->key_field_ranges[jj]);
                }
            }
        }

        
        if ((((entry_handle->phy_entry.key[0] - 1) + num_of_entries) == max_key_handle->phy_entry.key[0])
            && (!is_illegal_value_exists))
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U
                         (unit, "\n***** (consecutive) min entry offset %d max entry offset %d nof entries %d ****\n"),
                         entry_handle->phy_entry.key[0], max_key_handle->phy_entry.key[0], num_of_entries));

            if (action == DBAL_ACTION_ENTRY_COMMIT_RANGE)
            {
                SHR_IF_ERR_EXIT(dbal_actions_access_commit_consecutive_range(unit, entry_handle, num_of_entries));
                SHR_IF_ERR_EXIT(dbal_tables_entry_counter_update(unit, entry_handle, num_of_entries, TRUE));
                SHR_EXIT();
            }
            else if (action == DBAL_ACTION_ENTRY_CLEAR_RANGE)
            {
                SHR_IF_ERR_EXIT(dbal_actions_access_clear_consecutive_range(unit, entry_handle, num_of_entries));
                SHR_IF_ERR_EXIT(dbal_tables_entry_counter_update(unit, entry_handle, num_of_entries, FALSE));
                SHR_EXIT();
            }
        }
    }

    
    SHR_IF_ERR_EXIT(dbal_entry_non_consecutive_range_action_perform(unit, entry_handle, num_of_entries,
                                                                    key_field_min_values, action));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_action_finish(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_action_flags_e flags,
    dbal_actions_e action)
{
    CONST dbal_action_info_t *actions_info = &dbal_actions_info[(action)];

    SHR_FUNC_INIT_VARS(unit);

    if (actions_info->is_pre_acces_prints == POST_ACCESS_PRINTS_ENABLED)
    {
        SHR_IF_ERR_EXIT(dbal_action_prints(unit, entry_handle, flags, action));
    }

    SHR_IF_ERR_EXIT(dbal_logger_table_internal_unlock(unit, entry_handle->table_id));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_action_validate(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_actions_e action,
    dbal_entry_action_flags_e flags)
{
    dbal_logical_table_t *table = entry_handle->table;
    CONST dbal_action_info_t *actions_info = &dbal_actions_info[(action)];

    SHR_FUNC_INIT_VARS(unit);
    

    if (table->access_method == DBAL_ACCESS_METHOD_PEMLA)
    {
        dbal_status_e status;

        SHR_IF_ERR_EXIT(dbal_pemla_status_get(unit, &status));

        if (status != DBAL_STATUS_DBAL_INIT_DONE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "PEMLA Init not done cannot access table %s \n", table->table_name);
        }
    }


    if (entry_handle->error_info.error_exists)
    {
        int res_type = 0;

        if (entry_handle->cur_res_type > 0)
        {
            res_type = entry_handle->cur_res_type;
        }

        if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
        {
            SHR_ERR_EXIT(entry_handle->error_info.error_exists,
                         "Cannot perform action. there is an issue with field %s table %s res type %s\n",
                         dbal_field_to_string(unit, entry_handle->error_info.field_id), table->table_name,
                         table->multi_res_info[res_type].result_type_name);
        }
        else
        {
            SHR_ERR_EXIT(entry_handle->error_info.error_exists,
                         "Cannot perform action. there is an issue with field %s table %s\n",
                         dbal_field_to_string(unit, entry_handle->error_info.field_id), table->table_name);
        }
    }

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE)
        && entry_handle->cur_res_type == entry_handle->table->nof_result_types)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "when setting superset result type value cannot perform action. handle_id=%d table %s\n",
                     entry_handle->handle_id, table->table_name);
    }

    if (!(actions_info->action_validation_flags & CORE_COULD_BE_NOT_INITIALIZED))
    {
        if ((table->core_mode == DBAL_CORE_MODE_SBC) && (entry_handle->core_id != DBAL_CORE_ALL))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "table %s of type SBC, but core in handle is not CORE_ALL. core=%d\n",
                         table->table_name, entry_handle->core_id);
        }

        if ((table->core_mode == DBAL_CORE_MODE_DPC) && (entry_handle->core_id == DBAL_CORE_NOT_INTIATED))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "table %s of type DPC, but core in handle is not initialized\n",
                         table->table_name);
        }
    }

    
    if (table->access_method != DBAL_ACCESS_METHOD_HARD_LOGIC &&
        (actions_info->action_validation_flags & CORE_ID_MUST_BE_SET_TO_ONE_CORE_ONLY))
    {
        if ((table->core_mode == DBAL_CORE_MODE_DPC) &&
            ((entry_handle->core_id < 0) || (entry_handle->core_id > DBAL_MAX_NUM_OF_CORES - 1)))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "table %s is DPC, but core in handle is not initialized\n",
                         table->table_name);
        }
    }

    if (actions_info->action_validation_flags & ALL_KEY_FIELDS_MUST_EXIST)
    {
        
        if (!DBAL_TABLE_IS_TCAM(table) && (table->table_type != DBAL_TABLE_TYPE_LPM))
        {
            if (entry_handle->nof_key_fields != table->nof_key_fields)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "ilegal number of key fields for action %s in table %s, requested %d set %d\n",
                             actions_info->name, table->table_name, table->nof_key_fields,
                             entry_handle->nof_key_fields);
            }
        }
    }


    
    if ((action != DBAL_ACTION_TABLE_CLEAR) && (action != DBAL_ACTION_ENTRY_GET_ACCESS_ID))
    {
        if (DBAL_TABLE_IS_TCAM(table) && !DBAL_ACTIONS_IS_TCAM_BY_KEY(table))
        {
            if (entry_handle->access_id_set == 0)
            {
                if ((table->access_method != DBAL_ACCESS_METHOD_KBP))
                {
                    
                    SHR_ERR_EXIT(_SHR_E_PARAM, "tcam Tables must set HW_ACCESS_ID table %s\n", table->table_name);
                }
            }
        }
    }

    if (actions_info->action_validation_flags & RESULT_FIELD_MUST_EXIST)
    {
        if (entry_handle->nof_result_fields == 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "No value fields where set, cannot perform the action %s on table %s\n",
                         actions_info->name, table->table_name);
        }
    }

    
    if ((action == DBAL_ACTION_ENTRY_GET) && (flags & DBAL_COMMIT_VALIDATE_OTHER_CORE))
    {
        if (table->core_mode == DBAL_CORE_MODE_DPC)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "cannot use COMMIT_VALIDATE_OTHER_CORE in DPC table %s\n", table->table_name);
        }
        entry_handle->core_id = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_action_prints(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_action_flags_e flags,
    dbal_actions_e action)
{
    int log_enabled;
    CONST dbal_logger_info_t *logger_info;

    SHR_FUNC_INIT_VARS(unit);

    if (!(flags & DBAL_COMMIT_DISABLE_ACTION_PRINTS))
    {
        if (dbal_logger_is_enable(unit, entry_handle->table_id))
        {
            dbal_logger_info_get_internal(unit, &logger_info);
            if ((logger_info->logger_mode == DBAL_LOGGER_MODE_REGULAR) ||
                (logger_info->logger_mode == DBAL_LOGGER_MODE_WRITE_ONLY && action <= DBAL_ACTION_ENTRY_COMMIT_RANGE) ||
                (logger_info->logger_mode == DBAL_LOGGER_MODE_WRITE_ONLY && action >= DBAL_ACTION_ENTRY_CLEAR))
            {
                log_enabled = bsl_check(bslLayerSocdnx, bslSourceDbaldnx, bslSeverityInfo, unit);
                if (log_enabled || (logger_info->dbal_file != NULL))
                {
                    char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE] = { 0 };
                    DBAL_PRINT_FRAME_FOR_API_PRINTS(TRUE, bslSeverityInfo);
                    
                    DBAL_DUMP("\n", 0, logger_info->dbal_file);

                    if (DBAL_TABLE_IS_TCAM(entry_handle->table) && !(DBAL_ACTIONS_IS_TCAM_BY_KEY(entry_handle->table)))
                    {
                        sal_snprintf(buffer, 2 * DBAL_MAX_STRING_LENGTH,
                                     "Action %s: table %s, acces_id %d (handle ID %d)\n",
                                     dbal_actions_info[action].name, entry_handle->table->table_name,
                                     entry_handle->phy_entry.entry_hw_id, entry_handle->handle_id);
                    }
                    else
                    {
                        sal_snprintf(buffer, 2 * DBAL_MAX_STRING_LENGTH, "Action %s: table %s, (handle ID %d)\n",
                                     dbal_actions_info[action].name,
                                     entry_handle->table->table_name, entry_handle->handle_id);
                    }

                    DBAL_DUMP(buffer, log_enabled, logger_info->dbal_file);

                    if (action != DBAL_ACTION_TABLE_CLEAR)
                    {
                        SHR_IF_ERR_EXIT(dbal_entry_print(unit, entry_handle, 0));
                    }

                    DBAL_DUMP("\n", 0, logger_info->dbal_file);
                    DBAL_PRINT_FRAME_FOR_API_PRINTS(FALSE, bslSeverityInfo);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_action_entry_commit_parameters_update(
    int unit,
    dbal_entry_handle_t ** entry_handle,
    dbal_entry_action_flags_e flags,
    dbal_actions_e * action)
{
    dbal_logical_table_t *table;
    dbal_entry_action_flags_e unique_action_flag_set = 0;

    SHR_FUNC_INIT_VARS(unit);

    table = (*entry_handle)->table;

    if ((*entry_handle)->nof_ranged_fields > 0)
    {
        if (!((table->access_method == DBAL_ACCESS_METHOD_KBP) && (table->table_type == DBAL_TABLE_TYPE_TCAM_BY_ID)))
        {
            (*action) = DBAL_ACTION_ENTRY_COMMIT_RANGE;
        }
    }

    
    if (flags & DBAL_COMMIT)
    {
        unique_action_flag_set |= DBAL_COMMIT;
    }
    if (flags & DBAL_COMMIT_UPDATE)
    {
        if (DBAL_TABLE_IS_NONE_DIRECT((*entry_handle)->table) == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "COMMIT_UPDATE flag is not valid for direct tables. Table %s\n",
                         table->table_name);
        }
        (*action) = DBAL_ACTION_ENTRY_COMMIT_UPDATE;
        unique_action_flag_set |= DBAL_COMMIT_UPDATE;
    }
    if (flags & DBAL_COMMIT_FORCE)
    {
        if (DBAL_TABLE_IS_NONE_DIRECT((*entry_handle)->table) == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "COMMIT_FORCE flag is not valid for direct tables. Table %s\n",
                         table->table_name);
        }
        (*action) = DBAL_ACTION_ENTRY_COMMIT_FORCE;
        unique_action_flag_set |= DBAL_COMMIT_FORCE;
    }
    
    if (flags & DBAL_COMMIT_OVERRIDE_DEFAULT)
    {
        if (!DBAL_TABLE_IS_TCAM(table) && (table->access_method != DBAL_ACCESS_METHOD_MDB))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "COMMIT_OVERRIDE_DEFAULT_ENRTY flag is only valid for MDB or TCAM. Table %s\n",
                         table->table_name);
        }
        SHR_IF_ERR_EXIT(dbal_entry_default_values_add(unit, *entry_handle));
    }
    if (flags & DBAL_COMMIT_OVERRUN)
    {
        dbal_status_e status;
        SHR_IF_ERR_EXIT(dbal_status_get(unit, &status));
        if (status != DBAL_STATUS_DEVICE_INIT_DONE)
        {
            (*entry_handle)->overrun_entry = 1;
        }
    }

    
    if (DBAL_TABLE_IS_TCAM(table) && !(flags & DBAL_COMMIT_OVERRIDE_DEFAULT))
    {
        if ((*entry_handle)->nof_result_fields !=
            table->multi_res_info[(*entry_handle)->cur_res_type].nof_result_fields)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Some result fields were not set. Use flag DBAL_COMMIT_OVERRIDE_DEFAULT. table: %s\n",
                         table->table_name);
        }
    }

    
    if (!(flags & DBAL_COMMIT_IGNORE_ALLOC_ERROR))
    {
        uint32 nof_unique_flags = utilex_nof_on_bits_in_long(unique_action_flag_set);
        if (nof_unique_flags <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Entry commit expects at least one of the following flags:"
                         "COMMIT, COMMIT_UPDATE_ENTRY or COMMIT_FORCE_ENTRY. Flags combination is %u.\n", flags);
        }
        if (nof_unique_flags > 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Entry commit expects no more than one of the following flags:"
                         "COMMIT, COMMIT_UPDATE_ENTRY or COMMIT_FORCE_ENTRY. Flags combination is %u.\n", flags);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_action_skip_check(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_actions_e * action)
{
    dbal_status_e status;
    dbal_table_status_e table_status;
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_status_get(unit, &status));

    table = entry_handle->table;

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, entry_handle->table_id, &table_status));

    
    if (table_status == DBAL_TABLE_INCOMPATIBLE_IMAGE)
    {
        if (status == DBAL_STATUS_DEVICE_INIT_DONE)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Operation skipped table %s is not active in image\n\n"), table->table_name));
        }
        (*action) = DBAL_ACTION_SKIP;
        SHR_EXIT();
    }

    
    if (!dbal_image_name_is_std_1(unit))
    {
        if ((*action == DBAL_ACTION_ENTRY_COMMIT) || (*action == DBAL_ACTION_ENTRY_COMMIT_UPDATE) ||
            (*action == DBAL_ACTION_ENTRY_COMMIT_FORCE) || (*action == DBAL_ACTION_ENTRY_COMMIT_RANGE))
        {
            if (entry_handle->nof_result_fields == 0)
            {
                (*action) = DBAL_ACTION_SKIP;
                SHR_EXIT();
            }

        }
    }

    
    if (table_status == DBAL_TABLE_HW_ERROR)
    {
        if (DBAL_DEVICE_STATE_ON_PRODUCTION == dnx_data_dbal.db_init.dbal_device_state_get(unit)
            && dbal_image_name_is_std_1(unit))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "table %s in on hw error status, please fix\n", table->table_name);
        }
        if (status == DBAL_STATUS_DEVICE_INIT_DONE)
        {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Operation skipped table %s due to HW issues\n\n"), table->table_name));
        }
        (*action) = DBAL_ACTION_SKIP;
        SHR_EXIT();
    }

    
    if ((entry_handle->error_info.error_exists) && (!dbal_image_name_is_std_1(unit)))
    {
        if (status == DBAL_STATUS_DEVICE_INIT_DONE)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Operation skipped key field %s for table %s is not mapped for image\n\n"),
                       dbal_field_to_string(unit, entry_handle->error_info.field_id), table->table_name));
        }
        (*action) = DBAL_ACTION_SKIP;
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_action_start(
    int unit,
    uint32 handle_id,
    dbal_entry_handle_t ** entry_handle,
    dbal_entry_action_flags_e flags,
    dbal_actions_e * action)
{
    dbal_logical_table_t *table;
    CONST dbal_action_info_t *actions_info;
    int logger_severity_check_pass;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, handle_id, entry_handle));
    table = (*entry_handle)->table;

    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_NO_VALIDATIONS))
    {
        SHR_IF_ERR_EXIT(dbal_action_skip_check(unit, (*entry_handle), action));
        if ((*action) == DBAL_ACTION_SKIP)
        {
            SHR_EXIT();
        }
    }
    logger_severity_check_pass = bsl_fast_check_two(BSL_SOURCE_DBAL_DNX_CHK, BSL_SOURCE_DBALACCESS_DNX_CHK);

    if (logger_severity_check_pass)
    {
        SHR_IF_ERR_EXIT(dbal_logger_table_internal_lock(unit, (*entry_handle)->table_id));
    }

    
    if ((*action != DBAL_ACTION_TABLE_CLEAR) && (*action != DBAL_ACTION_ENTRY_GET_ACCESS_ID))
    {
        
        if (flags == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "No flags were set. At least one flag must be set.\n");
        }

        
        switch (*action)
        {
            case (DBAL_ACTION_ENTRY_GET):
                if (flags & DBAL_GET_ALL_FIELDS)
                {
                    (*entry_handle)->get_all_fields = TRUE;
                    (*action) = DBAL_ACTION_ENTRY_GET_ALL;
                }
                break;

            case (DBAL_ACTION_ENTRY_CLEAR):
                if (flags & DBAL_COMMIT)
                {
                    if ((*entry_handle)->nof_ranged_fields > 0)
                    {
                        (*action) = DBAL_ACTION_ENTRY_CLEAR_RANGE;
                    }
                }
                break;

            case (DBAL_ACTION_ENTRY_COMMIT):
                SHR_IF_ERR_EXIT(dbal_action_entry_commit_parameters_update(unit, entry_handle, flags, action));
                break;

            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "DBAL received unexpected action: %u\n", *action);
                break;
        }
    }

    actions_info = &dbal_actions_info[(*action)];

    if ((flags) && (flags & (~(actions_info->supported_flags))))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "flags combination %u is not supported for action %s\n",
                     flags, actions_info->name);
    }

    if ((actions_info->is_pre_acces_prints == PRE_ACCESS_PRINTS_ENABLED) && logger_severity_check_pass)
    {
        SHR_IF_ERR_EXIT(dbal_action_prints(unit, (*entry_handle), flags, (*action)));
    }

    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_NO_VALIDATIONS))
    {
        DBAL_VERIFICATIONS(dbal_action_validate(unit, (*entry_handle), *action, flags));
    }

    
    if ((table->table_type == DBAL_TABLE_TYPE_LPM) && (*action != DBAL_ACTION_TABLE_CLEAR))
    {
        SHR_IF_ERR_EXIT(dbal_lpm_mask_to_prefix_length(unit, (*entry_handle)));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_actions_get_next_bitmap_entry(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 *entry_found)
{
    dbal_iterator_info_t *iterator_info;
    uint32 word_offset = 0;
    uint32 bit_offset = 0;
    uint8 bit_index;
    uint32 optimized_iterator_bitmap_val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    *entry_found = FALSE;

    while (!*entry_found && !iterator_info->is_end)
    {
        if (!iterator_info->used_first_key)
        {
            entry_handle->phy_entry.key[0] = 0;
            if ((entry_handle->table->core_mode == DBAL_CORE_MODE_DPC))
            {
                
                entry_handle->core_id = DBAL_MAX_NUM_OF_CORES - 1;
            }
            else
            {
                
                entry_handle->core_id = 0;
            }
        }
        else
        {
            entry_handle->phy_entry.key[0]++;
            if (entry_handle->phy_entry.key[0] > iterator_info->max_num_of_iterations)
            {
                entry_handle->core_id--;
                if (entry_handle->core_id < 0)
                {
                    iterator_info->is_end = TRUE;
                    break;
                }
                else
                {
                    entry_handle->phy_entry.key[0] = 0;
                }
            }
            word_offset = DBAL_OPTIMIZED_ITERATOR_GET_WORD_INDEX(entry_handle->phy_entry.key[0]);
            bit_offset = DBAL_OPTIMIZED_ITERATOR_GET_BIT_OFFSET(entry_handle->phy_entry.key[0]);
        }

        iterator_info->used_first_key = TRUE;
        optimized_iterator_bitmap_val = entry_handle->table->iterator_optimized[entry_handle->core_id][word_offset];

        if (optimized_iterator_bitmap_val != 0)
        {
            int test_bit_val;
            for (bit_index = bit_offset; bit_index < 32; bit_index++)
            {
                test_bit_val = 0x1 << bit_index;
                if (optimized_iterator_bitmap_val & test_bit_val)
                {
                    *entry_found = TRUE;
                    
                    if (iterator_info->nof_key_rules > 0)
                    {
                        uint8 condition_pass = TRUE;
                        SHR_IF_ERR_EXIT(dbal_key_or_value_condition_check
                                        (unit, entry_handle, TRUE, iterator_info->key_rules_info,
                                         iterator_info->nof_key_rules, &condition_pass));
                        if (condition_pass == FALSE)
                        {
                            *entry_found = FALSE;
                        }
                    }
                    if (*entry_found)
                    {
                        break;
                    }
                }
                else if (test_bit_val > optimized_iterator_bitmap_val)
                {
                    
                    entry_handle->phy_entry.key[0] = ((word_offset + 1) << 5);
                    break;
                }
                entry_handle->phy_entry.key[0]++;
            }
            if (!*entry_found)
            {
                
                entry_handle->phy_entry.key[0]--;
            }
        }
        else
        {
            entry_handle->phy_entry.key[0] = ((word_offset) << 5) + 31;
        }
    }

    if (*entry_found)
    {
        
        
        if ((dbal_table_is_out_lif(entry_handle->table)))
        {
            SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get
                            (unit, *entry_handle->phy_entry.key, NULL, (uint32 *) &entry_handle->cur_res_type, NULL,
                             NULL, NULL, NULL));
        }
        
        if (dbal_table_is_in_lif(entry_handle->table))
        {
            SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get
                            (unit, *entry_handle->phy_entry.key, entry_handle->core_id,
                             entry_handle->table->physical_db_id[0], NULL, (uint32 *) &entry_handle->cur_res_type,
                             NULL));
        }
    }

exit:
    SHR_FUNC_EXIT;

}


static void
dbal_action_consecutive_range_bit_map_entry(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int num_of_entries,
    uint8 bit_set)
{
    uint32 word_offset;
    uint32 bit_offset;
    uint32 entry_count;
    uint32 entry_key = entry_handle->phy_entry.key[0];
    int core_id;

    for (entry_count = 0; entry_count < num_of_entries; entry_count++)
    {
        word_offset = DBAL_OPTIMIZED_ITERATOR_GET_WORD_INDEX(entry_key);
        bit_offset = DBAL_OPTIMIZED_ITERATOR_GET_BIT_OFFSET(entry_key);

        if ((entry_handle->table->core_mode == DBAL_CORE_MODE_DPC))
        {
            if (entry_handle->core_id == DBAL_CORE_ALL)
            {
                for (core_id = 0; core_id < DBAL_MAX_NUM_OF_CORES; core_id++)
                {
                    if (bit_set)
                    {
                        SHR_BITSET(&entry_handle->table->iterator_optimized[core_id][word_offset], bit_offset);
                        DBAL_SW_STATE_TBL_PROP.iterator_optimized.bitmap.bit_set(unit,
                                                                                 entry_handle->table_id,
                                                                                 core_id, entry_key);
                    }
                    else
                    {
                        SHR_BITCLR(&entry_handle->table->iterator_optimized[core_id][word_offset], bit_offset);
                        DBAL_SW_STATE_TBL_PROP.iterator_optimized.bitmap.bit_clear(unit, entry_handle->table_id,
                                                                                   core_id, entry_key);
                    }
                }
            }
            else
            {
                if (bit_set)
                {
                    SHR_BITSET(&entry_handle->table->iterator_optimized[entry_handle->core_id][word_offset],
                               bit_offset);
                    DBAL_SW_STATE_TBL_PROP.iterator_optimized.bitmap.bit_set(unit, entry_handle->table_id,
                                                                             entry_handle->core_id, entry_key);
                }
                else
                {
                    SHR_BITCLR(&entry_handle->table->iterator_optimized[entry_handle->core_id][word_offset],
                               bit_offset);
                    DBAL_SW_STATE_TBL_PROP.iterator_optimized.bitmap.bit_clear(unit, entry_handle->table_id,
                                                                               entry_handle->core_id, entry_key);
                }
            }
        }
        else
        {
            if (bit_set)
            {
                SHR_BITSET(&entry_handle->table->iterator_optimized[0][word_offset], bit_offset);
                DBAL_SW_STATE_TBL_PROP.iterator_optimized.bitmap.bit_set(unit, entry_handle->table_id, 0, entry_key);
            }
            else
            {
                SHR_BITCLR(&entry_handle->table->iterator_optimized[0][word_offset], bit_offset);
                DBAL_SW_STATE_TBL_PROP.iterator_optimized.bitmap.bit_clear(unit, entry_handle->table_id, 0, entry_key);
            }
        }

        entry_key++;
    }
}

shr_error_e
dbal_actions_optimized_is_exists(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *is_entry_exists)
{
    uint32 word_offset = DBAL_OPTIMIZED_ITERATOR_GET_WORD_INDEX(entry_handle->phy_entry.key[0]);
    uint32 bit_offset = DBAL_OPTIMIZED_ITERATOR_GET_BIT_OFFSET(entry_handle->phy_entry.key[0]);
    uint32 bitmap_val;

    SHR_FUNC_INIT_VARS(unit);

    (*is_entry_exists) = TRUE;

    if (!SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_ITERATOR_OPTIMIZED))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_CONFIG);
        SHR_EXIT();
        
    }

    if ((entry_handle->table->core_mode == DBAL_CORE_MODE_DPC))
    {
        if (entry_handle->core_id == DBAL_CORE_ALL)
        {
            bitmap_val = entry_handle->table->iterator_optimized[0][word_offset];
        }
        else
        {
            bitmap_val = entry_handle->table->iterator_optimized[entry_handle->core_id][word_offset];
        }
    }
    else
    {
        bitmap_val = entry_handle->table->iterator_optimized[0][word_offset];
    }

    if (!(bitmap_val & (0x1 << bit_offset)))
    {
        *is_entry_exists = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_actions_access_commit_consecutive_range(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int num_of_entries)
{
    dbal_logical_table_t *table;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_WRITE, DBAL_ACTION_ACCESS_FUNC_COMMIT_CONSEC));

    table = entry_handle->table;

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_ITERATOR_OPTIMIZED))
    {
        dbal_action_consecutive_range_bit_map_entry(unit, entry_handle, num_of_entries, TRUE);
    }

    switch (table->access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "MDB consecutive mode not supported yet \n");
            break;

        case DBAL_ACCESS_METHOD_TCAM_CS:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "TCAM CS consecutive mode not supported yet \n");
            break;

        case DBAL_ACCESS_METHOD_HARD_LOGIC:
            SHR_IF_ERR_EXIT(dbal_hl_range_entry_set(unit, entry_handle, num_of_entries));
            break;

        case DBAL_ACCESS_METHOD_SW_STATE:
            
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "SW consecutive mode not supported yet \n");
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
            
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "pemla consecutive mode not supported yet \n");
            break;

        case DBAL_ACCESS_METHOD_KBP:
            
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP consecutive mode not supported yet \n");
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %s \n",
                         dbal_access_method_to_string(unit, table->access_method));
            break;
    }

exit:
    DBAL_ACTIONS_SET_ERR(dbal_action_access_post_access(unit, entry_handle, NULL, DBAL_ACTION_ACCESS_TYPE_WRITE,
                                                        DBAL_ACTION_ACCESS_FUNC_COMMIT_CONSEC, SHR_GET_CURRENT_ERR()));
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_actions_access_clear_consecutive_range(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int num_of_entries)
{
    dbal_logical_table_t *table;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_WRITE, DBAL_ACTION_ACCESS_FUNC_CLEAR_CONSEC));

    table = entry_handle->table;

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_ITERATOR_OPTIMIZED))
    {
        dbal_action_consecutive_range_bit_map_entry(unit, entry_handle, num_of_entries, FALSE);
    }

    
    switch (table->access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "MDB consecutive mode not supported yet \n");
            break;

        case DBAL_ACCESS_METHOD_TCAM_CS:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "TCAM CS consecutive mode not supported yet \n");
            break;

        case DBAL_ACCESS_METHOD_HARD_LOGIC:
            SHR_IF_ERR_EXIT(dbal_hl_range_entry_clear(unit, entry_handle, num_of_entries));
            break;

        case DBAL_ACCESS_METHOD_SW_STATE:
            
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "SW consecutive mode not supported yet \n");
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
            
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "pemla consecutive mode not supported yet \n");
            break;

        case DBAL_ACCESS_METHOD_KBP:
            
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP consecutive mode not supported yet \n");
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %s \n",
                         dbal_access_method_to_string(unit, table->access_method));
            break;
    }

exit:
    DBAL_ACTIONS_SET_ERR(dbal_action_access_post_access(unit, entry_handle, NULL, DBAL_ACTION_ACCESS_TYPE_WRITE,
                                                        DBAL_ACTION_ACCESS_FUNC_CLEAR_CONSEC, SHR_GET_CURRENT_ERR()));
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_actions_access_result_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_READ, DBAL_ACTION_ACCESS_FUNC_RESULT_TYPE_RES));

    if (!SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        entry_handle->cur_res_type = 0;
        SHR_EXIT();
    }

    if (entry_handle->cur_res_type > DBAL_RESULT_TYPE_NOT_INITIALIZED)
    {
        
        SHR_EXIT();
    }

    switch (entry_handle->table->access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
            {
                SHR_SET_CURRENT_ERR(dbal_mdb_res_type_resolution(unit, entry_handle, TRUE));
            }
            break;

        case DBAL_ACCESS_METHOD_HARD_LOGIC:
            SHR_SET_CURRENT_ERR(dbal_hl_res_type_resolution(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_SW_STATE:
            SHR_SET_CURRENT_ERR(dbal_sw_res_type_resolution(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_KBP:
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
        case DBAL_ACCESS_METHOD_TCAM_CS:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Result type resolution is not supported for access method %s\n",
                         dbal_access_method_to_string(unit, entry_handle->table->access_method));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %s \n",
                         dbal_access_method_to_string(unit, entry_handle->table->access_method));
            break;
    }

exit:
    DBAL_ACTIONS_SET_ERR(dbal_action_access_post_access(unit, entry_handle, NULL, DBAL_ACTION_ACCESS_TYPE_READ,
                                                        DBAL_ACTION_ACCESS_FUNC_RESULT_TYPE_RES,
                                                        SHR_GET_CURRENT_ERR()));
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_actions_access_entry_commit(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_handle_t * get_handle)
{
    dbal_logical_table_t *table = entry_handle->table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_WRITE, DBAL_ACTION_ACCESS_FUNC_ENTRY_COMMIT));

    switch (table->access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            SHR_IF_ERR_EXIT(dbal_mdb_table_entry_add(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_TCAM_CS:
            SHR_IF_ERR_EXIT(dbal_tcam_cs_entry_add(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_HARD_LOGIC:
            SHR_IF_ERR_EXIT(dbal_hl_entry_set(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_SW_STATE:
            SHR_IF_ERR_EXIT(dbal_sw_table_entry_set(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
            SHR_IF_ERR_EXIT(dbal_pemla_table_entry_set(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_KBP:
            SHR_IF_ERR_EXIT(dbal_kbp_entry_add(unit, entry_handle));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %s \n",
                         dbal_access_method_to_string(unit, table->access_method));
            break;
    }

    if (entry_handle->phy_entry.indirect_commit_mode == DBAL_INDIRECT_COMMIT_MODE_NORMAL)
    {
        
        SHR_IF_ERR_EXIT(dbal_tables_entry_counter_update(unit, entry_handle, 1, TRUE));
    }

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_ITERATOR_OPTIMIZED))
    {
        dbal_action_consecutive_range_bit_map_entry(unit, entry_handle, 1, TRUE);
    }

exit:
    DBAL_ACTIONS_SET_ERR(dbal_action_access_post_access(unit, entry_handle, get_handle, DBAL_ACTION_ACCESS_TYPE_WRITE,
                                                        DBAL_ACTION_ACCESS_FUNC_ENTRY_COMMIT, SHR_GET_CURRENT_ERR()));
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_actions_access_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_handle_t * get_entry_handle,
    int is_sw_get)
{
    int is_get_on_new_entry = 0;
    int er_post_access_required = FALSE;
    dbal_logical_table_t *table = entry_handle->table;

    SHR_FUNC_INIT_VARS(unit);

    
    if (get_entry_handle != NULL)
    {
        
        int size_to_copy = (sizeof(dbal_entry_handle_t) - (2 * DBAL_PHYSICAL_RES_SIZE_IN_BYTES));

        is_get_on_new_entry = 1;

        
        sal_memcpy(get_entry_handle, entry_handle, size_to_copy);

        
        sal_memset(&get_entry_handle->phy_entry.payload, 0, DBAL_TABLE_BUFFER_IN_BYTES(table));
        sal_memset(&get_entry_handle->phy_entry.p_mask, 0, DBAL_TABLE_BUFFER_IN_BYTES(table));
        if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
        {
            
            if (table->access_method != DBAL_ACCESS_METHOD_MDB
                || (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW)
                    && !(dbal_table_is_out_lif(table)) && !(dbal_table_is_in_lif(table))))
            {
                get_entry_handle->cur_res_type = DBAL_RESULT_TYPE_NOT_INITIALIZED;
            }
        }
        get_entry_handle->get_all_fields = 1;

        if (is_sw_get)
        {
            get_entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_SW_SHADOW;
        }
    }
    else
    {
        
        get_entry_handle = entry_handle;
        if (is_sw_get)
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "get from SW is not supported for operation on the same handle");
        }
    }

    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        entry_handle->cur_res_type = 0;
    }
    else if (get_entry_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
    {
        SHR_SET_CURRENT_ERR(dbal_actions_access_result_type_resolution(unit, get_entry_handle));
    }

    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_READ, DBAL_ACTION_ACCESS_FUNC_ENTRY_GET));
    er_post_access_required = TRUE;

    if (!SHR_FUNC_ERR())
    {
        
        switch (table->access_method)
        {
            case DBAL_ACCESS_METHOD_MDB:
                SHR_SET_CURRENT_ERR(dbal_mdb_table_entry_get(unit, get_entry_handle));
                break;

            case DBAL_ACCESS_METHOD_TCAM_CS:
                SHR_SET_CURRENT_ERR(dbal_tcam_cs_entry_get(unit, get_entry_handle));
                break;

            case DBAL_ACCESS_METHOD_HARD_LOGIC:
                SHR_SET_CURRENT_ERR(dbal_hl_entry_get(unit, get_entry_handle));
                break;

            case DBAL_ACCESS_METHOD_SW_STATE:
                SHR_SET_CURRENT_ERR(dbal_sw_table_entry_get(unit, get_entry_handle));
                break;

            case DBAL_ACCESS_METHOD_PEMLA:
                SHR_SET_CURRENT_ERR(dbal_pemla_table_entry_get(unit, get_entry_handle));
                break;

            case DBAL_ACCESS_METHOD_KBP:
                SHR_SET_CURRENT_ERR(dbal_kbp_entry_get(unit, get_entry_handle));
                break;

            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %s \n",
                             dbal_access_method_to_string(unit, table->access_method));
                break;
        }

        if (!SHR_FUNC_ERR())
        {
            
            if (is_get_on_new_entry)
            {
                SHR_IF_ERR_EXIT(dbal_entry_get_handle_update_value_field(unit, get_entry_handle));
            }
        }
    }

exit:
    if (er_post_access_required)
    {
        DBAL_ACTIONS_SET_ERR(dbal_action_access_post_access
                             (unit, entry_handle, get_entry_handle, DBAL_ACTION_ACCESS_TYPE_READ,
                              DBAL_ACTION_ACCESS_FUNC_ENTRY_GET, SHR_GET_CURRENT_ERR()));
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_actions_access_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_logical_table_t *table;
    dbal_entry_handle_t *get_entry_handle = NULL;
    dbal_entry_handle_t *entry_handle_for_er = NULL;
    int is_sw_get = 1;
    int rv;

    uint8 post_access_required = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    table = entry_handle->table;

    
    entry_handle->phy_entry.hitbit = 0;

    
    entry_handle_for_er = entry_handle;

    if ((entry_handle->er_flags) || (DBAL_TABLE_IS_NONE_DIRECT(entry_handle->table))
        || ((table->nof_result_types > 1) && (entry_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)))
    {
        SHR_ALLOC_SET_ZERO(get_entry_handle, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n", EMPTY, EMPTY,
                           EMPTY);

        
        if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_LEARNING_EN))
        {
            is_sw_get = 0;
        }
        
        rv = dbal_actions_access_entry_get(unit, entry_handle, get_entry_handle, is_sw_get);

        
        get_entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_ALL;

        entry_handle_for_er = get_entry_handle;
        if (DBAL_TABLE_IS_NONE_DIRECT(entry_handle->table))
        {
            SHR_IF_ERR_EXIT(rv);
        }
        else
        {
            int is_default = 0;

            
            SHR_IF_ERR_EXIT(dbal_entry_direct_is_default(unit, get_entry_handle, &is_default));
            if (is_default)
            {
                SHR_EXIT();
            }
        }

        
        if (entry_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
        {
            if (get_entry_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "No result type for current handle, table name %s", table->table_name);
            }
            entry_handle->cur_res_type = get_entry_handle->cur_res_type;
        }
    }

    post_access_required = TRUE;
    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_WRITE, DBAL_ACTION_ACCESS_FUNC_ENTRY_CLEAR));

    if (DBAL_TABLE_IS_NONE_DIRECT(entry_handle->table))
    {
        
        if (dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_limit_per_lif_counters) &&
            dbal_logical_table_is_mact(unit, entry_handle->table_id))
        {
            sal_memcpy(&entry_handle->phy_entry.payload, &get_entry_handle->phy_entry.payload,
                       DBAL_TABLE_BUFFER_IN_BYTES(entry_handle->table));
            sal_memcpy(&entry_handle->phy_entry.p_mask, &get_entry_handle->phy_entry.p_mask,
                       DBAL_TABLE_BUFFER_IN_BYTES(entry_handle->table));
        }
    }

    switch (table->access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            SHR_IF_ERR_EXIT(dbal_mdb_table_entry_delete(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_TCAM_CS:
            SHR_IF_ERR_EXIT(dbal_tcam_cs_entry_delete(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_HARD_LOGIC:
            SHR_IF_ERR_EXIT(dbal_hl_entry_clear(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_SW_STATE:
            SHR_IF_ERR_EXIT(dbal_sw_table_entry_clear(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
            SHR_IF_ERR_EXIT(dbal_pemla_table_entry_clear(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_KBP:
            SHR_IF_ERR_EXIT(dbal_kbp_entry_delete(unit, entry_handle));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %s \n",
                         dbal_access_method_to_string(unit, table->access_method));
            break;
    }

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_ITERATOR_OPTIMIZED))
    {
        dbal_action_consecutive_range_bit_map_entry(unit, entry_handle, 1, FALSE);
    }

    
    SHR_IF_ERR_EXIT(dbal_tables_entry_counter_update(unit, entry_handle, 1, FALSE));

exit:
    if (post_access_required)
    {
        DBAL_ACTIONS_SET_ERR(dbal_action_access_post_access
                             (unit, entry_handle, entry_handle_for_er, DBAL_ACTION_ACCESS_TYPE_WRITE,
                              DBAL_ACTION_ACCESS_FUNC_ENTRY_CLEAR, SHR_GET_CURRENT_ERR()));
    }
    SHR_FREE(get_entry_handle);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_actions_access_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_WRITE, DBAL_ACTION_ACCESS_FUNC_TABLE_CLEAR));

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_ITERATOR_OPTIMIZED))
    {
        dbal_iterator_info_t *iterator_info;
        uint8 entry_found = FALSE;

        SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));
        SHR_IF_ERR_EXIT(dbal_entry_handle_clear_macro(unit, entry_handle->table_id, entry_handle->handle_id));

        iterator_info->mode = DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT;
        iterator_info->is_end = FALSE;
        iterator_info->used_first_key = FALSE;
        entry_handle->get_all_fields = 1;
        iterator_info->is_init = 1;

        SHR_IF_ERR_EXIT(dbal_iterator_init_handle_info(unit, entry_handle));

        if (entry_handle->table->table_type == DBAL_TABLE_TYPE_DIRECT)
        {
            SHR_IF_ERR_EXIT(dbal_tables_max_key_value_get
                            (unit, entry_handle->table_id, &iterator_info->max_num_of_iterations));
        }
        else
        {
            
            if (entry_handle->table_id == DBAL_TABLE_GLOBAL_RIF_EM)
            {
                iterator_info->max_num_of_iterations = dnx_data_l3.rif.nof_rifs_get(unit);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported access method %s \n",
                             dbal_access_method_to_string(unit, entry_handle->table->access_method));
            }
        }

        SHR_IF_ERR_EXIT(dbal_actions_get_next_bitmap_entry(unit, entry_handle, &entry_found));
        while (!iterator_info->is_end)
        {
            SHR_IF_ERR_EXIT(dbal_actions_access_entry_clear(unit, entry_handle));
            SHR_IF_ERR_EXIT(dbal_actions_get_next_bitmap_entry(unit, entry_handle, &entry_found));
        }
    }
    else
    {
        switch (entry_handle->table->access_method)
        {
            case DBAL_ACCESS_METHOD_MDB:
                SHR_IF_ERR_EXIT(dbal_mdb_table_clear(unit, entry_handle));
                break;

            case DBAL_ACCESS_METHOD_TCAM_CS:
                SHR_IF_ERR_EXIT(dbal_tcam_cs_table_clear(unit, entry_handle));
                break;

            case DBAL_ACCESS_METHOD_HARD_LOGIC:
                SHR_IF_ERR_EXIT(dbal_hl_table_clear(unit, entry_handle));
                break;

            case DBAL_ACCESS_METHOD_SW_STATE:
                SHR_IF_ERR_EXIT(dbal_sw_table_clear(unit, entry_handle));
                break;

            case DBAL_ACCESS_METHOD_PEMLA:
                SHR_IF_ERR_EXIT(dbal_pemla_table_clear(unit, entry_handle));
                break;

            case DBAL_ACCESS_METHOD_KBP:
                SHR_IF_ERR_EXIT(dbal_kbp_table_clear(unit, entry_handle));
                break;

            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %s \n",
                             dbal_access_method_to_string(unit, entry_handle->table->access_method));
                break;
        }
    }

    
    SHR_IF_ERR_EXIT(dbal_tables_entry_counter_update(unit, entry_handle, -1, FALSE));

exit:
    DBAL_ACTIONS_SET_ERR(dbal_action_access_post_access(unit, entry_handle, NULL, DBAL_ACTION_ACCESS_TYPE_WRITE,
                                                        DBAL_ACTION_ACCESS_FUNC_TABLE_CLEAR, SHR_GET_CURRENT_ERR()));
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_actions_access_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_SW_ONLY, DBAL_ACTION_ACCESS_FUNC_ITERATOR_INIT));

    
    switch (entry_handle->table->access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            SHR_IF_ERR_EXIT(dbal_mdb_table_iterator_init(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_TCAM_CS:
            SHR_IF_ERR_EXIT(dbal_tcam_cs_iterator_init(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_HARD_LOGIC:
            SHR_IF_ERR_EXIT(dbal_hl_table_iterator_init(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_SW_STATE:
            SHR_IF_ERR_EXIT(dbal_sw_table_iterator_init(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
            SHR_IF_ERR_EXIT(dbal_pemla_table_iterator_init(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_KBP:
            SHR_IF_ERR_EXIT(dbal_kbp_table_iterator_init(unit, entry_handle));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %d \n", entry_handle->table->access_method);
            break;
    }

exit:
    DBAL_ACTIONS_SET_ERR(dbal_action_access_post_access(unit, entry_handle, NULL, DBAL_ACTION_ACCESS_TYPE_SW_ONLY,
                                                        DBAL_ACTION_ACCESS_FUNC_ITERATOR_INIT, SHR_GET_CURRENT_ERR()));
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_actions_access_iterator_destroy(
    int unit,
    dbal_entry_handle_t * entry_handle)
{

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_SW_ONLY, DBAL_ACTION_ACCESS_FUNC_ITERATOR_DESTROY));

    switch (entry_handle->table->access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            SHR_IF_ERR_EXIT(dbal_mdb_table_iterator_deinit(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_TCAM_CS:
            SHR_IF_ERR_EXIT(dbal_tcam_cs_iterator_deinit(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_HARD_LOGIC:
        case DBAL_ACCESS_METHOD_SW_STATE:
            
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
            SHR_IF_ERR_EXIT(dbal_pemla_table_iterator_deinit(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_KBP:
            SHR_IF_ERR_EXIT(dbal_kbp_table_iterator_deinit(unit, entry_handle));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %d \n", entry_handle->table->access_method);
            break;
    }

exit:
    DBAL_ACTIONS_SET_ERR(dbal_action_access_post_access(unit, entry_handle, NULL, DBAL_ACTION_ACCESS_TYPE_SW_ONLY,
                                                        DBAL_ACTION_ACCESS_FUNC_ITERATOR_DESTROY,
                                                        SHR_GET_CURRENT_ERR()));
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_iterator_pre_action_handle_update(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_iterator_info_t * iterator_info)
{
    dbal_iterator_attribute_info_t *iterator_attrib_info = &iterator_info->attrib_info;

    SHR_FUNC_INIT_VARS(unit);

    
    if (iterator_attrib_info->hit_bit_rule_valid)
    {
        
        if (!dbal_logical_table_is_mact(unit, entry_handle->table_id))
        {
            SHR_IF_ERR_EXIT(dbal_entry_attribute_request_internal
                            (unit, entry_handle, iterator_attrib_info->hit_bit_rule_type, NULL));
        }
        else
        {
            if (iterator_info->mdb_iterator.start_non_flush_iteration)
            {
                SHR_IF_ERR_EXIT(dbal_entry_attribute_request_internal
                                (unit, entry_handle, iterator_attrib_info->hit_bit_rule_type, NULL));
            }
        }
    }

    
    if (iterator_attrib_info->age_rule_valid)
    {
        iterator_attrib_info->age_rule_entry_value = 0;
        SHR_IF_ERR_EXIT(dbal_entry_attribute_request_internal
                        (unit, entry_handle, DBAL_ENTRY_ATTR_AGE, &iterator_attrib_info->age_rule_entry_value));
    }

    
    if (iterator_attrib_info->hit_bit_action_get)
    {
        SHR_IF_ERR_EXIT(dbal_entry_attribute_request_internal
                        (unit, entry_handle, iterator_attrib_info->hit_bit_action_get_type, NULL));
    }

    
    if (iterator_attrib_info->hit_bit_action_clear)
    {
        SHR_IF_ERR_EXIT(dbal_entry_attribute_request_internal
                        (unit, entry_handle, iterator_attrib_info->hit_bit_action_clear_type, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_actions_access_iterator_get_next(
    int unit,
    uint32 entry_handle_id,
    int *is_end,
    uint8 *continue_iterating)
{
    dbal_entry_handle_t *entry_handle = NULL;
    uint8 post_access_required = FALSE;
    dbal_iterator_info_t *iterator_info;
    dbal_logical_table_t *table = NULL;

    SHR_FUNC_INIT_VARS(unit);

    (*is_end) = 0;

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &(entry_handle)));
    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    if (!iterator_info->is_init)
    {
        SHR_ERR_EXIT(_SHR_E_EMPTY, "Trying to get an entry with uninitialized iterator\n");
    }

    table = entry_handle->table;
    post_access_required = TRUE;
    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_READ, DBAL_ACTION_ACCESS_FUNC_ITERATOR_GET_NEXT));


    
    entry_handle->phy_entry.payload_size = table->max_payload_size;
    entry_handle->phy_entry.hitbit = 0;

    if ((iterator_info->nof_val_rules > 0) || (iterator_info->nof_actions > 0))
    {
        SHR_IF_ERR_EXIT(dbal_iterator_pre_action_handle_update(unit, entry_handle, iterator_info));
    }

    
    if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_ITERATOR_OPTIMIZED))
    {
        uint8 entry_found = FALSE;
        SHR_IF_ERR_EXIT(dbal_actions_get_next_bitmap_entry(unit, entry_handle, &entry_found));
        if (entry_found)
        {
            SHR_IF_ERR_EXIT(dbal_actions_access_entry_get(unit, entry_handle, NULL, 0));
        }
    }
    else
    {
        switch (entry_handle->table->access_method)
        {
            case DBAL_ACCESS_METHOD_MDB:
                SHR_IF_ERR_EXIT(dbal_mdb_table_entry_get_next(unit, entry_handle));
                break;

            case DBAL_ACCESS_METHOD_TCAM_CS:
                SHR_IF_ERR_EXIT(dbal_tcam_cs_entry_get_next(unit, entry_handle));
                break;

            case DBAL_ACCESS_METHOD_HARD_LOGIC:
                SHR_IF_ERR_EXIT(dbal_hl_entry_get_next(unit, entry_handle));
                break;

            case DBAL_ACCESS_METHOD_SW_STATE:
                SHR_IF_ERR_EXIT(dbal_sw_table_entry_get_next(unit, entry_handle));
                break;

            case DBAL_ACCESS_METHOD_PEMLA:
                SHR_IF_ERR_EXIT(dbal_pemla_table_iterator_get_next(unit, entry_handle));
                break;

            case DBAL_ACCESS_METHOD_KBP:
                SHR_IF_ERR_EXIT(dbal_kbp_table_entry_get_next(unit, entry_handle));
                break;

            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %d \n", table->access_method);
                break;
        }
    }

    if (!iterator_info->is_end)
    {
        uint8 is_value_rules_pass = TRUE;
        dbal_iterator_attribute_info_t *iterator_attrib_info = &iterator_info->attrib_info;
        entry_handle->handle_status = DBAL_HANDLE_STATUS_ACTION_PREFORMED;
        if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
        {
            int ii;
            
            for (ii = 0; ii < DBAL_RES_INFO.nof_result_fields; ii++)
            {
                entry_handle->value_field_ids[ii] = DBAL_RES_INFO.results_info[ii].field_id;
            }
            entry_handle->nof_result_fields = DBAL_RES_INFO.nof_result_fields;
        }
        if (iterator_attrib_info->age_rule_valid ||
            ((iterator_attrib_info->hit_bit_rule_valid || (iterator_info->nof_val_rules > 0)) &&
             (entry_handle->phy_entry.mdb_action_apply != DBAL_MDB_ACTION_APPLY_SW_SHADOW) &&
             (entry_handle->phy_entry.mdb_action_apply != DBAL_MDB_ACTION_APPLY_NONE)))
        {
            SHR_IF_ERR_EXIT(dbal_key_or_value_condition_check(unit, entry_handle, FALSE,
                                                              iterator_info->val_rules_info,
                                                              iterator_info->nof_val_rules, &is_value_rules_pass));
        }

        if (!is_value_rules_pass)
        {
            
            *continue_iterating = TRUE;
            SHR_EXIT();
        }

        
        if (table->table_type == DBAL_TABLE_TYPE_LPM)
        {
            SHR_IF_ERR_EXIT(dbal_lpm_prefix_length_to_mask(unit, entry_handle));
        }
        iterator_info->entries_counter++;
    }
    else
    {
        (*is_end) = 1;
        SHR_EXIT();
    }

exit:
    if (post_access_required)
    {
        DBAL_ACTIONS_SET_ERR(dbal_action_access_post_access(unit, entry_handle, NULL, DBAL_ACTION_ACCESS_TYPE_READ,
                                                            DBAL_ACTION_ACCESS_FUNC_ITERATOR_GET_NEXT,
                                                            SHR_GET_CURRENT_ERR()));
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_actions_access_access_id_by_key_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_READ, DBAL_ACTION_ACCESS_FUNC_ITERATOR_ACCESS_ID_GET));

    if (entry_handle->table->table_type != DBAL_TABLE_TYPE_TCAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "table type not supported %d for this operation\n",
                     entry_handle->table->table_type);
    }

    switch (entry_handle->table->access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            rv = dbal_mdb_table_access_id_by_key_get(unit, entry_handle);
            if (rv == _SHR_E_NOT_FOUND)
            {
                SHR_IF_ERR_EXIT_NO_MSG(rv);
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
            break;

        case DBAL_ACCESS_METHOD_KBP:
            rv = dbal_kbp_access_id_by_key_get(unit, entry_handle);
            if (rv == _SHR_E_NOT_FOUND)
            {
                SHR_IF_ERR_EXIT_NO_MSG(rv);
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
            break;
        case DBAL_ACCESS_METHOD_TCAM_CS:
        case DBAL_ACCESS_METHOD_HARD_LOGIC:
        case DBAL_ACCESS_METHOD_SW_STATE:
        case DBAL_ACCESS_METHOD_PEMLA:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "API not support for access type %s \n",
                         dbal_access_method_to_string(unit, entry_handle->table->access_method));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %d \n", entry_handle->table->access_method);
            break;
    }

exit:
    DBAL_ACTIONS_SET_ERR(dbal_action_access_post_access(unit, entry_handle, NULL, DBAL_ACTION_ACCESS_TYPE_READ,
                                                        DBAL_ACTION_ACCESS_FUNC_ITERATOR_ACCESS_ID_GET,
                                                        SHR_GET_CURRENT_ERR()));
    SHR_FUNC_EXIT;
}
