/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE1600 FABRIC FLOW CONTROL
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC

#include <shared/bsl.h>

#include <soc/dcmn/error.h>

#include <bcm/types.h>
#include <soc/defs.h>

#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>
#include <soc/error.h>
#include <soc/dfe/cmn/dfe_drv.h>

#include <shared/bitop.h>

#if defined(BCM_88750_A0)
 
#include <soc/dfe/fe1600/fe1600_fabric_flow_control.h>
#include <soc/dfe/fe1600/fe1600_defs.h>


soc_error_t 
soc_fe1600_fabric_flow_control_rci_gci_control_source_set(int unit, bcm_fabric_control_t type, soc_dcmn_fabric_pipe_t val) 
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;
	SOC_FE1600_ONLY(unit);
    
    
   
    SOCDNX_IF_ERR_EXIT(READ_DCMC_DCM_ENABLERS_REGISTERr(unit, &reg_val));
    
    switch(type)
    {
        case bcmFabricRCIControlSource:
            soc_reg_field_set(unit, DCMC_DCM_ENABLERS_REGISTERr, &reg_val, RCI_SOURCEf, val-1); 
            break;
        
        case bcmFabricGCIControlSource:
            soc_reg_field_set(unit, DCMC_DCM_ENABLERS_REGISTERr, &reg_val, GCI_SOURCEf, val-1);
            break;
        
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong control source type %d"),type)); 
    }
    
    SOCDNX_IF_ERR_EXIT(WRITE_DCMC_DCM_ENABLERS_REGISTERr(unit, reg_val));
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe1600_fabric_flow_control_rci_gci_control_source_get(int unit, bcm_fabric_control_t type, soc_dcmn_fabric_pipe_t* val) 
{
    uint32 reg_val, enabler_val,mask=0;
    int nof_pipes;
    SOCDNX_INIT_FUNC_DEFS;
	SOC_FE1600_ONLY(unit);

    nof_pipes=SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;
    SOC_DCMN_FABRIC_PIPE_ALL_PIPES_SET(&mask,nof_pipes); 
    
    SOCDNX_IF_ERR_EXIT(READ_DCMC_DCM_ENABLERS_REGISTERr(unit, &reg_val));
      
    switch(type)
    {
        case bcmFabricRCIControlSource:
            enabler_val = soc_reg_field_get(unit, DCMC_DCM_ENABLERS_REGISTERr, reg_val, RCI_SOURCEf);
            break;
        
        case bcmFabricGCIControlSource:
            enabler_val = soc_reg_field_get(unit, DCMC_DCM_ENABLERS_REGISTERr, reg_val, GCI_SOURCEf);
            break;
        
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong control source type %d"),type)); 
    }
    
    
    enabler_val=SOC_DCMN_FABRIC_PIPE_ALL_PIPES_GET(enabler_val,mask);
    *val=enabler_val+1;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe1600_fabric_flow_control_thresholds_flags_validate(int unit,uint32 flags){
    uint32 valid_flags= BCM_FABRIC_LINK_TH_FE1_LINKS_ONLY | BCM_FABRIC_LINK_TH_FE3_LINKS_ONLY | BCM_FABRIC_LINK_TH_PRIM_ONLY |
                        BCM_FABRIC_LINK_TH_SCND_ONLY | BCM_FABRIC_LINK_THRESHOLD_WITH_ID | BCM_FABRIC_LINK_THRESHOLD_RX_FIFO_ONLY |
                        BCM_FABRIC_LINK_THRESHOLD_TX_FIFO_ONLY;
    SOCDNX_INIT_FUNC_DEFS;

    if (~valid_flags & flags )
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid flags for threshold handle")));
    }
      
exit:
    SOCDNX_FUNC_RETURN;
}

      
soc_error_t 
soc_fe1600_fabric_flow_control_rx_llfc_threshold_validate(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value)
{
    soc_error_t rc = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    { 
        if(type_index == soc_dfe_fabric_link_fifo_type_index_0){
            rc = soc_reg_field_validate(unit,DCH_LINK_LEVEL_FLOW_CONTROL_Pr, LNK_LVL_FC_TH_0_Pf, value);
        } else {
            rc = soc_reg_field_validate(unit,DCH_LINK_LEVEL_FLOW_CONTROL_Pr, LNK_LVL_FC_TH_1_Pf, value); 
        }
    }
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for RX LLFC threshold"),value)); 
    }
    
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {       
        if(type_index == soc_dfe_fabric_link_fifo_type_index_0) {
            rc = soc_reg_field_validate(unit,DCH_LINK_LEVEL_FLOW_CONTROL_Sr, LNK_LVL_FC_TH_0_Sf, value);
        } else {
            rc = soc_reg_field_validate(unit,DCH_LINK_LEVEL_FLOW_CONTROL_Sr, LNK_LVL_FC_TH_1_Sf, value); 
        }
    }
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for RX LLFC threshold"),value)); 
    }
    
      
exit:
    SOCDNX_FUNC_RETURN;
}

      
soc_error_t 
soc_fe1600_fabric_flow_control_rx_llfc_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value)
{
    int min_idx, max_idx,idx;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    
    if(fe1 && fe3)
        {min_idx = 0; max_idx = SOC_DFE_DEFS_GET(unit, nof_instances_dch) - 1;}
    else if(fe1)
        {min_idx = 0; max_idx = 1;}
    else
        {min_idx = 2; max_idx = 3;}

   
    
    
    for(idx = min_idx ; idx <= max_idx ; idx++) {

        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_Pr(unit, idx, &reg_val));
            
            if(type_index == soc_dfe_fabric_link_fifo_type_index_0)
                soc_reg_field_set(unit,DCH_LINK_LEVEL_FLOW_CONTROL_Pr,&reg_val,LNK_LVL_FC_TH_0_Pf,value);
            else
                soc_reg_field_set(unit,DCH_LINK_LEVEL_FLOW_CONTROL_Pr,&reg_val,LNK_LVL_FC_TH_1_Pf,value); 
           
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_Pr(unit, idx, reg_val));
        }
        
        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_Sr(unit, idx, &reg_val));
            
            if(type_index == soc_dfe_fabric_link_fifo_type_index_0)
                soc_reg_field_set(unit,DCH_LINK_LEVEL_FLOW_CONTROL_Sr,&reg_val,LNK_LVL_FC_TH_0_Sf,value);
            else
                soc_reg_field_set(unit,DCH_LINK_LEVEL_FLOW_CONTROL_Sr,&reg_val,LNK_LVL_FC_TH_1_Sf,value); 
            
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_Sr(unit, idx, reg_val));
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

   
soc_error_t 
soc_fe1600_fabric_flow_control_rx_llfc_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value)
{
    int blk_id;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);
    
    if(fe1)
        blk_id = 0;
    else
        blk_id = 2;
    
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_Pr(unit, blk_id, &reg_val));
        
        if(type_index == soc_dfe_fabric_link_fifo_type_index_0)
            *value = soc_reg_field_get(unit,DCH_LINK_LEVEL_FLOW_CONTROL_Pr,reg_val,LNK_LVL_FC_TH_0_Pf);
        else
            *value = soc_reg_field_get(unit,DCH_LINK_LEVEL_FLOW_CONTROL_Pr,reg_val,LNK_LVL_FC_TH_1_Pf); 
    }
    else 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_Sr(unit, blk_id, &reg_val));
        
        if(type_index == soc_dfe_fabric_link_fifo_type_index_0)
            *value = soc_reg_field_get(unit,DCH_LINK_LEVEL_FLOW_CONTROL_Sr,reg_val,LNK_LVL_FC_TH_0_Sf);
        else
            *value = soc_reg_field_get(unit,DCH_LINK_LEVEL_FLOW_CONTROL_Sr,reg_val,LNK_LVL_FC_TH_1_Sf); 
    }
  
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe1600_fabric_flow_control_rx_gci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value, int is_fe1, int is_fe3)
{
    soc_error_t rc = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
        {
            switch(type)
            {
                case bcmFabricLinkRxGciLvl1FC: rc = soc_reg_field_validate(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Pr, DCH_LOCAL_GCI_0_TYPE_0_TH_Pf, value); break;
                case bcmFabricLinkRxGciLvl2FC: rc = soc_reg_field_validate(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Pr, DCH_LOCAL_GCI_1_TYPE_0_TH_Pf, value); break;
                case bcmFabricLinkRxGciLvl3FC: rc = soc_reg_field_validate(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Pr, DCH_LOCAL_GCI_2_TYPE_0_TH_Pf, value); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
            }
        }
        else
        {     
            switch(type)
            {
                case bcmFabricLinkRxGciLvl1FC: rc = soc_reg_field_validate(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Pr, DCH_LOCAL_GCI_0_TYPE_1_TH_Pf, value); break;
                case bcmFabricLinkRxGciLvl2FC: rc = soc_reg_field_validate(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Pr, DCH_LOCAL_GCI_1_TYPE_1_TH_Pf, value); break;
                case bcmFabricLinkRxGciLvl3FC: rc = soc_reg_field_validate(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Pr, DCH_LOCAL_GCI_2_TYPE_1_TH_Pf, value); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
            }
        }
    }
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for RX GCI threshold"),value)); 
    }

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
        {    
            switch(type)
            {
                case bcmFabricLinkRxGciLvl1FC: rc = soc_reg_field_validate(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Sr, DCH_LOCAL_GCI_0_TYPE_0_TH_Sf, value); break;
                case bcmFabricLinkRxGciLvl2FC: rc = soc_reg_field_validate(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Sr, DCH_LOCAL_GCI_1_TYPE_0_TH_Sf, value); break;
                case bcmFabricLinkRxGciLvl3FC: rc = soc_reg_field_validate(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Sr, DCH_LOCAL_GCI_2_TYPE_0_TH_Sf, value); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
            }
        }
        else
        {
            switch(type)
            {
                case bcmFabricLinkRxGciLvl1FC: rc = soc_reg_field_validate(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Sr, DCH_LOCAL_GCI_0_TYPE_1_TH_Sf, value); break;
                case bcmFabricLinkRxGciLvl2FC: rc = soc_reg_field_validate(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Sr, DCH_LOCAL_GCI_1_TYPE_1_TH_Sf, value); break;
                case bcmFabricLinkRxGciLvl3FC: rc = soc_reg_field_validate(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Sr, DCH_LOCAL_GCI_2_TYPE_1_TH_Sf, value); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
            }
        }
    }
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for RX GCI threshold"),value)); 
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe1600_fabric_flow_control_rx_gci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value)
{
    int min_idx, max_idx,idx;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    
    if(fe1 && fe3)
        {min_idx = 0; max_idx = SOC_DFE_DEFS_GET(unit, nof_instances_dch) - 1;}
    else if(fe1)
        {min_idx = 0; max_idx = 1;}
    else
        {min_idx = 2; max_idx = 3;}
    
    
    for(idx = min_idx ; idx <= max_idx ; idx++) {
        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
        {
            if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_LOCAL_GCI_TYPE_0_TH_Pr(unit, idx, &reg_val));
                
                switch(type)
                {
                    case bcmFabricLinkRxGciLvl1FC: soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Pr,&reg_val, DCH_LOCAL_GCI_0_TYPE_0_TH_Pf, value); break;
                    case bcmFabricLinkRxGciLvl2FC: soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Pr,&reg_val, DCH_LOCAL_GCI_1_TYPE_0_TH_Pf, value); break;
                    case bcmFabricLinkRxGciLvl3FC: soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Pr,&reg_val, DCH_LOCAL_GCI_2_TYPE_0_TH_Pf, value); break;
                    default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
                }
                
                SOCDNX_IF_ERR_EXIT(WRITE_DCH_DCH_LOCAL_GCI_TYPE_0_TH_Pr(unit, idx, reg_val));
            }
            else
            {
                SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_LOCAL_GCI_TYPE_1_TH_Pr(unit, idx, &reg_val));
                
                switch(type)
                {
                    case bcmFabricLinkRxGciLvl1FC: soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Pr,&reg_val, DCH_LOCAL_GCI_0_TYPE_1_TH_Pf, value); break;
                    case bcmFabricLinkRxGciLvl2FC: soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Pr,&reg_val, DCH_LOCAL_GCI_1_TYPE_1_TH_Pf, value); break;
                    case bcmFabricLinkRxGciLvl3FC: soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Pr,&reg_val, DCH_LOCAL_GCI_2_TYPE_1_TH_Pf, value); break;
                    default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
                }
                
                SOCDNX_IF_ERR_EXIT(WRITE_DCH_DCH_LOCAL_GCI_TYPE_1_TH_Pr(unit, idx, reg_val));
            }
        }
        
        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
        {
            if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_LOCAL_GCI_TYPE_0_TH_Sr(unit, idx, &reg_val));
                
                switch(type)
                {
                    case bcmFabricLinkRxGciLvl1FC: soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Sr,&reg_val, DCH_LOCAL_GCI_0_TYPE_0_TH_Sf, value); break;
                    case bcmFabricLinkRxGciLvl2FC: soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Sr,&reg_val, DCH_LOCAL_GCI_1_TYPE_0_TH_Sf, value); break;
                    case bcmFabricLinkRxGciLvl3FC: soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Sr,&reg_val, DCH_LOCAL_GCI_2_TYPE_0_TH_Sf, value); break;
                    default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
                }
                 
                SOCDNX_IF_ERR_EXIT(WRITE_DCH_DCH_LOCAL_GCI_TYPE_0_TH_Sr(unit, idx, reg_val));
            }
            else
            {
                SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_LOCAL_GCI_TYPE_1_TH_Sr(unit, idx, &reg_val));
                
                switch(type)
                {
                    case bcmFabricLinkRxGciLvl1FC: soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Sr,&reg_val, DCH_LOCAL_GCI_0_TYPE_1_TH_Sf, value); break;
                    case bcmFabricLinkRxGciLvl2FC: soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Sr,&reg_val, DCH_LOCAL_GCI_1_TYPE_1_TH_Sf, value); break;
                    case bcmFabricLinkRxGciLvl3FC: soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Sr,&reg_val, DCH_LOCAL_GCI_2_TYPE_1_TH_Sf, value); break;
                    default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
                }
                 
                SOCDNX_IF_ERR_EXIT(WRITE_DCH_DCH_LOCAL_GCI_TYPE_1_TH_Sr(unit, idx, reg_val));
            }
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe1600_fabric_flow_control_rx_gci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value)
{
    int blk_id;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    
    if(fe1)
        blk_id = 0;
    else
        blk_id = 2;
        
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_LOCAL_GCI_TYPE_0_TH_Pr(unit, blk_id, &reg_val));
            
            switch(type)
            {
                case bcmFabricLinkRxGciLvl1FC: *value = soc_reg_field_get(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Pr,reg_val, DCH_LOCAL_GCI_0_TYPE_0_TH_Pf); break;
                case bcmFabricLinkRxGciLvl2FC: *value = soc_reg_field_get(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Pr,reg_val, DCH_LOCAL_GCI_1_TYPE_0_TH_Pf); break;
                case bcmFabricLinkRxGciLvl3FC: *value = soc_reg_field_get(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Pr,reg_val, DCH_LOCAL_GCI_2_TYPE_0_TH_Pf); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
            }
        }
        else
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_LOCAL_GCI_TYPE_1_TH_Pr(unit, blk_id, &reg_val));
            
            switch(type)
            {
                case bcmFabricLinkRxGciLvl1FC: *value = soc_reg_field_get(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Pr,reg_val, DCH_LOCAL_GCI_0_TYPE_1_TH_Pf); break;
                case bcmFabricLinkRxGciLvl2FC: *value = soc_reg_field_get(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Pr,reg_val, DCH_LOCAL_GCI_1_TYPE_1_TH_Pf); break;
                case bcmFabricLinkRxGciLvl3FC: *value = soc_reg_field_get(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Pr,reg_val, DCH_LOCAL_GCI_2_TYPE_1_TH_Pf); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
            }
        }
    }
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_LOCAL_GCI_TYPE_0_TH_Sr(unit, blk_id, &reg_val));
            
            switch(type)
            {
                case bcmFabricLinkRxGciLvl1FC: *value = soc_reg_field_get(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Sr, reg_val, DCH_LOCAL_GCI_0_TYPE_0_TH_Sf); break;
                case bcmFabricLinkRxGciLvl2FC: *value = soc_reg_field_get(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Sr, reg_val, DCH_LOCAL_GCI_1_TYPE_0_TH_Sf); break;
                case bcmFabricLinkRxGciLvl3FC: *value = soc_reg_field_get(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Sr, reg_val, DCH_LOCAL_GCI_2_TYPE_0_TH_Sf); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
            }
        }
        else
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_LOCAL_GCI_TYPE_1_TH_Sr(unit, blk_id, &reg_val));
            
            switch(type)
            {
                case bcmFabricLinkRxGciLvl1FC: *value = soc_reg_field_get(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Sr, reg_val, DCH_LOCAL_GCI_0_TYPE_1_TH_Sf); break;
                case bcmFabricLinkRxGciLvl2FC: *value = soc_reg_field_get(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Sr, reg_val, DCH_LOCAL_GCI_1_TYPE_1_TH_Sf); break;
                case bcmFabricLinkRxGciLvl3FC: *value = soc_reg_field_get(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Sr, reg_val, DCH_LOCAL_GCI_2_TYPE_1_TH_Sf); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
            }
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe1600_fabric_flow_control_rx_drop_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dcmn_fabric_pipe_t pipe, int value)
{
    soc_error_t rc1 = SOC_E_NONE, rc2 = SOC_E_NONE;
    uint64      val64;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);
	 
    COMPILER_64_SET(val64, 0, value);
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        switch(type)
        {
            case bcmFabricLinkRxPrio0Drop: 
                rc1 = soc_reg64_field_validate(unit, DCMA_DCMUA_PRIORITY_DROP_THRESHOLDr, DCMUA_P_0_DROP_THf, val64); 
                rc2 = soc_reg64_field_validate(unit, DCMB_DCMUB_PRIORITY_DROP_THRESHOLDr, DCMUB_P_0_DROP_THf, val64); 
                break;
            case bcmFabricLinkRxPrio1Drop: 
                rc1 = soc_reg64_field_validate(unit, DCMA_DCMUA_PRIORITY_DROP_THRESHOLDr, DCMUA_P_1_DROP_THf, val64);
                rc2 = soc_reg64_field_validate(unit, DCMB_DCMUB_PRIORITY_DROP_THRESHOLDr, DCMUB_P_1_DROP_THf, val64); 
                break;
            case bcmFabricLinkRxPrio2Drop: 
                rc1 = soc_reg64_field_validate(unit, DCMA_DCMUA_PRIORITY_DROP_THRESHOLDr, DCMUA_P_2_DROP_THf, val64); 
                rc2 = soc_reg64_field_validate(unit, DCMB_DCMUB_PRIORITY_DROP_THRESHOLDr, DCMUB_P_2_DROP_THf, val64); 
                break;
            case bcmFabricLinkRxPrio3Drop: 
                rc1 = soc_reg64_field_validate(unit, DCMA_DCMUA_PRIORITY_DROP_THRESHOLDr, DCMUA_P_3_DROP_THf, val64); 
                rc2 = soc_reg64_field_validate(unit, DCMB_DCMUB_PRIORITY_DROP_THRESHOLDr, DCMUB_P_3_DROP_THf, val64); 
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
        }
    }

    if(SOC_FAILURE(rc1) || SOC_FAILURE(rc2)) {
        SOCDNX_EXIT_WITH_ERR(SOC_FAILURE(rc1) ? rc1 : rc2, (_BSL_SOCDNX_MSG("value %d isn't a valid for RX drop threshold"),value)); 
    }

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {       
        switch(type)
        {
            case bcmFabricLinkRxPrio0Drop: 
                rc1 = soc_reg64_field_validate(unit, DCMA_DCMMA_PRIORITY_DROP_THRESHOLDr, DCMMA_P_0_DROP_THf, val64); 
                rc2 = soc_reg64_field_validate(unit, DCMB_DCMMB_PRIORITY_DROP_THRESHOLDr, DCMMB_P_0_DROP_THf, val64); 
                break;
            case bcmFabricLinkRxPrio1Drop: 
                rc1 = soc_reg64_field_validate(unit, DCMA_DCMMA_PRIORITY_DROP_THRESHOLDr, DCMMA_P_1_DROP_THf, val64); 
                rc2 = soc_reg64_field_validate(unit, DCMB_DCMMB_PRIORITY_DROP_THRESHOLDr, DCMMB_P_1_DROP_THf, val64); 
                break;
            case bcmFabricLinkRxPrio2Drop: 
                rc1 = soc_reg64_field_validate(unit, DCMA_DCMMA_PRIORITY_DROP_THRESHOLDr, DCMMA_P_2_DROP_THf, val64); 
                rc2 = soc_reg64_field_validate(unit, DCMB_DCMMB_PRIORITY_DROP_THRESHOLDr, DCMMB_P_2_DROP_THf, val64); 
                break;
            case bcmFabricLinkRxPrio3Drop: 
                rc1 = soc_reg64_field_validate(unit, DCMA_DCMMA_PRIORITY_DROP_THRESHOLDr, DCMMA_P_3_DROP_THf, val64); 
                rc2 = soc_reg64_field_validate(unit, DCMB_DCMMB_PRIORITY_DROP_THRESHOLDr, DCMMB_P_3_DROP_THf, val64); 
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
        }
    }
   
    if(SOC_FAILURE(rc1) || SOC_FAILURE(rc2)) {
        SOCDNX_EXIT_WITH_ERR(SOC_FAILURE(rc1) ? rc1 : rc2, (_BSL_SOCDNX_MSG("value %d isn't a valid for RX drop threshold"),value)); 
    }

    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe1600_fabric_flow_control_rx_drop_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value)
{
    int min_idx, max_idx,idx;
    uint64 reg_valA, reg_valB, val64;
    uint32 reg_dcma_registers_en[SOC_FE1600_NOF_INSTANCES_DCMA], reg_dcmb_registers_en[SOC_FE1600_NOF_INSTANCES_DCMB];
    uint32 enable_a, enable_b, prio;

    SOCDNX_INIT_FUNC_DEFS;
	SOC_FE1600_ONLY(unit);
	COMPILER_64_SET(val64, 0, value);
    
    if(fe1 && fe3)
        {min_idx = 0; max_idx = 1;}
    else if(fe1)
        {min_idx = 0; max_idx = 0;}
    else
        {min_idx = 1; max_idx = 1;}
         
     
    for(idx = min_idx ; idx <= max_idx ; idx++){
            
        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
        {
            SOCDNX_IF_ERR_EXIT(READ_DCMA_DCMUA_PRIORITY_DROP_THRESHOLDr(unit, idx, &reg_valA));
            SOCDNX_IF_ERR_EXIT(READ_DCMB_DCMUB_PRIORITY_DROP_THRESHOLDr(unit, idx, &reg_valB));

            SOCDNX_IF_ERR_EXIT(READ_DCMA_DCM_ENABLERS_REGISTERr(unit, idx, reg_dcma_registers_en+ idx));
            SOCDNX_IF_ERR_EXIT(READ_DCMB_DCM_ENABLERS_REGISTERr(unit, idx, reg_dcmb_registers_en+ idx));
            enable_a = soc_reg_field_get(unit, DCMA_DCM_ENABLERS_REGISTERr, reg_dcma_registers_en[idx], LOW_PR_DROP_EN_Pf);
            enable_b = soc_reg_field_get(unit, DCMB_DCM_ENABLERS_REGISTERr, reg_dcmb_registers_en[idx], LOW_PR_DROP_EN_Pf);
      
            switch(type)
            {
                case bcmFabricLinkRxPrio0Drop: 
                    soc_reg64_field_set(unit, DCMA_DCMUA_PRIORITY_DROP_THRESHOLDr,&reg_valA, DCMUA_P_0_DROP_THf, val64); 
                    soc_reg64_field_set(unit, DCMB_DCMUB_PRIORITY_DROP_THRESHOLDr,&reg_valB, DCMUB_P_0_DROP_THf, val64);
                    prio = 0;
                    enable_a |= 1 << prio;
                    enable_b |= 1 << prio;
                    soc_reg_field_set(unit, DCMA_DCM_ENABLERS_REGISTERr, reg_dcma_registers_en + idx, LOW_PR_DROP_EN_Pf, enable_a);
                    soc_reg_field_set(unit, DCMB_DCM_ENABLERS_REGISTERr, reg_dcmb_registers_en + idx, LOW_PR_DROP_EN_Pf, enable_b);
                    break;
                case bcmFabricLinkRxPrio1Drop: 
                    soc_reg64_field_set(unit, DCMA_DCMUA_PRIORITY_DROP_THRESHOLDr,&reg_valA, DCMUA_P_1_DROP_THf, val64); 
                    soc_reg64_field_set(unit, DCMB_DCMUB_PRIORITY_DROP_THRESHOLDr,&reg_valB, DCMUB_P_1_DROP_THf, val64);
                    prio = 1;
                    enable_a |= 1 << prio;
                    enable_b |= 1 << prio;
                    soc_reg_field_set(unit, DCMA_DCM_ENABLERS_REGISTERr, reg_dcma_registers_en + idx, LOW_PR_DROP_EN_Pf, enable_a);
                    soc_reg_field_set(unit, DCMB_DCM_ENABLERS_REGISTERr, reg_dcmb_registers_en + idx, LOW_PR_DROP_EN_Pf, enable_b);
                    break;
                case bcmFabricLinkRxPrio2Drop: 
                    soc_reg64_field_set(unit, DCMA_DCMUA_PRIORITY_DROP_THRESHOLDr,&reg_valA, DCMUA_P_2_DROP_THf, val64); 
                    soc_reg64_field_set(unit, DCMB_DCMUB_PRIORITY_DROP_THRESHOLDr,&reg_valB, DCMUB_P_2_DROP_THf, val64);
                    prio = 2;
                    enable_a |= 1 << prio;
                    enable_b |= 1 << prio;
                    soc_reg_field_set(unit, DCMA_DCM_ENABLERS_REGISTERr, reg_dcma_registers_en + idx, LOW_PR_DROP_EN_Pf, enable_a);
                    soc_reg_field_set(unit, DCMB_DCM_ENABLERS_REGISTERr, reg_dcmb_registers_en + idx, LOW_PR_DROP_EN_Pf, enable_b);
                    break;
                case bcmFabricLinkRxPrio3Drop: 
                    soc_reg64_field_set(unit, DCMA_DCMUA_PRIORITY_DROP_THRESHOLDr,&reg_valA, DCMUA_P_3_DROP_THf, val64); 
                    soc_reg64_field_set(unit, DCMB_DCMUB_PRIORITY_DROP_THRESHOLDr,&reg_valB, DCMUB_P_3_DROP_THf, val64);
                    prio = 3;
                    enable_a |= 1 << prio;
                    enable_b |= 1 << prio;
                    soc_reg_field_set(unit, DCMA_DCM_ENABLERS_REGISTERr, reg_dcma_registers_en + idx, LOW_PR_DROP_EN_Pf, enable_a);
                    soc_reg_field_set(unit, DCMB_DCM_ENABLERS_REGISTERr, reg_dcmb_registers_en + idx, LOW_PR_DROP_EN_Pf, enable_b);
                    break;
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
            }
            
            SOCDNX_IF_ERR_EXIT(WRITE_DCMA_DCMUA_PRIORITY_DROP_THRESHOLDr(unit, idx, reg_valA));
            SOCDNX_IF_ERR_EXIT(WRITE_DCMB_DCMUB_PRIORITY_DROP_THRESHOLDr(unit, idx, reg_valB));
            SOCDNX_IF_ERR_EXIT(WRITE_DCMA_DCM_ENABLERS_REGISTERr(unit, idx, reg_dcma_registers_en[idx]));
            SOCDNX_IF_ERR_EXIT(WRITE_DCMB_DCM_ENABLERS_REGISTERr(unit, idx, reg_dcmb_registers_en[idx]));
        }

        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
        {
            SOCDNX_IF_ERR_EXIT(READ_DCMA_DCMMA_PRIORITY_DROP_THRESHOLDr(unit, idx, &reg_valA));
            SOCDNX_IF_ERR_EXIT(READ_DCMB_DCMMB_PRIORITY_DROP_THRESHOLDr(unit, idx, &reg_valB));

            SOCDNX_IF_ERR_EXIT(READ_DCMA_DCM_ENABLERS_REGISTERr(unit, idx, reg_dcma_registers_en+ idx));
            SOCDNX_IF_ERR_EXIT(READ_DCMB_DCM_ENABLERS_REGISTERr(unit, idx, reg_dcmb_registers_en+ idx));
            enable_a = soc_reg_field_get(unit, DCMA_DCM_ENABLERS_REGISTERr, reg_dcma_registers_en[idx], LOW_PR_DROP_EN_Sf);
            enable_b = soc_reg_field_get(unit, DCMB_DCM_ENABLERS_REGISTERr, reg_dcmb_registers_en[idx], LOW_PR_DROP_EN_Sf);
            
            switch(type)
            {
                case bcmFabricLinkRxPrio0Drop: 
                    soc_reg64_field_set(unit, DCMA_DCMMA_PRIORITY_DROP_THRESHOLDr,&reg_valA, DCMMA_P_0_DROP_THf, val64); 
                    soc_reg64_field_set(unit, DCMB_DCMMB_PRIORITY_DROP_THRESHOLDr,&reg_valB, DCMMB_P_0_DROP_THf, val64);
                    prio = 0;
                    enable_a |= 1 << prio;
                    enable_b |= 1 << prio;
                    soc_reg_field_set(unit, DCMA_DCM_ENABLERS_REGISTERr, reg_dcma_registers_en + idx, LOW_PR_DROP_EN_Sf, enable_a);
                    soc_reg_field_set(unit, DCMB_DCM_ENABLERS_REGISTERr, reg_dcmb_registers_en + idx, LOW_PR_DROP_EN_Sf, enable_b);
                    break;
                case bcmFabricLinkRxPrio1Drop: 
                    soc_reg64_field_set(unit, DCMA_DCMMA_PRIORITY_DROP_THRESHOLDr,&reg_valA, DCMMA_P_1_DROP_THf, val64); 
                    soc_reg64_field_set(unit, DCMB_DCMMB_PRIORITY_DROP_THRESHOLDr,&reg_valB, DCMMB_P_1_DROP_THf, val64);
                    prio = 1;
                    enable_a |= 1 << prio;
                    enable_b |= 1 << prio;
                    soc_reg_field_set(unit, DCMA_DCM_ENABLERS_REGISTERr, reg_dcma_registers_en + idx, LOW_PR_DROP_EN_Sf, enable_a);
                    soc_reg_field_set(unit, DCMB_DCM_ENABLERS_REGISTERr, reg_dcmb_registers_en + idx, LOW_PR_DROP_EN_Sf, enable_b);
                    break;
                case bcmFabricLinkRxPrio2Drop: 
                    soc_reg64_field_set(unit, DCMA_DCMMA_PRIORITY_DROP_THRESHOLDr,&reg_valA, DCMMA_P_2_DROP_THf, val64); 
                    soc_reg64_field_set(unit, DCMB_DCMMB_PRIORITY_DROP_THRESHOLDr,&reg_valB, DCMMB_P_2_DROP_THf, val64);
                    prio = 2;
                    enable_a |= 1 << prio;
                    enable_b |= 1 << prio;
                    soc_reg_field_set(unit, DCMA_DCM_ENABLERS_REGISTERr, reg_dcma_registers_en + idx, LOW_PR_DROP_EN_Sf, enable_a);
                    soc_reg_field_set(unit, DCMB_DCM_ENABLERS_REGISTERr, reg_dcmb_registers_en + idx, LOW_PR_DROP_EN_Sf, enable_b);            
                    break;
                case bcmFabricLinkRxPrio3Drop: 
                    soc_reg64_field_set(unit, DCMA_DCMMA_PRIORITY_DROP_THRESHOLDr,&reg_valA, DCMMA_P_3_DROP_THf, val64); 
                    soc_reg64_field_set(unit, DCMB_DCMMB_PRIORITY_DROP_THRESHOLDr,&reg_valB, DCMMB_P_3_DROP_THf, val64);
                    prio = 3;
                    enable_a |= 1 << prio;
                    enable_b |= 1 << prio;
                    soc_reg_field_set(unit, DCMA_DCM_ENABLERS_REGISTERr, reg_dcma_registers_en + idx, LOW_PR_DROP_EN_Sf, enable_a);
                    soc_reg_field_set(unit, DCMB_DCM_ENABLERS_REGISTERr, reg_dcmb_registers_en + idx, LOW_PR_DROP_EN_Sf, enable_b);                   
                    break;
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
            }
            
            SOCDNX_IF_ERR_EXIT(WRITE_DCMA_DCMMA_PRIORITY_DROP_THRESHOLDr(unit, idx, reg_valA));
            SOCDNX_IF_ERR_EXIT(WRITE_DCMB_DCMMB_PRIORITY_DROP_THRESHOLDr(unit, idx, reg_valB));
            SOCDNX_IF_ERR_EXIT(WRITE_DCMA_DCM_ENABLERS_REGISTERr(unit, idx, reg_dcma_registers_en[idx]));
            SOCDNX_IF_ERR_EXIT(WRITE_DCMB_DCM_ENABLERS_REGISTERr(unit, idx, reg_dcmb_registers_en[idx]));
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe1600_fabric_flow_control_rx_drop_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dcmn_fabric_control_source_t pipe, int fe1, int fe3, int* value)
{
    int blk_id;
    uint64 reg_valA;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);
    
    if(fe1)
        blk_id = 0;
    else
        blk_id = 1;
      
    if(pipe == soc_dcmn_fabric_control_source_primary || pipe == soc_dcmn_fabric_control_source_both)
    {
        SOCDNX_IF_ERR_EXIT(READ_DCMA_DCMUA_PRIORITY_DROP_THRESHOLDr(unit, blk_id, &reg_valA));
        
        switch(type)
        {
            case bcmFabricLinkRxPrio0Drop: 
                *value = soc_reg64_field32_get(unit, DCMA_DCMUA_PRIORITY_DROP_THRESHOLDr, reg_valA, DCMUA_P_0_DROP_THf); 
                break;
            case bcmFabricLinkRxPrio1Drop: 
                *value = soc_reg64_field32_get(unit, DCMA_DCMUA_PRIORITY_DROP_THRESHOLDr, reg_valA, DCMUA_P_1_DROP_THf); 
                break;
            case bcmFabricLinkRxPrio2Drop: 
                *value = soc_reg64_field32_get(unit, DCMA_DCMUA_PRIORITY_DROP_THRESHOLDr, reg_valA, DCMUA_P_2_DROP_THf); 
                break;
            case bcmFabricLinkRxPrio3Drop: 
                *value = soc_reg64_field32_get(unit, DCMA_DCMUA_PRIORITY_DROP_THRESHOLDr, reg_valA, DCMUA_P_3_DROP_THf); 
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
        }
    }
    else 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCMA_DCMMA_PRIORITY_DROP_THRESHOLDr(unit, blk_id, &reg_valA));
        switch(type)
        {
            case bcmFabricLinkRxPrio0Drop: 
                *value = soc_reg64_field32_get(unit, DCMA_DCMMA_PRIORITY_DROP_THRESHOLDr, reg_valA, DCMMA_P_0_DROP_THf); 
                break;
            case bcmFabricLinkRxPrio1Drop: 
                *value = soc_reg64_field32_get(unit, DCMA_DCMMA_PRIORITY_DROP_THRESHOLDr, reg_valA, DCMMA_P_1_DROP_THf); 
                break;
            case bcmFabricLinkRxPrio2Drop: 
                *value = soc_reg64_field32_get(unit, DCMA_DCMMA_PRIORITY_DROP_THRESHOLDr, reg_valA, DCMMA_P_2_DROP_THf);  
                break;
            case bcmFabricLinkRxPrio3Drop: 
                *value = soc_reg64_field32_get(unit, DCMA_DCMMA_PRIORITY_DROP_THRESHOLDr, reg_valA, DCMMA_P_3_DROP_THf); 
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

    
soc_error_t 
soc_fe1600_fabric_flow_control_tx_rci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value)
{
    soc_error_t rc = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);  

    if (type != bcmFabricLinkRciFC)
    {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("invalid type for fe1600")));
    }

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index) {
            rc = soc_reg_field_validate(unit, DCL_TYPE_01_RCI_TH_Pr, RCI_TH_HIGH_0_Pf, value);
        } else {
            rc = soc_reg_field_validate(unit, DCL_TYPE_01_RCI_TH_Pr, RCI_TH_HIGH_1_Pf, value);  
        }
    }
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for TX RCI threshold"),value)); 
    }

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    { 
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index) {
            rc = soc_reg_field_validate(unit, DCL_TYPE_01_RCI_TH_Sr, RCI_TH_HIGH_0_Sf, value);
        } else {
            rc = soc_reg_field_validate(unit, DCL_TYPE_01_RCI_TH_Sr, RCI_TH_HIGH_1_Sf, value);
        }
    }
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for TX RCI threshold"),value)); 
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}   

    
soc_error_t 
soc_fe1600_fabric_flow_control_tx_rci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value)
{
    int min_idx, max_idx,idx;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);
    
    if(fe1 && fe3)
        {min_idx = 0; max_idx = SOC_DFE_DEFS_GET(unit, nof_instances_dcl) - 1;}
    else if(fe1)
        {min_idx = 2; max_idx = 3;}
    else
        {min_idx = 0; max_idx = 1;}
    
     
    for(idx = min_idx ; idx <= max_idx ; idx++) {
      
      if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
      {
          SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_01_RCI_TH_Pr(unit, idx, &reg_val));
          
          if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
              soc_reg_field_set(unit, DCL_TYPE_01_RCI_TH_Pr, &reg_val, RCI_TH_HIGH_0_Pf, value);
          else
              soc_reg_field_set(unit, DCL_TYPE_01_RCI_TH_Pr, &reg_val, RCI_TH_HIGH_1_Pf, value);
          
          SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_01_RCI_TH_Pr(unit, idx, reg_val));
        
      }
      
      if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
      {
          SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_01_RCI_TH_Sr(unit, idx, &reg_val));
          
          if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
              soc_reg_field_set(unit, DCL_TYPE_01_RCI_TH_Sr, &reg_val, RCI_TH_HIGH_0_Sf, value);
          else
              soc_reg_field_set(unit, DCL_TYPE_01_RCI_TH_Sr, &reg_val, RCI_TH_HIGH_1_Sf, value);
          
          SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_01_RCI_TH_Sr(unit, idx, reg_val));
      }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}      

 
soc_error_t 
soc_fe1600_fabric_flow_control_tx_rci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value)
{
    int blk_id;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    
    if(fe1)
        blk_id = 2;
    else
        blk_id = 0;

    if (type != bcmFabricLinkRciFC)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid type for fe1600 %d"),type)); 
    }
    
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_01_RCI_TH_Pr(unit, blk_id, &reg_val));
        
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            *value = soc_reg_field_get(unit, DCL_TYPE_01_RCI_TH_Pr, reg_val, RCI_TH_HIGH_0_Pf);
        else
            *value = soc_reg_field_get(unit, DCL_TYPE_01_RCI_TH_Pr, reg_val, RCI_TH_HIGH_1_Pf);
      
    }
    else 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_01_RCI_TH_Pr(unit, blk_id, &reg_val));
        
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            *value = soc_reg_field_get(unit, DCL_TYPE_01_RCI_TH_Sr, reg_val, RCI_TH_HIGH_0_Sf);
        else
            *value = soc_reg_field_get(unit, DCL_TYPE_01_RCI_TH_Sr, reg_val, RCI_TH_HIGH_1_Sf);
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}    

 
soc_error_t 
soc_fe1600_fabric_flow_control_tx_bypass_llfc_threshold_validate(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value)
{
    soc_error_t rc = SOC_E_NONE;
    uint64  val64;


    SOCDNX_INIT_FUNC_DEFS;
	SOC_FE1600_ONLY(unit);
	COMPILER_64_SET(val64, 0, value);
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            rc = soc_reg64_field_validate(unit, DCL_DCL_LLFC_THr, DCLU_LLFC_TH_TYPE_0f, val64);
        else
            rc = soc_reg64_field_validate(unit, DCL_DCL_LLFC_THr, DCLU_LLFC_TH_TYPE_1f, val64);
    }
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for TX BYBASS LLFC threshold"),value)); 
    }

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            rc = soc_reg64_field_validate(unit, DCL_DCL_LLFC_THr, DCLM_LLFC_TH_TYPE_0f, val64);
        else
            rc = soc_reg64_field_validate(unit, DCL_DCL_LLFC_THr, DCLM_LLFC_TH_TYPE_1f, val64);
    }
     if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid for TX BYBASS LLFC threshold"),value)); 
    }   

  
exit:
    SOCDNX_FUNC_RETURN;
}   

 
soc_error_t 
soc_fe1600_fabric_flow_control_tx_bypass_llfc_threshold_set(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value)
{
    int min_idx, max_idx,idx;
    uint64 reg_val, val64;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);
	 
    
    if(fe1 && fe3)
        {min_idx = 0; max_idx = SOC_DFE_DEFS_GET(unit, nof_instances_dcl) - 1;}
    else if(fe1)
        {min_idx = 2; max_idx = 3;}
    else
        {min_idx = 0; max_idx = 1;}
    
    COMPILER_64_SET(val64, 0, value);
     
    for(idx = min_idx ; idx <= max_idx ; idx++)  {
      
        SOCDNX_IF_ERR_EXIT(READ_DCL_DCL_LLFC_THr(unit, idx, &reg_val));
        
        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
        {
            if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
                soc_reg64_field_set(unit, DCL_DCL_LLFC_THr, &reg_val, DCLU_LLFC_TH_TYPE_0f, val64);
            else
                soc_reg64_field_set(unit, DCL_DCL_LLFC_THr, &reg_val, DCLU_LLFC_TH_TYPE_1f, val64);
        }
        
        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
        {
            if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
                soc_reg64_field_set(unit, DCL_DCL_LLFC_THr, &reg_val, DCLM_LLFC_TH_TYPE_0f, val64);
            else
                soc_reg64_field_set(unit, DCL_DCL_LLFC_THr, &reg_val, DCLM_LLFC_TH_TYPE_1f, val64);
        }
        
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_DCL_LLFC_THr(unit, idx, reg_val));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}   

 
soc_error_t 
soc_fe1600_fabric_flow_control_tx_bypass_llfc_threshold_get(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value)
{
    int blk_id;
    uint64 reg_val, ret_val;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    
    if(fe1)
        blk_id = 2;
    else
        blk_id = 0;
      
    SOCDNX_IF_ERR_EXIT(READ_DCL_DCL_LLFC_THr(unit, blk_id, &reg_val));
    
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            ret_val = soc_reg64_field_get(unit, DCL_DCL_LLFC_THr, reg_val, DCLU_LLFC_TH_TYPE_0f);
        else
            ret_val = soc_reg64_field_get(unit, DCL_DCL_LLFC_THr, reg_val, DCLU_LLFC_TH_TYPE_1f); 
    }
    else 
    {
        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
            ret_val = soc_reg64_field_get(unit, DCL_DCL_LLFC_THr, reg_val, DCLM_LLFC_TH_TYPE_0f);
        else
            ret_val = soc_reg64_field_get(unit, DCL_DCL_LLFC_THr, reg_val, DCLM_LLFC_TH_TYPE_1f);
    }
    *value = COMPILER_64_LO(ret_val);
exit:
    SOCDNX_FUNC_RETURN;
}   

 
soc_error_t 
soc_fe1600_fabric_flow_control_tx_gci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value)
{
    soc_error_t rc = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
        {
            switch(type)
            {
                case bcmFabricLinkTxGciLvl1FC: rc = soc_reg_field_validate(unit, DCL_TYPE_0_GCI_TH_Pr, GCI_TH_LOW_0_Pf, value); break;
                case bcmFabricLinkTxGciLvl2FC: rc = soc_reg_field_validate(unit, DCL_TYPE_0_GCI_TH_Pr, GCI_TH_MED_0_Pf, value); break;
                case bcmFabricLinkTxGciLvl3FC: rc = soc_reg_field_validate(unit, DCL_TYPE_0_GCI_TH_Pr, GCI_TH_HIGH_0_Pf, value); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
            }
        }
        else
        {
            switch(type)
            {
                case bcmFabricLinkTxGciLvl1FC: rc = soc_reg_field_validate(unit, DCL_TYPE_1_GCI_TH_Pr, GCI_TH_LOW_1_Pf, value); break;
                case bcmFabricLinkTxGciLvl2FC: rc = soc_reg_field_validate(unit, DCL_TYPE_1_GCI_TH_Pr, GCI_TH_MED_1_Pf, value); break;
                case bcmFabricLinkTxGciLvl3FC: rc = soc_reg_field_validate(unit, DCL_TYPE_1_GCI_TH_Pr, GCI_TH_HIGH_1_Pf, value); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
            }
        }
    }
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid threshold for type %d"),value,type)); 
    } 

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
        {
            switch(type)
            {
                case bcmFabricLinkTxGciLvl1FC: rc = soc_reg_field_validate(unit, DCL_TYPE_0_GCI_TH_Sr, GCI_TH_LOW_0_Sf, value); break;
                case bcmFabricLinkTxGciLvl2FC: rc = soc_reg_field_validate(unit, DCL_TYPE_0_GCI_TH_Sr, GCI_TH_MED_0_Sf, value); break;
                case bcmFabricLinkTxGciLvl3FC: rc = soc_reg_field_validate(unit, DCL_TYPE_0_GCI_TH_Sr, GCI_TH_HIGH_0_Sf, value); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
            }
        }
        else
        {
            switch(type)
            {
                case bcmFabricLinkTxGciLvl1FC: rc = soc_reg_field_validate(unit, DCL_TYPE_1_GCI_TH_Sr, GCI_TH_LOW_1_Sf, value); break;
                case bcmFabricLinkTxGciLvl2FC: rc = soc_reg_field_validate(unit, DCL_TYPE_1_GCI_TH_Sr, GCI_TH_MED_1_Sf, value); break;
                case bcmFabricLinkTxGciLvl3FC: rc = soc_reg_field_validate(unit, DCL_TYPE_1_GCI_TH_Sr, GCI_TH_HIGH_1_Sf, value); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
            }
        }
    }
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid threshold for type %d"),value,type)); 
    }
      
exit:
    SOCDNX_FUNC_RETURN;
} 

 
soc_error_t 
soc_fe1600_fabric_flow_control_tx_gci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value)
{
    int min_idx, max_idx,idx;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    
    if(fe1 && fe3)
        {min_idx = 0; max_idx = SOC_DFE_DEFS_GET(unit, nof_instances_dcl) - 1;}
    else if(fe1)
        {min_idx = 2; max_idx = 3;}
    else
        {min_idx = 0; max_idx = 1;}
    
      
    for(idx = min_idx ; idx <= max_idx ; idx++) { 
        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
        {
            if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_0_GCI_TH_Pr(unit, idx, &reg_val));
                
                switch(type)
                {
                    case bcmFabricLinkTxGciLvl1FC: soc_reg_field_set(unit, DCL_TYPE_0_GCI_TH_Pr, &reg_val, GCI_TH_LOW_0_Pf, value); break;
                    case bcmFabricLinkTxGciLvl2FC: soc_reg_field_set(unit, DCL_TYPE_0_GCI_TH_Pr, &reg_val, GCI_TH_MED_0_Pf, value); break;
                    case bcmFabricLinkTxGciLvl3FC: soc_reg_field_set(unit, DCL_TYPE_0_GCI_TH_Pr, &reg_val, GCI_TH_HIGH_0_Pf, value); break;
                    default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
                }
                
                SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_0_GCI_TH_Pr(unit, idx, reg_val));
            }
            else
            {
                SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_1_GCI_TH_Pr(unit, idx, &reg_val));
                
                switch(type)
                {
                    case bcmFabricLinkTxGciLvl1FC: soc_reg_field_set(unit, DCL_TYPE_1_GCI_TH_Pr, &reg_val, GCI_TH_LOW_1_Pf, value); break;
                    case bcmFabricLinkTxGciLvl2FC: soc_reg_field_set(unit, DCL_TYPE_1_GCI_TH_Pr, &reg_val, GCI_TH_MED_1_Pf, value); break;
                    case bcmFabricLinkTxGciLvl3FC: soc_reg_field_set(unit, DCL_TYPE_1_GCI_TH_Pr, &reg_val, GCI_TH_HIGH_1_Pf, value); break;
                    default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
                }
             
                SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_1_GCI_TH_Pr(unit, idx, reg_val));
            }
        }
        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
        {
            if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_0_GCI_TH_Sr(unit, idx, &reg_val));
                
                switch(type)
                {
                    case bcmFabricLinkTxGciLvl1FC: soc_reg_field_set(unit, DCL_TYPE_0_GCI_TH_Sr, &reg_val, GCI_TH_LOW_0_Sf, value); break;
                    case bcmFabricLinkTxGciLvl2FC: soc_reg_field_set(unit, DCL_TYPE_0_GCI_TH_Sr, &reg_val, GCI_TH_MED_0_Sf, value); break;
                    case bcmFabricLinkTxGciLvl3FC: soc_reg_field_set(unit, DCL_TYPE_0_GCI_TH_Sr, &reg_val, GCI_TH_HIGH_0_Sf, value); break;
                    default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
                }
               
                SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_0_GCI_TH_Sr(unit, idx, reg_val));
            }
            else
            {
                SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_1_GCI_TH_Sr(unit, idx, &reg_val));
                
                switch(type)
                {
                    case bcmFabricLinkTxGciLvl1FC: soc_reg_field_set(unit, DCL_TYPE_1_GCI_TH_Sr, &reg_val, GCI_TH_LOW_1_Sf, value); break;
                    case bcmFabricLinkTxGciLvl2FC: soc_reg_field_set(unit, DCL_TYPE_1_GCI_TH_Sr, &reg_val, GCI_TH_MED_1_Sf, value); break;
                    case bcmFabricLinkTxGciLvl3FC: soc_reg_field_set(unit, DCL_TYPE_1_GCI_TH_Sr, &reg_val, GCI_TH_HIGH_1_Sf, value); break;
                    default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
                }
                
                SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_1_GCI_TH_Sr(unit, idx, reg_val));
            }
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}   

 
soc_error_t 
soc_fe1600_fabric_flow_control_tx_gci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value)
{
    int blk_id;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    
    if(fe3)
        blk_id = 0;
    else if(fe1)
        blk_id = 2;
    else
    {
        LOG_WARN(BSL_LS_SOC_FABRIC,
                 (BSL_META_U(unit,
                             "No blocks to configure\n")));
        SOC_EXIT;
    }
      
      
    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_0_GCI_TH_Pr(unit, blk_id, &reg_val));
            switch(type)
            {
                case bcmFabricLinkTxGciLvl1FC: *value = soc_reg_field_get(unit, DCL_TYPE_0_GCI_TH_Pr, reg_val, GCI_TH_LOW_0_Pf); break;
                case bcmFabricLinkTxGciLvl2FC: *value = soc_reg_field_get(unit, DCL_TYPE_0_GCI_TH_Pr, reg_val, GCI_TH_MED_0_Pf); break;
                case bcmFabricLinkTxGciLvl3FC: *value = soc_reg_field_get(unit, DCL_TYPE_0_GCI_TH_Pr, reg_val, GCI_TH_HIGH_0_Pf); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
            }
        }
        else
        {
            SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_1_GCI_TH_Pr(unit, blk_id, &reg_val));
            switch(type)
            {
                case bcmFabricLinkTxGciLvl1FC: *value = soc_reg_field_get(unit, DCL_TYPE_1_GCI_TH_Pr, reg_val, GCI_TH_LOW_1_Pf); break;
                case bcmFabricLinkTxGciLvl2FC: *value = soc_reg_field_get(unit, DCL_TYPE_1_GCI_TH_Pr, reg_val, GCI_TH_MED_1_Pf); break;
                case bcmFabricLinkTxGciLvl3FC: *value = soc_reg_field_get(unit, DCL_TYPE_1_GCI_TH_Pr, reg_val, GCI_TH_HIGH_1_Pf); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
            }
        }
    }
    else 
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_0_GCI_TH_Sr(unit, blk_id, &reg_val));
            switch(type)
            {
                case bcmFabricLinkTxGciLvl1FC: *value = soc_reg_field_get(unit, DCL_TYPE_0_GCI_TH_Sr, reg_val, GCI_TH_LOW_0_Sf); break;
                case bcmFabricLinkTxGciLvl2FC: *value = soc_reg_field_get(unit, DCL_TYPE_0_GCI_TH_Sr, reg_val, GCI_TH_MED_0_Sf); break;
                case bcmFabricLinkTxGciLvl3FC: *value = soc_reg_field_get(unit, DCL_TYPE_0_GCI_TH_Sr, reg_val, GCI_TH_HIGH_0_Sf); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
            }
        }
        else
        {
            SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_1_GCI_TH_Sr(unit, blk_id, &reg_val));
            switch(type)
            {
                case bcmFabricLinkTxGciLvl1FC: *value = soc_reg_field_get(unit, DCL_TYPE_1_GCI_TH_Sr, reg_val, GCI_TH_LOW_1_Sf); break;
                case bcmFabricLinkTxGciLvl2FC: *value = soc_reg_field_get(unit, DCL_TYPE_1_GCI_TH_Sr, reg_val, GCI_TH_MED_1_Sf); break;
                case bcmFabricLinkTxGciLvl3FC: *value = soc_reg_field_get(unit, DCL_TYPE_1_GCI_TH_Sr, reg_val, GCI_TH_HIGH_1_Sf); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type)); 
            }
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

 
soc_error_t 
soc_fe1600_fabric_flow_control_tx_drop_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value)
{
    
    uint64          val64;
    soc_error_t rc = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;  
	SOC_FE1600_ONLY(unit);
	COMPILER_64_SET(val64, 0, value);

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
        {
            switch(type)
            {
                case bcmFabricLinkTxPrio0Drop: rc = soc_reg64_field_validate(unit, DCL_TYPE_0_DRP_PPr, TYPE_0_DRP_P_0_Pf, val64); break;
                case bcmFabricLinkTxPrio1Drop: rc = soc_reg64_field_validate(unit, DCL_TYPE_0_DRP_PPr, TYPE_0_DRP_P_1_Pf, val64); break;
                case bcmFabricLinkTxPrio2Drop: rc = soc_reg64_field_validate(unit, DCL_TYPE_0_DRP_PPr, TYPE_0_DRP_P_2_Pf, val64); break;
                case bcmFabricLinkTxPrio3Drop: rc = soc_reg64_field_validate(unit, DCL_TYPE_0_DRP_PPr, TYPE_0_DRP_P_3_Pf, val64); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
            }
        }
        else
        {     
            switch(type)
            {
                case bcmFabricLinkTxPrio0Drop: rc = soc_reg64_field_validate(unit, DCL_TYPE_1_DRP_PPr, TYPE_1_DRP_P_0_Pf, val64); break;
                case bcmFabricLinkTxPrio1Drop: rc = soc_reg64_field_validate(unit, DCL_TYPE_1_DRP_PPr, TYPE_1_DRP_P_1_Pf, val64); break;
                case bcmFabricLinkTxPrio2Drop: rc = soc_reg64_field_validate(unit, DCL_TYPE_1_DRP_PPr, TYPE_1_DRP_P_2_Pf, val64); break;
                case bcmFabricLinkTxPrio3Drop: rc = soc_reg64_field_validate(unit, DCL_TYPE_1_DRP_PPr, TYPE_1_DRP_P_3_Pf, val64); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
            }
        }
    }
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid threshold for type %d"),value, type));
    }

    if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
        {       
          switch(type)
          {
              case bcmFabricLinkTxPrio0Drop: rc = soc_reg64_field_validate(unit, DCL_TYPE_0_DRP_PSr, TYPE_0_DRP_P_0_Sf, val64); break;
              case bcmFabricLinkTxPrio1Drop: rc = soc_reg64_field_validate(unit, DCL_TYPE_0_DRP_PSr, TYPE_0_DRP_P_1_Sf, val64); break;
              case bcmFabricLinkTxPrio2Drop: rc = soc_reg64_field_validate(unit, DCL_TYPE_0_DRP_PSr, TYPE_0_DRP_P_2_Sf, val64); break;
              case bcmFabricLinkTxPrio3Drop: rc = soc_reg64_field_validate(unit, DCL_TYPE_0_DRP_PSr, TYPE_0_DRP_P_3_Sf, val64); break;
              default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
          }
        }
        else
        { 
            switch(type)
            {
                case bcmFabricLinkTxPrio0Drop: rc = soc_reg64_field_validate(unit, DCL_TYPE_1_DRP_PSr, TYPE_1_DRP_P_0_Sf, val64); break;
                case bcmFabricLinkTxPrio1Drop: rc = soc_reg64_field_validate(unit, DCL_TYPE_1_DRP_PSr, TYPE_1_DRP_P_1_Sf, val64); break;
                case bcmFabricLinkTxPrio2Drop: rc = soc_reg64_field_validate(unit, DCL_TYPE_1_DRP_PSr, TYPE_1_DRP_P_2_Sf, val64); break;
                case bcmFabricLinkTxPrio3Drop: rc = soc_reg64_field_validate(unit, DCL_TYPE_1_DRP_PSr, TYPE_1_DRP_P_3_Sf, val64); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
            }
        }
    }
    if(SOC_FAILURE(rc)) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("value %d isn't a valid threshold for type %d"),value, type));
    }
    
      
exit:
    SOCDNX_FUNC_RETURN;
}  

 
soc_error_t 
soc_fe1600_fabric_flow_control_tx_drop_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value)
{
    int min_idx, max_idx,idx;
    uint64 reg_val, val64;
    uint32 reg_dcl_registers_en[SOC_FE1600_NOF_INSTANCES_DCL];
    uint32 enable, prio;
    SOCDNX_INIT_FUNC_DEFS;  
    SOC_FE1600_ONLY(unit);

    
    if(fe1 && fe3)
        {min_idx = 0; max_idx = SOC_DFE_DEFS_GET(unit, nof_instances_dcl) - 1;}
    else if(fe1)
        {min_idx = 2; max_idx = 3;}
    else
        {min_idx = 0; max_idx = 1;}
    
    COMPILER_64_SET(val64, 0, value);
     
    for(idx = min_idx ; idx <= max_idx ; idx++) {
        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0))
        {
            SOCDNX_IF_ERR_EXIT(READ_DCL_DCL_ENABLERS_REGISTERr_REG32(unit, idx, reg_dcl_registers_en+ idx));
            enable = soc_reg_field_get(unit, DCL_DCL_ENABLERS_REGISTERr, reg_dcl_registers_en[idx], LOW_PR_DROP_EN_Pf);

            if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_0_DRP_PPr(unit, idx, &reg_val));
                
                switch(type)
                {
                    case bcmFabricLinkTxPrio0Drop:
                        soc_reg64_field_set(unit, DCL_TYPE_0_DRP_PPr, &reg_val, TYPE_0_DRP_P_0_Pf, val64);
                        prio = 0;
                        enable |= 1 << prio;
                        soc_reg_field_set(unit, DCL_DCL_ENABLERS_REGISTERr, reg_dcl_registers_en + idx, LOW_PR_DROP_EN_Pf, enable);
                        break;
                    case bcmFabricLinkTxPrio1Drop:
                        soc_reg64_field_set(unit, DCL_TYPE_0_DRP_PPr, &reg_val, TYPE_0_DRP_P_1_Pf, val64);
                        prio = 1;
                        enable |= 1 << prio;
                        soc_reg_field_set(unit, DCL_DCL_ENABLERS_REGISTERr, reg_dcl_registers_en + idx, LOW_PR_DROP_EN_Pf, enable);
                        break;
                    case bcmFabricLinkTxPrio2Drop: 
                        soc_reg64_field_set(unit, DCL_TYPE_0_DRP_PPr, &reg_val, TYPE_0_DRP_P_2_Pf, val64);
                        prio = 2;
                        enable |= 1 << prio;
                        soc_reg_field_set(unit, DCL_DCL_ENABLERS_REGISTERr, reg_dcl_registers_en + idx, LOW_PR_DROP_EN_Pf, enable);
                        break;
                    case bcmFabricLinkTxPrio3Drop:
                        soc_reg64_field_set(unit, DCL_TYPE_0_DRP_PPr, &reg_val, TYPE_0_DRP_P_3_Pf, val64);
                        prio = 3;
                        enable |= 1 << prio;
                        soc_reg_field_set(unit, DCL_DCL_ENABLERS_REGISTERr, reg_dcl_registers_en + idx, LOW_PR_DROP_EN_Pf, enable);
                        break;
                    default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
                }
        
                SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_0_DRP_PPr(unit, idx, reg_val));
               
            }
            else
            {
                SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_1_DRP_PPr(unit, idx, &reg_val));
                
                switch(type)
                {
                    case bcmFabricLinkTxPrio0Drop: 
                        soc_reg64_field_set(unit, DCL_TYPE_1_DRP_PPr, &reg_val, TYPE_1_DRP_P_0_Pf, val64);
                        prio = 0;
                        enable |= 1 << prio;
                        soc_reg_field_set(unit, DCL_DCL_ENABLERS_REGISTERr, reg_dcl_registers_en + idx, LOW_PR_DROP_EN_Pf, enable);
                        break;
                    case bcmFabricLinkTxPrio1Drop: 
                        soc_reg64_field_set(unit, DCL_TYPE_1_DRP_PPr, &reg_val, TYPE_1_DRP_P_1_Pf, val64);
                        prio = 1;
                        enable |= 1 << prio;
                        soc_reg_field_set(unit, DCL_DCL_ENABLERS_REGISTERr, reg_dcl_registers_en + idx, LOW_PR_DROP_EN_Pf, enable); 
                        break;
                    case bcmFabricLinkTxPrio2Drop: 
                        soc_reg64_field_set(unit, DCL_TYPE_1_DRP_PPr, &reg_val, TYPE_1_DRP_P_2_Pf, val64); 
                        prio = 2;
                        enable |= 1 << prio;
                        soc_reg_field_set(unit, DCL_DCL_ENABLERS_REGISTERr, reg_dcl_registers_en + idx, LOW_PR_DROP_EN_Pf, enable);     
                        break;
                    case bcmFabricLinkTxPrio3Drop: 
                        soc_reg64_field_set(unit, DCL_TYPE_1_DRP_PPr, &reg_val, TYPE_1_DRP_P_3_Pf, val64);
                        prio = 3;
                        enable |= 1 << prio;
                        soc_reg_field_set(unit, DCL_DCL_ENABLERS_REGISTERr, reg_dcl_registers_en + idx, LOW_PR_DROP_EN_Pf, enable);   
                        break;
                    default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
                }
        
                SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_1_DRP_PPr(unit, idx, reg_val));
            }
             SOCDNX_IF_ERR_EXIT(WRITE_DCL_DCL_ENABLERS_REGISTERr_REG32(unit, idx, reg_dcl_registers_en[idx]));
        }
        if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1))
        {
            SOCDNX_IF_ERR_EXIT(READ_DCL_DCL_ENABLERS_REGISTERr_REG32(unit, idx, reg_dcl_registers_en+ idx));
            enable = soc_reg_field_get(unit, DCL_DCL_ENABLERS_REGISTERr, reg_dcl_registers_en[idx], LOW_PR_DROP_EN_Pf);

            if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_0_DRP_PSr(unit, idx, &reg_val));
                
                switch(type)
                {
                    case bcmFabricLinkTxPrio0Drop:
                        soc_reg64_field_set(unit, DCL_TYPE_0_DRP_PSr, &reg_val, TYPE_0_DRP_P_0_Sf, val64);
                        prio = 0;
                        enable |= 1 << prio;
                        soc_reg_field_set(unit, DCL_DCL_ENABLERS_REGISTERr, reg_dcl_registers_en + idx, LOW_PR_DROP_EN_Sf, enable); 
                        break;
                    case bcmFabricLinkTxPrio1Drop: 
                        soc_reg64_field_set(unit, DCL_TYPE_0_DRP_PSr, &reg_val, TYPE_0_DRP_P_1_Sf, val64);
                        prio = 1;
                        enable |= 1 << prio;
                        soc_reg_field_set(unit, DCL_DCL_ENABLERS_REGISTERr, reg_dcl_registers_en + idx, LOW_PR_DROP_EN_Sf, enable);  
                        break;
                    case bcmFabricLinkTxPrio2Drop: 
                        soc_reg64_field_set(unit, DCL_TYPE_0_DRP_PSr, &reg_val, TYPE_0_DRP_P_2_Sf, val64);
                        prio = 2;
                        enable |= 1 << prio;
                        soc_reg_field_set(unit, DCL_DCL_ENABLERS_REGISTERr, reg_dcl_registers_en + idx, LOW_PR_DROP_EN_Sf, enable);      
                        break;
                    case bcmFabricLinkTxPrio3Drop: 
                        soc_reg64_field_set(unit, DCL_TYPE_0_DRP_PSr, &reg_val, TYPE_0_DRP_P_3_Sf, val64);
                        prio = 3;
                        enable |= 1 << prio;
                        soc_reg_field_set(unit, DCL_DCL_ENABLERS_REGISTERr, reg_dcl_registers_en + idx, LOW_PR_DROP_EN_Sf, enable);  
                        break;
                    default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
                }
        
                SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_0_DRP_PSr(unit, idx, reg_val));
            }
            else
            {
                SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_1_DRP_PSr(unit, idx, &reg_val));
                
                switch(type)
                {
                    case bcmFabricLinkTxPrio0Drop: 
                        soc_reg64_field_set(unit, DCL_TYPE_1_DRP_PSr, &reg_val, TYPE_1_DRP_P_0_Sf, val64); 
                        prio = 0;
                        enable |= 1 << prio;
                        soc_reg_field_set(unit, DCL_DCL_ENABLERS_REGISTERr, reg_dcl_registers_en + idx, LOW_PR_DROP_EN_Sf, enable);
                        break;
                    case bcmFabricLinkTxPrio1Drop: 
                        soc_reg64_field_set(unit, DCL_TYPE_1_DRP_PSr, &reg_val, TYPE_1_DRP_P_1_Sf, val64);
                        prio = 1;
                        enable |= 1 << prio;
                        soc_reg_field_set(unit, DCL_DCL_ENABLERS_REGISTERr, reg_dcl_registers_en + idx, LOW_PR_DROP_EN_Sf, enable);  
                        break;
                    case bcmFabricLinkTxPrio2Drop: 
                        soc_reg64_field_set(unit, DCL_TYPE_1_DRP_PSr, &reg_val, TYPE_1_DRP_P_2_Sf, val64);
                        prio = 2;
                        enable |= 1 << prio;
                        soc_reg_field_set(unit, DCL_DCL_ENABLERS_REGISTERr, reg_dcl_registers_en + idx, LOW_PR_DROP_EN_Sf, enable);      
                        break;
                    case bcmFabricLinkTxPrio3Drop: 
                        soc_reg64_field_set(unit, DCL_TYPE_1_DRP_PSr, &reg_val, TYPE_1_DRP_P_3_Sf, val64);
                        prio = 3;
                        enable |= 1 << prio;
                        soc_reg_field_set(unit, DCL_DCL_ENABLERS_REGISTERr, reg_dcl_registers_en + idx, LOW_PR_DROP_EN_Sf, enable);  
                        break;
                    default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
                }
        
                SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_1_DRP_PSr(unit, idx, reg_val));
            }
             SOCDNX_IF_ERR_EXIT(WRITE_DCL_DCL_ENABLERS_REGISTERr_REG32(unit, idx, reg_dcl_registers_en[idx]));
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}  

 
soc_error_t 
soc_fe1600_fabric_flow_control_tx_drop_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value)
{
    int blk_id;
    uint64 reg_val, ret_val;
    SOCDNX_INIT_FUNC_DEFS;  
    SOC_FE1600_ONLY(unit);

    
    if(fe3)
        blk_id = 0;
    else if(fe1)
        blk_id = 2;
    else
    {
        LOG_WARN(BSL_LS_SOC_FABRIC,
                 (BSL_META_U(unit,
                             "No blocks to configure\n")));
        SOC_EXIT;
    }
      
    if(pipe == soc_dcmn_fabric_control_source_primary || pipe == soc_dcmn_fabric_control_source_both)
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_0_DRP_PPr(unit, blk_id, &reg_val));
            switch(type)
            {
                case bcmFabricLinkTxPrio0Drop: ret_val = soc_reg64_field_get(unit, DCL_TYPE_0_DRP_PPr, reg_val, TYPE_0_DRP_P_0_Pf); break;
                case bcmFabricLinkTxPrio1Drop: ret_val = soc_reg64_field_get(unit, DCL_TYPE_0_DRP_PPr, reg_val, TYPE_0_DRP_P_1_Pf); break;
                case bcmFabricLinkTxPrio2Drop: ret_val = soc_reg64_field_get(unit, DCL_TYPE_0_DRP_PPr, reg_val, TYPE_0_DRP_P_2_Pf); break;
                case bcmFabricLinkTxPrio3Drop: ret_val = soc_reg64_field_get(unit, DCL_TYPE_0_DRP_PPr, reg_val, TYPE_0_DRP_P_3_Pf); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
            }
            *value = COMPILER_64_LO(ret_val);
        }
        else
        {
            SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_1_DRP_PPr(unit, blk_id, &reg_val));
            switch(type)
            {
                case bcmFabricLinkTxPrio0Drop: ret_val = soc_reg64_field_get(unit, DCL_TYPE_1_DRP_PPr, reg_val, TYPE_1_DRP_P_0_Pf); break;
                case bcmFabricLinkTxPrio1Drop: ret_val = soc_reg64_field_get(unit, DCL_TYPE_1_DRP_PPr, reg_val, TYPE_1_DRP_P_1_Pf); break;
                case bcmFabricLinkTxPrio2Drop: ret_val = soc_reg64_field_get(unit, DCL_TYPE_1_DRP_PPr, reg_val, TYPE_1_DRP_P_2_Pf); break;
                case bcmFabricLinkTxPrio3Drop: ret_val = soc_reg64_field_get(unit, DCL_TYPE_1_DRP_PPr, reg_val, TYPE_1_DRP_P_3_Pf); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
            }
            *value = COMPILER_64_LO(ret_val);
        }
    }
    else 
    {
        if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_0_DRP_PSr(unit, blk_id, &reg_val));
            switch(type)
            {
                case bcmFabricLinkTxPrio0Drop: ret_val = soc_reg64_field_get(unit, DCL_TYPE_0_DRP_PSr, reg_val, TYPE_0_DRP_P_0_Sf); break;
                case bcmFabricLinkTxPrio1Drop: ret_val = soc_reg64_field_get(unit, DCL_TYPE_0_DRP_PSr, reg_val, TYPE_0_DRP_P_1_Sf); break;
                case bcmFabricLinkTxPrio2Drop: ret_val = soc_reg64_field_get(unit, DCL_TYPE_0_DRP_PSr, reg_val, TYPE_0_DRP_P_2_Sf); break;
                case bcmFabricLinkTxPrio3Drop: ret_val = soc_reg64_field_get(unit, DCL_TYPE_0_DRP_PSr, reg_val, TYPE_0_DRP_P_3_Sf); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
            }
            *value = COMPILER_64_LO(ret_val);
        }
        else
        {
            SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_1_DRP_PSr(unit, blk_id, &reg_val));
            switch(type)
            {
                case bcmFabricLinkTxPrio0Drop: ret_val = soc_reg64_field_get(unit, DCL_TYPE_1_DRP_PSr, reg_val, TYPE_1_DRP_P_0_Sf); break;
                case bcmFabricLinkTxPrio1Drop: ret_val = soc_reg64_field_get(unit, DCL_TYPE_1_DRP_PSr, reg_val, TYPE_1_DRP_P_1_Sf); break;
                case bcmFabricLinkTxPrio2Drop: ret_val = soc_reg64_field_get(unit, DCL_TYPE_1_DRP_PSr, reg_val, TYPE_1_DRP_P_2_Sf); break;
                case bcmFabricLinkTxPrio3Drop: ret_val = soc_reg64_field_get(unit, DCL_TYPE_1_DRP_PSr, reg_val, TYPE_1_DRP_P_3_Sf); break;
                default: SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong threshold type %d"),type));
            }
            *value = COMPILER_64_LO(ret_val);
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe1600_fabric_links_link_type_set(int unit, soc_dcmn_fabric_pipe_t pipe, soc_dfe_fabric_link_fifo_type_index_t type_index, 
                                        int is_rx, int is_tx, int is_fe1, int is_fe3, uint32 links_count, soc_port_t* links)
{
    uint32 reg_val_prim = 0, reg_val_scnd = 0;
    int blk_id, i;
    soc_port_t link, inner_lnk;
    int half_of_links = (SOC_DFE_DEFS_GET(unit, nof_links)/2);
    SOCDNX_INIT_FUNC_DEFS;  
    SOC_FE1600_ONLY(unit);

    for(i=0 ; i<links_count ; i++)
    {
        link = links[i];
        blk_id = INT_DEVIDE(link,32);
        inner_lnk = link % 32; 
        
        if(is_rx && ((is_fe1 && link<half_of_links) || (is_fe3 &&link>=half_of_links)))
        {
            
            if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0)) {
                SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_BMP_Pr(unit, blk_id, &reg_val_prim));
            }
            if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1)) {
                SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_BMP_Sr(unit, blk_id, &reg_val_scnd));

            }
            
            if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                
                SHR_BITCLR(&reg_val_prim, inner_lnk);
                SHR_BITCLR(&reg_val_scnd, inner_lnk);
            }
            else
            {
                
                SHR_BITSET(&reg_val_prim, inner_lnk);
                SHR_BITSET(&reg_val_scnd, inner_lnk);
            }
            
            
            if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0)) {
                SOCDNX_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_BMP_Pr(unit, blk_id, reg_val_prim));
            }
            if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1)) {
                SOCDNX_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_BMP_Sr(unit, blk_id, reg_val_scnd));

            }
        }
        
        if(is_tx && ((is_fe1 && link>=half_of_links) || (is_fe3 && link<half_of_links)))
        {
            
            if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0)) {
                SOCDNX_IF_ERR_EXIT(READ_DCL_LINK_TYPE_BMP_Pr(unit, blk_id, &reg_val_prim));
            }
            if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1)) {
                SOCDNX_IF_ERR_EXIT(READ_DCL_LINK_TYPE_BMP_Sr(unit, blk_id, &reg_val_scnd));
            }
            
            if(soc_dfe_fabric_link_fifo_type_index_0 == type_index)
            {
                
                SHR_BITCLR(&reg_val_prim, inner_lnk);
                SHR_BITCLR(&reg_val_scnd, inner_lnk);
            }
            else
            {
                
                SHR_BITSET(&reg_val_prim, inner_lnk);
                SHR_BITSET(&reg_val_scnd, inner_lnk);
            }
            
            
            if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0)) {
                SOCDNX_IF_ERR_EXIT(WRITE_DCL_LINK_TYPE_BMP_Pr(unit, blk_id, reg_val_prim));
            }
            if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,1)) {
                SOCDNX_IF_ERR_EXIT(WRITE_DCL_LINK_TYPE_BMP_Sr(unit, blk_id, reg_val_scnd));

            }
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN; 
}


soc_error_t 
soc_fe1600_fabric_links_link_type_get(int unit, soc_dcmn_fabric_pipe_t pipe, soc_dfe_fabric_link_fifo_type_index_t type_index, 
                                          int is_rx, int is_tx, int is_fe1, int is_fe3, uint32 links_count_max, soc_port_t* links, uint32* links_count)
{
    uint32 reg_val[1];
    int blk_id;
    soc_port_t link, inner_lnk;
    int actual_type_index;
    int half_of_links = (SOC_DFE_DEFS_GET(unit, nof_links)/2);
    pbmp_t valid_link_bitmap;
    SOCDNX_INIT_FUNC_DEFS;  
    SOC_FE1600_ONLY(unit);

    *links_count = 0;
    SOC_PBMP_ASSIGN(valid_link_bitmap, SOC_INFO(unit).sfi.bitmap);
    SOC_PBMP_ITER(valid_link_bitmap, link)
    {
        actual_type_index = -1;
        blk_id = INT_DEVIDE(link,32);
        inner_lnk = link % 32; 
        
        if(is_rx && ((is_fe1 && link<half_of_links) || (is_fe3 &&link>=half_of_links)))
        {
            if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0)) {
                SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_BMP_Pr(unit, blk_id, reg_val));

            } else  {
                SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_BMP_Sr(unit, blk_id, reg_val));
            }
                
            if(SHR_BITGET(reg_val, inner_lnk))
                actual_type_index = soc_dfe_fabric_link_fifo_type_index_1;
            else
                actual_type_index = soc_dfe_fabric_link_fifo_type_index_0;
        }
        else if(is_tx && ((is_fe1 && link>=half_of_links) || (is_fe3 && link<half_of_links)))
        {
            if(SOC_DCMN_FABRIC_PIPE_IS_SET(&pipe,0)) {
                SOCDNX_IF_ERR_EXIT(READ_DCL_LINK_TYPE_BMP_Pr(unit, blk_id, reg_val));
            } else  {
                SOCDNX_IF_ERR_EXIT(READ_DCL_LINK_TYPE_BMP_Sr(unit, blk_id, reg_val));
            }
            
            if(SHR_BITGET(reg_val, inner_lnk))
                actual_type_index = soc_dfe_fabric_link_fifo_type_index_1;
            else
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

#endif 

#undef _ERR_MSG_MODULE_NAME

