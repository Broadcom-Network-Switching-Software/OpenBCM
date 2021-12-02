/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 */

#include "dbal_hl_access.h"

#define HL_TCAM_VALID_BITS_FULL 0x3

#define HL_TCAM_VALID_BITS_HALF_FIRST 0x1

#define HL_TCAM_VALID_BITS_HALF_SECOND 0x2

#define HL_TCAM_VALID_BITS_NONE 0x0

#define HL_TCAM_VALID_BITS_FIELD CMD_VALIDf

static shr_error_e
dbal_hl_tcam_arr_offset_and_block_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_hl_l2p_field_info_t * curr_l2p_info,
    uint32 *array_offset,
    int *block,
    int *num_of_blocks)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_hl_offset_calculate
                    (unit, entry_handle, 0, 0, &(curr_l2p_info->array_offset_info), array_offset));

    SHR_IF_ERR_EXIT(dbal_hl_mem_block_calc(unit, entry_handle, entry_handle->core_id, curr_l2p_info->memory[0],
                                           &(curr_l2p_info->block_index_info), curr_l2p_info, block, num_of_blocks));

exit:
    SHR_FUNC_EXIT;
}

int
dbal_hl_tcam_is_table_supports_half_entry(
    int unit,
    dbal_tables_e table_id)
{
    return FALSE;
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
    dbal_hl_l2p_info_t *access_info =
        &table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY];
    dbal_hl_l2p_field_info_t *l2p_field_info;

    SHR_FUNC_INIT_VARS(unit);

    for (int ii = 0; ii < DBAL_NOF_HL_TCAM_ACCESS_TYPES; ++ii)
    {
        l2p_field_info = &access_info->l2p_fields_info[ii];
        memory = access_info->l2p_fields_info[ii].memory[0];

        SHR_IF_ERR_EXIT(dbal_hl_tcam_arr_offset_and_block_get
                        (unit, entry_handle, l2p_field_info, &array_offset, &block, &num_of_blocks));

        for (iter = block; iter < block + num_of_blocks; ++iter)
        {
            SHR_IF_ERR_EXIT(sand_fill_partial_table_with_entry
                            (unit, memory, array_offset, array_offset, iter, 0, SOC_MEM_INFO(unit, memory).index_max,
                             data));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_entry_offset_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_hl_l2p_field_info_t * curr_l2p_info,
    uint32 *mem_entry_offset)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(mem_entry_offset, _SHR_E_PARAM, "entry_offset");

    if (curr_l2p_info->entry_offset_info.formula != NULL)
    {
        SHR_IF_ERR_EXIT(dbal_hl_offset_calculate(unit, entry_handle, curr_l2p_info,
                                                 entry_handle->phy_entry.entry_hw_id,
                                                 &(curr_l2p_info->entry_offset_info), mem_entry_offset));
    }
    else
    {
        (*mem_entry_offset) = entry_handle->phy_entry.entry_hw_id;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_hl_tcam_access_info_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_hl_l2p_info_t * access_info,
    dbal_hl_tcam_access_type_e access_type,
    dbal_hl_l2p_field_info_t ** l2p_field_info,
    soc_mem_t * memory,
    uint32 *entry_offset,
    int *nof_group_element)
{
    uint32 group_offset;

    SHR_FUNC_INIT_VARS(unit);

    *l2p_field_info = &(access_info->l2p_fields_info[access_type]);
    SHR_IF_ERR_EXIT(dbal_hl_offset_calculate(unit, entry_handle, *l2p_field_info,
                                             -1, &((*l2p_field_info)->group_offset_info), &group_offset));

    if ((nof_group_element != NULL) && (group_offset == -1) &&
        ((*l2p_field_info)->hw_entity_group_id != DBAL_HW_ENTITY_GROUP_EMPTY))
    {
        *nof_group_element = DBAL_MAX_NUMBER_OF_HW_ELEMENTS;
    }

    if (group_offset == -1)
    {
        group_offset = 0;
    }
    *memory = (*l2p_field_info)->memory[group_offset];
    SHR_IF_ERR_EXIT(dbal_entry_offset_get(unit, entry_handle, *l2p_field_info, entry_offset));

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
    soc_mem_t memory, prev_memory;
    uint32 *data_target;
    uint32 data[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 };
    soc_field_info_t *fieldinfo;
    soc_mem_info_t *meminfo;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_hl_l2p_info_t *access_info;
    dbal_hl_l2p_field_info_t *curr_l2p_info = NULL, *prev_l2p_info = NULL;
    int logger_enable = 0;
    uint32 entry_offset, prev_entry_offset;
    uint32 table_key_size = table->key_size;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_HL_LOGGER_STATUS_UPDATE;

    if (entry_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal result type for entry table %s\n", table->table_name);
    }

    if (table->core_mode == DBAL_CORE_MODE_DPC)
    {
        table_key_size -= DBAL_CORE_SIZE_IN_BITS;
    }

    access_info = &table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY];

    for (iter = 0; iter < DBAL_NOF_HL_TCAM_ACCESS_TYPES; ++iter)
    {
        uint32 offset = 0;
        dbal_hl_tcam_access_type_e access_type_id;

        SHR_IF_ERR_EXIT(dbal_hl_tcam_access_info_get(unit, entry_handle, access_info, iter,
                                                     &curr_l2p_info, &memory, &entry_offset, NULL));

        if (iter > DBAL_HL_TCAM_ACCESS_TYPE_KEY)
        {
            SHR_IF_ERR_EXIT(dbal_hl_tcam_access_info_get(unit, entry_handle, access_info, iter - 1,
                                                         &prev_l2p_info, &prev_memory, &prev_entry_offset, NULL));
        }

        if ((iter == DBAL_HL_TCAM_ACCESS_TYPE_KEY) || (prev_memory != memory) || (prev_entry_offset != entry_offset))
        {

            SHR_IF_ERR_EXIT(dbal_hl_tcam_arr_offset_and_block_get(unit, entry_handle, curr_l2p_info,
                                                                  &array_offset, &block, &num_of_blocks));
            DBAL_MEM_READ_LOG(memory, entry_handle->phy_entry.entry_hw_id, block, array_offset);
            SHR_IF_ERR_EXIT(soc_mem_array_read(unit, memory, array_offset, block, entry_offset, data));
            DBAL_ACTION_PERFORMED_LOG;
            DBAL_DATA_LOG(data, 1);
        }

        access_type_id = curr_l2p_info->hl_tcam_access_type;

        switch (access_type_id)
        {
            case (DBAL_HL_TCAM_ACCESS_TYPE_KEY):
                data_target = entry_handle->phy_entry.key;
                data_size = table_key_size;
                break;

            case (DBAL_HL_TCAM_ACCESS_TYPE_KEY_MASK):
                data_target = entry_handle->phy_entry.k_mask;
                data_size = table_key_size;
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

        if (access_info->l2p_fields_info[iter].hw_field != INVALIDf)
        {

            meminfo = &SOC_MEM_INFO(unit, memory);
            SOC_FIND_FIELD(access_info->l2p_fields_info[iter].hw_field, meminfo->fields, meminfo->nFields, fieldinfo);
            if (fieldinfo == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "fieldinfo is NULL. Illegal.\r\n");
            }
            offset = fieldinfo->bp;
            LOG_INFO_EX(BSL_LOG_MODULE, "%s offset decided by field %-30s, field offset %d size %d\n",
                        dbal_hl_tcam_access_type_to_string(unit, access_type_id),
                        SOC_FIELD_NAME(unit, access_info->l2p_fields_info[iter].hw_field), offset, data_size);
        }
        else
        {

            SHR_IF_ERR_EXIT(dbal_hl_offset_calculate
                            (unit, entry_handle, 0, 0, &(access_info->l2p_fields_info[iter].data_offset_info),
                             &offset));
            LOG_INFO_EX(BSL_LOG_MODULE, "%s offset decided, offset %d size %d %s\n",
                        dbal_hl_tcam_access_type_to_string(unit, access_type_id), offset, data_size, EMPTY);
        }

        if (dbal_hl_tcam_is_table_supports_half_entry(unit, entry_handle->table_id))
        {
            if ((entry_handle->phy_entry.entry_hw_id % 2) != 0)
            {
                offset += data_size;
            }
        }

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(data, offset, data_size, data_target));

        if (curr_l2p_info->encode_info.encode_mode != DBAL_VALUE_FIELD_ENCODE_NONE)
        {
            SHR_IF_ERR_EXIT(dbal_hl_access_decode(unit, curr_l2p_info->encode_info.encode_mode,
                                                  curr_l2p_info->encode_info.input_param,
                                                  data_target, data_target, curr_l2p_info->nof_bits_in_interface));
        }

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
    uint32 array_offset = 0;
    soc_mem_t memory, next_memory;
    uint32 data[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 }, *data_to_copy;
    soc_field_info_t *fieldinfo;
    soc_mem_info_t *meminfo;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_hl_l2p_info_t *access_info =
        &table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY];
    dbal_hl_l2p_field_info_t *curr_l2p_info = NULL, *next_l2p_info = NULL;
    int nof_group_elements = 1;
    int logger_enable = 0;
    uint32 entry_offset, next_entry_offset;
    uint32 offset = 0;
    dbal_hl_tcam_access_type_e access_type_id;
    uint8 previously_written = 0;
    uint32 table_key_size = table->key_size;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_HL_LOGGER_STATUS_UPDATE;

    if (table->core_mode == DBAL_CORE_MODE_DPC)
    {
        table_key_size -= DBAL_CORE_SIZE_IN_BITS;
    }

    for (iter = DBAL_HL_TCAM_ACCESS_TYPE_RESULT; iter >= 0; --iter)
    {
        SHR_IF_ERR_EXIT(dbal_hl_tcam_access_info_get(unit, entry_handle, access_info, iter,
                                                     &curr_l2p_info, &memory, &entry_offset, &nof_group_elements));

        if (iter > 0)
        {
            SHR_IF_ERR_EXIT(dbal_hl_tcam_access_info_get(unit, entry_handle, access_info, iter - 1,
                                                         &next_l2p_info, &next_memory, &next_entry_offset, NULL));
        }

        access_type_id = curr_l2p_info->hl_tcam_access_type;

        switch (access_type_id)
        {
            case DBAL_HL_TCAM_ACCESS_TYPE_KEY:
                data_size = table_key_size;
                data_to_copy = entry_handle->phy_entry.key;
                break;

            case DBAL_HL_TCAM_ACCESS_TYPE_KEY_MASK:
                data_size = table_key_size;
                data_to_copy = entry_handle->phy_entry.k_mask;
                break;

            case DBAL_HL_TCAM_ACCESS_TYPE_RESULT:
                data_to_copy = entry_handle->phy_entry.payload;
                data_size = table->max_payload_size;
                break;

            default:
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "hl_tcam_access_type was not set to a valid value in table %s, l2p_fields_info[%d]\n",
                             table->table_name, iter);
        }

        if (curr_l2p_info->encode_info.encode_mode != DBAL_VALUE_FIELD_ENCODE_NONE)
        {
            SHR_IF_ERR_EXIT(dbal_hl_access_encode(unit, curr_l2p_info->encode_info.encode_mode,
                                                  curr_l2p_info->encode_info.input_param,
                                                  data_to_copy, data_to_copy, curr_l2p_info->nof_bits_in_interface));
        }

        meminfo = &SOC_MEM_INFO(unit, memory);
        if (curr_l2p_info->hw_field != INVALIDf)
        {

            SOC_FIND_FIELD(curr_l2p_info->hw_field, meminfo->fields, meminfo->nFields, fieldinfo);
            if (fieldinfo != NULL)
            {
                offset = fieldinfo->bp;
                LOG_INFO_EX(BSL_LOG_MODULE, "%s offset decided by field %-30s, field offset %d size %d\n",
                            dbal_hl_tcam_access_type_to_string(unit, access_type_id),
                            SOC_FIELD_NAME(unit, curr_l2p_info->hw_field), offset, data_size);
                DBAL_DATA_LOG(data_to_copy, 1);
            }

        }
        else
        {

            SHR_IF_ERR_EXIT(dbal_hl_offset_calculate
                            (unit, entry_handle, 0, 0, &(curr_l2p_info->data_offset_info), &offset));
            LOG_INFO_EX(BSL_LOG_MODULE, "%s offset decided, offset %d size %d %s\n",
                        dbal_hl_tcam_access_type_to_string(unit, access_type_id), offset, data_size, EMPTY);
        }

        if (previously_written)
        {
            sal_memset(data, 0, sizeof(data));
            previously_written = 0;
        }

        SHR_IF_ERR_EXIT(dbal_hl_tcam_arr_offset_and_block_get(unit, entry_handle, curr_l2p_info,
                                                              &array_offset, &block, &num_of_blocks));

        if (dbal_hl_tcam_is_table_supports_half_entry(unit, entry_handle->table_id))
        {

            SHR_IF_ERR_EXIT(soc_mem_array_read(unit, memory, array_offset, block, entry_offset, data));

            if ((entry_handle->phy_entry.entry_hw_id % 2) != 0)
            {
                offset += data_size;
            }
        }

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(data_to_copy, offset, data_size, data));

        if ((iter == 0) || (memory != next_memory) || (next_entry_offset != entry_offset))
        {
            int ii, jj;

            previously_written = 1;

            for (ii = 0; ii < nof_group_elements; ii++)
            {
                memory = curr_l2p_info->memory[ii];
                if (memory == INVALIDm)
                {
                    break;
                }
                for (jj = block; jj < block + num_of_blocks; ++jj)
                {
                    DBAL_DATA_LOG(data, 1);
                    DBAL_MEM_WRITE_LOG(memory, entry_offset, jj, array_offset);
                    SHR_IF_ERR_EXIT(soc_mem_array_write(unit, memory, array_offset, jj, entry_offset, data));
                    DBAL_ACTION_PERFORMED_LOG;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_tcam_is_entry_default(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *is_default)
{
    uint8 ii = 0;
    uint32 default_entry[DBAL_NOF_HL_TCAM_ACCESS_TYPES][DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { {0} };
    dbal_hl_l2p_field_info_t *l2p_mask_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    (*is_default) = 0;

    for (ii = 0; ii < DBAL_NOF_HL_TCAM_ACCESS_TYPES; ++ii)
    {
        l2p_mask_info =
            &entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].
            l2p_hl_info[DBAL_HL_ACCESS_MEMORY].l2p_fields_info[ii];

        if (l2p_mask_info->encode_info.encode_mode != DBAL_VALUE_FIELD_ENCODE_NONE)
        {
            SHR_IF_ERR_EXIT(dbal_hl_access_decode(unit, l2p_mask_info->encode_info.encode_mode,
                                                  l2p_mask_info->encode_info.input_param,
                                                  default_entry[ii], default_entry[ii],
                                                  l2p_mask_info->nof_bits_in_interface));
        }
    }

    if ((sal_memcmp
         (default_entry[DBAL_HL_TCAM_ACCESS_TYPE_RESULT], entry_handle->phy_entry.payload,
          entry_handle->table->max_payload_size) == 0)
        &&
        (sal_memcmp
         (default_entry[DBAL_HL_TCAM_ACCESS_TYPE_KEY], entry_handle->phy_entry.key,
          DBAL_PHYSICAL_KEY_SIZE_IN_BYTES) == 0)
        &&
        (sal_memcmp
         (default_entry[DBAL_HL_TCAM_ACCESS_TYPE_KEY_MASK], entry_handle->phy_entry.k_mask,
          DBAL_PHYSICAL_KEY_SIZE_IN_BYTES) == 0))
    {
        (*is_default) = 1;
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

                if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
                {
                    entry_handle->core_id--;
                    if (entry_handle->core_id < 0)
                    {
                        break;
                    }
                    else
                    {
                        entry_handle->phy_entry.entry_hw_id = 0;
                    }
                }
                else
                {
                    break;
                }
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

                SHR_IF_ERR_EXIT(dbal_hl_tcam_is_entry_default(unit, entry_handle, &is_default));
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
