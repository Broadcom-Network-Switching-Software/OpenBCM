/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "dbal_hl_access.h"
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/access.h>
#include <soc/sand/sand_ser_correction.h>

extern uint32 G_dbal_field_full_mask[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];

extern shr_error_e dbal_hl_field_from_hw_buffer_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_hl_l2p_info_t * l2p_hl_info,
    soc_mem_t memory,
    soc_reg_t reg,
    int field_offset,
    int field_pos,
    uint32 *src,
    uint32 *dst,
    int logger_enable);

extern shr_error_e dbal_hl_pre_access_calculate(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_hl_l2p_field_info_t * curr_l2p_info,
    uint8 *is_condition_pass,
    uint32 *mem_array_offset,
    uint32 *mem_entry_offset,
    uint32 *field_offset,
    uint32 *group_offset);




static shr_error_e
dbal_hl_hook_egress_pp_port_hooked_fields_set(
    int unit,
    int access_type,
    dbal_entry_handle_t * entry_handle_hook)
{
    bcm_pbmp_t pbmp_tm_ports;
    bcm_port_t pp_port;
    uint32 dsp_pp = 0;
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(pbmp_tm_ports);
    pp_port = entry_handle_hook->phy_entry.key[0];
    core_id = entry_handle_hook->core_id;

    
    SHR_IF_ERR_EXIT(dnx_algo_port_pp_to_tm_pbmp_get(unit, core_id, pp_port, &pbmp_tm_ports));

    
    if (BCM_PBMP_IS_NULL(pbmp_tm_ports))
    {
        entry_handle_hook->error_info.error_exists = 0;
        SHR_EXIT();
    }
    
    BCM_PBMP_ITER(pbmp_tm_ports, dsp_pp)
    {
        
        entry_handle_hook->phy_entry.key[0] = dsp_pp;
        
        SHR_IF_ERR_EXIT(dbal_hl_set_by_access_type(unit, access_type, entry_handle_hook));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_hl_hook_egress_pp_port_handles_update(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_handle_t * entry_handle_regular_fields,
    dbal_entry_handle_t * entry_handle_hook,
    uint32 iter,
    int field_pos,
    uint32 *has_regular_fields,
    uint32 *has_hook_fields)
{
    int memory_feature_jr2 = 0, different_previous_and_next_field_ids = 0, same_current_and_next_field_id = 0;
    int curr_next_field_not_mapped_to_memories_with_features = 0, prev_curr_field_not_mapped_to_memories_with_features =
        0;
    SHR_FUNC_INIT_VARS(unit);

    if ((!
         (SOC_IS_JERICHO2_ONLY(unit)
          && *entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
          l2p_fields_info[iter].memory == ETPPA_PER_PORT_TABLEm))
        && (*entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
            l2p_fields_info[iter].memory != ERPP_PER_PORT_TABLEm)
        && (*entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
            l2p_fields_info[iter].memory != ERPP_CFG_ENABLE_FILTER_PER_PORT_TABLEm)
        &&
        (!((*entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
            l2p_fields_info[iter].memory == ETPPC_EDITING_PER_PORT_TABLEm))))
    {
        
        if ((entry_handle->get_all_fields == TRUE)
            && (entry_handle_regular_fields->value_field_ids[field_pos] == DBAL_FIELD_EMPTY))
        {
            entry_handle_regular_fields->value_field_ids[field_pos] =
                entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info
                [DBAL_HL_ACCESS_MEMORY].l2p_fields_info[iter].field_id;
            entry_handle_regular_fields->nof_result_fields = entry_handle_regular_fields->nof_result_fields + 1;
        }

        prev_curr_field_not_mapped_to_memories_with_features =
            (entry_handle_hook->value_field_ids[field_pos] != DBAL_FIELD_EMPTY) && (field_pos > 0) && (iter > 0)
            && (entry_handle->table->
                hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                l2p_fields_info[iter].field_id ==
                entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                          cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                l2p_fields_info[iter - 1].field_id)
            &&
            ((!(SOC_IS_JERICHO2_ONLY(unit)
                && *entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                              cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                l2p_fields_info[iter - 1].memory == ETPPA_PER_PORT_TABLEm))
             && *entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                           cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
             l2p_fields_info[iter - 1].memory != ERPP_PER_PORT_TABLEm
             && *entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                           cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
             l2p_fields_info[iter - 1].memory != ERPP_CFG_ENABLE_FILTER_PER_PORT_TABLEm
             &&
             (!((*entry_handle->table->
                 hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                 l2p_fields_info[iter - 1].memory != ETPPC_EDITING_PER_PORT_TABLEm))));

        different_previous_and_next_field_ids = (field_pos > 0) && (iter > 0)
            && (entry_handle->table->
                hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                l2p_fields_info[iter].field_id !=
                entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                          cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                l2p_fields_info[iter - 1].field_id)
            && (entry_handle->table->
                hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                l2p_fields_info[iter].field_id !=
                entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                          cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                l2p_fields_info[iter + 1].field_id);

        curr_next_field_not_mapped_to_memories_with_features = (field_pos > 0) && (iter > 0)
            && (iter <
                entry_handle->table->hl_mapping_multi_res[0].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].num_of_access_fields -
                1)
            &&
            ((!(SOC_IS_JERICHO2_ONLY(unit)
                && *entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                              cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                l2p_fields_info[iter + 1].memory == ETPPA_PER_PORT_TABLEm))
             && *entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                           cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
             l2p_fields_info[iter + 1].memory != ERPP_PER_PORT_TABLEm
             && *entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                           cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
             l2p_fields_info[iter + 1].memory != ERPP_CFG_ENABLE_FILTER_PER_PORT_TABLEm
             &&
             (!(*entry_handle->table->
                hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                l2p_fields_info[iter + 1].memory != ETPPC_EDITING_PER_PORT_TABLEm)))
            && (entry_handle->table->
                hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                l2p_fields_info[iter].field_id ==
                entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                          cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                l2p_fields_info[iter + 1].field_id);
        
        if (((entry_handle_hook->value_field_ids[field_pos] != DBAL_FIELD_EMPTY) && (iter == 0))
            || prev_curr_field_not_mapped_to_memories_with_features || different_previous_and_next_field_ids
            || curr_next_field_not_mapped_to_memories_with_features)
        {
            entry_handle_hook->value_field_ids[field_pos] = DBAL_FIELD_EMPTY;
            entry_handle_hook->nof_result_fields = entry_handle_hook->nof_result_fields - 1;
        }

        *has_regular_fields = TRUE;
        
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (G_dbal_field_full_mask,
                         entry_handle_regular_fields->table->hl_mapping_multi_res[entry_handle_regular_fields->
                                                                                  cur_res_type].l2p_hl_info
                         [DBAL_HL_ACCESS_MEMORY].l2p_fields_info[iter].offset_in_interface,
                         entry_handle_regular_fields->table->hl_mapping_multi_res[entry_handle_regular_fields->
                                                                                  cur_res_type].l2p_hl_info
                         [DBAL_HL_ACCESS_MEMORY].l2p_fields_info[iter].nof_bits_in_interface,
                         &(entry_handle_regular_fields->phy_entry.p_mask[0])));

    }
    else
    {

        
        if ((entry_handle->get_all_fields == TRUE)
            && (entry_handle_hook->value_field_ids[field_pos] == DBAL_FIELD_EMPTY))
        {
            entry_handle_hook->value_field_ids[field_pos] =
                entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info
                [DBAL_HL_ACCESS_MEMORY].l2p_fields_info[iter].field_id;
            entry_handle_hook->nof_result_fields = entry_handle_hook->nof_result_fields + 1;
        }

        memory_feature_jr2 = ((entry_handle_regular_fields->value_field_ids[field_pos] != DBAL_FIELD_EMPTY)
                              && (field_pos > 0) && (iter > 0)
                              && (entry_handle->table->
                                  hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                                  l2p_fields_info[iter].field_id ==
                                  entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                                            cur_res_type].l2p_hl_info
                                  [DBAL_HL_ACCESS_MEMORY].l2p_fields_info[iter - 1].field_id)
                              &&
                              ((SOC_IS_JERICHO2_ONLY(unit)
                                && *entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                                              cur_res_type].l2p_hl_info
                                [DBAL_HL_ACCESS_MEMORY].l2p_fields_info[iter - 1].memory == ETPPA_PER_PORT_TABLEm)
                               || *entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                                             cur_res_type].l2p_hl_info
                               [DBAL_HL_ACCESS_MEMORY].l2p_fields_info[iter - 1].memory == ERPP_PER_PORT_TABLEm
                               || *entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                                             cur_res_type].l2p_hl_info
                               [DBAL_HL_ACCESS_MEMORY].l2p_fields_info[iter - 1].memory ==
                               ERPP_CFG_ENABLE_FILTER_PER_PORT_TABLEm
                               ||
                               ((*entry_handle->table->
                                 hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                                 l2p_fields_info[iter - 1].memory == ETPPC_EDITING_PER_PORT_TABLEm))));
        if ((field_pos > 0)
            && (field_pos <
                entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                          cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                num_of_access_fields - 1))
        {
            different_previous_and_next_field_ids = (field_pos > 0) && (iter > 0) &&
                (entry_handle->table->
                 hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                 l2p_fields_info[iter].field_id !=
                 entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                           cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                 l2p_fields_info[iter - 1].field_id)
                && (entry_handle->table->
                    hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                    l2p_fields_info[iter].field_id !=
                    entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                              cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                    l2p_fields_info[iter + 1].field_id);
        }
        same_current_and_next_field_id = (field_pos > 0) && (iter > 0)
            && (iter <
                entry_handle->table->hl_mapping_multi_res[0].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].num_of_access_fields -
                1) && ((SOC_IS_JERICHO2_ONLY(unit)
                        && *entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                                      cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                        l2p_fields_info[iter + 1].memory == ETPPA_PER_PORT_TABLEm)
                       || *entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                                     cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                       l2p_fields_info[iter + 1].memory == ERPP_PER_PORT_TABLEm
                       || *entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                                     cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                       l2p_fields_info[iter + 1].memory == ERPP_CFG_ENABLE_FILTER_PER_PORT_TABLEm
                       || (*entry_handle->table->
                           hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                           l2p_fields_info[iter + 1].memory == ETPPC_EDITING_PER_PORT_TABLEm))
            && (entry_handle->table->
                hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                l2p_fields_info[iter + 1].field_id ==
                entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                          cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                l2p_fields_info[iter].field_id);
        
        if (((entry_handle_regular_fields->value_field_ids[field_pos] != DBAL_FIELD_EMPTY) && (iter == 0))
            || memory_feature_jr2 || different_previous_and_next_field_ids || same_current_and_next_field_id)
        {
            entry_handle_regular_fields->value_field_ids[field_pos] = DBAL_FIELD_EMPTY;
            entry_handle_regular_fields->nof_result_fields = entry_handle_regular_fields->nof_result_fields - 1;
        }

        *has_hook_fields = TRUE;
        
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (G_dbal_field_full_mask,
                         entry_handle_hook->table->hl_mapping_multi_res[entry_handle_hook->cur_res_type].l2p_hl_info
                         [DBAL_HL_ACCESS_MEMORY].l2p_fields_info[iter].offset_in_interface,
                         entry_handle_hook->table->hl_mapping_multi_res[entry_handle_hook->cur_res_type].l2p_hl_info
                         [DBAL_HL_ACCESS_MEMORY].l2p_fields_info[iter].nof_bits_in_interface,
                         &(entry_handle_hook->phy_entry.p_mask[0])));
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_hl_hook_ecc_sw_check_memory_get(
    int unit,
    soc_block_type_t block_type,
    dbal_entry_handle_t * entry_handle)
{
    int iter, mem_row_bit_width = 0;
    uint32 ecc = 0, ecc_expected = 0;
    soc_mem_t last_memory_used = INVALIDm, curr_mem;
    int last_mem_entry_offset = 0;
    int last_mem_array_offset_used = 0;
    uint32 mem_entry_offset = 0, field_offset = 0, mem_array_offset = 0, group_offset;
    uint8 is_condition_pass = 0;
    uint32 data[DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS] = { 0 };
    uint32 core_id = DBAL_CORE_DEFAULT;
    dbal_hl_l2p_info_t *l2p_hl_info;
    dbal_hl_l2p_field_info_t *curr_l2p_info;
    int block = MEM_BLOCK_ANY;
    int index = 0;
    uint32 ecc_1bit_err_mask_value[DNX_MAX_NOF_ECC_ERR_MEMS] = { 0 };
    uint32 ecc_2bit_err_mask_value[DNX_MAX_NOF_ECC_ERR_MEMS] = { 0 };
    soc_reg_t ecc_mask_reg = INVALIDr;
    soc_field_t ecc_mask_field = INVALIDf;
    soc_reg_above_64_val_t ecc_mask_data;
    soc_field_info_t *fieldinfo;
    soc_mem_info_t *meminfo;
    int logger_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_HL_LOGGER_STATUS_UPDATE;

    
    index = 0;
    while (dnx_data_device.general.ecc_err_masking_get(unit, block_type)->ecc_1bit_err_field[index] != -1)
    {
        ecc_mask_reg = dnx_data_device.general.ecc_err_masking_get(unit, block_type)->ecc_1bit_err_mask_reg;
        ecc_mask_field = dnx_data_device.general.ecc_err_masking_get(unit, block_type)->ecc_1bit_err_field[index];
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, ecc_mask_reg, REG_PORT_ANY, 0, ecc_mask_data));
        ecc_1bit_err_mask_value[index] =
            soc_reg_above_64_field32_get(unit, ecc_mask_reg, ecc_mask_data, ecc_mask_field);
        SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ecc_mask_reg, REG_PORT_ANY, 0, ecc_mask_field, 0));

        ecc_mask_reg = dnx_data_device.general.ecc_err_masking_get(unit, block_type)->ecc_2bit_err_mask_reg;
        ecc_mask_field = dnx_data_device.general.ecc_err_masking_get(unit, block_type)->ecc_2bit_err_field[index];
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, ecc_mask_reg, REG_PORT_ANY, 0, ecc_mask_data));
        ecc_2bit_err_mask_value[index] =
            soc_reg_above_64_field32_get(unit, ecc_mask_reg, ecc_mask_data, ecc_mask_field);
        SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ecc_mask_reg, REG_PORT_ANY, 0, ecc_mask_field, 0));
        index++;
    }

    core_id = entry_handle->core_id;

    l2p_hl_info =
        &(entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY]);

    
    for (iter = 0; iter < l2p_hl_info->num_of_access_fields; iter++)
    {
        uint8 correct_data_is_read = FALSE;
        int nof_reads = 0;
        int field_pos;
        curr_l2p_info = &l2p_hl_info->l2p_fields_info[iter];
        field_pos = curr_l2p_info->field_pos_in_interface;

        if (!entry_handle->get_all_fields)
        {
            
            if (entry_handle->value_field_ids[field_pos] == DBAL_FIELD_EMPTY)
            {
                continue;
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
                soc_mem_t memory_to_read = curr_mem;
                last_memory_used = curr_mem;
                last_mem_array_offset_used = mem_array_offset;
                last_mem_entry_offset = mem_entry_offset;
                sal_memset(data, 0x0, DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_BYTES);

                
                SHR_IF_ERR_EXIT(dbal_hl_mem_block_calc(unit, entry_handle, core_id, last_memory_used,
                                                       &(curr_l2p_info->block_index_info), curr_l2p_info->field_id,
                                                       &block, &num_of_blocks));

                DBAL_MEM_READ_LOG(memory_to_read, mem_entry_offset, block, last_mem_array_offset_used);

                
                while (correct_data_is_read != TRUE)
                {
                    
                    SHR_IF_ERR_EXIT_WITH_LOG(soc_mem_array_read(unit, memory_to_read,
                                                                last_mem_array_offset_used, block, mem_entry_offset,
                                                                data), "Failed reading memory %s port=%d arr_ind=%d",
                                             SOC_MEM_NAME(unit, memory_to_read), block, last_mem_array_offset_used);
                    
                    meminfo = &SOC_MEM_INFO(unit, memory_to_read);
                    SOC_FIND_FIELD(ECCf, meminfo->fields, meminfo->nFields, fieldinfo);
                    if (fieldinfo != NULL)
                    {
                        
                        soc_mem_field_get(unit, memory_to_read, data, ECCf, &ecc);

                        
                        mem_row_bit_width =
                            soc_mem_entry_bits(unit, memory_to_read) - soc_mem_field_length(unit, memory_to_read, ECCf);
                        SHR_IF_ERR_EXIT(calc_ecc(unit, mem_row_bit_width, data, &ecc_expected));
                        nof_reads++;

                        
                        if ((ecc != ecc_expected) && (nof_reads > 500))
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Could not read correct data, ECC error check failed!!!\n");
                        }

                        
                        if (ecc == ecc_expected)
                        {
                            correct_data_is_read = TRUE;
                        }
                    }
                    else
                    {
                        correct_data_is_read = TRUE;
                    }
                }

                DBAL_ACTION_PERFORMED_LOG;
                DBAL_DATA_LOG(data, 1);
            }

            SHR_IF_ERR_EXIT(dbal_hl_field_from_hw_buffer_get
                            (unit, entry_handle, l2p_hl_info, curr_mem, INVALIDr, field_offset, iter, data,
                             entry_handle->phy_entry.payload, logger_enable));
        }
    }

    
    index = 0;
    while (dnx_data_device.general.ecc_err_masking_get(unit, block_type)->ecc_1bit_err_field[index] != -1)
    {
        ecc_mask_reg = dnx_data_device.general.ecc_err_masking_get(unit, block_type)->ecc_1bit_err_mask_reg;
        ecc_mask_field = dnx_data_device.general.ecc_err_masking_get(unit, block_type)->ecc_1bit_err_field[index];
        SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify
                        (unit, ecc_mask_reg, REG_PORT_ANY, 0, ecc_mask_field, ecc_1bit_err_mask_value[index]));

        ecc_mask_reg = dnx_data_device.general.ecc_err_masking_get(unit, block_type)->ecc_2bit_err_mask_reg;
        ecc_mask_field = dnx_data_device.general.ecc_err_masking_get(unit, block_type)->ecc_2bit_err_field[index];
        SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify
                        (unit, ecc_mask_reg, REG_PORT_ANY, 0, ecc_mask_field, ecc_2bit_err_mask_value[index]));
        index++;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_hl_hook_ecc_sw_check_entry_get(
    int unit,
    soc_block_type_t block_type,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    if (entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].
        l2p_hl_info[DBAL_HL_ACCESS_MEMORY].num_of_access_fields > 0)
    {
        
        SHR_IF_ERR_EXIT(dbal_hl_hook_ecc_sw_check_memory_get(unit, block_type, entry_handle));
    }

exit:
    SHR_FUNC_EXIT;
}




static shr_error_e
dbal_hl_hook_egress_pp_port_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int access_type;
    dbal_logical_table_t *table = entry_handle->table;
    SHR_FUNC_INIT_VARS(unit);

    for (access_type = 0; access_type < DBAL_NOF_HL_ACCESS_TYPES; access_type++)
    {
        if (table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[access_type].num_of_access_fields > 0)
        {
            
            if ((access_type == DBAL_HL_ACCESS_MEMORY))
            {
                SHR_IF_ERR_EXIT(dbal_hl_get_by_access_type(unit, DBAL_HL_ACCESS_SW, entry_handle, DBAL_INVALID_PARAM));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_hl_get_by_access_type(unit, access_type, entry_handle, DBAL_INVALID_PARAM));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_hl_hook_egress_pp_port_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int access_type;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_entry_handle_t *entry_handle_regular_fields = NULL;
    dbal_entry_handle_t *entry_handle_hook = NULL;
    uint32 iter;
    uint32 has_regular_fields;
    uint32 has_hook_fields;
    int is_valid;
    bcm_core_t core_id;
    bcm_port_t pp_port;
    uint8 save_only_in_sw_state = 0;
    SHR_FUNC_INIT_VARS(unit);

    has_regular_fields = FALSE;
    has_hook_fields = FALSE;
    pp_port = entry_handle->phy_entry.key[0];

    
    SHR_ALLOC_SET_ZERO(entry_handle_regular_fields, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n", EMPTY,
                       EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(entry_handle_hook, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n", EMPTY, EMPTY,
                       EMPTY);
    *entry_handle_regular_fields = *entry_handle;
    *entry_handle_hook = *entry_handle;

    if (entry_handle->core_id == DBAL_CORE_ALL)
    {
        for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.ref_count.get(unit, core_id, pp_port, &is_valid));
            if ((core_id == 0) && !is_valid)
            {
                entry_handle->core_id = 1;
            }

            if ((core_id == 1) && !is_valid)
            {
                
                save_only_in_sw_state = 1;
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.ref_count.get(unit, entry_handle->core_id, pp_port, &is_valid));
        if (!is_valid)
        {
            
            save_only_in_sw_state = 1;
        }
    }

    for (access_type = 0; access_type < DBAL_NOF_HL_ACCESS_TYPES; access_type++)
    {
        if (table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[access_type].num_of_access_fields > 0)
        {
            if (access_type == DBAL_HL_ACCESS_MEMORY)
            {
                
                SHR_IF_ERR_EXIT(dbal_hl_set_by_access_type(unit, DBAL_HL_ACCESS_SW, entry_handle));
            }
            
            if ((access_type == DBAL_HL_ACCESS_MEMORY) && (save_only_in_sw_state == 0))
            {
                for (iter = 0;
                     iter <
                     entry_handle->table->hl_mapping_multi_res[0].
                     l2p_hl_info[DBAL_HL_ACCESS_MEMORY].num_of_access_fields; iter++)
                {
                    int field_pos;
                    field_pos =
                        entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info
                        [DBAL_HL_ACCESS_MEMORY].l2p_fields_info[iter].field_pos_in_interface;

                    if (entry_handle->value_field_ids[field_pos] == DBAL_FIELD_EMPTY)
                    {
                        
                        continue;
                    }

                    
                    SHR_IF_ERR_EXIT(dbal_hl_hook_egress_pp_port_handles_update
                                    (unit, entry_handle, entry_handle_regular_fields, entry_handle_hook,
                                     iter, field_pos, &has_regular_fields, &has_hook_fields));
                }

                
                if (has_regular_fields == TRUE)
                {
                    SHR_IF_ERR_EXIT(dbal_hl_set_by_access_type(unit, access_type, entry_handle_regular_fields));
                }
                {
                    
                    if ((entry_handle_hook->core_id == DBAL_CORE_ALL) && (has_hook_fields == TRUE))
                    {
                        pp_port = entry_handle_hook->phy_entry.key[0];

                        for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
                        {
                            entry_handle_hook->core_id = core_id;
                            
                            SHR_IF_ERR_EXIT(dbal_hl_hook_egress_pp_port_hooked_fields_set
                                            (unit, access_type, entry_handle_hook));
                            if (core_id == 0)
                            {
                                
                                entry_handle_hook->phy_entry.key[0] = pp_port;
                            }
                        }
                    }
                    
                    else if ((entry_handle_hook->core_id != DBAL_CORE_ALL) && (has_hook_fields == TRUE))
                    {
                        
                        SHR_IF_ERR_EXIT(dbal_hl_hook_egress_pp_port_hooked_fields_set
                                        (unit, access_type, entry_handle_hook));
                    }
                }
                if (((has_regular_fields == TRUE) && (entry_handle_regular_fields->error_info.error_exists == 0))
                    && ((has_hook_fields == TRUE) && (entry_handle_hook->error_info.error_exists == 0)))
                {
                    entry_handle->error_info.error_exists = 0;
                }
                else if ((has_regular_fields == TRUE) && (entry_handle_regular_fields->error_info.error_exists == 0)
                         && (has_hook_fields == FALSE))
                {
                    entry_handle->error_info.error_exists = 0;
                }
                else if ((has_hook_fields == TRUE) && (entry_handle_hook->error_info.error_exists == 0)
                         && (has_regular_fields == FALSE))
                {
                    entry_handle->error_info.error_exists = 0;
                }

            }
            else if (access_type != DBAL_HL_ACCESS_MEMORY)
            {
                SHR_IF_ERR_EXIT(dbal_hl_set_by_access_type(unit, access_type, entry_handle));
            }
        }
    }

exit:
    SHR_FREE(entry_handle_hook);
    SHR_FREE(entry_handle_regular_fields);
    SHR_FUNC_EXIT;

}


shr_error_e
dbal_hl_hook_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (entry_handle->table_id)
    {
        case DBAL_TABLE_EGRESS_PP_PORT:
        {
            SHR_IF_ERR_EXIT(dbal_hl_hook_egress_pp_port_entry_get(unit, entry_handle));
            break;
        }
        case DBAL_TABLE_INGRESS_CONG_VSQ_GROUP_C_OCCUPANCY:
        case DBAL_TABLE_INGRESS_CONG_VSQ_GROUP_D_OCCUPANCY:
        case DBAL_TABLE_INGRESS_CONG_VSQ_GROUP_E_OCCUPANCY:
        case DBAL_TABLE_INGRESS_CONG_VSQ_GROUP_F_OCCUPANCY:
        {
            SHR_IF_ERR_EXIT(dbal_hl_hook_ecc_sw_check_entry_get(unit, SOC_BLK_CGM, entry_handle));
            break;
        }
        case DBAL_TABLE_ECGM_QUEUE_RESOURCES_STATISTICS_CURRENT:
        case DBAL_TABLE_ECGM_PORT_RESOURCES_STATISTICS_CURRENT:
        case DBAL_TABLE_ECGM_INTERFACE_RESOURCES_STATISTICS_CURRENT:
        {
            SHR_IF_ERR_EXIT(dbal_hl_hook_ecc_sw_check_entry_get(unit, SOC_BLK_ECGM, entry_handle));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "no hook supported for table %s operatoion get\n",
                         entry_handle->table->table_name);
            break;
        }
    }

    

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_hook_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (entry_handle->table_id)
    {
        case DBAL_TABLE_EGRESS_PP_PORT:
        {
            SHR_IF_ERR_EXIT(dbal_hl_hook_egress_pp_port_entry_set(unit, entry_handle));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "no hook supported for table %s operatoion set\n",
                         entry_handle->table->table_name);
            break;
        }
    }

    
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_hook_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (entry_handle->table_id)
    {
        case DBAL_TABLE_EGRESS_PP_PORT:
        {
            
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "res_type_resolution not needed for table egress_pp_port \n");
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "no hook supported for table %s operatoion res_type_resolution\n",
                         entry_handle->table->table_name);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
