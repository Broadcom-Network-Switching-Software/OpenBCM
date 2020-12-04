/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC RAMON FABRIC STATUS
 */
 
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC


#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/ramon/ramon_fabric_status.h>
#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>
#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnxc/error.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>

#include <soc/mem.h>



shr_error_e 
soc_ramon_fabric_link_status_get(int unit, soc_port_t link_id, uint32 *link_status, uint32 *errored_token_count)
{
    uint32 reg_val, field_val[1];
    uint32 sig_acc = 0;
    uint32 sig_lock[1] = {0};
    int blk_id, reg_select, links_in_fmac;
    soc_port_t inner_lnk;
    int srd_id;
    int rv;
    uint32 link_status_tmp = 0x0;
    SHR_FUNC_INIT_VARS(unit);

    links_in_fmac = dnxf_data_device.blocks.nof_links_in_fmac_get(unit);
    blk_id = INT_DEVIDE(link_id, links_in_fmac);
    reg_select = link_id % links_in_fmac;
   
    
    if (reg_select >= 0 && reg_select < links_in_fmac)
    {
        SHR_IF_ERR_EXIT(READ_FMAC_LEAKY_BUCKETr(unit, blk_id, reg_select, &reg_val));
        *errored_token_count = soc_reg_field_get(unit, FMAC_LEAKY_BUCKETr, reg_val, MACR_N_LKY_BKT_VALUEf);
    } else {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Can't find register for link %d",link_id);
    }
    
  
  
    inner_lnk = link_id % links_in_fmac;
    SHR_IF_ERR_EXIT(READ_FMAC_FMAC_INTERRUPT_REGISTER_1r(unit,blk_id,&reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_FMAC_INTERRUPT_REGISTER_1r, reg_val, RX_CRC_ERR_N_INTf);
    if(SHR_BITGET(field_val, inner_lnk)) {
        link_status_tmp |= DNXC_FABRIC_LINK_STATUS_CRC_ERROR;
    }
    
    *field_val = soc_reg_field_get(unit, FMAC_FMAC_INTERRUPT_REGISTER_1r, reg_val, WRONG_SIZE_INTf);
    if(SHR_BITGET(field_val, inner_lnk)) {
        link_status_tmp |= DNXC_FABRIC_LINK_STATUS_SIZE_ERROR;  
    }
       
    SHR_IF_ERR_EXIT(READ_FMAC_FMAC_INTERRUPT_REGISTER_2r(unit,blk_id,&reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_FMAC_INTERRUPT_REGISTER_2r, reg_val, RX_LOST_OF_SYNCf);
    if(SHR_BITGET(field_val, inner_lnk)) {
       link_status_tmp |= DNXC_FABRIC_LINK_STATUS_MISALIGN;  
    }
      
    SHR_IF_ERR_EXIT(READ_FMAC_FMAC_INTERRUPT_REGISTER_4r(unit,blk_id,&reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_FMAC_INTERRUPT_REGISTER_4r, reg_val, DEC_ERR_INTf);
    if(SHR_BITGET(field_val, inner_lnk)) {
       link_status_tmp |= DNXC_FABRIC_LINK_STATUS_CODE_GROUP_ERROR; 
    } 
      
    srd_id = link_id / dnxf_data_device.blocks.nof_links_in_fsrd_get(unit);
    inner_lnk = link_id % dnxf_data_device.blocks.nof_links_in_fsrd_get(unit);
    SHR_IF_ERR_EXIT(READ_FSRD_SRD_STATUSr(unit, srd_id, &reg_val));
    *sig_lock = soc_reg_field_get(unit, FSRD_SRD_STATUSr, reg_val, SRD_N_SYNC_STATUSf);
    if(!(SHR_BITGET(sig_lock,inner_lnk))) {
        link_status_tmp |= DNXC_FABRIC_LINK_STATUS_NO_SIG_LOCK;
    }
 
    
    rv = soc_dnxc_port_rx_locked_get(unit, link_id, &sig_acc);
    SHR_IF_ERR_EXIT(rv);
    if(!sig_acc) {
        link_status_tmp |= DNXC_FABRIC_LINK_STATUS_NO_SIG_ACCEP;
    }
    
    if(*errored_token_count < 63) {
       link_status_tmp |= DNXC_FABRIC_LINK_STATUS_ERRORED_TOKENS;
    }
    
    
    rv = soc_ramon_fabric_link_status_clear(unit, link_id);
    SHR_IF_ERR_EXIT(rv);

    *link_status = link_status_tmp;

exit:
    SHR_FUNC_EXIT;
  
}


shr_error_e 
soc_ramon_fabric_link_status_clear(int unit, soc_port_t link)
{
    int blk_id, inner_link, links_in_fmac;
    uint32 reg_val;
    SHR_FUNC_INIT_VARS(unit);

    links_in_fmac = dnxf_data_device.blocks.nof_links_in_fmac_get(unit);
    blk_id = link / links_in_fmac;
    inner_link = link % links_in_fmac;

    reg_val = 0x0;
    soc_reg_field_set(unit, FMAC_FMAC_INTERRUPT_REGISTER_1r, &reg_val, RX_CRC_ERR_N_INTf, 1 << inner_link);
    soc_reg_field_set(unit, FMAC_FMAC_INTERRUPT_REGISTER_1r, &reg_val, WRONG_SIZE_INTf, 1 << inner_link);
    SHR_IF_ERR_EXIT(WRITE_FMAC_FMAC_INTERRUPT_REGISTER_1r(unit,blk_id,reg_val));

    reg_val = 0x0;
    soc_reg_field_set(unit, FMAC_FMAC_INTERRUPT_REGISTER_2r, &reg_val, RX_LOST_OF_SYNCf, 1 << inner_link);
    SHR_IF_ERR_EXIT(WRITE_FMAC_FMAC_INTERRUPT_REGISTER_2r(unit,blk_id,reg_val));

    reg_val = 0x0;
    soc_reg_field_set(unit, FMAC_FMAC_INTERRUPT_REGISTER_4r, &reg_val, DEC_ERR_INTf, 1 << inner_link);
    SHR_IF_ERR_EXIT(WRITE_FMAC_FMAC_INTERRUPT_REGISTER_4r(unit,blk_id,reg_val));
    
exit:
    SHR_FUNC_EXIT;
  
}


shr_error_e 
soc_ramon_fabric_reachability_status_get(int unit, int moduleid, int links_max, uint32 *links_array, int *links_count)
{
    int i;
    uint32 rtp_reg_val[DNXF_DATA_MAX_FABRIC_REACHABILITY_TABLE_ROW_SIZE_IN_UINT32];
    soc_reg_above_64_val_t intergrity_vec;

    SHR_FUNC_INIT_VARS(unit);
    
    SHR_IF_ERR_EXIT(READ_RTP_RMHMTm(unit, MEM_BLOCK_ANY, moduleid, rtp_reg_val));
    SHR_IF_ERR_EXIT(READ_RTP_LINK_INTEGRITY_VECTORr(unit, intergrity_vec));
    
    *links_count = 0;

    
    
    BCM_PBMP_ITER(PBMP_SFI_ALL(unit), i)
    {
        if(SHR_BITGET(intergrity_vec,i) && SHR_BITGET(rtp_reg_val,i)) 
        {
            if(*links_count >= links_max) {
                SHR_ERR_EXIT(_SHR_E_FULL, "links_array is too small");
            }
            
            links_array[*links_count] = i;
            (*links_count)++;
        }
    }
    
exit:
    SHR_FUNC_EXIT;
}



shr_error_e 
soc_ramon_fabric_link_connectivity_status_get(int unit, soc_port_t link_id, bcm_fabric_link_connectivity_t *link_partner)
{
    int blk_id, source_lvl;
    soc_port_t internal_link_id;
    uint32 reg_val;
    soc_reg_above_64_val_t intergrity_vec;
    int nof_links_in_cch;
    int link_mask;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_RTP_LINK_INTEGRITY_VECTORr(unit, intergrity_vec));
    if(SHR_BITGET(intergrity_vec,link_id)) {

        nof_links_in_cch = dnxf_data_device.blocks.nof_links_in_cch_get(unit);

        internal_link_id = link_id % nof_links_in_cch;
        blk_id = INT_DEVIDE(link_id, nof_links_in_cch);
        
        switch(internal_link_id)
        {
            case 0:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_0r, blk_id, 0, &reg_val));
                break;
            case 1:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_1r, blk_id, 0, &reg_val));
                break;
            case 2:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_2r, blk_id, 0, &reg_val));
                break;
            case 3:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_3r, blk_id, 0, &reg_val));
                break;
            case 4:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_4r, blk_id, 0, &reg_val));
                break;
            case 5:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_5r, blk_id, 0, &reg_val));
                break;
            case 6:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_6r, blk_id, 0, &reg_val));
                break;
            case 7:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_7r, blk_id, 0, &reg_val));
                break;
            case 8:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_8r, blk_id, 0, &reg_val));
                break;
            case 9:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_9r, blk_id, 0, &reg_val));
                break;
            case 10:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_10r, blk_id, 0, &reg_val));
                break;
            case 11:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_11r, blk_id, 0, &reg_val));
                break;
            case 12:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_12r, blk_id, 0, &reg_val));
                break;
            case 13:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_13r, blk_id, 0, &reg_val));
                break;
            case 14:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_14r, blk_id, 0, &reg_val));
                break;
            case 15:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_15r, blk_id, 0, &reg_val));
                break;
            case 16:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_16r, blk_id, 0, &reg_val));
                break;
            case 17:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_17r, blk_id, 0, &reg_val));
                break;
            case 18:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_18r, blk_id, 0, &reg_val));
                break;
            case 19:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_19r, blk_id, 0, &reg_val));
                break;
            case 20:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_20r, blk_id, 0, &reg_val));
                break;
            case 21:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_21r, blk_id, 0, &reg_val));
                break;
            case 22:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_22r, blk_id, 0, &reg_val));
                break;
            case 23:
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_CONNECTIVITY_LINK_23r, blk_id, 0, &reg_val));
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong link id");
        }
        
        if(reg_val & (0x1 << 22)) 
        {
            link_partner->module_id = reg_val & 0x7FF;
            
            source_lvl = (reg_val >> 11) & 0x7;
            
            
          
          
            switch(source_lvl)
            {
                case soc_dnxc_device_type_actual_value_FAP_1:
                case soc_dnxc_device_type_actual_value_FIP:
                case soc_dnxc_device_type_actual_value_FOP:
                case soc_dnxc_device_type_actual_value_FAP:
                    link_partner->device_type = bcmFabricDeviceTypeFAP;
                    break;
                case soc_dnxc_device_type_actual_value_FE3:
                case soc_dnxc_device_type_actual_value_FE1:
                    link_partner->device_type = bcmFabricDeviceTypeFE13;
                    break;
                case soc_dnxc_device_type_actual_value_FE2:
                case soc_dnxc_device_type_actual_value_FE2_1:
                    link_partner->device_type = bcmFabricDeviceTypeFE2;
                    break;
            } 
            
            link_mask = dnxf_data_port.general.link_mask_get(unit);
            link_partner->link_id = (reg_val >> 14) & link_mask;
        }
        else
        {
            link_partner->link_id = DNXC_FABRIC_LINK_NO_CONNECTIVITY; 
        }
    } else {
        link_partner->link_id = DNXC_FABRIC_LINK_NO_CONNECTIVITY; 
    }
    
exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
