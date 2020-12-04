/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE3200 FABRIC FLOW CONTROL
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC

#include <shared/bsl.h>
#include <soc/dfe/fe3200/fe3200_fabric_flow_control.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/fabric.h>

#include <bcm/types.h>
#include <soc/defs.h>

#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>
#include <soc/error.h>
#include <soc/dfe/cmn/dfe_drv.h>

#include <shared/bitop.h>

#define SOC_FE3200_DCL_DCM_OCCUPANCY_COUNTER_PER_LINK_MAX_VAL       (4095)

STATIC soc_error_t
soc_fe3200_fabric_flow_control_mid_tx_prevent_counter_per_link_overflow (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3);




soc_error_t 
soc_fe3200_fabric_flow_control_rci_gci_control_source_set(int unit, bcm_fabric_control_t type, soc_dcmn_fabric_pipe_t val) 
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_DCMC_CIG_ENABLERS_REGISTERr(unit, &reg_val));
    
    switch(type)
    {
        case bcmFabricRCIControlSource:
            soc_reg_field_set(unit, DCMC_CIG_ENABLERS_REGISTERr, &reg_val, RCI_SOURCEf, val);
            break;
        
        case bcmFabricGCIControlSource:
            soc_reg_field_set(unit, DCMC_CIG_ENABLERS_REGISTERr, &reg_val, GCI_SOURCEf, val);
            break;
        
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong control source type %d"),type)); 
    }
    
    SOCDNX_IF_ERR_EXIT(WRITE_DCMC_CIG_ENABLERS_REGISTERr(unit, reg_val));
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe3200_fabric_flow_control_rci_gci_control_source_get(int unit, bcm_fabric_control_t type, soc_dcmn_fabric_pipe_t* val) 
{
    int nof_pipes;
    uint32 reg_val, enabler_val,mask=0;
    SOCDNX_INIT_FUNC_DEFS;

    nof_pipes=SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;
    SOC_DCMN_FABRIC_PIPE_ALL_PIPES_SET(&mask,nof_pipes);
    
    SOCDNX_IF_ERR_EXIT(READ_DCMC_CIG_ENABLERS_REGISTERr(unit, &reg_val));
      
    switch(type)
    {
        case bcmFabricRCIControlSource:
            enabler_val = soc_reg_field_get(unit, DCMC_CIG_ENABLERS_REGISTERr, reg_val, RCI_SOURCEf);
            break;
        
        case bcmFabricGCIControlSource:
            enabler_val = soc_reg_field_get(unit, DCMC_CIG_ENABLERS_REGISTERr, reg_val, GCI_SOURCEf);
            break;
        
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong control source type %d"),type)); 
    }
    
    enabler_val=SOC_DCMN_FABRIC_PIPE_ALL_PIPES_GET(enabler_val,mask);
    *val=enabler_val;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_fe3200_fabric_links_link_type_set(int unit, soc_dcmn_fabric_pipe_t pipe, soc_dfe_fabric_link_fifo_type_index_t type_index, 
                                        int is_rx, int is_tx, int is_fe1, int is_fe3, uint32 links_count, soc_port_t* links){
    
    int i,blk_id;
    soc_port_t link, inner_link;
    uint64 regs_values_dch[SOC_DFE_DEFS_MAX(NOF_INSTANCES_DCH)];
    uint64 regs_values_dcl[SOC_DFE_DEFS_MAX(NOF_INSTANCES_DCL)];
    SOCDNX_INIT_FUNC_DEFS;

    for (blk_id = 0; blk_id < SOC_DFE_DEFS_GET(unit, nof_instances_dch); blk_id++)
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_BMPr(unit, blk_id, &regs_values_dch[blk_id]));
    }

    for (blk_id = 0; blk_id < SOC_DFE_DEFS_GET(unit, nof_instances_dcl); blk_id++)
    {
        SOCDNX_IF_ERR_EXIT(READ_DCL_LINK_TYPEr(unit, blk_id, &regs_values_dcl[blk_id]));
    }


    for (i = 0; i<links_count ; i++)
    {
        link=links[i];
        
        
        SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_drv_link_to_block_mapping,(unit,link,&blk_id,&inner_link, SOC_BLK_DCH)));

        if (type_index == soc_dfe_fabric_link_fifo_type_index_0)
        {
            COMPILER_64_BITCLR(regs_values_dch[blk_id], inner_link);
        }
        else
        {
            COMPILER_64_BITSET(regs_values_dch[blk_id], inner_link);
        }

        SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_drv_link_to_block_mapping,(unit,link,&blk_id,&inner_link, SOC_BLK_DCL)));

        if (type_index == soc_dfe_fabric_link_fifo_type_index_0)
        {
            COMPILER_64_BITCLR(regs_values_dcl[blk_id], inner_link);
        }
        else
        {
            COMPILER_64_BITSET(regs_values_dcl[blk_id], inner_link);
        }

    }

    for (blk_id = 0; blk_id < SOC_DFE_DEFS_GET(unit, nof_instances_dch); blk_id++)
    {
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_BMPr(unit, blk_id, regs_values_dch[blk_id]));
    }

    for (blk_id = 0; blk_id < SOC_DFE_DEFS_GET(unit, nof_instances_dcl); blk_id++)
    {
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_LINK_TYPEr(unit, blk_id, regs_values_dcl[blk_id]));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_fe3200_fabric_links_link_type_get(int unit, soc_dcmn_fabric_pipe_t pipe, soc_dfe_fabric_link_fifo_type_index_t type_index, 
                                          int is_rx, int is_tx, int is_fe1, int is_fe3, uint32 links_count_max, soc_port_t* links, uint32* links_count)
{
    int blk_id;
    soc_port_t link, inner_link;
    int actual_type_index;
    pbmp_t valid_link_bitmap;
    uint64 regs_values[SOC_DFE_DEFS_MAX(NOF_INSTANCES_DCH)];
    SOCDNX_INIT_FUNC_DEFS;  


    
    
    for (blk_id = 0; blk_id < SOC_DFE_DEFS_GET(unit, nof_instances_dch); blk_id++)
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_BMPr(unit, blk_id, &regs_values[blk_id]));
    }


    *links_count = 0;
    SOC_PBMP_ASSIGN(valid_link_bitmap, SOC_INFO(unit).sfi.bitmap);
    SOC_PBMP_ITER(valid_link_bitmap, link)
    {
        actual_type_index = -1;
        
        SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_drv_link_to_block_mapping,(unit,link,&blk_id,&inner_link, SOC_BLK_DCH)));

        if (COMPILER_64_BITTEST(regs_values[blk_id], inner_link))
        {
            actual_type_index = soc_dfe_fabric_link_fifo_type_index_1;
        } else {
            actual_type_index = soc_dfe_fabric_link_fifo_type_index_0;
        }
        
        if(actual_type_index == type_index)
        {
            if(*links_count >= links_count_max)
            {
                *links_count = 0;
                SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("links_count_max %d is too small"),links_count_max));
            }
            links[*links_count] = link;
            (*links_count)++;
        }
    }
      
exit:
    SOCDNX_FUNC_RETURN;  
}

soc_error_t
soc_fe3200_fabric_flow_control_thresholds_flags_validate(int unit,uint32 flags){
    uint32 valid_flags= BCM_FABRIC_LINK_TH_FE1_LINKS_ONLY | BCM_FABRIC_LINK_TH_FE3_LINKS_ONLY ;
    SOCDNX_INIT_FUNC_DEFS;

    if (~valid_flags & flags )
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid flags for threshold handle")));
    }
      
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_rx_llfc_threshold_validate(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value)
{

    soc_error_t rc = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;


    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    { 
        if(type_index == soc_dfe_fabric_link_fifo_type_index_0){
            rc = soc_reg_field_validate(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_0r, LNK_LVL_FC_TH_0_P_0f, value);
        } else {
            rc = soc_reg_field_validate(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_0r, LNK_LVL_FC_TH_1_P_0f, value); 
        }
    }
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for RX LLFC threshold"),value)); 
    }
    
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {       
        if(type_index == soc_dfe_fabric_link_fifo_type_index_0) {
            rc = soc_reg_field_validate(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_1r, LNK_LVL_FC_TH_0_P_1f, value);
        } else {
            rc = soc_reg_field_validate(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_1r, LNK_LVL_FC_TH_1_P_1f, value); 
        }
    }
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for RX LLFC threshold"),value)); 
    }

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,2))
    {       
        if(type_index == soc_dfe_fabric_link_fifo_type_index_0) {
            rc = soc_reg_field_validate(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_2r, LNK_LVL_FC_TH_0_P_2f, value);
        } else {
            rc = soc_reg_field_validate(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_2r, LNK_LVL_FC_TH_1_P_2f, value); 
        }
    }
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for RX LLFC threshold"),value)); 
    }
         
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_tx_bypass_llfc_threshold_validate(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value)
{

    soc_error_t rc = SOC_E_NONE;
    uint64  val64;


    SOCDNX_INIT_FUNC_DEFS;
    COMPILER_64_SET(val64, 0, value);

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            rc = soc_reg64_field_validate(unit, DCL_DCL_LLFC_THr, P_0_LLFC_TH_TYPE_0f, val64);
        else
            rc = soc_reg64_field_validate(unit, DCL_DCL_LLFC_THr, P_0_LLFC_TH_TYPE_1f, val64);
    }
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for TX BYBASS LLFC threshold"),value)); 
    }

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            rc = soc_reg64_field_validate(unit, DCL_DCL_LLFC_THr, P_1_LLFC_TH_TYPE_0f, val64);
        else
            rc = soc_reg64_field_validate(unit, DCL_DCL_LLFC_THr, P_1_LLFC_TH_TYPE_1f, val64);
    }
     if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for TX BYBASS LLFC threshold"),value)); 
    }
     
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,2))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            rc = soc_reg64_field_validate(unit, DCL_DCL_LLFC_THr, P_2_LLFC_TH_TYPE_0f, val64);
        else
            rc = soc_reg64_field_validate(unit, DCL_DCL_LLFC_THr, P_2_LLFC_TH_TYPE_1f, val64);
    }
     if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for TX BYBASS LLFC threshold"),value)); 
    }       
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_tx_rci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value)
{

    soc_error_t rc = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;
    
    if(soc_dfe_fabric_link_fifo_type_index_0 == type_index) 
    {
        switch (type)
        {
            case bcmFabricLinkRciFC:
                rc = soc_reg_field_validate(unit, DCL_TYPE_0_RCI_TH_Pr, RCI_TH_LOW_0_P_Nf, value);
                break;
            case bcmFabricLinkTxRciLvl1FC:
                rc = soc_reg_field_validate(unit, DCL_TYPE_0_RCI_TH_Pr, RCI_TH_LOW_0_P_Nf, value);
                break;
            case bcmFabricLinkTxRciLvl2FC:
                rc = soc_reg_field_validate(unit, DCL_TYPE_0_RCI_TH_Pr, RCI_TH_MED_0_P_Nf, value);
                break;
            case bcmFabricLinkTxRciLvl3FC:
                rc = soc_reg_field_validate(unit, DCL_TYPE_0_RCI_TH_Pr, RCI_TH_HIGH_0_P_Nf, value);
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
        }
    } 
    else 
    {
        switch (type)
        {
            case bcmFabricLinkRciFC:
                rc = soc_reg_field_validate(unit, DCL_TYPE_1_RCI_TH_Pr, RCI_TH_LOW_1_P_Nf, value);
                break;
            case bcmFabricLinkTxRciLvl1FC:
                rc = soc_reg_field_validate(unit, DCL_TYPE_1_RCI_TH_Pr, RCI_TH_LOW_1_P_Nf, value);
                break;
            case bcmFabricLinkTxRciLvl2FC:
                rc = soc_reg_field_validate(unit, DCL_TYPE_1_RCI_TH_Pr, RCI_TH_MED_1_P_Nf, value);
                break;
            case bcmFabricLinkTxRciLvl3FC:
                rc = soc_reg_field_validate(unit, DCL_TYPE_1_RCI_TH_Pr, RCI_TH_HIGH_1_P_Nf, value);
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
        }
    }
    
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for TX RCI threshold"),value)); 
    }
   
exit:
    SOCDNX_FUNC_RETURN;

}

soc_error_t soc_fe3200_fabric_flow_control_tx_gci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value)
{
    soc_error_t rc = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;
    


    if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
    {
        switch(type)
        {
            case bcmFabricLinkTxGciLvl1FC: 
                rc = soc_reg_field_validate(unit, DCL_TYPE_0_GCI_TH_Pr, GCI_TH_LOW_0_P_Nf, value); 
                break;
            case bcmFabricLinkTxGciLvl2FC: 
                rc = soc_reg_field_validate(unit, DCL_TYPE_0_GCI_TH_Pr, GCI_TH_MED_0_P_Nf, value); 
                break;
            case bcmFabricLinkTxGciLvl3FC: 
                rc = soc_reg_field_validate(unit, DCL_TYPE_0_GCI_TH_Pr, GCI_TH_HIGH_0_P_Nf, value); 
                break;
            default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
        }
    }
    else
    {
        switch(type)
        {
            case bcmFabricLinkTxGciLvl1FC: 
                rc = soc_reg_field_validate(unit, DCL_TYPE_1_GCI_TH_Pr, GCI_TH_LOW_1_P_Nf, value); 
                break;
            case bcmFabricLinkTxGciLvl2FC: 
                rc = soc_reg_field_validate(unit, DCL_TYPE_1_GCI_TH_Pr, GCI_TH_MED_1_P_Nf, value); 
                break;
            case bcmFabricLinkTxGciLvl3FC: 
                rc = soc_reg_field_validate(unit, DCL_TYPE_1_GCI_TH_Pr, GCI_TH_HIGH_1_P_Nf, value); 
                break;
            default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
        }
    }
    
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid threshold for type %d"),value,type)); 
    } 


      
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_rx_gci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value, int is_fe1, int is_fe3)
{
    soc_error_t rc = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;

    
    if(SOC_DFE_IS_FE13(unit))
    {
        if (is_fe3 && !is_fe1)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("rx gci unavailable for fe3 links only")));
        }
    }
    
    if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
    {
        switch(type)
        {
            case bcmFabricLinkRxGciLvl1FC: 
                rc = soc_reg_field_validate(unit, DCH_LOCAL_GCI_RCI_TYPE_0_Pr, LOCAL_GCI_RCI_0_TYPE_0_TH_P_Nf, value); 
                break;
            case bcmFabricLinkRxGciLvl2FC: 
                rc = soc_reg_field_validate(unit, DCH_LOCAL_GCI_RCI_TYPE_0_Pr, LOCAL_GCI_RCI_1_TYPE_0_TH_P_Nf, value); 
                break;
            case bcmFabricLinkRxGciLvl3FC: 
                rc = soc_reg_field_validate(unit, DCH_LOCAL_GCI_RCI_TYPE_0_Pr, LOCAL_GCI_RCI_2_TYPE_0_TH_P_Nf, value); 
                break;
            default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
        }
    }
    else
    {     
        switch(type)
        {
            case bcmFabricLinkRxGciLvl1FC: 
                rc = soc_reg_field_validate(unit, DCH_LOCAL_GCI_RCI_TYPE_1_Pr, LOCAL_GCI_RCI_0_TYPE_1_TH_P_Nf, value);
                break;
            case bcmFabricLinkRxGciLvl2FC: 
                rc = soc_reg_field_validate(unit, DCH_LOCAL_GCI_RCI_TYPE_1_Pr, LOCAL_GCI_RCI_1_TYPE_1_TH_P_Nf, value); 
                break;
            case bcmFabricLinkRxGciLvl3FC: 
                rc = soc_reg_field_validate(unit, DCH_LOCAL_GCI_RCI_TYPE_1_Pr, LOCAL_GCI_RCI_2_TYPE_1_TH_P_Nf, value); 
                break;
            default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
        }
    }
    
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for RX GCI threshold"),value)); 
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_tx_drop_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value)
{
    uint64          val64;
    soc_error_t rc = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;  
    
    COMPILER_64_SET(val64, 0, value);

    if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
    {
        switch(type)
        {
            case bcmFabricLinkTxPrio0Drop: 
                rc = soc_reg64_field_validate(unit, DCL_TYPE_0_DRP_PRI_Pr, TYPE_0_DRP_PRI_0_P_Nf, val64); 
                break;
            case bcmFabricLinkTxPrio1Drop: 
                rc = soc_reg64_field_validate(unit, DCL_TYPE_0_DRP_PRI_Pr, TYPE_0_DRP_PRI_1_P_Nf, val64); 
                break;
            case bcmFabricLinkTxPrio2Drop: 
                rc = soc_reg64_field_validate(unit, DCL_TYPE_0_DRP_PRI_Pr, TYPE_0_DRP_PRI_2_P_Nf, val64); 
                break;
            case bcmFabricLinkTxPrio3Drop: 
                rc = soc_reg64_field_validate(unit, DCL_TYPE_0_DRP_PRI_Pr, TYPE_0_DRP_PRI_3_P_Nf, val64); 
                break;
            default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
        }
    }
    else
    {     
        switch(type)
        {
            case bcmFabricLinkTxPrio0Drop: 
                rc = soc_reg64_field_validate(unit, DCL_TYPE_1_DRP_PRI_Pr, TYPE_1_DRP_PRI_0_P_Nf, val64); 
                break;
            case bcmFabricLinkTxPrio1Drop: 
                rc = soc_reg64_field_validate(unit, DCL_TYPE_1_DRP_PRI_Pr, TYPE_1_DRP_PRI_1_P_Nf, val64); 
                break;
            case bcmFabricLinkTxPrio2Drop: 
                rc = soc_reg64_field_validate(unit, DCL_TYPE_1_DRP_PRI_Pr, TYPE_1_DRP_PRI_2_P_Nf, val64); 
                break;
            case bcmFabricLinkTxPrio3Drop: 
                rc = soc_reg64_field_validate(unit, DCL_TYPE_1_DRP_PRI_Pr, TYPE_1_DRP_PRI_3_P_Nf, val64); 
                break;
            default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
        }
    }
    
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid threshold for type %d"),value, type));
    }
    
      
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_rx_llfc_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value)
{
    int min_idx, max_idx,idx,nof_instances_dch;
    uint64 reg_val64;
    SOCDNX_INIT_FUNC_DEFS;

    nof_instances_dch = SOC_DFE_DEFS_GET(unit, nof_instances_dch);
    
    if(fe1 && fe3)
        {min_idx = 0; max_idx = nof_instances_dch - 1;}
    else if(fe1)
        {min_idx = 0; max_idx = 1;}
    else
        {min_idx = 2; max_idx = 3;}

   
    
    
    for(idx = min_idx ; idx <= max_idx ; idx++) {

        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_0r(unit, idx, &reg_val64));
            
            if(type_index == soc_dfe_fabric_link_fifo_type_index_0)
                soc_reg64_field32_set(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_0r,&reg_val64,LNK_LVL_FC_TH_0_P_0f,value);
            else
                soc_reg64_field32_set(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_0r,&reg_val64,LNK_LVL_FC_TH_1_P_0f,value); 
           
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_P_0r(unit, idx, reg_val64));
        }
        
        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_1r(unit, idx, &reg_val64));
            
            if(type_index == soc_dfe_fabric_link_fifo_type_index_0)
                soc_reg64_field32_set(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_1r,&reg_val64,LNK_LVL_FC_TH_0_P_1f,value);
            else
                soc_reg64_field32_set(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_1r,&reg_val64,LNK_LVL_FC_TH_1_P_1f,value); 
           
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_P_1r(unit, idx, reg_val64));
        }

        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,2))
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_2r(unit, idx, &reg_val64));
            
            if(type_index == soc_dfe_fabric_link_fifo_type_index_0)
                soc_reg64_field32_set(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_2r,&reg_val64,LNK_LVL_FC_TH_0_P_2f,value);
            else
                soc_reg64_field32_set(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_2r,&reg_val64,LNK_LVL_FC_TH_1_P_2f,value); 
           
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_P_2r(unit, idx, reg_val64));
        }

    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_tx_bypass_llfc_threshold_set(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value)
{
    int min_idx, max_idx,idx,nof_instances_dch, min_idx_dcmb, max_idx_dcmb;
    uint64 dcl_reg_val64, dcl_dcmb_reg_val64;
    SOCDNX_INIT_FUNC_DEFS;
    
    nof_instances_dch = SOC_DFE_DEFS_GET(unit, nof_instances_dch);
    
    if(fe1 && fe3)
        {min_idx = 0; max_idx = nof_instances_dch - 1;}
    else if(fe1)
        {min_idx = 2; max_idx = 3;}
    else
        {min_idx = 0; max_idx = 1;}
    
    
    
    if(SOC_DFE_IS_FE13_ASYMMETRIC(unit) && fe3)
    {
        min_idx_dcmb = 0;
        max_idx_dcmb = 3;
    } else {
        min_idx_dcmb = 0;
        max_idx_dcmb = -1;
    }
    
    
     
    for(idx = min_idx ; idx <= max_idx ; idx++)  {
      
        SOCDNX_IF_ERR_EXIT(READ_DCL_DCL_LLFC_THr(unit, idx, &dcl_reg_val64));
        
        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
        {
            if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                soc_reg64_field32_set(unit, DCL_DCL_LLFC_THr, &dcl_reg_val64, P_0_LLFC_TH_TYPE_0f, value);
            }
            else
            {
                soc_reg64_field32_set(unit, DCL_DCL_LLFC_THr, &dcl_reg_val64, P_0_LLFC_TH_TYPE_1f, value);
            }
        }
        
        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
        {
            if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                soc_reg64_field32_set(unit, DCL_DCL_LLFC_THr, &dcl_reg_val64, P_1_LLFC_TH_TYPE_0f, value);
            }
            else
            {
                soc_reg64_field32_set(unit, DCL_DCL_LLFC_THr, &dcl_reg_val64, P_1_LLFC_TH_TYPE_1f, value);
            }
        }

        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,2))
        {
            if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                soc_reg64_field32_set(unit, DCL_DCL_LLFC_THr, &dcl_reg_val64, P_2_LLFC_TH_TYPE_0f, value);
            }
            else
            {
                soc_reg64_field32_set(unit, DCL_DCL_LLFC_THr, &dcl_reg_val64, P_2_LLFC_TH_TYPE_1f, value);
            }
        }
     
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_DCL_LLFC_THr(unit, idx, dcl_reg_val64));
    }
    
    for(idx = min_idx_dcmb ; idx <= max_idx_dcmb ; idx++)  {
      
        SOCDNX_IF_ERR_EXIT(READ_DCL_DCMB_DCL_LLFC_THr(unit, idx, &dcl_dcmb_reg_val64));
        
        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
        {
            if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                soc_reg64_field32_set(unit, DCL_DCMB_DCL_LLFC_THr, &dcl_dcmb_reg_val64, DCMB_P_0_LLFC_TH_TYPE_0f, value);
            }
            else
            {
                soc_reg64_field32_set(unit, DCL_DCMB_DCL_LLFC_THr, &dcl_dcmb_reg_val64, DCMB_P_0_LLFC_TH_TYPE_1f, value);
            }
        }
        
        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
        {
            if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                soc_reg64_field32_set(unit, DCL_DCMB_DCL_LLFC_THr, &dcl_dcmb_reg_val64, DCMB_P_1_LLFC_TH_TYPE_0f, value);
            }
            else
            {
                soc_reg64_field32_set(unit, DCL_DCMB_DCL_LLFC_THr, &dcl_dcmb_reg_val64, DCMB_P_1_LLFC_TH_TYPE_1f, value);
            }
        }

        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,2))
        {
            if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                soc_reg64_field32_set(unit, DCL_DCMB_DCL_LLFC_THr, &dcl_dcmb_reg_val64, DCMB_P_2_LLFC_TH_TYPE_0f, value);
            }
            else
            {
                soc_reg64_field32_set(unit, DCL_DCMB_DCL_LLFC_THr, &dcl_dcmb_reg_val64, DCMB_P_2_LLFC_TH_TYPE_1f, value);
            }
        }
     
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_DCMB_DCL_LLFC_THr(unit, idx, dcl_dcmb_reg_val64));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_tx_rci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value)
{
    int min_idx, max_idx,idx,nof_instances_dch,i, nof_pipes;
    int min_idx_dcmb, max_idx_dcmb;
    int set_low = 0, set_med = 0 , set_high = 0;
    uint32 dcl_reg_val, dcl_dcmb_reg_val;
    soc_dcmn_fabric_pipe_t pipe_lcl[1];
    SOCDNX_INIT_FUNC_DEFS;

    nof_instances_dch = SOC_DFE_DEFS_GET(unit, nof_instances_dch);
    
    if(fe1 && fe3)
        {min_idx = 0; max_idx = nof_instances_dch - 1;}
    else if(fe1)
        {min_idx = 2; max_idx = 3;}
    else
        {min_idx = 0; max_idx = 1;}

    
    
    if(SOC_DFE_IS_FE13_ASYMMETRIC(unit) && fe3)
    {
        min_idx_dcmb = 0;
        max_idx_dcmb = 3;
    } else {
        min_idx_dcmb = 0;
        max_idx_dcmb = -1;
    }

    nof_pipes=SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;

    switch (type)
    {
       case bcmFabricLinkRciFC:
           set_low = 1;
           set_med = 1;
           set_high = 1;
           break;
       case bcmFabricLinkTxRciLvl1FC:
           set_low = 1;
           break;
        case bcmFabricLinkTxRciLvl2FC:
            set_med = 1;
            break;
        case bcmFabricLinkTxRciLvl3FC:
            set_high = 1;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));

    }
     
    *pipe_lcl = pipe;
     
    
    for(idx = min_idx ; idx <= max_idx ; idx++) {
        for (i=0; i < nof_pipes; i++)
        {
            if (SOC_DCMN_FABRIC_PIPE_IS_SET(pipe_lcl, i))
            {
                if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
                {
                    
                    SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_0_RCI_TH_Pr(unit, idx, i, &dcl_reg_val));
                    if (set_low)
                    {
                        soc_reg_field_set(unit, DCL_TYPE_0_RCI_TH_Pr, &dcl_reg_val, RCI_TH_LOW_0_P_Nf, value);
                    }
                    if (set_med)
                    {
                        soc_reg_field_set(unit, DCL_TYPE_0_RCI_TH_Pr, &dcl_reg_val, RCI_TH_MED_0_P_Nf, value);
                    }
                    if (set_high)
                    {
                        soc_reg_field_set(unit, DCL_TYPE_0_RCI_TH_Pr, &dcl_reg_val, RCI_TH_HIGH_0_P_Nf, value);
                    }

                    SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_0_RCI_TH_Pr(unit, idx, i, dcl_reg_val));

                }
                else
                {
                    SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_1_RCI_TH_Pr(unit, idx, i, &dcl_reg_val));

                    if (set_low)
                    {
                        soc_reg_field_set(unit, DCL_TYPE_1_RCI_TH_Pr, &dcl_reg_val, RCI_TH_LOW_1_P_Nf, value);
                    }
                    if (set_med)
                    {
                        soc_reg_field_set(unit, DCL_TYPE_1_RCI_TH_Pr, &dcl_reg_val, RCI_TH_MED_1_P_Nf, value);
                    }
                    if (set_high)
                    {
                        soc_reg_field_set(unit, DCL_TYPE_1_RCI_TH_Pr, &dcl_reg_val, RCI_TH_HIGH_1_P_Nf, value);
                    }

                    SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_1_RCI_TH_Pr(unit, idx, i, dcl_reg_val));
                }
            }
        }
    }
    

    for (idx = min_idx_dcmb; idx <= max_idx_dcmb; idx++)
    {
        for (i=0; i < nof_pipes; i++)
        {
            if (SOC_DCMN_FABRIC_PIPE_IS_SET(pipe_lcl, i))
            {
                if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
                {
                    SOCDNX_IF_ERR_EXIT(READ_DCL_DCMB_TYPE_0_RCI_TH_Pr(unit, idx, i , &dcl_dcmb_reg_val));
                    if (set_low)
                    {
                        soc_reg_field_set(unit, DCL_DCMB_TYPE_0_RCI_TH_Pr, &dcl_dcmb_reg_val, DCMB_RCI_TH_LOW_0_P_Nf, value);
                    }
                    if (set_med)
                    {
                        soc_reg_field_set(unit, DCL_DCMB_TYPE_0_RCI_TH_Pr, &dcl_dcmb_reg_val, DCMB_RCI_TH_MED_0_P_Nf, value);
                    }
                    if (set_high)
                    {
                        soc_reg_field_set(unit, DCL_DCMB_TYPE_0_RCI_TH_Pr, &dcl_dcmb_reg_val, DCMB_RCI_TH_HIGH_0_P_Nf, value);
                    }
                    SOCDNX_IF_ERR_EXIT(WRITE_DCL_DCMB_TYPE_0_RCI_TH_Pr(unit, idx, i, dcl_dcmb_reg_val));
                   
                }
                else
                {
                    SOCDNX_IF_ERR_EXIT(READ_DCL_DCMB_TYPE_1_RCI_TH_Pr(unit, idx, i , &dcl_dcmb_reg_val));

                    if (set_low)
                    {
                        soc_reg_field_set(unit, DCL_DCMB_TYPE_1_RCI_TH_Pr, &dcl_dcmb_reg_val, DCMB_RCI_TH_LOW_1_P_Nf, value);
                    }
                    if (set_med)
                    {
                        soc_reg_field_set(unit, DCL_DCMB_TYPE_1_RCI_TH_Pr, &dcl_dcmb_reg_val, DCMB_RCI_TH_MED_1_P_Nf, value);
                    }
                    if (set_high)
                    {
                        soc_reg_field_set(unit, DCL_DCMB_TYPE_1_RCI_TH_Pr, &dcl_dcmb_reg_val, DCMB_RCI_TH_HIGH_1_P_Nf, value);
                    }

                    SOCDNX_IF_ERR_EXIT(WRITE_DCL_DCMB_TYPE_1_RCI_TH_Pr(unit, idx, i, dcl_dcmb_reg_val));
                }
            }
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_tx_gci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value)
{

    int min_idx, max_idx,idx,nof_instances_dch,i,nof_pipes, min_idx_dcmb, max_idx_dcmb;
    uint32 dcl_reg_val, dcl_dcmb_reg_val;
    soc_dcmn_fabric_pipe_t pipe_lcl[1];
    SOCDNX_INIT_FUNC_DEFS;
 
    nof_instances_dch = SOC_DFE_DEFS_GET(unit, nof_instances_dch);
    
    if(fe1 && fe3)
        {min_idx = 0; max_idx = nof_instances_dch - 1;}
    else if(fe1)
        {min_idx = 2; max_idx = 3;}
    else
        {min_idx = 0; max_idx = 1;}

    
    
    if(SOC_DFE_IS_FE13_ASYMMETRIC(unit) && fe3)
    {
        min_idx_dcmb = 0;
        max_idx_dcmb = 3;
    } else {
        min_idx_dcmb = 0;
        max_idx_dcmb = -1;
    }
    
    nof_pipes=SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;
    *pipe_lcl = pipe;

      
    
    for(idx = min_idx ; idx <= max_idx ; idx++) {
        for (i=0; i < nof_pipes ;i++)
        {
            if (SOC_DCMN_FABRIC_PIPE_IS_SET(pipe_lcl, i))
            {
                if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
                {
                    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DCL_TYPE_0_GCI_TH_Pr, idx, i, &dcl_reg_val));
                    
                    switch (type)
                    {
                        case bcmFabricLinkTxGciLvl1FC:
                            soc_reg_field_set(unit, DCL_TYPE_0_GCI_TH_Pr, &dcl_reg_val, GCI_TH_LOW_0_P_Nf, value);
                            break;
                        case bcmFabricLinkTxGciLvl2FC:
                            soc_reg_field_set(unit, DCL_TYPE_0_GCI_TH_Pr, &dcl_reg_val, GCI_TH_MED_0_P_Nf, value);
                            break;
                        case bcmFabricLinkTxGciLvl3FC:
                            soc_reg_field_set(unit, DCL_TYPE_0_GCI_TH_Pr, &dcl_reg_val, GCI_TH_HIGH_0_P_Nf, value);
                            break;
                        default:
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
                    }
                   
                    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DCL_TYPE_0_GCI_TH_Pr, idx, i, dcl_reg_val));
                }
                else
                {
                    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DCL_TYPE_1_GCI_TH_Pr, idx, i, &dcl_reg_val));
                    
                    switch (type)
                    {
                        case bcmFabricLinkTxGciLvl1FC:
                            soc_reg_field_set(unit, DCL_TYPE_1_GCI_TH_Pr, &dcl_reg_val, GCI_TH_LOW_1_P_Nf, value);
                            break;
                        case bcmFabricLinkTxGciLvl2FC:
                            soc_reg_field_set(unit, DCL_TYPE_1_GCI_TH_Pr, &dcl_reg_val, GCI_TH_MED_1_P_Nf, value);
                            break;
                        case bcmFabricLinkTxGciLvl3FC:
                            soc_reg_field_set(unit, DCL_TYPE_1_GCI_TH_Pr, &dcl_reg_val,  GCI_TH_HIGH_1_P_Nf, value);
                            break;
                        default:
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
                    }
                    
                    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DCL_TYPE_1_GCI_TH_Pr, idx, i, dcl_reg_val));
                }
            }
        }
    }

    
    for(idx = min_idx_dcmb ; idx <= max_idx_dcmb ; idx++) {
        for (i=0; i < nof_pipes ;i++)
        {
            if (SOC_DCMN_FABRIC_PIPE_IS_SET(pipe_lcl, i))
            {
                if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
                {
                    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DCL_DCMB_TYPE_0_GCI_TH_Pr, idx, i, &dcl_dcmb_reg_val));
                    
                    switch (type)
                    {
                        case bcmFabricLinkTxGciLvl1FC:
                            soc_reg_field_set(unit, DCL_DCMB_TYPE_0_GCI_TH_Pr, &dcl_dcmb_reg_val, DCMB_GCI_TH_LOW_0_P_Nf, value);
                            break;
                        case bcmFabricLinkTxGciLvl2FC:
                            soc_reg_field_set(unit, DCL_DCMB_TYPE_0_GCI_TH_Pr, &dcl_dcmb_reg_val, DCMB_GCI_TH_MED_0_P_Nf, value);
                            break;
                        case bcmFabricLinkTxGciLvl3FC:
                            soc_reg_field_set(unit, DCL_DCMB_TYPE_0_GCI_TH_Pr, &dcl_dcmb_reg_val, DCMB_GCI_TH_HIGH_0_P_Nf, value);
                            break;
                        default:
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
                    }
                   
                    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DCL_DCMB_TYPE_0_GCI_TH_Pr, idx, i, dcl_dcmb_reg_val));

                }
                else
                {
                    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DCL_DCMB_TYPE_1_GCI_TH_Pr, idx, i, &dcl_dcmb_reg_val));
                    
                    switch (type)
                    {
                        case bcmFabricLinkTxGciLvl1FC:
                            soc_reg_field_set(unit, DCL_DCMB_TYPE_1_GCI_TH_Pr, &dcl_dcmb_reg_val, DCMB_GCI_TH_LOW_1_P_Nf, value);
                            break;
                        case bcmFabricLinkTxGciLvl2FC:
                            soc_reg_field_set(unit, DCL_DCMB_TYPE_1_GCI_TH_Pr, &dcl_dcmb_reg_val, DCMB_GCI_TH_MED_1_P_Nf, value);
                            break;
                        case bcmFabricLinkTxGciLvl3FC:
                            soc_reg_field_set(unit, DCL_DCMB_TYPE_1_GCI_TH_Pr, &dcl_dcmb_reg_val, DCMB_GCI_TH_HIGH_1_P_Nf, value);
                            break;
                        default:
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
                    }
                    
                    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DCL_DCMB_TYPE_1_GCI_TH_Pr, idx, i, dcl_dcmb_reg_val));
                }
            }
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
   
}


soc_error_t soc_fe3200_fabric_flow_control_rx_gci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value)
{
    int min_idx, max_idx,idx,nof_instances_dch,i,nof_pipes;
    uint32 reg_val;
    soc_dcmn_fabric_pipe_t pipe_lcl[1];
    SOCDNX_INIT_FUNC_DEFS;
 
    nof_instances_dch = SOC_DFE_DEFS_GET(unit, nof_instances_dch);
    
    if(fe1 && fe3)
        {min_idx = 0; max_idx = nof_instances_dch - 1;}
    else if(fe1)
        {min_idx = 0; max_idx = 1;}
    else
        {min_idx = 2; max_idx = 3;}

    nof_pipes=SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;
    *pipe_lcl = pipe;
    
     
    for(idx = min_idx ; idx <= max_idx ; idx++) {
        for (i=0;i<nof_pipes;i++)
        {
            if (SOC_DCMN_FABRIC_PIPE_IS_SET(pipe_lcl, i))
            {
                if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
                {
                    SOCDNX_IF_ERR_EXIT(READ_DCH_LOCAL_GCI_RCI_TYPE_0_Pr(unit, idx, i, &reg_val)); 
                    switch (type)
                    {
                        case bcmFabricLinkRxGciLvl1FC:
                            soc_reg_field_set(unit, DCH_LOCAL_GCI_RCI_TYPE_0_Pr, &reg_val, LOCAL_GCI_RCI_0_TYPE_0_TH_P_Nf, value);
                            break;
                        case bcmFabricLinkRxGciLvl2FC:
                            soc_reg_field_set(unit, DCH_LOCAL_GCI_RCI_TYPE_0_Pr, &reg_val, LOCAL_GCI_RCI_1_TYPE_0_TH_P_Nf, value);
                            break;
                        case bcmFabricLinkRxGciLvl3FC:
                            soc_reg_field_set(unit, DCH_LOCAL_GCI_RCI_TYPE_0_Pr, &reg_val, LOCAL_GCI_RCI_2_TYPE_0_TH_P_Nf, value);
                            break;
                        default:
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
                    }
                    SOCDNX_IF_ERR_EXIT(WRITE_DCH_LOCAL_GCI_RCI_TYPE_0_Pr(unit, idx, i, reg_val));

                }
                else
                {
                    SOCDNX_IF_ERR_EXIT(READ_DCH_LOCAL_GCI_RCI_TYPE_1_Pr(unit, idx, i, &reg_val));
                    switch (type)
                    {
                        case bcmFabricLinkRxGciLvl1FC:
                            soc_reg_field_set(unit, DCH_LOCAL_GCI_RCI_TYPE_1_Pr, &reg_val, LOCAL_GCI_RCI_0_TYPE_1_TH_P_Nf, value);
                            break;
                        case bcmFabricLinkRxGciLvl2FC:
                            soc_reg_field_set(unit, DCH_LOCAL_GCI_RCI_TYPE_1_Pr, &reg_val, LOCAL_GCI_RCI_1_TYPE_1_TH_P_Nf, value);
                            break;
                        case bcmFabricLinkRxGciLvl3FC:
                            soc_reg_field_set(unit, DCH_LOCAL_GCI_RCI_TYPE_1_Pr, &reg_val, LOCAL_GCI_RCI_2_TYPE_1_TH_P_Nf, value);
                            break;
                        default:
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
                    }
                    SOCDNX_IF_ERR_EXIT(WRITE_DCH_LOCAL_GCI_RCI_TYPE_1_Pr(unit, idx, i, reg_val));
                }
            }
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_rx_rci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value)
{
    int min_idx, max_idx,idx,nof_instances_dch,i,nof_pipes;
    uint32 reg_val;
    soc_dcmn_fabric_pipe_t pipe_lcl[1];
    SOCDNX_INIT_FUNC_DEFS;
 

    nof_instances_dch = SOC_DFE_DEFS_GET(unit, nof_instances_dch);
    
    if(fe1 && fe3)
        {min_idx = 0; max_idx = nof_instances_dch - 1;}
    else if(fe1)
        {min_idx = 0; max_idx = 1;}
    else
        {min_idx = 2; max_idx = 3;}

    nof_pipes=SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;

    *pipe_lcl = pipe;

    
    for(idx = min_idx ; idx <= max_idx ; idx++) {
        for (i=0;i<nof_pipes;i++)
        {
            if (SOC_DCMN_FABRIC_PIPE_IS_SET(pipe_lcl, i))
            {
                if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
                {
                    SOCDNX_IF_ERR_EXIT(READ_DCH_LOCAL_GCI_RCI_TYPE_0_Pr(unit, idx, i, &reg_val)); 
                    switch (type)
                    {
                        case bcmFabricLinkRxRciLvl1FC:
                            soc_reg_field_set(unit, DCH_LOCAL_GCI_RCI_TYPE_0_Pr, &reg_val, LOCAL_GCI_RCI_0_TYPE_0_TH_P_Nf, value);
                            break;
                        case bcmFabricLinkRxRciLvl2FC:
                            soc_reg_field_set(unit, DCH_LOCAL_GCI_RCI_TYPE_0_Pr, &reg_val, LOCAL_GCI_RCI_1_TYPE_0_TH_P_Nf, value);
                            break;
                        case bcmFabricLinkRxRciLvl3FC:
                            soc_reg_field_set(unit, DCH_LOCAL_GCI_RCI_TYPE_0_Pr, &reg_val, LOCAL_GCI_RCI_2_TYPE_0_TH_P_Nf, value);
                            break;
                        default:
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
                    }
                    SOCDNX_IF_ERR_EXIT(WRITE_DCH_LOCAL_GCI_RCI_TYPE_0_Pr(unit, idx, i, reg_val));

                }
                else
                {
                    SOCDNX_IF_ERR_EXIT(READ_DCH_LOCAL_GCI_RCI_TYPE_1_Pr(unit, idx, i, &reg_val));
                    switch (type)
                    {
                        case bcmFabricLinkRxRciLvl1FC:
                            soc_reg_field_set(unit, DCH_LOCAL_GCI_RCI_TYPE_1_Pr, &reg_val, LOCAL_GCI_RCI_0_TYPE_1_TH_P_Nf, value);
                            break;
                        case bcmFabricLinkRxRciLvl2FC:
                            soc_reg_field_set(unit, DCH_LOCAL_GCI_RCI_TYPE_1_Pr, &reg_val, LOCAL_GCI_RCI_1_TYPE_1_TH_P_Nf, value);
                            break;
                        case bcmFabricLinkRxRciLvl3FC:
                            soc_reg_field_set(unit, DCH_LOCAL_GCI_RCI_TYPE_1_Pr, &reg_val, LOCAL_GCI_RCI_2_TYPE_1_TH_P_Nf, value);
                            break;
                        default:
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
                    }
                    SOCDNX_IF_ERR_EXIT(WRITE_DCH_LOCAL_GCI_RCI_TYPE_1_Pr(unit, idx, i, reg_val));
                }
            }
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_tx_drop_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value)
{
    int min_idx, max_idx,idx,nof_instances_dch,i,nof_pipes, min_idx_dcmb, max_idx_dcmb;
    uint64 dcl_reg_val64, dcl_dcmb_reg_val64;
    soc_dcmn_fabric_pipe_t pipe_lcl[1];
    SOCDNX_INIT_FUNC_DEFS;

    nof_instances_dch = SOC_DFE_DEFS_GET(unit, nof_instances_dch);
    
    if(fe1 && fe3)
        {min_idx = 0; max_idx = nof_instances_dch - 1;}
    else if(fe1)
        {min_idx = 2; max_idx = 3;}
    else
        {min_idx = 0; max_idx = 1;}

    
    
    if(SOC_DFE_IS_FE13_ASYMMETRIC(unit) && fe3)
    {
        min_idx_dcmb = 0;
        max_idx_dcmb = 3;
    } else {
        min_idx_dcmb = 0;
        max_idx_dcmb = -1;
    }

    nof_pipes=SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;
    *pipe_lcl = pipe;
    
     
    
    for(idx = min_idx ; idx <= max_idx ; idx++) {
        for (i=0; i<nof_pipes ;i++)
        {
            if (SOC_DCMN_FABRIC_PIPE_IS_SET(pipe_lcl, i))
            {
                if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
                {
                    SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_0_DRP_PRI_Pr(unit, idx, i, &dcl_reg_val64)); 

                    switch (type)
                    {
                        case bcmFabricLinkTxPrio0Drop:
                            soc_reg64_field32_set(unit, DCL_TYPE_0_DRP_PRI_Pr, &dcl_reg_val64, TYPE_0_DRP_PRI_0_P_Nf, value);
                            break;
                        case bcmFabricLinkTxPrio1Drop:
                            soc_reg64_field32_set(unit, DCL_TYPE_0_DRP_PRI_Pr, &dcl_reg_val64, TYPE_0_DRP_PRI_1_P_Nf, value);
                            break;
                        case bcmFabricLinkTxPrio2Drop:
                            soc_reg64_field32_set(unit, DCL_TYPE_0_DRP_PRI_Pr, &dcl_reg_val64, TYPE_0_DRP_PRI_2_P_Nf, value);
                            break;
                        case bcmFabricLinkTxPrio3Drop:
                            soc_reg64_field32_set(unit, DCL_TYPE_0_DRP_PRI_Pr, &dcl_reg_val64, TYPE_0_DRP_PRI_3_P_Nf, value);
                            break;
                        default:
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
                    }
                    SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_0_DRP_PRI_Pr(unit, idx, i, dcl_reg_val64));

                }
                else
                {
                    SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_1_DRP_PRI_Pr(unit, idx, i, &dcl_reg_val64));

                    switch (type)
                    {
                        case bcmFabricLinkTxPrio0Drop:
                            soc_reg64_field32_set(unit, DCL_TYPE_1_DRP_PRI_Pr, &dcl_reg_val64, TYPE_1_DRP_PRI_0_P_Nf, value);
                            break;
                        case bcmFabricLinkTxPrio1Drop:
                            soc_reg64_field32_set(unit, DCL_TYPE_1_DRP_PRI_Pr, &dcl_reg_val64, TYPE_1_DRP_PRI_1_P_Nf, value);
                            break;
                        case bcmFabricLinkTxPrio2Drop:
                            soc_reg64_field32_set(unit, DCL_TYPE_1_DRP_PRI_Pr, &dcl_reg_val64, TYPE_1_DRP_PRI_2_P_Nf, value);
                            break;
                        case bcmFabricLinkTxPrio3Drop:
                            soc_reg64_field32_set(unit, DCL_TYPE_1_DRP_PRI_Pr, &dcl_reg_val64, TYPE_1_DRP_PRI_3_P_Nf, value);
                            break;
                        default:
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
                    }

                    SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_1_DRP_PRI_Pr(unit, idx, i, dcl_reg_val64));
                }
            }
        }
    }
    
    for (idx = min_idx_dcmb ; idx <= max_idx_dcmb ; idx++)
    {
        for (i=0; i<nof_pipes ;i++)
        {
            if (SOC_DCMN_FABRIC_PIPE_IS_SET(pipe_lcl, i))
            {
                if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
                {
                    SOCDNX_IF_ERR_EXIT(READ_DCL_DCMB_TYPE_0_DRP_PRI_Pr(unit, idx, i , &dcl_dcmb_reg_val64));
                    switch (type)
                    {
                        case bcmFabricLinkTxPrio0Drop:
                            soc_reg64_field32_set(unit, DCL_DCMB_TYPE_0_DRP_PRI_Pr, &dcl_dcmb_reg_val64, DCMB_TYPE_0_DRP_PRI_0_P_Nf, value);
                            break;
                        case bcmFabricLinkTxPrio1Drop:
                            soc_reg64_field32_set(unit, DCL_DCMB_TYPE_0_DRP_PRI_Pr, &dcl_dcmb_reg_val64, DCMB_TYPE_0_DRP_PRI_1_P_Nf, value);
                            break;
                        case bcmFabricLinkTxPrio2Drop:
                            soc_reg64_field32_set(unit, DCL_DCMB_TYPE_0_DRP_PRI_Pr, &dcl_dcmb_reg_val64, DCMB_TYPE_0_DRP_PRI_2_P_Nf, value);
                            break;
                        case bcmFabricLinkTxPrio3Drop:
                            soc_reg64_field32_set(unit, DCL_DCMB_TYPE_0_DRP_PRI_Pr, &dcl_dcmb_reg_val64, DCMB_TYPE_0_DRP_PRI_3_P_Nf, value);
                            break;
                        default:
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
                    }

                    SOCDNX_IF_ERR_EXIT(WRITE_DCL_DCMB_TYPE_0_DRP_PRI_Pr(unit, idx, i , dcl_dcmb_reg_val64));
                }
                else
                {
                    SOCDNX_IF_ERR_EXIT(READ_DCL_DCMB_TYPE_1_DRP_PRI_Pr(unit, idx, i, &dcl_dcmb_reg_val64));

                    switch (type)
                    {
                        case bcmFabricLinkTxPrio0Drop:
                            soc_reg64_field32_set(unit, DCL_DCMB_TYPE_1_DRP_PRI_Pr, &dcl_dcmb_reg_val64, DCMB_TYPE_1_DRP_PRI_0_P_Nf, value);
                            break;
                        case bcmFabricLinkTxPrio1Drop:
                            soc_reg64_field32_set(unit, DCL_DCMB_TYPE_1_DRP_PRI_Pr, &dcl_dcmb_reg_val64, DCMB_TYPE_1_DRP_PRI_1_P_Nf, value);
                            break;
                        case bcmFabricLinkTxPrio2Drop:
                            soc_reg64_field32_set(unit, DCL_DCMB_TYPE_1_DRP_PRI_Pr, &dcl_dcmb_reg_val64, DCMB_TYPE_1_DRP_PRI_2_P_Nf, value);
                            break;
                        case bcmFabricLinkTxPrio3Drop:
                            soc_reg64_field32_set(unit, DCL_DCMB_TYPE_1_DRP_PRI_Pr, &dcl_dcmb_reg_val64, DCMB_TYPE_1_DRP_PRI_3_P_Nf, value);
                            break;
                        default:
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
                    }

                    SOCDNX_IF_ERR_EXIT(WRITE_DCL_DCMB_TYPE_1_DRP_PRI_Pr(unit, idx, i , dcl_dcmb_reg_val64));
                }
            }
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_rx_llfc_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value)
{
    int blk_id;
    uint64 reg_val64;
    SOCDNX_INIT_FUNC_DEFS;
    
    
    if(fe1)
        blk_id = 0;
    else
        blk_id = 2;

    
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_0r(unit, blk_id, &reg_val64));
        
        if(type_index == soc_dfe_fabric_link_fifo_type_index_0)
            *value = soc_reg64_field32_get(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_0r,reg_val64,LNK_LVL_FC_TH_0_P_0f);
        else
            *value = soc_reg64_field32_get(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_0r,reg_val64,LNK_LVL_FC_TH_1_P_0f); 
    }

    else if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_1r(unit, blk_id, &reg_val64));
        
        if(type_index == soc_dfe_fabric_link_fifo_type_index_0)
            *value = soc_reg64_field32_get(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_1r,reg_val64,LNK_LVL_FC_TH_0_P_1f);
        else
            *value = soc_reg64_field32_get(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_1r,reg_val64,LNK_LVL_FC_TH_1_P_1f); 
    }

    else
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_2r(unit, blk_id, &reg_val64));
        
        if(type_index == soc_dfe_fabric_link_fifo_type_index_0)
            *value = soc_reg64_field32_get(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_2r,reg_val64,LNK_LVL_FC_TH_0_P_2f);
        else
            *value = soc_reg64_field32_get(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_2r,reg_val64,LNK_LVL_FC_TH_1_P_2f); 
    }
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_tx_bypass_llfc_threshold_get(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value)
{
    int blk_id;
    uint64 reg_val64;
    SOCDNX_INIT_FUNC_DEFS;


    
    if(fe1)
        blk_id = 2;
    else
        blk_id = 0;

    SOCDNX_IF_ERR_EXIT(READ_DCL_DCL_LLFC_THr(unit, blk_id, &reg_val64));

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            *value = soc_reg64_field32_get(unit, DCL_DCL_LLFC_THr, reg_val64, P_0_LLFC_TH_TYPE_0f);
        else
            *value = soc_reg64_field32_get(unit, DCL_DCL_LLFC_THr, reg_val64, P_0_LLFC_TH_TYPE_1f); 
    }

    else if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            *value = soc_reg64_field32_get(unit, DCL_DCL_LLFC_THr, reg_val64, P_1_LLFC_TH_TYPE_0f);
        else
            *value = soc_reg64_field32_get(unit, DCL_DCL_LLFC_THr, reg_val64, P_1_LLFC_TH_TYPE_1f); 
    }

    else
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            *value = soc_reg64_field32_get(unit, DCL_DCL_LLFC_THr, reg_val64, P_2_LLFC_TH_TYPE_0f);
        else
            *value = soc_reg64_field32_get(unit, DCL_DCL_LLFC_THr, reg_val64, P_2_LLFC_TH_TYPE_1f); 
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_tx_rci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value)
{
    int blk_id, nof_pipes, i ;
    uint32 reg_val;
    soc_dcmn_fabric_pipe_t pipe_lcl[1];
    SOCDNX_INIT_FUNC_DEFS;

    nof_pipes=SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;
    *pipe_lcl = pipe;
    
    
    if(fe1)
        blk_id = 2;
    else
        blk_id = 0;
    for (i=0 ; i < nof_pipes; i++)
    {
        if(SOC_DCMN_FABRIC_PIPE_IS_SET(pipe_lcl, i))
        {
            if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_0_RCI_TH_Pr(unit, blk_id, i,&reg_val));
                switch (type)
                {
                    case bcmFabricLinkRciFC:
                    case bcmFabricLinkTxRciLvl1FC:
                        *value = soc_reg_field_get(unit, DCL_TYPE_0_RCI_TH_Pr, reg_val, RCI_TH_LOW_0_P_Nf);
                        break;
                    case bcmFabricLinkTxRciLvl2FC:
                        *value = soc_reg_field_get(unit, DCL_TYPE_0_RCI_TH_Pr, reg_val, RCI_TH_MED_0_P_Nf);
                        break;
                    case bcmFabricLinkTxRciLvl3FC:
                        *value = soc_reg_field_get(unit, DCL_TYPE_0_RCI_TH_Pr, reg_val, RCI_TH_HIGH_0_P_Nf);
                        break;
                    default:
                        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
                }
            }
            else
            {
                SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_1_RCI_TH_Pr(unit, blk_id, i,&reg_val));
                switch (type)
                {
                        case bcmFabricLinkRciFC:
                        case bcmFabricLinkTxRciLvl1FC:
                            *value = soc_reg_field_get(unit, DCL_TYPE_1_RCI_TH_Pr, reg_val, RCI_TH_LOW_1_P_Nf);
                            break;
                        case bcmFabricLinkTxRciLvl2FC:
                            *value = soc_reg_field_get(unit, DCL_TYPE_1_RCI_TH_Pr, reg_val, RCI_TH_MED_1_P_Nf);
                            break;
                        case bcmFabricLinkTxRciLvl3FC:
                            *value = soc_reg_field_get(unit, DCL_TYPE_1_RCI_TH_Pr, reg_val, RCI_TH_HIGH_1_P_Nf);
                            break;
                        default:
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
                }
            }
        break; 
        }
    }

exit:
    SOCDNX_FUNC_RETURN;

}

soc_error_t soc_fe3200_fabric_flow_control_tx_gci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value)
{
    int blk_id;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    
    if(fe3)
        blk_id = 0;
    else
        blk_id = 2;

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index) {
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DCL_TYPE_0_GCI_TH_Pr, blk_id, 0, &reg_val));
 
        } else {
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DCL_TYPE_1_GCI_TH_Pr, blk_id, 0, &reg_val));
        }
    }
    else if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index) {
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DCL_TYPE_0_GCI_TH_Pr, blk_id, 1, &reg_val));
        } else {
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DCL_TYPE_1_GCI_TH_Pr, blk_id, 1, &reg_val));
        }
    }
    else
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index) {
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DCL_TYPE_0_GCI_TH_Pr, blk_id, 2, &reg_val));
        } else {
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DCL_TYPE_1_GCI_TH_Pr, blk_id, 2, &reg_val));
        }
    }

    switch (type)
    {
        case bcmFabricLinkTxGciLvl1FC:
            if (soc_dfe_fabric_link_fifo_type_index_0 == type_index) {
                *value = soc_reg_field_get(unit, DCL_TYPE_0_GCI_TH_Pr, reg_val, GCI_TH_LOW_0_P_Nf);
            } else {
                *value = soc_reg_field_get(unit, DCL_TYPE_1_GCI_TH_Pr, reg_val, GCI_TH_LOW_1_P_Nf);
            }
            break;
        case bcmFabricLinkTxGciLvl2FC:
            if (soc_dfe_fabric_link_fifo_type_index_0 == type_index) {
                *value = soc_reg_field_get(unit, DCL_TYPE_0_GCI_TH_Pr, reg_val, GCI_TH_MED_0_P_Nf); 
            } else {
                *value = soc_reg_field_get(unit, DCL_TYPE_1_GCI_TH_Pr, reg_val, GCI_TH_MED_1_P_Nf);
            }
            break;
        case bcmFabricLinkTxGciLvl3FC:
            if (soc_dfe_fabric_link_fifo_type_index_0 == type_index) {
                *value = soc_reg_field_get(unit, DCL_TYPE_0_GCI_TH_Pr, reg_val, GCI_TH_HIGH_0_P_Nf);
            } else {
                *value = soc_reg_field_get(unit, DCL_TYPE_1_GCI_TH_Pr, reg_val, GCI_TH_HIGH_1_P_Nf);
            }
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));

    }   


exit:
    SOCDNX_FUNC_RETURN;

}

soc_error_t soc_fe3200_fabric_flow_control_rx_gci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value)
{
    int blk_id;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    
    if(fe1)
        blk_id = 0;
    else
        blk_id = 2;

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LOCAL_GCI_RCI_TYPE_0_Pr(unit, blk_id, 0, &reg_val));

        }
        else
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LOCAL_GCI_RCI_TYPE_1_Pr(unit, blk_id, 0, &reg_val));
        }
      
    }
    else if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LOCAL_GCI_RCI_TYPE_0_Pr(unit, blk_id, 1, &reg_val)); 
        }
        else
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LOCAL_GCI_RCI_TYPE_1_Pr(unit, blk_id, 1, &reg_val));

        }
    }
    else
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LOCAL_GCI_RCI_TYPE_0_Pr(unit, blk_id, 2, &reg_val)); 
        }
        else
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LOCAL_GCI_RCI_TYPE_1_Pr(unit, blk_id, 2, &reg_val));

        }

    }

    switch (type)
    {
        case bcmFabricLinkRxGciLvl1FC:
            if (soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                *value = soc_reg_field_get(unit, DCH_LOCAL_GCI_RCI_TYPE_0_Pr, reg_val, LOCAL_GCI_RCI_0_TYPE_0_TH_P_Nf);
            }
            else
            {
                *value = soc_reg_field_get(unit, DCH_LOCAL_GCI_RCI_TYPE_1_Pr, reg_val, LOCAL_GCI_RCI_0_TYPE_1_TH_P_Nf);
            }
            break;
        case bcmFabricLinkRxGciLvl2FC:
            if (soc_dfe_fabric_link_fifo_type_index_0 == type_index) 
            {
                *value = soc_reg_field_get(unit, DCH_LOCAL_GCI_RCI_TYPE_0_Pr, reg_val, LOCAL_GCI_RCI_1_TYPE_0_TH_P_Nf);
            }
            else
            {
                *value = soc_reg_field_get(unit, DCH_LOCAL_GCI_RCI_TYPE_1_Pr, reg_val, LOCAL_GCI_RCI_1_TYPE_1_TH_P_Nf);
            }
            break;
        case bcmFabricLinkRxGciLvl3FC:
            if (soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                *value = soc_reg_field_get(unit, DCH_LOCAL_GCI_RCI_TYPE_0_Pr, reg_val, LOCAL_GCI_RCI_2_TYPE_0_TH_P_Nf);
            }
            else
            {
                *value = soc_reg_field_get(unit, DCH_LOCAL_GCI_RCI_TYPE_1_Pr, reg_val, LOCAL_GCI_RCI_2_TYPE_1_TH_P_Nf);
            }  
            break;
       default:
           SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
    } 
                
exit:
    SOCDNX_FUNC_RETURN;    
}

soc_error_t soc_fe3200_fabric_flow_control_rx_rci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value)
{
    int blk_id;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    
    if(fe1)
        blk_id = 0;
    else
        blk_id = 2;

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LOCAL_GCI_RCI_TYPE_0_Pr(unit, blk_id, 0, &reg_val));

        }
        else
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LOCAL_GCI_RCI_TYPE_1_Pr(unit, blk_id, 0, &reg_val));
        }
      
    }
    else if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LOCAL_GCI_RCI_TYPE_0_Pr(unit, blk_id, 1, &reg_val)); 
        }
        else
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LOCAL_GCI_RCI_TYPE_1_Pr(unit, blk_id, 1, &reg_val));

        }
    }
    else
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LOCAL_GCI_RCI_TYPE_0_Pr(unit, blk_id, 2, &reg_val)); 
        }
        else
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LOCAL_GCI_RCI_TYPE_1_Pr(unit, blk_id, 2, &reg_val));

        }

    }

    switch (type)
    {
        case bcmFabricLinkRxRciLvl1FC:
            if (soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                *value = soc_reg_field_get(unit, DCH_LOCAL_GCI_RCI_TYPE_0_Pr, reg_val, LOCAL_GCI_RCI_0_TYPE_0_TH_P_Nf);
            }
            else
            {
                *value = soc_reg_field_get(unit, DCH_LOCAL_GCI_RCI_TYPE_1_Pr, reg_val, LOCAL_GCI_RCI_0_TYPE_1_TH_P_Nf);
            }
            break;
        case bcmFabricLinkRxRciLvl2FC:
            if (soc_dfe_fabric_link_fifo_type_index_0 == type_index) 
            {
                *value = soc_reg_field_get(unit, DCH_LOCAL_GCI_RCI_TYPE_0_Pr, reg_val, LOCAL_GCI_RCI_1_TYPE_0_TH_P_Nf);
            }
            else
            {
                *value = soc_reg_field_get(unit, DCH_LOCAL_GCI_RCI_TYPE_1_Pr, reg_val, LOCAL_GCI_RCI_1_TYPE_1_TH_P_Nf);
            }
            break;
        case bcmFabricLinkRxRciLvl3FC:
            if (soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                *value = soc_reg_field_get(unit, DCH_LOCAL_GCI_RCI_TYPE_0_Pr, reg_val, LOCAL_GCI_RCI_2_TYPE_0_TH_P_Nf);
            }
            else
            {
                *value = soc_reg_field_get(unit, DCH_LOCAL_GCI_RCI_TYPE_1_Pr, reg_val, LOCAL_GCI_RCI_2_TYPE_1_TH_P_Nf);
            }  
            break;
       default:
           SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
    } 
                
exit:
    SOCDNX_FUNC_RETURN;    
}

soc_error_t soc_fe3200_fabric_flow_control_tx_drop_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value)
{
    int blk_id;
    uint64 reg_val64;

    SOCDNX_INIT_FUNC_DEFS;  

    if(fe3)
        blk_id = 0;
    else 
        blk_id = 2;

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_0_DRP_PRI_Pr(unit, blk_id, 0, &reg_val64));

        }
        else
        {
            SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_1_DRP_PRI_Pr(unit, blk_id, 0, &reg_val64));
        }
      
    }
    else if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_0_DRP_PRI_Pr(unit, blk_id, 1, &reg_val64)); 
        }
        else
        {
            SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_1_DRP_PRI_Pr(unit, blk_id, 1, &reg_val64));

        }
    }
    else
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_0_DRP_PRI_Pr(unit, blk_id, 2, &reg_val64)); 
        }
        else
        {
            SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_1_DRP_PRI_Pr(unit, blk_id, 2, &reg_val64));

        }

    }

    switch (type)
    {
        case bcmFabricLinkTxPrio0Drop:
            if (soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                *value = soc_reg64_field32_get(unit, DCL_TYPE_0_DRP_PRI_Pr, reg_val64, TYPE_0_DRP_PRI_0_P_Nf);
            }
            else
            {
                *value = soc_reg64_field32_get(unit, DCL_TYPE_1_DRP_PRI_Pr, reg_val64, TYPE_1_DRP_PRI_0_P_Nf);
            }
            break;
        case bcmFabricLinkTxPrio1Drop:
            if (soc_dfe_fabric_link_fifo_type_index_0 == type_index) 
            {
                *value = soc_reg64_field32_get(unit, DCL_TYPE_0_DRP_PRI_Pr, reg_val64, TYPE_0_DRP_PRI_1_P_Nf);
            }
            else
            {
                *value = soc_reg64_field32_get(unit, DCL_TYPE_1_DRP_PRI_Pr, reg_val64, TYPE_1_DRP_PRI_1_P_Nf);
            }
            break;
        case bcmFabricLinkTxPrio2Drop:
            if (soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                *value = soc_reg64_field32_get(unit, DCL_TYPE_0_DRP_PRI_Pr, reg_val64, TYPE_0_DRP_PRI_2_P_Nf);
            }
            else
            {
                *value = soc_reg64_field32_get(unit, DCL_TYPE_1_DRP_PRI_Pr, reg_val64, TYPE_1_DRP_PRI_2_P_Nf);
            }  
            break;
        case bcmFabricLinkTxPrio3Drop:
            if (soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                *value = soc_reg64_field32_get(unit, DCL_TYPE_0_DRP_PRI_Pr, reg_val64, TYPE_0_DRP_PRI_3_P_Nf);
            }
            else
            {
                *value = soc_reg64_field32_get(unit, DCL_TYPE_1_DRP_PRI_Pr, reg_val64, TYPE_1_DRP_PRI_3_P_Nf);
            }  
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
    } 
exit:
    SOCDNX_FUNC_RETURN;   

}


soc_error_t soc_fe3200_fabric_flow_control_rx_rci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value, int is_fe1, int is_fe3)
{
    soc_error_t rc = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;

    
    if(SOC_DFE_IS_FE13(unit))
    {
        if (is_fe1 && !is_fe3)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("rx rci unavailable for fe1 links only")));
        }
    }
    
    if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
    {
        switch(type)
        {
            case bcmFabricLinkRxRciLvl1FC: 
                rc = soc_reg_field_validate(unit, DCH_LOCAL_GCI_RCI_TYPE_0_Pr, LOCAL_GCI_RCI_0_TYPE_0_TH_P_Nf, value); 
                break;
            case bcmFabricLinkRxRciLvl2FC: 
                rc = soc_reg_field_validate(unit, DCH_LOCAL_GCI_RCI_TYPE_0_Pr, LOCAL_GCI_RCI_1_TYPE_0_TH_P_Nf, value); 
                break;
            case bcmFabricLinkRxRciLvl3FC: 
                rc = soc_reg_field_validate(unit, DCH_LOCAL_GCI_RCI_TYPE_0_Pr, LOCAL_GCI_RCI_2_TYPE_0_TH_P_Nf, value); 
                break;
            default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
        }
    }
    else
    {     
        switch(type)
        {
            case bcmFabricLinkRxRciLvl1FC: 
                rc = soc_reg_field_validate(unit, DCH_LOCAL_GCI_RCI_TYPE_1_Pr, LOCAL_GCI_RCI_0_TYPE_1_TH_P_Nf, value);
                break;
            case bcmFabricLinkRxRciLvl2FC: 
                rc = soc_reg_field_validate(unit, DCH_LOCAL_GCI_RCI_TYPE_1_Pr, LOCAL_GCI_RCI_1_TYPE_1_TH_P_Nf, value); 
                break;
            case bcmFabricLinkRxRciLvl3FC: 
                rc = soc_reg_field_validate(unit, DCH_LOCAL_GCI_RCI_TYPE_1_Pr, LOCAL_GCI_RCI_2_TYPE_1_TH_P_Nf, value); 
                break;
            default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
        }
    }
    
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for RX RCI threshold"),value)); 
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_mid_full_threshold_validate(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value)
{
    uint64 field_val64;
    soc_error_t rc = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_64_SET(field_val64, 0, value);

    if (soc_dfe_fabric_link_fifo_type_index_0 == type_index)
    {
        rc = soc_reg64_field_validate(unit, DCM_FIFOS_THRESHOLDS_P_TYPE_0r, DCM_FULL_TH_P_N_TYPE_0f, field_val64);
        if(SOC_FAILURE(rc)) {
            SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for mid full threshold"),value)); 
        }
    }
    else
    {
        rc = soc_reg64_field_validate(unit, DCM_FIFOS_THRESHOLDS_P_TYPE_1r, DCM_FULL_TH_P_N_TYPE_1f, field_val64);
        if(SOC_FAILURE(rc)) {
            SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for mid full threshold"),value)); 
        }
    }


exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_mid_full_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value)
{
    uint64 reg_val64;
    int idx;
    int i;
    int nof_pipes;
    soc_dcmn_fabric_pipe_t pipe_lcl[1];
    int dcm_instance_start, dcm_instance_end;
    SOCDNX_INIT_FUNC_DEFS;

    dcm_instance_start = is_fe1 ? 0 : SOC_DFE_DEFS_GET(unit, nof_instances_dcm)/2;
    dcm_instance_end = is_fe3 ?  SOC_DFE_DEFS_GET(unit, nof_instances_dcm) : SOC_DFE_DEFS_GET(unit, nof_instances_dcm)/2;


    nof_pipes=SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;

    *pipe_lcl = pipe;
    for (idx = dcm_instance_start ; idx < dcm_instance_end; idx++)
    {
        for (i = 0 ; i < nof_pipes; i++)
        {
            if (SOC_DCMN_FABRIC_PIPE_IS_SET(pipe_lcl, i))
            {
                if (soc_dfe_fabric_link_fifo_type_index_0 == type_index)
                {
                    SOCDNX_IF_ERR_EXIT(READ_DCM_FIFOS_THRESHOLDS_P_TYPE_0r(unit, idx, i, &reg_val64));
                    soc_reg64_field32_set(unit, DCM_FIFOS_THRESHOLDS_P_TYPE_0r, &reg_val64, DCM_FULL_TH_P_N_TYPE_0f, value);
                    SOCDNX_IF_ERR_EXIT(WRITE_DCM_FIFOS_THRESHOLDS_P_TYPE_0r(unit, idx, i, reg_val64));
                }
                else
                {
                    SOCDNX_IF_ERR_EXIT(READ_DCM_FIFOS_THRESHOLDS_P_TYPE_1r(unit, idx, i, &reg_val64));
                    soc_reg64_field32_set(unit, DCM_FIFOS_THRESHOLDS_P_TYPE_1r, &reg_val64, DCM_FULL_TH_P_N_TYPE_1f, value);
                    SOCDNX_IF_ERR_EXIT(WRITE_DCM_FIFOS_THRESHOLDS_P_TYPE_1r(unit, idx, i, reg_val64));
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_mid_full_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int *value)
{
    uint64 reg_val64;
    int i, nof_pipes;
    soc_dcmn_fabric_pipe_t pipe_lcl[1];
    int dcm_instance;
    SOCDNX_INIT_FUNC_DEFS;
    
    dcm_instance = is_fe1 ? 0 : SOC_DFE_DEFS_GET(unit, nof_instances_dcm)/2;
    nof_pipes=SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;
    
    *pipe_lcl = pipe;
    for (i = 0; i < nof_pipes ; i++)
    {
        if (SOC_DCMN_FABRIC_PIPE_IS_SET(pipe_lcl, i))
        {
            if (soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                SOCDNX_IF_ERR_EXIT(READ_DCM_FIFOS_THRESHOLDS_P_TYPE_0r(unit, dcm_instance, i, &reg_val64));
                *value = soc_reg64_field32_get(unit, DCM_FIFOS_THRESHOLDS_P_TYPE_0r, reg_val64, DCM_FULL_TH_P_N_TYPE_0f);
                break;
            }
            else
            {
                SOCDNX_IF_ERR_EXIT(READ_DCM_FIFOS_THRESHOLDS_P_TYPE_1r(unit, dcm_instance, i, &reg_val64));
                *value = soc_reg64_field32_get(unit, DCM_FIFOS_THRESHOLDS_P_TYPE_1r, reg_val64, DCM_FULL_TH_P_N_TYPE_1f);
                break;
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_rx_full_threshold_validate(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value)
{

    soc_error_t rc = SOC_E_NONE;
    uint64  val64;

    SOCDNX_INIT_FUNC_DEFS;
    COMPILER_64_SET(val64, 0, value);

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        rc = soc_reg64_field_validate(unit, DCH_LINK_LEVEL_FLOW_CONTROL_P_0r, IFM_FIFO_FULL_P_0f, val64);
    }
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for RX full threshold"),value)); 
    }

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {       
        rc = soc_reg64_field_validate(unit, DCH_LINK_LEVEL_FLOW_CONTROL_P_1r, IFM_FIFO_FULL_P_1f, val64);
    }
     if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for RX full threshold"),value)); 
    }
     
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,2))
    {
        rc = soc_reg64_field_validate(unit, DCH_LINK_LEVEL_FLOW_CONTROL_P_2r, IFM_FIFO_FULL_P_2f, val64);
    }
     if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for RX full threshold"),value)); 
    }       
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_rx_full_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value)
{
    int min_idx, max_idx,idx,nof_instances_dch;
    uint64 reg_val64;
    SOCDNX_INIT_FUNC_DEFS;
 
    nof_instances_dch = SOC_DFE_DEFS_GET(unit, nof_instances_dch);
    
    if(is_fe1 && is_fe3)
        {min_idx = 0; max_idx = nof_instances_dch - 1;}
    else if(is_fe1)
        {min_idx = 0; max_idx = 1;}
    else
        {min_idx = 2; max_idx = 3;}
   
     
    for(idx = min_idx ; idx <= max_idx ; idx++) {

        if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_0r(unit, idx, &reg_val64));
          
            soc_reg64_field32_set(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_0r,&reg_val64,IFM_FIFO_FULL_P_0f,value);

            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_P_0r(unit, idx, reg_val64));
        }

        if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_1r(unit, idx, &reg_val64));
          
            soc_reg64_field32_set(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_1r,&reg_val64,IFM_FIFO_FULL_P_1f,value);

            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_P_1r(unit, idx, reg_val64));
        }

        if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,2))
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_2r(unit, idx, &reg_val64));
          
            soc_reg64_field32_set(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_2r,&reg_val64,IFM_FIFO_FULL_P_2f,value);

            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_P_2r(unit, idx, reg_val64));
        }
           
    }
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_rx_full_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int* value)
{
    int blk_id;
    uint64 reg_val64;
    SOCDNX_INIT_FUNC_DEFS;
    
    
    if(is_fe1)
        blk_id = 0;
    else
        blk_id = 2;

    
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_0r(unit, blk_id, &reg_val64));

        *value = soc_reg64_field32_get(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_0r,reg_val64,IFM_FIFO_FULL_P_0f);

    }

    else if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_1r(unit, blk_id, &reg_val64));
        
        *value = soc_reg64_field32_get(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_1r,reg_val64,IFM_FIFO_FULL_P_1f);

    }

    else
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_2r(unit, blk_id, &reg_val64));
        
        *value = soc_reg64_field32_get(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_2r,reg_val64,IFM_FIFO_FULL_P_2f);

    }
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_rx_fifo_size_threshold_validate(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value)
{

    soc_error_t rc = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;
    
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        rc = soc_reg_field_validate(unit, DCH_FIFOS_DEPTH_TYPEr, FIFO_DEPTH_P_0_TYPE_Nf, value);
    }
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for RX fifo size threshold"),value)); 
    }

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {       
        rc = soc_reg_field_validate(unit, DCH_FIFOS_DEPTH_TYPEr , FIFO_DEPTH_P_1_TYPE_Nf, value);
    }
     if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for RX fifo size threshold"),value)); 
    }
     
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,2))
    {
        rc = soc_reg_field_validate(unit, DCH_FIFOS_DEPTH_TYPEr, FIFO_DEPTH_P_2_TYPE_Nf, value);
    }
     if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for RX fifo size threshold"),value)); 
    }       
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_rx_fifo_size_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value)
{
    int min_idx, max_idx,idx,nof_instances_dch, array_elem;
    uint32 reg_val;
    soc_error_t rc = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;
 
    nof_instances_dch = SOC_DFE_DEFS_GET(unit, nof_instances_dch);
    
    if(is_fe1 && is_fe3)
        {min_idx = 0; max_idx = nof_instances_dch - 1;}
    else if(is_fe1)
        {min_idx = 0; max_idx = 1;}
    else
        {min_idx = 2; max_idx = 3;}

    if (type_index == soc_dfe_fabric_link_fifo_type_index_0)
    {
        array_elem = 0;
    }
    else 
    {
        array_elem = 1;
    }

    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_soft_init, (unit, SOC_DCMN_RESET_ACTION_IN_RESET));
    SOCDNX_IF_ERR_EXIT(rc);
   
     
    for(idx = min_idx ; idx <= max_idx ; idx++) {

        SOCDNX_IF_ERR_EXIT(READ_DCH_FIFOS_DEPTH_TYPEr(unit, idx, array_elem, &reg_val));

        if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
        {
            
            soc_reg_field_set(unit,DCH_FIFOS_DEPTH_TYPEr,&reg_val,FIFO_DEPTH_P_0_TYPE_Nf,value);
           
        }

        if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
        {
                     
            soc_reg_field_set(unit,DCH_FIFOS_DEPTH_TYPEr,&reg_val,FIFO_DEPTH_P_1_TYPE_Nf,value);
           
        }

        if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,2))
        {
                    
            soc_reg_field_set(unit,DCH_FIFOS_DEPTH_TYPEr,&reg_val,FIFO_DEPTH_P_2_TYPE_Nf,value);
           
        }

        SOCDNX_IF_ERR_EXIT(WRITE_DCH_FIFOS_DEPTH_TYPEr(unit, idx, array_elem, reg_val));
           
    }


    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_soft_init, (unit, SOC_DCMN_RESET_ACTION_OUT_RESET));
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_rx_fifo_size_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int* value)
{
    int blk_id, array_elem;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;
    
    
    if(is_fe1)
        blk_id = 0;
    else
        blk_id = 2;

    if (type_index == soc_dfe_fabric_link_fifo_type_index_0)
    {
        array_elem = 0;
    }
    else 
    {
        array_elem = 1;
    }


    SOCDNX_IF_ERR_EXIT(READ_DCH_FIFOS_DEPTH_TYPEr(unit, blk_id, array_elem,&reg_val));
    
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {  
        *value = soc_reg_field_get(unit,DCH_FIFOS_DEPTH_TYPEr,reg_val,FIFO_DEPTH_P_0_TYPE_Nf);
    }
    else if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {   
        *value = soc_reg_field_get(unit,DCH_FIFOS_DEPTH_TYPEr,reg_val,FIFO_DEPTH_P_1_TYPE_Nf);
    }
    else
    {
        *value = soc_reg_field_get(unit,DCH_FIFOS_DEPTH_TYPEr,reg_val,FIFO_DEPTH_P_2_TYPE_Nf);
    }
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_rx_multicast_low_prio_drop_threshold_validate(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value)
{

    soc_error_t rc = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;
    
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        rc = soc_reg_field_validate(unit, DCH_LINK_LEVEL_FLOW_CONTROL_P_0r, LOW_PRI_CELL_DROP_TH_P_0f, value);
    }
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for RX multicast low priority drops threshold"),value)); 
    }

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {       
        rc = soc_reg_field_validate(unit, DCH_LINK_LEVEL_FLOW_CONTROL_P_1r , LOW_PRI_CELL_DROP_TH_P_1f, value);
    }
     if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for RX multicast low priority drops threshold"),value)); 
    }
     
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,2))
    {
        rc = soc_reg_field_validate(unit, DCH_LINK_LEVEL_FLOW_CONTROL_P_2r, LOW_PRI_CELL_DROP_TH_P_2f, value);
    }
     if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for RX multicast low priority drops threshold"),value)); 
    }       
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_rx_multicast_low_prio_drop_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value)
{
    int min_idx, max_idx,idx,nof_instances_dch;
    uint64 reg_val64;
    SOCDNX_INIT_FUNC_DEFS;
 
    nof_instances_dch = SOC_DFE_DEFS_GET(unit, nof_instances_dch);
    
    if(is_fe1 && is_fe3)
        {min_idx = 0; max_idx = nof_instances_dch - 1;}
    else if(is_fe1)
        {min_idx = 0; max_idx = 1;}
    else
        {min_idx = 2; max_idx = 3;}
   
     
    for(idx = min_idx ; idx <= max_idx ; idx++) {

        if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_0r(unit, idx, &reg_val64));
          
            soc_reg64_field32_set(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_0r,&reg_val64,LOW_PRI_CELL_DROP_TH_P_0f,value);

            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_P_0r(unit, idx, reg_val64));
        }

        if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_1r(unit, idx, &reg_val64));
          
            soc_reg64_field32_set(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_1r,&reg_val64,LOW_PRI_CELL_DROP_TH_P_1f,value);

            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_P_1r(unit, idx, reg_val64));
        }

        if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,2))
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_2r(unit, idx, &reg_val64));
          
            soc_reg64_field32_set(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_2r,&reg_val64,LOW_PRI_CELL_DROP_TH_P_2f,value);

            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_P_2r(unit, idx, reg_val64));
        }
           
    }
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t soc_fe3200_fabric_flow_control_rx_multicast_low_prio_drop_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int* value)
{
    int blk_id;
    uint64 reg_val64;
    SOCDNX_INIT_FUNC_DEFS;
    
    
    if(is_fe1)
        blk_id = 0;
    else
        blk_id = 2;

    
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_0r(unit, blk_id, &reg_val64));

        *value = soc_reg64_field32_get(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_0r,reg_val64,LOW_PRI_CELL_DROP_TH_P_0f);

    }

    else if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_1r(unit, blk_id, &reg_val64));
        
        *value = soc_reg64_field32_get(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_1r,reg_val64,LOW_PRI_CELL_DROP_TH_P_1f);

    }

    else
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_2r(unit, blk_id, &reg_val64));
        
        *value = soc_reg64_field32_get(unit,DCH_LINK_LEVEL_FLOW_CONTROL_P_2r,reg_val64,LOW_PRI_CELL_DROP_TH_P_2f);

    }
  
exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t soc_fe3200_fabric_flow_control_mid_gci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value)
{
    soc_error_t rc = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;
    
    if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
    {
        switch(type)
        {
            case bcmFabricLinkMidGciLvl1FC: 
                rc = soc_reg_field_validate(unit, DCM_GCI_TH_P_TYPE_0r, GCI_TH_LOW_P_N_TYPE_0f, value); 
                break;
            case bcmFabricLinkMidGciLvl2FC: 
                rc = soc_reg_field_validate(unit, DCM_GCI_TH_P_TYPE_0r, GCI_TH_MED_P_N_TYPE_0f, value); 
                break;
            case bcmFabricLinkMidGciLvl3FC: 
                rc = soc_reg_field_validate(unit, DCM_GCI_TH_P_TYPE_0r, GCI_TH_HIGH_P_N_TYPE_0f, value); 
                break;
            default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
        }
    }
    else
    {     
        switch(type)
        {
            case bcmFabricLinkMidGciLvl1FC: 
                rc = soc_reg_field_validate(unit, DCM_GCI_TH_P_TYPE_1r, GCI_TH_LOW_P_N_TYPE_1f, value);
                break;
            case bcmFabricLinkMidGciLvl2FC: 
                rc = soc_reg_field_validate(unit, DCM_GCI_TH_P_TYPE_1r, GCI_TH_MED_P_N_TYPE_1f, value); 
                break;
            case bcmFabricLinkMidGciLvl3FC: 
                rc = soc_reg_field_validate(unit, DCM_GCI_TH_P_TYPE_1r, GCI_TH_HIGH_P_N_TYPE_1f, value); 
                break;
            default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
        }
    }
    
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for mid GCI threshold"),value)); 
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t soc_fe3200_fabric_flow_control_mid_gci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value)
{
    int  idx, i, nof_pipes;
    uint64 reg_val64;
    soc_dcmn_fabric_pipe_t pipe_lcl[1];
    int dcm_instance_start, dcm_instance_end;
    SOCDNX_INIT_FUNC_DEFS;

    dcm_instance_start = is_fe1 ? 0 : SOC_DFE_DEFS_GET(unit, nof_instances_dcm)/2;
    dcm_instance_end = is_fe3 ?  SOC_DFE_DEFS_GET(unit, nof_instances_dcm) : SOC_DFE_DEFS_GET(unit, nof_instances_dcm)/2;

    nof_pipes=SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;
    *pipe_lcl = pipe;
    
     
    for(idx = dcm_instance_start ; idx < dcm_instance_end ; idx++) {
        for (i=0; i < nof_pipes; i++)
        {
            if (SOC_DCMN_FABRIC_PIPE_IS_SET(pipe_lcl, i))
            {
                if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
                {                    
                    SOCDNX_IF_ERR_EXIT(READ_DCM_GCI_TH_P_TYPE_0r(unit, idx, i, &reg_val64));

                    switch (type)
                    {
                        case bcmFabricLinkMidGciLvl1FC:
                            soc_reg64_field32_set(unit, DCM_GCI_TH_P_TYPE_0r, &reg_val64, GCI_TH_LOW_P_N_TYPE_0f, value);
                            break;
                        case bcmFabricLinkMidGciLvl2FC:
                            soc_reg64_field32_set(unit, DCM_GCI_TH_P_TYPE_0r, &reg_val64, GCI_TH_MED_P_N_TYPE_0f, value);
                            break;
                        case bcmFabricLinkMidGciLvl3FC:
                             soc_reg64_field32_set(unit,DCM_GCI_TH_P_TYPE_0r, &reg_val64, GCI_TH_HIGH_P_N_TYPE_0f, value);
                             break;

                        default: 
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
                    }
                                     
                    SOCDNX_IF_ERR_EXIT(WRITE_DCM_GCI_TH_P_TYPE_0r(unit, idx, i, reg_val64));
                }
                else
                {
                    SOCDNX_IF_ERR_EXIT(READ_DCM_GCI_TH_P_TYPE_1r(unit, idx, i, &reg_val64));

                    switch (type)
                    {
                        case bcmFabricLinkMidGciLvl1FC:
                            soc_reg64_field32_set(unit, DCM_GCI_TH_P_TYPE_1r, &reg_val64, GCI_TH_LOW_P_N_TYPE_1f, value);
                            break;
                        case bcmFabricLinkMidGciLvl2FC:
                            soc_reg64_field32_set(unit, DCM_GCI_TH_P_TYPE_1r, &reg_val64, GCI_TH_MED_P_N_TYPE_1f, value);
                            break;
                        case bcmFabricLinkMidGciLvl3FC:
                            soc_reg64_field32_set(unit,DCM_GCI_TH_P_TYPE_1r, &reg_val64, GCI_TH_HIGH_P_N_TYPE_1f, value);
                            break;

                        default: 
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
                    }
                                     
                    SOCDNX_IF_ERR_EXIT(WRITE_DCM_GCI_TH_P_TYPE_1r(unit, idx, i, reg_val64));
                }
            }
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_mid_gci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int* value)
{
    int i, nof_pipes;
    uint64 reg_val64;
    soc_dcmn_fabric_pipe_t pipe_lcl[1];
    int dcm_instance;
    SOCDNX_INIT_FUNC_DEFS;

    dcm_instance = is_fe1 ? 0 : SOC_DFE_DEFS_GET(unit, nof_instances_dcm)/2;
    nof_pipes=SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;
    *pipe_lcl = pipe;

    for (i=0; i< nof_pipes; i++)
    {    
        if(SOC_DCMN_FABRIC_PIPE_IS_SET(pipe_lcl, i))
        { 
            if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                SOCDNX_IF_ERR_EXIT(READ_DCM_GCI_TH_P_TYPE_0r(unit, dcm_instance, i, &reg_val64));
                switch (type)
                {
                    case bcmFabricLinkMidGciLvl1FC:
                        *value = soc_reg64_field32_get(unit, DCM_GCI_TH_P_TYPE_0r, reg_val64, GCI_TH_LOW_P_N_TYPE_0f);
                        break;

                    case bcmFabricLinkMidGciLvl2FC:
                        *value = soc_reg64_field32_get(unit, DCM_GCI_TH_P_TYPE_0r, reg_val64, GCI_TH_MED_P_N_TYPE_0f);
                        break;
                           
                    case bcmFabricLinkMidGciLvl3FC:
                        *value = soc_reg64_field32_get(unit, DCM_GCI_TH_P_TYPE_0r, reg_val64, GCI_TH_HIGH_P_N_TYPE_0f);
                        break;   
                                                                                      
                    default: 
                        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));                                                                                                               
                }
                
            } 
            else
            {
                SOCDNX_IF_ERR_EXIT(READ_DCM_GCI_TH_P_TYPE_1r(unit, dcm_instance, i, &reg_val64));
                switch (type)
                {
                    case bcmFabricLinkMidGciLvl1FC:
                        *value = soc_reg64_field32_get(unit, DCM_GCI_TH_P_TYPE_1r, reg_val64, GCI_TH_LOW_P_N_TYPE_1f);
                        break;

                    case bcmFabricLinkMidGciLvl2FC:
                        *value = soc_reg64_field32_get(unit, DCM_GCI_TH_P_TYPE_1r, reg_val64, GCI_TH_MED_P_N_TYPE_1f);
                        break;
                           
                    case bcmFabricLinkMidGciLvl3FC:
                        *value = soc_reg64_field32_get(unit, DCM_GCI_TH_P_TYPE_1r, reg_val64, GCI_TH_HIGH_P_N_TYPE_1f);
                        break;

                    default: 
                        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
                }
            }
            break; 
        }
    }


exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t soc_fe3200_fabric_flow_control_mid_rci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value)
{
    soc_error_t rc = SOC_E_NONE;
    uint64 field_val_64;

    SOCDNX_INIT_FUNC_DEFS;
    
    COMPILER_64_SET(field_val_64, 0, value);

    if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
    {
        switch(type)
        {
            case bcmFabricLinkMidRciLvl1FC: 
                rc = soc_reg64_field_validate(unit, DCM_RCI_TH_P_TYPE_0r, RCI_TH_LOW_P_N_TYPE_0f, field_val_64); 
                break;
            case bcmFabricLinkMidRciLvl2FC: 
                rc = soc_reg64_field_validate(unit, DCM_RCI_TH_P_TYPE_0r, RCI_TH_MED_P_N_TYPE_0f, field_val_64); 
                break;
            case bcmFabricLinkMidRciLvl3FC: 
                rc = soc_reg64_field_validate(unit, DCM_RCI_TH_P_TYPE_0r, RCI_TH_HIGH_P_N_TYPE_0f, field_val_64); 
                break;
            default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
        }
    }
    else
    {     
        switch(type)
        {
            case bcmFabricLinkMidRciLvl1FC: 
                rc = soc_reg64_field_validate(unit, DCM_RCI_TH_P_TYPE_1r, RCI_TH_LOW_P_N_TYPE_1f, field_val_64);
                break;
            case bcmFabricLinkMidRciLvl2FC: 
                rc = soc_reg64_field_validate(unit, DCM_RCI_TH_P_TYPE_1r, RCI_TH_MED_P_N_TYPE_1f, field_val_64); 
                break;
            case bcmFabricLinkMidRciLvl3FC: 
                rc = soc_reg64_field_validate(unit, DCM_RCI_TH_P_TYPE_1r, RCI_TH_HIGH_P_N_TYPE_1f, field_val_64); 
                break;
            default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
        }
    }
    
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for mid RCI threshold"),value)); 
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t soc_fe3200_fabric_flow_control_mid_rci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value)
{
    int idx, i, nof_pipes;
    uint64 reg_val64;
    soc_dcmn_fabric_pipe_t pipe_lcl[1];
    int dcm_instance_start, dcm_instance_end;
    SOCDNX_INIT_FUNC_DEFS;

    dcm_instance_start = is_fe1 ? 0 : SOC_DFE_DEFS_GET(unit, nof_instances_dcm)/2;
    dcm_instance_end = is_fe3 ?  SOC_DFE_DEFS_GET(unit, nof_instances_dcm) : SOC_DFE_DEFS_GET(unit, nof_instances_dcm)/2;

    nof_pipes=SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;
    *pipe_lcl = pipe;
    
     
    for(idx = dcm_instance_start ; idx < dcm_instance_end ; idx++) {
        for (i=0; i < nof_pipes; i++)
        {
            if (SOC_DCMN_FABRIC_PIPE_IS_SET(pipe_lcl, i))
            {
                if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
                {                    
                    SOCDNX_IF_ERR_EXIT(READ_DCM_RCI_TH_P_TYPE_0r(unit, idx, i, &reg_val64));

                    switch (type)
                    {
                        case bcmFabricLinkMidRciLvl1FC:
                            soc_reg64_field32_set(unit, DCM_RCI_TH_P_TYPE_0r, &reg_val64, RCI_TH_LOW_P_N_TYPE_0f, value);
                            break;
                        case bcmFabricLinkMidRciLvl2FC:
                            soc_reg64_field32_set(unit, DCM_RCI_TH_P_TYPE_0r, &reg_val64, RCI_TH_MED_P_N_TYPE_0f, value);
                            break;
                        case bcmFabricLinkMidRciLvl3FC:
                             soc_reg64_field32_set(unit,DCM_RCI_TH_P_TYPE_0r, &reg_val64, RCI_TH_HIGH_P_N_TYPE_0f, value);
                             break;

                        default: 
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
                    }
                                     
                    SOCDNX_IF_ERR_EXIT(WRITE_DCM_RCI_TH_P_TYPE_0r(unit, idx, i, reg_val64));
                }
                else
                {
                    SOCDNX_IF_ERR_EXIT(READ_DCM_RCI_TH_P_TYPE_1r(unit, idx, i, &reg_val64));

                    switch (type)
                    {
                        case bcmFabricLinkMidRciLvl1FC:
                            soc_reg64_field32_set(unit, DCM_RCI_TH_P_TYPE_1r, &reg_val64, RCI_TH_LOW_P_N_TYPE_1f, value);
                            break;
                        case bcmFabricLinkMidRciLvl2FC:
                            soc_reg64_field32_set(unit, DCM_RCI_TH_P_TYPE_1r, &reg_val64, RCI_TH_MED_P_N_TYPE_1f, value);
                            break;
                        case bcmFabricLinkMidRciLvl3FC:
                            soc_reg64_field32_set(unit, DCM_RCI_TH_P_TYPE_1r, &reg_val64, RCI_TH_HIGH_P_N_TYPE_1f, value);
                            break;

                        default: 
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
                    }
                                     
                    SOCDNX_IF_ERR_EXIT(WRITE_DCM_RCI_TH_P_TYPE_1r(unit, idx, i, reg_val64));
                }
            }
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t soc_fe3200_fabric_flow_control_mid_rci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int* value)
{
    int i, nof_pipes;
    uint64 reg_val64;
    soc_dcmn_fabric_pipe_t pipe_lcl[1];
    int dcm_instance;
    SOCDNX_INIT_FUNC_DEFS;
    dcm_instance = is_fe1 ? 0 : SOC_DFE_DEFS_GET(unit, nof_instances_dcm)/2;
    nof_pipes=SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;
    *pipe_lcl = pipe;

    for (i=0; i< nof_pipes; i++)
    {    
        if(SOC_DCMN_FABRIC_PIPE_IS_SET(pipe_lcl, i))
        { 
            if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                SOCDNX_IF_ERR_EXIT(READ_DCM_RCI_TH_P_TYPE_0r(unit, dcm_instance, i, &reg_val64));
                switch (type)
                {
                    case bcmFabricLinkMidRciLvl1FC:
                        *value = soc_reg64_field32_get(unit, DCM_RCI_TH_P_TYPE_0r, reg_val64, RCI_TH_LOW_P_N_TYPE_0f);
                        break;

                    case bcmFabricLinkMidRciLvl2FC:
                        *value = soc_reg64_field32_get(unit, DCM_RCI_TH_P_TYPE_0r, reg_val64, RCI_TH_MED_P_N_TYPE_0f);
                        break;
                           
                    case bcmFabricLinkMidRciLvl3FC:
                        *value = soc_reg64_field32_get(unit, DCM_RCI_TH_P_TYPE_0r, reg_val64, RCI_TH_HIGH_P_N_TYPE_0f);
                        break;   
                                                                                      
                    default: 
                        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));                                                                                                               
                }
                
            } 
            else
            {
                SOCDNX_IF_ERR_EXIT(READ_DCM_RCI_TH_P_TYPE_1r(unit, dcm_instance, i, &reg_val64));
                switch (type)
                {
                    case bcmFabricLinkMidRciLvl1FC:
                        *value = soc_reg64_field32_get(unit, DCM_RCI_TH_P_TYPE_1r, reg_val64, RCI_TH_LOW_P_N_TYPE_1f);
                        break;

                    case bcmFabricLinkMidRciLvl2FC:
                        *value = soc_reg64_field32_get(unit, DCM_RCI_TH_P_TYPE_1r, reg_val64, RCI_TH_MED_P_N_TYPE_1f);
                        break;
                           
                    case bcmFabricLinkMidRciLvl3FC:
                        *value = soc_reg64_field32_get(unit, DCM_RCI_TH_P_TYPE_1r, reg_val64, RCI_TH_HIGH_P_N_TYPE_1f);
                        break;

                    default: 
                        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
                }
            }
            break; 
        }
    }


exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_mid_prio_drop_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value)
{
    soc_error_t rc = SOC_E_NONE;
    uint64 field_val_64;

    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_64_SET(field_val_64, 0, value);

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        switch (type)
        {
            case bcmFabricLinkMidPrio0Drop:
                rc = soc_reg64_field_validate(unit, DCM_PIPE_0_PRIORITY_DROP_THRESHOLD_TYPEr, PRI_0_DROP_TH_P_0_TYPE_Nf, field_val_64);
                break;
            case bcmFabricLinkMidPrio1Drop:
                rc = soc_reg64_field_validate(unit, DCM_PIPE_0_PRIORITY_DROP_THRESHOLD_TYPEr, PRI_1_DROP_TH_P_0_TYPE_Nf, field_val_64);
                break;
            case bcmFabricLinkMidPrio2Drop:
                rc = soc_reg64_field_validate(unit, DCM_PIPE_0_PRIORITY_DROP_THRESHOLD_TYPEr, PRI_2_DROP_TH_P_0_TYPE_Nf, field_val_64);
                break;
            case bcmFabricLinkMidPrio3Drop:
                rc = soc_reg64_field_validate(unit, DCM_PIPE_0_PRIORITY_DROP_THRESHOLD_TYPEr, PRI_3_DROP_TH_P_0_TYPE_Nf, field_val_64);
                break;
            default: 
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));  
        }
    }
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {
        switch (type)
        {
            case bcmFabricLinkMidPrio0Drop:
                rc = soc_reg64_field_validate(unit, DCM_PIPE_1_PRIORITY_DROP_THRESHOLD_TYPEr, PRI_0_DROP_TH_P_1_TYPE_Nf, field_val_64);
                break;
            case bcmFabricLinkMidPrio1Drop:
                rc = soc_reg64_field_validate(unit, DCM_PIPE_1_PRIORITY_DROP_THRESHOLD_TYPEr, PRI_1_DROP_TH_P_1_TYPE_Nf, field_val_64);
                break;
            case bcmFabricLinkMidPrio2Drop:
                rc = soc_reg64_field_validate(unit, DCM_PIPE_1_PRIORITY_DROP_THRESHOLD_TYPEr, PRI_2_DROP_TH_P_1_TYPE_Nf, field_val_64);
                break;
            case bcmFabricLinkMidPrio3Drop:
                rc = soc_reg64_field_validate(unit, DCM_PIPE_1_PRIORITY_DROP_THRESHOLD_TYPEr, PRI_3_DROP_TH_P_1_TYPE_Nf, field_val_64);
                break;
            default: 
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));  
        }
    }
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,2))
    {
        switch (type)
        {
            case bcmFabricLinkMidPrio0Drop:
                rc = soc_reg64_field_validate(unit, DCM_PIPE_2_PRIORITY_DROP_THRESHOLD_TYPEr, PRI_0_DROP_TH_P_2_TYPE_Nf, field_val_64);
                break;
            case bcmFabricLinkMidPrio1Drop:
                rc = soc_reg64_field_validate(unit, DCM_PIPE_2_PRIORITY_DROP_THRESHOLD_TYPEr, PRI_1_DROP_TH_P_2_TYPE_Nf, field_val_64);
                break;
            case bcmFabricLinkMidPrio2Drop:
                rc = soc_reg64_field_validate(unit, DCM_PIPE_2_PRIORITY_DROP_THRESHOLD_TYPEr, PRI_2_DROP_TH_P_2_TYPE_Nf, field_val_64);
                break;
            case bcmFabricLinkMidPrio3Drop:
                rc = soc_reg64_field_validate(unit, DCM_PIPE_2_PRIORITY_DROP_THRESHOLD_TYPEr, PRI_3_DROP_TH_P_2_TYPE_Nf, field_val_64);
                break;
            default: 
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));  
        }
    }
    
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for mid RCI threshold"),value)); 
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t soc_fe3200_fabric_flow_control_mid_prio_drop_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value)
{

    int idx, array_block;
    int enablers_bmp;
    uint64 reg_val64;
    uint32 reg_val_enablers;
    int dcm_instance_start, dcm_instance_end;
    SOCDNX_INIT_FUNC_DEFS;

    dcm_instance_start = is_fe1 ? 0 : SOC_DFE_DEFS_GET(unit, nof_instances_dcm)/2;
    dcm_instance_end = is_fe3 ?  SOC_DFE_DEFS_GET(unit, nof_instances_dcm) : SOC_DFE_DEFS_GET(unit, nof_instances_dcm)/2;

    if (type_index == soc_dfe_fabric_link_fifo_type_index_0)
    {
        array_block = 0;
    }
    else
    {
        array_block = 1;
    }

    for (idx=dcm_instance_start; idx < dcm_instance_end; idx++)
    {
      
        if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
        {
            SOCDNX_IF_ERR_EXIT(READ_DCM_PIPE_0_PRIORITY_DROP_THRESHOLD_TYPEr(unit, idx, array_block, &reg_val64));
            SOCDNX_IF_ERR_EXIT(READ_DCM_DCM_ENABLERS_REGISTERr(unit, idx, &reg_val_enablers));
            enablers_bmp = soc_reg_field_get(unit, DCM_DCM_ENABLERS_REGISTERr, reg_val_enablers, LOW_PR_DROP_EN_P_0f);

            switch (type)
            {
                case bcmFabricLinkMidPrio0Drop:
                    soc_reg64_field32_set(unit, DCM_PIPE_0_PRIORITY_DROP_THRESHOLD_TYPEr, &reg_val64, PRI_0_DROP_TH_P_0_TYPE_Nf, value);
                    enablers_bmp |= 1;
                    break;
                case bcmFabricLinkMidPrio1Drop:
                    soc_reg64_field32_set(unit, DCM_PIPE_0_PRIORITY_DROP_THRESHOLD_TYPEr, &reg_val64, PRI_1_DROP_TH_P_0_TYPE_Nf, value);
                    enablers_bmp |= (1 << 1);
                    break;
                 case bcmFabricLinkMidPrio2Drop:
                    soc_reg64_field32_set(unit, DCM_PIPE_0_PRIORITY_DROP_THRESHOLD_TYPEr, &reg_val64, PRI_2_DROP_TH_P_0_TYPE_Nf, value);
                    enablers_bmp |= (1 << 2);
                    break;
                 case bcmFabricLinkMidPrio3Drop:
                    soc_reg64_field32_set(unit, DCM_PIPE_0_PRIORITY_DROP_THRESHOLD_TYPEr, &reg_val64, PRI_3_DROP_TH_P_0_TYPE_Nf, value);
                    enablers_bmp |= (1 << 3);
                    break;
                default: 
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));  
            }

            soc_reg_field_set(unit, DCM_DCM_ENABLERS_REGISTERr, &reg_val_enablers, LOW_PR_DROP_EN_P_0f, enablers_bmp);
            SOCDNX_IF_ERR_EXIT(WRITE_DCM_DCM_ENABLERS_REGISTERr(unit, idx, reg_val_enablers));
            SOCDNX_IF_ERR_EXIT(WRITE_DCM_PIPE_0_PRIORITY_DROP_THRESHOLD_TYPEr(unit, idx, array_block, reg_val64));
        }

        if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
        {
            SOCDNX_IF_ERR_EXIT(READ_DCM_PIPE_1_PRIORITY_DROP_THRESHOLD_TYPEr(unit, idx, array_block, &reg_val64));
            SOCDNX_IF_ERR_EXIT(READ_DCM_DCM_ENABLERS_REGISTERr(unit, idx, &reg_val_enablers));
            enablers_bmp = soc_reg_field_get(unit, DCM_DCM_ENABLERS_REGISTERr, reg_val_enablers, LOW_PR_DROP_EN_P_1f);

            switch (type)
            {
                case bcmFabricLinkMidPrio0Drop:
                    soc_reg64_field32_set(unit, DCM_PIPE_1_PRIORITY_DROP_THRESHOLD_TYPEr, &reg_val64, PRI_0_DROP_TH_P_1_TYPE_Nf, value);
                    enablers_bmp |= 1;
                    break;
                case bcmFabricLinkMidPrio1Drop:
                    soc_reg64_field32_set(unit, DCM_PIPE_1_PRIORITY_DROP_THRESHOLD_TYPEr, &reg_val64, PRI_1_DROP_TH_P_1_TYPE_Nf, value);
                    enablers_bmp |= (1 << 1);
                    break;
                 case bcmFabricLinkMidPrio2Drop:
                    soc_reg64_field32_set(unit, DCM_PIPE_1_PRIORITY_DROP_THRESHOLD_TYPEr, &reg_val64, PRI_2_DROP_TH_P_1_TYPE_Nf, value);
                    enablers_bmp |= (1 << 2);
                    break;
                 case bcmFabricLinkMidPrio3Drop:
                    soc_reg64_field32_set(unit, DCM_PIPE_1_PRIORITY_DROP_THRESHOLD_TYPEr, &reg_val64, PRI_3_DROP_TH_P_1_TYPE_Nf, value);
                    enablers_bmp |= (1 << 3);
                    break;
                default: 
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));  
            }

            soc_reg_field_set(unit, DCM_DCM_ENABLERS_REGISTERr, &reg_val_enablers, LOW_PR_DROP_EN_P_1f, enablers_bmp);
            SOCDNX_IF_ERR_EXIT(WRITE_DCM_DCM_ENABLERS_REGISTERr(unit, idx, reg_val_enablers));
            SOCDNX_IF_ERR_EXIT(WRITE_DCM_PIPE_1_PRIORITY_DROP_THRESHOLD_TYPEr(unit, idx, array_block, reg_val64));
        }

        if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,2))
        {
            SOCDNX_IF_ERR_EXIT(READ_DCM_PIPE_2_PRIORITY_DROP_THRESHOLD_TYPEr(unit, idx, array_block, &reg_val64));
            SOCDNX_IF_ERR_EXIT(READ_DCM_DCM_ENABLERS_REGISTERr(unit, idx, &reg_val_enablers));
            enablers_bmp = soc_reg_field_get(unit, DCM_DCM_ENABLERS_REGISTERr, reg_val_enablers, LOW_PR_DROP_EN_P_2f);

            switch (type)
            {
                case bcmFabricLinkMidPrio0Drop:
                    soc_reg64_field32_set(unit, DCM_PIPE_2_PRIORITY_DROP_THRESHOLD_TYPEr, &reg_val64, PRI_0_DROP_TH_P_2_TYPE_Nf, value);
                    enablers_bmp |= 1;
                    break;
                case bcmFabricLinkMidPrio1Drop:
                    soc_reg64_field32_set(unit, DCM_PIPE_2_PRIORITY_DROP_THRESHOLD_TYPEr, &reg_val64, PRI_1_DROP_TH_P_2_TYPE_Nf, value);
                    enablers_bmp |= (1 << 1);
                    break;
                 case bcmFabricLinkMidPrio2Drop:
                    soc_reg64_field32_set(unit, DCM_PIPE_2_PRIORITY_DROP_THRESHOLD_TYPEr, &reg_val64, PRI_2_DROP_TH_P_2_TYPE_Nf, value);
                    enablers_bmp |= (1 << 2);
                    break;
                 case bcmFabricLinkMidPrio3Drop:
                    soc_reg64_field32_set(unit, DCM_PIPE_2_PRIORITY_DROP_THRESHOLD_TYPEr, &reg_val64, PRI_3_DROP_TH_P_2_TYPE_Nf, value);
                    enablers_bmp |= (1 << 3);
                    break;
                default: 
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));  
            }

            soc_reg_field_set(unit, DCM_DCM_ENABLERS_REGISTERr, &reg_val_enablers, LOW_PR_DROP_EN_P_2f, enablers_bmp);
            SOCDNX_IF_ERR_EXIT(WRITE_DCM_DCM_ENABLERS_REGISTERr(unit, idx, reg_val_enablers));
            SOCDNX_IF_ERR_EXIT(WRITE_DCM_PIPE_2_PRIORITY_DROP_THRESHOLD_TYPEr(unit, idx, array_block, reg_val64));
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_mid_prio_drop_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int* value)
{

    int array_block;
    uint64 reg_val64;
    int dcm_instance;
    SOCDNX_INIT_FUNC_DEFS;

    dcm_instance = is_fe1 ? 0 : SOC_DFE_DEFS_GET(unit, nof_instances_dcm)/2;
    if (type_index == soc_dfe_fabric_link_fifo_type_index_0)
    {
        array_block = 0;
    }
    else
    {
        array_block = 1;
    }
     
    if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {

        SOCDNX_IF_ERR_EXIT(READ_DCM_PIPE_0_PRIORITY_DROP_THRESHOLD_TYPEr(unit, dcm_instance, array_block, &reg_val64));
        
        switch (type)
        {
        case bcmFabricLinkMidPrio0Drop:
                *value = soc_reg64_field32_get(unit, DCM_PIPE_0_PRIORITY_DROP_THRESHOLD_TYPEr, reg_val64, PRI_0_DROP_TH_P_0_TYPE_Nf);                  
                break;
            case bcmFabricLinkMidPrio1Drop:
                *value = soc_reg64_field32_get(unit, DCM_PIPE_0_PRIORITY_DROP_THRESHOLD_TYPEr, reg_val64, PRI_1_DROP_TH_P_0_TYPE_Nf);                
                break;
             case bcmFabricLinkMidPrio2Drop:
                *value = soc_reg64_field32_get(unit, DCM_PIPE_0_PRIORITY_DROP_THRESHOLD_TYPEr, reg_val64, PRI_2_DROP_TH_P_0_TYPE_Nf);                
                break;
             case bcmFabricLinkMidPrio3Drop:
                *value = soc_reg64_field32_get(unit, DCM_PIPE_0_PRIORITY_DROP_THRESHOLD_TYPEr, reg_val64, PRI_3_DROP_TH_P_0_TYPE_Nf);                
                break;
            default: 
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));  
        }
    }
    else if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {
        SOCDNX_IF_ERR_EXIT(READ_DCM_PIPE_1_PRIORITY_DROP_THRESHOLD_TYPEr(unit, dcm_instance, array_block, &reg_val64));
        
        switch (type)
        {
            case bcmFabricLinkMidPrio0Drop:
                *value = soc_reg64_field32_get(unit, DCM_PIPE_1_PRIORITY_DROP_THRESHOLD_TYPEr, reg_val64, PRI_0_DROP_TH_P_1_TYPE_Nf);                  
                break;
            case bcmFabricLinkMidPrio1Drop:
                *value = soc_reg64_field32_get(unit, DCM_PIPE_1_PRIORITY_DROP_THRESHOLD_TYPEr, reg_val64, PRI_1_DROP_TH_P_1_TYPE_Nf);                
                break;
             case bcmFabricLinkMidPrio2Drop:
                *value = soc_reg64_field32_get(unit, DCM_PIPE_1_PRIORITY_DROP_THRESHOLD_TYPEr, reg_val64, PRI_2_DROP_TH_P_1_TYPE_Nf);                
                break;
             case bcmFabricLinkMidPrio3Drop:
                *value = soc_reg64_field32_get(unit, DCM_PIPE_1_PRIORITY_DROP_THRESHOLD_TYPEr, reg_val64, PRI_3_DROP_TH_P_1_TYPE_Nf);                
                break;
            default: 
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));  
        }
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(READ_DCM_PIPE_2_PRIORITY_DROP_THRESHOLD_TYPEr(unit, dcm_instance, array_block, &reg_val64));
        
        switch (type)
        {
            case bcmFabricLinkMidPrio0Drop:
                *value = soc_reg64_field32_get(unit, DCM_PIPE_2_PRIORITY_DROP_THRESHOLD_TYPEr, reg_val64, PRI_0_DROP_TH_P_2_TYPE_Nf);                  
                break;
            case bcmFabricLinkMidPrio1Drop:
                *value = soc_reg64_field32_get(unit, DCM_PIPE_2_PRIORITY_DROP_THRESHOLD_TYPEr, reg_val64, PRI_1_DROP_TH_P_2_TYPE_Nf);                
                break;
             case bcmFabricLinkMidPrio2Drop:
                *value = soc_reg64_field32_get(unit, DCM_PIPE_2_PRIORITY_DROP_THRESHOLD_TYPEr, reg_val64, PRI_2_DROP_TH_P_2_TYPE_Nf);                
                break;
             case bcmFabricLinkMidPrio3Drop:
                *value = soc_reg64_field32_get(unit, DCM_PIPE_2_PRIORITY_DROP_THRESHOLD_TYPEr, reg_val64, PRI_3_DROP_TH_P_2_TYPE_Nf);                
                break;
            default: 
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));  
        }

    }
    
exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t soc_fe3200_fabric_flow_control_mid_almost_full_threshold_validate(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value)
{
    soc_error_t rc1 = SOC_E_NONE;
    uint64 field_val_64;

    SOCDNX_INIT_FUNC_DEFS;
    


    COMPILER_64_SET(field_val_64, 0, value);


    rc1 = soc_reg64_field_validate(unit, DCM_FIFOS_THRESHOLDS_P_TYPE_0r, ALM_FULL_0_TH_P_N_TYPE_0f, field_val_64);
    if (SOC_FAILURE(rc1))
    {
        SOCDNX_EXIT_WITH_ERR(rc1, (_BSL_SOCDNX_MSG("value %d isn't a valid for mid almost full threshold"),value));
    }
   
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t soc_fe3200_fabric_flow_control_mid_almost_full_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int almost_full_index, int is_fe1, int is_fe3, int value)
{
    uint64 reg_val64;
    int idx, nof_pipes, i;
    soc_dcmn_fabric_pipe_t pipe_lcl[1];
    int dcm_instance_start, dcm_instance_end;
    SOCDNX_INIT_FUNC_DEFS;

    dcm_instance_start = is_fe1 ? 0 : SOC_DFE_DEFS_GET(unit, nof_instances_dcm)/2;
    dcm_instance_end = is_fe3 ?  SOC_DFE_DEFS_GET(unit, nof_instances_dcm) : SOC_DFE_DEFS_GET(unit, nof_instances_dcm)/2;

    nof_pipes=SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;
    *pipe_lcl = pipe;
        
    for (idx = dcm_instance_start; idx < dcm_instance_end ; idx++)
    {
        for (i = 0 ; i < nof_pipes ; i++)
        {
            if (SOC_DCMN_FABRIC_PIPE_IS_SET(pipe_lcl, i))
            {
                if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
                {
                    SOCDNX_IF_ERR_EXIT(READ_DCM_FIFOS_THRESHOLDS_P_TYPE_0r(unit, idx, i, &reg_val64));
                    if (almost_full_index == 0 || almost_full_index == -1 )
                    {
                        soc_reg64_field32_set(unit, DCM_FIFOS_THRESHOLDS_P_TYPE_0r, &reg_val64, ALM_FULL_0_TH_P_N_TYPE_0f, value);
                    }
                    if (almost_full_index == 1 || almost_full_index == -1 )
                    {
                        soc_reg64_field32_set(unit, DCM_FIFOS_THRESHOLDS_P_TYPE_0r, &reg_val64, ALM_FULL_1_TH_P_N_TYPE_0f, value);
                    }
                    SOCDNX_IF_ERR_EXIT(WRITE_DCM_FIFOS_THRESHOLDS_P_TYPE_0r(unit, idx, i, reg_val64));
                }
                else
                {
                    SOCDNX_IF_ERR_EXIT(READ_DCM_FIFOS_THRESHOLDS_P_TYPE_1r(unit, idx, i, &reg_val64));
                    if (almost_full_index == 0 || almost_full_index == -1 )
                    {
                        soc_reg64_field32_set(unit, DCM_FIFOS_THRESHOLDS_P_TYPE_1r, &reg_val64, ALM_FULL_0_TH_P_N_TYPE_1f, value);
                    }
                    if (almost_full_index == 1 || almost_full_index == -1 )
                    {
                        soc_reg64_field32_set(unit, DCM_FIFOS_THRESHOLDS_P_TYPE_1r, &reg_val64, ALM_FULL_1_TH_P_N_TYPE_1f, value);
                    }
                    SOCDNX_IF_ERR_EXIT(WRITE_DCM_FIFOS_THRESHOLDS_P_TYPE_1r(unit, idx, i, reg_val64));
                }
            }
        }
    }
   
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_mid_almost_full_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int almost_full_index, int is_fe1, int* value)
{
    int i, nof_pipes;
    uint64 reg_val64;
    soc_dcmn_fabric_pipe_t pipe_lcl[1];
    int dcm_instance;
    SOCDNX_INIT_FUNC_DEFS;

    dcm_instance = is_fe1 ? 0 : SOC_DFE_DEFS_GET(unit, nof_instances_dcm)/2;

    nof_pipes=SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;
    *pipe_lcl = pipe;

    for (i=0; i< nof_pipes; i++)
    {    
        if(SOC_DCMN_FABRIC_PIPE_IS_SET(pipe_lcl, i))
        { 
            if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                SOCDNX_IF_ERR_EXIT(READ_DCM_FIFOS_THRESHOLDS_P_TYPE_0r(unit, dcm_instance, i, &reg_val64));
                if (almost_full_index == 0)
                {
                    *value = soc_reg64_field32_get(unit, DCM_FIFOS_THRESHOLDS_P_TYPE_0r, reg_val64, ALM_FULL_0_TH_P_N_TYPE_0f);
                } else 
                {
                    *value = soc_reg64_field32_get(unit, DCM_FIFOS_THRESHOLDS_P_TYPE_0r, reg_val64, ALM_FULL_1_TH_P_N_TYPE_0f);
                }
            } 
            else
            {
                SOCDNX_IF_ERR_EXIT(READ_DCM_FIFOS_THRESHOLDS_P_TYPE_1r(unit, dcm_instance, i, &reg_val64));
                if (almost_full_index == 0)
                {
                    *value = soc_reg64_field32_get(unit, DCM_FIFOS_THRESHOLDS_P_TYPE_1r, reg_val64, ALM_FULL_0_TH_P_N_TYPE_1f);
                } else 
                {
                    *value = soc_reg64_field32_get(unit, DCM_FIFOS_THRESHOLDS_P_TYPE_1r, reg_val64, ALM_FULL_1_TH_P_N_TYPE_1f);
                }
            }
            break; 
        }
    }


exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_mid_fifo_size_threshold_validate(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value)
{
    soc_error_t rc = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;

    
    if (value % 4 != 0)
    {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't valid for mid fifo size threshold, value must be divisible by 4"),value));
    }

    if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        rc = soc_reg_field_validate(unit, DCM_FIFOS_DEPTH_TYPEr, FIFO_DEPTH_P_0_TYPE_Nf, value);
    }

    if (SOC_FAILURE(rc))
    {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for mid fifo size threshold"),value));
    }

    if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {
        rc = soc_reg_field_validate(unit, DCM_FIFOS_DEPTH_TYPEr, FIFO_DEPTH_P_1_TYPE_Nf, value);
    }

    if (SOC_FAILURE(rc))
    {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for mid fifo size threshold"),value));
    }

    if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,2) && !SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1) && !SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Mid Fifo size for pipe 2 can't be configured, it is derived from total buffer size minus pipe 0 fifo size minus pipe 1 fifo size")));
    }
 
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_mid_fifo_size_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value, int to_check_counter_overflow)
{
    uint32 reg_val;
    int idx, array_block;
    soc_error_t rc = SOC_E_NONE;
    int dcm_instance_start, dcm_instance_end;
    SOCDNX_INIT_FUNC_DEFS;

    dcm_instance_start = is_fe1 ? 0 : SOC_DFE_DEFS_GET(unit, nof_instances_dcm)/2;
    dcm_instance_end = is_fe3 ?  SOC_DFE_DEFS_GET(unit, nof_instances_dcm) : SOC_DFE_DEFS_GET(unit, nof_instances_dcm)/2;

    if (type_index == soc_dfe_fabric_link_fifo_type_index_0)
    {
        array_block = 0;
    }
    else
    {
        array_block = 1;
    }

    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_soft_init, (unit, SOC_DCMN_RESET_ACTION_IN_RESET));
    SOCDNX_IF_ERR_EXIT(rc);
    
    

    for (idx = dcm_instance_start; idx < dcm_instance_end; idx ++)
    {
        if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe, 0))
        {
            SOCDNX_IF_ERR_EXIT(READ_DCM_FIFOS_DEPTH_TYPEr(unit, idx, array_block, &reg_val));
            soc_reg_field_set(unit, DCM_FIFOS_DEPTH_TYPEr, &reg_val, FIFO_DEPTH_P_0_TYPE_Nf, value);
            SOCDNX_IF_ERR_EXIT(WRITE_DCM_FIFOS_DEPTH_TYPEr(unit, idx, array_block, reg_val));
        }

        if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe, 1))
        {            
            SOCDNX_IF_ERR_EXIT(READ_DCM_FIFOS_DEPTH_TYPEr(unit, idx, array_block, &reg_val));
            soc_reg_field_set(unit, DCM_FIFOS_DEPTH_TYPEr, &reg_val, FIFO_DEPTH_P_1_TYPE_Nf, value);
            SOCDNX_IF_ERR_EXIT(WRITE_DCM_FIFOS_DEPTH_TYPEr(unit, idx, array_block, reg_val));
        }
    }

    if (to_check_counter_overflow) {
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_tx_prevent_counter_per_link_overflow (unit, type_index, pipe, is_fe1, is_fe3));
    }

    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_soft_init, (unit, SOC_DCMN_RESET_ACTION_OUT_RESET));
    SOCDNX_IF_ERR_EXIT(rc);


    
 
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_mid_fifo_size_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int* value)
{
    uint32 reg_val;
    int array_block;
    int fifo_size_pipe_0;
    int fifo_size_pipe_1;
    int dcm_instance;
    SOCDNX_INIT_FUNC_DEFS;

    dcm_instance = is_fe1 ? 0 : SOC_DFE_DEFS_GET(unit, nof_instances_dcm)/2;
    if (type_index == soc_dfe_fabric_link_fifo_type_index_0)
    {
        array_block = 0;
    }
    else
    {
        array_block = 1;
    }
    
    
    SOCDNX_IF_ERR_EXIT(READ_DCM_FIFOS_DEPTH_TYPEr(unit, dcm_instance, array_block, &reg_val));

    if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe, 0))
    {
        *value = soc_reg_field_get(unit, DCM_FIFOS_DEPTH_TYPEr, reg_val, FIFO_DEPTH_P_0_TYPE_Nf);
    }

    else if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe, 1))
    {            
        *value = soc_reg_field_get(unit, DCM_FIFOS_DEPTH_TYPEr, reg_val, FIFO_DEPTH_P_1_TYPE_Nf);
    }
    else if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,2))
    {
        fifo_size_pipe_0 = soc_reg_field_get(unit, DCM_FIFOS_DEPTH_TYPEr, reg_val, FIFO_DEPTH_P_0_TYPE_Nf);
        fifo_size_pipe_1 = soc_reg_field_get(unit, DCM_FIFOS_DEPTH_TYPEr, reg_val, FIFO_DEPTH_P_1_TYPE_Nf);

        *value = SOC_FE3200_DCM_FIFO_SIZE_BUFFER_SIZE - fifo_size_pipe_0 - fifo_size_pipe_1; 
        if (*value < 0)
        {
            *value = 0;
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_tx_almost_full_threshold_validate(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value)
{
    soc_error_t rc1 = SOC_E_NONE;
    soc_error_t rc2 = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;
    
    if (type_index == soc_dfe_fabric_link_fifo_type_index_0)
    {
        rc1 = soc_reg_field_validate(unit, DCL_ALM_FULL_Pr, TYPE_0_ALM_FULL_P_Nf, value);
        rc2 = soc_reg_field_validate(unit, DCL_DCMB_ALM_FULL_Pr, DCMB_TYPE_0_ALM_FULL_P_Nf, value);
    }
    else
    {
        rc1 = soc_reg_field_validate(unit, DCL_ALM_FULL_Pr, TYPE_1_ALM_FULL_P_Nf, value);
        rc2 = soc_reg_field_validate(unit, DCL_DCMB_ALM_FULL_Pr, DCMB_TYPE_1_ALM_FULL_P_Nf, value);
    }

    if (SOC_FAILURE(rc1))
    {
        SOCDNX_EXIT_WITH_ERR(rc1, (_BSL_SOCDNX_MSG("value %d isn't a valid for tx almost full threshold"),value));
    }

    if (SOC_FAILURE(rc2))
    {
        SOCDNX_EXIT_WITH_ERR(rc2, (_BSL_SOCDNX_MSG("value %d isn't a valid for tx almost full threshold"),value));
    }

   
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_tx_almost_full_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value, int to_check_counter_overflow)
{

    uint32 dcl_dcmb_reg_val,dcl_reg_val;
    int nof_pipes, nof_instances_dcl, min_idx, max_idx, idx, i, min_idx_dcmb, max_idx_dcmb;
    soc_dcmn_fabric_pipe_t pipe_lcl[1];

    SOCDNX_INIT_FUNC_DEFS;

    nof_instances_dcl = SOC_DFE_DEFS_GET(unit, nof_instances_dcl);
    nof_pipes=SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;

    if(fe1 && fe3)
    {
        min_idx = 0; 
        max_idx = nof_instances_dcl - 1;
    }
    else if(fe1)
    {
        min_idx = 2; max_idx = 3;
    }
    else
    {
        min_idx = 0; 
        max_idx = 1;
    }

    
    
    if(SOC_DFE_IS_FE13_ASYMMETRIC(unit) && fe3)
    {
        min_idx_dcmb = 0;
        max_idx_dcmb = 3;
    } else {
        min_idx_dcmb = 0;
        max_idx_dcmb = -1;
    }
    
    *pipe_lcl = pipe;
    
    for (idx = min_idx; idx <= max_idx ; idx++)
    {
        for (i = 0; i < nof_pipes; i++)
        {
            if (SOC_DCMN_FABRIC_PIPE_IS_SET(pipe_lcl, i))
            {
                SOCDNX_IF_ERR_EXIT(READ_DCL_ALM_FULL_Pr(unit, idx, i, &dcl_reg_val));

                if (type_index == soc_dfe_fabric_link_fifo_type_index_0)
                {
                    soc_reg_field_set(unit, DCL_ALM_FULL_Pr, &dcl_reg_val, TYPE_0_ALM_FULL_P_Nf, value);
                }
                else
                {
                    soc_reg_field_set(unit, DCL_ALM_FULL_Pr, &dcl_reg_val, TYPE_1_ALM_FULL_P_Nf, value);
                }

                SOCDNX_IF_ERR_EXIT(WRITE_DCL_ALM_FULL_Pr(unit, idx, i , dcl_reg_val));
            }
        }
    }
    
    for (idx = min_idx_dcmb; idx <= max_idx_dcmb; idx++)
    {
        for (i = 0; i < nof_pipes; i++)
        {
            if (SOC_DCMN_FABRIC_PIPE_IS_SET(pipe_lcl, i))
            {
                SOCDNX_IF_ERR_EXIT(READ_DCL_DCMB_ALM_FULL_Pr(unit, idx, i, &dcl_dcmb_reg_val));

                if (type_index == soc_dfe_fabric_link_fifo_type_index_0)
                {
                    soc_reg_field_set(unit, DCL_DCMB_ALM_FULL_Pr, &dcl_dcmb_reg_val, DCMB_TYPE_0_ALM_FULL_P_Nf, value);
                }
                else
                {
                    soc_reg_field_set(unit, DCL_DCMB_ALM_FULL_Pr, &dcl_dcmb_reg_val, DCMB_TYPE_1_ALM_FULL_P_Nf, value);
                }

                SOCDNX_IF_ERR_EXIT(WRITE_DCL_DCMB_ALM_FULL_Pr(unit, idx, i, dcl_dcmb_reg_val));
            }
        }      
    }

    if (to_check_counter_overflow) {
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_tx_prevent_counter_per_link_overflow (unit, type_index, pipe, fe1, fe3));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t soc_fe3200_fabric_flow_control_tx_almost_full_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value)
{


    int nof_pipes ,idx, i;
    uint32 reg_val;
    soc_dcmn_fabric_pipe_t pipe_lcl[1];

    SOCDNX_INIT_FUNC_DEFS;

    nof_pipes=SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;

    if(fe1 && fe3)
    {
        idx = 0; 
    }
    else if(fe1)
    {
        idx = 2;
    }
    else
    {
        idx = 0;
    }
    
    *pipe_lcl = pipe;
    for (i = 0; i < nof_pipes; i++)
    {
        if (SOC_DCMN_FABRIC_PIPE_IS_SET(pipe_lcl, i))
        {
            SOCDNX_IF_ERR_EXIT(READ_DCL_ALM_FULL_Pr(unit, idx, i, &reg_val));
            if (type_index == soc_dfe_fabric_link_fifo_type_index_0)
            {
                *value = soc_reg_field_get(unit, DCL_ALM_FULL_Pr, reg_val, TYPE_0_ALM_FULL_P_Nf);
            }
            else
            {
                *value = soc_reg_field_get(unit, DCL_ALM_FULL_Pr, reg_val, TYPE_1_ALM_FULL_P_Nf);
            }
             
            break; 
        }
    }
     
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_fe3200_fabric_flow_control_tx_fifo_size_threshold_validate(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value)
{
    soc_error_t rc1 = SOC_E_NONE;
    soc_error_t rc2 = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    if (value % 4 != 0)
    {
        SOCDNX_EXIT_WITH_ERR(rc1, (_BSL_SOCDNX_MSG("value %d isn't valid for tx fifo size threshold, value must be divisible by 4"),value));
    }

    if (value < 0 || value > 768)
    {
        SOCDNX_EXIT_WITH_ERR(rc1, (_BSL_SOCDNX_MSG("value %d isn't valid for tx fifo size threshold, value must be between 0 and 768"),value));
    }

    if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        rc1 = soc_reg_field_validate(unit, DCL_FIFO_DEPTH_TYPEr, FIFO_DEPTH_P_0_TYPE_Nf, value);
        rc2 = soc_reg_field_validate(unit, DCL_DCMB_FIFO_DEPTH_TYPEr, DCMB_FIFO_DEPTH_P_0_TYPE_Nf, value);
    }

    if (SOC_FAILURE(rc1))
    {
        SOCDNX_EXIT_WITH_ERR(rc1, (_BSL_SOCDNX_MSG("value %d isn't a valid for tx fifo size threshold"),value));
    }

    if (SOC_FAILURE(rc2))
    {
        SOCDNX_EXIT_WITH_ERR(rc2, (_BSL_SOCDNX_MSG("value %d isn't a valid for tx fifo size threshold"),value));
    }

    
    if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {
        rc1 = soc_reg_field_validate(unit, DCL_FIFO_DEPTH_TYPEr, FIFO_DEPTH_P_1_TYPE_Nf, value);
        rc2 = soc_reg_field_validate(unit, DCL_DCMB_FIFO_DEPTH_TYPEr, DCMB_FIFO_DEPTH_P_1_TYPE_Nf, value);
    }

    if (SOC_FAILURE(rc1))
    {
        SOCDNX_EXIT_WITH_ERR(rc1, (_BSL_SOCDNX_MSG("value %d isn't a valid for tx fifo size threshold"),value));
    }

    if (SOC_FAILURE(rc2))
    {
        SOCDNX_EXIT_WITH_ERR(rc2, (_BSL_SOCDNX_MSG("value %d isn't a valid for tx fifo size threshold"),value));
    }

    if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,2))
    {
        rc1 = soc_reg_field_validate(unit, DCL_FIFO_DEPTH_TYPEr, FIFO_DEPTH_P_2_TYPE_Nf, value);
        rc2 = soc_reg_field_validate(unit, DCL_DCMB_FIFO_DEPTH_TYPEr, DCMB_FIFO_DEPTH_P_2_TYPE_Nf, value);
    }

    if (SOC_FAILURE(rc1))
    {
        SOCDNX_EXIT_WITH_ERR(rc1, (_BSL_SOCDNX_MSG("value %d isn't a valid for tx fifo size threshold"),value));
    }

    if (SOC_FAILURE(rc2))
    {
        SOCDNX_EXIT_WITH_ERR(rc2, (_BSL_SOCDNX_MSG("value %d isn't a valid for tx fifo size threshold"),value));
    }
   
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t soc_fe3200_fabric_flow_control_tx_fifo_size_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value, int to_check_counter_overflow)
{
    uint32 dcl_dcmb_reg_val,dcl_reg_val;
    int nof_instances_dcl, min_idx, max_idx, idx, array_block, min_idx_dcmb, max_idx_dcmb;
    soc_error_t rc = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    nof_instances_dcl = SOC_DFE_DEFS_GET(unit, nof_instances_dcl);

    if(fe1 && fe3)
    {
        min_idx = 0; 
        max_idx = nof_instances_dcl - 1;
    }
    else if(fe1)
    {
        min_idx = 2; max_idx = 3;
    }
    else
    {
        min_idx = 0; 
        max_idx = 1;
    }

    
    
    if(SOC_DFE_IS_FE13_ASYMMETRIC(unit) && fe3)
    {
        min_idx_dcmb = 0;
        max_idx_dcmb = 3;
    } else {
        min_idx_dcmb = 0;
        max_idx_dcmb = -1;
    }

    if (type_index == soc_dfe_fabric_link_fifo_type_index_0)
    {
        array_block = 0;
    }
    else
    {
        array_block = 1;
    }

    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_soft_init, (unit, SOC_DCMN_RESET_ACTION_IN_RESET));
    SOCDNX_IF_ERR_EXIT(rc);

    
    for (idx = min_idx; idx <= max_idx; idx++)
    {
        SOCDNX_IF_ERR_EXIT(READ_DCL_FIFO_DEPTH_TYPEr(unit, idx, array_block, &dcl_reg_val));

        if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
        {
            soc_reg_field_set(unit, DCL_FIFO_DEPTH_TYPEr, &dcl_reg_val, FIFO_DEPTH_P_0_TYPE_Nf, value);
        }

        if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
        {
            soc_reg_field_set(unit, DCL_FIFO_DEPTH_TYPEr, &dcl_reg_val, FIFO_DEPTH_P_1_TYPE_Nf, value);
        }

        if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,2))
        {
            soc_reg_field_set(unit, DCL_FIFO_DEPTH_TYPEr, &dcl_reg_val, FIFO_DEPTH_P_2_TYPE_Nf, value);
        }

        SOCDNX_IF_ERR_EXIT(WRITE_DCL_FIFO_DEPTH_TYPEr(unit, idx, array_block, dcl_reg_val));
    }
    
    for (idx = min_idx_dcmb; idx <= max_idx_dcmb ; idx++)
    {
        SOCDNX_IF_ERR_EXIT(READ_DCL_DCMB_FIFO_DEPTH_TYPEr(unit, idx, array_block, &dcl_dcmb_reg_val));

        if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
        {
            soc_reg_field_set(unit, DCL_DCMB_FIFO_DEPTH_TYPEr, &dcl_dcmb_reg_val, DCMB_FIFO_DEPTH_P_0_TYPE_Nf, value);
        }

        if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
        {
            soc_reg_field_set(unit, DCL_DCMB_FIFO_DEPTH_TYPEr, &dcl_dcmb_reg_val, DCMB_FIFO_DEPTH_P_1_TYPE_Nf, value);
        }

        if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,2))
        {
            soc_reg_field_set(unit, DCL_DCMB_FIFO_DEPTH_TYPEr, &dcl_dcmb_reg_val, DCMB_FIFO_DEPTH_P_2_TYPE_Nf, value);
        }

        SOCDNX_IF_ERR_EXIT(WRITE_DCL_DCMB_FIFO_DEPTH_TYPEr(unit, idx, array_block, dcl_dcmb_reg_val));
    }

    if (to_check_counter_overflow) {
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_tx_prevent_counter_per_link_overflow (unit, type_index, pipe, fe1, fe3));
    }

    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_soft_init, (unit, SOC_DCMN_RESET_ACTION_OUT_RESET));
    SOCDNX_IF_ERR_EXIT(rc);
  
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t soc_fe3200_fabric_flow_control_tx_fifo_size_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value)
{

    int idx, array_block;
    uint32 reg_val;

    SOCDNX_INIT_FUNC_DEFS;

    if(fe1 && fe3)
    {
        idx = 0; 
    }
    else if(fe1)
    {
        idx = 2;
    }
    else
    {
        idx = 0;
    }

    if (type_index == soc_dfe_fabric_link_fifo_type_index_0)
    {
        array_block = 0;
    }
    else
    {
        array_block = 1;
    }
    
    SOCDNX_IF_ERR_EXIT(READ_DCL_FIFO_DEPTH_TYPEr(unit, idx, array_block, &reg_val));

    if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        *value = soc_reg_field_get(unit, DCL_FIFO_DEPTH_TYPEr, reg_val, FIFO_DEPTH_P_0_TYPE_Nf);
    }
    else if (SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {
        *value = soc_reg_field_get(unit, DCL_FIFO_DEPTH_TYPEr, reg_val, FIFO_DEPTH_P_1_TYPE_Nf);
    }
    else
    {
        *value = soc_reg_field_get(unit, DCL_FIFO_DEPTH_TYPEr, reg_val, FIFO_DEPTH_P_2_TYPE_Nf);
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_fabric_flow_control_mid_fifo_type_set(int unit, int dcm_instance, int fifo_index, soc_dfe_fabric_link_fifo_type_index_t fifo_type)
{
    uint32 reg32_val[1];
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_DCM_FIFO_TYPEr(unit, dcm_instance, reg32_val));
    if (fifo_type == soc_dfe_fabric_link_fifo_type_index_1)
    {
        SHR_BITSET(reg32_val, fifo_index);
    } else 
    {
        SHR_BITCLR(reg32_val, fifo_index);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_DCM_FIFO_TYPEr(unit, dcm_instance, *reg32_val));
    
exit:
    SOCDNX_FUNC_RETURN;
}



STATIC soc_error_t
soc_fe3200_fabric_flow_control_mid_tx_prevent_counter_per_link_overflow (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3)
{
    int nof_pipes, dcl_fifo_size, dcm_fifo_size, dcl_almost_full_threshold, nof_dcm_instances;
    int cell_in_trans_from_RTP = 45, max_dcm_dcl_buffer_used_per_link, max_allowed_dcm_fifo_size;

    SOCDNX_INIT_FUNC_DEFS;

    nof_pipes = SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;

    if ((nof_pipes == 1) && SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe, 0) && SOC_DFE_IS_FE2(unit)) {

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_fifo_size_threshold_get(unit, type_index, pipe, fe1, &dcm_fifo_size));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_fifo_size_threshold_get(unit, type_index, pipe, fe1, fe3, &dcl_fifo_size));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_almost_full_threshold_get(unit, type_index, pipe, fe1, fe3, &dcl_almost_full_threshold));

        
        if (dcl_almost_full_threshold <= dcl_fifo_size) {

            nof_dcm_instances = SOC_DFE_DEFS_GET(unit, nof_instances_dcm);
            max_dcm_dcl_buffer_used_per_link = (dcm_fifo_size + cell_in_trans_from_RTP)*nof_dcm_instances + dcl_fifo_size;

            
            if (max_dcm_dcl_buffer_used_per_link > SOC_FE3200_DCL_DCM_OCCUPANCY_COUNTER_PER_LINK_MAX_VAL) {
                max_allowed_dcm_fifo_size = (SOC_FE3200_DCL_DCM_OCCUPANCY_COUNTER_PER_LINK_MAX_VAL - dcl_fifo_size)/nof_dcm_instances - cell_in_trans_from_RTP;
                LOG_WARN(BSL_LS_SOC_FABRIC,
                                    (BSL_META_U(unit,
                                                " Warning: Need to configure bcmFabricLinkMidFifoSize and bcmFabricLinkMidPrio[0-3]Drop thresholds to be below %d\n"),max_allowed_dcm_fifo_size+1));
            }
        }
    }

    exit:
        SOCDNX_FUNC_RETURN;
}


int
soc_fe3200_init_dch_thresholds_config(int unit, soc_dfe_drv_dch_default_thresholds_t* dch_thresholds_default_values)
{
    int nof_pipes;

    SOCDNX_INIT_FUNC_DEFS;

    nof_pipes = SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;

    switch (nof_pipes)
    {
        case 1:
            dch_thresholds_default_values->fifo_size[0] = SOC_FE3200_DCH_DEFAULT_FIFO_SIZE_SINGLE_PIPE;
            dch_thresholds_default_values->fifo_size[1] = 0;
            dch_thresholds_default_values->fifo_size[2] = 0;
            dch_thresholds_default_values->full_threshold[0] = SOC_FE3200_DCH_DEFAULT_FULL_THRESHOLD_SINGLE_PIPE;
            dch_thresholds_default_values->full_threshold[1] = 0;
            dch_thresholds_default_values->full_threshold[2] = 0;
            dch_thresholds_default_values->llfc_threshold[0] = SOC_FE3200_DCH_DEFAULT_LLFC_THRESHOLD_SINGLE_PIPE;
            dch_thresholds_default_values->llfc_threshold[1] = 0;
            dch_thresholds_default_values->llfc_threshold[2] = 0;
            dch_thresholds_default_values->mc_low_prio_threshold[0] = SOC_FE3200_DCH_DEFAULT_MC_LOW_PRIO_THRESHOLD_SINGLE_PIPE;
            dch_thresholds_default_values->mc_low_prio_threshold[1] = 0;
            dch_thresholds_default_values->mc_low_prio_threshold[2] = 0;
            break;
        case 2:
            dch_thresholds_default_values->fifo_size[0] = SOC_FE3200_DCH_DEFAULT_FIFO_SIZE_DUAL_PIPE;
            dch_thresholds_default_values->fifo_size[1] = SOC_FE3200_DCH_DEFAULT_FIFO_SIZE_DUAL_PIPE;
            dch_thresholds_default_values->fifo_size[2] = 0;
            dch_thresholds_default_values->full_threshold[0] = SOC_FE3200_DCH_DEFAULT_FULL_THRESHOLD_DUAL_PIPE;
            dch_thresholds_default_values->full_threshold[1] = SOC_FE3200_DCH_DEFAULT_FULL_THRESHOLD_DUAL_PIPE;
            dch_thresholds_default_values->full_threshold[2] = 0;
            dch_thresholds_default_values->llfc_threshold[0] = SOC_FE3200_DCH_DEFAULT_LLFC_THRESHOLD_DUAL_PIPE;
            dch_thresholds_default_values->llfc_threshold[1] = SOC_FE3200_DCH_DEFAULT_LLFC_THRESHOLD_DUAL_PIPE;
            dch_thresholds_default_values->llfc_threshold[2] = 0;
            dch_thresholds_default_values->mc_low_prio_threshold[0] = SOC_FE3200_DCH_DEFAULT_MC_LOW_PRIO_THRESHOLD_DUAL_PIPE;
            dch_thresholds_default_values->mc_low_prio_threshold[1] = SOC_FE3200_DCH_DEFAULT_MC_LOW_PRIO_THRESHOLD_DUAL_PIPE;
            dch_thresholds_default_values->mc_low_prio_threshold[2] = 0;
            break;
        case 3:
            dch_thresholds_default_values->fifo_size[0] = SOC_FE3200_DCH_DEFAULT_FIFO_SIZE_TRIPLE_PIPE;
            dch_thresholds_default_values->fifo_size[1] = SOC_FE3200_DCH_DEFAULT_FIFO_SIZE_TRIPLE_PIPE;
            dch_thresholds_default_values->fifo_size[2] = SOC_FE3200_DCH_DEFAULT_FIFO_SIZE_TRIPLE_PIPE;
            dch_thresholds_default_values->full_threshold[0] = SOC_FE3200_DCH_DEFAULT_FULL_THRESHOLD_TRIPLE_PIPE;
            dch_thresholds_default_values->full_threshold[1] = SOC_FE3200_DCH_DEFAULT_FULL_THRESHOLD_TRIPLE_PIPE;
            dch_thresholds_default_values->full_threshold[2] = SOC_FE3200_DCH_DEFAULT_FULL_THRESHOLD_TRIPLE_PIPE;
            dch_thresholds_default_values->llfc_threshold[0] = SOC_FE3200_DCH_DEFAULT_LLFC_THRESHOLD_TRIPLE_PIPE;
            dch_thresholds_default_values->llfc_threshold[1] = SOC_FE3200_DCH_DEFAULT_LLFC_THRESHOLD_TRIPLE_PIPE;
            dch_thresholds_default_values->llfc_threshold[2] = SOC_FE3200_DCH_DEFAULT_LLFC_THRESHOLD_TRIPLE_PIPE;
            dch_thresholds_default_values->mc_low_prio_threshold[0] = SOC_FE3200_DCH_DEFAULT_MC_LOW_PRIO_THRESHOLD_TRIPLE_PIPE;
            dch_thresholds_default_values->mc_low_prio_threshold[1] = SOC_FE3200_DCH_DEFAULT_MC_LOW_PRIO_THRESHOLD_DUAL_PIPE;
            dch_thresholds_default_values->mc_low_prio_threshold[2] = SOC_FE3200_DCH_DEFAULT_MC_LOW_PRIO_THRESHOLD_TRIPLE_PIPE;
            break;
    }

    SOCDNX_FUNC_RETURN;
}


int
soc_fe3200_init_dcm_thresholds_config(int unit, soc_dfe_drv_dcm_default_thresholds_t* dcm_thresholds_default_values)
{
    int nof_pipes;

    SOCDNX_INIT_FUNC_DEFS;

    nof_pipes = SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;

    switch (nof_pipes)
    {
        case 1:
            dcm_thresholds_default_values-> fifo_size[0] = SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_SINGLE_PIPE;
            dcm_thresholds_default_values-> fifo_size[1] = 0;
            dcm_thresholds_default_values-> fifo_size[2] = 0;
            dcm_thresholds_default_values-> prio_0_threshold[0] = SOC_FE3200_DCM_DEFAULT_PRIO_0_DROP_THRESHOLD_SINGLE_PIPE;
            dcm_thresholds_default_values-> prio_0_threshold[1] = 0;
            dcm_thresholds_default_values-> prio_0_threshold[2] = 0;
            dcm_thresholds_default_values-> prio_1_threshold[0] = SOC_FE3200_DCM_DEFAULT_PRIO_1_DROP_THRESHOLD_SINGLE_PIPE;
            dcm_thresholds_default_values-> prio_1_threshold[1] = 0;
            dcm_thresholds_default_values-> prio_1_threshold[2] = 0;
            dcm_thresholds_default_values-> prio_2_threshold[0] = SOC_FE3200_DCM_DEFAULT_PRIO_2_DROP_THRESHOLD_SINGLE_PIPE;
            dcm_thresholds_default_values-> prio_2_threshold[1] = 0;
            dcm_thresholds_default_values-> prio_2_threshold[2] = 0;
            dcm_thresholds_default_values-> prio_3_threshold[0] = SOC_FE3200_DCM_DEFAULT_PRIO_3_DROP_THRESHOLD_SINGLE_PIPE;
            dcm_thresholds_default_values-> prio_3_threshold[1] = 0;
            dcm_thresholds_default_values-> prio_3_threshold[2] = 0;
            dcm_thresholds_default_values-> full_threshold[0] = SOC_FE3200_DCM_DEFAULT_FULL_THRESHOLD_SINGLE_PIPE;
            dcm_thresholds_default_values-> full_threshold[1] = SOC_FE3200_DCM_DEFAULT_FULL_THRESHOLD_SINGLE_PIPE;
            dcm_thresholds_default_values-> full_threshold[2] = SOC_FE3200_DCM_DEFAULT_FULL_THRESHOLD_SINGLE_PIPE;
            dcm_thresholds_default_values-> almost_full_threshold[0] = SOC_FE3200_DCM_DEFAULT_ALMOST_FULL_THRESHOLD_SINGLE_PIPE;
            dcm_thresholds_default_values-> almost_full_threshold[1] = SOC_FE3200_DCM_DEFAULT_ALMOST_FULL_THRESHOLD_SINGLE_PIPE;
            dcm_thresholds_default_values-> almost_full_threshold[2] = SOC_FE3200_DCM_DEFAULT_ALMOST_FULL_THRESHOLD_SINGLE_PIPE;
            dcm_thresholds_default_values-> gci_low_threshold[0] = SOC_FE3200_DCM_DEFAULT_GCI_LOW_THRESHOLD_SINGLE_PIPE;
            dcm_thresholds_default_values-> gci_low_threshold[1] = 0;
            dcm_thresholds_default_values-> gci_low_threshold[2] = 0;
            dcm_thresholds_default_values-> gci_med_threshold[0] = SOC_FE3200_DCM_DEFAULT_GCI_MED_THRESHOLD_SINGLE_PIPE;
            dcm_thresholds_default_values-> gci_med_threshold[1] = 0;
            dcm_thresholds_default_values-> gci_med_threshold[2] = 0;
            dcm_thresholds_default_values-> gci_high_threshold[0] = SOC_FE3200_DCM_DEFAULT_GCI_HIGH_THRESHOLD_SINGLE_PIPE;
            dcm_thresholds_default_values-> gci_high_threshold[1] = 0;
            dcm_thresholds_default_values-> gci_high_threshold[2] = 0;
            dcm_thresholds_default_values-> rci_low_threshold[0] = SOC_FE3200_DCM_DEFAULT_RCI_LOW_THRESHOLD_SINGLE_PIPE;
            dcm_thresholds_default_values-> rci_low_threshold[1] = 0;
            dcm_thresholds_default_values-> rci_low_threshold[2] = 0;
            dcm_thresholds_default_values-> rci_med_threshold[0] = SOC_FE3200_DCM_DEFAULT_RCI_MED_THRESHOLD_SINGLE_PIPE;
            dcm_thresholds_default_values-> rci_med_threshold[1] = 0;
            dcm_thresholds_default_values-> rci_med_threshold[2] = 0;
            dcm_thresholds_default_values-> rci_high_threshold[0] = SOC_FE3200_DCM_DEFAULT_RCI_HIGH_THRESHOLD_SINGLE_PIPE;
            dcm_thresholds_default_values-> rci_high_threshold[1] = 0;
            dcm_thresholds_default_values-> rci_high_threshold[2] = 0;

            break;
        case 2:
            dcm_thresholds_default_values-> fifo_size[0] = SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_DUAL_PIPE;
            dcm_thresholds_default_values-> fifo_size[1] = SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_DUAL_PIPE;
            dcm_thresholds_default_values-> fifo_size[2] = 0;
            dcm_thresholds_default_values-> prio_0_threshold[0] = SOC_FE3200_DCM_DEFAULT_PRIO_0_DROP_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> prio_0_threshold[1] = SOC_FE3200_DCM_DEFAULT_PRIO_0_DROP_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> prio_0_threshold[2] = 0;
            dcm_thresholds_default_values-> prio_1_threshold[0] = SOC_FE3200_DCM_DEFAULT_PRIO_1_DROP_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> prio_1_threshold[1] = SOC_FE3200_DCM_DEFAULT_PRIO_1_DROP_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> prio_1_threshold[2] = 0;
            dcm_thresholds_default_values-> prio_2_threshold[0] = SOC_FE3200_DCM_DEFAULT_PRIO_2_DROP_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> prio_2_threshold[1] = SOC_FE3200_DCM_DEFAULT_PRIO_2_DROP_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> prio_2_threshold[2] = 0;
            dcm_thresholds_default_values-> prio_3_threshold[0] = SOC_FE3200_DCM_DEFAULT_PRIO_3_DROP_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> prio_3_threshold[1] = SOC_FE3200_DCM_DEFAULT_PRIO_3_DROP_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> prio_3_threshold[2] = 0;
            dcm_thresholds_default_values-> full_threshold[0] = SOC_FE3200_DCM_DEFAULT_FULL_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> full_threshold[1] = SOC_FE3200_DCM_DEFAULT_FULL_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> full_threshold[2] = SOC_FE3200_DCM_DEFAULT_FULL_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> almost_full_threshold[0] = SOC_FE3200_DCM_DEFAULT_ALMOST_FULL_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> almost_full_threshold[1] = SOC_FE3200_DCM_DEFAULT_ALMOST_FULL_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> almost_full_threshold[2] = SOC_FE3200_DCM_DEFAULT_ALMOST_FULL_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> gci_low_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> gci_low_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> gci_low_threshold[2] = 0;
            dcm_thresholds_default_values-> gci_med_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> gci_med_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> gci_med_threshold[2] = 0;
            dcm_thresholds_default_values-> gci_high_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> gci_high_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> gci_high_threshold[2] = 0;
            dcm_thresholds_default_values-> rci_low_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> rci_low_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> rci_low_threshold[2] = 0;
            dcm_thresholds_default_values-> rci_med_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> rci_med_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> rci_med_threshold[2] = 0;
            dcm_thresholds_default_values-> rci_high_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> rci_high_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE;
            dcm_thresholds_default_values-> rci_high_threshold[2] = 0;
            break;
        case 3:
            dcm_thresholds_default_values-> fifo_size[0] = SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_TRIPLE_PIPE;
            dcm_thresholds_default_values-> fifo_size[1] = SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_TRIPLE_PIPE;
            dcm_thresholds_default_values-> fifo_size[2] = SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_TRIPLE_PIPE;
            dcm_thresholds_default_values-> prio_0_threshold[0] = SOC_FE3200_DCM_DEFAULT_PRIO_0_DROP_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> prio_0_threshold[1] = SOC_FE3200_DCM_DEFAULT_PRIO_0_DROP_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> prio_0_threshold[2] = SOC_FE3200_DCM_DEFAULT_PRIO_0_DROP_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> prio_1_threshold[0] = SOC_FE3200_DCM_DEFAULT_PRIO_1_DROP_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> prio_1_threshold[1] = SOC_FE3200_DCM_DEFAULT_PRIO_1_DROP_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> prio_1_threshold[2] = SOC_FE3200_DCM_DEFAULT_PRIO_1_DROP_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> prio_2_threshold[0] = SOC_FE3200_DCM_DEFAULT_PRIO_2_DROP_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> prio_2_threshold[1] = SOC_FE3200_DCM_DEFAULT_PRIO_2_DROP_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> prio_2_threshold[2] = SOC_FE3200_DCM_DEFAULT_PRIO_2_DROP_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> prio_3_threshold[0] = SOC_FE3200_DCM_DEFAULT_PRIO_3_DROP_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> prio_3_threshold[1] = SOC_FE3200_DCM_DEFAULT_PRIO_3_DROP_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> prio_3_threshold[2] = SOC_FE3200_DCM_DEFAULT_PRIO_3_DROP_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> full_threshold[0] = SOC_FE3200_DCM_DEFAULT_FULL_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> full_threshold[1] = SOC_FE3200_DCM_DEFAULT_FULL_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> full_threshold[2] = SOC_FE3200_DCM_DEFAULT_FULL_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> almost_full_threshold[0] = SOC_FE3200_DCM_DEFAULT_ALMOST_FULL_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> almost_full_threshold[1] = SOC_FE3200_DCM_DEFAULT_ALMOST_FULL_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> almost_full_threshold[2] = SOC_FE3200_DCM_DEFAULT_ALMOST_FULL_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> gci_low_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> gci_low_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> gci_low_threshold[2] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCM_DEFAULT_GCI_LOW_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> gci_med_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> gci_med_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> gci_med_threshold[2] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCM_DEFAULT_GCI_MED_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> gci_high_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> gci_high_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> gci_high_threshold[2] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCM_DEFAULT_GCI_HIGH_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> rci_low_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> rci_low_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> rci_low_threshold[2] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCM_DEFAULT_RCI_LOW_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> rci_med_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> rci_med_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> rci_med_threshold[2] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCM_DEFAULT_RCI_MED_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> rci_high_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> rci_high_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcm_thresholds_default_values-> rci_high_threshold[2] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCM_DEFAULT_RCI_HIGH_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            break;
    }

    if (SOC_DFE_IS_FE13(unit) && SOC_DFE_CONFIG(unit).fabric_local_routing_enable)
    {
        switch (nof_pipes)
        {
            case 1:
                dcm_thresholds_default_values-> local_switch_fifo_size[0] = SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_LOCAL_SWITCH_SINGLE_PIPE;
                dcm_thresholds_default_values-> local_switch_fifo_size[1] = 0;
                dcm_thresholds_default_values-> local_switch_fifo_size[2] = 0;

                dcm_thresholds_default_values-> local_switch_almost_full_0_threshold[0] = dcm_thresholds_default_values-> local_switch_fifo_size[0] - SOC_FE3200_DCM_LOCAL_SWITCH_ALMOST_FULL_OFFSET;
                dcm_thresholds_default_values-> local_switch_almost_full_0_threshold[1] = SOC_FE3200_DCM_MAX_THRESHOLD;
                dcm_thresholds_default_values-> local_switch_almost_full_0_threshold[2] = SOC_FE3200_DCM_MAX_THRESHOLD;

                dcm_thresholds_default_values-> local_switch_almost_full_1_threshold[0] = dcm_thresholds_default_values-> local_switch_fifo_size[0]/2;
                dcm_thresholds_default_values-> local_switch_almost_full_1_threshold[1] = SOC_FE3200_DCM_MAX_THRESHOLD;
                dcm_thresholds_default_values-> local_switch_almost_full_1_threshold[2] = SOC_FE3200_DCM_MAX_THRESHOLD;

                
                break;
            case 2:
                dcm_thresholds_default_values-> local_switch_fifo_size[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm) ? SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_LOCAL_SWITCH_IP_TDM_DUAL_PIPE : SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_LOCAL_SWITCH_UC_MC_DUAL_PIPE_0;
                dcm_thresholds_default_values-> local_switch_fifo_size[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm) ? SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_LOCAL_SWITCH_IP_TDM_DUAL_PIPE : SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_LOCAL_SWITCH_UC_MC_DUAL_PIPE_1;
                dcm_thresholds_default_values-> local_switch_fifo_size[2] = 0;

                dcm_thresholds_default_values-> local_switch_almost_full_0_threshold[0] = dcm_thresholds_default_values-> local_switch_fifo_size[0] - SOC_FE3200_DCM_LOCAL_SWITCH_ALMOST_FULL_OFFSET;
                dcm_thresholds_default_values-> local_switch_almost_full_0_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm) ? dcm_thresholds_default_values-> local_switch_fifo_size[1] - SOC_FE3200_DCM_LOCAL_SWITCH_ALMOST_FULL_OFFSET  : SOC_FE3200_DCM_MAX_THRESHOLD;
                dcm_thresholds_default_values-> local_switch_almost_full_0_threshold[2] = SOC_FE3200_DCM_MAX_THRESHOLD;

                dcm_thresholds_default_values-> local_switch_almost_full_1_threshold[0] = dcm_thresholds_default_values-> local_switch_fifo_size[0]/2;
                dcm_thresholds_default_values-> local_switch_almost_full_1_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm) ? dcm_thresholds_default_values-> local_switch_fifo_size[1]/2 : SOC_FE3200_DCM_MAX_THRESHOLD;
                dcm_thresholds_default_values-> local_switch_almost_full_1_threshold[2] = SOC_FE3200_DCM_MAX_THRESHOLD;
               
                break;
            case 3:
                dcm_thresholds_default_values-> local_switch_fifo_size[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc) ? SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE_0 : SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE_0;
                dcm_thresholds_default_values-> local_switch_fifo_size[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc) ? SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE_1 : SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE_1;
                dcm_thresholds_default_values-> local_switch_fifo_size[2] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc) ? SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE_2 : SOC_FE3200_DCM_DEFAULT_FIFO_SIZE_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE_2;

                dcm_thresholds_default_values-> local_switch_almost_full_0_threshold[0] = dcm_thresholds_default_values-> local_switch_fifo_size[0] - SOC_FE3200_DCM_LOCAL_SWITCH_ALMOST_FULL_OFFSET;
                dcm_thresholds_default_values-> local_switch_almost_full_0_threshold[1] = SOC_FE3200_DCM_MAX_THRESHOLD;
                dcm_thresholds_default_values-> local_switch_almost_full_0_threshold[2] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc) ? SOC_FE3200_DCM_MAX_THRESHOLD : dcm_thresholds_default_values-> local_switch_fifo_size[2] - SOC_FE3200_DCM_LOCAL_SWITCH_ALMOST_FULL_OFFSET;

                dcm_thresholds_default_values-> local_switch_almost_full_1_threshold[0] = dcm_thresholds_default_values-> local_switch_fifo_size[0]/2;
                dcm_thresholds_default_values-> local_switch_almost_full_1_threshold[1] = SOC_FE3200_DCM_MAX_THRESHOLD;
                dcm_thresholds_default_values-> local_switch_almost_full_1_threshold[2] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc) ? SOC_FE3200_DCM_MAX_THRESHOLD : dcm_thresholds_default_values-> local_switch_fifo_size[2]/2 ;
                

                break;
            default:
                break;
        }
    }

    SOCDNX_FUNC_RETURN;
}

int
soc_fe3200_init_dcl_thresholds_config(int unit, soc_dfe_drv_dcl_default_thresholds_t* dcl_thresholds_default_values)
{
    int nof_pipes;

    SOCDNX_INIT_FUNC_DEFS;

    nof_pipes = SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;

    switch (nof_pipes)
    {

        case 1:
            dcl_thresholds_default_values-> fifo_size[0] = SOC_FE3200_DCL_DEFAULT_FIFO_SIZE_SINGLE_PIPE;
            dcl_thresholds_default_values-> fifo_size[1] = 0;
            dcl_thresholds_default_values-> fifo_size[2] = 0;
            dcl_thresholds_default_values-> prio_0_threshold[0] = SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_THERSHOLD_SINGLE_PIPE;
            dcl_thresholds_default_values-> prio_0_threshold[1] = 0;
            dcl_thresholds_default_values-> prio_0_threshold[2] = 0;
            dcl_thresholds_default_values-> prio_1_threshold[0] = SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_THRESHOLD_SINGLE_PIPE;
            dcl_thresholds_default_values-> prio_1_threshold[1] = 0;
            dcl_thresholds_default_values-> prio_1_threshold[2] = 0;
            dcl_thresholds_default_values-> prio_2_threshold[0] = SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_THRESHOLD_SINGLE_PIPE;
            dcl_thresholds_default_values-> prio_2_threshold[1] = 0;
            dcl_thresholds_default_values-> prio_2_threshold[2] = 0;
            dcl_thresholds_default_values-> prio_3_threshold[0] = SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_THRESHOLD_SINGLE_PIPE;
            dcl_thresholds_default_values-> prio_3_threshold[1] = 0;
            dcl_thresholds_default_values-> prio_3_threshold[2] = 0;
            dcl_thresholds_default_values-> almost_full_threshold[0] = SOC_FE3200_DCL_DEFAULT_ALMOST_FULL_THRESHOLD_SINGLE_PIPE;
            dcl_thresholds_default_values-> almost_full_threshold[1] = 0;
            dcl_thresholds_default_values-> almost_full_threshold[2] = 0;
            dcl_thresholds_default_values-> llfc_threshold[0] = SOC_FE3200_DCL_DEFAULT_LLFC_THRESHOLD_SINGLE_PIPE;
            dcl_thresholds_default_values-> llfc_threshold[1] = 0;
            dcl_thresholds_default_values-> llfc_threshold[2] = 0;
            dcl_thresholds_default_values-> gci_low_threshold[0] = SOC_FE3200_DCL_DEFAULT_GCI_LOW_THRESHOLD_SINGLE_PIPE;
            dcl_thresholds_default_values-> gci_low_threshold[1] = 0;
            dcl_thresholds_default_values-> gci_low_threshold[2] = 0;
            dcl_thresholds_default_values-> gci_med_threshold[0] = SOC_FE3200_DCL_DEFAULT_GCI_MED_THRESHOLD_SINGLE_PIPE;
            dcl_thresholds_default_values-> gci_med_threshold[1] = 0;
            dcl_thresholds_default_values-> gci_med_threshold[2] = 0;
            dcl_thresholds_default_values-> gci_high_threshold[0] = SOC_FE3200_DCL_DEFAULT_GCI_HIGH_THRESHOLD_SINGLE_PIPE;
            dcl_thresholds_default_values-> gci_high_threshold[1] = 0;
            dcl_thresholds_default_values-> gci_high_threshold[2] = 0;
            dcl_thresholds_default_values-> rci_low_threshold[0] = SOC_FE3200_DCL_DEFAULT_RCI_LOW_PIPE_0_THRESHOLD_SINGLE_PIPE;
            dcl_thresholds_default_values-> rci_low_threshold[1] = 0;
            dcl_thresholds_default_values-> rci_low_threshold[2] = 0;
            dcl_thresholds_default_values-> rci_med_threshold[0] = SOC_FE3200_DCL_DEFAULT_RCI_MED_PIPE_0_THRESHOLD_SINGLE_PIPE;
            dcl_thresholds_default_values-> rci_med_threshold[1] = 0;
            dcl_thresholds_default_values-> rci_med_threshold[2] = 0;
            dcl_thresholds_default_values-> rci_high_threshold[0] = SOC_FE3200_DCL_DEFAULT_RCI_HIGH_PIPE_0_THRESHOLD_SINGLE_PIPE;
            dcl_thresholds_default_values-> rci_high_threshold[1] = 0;
            dcl_thresholds_default_values-> rci_high_threshold[2] = 0;
            break;
        case 2:            
            dcl_thresholds_default_values-> fifo_size[0] = SOC_FE3200_DCL_DEFAULT_FIFO_SIZE_DUAL_PIPE;
            dcl_thresholds_default_values-> fifo_size[1] = SOC_FE3200_DCL_DEFAULT_FIFO_SIZE_DUAL_PIPE;
            dcl_thresholds_default_values-> fifo_size[2] = 0;
            dcl_thresholds_default_values-> prio_0_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE;
            dcl_thresholds_default_values-> prio_0_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE;
            dcl_thresholds_default_values-> prio_0_threshold[2] = 0;
            dcl_thresholds_default_values-> prio_1_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE;
            dcl_thresholds_default_values-> prio_1_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE;
            dcl_thresholds_default_values-> prio_1_threshold[2] = 0;
            dcl_thresholds_default_values-> prio_2_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE;
            dcl_thresholds_default_values-> prio_2_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE;
            dcl_thresholds_default_values-> prio_2_threshold[2] = 0;
            dcl_thresholds_default_values-> prio_3_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE;
            dcl_thresholds_default_values-> prio_3_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE;
            dcl_thresholds_default_values-> prio_3_threshold[2] = 0;
            dcl_thresholds_default_values-> almost_full_threshold[0] = SOC_FE3200_DCL_DEFAULT_ALMOST_FULL_THRESHOLD_DUAL_PIPE;
            dcl_thresholds_default_values-> almost_full_threshold[1] = SOC_FE3200_DCL_DEFAULT_ALMOST_FULL_THRESHOLD_DUAL_PIPE;
            dcl_thresholds_default_values-> almost_full_threshold[2] = SOC_FE3200_DCL_DEFAULT_ALMOST_FULL_THRESHOLD_DUAL_PIPE;
            dcl_thresholds_default_values-> llfc_threshold[0] = SOC_FE3200_DCL_DEFAULT_LLFC_THRESHOLD_DUAL_PIPE;
            dcl_thresholds_default_values-> llfc_threshold[1] = SOC_FE3200_DCL_DEFAULT_LLFC_THRESHOLD_DUAL_PIPE;
            dcl_thresholds_default_values-> llfc_threshold[2] = 0;
            dcl_thresholds_default_values-> gci_low_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE;
            dcl_thresholds_default_values-> gci_low_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE;
            dcl_thresholds_default_values-> gci_low_threshold[2] = 0;
            dcl_thresholds_default_values-> gci_med_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE;
            dcl_thresholds_default_values-> gci_med_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE;
            dcl_thresholds_default_values-> gci_med_threshold[2] = 0;
            dcl_thresholds_default_values-> gci_high_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE;
            dcl_thresholds_default_values-> gci_high_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_1_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_1_MC_UC_THRESHOLD_DUAL_PIPE;
            dcl_thresholds_default_values-> gci_high_threshold[2] = 0;
            dcl_thresholds_default_values-> rci_low_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCL_DEFAULT_RCI_LOW_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCL_DEFAULT_RCI_LOW_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE;
            dcl_thresholds_default_values-> rci_low_threshold[1] = SOC_FE3200_DCL_DEFAULT_RCI_THRESHOLD_PIPE_1;
            dcl_thresholds_default_values-> rci_low_threshold[2] = 0;
            dcl_thresholds_default_values-> rci_med_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCL_DEFAULT_RCI_MED_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCL_DEFAULT_RCI_MED_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE;
            dcl_thresholds_default_values-> rci_med_threshold[1] = SOC_FE3200_DCL_DEFAULT_RCI_THRESHOLD_PIPE_1;
            dcl_thresholds_default_values-> rci_med_threshold[2] = 0;
            dcl_thresholds_default_values-> rci_high_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm)? SOC_FE3200_DCL_DEFAULT_RCI_HIGH_PIPE_0_IP_TDM_THRESHOLD_DUAL_PIPE:SOC_FE3200_DCL_DEFAULT_RCI_HIGH_PIPE_0_MC_UC_THRESHOLD_DUAL_PIPE;;
            dcl_thresholds_default_values-> rci_high_threshold[1] = SOC_FE3200_DCL_DEFAULT_RCI_THRESHOLD_PIPE_1;
            dcl_thresholds_default_values-> rci_high_threshold[2] = 0;
            break;
        case 3:  
            dcl_thresholds_default_values-> fifo_size[0] = SOC_FE3200_DCL_DEFAULT_FIFO_SIZE_TRIPLE_PIPE;
            dcl_thresholds_default_values-> fifo_size[1] = SOC_FE3200_DCL_DEFAULT_FIFO_SIZE_TRIPLE_PIPE;
            dcl_thresholds_default_values-> fifo_size[2] = SOC_FE3200_DCL_DEFAULT_FIFO_SIZE_TRIPLE_PIPE;
            dcl_thresholds_default_values-> prio_0_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)?SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcl_thresholds_default_values-> prio_0_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)?SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcl_thresholds_default_values-> prio_0_threshold[2] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)?SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCL_DEFAULT_PRIO_0_DROP_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcl_thresholds_default_values-> prio_1_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)?SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcl_thresholds_default_values-> prio_1_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)?SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcl_thresholds_default_values-> prio_1_threshold[2] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)?SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCL_DEFAULT_PRIO_1_DROP_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcl_thresholds_default_values-> prio_2_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)?SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcl_thresholds_default_values-> prio_2_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)?SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcl_thresholds_default_values-> prio_2_threshold[2] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)?SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCL_DEFAULT_PRIO_2_DROP_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcl_thresholds_default_values-> prio_3_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)?SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcl_thresholds_default_values-> prio_3_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)?SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcl_thresholds_default_values-> prio_3_threshold[2] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)?SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCL_DEFAULT_PRIO_3_DROP_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE;
            dcl_thresholds_default_values-> almost_full_threshold[0] = SOC_FE3200_DCL_DEFAULT_ALMOST_FULL_THRESHOLD_TRIPLE_PIPE;
            dcl_thresholds_default_values-> almost_full_threshold[1] = SOC_FE3200_DCL_DEFAULT_ALMOST_FULL_THRESHOLD_TRIPLE_PIPE;
            dcl_thresholds_default_values-> almost_full_threshold[2] = SOC_FE3200_DCL_DEFAULT_ALMOST_FULL_THRESHOLD_TRIPLE_PIPE;
            dcl_thresholds_default_values-> llfc_threshold[0] = SOC_FE3200_DCL_DEFAULT_LLFC_THRESHOLD_TRIPLE_PIPE;
            dcl_thresholds_default_values-> llfc_threshold[1] = SOC_FE3200_DCL_DEFAULT_LLFC_THRESHOLD_TRIPLE_PIPE;
            dcl_thresholds_default_values-> llfc_threshold[2] = SOC_FE3200_DCL_DEFAULT_LLFC_THRESHOLD_TRIPLE_PIPE;
            dcl_thresholds_default_values-> gci_low_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE ;
            dcl_thresholds_default_values-> gci_low_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE ;
            dcl_thresholds_default_values-> gci_low_threshold[2] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCL_DEFAULT_GCI_LOW_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE ;
            dcl_thresholds_default_values-> gci_med_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE ;
            dcl_thresholds_default_values-> gci_med_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE ;
            dcl_thresholds_default_values-> gci_med_threshold[2] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCL_DEFAULT_GCI_MED_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE ;
            dcl_thresholds_default_values-> gci_high_threshold[0] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_0_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_0_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE ;
            dcl_thresholds_default_values-> gci_high_threshold[1] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_1_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_1_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE ;
            dcl_thresholds_default_values-> gci_high_threshold[2] = (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)? SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_2_UC_MCH_MCL_THRESHOLD_TRIPLE_PIPE:SOC_FE3200_DCL_DEFAULT_GCI_HIGH_PIPE_2_UC_MC_TDM_THRESHOLD_TRIPLE_PIPE ;
            dcl_thresholds_default_values-> rci_low_threshold[0] = SOC_FE3200_DCL_DEFAULT_RCI_LOW_PIPE_0_TRIPLE_PIPE;
            dcl_thresholds_default_values-> rci_low_threshold[1] = SOC_FE3200_DCL_DEFAULT_RCI_THRESHOLD_PIPE_1;
            dcl_thresholds_default_values-> rci_low_threshold[2] = SOC_FE3200_DCL_DEFAULT_RCI_THRESHOLD_PIPE_2;
            dcl_thresholds_default_values-> rci_med_threshold[0] = SOC_FE3200_DCL_DEFAULT_RCI_MED_PIPE_0_TRIPLE_PIPE;
            dcl_thresholds_default_values-> rci_med_threshold[1] = SOC_FE3200_DCL_DEFAULT_RCI_THRESHOLD_PIPE_1;
            dcl_thresholds_default_values-> rci_med_threshold[2] = SOC_FE3200_DCL_DEFAULT_RCI_THRESHOLD_PIPE_2;
            dcl_thresholds_default_values-> rci_high_threshold[0] = SOC_FE3200_DCL_DEFAULT_RCI_HIGH_PIPE_0_TRIPLE_PIPE;
            dcl_thresholds_default_values-> rci_high_threshold[1] = SOC_FE3200_DCL_DEFAULT_RCI_THRESHOLD_PIPE_1;
            dcl_thresholds_default_values-> rci_high_threshold[2] = SOC_FE3200_DCL_DEFAULT_RCI_THRESHOLD_PIPE_2;
            break;
    }

    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME
