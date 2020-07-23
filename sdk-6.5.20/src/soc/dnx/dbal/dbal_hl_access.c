/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "dbal_hl_access.h"
#include <soc/drv.h>    

shr_error_e dbal_hl_register_get_with_packed_fields(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int result_type_get);

shr_error_e dbal_hl_memory_get_with_packed_fields(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int result_type_get);


shr_error_e
dbal_hl_offset_calculate(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_fields_e mapped_field_id,
    uint32 default_val,
    dbal_offset_encode_info_t * encode_info,
    uint32 *calc_index)
{
    SHR_FUNC_INIT_VARS(unit);

    if (encode_info->formula != NULL)
    {
        if (encode_info->formula->cb)
        {
            SHR_IF_ERR_EXIT(encode_info->formula->cb(unit, entry_handle, mapped_field_id, calc_index));
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

static shr_error_e
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
        case DBAL_VALUE_FIELD_ENCODE_ADD:
        {
            uint32 input, output;
            input = *input_val;
            output = input + encode_param;
            *output_val = output;
            break;
        }
        case DBAL_VALUE_FIELD_ENCODE_SUBTRACT:
        case DBAL_VALUE_FIELD_ENCODE_DIVIDE:
        case DBAL_VALUE_FIELD_ENCODE_MODULO:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Encode type %s is not supported yet inside access\n",
                         dbal_field_encode_type_to_string(unit, encode_type));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal encoding type for access\n");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
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
        case DBAL_VALUE_FIELD_ENCODE_ADD:
        {
            uint32 input, output;
            input = *input_val;
            output = input - encode_param;
            *output_val = output;
            break;
        }
        case DBAL_VALUE_FIELD_ENCODE_SUBTRACT:
        case DBAL_VALUE_FIELD_ENCODE_DIVIDE:
        case DBAL_VALUE_FIELD_ENCODE_MODULO:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Encode type %s is not supported yet inside access\n",
                         dbal_field_encode_type_to_string(unit, encode_type));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal encoding type for access\n");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_reg_block_calc(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 core_id,
    soc_reg_t reg,
    dbal_offset_encode_info_t * block_index_info,
    dbal_fields_e mapped_field_id,
    uint32 *block,
    int *num_of_blocks)
{
    SHR_FUNC_INIT_VARS(unit);

    (*num_of_blocks) = 0;

        
    if (block_index_info->formula != NULL)
    {
        SHR_IF_ERR_EXIT(dbal_hl_offset_calculate(unit, entry_handle, mapped_field_id, 0, block_index_info, block));
        (*num_of_blocks) = 1;
    }
    else
    {

        if (core_id != DBAL_CORE_ALL)
        {
            (*num_of_blocks) = 0;

            
            
            (*num_of_blocks) = SOC_REG_BLOCK_NOF_INSTANCES(unit, reg);

            
            if (DBAL_MAX_NUM_OF_CORES != 1)
            {
                (*num_of_blocks) = ((*num_of_blocks) + 1) / DBAL_MAX_NUM_OF_CORES;
            }

            
            *block = core_id * (*num_of_blocks);
        }
        else
        {
            
            
            {
                (*num_of_blocks) = 1;
                (*block) = REG_PORT_ANY;
            }
            
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_mem_block_calc(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 core_id,
    soc_mem_t memory,
    dbal_offset_encode_info_t * block_index_info,
    dbal_fields_e mapped_field_id,
    int *block,
    int *num_of_blocks)
{
    uint32 calc_block = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (block_index_info->formula != NULL)
    {
        SHR_IF_ERR_EXIT(dbal_hl_offset_calculate
                        (unit, entry_handle, mapped_field_id, 0, block_index_info, &calc_block));

        (*block) = SOC_MEM_BLOCK_MIN(unit, memory) + calc_block;
        (*num_of_blocks) = 1;
    }
    else
    {
        if (core_id != DBAL_CORE_ALL)
        {
            (*num_of_blocks) = (SOC_MEM_BLOCK_MAX(unit, memory) - SOC_MEM_BLOCK_MIN(unit, memory) + 1);
            
            if (DBAL_MAX_NUM_OF_CORES != 1)
            {
                (*num_of_blocks) = ((*num_of_blocks) + 1) / DBAL_MAX_NUM_OF_CORES;
            }
            (*block) = SOC_MEM_BLOCK_MIN(unit, memory) + (core_id * (*num_of_blocks));
        }
        else
        {
            
            {
                (*num_of_blocks) = 1;
                (*block) = MEM_BLOCK_ANY; 
            }
            
        }
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
                SHR_IF_ERR_EXIT(dbal_hl_offset_calculate(unit, entry_handle, curr_l2p_info->field_id,
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
                SHR_IF_ERR_EXIT(dbal_hl_offset_calculate(unit, entry_handle, curr_l2p_info->field_id,
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
                SHR_IF_ERR_EXIT(dbal_hl_offset_calculate(unit, entry_handle, curr_l2p_info->field_id,
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
                SHR_IF_ERR_EXIT(dbal_hl_offset_calculate(unit, entry_handle, curr_l2p_info->field_id,
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
dbal_hl_field_to_hw_set(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_hl_l2p_info_t * l2p_hl_info,
    soc_mem_t memory,
    soc_reg_t reg,
    int field_offset,
    int field_pos,
    uint32 *src,
    uint32 *dst,
    int logger_enable)
{
    dbal_hl_l2p_field_info_t *curr_l2p_info = &(l2p_hl_info->l2p_fields_info[field_pos]);
    uint32 *field_data_ptr;
    uint32 field_data[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 };
    uint32 field_data_encoded[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 };

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

    if (curr_l2p_info->hw_field != INVALIDf)
    {
        
        int offset_in_hw = field_offset;
        int nof_fields;
        soc_field_info_t *fieldinfo;
        soc_field_info_t *fields_array;
        {
            if (memory != INVALIDm)
            {
                nof_fields = SOC_MEM_INFO(unit, memory).nFields;
                fields_array = SOC_MEM_INFO(unit, memory).fields;
            }
            else if (reg != INVALIDr)
            {
                nof_fields = SOC_REG_INFO(unit, reg).nFields;
                fields_array = SOC_REG_INFO(unit, reg).fields;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "memory/register must be set\n");
            }
            SOC_FIND_FIELD(curr_l2p_info->hw_field, fields_array, nof_fields, fieldinfo);

            if (fieldinfo)
            {
                offset_in_hw += fieldinfo->bp;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "mapping error for table %s, field %s,  hw_field %s does not exist in"
                             "register/memory %s\n",
                             dbal_logical_table_to_string(unit, entry_handle->table_id),
                             dbal_field_to_string(unit, curr_l2p_info->field_id),
                             SOC_FIELD_NAME(unit, curr_l2p_info->hw_field),
                             memory != INVALIDm ? SOC_MEM_NAME(unit, memory) : SOC_REG_NAME(unit, reg));
            }
        }

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_data_ptr, offset_in_hw,
                                                       curr_l2p_info->nof_bits_in_interface, dst));

        if (logger_enable)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U
                         (unit, "\twrite field %-25s to HWfield %-25s, offset in mem %d, nof_bits %d, value 0x%x\n"),
                         dbal_field_to_string(unit, curr_l2p_info->field_id), SOC_FIELD_NAME(unit,
                                                                                             curr_l2p_info->hw_field),
                         offset_in_hw, curr_l2p_info->nof_bits_in_interface, field_data_ptr[0]));
        }
    }
    else
    {
        if (logger_enable)
        {
            LOG_VERBOSE_EX(BSL_LOG_MODULE, "\twrite field %-25s, offset in mem %d, nof_bits %d, value 0x%x\n",
                           dbal_field_to_string(unit, curr_l2p_info->field_id), field_offset,
                           curr_l2p_info->nof_bits_in_interface, field_data_ptr[0]);
        }

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_data_ptr,
                                                       field_offset, curr_l2p_info->nof_bits_in_interface, dst));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_hl_field_from_hw_buffer_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_hl_l2p_info_t * l2p_hl_info,
    soc_mem_t memory,
    soc_reg_t reg,
    int field_offset,
    int field_pos,
    uint32 *src,
    uint32 *dst,
    int logger_enable)
{
    soc_field_info_t *fieldinfo = NULL;
    dbal_hl_l2p_field_info_t *curr_l2p_info = &(l2p_hl_info->l2p_fields_info[field_pos]);
    int offset_in_mem;
    uint32 *data_final_ptr;
    uint32 data_aligend[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 };
    int nof_fields;
    soc_field_info_t *fields_array;

    SHR_FUNC_INIT_VARS(unit);

    offset_in_mem = field_offset;

    
    if (curr_l2p_info->hw_field != INVALIDf)
    {
        if (memory != INVALIDm)
        {
            nof_fields = SOC_MEM_INFO(unit, memory).nFields;
            fields_array = SOC_MEM_INFO(unit, memory).fields;
        }
        else if (reg != INVALIDr)
        {
            nof_fields = SOC_REG_INFO(unit, reg).nFields;
            fields_array = SOC_REG_INFO(unit, reg).fields;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "memory/register must be set\n");
        }

        SOC_FIND_FIELD(curr_l2p_info->hw_field, fields_array, nof_fields, fieldinfo);

        if (fieldinfo)
        {
            offset_in_mem += fieldinfo->bp;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "mapping error for table %s, field %s:  hw_field %s does not exist in"
                         "register/memory %s\n",
                         dbal_logical_table_to_string(unit, entry_handle->table_id),
                         dbal_field_to_string(unit, curr_l2p_info->field_id),
                         SOC_FIELD_NAME(unit, curr_l2p_info->hw_field),
                         memory != INVALIDm ? SOC_MEM_NAME(unit, memory) : SOC_REG_NAME(unit, reg));
        }
    }

    data_final_ptr = src;
    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                    (src, offset_in_mem, (curr_l2p_info->nof_bits_in_interface), data_aligend));
    data_final_ptr = data_aligend;
    if (curr_l2p_info->hw_field != INVALIDf)
    {
        if (logger_enable)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U
                         (unit, "\tread field %-25s from HWfield %-25s, offset in mem %d nof_bits %d value 0x%x\n"),
                         dbal_field_to_string(unit, curr_l2p_info->field_id), SOC_FIELD_NAME(unit,
                                                                                             curr_l2p_info->hw_field),
                         offset_in_mem, curr_l2p_info->nof_bits_in_interface, data_final_ptr[0]));
        }
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
        if (curr_l2p_info->hw_field == INVALIDf)
        {
            if (logger_enable)
            {
                LOG_VERBOSE_EX(BSL_LOG_MODULE, "\tread field %-25s, offset in mem %d, nof_bits %d, value 0x%x\n",
                               dbal_field_to_string(unit, curr_l2p_info->field_id), curr_l2p_info->offset_in_interface,
                               offset_in_mem, data_final_ptr[0]);
            }
        }

    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_hl_non_standard_default_entry_build(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 *default_entry)
{
    uint32 default_val[SOC_MAX_MEM_WORDS] = { 0 };
    int field_pos, is_non_standard = 0;
    soc_mem_t memory = INVALIDm;
    soc_mem_t cur_memory = INVALIDm;
    soc_reg_t reg = INVALIDr;
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
            if (DBAL_HL_MEM_MUL_RES_INFO.l2p_fields_info[field_pos].alias_memory != INVALIDm)
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
                SHR_IF_ERR_EXIT(dbal_hl_mem_block_calc(unit, entry_handle, entry_handle->core_id, memory,
                                                       &(curr_l2p_info->block_index_info), curr_l2p_info->field_id,
                                                       &block, &num_of_blocks));

                SHR_IF_ERR_EXIT(dnx_init_mem_default_get(unit, memory, array_offset, block,
                                                         mem_entry_offset, default_val, NULL));

                SHR_IF_ERR_EXIT(dbal_hl_field_from_hw_buffer_get
                                (unit, entry_handle, &DBAL_HL_MEM_MUL_RES_INFO,
                                 DBAL_HL_MEM_MUL_RES_INFO.l2p_fields_info[field_pos].memory[0], INVALIDr, field_offset,
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
                uint32 block;
                int nof_blocks;
                
                SHR_IF_ERR_EXIT(dbal_hl_reg_block_calc
                                (unit, entry_handle, entry_handle->core_id, reg, &(curr_l2p_info->block_index_info),
                                 curr_l2p_info->field_id, &block, &nof_blocks));

                SHR_IF_ERR_EXIT(dbal_hl_reg_default_get(unit, reg, array_offset, block, default_val));

                SHR_IF_ERR_EXIT(dbal_hl_field_from_hw_buffer_get
                                (unit, entry_handle, &DBAL_HL_REG_MUL_RES_INFO, INVALIDm, reg, field_offset, field_pos,
                                 default_val, default_entry, 0));
            }
        }
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
    uint32 default_entry[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };
    uint32 payload[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };
    uint32 default_value[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 };
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    (*is_default) = 0;
    if ((entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].is_default_non_standard))
    {
        SHR_IF_ERR_EXIT(dbal_hl_non_standard_default_entry_build(unit, entry_handle, default_entry));
    }

    if (entry_handle->table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
    {
        if ((sal_memcmp(default_entry, entry_handle->phy_entry.payload, DBAL_PHYSICAL_RES_SIZE_IN_BYTES) == 0) &&
            (sal_memcmp(default_entry, entry_handle->phy_entry.key, DBAL_PHYSICAL_KEY_SIZE_IN_BYTES) == 0) &&
            (sal_memcmp(default_entry, entry_handle->phy_entry.k_mask, DBAL_PHYSICAL_KEY_SIZE_IN_BYTES) == 0))
        {
            (*is_default) = 1;
        }
    }
    else if (entry_handle->table->table_type == DBAL_TABLE_TYPE_DIRECT)
    {
        sal_memcpy(payload, entry_handle->phy_entry.payload, DBAL_TABLE_BUFFER_IN_BYTES(entry_handle->table));
                
        if (iterator_info->mode == DBAL_ITER_MODE_GET_NON_DEFAULT_PERMISSION_WRITE)
        {
            int iter;
            for (iter = 0; iter < entry_handle->table->multi_res_info[entry_handle->cur_res_type].nof_result_fields;
                 iter++)
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
                        entry_handle->table->multi_res_info[entry_handle->cur_res_type].
                        results_info[iter].field_nof_bits;
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
dbal_hl_get_by_access_type(
    int unit,
    dbal_hard_logic_access_types_e access_type,
    dbal_entry_handle_t * entry_handle,
    int result_type)
{
    uint8 is_packed_fields = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    
    if (result_type != DBAL_INVALID_PARAM)
    {
        is_packed_fields =
            entry_handle->table->hl_mapping_multi_res[result_type].l2p_hl_info[access_type].is_packed_fields;
    }
    else
    {
        is_packed_fields = DBAL_HL_MUL_RES_INFO(access_type).is_packed_fields;
    }

    if (is_packed_fields)
    {
        
        switch (access_type)
        {
            case DBAL_HL_ACCESS_REGISTER:
                SHR_IF_ERR_EXIT(dbal_hl_register_get_with_packed_fields(unit, entry_handle, result_type));
                break;
            case DBAL_HL_ACCESS_MEMORY:
                SHR_IF_ERR_EXIT(dbal_hl_memory_get_with_packed_fields(unit, entry_handle, result_type));
                break;
            default:
                break;
        }
    }
    else
    {
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
            default:
                break;
        }
    }

exit:
    SHR_FUNC_EXIT;

}


shr_error_e
dbal_hl_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int access_type, jj;
    int curr_res_type = 0;
    uint32 field_value[1] = { 0 };
    dbal_logical_table_t *table = entry_handle->table;
    dbal_table_field_info_t table_field_info;

    SHR_FUNC_INIT_VARS(unit);

    dbal_logger_internal_disable_set(unit);

    if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_HOOK_ACTIVE))
    {
        SHR_IF_ERR_EXIT(dbal_hl_hook_res_type_resolution(unit, entry_handle));
        SHR_EXIT();
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
            if (table->hl_mapping_multi_res[curr_res_type].l2p_hl_info[access_type].num_of_access_fields > 0)
            {
                SHR_IF_ERR_EXIT(dbal_hl_get_by_access_type(unit, access_type, entry_handle, curr_res_type));
            }
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
dbal_hl_memory_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int result_type_get)
{
    int iter;
    soc_mem_t last_memory_used = INVALIDm, curr_mem;
    int last_mem_entry_offset = 0;
    int last_mem_array_offset_used = 0;
    uint32 mem_entry_offset = 0, field_offset = 0, mem_array_offset = 0, alias_offset, group_offset;
    uint8 is_condition_pass = 0;
    uint32 data[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 };
    uint32 core_id = DBAL_CORE_DEFAULT;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_hl_l2p_info_t *l2p_hl_info;
    dbal_hl_l2p_field_info_t *curr_l2p_info;
    int block = MEM_BLOCK_ANY;
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
                soc_mem_t memory_to_read =
                    (curr_l2p_info->alias_memory != INVALIDm) ? curr_l2p_info->alias_memory : curr_mem;
                last_memory_used = curr_mem;
                last_mem_array_offset_used = mem_array_offset;
                last_mem_entry_offset = mem_entry_offset;
                sal_memset(data, 0x0, DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_BYTES);
                alias_offset = 0;

                SHR_IF_ERR_EXIT(dbal_hl_mem_block_calc(unit, entry_handle, core_id, last_memory_used,
                                                       &(curr_l2p_info->block_index_info), curr_l2p_info->field_id,
                                                       &block, &num_of_blocks));

                if ((curr_l2p_info->alias_memory != INVALIDm)
                    && (curr_l2p_info->alias_data_offset_info.formula != NULL))
                {
                    SHR_IF_ERR_EXIT(dbal_hl_offset_calculate(unit, entry_handle, curr_l2p_info->field_id, 0,
                                                             &(curr_l2p_info->alias_data_offset_info), &alias_offset));
                }

                if (alias_offset != 0)
                {
                    uint32 data_for_alias[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 };

                    DBAL_MEM_READ_LOG(memory_to_read, mem_entry_offset, block, last_mem_array_offset_used);

                    SHR_IF_ERR_EXIT_WITH_LOG(soc_mem_array_read(unit, memory_to_read, last_mem_array_offset_used,
                                                                block, mem_entry_offset, data_for_alias),
                                             "Failed reading memory %s port=%d arr_ind=%d",
                                             SOC_MEM_NAME(unit, memory_to_read), block, last_mem_array_offset_used);
                    DBAL_ACTION_PERFORMED_LOG;
                    DBAL_DATA_LOG(data_for_alias, 1);
                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Aliasing, offset = %d, "), alias_offset));

                    
                    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(data_for_alias, alias_offset,
                                                                   curr_l2p_info->
                                                                   alias_data_offset_info.internal_inparam, data));
                    DBAL_DATA_LOG(data, 1);
                }
                else
                {
                    DBAL_MEM_READ_LOG(memory_to_read, mem_entry_offset, block, last_mem_array_offset_used);
                    SHR_IF_ERR_EXIT_WITH_LOG(soc_mem_array_read(unit, memory_to_read,
                                                                last_mem_array_offset_used, block, mem_entry_offset,
                                                                data), "Failed reading memory %s port=%d arr_ind=%d",
                                             SOC_MEM_NAME(unit, memory_to_read), block, last_mem_array_offset_used);
                    DBAL_ACTION_PERFORMED_LOG;
                    DBAL_DATA_LOG(data, 1);

                }
            }

            SHR_IF_ERR_EXIT(dbal_hl_field_from_hw_buffer_get
                            (unit, entry_handle, l2p_hl_info, curr_mem, INVALIDr, field_offset, iter, data,
                             entry_handle->phy_entry.payload, logger_enable));
        }
    }

    entry_handle->cur_res_type = orig_result_type;

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
    int block = MEM_BLOCK_ANY;
    int is_array_fill_needed = 0;
    int array_index;
    soc_mem_t memory;
    int nof_group_elements = 1;
    int logger_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_HL_LOGGER_STATUS_UPDATE;

    if (!l2p_info)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "l2P not initialized\n");
    }

    if (group_offset != -1)
    {
        nof_group_elements = 1;
    }
    else
    {
        if (l2p_info->hw_entity_group_id != DBAL_HW_ENTITY_GROUP_EMPTY)
        {
            nof_group_elements = DBAL_MAX_NUMBER_OF_HW_ELEMENTS;
        }
    }

    for (ii = 0; ii < nof_group_elements; ii++)
    {
        is_array_fill_needed = 0;
        memory = l2p_info->memory[(group_offset == -1 ? ii : group_offset)];

        if (memory == INVALIDm)
        {
            break;
        }
        if (l2p_info->alias_memory != INVALIDm)
        {
            memory = l2p_info->alias_memory;
            if (nof_group_elements > 1)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "unsupported alias and memory group\n");
            }
        }
        SHR_IF_ERR_EXIT(dbal_hl_mem_block_calc(unit, entry_handle, core_id, memory,
                                               &(l2p_info->block_index_info), l2p_info->field_id, &block,
                                               &num_of_blocks));

        
        if (l2p_info->array_offset_info.formula == NULL)
        {
            array_offset = 0;
            if (SOC_MEM_IS_ARRAY(unit, memory))
            {
                is_array_fill_needed = 1;
            }
        }

        for (block_iter = 0; block_iter < num_of_blocks; block_iter++)
        {
            if (is_array_fill_needed)
            {
                int nof_elm = SOC_MEM_NUMELS(unit, memory);
                DBAL_MEM_WRITE_ARRAY_LOG(memory, entry_offset, block, nof_elm);
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n\tarray indexes: ")));

                for (array_index = SOC_MEM_FIRST_ARRAY_INDEX(unit, memory);
                     array_index < nof_elm + SOC_MEM_FIRST_ARRAY_INDEX(unit, memory); array_index++)
                {
                    if (num_of_entries > 1)
                    {
                        uint32 last_entry = (entry_offset - 1) + num_of_entries;
                        LOG_VERBOSE(BSL_LOG_MODULE,
                                    (BSL_META_U(unit, "%d nof entries %d, "), array_index, num_of_entries));
                        SHR_IF_ERR_EXIT(sand_fill_partial_table_with_entry
                                        (unit, memory, array_index, array_index, block, entry_offset,
                                         last_entry, data));
                    }
                    else
                    {
                        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%d, "), array_index));
                        SHR_IF_ERR_EXIT_WITH_LOG(soc_mem_array_write
                                                 (unit, memory, array_index, block, entry_offset, data),
                                                 "Failed writing memory %s port=%d arr_ind=%d", SOC_MEM_NAME(unit,
                                                                                                             memory),
                                                 block, array_index);
                    }
                }
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
                DBAL_ACTION_PERFORMED_LOG;
            }
            else
            {
                            
                DBAL_MEM_WRITE_LOG(memory, entry_offset, block, array_offset);
                if (num_of_entries > 1)
                {
                    uint32 last_entry = (entry_offset - 1) + num_of_entries;
                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "num of entries %d \n"), num_of_entries));
                    SHR_IF_ERR_EXIT(sand_fill_partial_table_with_entry(unit, memory, array_offset, array_offset, block,
                                                                       entry_offset, last_entry, data));
                }
                else
                {
                    SHR_IF_ERR_EXIT_WITH_LOG(soc_mem_array_write(unit, memory, array_offset, block, entry_offset, data),
                                             "Failed writing memory %s block %s arr_ind=%d", SOC_MEM_NAME(unit, memory),
                                             SOC_BLOCK_NAME(unit, block), array_offset);
                }
                DBAL_ACTION_PERFORMED_LOG;
            }
            block++;
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
        soc_mem_t last_memory_used = INVALIDm, curr_mem;
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
        int block = MEM_BLOCK_ANY, num_of_blocks;

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

                if (curr_mem == INVALIDm)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid memory access, group offset[%d]\n", group_offset);
                }

                
                if ((last_memory_used != curr_mem) ||
                    (last_mem_offset_used != mem_entry_offset) || (last_mem_array_offset_used != mem_array_offset))
                {
                    soc_mem_t memory_to_read =
                        (curr_l2p_info->alias_memory != INVALIDm) ? curr_l2p_info->alias_memory : curr_mem;

                    if (last_memory_used != INVALIDm)
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
                    block = MEM_BLOCK_ANY;
                    SHR_IF_ERR_EXIT(dbal_hl_mem_block_calc(unit, entry_handle, core_id, last_memory_used,
                                                           &(curr_l2p_info->block_index_info), curr_l2p_info->field_id,
                                                           &block, &num_of_blocks));

                    
                    if (!entry_handle->overrun_entry)
                    {
                        DBAL_MEM_READ_LOG(memory_to_read, mem_entry_offset, block, last_mem_array_offset_used);

                        SHR_IF_ERR_EXIT_WITH_LOG(soc_mem_array_read
                                                 (unit, memory_to_read, mem_array_offset, block, mem_entry_offset,
                                                  data), "Failed reading memory %s port=%d arr_ind=%d",
                                                 SOC_MEM_NAME(unit, memory_to_read), block, mem_array_offset);
                        DBAL_ACTION_PERFORMED_LOG;
                        DBAL_DATA_LOG(data, 1);
                    }

                    alias_offset = 0;
                    if (curr_l2p_info->alias_memory != INVALIDm)
                    {
                        SHR_IF_ERR_EXIT(dbal_hl_offset_calculate(unit, entry_handle, curr_l2p_info->field_id, 0,
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

                SHR_IF_ERR_EXIT(dbal_hl_field_to_hw_set
                                (unit, entry_handle, l2p_hl_info, last_memory_used, INVALIDr, field_offset, iter,
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


shr_error_e
dbal_hl_register_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int result_type_get)
{
    int iter;
    soc_reg_t last_reg_used = INVALIDr, curr_reg;
    uint32 field_offset = 0, last_reg_array_offset = 0, reg_array_offset = 0, group_offset;
    uint8 is_condition_pass = 0;
    uint32 data[SOC_REG_ABOVE_64_MAX_SIZE_U32] = { 0 };
    uint32 block = REG_PORT_ANY, last_block = REG_PORT_ANY;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_hl_l2p_info_t *l2p_hl_info;
    dbal_hl_l2p_field_info_t *curr_l2p_info = NULL;
    int result_type = entry_handle->cur_res_type;
    uint8 acc_type;
    int blk;
    uint32 addr, val32;
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

            
            SHR_IF_ERR_EXIT(dbal_hl_reg_block_calc
                            (unit, entry_handle, entry_handle->core_id, curr_reg, &(curr_l2p_info->block_index_info),
                             curr_l2p_info->field_id, &block, &nof_blocks));

            if ((last_reg_used != curr_reg) || (reg_array_offset != last_reg_array_offset) || (block != last_block))
            {
                last_reg_used = curr_reg;
                last_reg_array_offset = reg_array_offset;
                last_block = block;

                sal_memset(data, 0x0, SOC_REG_ABOVE_64_MAX_SIZE_U32 * sizeof(uint32));
                DBAL_REG_READ_LOG(last_reg_used, 0, last_block, last_reg_array_offset);

                if (soc_cpureg == SOC_REG_INFO(unit, last_reg_used).regtype)
                {
                    addr = soc_reg_addr_get(unit, last_reg_used, last_block, last_reg_array_offset,
                                            SOC_REG_ADDR_OPTION_NONE, &blk, &acc_type);
                    val32 = soc_pci_read(unit, addr);
                    data[0] = val32;
                }
                else if (soc_customreg == SOC_REG_INFO(unit, last_reg_used).regtype)
                {
                    SHR_IF_ERR_EXIT_WITH_LOG(soc_custom_reg_above_64_get
                                             (unit, last_reg_used, last_block, last_reg_array_offset, data),
                                             "Failed reading custom register %s port=%d arr_ind=%d", SOC_REG_NAME(unit,
                                                                                                                  last_reg_used),
                                             last_block, last_reg_array_offset);
                }
                else
                {
                    SHR_IF_ERR_EXIT_WITH_LOG(soc_reg_above_64_get
                                             (unit, last_reg_used, last_block, last_reg_array_offset, data),
                                             "Failed reading register %s port=%d arr_ind=%d", SOC_REG_NAME(unit,
                                                                                                           last_reg_used),
                                             last_block, last_reg_array_offset);
                }
                DBAL_ACTION_PERFORMED_LOG;
                DBAL_DATA_LOG(data, 0);
            }

            SHR_IF_ERR_EXIT(dbal_hl_field_from_hw_buffer_get
                            (unit, entry_handle, l2p_hl_info, INVALIDm, last_reg_used, field_offset, iter, data,
                             entry_handle->phy_entry.payload, logger_enable));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_register_write(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_hl_l2p_field_info_t * l2p_info,
    uint32 *data,
    uint32 block,
    int nof_blocks,
    int array_offset,
    int group_offset)
{
    int ii, block_indx;
    soc_reg_t reg;
    int all_reg_in_group = 0;
    int logger_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (!l2p_info)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "l2P not initialized\n");
    }

    DBAL_HL_LOGGER_STATUS_UPDATE;

    
    if ((block == REG_PORT_ANY) && (nof_blocks != 1))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal block params block=reg_port_any, nof_blocks = %d\n", nof_blocks);
    }

    if (group_offset != -1)
    {
        reg = l2p_info->reg[group_offset];
    }
    else
    {
        reg = l2p_info->reg[0];
        if (l2p_info->hw_entity_group_id != DBAL_HW_ENTITY_GROUP_EMPTY)
        {
            
            all_reg_in_group = 1;
        }
    }

    
    for (block_indx = 0; block_indx < nof_blocks; block_indx++)
    {
        
        if ((l2p_info->array_offset_info.formula == NULL) && SOC_REG_IS_ARRAY(unit, reg))
        {
            int array_iter;

            DBAL_REG_WRITE_ARRAY_LOG(reg, 0, block, SOC_REG_NUMELS(unit, reg));
            for (array_iter = 0; array_iter < SOC_REG_NUMELS(unit, reg); array_iter++)
            {
                if (block == REG_PORT_ANY)
                {
                    SHR_IF_ERR_EXIT_WITH_LOG(soc_reg_above_64_set_all_instances
                                             (unit, reg,
                                              array_iter + SOC_REG_FIRST_ARRAY_INDEX(unit, reg), data),
                                             "Failed writing register %s arr_ind=%d%s",
                                             SOC_REG_NAME(unit, reg), array_iter + SOC_REG_FIRST_ARRAY_INDEX(unit, reg),
                                             "");
                }
                else
                {
                    SHR_IF_ERR_EXIT_WITH_LOG(soc_reg_above_64_set
                                             (unit, reg, block, array_iter + SOC_REG_FIRST_ARRAY_INDEX(unit, reg),
                                              data), "Failed writing register %s port=%d arr_ind=%d", SOC_REG_NAME(unit,
                                                                                                                   reg),
                                             block, array_iter + SOC_REG_FIRST_ARRAY_INDEX(unit, reg));
                }
            }
            DBAL_ACTION_PERFORMED_LOG;

        }
        else
        {
            DBAL_REG_WRITE_LOG(reg, 0, block, array_offset);
            if (soc_cpureg == SOC_REG_INFO(unit, reg).regtype)
            {
                soc_pci_write(unit, SOC_REG_INFO(unit, reg).offset, data[0]);
            }
            else if (soc_customreg == SOC_REG_INFO(unit, reg).regtype)
            {
                SHR_IF_ERR_EXIT_WITH_LOG(soc_custom_reg_above_64_set(unit, reg, block, array_offset, data),
                                         "Failed writing register %s port=%d arr_ind=%d",
                                         SOC_REG_NAME(unit, reg), block, array_offset);
            }
            else
            {
                if (block == REG_PORT_ANY)
                {
                    SHR_IF_ERR_EXIT_WITH_LOG(soc_reg_above_64_set_all_instances
                                             (unit, reg, array_offset, data),
                                             "Failed writing register %s array offset=%d%s",
                                             SOC_REG_NAME(unit, reg), array_offset, "");
                }
                else
                {
                    SHR_IF_ERR_EXIT_WITH_LOG(soc_reg_above_64_set(unit, reg, block, array_offset, data),
                                             "Failed writing register %s port=%d arr_ind=%d",
                                             SOC_REG_NAME(unit, reg), block, array_offset);
                }
            }
            DBAL_ACTION_PERFORMED_LOG;
        }

        if (block != REG_PORT_ANY)
        {
            block++;
        }
    }

    
    if (all_reg_in_group)
    {
        for (ii = 1; ii < DBAL_MAX_NUMBER_OF_HW_ELEMENTS; ii++)
        {
            reg = l2p_info->reg[ii];
            if (l2p_info->reg[ii] != INVALIDr)
            {
                SHR_IF_ERR_EXIT(dbal_hl_reg_block_calc(unit, entry_handle, entry_handle->core_id, reg,
                                                       &(l2p_info->block_index_info), l2p_info->field_id,
                                                       &block, &nof_blocks));
                for (block_indx = 0; block_indx < nof_blocks; block_indx++)
                {
                    if ((l2p_info->array_offset_info.formula == NULL) && SOC_REG_IS_ARRAY(unit, reg))
                    {
                        int array_iter;

                        DBAL_REG_WRITE_ARRAY_LOG(reg, 0, block, SOC_REG_NUMELS(unit, reg))
                            for (array_iter = 0; array_iter < SOC_REG_NUMELS(unit, reg); array_iter++)
                        {
                            if (soc_cpureg == SOC_REG_INFO(unit, reg).regtype)
                            {
                                soc_pci_write(unit, SOC_REG_INFO(unit, reg).offset, data[0]);
                            }
                            else if (soc_customreg == SOC_REG_INFO(unit, reg).regtype)
                            {
                                SHR_IF_ERR_EXIT_WITH_LOG(soc_custom_reg_above_64_set
                                                         (unit, reg, block,
                                                          array_iter + SOC_REG_FIRST_ARRAY_INDEX(unit, reg), data),
                                                         "Failed writing register %s port=%d arr_ind=%d",
                                                         SOC_REG_NAME(unit, reg), block,
                                                         array_iter + SOC_REG_FIRST_ARRAY_INDEX(unit, reg));
                            }
                            else
                            {
                                if (block == REG_PORT_ANY)
                                {
                                    SHR_IF_ERR_EXIT_WITH_LOG(soc_reg_above_64_set_all_instances
                                                             (unit, reg,
                                                              array_iter + SOC_REG_FIRST_ARRAY_INDEX(unit, reg), data),
                                                             "Failed writing register %s arr_ind=%d%s",
                                                             SOC_REG_NAME(unit, reg),
                                                             array_iter + SOC_REG_FIRST_ARRAY_INDEX(unit, reg), "");
                                }
                                else
                                {
                                    SHR_IF_ERR_EXIT_WITH_LOG(soc_reg_above_64_set
                                                             (unit, reg, block,
                                                              array_iter + SOC_REG_FIRST_ARRAY_INDEX(unit, reg), data),
                                                             "Failed writing register %s port=%d arr_ind=%d",
                                                             SOC_REG_NAME(unit, reg), block,
                                                             array_iter + SOC_REG_FIRST_ARRAY_INDEX(unit, reg));
                                }
                            }
                        }
                        DBAL_ACTION_PERFORMED_LOG;
                    }
                    else
                    {
                        if (block == REG_PORT_ANY)
                        {
                            SHR_IF_ERR_EXIT_WITH_LOG(soc_reg_above_64_set_all_instances
                                                     (unit, reg, array_offset, data),
                                                     "Failed writing register %s arr_off=%d%s",
                                                     SOC_REG_NAME(unit, reg), array_offset, "");
                        }
                        else
                        {
                            SHR_IF_ERR_EXIT_WITH_LOG(soc_reg_above_64_set(unit, reg, block, array_offset, data),
                                                     "Failed writing register %s port=%d arr_ind=%d",
                                                     SOC_REG_NAME(unit, reg), block, array_offset);
                        }
                        DBAL_ACTION_PERFORMED_LOG;
                    }
                    if (block != REG_PORT_ANY)
                    {
                        block++;
                    }
                }
            }
            else
            {
                break;
            }
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
        soc_reg_t last_reg_used = INVALIDr, curr_reg;
        uint32 reg_array_offset = 0, last_reg_array_offset = 0, field_offset = 0, group_offset = 0, last_group_offset =
            0;
        uint8 is_condition_pass = 0;
        uint32 data[SOC_REG_ABOVE_64_MAX_SIZE_U32] = { 0 };
        uint32 block = REG_PORT_ANY, last_block = REG_PORT_ANY;
        int last_nof_blocks = 0;
        dbal_hl_l2p_info_t *l2p_hl_info =
            &table->hl_mapping_multi_res[result_type].l2p_hl_info[DBAL_HL_ACCESS_REGISTER];
        dbal_hl_l2p_field_info_t *curr_l2p_info = NULL;
        dbal_hl_l2p_field_info_t *last_l2p_info = NULL;
        uint8 acc_type;
        int blk;
        uint32 addr, val32;

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

                if (curr_reg == INVALIDm)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid register access, group offset[%d]\n", group_offset);
                }

                
                SHR_IF_ERR_EXIT(dbal_hl_reg_block_calc
                                (unit, entry_handle, core_id, curr_reg, &(curr_l2p_info->block_index_info),
                                 curr_l2p_info->field_id, &block, &nof_blocks));

                if (((last_reg_used != curr_reg)) || (reg_array_offset != last_reg_array_offset)
                    || (block != last_block))
                {
                    if (last_reg_used != INVALIDr)
                    {
                        SHR_IF_ERR_EXIT(dbal_hl_register_write
                                        (unit, entry_handle, last_l2p_info, data, last_block, last_nof_blocks,
                                         last_reg_array_offset, last_group_offset));

                        sal_memset(data, 0x0, SOC_REG_ABOVE_64_MAX_SIZE_U32 * sizeof(uint32));
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

                        if (soc_cpureg == SOC_REG_INFO(unit, last_reg_used).regtype)
                        {
                            addr = soc_reg_addr_get(unit, last_reg_used, last_block, last_reg_array_offset,
                                                    SOC_REG_ADDR_OPTION_NONE, &blk, &acc_type);
                            val32 = soc_pci_read(unit, addr);
                            data[0] = val32;
                        }
                        else if (soc_customreg == SOC_REG_INFO(unit, last_reg_used).regtype)
                        {
                            SHR_IF_ERR_EXIT_WITH_LOG(soc_custom_reg_above_64_get
                                                     (unit, last_reg_used, last_block, last_reg_array_offset, data),
                                                     "Failed reading register %s port=%d arr_ind=%d",
                                                     SOC_REG_NAME(unit, last_reg_used), last_block,
                                                     last_reg_array_offset);
                            DBAL_ACTION_PERFORMED_LOG;
                            DBAL_DATA_LOG(data, 0);
                        }
                        else
                        {
                            SHR_IF_ERR_EXIT_WITH_LOG(soc_reg_above_64_get
                                                     (unit, last_reg_used, last_block, last_reg_array_offset, data),
                                                     "Failed reading register %s port=%d arr_ind=%d",
                                                     SOC_REG_NAME(unit, last_reg_used), last_block,
                                                     last_reg_array_offset);
                            DBAL_ACTION_PERFORMED_LOG;
                            DBAL_DATA_LOG(data, 0);
                        }
                    }
                    else
                    {
                        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Get skipped due to overrun flag")));
                    }
                }

                SHR_IF_ERR_EXIT(dbal_hl_field_to_hw_set
                                (unit, entry_handle, l2p_hl_info, INVALIDm, last_reg_used, field_offset, iter,
                                 entry_handle->phy_entry.payload, data, logger_enable));
            }
        }

        if (last_reg_used != INVALIDr)
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
dbal_hl_register_get_with_packed_fields(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int result_type_get)
{
    soc_reg_t curr_reg;
    uint32 field_offset = 0, reg_array_offset = 0, group_offset;
    uint8 is_condition_pass = 0;
    uint32 block = REG_PORT_ANY;
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

    l2p_hl_info = &table->hl_mapping_multi_res[result_type].l2p_hl_info[DBAL_HL_ACCESS_REGISTER];

    curr_l2p_info = &l2p_hl_info->l2p_fields_info[0];

    SHR_IF_ERR_EXIT(dbal_hl_pre_access_calculate(unit, entry_handle, curr_l2p_info, &is_condition_pass,
                                                 &reg_array_offset, NULL, &field_offset, &group_offset));

    if (is_condition_pass)
    {
        int nof_blocks;

        curr_reg = (group_offset == -1) ? curr_l2p_info->reg[0] : curr_l2p_info->reg[group_offset];
        DBAL_PACKED_START_LOG(0, curr_reg);

        
        SHR_IF_ERR_EXIT(dbal_hl_reg_block_calc
                        (unit, entry_handle, entry_handle->core_id, curr_reg, &(curr_l2p_info->block_index_info),
                         curr_l2p_info->field_id, &block, &nof_blocks));

        DBAL_REG_READ_LOG(curr_reg, 0, block, reg_array_offset);

        SHR_IF_ERR_EXIT_WITH_LOG(soc_reg_above_64_get
                                 (unit, curr_reg, block, reg_array_offset, entry_handle->phy_entry.payload),
                                 "Failed reading register %s port=%d arr_ind=%d", SOC_REG_NAME(unit, curr_reg), block,
                                 reg_array_offset);
        DBAL_ACTION_PERFORMED_LOG;
        DBAL_DATA_LOG(entry_handle->phy_entry.payload, 0);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_hl_memory_get_with_packed_fields(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int result_type_get)
{
    soc_mem_t curr_mem;
    uint32 mem_entry_offset = 0, field_offset = 0, mem_array_offset = 0, alias_offset, group_offset;
    uint8 is_condition_pass = 0;
    uint32 core_id = DBAL_CORE_DEFAULT;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_hl_l2p_info_t *l2p_hl_info;
    dbal_hl_l2p_field_info_t *curr_l2p_info;
    int block = MEM_BLOCK_ANY;
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

    curr_l2p_info = &l2p_hl_info->l2p_fields_info[0];

    SHR_IF_ERR_EXIT(dbal_hl_pre_access_calculate(unit, entry_handle, curr_l2p_info,
                                                 &is_condition_pass, &mem_array_offset, &mem_entry_offset,
                                                 &field_offset, &group_offset));

    if (is_condition_pass)
    {
        int num_of_blocks;
        soc_mem_t memory_to_read;

        curr_mem = (group_offset == -1) ? curr_l2p_info->memory[0] : curr_l2p_info->memory[group_offset];
        DBAL_PACKED_START_LOG(1, curr_mem);

        
        memory_to_read = (curr_l2p_info->alias_memory != INVALIDm) ? curr_l2p_info->alias_memory : curr_mem;

        alias_offset = 0;

        SHR_IF_ERR_EXIT(dbal_hl_mem_block_calc(unit, entry_handle, core_id, curr_mem,
                                               &(curr_l2p_info->block_index_info), curr_l2p_info->field_id,
                                               &block, &num_of_blocks));

        if (curr_l2p_info->alias_memory != INVALIDm)
        {
            SHR_IF_ERR_EXIT(dbal_hl_offset_calculate(unit, entry_handle, curr_l2p_info->field_id, 0,
                                                     &(curr_l2p_info->alias_data_offset_info), &alias_offset));

        }

        if (alias_offset != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "in table %s, alias offset is not supported with packedFields.\n", table->table_name);
        }
        else
        {
            DBAL_MEM_READ_LOG(memory_to_read, mem_entry_offset, block, mem_array_offset);
            SHR_IF_ERR_EXIT_WITH_LOG(soc_mem_array_read(unit, memory_to_read,
                                                        mem_array_offset, block, mem_entry_offset,
                                                        entry_handle->phy_entry.payload),
                                     "Failed reading memory %s port=%d arr_ind=%d",
                                     SOC_MEM_NAME(unit, memory_to_read), block, mem_array_offset);
            DBAL_ACTION_PERFORMED_LOG;
            DBAL_DATA_LOG(entry_handle->phy_entry.payload, 1);

        }
    }

    entry_handle->cur_res_type = orig_result_type;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_hl_memory_set_with_packed_fields(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int num_of_entries)
{
    soc_mem_t curr_mem;
    uint32 mem_entry_offset = 0, field_offset = 0, mem_array_offset = 0, group_offset = 0;
    uint8 is_condition_pass = 0;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_hl_l2p_info_t *l2p_hl_info;
    dbal_hl_l2p_field_info_t *curr_l2p_info = NULL;
    int block = MEM_BLOCK_ANY, num_of_blocks;
    int result_type = entry_handle->cur_res_type;
    uint32 core_id = entry_handle->core_id;
    int logger_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_HL_LOGGER_STATUS_UPDATE;

    l2p_hl_info = &table->hl_mapping_multi_res[result_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY];
    curr_l2p_info = &l2p_hl_info->l2p_fields_info[0];

    
    SHR_IF_ERR_EXIT(dbal_hl_pre_access_calculate(unit, entry_handle, curr_l2p_info,
                                                 &is_condition_pass, &mem_array_offset, &mem_entry_offset,
                                                 &field_offset, &group_offset));
    if (is_condition_pass)
    {
        curr_mem = (group_offset == -1) ? curr_l2p_info->memory[0] : curr_l2p_info->memory[group_offset];
        DBAL_PACKED_START_LOG(1, curr_mem);

        
        block = MEM_BLOCK_ANY;
        SHR_IF_ERR_EXIT(dbal_hl_mem_block_calc(unit, entry_handle, core_id, curr_mem,
                                               &(curr_l2p_info->block_index_info), curr_l2p_info->field_id,
                                               &block, &num_of_blocks));

        
        SHR_IF_ERR_EXIT(dbal_hl_memory_write
                        (unit, entry_handle, curr_l2p_info, entry_handle->phy_entry.payload, core_id, mem_array_offset,
                         mem_entry_offset, group_offset, num_of_entries));

    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_hl_register_set_with_packed_fields(
    int unit,
    dbal_entry_handle_t * entry_handle)
{

    soc_reg_t curr_reg;
    uint32 reg_array_offset = 0, field_offset = 0, group_offset = 0;
    uint8 is_condition_pass = 0;
    uint32 block = REG_PORT_ANY;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_hl_l2p_info_t *l2p_hl_info = NULL;
    dbal_hl_l2p_field_info_t *curr_l2p_info = NULL;
    int result_type = entry_handle->cur_res_type;
    uint32 core_id = entry_handle->core_id;
    int logger_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_HL_LOGGER_STATUS_UPDATE;

    l2p_hl_info = &table->hl_mapping_multi_res[result_type].l2p_hl_info[DBAL_HL_ACCESS_REGISTER];
    curr_l2p_info = &l2p_hl_info->l2p_fields_info[0];

    
    SHR_IF_ERR_EXIT(dbal_hl_pre_access_calculate(unit, entry_handle, curr_l2p_info,
                                                 &is_condition_pass, &reg_array_offset, NULL, &field_offset,
                                                 &group_offset));
    if (is_condition_pass)
    {
        int nof_blocks;

        curr_reg = (group_offset == -1) ? curr_l2p_info->reg[0] : curr_l2p_info->reg[group_offset];
        DBAL_PACKED_START_LOG(0, curr_reg);

        
        SHR_IF_ERR_EXIT(dbal_hl_reg_block_calc
                        (unit, entry_handle, core_id, curr_reg, &(curr_l2p_info->block_index_info),
                         curr_l2p_info->field_id, &block, &nof_blocks));

        
        SHR_IF_ERR_EXIT(dbal_hl_register_write
                        (unit, entry_handle, curr_l2p_info, entry_handle->phy_entry.payload, block, nof_blocks,
                         reg_array_offset, group_offset));

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

    if (DBAL_HL_MUL_RES_INFO(access_type).is_packed_fields)
    {
        
        if (entry_handle->nof_result_fields !=
            entry_handle->table->multi_res_info[entry_handle->cur_res_type].nof_result_fields)
        {
            if (!entry_handle->entry_merged)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "In table %s, something wrong happened. It was expected to have all the fields set "
                             "(by the user or by merge procedure) nof fields %d, expected %d.\n",
                             entry_handle->table->table_name, entry_handle->nof_result_fields,
                             entry_handle->table->multi_res_info[entry_handle->cur_res_type].nof_result_fields);
            }
        }

        switch (access_type)
        {
            case DBAL_HL_ACCESS_REGISTER:
                SHR_IF_ERR_EXIT(dbal_hl_register_set_with_packed_fields(unit, entry_handle));
                break;
            case DBAL_HL_ACCESS_MEMORY:
                SHR_IF_ERR_EXIT(dbal_hl_memory_set_with_packed_fields(unit, entry_handle, 1));
            default:
                break;
        }
        
        entry_handle->error_info.error_exists = 0;
    }
    else
    {
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
            default:
                break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_hl_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int access_type;
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
            SHR_IF_ERR_EXIT(dbal_hl_hook_entry_get(unit, entry_handle));
            DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_HL, 1);
            SHR_EXIT();
        }

        for (access_type = 0; access_type < DBAL_NOF_HL_ACCESS_TYPES; access_type++)
        {
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
                SHR_IF_ERR_EXIT(dbal_hl_get_by_access_type(unit, access_type, entry_handle, DBAL_INVALID_PARAM));
            }
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

    SHR_FUNC_INIT_VARS(unit);

    if (table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
    {
        SHR_IF_ERR_EXIT(dbal_hl_tcam_entry_set(unit, entry_handle));
    }
    else if (table->table_type == DBAL_TABLE_TYPE_DIRECT)
    {
        
        entry_handle->error_info.error_exists = 4;

        DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(TRUE, bslSeverityInfo, DBAL_ACCESS_PRINTS_HL, 0);

        if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_HOOK_ACTIVE))
        {
            SHR_IF_ERR_EXIT(dbal_hl_hook_entry_set(unit, entry_handle));
            DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_HL, 0);
            SHR_EXIT();
        }

        for (access_type = 0; access_type < DBAL_NOF_HL_ACCESS_TYPES; access_type++)
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
                SHR_IF_ERR_EXIT(dbal_hl_set_by_access_type(unit, access_type, entry_handle));
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

    sal_memcpy(default_handle, entry_handle, sizeof(dbal_entry_handle_t));

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

    if (entry_handle->table->table_type == DBAL_TABLE_TYPE_DIRECT)
    {
        if (entry_handle->table->hl_mapping_multi_res[res_type_idx].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].is_packed_fields
            || entry_handle->table->hl_mapping_multi_res[res_type_idx].
            l2p_hl_info[DBAL_HL_ACCESS_REGISTER].is_packed_fields)
        {
            
            default_handle->entry_merged = 1;
        }
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
            
            SHR_IF_ERR_EXIT(dbal_hl_entry_set(unit, default_handle));
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
    SHR_FREE(default_handle);
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

    if (entry_handle->table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
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
                        SHR_IF_ERR_EXIT(dbal_hl_is_entry_default(unit, entry_handle, &is_default));
                        if (!is_default)
                        {
                            entry_found = TRUE;
                        }
                    }
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
    SHR_FREE(default_handle);
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

shr_error_e
dbal_hl_reg_default_get(
    int unit,
    soc_reg_t reg,
    int copyno,
    int index,
    uint32 *entry)
{
    soc_reg_above_64_val_t rval, rmsk;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(entry, 0x0, WORDS2BYTES(SOC_MAX_MEM_WORDS));

    SOC_REG_ABOVE_64_RST_VAL_GET(unit, reg, rval);
    SOC_REG_ABOVE_64_RST_MSK_GET(unit, reg, rmsk);

    SOC_REG_ABOVE_64_AND(rval, rmsk);

    sal_memcpy(entry, rval, WORDS2BYTES(SOC_REG_ABOVE_64_MAX_SIZE_U32));

    SHR_FUNC_EXIT;
}
