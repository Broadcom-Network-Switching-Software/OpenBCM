/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE1600 FABRIC STATUS
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/mem.h>

#include <soc/defs.h>
#include <soc/error.h>
#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <shared/bitop.h>
#include <soc/dcmn/dcmn_defs.h>

#if defined(BCM_88750_A0)

#include <soc/dfe/fe1600/fe1600_defs.h>
#include <soc/dfe/fe1600/fe1600_fabric_status.h>


soc_error_t
soc_fe1600_fabric_link_status_all_get(int unit, int links_array_max_size, uint32* link_status, uint32* errored_token_count, int* links_array_count)
{
    int i, rc;
    SOCDNX_INIT_FUNC_DEFS;

    (*links_array_count) = 0;

    for(i=0 ; i<SOC_DFE_DEFS_GET(unit, nof_links) && i<links_array_max_size; i++ ) {
        if (!SOC_PBMP_MEMBER(PBMP_SFI_ALL(unit), i)) {
            link_status[i] = DCMN_FABRIC_LINK_NO_CONNECTIVITY;
            errored_token_count[i] = 0;
        } else {
            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_link_status_get, (unit, i, &(link_status[i]), &(errored_token_count[i])));
            SOCDNX_IF_ERR_EXIT(rc);
        }
        (*links_array_count)++;
    }

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t 
soc_fe1600_fabric_link_status_get(int unit, soc_port_t link_id, uint32 *link_status, uint32 *errored_token_count)
{
    uint32 reg_val, field_val[1], sig_acc = 0;
    uint32 sig_lock[1] = {0};
    int blk_id, reg_select;
    soc_port_t inner_lnk;
    int srd_id, srd_arr_id;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;
	SOC_FE1600_ONLY(unit);

    *link_status = 0;

    blk_id = INT_DEVIDE(link_id, 4);
    reg_select = link_id % 4;
   
    
    if (reg_select >= 0 && reg_select < 4)
    {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_LEAKY_BUCKETr(unit, blk_id, reg_select, &reg_val));
        *errored_token_count = soc_reg_field_get(unit, FMAC_LEAKY_BUCKETr, reg_val, MACR_N_LKY_BKT_VALUEf);
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Can't find register for link %d"),link_id));
    }
    
  
  
  
    inner_lnk = link_id % 4;
    SOCDNX_IF_ERR_EXIT(READ_FMAC_INTERRUPT_REGISTER_1r(unit,blk_id,&reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_INTERRUPT_REGISTER_1r, reg_val, RX_CRC_ERR_N_INTf);
    if(SHR_BITGET(field_val, inner_lnk)) {
        *link_status |= DCMN_FABRIC_LINK_STATUS_CRC_ERROR;
    }
    
    *field_val = soc_reg_field_get(unit, FMAC_INTERRUPT_REGISTER_1r, reg_val, WRONG_SIZE_INTf);
    if(SHR_BITGET(field_val, inner_lnk)) {
        *link_status |= DCMN_FABRIC_LINK_STATUS_SIZE_ERROR;  
    }
       
    SOCDNX_IF_ERR_EXIT(READ_FMAC_INTERRUPT_REGISTER_2r(unit,blk_id,&reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_INTERRUPT_REGISTER_2r, reg_val, RX_LOST_OF_SYNCf);
    if(SHR_BITGET(field_val, inner_lnk)) {
       *link_status |= DCMN_FABRIC_LINK_STATUS_MISALIGN;  
    }
      
    SOCDNX_IF_ERR_EXIT(READ_FMAC_INTERRUPT_REGISTER_4r(unit,blk_id,&reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_INTERRUPT_REGISTER_4r, reg_val, DEC_ERR_INTf);
    if(SHR_BITGET(field_val, inner_lnk)) {
       *link_status |= DCMN_FABRIC_LINK_STATUS_CODE_GROUP_ERROR; 
    } 
      
#ifdef BCM_88754_A0
    if (!SOC_IS_BCM88754_A0(unit))
    {
#endif
        srd_id = (link_id*SOC_DFE_DEFS_GET(unit, nof_instances_mac_fsrd))/SOC_DFE_DEFS_GET(unit, nof_links);
        srd_arr_id = (link_id%(SOC_DFE_DEFS_GET(unit, nof_links)/SOC_DFE_DEFS_GET(unit, nof_instances_mac_fsrd)))/SOC_FE1600_NOF_QUADS_IN_FSRD;
        SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_QUAD_STATUSr(unit, srd_id, srd_arr_id, &reg_val));
        *sig_lock = soc_reg_field_get(unit, FSRD_SRD_QUAD_STATUSr, reg_val, SRD_QUAD_N_SYNC_STATUSf);
        if(!(SHR_BITGET(sig_lock,inner_lnk))) {
            *link_status |= DCMN_FABRIC_LINK_STATUS_NO_SIG_LOCK;
        }
#ifdef BCM_88754_A0
    }
#endif
    
    SOCDNX_IF_ERR_EXIT(soc_phyctrl_control_get(unit, link_id, SOC_PHY_CONTROL_RX_SIGNAL_DETECT, &sig_acc)); 
    if(!sig_acc) {
        *link_status |= DCMN_FABRIC_LINK_STATUS_NO_SIG_ACCEP;
    }
    
    if(*errored_token_count < 63) {
       *link_status |= DCMN_FABRIC_LINK_STATUS_ERRORED_TOKENS;
    }
    
    
    rv = soc_fe1600_fabric_link_status_clear(unit, link_id);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
  
}


soc_error_t 
soc_fe1600_fabric_link_status_clear(int unit, soc_port_t link)
{
    int blk_id, inner_link;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;
	SOC_FE1600_ONLY(unit);

    blk_id = link / 4;
    inner_link = link % 4;

    reg_val = 0x0;
    soc_reg_field_set(unit, FMAC_INTERRUPT_REGISTER_1r, &reg_val, RX_CRC_ERR_N_INTf, 1 << inner_link);
    soc_reg_field_set(unit, FMAC_INTERRUPT_REGISTER_1r, &reg_val, WRONG_SIZE_INTf, 1 << inner_link);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_1r(unit,blk_id,reg_val));

    reg_val = 0x0;
    soc_reg_field_set(unit, FMAC_INTERRUPT_REGISTER_2r, &reg_val, RX_LOST_OF_SYNCf, 1 << inner_link);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_2r(unit,blk_id,reg_val));

    reg_val = 0x0;
    soc_reg_field_set(unit, FMAC_INTERRUPT_REGISTER_4r, &reg_val, DEC_ERR_INTf, 1 << inner_link);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_4r(unit,blk_id,reg_val));
    
exit:
    SOCDNX_FUNC_RETURN;
  
}

soc_error_t 
soc_fe1600_fabric_reachability_status_get(int unit, int moduleid, int links_max, uint32 *links_array, int *links_count)
{
    int i;
    uint32 rtp_reg_val[5];
    soc_reg_above_64_val_t intergrity_vec;
    int nof_links;
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(READ_RTP_RMHMTm(unit, MEM_BLOCK_ANY, moduleid, rtp_reg_val));
    SOCDNX_IF_ERR_EXIT(READ_RTP_LINK_INTEGRITY_VECTORr(unit, intergrity_vec));
    
    *links_count = 0;

    nof_links = SOC_DFE_DEFS_GET(unit, nof_links);
    
    for(i=0 ; i<nof_links ; i++)
    {
        
        if (!IS_SFI_PORT(unit, i)) {
            continue;
        }

        if(SHR_BITGET(intergrity_vec,i) && SHR_BITGET(rtp_reg_val,i)) 
        {
            if(*links_count >= links_max) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("links_array is too small")));
            }
            
            links_array[*links_count] = i;
            (*links_count)++;
        }
    } 
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe1600_fabric_link_connectivity_status_get(int unit, soc_port_t link_id, bcm_fabric_link_connectivity_t *link_partner)
{
    int blk_id, source_lvl;
    soc_port_t internal_link_id;
    uint32 reg_val;
    soc_reg_above_64_val_t intergrity_vec;
    int nof_links_in_dcl;
    int link_mask;
    SOCDNX_INIT_FUNC_DEFS;
    

    SOCDNX_IF_ERR_EXIT(READ_RTP_LINK_INTEGRITY_VECTORr(unit, intergrity_vec));
    if(SHR_BITGET(intergrity_vec,link_id)) {

        nof_links_in_dcl = SOC_DFE_DEFS_GET(unit, nof_links_in_dcl);

        internal_link_id = link_id % nof_links_in_dcl;
        blk_id = INT_DEVIDE(link_id, nof_links_in_dcl);
        
        switch(internal_link_id)
        {
            case 0:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_0r(unit, blk_id, &reg_val));
                break;
            case 1:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_1r(unit, blk_id, &reg_val)); 
                break;
            case 2:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_2r(unit, blk_id, &reg_val));
                break;
            case 3:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_3r(unit, blk_id, &reg_val));
                break;
            case 4:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_4r(unit, blk_id, &reg_val));
                break;
            case 5:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_5r(unit, blk_id, &reg_val));
                break;
            case 6:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_6r(unit, blk_id, &reg_val));
                break;
            case 7:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_7r(unit, blk_id, &reg_val));
                break;
            case 8:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_8r(unit, blk_id, &reg_val));
                break;
            case 9:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_9r(unit, blk_id, &reg_val));
                break;
            case 10:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_10r(unit, blk_id, &reg_val));
                break;
            case 11:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_11r(unit, blk_id, &reg_val));
                break;
            case 12:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_12r(unit, blk_id, &reg_val));
                break;
            case 13:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_13r(unit, blk_id, &reg_val));
                break;
            case 14:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_14r(unit, blk_id, &reg_val));
                break;
            case 15:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_15r(unit, blk_id, &reg_val));
                break;
            case 16:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_16r(unit, blk_id, &reg_val));
                break;
            case 17:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_17r(unit, blk_id, &reg_val)); 
                break;
            case 18:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_18r(unit, blk_id, &reg_val));
                break;
            case 19:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_19r(unit, blk_id, &reg_val));
                break;
            case 20:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_20r(unit, blk_id, &reg_val));
                break;
            case 21:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_21r(unit, blk_id, &reg_val));
                break;
            case 22:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_22r(unit, blk_id, &reg_val));
                break;
            case 23:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_23r(unit, blk_id, &reg_val));
                break;
            case 24:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_24r(unit, blk_id, &reg_val)); 
                break;
            case 25:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_25r(unit, blk_id, &reg_val));
                break;
            case 26:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_26r(unit, blk_id, &reg_val));
                break;
            case 27:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_27r(unit, blk_id, &reg_val));
                break;
            case 28:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_28r(unit, blk_id, &reg_val));
                break;
            case 29:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_29r(unit, blk_id, &reg_val)); 
                break;
            case 30:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_30r(unit, blk_id, &reg_val));
                break;
            case 31:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_31r(unit, blk_id, &reg_val));
                break;
#ifdef BCM_88950
             case 32:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_32r(unit, blk_id, &reg_val));
                break;
             case 33:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_33r(unit, blk_id, &reg_val));
                break;
             case 34:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_34r(unit, blk_id, &reg_val));
                break;
             case 35:
                SOCDNX_IF_ERR_EXIT(READ_DCL_CONNECTIVITY_LINK_35r(unit, blk_id, &reg_val));
                break;
#endif
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong link id")));
        }
        
        if(reg_val & (0x1 << 22)) 
        {
            link_partner->module_id = reg_val & 0x7FF;
            
            source_lvl = (reg_val >> 11) & 0x7;
            
            
          
          
            switch(source_lvl)
            {
                case soc_dcmn_device_type_actual_value_FAP_1:
                case soc_dcmn_device_type_actual_value_FIP:
                case soc_dcmn_device_type_actual_value_FOP:
                case soc_dcmn_device_type_actual_value_FAP:
                    link_partner->device_type = bcmFabricDeviceTypeFAP;
                    break;
                case soc_dcmn_device_type_actual_value_FE3:
                case soc_dcmn_device_type_actual_value_FE1:
                    link_partner->device_type = bcmFabricDeviceTypeFE13;
                    break;
                case soc_dcmn_device_type_actual_value_FE2:
                case soc_dcmn_device_type_actual_value_FE2_1:
                    link_partner->device_type = bcmFabricDeviceTypeFE2;
                    break;
            } 
            
            link_mask = SOC_DFE_DEFS_GET(unit, link_mask);
            link_partner->link_id = (reg_val >> 14) & link_mask;
        }
        else
        {
            link_partner->link_id = DCMN_FABRIC_LINK_NO_CONNECTIVITY; 
        }
    } else {
        link_partner->link_id = DCMN_FABRIC_LINK_NO_CONNECTIVITY; 
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

#endif 

#undef _ERR_MSG_MODULE_NAME

