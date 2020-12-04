/*
* 
*
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* SOC ARAD FABRIC CELL
*/
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC


#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/mcm/memregs.h>
#include <soc/error.h>
#include <shared/bitop.h>
#include <soc/dpp/ARAD/arad_fabric_cell.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_multicast_fabric.h>
#include <soc/dpp/ARAD/arad_cell.h>
#include <soc/dcmn/dcmn_captured_buffer.h>
#include <soc/dcmn/vsc256_fabric_cell.h>
#include <soc/dcmn/dcmn_cells_buffer.h>
#include <soc/dcmn/dcmn_fabric_cell.h>

soc_error_t 
soc_arad_cell_filter_set(int unit, uint32 array_size, soc_dcmn_filter_type_t* filter_type_arr, uint32* filter_type_val) 
{
    int i, rc;
    soc_reg_above_64_val_t reg_val, mask_val;
    uint32 table_size;

    SOCDNX_INIT_FUNC_DEFS;
    SOC_REG_ABOVE_64_CLEAR(reg_val);
    SOC_REG_ABOVE_64_CLEAR(mask_val);
    
    for(i=0 ; i<array_size ; i++) {
        switch(filter_type_arr[i]) {
        case soc_dcmn_filter_type_source_id:
            SHR_BITCOPY_RANGE(reg_val, 33, &filter_type_val[i], 0, 11);
            SHR_BITSET_RANGE(reg_val, 33, 11);
            break;
        case soc_dcmn_filter_type_multicast_id:
            rc = arad_multicast_table_size_get(unit, &table_size);
            SOCDNX_IF_ERR_EXIT(rc);
            if(filter_type_val[i] >= table_size) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("multicast id out of range")));
            }
            SHR_BITCOPY_RANGE(reg_val, 44, &filter_type_val[i], 0, 19);
            --table_size;
            SHR_BITCOPY_RANGE(mask_val, 44, &table_size, 0, 19);
            ++table_size;
            break;
        case soc_dcmn_filter_type_priority:
            if((filter_type_val[i] & (~0x3)) != 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("the priority type is out of range")));
            }
            SHR_BITCOPY_RANGE(reg_val, 63, &filter_type_val[i], 0, 2);
            SHR_BITSET_RANGE(mask_val, 63, 2);
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unsupported filter type")));
        }
    }

    SOCDNX_IF_ERR_EXIT(WRITE_FDR_PROGRAMMABLE_DATA_CELL_COUNTER_0r(unit, reg_val));
    SOCDNX_IF_ERR_EXIT(WRITE_FDR_PROGRAMMABLE_DATA_CELL_COUNTER_MASK_0r(unit, mask_val));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_arad_cell_filter_clear(int unit)
{
    soc_reg_above_64_val_t cleared_reg;
    uint64 cleared_64;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(cleared_reg);
    COMPILER_64_ZERO(cleared_64);
    SOCDNX_IF_ERR_EXIT(WRITE_FDR_PROGRAMMABLE_DATA_CELL_COUNTER_0r(unit, cleared_reg));
    SOCDNX_IF_ERR_EXIT(WRITE_FDR_PROGRAMMABLE_DATA_CELL_COUNTER_1r_REG64(unit, cleared_64));
    SOCDNX_IF_ERR_EXIT(WRITE_FDR_PROGRAMMABLE_DATA_CELL_COUNTER_MASK_0r(unit, cleared_reg));
    SOCDNX_IF_ERR_EXIT(WRITE_FDR_PROGRAMMABLE_DATA_CELL_COUNTER_MASK_1r_REG64(unit, cleared_64));
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
soc_arad_cpu2cpu_read(int unit)
{
    int rc = SOC_E_NONE;
    uint8 success;
    uint32 packed_cpu_data_cell_rcv[ARAD_DATA_CELL_RECEIVED_UINT32_SIZE] ;
    vsc256_sr_cell_t cell;
    dcmn_captured_cell_t captured_cell;
    uint32 cell_type = 0;

    SOCDNX_INIT_FUNC_DEFS;



    rc = arad_cell_ack_get(unit, FALSE, packed_cpu_data_cell_rcv, &success);
    SOCDNX_IF_ERR_EXIT(rc);
    SHR_BITCOPY_RANGE(&cell_type, 0, packed_cpu_data_cell_rcv, ARAD_PARSING_CELL_TYPE_START, ARAD_PARSING_CELL_TYPE_LENGTH);
    if(cell_type == SR_CELL_TYPE) {
        rc = soc_arad_parse_cell(unit, FALSE, packed_cpu_data_cell_rcv, &cell);
        SOCDNX_IF_ERR_EXIT(rc);
        rc = dcmn_cells_buffer_add(unit, &SOC_DPP_CONFIG(unit)->arad->sr_cells_buffer, &cell);
        SOCDNX_IF_ERR_EXIT(rc);
    } else {
        rc = soc_dcmn_parse_captured_cell(unit, packed_cpu_data_cell_rcv, &captured_cell);
        SOCDNX_IF_ERR_EXIT(rc);
        rc = dcmn_captured_buffer_add(unit, &SOC_DPP_CONFIG(unit)->arad->captured_cells_buffer, &captured_cell);
        SOCDNX_IF_ERR_EXIT(rc);
    }
exit:
    SOCDNX_FUNC_RETURN; 
}
soc_error_t 
soc_arad_cell_filter_receive(int unit,  dcmn_captured_cell_t* data_out)
{
    int rc = SOC_E_NONE, is_empty = TRUE;
    SOCDNX_INIT_FUNC_DEFS;

    rc = dcmn_captured_buffer_is_empty(unit, &SOC_DPP_CONFIG(unit)->arad->captured_cells_buffer, &is_empty);
    SOCDNX_IF_ERR_EXIT(rc);

    if(is_empty) {
        rc = soc_arad_cpu2cpu_read(unit);
        SOCDNX_IF_ERR_EXIT(rc);
    }

    rc = dcmn_captured_buffer_get(unit, &SOC_DPP_CONFIG(unit)->arad->captured_cells_buffer, data_out);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
soc_arad_control_cell_filter_set(int unit, soc_dcmn_control_cell_types_t cell_type, uint32 array_size, soc_dcmn_control_cell_filter_type_t* filter_type_arr, uint32* filter_type_val) 
{
    int i = 0;
    soc_reg_above_64_val_t reg_val, mask_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_val);
    
    SOC_REG_ABOVE_64_ALLONES(mask_val);

    if ((cell_type == soc_dcmn_flow_status_cell) || (cell_type == soc_dcmn_credit_cell))
    {
        for(i=0 ; i<array_size ; i++) 
        {
            switch(filter_type_arr[i]) {
                case soc_dcmn_filter_control_cell_type_source_device:
                    if((filter_type_val[i] & (~0x7FF)) != 0) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("source device id is out of range")));
                    }
                    SHR_BITCOPY_RANGE(reg_val, ARAD_VSC256_CONTROL_CELL_SOURCE_ID_START, &(filter_type_val[i]), 0, ARAD_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH);
                    
                    SHR_BITCLR_RANGE(mask_val, ARAD_VSC256_CONTROL_CELL_SOURCE_ID_START, ARAD_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH);
                    break;
                case soc_dcmn_filter_control_cell_type_dest_device:
                    if((filter_type_val[i] & (~0x7FF)) != 0) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("dest device id is out of range")));
                    }
                    SHR_BITCOPY_RANGE(reg_val, ARAD_VSC256_CONTROL_CELL_SOURCE_ID_START, &(filter_type_val[i]), 0, ARAD_VSC256_CONTROL_CELL_DEST_DEVICE_LENGTH);
                    
                    SHR_BITCLR_RANGE(mask_val, ARAD_VSC256_CONTROL_CELL_SOURCE_ID_START, ARAD_VSC256_CONTROL_CELL_DEST_DEVICE_LENGTH);
                    break;
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unsupported filter type")));
            }
        }
    }
    
    else
    {
        SHR_BITCOPY_RANGE(reg_val, ARAD_VSC256_CONTROL_CELL_REACHABILITY_SOURCE_ID_START, &(filter_type_val[i]), 0, ARAD_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH);
        
        SHR_BITCLR_RANGE(mask_val, ARAD_VSC256_CONTROL_CELL_REACHABILITY_SOURCE_ID_START, ARAD_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH);
    }

     
    SHR_BITCOPY_RANGE(reg_val, ARAD_VSC256_CONTROL_CELL_CONTROL_TYPE_START, &cell_type, 0, ARAD_VSC256_CONTROL_CELL_CONTROL_TYPE_LENGTH);
    
    SHR_BITCLR_RANGE(mask_val, ARAD_VSC256_CONTROL_CELL_CONTROL_TYPE_START, ARAD_VSC256_CONTROL_CELL_CONTROL_TYPE_LENGTH);

    
    SOCDNX_IF_ERR_EXIT(WRITE_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTERr(unit, reg_val));
    SOCDNX_IF_ERR_EXIT(WRITE_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_MASK_0r(unit, mask_val));
    
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_arad_control_cell_filter_clear(int unit)
{
    soc_reg_above_64_val_t reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_val);
    SOCDNX_IF_ERR_EXIT(WRITE_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTERr(unit, reg_val));
    SOCDNX_IF_ERR_EXIT(WRITE_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_MASK_0r(unit, reg_val));
    
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_arad_control_cell_filter_receive(int unit,  soc_dcmn_captured_control_cell_t* data_out)
{
    soc_reg_above_64_val_t  reg_val;
    uint32 interrupt_reg;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_FCR_INTERRUPT_REGISTERr(unit, &interrupt_reg));
    if(soc_reg_field_get(unit, FCR_INTERRUPT_REGISTERr, interrupt_reg, CPU_CNT_CELL_FNEf))
    {
        data_out->valid = TRUE;
    }

    if (data_out->valid) 
    {
        SOCDNX_IF_ERR_EXIT(READ_FCR_CONTROL_CELL_FIFO_BUFFERr(unit, reg_val));
        
        SHR_BITCOPY_RANGE((uint32*)&(data_out->control_type), 0, reg_val, ARAD_VSC256_CONTROL_CELL_CONTROL_TYPE_LENGTH, ARAD_VSC256_CONTROL_CELL_CONTROL_TYPE_LENGTH);

        switch(data_out->control_type) {
                case soc_dcmn_flow_status_cell:
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->dest_port), 0, reg_val, ARAD_VSC256_CONTROL_CELL_FSM_DEST_PORT_START, ARAD_VSC256_CONTROL_CELL_FSM_DEST_PORT_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->src_queue_num), 0, reg_val, ARAD_VSC256_CONTROL_CELL_FSM_SRC_QUEUE_NUM_START, ARAD_VSC256_CONTROL_CELL_FSM_SRC_QUEUE_NUM_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->flow_id), 0, reg_val, ARAD_VSC256_CONTROL_CELL_FSM_FLOW_ID_START, ARAD_VSC256_CONTROL_CELL_FSM_FLOW_ID_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->source_device), 0, reg_val, ARAD_VSC256_CONTROL_CELL_SOURCE_ID_START, ARAD_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->dest_device), 0, reg_val, ARAD_VSC256_CONTROL_CELL_DEST_DEVICE_START, ARAD_VSC256_CONTROL_CELL_DEST_DEVICE_LENGTH);
                    break;
                case soc_dcmn_credit_cell:
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->dest_queue_num),0 , reg_val, ARAD_VSC256_CONTROL_CELL_CREDIT_DEST_Q_NUM_START, ARAD_VSC256_CONTROL_CELL_CREDIT_DEST_Q_NUM_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->sub_flow_id), 0, reg_val, ARAD_VSC256_CONTROL_CELL_CREDIT_SUB_FLOW_ID_START, ARAD_VSC256_CONTROL_CELL_CREDIT_SUB_FLOW_ID_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->flow_id), 0, reg_val, ARAD_VSC256_CONTROL_CELL_CREDIT_FLOW_ID_START, ARAD_VSC256_CONTROL_CELL_CREDIT_FLOW_ID_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->source_device), 0, reg_val, ARAD_VSC256_CONTROL_CELL_SOURCE_ID_START, ARAD_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->dest_device), 0, reg_val, ARAD_VSC256_CONTROL_CELL_DEST_DEVICE_START, ARAD_VSC256_CONTROL_CELL_DEST_DEVICE_LENGTH);
                    break;
                case soc_dcmn_reachability_cell:
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->reachability_bitmap),0 , reg_val, ARAD_VSC256_CONTROL_CELL_REACHABILITY_BITMAP_START, ARAD_VSC256_CONTROL_CELL_REACHABILITY_BITMAP_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->base_index),0 , reg_val, ARAD_VSC256_CONTROL_CELL_REACHABILITY_BASE_ID_START, ARAD_VSC256_CONTROL_CELL_REACHABILITY_BASE_ID_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->source_link_number),0 , reg_val, ARAD_VSC256_CONTROL_CELL_REACHABILITY_SRC_LINK_NUM_START, ARAD_VSC256_CONTROL_CELL_REACHABILITY_SRC_LINK_NUM_LENGTH);
                    SHR_BITCOPY_RANGE((uint32*)&(data_out->source_device),0 , reg_val, ARAD_VSC256_CONTROL_CELL_REACHABILITY_SOURCE_ID_START, ARAD_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH);
                    break;
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unsupported filter type")));
        }

    }

exit:
    SOCDNX_FUNC_RETURN; 
}
