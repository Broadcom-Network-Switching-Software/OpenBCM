/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: qax_cnt.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_CNT


#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/QAX/qax_cnt.h>
#include <soc/dpp/drv.h>




int qax_cnt_get_field_name_per_src_type(int unit, SOC_TMC_CNT_SRC_TYPE src_type, int command_id, soc_field_t * src_mask_field_name, soc_field_t * src_cfg_field_name, soc_field_t * group_size_field_name)
{

    SOCDNX_INIT_FUNC_DEFS

    switch(src_type)
    {
    case SOC_TMC_CNT_SRC_TYPE_ING_PP:
        * src_mask_field_name = (command_id == 0) ? CRPS_N_IRPP_A_MASKf : CRPS_N_IRPP_B_MASKf;
        * src_cfg_field_name = CRPS_N_BUBBLE_REQ_CGMf; 
        * group_size_field_name = (command_id == 0) ? IRPP_A_GROUP_SIZEf : IRPP_B_GROUP_SIZEf;
        break;
    case SOC_TMC_CNT_SRC_TYPE_VOQ:
    case SOC_TMC_CNT_SRC_TYPE_INGRESS_EXT_STAT: 
    case SOC_TMC_CNT_SRC_TYPE_STAG:
    case SOC_TMC_CNT_SRC_TYPE_VSQ:
    case SOC_TMC_CNT_SRC_TYPE_CNM_ID:    
        * src_mask_field_name = (command_id == 0) ? CRPS_N_CGM_A_MASKf : CRPS_N_CGM_B_MASKf;
        * src_cfg_field_name = CRPS_N_BUBBLE_REQ_CGMf;
        * group_size_field_name = (command_id == 0) ? CGM_A_GROUP_SIZEf : CGM_B_GROUP_SIZEf;                
        break;
    case SOC_TMC_CNT_SRC_TYPE_EGR_PP:     
        * src_mask_field_name = (command_id == 0) ? CRPS_N_EGQ_A_MASKf : CRPS_N_EGQ_B_MASKf;     
        * src_cfg_field_name = CRPS_N_BUBBLE_REQ_EGQf;      
        * group_size_field_name = (command_id == 0) ? EGQ_A_GROUP_SIZEf : EGQ_B_GROUP_SIZEf;        
        break;
    case SOC_TMC_CNT_SRC_TYPE_EPNI:
        * src_mask_field_name = (command_id == 0) ? CRPS_N_EPNI_A_MASKf : CRPS_N_EPNI_B_MASKf;   
        * src_cfg_field_name = CRPS_N_BUBBLE_REQ_EPNIf;        
        * group_size_field_name = (command_id == 0) ? EPNI_A_GROUP_SIZEf : EPNI_B_GROUP_SIZEf;                
        break;
    case SOC_TMC_CNT_SRC_TYPE_INGRESS_OAM:   
    case SOC_TMC_CNT_SRC_TYPE_EGRESS_OAM:           
        * src_mask_field_name = (command_id == 0) ? CRPS_N_IHP_A_MASKf : CRPS_N_IHP_B_MASKf;  
        * src_cfg_field_name = CRPS_N_BUBBLE_REQ_IHPf;        
        * group_size_field_name = INVALIDf;                
        break;
    case SOC_TMC_CNT_SRC_TYPES_EGQ_TM:
        * src_mask_field_name = INVALIDf;
        * src_cfg_field_name = INVALIDf;   
        * group_size_field_name = EGQ_TM_GROUP_SIZEf;                
        break;  
    case SOC_TMC_CNT_SRC_TYPES_IPT_LATENCY:
        * src_mask_field_name = INVALIDf;
        * src_cfg_field_name = INVALIDf;        
        * group_size_field_name = INVALIDf;                
        break;                     
    default:
        LOG_ERROR(BSL_LS_SOC_CNT, (BSL_META_U(unit, "src_type=%d is invalid\n"), src_type));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);        
    }

exit:
    SOCDNX_FUNC_RETURN  
}


int qax_cnt_ingress_compensation_outLif_delta_set(SOC_SAND_IN  int unit, SOC_SAND_IN  int core, SOC_SAND_IN  int commandId, SOC_SAND_IN  int src_type, SOC_SAND_IN  int lifIndex, SOC_SAND_IN  int delta)
{
    uint32 data[2];
    int rv;
    int coreIndex;
    int baseLifIndex;
    SOCDNX_INIT_FUNC_DEFS;

    if(lifIndex > MAX_ING_COMP_LIF_NUMBER || lifIndex < 0)
    {
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "lifIndex=%d. allowed values 0..31\n"), lifIndex));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    baseLifIndex = (lifIndex & 0x1F);
    if(src_type == SOC_PKT_SIZE_ADJUST_SRC_CRPS_IQM)
    {
        SOC_DPP_CORES_ITER(core, coreIndex)
        {
                rv = READ_CGM_HAPMm(unit, CGM_BLOCK(unit,coreIndex), baseLifIndex , data);
                SOCDNX_IF_ERR_EXIT(rv);
                if(commandId == 0)
                {
                    soc_CGM_HAPMm_field32_set(unit, data, TM_CTR_A_DELTAf, CONVERT_SIGNED_NUM_TO_TWO_COMPLEMENT_METHOD(delta, EIGHT_BITS));
                }
                else
                {
                    soc_CGM_HAPMm_field32_set(unit, data, TM_CTR_B_DELTAf, CONVERT_SIGNED_NUM_TO_TWO_COMPLEMENT_METHOD(delta, EIGHT_BITS));
                }

                rv = WRITE_CGM_HAPMm(unit, CGM_BLOCK(unit,coreIndex), baseLifIndex , data);
                SOCDNX_IF_ERR_EXIT(rv);
        }
    }
    else if (src_type == SOC_PKT_SIZE_ADJUST_SRC_CRPS_IQM_IRPP)
    {
        SOC_DPP_CORES_ITER(core, coreIndex)
        {
                rv = READ_CGM_HAPMm(unit, CGM_BLOCK(unit,coreIndex), baseLifIndex , data);
                SOCDNX_IF_ERR_EXIT(rv);
                if(commandId == 0)
                {
                    soc_CGM_HAPMm_field32_set(unit, data, PP_CTR_A_DELTAf, CONVERT_SIGNED_NUM_TO_TWO_COMPLEMENT_METHOD(delta, EIGHT_BITS));
                }
                else
                {
                    soc_CGM_HAPMm_field32_set(unit, data, PP_CTR_B_DELTAf, CONVERT_SIGNED_NUM_TO_TWO_COMPLEMENT_METHOD(delta, EIGHT_BITS));
                }

                rv = WRITE_CGM_HAPMm(unit, CGM_BLOCK(unit,coreIndex), baseLifIndex, data);
                SOCDNX_IF_ERR_EXIT(rv);
        }
    } else if (src_type == SOC_PKT_SIZE_ADJUST_SRC_STAT_REPOR_IN) {
        SOC_DPP_CORES_ITER(core, coreIndex)
        {
                rv = READ_CGM_HAPMm(unit, CGM_BLOCK(unit, coreIndex), baseLifIndex , data);
                SOCDNX_IF_ERR_EXIT(rv);
                soc_CGM_HAPMm_field32_set(unit, data, STAT_DELTAf, CONVERT_SIGNED_NUM_TO_TWO_COMPLEMENT_METHOD(delta, EIGHT_BITS));
                rv = WRITE_CGM_HAPMm(unit, CGM_BLOCK(unit, coreIndex), baseLifIndex , data);
                SOCDNX_IF_ERR_EXIT(rv);
        }
    } else {
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "src_type=%d. allowed types: SOC_COMPENSATION_PKT_SIZE_SRC_IQM_IRPP|SOC_COMPENSATION_PKT_SIZE_SRC_IQM\n"), src_type));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    

exit:
    SOCDNX_FUNC_RETURN;
}


int qax_cnt_ingress_compensation_profile_delta_set(SOC_SAND_IN int unit, SOC_SAND_IN int core, SOC_SAND_IN int src_type, SOC_SAND_IN int profileIndex, SOC_SAND_IN int delta)
{
    uint32 data;
    uint32 ctr_profile_cmp;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    if((delta < MIN_ING_COMP_DELTA_VALUE) || (delta > MAX_ING_COMP_DELTA_VALUE))
    {
        LOG_ERROR(BSL_LS_SOC_CNT, (BSL_META_U(unit, "delta=%d, out of the legal values\n"), delta));
        SOCDNX_IF_ERR_EXIT(SOC_E_INTERNAL);
    }
    if (profileIndex < 0)
    {
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "negative profile index\n")));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    if (src_type == SOC_PKT_SIZE_ADJUST_SRC_CRPS_IQM)
    {
        
            
        rv = READ_CGM_TM_CTR_CPMm(unit, CGM_BLOCK(unit,core), profileIndex , &data);
        SOCDNX_IF_ERR_EXIT(rv);
        ctr_profile_cmp = CONVERT_SIGNED_NUM_TO_TWO_COMPLEMENT_METHOD(delta,EIGHT_BITS);
        soc_CGM_TM_CTR_CPMm_field32_set(unit, &data, HDR_DELTAf, ctr_profile_cmp);
        rv = WRITE_CGM_TM_CTR_CPMm(unit, CGM_BLOCK(unit,core), profileIndex, &data);
        SOCDNX_IF_ERR_EXIT(rv);

        rv = READ_CGM_TM_CTR_CPMm(unit, CGM_BLOCK(unit,core), profileIndex+64 , &data);
        SOCDNX_IF_ERR_EXIT(rv);
        ctr_profile_cmp = CONVERT_SIGNED_NUM_TO_TWO_COMPLEMENT_METHOD(delta,EIGHT_BITS);
        soc_CGM_TM_CTR_CPMm_field32_set(unit, &data, HDR_DELTAf, ctr_profile_cmp);
        rv = WRITE_CGM_TM_CTR_CPMm(unit, CGM_BLOCK(unit,core), profileIndex+64, &data);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    else if (src_type == SOC_PKT_SIZE_ADJUST_SRC_CRPS_IQM_IRPP)
    {
        
           
        rv = READ_CGM_IRPP_CTR_CPMm(unit, CGM_BLOCK(unit,core), profileIndex , &data);
        SOCDNX_IF_ERR_EXIT(rv);
        ctr_profile_cmp = CONVERT_SIGNED_NUM_TO_TWO_COMPLEMENT_METHOD(delta, EIGHT_BITS);
        soc_CGM_IRPP_CTR_CPMm_field32_set(unit, &data, HDR_DELTAf, ctr_profile_cmp);
        rv = WRITE_CGM_IRPP_CTR_CPMm(unit, CGM_BLOCK(unit,core), profileIndex, &data);
        SOCDNX_IF_ERR_EXIT(rv);

        rv = READ_CGM_IRPP_CTR_CPMm(unit, CGM_BLOCK(unit,core), profileIndex+64 , &data);
        SOCDNX_IF_ERR_EXIT(rv);
        ctr_profile_cmp = CONVERT_SIGNED_NUM_TO_TWO_COMPLEMENT_METHOD(delta, EIGHT_BITS);
        soc_CGM_IRPP_CTR_CPMm_field32_set(unit, &data, HDR_DELTAf, ctr_profile_cmp);
        rv = WRITE_CGM_IRPP_CTR_CPMm(unit, CGM_BLOCK(unit,core), profileIndex+64, &data);
        SOCDNX_IF_ERR_EXIT(rv);
    } else if (src_type == SOC_PKT_SIZE_ADJUST_SRC_STAT_REPOR_IN) 
    {
        rv = READ_CGM_STAT_CPMm(unit, CGM_BLOCK(unit,core), profileIndex , &data);
        SOCDNX_IF_ERR_EXIT(rv);
        ctr_profile_cmp = CONVERT_SIGNED_NUM_TO_TWO_COMPLEMENT_METHOD(delta,EIGHT_BITS);
        soc_CGM_STAT_CPMm_field32_set(unit, &data, HDR_DELTAf, ctr_profile_cmp); 
        rv = WRITE_CGM_STAT_CPMm(unit, CGM_BLOCK(unit,core), profileIndex, &data);
        SOCDNX_IF_ERR_EXIT(rv);
    } else 
    {
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "src_type=%d. allowed types: SOC_COMPENSATION_PKT_SIZE_SRC_IQM_IRPP|SOC_COMPENSATION_PKT_SIZE_SRC_IQM\n"), src_type));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }


exit:
    SOCDNX_FUNC_RETURN;
}


int qax_cnt_ingress_compensation_port_profile_set(SOC_SAND_IN  int unit, SOC_SAND_IN  int core, SOC_SAND_IN  int commandId, SOC_SAND_IN  int src_type, SOC_SAND_IN  int port, SOC_SAND_IN  int profileIndex)
{
    uint32 data;
    int nof_loops, rv;
    uint32 port_index;

    SOCDNX_INIT_FUNC_DEFS;

    if (profileIndex < 0)
    {
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "negative profile index\n")));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (port < 0)
    {
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "negative port index\n")));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    
    if(port == MAX_PORTS_IN_CORE)
    {
        nof_loops = MAX_PORTS_IN_CORE;
        port_index = 0;
    }
    else 
    {
        nof_loops = 1;
        port_index = (uint32)port;
    }

    if (src_type == SOC_PKT_SIZE_ADJUST_SRC_CRPS_IQM)
    {
        while (nof_loops > 0)
        {
            rv = READ_CGM_IPP_HCPm(unit, CGM_BLOCK(unit,core), port_index , &data);
            SOCDNX_IF_ERR_EXIT(rv);
            if(commandId == 0)
            {
                soc_CGM_IPP_HCPm_field32_set(unit, &data, TM_CTR_A_PROFILEf, profileIndex);
            }
            else
            {
                soc_CGM_IPP_HCPm_field32_set(unit, &data, TM_CTR_B_PROFILEf, profileIndex);
            }

            rv = WRITE_CGM_IPP_HCPm(unit, CGM_BLOCK(unit,core), port_index, &data);
            SOCDNX_IF_ERR_EXIT(rv);
            port_index++;
            nof_loops--;
        }
    }
    else if (src_type == SOC_PKT_SIZE_ADJUST_SRC_CRPS_IQM_IRPP)
    {
        while (nof_loops > 0)
        {
            rv = READ_CGM_IPP_HCPm(unit, CGM_BLOCK(unit,core), port_index , &data);
            SOCDNX_IF_ERR_EXIT(rv);
            if(commandId == 0)
            {
                soc_CGM_IPP_HCPm_field32_set(unit, &data, PP_CTR_A_PROFILEf, profileIndex);
            }
            else
            {
                soc_CGM_IPP_HCPm_field32_set(unit, &data, PP_CTR_B_PROFILEf, profileIndex);
            }

            rv = WRITE_CGM_IPP_HCPm(unit, CGM_BLOCK(unit,core), port_index, &data);
            SOCDNX_IF_ERR_EXIT(rv);
            port_index++;
            nof_loops--;
        }
    }
    else if (src_type == SOC_PKT_SIZE_ADJUST_SRC_STAT_REPOR_IN) 
    {
        while (nof_loops > 0)
        {
            rv = READ_CGM_IPP_HCPm(unit, CGM_BLOCK(unit,core), port_index , &data);
            SOCDNX_IF_ERR_EXIT(rv);
            soc_CGM_IPP_HCPm_field32_set(unit, &data, STAT_PROFILEf, profileIndex);
            rv = WRITE_CGM_IPP_HCPm(unit, CGM_BLOCK(unit,core), port_index, &data);
            SOCDNX_IF_ERR_EXIT(rv);       
            port_index++;
            nof_loops--;
        }
    }
    else 
    {
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "src_type=%d. allowed types: SOC_COMPENSATION_PKT_SIZE_SRC_IQM_IRPP|SOC_COMPENSATION_PKT_SIZE_SRC_IQM\n"), src_type));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }


exit:
    SOCDNX_FUNC_RETURN;
}


int qax_cnt_ingress_compensation_outLif_delta_get(SOC_SAND_IN  int unit, SOC_SAND_IN  int core, SOC_SAND_IN  int commandId, SOC_SAND_IN  int src_type, SOC_SAND_IN  int lifIndex, SOC_SAND_OUT  int * delta)
{
    uint32 fieldData;
    uint32 memData[2];
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    if(lifIndex >= MAX_ING_COMP_LIF_NUMBER || lifIndex < 0)
    {
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "lifIndex=%d. allowed values 0..31\n"), lifIndex));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    if(src_type == SOC_PKT_SIZE_ADJUST_SRC_CRPS_IQM)
    {
        rv = READ_CGM_HAPMm(unit, CGM_BLOCK(unit,core), (lifIndex & 0x1F) , memData);
        SOCDNX_IF_ERR_EXIT(rv);
        if(commandId == 0)
        {
            fieldData = soc_CGM_HAPMm_field32_get(unit, memData, TM_CTR_A_DELTAf);
        }
        else
        {
            fieldData = soc_CGM_HAPMm_field32_get(unit, memData, TM_CTR_B_DELTAf);
        }
    }
    else if (src_type == SOC_PKT_SIZE_ADJUST_SRC_CRPS_IQM_IRPP)
    {
        rv = READ_CGM_HAPMm(unit, CGM_BLOCK(unit,core), (lifIndex & 0x1F) , memData);
        SOCDNX_IF_ERR_EXIT(rv);
        if(commandId == 0)
        {
            fieldData = soc_CGM_HAPMm_field32_get(unit, memData, PP_CTR_A_DELTAf);
        }
        else
        {
            fieldData = soc_CGM_HAPMm_field32_get(unit, memData, PP_CTR_B_DELTAf);
        }
    }
    else if (src_type == SOC_PKT_SIZE_ADJUST_SRC_STAT_REPOR_IN) 
    {
        rv = READ_CGM_HAPMm(unit, CGM_BLOCK(unit,core), (lifIndex & 0x1F) , memData);
        SOCDNX_IF_ERR_EXIT(rv);
        fieldData = soc_CGM_HAPMm_field32_get(unit, memData, STAT_DELTAf);
    }
    else
    {
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "src_type=%d. allowed types: SOC_COMPENSATION_PKT_SIZE_SRC_IQM_IRPP|SOC_COMPENSATION_PKT_SIZE_SRC_IQM\n"), src_type));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    * delta = CONVERT_TWO_COMPLEMENT_INTO_SIGNED_NUM(fieldData, EIGHT_BITS);
exit:
    SOCDNX_FUNC_RETURN;
}


int qax_cnt_ingress_compensation_port_delta_and_profile_get(SOC_SAND_IN  int unit, SOC_SAND_IN  int core, SOC_SAND_IN  int commandId, SOC_SAND_IN  int src_type, SOC_SAND_IN  int port, SOC_SAND_OUT  int * profileIndex,  SOC_SAND_OUT  int * delta)
{
    uint32 memData, fieldData;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;


    if(src_type == SOC_PKT_SIZE_ADJUST_SRC_CRPS_IQM)
    {
        rv = READ_CGM_IPP_HCPm(unit, CGM_BLOCK(unit,core), port , &memData);
        SOCDNX_IF_ERR_EXIT(rv);
        if(commandId == 0)
        {
            * profileIndex = (int)soc_CGM_IPP_HCPm_field32_get(unit, &memData, TM_CTR_A_PROFILEf);
        }
        else
        {
            * profileIndex = (int)soc_CGM_IPP_HCPm_field32_get(unit, &memData, TM_CTR_B_PROFILEf);
        }

        rv = READ_CGM_TM_CTR_CPMm(unit, CGM_BLOCK(unit,core), *profileIndex , &memData);
        SOCDNX_IF_ERR_EXIT(rv);
        fieldData = soc_CGM_TM_CTR_CPMm_field32_get(unit, &memData, HDR_DELTAf);
        * delta = CONVERT_TWO_COMPLEMENT_INTO_SIGNED_NUM(fieldData, EIGHT_BITS);
    }
    else if (src_type == SOC_PKT_SIZE_ADJUST_SRC_CRPS_IQM_IRPP)
    {
        rv = READ_CGM_IPP_HCPm(unit, CGM_BLOCK(unit,core), port , &memData);
        SOCDNX_IF_ERR_EXIT(rv);
        if(commandId == 0)
        {
            * profileIndex = (int)soc_CGM_IPP_HCPm_field32_get(unit, &memData, PP_CTR_A_PROFILEf);
        }
        else
        {
            * profileIndex = (int)soc_CGM_IPP_HCPm_field32_get(unit, &memData, PP_CTR_B_PROFILEf);
        }

        rv = READ_CGM_IRPP_CTR_CPMm(unit, CGM_BLOCK(unit,core), *profileIndex , &memData);
        SOCDNX_IF_ERR_EXIT(rv);
        fieldData = soc_CGM_IRPP_CTR_CPMm_field32_get(unit, &memData, HDR_DELTAf);
        * delta = CONVERT_TWO_COMPLEMENT_INTO_SIGNED_NUM(fieldData, EIGHT_BITS);
    } else if (src_type == SOC_PKT_SIZE_ADJUST_SRC_STAT_REPOR_IN) 
    {
        rv = READ_CGM_IPP_HCPm(unit, IQMT_BLOCK(unit), port , &memData);
        SOCDNX_IF_ERR_EXIT(rv);
        * profileIndex = (int)soc_CGM_IPP_HCPm_field32_get(unit, &memData, STAT_PROFILEf);

        rv = READ_CGM_STAT_CPMm(unit, IQMT_BLOCK(unit), *profileIndex , &memData);
        SOCDNX_IF_ERR_EXIT(rv);
        fieldData = soc_CGM_STAT_CPMm_field32_get(unit, &memData, HDR_DELTAf);
        * delta = CONVERT_TWO_COMPLEMENT_INTO_SIGNED_NUM(fieldData, EIGHT_BITS);
    } else 
    {
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "src_type=%d. allowed types: SOC_COMPENSATION_PKT_SIZE_SRC_IQM_IRPP|SOC_COMPENSATION_PKT_SIZE_SRC_IQM\n"), src_type));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

exit:
    SOCDNX_FUNC_RETURN;
}




int qax_cnt_filter_config_ingress_set_get(int unit, int relevant_bit, int command_id, int get, int* value)
{
    soc_reg_above_64_val_t reg_above_64;
    uint32 mask;
    uint32 field32_val;

    SOCDNX_INIT_FUNC_DEFS;

    
    mask = 1U << relevant_bit;

    SOCDNX_IF_ERR_EXIT(READ_CGM_TM_CTR_PTR_SETTINGSr(unit, reg_above_64));    
    
    if (command_id == 0) {
        field32_val = soc_reg_above_64_field32_get(unit, CGM_TM_CTR_PTR_SETTINGSr, reg_above_64, TM_CTR_A_RJCT_FILTER_MASKf);
        if (get) {
            *value = (field32_val & mask) ? 1 : 0;
        } else {
            field32_val = (*value) ? field32_val | mask : field32_val & (~mask) ;
            soc_reg_above_64_field32_set(unit, CGM_TM_CTR_PTR_SETTINGSr, reg_above_64, TM_CTR_A_RJCT_FILTER_MASKf, field32_val);
        }
    } else if (command_id == 1) {
        field32_val = soc_reg_above_64_field32_get(unit, CGM_TM_CTR_PTR_SETTINGSr, reg_above_64, TM_CTR_B_RJCT_FILTER_MASKf);
        if (get) {
            *value = (field32_val & mask) ? 1 : 0;
        } else {
            field32_val = (*value) ? field32_val | mask : field32_val & (~mask) ;
            soc_reg_above_64_field32_set(unit, CGM_TM_CTR_PTR_SETTINGSr, reg_above_64, TM_CTR_B_RJCT_FILTER_MASKf, field32_val);
        }        
    } else {
        LOG_ERROR(BSL_LS_SOC_CNT, (BSL_META_U(unit, "ingress counter doesn't support command id %d\n"), command_id));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_TM_CTR_PTR_SETTINGSr(unit, reg_above_64));

exit:
    SOCDNX_FUNC_RETURN;
}




uint32
  _qax_cnt_counter_bmap_mem_by_src_type_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                proc_id,
    SOC_SAND_IN  int                   src_core,
    SOC_SAND_IN  ARAD_CNT_SRC_TYPE     src_type,
    SOC_SAND_IN  int                   command_id,
    SOC_SAND_OUT soc_reg_t             *reg, 
    SOC_SAND_OUT soc_field_t           *fields)
{
    soc_reg_t
        iqm_counter_bmap_reg[] =  {CRPS_CGM_OFFSET_BMAP_Ar, CRPS_CGM_OFFSET_BMAP_Br},
        egq_counter_bmap_reg[] =  {CRPS_EGQ_OFFSET_BMAP_Ar, CRPS_EGQ_OFFSET_BMAP_Br},
        irpp_counter_bmap_reg[] = {CRPS_IRPP_OFFSET_BMAP_Ar, CRPS_IRPP_OFFSET_BMAP_Br},
        epni_counter_bmap_reg[] = {CRPS_EPNI_OFFSET_BMAP_Ar, CRPS_EPNI_OFFSET_BMAP_Br},
        egq_tm_counter_bmap_reg[] =  {CRPS_EGQ_TM_OFFSET_BMAPr},
        counter_bmap_reg = INVALIDf;
   int index, field_offset = 0;
    soc_field_t
       iqm_counter_bmap_fields[] =  {_BMAP_FIELDS_BLOCK_CMD_ID(CGM, A), _BMAP_FIELDS_BLOCK_CMD_ID(CGM, B)},
       egq_counter_bmap_fields[] =  {_BMAP_FIELDS_BLOCK_CMD_ID(EGQ, A), _BMAP_FIELDS_BLOCK_CMD_ID(EGQ, B)},
       irpp_counter_bmap_fields[] = {_BMAP_FIELDS_BLOCK_CMD_ID(IRPP, A), _BMAP_FIELDS_BLOCK_CMD_ID(IRPP, B)},
       epni_counter_bmap_fields[] = {_BMAP_FIELDS_BLOCK_CMD_ID(EPNI, A), _BMAP_FIELDS_BLOCK_CMD_ID(EPNI, B)},
       egq_tm_counter_bmap_fields[] = {_BMAP_FIELDS_BLOCK_CMD_ID(EGQ, _TM)},
       *counter_bmap_fields = NULL;
   SOC_SAND_INIT_ERROR_DEFINITIONS(0);

   switch (src_type) {
    case ARAD_CNT_SRC_TYPE_ING_PP:
        counter_bmap_reg = irpp_counter_bmap_reg[command_id + (src_core * 2)];
        counter_bmap_fields = irpp_counter_bmap_fields;
        field_offset = (command_id + (src_core * 2)) * SOC_TMC_CNT_BMAP_OFFSET_COUNT;
        break;      
    case ARAD_CNT_SRC_TYPE_VOQ:
    case ARAD_CNT_SRC_TYPE_INGRESS_EXT_STAT:				
    case ARAD_CNT_SRC_TYPE_STAG:
    case ARAD_CNT_SRC_TYPE_VSQ:
    case ARAD_CNT_SRC_TYPE_CNM_ID:
        counter_bmap_reg = iqm_counter_bmap_reg[command_id + (src_core * 2)];
        counter_bmap_fields = iqm_counter_bmap_fields;
        field_offset = (command_id + (src_core * 2)) * SOC_TMC_CNT_BMAP_OFFSET_COUNT;
        break;  
    case ARAD_CNT_SRC_TYPE_EGR_PP:
        counter_bmap_reg = egq_counter_bmap_reg[command_id + (src_core * 2)];
        counter_bmap_fields = egq_counter_bmap_fields;
        field_offset = (command_id + (src_core * 2)) * SOC_TMC_CNT_BMAP_OFFSET_COUNT;
        break;
    case ARAD_CNT_SRC_TYPE_INGRESS_OAM:
    case ARAD_CNT_SRC_TYPE_EGRESS_OAM:
    
      break;
    case ARAD_CNT_SRC_TYPES_IPT_LATENCY:
    
       break;
    case ARAD_CNT_SRC_TYPE_EPNI:
        counter_bmap_reg = epni_counter_bmap_reg[command_id + (src_core * 2)];
        counter_bmap_fields = epni_counter_bmap_fields;
        field_offset = (command_id + (src_core * 2)) * SOC_TMC_CNT_BMAP_OFFSET_COUNT;
        break;
   case ARAD_CNT_SRC_TYPES_EGQ_TM:
       counter_bmap_reg = egq_tm_counter_bmap_reg[src_core];
       counter_bmap_fields = egq_tm_counter_bmap_fields;
       field_offset = src_core * SOC_TMC_CNT_BMAP_OFFSET_COUNT;
       break;
   default:
        SOC_SAND_SET_ERROR_CODE(ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
  }
  *reg = counter_bmap_reg;
  if (counter_bmap_reg != INVALIDf && counter_bmap_fields) {
       for (index = 0; index  < SOC_TMC_CNT_BMAP_OFFSET_COUNT; index++) {
           fields[index] = counter_bmap_fields[field_offset + index];
       }
   }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in _qax_cnt_counter_bmap_mem_by_src_type_get()", 0, 0);
}







int qax_cnt_epni_counter_base_set(int unit, int src_core, int base_offset_field_val, int command_id, SOC_TMC_CNT_MODE_EG_TYPE type, SOC_TMC_CNT_SRC_TYPE source)
{
    int rv = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS

    switch (type) 
    {
    case SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT:
    case SOC_TMC_CNT_MODE_EG_TYPE_TM:
        rv = soc_reg_above_64_field32_modify(unit, EPNI_COUNTERS_BASEr, src_core, 0, PORT_COUNTER_BASE_POINTERf, 0); 
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    case ARAD_CNT_MODE_EG_TYPE_VSI:
        if((base_offset_field_val % QAX_VSI_OFFSET_RESOLUTION) != 0) 
        {
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "base_offset_field_val=%d, value must be in 2K resolution for EGRESS_VSI.\n"), base_offset_field_val));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);            
        }
        else
        {
            base_offset_field_val = base_offset_field_val/QAX_VSI_OFFSET_RESOLUTION;
            rv = soc_reg_above_64_field32_modify(unit, EPNI_COUNTERS_BASEr, src_core, 0, VSI_COUNTER_BASE_POINTERf, base_offset_field_val);
            SOCDNX_IF_ERR_EXIT(rv);            
        }
        break;
    case ARAD_CNT_MODE_EG_TYPE_OUTLIF:        
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid mode %s for source %s.\n"), SOC_TMC_CNT_MODE_EG_TYPE_to_string(type), SOC_TMC_CNT_SRC_TYPE_to_string(source)));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    
exit:
    SOCDNX_FUNC_RETURN;        
}




uint32 qax_cnt_crps_cgm_cmd_get(int unit, int proc_id, int command_id, SOC_SAND_OUT ARAD_CNT_CRPS_IQM_CMD *crps_iqm_cmd)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(crps_iqm_cmd);

    if (command_id == 0) {
        crps_iqm_cmd->cfg_1_r = CGM_TM_CTR_PTR_SETTINGSr;
        crps_iqm_cmd->cfg_2_r = CGM_TM_CTR_PTR_SETTINGSr;
        crps_iqm_cmd->src_cfg_r = CGM_TM_CTR_PTR_SETTINGSr;
        crps_iqm_cmd->src_type_f = TM_CTR_A_PTR_SELf;
        crps_iqm_cmd->queue_shift_f = TM_CTR_A_QNUM_SHIFTf;
        crps_iqm_cmd->base_q_f = TM_CTR_A_QNUM_LOWf;
        crps_iqm_cmd->top_q_f = TM_CTR_A_QNUM_HIGHf;
    } else if (command_id == 1) {
        crps_iqm_cmd->cfg_1_r = CGM_TM_CTR_PTR_SETTINGSr;
        crps_iqm_cmd->cfg_2_r = CGM_TM_CTR_PTR_SETTINGSr;
        crps_iqm_cmd->src_cfg_r = CGM_TM_CTR_PTR_SETTINGSr;
        crps_iqm_cmd->src_type_f = TM_CTR_B_PTR_SELf;
        crps_iqm_cmd->queue_shift_f = TM_CTR_B_QNUM_SHIFTf;
        crps_iqm_cmd->base_q_f = TM_CTR_B_QNUM_LOWf;
        crps_iqm_cmd->top_q_f = TM_CTR_B_QNUM_HIGHf;
    } else {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid command ID %d\n"), command_id));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }        
exit:
  SOCDNX_FUNC_RETURN;
}


uint32 qax_cnt_do_not_count_field_by_src_type_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 proc_id,
    SOC_SAND_IN  int                    src_core,
    SOC_SAND_IN  ARAD_CNT_SRC_TYPE      src_type,
    SOC_SAND_IN  int                    command_id,
    SOC_SAND_OUT soc_field_t            *field
    )
{
  soc_field_t       
      qax_cgm_do_not_count_entryf[] =      {CGM_A_DO_NOT_COUNT_OFFSETf,  CGM_B_DO_NOT_COUNT_OFFSETf},
      qax_egq_do_not_count_entryf[] =      {EGQ_A_DO_NOT_COUNT_OFFSETf,  EGQ_B_DO_NOT_COUNT_OFFSETf},
      qax_irpp_do_not_count_entryf[] =     {IRPP_A_DO_NOT_COUNT_OFFSETf, IRPP_B_DO_NOT_COUNT_OFFSETf},
      qax_epni_do_not_count_entryf[] =     {EPNI_A_DO_NOT_COUNT_OFFSETf, EPNI_B_DO_NOT_COUNT_OFFSETf},
      qax_egq_tm_do_not_count_entryf[] =   {EGQ_TM_DO_NOT_COUNT_OFFSETf},      
      do_not_count_fld = INVALIDf;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    switch (src_type) {
    case ARAD_CNT_SRC_TYPE_ING_PP:
      do_not_count_fld = qax_irpp_do_not_count_entryf[command_id];
      break;      
    case ARAD_CNT_SRC_TYPE_VOQ:
    case ARAD_CNT_SRC_TYPE_INGRESS_EXT_STAT:		
    case ARAD_CNT_SRC_TYPE_STAG:
    case ARAD_CNT_SRC_TYPE_VSQ:
    case ARAD_CNT_SRC_TYPE_CNM_ID:
      do_not_count_fld = qax_cgm_do_not_count_entryf[command_id];
      break;  
    case ARAD_CNT_SRC_TYPE_EGR_PP:
      do_not_count_fld = qax_egq_do_not_count_entryf[command_id];
      break;
    case ARAD_CNT_SRC_TYPE_INGRESS_OAM:
    case ARAD_CNT_SRC_TYPE_EGRESS_OAM:
    
      break;
    case ARAD_CNT_SRC_TYPE_EPNI:
      do_not_count_fld = qax_epni_do_not_count_entryf[command_id];
      break;
    case ARAD_CNT_SRC_TYPES_IPT_LATENCY:
      
      break;
    case ARAD_CNT_SRC_TYPES_EGQ_TM:
      do_not_count_fld = qax_egq_tm_do_not_count_entryf[0];
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
    } 
  *field = do_not_count_fld;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in qax_cnt_do_not_count_field_by_src_type_get()", 0, 0);
}




int qax_filter_config_egress_receive_set_get(int unit, int relevant_bit, int command_id, int get, int* value)
{
    uint32 reg32_val;
    uint32 mask;
    uint32 field32_val;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(value);

    
    mask = 1U << relevant_bit;

    
    if (command_id == BCM_STAT_COUNTER_TM_COMMAND) {
        SOCDNX_IF_ERR_EXIT(READ_ECGM_CRPS_TM_PQP_DROP_REASONS_MASKr(unit, &reg32_val));
        field32_val = soc_reg_field_get(unit, ECGM_CRPS_TM_PQP_DROP_REASONS_MASKr, reg32_val, CRPS_TM_PQP_DROP_REASONS_MASKf);
        if (get) {
            *value = (field32_val & mask) ? 1 : 0;
        } else {
            field32_val = (*value) ? field32_val | mask : field32_val & (~mask) ;
            soc_reg_field_set(unit, ECGM_CRPS_TM_PQP_DROP_REASONS_MASKr, &reg32_val, CRPS_TM_PQP_DROP_REASONS_MASKf, field32_val);
            SOCDNX_IF_ERR_EXIT(WRITE_ECGM_CRPS_TM_PQP_DROP_REASONS_MASKr(unit, reg32_val));
        }        
    } else if (command_id == 0) {
        SOCDNX_IF_ERR_EXIT(READ_ECGM_CRPS_PP_1_PQP_DROP_REASONS_MASKr(unit, &reg32_val));
        field32_val = soc_reg_field_get(unit, ECGM_CRPS_PP_1_PQP_DROP_REASONS_MASKr, reg32_val, CRPS_PP_1_PQP_DROP_REASONS_MASKf);
        if (get) {
            *value = (field32_val & mask) ? 1 : 0;
        } else {
            field32_val = (*value) ? field32_val | mask : field32_val & (~mask) ;
            soc_reg_field_set(unit, ECGM_CRPS_PP_1_PQP_DROP_REASONS_MASKr, &reg32_val, CRPS_PP_1_PQP_DROP_REASONS_MASKf, field32_val);
            SOCDNX_IF_ERR_EXIT(WRITE_ECGM_CRPS_PP_1_PQP_DROP_REASONS_MASKr(unit, reg32_val));
        }
    } else if (command_id == 1) {
        SOCDNX_IF_ERR_EXIT(READ_ECGM_CRPS_PP_2_PQP_DROP_REASONS_MASKr(unit, &reg32_val));
        field32_val = soc_reg_field_get(unit, ECGM_CRPS_PP_2_PQP_DROP_REASONS_MASKr, reg32_val, CRPS_PP_2_PQP_DROP_REASONS_MASKf);
        if (get) {
            *value = (field32_val & mask) ? 1 : 0;
        } else {
            field32_val = (*value) ? field32_val | mask : field32_val & (~mask) ;
            soc_reg_field_set(unit, ECGM_CRPS_PP_2_PQP_DROP_REASONS_MASKr, &reg32_val, CRPS_PP_2_PQP_DROP_REASONS_MASKf, field32_val);
            SOCDNX_IF_ERR_EXIT(WRITE_ECGM_CRPS_PP_2_PQP_DROP_REASONS_MASKr(unit, reg32_val));
        }        
    } else {
        LOG_ERROR(BSL_LS_SOC_CNT, (BSL_META_U(unit, "egress receive counter doesn't support command id %d\n"), command_id));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int qax_stat_counter_filter_group_get(int unit, int command_id, int filter_group, int *is_active)
{
    soc_reg_above_64_val_t tm_ctr_ptr_settings;
    uint32 tm_ctr_rjct_filter_mask = 0;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(is_active);

    if ((filter_group < 0) || (filter_group>=8))
    {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    SOC_REG_ABOVE_64_CLEAR(tm_ctr_ptr_settings);
    SOCDNX_IF_ERR_EXIT(READ_CGM_TM_CTR_PTR_SETTINGSr(unit, tm_ctr_ptr_settings));

    if (command_id == 0) {
        tm_ctr_rjct_filter_mask = soc_reg_above_64_field32_get(unit, CGM_TM_CTR_PTR_SETTINGSr, tm_ctr_ptr_settings, TM_CTR_A_RJCT_FILTER_MASKf);
    } else {
        tm_ctr_rjct_filter_mask = soc_reg_above_64_field32_get(unit, CGM_TM_CTR_PTR_SETTINGSr, tm_ctr_ptr_settings, TM_CTR_B_RJCT_FILTER_MASKf);
    }

    *is_active = ((tm_ctr_rjct_filter_mask & (0x1<<filter_group)) != 0);

exit:
    SOCDNX_FUNC_RETURN;
}


int qax_stat_counter_filter_group_set(int unit, int command_id, int filter_group, int is_active)
{
    soc_reg_above_64_val_t tm_ctr_ptr_settings;
    uint32 tm_ctr_rjct_filter_mask = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if ((filter_group < 0) || (filter_group>=8))
    {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    SOC_REG_ABOVE_64_CLEAR(tm_ctr_ptr_settings);
    SOCDNX_IF_ERR_EXIT(READ_CGM_TM_CTR_PTR_SETTINGSr(unit, tm_ctr_ptr_settings));

    if (command_id == 0) {
        tm_ctr_rjct_filter_mask = soc_reg_above_64_field32_get(unit, CGM_TM_CTR_PTR_SETTINGSr, tm_ctr_ptr_settings, TM_CTR_A_RJCT_FILTER_MASKf);
    } else {
        tm_ctr_rjct_filter_mask = soc_reg_above_64_field32_get(unit, CGM_TM_CTR_PTR_SETTINGSr, tm_ctr_ptr_settings, TM_CTR_B_RJCT_FILTER_MASKf);
    }

    if (is_active)
    {
        tm_ctr_rjct_filter_mask |= 0x1<<filter_group;
    }
    else
    {
        tm_ctr_rjct_filter_mask &= ~(0x1<<filter_group);
    }

    if (command_id == 0) {
        soc_reg_above_64_field32_set(unit, CGM_TM_CTR_PTR_SETTINGSr, tm_ctr_ptr_settings, TM_CTR_A_RJCT_FILTER_MASKf, tm_ctr_rjct_filter_mask);
    } else {
        soc_reg_above_64_field32_set(unit, CGM_TM_CTR_PTR_SETTINGSr, tm_ctr_ptr_settings, TM_CTR_B_RJCT_FILTER_MASKf, tm_ctr_rjct_filter_mask);
    }

    SOCDNX_IF_ERR_EXIT(WRITE_CGM_TM_CTR_PTR_SETTINGSr(unit, tm_ctr_ptr_settings));

exit:
    SOCDNX_FUNC_RETURN;
}


int qax_stat_group_drop_reasons_get(int unit, int filter_group, uint32 *drop_reasons_mask)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(drop_reasons_mask);

    if ((filter_group < 0) || (filter_group>=8))
    {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, CGM_DROP_REASON_RJCT_MASKm, MEM_BLOCK_ANY, filter_group, drop_reasons_mask));

exit:
    SOCDNX_FUNC_RETURN;
}


int qax_stat_group_drop_reasons_set(int unit, int filter_group, uint32 *drop_reasons_mask)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(drop_reasons_mask);

    if ((filter_group < 0) || (filter_group>=8))
    {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, CGM_DROP_REASON_RJCT_MASKm, MEM_BLOCK_ANY, filter_group, drop_reasons_mask));

exit:
    SOCDNX_FUNC_RETURN;
}

