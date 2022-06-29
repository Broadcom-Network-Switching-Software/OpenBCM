/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC

#include <shared/bsl.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/bitop.h>
#include <soc/drv.h>
#include <soc/dnxc/error.h>
#include <soc/dnxc/dnxc_fabric_cell.h>
#include <soc/dnxc/dnxc_memregs.h>
#include <soc/dnxc/dnxc_error.h>
#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnxc/dnxc_fabric_source_routed_cell.h>
#include <soc/dnxc/dnxc_fabric_cell.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#endif
#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#endif

#define DNXC_FABRIC_CELL_FTMH_FORMAT_TABLE { \
        \
        {soc_dnxc_ftmh_base_header_field_visibility,        NULL,          0,                87,            1,       "VISIBILITY"},\
        {soc_dnxc_ftmh_base_header_field_tm_profile,        NULL,          0,                88,            2,       "TM_PROFILE"},\
        {soc_dnxc_ftmh_base_header_field_pp_dsp,            NULL,          0,                55,            8,       "PP_DSP"},\
        {soc_dnxc_ftmh_base_header_field_paket_size,        NULL,          0,                18,            14,      "PKT_SIZE"},\
         \
        {soc_dnxc_ftmh_base_header_field_invalid,           NULL,          0,                -1,            -1,      "INVALID"}\
    }

#define DNXC_VSC256_FABRIC_CELL_DESTINATION_ROUTED_CELL_COMMAN_FORMAT_TABLE  \
    \
    {soc_dnxc_fabric_cell_field_cell_type,              NULL,          0,                0,             2,      "CELL_TYPE"},\
    {soc_dnxc_fabric_cell_field_destination,            NULL,          0,                11,            19,     "DESTINATION"},\
    {soc_dnxc_fabric_cell_field_src_device,             NULL,          0,                30,            11,     "SOURCE_DEVICE"},\
    {soc_dnxc_fabric_cell_field_fragment_number,        NULL,          0,                41,            7,      "FRAGMENT_NUMBER"}

#define DNXC_VSC256_FABRIC_CELL_DESTINATION_ROUTED_CELL_1_PIPE_FORMAT_TABLE { \
    \
    DNXC_VSC256_FABRIC_CELL_DESTINATION_ROUTED_CELL_COMMAN_FORMAT_TABLE, \
    {soc_dnxc_fabric_cell_field_last_cell_of_packet,    NULL,          0,                8,             1,      "IS_LAST_CELL_OF_PKT"},\
    {soc_dnxc_fabric_cell_field_pcp_type,               NULL,          0,                72,            1,      "PCP_TYPE"},\
     \
    {soc_dnxc_fabric_cell_field_invalid,                NULL,          0,                -1,            -1,     "INVALID"}\
    }

#define DNXC_VSC256_FABRIC_CELL_DESTINATION_ROUTED_CELL_2_PIPE_FORMAT_TABLE { \
    \
    DNXC_VSC256_FABRIC_CELL_DESTINATION_ROUTED_CELL_COMMAN_FORMAT_TABLE, \
    {soc_dnxc_fabric_cell_field_last_cell_of_packet,    NULL,          0,                8,             1,       "IS_LAST_CELL_OF_PKT"},\
    {soc_dnxc_fabric_cell_field_pcp_type,               NULL,          0,                88,            1,       "PCP_TYPE"},\
     \
    {soc_dnxc_fabric_cell_field_invalid,                NULL,          0,                -1,            -1,      "INVALID"}\
    }

#define DNXC_VSC256_FABRIC_CELL_DESTINATION_ROUTED_CELL_3_PIPE_FORMAT_TABLE { \
    \
    DNXC_VSC256_FABRIC_CELL_DESTINATION_ROUTED_CELL_COMMAN_FORMAT_TABLE, \
    {soc_dnxc_fabric_cell_field_last_cell_of_packet,    NULL,          0,                88,             1,      "IS_LAST_CELL_OF_PKT"},\
    {soc_dnxc_fabric_cell_field_pcp_type,               NULL,          0,                112,            1,      "PCP_TYPE"},\
     \
    {soc_dnxc_fabric_cell_field_invalid,                NULL,          0,                -1,            -1,      "INVALID"}\
    }

#define DNXC_VSC512_FABRIC_CELL_DESTINATION_ROUTED_CELL_COMMAN_FORMAT_TABLE  { \
    \
    {soc_dnxc_fabric_cell_field_cell_type,              NULL,          0,                0,             2,      "CELL_TYPE"},\
    {soc_dnxc_fabric_cell_field_last_cell_of_packet,    NULL,          0,                8,             1,      "IS_LAST_CELL_OF_PKT"},\
    {soc_dnxc_fabric_cell_field_destination,            NULL,          0,                11,            19,     "DESTINATION"},\
    {soc_dnxc_fabric_cell_field_src_device,             NULL,          0,                31,            13,     "SOURCE_DEVICE"},\
    {soc_dnxc_fabric_cell_field_fragment_number,        NULL,          0,                44,            7,      "FRAGMENT_NUMBER"}, \
     \
    {soc_dnxc_fabric_cell_field_invalid,                NULL,          0,                -1,            -1,      "INVALID"}\
    }

shr_error_e
soc_dnxc_fabric_cell_table_field_row_get(
    int unit,
    soc_dnxc_fabric_cell_field_t field,
    soc_dnxc_fabric_cell_parse_table_t * table,
    int *row)
{
    int current_row = 0, row_found = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    while (table[current_row].dest_id != soc_dnxc_fabric_cell_field_invalid)
    {
        if (table[current_row].dest_id == field)
        {
            *row = current_row;
            row_found = TRUE;
            break;
        }

        current_row++;
    }

    if (!row_found)
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

    while (parse_table[line].dest_id != soc_dnxc_fabric_cell_field_invalid)
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

    while (parse_table[line].dest_id != soc_dnxc_fabric_cell_field_invalid)
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

shr_error_e
soc_dnxc_ftmh_base_build_header(
    int unit,
    int pp_destination_port,
    int paket_size,
    int buf_size,
    uint8 *buf)
{
    int line = 0;

    uint32 visibility = 1, tm_profile = 3;
    uint32 pkt_size = paket_size, pp_dsp = pp_destination_port;
    soc_dnxc_fabric_cell_parse_table_t parse_table[] = DNXC_FABRIC_CELL_FTMH_FORMAT_TABLE;
    uint32 buffer_temp[DNXC_FTMH_BASE_HEADER_SIZE_WORDS];

    SHR_FUNC_INIT_VARS(unit);

    if (buf_size < DNXC_FTMH_BASE_HEADER_SIZE_BYTES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FTMH header buffer size should be more than %d",
                     DNXC_FTMH_BASE_HEADER_SIZE_BYTES);
    }

    sal_memset(buffer_temp, 0, DNXC_FTMH_BASE_HEADER_SIZE_BYTES);

    while (parse_table[line].dest_id != soc_dnxc_ftmh_base_header_field_invalid)
    {
        switch (parse_table[line].dest_id)
        {

            case soc_dnxc_ftmh_base_header_field_visibility:
                parse_table[line].dest = &visibility;
                break;
            case soc_dnxc_ftmh_base_header_field_tm_profile:
                parse_table[line].dest = &tm_profile;
                break;
            case soc_dnxc_ftmh_base_header_field_pp_dsp:
                parse_table[line].dest = &pp_dsp;
                break;
            case soc_dnxc_ftmh_base_header_field_paket_size:
                parse_table[line].dest = &pkt_size;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "UNAVAIL line id for creating data cell");
                break;
        }
        line++;
    }

    line = 0;
    while (parse_table[line].dest_id != soc_dnxc_ftmh_base_header_field_invalid)
    {
        SHR_BITCOPY_RANGE(buffer_temp, parse_table[line].src_start_bit, parse_table[line].dest, 0,
                          parse_table[line].length);
        line++;
    }

    SHR_IF_ERR_EXIT(utilex_U32_to_U8_be(buffer_temp, DNXC_FTMH_BASE_HEADER_SIZE_BYTES, buf));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_dest_routed_cell_build_header(
    int unit,
    dnxc_dest_routed_cell_t * cell,
    int buf_size,
    uint32 *buf)
{
    int line = 0;
    soc_dnxc_fabric_cell_parse_table_t vsc256_parse_table_1_pipe[] =
        DNXC_VSC256_FABRIC_CELL_DESTINATION_ROUTED_CELL_1_PIPE_FORMAT_TABLE;
    soc_dnxc_fabric_cell_parse_table_t vsc256_parse_table_2_pipe[] =
        DNXC_VSC256_FABRIC_CELL_DESTINATION_ROUTED_CELL_2_PIPE_FORMAT_TABLE;
    soc_dnxc_fabric_cell_parse_table_t vsc256_parse_table_3_pipe[] =
        DNXC_VSC256_FABRIC_CELL_DESTINATION_ROUTED_CELL_3_PIPE_FORMAT_TABLE;
    soc_dnxc_fabric_cell_parse_table_t vsc512_parse_table[] =
        DNXC_VSC512_FABRIC_CELL_DESTINATION_ROUTED_CELL_COMMAN_FORMAT_TABLE;
    soc_dnxc_fabric_cell_parse_table_t *table_pointer;
    int nof_pipes = 0, max_vsc_format_size = 0;
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        nof_pipes = dnxf_data_fabric.pipes.nof_pipes_get(unit);
        max_vsc_format_size = dnxf_data_fabric.cell.max_vsc_format_size_get(unit);
    }
#endif
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        nof_pipes = dnx_data_fabric.pipes.nof_pipes_get(unit);
        max_vsc_format_size = dnx_data_fabric.cell.max_vsc_format_size_get(unit);
    }
#endif

    switch (nof_pipes)
    {
        case 1:
            table_pointer =
                max_vsc_format_size ==
                DNXC_VSC256_DESTINATION_ROUTED_CELL_PAYLOAD_SIZE_BYTES ? vsc256_parse_table_1_pipe : vsc512_parse_table;
            break;
        case 2:
            table_pointer =
                max_vsc_format_size ==
                DNXC_VSC256_DESTINATION_ROUTED_CELL_PAYLOAD_SIZE_BYTES ? vsc256_parse_table_2_pipe :
                vsc512_parse_table;;
            break;
        case 3:
            table_pointer =
                max_vsc_format_size ==
                DNXC_VSC256_DESTINATION_ROUTED_CELL_PAYLOAD_SIZE_BYTES ? vsc256_parse_table_3_pipe : NULL;;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid nof pipes received %d", nof_pipes);
    }

    if (table_pointer == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No suitable parsing table pointer was found!");
    }

    if (buf_size < DNXC_DESTINATION_ROUTED_CELL_HEADER_SIZE_UINT32)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "destination routed cell header minimum buffer size is %d",
                     DNXC_DESTINATION_ROUTED_CELL_HEADER_SIZE_UINT32);
    }

    cell->header.last_cell_of_paket = 1;
    cell->header.fragment_number = 1;

    cell->header.pcp_type = 1;

    while (table_pointer[line].dest_id != soc_dnxc_fabric_cell_field_invalid)
    {
        switch (table_pointer[line].dest_id)
        {

            case soc_dnxc_fabric_cell_field_cell_type:
                table_pointer[line].dest = &(cell->header.cell_type);
                break;
            case soc_dnxc_fabric_cell_field_destination:
                table_pointer[line].dest = &(cell->header.dest_modid);
                break;
            case soc_dnxc_fabric_cell_field_src_device:
                table_pointer[line].dest = &(cell->header.source_modid);
                break;
            case soc_dnxc_fabric_cell_field_fragment_number:
                table_pointer[line].dest = &(cell->header.last_cell_of_paket);
                break;
            case soc_dnxc_fabric_cell_field_last_cell_of_packet:
                table_pointer[line].dest = &(cell->header.fragment_number);
                break;
            case soc_dnxc_fabric_cell_field_pcp_type:
                table_pointer[line].dest = &(cell->header.pcp_type);
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "UNAVAIL line id for creating data cell");
                break;
        }
        line++;
    }

    line = 0;
    while (table_pointer[line].dest_id != soc_dnxc_fabric_cell_field_invalid)
    {
        SHR_BITCOPY_RANGE(buf,
                          DNXC_DESTINATION_ROUTED_CELL_HEADER_SIZE_BITS - (table_pointer[line].src_start_bit +
                                                                           table_pointer[line].length),
                          table_pointer[line].dest, 0, table_pointer[line].length);
        line++;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_vsc256_sr_cell_build_header(
    int unit,
    const dnxc_vsc256_sr_cell_t * cell,
    int buf_size_bytes,
    uint32 *buf)
{
    soc_dnxc_device_type_actual_value_t tmp_actual_value;
    uint32 tmp_u32_data_cell_field;
    SHR_FUNC_INIT_VARS(unit);

    if (buf_size_bytes < DNXC_VSC256_SR_DATA_CELL_HEADER_SIZE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "SR header minimum buffer size is %d", DNXC_VSC256_SR_DATA_CELL_HEADER_SIZE);
    }

    tmp_u32_data_cell_field = cell->header.cell_type;
    SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_CELL_TYPE_START, &(tmp_u32_data_cell_field), 0,
                      DNXC_VSC256_SR_DATA_CELL_CELL_TYPE_LENGTH);

    tmp_u32_data_cell_field = cell->header.source_device;
    SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_SOURCE_ID_START, &(tmp_u32_data_cell_field), 0,
                      DNXC_VSC256_SR_DATA_CELL_SOURCE_ID_LENGTH);

    SHR_IF_ERR_EXIT(soc_dnxc_actual_entity_value(unit, cell->header.source_level, &tmp_actual_value));
    tmp_u32_data_cell_field = (uint32) tmp_actual_value;
    SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_SRC_LEVEL_START, &(tmp_u32_data_cell_field), 0,
                      DNXC_VSC256_SR_DATA_CELL_SRC_LEVEL_LENGTH);

    SHR_IF_ERR_EXIT(soc_dnxc_actual_entity_value(unit, cell->header.destination_level, &tmp_actual_value));
    tmp_u32_data_cell_field = (uint32) tmp_actual_value;
    SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_DEST_LEVEL_START, &(tmp_u32_data_cell_field), 0,
                      DNXC_VSC256_SR_DATA_CELL_DEST_LEVEL_LENGTH);

    tmp_u32_data_cell_field = cell->header.fip_link;
    SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FIP_SWITCH_START, &(tmp_u32_data_cell_field), 0,
                      DNXC_VSC256_SR_DATA_CELL_FIP_SWITCH_LENGTH);
    SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FIP_SWITCH_POSITION_1, &(tmp_u32_data_cell_field),
                      DNXC_VSC256_SR_DATA_CELL_FIP_SWITCH_START_1, DNXC_VSC256_SR_DATA_CELL_FIP_SWITCH_LENGTH_1);

    tmp_u32_data_cell_field = cell->header.fe1_link;
    SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_START, &(tmp_u32_data_cell_field), 0,
                      DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_LENGTH);
    SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_POSITION_1, &(tmp_u32_data_cell_field),
                      DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_START_1, DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_LENGTH_1);
    SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_POSITION_2, &(tmp_u32_data_cell_field),
                      DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_START_2, DNXC_VSC256_SR_DATA_CELL_FE1_SWITCH_LENGTH_2);

    tmp_u32_data_cell_field = cell->header.fe2_link;
    SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_START, &(tmp_u32_data_cell_field), 0,
                      DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_LENGTH);
    SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_POSITION_1, &(tmp_u32_data_cell_field),
                      DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_START_1, DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_LENGTH_1);
    SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_POSITION_2, &(tmp_u32_data_cell_field),
                      DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_START_2, DNXC_VSC256_SR_DATA_CELL_FE2_SWITCH_LENGTH_2);

    tmp_u32_data_cell_field = cell->header.fe3_link;
    SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_START, &(tmp_u32_data_cell_field), 0,
                      DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_LENGTH);
    SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_POSITION_1, &(tmp_u32_data_cell_field),
                      DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_START_1, DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_LENGTH_1);
    SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_POSITION_2, &(tmp_u32_data_cell_field),
                      DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_START_2, DNXC_VSC256_SR_DATA_CELL_FE3_SWITCH_LENGTH_2);

    tmp_u32_data_cell_field = cell->header.is_inband;
    SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_INBAND_CELL_POSITION, &(tmp_u32_data_cell_field), 0,
                      DNXC_VSC256_SR_DATA_CELL_INBAND_CELL_LENGTH);

    if (cell->header.pipe_id != -1)
    {
        tmp_u32_data_cell_field = 0x1;
        SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_PIPE_ID_ENABLE_POSITION, &(tmp_u32_data_cell_field), 0,
                          DNXC_VSC256_SR_DATA_CELL_PIPE_ID_ENABLE_LENGTH);
        tmp_u32_data_cell_field = cell->header.pipe_id;
        SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_PIPE_ID_POSITION, &(tmp_u32_data_cell_field), 0,
                          DNXC_VSC256_SR_DATA_CELL_PIPE_ID_LENGTH);
    }
    else
    {
        tmp_u32_data_cell_field = 0x0;
        SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_PIPE_ID_ENABLE_POSITION, &(tmp_u32_data_cell_field), 0,
                          DNXC_VSC256_SR_DATA_CELL_PIPE_ID_ENABLE_LENGTH);
        tmp_u32_data_cell_field = 0x0;
        SHR_BITCOPY_RANGE(buf, DNXC_VSC256_SR_DATA_CELL_PIPE_ID_POSITION, &(tmp_u32_data_cell_field), 0,
                          DNXC_VSC256_SR_DATA_CELL_PIPE_ID_LENGTH);
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
