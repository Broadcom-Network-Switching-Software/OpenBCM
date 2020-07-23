/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "dbal_hl_access.h"

static shr_error_e
dbal_hl_tcam_arr_offset_and_block_get_and_verify(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 *array_offset,
    int *block,
    int *num_of_blocks)
{
    int iter;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_hl_l2p_info_t access_info =
        table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY];
    soc_mem_t memory = access_info.l2p_fields_info[DBAL_HL_TCAM_ACCESS_TYPE_KEY].memory[0];

    SHR_FUNC_INIT_VARS(unit);

    
    for (iter = 0; iter < DBAL_NOF_HL_TCAM_ACCESS_TYPES; ++iter)
    {
        dbal_hl_l2p_field_info_t curr_l2p_info = access_info.l2p_fields_info[iter];
        uint32 new_array_offset;
        int new_block;
        int new_block_num;

        *array_offset = 0;
        SHR_IF_ERR_EXIT(dbal_hl_offset_calculate
                        (unit, entry_handle, INVALIDf, 0, &(curr_l2p_info.array_offset_info), &new_array_offset));
        if (iter == 0)
        {
            *array_offset = new_array_offset;
        }
        else if (new_array_offset != *array_offset)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Table %s (HL TCAM) - All array offsets must be identical\n", table->table_name);
        }

        SHR_IF_ERR_EXIT(dbal_hl_mem_block_calc(unit, entry_handle, entry_handle->core_id, memory,
                                               &(curr_l2p_info.block_index_info), curr_l2p_info.field_id,
                                               &new_block, &new_block_num));
        if (iter == 0)
        {
            *block = new_block;
            *num_of_blocks = new_block_num;
        }
        else if (new_block != *block)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Table %s (HL TCAM) - All blocks must be identical\n", table->table_name);
        }
        else if (new_block_num != *num_of_blocks)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Table %s (HL TCAM) - All num_of_blocks must be identical\n", table->table_name);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_tcam_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int iter, block = MEM_BLOCK_ANY, num_of_blocks;
    uint32 array_offset;
    soc_mem_t memory;
    uint32 data[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 };
    dbal_logical_table_t *table = entry_handle->table;
    dbal_hl_l2p_info_t access_info =
        table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY];

    SHR_FUNC_INIT_VARS(unit);

    memory = access_info.l2p_fields_info[DBAL_HL_TCAM_ACCESS_TYPE_KEY].memory[0];

    
    SHR_IF_ERR_EXIT(dbal_hl_tcam_arr_offset_and_block_get_and_verify
                    (unit, entry_handle, &array_offset, &block, &num_of_blocks));

                
    for (iter = block; iter < block + num_of_blocks; ++iter)
    {
        SHR_IF_ERR_EXIT(sand_fill_partial_table_with_entry
                        (unit, memory, array_offset, array_offset, iter, 0, table->max_capacity - 1, data));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_tcam_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int iter, data_size, block = MEM_BLOCK_ANY, num_of_blocks;
    uint32 array_offset;
    soc_mem_t memory;
    uint32 *data_target;
    uint32 data[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 };
    soc_field_info_t *fieldinfo;
    soc_mem_info_t *meminfo;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_hl_l2p_info_t access_info;
    dbal_hl_l2p_field_info_t *curr_l2p_info;
    uint32 group_offset;
    int logger_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_HL_LOGGER_STATUS_UPDATE;

    if (entry_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Ilegal result type for entry table %s\n", table->table_name);
    }

    access_info = table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY];
    curr_l2p_info = &(access_info.l2p_fields_info[DBAL_HL_TCAM_ACCESS_TYPE_KEY]);

    SHR_IF_ERR_EXIT(dbal_hl_offset_calculate(unit, entry_handle, curr_l2p_info->field_id,
                                             -1, &(curr_l2p_info->group_offset_info), &group_offset));

    memory = (group_offset == -1) ? curr_l2p_info->memory[0] : curr_l2p_info->memory[group_offset];

    
    SHR_IF_ERR_EXIT(dbal_hl_tcam_arr_offset_and_block_get_and_verify
                    (unit, entry_handle, &array_offset, &block, &num_of_blocks));

    
    DBAL_MEM_READ_LOG(memory, entry_handle->phy_entry.entry_hw_id, block, array_offset);
    SHR_IF_ERR_EXIT(soc_mem_array_read(unit, memory, array_offset, block, entry_handle->phy_entry.entry_hw_id, data));
    DBAL_ACTION_PERFORMED_LOG;
    DBAL_DATA_LOG(data, 1);

    meminfo = &SOC_MEM_INFO(unit, memory);

    
    for (iter = 0; iter < DBAL_NOF_HL_TCAM_ACCESS_TYPES; ++iter)
    {
        uint32 offset = 0;
        dbal_hl_tcam_access_type_e access_type_id = access_info.l2p_fields_info[iter].hl_tcam_access_type;

        
        switch (access_type_id)
        {
            case (DBAL_HL_TCAM_ACCESS_TYPE_KEY):
                data_target = entry_handle->phy_entry.key;
                data_size = table->key_size;
                break;

            case (DBAL_HL_TCAM_ACCESS_TYPE_KEY_MASK):
                data_target = entry_handle->phy_entry.k_mask;
                data_size = table->key_size;
                break;

            case (DBAL_HL_TCAM_ACCESS_TYPE_RESULT):
                data_target = entry_handle->phy_entry.payload;
                data_size = table->max_payload_size;
                break;

            default:
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "hl_tcam_access_type was not set to a valid value in table %s, l2p_fields_info[%d] \n",
                             table->table_name, iter);
        }

        if (access_info.l2p_fields_info[iter].hw_field != INVALIDf)
        {
            
            SOC_FIND_FIELD(access_info.l2p_fields_info[iter].hw_field, meminfo->fields, meminfo->nFields, fieldinfo);
            offset = fieldinfo->bp;
            LOG_INFO_EX(BSL_LOG_MODULE, "%s offset decided by field %-30s, field offset %d size %d\n",
                        dbal_hl_tcam_access_type_to_string(unit, access_type_id),
                        SOC_FIELD_NAME(unit, access_info.l2p_fields_info[iter].hw_field), offset, data_size);
        }
        else
        {
            
            SHR_IF_ERR_EXIT(dbal_hl_offset_calculate
                            (unit, entry_handle, INVALIDf, 0, &(access_info.l2p_fields_info[iter].data_offset_info),
                             &offset));
            LOG_INFO_EX(BSL_LOG_MODULE, "%s offset decided, offset %d size %d %s\n",
                        dbal_hl_tcam_access_type_to_string(unit, access_type_id), offset, data_size, EMPTY);
        }
        
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(data, offset, data_size, data_target));
    }

exit:
    SHR_FUNC_EXIT;
}

#if 0



static shr_error_e
dbal_hl_tcam_merge_buffers(
    int unit,
    uint32 *prim_buffer,
    uint32 *mask,
    uint32 *secondary_buffer,
    uint32 num_of_uint32)
{
    uint32 converted_mask[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(utilex_bitstream_and(prim_buffer, mask, num_of_uint32));

    
    sal_memcpy(converted_mask, mask, sizeof(converted_mask));
    SHR_IF_ERR_EXIT(utilex_bitstream_not(converted_mask, num_of_uint32));
    SHR_IF_ERR_EXIT(utilex_bitstream_and(secondary_buffer, converted_mask, num_of_uint32));

    
    SHR_IF_ERR_EXIT(utilex_bitstream_or(prim_buffer, secondary_buffer, num_of_uint32));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_hl_tcam_read_and_merge_entry(
    int unit,
    dbal_entry_handle_t * prim_entry)
{
    int field_pos, num_of_uint32;
    int res_type = prim_entry->cur_res_type;
    dbal_entry_handle_t get_entry;
    dbal_entry_handle_t *secondary_entry = &get_entry;

    SHR_FUNC_INIT_VARS(unit);

    
    sal_memcpy(secondary_entry, prim_entry, sizeof(dbal_entry_handle_t));
    secondary_entry->get_all_fields = 1;

    
    SHR_IF_ERR_EXIT(dbal_hl_tcam_entry_get(unit, secondary_entry));

    

    
    for (field_pos = 0; field_pos < prim_entry->table->multi_res_info[res_type].nof_result_fields; field_pos++)
    {
        if (prim_entry->value_field_ids[field_pos] == DBAL_FIELD_EMPTY)
        {
            prim_entry->value_field_ids[field_pos] =
                prim_entry->table->multi_res_info[res_type].results_info[field_pos].field_id;
        }
    }

    
    num_of_uint32 = (prim_entry->table->max_payload_size + 31) / 32;
    SHR_IF_ERR_EXIT(dbal_hl_tcam_merge_buffers
                    (unit, prim_entry->phy_entry.payload, prim_entry->phy_entry.p_mask,
                     secondary_entry->phy_entry.payload, num_of_uint32));

    
    SHR_IF_ERR_EXIT(utilex_bitstream_fill(prim_entry->phy_entry.p_mask, num_of_uint32));

exit:
    SHR_FUNC_EXIT;
}
#endif

shr_error_e
dbal_hl_tcam_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int iter, data_size, block = MEM_BLOCK_ANY, num_of_blocks;
    uint32 array_offset = 0, group_offset;
    soc_mem_t memory;
    uint32 data[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 }, *data_to_copy;
    soc_field_info_t *fieldinfo;
    soc_mem_info_t *meminfo;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_hl_l2p_info_t access_info =
        table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY];

    dbal_hl_l2p_field_info_t *curr_l2p_info = &(access_info.l2p_fields_info[DBAL_HL_TCAM_ACCESS_TYPE_KEY]);
    int nof_group_elements = 1, ii;
    int logger_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_HL_LOGGER_STATUS_UPDATE;

    
    SHR_IF_ERR_EXIT(dbal_hl_offset_calculate(unit, entry_handle, curr_l2p_info->field_id,
                                             -1, &(curr_l2p_info->group_offset_info), &group_offset));

    memory = (group_offset == -1) ? curr_l2p_info->memory[0] : curr_l2p_info->memory[group_offset];

    meminfo = &SOC_MEM_INFO(unit, memory);

    if (group_offset != -1)
    {
        nof_group_elements = 1;
    }
    else
    {
        if (curr_l2p_info->hw_entity_group_id != DBAL_HW_ENTITY_GROUP_EMPTY)
        {
            nof_group_elements = DBAL_MAX_NUMBER_OF_HW_ELEMENTS;
        }
    }

    
    SHR_IF_ERR_EXIT(dbal_hl_tcam_arr_offset_and_block_get_and_verify
                    (unit, entry_handle, &array_offset, &block, &num_of_blocks));

    
    for (iter = 0; iter < DBAL_NOF_HL_TCAM_ACCESS_TYPES; ++iter)
    {
        uint32 offset = 0;
        dbal_hl_tcam_access_type_e access_type_id = access_info.l2p_fields_info[iter].hl_tcam_access_type;

        
        switch (access_type_id)
        {
            case (DBAL_HL_TCAM_ACCESS_TYPE_KEY):
                data_to_copy = entry_handle->phy_entry.key;
                data_size = table->key_size;
                break;

            case (DBAL_HL_TCAM_ACCESS_TYPE_KEY_MASK):
                data_to_copy = entry_handle->phy_entry.k_mask;
                data_size = table->key_size;
                break;

            case (DBAL_HL_TCAM_ACCESS_TYPE_RESULT):
                data_to_copy = entry_handle->phy_entry.payload;
                data_size = table->max_payload_size;
                break;

            default:
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "hl_tcam_access_type was not set to a valid value in table %s, l2p_fields_info[%d] \n",
                             table->table_name, iter);
        }

        if (access_info.l2p_fields_info[iter].hw_field != INVALIDf)
        {
            
            SOC_FIND_FIELD(access_info.l2p_fields_info[iter].hw_field, meminfo->fields, meminfo->nFields, fieldinfo);
            offset = fieldinfo->bp;
            LOG_INFO_EX(BSL_LOG_MODULE, "%s offset decided by field %-30s, field offset %d size %d\n",
                        dbal_hl_tcam_access_type_to_string(unit, access_type_id), SOC_FIELD_NAME(unit,
                                                                                                 access_info.l2p_fields_info
                                                                                                 [iter].hw_field),
                        offset, data_size);
            DBAL_DATA_LOG(data_to_copy, 1);

        }
        else
        {
            
            SHR_IF_ERR_EXIT(dbal_hl_offset_calculate
                            (unit, entry_handle, INVALIDf, 0, &(access_info.l2p_fields_info[iter].data_offset_info),
                             &offset));
            LOG_INFO_EX(BSL_LOG_MODULE, "%s offset decided, offset %d size %d %s\n",
                        dbal_hl_tcam_access_type_to_string(unit, access_type_id), offset, data_size, EMPTY);
        }

        
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(data_to_copy, offset, data_size, data));
    }

    
    for (ii = 0; ii < nof_group_elements; ii++)
    {
        memory = curr_l2p_info->memory[ii];
        if (memory == INVALIDm)
        {
            break;
        }
        for (iter = block; iter < block + num_of_blocks; ++iter)
        {
            DBAL_DATA_LOG(data, 1);
            DBAL_MEM_WRITE_LOG(memory, entry_handle->phy_entry.entry_hw_id, iter, array_offset);
            SHR_IF_ERR_EXIT(soc_mem_array_write
                            (unit, memory, array_offset, iter, entry_handle->phy_entry.entry_hw_id, data));
            DBAL_ACTION_PERFORMED_LOG;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_tcam_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int rv;
    uint8 entry_found = FALSE;
    dbal_key_value_validity_e is_valid_entry = DBAL_KEY_IS_INVALID;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    while ((!entry_found) && (!iterator_info->is_end))
    {
        if (iterator_info->used_first_key)
        {
            
            entry_handle->phy_entry.entry_hw_id++;

            
            if (entry_handle->phy_entry.entry_hw_id == entry_handle->table->max_capacity)
            {
                break;
            }
        }
        iterator_info->used_first_key = TRUE;
        is_valid_entry = DBAL_KEY_IS_VALID;
        
        
        if (is_valid_entry == DBAL_KEY_IS_VALID)
        {
            
            sal_memset(entry_handle->phy_entry.k_mask, 0, DBAL_PHYSICAL_KEY_SIZE_IN_BYTES);
            sal_memset(entry_handle->phy_entry.payload, 0, DBAL_PHYSICAL_KEY_SIZE_IN_BYTES);
            rv = dbal_hl_entry_get(unit, entry_handle);
            if (rv == _SHR_E_NONE)
            {
                int is_default;
                
                SHR_IF_ERR_EXIT(dbal_hl_is_entry_default(unit, entry_handle, &is_default));
                if ((!is_default))
                {
                    entry_found = TRUE;
                }
            }
            else if (rv != _SHR_E_NOT_FOUND)
            {
                SHR_ERR_EXIT(rv, "Error from HL TCAM entry get");
            }
        }
    }

    if (entry_found == FALSE)
    {
        iterator_info->is_end = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}
