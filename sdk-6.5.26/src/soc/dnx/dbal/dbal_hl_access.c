/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#include "dbal_hl_access.h"
#include <soc/drv.h>

shr_error_e
dbal_hl_offset_calculate(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_hl_l2p_field_info_t * curr_l2p_info,
    uint32 default_val,
    dbal_offset_encode_info_t * encode_info,
    uint32 *calc_index)
{
    SHR_FUNC_INIT_VARS(unit);

    if (encode_info->formula != NULL)
    {
        if (encode_info->formula->cb)
        {
            uint32 encoded_field;
            DBAL_ENCODE_FIELD_ID_WITH_INSTANCE(encoded_field, curr_l2p_info->field_id, curr_l2p_info->inst_idx);
            SHR_IF_ERR_EXIT(encode_info->formula->cb(unit, entry_handle, encoded_field, calc_index));
        }
        else
        {
            (*calc_index) = encode_info->formula->val;
        }
    }
    else
    {
        (*calc_index) = default_val;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_access_encode(
    int unit,
    dbal_value_field_encode_types_e encode_type,
    uint32 encode_param,
    uint32 *input_val,
    uint32 *output_val,
    uint32 field_size)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (encode_type)
    {
        case DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT:
            SHR_IF_ERR_EXIT(dbal_fields_encode_decode_bitwise_not(unit, input_val, field_size, output_val));
            break;
        case DBAL_VALUE_FIELD_ENCODE_MULTIPLY:
        {
            uint32 input, output;
            input = *input_val;
            output = input * encode_param;
            *output_val = output;
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal encoding type for access\n");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_access_decode(
    int unit,
    dbal_value_field_encode_types_e encode_type,
    uint32 encode_param,
    uint32 *input_val,
    uint32 *output_val,
    uint32 field_size)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (encode_type)
    {
        case DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT:
            SHR_IF_ERR_EXIT(dbal_fields_encode_decode_bitwise_not(unit, input_val, field_size, output_val));
            break;
        case DBAL_VALUE_FIELD_ENCODE_MULTIPLY:
        {
            uint32 input, output;
            input = *input_val;
            output = input / encode_param;
            *output_val = output;
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal encoding type for access\n");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

#if defined(BCM_ACCESS_SUPPORT)
shr_error_e
dbal_hl_new_access_regmem_local_id_get(
    int unit,
    int global_regmem,
    access_local_regmem_id_t * local_regmem_id)
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = access_regmem_global2local(SOC_NEW_ACCESS_RUNTIME_INFO(unit), global_regmem, local_regmem_id);
    if (rv != 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Memory %d global_regmem does not exist in current device\n", global_regmem);
    }

exit:
    SHR_FUNC_EXIT;
}
#endif

shr_error_e
dbal_hl_access_regmem_validate(
    int unit,
    int table_id,
    int regmem,
    int is_reg)
{
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    if (regmem == DBAL_HL_ACCESS_HW_ENTITY_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s: Invalid hw entity wasn't expected\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_NEW_ACCESS))
    {
        if (is_reg && (!SOC_REG_IS_VALID(unit, regmem)))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s, register %s is invalid for device \n",
                         dbal_logical_table_to_string(unit, table_id), SOC_REG_NAME(unit, regmem));
        }

        if ((!is_reg) && (!SOC_MEM_IS_VALID(unit, regmem)))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s, memory %s is invalid for device \n",
                         dbal_logical_table_to_string(unit, table_id), SOC_MEM_NAME(unit, regmem));
        }
    }
#if defined(BCM_ACCESS_SUPPORT)
    else
    {
        access_local_regmem_id_t local_regmem_id;
        SHR_IF_ERR_EXIT(dbal_hl_new_access_regmem_local_id_get(unit, regmem, &local_regmem_id));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

#if defined(BCM_ACCESS_SUPPORT)

shr_error_e
dbal_hl_new_access_regmem_info_get(
    int unit,
    int global_regmem,
    const access_device_regmem_t ** rm_info)
{
    access_local_regmem_id_t local_regmem_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_hl_new_access_regmem_local_id_get(unit, global_regmem, &local_regmem_id));
    *rm_info = DBAL_HL_NEW_ACCESS_DEVICE_TYPE_INFO(unit)->local_regs + local_regmem_id;

exit:
    SHR_FUNC_EXIT;
}
#endif

shr_error_e
dbal_hl_access_memreg_size_get_per_access(
    int unit,
    int global_regmem,
    int is_new_access,
    uint32 *regmem_size_bits)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!is_new_access)
    {
        int mem_size = SOC_MEM_BYTES(unit, global_regmem);
        *regmem_size_bits = BYTES2BITS(mem_size);
    }
#if defined(BCM_ACCESS_SUPPORT)
    else
    {
        const access_device_regmem_t *rm_info = NULL;
        SHR_IF_ERR_EXIT(dbal_hl_new_access_regmem_info_get(unit, global_regmem, &rm_info));
        *regmem_size_bits = rm_info->width_in_bits;
    }

exit:
#endif
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_access_memory_max_index_get_per_access(
    int unit,
    int mem,
    dbal_logical_table_t * table,
    uint32 *index_max)
{
    SHR_FUNC_INIT_VARS(unit);

#if defined(BCM_ACCESS_SUPPORT)
    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_NEW_ACCESS))
#endif
    {
        *index_max = SOC_MEM_INFO(unit, mem).index_max;
    }
#if defined(BCM_ACCESS_SUPPORT)
    else
    {
        const access_device_regmem_t *rm_info;
        SHR_IF_ERR_EXIT(dbal_hl_new_access_regmem_info_get(unit, mem, &rm_info));
        *index_max = rm_info->u.mem.mem_nof_elements - 1;
    }

exit:
#endif
    SHR_FUNC_EXIT;
}

#if defined(BCM_ACCESS_SUPPORT)

static shr_error_e
dbal_hl_new_access_block_info_get(
    int unit,
    int global_regmem,
    const access_device_block_t ** new_access_local_block)
{
    const access_device_regmem_t *rm_info;
    int rv = 0;

    SHR_FUNC_INIT_VARS(unit);

    rm_info = NULL;
    *new_access_local_block = NULL;

    rv = dbal_hl_new_access_regmem_info_get(unit, global_regmem, &rm_info);
    if (rv != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error in global to local memory mapping\n");
    }

    *new_access_local_block = DBAL_HL_NEW_ACCESS_DEVICE_TYPE_INFO(unit)->blocks + rm_info->local_block_id;

exit:
    SHR_FUNC_EXIT;
}
#endif

shr_error_e
dbal_hl_access_block_calc(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 core_id,
    int regmem,
    dbal_offset_encode_info_t * block_index_info,
    dbal_hl_l2p_field_info_t * curr_l2p_info,
    int is_mem,
    int *block,
    int *num_of_blocks)
{
    uint32 calc_block = 0;
    int min_block = 0, max_block;
    int total_nof_blocks;
    SHR_FUNC_INIT_VARS(unit);

    (*num_of_blocks) = 1;
    (*block) = dbal_hl_access_block_any_get(unit, entry_handle, is_mem);

#if defined(BCM_ACCESS_SUPPORT)
    if (!SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_NEW_ACCESS))
#endif
    {
        if (is_mem)
        {
            min_block = SOC_MEM_BLOCK_MIN(unit, regmem);
            max_block = SOC_MEM_BLOCK_MAX(unit, regmem);
        }
        else
        {
            max_block = SOC_REG_BLOCK_NOF_INSTANCES(unit, regmem) - 1;
        }
        total_nof_blocks = max_block - min_block + 1;
    }
#if defined(BCM_ACCESS_SUPPORT)
    else
    {
        const access_device_block_t *local_block;
        SHR_IF_ERR_EXIT(dbal_hl_new_access_block_info_get(unit, regmem, &local_block));
        if (local_block == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "local_block should not be null");
        }
        total_nof_blocks = local_block->nof_instances;
    }
#endif

    if (block_index_info->formula != NULL)
    {

        SHR_IF_ERR_EXIT(dbal_hl_offset_calculate(unit, entry_handle, curr_l2p_info, 0, block_index_info, &calc_block));
        (*block) = min_block + calc_block;
    }
    else if (core_id != DBAL_CORE_ALL)
    {

        (*num_of_blocks) = total_nof_blocks;

        if (DBAL_MAX_NUM_OF_CORES != 1)
        {
            (*num_of_blocks) = ((*num_of_blocks) + 1) / DBAL_MAX_NUM_OF_CORES;
        }
        (*block) = min_block + (core_id * (*num_of_blocks));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_pre_access_calculate(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_hl_l2p_field_info_t * curr_l2p_info,
    uint8 *is_condition_pass,
    uint32 *mem_array_offset,
    uint32 *mem_entry_offset,
    uint32 *field_offset,
    uint32 *group_offset)
{
    uint32 *key = entry_handle->phy_entry.key;

    SHR_FUNC_INIT_VARS(unit);

    if (curr_l2p_info->nof_conditions > 0)
    {
        SHR_IF_ERR_EXIT(dbal_key_or_value_condition_check(unit, entry_handle, TRUE, curr_l2p_info->mapping_condition,
                                                          curr_l2p_info->nof_conditions, is_condition_pass));
    }
    else
    {
        (*is_condition_pass) = 1;
    }
    if ((*is_condition_pass))
    {
        if (mem_array_offset)
        {

            if (curr_l2p_info->array_offset_info.formula != NULL)
            {
                SHR_IF_ERR_EXIT(dbal_hl_offset_calculate(unit, entry_handle, curr_l2p_info,
                                                         0, &(curr_l2p_info->array_offset_info), mem_array_offset));
            }
            else
            {
                (*mem_array_offset) = 0;

            }
        }

        if (mem_entry_offset)
        {

            if (curr_l2p_info->entry_offset_info.formula != NULL)
            {
                SHR_IF_ERR_EXIT(dbal_hl_offset_calculate(unit, entry_handle, curr_l2p_info,
                                                         key[0], &(curr_l2p_info->entry_offset_info),
                                                         mem_entry_offset));
            }
            else
            {
                (*mem_entry_offset) = key[0];

            }

        }

        if (field_offset)
        {

            if (curr_l2p_info->data_offset_info.formula != NULL)
            {
                SHR_IF_ERR_EXIT(dbal_hl_offset_calculate(unit, entry_handle, curr_l2p_info,
                                                         0, &(curr_l2p_info->data_offset_info), field_offset));
            }
            else
            {
                (*field_offset) = 0;

            }
        }

        if (group_offset)
        {

            if (curr_l2p_info->group_offset_info.formula != NULL)
            {
                SHR_IF_ERR_EXIT(dbal_hl_offset_calculate(unit, entry_handle, curr_l2p_info,
                                                         -1, &(curr_l2p_info->group_offset_info), group_offset));
            }
            else
            {
                (*group_offset) = -1;

            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_access_is_field_exist_per_access(
    int unit,
    CONST dbal_logical_table_t * table,
    int regmem,
    int hw_field,
    int is_mem,
    int *is_field_exist)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_field_exist = 0;

    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_NEW_ACCESS))
    {
        int nof_fields;
        soc_field_info_t *fields_array;
        soc_field_info_t *fieldinfo = NULL;

        if (is_mem && (regmem != DBAL_HL_ACCESS_HW_ENTITY_INVALID))
        {
            nof_fields = SOC_MEM_INFO(unit, regmem).nFields;
            fields_array = SOC_MEM_INFO(unit, regmem).fields;
        }
        else if (!is_mem && (regmem != DBAL_HL_ACCESS_HW_ENTITY_INVALID))
        {
            nof_fields = SOC_REG_INFO(unit, regmem).nFields;
            fields_array = SOC_REG_INFO(unit, regmem).fields;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "memory/register must be set\n");
        }

        SOC_FIND_FIELD(hw_field, fields_array, nof_fields, fieldinfo);
        if (fieldinfo)
        {
            *is_field_exist = 1;
        }
    }
#if defined(BCM_ACCESS_SUPPORT)
    else
    {
        access_local_field_id_t local_field_id =
            DBAL_HL_NEW_ACCESS_DEVICE_TYPE_INFO(unit)->fields_global2local_map[hw_field];
        if (local_field_id != ACCESS_INVALID_LOCAL_FIELD_ID)
        {
            *is_field_exist = 1;
        }
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_access_field_info_get_per_access(
    int unit,
    CONST dbal_logical_table_t * table,
    int regmem,
    int hw_field,
    dbal_hl_new_access_field_scope_e field_scope,
    int is_mem,
    int *field_offset,
    int *field_len,
    char *field_name)
{
    int field_offset_loc = 0, field_len_loc = 0;
    char field_name_loc[DBAL_MAX_SHORT_STRING_LENGTH];

    SHR_FUNC_INIT_VARS(unit);

#if defined(BCM_ACCESS_SUPPORT)
    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_NEW_ACCESS))
#endif
    {
        int nof_fields;
        soc_field_info_t *fields_array;
        soc_field_info_t *fieldinfo = NULL;

        if (is_mem && (regmem != DBAL_HL_ACCESS_HW_ENTITY_INVALID))
        {
            nof_fields = SOC_MEM_INFO(unit, regmem).nFields;
            fields_array = SOC_MEM_INFO(unit, regmem).fields;
        }
        else if (!is_mem && (regmem != DBAL_HL_ACCESS_HW_ENTITY_INVALID))
        {
            nof_fields = SOC_REG_INFO(unit, regmem).nFields;
            fields_array = SOC_REG_INFO(unit, regmem).fields;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "memory/register must be set\n");
        }

        SOC_FIND_FIELD(hw_field, fields_array, nof_fields, fieldinfo);

        if (fieldinfo)
        {
            field_offset_loc = fieldinfo->bp;
            field_len_loc = fieldinfo->len;
            if (field_name)
            {
                sal_strncpy(field_name_loc, SOC_FIELD_NAME(unit, hw_field), DBAL_MAX_SHORT_STRING_LENGTH - 1);
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "mapping error for table %s, field %s: hw_field %s does not exist in"
                         "register/memory %s\n",
                         table->table_name,
                         dbal_field_to_string(unit, hw_field),
                         SOC_FIELD_NAME(unit, hw_field),
                         is_mem ? SOC_MEM_NAME(unit, regmem) : SOC_REG_NAME(unit, regmem));
        }
    }
#if defined(BCM_ACCESS_SUPPORT)
    else
    {
        const access_device_field_t *field_info;
        const access_device_field_t *field_info_array = DBAL_HL_NEW_ACCESS_DEVICE_TYPE_INFO(unit)->local_fields;
        access_local_field_id_t local_field_id = hw_field;
        if (field_scope == DBAL_HL_NEW_ACCESS_SCOPE_GLOBAL_FIELD)
        {
            local_field_id = DBAL_HL_NEW_ACCESS_DEVICE_TYPE_INFO(unit)->fields_global2local_map[hw_field];
        }
        if (local_field_id != ACCESS_INVALID_LOCAL_FIELD_ID)
        {
            field_info = field_info_array + local_field_id;
            if (field_info)
            {
                field_offset_loc = field_info->start_bit;
                field_len_loc = field_info->size_in_bits;
                if (field_name)
                {
                    sal_strncpy(field_name_loc, ACCESS_FIELD_NAME(hw_field), DBAL_MAX_SHORT_STRING_LENGTH - 1);
                }
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "hw field %d does not exist in memreg %s\n",
                         hw_field, ACCESS_REGMEM_NAME(regmem));
        }
    }
#endif

    if (field_offset)
    {
        *field_offset = field_offset_loc;
    }
    if (field_len)
    {
        *field_len = field_len_loc;
    }
    if (field_name)
    {
        sal_strncpy(field_name, field_name_loc, DBAL_MAX_SHORT_STRING_LENGTH);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_access_field_offset_get_per_access(
    int unit,
    CONST dbal_logical_table_t * table,
    dbal_hl_l2p_field_info_t * curr_l2p_info,
    int is_mem,
    int *field_offset)
{
    int regmem;
    SHR_FUNC_INIT_VARS(unit);

    *field_offset = 0;
    if (curr_l2p_info->hw_field == DBAL_HL_ACCESS_HW_FIELD_INVALID)
    {
        SHR_EXIT();
    }

    regmem = is_mem ? curr_l2p_info->memory[0] : curr_l2p_info->reg[0];

    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_NEW_ACCESS))
    {
        int nof_fields;
        soc_field_info_t *fields_array;
        soc_field_info_t *fieldinfo = NULL;

        if (is_mem && (regmem != DBAL_HL_ACCESS_HW_ENTITY_INVALID))
        {
            nof_fields = SOC_MEM_INFO(unit, regmem).nFields;
            fields_array = SOC_MEM_INFO(unit, regmem).fields;
        }
        else if (!is_mem && (regmem != DBAL_HL_ACCESS_HW_ENTITY_INVALID))
        {
            nof_fields = SOC_REG_INFO(unit, regmem).nFields;
            fields_array = SOC_REG_INFO(unit, regmem).fields;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "memory/register must be set\n");
        }

        SOC_FIND_FIELD(curr_l2p_info->hw_field, fields_array, nof_fields, fieldinfo);

        if (fieldinfo)
        {
            *field_offset = fieldinfo->bp;
        }
    }
#if defined(BCM_ACCESS_SUPPORT)
    else
    {
        const access_device_field_t *field_info;
        const access_device_field_t *field_info_array = DBAL_HL_NEW_ACCESS_DEVICE_TYPE_INFO(unit)->local_fields;
        access_local_field_id_t local_field_id =
            DBAL_HL_NEW_ACCESS_DEVICE_TYPE_INFO(unit)->fields_global2local_map[curr_l2p_info->hw_field];
        if (local_field_id != ACCESS_INVALID_LOCAL_FIELD_ID)
        {
            field_info = field_info_array + local_field_id;
            if (field_info)
            {
                *field_offset = field_info->start_bit;
            }
        }
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

const char *
dbal_hl_access_field_name_get(
    int unit,
    CONST dbal_logical_table_t * table,
    dbal_hl_l2p_field_info_t * curr_l2p_info)
{
#if defined(BCM_ACCESS_SUPPORT)
    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_NEW_ACCESS))
#endif
    {
        return SOC_FIELD_NAME(unit, curr_l2p_info->hw_field);
    }
#if defined(BCM_ACCESS_SUPPORT)
    else
    {
        return ACCESS_FIELD_NAME(curr_l2p_info->hw_field);
    }
#endif
}

const char *
dbal_hl_access_regmem_name_get(
    int unit,
    CONST dbal_logical_table_t * table,
    int regmem,
    uint8 is_mem)
{
#if defined(BCM_ACCESS_SUPPORT)
    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_NEW_ACCESS))
#endif
    {
        if (is_mem)
        {
            return SOC_MEM_NAME(unit, (soc_mem_t) regmem);
        }
        else
        {
            return SOC_REG_NAME(unit, (soc_reg_t) regmem);
        }
    }
#if defined(BCM_ACCESS_SUPPORT)
    else
    {
        return ACCESS_REGMEM_NAME(regmem);
    }
#endif
}

static shr_error_e
dbal_hl_field_to_hw_buffer_set(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_hl_l2p_info_t * l2p_hl_info,
    int regmem,
    int is_mem,
    int offset_in_field,
    int field_pos,
    uint32 *src,
    uint32 *dst,
    int logger_enable)
{
    dbal_hl_l2p_field_info_t *curr_l2p_info = &(l2p_hl_info->l2p_fields_info[field_pos]);
    uint32 *field_data_ptr;
    uint32 field_data[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 };
    uint32 field_data_encoded[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 };
    int offset_in_hw;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(src, curr_l2p_info->offset_in_interface,
                                                   curr_l2p_info->nof_bits_in_interface, field_data));

    field_data_ptr = field_data;
    if (curr_l2p_info->encode_info.encode_mode != DBAL_VALUE_FIELD_ENCODE_NONE)
    {
        SHR_IF_ERR_EXIT(dbal_hl_access_encode(unit, curr_l2p_info->encode_info.encode_mode,
                                              curr_l2p_info->encode_info.input_param,
                                              field_data, field_data_encoded, curr_l2p_info->nof_bits_in_interface));
        field_data_ptr = field_data_encoded;
    }

    offset_in_hw = curr_l2p_info->hw_field_offset_in_mem + offset_in_field;

    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                    (field_data_ptr, offset_in_hw, curr_l2p_info->nof_bits_in_interface, dst));

    if (logger_enable)
    {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit, "\twrite field %-25s to HWfield %-25s, offset in mem %d, nof_bits %d, value 0x%x\n"),
                     dbal_field_to_string(unit, curr_l2p_info->field_id),
                     dbal_hl_access_field_name_get(unit, entry_handle->table, curr_l2p_info),
                     offset_in_hw, curr_l2p_info->nof_bits_in_interface, field_data_ptr[0]));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_field_from_hw_buffer_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_hl_l2p_info_t * l2p_hl_info,
    int regmem,
    int offset_in_field,
    int field_pos,
    uint32 *src,
    uint32 *dst,
    int logger_enable)
{
    dbal_hl_l2p_field_info_t *curr_l2p_info = &(l2p_hl_info->l2p_fields_info[field_pos]);
    int offset_in_hw;
    uint32 *data_final_ptr;
    uint32 data_aligned[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    offset_in_hw = offset_in_field + curr_l2p_info->hw_field_offset_in_mem;

    data_final_ptr = src;
    if (offset_in_hw >= 0)
    {

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (src, offset_in_hw, (curr_l2p_info->nof_bits_in_interface), data_aligned));
    }

    data_final_ptr = data_aligned;
    if (logger_enable && (curr_l2p_info->hw_field != DBAL_HL_ACCESS_HW_FIELD_INVALID))
    {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit, "\tread field %-25s from HWfield %-25s, offset in mem %d nof_bits %d value 0x%x\n"),
                     dbal_field_to_string(unit, curr_l2p_info->field_id),
                     dbal_hl_access_field_name_get(unit, entry_handle->table, curr_l2p_info),
                     offset_in_hw, curr_l2p_info->nof_bits_in_interface, data_final_ptr[0]));
    }

    {
        uint32 data_decoded[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 };

        if (curr_l2p_info->encode_info.encode_mode != DBAL_VALUE_FIELD_ENCODE_NONE)
        {
            SHR_IF_ERR_EXIT(dbal_hl_access_decode(unit, curr_l2p_info->encode_info.encode_mode,
                                                  curr_l2p_info->encode_info.input_param,
                                                  data_final_ptr, data_decoded, curr_l2p_info->nof_bits_in_interface));
            data_final_ptr = data_decoded;
        }

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(data_final_ptr,
                                                       (curr_l2p_info->offset_in_interface),
                                                       (curr_l2p_info->nof_bits_in_interface), dst));
        if (logger_enable && (curr_l2p_info->hw_field == DBAL_HL_ACCESS_HW_FIELD_INVALID))
        {
            LOG_VERBOSE_EX(BSL_LOG_MODULE, "\tread field %-25s, offset in mem %d, nof_bits %d, value 0x%x\n",
                           dbal_field_to_string(unit, curr_l2p_info->field_id), curr_l2p_info->offset_in_interface,
                           offset_in_hw, data_final_ptr[0]);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#if defined(BCM_ACCESS_SUPPORT)
static shr_error_e
dbal_hl_new_access_reg_default_get(
    int unit,
    soc_reg_t reg,
    const uint64 **default_val)
{
    const access_device_regmem_t *rm_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_hl_new_access_regmem_info_get(unit, reg, &rm_info));

    if (rm_info->width_in_bits <= 64)
    {
        *default_val = &rm_info->u.default_value_data;
    }
    else
    {
        *default_val = rm_info->u.default_value_pointer;
    }

exit:
    SHR_FUNC_EXIT;
}
#endif

static shr_error_e
dbal_hl_reg_default_get_per_access(
    int unit,
    int reg,
    int is_new_access,
    uint32 *entry)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!is_new_access)
    {
        SHR_IF_ERR_EXIT(dbal_hl_reg_default_get(unit, reg, entry));
    }
#if defined(BCM_ACCESS_SUPPORT)
    else
    {
        const uint64 *default_value_pointer;
        uint32 entry_size_in_bits;
        SHR_IF_ERR_EXIT(dbal_hl_access_memreg_size_get_per_access(unit, reg, 1, &entry_size_in_bits));
        SHR_IF_ERR_EXIT(dbal_hl_new_access_reg_default_get(unit, reg, &default_value_pointer));
        sal_memcpy(entry, default_value_pointer, BITS2WORDS(entry_size_in_bits) * 4);
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_hl_non_standard_default_entry_build(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 *default_entry)
{
    uint32 default_val[SOC_MAX_MEM_WORDS] = { 0 };
    int field_pos, is_non_standard = 0;
    int memory = DBAL_HL_ACCESS_HW_ENTITY_INVALID;
    int cur_memory = DBAL_HL_ACCESS_HW_ENTITY_INVALID;
    int reg = DBAL_HL_ACCESS_HW_ENTITY_INVALID;
    dbal_hl_l2p_field_info_t *curr_l2p_info;
    uint8 is_condition_pass;
    uint32 array_offset, mem_entry_offset, field_offset, group_offset;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(default_entry, 0, DBAL_PHYSICAL_RES_SIZE_IN_BYTES);

    for (field_pos = 0; field_pos < DBAL_HL_MEM_MUL_RES_INFO.num_of_access_fields; field_pos++)
    {
        curr_l2p_info = &(DBAL_HL_MEM_MUL_RES_INFO.l2p_fields_info[field_pos]);
        SHR_IF_ERR_EXIT(dbal_hl_pre_access_calculate(unit, entry_handle, curr_l2p_info, &is_condition_pass,
                                                     &array_offset, &mem_entry_offset, &field_offset, &group_offset));
        if (is_condition_pass)
        {
            if (DBAL_HL_MEM_MUL_RES_INFO.l2p_fields_info[field_pos].alias_memory != DBAL_HL_ACCESS_HW_ENTITY_INVALID)
            {
                cur_memory = DBAL_HL_MEM_MUL_RES_INFO.l2p_fields_info[field_pos].alias_memory;
            }
            else
            {
                cur_memory = DBAL_HL_MEM_MUL_RES_INFO.l2p_fields_info[field_pos].memory[0];
            }
            if (memory != cur_memory)
            {
                is_non_standard = DBAL_HL_MEM_MUL_RES_INFO.l2p_fields_info[field_pos].is_default_non_standard;
            }
            memory = cur_memory;
            if (is_non_standard)
            {
                int block, num_of_blocks;
                SHR_IF_ERR_EXIT(dbal_hl_access_block_calc(unit, entry_handle, entry_handle->core_id, memory,
                                                          &(curr_l2p_info->block_index_info), curr_l2p_info,
                                                          1, &block, &num_of_blocks));

                SHR_IF_ERR_EXIT(dnx_init_mem_default_get(unit, memory, array_offset, block,
                                                         mem_entry_offset, default_val, NULL));

                SHR_IF_ERR_EXIT(dbal_hl_field_from_hw_buffer_get
                                (unit, entry_handle, &DBAL_HL_MEM_MUL_RES_INFO,
                                 DBAL_HL_MEM_MUL_RES_INFO.l2p_fields_info[field_pos].memory[0], field_offset,
                                 field_pos, default_val, default_entry, 0));
            }
        }
    }

    for (field_pos = 0; field_pos < DBAL_HL_REG_MUL_RES_INFO.num_of_access_fields; field_pos++)
    {
        curr_l2p_info = &(DBAL_HL_REG_MUL_RES_INFO.l2p_fields_info[field_pos]);
        SHR_IF_ERR_EXIT(dbal_hl_pre_access_calculate(unit, entry_handle, curr_l2p_info, &is_condition_pass,
                                                     &array_offset, NULL, &field_offset, &group_offset));
        if (is_condition_pass)
        {
            if (reg != DBAL_HL_REG_MUL_RES_INFO.l2p_fields_info[field_pos].reg[0])
            {
                is_non_standard = DBAL_HL_REG_MUL_RES_INFO.l2p_fields_info[field_pos].is_default_non_standard;
            }
            reg = DBAL_HL_REG_MUL_RES_INFO.l2p_fields_info[field_pos].reg[0];
            if (is_non_standard)
            {
                int block;
                int nof_blocks;

                SHR_IF_ERR_EXIT(dbal_hl_access_block_calc
                                (unit, entry_handle, entry_handle->core_id, reg,
                                 &curr_l2p_info->block_index_info, curr_l2p_info, 0, &block, &nof_blocks));

                SHR_IF_ERR_EXIT(dbal_hl_reg_default_get_per_access(unit, reg,
                                                                   SHR_IS_BITSET(entry_handle->table->indications_bm,
                                                                                 DBAL_TABLE_IND_IS_NEW_ACCESS),
                                                                   default_val));

                SHR_IF_ERR_EXIT(dbal_hl_field_from_hw_buffer_get
                                (unit, entry_handle, &DBAL_HL_REG_MUL_RES_INFO, reg, field_offset, field_pos,
                                 default_val, default_entry, 0));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_hl_direct_is_entry_default(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *is_default)
{
    uint32 default_entry[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };
    uint32 payload[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };
    uint32 default_value[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 };
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    (*is_default) = 0;

    if ((entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].is_default_non_standard))
    {
        SHR_IF_ERR_EXIT(dbal_hl_non_standard_default_entry_build(unit, entry_handle, default_entry));
    }

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    sal_memcpy(payload, entry_handle->phy_entry.payload, DBAL_TABLE_BUFFER_IN_BYTES(entry_handle->table));

    if (iterator_info->mode == DBAL_ITER_MODE_GET_NON_DEFAULT_PERMISSION_WRITE)
    {
        int iter;
        for (iter = 0; iter < entry_handle->table->multi_res_info[entry_handle->cur_res_type].nof_result_fields; iter++)
        {
            if ((entry_handle->table->multi_res_info[entry_handle->cur_res_type].results_info[iter].permission ==
                 DBAL_PERMISSION_READONLY)
                || (entry_handle->table->multi_res_info[entry_handle->cur_res_type].results_info[iter].permission ==
                    DBAL_PERMISSION_TRIGGER))
            {
                int start_bit =
                    entry_handle->table->multi_res_info[entry_handle->cur_res_type].
                    results_info[iter].bits_offset_in_buffer;
                int end_bit =
                    start_bit +
                    entry_handle->table->multi_res_info[entry_handle->cur_res_type].results_info[iter].field_nof_bits;
                int nof_bits = end_bit - start_bit;

                SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(default_entry, start_bit, nof_bits, default_value));
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(default_value, start_bit, nof_bits, payload));
            }
        }
    }

    if (sal_memcmp(default_entry, payload, DBAL_TABLE_BUFFER_IN_BYTES(entry_handle->table)) == 0)
    {
        (*is_default) = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_is_entry_default(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *is_default)
{
    SHR_FUNC_INIT_VARS(unit);

    if (entry_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal result type for entry table %s\n", entry_handle->table->table_name);
    }

    if (entry_handle->table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
    {
        SHR_IF_ERR_EXIT(dbal_hl_tcam_is_entry_default(unit, entry_handle, is_default));
    }
    else if (entry_handle->table->table_type == DBAL_TABLE_TYPE_DIRECT)
    {
        SHR_IF_ERR_EXIT(dbal_hl_direct_is_entry_default(unit, entry_handle, is_default));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported table type for hard logic table: %s. Table %s \n",
                     dbal_table_type_to_string(unit, entry_handle->table->table_type), entry_handle->table->table_name);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_unmapped_access_handle(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int res_type_get)
{
    int iter;
    int result_type = entry_handle->cur_res_type;
    dbal_hl_l2p_info_t *l2p_hl_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (res_type_get != DBAL_INVALID_PARAM)
    {
        result_type = res_type_get;
    }
    if (result_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "illegal result type\n");
    }

    entry_handle->cur_res_type = result_type;

    l2p_hl_info = &entry_handle->table->hl_mapping_multi_res[result_type].l2p_hl_info[DBAL_HL_ACCESS_UNMAPPED];

    for (iter = 0; iter < l2p_hl_info->num_of_access_fields; ++iter)
    {
        uint8 is_key_comb_unmapped = TRUE;
        if (l2p_hl_info->l2p_fields_info[iter].nof_conditions > 0)
        {

            SHR_IF_ERR_EXIT(dbal_key_or_value_condition_check
                            (unit, entry_handle, TRUE, l2p_hl_info->l2p_fields_info[iter].mapping_condition,
                             l2p_hl_info->l2p_fields_info[iter].nof_conditions, &is_key_comb_unmapped));
        }
        if (is_key_comb_unmapped)
        {
            entry_handle->error_info.error_exists = 1;
            entry_handle->error_info.field_and_instance =
                l2p_hl_info->l2p_fields_info[iter].mapping_condition->field_id;
            SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_ACCESS);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_get_by_access_type(
    int unit,
    dbal_hard_logic_access_types_e access_type,
    dbal_entry_handle_t * entry_handle,
    int result_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (access_type)
    {
        case DBAL_HL_ACCESS_MEMORY:
            SHR_IF_ERR_EXIT(dbal_hl_memory_get(unit, entry_handle, result_type));
            break;
        case DBAL_HL_ACCESS_REGISTER:
            SHR_IF_ERR_EXIT(dbal_hl_register_get(unit, entry_handle, result_type));
            break;
        case DBAL_HL_ACCESS_SW:
            SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_get(unit, entry_handle, result_type, NULL));
            break;
        case DBAL_HL_ACCESS_UNMAPPED:
            SHR_IF_ERR_EXIT_NO_MSG(dbal_hl_unmapped_access_handle(unit, entry_handle, result_type));
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dbal_hl_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int rv = 0;
    int access_type, jj;
    int curr_res_type = 0;
    uint32 field_value[1] = { 0 };
    dbal_logical_table_t *table = entry_handle->table;
    dbal_table_field_info_t table_field_info;
    int continue_regular_process;

    SHR_FUNC_INIT_VARS(unit);

    dbal_logger_internal_disable_set(unit);

    if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_HOOK_ACTIVE))
    {
        SHR_IF_ERR_EXIT(dbal_hl_hook_res_type_resolution(unit, entry_handle, &continue_regular_process));
        if (!continue_regular_process)
        {

            SHR_EXIT();
        }
    }

    while (curr_res_type < table->nof_result_types)
    {

        if (table->multi_res_info[curr_res_type].is_disabled)
        {
            curr_res_type++;
            continue;
        }
        for (access_type = 0; access_type < DBAL_NOF_HL_ACCESS_TYPES; access_type++)
        {
            if ((access_type == DBAL_HL_ACCESS_UNMAPPED) &&
                !SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_ASYMMETRIC))
            {
                continue;
            }
            if (table->hl_mapping_multi_res[curr_res_type].l2p_hl_info[access_type].num_of_access_fields > 0)
            {
                rv = dbal_hl_get_by_access_type(unit, access_type, entry_handle, curr_res_type);
                if (rv == _SHR_E_ACCESS)
                {
                    break;
                }
                SHR_IF_ERR_EXIT(rv);
            }
        }

        if (rv == _SHR_E_ACCESS)
        {
            curr_res_type++;
            continue;
        }

        SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, entry_handle->table_id,
                                                   DBAL_FIELD_RESULT_TYPE, 0, curr_res_type, 0, &table_field_info));

        SHR_IF_ERR_EXIT(dbal_field_from_buffer_get(unit, &table_field_info, DBAL_FIELD_RESULT_TYPE,
                                                   entry_handle->phy_entry.payload, field_value));

        for (jj = 0; jj < entry_handle->table->multi_res_info[curr_res_type].result_type_nof_hw_values; jj++)
        {
            if (entry_handle->table->multi_res_info[curr_res_type].result_type_hw_value[jj] == field_value[0])
            {

                entry_handle->cur_res_type = curr_res_type;
                SHR_EXIT();
            }
        }
        curr_res_type++;
    }

    if (curr_res_type == table->nof_result_types)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

exit:
    dbal_logger_internal_disable_clear(unit);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_mem_read_per_access(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int mem,
    int block_indx,
    int array_indx,
    int entry_indx,
    uint32 *data)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_NEW_ACCESS))
    {
        SHR_IF_ERR_EXIT_WITH_LOG(soc_mem_array_read
                                 (unit, mem, array_indx, block_indx, entry_indx, data),
                                 "Failed writing memory %s port=%d arr_ind=%d",
                                 SOC_MEM_NAME(unit, mem), block_indx, array_indx);
    }
#if defined(BCM_ACCESS_SUPPORT)
    else
    {
        SHR_IF_ERR_EXIT_WITH_LOG(access_regmem
                                 (unit, 0, mem, block_indx, array_indx, entry_indx, data),
                                 "Failed writing memory %s port=%d arr_ind=%d",
                                 ACCESS_REGMEM_NAME(mem), block_indx, array_indx);
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_memory_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int result_type_get)
{
    int iter;
    soc_mem_t last_memory_used = DBAL_HL_ACCESS_HW_ENTITY_INVALID, curr_mem;
    int last_mem_entry_offset = 0;
    int last_mem_array_offset_used = 0;
    uint32 mem_entry_offset = 0, field_offset = 0, mem_array_offset = 0, alias_offset, group_offset;
    uint8 is_condition_pass = 0;
    uint32 data[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 };
    uint32 data_for_alias[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 };
    uint32 *data_ptr_for_read;
    uint32 core_id = DBAL_CORE_DEFAULT;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_hl_l2p_info_t *l2p_hl_info;
    dbal_hl_l2p_field_info_t *curr_l2p_info;
    int block;
    int result_type = entry_handle->cur_res_type;
    int orig_result_type = entry_handle->cur_res_type;
    int logger_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_HL_LOGGER_STATUS_UPDATE;

    if (result_type_get != DBAL_INVALID_PARAM)
    {
        result_type = result_type_get;
    }

    if (result_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "illegal result type\n");
    }

    entry_handle->cur_res_type = result_type;

    core_id = entry_handle->core_id;

    l2p_hl_info = &(table->hl_mapping_multi_res[result_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY]);

    for (iter = 0; iter < l2p_hl_info->num_of_access_fields; iter++)
    {
        int field_pos;
        curr_l2p_info = &l2p_hl_info->l2p_fields_info[iter];

        field_pos = curr_l2p_info->field_pos_in_interface;

        if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
        {

            if (result_type_get != DBAL_INVALID_PARAM)
            {
                if (curr_l2p_info->field_id != DBAL_FIELD_RESULT_TYPE)
                {
                    continue;
                }
            }
        }
        else
        {
            if (!entry_handle->get_all_fields)
            {

                if (entry_handle->value_field_ids[field_pos] == DBAL_FIELD_EMPTY)
                {
                    continue;
                }
            }
        }

        SHR_IF_ERR_EXIT(dbal_hl_pre_access_calculate(unit, entry_handle, curr_l2p_info,
                                                     &is_condition_pass, &mem_array_offset, &mem_entry_offset,
                                                     &field_offset, &group_offset));

        if (is_condition_pass)
        {
            curr_mem = (group_offset == -1) ? curr_l2p_info->memory[0] : curr_l2p_info->memory[group_offset];
            DBAL_FIELD_START_LOG(curr_l2p_info->field_id, 1, curr_mem);

            if ((last_memory_used != curr_mem) || (last_mem_entry_offset != mem_entry_offset) ||
                (last_mem_array_offset_used != mem_array_offset))
            {

                int num_of_blocks;
                int memory_to_read =
                    (curr_l2p_info->alias_memory !=
                     DBAL_HL_ACCESS_HW_ENTITY_INVALID) ? curr_l2p_info->alias_memory : curr_mem;
                last_memory_used = curr_mem;
                last_mem_array_offset_used = mem_array_offset;
                last_mem_entry_offset = mem_entry_offset;
                sal_memset(data, 0x0, DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_BYTES);
                alias_offset = 0;

                SHR_IF_ERR_EXIT(dbal_hl_access_block_calc(unit, entry_handle, core_id, last_memory_used,
                                                          &(curr_l2p_info->block_index_info), curr_l2p_info,
                                                          1, &block, &num_of_blocks));

                if ((curr_l2p_info->alias_memory != DBAL_HL_ACCESS_HW_ENTITY_INVALID)
                    && (curr_l2p_info->alias_data_offset_info.formula != NULL))
                {
                    SHR_IF_ERR_EXIT(dbal_hl_offset_calculate(unit, entry_handle, curr_l2p_info, 0,
                                                             &(curr_l2p_info->alias_data_offset_info), &alias_offset));
                }

                DBAL_MEM_READ_LOG(memory_to_read, mem_entry_offset, block, last_mem_array_offset_used);
                if (alias_offset != 0)
                {
                    data_ptr_for_read = data_for_alias;
                }
                else
                {
                    data_ptr_for_read = data;
                }

                SHR_IF_ERR_EXIT(dbal_hl_mem_read_per_access(unit, entry_handle, memory_to_read,
                                                            block, last_mem_array_offset_used, mem_entry_offset,
                                                            data_ptr_for_read));
                DBAL_ACTION_PERFORMED_LOG;
                DBAL_DATA_LOG(data_ptr_for_read, 1);

                if (alias_offset != 0)
                {
                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Aliasing, offset = %d, "), alias_offset));

                    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(data_for_alias, alias_offset,
                                                                   curr_l2p_info->
                                                                   alias_data_offset_info.internal_inparam, data));
                }
            }

            SHR_IF_ERR_EXIT(dbal_hl_field_from_hw_buffer_get
                            (unit, entry_handle, l2p_hl_info, curr_mem, field_offset, iter, data,
                             entry_handle->phy_entry.payload, logger_enable));
        }
    }

    entry_handle->cur_res_type = orig_result_type;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_hl_access_memreg_is_array_per_access(
    int unit,
    const dbal_logical_table_t * table,
    int regmem,
    int is_mem,
    int *is_array)
{
    SHR_FUNC_INIT_VARS(unit);

#if defined(BCM_ACCESS_SUPPORT)
    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_NEW_ACCESS))
#endif
    {
        if (is_mem)
        {
            *is_array = (SOC_MEM_IS_ARRAY(unit, regmem)) ? 1 : 0;
        }
        else
        {
            *is_array = (SOC_REG_IS_ARRAY(unit, regmem)) ? 1 : 0;
        }
    }
#if defined(BCM_ACCESS_SUPPORT)
    else
    {
        const access_device_regmem_t *rm_info;
        SHR_IF_ERR_EXIT(dbal_hl_new_access_regmem_info_get(unit, regmem, &rm_info));
        *is_array = (rm_info->nof_array_indices != 0) ? 1 : 0;
    }

exit:
#endif
    SHR_FUNC_EXIT;
}

static int
dbal_hl_access_block_is_any(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int block,
    int is_mem)
{
#if defined(BCM_ACCESS_SUPPORT)
    if (!SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_NEW_ACCESS))
#endif
    {
        if (is_mem)
        {
            return (block == MEM_BLOCK_ANY);
        }
        else
        {
            return (block == REG_PORT_ANY);
        }
    }
#if defined(BCM_ACCESS_SUPPORT)
    else
    {
        return (block == ACCESS_ALL_BLOCK_INSTANCES);
    }
#endif
}

int
dbal_hl_access_block_any_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int is_mem)
{
#if defined(BCM_ACCESS_SUPPORT)
    if (!SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_NEW_ACCESS))
#endif
    {
        if (is_mem)
        {
            return MEM_BLOCK_ANY;
        }
        else
        {
            return REG_PORT_ANY;
        }
    }
#if defined(BCM_ACCESS_SUPPORT)
    else
    {
        return ACCESS_ALL_BLOCK_INSTANCES;
    }
#endif
}

static shr_error_e
dbal_hl_access_memreg_array_nof_els_get_per_access(
    int unit,
    const dbal_logical_table_t * table,
    int regmem,
    int is_mem,
    uint16 *nof_array_elements)
{
    SHR_FUNC_INIT_VARS(unit);

#if defined(BCM_ACCESS_SUPPORT)
    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_NEW_ACCESS))
#endif
    {
        if (is_mem)
        {
            *nof_array_elements = SOC_MEM_NUMELS(unit, regmem);
        }
        else
        {
            *nof_array_elements = SOC_REG_NUMELS(unit, regmem);
        }
    }
#if defined(BCM_ACCESS_SUPPORT)
    else
    {
        const access_device_regmem_t *rm_info;
        SHR_IF_ERR_EXIT(dbal_hl_new_access_regmem_info_get(unit, regmem, &rm_info));
        *nof_array_elements = (rm_info->nof_array_indices);
    }

exit:
#endif
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_hl_access_memreg_first_array_index_get(
    int unit,
    const dbal_logical_table_t * table,
    int regmem,
    int is_mem,
    uint16 *first_array_index)
{
    SHR_FUNC_INIT_VARS(unit);

#if defined(BCM_ACCESS_SUPPORT)
    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_NEW_ACCESS))
#endif
    {
        if (is_mem)
        {
            *first_array_index = SOC_MEM_FIRST_ARRAY_INDEX(unit, regmem);
        }
        else
        {
            *first_array_index = SOC_REG_FIRST_ARRAY_INDEX(unit, regmem);
        }
    }
#if defined(BCM_ACCESS_SUPPORT)
    else
    {
        const access_device_regmem_t *rm_info;
        SHR_IF_ERR_EXIT(dbal_hl_new_access_regmem_info_get(unit, regmem, &rm_info));
        *first_array_index = rm_info->first_array_index;
    }

exit:
#endif
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_mem_write_per_access(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int mem,
    int block_indx,
    int array_indx,
    int entry_indx,
    uint32 *data)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_NEW_ACCESS))
    {
        SHR_IF_ERR_EXIT_WITH_LOG(soc_mem_array_write
                                 (unit, mem, array_indx, block_indx, entry_indx, data),
                                 "Failed writing memory %s port=%d arr_ind=%d",
                                 SOC_MEM_NAME(unit, mem), block_indx, array_indx);
    }
#if defined(BCM_ACCESS_SUPPORT)
    else
    {
        SHR_IF_ERR_EXIT_WITH_LOG(access_regmem
                                 (unit,
                                  FLAG_ACCESS_IS_WRITE, mem, block_indx, array_indx, entry_indx, data),
                                 "Failed writing memory %s port=%d arr_ind=%d",
                                 ACCESS_REGMEM_NAME(mem), block_indx, array_indx);
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_hl_access_array_info_get(
    int unit,
    const dbal_logical_table_t * table,
    dbal_hl_l2p_field_info_t * l2p_info,
    int regmem,
    int is_mem,
    uint16 default_array_offset,
    uint16 *first_array_index,
    uint16 *nof_elements)
{
    SHR_FUNC_INIT_VARS(unit);

    *first_array_index = default_array_offset;
    *nof_elements = 1;

    if (l2p_info->array_offset_info.formula == NULL)
    {
        int is_array;
        SHR_IF_ERR_EXIT(dbal_hl_access_memreg_is_array_per_access(unit, table, regmem, is_mem, &is_array));
        if (is_array)
        {
            SHR_IF_ERR_EXIT(dbal_hl_access_memreg_first_array_index_get
                            (unit, table, regmem, is_mem, first_array_index));
            SHR_IF_ERR_EXIT(dbal_hl_access_memreg_array_nof_els_get_per_access
                            (unit, table, regmem, is_mem, nof_elements));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_access_fill_partial_table_with_entry_per_access(
    int unit,
    const dbal_logical_table_t * table,
    int mem,
    unsigned array_index_start,
    int block_index,
    int index_start,
    int nof_entries,
    void *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_NEW_ACCESS))
    {
        SHR_IF_ERR_EXIT(sand_fill_partial_table_with_entry(unit, mem, array_index_start, array_index_start, block_index,
                                                           index_start, index_start + nof_entries - 1, entry_data));
    }
#if defined(BCM_ACCESS_SUPPORT)
    else
    {
        SHR_IF_ERR_EXIT(access_fill_partial_regmem(unit, 0, mem, block_index, array_index_start,
                                                   index_start, nof_entries, 0, entry_data));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_memory_write(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_hl_l2p_field_info_t * l2p_info,
    uint32 *data,
    uint32 core_id,
    int array_offset,
    int entry_offset,
    int group_offset,
    int num_of_entries)
{
    int block_iter, num_of_blocks, ii;
    int block;
    soc_mem_t memory;
    int nof_group_elements = 1;
    int logger_enable = 0;
    uint16 first_array_index;
    uint16 nof_els;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_HL_LOGGER_STATUS_UPDATE;

    if (!l2p_info)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "l2P not initialized\n");
    }

    for (ii = 0; ii < DBAL_MAX_NUMBER_OF_HW_ELEMENTS; ii++)
    {

        if ((group_offset != -1) && (ii != group_offset))
        {
            continue;
        }

        memory = l2p_info->memory[ii];
        if (memory == DBAL_HL_ACCESS_HW_ENTITY_INVALID)
        {
            break;
        }

        if (l2p_info->alias_memory != DBAL_HL_ACCESS_HW_ENTITY_INVALID)
        {
            memory = l2p_info->alias_memory;
            if (nof_group_elements > 1)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "unsupported alias and memory group\n");
            }
        }

        SHR_IF_ERR_EXIT(dbal_hl_access_block_calc(unit, entry_handle, core_id, memory,
                                                  &(l2p_info->block_index_info), l2p_info, 1, &block, &num_of_blocks));

        SHR_IF_ERR_EXIT(dbal_hl_access_array_info_get(unit, entry_handle->table, l2p_info, memory, 1, array_offset,
                                                      &first_array_index, &nof_els));

        for (block_iter = block; block_iter < block + num_of_blocks; block_iter++)
        {

            if (nof_els > 1)
            {
                DBAL_MEM_WRITE_ARRAY_LOG(memory, entry_offset, block, nof_els);
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n\tarray indexes: ")));
            }
            else
            {
                DBAL_MEM_WRITE_LOG(memory, entry_offset, block_iter, first_array_index);
            }

            for (int array_index = first_array_index; array_index < first_array_index + nof_els; array_index++)
            {
                if (num_of_entries > 1)
                {
                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%d nof entries %d, "), array_index, num_of_entries));
                    SHR_IF_ERR_EXIT(dbal_hl_access_fill_partial_table_with_entry_per_access
                                    (unit, entry_handle->table, memory, array_index, block_iter, entry_offset,
                                     num_of_entries, data));
                }
                else
                {
                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%d, "), array_index));
                    SHR_IF_ERR_EXIT(dbal_hl_mem_write_per_access
                                    (unit, entry_handle, memory, block_iter, array_index, entry_offset, data));
                }
            }
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
            DBAL_ACTION_PERFORMED_LOG;
        }
    }
    DBAL_DATA_LOG(data, 1);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_memory_set(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int num_of_entries)
{
    int core_id, start_iteration, end_iteration, core_iteration, logger_enable = 0;
    int result_type = entry_handle->cur_res_type;
    dbal_logical_table_t *table = entry_handle->table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_HL_LOGGER_STATUS_UPDATE;

    if ((table->core_mode == DBAL_CORE_MODE_DPC) && (entry_handle->core_id == DBAL_CORE_ALL))
    {

        core_id = 0;
        start_iteration = 0;
        end_iteration = DBAL_MAX_NUM_OF_CORES;
    }
    else
    {

        start_iteration = 0;
        end_iteration = 1;
        core_id = entry_handle->core_id;
    }

    for (core_iteration = start_iteration; core_iteration < end_iteration; core_iteration++)
    {
        int iter;
        soc_mem_t last_memory_used = DBAL_HL_ACCESS_HW_ENTITY_INVALID, curr_mem;
        int last_mem_offset_used = 0;
        int last_mem_array_offset_used = 0;
        uint32 mem_entry_offset = 0, field_offset = 0, mem_array_offset = 0, alias_offset = 0, group_offset =
            0, last_group_offset = 0;
        uint8 is_condition_pass = 0;
        uint32 data[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 };
        uint32 data_for_alias[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 };
        dbal_hl_l2p_info_t *l2p_hl_info =
            &(table->hl_mapping_multi_res[result_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY]);
        dbal_hl_l2p_field_info_t *curr_l2p_info = NULL;
        dbal_hl_l2p_field_info_t *last_l2p_info = NULL;
        int block, num_of_blocks;

        for (iter = 0; iter < l2p_hl_info->num_of_access_fields; iter++)
        {
            int field_pos;
            last_l2p_info = curr_l2p_info;
            field_pos = l2p_hl_info->l2p_fields_info[iter].field_pos_in_interface;

            if (entry_handle->value_field_ids[field_pos] == DBAL_FIELD_EMPTY)
            {

                continue;
            }

            SHR_IF_ERR_EXIT(dbal_hl_pre_access_calculate(unit, entry_handle, &l2p_hl_info->l2p_fields_info[iter],
                                                         &is_condition_pass, &mem_array_offset, &mem_entry_offset,
                                                         &field_offset, &group_offset));
            if (is_condition_pass)
            {

                entry_handle->error_info.error_exists = 0;

                curr_l2p_info = &l2p_hl_info->l2p_fields_info[iter];
                curr_mem = (group_offset == -1) ? curr_l2p_info->memory[0] : curr_l2p_info->memory[group_offset];

                if (curr_mem == DBAL_HL_ACCESS_HW_ENTITY_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid memory access, group offset[%d]\n", group_offset);
                }

                if ((last_memory_used != curr_mem) ||
                    (last_mem_offset_used != mem_entry_offset) || (last_mem_array_offset_used != mem_array_offset))
                {
                    int memory_to_read =
                        (curr_l2p_info->alias_memory !=
                         DBAL_HL_ACCESS_HW_ENTITY_INVALID) ? curr_l2p_info->alias_memory : curr_mem;

                    if (last_memory_used != DBAL_HL_ACCESS_HW_ENTITY_INVALID)
                    {
                        if (alias_offset > 0)
                        {

                            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(data, alias_offset,
                                                                           last_l2p_info->
                                                                           alias_data_offset_info.internal_inparam,
                                                                           data_for_alias));

                            SHR_IF_ERR_EXIT(dbal_hl_memory_write(unit,
                                                                 entry_handle,
                                                                 last_l2p_info,
                                                                 &(data_for_alias[0]),
                                                                 core_id,
                                                                 last_mem_array_offset_used,
                                                                 last_mem_offset_used, last_group_offset,
                                                                 num_of_entries));
                        }
                        else
                        {
                            SHR_IF_ERR_EXIT(dbal_hl_memory_write(unit,
                                                                 entry_handle,
                                                                 last_l2p_info,
                                                                 &(data[0]),
                                                                 core_id,
                                                                 last_mem_array_offset_used,
                                                                 last_mem_offset_used, last_group_offset,
                                                                 num_of_entries));
                        }

                        sal_memset(data, 0x0, DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_BYTES);
                    }

                    DBAL_FIELD_START_LOG(curr_l2p_info->field_id, 1, curr_mem);

                    last_memory_used = curr_mem;
                    last_mem_offset_used = mem_entry_offset;
                    last_mem_array_offset_used = mem_array_offset;
                    last_group_offset = group_offset;

                    SHR_IF_ERR_EXIT(dbal_hl_access_block_calc(unit, entry_handle, core_id, last_memory_used,
                                                              &(curr_l2p_info->block_index_info), curr_l2p_info,
                                                              1, &block, &num_of_blocks));

                    if (!entry_handle->overrun_entry)
                    {
                        DBAL_MEM_READ_LOG(memory_to_read, mem_entry_offset, block, last_mem_array_offset_used);

                        SHR_IF_ERR_EXIT(dbal_hl_mem_read_per_access(unit, entry_handle, memory_to_read,
                                                                    block, mem_array_offset, mem_entry_offset, data));
                        DBAL_ACTION_PERFORMED_LOG;
                        DBAL_DATA_LOG(data, 1);
                    }

                    alias_offset = 0;
                    if (curr_l2p_info->alias_memory != DBAL_HL_ACCESS_HW_ENTITY_INVALID)
                    {
                        SHR_IF_ERR_EXIT(dbal_hl_offset_calculate(unit, entry_handle, curr_l2p_info, 0,
                                                                 &(curr_l2p_info->alias_data_offset_info),
                                                                 &alias_offset));
                        if (alias_offset != 0)
                        {
                            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Aliasing, offset %d, "), alias_offset));
                            sal_memcpy(data_for_alias, data, DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_BYTES);
                            sal_memset(data, 0x0, DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_BYTES);

                            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(data_for_alias, alias_offset,
                                                                           curr_l2p_info->
                                                                           alias_data_offset_info.internal_inparam,
                                                                           data));
                            DBAL_DATA_LOG(data, 1);
                        }
                    }
                }

                SHR_IF_ERR_EXIT(dbal_hl_field_to_hw_buffer_set
                                (unit, entry_handle, l2p_hl_info, last_memory_used, 1, field_offset, iter,
                                 entry_handle->phy_entry.payload, data, logger_enable));
            }
        }

        if (last_memory_used != 0)
        {
            if (curr_l2p_info)
            {
                if (alias_offset > 0)
                {

                    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(data, alias_offset,
                                                                   curr_l2p_info->
                                                                   alias_data_offset_info.internal_inparam,
                                                                   data_for_alias));

                    SHR_IF_ERR_EXIT(dbal_hl_memory_write(unit,
                                                         entry_handle,
                                                         curr_l2p_info,
                                                         &(data_for_alias[0]),
                                                         core_id,
                                                         last_mem_array_offset_used, last_mem_offset_used,
                                                         last_group_offset, num_of_entries));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dbal_hl_memory_write(unit,
                                                         entry_handle,
                                                         curr_l2p_info,
                                                         &(data[0]),
                                                         core_id,
                                                         last_mem_array_offset_used, last_mem_offset_used,
                                                         last_group_offset, num_of_entries));
                }
            }
        }
        core_id++;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_hl_register_read_per_access(
    int unit,
    const dbal_logical_table_t * table,
    int reg,
    int block_indx,
    int array_indx,
    uint32 *data)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_NEW_ACCESS))
    {
        if (soc_cpureg == SOC_REG_INFO(unit, reg).regtype)
        {
            int blk;
            uint8 acc_type;
            uint32 addr;
            addr = soc_reg_addr_get(unit, reg, block_indx, array_indx, SOC_REG_ADDR_OPTION_NONE, &blk, &acc_type);
            data[0] = soc_pci_read(unit, addr);
        }
        else if (soc_customreg == SOC_REG_INFO(unit, reg).regtype)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(soc_custom_reg_above_64_get
                                     (unit, reg, block_indx, array_indx, data),
                                     "Failed reading register %s port=%d arr_ind=%d",
                                     SOC_REG_NAME(unit, reg), block_indx, array_indx);
        }
        else
        {
            SHR_IF_ERR_EXIT_WITH_LOG(soc_reg_above_64_get
                                     (unit, reg, block_indx, array_indx, data),
                                     "Failed reading register %s port=%d arr_ind=%d",
                                     SOC_REG_NAME(unit, reg), block_indx, array_indx);
        }
    }
#if defined(BCM_ACCESS_SUPPORT)
    else
    {
        int rv;
        rv = access_regmem(unit, 0, reg, block_indx, array_indx, 0, data);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "Failed reading table %s, port %d, arr_ind=%d",
                                 table->table_name, block_indx, array_indx);
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_register_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int result_type_get)
{
    int iter;
    int last_reg_used = DBAL_HL_ACCESS_HW_ENTITY_INVALID, curr_reg;
    uint32 field_offset = 0, last_reg_array_offset = 0, reg_array_offset = 0, group_offset;
    uint8 is_condition_pass = 0;
    uint32 data[DBAL_HL_MAX_REG_LINE_SIZE_IN_WORDS] = { 0 };
    int block, last_block;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_hl_l2p_info_t *l2p_hl_info;
    dbal_hl_l2p_field_info_t *curr_l2p_info = NULL;
    int result_type = entry_handle->cur_res_type;
    int logger_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_HL_LOGGER_STATUS_UPDATE;

    if (result_type_get != DBAL_INVALID_PARAM)
    {
        result_type = result_type_get;
    }
    if (result_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "illegal result type\n");
    }

    entry_handle->cur_res_type = result_type;

    l2p_hl_info = &table->hl_mapping_multi_res[result_type].l2p_hl_info[DBAL_HL_ACCESS_REGISTER];

    last_block = dbal_hl_access_block_any_get(unit, entry_handle, 0);
    for (iter = 0; iter < l2p_hl_info->num_of_access_fields; iter++)
    {
        int field_pos;

        curr_l2p_info = &l2p_hl_info->l2p_fields_info[iter];
        field_pos = curr_l2p_info->field_pos_in_interface;

        if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
        {

            if (result_type_get != DBAL_INVALID_PARAM)
            {
                if (curr_l2p_info->field_id != DBAL_FIELD_RESULT_TYPE)
                {
                    continue;
                }
            }
        }
        else
        {
            if (!entry_handle->get_all_fields)
            {
                if (entry_handle->value_field_ids[field_pos] == DBAL_FIELD_EMPTY)
                {
                    continue;
                }
            }
        }

        SHR_IF_ERR_EXIT(dbal_hl_pre_access_calculate(unit, entry_handle, curr_l2p_info, &is_condition_pass,
                                                     &reg_array_offset, NULL, &field_offset, &group_offset));

        if (is_condition_pass)
        {
            int nof_blocks;

            curr_reg = (group_offset == -1) ? curr_l2p_info->reg[0] : curr_l2p_info->reg[group_offset];
            DBAL_FIELD_START_LOG(curr_l2p_info->field_id, 0, curr_reg);

            SHR_IF_ERR_EXIT(dbal_hl_access_block_calc
                            (unit, entry_handle, entry_handle->core_id, curr_reg,
                             &curr_l2p_info->block_index_info, curr_l2p_info, 0, &block, &nof_blocks));

            if ((last_reg_used != curr_reg) || (reg_array_offset != last_reg_array_offset) || (block != last_block))
            {
                last_reg_used = curr_reg;
                last_reg_array_offset = reg_array_offset;
                last_block = block;

                sal_memset(data, 0x0, DBAL_HL_MAX_REG_LINE_SIZE_IN_WORDS * sizeof(uint32));
                DBAL_REG_READ_LOG(last_reg_used, 0, last_block, last_reg_array_offset);

                SHR_IF_ERR_EXIT(dbal_hl_register_read_per_access(unit, entry_handle->table, last_reg_used,
                                                                 last_block, last_reg_array_offset, data));
                DBAL_ACTION_PERFORMED_LOG;
                DBAL_DATA_LOG(data, 0);
            }

            SHR_IF_ERR_EXIT(dbal_hl_field_from_hw_buffer_get
                            (unit, entry_handle, l2p_hl_info, last_reg_used, field_offset, iter, data,
                             entry_handle->phy_entry.payload, logger_enable));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_register_write_per_access(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int reg,
    int block_indx,
    int array_indx,
    uint32 *data)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_NEW_ACCESS))
    {
        if (soc_cpureg == SOC_REG_INFO(unit, reg).regtype)
        {
            soc_pci_write(unit, SOC_REG_INFO(unit, reg).offset, data[0]);
        }
        else if (soc_customreg == SOC_REG_INFO(unit, reg).regtype)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(soc_custom_reg_above_64_set(unit, reg, block_indx, array_indx, data),
                                     "Failed writing register %s port=%d arr_ind=%d",
                                     SOC_REG_NAME(unit, reg), block_indx, array_indx);
        }
        else
        {
            if (block_indx == REG_PORT_ANY)
            {
                SHR_IF_ERR_EXIT_WITH_LOG(soc_reg_above_64_set_all_instances
                                         (unit, reg, array_indx, data),
                                         "Failed writing register %s array offset=%d%s",
                                         SOC_REG_NAME(unit, reg), array_indx, "");
            }
            else
            {
                SHR_IF_ERR_EXIT_WITH_LOG(soc_reg_above_64_set(unit, reg, block_indx, array_indx, data),
                                         "Failed writing register %s port=%d arr_ind=%d",
                                         SOC_REG_NAME(unit, reg), block_indx, array_indx);
            }
        }
    }
#if defined(BCM_ACCESS_SUPPORT)
    else
    {
        SHR_IF_ERR_EXIT_WITH_LOG(access_regmem
                                 (unit, FLAG_ACCESS_IS_WRITE, reg, block_indx, array_indx, 0, data),
                                 "Failed writing memory %s port=%d arr_ind=%d",
                                 ACCESS_REGMEM_NAME(reg), block_indx, array_indx);
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_register_write(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_hl_l2p_field_info_t * l2p_info,
    uint32 *data,
    uint32 first_block,
    int nof_blocks,
    int array_offset,
    int group_offset)
{
    int ii, block_indx;
    int reg;
    int logger_enable = 0;
    uint16 first_array_index;
    uint16 nof_els;

    SHR_FUNC_INIT_VARS(unit);

    if (!l2p_info)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "l2P not initialized\n");
    }

    DBAL_HL_LOGGER_STATUS_UPDATE;

    if ((dbal_hl_access_block_is_any(unit, entry_handle, first_block, 0)) && (nof_blocks != 1))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal block params block=reg_port_any, nof_blocks = %d\n", nof_blocks);
    }

    for (ii = 0; ii < DBAL_MAX_NUMBER_OF_HW_ELEMENTS; ii++)
    {

        if ((group_offset != -1) && (ii != group_offset))
        {
            continue;
        }

        reg = l2p_info->reg[ii];
        if (reg == DBAL_HL_ACCESS_HW_ENTITY_INVALID)
        {
            break;
        }

        SHR_IF_ERR_EXIT(dbal_hl_access_array_info_get(unit, entry_handle->table, l2p_info, reg, 0,
                                                      array_offset, &first_array_index, &nof_els));

        for (block_indx = first_block; block_indx < first_block + nof_blocks; block_indx++)
        {

            DBAL_REG_WRITE_ARRAY_LOG(reg, 0, block_indx, nof_els);

            for (int array_iter = first_array_index; array_iter < first_array_index + nof_els; array_iter++)
            {
                SHR_IF_ERR_EXIT(dbal_hl_register_write_per_access
                                (unit, entry_handle, reg, block_indx, array_iter, data));
            }
            DBAL_ACTION_PERFORMED_LOG;
        }
    }

    DBAL_DATA_LOG(data, 0);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_register_set(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int core_id, start_iteration, end_iteration, core_iteration, iter, logger_enable = 0;
    int result_type = entry_handle->cur_res_type;
    dbal_logical_table_t *table = entry_handle->table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_HL_LOGGER_STATUS_UPDATE;

    if ((table->core_mode == DBAL_CORE_MODE_DPC) && (entry_handle->core_id == DBAL_CORE_ALL))
    {

        core_id = 0;
        start_iteration = 0;
        end_iteration = DBAL_MAX_NUM_OF_CORES;
    }
    else
    {

        start_iteration = 0;
        end_iteration = 1;
        core_id = entry_handle->core_id;
    }

    for (core_iteration = start_iteration; core_iteration < end_iteration; core_iteration++)
    {
        int last_reg_used = DBAL_HL_ACCESS_HW_ENTITY_INVALID, curr_reg;
        uint32 reg_array_offset = 0, last_reg_array_offset = 0, field_offset = 0, group_offset = 0, last_group_offset =
            0;
        uint8 is_condition_pass = 0;
        uint32 data[DBAL_HL_MAX_REG_LINE_SIZE_IN_WORDS] = { 0 };
        int block, last_block;
        int last_nof_blocks = 0;
        dbal_hl_l2p_info_t *l2p_hl_info =
            &table->hl_mapping_multi_res[result_type].l2p_hl_info[DBAL_HL_ACCESS_REGISTER];
        dbal_hl_l2p_field_info_t *curr_l2p_info = NULL;
        dbal_hl_l2p_field_info_t *last_l2p_info = NULL;

        block = last_block = dbal_hl_access_block_any_get(unit, entry_handle, 0);

        for (iter = 0; iter < l2p_hl_info->num_of_access_fields; iter++)
        {
            int field_pos;

            last_l2p_info = curr_l2p_info;

            field_pos = l2p_hl_info->l2p_fields_info[iter].field_pos_in_interface;

            if (entry_handle->value_field_ids[field_pos] == DBAL_FIELD_EMPTY)
            {
                continue;
            }

            SHR_IF_ERR_EXIT(dbal_hl_pre_access_calculate(unit, entry_handle, &l2p_hl_info->l2p_fields_info[iter],
                                                         &is_condition_pass, &reg_array_offset, NULL, &field_offset,
                                                         &group_offset));

            if (is_condition_pass)
            {
                int nof_blocks;

                entry_handle->error_info.error_exists = 0;

                curr_l2p_info = &l2p_hl_info->l2p_fields_info[iter];
                curr_reg = (group_offset == -1) ? curr_l2p_info->reg[0] : curr_l2p_info->reg[group_offset];

                if (curr_reg == DBAL_HL_ACCESS_HW_ENTITY_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid register access, group offset[%d]\n", group_offset);
                }

                SHR_IF_ERR_EXIT(dbal_hl_access_block_calc
                                (unit, entry_handle, core_id, curr_reg,
                                 &curr_l2p_info->block_index_info, curr_l2p_info, 0, &block, &nof_blocks));

                if (((last_reg_used != curr_reg)) || (reg_array_offset != last_reg_array_offset)
                    || (block != last_block))
                {
                    if (last_reg_used != DBAL_HL_ACCESS_HW_ENTITY_INVALID)
                    {
                        SHR_IF_ERR_EXIT(dbal_hl_register_write
                                        (unit, entry_handle, last_l2p_info, data, last_block, last_nof_blocks,
                                         last_reg_array_offset, last_group_offset));

                        sal_memset(data, 0x0, DBAL_HL_MAX_REG_LINE_SIZE_IN_WORDS * sizeof(uint32));
                    }

                    DBAL_FIELD_START_LOG(curr_l2p_info->field_id, 0, curr_reg);

                    last_reg_used = curr_reg;
                    last_reg_array_offset = reg_array_offset;
                    last_block = block;
                    last_nof_blocks = nof_blocks;
                    last_group_offset = group_offset;
                    DBAL_REG_READ_LOG(last_reg_used, 0, last_block, last_reg_array_offset);
                    if (!entry_handle->overrun_entry)
                    {
                        SHR_IF_ERR_EXIT(dbal_hl_register_read_per_access(unit, entry_handle->table, last_reg_used,
                                                                         last_block, last_reg_array_offset, data));
                        DBAL_ACTION_PERFORMED_LOG;
                        DBAL_DATA_LOG(data, 0);
                    }
                    else
                    {
                        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Get skipped due to overrun flag")));
                    }
                }

                SHR_IF_ERR_EXIT(dbal_hl_field_to_hw_buffer_set
                                (unit, entry_handle, l2p_hl_info, last_reg_used, 0, field_offset, iter,
                                 entry_handle->phy_entry.payload, data, logger_enable));
            }
        }

        if (last_reg_used != DBAL_HL_ACCESS_HW_ENTITY_INVALID)
        {
            SHR_IF_ERR_EXIT(dbal_hl_register_write(unit, entry_handle, curr_l2p_info, data, last_block, last_nof_blocks,
                                                   last_reg_array_offset, last_group_offset));
        }
        core_id++;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_set_by_access_type(
    int unit,
    dbal_hard_logic_access_types_e access_type,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (access_type)
    {
        case DBAL_HL_ACCESS_MEMORY:
            SHR_IF_ERR_EXIT(dbal_hl_memory_set(unit, entry_handle, 1));
            break;
        case DBAL_HL_ACCESS_REGISTER:
            SHR_IF_ERR_EXIT(dbal_hl_register_set(unit, entry_handle));
            break;
        case DBAL_HL_ACCESS_SW:
            if (DBAL_HL_MUL_RES_INFO(access_type).num_of_access_fields)
            {

                entry_handle->error_info.error_exists = 0;
                SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_set(unit, entry_handle));
            }
            break;
        case DBAL_HL_ACCESS_UNMAPPED:
            SHR_IF_ERR_EXIT_NO_MSG(dbal_hl_unmapped_access_handle(unit, entry_handle, entry_handle->cur_res_type));
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int rv;
    int access_type, continue_regular_process;
    uint8 has_configuration = 0;
    dbal_logical_table_t *table = entry_handle->table;

    SHR_FUNC_INIT_VARS(unit);

    if (table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
    {
        SHR_IF_ERR_EXIT(dbal_hl_tcam_entry_get(unit, entry_handle));
    }
    else if (table->table_type == DBAL_TABLE_TYPE_DIRECT)
    {
        if (entry_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "result type not initialized for entry table %s\n", table->table_name);
        }

        DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(TRUE, bslSeverityInfo, DBAL_ACCESS_PRINTS_HL, 1);

        if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_HOOK_ACTIVE))
        {
            SHR_IF_ERR_EXIT(dbal_hl_hook_entry_get(unit, entry_handle, &continue_regular_process));
            if (!continue_regular_process)
            {

                DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_HL, 1);
                SHR_EXIT();
            }
        }

        for (access_type = 0; access_type < DBAL_NOF_HL_ACCESS_TYPES; access_type++)
        {
            if ((access_type == DBAL_HL_ACCESS_UNMAPPED) &&
                !SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_ASYMMETRIC))
            {
                continue;
            }
            if (table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[access_type].num_of_access_fields >
                0)
            {

                if (access_type == DBAL_HL_ACCESS_SW)
                {
                    has_configuration = 2;
                    DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_HL, 1);
                }
                else
                {
                    has_configuration = 1;
                }
                rv = dbal_hl_get_by_access_type(unit, access_type, entry_handle, DBAL_INVALID_PARAM);
                if (rv == _SHR_E_ACCESS)
                {
                    SHR_IF_ERR_EXIT_NO_MSG(rv);
                }
                SHR_IF_ERR_EXIT(rv);
            }
        }

        if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_HOOK_ACTIVE))
        {
            SHR_IF_ERR_EXIT(dbal_hl_hook_entry_decode(unit, entry_handle));
        }

        if (has_configuration != 2)
        {
            DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_HL, 1);
        }

        if (has_configuration == 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Missing configuration for log2phy in table %s \n", table->table_name);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported table type for hard logic table: %s. Table %s \n",
                     dbal_table_type_to_string(unit, table->table_type), table->table_name);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_hard_logic_access_types_e access_type;
    uint8 has_configuration = 0;
    dbal_logical_table_t *table = entry_handle->table;
    int continue_regular_process;

    SHR_FUNC_INIT_VARS(unit);

    if (table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
    {
        SHR_IF_ERR_EXIT(dbal_hl_tcam_entry_set(unit, entry_handle, 1));
    }
    else if (table->table_type == DBAL_TABLE_TYPE_DIRECT)
    {

        entry_handle->error_info.error_exists = 4;

        DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(TRUE, bslSeverityInfo, DBAL_ACCESS_PRINTS_HL, 0);

        if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_HOOK_ACTIVE))
        {
            SHR_IF_ERR_EXIT(dbal_hl_hook_entry_set(unit, entry_handle, &continue_regular_process));
            if (!continue_regular_process)
            {
                DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_HL, 0);
                SHR_EXIT();
            }
        }

        for (access_type = 0; access_type < DBAL_NOF_HL_ACCESS_TYPES; ++access_type)
        {
            if (table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[access_type].num_of_access_fields >
                0)
            {
                if (access_type == DBAL_HL_ACCESS_SW)
                {
                    has_configuration = 2;
                    DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_HL, 0);
                }
                else
                {
                    has_configuration = 1;
                }
                SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(dbal_hl_set_by_access_type(unit, access_type, entry_handle),
                                                   _SHR_E_ACCESS);
            }
        }

        if (has_configuration != 2)
        {
            DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_HL, 0);
        }

        if (has_configuration == 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Missing configuration for log2phy in table %s \n", table->table_name);
        }
        else
        {
            if (entry_handle->error_info.error_exists == 4)
            {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Key not mapped ERROR %s (the entry added not mapped to HW)\n"),
                           table->table_name));
                SHR_IF_ERR_EXIT(dbal_entry_print(unit, entry_handle, 1));
                SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
            }

        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported table type for hard logic table: %s. Table %s \n",
                     dbal_table_type_to_string(unit, table->table_type), table->table_name);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_entry_default_entry_build(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_handle_t * default_handle)
{
    int iter;
    dbal_table_field_info_t *fields_info;
    int res_type_idx, nof_fields;

    SHR_FUNC_INIT_VARS(unit);

    if (default_handle != NULL)
    {
        sal_memcpy(default_handle, entry_handle, sizeof(dbal_entry_handle_t));
        DBAL_ENTRY_HANDLE_KEY_FIELD_RANGES_COPY(*entry_handle, *default_handle);
    }
    else
    {
        default_handle = entry_handle;
    }

    for (iter = 0; iter < entry_handle->table->nof_key_fields; iter++)
    {
        default_handle->key_field_ids[iter] = entry_handle->table->keys_info[iter].field_id;
    }

    res_type_idx = entry_handle->cur_res_type;
    if ((SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
        && (default_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED))
    {
        int rv;
        rv = dbal_hl_res_type_resolution(unit, default_handle);
        if (rv == _SHR_E_NOT_FOUND)
        {

            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "result type not resolved\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
        else if (rv != _SHR_E_NONE)
        {
            SHR_EXIT();
        }
        res_type_idx = default_handle->cur_res_type;
    }

    if (entry_handle->table->table_type != DBAL_TABLE_TYPE_TCAM_DIRECT)
    {
        if ((entry_handle->table->table_type == DBAL_TABLE_TYPE_DIRECT)
            && (entry_handle->table->hl_mapping_multi_res[res_type_idx].is_default_non_standard))
        {
            SHR_IF_ERR_EXIT(dbal_hl_non_standard_default_entry_build
                            (unit, default_handle, default_handle->phy_entry.payload));
        }
        else
        {
            sal_memset(default_handle->phy_entry.payload, 0, DBAL_TABLE_BUFFER_IN_BYTES(default_handle->table));
        }
    }

    nof_fields = entry_handle->table->multi_res_info[res_type_idx].nof_result_fields;
    fields_info = entry_handle->table->multi_res_info[res_type_idx].results_info;
    default_handle->phy_entry.payload_size = entry_handle->table->multi_res_info[res_type_idx].entry_payload_size;
    default_handle->nof_result_fields = 0;

    for (iter = 0; iter < nof_fields; iter++)
    {
        if (fields_info[iter].permission == DBAL_PERMISSION_READONLY)
        {
            continue;
        }
        default_handle->nof_result_fields++;
        default_handle->value_field_ids[iter] = fields_info[iter].field_id;
    }

    if (entry_handle->table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
    {

        sal_memset(default_handle->phy_entry.payload, 0, DBAL_PHYSICAL_RES_SIZE_IN_BYTES);
        sal_memset(default_handle->phy_entry.key, 0, DBAL_PHYSICAL_KEY_SIZE_IN_BYTES);
        sal_memset(default_handle->phy_entry.k_mask, 0, DBAL_PHYSICAL_KEY_SIZE_IN_BYTES);

        fields_info = entry_handle->table->keys_info;
        nof_fields = entry_handle->table->nof_key_fields;
        default_handle->nof_key_fields = 0;
        for (iter = 0; iter < nof_fields; iter++)
        {
            default_handle->key_field_ids[iter] = fields_info[iter].field_id;
            default_handle->nof_key_fields++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_entry_handle_t *default_handle = NULL;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(default_handle, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    rv = dbal_hl_entry_default_entry_build(unit, entry_handle, default_handle);

    if (!rv)
    {

        if (default_handle->nof_result_fields > 0)
        {

            if (entry_handle->table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
            {
                SHR_IF_ERR_EXIT(dbal_hl_tcam_entry_set(unit, default_handle, 0));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_hl_entry_set(unit, default_handle));
            }
        }
    }
    else
    {
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_SET_CURRENT_ERR(rv);
        }
    }

exit:
    DBAL_ENTRY_HANDLE_FREE(default_handle);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    if ((entry_handle->table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
        && !dbal_hl_tcam_is_table_supports_half_entry(unit, entry_handle->table_id))
    {
        SHR_IF_ERR_EXIT(dbal_hl_tcam_table_clear(unit, entry_handle));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_clear_macro(unit, entry_handle->table_id, entry_handle->handle_id));

        iterator_info->mode = DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT;
        iterator_info->is_end = FALSE;
        iterator_info->used_first_key = FALSE;
        entry_handle->get_all_fields = 1;
        iterator_info->is_init = 1;

        SHR_IF_ERR_EXIT(dbal_iterator_init_handle_info(unit, entry_handle));
        SHR_IF_ERR_EXIT(dbal_hl_table_iterator_init(unit, entry_handle));

        SHR_IF_ERR_EXIT(dbal_hl_entry_get_next(unit, entry_handle));

        while (!iterator_info->is_end)
        {
            SHR_IF_ERR_EXIT(dbal_hl_entry_clear(unit, entry_handle));
            SHR_IF_ERR_EXIT(dbal_hl_entry_get_next(unit, entry_handle));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_table_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    if (entry_handle->table->table_type != DBAL_TABLE_TYPE_TCAM_DIRECT)
    {
        SHR_IF_ERR_EXIT(dbal_tables_max_key_value_get
                        (unit, entry_handle->table_id, &iterator_info->max_num_of_iterations));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int rv;
    dbal_key_value_validity_e is_valid_entry = DBAL_KEY_IS_INVALID;
    uint8 entry_found = FALSE;
    dbal_logical_table_t *table;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    table = entry_handle->table;

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    if (table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
    {
        SHR_IF_ERR_EXIT(dbal_hl_tcam_entry_get_next(unit, entry_handle));
    }
    else if (table->table_type == DBAL_TABLE_TYPE_DIRECT)
    {
        while ((!entry_found) && (!iterator_info->is_end))
        {

            entry_handle->error_info.error_exists = 0;
            if (iterator_info->used_first_key)
            {
                if (entry_handle->phy_entry.key[0] == iterator_info->max_num_of_iterations)
                {
                    entry_handle->core_id--;
                    if (entry_handle->core_id < 0)
                    {
                        break;
                    }
                    else
                    {
                        entry_handle->phy_entry.key[0] = 0;
                    }
                }
                else
                {
                    entry_handle->phy_entry.key[0]++;
                }
            }
            iterator_info->used_first_key = TRUE;
            is_valid_entry = DBAL_KEY_IS_VALID;
            if (table->allocator_field_id != DBAL_FIELD_EMPTY)
            {
                SHR_IF_ERR_EXIT(dbal_iterator_increment_by_allocator(unit, entry_handle, &is_valid_entry));
            }
            if (is_valid_entry == DBAL_KEY_IS_VALID)
            {
                SHR_IF_ERR_EXIT(dbal_key_buffer_validate(unit, entry_handle, &is_valid_entry));
            }

            if (is_valid_entry == DBAL_KEY_IS_VALID)
            {
                sal_memset(entry_handle->phy_entry.payload, 0, DBAL_PHYSICAL_RES_SIZE_IN_BYTES);
                entry_handle->get_all_fields = TRUE;
                if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
                {
                    entry_handle->cur_res_type = DBAL_RESULT_TYPE_NOT_INITIALIZED;

                    rv = dbal_hl_res_type_resolution(unit, entry_handle);
                    if (rv != _SHR_E_NONE)
                    {
                        if (rv == _SHR_E_NOT_FOUND)
                        {
                            continue;
                        }
                        else
                        {

                            if (((entry_handle->phy_entry.key[0] + 1) == iterator_info->max_num_of_iterations) &&
                                (entry_handle->core_id == 0))
                            {
                                iterator_info->is_end = TRUE;
                            }
                            SHR_ERR_EXIT(rv, "HL entry get");
                        }
                    }
                }
                else
                {
                    entry_handle->cur_res_type = 0;
                }

                rv = dbal_hl_entry_get(unit, entry_handle);
                if (rv == _SHR_E_NONE)
                {
                    int is_default;

                    if (iterator_info->mode == DBAL_ITER_MODE_ALL)
                    {
                        entry_found = TRUE;
                    }
                    else if ((iterator_info->mode == DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT) ||
                             (iterator_info->mode == DBAL_ITER_MODE_GET_NON_DEFAULT_PERMISSION_WRITE))
                    {
                        SHR_IF_ERR_EXIT(dbal_hl_direct_is_entry_default(unit, entry_handle, &is_default));
                        if (!is_default)
                        {
                            entry_found = TRUE;
                        }
                    }
                }
                else if (rv == _SHR_E_ACCESS)
                {

                    continue;
                }
                else if (rv != _SHR_E_NOT_FOUND)
                {

                    if (((entry_handle->phy_entry.key[0] + 1) == iterator_info->max_num_of_iterations) &&
                        (entry_handle->core_id == 0))
                    {
                        iterator_info->is_end = TRUE;
                    }
                    SHR_ERR_EXIT(rv, "HL entry get");
                }
            }
        }

        if (entry_found == FALSE)
        {
            iterator_info->is_end = TRUE;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported table type for hard logic table: %s. Table %s \n",
                     dbal_table_type_to_string(unit, table->table_type), table->table_name);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_range_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int num_of_entries)
{
    dbal_entry_handle_t *default_handle = NULL;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC_SET_ZERO(default_handle, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dbal_hl_entry_default_entry_build(unit, entry_handle, default_handle));
    SHR_IF_ERR_EXIT(dbal_hl_range_entry_set(unit, default_handle, num_of_entries));

exit:
    DBAL_ENTRY_HANDLE_FREE(default_handle);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_range_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int num_of_entries)
{
    int access_type;
    uint8 has_configuration = 0;
    dbal_logical_table_t *table = entry_handle->table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(TRUE, bslSeverityInfo, DBAL_ACCESS_PRINTS_HL, 0);

    for (access_type = 0; access_type < DBAL_NOF_HL_ACCESS_TYPES; access_type++)
    {
        if (table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[access_type].num_of_access_fields > 0)
        {
            has_configuration = 1;
            if (access_type == DBAL_HL_ACCESS_MEMORY)
            {
                SHR_IF_ERR_EXIT(dbal_hl_memory_set(unit, entry_handle, num_of_entries));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Range operation not supported for access_type %d, table %s\n",
                             access_type, table->table_name);
            }
        }
    }

    DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_HL, 0);

    if (has_configuration == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Missing configuration for log2phy in table %s \n", table->table_name);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_reg_default_mode_get(
    int unit,
    soc_reg_t reg,
    int *is_non_standard)
{
    soc_reg_above_64_val_t rval, rmsk;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(rval);
    SOC_REG_ABOVE_64_CLEAR(rmsk);
    SOC_REG_ABOVE_64_RST_VAL_GET(unit, reg, rval);
    SOC_REG_ABOVE_64_RST_MSK_GET(unit, reg, rmsk);

    SOC_REG_ABOVE_64_AND(rval, rmsk);

    (*is_non_standard) = 0;

    if (!SOC_REG_ABOVE_64_IS_ZERO(rval))
    {
        (*is_non_standard) = 1;
    }

    SHR_FUNC_EXIT;
}

#define BITS2LONGWORD(x) (((x) + 63) / 64)

#if defined(BCM_ACCESS_SUPPORT)
shr_error_e
dbal_hl_new_access_reg_default_mode_get(
    int unit,
    int reg,
    int *is_non_standard)
{
    int ii;
    const uint64 *default_value_pointer;
    uint32 mem_entry_size_bits;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_hl_access_memreg_size_get_per_access(unit, reg, 1, &mem_entry_size_bits));
    SHR_IF_ERR_EXIT(dbal_hl_new_access_reg_default_get(unit, reg, &default_value_pointer));

    for (ii = 0; ii < BITS2LONGWORD(mem_entry_size_bits); ++ii)
    {
        if (default_value_pointer[ii] != 0)
        {
            *is_non_standard = 1;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
#endif

shr_error_e
dbal_hl_reg_default_mode_get_per_access(
    int unit,
    int reg,
    int is_new_access,
    int *is_non_standard)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!is_new_access)
    {
        SHR_IF_ERR_EXIT(dbal_hl_reg_default_mode_get(unit, reg, is_non_standard));
    }
#if defined(BCM_ACCESS_SUPPORT)
    else
    {
        SHR_IF_ERR_EXIT(dbal_hl_new_access_reg_default_mode_get(unit, reg, is_non_standard));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_reg_default_get(
    int unit,
    soc_reg_t reg,
    uint32 *entry)
{
    soc_reg_above_64_val_t rval, rmsk;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(entry, 0x0, WORDS2BYTES(SOC_MAX_MEM_WORDS));

    SOC_REG_ABOVE_64_RST_VAL_GET(unit, reg, rval);
    SOC_REG_ABOVE_64_RST_MSK_GET(unit, reg, rmsk);

    SOC_REG_ABOVE_64_AND(rval, rmsk);

    sal_memcpy(entry, rval, WORDS2BYTES(DBAL_HL_MAX_REG_LINE_SIZE_IN_WORDS));

    SHR_FUNC_EXIT;
}
