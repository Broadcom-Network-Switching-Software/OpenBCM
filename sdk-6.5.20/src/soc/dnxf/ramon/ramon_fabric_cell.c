/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC RAMON FABRIC CELL
 */
 
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC 
#include <shared/bsl.h>
#include <shared/utilex/utilex_bitstream.h>

#include <soc/mcm/memregs.h>
#include <soc/error.h>
#include <soc/defs.h>

#include <soc/dnxc/error.h>
#include <soc/dnxc/dnxc_cells_buffer.h>
#include <soc/sand/sand_mem.h>

#include <soc/dnxf/cmn/dnxf_drv.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>

#include <soc/dnxf/ramon/ramon_fabric_cell.h>

#define DNXF_RAMON_FILTER_NOF_COUNTERS      (3)

#define DNXF_RAMON_IS_COUNTER_OVERFLOW(counter, overflow_bit) (utilex_bitstream_test_bit(counter, overflow_bit))

STATIC shr_error_e 
soc_ramon_sr_cell_format(int unit, const dnxc_vsc256_sr_cell_t* cell, int buf_size_bytes, uint32* buf) 
{
    shr_error_e rc;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(buf, 0, buf_size_bytes);

    if(buf_size_bytes < RAMON_DATA_CELL_BYTE_SIZE) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "SR cell minimum buffer size is %d",RAMON_DATA_CELL_BYTE_SIZE);
    }

    rc = soc_dnxc_vsc256_build_header(unit, cell, buf_size_bytes - (RAMON_SR_DATA_CELL_PAYLOAD_LENGTH/8), (uint32*)(((uint8*)buf) + (RAMON_SR_DATA_CELL_PAYLOAD_LENGTH/8) +dnxf_data_fabric.cell.source_routed_cells_header_offset_get(unit)/8));             
    SHR_IF_ERR_EXIT(rc);
    
    SHR_BITCOPY_RANGE(buf, RAMON_SR_DATA_CELL_PAYLOAD_START, cell->payload.data, 0,  RAMON_SR_DATA_CELL_PAYLOAD_LENGTH);
    
exit:
    SHR_FUNC_EXIT;  
}

shr_error_e 
soc_ramon_sr_cell_send(int unit, const dnxc_vsc256_sr_cell_t* cell)
{
    uint8 buf[RAMON_DATA_CELL_BYTE_SIZE]; 
    shr_error_e rc;
    soc_port_t first_link, inner_link;
    soc_reg_above_64_val_t reg_val;
    uint32 reg_val32;
    int blk_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    
    rc = soc_ramon_sr_cell_format(unit, cell, RAMON_DATA_CELL_BYTE_SIZE, (uint32*)buf); 
    SHR_IF_ERR_EXIT(rc);

    
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
            SHR_ERR_EXIT(_SHR_E_PARAM, "Can't retrieve first link from cell");
    }

    
    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, first_link, &blk_id, &inner_link, SOC_BLK_DCML));
    SHR_IF_ERR_EXIT(rc);

    SOC_REG_ABOVE_64_CLEAR(reg_val);
    SHR_BITCOPY_RANGE(reg_val, 0, (uint32*)buf, 0*256, 256);
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, DCML_TX_CPU_CELL_DATA_0r, blk_id, 0, reg_val));

    SOC_REG_ABOVE_64_CLEAR(reg_val);
    SHR_BITCOPY_RANGE(reg_val, 0, (uint32*)buf, 1*256, 256);
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, DCML_TX_CPU_CELL_DATA_1r, blk_id, 0, reg_val));

    SOC_REG_ABOVE_64_CLEAR(reg_val);
    SHR_BITCOPY_RANGE(reg_val, 0, (uint32*)buf, 2*256, 120);
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, DCML_TX_CPU_CELL_HEADERr, blk_id, 0, reg_val));

    
    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, first_link, &blk_id, &inner_link, SOC_BLK_LCM));
    SHR_IF_ERR_EXIT(rc);

    reg_val32 = 0;
    soc_reg_field_set(unit, LCM_TX_CPU_CELL_OUTPUT_LINKr, &reg_val32, TX_CPU_CELL_LINK_NUMf, inner_link);
    
    soc_reg_field_set(unit, LCM_TX_CPU_CELL_OUTPUT_LINKr, &reg_val32, TX_CPU_CELL_SIZEf, 127);
    SHR_IF_ERR_EXIT(soc_reg32_set(unit, LCM_TX_CPU_CELL_OUTPUT_LINKr, blk_id, 0, reg_val32));

    SHR_IF_ERR_EXIT(soc_reg32_set(unit, LCM_TX_CPU_CELL_TRGr, blk_id, 0, 0x1));

exit:
    SHR_FUNC_EXIT;  

}

shr_error_e 
soc_ramon_cell_filter_init(int unit) 
{
    uint32 reg_val32, dch_all_links_mask = 0;
    uint64 reg_val64;
    int dch_block_idx;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_BITSET_RANGE(&dch_all_links_mask, 0, dnxf_data_device.blocks.nof_links_in_dch_get(unit));

    
    for(dch_block_idx=0 ; dch_block_idx<dnxf_data_device.blocks.nof_instances_dch_get(unit) ; dch_block_idx++) {
        
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, DCH_FILTER_MATCH_INPUT_LINKr, dch_block_idx, 0, &reg_val32));
        soc_reg_field_set(unit, DCH_FILTER_MATCH_INPUT_LINKr, &reg_val32, PC_MTCH_LINK_ONf, dch_all_links_mask);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, DCH_FILTER_MATCH_INPUT_LINKr, dch_block_idx, 0, reg_val32));

        
        SHR_IF_ERR_EXIT(READ_DCH_DCH_ENABLERS_REGISTER_1r(unit, dch_block_idx, &reg_val64));
        soc_reg64_field32_set(unit, DCH_DCH_ENABLERS_REGISTER_1r, &reg_val64, AUTO_DOC_NAME_5f, 1);
        SHR_IF_ERR_EXIT(WRITE_DCH_DCH_ENABLERS_REGISTER_1r(unit, dch_block_idx, reg_val64));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e 
soc_ramon_cell_filter_clear(int unit)
{
    int i,nof_instances_dch;
    soc_reg_above_64_val_t reg_val;
    SHR_FUNC_INIT_VARS(unit);

    nof_instances_dch = dnxf_data_device.blocks.nof_instances_dch_get(unit);

    SOC_REG_ABOVE_64_CLEAR(reg_val);
    for(i=0 ; i<nof_instances_dch; i++) {  
        SHR_IF_ERR_EXIT(WRITE_DCH_MATCH_FILTr(unit, i, reg_val));
        SHR_IF_ERR_EXIT(WRITE_DCH_MATCH_FILT_MASKr(unit, i, reg_val));
    }

exit:
     SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_cell_filter_set(int unit, uint32 flags, uint32 array_size, soc_dnxc_filter_type_t* filter_type_arr, uint32* filter_type_val) 
{
    int i, rc;
    uint32 table_size;
    uint64 reg_val64;
    soc_reg_above_64_val_t filter_reg, mask_reg;
    soc_dnxf_cell_filter_mode_t filter_mode = soc_dnxf_cell_filter_copy_mode;

    SHR_FUNC_INIT_VARS(unit);

    if((flags & SOC_CELL_FILTER_FLAG_DONT_FORWARD) && (flags & SOC_CELL_FILTER_FLAG_DONT_TRAP)) {
        filter_mode = soc_dnxf_cell_filter_filter_mode;
    } else if (flags & SOC_CELL_FILTER_FLAG_DONT_FORWARD) {
        filter_mode = soc_dnxf_cell_filter_capture_mode;
    } else if (flags & SOC_CELL_FILTER_FLAG_DONT_TRAP) {
        filter_mode = soc_dnxf_cell_filter_normal_mode;
    }
    
    for(i=0 ; i<dnxf_data_device.blocks.nof_instances_dch_get(unit) ; i++) {
        SHR_IF_ERR_EXIT(READ_DCH_DCH_ENABLERS_REGISTER_1r(unit, i, &reg_val64));
        soc_reg64_field32_set(unit, DCH_DCH_ENABLERS_REGISTER_1r, &reg_val64, DCH_MTCH_ACTf, filter_mode);
        SHR_IF_ERR_EXIT(WRITE_DCH_DCH_ENABLERS_REGISTER_1r(unit, i, reg_val64));
    }

    SOC_REG_ABOVE_64_CLEAR(filter_reg);
    
    SOC_REG_ABOVE_64_ALLONES(mask_reg);

    for (i = 0 ; i < array_size ; i++)
    {
        switch (filter_type_arr[i])
        {
            case soc_dnxc_filter_type_source_id:
                if((filter_type_val[i] & (~0x7FF)) != 0) {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "source id %d out of range", filter_type_val[i]);
                }
                
                SOC_REG_ABOVE_64_RANGE_COPY(filter_reg, SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_SOURCE_ID_START, &(filter_type_val[i]), 0, SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_SOURCE_ID_LENGTH);
                SHR_BITCLR_RANGE(mask_reg, SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_SOURCE_ID_START, SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_SOURCE_ID_LENGTH);
                break;

            case soc_dnxc_filter_type_multicast_id:
            case soc_dnxc_filter_type_dest_id:
                rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_multicast_table_size_get,(unit, &table_size));
                SHR_IF_ERR_EXIT(rc);
                if(filter_type_val[i] >= table_size) {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "dest id out of range");
                }
                SOC_REG_ABOVE_64_RANGE_COPY(filter_reg, SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_DEST_ID_START, &(filter_type_val[i]), 0, SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_DEST_ID_LENGTH);
                SHR_BITCLR_RANGE(mask_reg, SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_DEST_ID_START, SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_DEST_ID_LENGTH);
                break;

            case soc_dnxc_filter_type_priority:
                if((filter_type_val[i] & (~0x3)) != 0) {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "the priority type is out of range");
                }
                SOC_REG_ABOVE_64_RANGE_COPY(filter_reg, SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_PRIORITY_TYPE_START, &(filter_type_val[i]), 0, SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_PRIORITY_TYPE_LENGTH);
                SHR_BITCLR_RANGE(mask_reg, SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_PRIORITY_TYPE_START, SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_PRIORITY_TYPE_LENGTH);
                break;

           case soc_dnxc_filter_type_traffic_cast:
                if((filter_type_val[i] & (~0x3)) != 0) {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "the traffic cast is out of range");
                }
                SOC_REG_ABOVE_64_RANGE_COPY(filter_reg, SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_TRAFFIC_CAST_START, &(filter_type_val[i]), 0, SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_TRAFFIC_CAST_LENGTH);
                SHR_BITCLR_RANGE(mask_reg, SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_TRAFFIC_CAST_START, SOC_RAMON_FABRIC_CELL_MATCH_FILTER_REG_TRAFFIC_CAST_LENGTH);
                break;

            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported Filter Type");
        }
    }

    for (i = 0 ; i < dnxf_data_device.blocks.nof_instances_dch_get(unit); i++)
    {
        SHR_IF_ERR_EXIT(WRITE_DCH_MATCH_FILTr(unit, i, filter_reg));
        SHR_IF_ERR_EXIT(WRITE_DCH_MATCH_FILT_MASKr(unit, i, mask_reg));
    }

exit:
     SHR_FUNC_EXIT;
}

shr_error_e 
soc_ramon_cell_filter_count_get(int unit, int *count)
{
    int i, counter;
    uint32 temp_count[1] = {0};
    uint32 overflow_bit = 0;
    uint32 reg32_val;
    soc_reg_t dch_prg_cell_cnt_reg[DNXF_RAMON_FILTER_NOF_COUNTERS] = {DCH_PRG_CELL_CNT_0r, DCH_PRG_CELL_CNT_1r, DCH_PRG_CELL_CNT_2r};
    soc_field_t prg_cell_cnt_field[DNXF_RAMON_FILTER_NOF_COUNTERS] = {PRG_CELL_CNT_0f, PRG_CELL_CNT_1f, PRG_CELL_CNT_2f};
    SHR_FUNC_INIT_VARS(unit);

    *count = 0;
    for (counter = 0 ; counter < DNXF_RAMON_FILTER_NOF_COUNTERS ; counter++)
    {
        for(i = 0 ; i < dnxf_data_device.blocks.nof_instances_dch_get(unit) ; i++)
        {  
            SHR_IF_ERR_EXIT(soc_reg32_get(unit, dch_prg_cell_cnt_reg[counter], i, 0, &reg32_val));
            temp_count[0] = soc_reg_field_get(unit, dch_prg_cell_cnt_reg[counter], reg32_val, prg_cell_cnt_field[counter]);
            overflow_bit = soc_reg_field_length(unit, dch_prg_cell_cnt_reg[counter], prg_cell_cnt_field[counter]) - 1;

            
            if (DNXF_RAMON_IS_COUNTER_OVERFLOW(temp_count, overflow_bit))
            {
                *count = -1;
                break;
            }
            else
            {
                *count += temp_count[0];
            }
        }

        if (*count == -1)
        {
            break;
        }
    }
    
exit:
     SHR_FUNC_EXIT;
}

shr_error_e 
soc_ramon_control_cell_filter_set(int unit, uint32 flags, soc_dnxc_control_cell_types_t cell_type, uint32 array_size, soc_dnxc_control_cell_filter_type_t* filter_type_arr, uint32* filter_type_val) 
{
    int i;
    uint32 reg_val32;
    soc_reg_above_64_val_t reg_val, mask_val;
    soc_dnxf_cell_filter_mode_t filter_mode = soc_dnxf_control_cell_filter_copy_mode;

    SHR_FUNC_INIT_VARS(unit);

    if((flags & SOC_CELL_FILTER_FLAG_DONT_FORWARD) && (flags & SOC_CELL_FILTER_FLAG_DONT_TRAP)) {
        filter_mode = soc_dnxf_control_cell_filter_filter_mode;
    } else if (flags & SOC_CELL_FILTER_FLAG_DONT_FORWARD) {
        filter_mode = soc_dnxf_control_cell_filter_capture_mode;
    } else if (flags & SOC_CELL_FILTER_FLAG_DONT_TRAP) {
        filter_mode = soc_dnxf_control_cell_filter_normal_mode;
    }

    for (i = 0 ; i < dnxf_data_device.blocks.nof_instances_cch_get(unit) ; i++)
    {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_DEBUG_CONFIGURATIONSr, i, 0, &reg_val32));
        soc_reg_field_set(unit, CCH_DEBUG_CONFIGURATIONSr, &reg_val32, CAPTURE_OPERATIONf, filter_mode);
        
        soc_reg_field_set(unit, CCH_DEBUG_CONFIGURATIONSr, &reg_val32, AUTO_DOC_NAME_3f, (cell_type == soc_dnxc_fe2_filtered_cell) ? 1 : 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, CCH_DEBUG_CONFIGURATIONSr, i, 0, reg_val32));
    }

    SOC_REG_ABOVE_64_CLEAR(reg_val);
    
    SOC_REG_ABOVE_64_ALLONES(mask_val);

    if (cell_type == soc_dnxc_flow_status_cell || cell_type == soc_dnxc_credit_cell)
    {
        for (i = 0 ; i < array_size ; i++)
        {
            switch(filter_type_arr[i]) {
                case soc_dnxc_filter_control_cell_type_source_device:
                    if((filter_type_val[i] & (~0x7FF)) != 0) {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "source device id is out of range");
                    }
                    SHR_BITCOPY_RANGE(reg_val, RAMON_VSC256_CONTROL_CELL_SOURCE_ID_START, &(filter_type_val[i]), 0, RAMON_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH);
                    
                    SHR_BITCLR_RANGE(mask_val, RAMON_VSC256_CONTROL_CELL_SOURCE_ID_START, RAMON_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH);
                    break;
                case soc_dnxc_filter_control_cell_type_dest_device:
                    if((filter_type_val[i] & (~0x7FF)) != 0) {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "dest device id is out of range");
                    }
                    SHR_BITCOPY_RANGE(reg_val, RAMON_VSC256_CONTROL_CELL_DEST_DEVICE_START, &(filter_type_val[i]), 0, RAMON_VSC256_CONTROL_CELL_DEST_DEVICE_LENGTH);
                    
                    SHR_BITCLR_RANGE(mask_val, RAMON_VSC256_CONTROL_CELL_DEST_DEVICE_START, RAMON_VSC256_CONTROL_CELL_DEST_DEVICE_LENGTH);
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported filter type");
            }
        }
    } else if (cell_type == soc_dnxc_reachability_cell) 
    {
       SHR_BITCOPY_RANGE(reg_val, RAMON_VSC256_CONTROL_CELL_REACHABILITY_SOURCE_ID_START, &(filter_type_val[0]), 0, RAMON_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH);
       
       SHR_BITCLR_RANGE(mask_val, RAMON_VSC256_CONTROL_CELL_REACHABILITY_SOURCE_ID_START, RAMON_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH);
    } else { 
    }

     
    SHR_BITCOPY_RANGE(reg_val, RAMON_VSC256_CONTROL_CELL_CONTROL_TYPE_START, &cell_type, 0, RAMON_VSC256_CONTROL_CELL_CONTROL_TYPE_LENGTH);
    
    SHR_BITCLR_RANGE(mask_val, RAMON_VSC256_CONTROL_CELL_CONTROL_TYPE_START, RAMON_VSC256_CONTROL_CELL_CONTROL_TYPE_LENGTH);

    
   for(i=0 ; i < dnxf_data_device.blocks.nof_instances_cch_get(unit) ; i++) {
       SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, CCH_CAPTURE_FILTER_CELL_0r, i, 0, reg_val));
       SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, CCH_CAPTURE_FILTER_MASK_0r, i, 0, mask_val));
   }

exit:
     SHR_FUNC_EXIT;
}

shr_error_e 
soc_ramon_control_cell_filter_clear(int unit)
{
    soc_reg_above_64_val_t reg_val;
    uint32 reg_val32;
    int i;
    SHR_FUNC_INIT_VARS(unit);
    
    SOC_REG_ABOVE_64_CLEAR(reg_val);

    for (i = 0 ; i < dnxf_data_device.blocks.nof_instances_cch_get(unit); i++)
    {
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, CCH_CAPTURE_FILTER_CELL_0r, i, 0, reg_val));

        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, CCH_CAPTURE_FILTER_MASK_0r, i, 0, reg_val));

        
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_DEBUG_CONFIGURATIONSr, i, 0, &reg_val32));
        soc_reg_field_set(unit, CCH_DEBUG_CONFIGURATIONSr, &reg_val32, AUTO_DOC_NAME_3f, 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, CCH_DEBUG_CONFIGURATIONSr, i, 0, reg_val32));
    }

exit:
     SHR_FUNC_EXIT;
}

shr_error_e 
soc_ramon_control_cell_filter_receive(int unit,  soc_dnxc_captured_control_cell_t* data_out)
{
    soc_reg_above_64_val_t  reg_val;
    int i;
    uint32 valid_cell_reg;
    SHR_FUNC_INIT_VARS(unit);
    
    for (i = 0 ; i < dnxf_data_device.blocks.nof_instances_cch_get(unit); i++)
    {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CAPTURED_CELL_VALIDr, i, 0, &valid_cell_reg));
        if (valid_cell_reg)
        {
            data_out->valid = 1;
            break;
        }
    }

    if (data_out->valid)
    {
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, CCH_CAPTURED_CELLr, i, 0, reg_val));

        SHR_BITCOPY_RANGE((uint32*)&(data_out->control_type), 0, reg_val, RAMON_VSC256_CONTROL_CELL_CONTROL_TYPE_START, RAMON_VSC256_CONTROL_CELL_CONTROL_TYPE_LENGTH);

        switch(data_out->control_type) {
                case soc_dnxc_flow_status_cell:
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->dest_port), 0, reg_val, RAMON_VSC256_CONTROL_CELL_FSM_DEST_PORT_START, RAMON_VSC256_CONTROL_CELL_FSM_DEST_PORT_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->src_queue_num), 0, reg_val, RAMON_VSC256_CONTROL_CELL_FSM_SRC_QUEUE_NUM_START, RAMON_VSC256_CONTROL_CELL_FSM_SRC_QUEUE_NUM_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->flow_id), 0, reg_val, RAMON_VSC256_CONTROL_CELL_FSM_FLOW_ID_START, RAMON_VSC256_CONTROL_CELL_FSM_FLOW_ID_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->source_device), 0, reg_val, RAMON_VSC256_CONTROL_CELL_SOURCE_ID_START, RAMON_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->dest_device), 0, reg_val, RAMON_VSC256_CONTROL_CELL_DEST_DEVICE_START, RAMON_VSC256_CONTROL_CELL_DEST_DEVICE_LENGTH);
                    break;
                case soc_dnxc_credit_cell:
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->dest_queue_num),0 , reg_val, RAMON_VSC256_CONTROL_CELL_CREDIT_DEST_Q_NUM_START, RAMON_VSC256_CONTROL_CELL_CREDIT_DEST_Q_NUM_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->sub_flow_id), 0, reg_val, RAMON_VSC256_CONTROL_CELL_CREDIT_SUB_FLOW_ID_START, RAMON_VSC256_CONTROL_CELL_CREDIT_SUB_FLOW_ID_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->flow_id), 0, reg_val, RAMON_VSC256_CONTROL_CELL_CREDIT_FLOW_ID_START, RAMON_VSC256_CONTROL_CELL_CREDIT_FLOW_ID_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->source_device), 0, reg_val, RAMON_VSC256_CONTROL_CELL_SOURCE_ID_START, RAMON_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->dest_device), 0, reg_val, RAMON_VSC256_CONTROL_CELL_DEST_DEVICE_START, RAMON_VSC256_CONTROL_CELL_DEST_DEVICE_LENGTH);
                    break;
                case soc_dnxc_reachability_cell:
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->reachability_bitmap),0 , reg_val, RAMON_VSC256_CONTROL_CELL_REACHABILITY_BITMAP_START, RAMON_VSC256_CONTROL_CELL_REACHABILITY_BITMAP_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->base_index),0 , reg_val, RAMON_VSC256_CONTROL_CELL_REACHABILITY_BASE_ID_START, RAMON_VSC256_CONTROL_CELL_REACHABILITY_BASE_ID_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->source_link_number),0 , reg_val, RAMON_VSC256_CONTROL_CELL_REACHABILITY_SRC_LINK_NUM_START, RAMON_VSC256_CONTROL_CELL_REACHABILITY_SRC_LINK_NUM_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->source_device),0 , reg_val, RAMON_VSC256_CONTROL_CELL_REACHABILITY_SOURCE_ID_START, RAMON_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH);
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported filter type");
        }
    }

exit:
    SHR_FUNC_EXIT; 
}



shr_error_e
soc_ramon_fabric_cell_get(int unit, soc_dnxc_fabric_cell_entry_t entry)
{
    uint32 reg = 0;
    uint32 is_fifo_not_empty = 0;
    int found_cell = 0;
    int blk_idx = 0;
    SHR_FUNC_INIT_VARS(unit);

    for(blk_idx=0 ; blk_idx < dnxf_data_device.blocks.nof_instances_dcml_get(unit) ; blk_idx++) 
    {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, DCML_INTERRUPT_REGISTERr, blk_idx, 0, &reg));
        is_fifo_not_empty = soc_reg_field_get(unit, DCML_INTERRUPT_REGISTERr, reg, DFL_CPU_CELL_FIFO_NE_INTf);       
   
        if(is_fifo_not_empty) {
            sal_memset(entry, 0x00, sizeof(soc_dnxc_fabric_cell_entry_t));

            
            SHR_IF_ERR_EXIT(sand_mem_array_wide_access(unit, SOC_MEM_NO_FLAGS, DCML_AUTO_DOC_NAME_55m, 0, DCML_BLOCK(unit, blk_idx), 0, entry, 1));
            found_cell = 1;
            break;
        }
    }

    if (!found_cell)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_ramon_fabric_cell_parse_table_get(int unit, soc_dnxc_fabric_cell_entry_t entry, soc_dnxc_fabric_cell_type_t *cell_type, soc_dnxc_fabric_cell_parse_table_t *parse_table)
{
    int line = 0;
    uint32 data_in_msb = 0, cell_size_bytes = 0;
    soc_dnxc_fabric_cell_parse_table_t captured_cell_common_table[] = {SOC_RAMON_FABRIC_CAPTURED_CELL_COMMON_FORMAT};
    SHR_FUNC_INIT_VARS(unit);

    
    soc_dnxc_fabric_cell_table_field_row_get(unit, soc_dnxc_fabric_cell_field_data_in_msb, captured_cell_common_table, &line);
    SHR_BITCOPY_RANGE(&data_in_msb, 0, entry,           captured_cell_common_table[line].src_start_bit,     captured_cell_common_table[line].length);

    soc_dnxc_fabric_cell_table_field_row_get(unit, soc_dnxc_fabric_cell_field_cell_size, captured_cell_common_table, &line);
    SHR_BITCOPY_RANGE(&cell_size_bytes, 0, entry,       captured_cell_common_table[line].src_start_bit,     captured_cell_common_table[line].length);
    
    cell_size_bytes +=1;

    soc_dnxc_fabric_cell_table_field_row_get(unit, soc_dnxc_fabric_cell_field_cell_type, captured_cell_common_table, &line);
    SHR_BITCOPY_RANGE((uint32 *) cell_type, 0, entry,  captured_cell_common_table[line].src_start_bit,     captured_cell_common_table[line].length);

    
    if (*cell_type == soc_dnxc_fabric_cell_type_sr_cell)
    {
        soc_dnxc_fabric_cell_parse_table_t temp_table_src[] = SOC_RAMON_FABRIC_CELL_CAPTURED_SR_CELL_FORMAT_TABLE;

        sal_memcpy(parse_table, temp_table_src, sizeof(temp_table_src) );
        
        soc_dnxc_fabric_cell_table_field_row_get(unit, soc_dnxc_fabric_cell_field_payload, parse_table, &line);

        if (cell_size_bytes <= SOC_RAMON_FABRIC_CAPTURED_CELL_SMALL_CELL_MAX_SIZE)
        {
            
            if (data_in_msb)
            {
                
                parse_table[line].src_start_bit += SOC_RAMON_FABRIC_CAPTURED_CELL_DATA_IN_MSB_OFFSET;
            }
            else
            {
                
            }
            
            parse_table[line].src_start_bit += BYTES2BITS(SOC_RAMON_FABRIC_CAPTURED_CELL_SMALL_CELL_MAX_SIZE - cell_size_bytes);
            parse_table[line].length = BYTES2BITS(cell_size_bytes);
        }
        else
        {
            
        }

    }
    else
    {
        soc_dnxc_fabric_cell_parse_table_t temp_table_src[] = SOC_RAMON_FABRIC_CELL_CAPTURED_DATA_CELL_FORMAT_TABLE;

        sal_memcpy(parse_table, temp_table_src, sizeof(temp_table_src) );
        soc_dnxc_fabric_cell_table_field_row_get(unit, soc_dnxc_fabric_cell_field_payload, parse_table, &line);

        if (cell_size_bytes <= SOC_RAMON_FABRIC_CAPTURED_CELL_SMALL_CELL_MAX_SIZE)
        {
            if (data_in_msb)
            {
                
                parse_table[line].src_start_bit += SOC_RAMON_FABRIC_CAPTURED_CELL_DATA_IN_MSB_OFFSET;
            }
            else
            {
                
                parse_table[line].length -= SOC_RAMON_FABRIC_CAPTURED_CELL_DATA_IN_MSB_OFFSET;
            }
        }


    }


    SHR_FUNC_EXIT; 
}

#undef BSL_LOG_MODULE
