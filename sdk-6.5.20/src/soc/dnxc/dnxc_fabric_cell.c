/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC

#include <shared/bsl.h>

#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnxc/dnxc_fabric_cell.h>


shr_error_e
soc_dnxc_fabric_cell_table_field_row_get(
    int unit,
    soc_dnxc_fabric_cell_field_t field,
    soc_dnxc_fabric_cell_parse_table_t * table,
    int * row)
{
    int current_row = 0, row_found = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    while(table[current_row].dest_id != soc_dnxc_fabric_cell_field_invalid)
    {
        if (table[current_row].dest_id == field)
        {
            *row = current_row;
            row_found = TRUE;
            break;
        }

        current_row++;
    }

    if(!row_found)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Searched field %d is not found in provided cell parse table!", field);
    }
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
soc_dnxc_fabric_cell_table_dest_fill(
    int unit,
    soc_dnxc_fabric_cell_parse_table_t * parse_table,
    soc_dnxc_fabric_cell_info_t * cell_info)
{
    int line = 0;
    SHR_FUNC_INIT_VARS(unit);

    while(parse_table[line].dest_id != soc_dnxc_fabric_cell_field_invalid)
    {
        switch (parse_table[line].dest_id)
        {
            case soc_dnxc_fabric_cell_field_cell_type:
                if (cell_info->cell_type == soc_dnxc_fabric_cell_type_sr_cell)
                {
                    parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.cell_type;
                }
                else
                {
                    parse_table[line].dest = (uint32 *) &cell_info->cell.captured_cell.cell_type;
                }
                break;
            case soc_dnxc_fabric_cell_field_src_device:
                if (cell_info->cell_type == soc_dnxc_fabric_cell_type_sr_cell)
                {
                    parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.source_device;
                }
                else
                {
                    parse_table[line].dest = (uint32 *) &cell_info->cell.captured_cell.source_device;
                }
                break;
            case soc_dnxc_fabric_cell_field_src_level:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.source_level;
                break;
            case soc_dnxc_fabric_cell_field_dest_level:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.destination_level;
                break;
            case soc_dnxc_fabric_cell_field_fip:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.fip_link;
                break;
            case soc_dnxc_fabric_cell_field_fe1:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.fe1_link;
                break;
            case soc_dnxc_fabric_cell_field_fe2:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.fe2_link;
                break;
            case soc_dnxc_fabric_cell_field_fe3:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.fe3_link;
                break;
            case soc_dnxc_fabric_cell_field_is_inband:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.is_inband;
                break;
            case soc_dnxc_fabric_cell_field_ack:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.ack;
                break;
            case soc_dnxc_fabric_cell_field_pipe_id:
                if (cell_info->cell_type == soc_dnxc_fabric_cell_type_sr_cell)
                {
                    parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.pipe_id;
                }
                else
                {
                    parse_table[line].dest = (uint32 *) &cell_info->cell.captured_cell.pipe_index;
                }
                break;
            case soc_dnxc_fabric_cell_field_data_in_msb:
                if (cell_info->cell_type == soc_dnxc_fabric_cell_type_sr_cell)
                {
                    parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.data_in_msb;
                }
                else
                {
                    parse_table[line].dest = (uint32 *) &cell_info->cell.captured_cell.data_in_msb;
                }
                break;
            case soc_dnxc_fabric_cell_field_cell_size:
                if (cell_info->cell_type == soc_dnxc_fabric_cell_type_sr_cell)
                {
                    parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.cell_size;
                }
                else
                {
                    parse_table[line].dest = (uint32 *) &cell_info->cell.captured_cell.cell_size;
                }
                break;
            case soc_dnxc_fabric_cell_field_payload:
                if (cell_info->cell_type == soc_dnxc_fabric_cell_type_sr_cell)
                {
                    parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.payload.data;
                }
                else
                {
                    parse_table[line].dest = (uint32 *) cell_info->cell.captured_cell.data;
                }
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "UNAVAIL line id");
                break;
        }
        line++;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_fabric_cell_parser(
    int unit,
    soc_dnxc_fabric_cell_entry_t entry,
    soc_dnxc_fabric_cell_parse_table_t * parse_table,
    soc_dnxc_fabric_cell_info_t * cell_info)
{
    int line = 0, nof_uint32, i, rv;
    soc_dnxc_fabric_cell_entry_t val;
    SHR_FUNC_INIT_VARS(unit);

    rv = soc_dnxc_fabric_cell_table_dest_fill(unit, parse_table, cell_info);
    SHR_IF_ERR_EXIT(rv);

    sal_memset(val, 0x00, sizeof(soc_dnxc_fabric_cell_entry_t));

    LOG_DEBUG(BSL_LS_SOC_PKT, (BSL_META_U(unit, "Received Cell Parse\n")));
    LOG_DEBUG(BSL_LS_SOC_PKT, (BSL_META_U(unit, "-------------------\n")));
    LOG_DEBUG(BSL_LS_SOC_PKT, (BSL_META_U(unit, "Entry print: ")));
    for (i = 0; i < SOC_DNXC_FABRIC_CELL_ENTRY_MAX_SIZE_UINT32; i++)
    {
        LOG_DEBUG(BSL_LS_SOC_PKT, (BSL_META_U(unit, "0x%08x, "), entry[i]));
    }
    LOG_DEBUG(BSL_LS_SOC_PKT, (BSL_META_U(unit, "\n")));

    while(parse_table[line].dest_id != soc_dnxc_fabric_cell_field_invalid)
    {
        
        SHR_BITCOPY_RANGE(val, 0, entry, parse_table[line].src_start_bit, parse_table[line].length);

        
        nof_uint32 = (parse_table[line].length + 31) / 32;      
        if (nof_uint32 == 1)
        {
            LOG_DEBUG(BSL_LS_SOC_PKT, (BSL_META_U(unit, "%s: 0x%x"), parse_table[line].field_name, *val));
        }
        else
        {
            LOG_DEBUG(BSL_LS_SOC_PKT, (BSL_META_U(unit, "%s: 0x%08x"), parse_table[line].field_name, *val));
            for (i = 1; i < nof_uint32; i++)
            {
                LOG_DEBUG(BSL_LS_SOC_PKT, (BSL_META_U(unit, ", 0x%08x"), val[i]));
            }
        }
        LOG_DEBUG(BSL_LS_SOC_PKT, (BSL_META_U(unit, "\n")));

        
        SHR_BITCOPY_RANGE(parse_table[line].dest, parse_table[line].dest_start_bit, val, 0, parse_table[line].length);

        SHR_BITCLR_RANGE(val, 0, parse_table[line].length);

        line++;
    }
    LOG_DEBUG(BSL_LS_SOC_PKT, (BSL_META_U(unit, "-------------------\n\n")));

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
