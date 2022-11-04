/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#include "dbal_hl_access.h"
#include <soc/drv.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/access.h>
#include <soc/sand/sand_ser_correction.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>

extern uint32 G_dbal_field_full_mask[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];

extern shr_error_e dbal_hl_field_from_hw_buffer_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_hl_l2p_info_t * l2p_hl_info,
    int regmem,
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
    bcm_pbmp_t pbmp_pp_dsp;
    bcm_port_t pp_port;
    uint32 dsp_pp = 0;
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(pbmp_pp_dsp);
    pp_port = entry_handle_hook->phy_entry.key[0];
    core_id = entry_handle_hook->phy_entry.core_id;

    SHR_IF_ERR_EXIT(dnx_algo_port_pp_to_pp_dsp_pbmp_get(unit, core_id, pp_port, &pbmp_pp_dsp));

    if (BCM_PBMP_IS_NULL(pbmp_pp_dsp))
    {
        entry_handle_hook->error_info.error_exists = 0;
        SHR_EXIT();
    }

    BCM_PBMP_ITER(pbmp_pp_dsp, dsp_pp)
    {

        entry_handle_hook->phy_entry.key[0] = dsp_pp;

        SHR_IF_ERR_EXIT(dbal_hl_set_by_access_type(unit, access_type, entry_handle_hook));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

uint8
dbal_hl_hook_egress_pp_port_memory_per_pp_dsp_check(
    int unit,
    int mem)
{
    if ((mem == ERPP_CFG_ENABLE_FILTER_PER_PORT_TABLEm) ||
        (mem == ETPPC_EDITING_PER_PORT_TABLEm) ||
        (mem == ERPP_PER_PORT_TABLEm) || ((SOC_IS_JERICHO2(unit)) && (mem == ETPPA_PER_PORT_TABLEm)))
    {
        return TRUE;
    }
    return FALSE;
}

static shr_error_e
dbal_hl_hook_egress_pp_port_handles_update(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_handle_t * entry_handle_regular_fields,
    dbal_entry_handle_t * entry_handle_hook,
    int mem_access_idx,
    int field_pos,
    uint32 *has_regular_fields,
    uint32 *has_hook_fields)
{
    int mem_to_set = 0;
    uint8 mem_to_set_is_per_pp_dsp = 0;
    dbal_fields_e field_to_map;
    uint32 field_offset_in_buffer = 0, field_size_in_buffer = 0;
    dbal_hl_l2p_field_info_t *physical_info_for_access;

    SHR_FUNC_INIT_VARS(unit);

    physical_info_for_access =
        &(entry_handle->table->hl_mapping_multi_res[0].
          l2p_hl_info[DBAL_HL_ACCESS_MEMORY].l2p_fields_info[mem_access_idx]);
    mem_to_set = *(physical_info_for_access->memory);
    field_to_map = physical_info_for_access->field_id;
    field_offset_in_buffer = physical_info_for_access->offset_in_interface;
    field_size_in_buffer = physical_info_for_access->nof_bits_in_interface;

    if (dbal_hl_hook_egress_pp_port_memory_per_pp_dsp_check(unit, mem_to_set))
    {
        mem_to_set_is_per_pp_dsp = 1;
    }

    if (!mem_to_set_is_per_pp_dsp)
    {

        if ((entry_handle->get_all_fields == TRUE)
            && (entry_handle_regular_fields->value_field_ids[field_pos] == DBAL_FIELD_EMPTY))
        {
            entry_handle_regular_fields->value_field_ids[field_pos] = field_to_map;
            entry_handle_regular_fields->nof_result_fields++;
        }

        if (entry_handle_hook->value_field_ids[field_pos] != DBAL_FIELD_EMPTY)
        {
            entry_handle_hook->value_field_ids[field_pos] = DBAL_FIELD_EMPTY;
            entry_handle_hook->nof_result_fields--;
        }

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (G_dbal_field_full_mask, field_offset_in_buffer, field_size_in_buffer,
                         &(entry_handle_regular_fields->phy_entry.p_mask[0])));
        *has_regular_fields = TRUE;
    }
    else
    {

        if ((entry_handle->get_all_fields == TRUE)
            && (entry_handle_hook->value_field_ids[field_pos] == DBAL_FIELD_EMPTY))
        {
            entry_handle_hook->value_field_ids[field_pos] = field_to_map;
            entry_handle_hook->nof_result_fields++;
        }

        if (entry_handle_regular_fields->value_field_ids[field_pos] != DBAL_FIELD_EMPTY)
        {
            entry_handle_regular_fields->value_field_ids[field_pos] = DBAL_FIELD_EMPTY;
            entry_handle_regular_fields->nof_result_fields--;
        }

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (G_dbal_field_full_mask, field_offset_in_buffer, field_size_in_buffer,
                         &(entry_handle_hook->phy_entry.p_mask[0])));
        *has_hook_fields = TRUE;
    }

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

    core_id = entry_handle->phy_entry.core_id;

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

                SHR_IF_ERR_EXIT(dbal_hl_access_block_calc(unit, entry_handle, core_id, last_memory_used,
                                                          &(curr_l2p_info->block_index_info), curr_l2p_info,
                                                          1, &block, &num_of_blocks));

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
                            (unit, entry_handle, l2p_hl_info, curr_mem, field_offset, iter, data,
                             entry_handle->phy_entry.payload, logger_enable));
        }
    }

exit:

    index -= 1;
    while (index >= 0)
    {
        ecc_mask_reg = dnx_data_device.general.ecc_err_masking_get(unit, block_type)->ecc_1bit_err_mask_reg;
        ecc_mask_field = dnx_data_device.general.ecc_err_masking_get(unit, block_type)->ecc_1bit_err_field[index];
        SHR_IF_ERR_CONT(soc_reg_above_64_field32_modify
                        (unit, ecc_mask_reg, REG_PORT_ANY, 0, ecc_mask_field, ecc_1bit_err_mask_value[index]));

        ecc_mask_reg = dnx_data_device.general.ecc_err_masking_get(unit, block_type)->ecc_2bit_err_mask_reg;
        ecc_mask_field = dnx_data_device.general.ecc_err_masking_get(unit, block_type)->ecc_2bit_err_field[index];
        SHR_IF_ERR_CONT(soc_reg_above_64_field32_modify
                        (unit, ecc_mask_reg, REG_PORT_ANY, 0, ecc_mask_field, ecc_2bit_err_mask_value[index]));
        index--;
    }
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
dbal_hl_hook_macsec_tcam_entry_encode(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 decode)
{

    uint32 data_size, table_id, first_tcam_key;
    int total_payload_size, start_offset;
    int nof_words, word;
    dbal_table_field_info_t field_info;
    dbal_logical_table_t table;
    uint32 payload_data[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };
    uint32 payload_mask[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

#ifdef ADAPTER_SERVER_MODE
    SHR_EXIT();
#endif

    table_id = entry_handle->table_id;

    first_tcam_key = 0;
    switch (table_id)
    {
        case DBAL_TABLE_MACSEC_INGRESS_SUB_PORT_TCAM:
        {
            first_tcam_key = DBAL_FIELD_FRAME_FORMAT;
            break;
        }
        case DBAL_TABLE_MACSEC_INGRESS_SECURE_CHANNEL_TCAM:
        {
            first_tcam_key = DBAL_FIELD_POLICY_ID;
            break;
        }
        default:
            break;
    }

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                    (unit, table_id, first_tcam_key, 0, entry_handle->cur_res_type, 0, &field_info));
    start_offset = field_info.bits_offset_in_buffer;

    SHR_IF_ERR_EXIT(dbal_tables_payload_size_get(unit, table_id, entry_handle->cur_res_type, &total_payload_size));

    data_size = (total_payload_size - start_offset) / 2;

    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                    (entry_handle->phy_entry.payload, start_offset, data_size, payload_data));

    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                    (entry_handle->phy_entry.payload, data_size + start_offset, data_size, payload_mask));

    if (!decode)
    {

        table = *(entry_handle->table);

        nof_words = UTILEX_DIV_ROUND_UP(data_size, 32);
        for (word = 0; word < nof_words; word++)
        {

            if ((payload_data[word] & ~payload_mask[word]) > 0)
            {

                LOG_WARN_EX(BSL_LOG_MODULE,
                            "TCAM Key bits are set for masked out bits in table %s, meaning set-get operation symmetry might be broken %s%s%s\r\n",
                            table.table_name, EMPTY, EMPTY, EMPTY);
                break;
            }
        }

        SHR_IF_ERR_EXIT(dbal_internal_encode(unit, payload_data, payload_mask, payload_data,
                                             payload_mask, data_size, DBAL_ENCODING_TYPE_XY));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_internal_decode(unit, payload_data, payload_mask, payload_data,
                                             payload_mask, data_size, DBAL_ENCODING_TYPE_XY));
    }

    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                    (payload_data, start_offset, data_size, entry_handle->phy_entry.payload));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                    (payload_mask, start_offset + data_size, data_size, entry_handle->phy_entry.payload));

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

    if (entry_handle->phy_entry.core_id == DBAL_CORE_ALL)
    {
        for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.ref_count.get(unit, core_id, pp_port, &is_valid));
            if ((core_id == 0) && !is_valid)
            {
                entry_handle->phy_entry.core_id = 1;
            }

            if ((core_id == 1) && !is_valid)
            {

                save_only_in_sw_state = 1;
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.ref_count.get(unit, entry_handle->phy_entry.core_id, pp_port, &is_valid));
        if (!is_valid)
        {

            save_only_in_sw_state = 1;
        }
    }

    for (access_type = 0; access_type < DBAL_NOF_HL_ACCESS_TYPES; access_type++)
    {
        if (access_type != DBAL_HL_ACCESS_MEMORY)
        {
            if (table->hl_mapping_multi_res[0].l2p_hl_info[access_type].num_of_access_fields > 0)
            {
                SHR_IF_ERR_EXIT(dbal_hl_set_by_access_type(unit, access_type, entry_handle));
            }
        }
        else
        {
            int nof_mem_accesses =
                entry_handle->table->hl_mapping_multi_res[0].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].num_of_access_fields;
            int mem_access_idx;

            SHR_IF_ERR_EXIT(dbal_hl_set_by_access_type(unit, DBAL_HL_ACCESS_SW, entry_handle));

            if (save_only_in_sw_state == 0)
            {
                for (mem_access_idx = 0; mem_access_idx < nof_mem_accesses; mem_access_idx++)
                {
                    int field_pos =
                        entry_handle->table->hl_mapping_multi_res[0].
                        l2p_hl_info[DBAL_HL_ACCESS_MEMORY].l2p_fields_info[mem_access_idx].field_pos_in_interface;

                    if (entry_handle->value_field_ids[field_pos] == DBAL_FIELD_EMPTY)
                    {

                        continue;
                    }

                    SHR_IF_ERR_EXIT(dbal_hl_hook_egress_pp_port_handles_update
                                    (unit, entry_handle, entry_handle_regular_fields, entry_handle_hook,
                                     mem_access_idx, field_pos, &has_regular_fields, &has_hook_fields));
                }

                if (has_regular_fields)
                {
                    SHR_IF_ERR_EXIT(dbal_hl_set_by_access_type(unit, access_type, entry_handle_regular_fields));
                }
                if (has_hook_fields)
                {

                    if (entry_handle_hook->phy_entry.core_id == DBAL_CORE_ALL)
                    {
                        pp_port = entry_handle_hook->phy_entry.key[0];

                        for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
                        {
                            entry_handle_hook->phy_entry.core_id = core_id;

                            SHR_IF_ERR_EXIT(dbal_hl_hook_egress_pp_port_hooked_fields_set
                                            (unit, access_type, entry_handle_hook));
                            if (core_id == 0)
                            {

                                entry_handle_hook->phy_entry.key[0] = pp_port;
                            }
                        }
                    }

                    else
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
        }
    }

exit:
    SHR_FREE(entry_handle_hook);
    SHR_FREE(entry_handle_regular_fields);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_hook_entry_decode(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (entry_handle->table_id)
    {
        case DBAL_TABLE_MACSEC_INGRESS_SECURE_CHANNEL_TCAM:
        case DBAL_TABLE_MACSEC_INGRESS_SUB_PORT_TCAM:
        {
            SHR_IF_ERR_EXIT(dbal_hl_hook_macsec_tcam_entry_encode(unit, entry_handle, 1));
            break;
        }
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_hook_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *continue_regular_process)
{
    SHR_FUNC_INIT_VARS(unit);

    *continue_regular_process = FALSE;

    switch (entry_handle->table_id)
    {
        case DBAL_TABLE_MACSEC_INGRESS_SECURE_CHANNEL_TCAM:
        case DBAL_TABLE_MACSEC_INGRESS_SUB_PORT_TCAM:
            *continue_regular_process = TRUE;
            break;
        case DBAL_TABLE_EGRESS_PP_PORT:
        {
            *continue_regular_process = TRUE;
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
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "no hook supported for table %s operation get\n",
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
    dbal_entry_handle_t * entry_handle,
    int *continue_regular_process)
{
    SHR_FUNC_INIT_VARS(unit);

    *continue_regular_process = FALSE;

    switch (entry_handle->table_id)
    {

        case DBAL_TABLE_MACSEC_INGRESS_SECURE_CHANNEL_TCAM:
        case DBAL_TABLE_MACSEC_INGRESS_SUB_PORT_TCAM:
        {
            SHR_IF_ERR_EXIT(dbal_hl_hook_macsec_tcam_entry_encode(unit, entry_handle, 0));
            *continue_regular_process = TRUE;
            break;
        }

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
    dbal_entry_handle_t * entry_handle,
    int *continue_regular_process)
{
    SHR_FUNC_INIT_VARS(unit);

    *continue_regular_process = FALSE;

    switch (entry_handle->table_id)
    {

        case DBAL_TABLE_MACSEC_INGRESS_SECURE_CHANNEL_TCAM:
        case DBAL_TABLE_MACSEC_INGRESS_SUB_PORT_TCAM:
        {
            *continue_regular_process = TRUE;
            break;
        }
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
