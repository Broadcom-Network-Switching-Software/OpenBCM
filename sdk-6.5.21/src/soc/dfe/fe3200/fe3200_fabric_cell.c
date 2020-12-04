/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE3200 FABRIC CELL
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC 
#include <shared/bsl.h>

#include <soc/mcm/memregs.h>
#include <soc/error.h>
#include <soc/defs.h>

#ifdef BCM_88950

#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_cells_buffer.h>
#include <soc/dcmn/dcmn_mem.h>
#include <soc/dcmn/vsc256_fabric_cell.h>
#include <soc/dcmn/dcmn_fabric_source_routed_cell.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_config_defs.h>
#include <soc/dfe/cmn/dfe_fabric_source_routed_cell.h>
#include <soc/dfe/fe3200/fe3200_fabric_cell.h>

STATIC soc_error_t 
soc_fe3200_sr_cell_format(int unit, const vsc256_sr_cell_t* cell, int buf_size_bytes, uint32* buf) 
{
    soc_dcmn_device_type_actual_value_t tmp_actual_value;
    soc_dfe_fabric_link_cell_size_t vsc_format;
    soc_error_t rc;
    uint32 tmp_u32_data_cell_field;
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(buf, 0, buf_size_bytes);

    if(buf_size_bytes < FE3200_DATA_CELL_BYTE_SIZE) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("SR cell minimum buffer size is %d"),FE3200_DATA_CELL_BYTE_SIZE));
    }

    rc = soc_dfe_sr_cell_format_type_get(unit, cell, &vsc_format);
    SOCDNX_IF_ERR_EXIT(rc);

    if((vsc_format == soc_dfe_fabric_link_cell_size_VSC128)) {
         
        tmp_u32_data_cell_field = cell->header.cell_type;
        SHR_BITCOPY_RANGE(buf, FE3200_VSC128_DATA_CELL_CELL_TYPE_START, &(tmp_u32_data_cell_field), 0, FE3200_VSC128_DATA_CELL_CELL_TYPE_LENGTH);  
    
        
        tmp_u32_data_cell_field = cell->header.source_device;
        SHR_BITCOPY_RANGE(buf, FE3200_VSC128_DATA_CELL_SOURCE_ID_START, &(tmp_u32_data_cell_field), 0, FE3200_VSC128_DATA_CELL_SOURCE_ID_LENGTH);  
    
    
        
        SOCDNX_IF_ERR_EXIT(soc_dcmn_actual_entity_value(unit,cell->header.source_level, &tmp_actual_value));
        tmp_u32_data_cell_field = (uint32)tmp_actual_value;
        SHR_BITCOPY_RANGE(buf, FE3200_VSC128_DATA_CELL_SRC_LEVEL_START, &(tmp_u32_data_cell_field), 0, FE3200_VSC128_DATA_CELL_SRC_LEVEL_LENGTH);
   
        
        SOCDNX_IF_ERR_EXIT(soc_dcmn_actual_entity_value(unit, cell->header.destination_level, &tmp_actual_value));
        tmp_u32_data_cell_field = (uint32)tmp_actual_value;
        SHR_BITCOPY_RANGE(buf, FE3200_VSC128_DATA_CELL_DEST_LEVEL_START, &(tmp_u32_data_cell_field), 0, FE3200_VSC128_DATA_CELL_DEST_LEVEL_LENGTH);
    
        
        tmp_u32_data_cell_field = cell->header.fip_link;
        SHR_BITCOPY_RANGE(buf, FE3200_VSC128_DATA_CELL_FIP_SWITCH_START, &(tmp_u32_data_cell_field), 0,  FE3200_VSC128_DATA_CELL_FIP_SWITCH_LENGTH);
        SHR_BITCOPY_RANGE(buf, FE3200_VSC128_SR_DATA_CELL_FIP_SWITCH_POSITION, &(tmp_u32_data_cell_field), FE3200_VSC128_DATA_CELL_FIP_SWITCH_LENGTH,  FE3200_VSC128_SR_DATA_CELL_FIP_SWITCH_LENGTH);
    
        
        tmp_u32_data_cell_field = cell->header.fe1_link;
        SHR_BITCOPY_RANGE(buf, FE3200_VSC128_DATA_CELL_FE1_SWITCH_START, &(tmp_u32_data_cell_field), 0,  FE3200_VSC128_DATA_CELL_FE1_SWITCH_LENGTH);
        SHR_BITCOPY_RANGE(buf, FE3200_VSC128_SR_DATA_CELL_FE1_SWITCH_POSITION, &(tmp_u32_data_cell_field), FE3200_VSC128_DATA_CELL_FE1_SWITCH_LENGTH,  FE3200_VSC128_SR_DATA_CELL_FE1_SWITCH_LENGTH);
    
        
        tmp_u32_data_cell_field = cell->header.fe2_link;
        SHR_BITCOPY_RANGE(buf, FE3200_VSC128_DATA_CELL_FE2_SWITCH_START, &(tmp_u32_data_cell_field), 0,  FE3200_VSC128_DATA_CELL_FE2_SWITCH_LENGTH);
        SHR_BITCOPY_RANGE(buf, FE3200_VSC128_SR_DATA_CELL_FE2_SWITCH_POSITION, &(tmp_u32_data_cell_field), FE3200_VSC128_DATA_CELL_FE2_SWITCH_LENGTH,  FE3200_VSC128_SR_DATA_CELL_FE2_SWITCH_LENGTH);
    
        
        tmp_u32_data_cell_field = cell->header.fe3_link;
        SHR_BITCOPY_RANGE(buf, FE3200_VSC128_DATA_CELL_FE3_SWITCH_START, &(tmp_u32_data_cell_field), 0,  FE3200_VSC128_DATA_CELL_FE3_SWITCH_LENGTH);
        SHR_BITCOPY_RANGE(buf, FE3200_VSC128_SR_DATA_CELL_FE3_SWITCH_POSITION, &(tmp_u32_data_cell_field), FE3200_VSC128_DATA_CELL_FE3_SWITCH_LENGTH,  FE3200_VSC128_SR_DATA_CELL_FE3_SWITCH_LENGTH);
    
        
        tmp_u32_data_cell_field = cell->header.is_inband;
        SHR_BITCOPY_RANGE(buf, FE3200_VSC128_SR_DATA_CELL_INBAND_CELL_POSITION, &(tmp_u32_data_cell_field), 0,  FE3200_VSC128_SR_DATA_CELL_INBAND_CELL_LENGTH);
    
                
        SHR_BITCOPY_RANGE(buf, FE3200_VSC128_SR_DATA_CELL_PAYLOAD_START, cell->payload.data, 0,  FE3200_VSC128_SR_DATA_CELL_PAYLOAD_LENGTH);
        
    } 
    else
    {   
        if(cell->header.is_inband && VSC256_CELL_FORMAT_FE600 == cell->payload.inband.cell_format) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("VSC256 can't be sent in FE600 format")));
        }
        soc_vsc256_build_header(unit, cell, buf_size_bytes - (FE3200_SR_DATA_CELL_PAYLOAD_LENGTH/8), (uint32*)(((uint8*)buf) + (FE3200_SR_DATA_CELL_PAYLOAD_LENGTH/8) + SOC_DFE_DEFS_GET(unit, source_routed_cells_header_offset)/8));
        
        SHR_BITCOPY_RANGE(buf, FE3200_SR_DATA_CELL_PAYLOAD_START, cell->payload.data, 0,  FE3200_SR_DATA_CELL_PAYLOAD_LENGTH);
    }  
    
exit:
    SOCDNX_FUNC_RETURN;  
}
soc_error_t 
soc_fe3200_sr_cell_send(int unit, const vsc256_sr_cell_t* cell)
{
    uint8 buf[FE3200_DATA_CELL_BYTE_SIZE]; 
    soc_error_t rc;
    soc_port_t first_link, inner_link;
    soc_reg_above_64_val_t reg_val;
    uint32 reg_val32, blk_id;
    SOCDNX_INIT_FUNC_DEFS;

    
    rc = soc_fe3200_sr_cell_format(unit, cell, FE3200_DATA_CELL_BYTE_SIZE, (uint32*)buf); 
    SOCDNX_IF_ERR_EXIT(rc);

    
    switch(cell->header.source_level) {
        case     bcmFabricDeviceTypeFIP:
            first_link = cell->header.fip_link;
            break;
        case     bcmFabricDeviceTypeFE1:
            first_link = cell->header.fe1_link;
            break;
        case     bcmFabricDeviceTypeFE2:
            first_link = cell->header.fe2_link;
            break;
        case     bcmFabricDeviceTypeFE3:
            first_link = cell->header.fe3_link;
            break;
        case     bcmFabricDeviceTypeFAP:
        case     bcmFabricDeviceTypeFE13:
        case     bcmFabricDeviceTypeFOP:
        case     bcmFabricDeviceTypeUnknown:
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Can't retrive first link from cell")));
    }

    blk_id = INT_DEVIDE(first_link,SOC_DFE_DEFS_GET(unit, nof_links_in_dcq));
    inner_link = first_link%SOC_DFE_DEFS_GET(unit, nof_links_in_dcq);

    SOC_REG_ABOVE_64_CLEAR(reg_val);
    SHR_BITCOPY_RANGE(reg_val, 0, (uint32*)buf, 0*128, 128);
    SOCDNX_IF_ERR_EXIT(WRITE_DCL_CPU_DATA_CELL_0r(unit, blk_id, reg_val));

    SOC_REG_ABOVE_64_CLEAR(reg_val);
    SHR_BITCOPY_RANGE(reg_val, 0, (uint32*)buf, 1*128, 128);
    SOCDNX_IF_ERR_EXIT(WRITE_DCL_CPU_DATA_CELL_1r(unit, blk_id, reg_val));

    SOC_REG_ABOVE_64_CLEAR(reg_val);
    SHR_BITCOPY_RANGE(reg_val, 0, (uint32*)buf, 2*128, 128);
    SOCDNX_IF_ERR_EXIT(WRITE_DCL_CPU_DATA_CELL_2r(unit, blk_id, reg_val));

    SOC_REG_ABOVE_64_CLEAR(reg_val);
    SHR_BITCOPY_RANGE(reg_val, 0, (uint32*)buf, 3*128, 128);
    SOCDNX_IF_ERR_EXIT(WRITE_DCL_CPU_DATA_CELL_3r(unit, blk_id, reg_val));

    SOC_REG_ABOVE_64_CLEAR(reg_val);
    SHR_BITCOPY_RANGE(reg_val, 0, (uint32*)buf, 4*128, 120);
    SOCDNX_IF_ERR_EXIT(WRITE_DCL_CPU_DATA_CELL_4r(unit, blk_id, reg_val));

    reg_val32 = 0;
    soc_reg_field_set(unit, DCL_TRANSMIT_CELL_OUTPUT_LINK_NUMBERr, &reg_val32, CPU_LINK_NUMf, inner_link);


    
    soc_reg_field_set(unit, DCL_TRANSMIT_CELL_OUTPUT_LINK_NUMBERr, &reg_val32, CPU_CELL_SIZEf, 127);
    
    SOCDNX_IF_ERR_EXIT(WRITE_DCL_TRANSMIT_CELL_OUTPUT_LINK_NUMBERr(unit, blk_id, reg_val32));
    SOCDNX_IF_ERR_EXIT(WRITE_DCL_TRANSMIT_DATA_CELL_TRIGGERr(unit, blk_id, 0x1));

exit:
    SOCDNX_FUNC_RETURN;  

}

soc_error_t 
soc_fe3200_cell_filter_clear(int unit)
{
    int i,nof_instances_dch;
	soc_reg_above_64_val_t reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    nof_instances_dch = SOC_DFE_DEFS_GET(unit, nof_instances_dch);

	SOC_REG_ABOVE_64_CLEAR(reg_val);
    for(i=0 ; i<nof_instances_dch; i++) {  
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_MATCH_FILTr(unit, i, reg_val));
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_MATCH_FILT_MASKr(unit, i, reg_val));
    }

exit:
     SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_cell_filter_set(int unit, uint32 flags, uint32 array_size, soc_dcmn_filter_type_t* filter_type_arr, uint32* filter_type_val) 
{
	int i, rc;
	uint32 table_size;
	uint64 reg_val64;
	soc_reg_above_64_val_t filter_reg, mask_reg;
	soc_dfe_cell_filter_mode_t filter_mode = soc_dfe_cell_filter_copy_mode;

	SOCDNX_INIT_FUNC_DEFS;

	if((flags & SOC_CELL_FILTER_FLAG_DONT_FORWARD) && (flags & SOC_CELL_FILTER_FLAG_DONT_TRAP)) {
        filter_mode = soc_dfe_cell_filter_filter_mode;
    } else if (flags & SOC_CELL_FILTER_FLAG_DONT_FORWARD) {
        filter_mode = soc_dfe_cell_filter_capture_mode;
    } else if (flags & SOC_CELL_FILTER_FLAG_DONT_TRAP) {
        filter_mode = soc_dfe_cell_filter_normal_mode;
    }

    for(i=0 ; i<SOC_DFE_DEFS_GET(unit, nof_instances_dch) ; i++) {
        SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_ENABLERS_REGISTER_1r(unit, i, &reg_val64));
        soc_reg64_field32_set(unit, DCH_DCH_ENABLERS_REGISTER_1r, &reg_val64, DCH_MTCH_ACTf, filter_mode);
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_DCH_ENABLERS_REGISTER_1r(unit, i, reg_val64));
    }

	SOC_REG_ABOVE_64_CLEAR(filter_reg);
	SOC_REG_ABOVE_64_ALLONES(mask_reg);

	for (i = 0 ; i < array_size ; i++)
	{
		switch (filter_type_arr[i])
		{
			case soc_dcmn_filter_type_source_id:
				if((filter_type_val[i] & (~0x7FF)) != 0) {
					SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("source id %d out of range"), filter_type_val[i]));
				}
				
				SOC_REG_ABOVE_64_RANGE_COPY(filter_reg, SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_SOURCE_ID_START, &(filter_type_val[i]), 0, SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_SOURCE_ID_LENGTH);
				SHR_BITCLR_RANGE(mask_reg, SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_SOURCE_ID_START, SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_SOURCE_ID_LENGTH);
				break;

			case soc_dcmn_filter_type_multicast_id:
            case soc_dcmn_filter_type_dest_id:
				rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_multicast_table_size_get,(unit, &table_size));
				SOCDNX_IF_ERR_EXIT(rc);
				if(filter_type_val[i] >= table_size) {
					SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("dest id out of range")));
				}
				SOC_REG_ABOVE_64_RANGE_COPY(filter_reg, SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_DEST_ID_START, &(filter_type_val[i]), 0, SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_DEST_ID_LENGTH);
				SHR_BITCLR_RANGE(mask_reg, SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_DEST_ID_START, SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_DEST_ID_LENGTH);
				break;

			case soc_dcmn_filter_type_priority:
				if((filter_type_val[i] & (~0x3)) != 0) {
					SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("the priority type is out of range")));
				}
				SOC_REG_ABOVE_64_RANGE_COPY(filter_reg, SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_PRIORITY_TYPE_START, &(filter_type_val[i]), 0, SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_PRIORITY_TYPE_LENGTH);
				SHR_BITCLR_RANGE(mask_reg, SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_PRIORITY_TYPE_START, SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_PRIORITY_TYPE_LENGTH);
				break;
           case soc_dcmn_filter_type_traffic_cast:
				if((filter_type_val[i] & (~0x3)) != 0) {
					SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("the traffic cast is out of range")));
				}
				SOC_REG_ABOVE_64_RANGE_COPY(filter_reg, SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_TRAFFIC_CAST_START, &(filter_type_val[i]), 0, SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_TRAFFIC_CAST_LENGTH);
				SHR_BITCLR_RANGE(mask_reg, SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_TRAFFIC_CAST_START, SOC_FE3200_FABRIC_CELL_MATCH_FILTER_REG_TRAFFIC_CAST_LENGTH);
				break;
			default:
				SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unsupported Filter Type")));

		}

	}

	for (i = 0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_dch); i++)
	{
		SOCDNX_IF_ERR_EXIT(WRITE_DCH_MATCH_FILTr(unit, i, filter_reg));
		SOCDNX_IF_ERR_EXIT(WRITE_DCH_MATCH_FILT_MASKr(unit, i, mask_reg));
	}

exit:
     SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_fe3200_cell_filter_count_get(int unit, int *count)
{
    int i;
    int temp_count;
    uint32 reg32_val;
    SOCDNX_INIT_FUNC_DEFS;
    
    *count = 0;
    for(i=0 ; i<SOC_DFE_DEFS_GET(unit, nof_instances_dch) ; i++) {  
        SOCDNX_IF_ERR_EXIT(READ_DCH_PROGRAMMABLE_CELLS_COUNTER_1r(unit, i, &reg32_val));

        if (*count == -1) {
            continue;
        }

        temp_count = soc_reg_field_get(unit, DCH_PROGRAMMABLE_CELLS_COUNTER_1r, reg32_val, PRG_CELL_CNT_Af);
        *count += temp_count;
        temp_count = soc_reg_field_get(unit, DCH_PROGRAMMABLE_CELLS_COUNTER_1r, reg32_val, PRG_CELL_CNT_Bf);
        *count += temp_count;

        
        temp_count = soc_reg_field_get(unit, DCH_PROGRAMMABLE_CELLS_COUNTER_1r, reg32_val, PRG_CELL_CNT_AOf);
        if (temp_count) {
            *count = -1;
            continue;
        }
        temp_count = soc_reg_field_get(unit, DCH_PROGRAMMABLE_CELLS_COUNTER_1r, reg32_val, PRG_CELL_CNT_BOf);
        if (temp_count) {
            *count = -1;
            continue;
        }

    }

    
    for(i=0 ; i<SOC_DFE_DEFS_GET(unit, nof_instances_dch) ; i++) {  
        SOCDNX_IF_ERR_EXIT(READ_DCH_PROGRAMMABLE_CELLS_COUNTER_2r(unit, i, &reg32_val));

        if (*count == -1) {
            continue;
        }

        temp_count = soc_reg_field_get(unit, DCH_PROGRAMMABLE_CELLS_COUNTER_2r, reg32_val, PRG_CELL_CNT_Cf);
        *count += temp_count;
        temp_count = soc_reg_field_get(unit, DCH_PROGRAMMABLE_CELLS_COUNTER_2r, reg32_val, PRG_CELL_CNT_Df);
        *count += temp_count;

        
        temp_count = soc_reg_field_get(unit, DCH_PROGRAMMABLE_CELLS_COUNTER_2r, reg32_val, PRG_CELL_CNT_COf);
        if(temp_count) {
            *count = -1;
            continue;
        }
        temp_count = soc_reg_field_get(unit, DCH_PROGRAMMABLE_CELLS_COUNTER_2r, reg32_val, PRG_CELL_CNT_DOf);
        if(temp_count) {
            *count = -1;
            continue;
        }
    }
    
exit:
     SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_fe3200_control_cell_filter_set(int unit, uint32 flags, soc_dcmn_control_cell_types_t cell_type, uint32 array_size, soc_dcmn_control_cell_filter_type_t* filter_type_arr, uint32* filter_type_val) 
{
	int i;
	uint32 reg_val32;
	soc_reg_above_64_val_t reg_val, mask_val;
	soc_dfe_cell_filter_mode_t filter_mode = soc_dfe_control_cell_filter_copy_mode;

    SOCDNX_INIT_FUNC_DEFS;
    
	if((flags & SOC_CELL_FILTER_FLAG_DONT_FORWARD) && (flags & SOC_CELL_FILTER_FLAG_DONT_TRAP)) {
		filter_mode = soc_dfe_control_cell_filter_filter_mode;
	} else if (flags & SOC_CELL_FILTER_FLAG_DONT_FORWARD) {
		filter_mode = soc_dfe_control_cell_filter_capture_mode;
	} else if (flags & SOC_CELL_FILTER_FLAG_DONT_TRAP) {
		filter_mode = soc_dfe_control_cell_filter_normal_mode;
	}

	for (i = 0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_ccs) ; i++)
	{
		SOCDNX_IF_ERR_EXIT(READ_CCS_DEBUG_CONFIGURATIONSr(unit, i, &reg_val32));
		soc_reg_field_set(unit, CCS_DEBUG_CONFIGURATIONSr, &reg_val32, CAPTURE_OPERATIONf, filter_mode);
        
        
        soc_reg_field_set(unit, CCS_DEBUG_CONFIGURATIONSr, &reg_val32, FIELD_7_7f, 1);
        
        soc_reg_field_set(unit, CCS_DEBUG_CONFIGURATIONSr, &reg_val32, FIELD_5_5f, (cell_type == soc_dcmn_fe2_filtered_cell) ? 1 : 0);
        SOCDNX_IF_ERR_EXIT(WRITE_CCS_DEBUG_CONFIGURATIONSr(unit, i, reg_val32));
	}

	SOC_REG_ABOVE_64_CLEAR(reg_val);
    
    SOC_REG_ABOVE_64_ALLONES(mask_val);

	if (cell_type == soc_dcmn_flow_status_cell || cell_type == soc_dcmn_credit_cell)
	{
		for (i = 0 ; i < array_size ; i++)
		{
			switch(filter_type_arr[i]) {
                case soc_dcmn_filter_control_cell_type_source_device:
                    if((filter_type_val[i] & (~0x7FF)) != 0) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("source device id is out of range")));
                    }
                    SHR_BITCOPY_RANGE(reg_val, FE3200_VSC256_CONTROL_CELL_SOURCE_ID_START, &(filter_type_val[i]), 0, FE3200_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH);
                    
                    SHR_BITCLR_RANGE(mask_val, FE3200_VSC256_CONTROL_CELL_SOURCE_ID_START, FE3200_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH);
                    break;
                case soc_dcmn_filter_control_cell_type_dest_device:
                    if((filter_type_val[i] & (~0x7FF)) != 0) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("dest device id is out of range")));
                    }
                    SHR_BITCOPY_RANGE(reg_val, FE3200_VSC256_CONTROL_CELL_DEST_DEVICE_START, &(filter_type_val[i]), 0, FE3200_VSC256_CONTROL_CELL_DEST_DEVICE_LENGTH);
                    
                    SHR_BITCLR_RANGE(mask_val, FE3200_VSC256_CONTROL_CELL_DEST_DEVICE_START, FE3200_VSC256_CONTROL_CELL_DEST_DEVICE_LENGTH);
                    break;
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unsupported filter type")));
            }
		}
	} else if (cell_type == soc_dcmn_reachability_cell) 
	{
	   SHR_BITCOPY_RANGE(reg_val, FE3200_VSC256_CONTROL_CELL_REACHABILITY_SOURCE_ID_START, &(filter_type_val[0]), 0, FE3200_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH);
       
       SHR_BITCLR_RANGE(mask_val, FE3200_VSC256_CONTROL_CELL_REACHABILITY_SOURCE_ID_START, FE3200_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH);
	} else { 
	}

     
    SHR_BITCOPY_RANGE(reg_val, FE3200_VSC256_CONTROL_CELL_CONTROL_TYPE_START, &cell_type, 0, FE3200_VSC256_CONTROL_CELL_CONTROL_TYPE_LENGTH);
    
    SHR_BITCLR_RANGE(mask_val, FE3200_VSC256_CONTROL_CELL_CONTROL_TYPE_START, FE3200_VSC256_CONTROL_CELL_CONTROL_TYPE_LENGTH);

	
   for(i=0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_ccs) ; i++) {
       SOCDNX_IF_ERR_EXIT(WRITE_CCS_CAPTURE_FILTER_CELL_0r(unit, i, reg_val));
       SOCDNX_IF_ERR_EXIT(WRITE_CCS_CAPTURE_FILTER_MASK_0r(unit, i, mask_val));
   }

exit:
     SOCDNX_FUNC_RETURN;

}

soc_error_t 
soc_fe3200_control_cell_filter_clear(int unit)
{
    soc_reg_above_64_val_t reg_val;
    uint32 reg_val32;
    int i;
    SOCDNX_INIT_FUNC_DEFS;
	
    SOC_REG_ABOVE_64_CLEAR(reg_val);

	for (i = 0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_ccs); i++)
	{
		SOCDNX_IF_ERR_EXIT(WRITE_CCS_CAPTURE_FILTER_CELL_0r(unit, i, reg_val));
		SOCDNX_IF_ERR_EXIT(WRITE_CCS_CAPTURE_FILTER_MASK_0r(unit, i, reg_val));

		SOCDNX_IF_ERR_EXIT(READ_CCS_DEBUG_CONFIGURATIONSr(unit, i, &reg_val32));
		soc_reg_field_set(unit, CCS_DEBUG_CONFIGURATIONSr, &reg_val32, FIELD_6_6f, 0); 
		SOCDNX_IF_ERR_EXIT(WRITE_CCS_DEBUG_CONFIGURATIONSr(unit, i, reg_val32));
	}

exit:
     SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_fe3200_control_cell_filter_receive(int unit,  soc_dcmn_captured_control_cell_t* data_out)
{
	soc_reg_above_64_val_t  reg_val;
    int i;
	uint32 valid_cell_reg;
    SOCDNX_INIT_FUNC_DEFS;
	
	sal_memset(data_out, 0 ,sizeof(soc_dcmn_captured_control_cell_t));

	for (i = 0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_ccs); i++)
	{
		SOCDNX_IF_ERR_EXIT(READ_CCS_CAPTURED_CELL_VALIDr(unit, i, &valid_cell_reg));
		if (valid_cell_reg)
		{
			data_out->valid = 1;
			break;
		}
	}

	if (data_out->valid)
	{
		SOCDNX_IF_ERR_EXIT(READ_CCS_CAPTURED_CELLr(unit, i, reg_val));

        SHR_BITCOPY_RANGE((uint32*)&(data_out->control_type), 0, reg_val, FE3200_VSC256_CONTROL_CELL_CONTROL_TYPE_START, FE3200_VSC256_CONTROL_CELL_CONTROL_TYPE_LENGTH);

        switch(data_out->control_type) {
                case soc_dcmn_flow_status_cell:
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->dest_port), 0, reg_val, FE3200_VSC256_CONTROL_CELL_FSM_DEST_PORT_START, FE3200_VSC256_CONTROL_CELL_FSM_DEST_PORT_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->src_queue_num), 0, reg_val, FE3200_VSC256_CONTROL_CELL_FSM_SRC_QUEUE_NUM_START, FE3200_VSC256_CONTROL_CELL_FSM_SRC_QUEUE_NUM_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->flow_id), 0, reg_val, FE3200_VSC256_CONTROL_CELL_FSM_FLOW_ID_START, FE3200_VSC256_CONTROL_CELL_FSM_FLOW_ID_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->source_device), 0, reg_val, FE3200_VSC256_CONTROL_CELL_SOURCE_ID_START, FE3200_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->dest_device), 0, reg_val, FE3200_VSC256_CONTROL_CELL_DEST_DEVICE_START, FE3200_VSC256_CONTROL_CELL_DEST_DEVICE_LENGTH);
                    break;
                case soc_dcmn_credit_cell:
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->dest_queue_num),0 , reg_val, FE3200_VSC256_CONTROL_CELL_CREDIT_DEST_Q_NUM_START, FE3200_VSC256_CONTROL_CELL_CREDIT_DEST_Q_NUM_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->sub_flow_id), 0, reg_val, FE3200_VSC256_CONTROL_CELL_CREDIT_SUB_FLOW_ID_START, FE3200_VSC256_CONTROL_CELL_CREDIT_SUB_FLOW_ID_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->flow_id), 0, reg_val, FE3200_VSC256_CONTROL_CELL_CREDIT_FLOW_ID_START, FE3200_VSC256_CONTROL_CELL_CREDIT_FLOW_ID_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->source_device), 0, reg_val, FE3200_VSC256_CONTROL_CELL_SOURCE_ID_START, FE3200_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->dest_device), 0, reg_val, FE3200_VSC256_CONTROL_CELL_DEST_DEVICE_START, FE3200_VSC256_CONTROL_CELL_DEST_DEVICE_LENGTH);
                    break;
                case soc_dcmn_reachability_cell:
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->reachability_bitmap),0 , reg_val, FE3200_VSC256_CONTROL_CELL_REACHABILITY_BITMAP_START, FE3200_VSC256_CONTROL_CELL_REACHABILITY_BITMAP_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->base_index),0 , reg_val, FE3200_VSC256_CONTROL_CELL_REACHABILITY_BASE_ID_START, FE3200_VSC256_CONTROL_CELL_REACHABILITY_BASE_ID_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->source_link_number),0 , reg_val, FE3200_VSC256_CONTROL_CELL_REACHABILITY_SRC_LINK_NUM_START, FE3200_VSC256_CONTROL_CELL_REACHABILITY_SRC_LINK_NUM_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->source_device),0 , reg_val, FE3200_VSC256_CONTROL_CELL_REACHABILITY_SOURCE_ID_START, FE3200_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH);
                    break;
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unsupported filter type")));
        }
	}

exit:
    SOCDNX_FUNC_RETURN; 
}



soc_error_t
soc_fe3200_fabric_cell_get(int unit, soc_dcmn_fabric_cell_entry_t entry)
{
    uint32 reg, blk_idx;
    uint32 nof_cell;
    int found_cell;
    SOCDNX_INIT_FUNC_DEFS;   

    found_cell = 0;
    
    for(blk_idx=0 ; blk_idx<SOC_DFE_DEFS_GET(unit, nof_instances_dcl) ; blk_idx++) 
    {

        SOCDNX_IF_ERR_EXIT(READ_DCL_CPU_CELL_FIFO_STATUSr(unit, blk_idx, &reg));

        nof_cell = soc_reg_field_get(unit, DCL_CPU_CELL_FIFO_STATUSr, reg, CPU_CELL_FIFO_STATUSf);       
   
        sal_memset(entry, 0x00, sizeof(soc_dcmn_fabric_cell_entry_t));
        if(nof_cell) {
            SOCDNX_IF_ERR_EXIT(dcmn_mem_array_wide_access(unit, SOC_MEM_NO_FLAGS, DCL_CPU_Hm, 0, blk_idx, 0, entry, 1));
            found_cell = 1;
            break;
        }
    }

    if (!found_cell)
    {
        SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_EMPTY);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_fe3200_fabric_cell_parse_table_get(int unit, soc_dcmn_fabric_cell_type_t cell_type, uint32 max_nof_lines, soc_dcmn_fabric_cell_parse_table_t *parse_table, uint32 *nof_lines, int is_two_parts)
{
    
    int actual_nof_lines;
    SOCDNX_INIT_FUNC_DEFS;

    
    if (cell_type == soc_dcmn_fabric_cell_type_sr_cell)
    {
        soc_dcmn_fabric_cell_parse_table_t temp_table_src[] = SOC_FE3200_FABRIC_CELL_CAPTURED_SRC_FORMAT;

        if (max_nof_lines < SOC_FE3200_FABRIC_CELL_CAPTURED_SRC_FORMAT_NOF_LINES)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Bigger table is required\n")));
        } 
        
        sal_memcpy(parse_table, temp_table_src, sizeof(soc_dcmn_fabric_cell_parse_table_t) * SOC_FE3200_FABRIC_CELL_CAPTURED_SRC_FORMAT_NOF_LINES);
        *nof_lines = SOC_FE3200_FABRIC_CELL_CAPTURED_SRC_FORMAT_NOF_LINES;
    } else {        
        soc_dcmn_fabric_cell_parse_table_t temp_table_captured[] = SOC_FE3200_FABRIC_CELL_CPU_CAPTURED_FORMAT;
        actual_nof_lines = is_two_parts ? SOC_FE3200_FABRIC_CELL_CPU_CAPTURED_FORMAT_NOF_LINES_DOUBLE_CELL:SOC_FE3200_FABRIC_CELL_CPU_CAPTURED_FORMAT_NOF_LINES_ONE_CELL ; 

        if (max_nof_lines < actual_nof_lines)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Bigger table is required\n")));
        }
        sal_memcpy(parse_table, temp_table_captured, sizeof(soc_dcmn_fabric_cell_parse_table_t) * actual_nof_lines);
        *nof_lines = actual_nof_lines;
    }
   
exit:
    SOCDNX_FUNC_RETURN; 
}

#define _SOC_FE3200_PARSING_CELL_TYPE_START         (1115)
#define _SOC_FE3200_PARSING_CELL_TYPE_LENGTH        (1)
soc_error_t
soc_fe3200_fabric_cell_type_get(int unit, soc_dcmn_fabric_cell_entry_t entry, soc_dcmn_fabric_cell_type_t *cell_type)
{
    SOCDNX_INIT_FUNC_DEFS;

    *cell_type = 0;
    SHR_BITCOPY_RANGE((uint32 *) cell_type, 0, (uint32 *) entry, _SOC_FE3200_PARSING_CELL_TYPE_START, _SOC_FE3200_PARSING_CELL_TYPE_LENGTH);

    SOCDNX_FUNC_RETURN; 
}

soc_error_t
soc_fe3200_fabric_cell_is_cell_in_two_parts(int unit, soc_dcmn_fabric_cell_entry_t entry, int* is_two_parts)
{
    uint32 cell_size = 0 ;
    soc_dcmn_fabric_cell_type_t cell_type;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_cell_type_get(unit, entry, &cell_type));

    if(cell_type == soc_dcmn_fabric_cell_type_sr_cell) 
    {
        *is_two_parts = 0;
        
    }
    else 
    {
        
        SHR_BITCOPY_RANGE(&cell_size, 0, entry, 1117, 8);
        if (cell_size + 1 > 128)
        {
            *is_two_parts = 1;
        }
        else
        {
            *is_two_parts = 0;
        }
        
    }
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_fabric_cell_dcl_cpu_wide_memory_read(int unit, int blk_idx, void* entry)
{
    uint32 reg_val32;
    soc_reg_above_64_val_t low_entry, hi_entry;
    uint32* mem_val = (uint32*) entry;
    int indirect_command_trigger, indirect_command_status;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(low_entry);
    SOC_REG_ABOVE_64_CLEAR(hi_entry);

    SOCDNX_IF_ERR_EXIT(READ_DCL_INDIRECT_COMMAND_ADDRESSr(unit, blk_idx, &reg_val32));
    soc_reg_field_set(unit, DCL_INDIRECT_COMMAND_ADDRESSr, &reg_val32, INDIRECT_COMMAND_ADDRf, 0x02300000);
    soc_reg_field_set(unit, DCL_INDIRECT_COMMAND_ADDRESSr, &reg_val32, INDIRECT_COMMAND_TYPEf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_DCL_INDIRECT_COMMAND_ADDRESSr(unit, blk_idx, reg_val32));

    SOCDNX_IF_ERR_EXIT(READ_DCL_INDIRECT_COMMANDr(unit, blk_idx, &reg_val32));
    soc_reg_field_set(unit, DCL_INDIRECT_COMMANDr, &reg_val32, INDIRECT_COMMAND_TIMEOUTf, 100);
    soc_reg_field_set(unit, DCL_INDIRECT_COMMANDr, &reg_val32, INDIRECT_COMMAND_TRIGGERf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_DCL_INDIRECT_COMMANDr(unit, blk_idx, reg_val32));

    SOCDNX_IF_ERR_EXIT(READ_DCL_INDIRECT_COMMANDr(unit, blk_idx, &reg_val32));
    indirect_command_trigger = soc_reg_field_get(unit, DCL_INDIRECT_COMMANDr, reg_val32, INDIRECT_COMMAND_TRIGGERf);
    indirect_command_status = soc_reg_field_get(unit, DCL_INDIRECT_COMMANDr, reg_val32, INDIRECT_COMMAND_STATUSf);
    if ((indirect_command_trigger != 0) || (indirect_command_status != 0))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Indirect command failed - DCL indirect command register holds false values \n")));
    }

    sal_usleep(20000);

    SOCDNX_IF_ERR_EXIT(READ_DCL_INDIRECT_COMMAND_RD_DATAr(unit, blk_idx, low_entry));
    sal_usleep(20000);
    SOCDNX_IF_ERR_EXIT(READ_DCL_INDIRECT_COMMAND_RD_DATAr(unit, blk_idx, hi_entry));

    SHR_BITCOPY_RANGE(mem_val, 0, low_entry, 0, 640);
    SHR_BITCOPY_RANGE(mem_val, 640, hi_entry, 0, 488);

exit:
    SOCDNX_FUNC_RETURN;

}

#endif 

#undef _ERR_MSG_MODULE_NAME
