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

#include <soc/dnxc/dnxc_memregs.h>
#include <soc/dnxc/dnxc_error.h>
#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnxc/dnxc_fabric_source_routed_cell.h>
#include <shared/bitop.h>

#include <soc/dnxc/vsc256_fabric_cell.h>

shr_error_e
soc_dnxc_vsc256_build_header(
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
