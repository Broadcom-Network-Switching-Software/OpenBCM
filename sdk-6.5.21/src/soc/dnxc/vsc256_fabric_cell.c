/*
* 
*
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* SOC VSC256 FABRIC CELL
*/

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC
#include <shared/bsl.h>
#include <soc/dnxc/error.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bitop.h>

#include <soc/dnxc/dnxc_memregs.h>
#include <soc/dnxc/dnxc_error.h>
#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnxc/dnxc_fabric_source_routed_cell.h>
#include <soc/dnxc/dnxc_fabric_cell.h>
#include <soc/dnxc/vsc256_fabric_cell.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#endif
#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#endif

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

#define DNXC_FABRIC_CELL_DESTINATION_ROUTED_CELL_COMMAN_FORMAT_TABLE  \
    \
    {soc_dnxc_fabric_cell_field_cell_type,              NULL,          0,                0,             2,      "CELL_TYPE"},\
    {soc_dnxc_fabric_cell_field_destination,            NULL,          0,                11,            19,     "DESTINATION"},\
    {soc_dnxc_fabric_cell_field_src_device,             NULL,          0,                30,            11,     "SOURCE_DEVICE"},\
    {soc_dnxc_fabric_cell_field_fragment_number,        NULL,          0,                41,            7,      "FRAGMENT_NUMBER"}

#define DNXC_FABRIC_CELL_DESTINATION_ROUTED_CELL_1_PIPE_FORMAT_TABLE { \
    \
    DNXC_FABRIC_CELL_DESTINATION_ROUTED_CELL_COMMAN_FORMAT_TABLE, \
    {soc_dnxc_fabric_cell_field_last_cell_of_packet,    NULL,          0,                8,             1,      "IS_LAST_CELL_OF_PKT"},\
    {soc_dnxc_fabric_cell_field_pcp_type,               NULL,          0,                72,            1,      "PCP_TYPE"},\
     \
    {soc_dnxc_fabric_cell_field_invalid,                NULL,          0,                -1,            -1,     "INVALID"}\
    }

#define DNXC_FABRIC_CELL_DESTINATION_ROUTED_CELL_2_PIPE_FORMAT_TABLE { \
    \
    DNXC_FABRIC_CELL_DESTINATION_ROUTED_CELL_COMMAN_FORMAT_TABLE, \
    {soc_dnxc_fabric_cell_field_last_cell_of_packet,    NULL,          0,                8,             1,       "IS_LAST_CELL_OF_PKT"},\
    {soc_dnxc_fabric_cell_field_pcp_type,               NULL,          0,                88,            1,       "PCP_TYPE"},\
     \
    {soc_dnxc_fabric_cell_field_invalid,                NULL,          0,                -1,            -1,      "INVALID"}\
    }

#define DNXC_FABRIC_CELL_DESTINATION_ROUTED_CELL_3_PIPE_FORMAT_TABLE { \
    \
    DNXC_FABRIC_CELL_DESTINATION_ROUTED_CELL_COMMAN_FORMAT_TABLE, \
    {soc_dnxc_fabric_cell_field_last_cell_of_packet,    NULL,          0,                88,             1,      "IS_LAST_CELL_OF_PKT"},\
    {soc_dnxc_fabric_cell_field_pcp_type,               NULL,          0,                112,            1,      "PCP_TYPE"},\
     \
    {soc_dnxc_fabric_cell_field_invalid,                NULL,          0,                -1,            -1,      "INVALID"}\
    }

#define DNXC_FABRIC_CELL_FTMH_FORMAT_TABLE { \
        \
        {soc_dnxc_ftmh_base_header_field_visibility,        NULL,          0,                87,            1,       "VISIBILITY"},\
        {soc_dnxc_ftmh_base_header_field_tm_profile,        NULL,          0,                88,            2,       "TM_PROFILE"},\
        {soc_dnxc_ftmh_base_header_field_pp_dsp,            NULL,          0,                55,            8,       "PP_DSP"},\
        {soc_dnxc_ftmh_base_header_field_paket_size,        NULL,          0,                18,            14,      "PKT_SIZE"},\
         \
        {soc_dnxc_ftmh_base_header_field_invalid,           NULL,          0,                -1,            -1,      "INVALID"}\
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

    sal_memcpy(buf, buffer_temp, DNXC_FTMH_BASE_HEADER_SIZE_BYTES);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_vsc256_dest_routed_cell_build_header(
    int unit,
    dnxc_vsc256_dest_routed_cell_t * cell,
    int buf_size,
    uint32 *buf)
{
    int line = 0;
    soc_dnxc_fabric_cell_parse_table_t parse_table_1_pipe[] =
        DNXC_FABRIC_CELL_DESTINATION_ROUTED_CELL_1_PIPE_FORMAT_TABLE;
    soc_dnxc_fabric_cell_parse_table_t parse_table_2_pipe[] =
        DNXC_FABRIC_CELL_DESTINATION_ROUTED_CELL_2_PIPE_FORMAT_TABLE;
    soc_dnxc_fabric_cell_parse_table_t parse_table_3_pipe[] =
        DNXC_FABRIC_CELL_DESTINATION_ROUTED_CELL_3_PIPE_FORMAT_TABLE;
    soc_dnxc_fabric_cell_parse_table_t *table_pointer;
    int nof_pipes = 0;
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        nof_pipes = dnxf_data_fabric.pipes.nof_pipes_get(unit);
    }
#endif
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        nof_pipes = dnx_data_fabric.pipes.nof_pipes_get(unit);
    }
#endif

    switch (nof_pipes)
    {
        case 1:
            table_pointer = parse_table_1_pipe;
            break;
        case 2:
            table_pointer = parse_table_2_pipe;
            break;
        case 3:
            table_pointer = parse_table_3_pipe;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid nof pipes received %d", nof_pipes);
    }

    if (buf_size < DNXC_VSC256_DESTINATION_ROUTED_CELL_HEADER_SIZE_UINT32)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "SR header minimum buffer size is %d",
                     DNXC_VSC256_DESTINATION_ROUTED_CELL_HEADER_SIZE_UINT32);
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
                          DNXC_VSC256_DESTINATION_ROUTED_CELL_HEADER_SIZE_BITS - (table_pointer[line].src_start_bit +
                                                                                  table_pointer[line].length),
                          table_pointer[line].dest, 0, table_pointer[line].length);
        line++;
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
