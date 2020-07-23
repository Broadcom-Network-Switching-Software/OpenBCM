/**
 * \file dbal_iterator_api.c
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * System wide Logical Table Manager.
 *
 * To be used for: All DBAL table iterator APIs
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

#include "dbal_internal.h"
#include <soc/dnx/dbal/dbal.h>
#include <src/soc/dnx/flush/flush_internal.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_framework.h>
#include <bcm_int/dnx/init/init_time_analyzer.h>


extern uint32 G_dbal_field_full_mask[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];



static uint8
dbal_iterator_entry_action_check(
    int unit,
    dbal_iterator_action_types_e action_type)
{
    if ((action_type == DBAL_ITER_ACTION_DELETE) || (action_type == DBAL_ITER_ACTION_HIT_CLEAR)
        || (action_type == DBAL_ITER_ACTION_READ_TO_DMA))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


static uint8
dbal_iterator_field_action_check(
    int unit,
    dbal_iterator_action_types_e action_type)
{
    if (action_type == DBAL_ITER_ACTION_UPDATE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


static shr_error_e
dbal_iterator_action_validate(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int ii;
    dbal_iterator_info_t *iterator_info;
    dbal_iterator_attribute_info_t *iterator_attrib_info;
    uint8 has_update_action = FALSE;
    uint8 has_delete_action = FALSE;
    uint8 has_get_action = FALSE;
    uint8 has_get_dma_action = FALSE;
    uint8 has_hit_bit_clear_action = FALSE;
    uint8 has_hit_bit_get_action = FALSE;
    uint32 actions = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));
    iterator_attrib_info = &iterator_info->attrib_info;

    for (ii = 0; ii < iterator_info->nof_actions; ii++)
    {
        switch (iterator_info->actions_info[ii].action_type)
        {
            case DBAL_ITER_ACTION_UPDATE:
                has_update_action = TRUE;
                break;
            case DBAL_ITER_ACTION_DELETE:
                if ((entry_handle->table->physical_db_id[0] == DBAL_PHYSICAL_TABLE_KBP) ||
                    (entry_handle->table->physical_db_id[0] == DBAL_PHYSICAL_TABLE_KAPS_1) ||
                    (entry_handle->table->physical_db_id[0] == DBAL_PHYSICAL_TABLE_KAPS_2))
                {
                    SHR_ERR_EXIT(_SHR_E_EMPTY, "action delete not supported for KBP and KAPS tables\n");
                }
                has_delete_action = TRUE;
                break;
            case DBAL_ITER_ACTION_HIT_CLEAR:
                has_hit_bit_clear_action = TRUE;
                break;
            case DBAL_ITER_ACTION_HIT_GET:
                has_hit_bit_get_action = TRUE;
                break;
            case DBAL_ITER_ACTION_GET:
                has_get_action = TRUE;
                break;
            case DBAL_ITER_ACTION_READ_TO_DMA:
                has_get_dma_action = TRUE;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown action type, %d\n", iterator_info->actions_info[ii].action_type);
        }

        
        actions |= iterator_info->actions_info[ii].action_type;

        
        if (iterator_info->actions_info[ii].field_id == DBAL_FIELD_RESULT_TYPE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Action can not be set on RESULT_TYPE field. table %s\n",
                         entry_handle->table->table_name);
        }
    }

    
    if ((has_hit_bit_get_action || has_hit_bit_clear_action) && iterator_attrib_info->hit_bit_rule_valid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Hit bits rule cannot be set along with hit bit actions. table %s",
                     entry_handle->table->table_name);
    }

    
    if (has_hit_bit_get_action || has_hit_bit_clear_action || iterator_attrib_info->hit_bit_rule_valid)
    {
        if (entry_handle->table->access_method != DBAL_ACCESS_METHOD_MDB)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Hit bits rules/actions can be set to MDB tables. table %s \n",
                         entry_handle->table->table_name);
        }
    }

    if (has_get_action && has_get_dma_action)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "action GET and action GET to DMA cannot be set together. table %s\n",
                     entry_handle->table->table_name);
    }

    if ((iterator_info->mode != DBAL_ITER_MODE_ACTION_NON_BLOCKING) && has_get_dma_action)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "action GET to DMA must be set with non_blocking iterator mode. table %s\n",
                     entry_handle->table->table_name);
    }

    if (has_hit_bit_get_action && !has_get_action)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Hit bits get action can be set only with action GET, means calling iteator_get_next API. table %s\n",
                     entry_handle->table->table_name);
    }

    
    if (has_update_action && has_delete_action)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "action types DELETE and UPDATE cannot be set together\n");
    }

    
    if (has_hit_bit_clear_action && has_delete_action)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "action types of HIT BIT and DELETE cannot be set together\n");
    }

    if ((has_hit_bit_clear_action || has_delete_action || has_update_action) &&
        (iterator_info->attrib_info.age_rule_valid))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "age rule is invalid with delete/update/hit_bit clear actions\n");
    }

    iterator_info->actions_bitmap = actions;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_iterator_action_payload_buffer_build(
    int unit,
    uint32 iterated_entry_handle_id,
    uint32 updated_entry_handle_id,
    uint8 has_result_type,
    int *nof_fields_to_update)
{
    int ii;
    int fields_to_update = 0;
    uint32 res_type_value = 0;
    dbal_entry_handle_t *iter_entry_handle;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, iterated_entry_handle_id, &iter_entry_handle));
    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, iterated_entry_handle_id, &iterator_info));
    if (has_result_type)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, iterated_entry_handle_id, DBAL_FIELD_RESULT_TYPE,
                                                            0, &res_type_value));
        dbal_entry_value_field32_set(unit, updated_entry_handle_id, DBAL_FIELD_RESULT_TYPE, 0, res_type_value);
    }
    for (ii = 0; ii < iterator_info->nof_actions; ii++)
    {
        if (iterator_info->actions_info[ii].action_type == DBAL_ITER_ACTION_UPDATE)
        {
            dbal_fields_e field_id_to_update = iterator_info->actions_info[ii].field_id;
            dbal_table_field_info_t field_info;
            int rv = _SHR_E_NONE;
            if (has_result_type)
            {
                rv = dbal_tables_field_info_get_no_err(unit, iter_entry_handle->table_id, field_id_to_update, FALSE,
                                                       res_type_value, 0, &field_info);
            }
            if (rv == _SHR_E_NONE)
            {
                dbal_entry_value_field_set(unit, updated_entry_handle_id, field_id_to_update,
                                           0, DBAL_POINTER_TYPE_ARR_UINT32,
                                           iterator_info->actions_info[ii].value, iterator_info->actions_info[ii].mask);
                fields_to_update++;
            }
        }
    }
    *nof_fields_to_update = fields_to_update;
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_iterator_action_result_info_update(
    int unit,
    uint32 updated_entry_handle_id,
    uint32 iterated_entry_handle_id)
{
    dbal_entry_handle_t *iter_entry_handle;
    dbal_entry_handle_t *updated_entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, updated_entry_handle_id, &updated_entry_handle));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, iterated_entry_handle_id, &iter_entry_handle));

    sal_memcpy(iter_entry_handle->phy_entry.payload, updated_entry_handle->phy_entry.payload,
               sizeof(iter_entry_handle->phy_entry.payload));
    sal_memcpy(iter_entry_handle->phy_entry.p_mask, updated_entry_handle->phy_entry.p_mask,
               sizeof(iter_entry_handle->phy_entry.p_mask));
    
    DBAL_ENTRY_HANDLE_USER_OUTPUT_INFO_COPY(*updated_entry_handle, *iter_entry_handle);

    iter_entry_handle->nof_result_fields = updated_entry_handle->nof_result_fields;
    iter_entry_handle->error_info = updated_entry_handle->error_info;

    sal_memcpy(iter_entry_handle->value_field_ids, updated_entry_handle->value_field_ids,
               sizeof(dbal_fields_e) * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_iterator_perform_action(
    int unit,
    uint32 entry_handle_id)
{
    dbal_iterator_info_t *iterator_info;
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle_id, &iterator_info));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    
    if (dbal_logical_table_is_mact(unit, entry_handle->table_id))
    {
        if (entry_handle->phy_entry.mdb_action_apply == DBAL_MDB_ACTION_APPLY_NONE)
        {
                        
            entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_ALL;
            SHR_EXIT();
        }
    }

    if (iterator_info->actions_bitmap & DBAL_ITER_ACTION_DELETE)
    {
        SHR_IF_ERR_EXIT(dbal_actions_access_entry_clear(unit, entry_handle));
    }
    if (iterator_info->actions_bitmap & DBAL_ITER_ACTION_UPDATE)
    {
        int nof_fields_to_update = 0;
        uint32 updated_entry_handle_id;
        dbal_entry_handle_t *update_entry_handle;

        SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &update_entry_handle));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, update_entry_handle->table_id, &updated_entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_action_payload_buffer_build
                        (unit, entry_handle_id, updated_entry_handle_id,
                         SHR_IS_BITSET(update_entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE),
                         &nof_fields_to_update));
        if (nof_fields_to_update > 0)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_action_result_info_update(unit, updated_entry_handle_id, entry_handle_id));
            if ((entry_handle->table->table_type == DBAL_TABLE_TYPE_DIRECT) ||
                (entry_handle->table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT))
            {
                SHR_IF_ERR_EXIT(dbal_entry_commit_normal(unit, entry_handle));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_commit_update(unit, entry_handle));
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_iterator_get_next_entry(
    int unit,
    uint32 entry_handle_id,
    int *is_end)
{
    dbal_entry_handle_t *entry_handle;
    dbal_iterator_info_t *iterator_info;
    uint8 continue_iterating = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));
    SHR_IF_ERR_EXIT(dbal_actions_access_iterator_get_next(unit, entry_handle_id, is_end, &continue_iterating));
    SHR_IF_ERR_EXIT(dbal_action_prints(unit, entry_handle, DBAL_GET_ALL_FIELDS, DBAL_ACTION_ITERATOR_GET));

    while (continue_iterating)
    {
        continue_iterating = FALSE;
        SHR_IF_ERR_EXIT(dbal_actions_access_iterator_get_next(unit, entry_handle_id, is_end, &continue_iterating));
        SHR_IF_ERR_EXIT(dbal_action_prints(unit, entry_handle, DBAL_GET_ALL_FIELDS, DBAL_ACTION_ITERATOR_GET));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_iterator_init(
    int unit,
    uint32 entry_handle_id,
    dbal_iterator_mode_e mode)
{
    dbal_entry_handle_t *entry_handle;
    dbal_actions_e action = DBAL_NOF_ACTIONS;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    SHR_IF_ERR_EXIT(dbal_entry_handle_clear_macro(unit, entry_handle->table_id, entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_action_skip_check(unit, entry_handle, &action));
    if (action == DBAL_ACTION_SKIP)
    {
        SHR_EXIT();
    }

    sal_memset(iterator_info, 0, sizeof(dbal_iterator_info_t));
        
    if (mode == DBAL_ITER_MODE_GET_NON_DEFAULT_PERMISSION_WRITE)
    {
        mode = DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT;
                
        if (entry_handle->table->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC)
        {
            int iter;
            int res_type;
            for (res_type = 0; res_type < entry_handle->table->nof_result_types; res_type++)
            {
                for (iter = 0; iter < entry_handle->table->multi_res_info[res_type].nof_result_fields; iter++)
                {
                    if ((entry_handle->table->multi_res_info[res_type].results_info[iter].permission ==
                         DBAL_PERMISSION_READONLY)
                        || (entry_handle->table->multi_res_info[res_type].results_info[iter].permission ==
                            DBAL_PERMISSION_TRIGGER))
                    {
                        mode = DBAL_ITER_MODE_GET_NON_DEFAULT_PERMISSION_WRITE;
                        break;
                    }
                }
                if (mode == DBAL_ITER_MODE_GET_NON_DEFAULT_PERMISSION_WRITE)
                {
                    break;
                }
            }
        }
    }

    iterator_info->mode = mode;
    iterator_info->is_end = FALSE;
    iterator_info->used_first_key = FALSE;
    iterator_info->nof_key_rules = 0;
    iterator_info->key_rules_info = NULL;
    iterator_info->is_init = TRUE;
    entry_handle->get_all_fields = 1;

    SHR_IF_ERR_EXIT(dbal_iterator_init_handle_info(unit, entry_handle));

    SHR_IF_ERR_EXIT(dbal_actions_access_iterator_init(unit, entry_handle));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_iterator_destroy_macro(
    int unit,
    uint32 entry_handle_id)
{
    dbal_entry_handle_t *entry_handle;
    int action_skip = 0;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &(entry_handle)));

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    SHR_IF_ERR_EXIT(dbal_iterator_common_validation(unit, entry_handle, &action_skip));
    if (action_skip)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dbal_actions_access_iterator_destroy(unit, entry_handle));

    if (iterator_info->nof_key_rules > 0)
    {
        SHR_FREE(iterator_info->key_rules_info);
    }

    if (iterator_info->nof_val_rules > 0)
    {
        SHR_FREE(iterator_info->val_rules_info);
    }

    if (iterator_info->nof_actions > 0)
    {
        SHR_FREE(iterator_info->actions_info);
    }
    sal_memset(iterator_info, 0x0, sizeof(dbal_iterator_info_t));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_iterator_entries_count_get(
    int unit,
    uint32 entry_handle_id,
    int *entries_counter)
{
    dbal_entry_handle_t *entry_handle;
    int action_skip = 0;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &(entry_handle)));
    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    SHR_IF_ERR_EXIT(dbal_iterator_common_validation(unit, entry_handle, &action_skip));
    if (action_skip)
    {
        SHR_EXIT();
    }

    *entries_counter = iterator_info->entries_counter;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_iterator_key_field_arr8_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    dbal_condition_types_e condition,
    uint8 *field_val,
    uint8 *field_mask)
{
    uint32 field_val_u32[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS] = { 0 };
    uint32 field_mask_u32[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS] = { 0 };
    dbal_entry_handle_t *entry_handle;
    uint32 *mask_ptr;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    SHR_IF_ERR_EXIT(dbal_fields_transform_arr8_to_arr32
                    (unit, entry_handle->table_id, field_id, TRUE, 0, 0, field_val, field_val_u32, NULL, NULL));
    if (field_mask)
    {
        SHR_IF_ERR_EXIT(dbal_fields_transform_arr8_to_arr32
                        (unit, entry_handle->table_id, field_id, TRUE, 0, 0, field_mask, field_mask_u32, NULL, NULL));
        mask_ptr = field_mask_u32;
    }
    else
    {
        mask_ptr = NULL;
    }
    SHR_IF_ERR_EXIT(dbal_iterator_key_rule_add(unit, entry_handle_id, field_id, condition, field_val_u32, mask_ptr));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_iterator_key_field_arr32_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    dbal_condition_types_e condition,
    uint32 *field_val,
    uint32 *field_mask)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_iterator_key_rule_add(unit, entry_handle_id, field_id, condition, field_val, field_mask));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_iterator_key_field32_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    dbal_condition_types_e condition,
    uint32 field_val,
    uint32 field_mask)
{
    uint32 field_val_u32[1];
    uint32 field_mask_u32[1];
    uint32 *field_mask_u32_p = NULL;

    SHR_FUNC_INIT_VARS(unit);

    field_val_u32[0] = field_val;
    field_mask_u32[0] = field_mask;

    if (field_mask)
    {
        field_mask_u32_p = field_mask_u32;
    }

    SHR_IF_ERR_EXIT(dbal_iterator_key_rule_add
                    (unit, entry_handle_id, field_id, condition, field_val_u32, field_mask_u32_p));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_iterator_value_field_arr8_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_idx,
    dbal_condition_types_e condition,
    uint8 *field_val,
    uint8 *field_mask)
{
    uint32 field_val_u32[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };
    uint32 field_mask_u32[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };
    uint32 *mask_ptr;
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    SHR_IF_ERR_EXIT(dbal_fields_transform_arr8_to_arr32
                    (unit, entry_handle->table_id, field_id, FALSE, entry_handle->cur_res_type, inst_idx, field_val,
                     field_val_u32, NULL, NULL));
    if (field_mask)
    {
        SHR_IF_ERR_EXIT(dbal_fields_transform_arr8_to_arr32
                        (unit, entry_handle->table_id, field_id, FALSE, entry_handle->cur_res_type, inst_idx,
                         field_mask, field_mask_u32, NULL, NULL));
        mask_ptr = field_mask_u32;
    }
    else
    {
        mask_ptr = NULL;
    }
    if (inst_idx == INST_SINGLE)
    {
        inst_idx = 0;
    }
    SHR_IF_ERR_EXIT(dbal_iterator_value_rule_add
                    (unit, entry_handle_id, field_id + inst_idx, condition, field_val_u32, mask_ptr));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_iterator_value_field_arr32_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_idx,
    dbal_condition_types_e condition,
    uint32 *field_val,
    uint32 *field_mask)
{
    SHR_FUNC_INIT_VARS(unit);

    if (inst_idx == INST_SINGLE)
    {
        inst_idx = 0;
    }

    SHR_IF_ERR_EXIT(dbal_iterator_value_rule_add
                    (unit, entry_handle_id, field_id + inst_idx, condition, field_val, field_mask));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_iterator_value_field32_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_idx,
    dbal_condition_types_e condition,
    uint32 field_val,
    uint32 field_mask)
{
    uint32 field_val_u32[1];
    uint32 field_mask_u32[1];
    uint32 *field_mask_u32_p = NULL;

    SHR_FUNC_INIT_VARS(unit);

    field_val_u32[0] = field_val;
    field_mask_u32[0] = field_mask;

    if (field_mask)
    {
        field_mask_u32_p = field_mask_u32;
    }

    if (inst_idx == INST_SINGLE)
    {
        inst_idx = 0;
    }

    SHR_IF_ERR_EXIT(dbal_iterator_value_rule_add
                    (unit, entry_handle_id, field_id + inst_idx, condition, field_val_u32, field_mask_u32_p));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_iterator_attribute_rule_add(
    int unit,
    uint32 entry_handle_id,
    uint32 attr_type,
    uint32 attr_val)
{
    dbal_entry_handle_t *entry_handle;
    int action_skip = 0;
    dbal_iterator_info_t *iterator_info;
    dbal_iterator_attribute_info_t *iterator_attrib_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));
    iterator_attrib_info = &iterator_info->attrib_info;

    SHR_IF_ERR_EXIT(dbal_iterator_common_validation(unit, entry_handle, &action_skip));
    if (action_skip)
    {
        SHR_EXIT();
    }

    switch (attr_type)
    {
        case DBAL_ENTRY_ATTR_PRIORITY:
            SHR_ERR_EXIT(_SHR_E_PARAM, "DBAL_ENTRY_ATTR_PRIORITY is not a valid rule");
            break;
        case DBAL_ENTRY_ATTR_AGE:
            if (!dbal_logical_table_is_mact(unit, entry_handle->table_id))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "DBAL_ENTRY_ATTR_AGE rule is valid for FWD_MACT only");
            }
            iterator_attrib_info->age_rule_valid = TRUE;
            iterator_attrib_info->age_rule_compare_value = attr_val;
            break;
        case (DBAL_ENTRY_ATTR_HIT_GET | DBAL_ENTRY_ATTR_HIT_PRIMARY):
        case (DBAL_ENTRY_ATTR_HIT_GET | DBAL_ENTRY_ATTR_HIT_SECONDARY):
        case (DBAL_ENTRY_ATTR_HIT_GET | DBAL_ENTRY_ATTR_HIT_PRIMARY | DBAL_ENTRY_ATTR_HIT_SECONDARY):
            
            if ((attr_val != DBAL_HITBIT_NOT_SET_RULE) && (attr_val != DBAL_HITBIT_SET_RULE))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Hit bit iterator rule does not support [%d] value\n", attr_val);
            }
            iterator_attrib_info->hit_bit_rule_valid = TRUE;
            iterator_attrib_info->hit_bit_rule_is_hit = attr_val;
            iterator_attrib_info->hit_bit_rule_type = attr_type;
            break;
        case (DBAL_ENTRY_ATTR_HIT_CLEAR | DBAL_ENTRY_ATTR_HIT_PRIMARY):
        case (DBAL_ENTRY_ATTR_HIT_CLEAR | DBAL_ENTRY_ATTR_HIT_SECONDARY):
        case (DBAL_ENTRY_ATTR_HIT_CLEAR | DBAL_ENTRY_ATTR_HIT_PRIMARY | DBAL_ENTRY_ATTR_HIT_SECONDARY):
            SHR_ERR_EXIT(_SHR_E_PARAM, "DBAL_ENTRY_ATTR_HIT_CLEAR is not supported");
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal attribute get type %d", attr_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_iterator_field_arr8_action_add(
    int unit,
    uint32 entry_handle_id,
    dbal_iterator_action_types_e action_type,
    dbal_fields_e field_id,
    int inst_idx,
    uint8 *field_val,
    uint8 *field_mask)
{
    uint32 field_val_u32[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS] = { 0 };
    uint32 field_mask_u32[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS] = { 0 };
    uint32 *mask_ptr;
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    if (!dbal_iterator_field_action_check(unit, action_type))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "action %d is invalid 'entry action'\n", action_type);
    }

    SHR_IF_ERR_EXIT(dbal_fields_transform_arr8_to_arr32
                    (unit, entry_handle->table_id, field_id, FALSE, entry_handle->cur_res_type, inst_idx, field_val,
                     field_val_u32, NULL, NULL));
    if (field_mask)
    {
        SHR_IF_ERR_EXIT(dbal_fields_transform_arr8_to_arr32
                        (unit, entry_handle->table_id, field_id, FALSE, entry_handle->cur_res_type, inst_idx,
                         field_mask, field_mask_u32, NULL, NULL));
        mask_ptr = field_mask_u32;
    }
    else
    {
        mask_ptr = NULL;
    }

    if (inst_idx == INST_SINGLE)
    {
        inst_idx = 0;
    }

    SHR_IF_ERR_EXIT(dbal_iterator_action_add
                    (unit, entry_handle, field_id + inst_idx, action_type, field_val_u32, mask_ptr));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_iterator_field_arr32_action_add(
    int unit,
    uint32 entry_handle_id,
    dbal_iterator_action_types_e action_type,
    dbal_fields_e field_id,
    int inst_idx,
    uint32 *field_val,
    uint32 *field_mask)
{
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    if (!dbal_iterator_field_action_check(unit, action_type))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "action %d is invalid 'field action'\n", action_type);
    }

    if (inst_idx == INST_SINGLE)
    {
        inst_idx = 0;
    }

    SHR_IF_ERR_EXIT(dbal_iterator_action_add
                    (unit, entry_handle, field_id + inst_idx, action_type, field_val, field_mask));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_iterator_attribute_action_add(
    int unit,
    uint32 entry_handle_id,
    uint32 attr_type)
{
    dbal_entry_handle_t *entry_handle;
    int action_skip = 0;
    dbal_iterator_info_t *iterator_info;
    dbal_iterator_attribute_info_t *iterator_attrib_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &(entry_handle)));
    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));
    iterator_attrib_info = &iterator_info->attrib_info;

    SHR_IF_ERR_EXIT(dbal_iterator_common_validation(unit, entry_handle, &action_skip));
    if (action_skip)
    {
        SHR_EXIT();
    }

    switch (attr_type)
    {
        case DBAL_ENTRY_ATTR_PRIORITY:
            SHR_ERR_EXIT(_SHR_E_PARAM, "DBAL_ENTRY_ATTR_PRIORITY is not a valid action");
            break;
        case DBAL_ENTRY_ATTR_AGE:
            SHR_ERR_EXIT(_SHR_E_PARAM, "DBAL_ENTRY_ATTR_AGE is not a valid action");
            break;

        case (DBAL_ENTRY_ATTR_HIT_GET):
        case (DBAL_ENTRY_ATTR_HIT_GET | DBAL_ENTRY_ATTR_HIT_PRIMARY):
        case (DBAL_ENTRY_ATTR_HIT_GET | DBAL_ENTRY_ATTR_HIT_SECONDARY):
        case (DBAL_ENTRY_ATTR_HIT_GET | DBAL_ENTRY_ATTR_HIT_PRIMARY | DBAL_ENTRY_ATTR_HIT_SECONDARY):
            iterator_attrib_info->hit_bit_action_get = TRUE;
            iterator_attrib_info->hit_bit_action_get_type = attr_type;
            SHR_IF_ERR_EXIT(dbal_iterator_action_add(unit, entry_handle, DBAL_FIELD_EMPTY, DBAL_ITER_ACTION_HIT_GET,
                                                     NULL, NULL));
            break;
        case (DBAL_ENTRY_ATTR_HIT_CLEAR):
        case (DBAL_ENTRY_ATTR_HIT_CLEAR | DBAL_ENTRY_ATTR_HIT_PRIMARY):
        case (DBAL_ENTRY_ATTR_HIT_CLEAR | DBAL_ENTRY_ATTR_HIT_SECONDARY):
        case (DBAL_ENTRY_ATTR_HIT_CLEAR | DBAL_ENTRY_ATTR_HIT_PRIMARY | DBAL_ENTRY_ATTR_HIT_SECONDARY):
            iterator_attrib_info->hit_bit_action_clear = TRUE;
            iterator_attrib_info->hit_bit_action_clear_type = attr_type;
            SHR_IF_ERR_EXIT(dbal_iterator_action_add
                            (unit, entry_handle, DBAL_FIELD_EMPTY, DBAL_ITER_ACTION_HIT_CLEAR, NULL, NULL));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal attribute get type %d", attr_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_iterator_entry_action_add(
    int unit,
    uint32 entry_handle_id,
    dbal_iterator_action_types_e action_type)
{
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &(entry_handle)));

    if (!dbal_iterator_entry_action_check(unit, action_type))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "action %d is invalid 'entry action'\n", action_type);
    }

    SHR_IF_ERR_EXIT(dbal_iterator_action_add(unit, entry_handle, DBAL_FIELD_EMPTY, action_type, NULL, NULL));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_iterator_action_commit(
    int unit,
    uint32 entry_handle_id)
{
    int is_end = FALSE;
    int action_skip = 0;
    dbal_entry_handle_t *entry_handle;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &(entry_handle)));
    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle_id, &iterator_info));

    SHR_IF_ERR_EXIT(dbal_iterator_common_validation(unit, entry_handle, &action_skip));
    if (action_skip)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dbal_iterator_action_validate(unit, entry_handle));

    if (iterator_info->actions_bitmap & DBAL_ITER_ACTION_GET)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "iterator action commit does not support GET action. Use iterator_get_next\n");
    }

    if (iterator_info->mode == DBAL_ITER_MODE_GET_ENTRIES_FROM_DMA_NO_TRIGGER)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "iterator action commit is invalid for iterator mode GET_ENTRIES_FROM_DMA_NO_TRIGGER\n");
    }

    
    SHR_IF_ERR_EXIT(dbal_iterator_get_next_entry(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_iterator_perform_action(unit, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next_entry(unit, entry_handle_id, &is_end));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_iterator_get_next(
    int unit,
    uint32 entry_handle_id,
    int *is_end)
{
    dbal_entry_handle_t *entry_handle;
    int action_skip = 0;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_DBAL_START(unit, DNX_INIT_TIME_ANALYZER_DBAL_ITER);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &(entry_handle)));
    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle_id, &iterator_info));

    SHR_IF_ERR_EXIT(dbal_iterator_common_validation(unit, entry_handle, &action_skip));
    if (action_skip)
    {
        SHR_EXIT();
    }

    
    if (!iterator_info->used_first_key)
    {
        SHR_IF_ERR_EXIT(dbal_iterator_action_add
                        (unit, entry_handle, DBAL_FIELD_EMPTY, DBAL_ITER_ACTION_GET, NULL, NULL));
        SHR_IF_ERR_EXIT(dbal_iterator_action_validate(unit, entry_handle));
    }

    if (iterator_info->mode == DBAL_ITER_MODE_GET_ENTRIES_FROM_DMA_NO_TRIGGER)
    {
        if ((iterator_info->nof_actions != 1) || (iterator_info->actions_bitmap != DBAL_ITER_ACTION_GET))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "No actions are allowed in mode: GET_ENTRIES_FROM_DMA_NO_TRIGGER. "
                         "nof_Actions = %d, actions_bitmap = 0x%08x\n", iterator_info->nof_actions,
                         iterator_info->actions_bitmap);
        }

        if ((iterator_info->nof_key_rules != 0) || (iterator_info->nof_val_rules != 0))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "No rules are allowed in mode: GET_ENTRIES_FROM_DMA_NO_TRIGGER. "
                         "nof_key_rules = %d, not_value_reules = %d\n", iterator_info->nof_key_rules,
                         iterator_info->nof_val_rules);
        }
        if (iterator_info->attrib_info.hit_bit_rule_valid || iterator_info->attrib_info.age_rule_valid)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "No attrib. rules are allowed in mode: GET_ENTRIES_FROM_DMA_NO_TRIGGER. "
                         "hit_bit_rule_valid = %d, hit_bit_rule_valid = %d\n",
                         iterator_info->attrib_info.hit_bit_rule_valid, iterator_info->attrib_info.age_rule_valid);
        }
    }

    SHR_IF_ERR_EXIT(dbal_iterator_get_next_entry(unit, entry_handle_id, is_end));

    
    if (!(*is_end) && (iterator_info->actions_bitmap & (~DBAL_ITER_ACTION_GET)))
    {
        SHR_IF_ERR_EXIT(dbal_iterator_perform_action(unit, entry_handle_id));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    DNX_INIT_TIME_ANALYZER_DBAL_STOP(unit, DNX_INIT_TIME_ANALYZER_DBAL_ITER);
    SHR_FUNC_EXIT;
}
