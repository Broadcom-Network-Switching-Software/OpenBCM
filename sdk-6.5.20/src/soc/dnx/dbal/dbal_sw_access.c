/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALACCESSDNX
#include <shared/bsl.h>

#include "dbal_internal.h"
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/mem.h>
#include <soc/mcm/memregs.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/swstate/auto_generated/access/dbal_access.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>

#define DBAL_SW_DIRECT_TABLES_MAX_KEY_SIZE_BITS   30

#define DBAL_SW_FIELD_LOG( field_id, value, offset, nof_bits)                                     \
        if(logger_enable)                                                                         \
        {                                                                                         \
            LOG_INFO_EX(BSL_LOG_MODULE, " using Field %-30s, Vlaue %d, offset %d, nof_bits %d\n", \
                    dbal_field_to_string(unit, field_id), value, offset, nof_bits);               \
        }

#define DBAL_SWSTATE_DPC_TBL_INGORE(unit, core_mode, core_id) \
        if ((dnx_data_device.general.nof_cores_get(unit) == 1) && \
        (core_mode == DBAL_CORE_MODE_DPC) && \
        (core_id >= dnx_data_device.general.nof_cores_get(unit))) \
        { \
            continue; \
        }


shr_error_e
dbal_sw_access_table_alloc(
    int unit,
    dbal_logical_table_t * table,
    dbal_tables_e table_id,
    int nof_entries,
    int result_length_in_bytes)
{
    sw_state_htbl_init_info_t hash_tbl_init_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&hash_tbl_init_info, 0, sizeof(hash_tbl_init_info));
    switch (table->table_type)
    {
        case DBAL_TABLE_TYPE_EM:
        case DBAL_TABLE_TYPE_LPM:
        case DBAL_TABLE_TYPE_TCAM:
        case DBAL_TABLE_TYPE_TCAM_DIRECT:
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.table_type.set(unit, table_id, DBAL_SW_TABLE_HASH));

            
            hash_tbl_init_info.max_nof_elements = nof_entries;
            hash_tbl_init_info.expected_nof_elements = nof_entries;

            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.hash_table.create(unit, table_id, &hash_tbl_init_info));

            break;

        case DBAL_TABLE_TYPE_DIRECT:
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.table_type.set(unit, table_id, DBAL_SW_TABLE_DIRECT));

            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown logical table type, %s. table %s\n",
                         dbal_table_type_to_string(unit, table->table_type), table->table_name);
            break;
    }

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.
                    entries.alloc_bitmap(unit, table_id, BYTES2BITS(nof_entries * result_length_in_bytes)));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_sw_access_init(
    int unit)
{
    int ii, max_result_length_bytes = 0;
    dbal_logical_table_t *table;
    uint32 num_of_entries = 0;
    dbal_table_status_e table_status;

    SHR_FUNC_INIT_VARS(unit);

    if (sw_state_is_warm_boot(unit))
    {
        SHR_EXIT();
    }

    for (ii = 0; ii < DBAL_NOF_TABLES; ii++)
    {
        max_result_length_bytes = 0;
        num_of_entries = 0;
        SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit, ii, &table));
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, ii, &table_status));

        if (table->maturity_level == DBAL_MATURITY_LOW || (table_status == DBAL_TABLE_INCOMPATIBLE_IMAGE))
        {
            continue;
        }

        switch (table->access_method)
        {
            case DBAL_ACCESS_METHOD_SW_STATE:
                max_result_length_bytes = table->sw_access_info.sw_payload_length_bytes;
                break;

            case DBAL_ACCESS_METHOD_MDB:
                if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
                {
                    max_result_length_bytes = BITS2BYTES(table->multi_res_info[0].results_info[0].field_nof_bits);
                }
                break;

            case DBAL_ACCESS_METHOD_HARD_LOGIC:
                max_result_length_bytes = table->sw_access_info.sw_payload_length_bytes;
                break;

            default:
                break;
        }

        if (!max_result_length_bytes)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dbal_sw_table_nof_entries_calculate(unit, table, &num_of_entries));

        if (num_of_entries == 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "SW state nof entries = 0  table %s\n", table->table_name);
        }

        if (max_result_length_bytes > DBAL_PHYSICAL_RES_SIZE_IN_BYTES)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "table %s, result length for SW table id %d bytes exceeding the max %u bytes.\n",
                         table->table_name, max_result_length_bytes, (uint32) DBAL_PHYSICAL_RES_SIZE_IN_BYTES);
        }

        
        table->sw_access_info.sw_payload_length_bytes = max_result_length_bytes;
        if (num_of_entries > 0)
        {
            SHR_IF_ERR_EXIT(dbal_sw_access_table_alloc(unit, table, ii, num_of_entries, max_result_length_bytes));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_sw_access_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_sw_table_entry_dump(
    int unit,
    dbal_tables_e table_id,
    int entry_index,
    uint32 *payload,
    int res_length,
    char *action)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    if (dbal_logger_is_enable(unit, table_id))
    {
        res_length = BYTES2WORDS(res_length);
        LOG_CLI((BSL_META_U(unit, "Entry %s \n"), action));
        LOG_CLI((BSL_META_U(unit, "Entry index : 0x%x\n"), entry_index));
        LOG_CLI((BSL_META_U(unit, "Payload (%d Words) : 0x"), res_length));
        for (ii = res_length - 1; ii >= 0; ii--)
        {
            LOG_CLI((BSL_META("%02x"), payload[ii]));
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
    }

    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_sw_table_entry_set_in_index_hl(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 *entry_sw_value,
    int *access_needed)
{
    int access_index;
    dbal_table_field_info_t *table_field_info;
    int field_pos_in_interface;
    multi_res_info_t *multi_res_info = &entry_handle->table->multi_res_info[entry_handle->cur_res_type];
    dbal_hl_l2p_info_t *l2p_hl_sw_info =
        &entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_SW];
    int logger_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (dbal_logger_is_enable(unit, entry_handle->table_id))
    {
        logger_enable = 1;
    }

    for (access_index = 0; access_index < l2p_hl_sw_info->num_of_access_fields; access_index++)
    {
        uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };

        field_pos_in_interface = l2p_hl_sw_info->l2p_fields_info[access_index].field_pos_in_interface;

        table_field_info = &(multi_res_info->results_info[field_pos_in_interface]);

        *access_needed = TRUE;

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (entry_handle->phy_entry.payload,
                         l2p_hl_sw_info->l2p_fields_info[access_index].offset_in_interface,
                         l2p_hl_sw_info->l2p_fields_info[access_index].access_nof_bits, field_value));

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (field_value, l2p_hl_sw_info->l2p_fields_info[access_index].access_offset,
                         l2p_hl_sw_info->l2p_fields_info[access_index].access_nof_bits, entry_sw_value));

        DBAL_SW_FIELD_LOG(table_field_info->field_id, field_value[0],
                          l2p_hl_sw_info->l2p_fields_info[access_index].access_offset,
                          l2p_hl_sw_info->l2p_fields_info[access_index].access_nof_bits);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_sw_table_entry_set_in_index_mdb(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 *entry_sw_value,
    int *access_needed)
{
    dbal_table_field_info_t *table_field_info;
    multi_res_info_t *multi_res_info = &entry_handle->table->multi_res_info[entry_handle->cur_res_type];
    int logger_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (dbal_logger_is_enable(unit, entry_handle->table_id))
    {
        logger_enable = 1;
    }
    table_field_info = &(multi_res_info->results_info[0]);

    if (entry_handle->value_field_ids[0] != DBAL_FIELD_EMPTY)
    {
        *access_needed = TRUE;
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (entry_handle->phy_entry.payload, table_field_info->bits_offset_in_buffer,
                         table_field_info->field_nof_bits, entry_sw_value));

        DBAL_SW_FIELD_LOG(table_field_info->field_id, entry_sw_value[0], 0, table_field_info->field_nof_bits);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_sw_table_entry_set_in_index(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int entry_index)
{
    int bsl_severity;
    uint32 entry_sw_value[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };
    int access_needed = FALSE;
    uint32 *payload_pointer;
    uint32 payload_length_bytes = 0;
    uint32 payload_length_bits;

    SHR_FUNC_INIT_VARS(unit);

    SHR_GET_SEVERITY_FOR_MODULE(bsl_severity);

    payload_length_bytes = entry_handle->table->sw_access_info.sw_payload_length_bytes;
    payload_length_bits = BYTES2BITS(payload_length_bytes);

    if ((entry_index > entry_handle->table->max_capacity) && (entry_handle->table->max_capacity))
    {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\nFor table %s Entry index %d exceeded max capacity %d\n"),
                                     entry_handle->table->table_name, entry_index, entry_handle->table->max_capacity));
    }

    if (bsl_severity >= bslSeverityInfo)
    {
        DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(TRUE, bslSeverityInfo, DBAL_ACCESS_PRINTS_SW, 0);
    }

    if (entry_handle->table->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC)
    {
        SHR_IF_ERR_EXIT(dbal_sw_table_entry_set_in_index_hl(unit, entry_handle, entry_sw_value, &access_needed));
        payload_pointer = entry_sw_value;
    }
    else if (entry_handle->table->access_method == DBAL_ACCESS_METHOD_MDB)
    {
        SHR_IF_ERR_EXIT(dbal_sw_table_entry_set_in_index_mdb(unit, entry_handle, entry_sw_value, &access_needed));
        payload_pointer = entry_sw_value;
    }
    else
    {
        int field_index;
        int result_type_index = entry_handle->cur_res_type;
        dbal_table_field_info_t *table_field_info;
        payload_pointer = entry_handle->phy_entry.payload;
        access_needed = TRUE;

        for (field_index = 0; field_index < entry_handle->table->multi_res_info[result_type_index].nof_result_fields;
             field_index++)
        {
            table_field_info = &(entry_handle->table->multi_res_info[result_type_index].results_info[field_index]);

            
            if (entry_handle->value_field_ids[field_index] == DBAL_FIELD_EMPTY)
            {
                continue;
            }

            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.entries.bit_range_write(unit, entry_handle->table_id,
                                                                         (entry_index * payload_length_bits) +
                                                                         table_field_info->bits_offset_in_buffer,
                                                                         table_field_info->bits_offset_in_buffer,
                                                                         table_field_info->field_nof_bits,
                                                                         (SHR_BITDCL *) entry_handle->
                                                                         phy_entry.payload));
        }
        
        access_needed = FALSE;
    }

    if (access_needed)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.
                        entries.bit_range_write(unit, entry_handle->table_id, entry_index * payload_length_bits, 0,
                                                payload_length_bits, (SHR_BITDCL *) payload_pointer));
    }

    if (bsl_severity >= bslSeverityInfo)
    {
        uint32 diag_payload[DBAL_PHYSICAL_RES_SIZE_IN_BYTES] = { 0 };
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.
                        entries.bit_range_read(unit, entry_handle->table_id, entry_index * payload_length_bits, 0,
                                               payload_length_bits, (SHR_BITDCL *) diag_payload));

        SHR_IF_ERR_EXIT(dbal_sw_table_entry_dump
                        (unit, entry_handle->table_id, entry_index, diag_payload, payload_length_bytes, "add"));

        DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_SW, 0);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_sw_table_entry_get_in_index_hl(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 *entry_sw_value,
    int result_type_index)
{
    int access_index;
    dbal_table_field_info_t *table_field_info;
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };
    dbal_hl_l2p_info_t l2p_sw_info =
        entry_handle->table->hl_mapping_multi_res[result_type_index].l2p_hl_info[DBAL_HL_ACCESS_SW];
    int logger_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (dbal_logger_is_enable(unit, entry_handle->table_id))
    {
        logger_enable = 1;
    }

    for (access_index = 0; access_index < l2p_sw_info.num_of_access_fields; access_index++)
    {
        int pos_in_interface = l2p_sw_info.l2p_fields_info[access_index].field_pos_in_interface;
        table_field_info = &(entry_handle->table->multi_res_info[result_type_index].results_info[pos_in_interface]);

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (entry_sw_value, l2p_sw_info.l2p_fields_info[access_index].access_offset,
                         l2p_sw_info.l2p_fields_info[access_index].access_nof_bits, field_value));

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_value,
                                                       l2p_sw_info.l2p_fields_info[access_index].offset_in_interface,
                                                       l2p_sw_info.l2p_fields_info[access_index].nof_bits_in_interface,
                                                       entry_handle->phy_entry.payload));

        entry_handle->phy_entry.payload_size += l2p_sw_info.l2p_fields_info[access_index].nof_bits_in_interface;

        DBAL_SW_FIELD_LOG(table_field_info->field_id, field_value[0], 0, table_field_info->field_nof_bits);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_sw_table_entry_get_in_index_mdb(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 *entry_sw_value,
    int result_type_index)
{
    dbal_table_field_info_t *table_field_info;
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };
    int logger_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (dbal_logger_is_enable(unit, entry_handle->table_id))
    {
        logger_enable = 1;
    }

    table_field_info = &(entry_handle->table->multi_res_info[result_type_index].results_info[0]);

    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(entry_sw_value, 0, table_field_info->field_nof_bits, field_value));

    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_value, table_field_info->bits_offset_in_buffer,
                                                   table_field_info->field_nof_bits, entry_handle->phy_entry.payload));

    DBAL_SW_FIELD_LOG(table_field_info->field_id, field_value[0], 0, table_field_info->field_nof_bits);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_sw_table_resolved_res_type_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 *entry_sw_value,
    int *resolved_res_type)
{
    dbal_table_field_info_t *table_field_info;
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };
    int result_type_size;
    int result_type_offset_in_sw_buffer;
    uint32 result_type_mask;

    SHR_FUNC_INIT_VARS(unit);

    table_field_info = &(entry_handle->table->multi_res_info[0].results_info[0]);

    result_type_size = table_field_info->field_nof_bits;

    if (entry_handle->table->access_method == DBAL_ACCESS_METHOD_SW_STATE)
    {
        result_type_offset_in_sw_buffer = table_field_info->bits_offset_in_buffer;
    }
    else if (entry_handle->table->access_method == DBAL_ACCESS_METHOD_MDB)
    {
        result_type_offset_in_sw_buffer = 0;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "access_method == DBAL_ACCESS_METHOD_HARD_LOGIC, iw wrong here\n");
    }

    result_type_mask = ((1 << result_type_size) - 1);

    if (result_type_offset_in_sw_buffer)
    {
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (entry_sw_value, result_type_offset_in_sw_buffer, table_field_info->field_nof_bits,
                         field_value));
    }
    else
    {
        
        field_value[0] = entry_sw_value[0];
    }

    *resolved_res_type = entry_sw_value[0] & result_type_mask;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_sw_table_entry_get_in_index(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int entry_index,
    int res_type_get,
    int *resolved_res_type)
{
    uint32 entry_sw_value[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };
    uint32 *buffer_pointer;
    int result_type_index = 0;
    int bsl_severity;
    uint32 payload_length_bytes = entry_handle->table->sw_access_info.sw_payload_length_bytes;
    uint32 payload_length_bits = BYTES2BITS(payload_length_bytes);

    SHR_FUNC_INIT_VARS(unit);

    SHR_GET_SEVERITY_FOR_MODULE(bsl_severity);

    if (entry_handle->table->access_method == DBAL_ACCESS_METHOD_SW_STATE)
    {
        

        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.
                        entries.bit_range_read(unit, entry_handle->table_id, entry_index * payload_length_bits, 0,
                                               payload_length_bits, (SHR_BITDCL *) entry_handle->phy_entry.payload));

        entry_handle->phy_entry.payload_size =
            entry_handle->table->multi_res_info[result_type_index].entry_payload_size;
        buffer_pointer = entry_handle->phy_entry.payload;
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.
                        entries.bit_range_read(unit, entry_handle->table_id, entry_index * payload_length_bits, 0,
                                               payload_length_bits, (SHR_BITDCL *) entry_sw_value));
        buffer_pointer = entry_sw_value;
    }

    if (bsl_severity >= bslSeverityInfo)
    {
        DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(TRUE, bslSeverityInfo, DBAL_ACCESS_PRINTS_SW, 1);
        SHR_IF_ERR_EXIT(dbal_sw_table_entry_dump
                        (unit, entry_handle->table_id, entry_index, buffer_pointer, payload_length_bytes, "get from"));
    }

    
    if (resolved_res_type != NULL)
    {
        SHR_IF_ERR_EXIT(dbal_sw_table_resolved_res_type_get(unit, entry_handle, buffer_pointer, resolved_res_type));
        SHR_EXIT();
    }

    if (res_type_get != DBAL_INVALID_PARAM)
    {
        result_type_index = res_type_get;
    }
    else
    {
        result_type_index = entry_handle->cur_res_type;
    }

    if (result_type_index == DBAL_RESULT_TYPE_NOT_INITIALIZED)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "result type not initialized for entry. table %s\n",
                     entry_handle->table->table_name);
    }

    switch (entry_handle->table->access_method)
    {
        case DBAL_ACCESS_METHOD_SW_STATE:
            
            break;
        case DBAL_ACCESS_METHOD_HARD_LOGIC:
            SHR_IF_ERR_EXIT(dbal_sw_table_entry_get_in_index_hl(unit, entry_handle, entry_sw_value, result_type_index));
            break;
        case DBAL_ACCESS_METHOD_MDB:
            SHR_IF_ERR_EXIT(dbal_sw_table_entry_get_in_index_mdb
                            (unit, entry_handle, entry_sw_value, result_type_index));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Access method %s cannot involve SW fields\n",
                         dbal_access_method_to_string(unit, entry_handle->table->access_method));
            break;
    }

    DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_SW, 1);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_sw_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int ii, jj;
    dbal_sw_state_table_type_e table_type;
    int resolved_result_type = 0;

    SHR_FUNC_INIT_VARS(unit);

    dbal_logger_internal_disable_set(unit);

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.table_type.get(unit, entry_handle->table_id, &table_type));
    switch (table_type)
    {
        case DBAL_SW_TABLE_DIRECT:
            SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_get
                            (unit, entry_handle, DBAL_INVALID_PARAM, &resolved_result_type));
            break;
        case DBAL_SW_TABLE_HASH:
            SHR_SET_CURRENT_ERR(dbal_sw_table_hash_entry_get(unit, entry_handle, &resolved_result_type));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "table type not supported %d\n", table_type);
            break;
    }

    for (ii = 0; ii < entry_handle->table->nof_result_types; ii++)
    {
        for (jj = 0; jj < entry_handle->table->multi_res_info[ii].result_type_nof_hw_values; jj++)
        {
            if (entry_handle->table->multi_res_info[ii].result_type_hw_value[jj] == resolved_result_type)
            {
                entry_handle->cur_res_type = ii;
                SHR_EXIT();
            }
        }
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);

exit:
    dbal_logger_internal_disable_clear(unit);
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_sw_table_direct_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int key_length, key_mask, entry_index;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    key_length = entry_handle->phy_entry.key_size;
    key_mask = ((1 << key_length) - 1);

    entry_index = key_mask & entry_handle->phy_entry.key[0];

    {
        DBAL_ITERATE_OVER_CORES(entry_handle, core_id)
        {
            DBAL_SWSTATE_DPC_TBL_INGORE(unit, entry_handle->table->core_mode, core_id);
            if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
            {
            
                entry_index &= key_mask;
                entry_index |= (core_id << key_length);
            }

            SHR_IF_ERR_EXIT(dbal_sw_table_entry_set_in_index(unit, entry_handle, entry_index));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_sw_table_direct_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int res_type_get,
    int *resolved_res_type)
{
    int key_length, key_mask, entry_index;

    SHR_FUNC_INIT_VARS(unit);

    key_length = entry_handle->phy_entry.key_size;
    key_mask = ((1 << key_length) - 1);

    entry_index = key_mask & entry_handle->phy_entry.key[0];

    if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
    {
        if (entry_handle->core_id >= 0)
        {
            entry_index |= (entry_handle->core_id << key_length);
        }
    }

    SHR_IF_ERR_EXIT(dbal_sw_table_entry_get_in_index(unit, entry_handle, entry_index, res_type_get, resolved_res_type));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_sw_table_direct_entry_clear_in_index(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int entry_index)
{
    uint8 default_payload[DBAL_PHYSICAL_RES_SIZE_IN_BYTES] = { 0 };
    int bsl_severity;
    uint32 payload_length_bytes = entry_handle->table->sw_access_info.sw_payload_length_bytes;
    uint32 payload_length_bits = BYTES2BITS(payload_length_bytes);

    SHR_FUNC_INIT_VARS(unit);

    SHR_GET_SEVERITY_FOR_MODULE(bsl_severity);

    if (bsl_severity >= bslSeverityInfo)
    {
        DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(TRUE, bslSeverityInfo, DBAL_ACCESS_PRINTS_SW, 2);
        LOG_CLI((BSL_META_U(unit, "DBAL SW Access (direct). Entry clear\n")));
        LOG_CLI((BSL_META_U(unit, "Entry index=0x%08x\n"), entry_index));
        DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_SW, 2);
    }

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.
                    entries.bit_range_write(unit, entry_handle->table_id, entry_index * payload_length_bits, 0,
                                            payload_length_bits, (SHR_BITDCL *) default_payload));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_sw_table_direct_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int key_length, key_mask, entry_index;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    key_length = entry_handle->phy_entry.key_size;
    key_mask = ((1 << key_length) - 1);

    entry_index = key_mask & entry_handle->phy_entry.key[0];

    {
        DBAL_ITERATE_OVER_CORES(entry_handle, core_id)
        {
            DBAL_SWSTATE_DPC_TBL_INGORE(unit, entry_handle->table->core_mode, core_id);
            if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
            {
            
                entry_index &= key_mask;
                entry_index |= (core_id << key_length);
            }

            SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_clear_in_index(unit, entry_handle, entry_index));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_sw_table_direct_iterator_init(
    int unit,
    dbal_tables_e table_id,
    uint32 key_size,
    dbal_iterator_info_t * dbal_iterator)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_max_key_value_get(unit, table_id, &dbal_iterator->max_num_of_iterations));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_sw_table_direct_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_key_value_validity_e is_valid_entry = DBAL_KEY_IS_INVALID;
    uint8 entry_found = FALSE;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

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
        if (entry_handle->table->allocator_field_id != DBAL_FIELD_EMPTY)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_increment_by_allocator(unit, entry_handle, &is_valid_entry));
        }
        if (is_valid_entry == DBAL_KEY_IS_VALID)
        {
            SHR_IF_ERR_EXIT(dbal_key_buffer_validate(unit, entry_handle, &is_valid_entry));
        }

        
        if (is_valid_entry == DBAL_KEY_IS_VALID)
        {
            int rv;
            entry_handle->get_all_fields = TRUE;
            sal_memset(entry_handle->phy_entry.payload, 0, DBAL_PHYSICAL_RES_SIZE_IN_BYTES);

            if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
            {
                entry_handle->cur_res_type = DBAL_RESULT_TYPE_NOT_INITIALIZED;

                rv = dbal_sw_res_type_resolution(unit, entry_handle);
                if (rv != _SHR_E_NONE)
                {
                    if (rv == _SHR_E_NOT_FOUND)
                    {
                        continue;
                    }
                    else
                    {
                        SHR_ERR_EXIT(rv, "SW entry get");
                    }
                }
            }
            else
            {
                entry_handle->cur_res_type = 0;
            }

            SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_get(unit, entry_handle, DBAL_INVALID_PARAM, NULL));

            
            if (iterator_info->mode == DBAL_ITER_MODE_ALL)
            {
                entry_found = TRUE;
            }
            else if (sal_memcmp
                     (zero_buffer_to_compare, entry_handle->phy_entry.payload, DBAL_PHYSICAL_RES_SIZE_IN_BYTES) != 0)
            {
                                
                entry_found = TRUE;
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


shr_error_e
dbal_sw_table_hash_entry_add(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    uint32 data_indx = UTILEX_U32_MAX;
    uint8 success = FALSE;
    uint32 key_val[BITS2WORDS(DBAL_SW_HASH_TABLES_MAX_HASH_KEY_SIZE_BITS)] = { 0 };
    dbal_sw_state_hash_key key;
    int core_id;
    int key_length = entry_handle->table->key_size;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&key, 0, sizeof(key));

    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(entry_handle->phy_entry.key, 0, entry_handle->phy_entry.key_size,
                                                   key_val));
    {
        DBAL_ITERATE_OVER_CORES(entry_handle, core_id)
        {
            DBAL_SWSTATE_DPC_TBL_INGORE(unit, entry_handle->table->core_mode, core_id);
            if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
            {
                
                SHR_IF_ERR_EXIT(utilex_bitstream_set_field(key_val, key_length - DBAL_CORE_SIZE_IN_BITS,
                                                           DBAL_CORE_SIZE_IN_BITS, core_id));
            }

            SHR_IF_ERR_EXIT(utilex_U32_to_U8(key_val, WORDS2BYTES(BITS2WORDS(key_length)), (uint8 *) &key));

            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.
                            hash_table.insert(unit, entry_handle->table_id, &key, &data_indx, &success));
            if (!success)
            {
                int nof_entries;
                SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_entries.get(unit, entry_handle->table_id, &nof_entries));
                if (entry_handle->table->max_capacity <= nof_entries)
                {
                    SHR_ERR_EXIT(SOC_E_FULL,
                                 "cant add entry to hash sw table %s, table is full, hash table ID %d max capacity is %d\n",
                                 entry_handle->table->table_name, entry_handle->table_id,
                                 entry_handle->table->max_capacity);

                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_RESOURCE, "cant add entry to hash sw table %s hash table ID %d\n",
                                 entry_handle->table->table_name, entry_handle->table_id);
                }
            }
            SHR_IF_ERR_EXIT(dbal_sw_table_entry_set_in_index(unit, entry_handle, data_indx));
            if (entry_handle->table->core_mode == DBAL_CORE_MODE_SBC)
            {
                
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_sw_table_hash_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *resolved_res_type)
{
    uint32 data_indx = { 0 };
    uint8 found = FALSE;
    uint32 key_val[BITS2WORDS(DBAL_SW_HASH_TABLES_MAX_HASH_KEY_SIZE_BITS)] = { 0 };
    dbal_sw_state_hash_key key;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&key, 0, sizeof(key));
    entry_handle->phy_entry.key_size = entry_handle->table->key_size;

    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(entry_handle->phy_entry.key, 0, entry_handle->phy_entry.key_size,
                                                   key_val));
    if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
    {
        int key_length = entry_handle->phy_entry.key_size - DBAL_CORE_SIZE_IN_BITS;
        if (entry_handle->core_id >= 0)
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_set_field
                            (key_val, key_length, DBAL_CORE_SIZE_IN_BITS, entry_handle->core_id));
        }
        SHR_IF_ERR_EXIT(utilex_U32_to_U8
                        (key_val, WORDS2BYTES(BITS2WORDS(entry_handle->phy_entry.key_size)), (uint8 *) &key));
    }
    else
    {
        SHR_IF_ERR_EXIT(utilex_U32_to_U8
                        (key_val, WORDS2BYTES(BITS2WORDS(entry_handle->phy_entry.key_size)), (uint8 *) &key));
    }

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.hash_table.find(unit, entry_handle->table_id, &key, &data_indx, &found));
    if (!found)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_sw_table_entry_get_in_index
                        (unit, entry_handle, data_indx, DBAL_INVALID_PARAM, resolved_res_type));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_sw_table_hash_entry_delete(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    uint32 key_val[BITS2WORDS(DBAL_SW_HASH_TABLES_MAX_HASH_KEY_SIZE_BITS)] = { 0 };
    dbal_sw_state_hash_key key;
    int bsl_severity;
    int core_id;
    uint32 data_indx = { 0 };
    uint8 found = FALSE;
    int key_length = entry_handle->table->key_size;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&key, 0, sizeof(key));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(entry_handle->phy_entry.key, 0, key_length, key_val));

    {
        DBAL_ITERATE_OVER_CORES(entry_handle, core_id)
        {
            DBAL_SWSTATE_DPC_TBL_INGORE(unit, entry_handle->table->core_mode, core_id);
            if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
            {
                
                SHR_IF_ERR_EXIT(utilex_bitstream_set_field(key_val, key_length - DBAL_CORE_SIZE_IN_BITS,
                                                           DBAL_CORE_SIZE_IN_BITS, core_id));
            }

            SHR_IF_ERR_EXIT(utilex_U32_to_U8(key_val, WORDS2BYTES(BITS2WORDS(key_length)), (uint8 *) &key));

            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.
                            hash_table.find(unit, entry_handle->table_id, &key, &data_indx, &found));
            if (!found)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.hash_table.delete(unit, entry_handle->table_id, &key));

            SHR_GET_SEVERITY_FOR_MODULE(bsl_severity);
            if (bsl_severity >= bslSeverityInfo)
            {
                if (dbal_logger_is_enable(unit, entry_handle->table_id))
                {
                    DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(TRUE, bslSeverityInfo, DBAL_ACCESS_PRINTS_SW, 2);
                    LOG_CLI((BSL_META_U(unit, "DBAL SW Access (hash). Entry clear\n")));
                    if (key_length > 32)
                    {
                        LOG_CLI((BSL_META_U(unit, "hash key=0x%02x%02x%02x%02x%02x%02x%02x%02x\n"),
                                 ((uint8 *) &key)[7], ((uint8 *) &key)[6], ((uint8 *) &key)[5], ((uint8 *) &key)[4],
                                 ((uint8 *) &key)[3], ((uint8 *) &key)[2], ((uint8 *) &key)[1], ((uint8 *) &key)[0]));
                    }
                    else
                    {
                        LOG_CLI((BSL_META_U(unit, "hash key=0x%02x%02x%02x%02x\n"), ((uint8 *) &key)[3],
                                 ((uint8 *) &key)[2], ((uint8 *) &key)[1], ((uint8 *) &key)[0]));
                    }
                    DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_SW, 2);
                }
            }
            SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_clear_in_index(unit, entry_handle, data_indx));
            if (entry_handle->table->core_mode == DBAL_CORE_MODE_SBC)
            {
                
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_sw_table_hash_iterator_init(
    int unit,
    dbal_tables_e table_id,
    dbal_sw_table_iterator_t * sw_iterator)
{
    SHR_FUNC_INIT_VARS(unit);

    sw_iterator->hash_table_id = table_id;
    sw_iterator->hash_entry_index = 0;

    SHR_FUNC_EXIT;
}


shr_error_e
dbal_sw_table_hash_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    uint32 data_indx;
    dbal_sw_state_hash_key key;
    dbal_iterator_info_t *iterator_info;
    dbal_key_value_validity_e is_valid_entry = DBAL_KEY_IS_INVALID;

    SHR_FUNC_INIT_VARS(unit);

    do
    {
        SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

        iterator_info->used_first_key = TRUE;

        sal_memset(&key, 0, sizeof(key));

        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.hash_table.get_next(unit, iterator_info->sw_iterator.hash_table_id,
                                                                 &iterator_info->sw_iterator.hash_entry_index, &key,
                                                                 &data_indx));

        if (SW_STATE_HASH_TABLE_ITER_IS_END(&iterator_info->sw_iterator.hash_entry_index))
        {
            iterator_info->is_end = TRUE;
            break;
        }

        entry_handle->phy_entry.key_size = entry_handle->table->key_size;

        SHR_IF_ERR_EXIT(utilex_U8_to_U32
                        ((uint8 *) &key, BITS2BYTES(DBAL_SW_HASH_TABLES_MAX_HASH_KEY_SIZE_BITS),
                         entry_handle->phy_entry.key));

        if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
        {
            int key_length = entry_handle->phy_entry.key_size - DBAL_CORE_SIZE_IN_BITS;
            uint32 core_id_u32 = 0;
            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(entry_handle->phy_entry.key, key_length,
                                                           DBAL_CORE_SIZE_IN_BITS, &core_id_u32));
            entry_handle->core_id = core_id_u32;
        }

        SHR_IF_ERR_EXIT(dbal_key_buffer_validate(unit, entry_handle, &is_valid_entry));
        if (is_valid_entry == DBAL_KEY_IS_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "SW Hash iterator found invalid entry\n");
        }
        else if (is_valid_entry == DBAL_KEY_IS_OUT_OF_ITERATOR_RULE)
        {
            continue;
        }

        entry_handle->get_all_fields = TRUE;
        sal_memset(entry_handle->phy_entry.payload, 0, DBAL_PHYSICAL_RES_SIZE_IN_BYTES);

        if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
        {
            entry_handle->cur_res_type = DBAL_RESULT_TYPE_NOT_INITIALIZED;
            SHR_IF_ERR_EXIT(dbal_sw_res_type_resolution(unit, entry_handle));
        }
        else
        {
            entry_handle->cur_res_type = 0;
        }
        SHR_IF_ERR_EXIT(dbal_sw_table_hash_entry_get(unit, entry_handle, NULL));

        break;

    }
    while (!SW_STATE_HASH_TABLE_ITER_IS_END(&iterator_info->sw_iterator.hash_entry_index));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_sw_table_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_sw_state_table_type_e table_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.table_type.get(unit, entry_handle->table_id, &table_type));
    switch (table_type)
    {
        case DBAL_SW_TABLE_DIRECT:
            SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_set(unit, entry_handle));
            break;
        case DBAL_SW_TABLE_HASH:
            SHR_IF_ERR_EXIT(dbal_sw_table_hash_entry_add(unit, entry_handle));
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_sw_table_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_sw_state_table_type_e table_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.table_type.get(unit, entry_handle->table_id, &table_type));
    switch (table_type)
    {
        case DBAL_SW_TABLE_DIRECT:
            SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_get(unit, entry_handle, DBAL_INVALID_PARAM, NULL));
            break;
        case DBAL_SW_TABLE_HASH:
            SHR_SET_CURRENT_ERR(dbal_sw_table_hash_entry_get(unit, entry_handle, NULL));
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_sw_table_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_sw_state_table_type_e table_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.table_type.get(unit, entry_handle->table_id, &table_type));
    switch (table_type)
    {
        case DBAL_SW_TABLE_DIRECT:
            SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_clear(unit, entry_handle));
            break;

        case DBAL_SW_TABLE_HASH:
            SHR_IF_ERR_EXIT(dbal_sw_table_hash_entry_delete(unit, entry_handle));
            break;

        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_sw_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_sw_state_table_type_e table_type;
    uint32 entry_index, max_entry_index, key_size;
    uint8 default_payload[DBAL_PHYSICAL_RES_SIZE_IN_BYTES] = { 0 };
    uint32 payload_length_bytes = entry_handle->table->sw_access_info.sw_payload_length_bytes;
    uint32 payload_length_bits = BYTES2BITS(payload_length_bytes);

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.table_type.get(unit, entry_handle->table_id, &table_type));
    switch (table_type)
    {
        case DBAL_SW_TABLE_DIRECT:
            key_size = entry_handle->table->key_size;
            max_entry_index = utilex_power_of_2(key_size);
            for (entry_index = 0; entry_index < max_entry_index; entry_index++)
            {
                SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.
                                entries.bit_range_write(unit, entry_handle->table_id, entry_index * payload_length_bits,
                                                        0, payload_length_bits, (SHR_BITDCL *) default_payload));
            }
            break;

        case DBAL_SW_TABLE_HASH:
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.hash_table.clear(unit, entry_handle->table_id));
            break;

        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_sw_table_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_sw_state_table_type_e sw_table_type;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.table_type.get(unit, entry_handle->table_id, &sw_table_type));

    switch (sw_table_type)
    {
        case DBAL_SW_TABLE_DIRECT:
            SHR_IF_ERR_EXIT(dbal_sw_table_direct_iterator_init(unit, entry_handle->table_id,
                                                               entry_handle->phy_entry.key_size, iterator_info));
            break;

        case DBAL_SW_TABLE_HASH:
            SHR_IF_ERR_EXIT(dbal_sw_table_hash_iterator_init(unit, entry_handle->table_id,
                                                             &iterator_info->sw_iterator));
            break;

        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_sw_table_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_sw_state_table_type_e sw_table_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.table_type.get(unit, entry_handle->table_id, &sw_table_type));

    switch (sw_table_type)
    {
        case DBAL_SW_TABLE_DIRECT:
            SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_get_next(unit, entry_handle));
            break;

        case DBAL_SW_TABLE_HASH:
            SHR_IF_ERR_EXIT(dbal_sw_table_hash_entry_get_next(unit, entry_handle));
            break;

        default:
            break;
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_sw_table_nof_entries_calculate(
    int unit,
    dbal_logical_table_t * table,
    uint32 *num_of_entries)
{
    SHR_FUNC_INIT_VARS(unit);

    (*num_of_entries) = 0;

    if (table->table_type == DBAL_TABLE_TYPE_DIRECT)
    {
        if (table->key_size > DBAL_SW_DIRECT_TABLES_MAX_KEY_SIZE_BITS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "key is too long.  current key %d bits. table %s\n",
                         table->key_size, table->table_name);
        }
        
        (*num_of_entries) = utilex_power_of_2(table->key_size);
    }
    else
    {
        if (table->max_capacity == 0)
        {
            if (table->access_method == DBAL_ACCESS_METHOD_MDB)
            {
                
                int jj;

                for (jj = 0; jj < table->nof_physical_tables; jj++)
                {
                    int max_capacity;
                    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, table->physical_db_id[jj], &max_capacity));
                    (*num_of_entries) += (uint32) max_capacity;
                }
            }
            else if (table->access_method == DBAL_ACCESS_METHOD_SW_STATE)
            {
                dbal_db_int_or_dnx_data_info_struct_t indirect_table_size_dnx_data;
                SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                                (unit, &indirect_table_size_dnx_data, table->sw_access_info.table_size_str, TRUE, 0, "",
                                 ""));

                (*num_of_entries) = (uint32) indirect_table_size_dnx_data.int_val;
                table->max_capacity = (*num_of_entries);
            }
        }
        else
        {
            (*num_of_entries) = table->max_capacity;
        }

        if (BITS2BYTES(table->key_size) > BITS2BYTES(DBAL_SW_HASH_TABLES_MAX_HASH_KEY_SIZE_BITS))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "key is too long.  max key size for sw state HASH is %u bytes. table %s\n",
                         (unsigned) BITS2BYTES(DBAL_SW_HASH_TABLES_MAX_HASH_KEY_SIZE_BITS), table->table_name);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
