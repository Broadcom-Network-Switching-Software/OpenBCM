/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE3200 FABRIC STATUS
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC


#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/fe3200/fe3200_fabric_status.h>
#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>
#include <soc/dcmn/dcmn_defs.h>
#include <soc/dcmn/error.h>

#include <soc/mem.h>



soc_error_t 
soc_fe3200_fabric_link_status_get(int unit, soc_port_t link_id, uint32 *link_status, uint32 *errored_token_count)
{
    uint32 reg_val, field_val[1];
    uint32 sig_acc = 0;
    uint32 sig_lock[1] = {0};
    int blk_id, reg_select;
    soc_port_t inner_lnk;
    int srd_id, srd_arr_id;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

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
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAC_INTERRUPT_REGISTER_1r(unit,blk_id,&reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_FMAC_INTERRUPT_REGISTER_1r, reg_val, RX_CRC_ERR_N_INTf);
    if(SHR_BITGET(field_val, inner_lnk)) {
        *link_status |= DCMN_FABRIC_LINK_STATUS_CRC_ERROR;
    }
    
    *field_val = soc_reg_field_get(unit, FMAC_FMAC_INTERRUPT_REGISTER_1r, reg_val, WRONG_SIZE_INTf);
    if(SHR_BITGET(field_val, inner_lnk)) {
        *link_status |= DCMN_FABRIC_LINK_STATUS_SIZE_ERROR;  
    }
       
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAC_INTERRUPT_REGISTER_2r(unit,blk_id,&reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_FMAC_INTERRUPT_REGISTER_2r, reg_val, RX_LOST_OF_SYNCf);
    if(SHR_BITGET(field_val, inner_lnk)) {
       *link_status |= DCMN_FABRIC_LINK_STATUS_MISALIGN;  
    }
      
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAC_INTERRUPT_REGISTER_4r(unit,blk_id,&reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_FMAC_INTERRUPT_REGISTER_4r, reg_val, DEC_ERR_INTf);
    if(SHR_BITGET(field_val, inner_lnk)) {
       *link_status |= DCMN_FABRIC_LINK_STATUS_CODE_GROUP_ERROR; 
    } 
      
    srd_id = (link_id*SOC_DFE_DEFS_GET(unit, nof_instances_mac_fsrd))/SOC_DFE_DEFS_GET(unit, nof_links);
    srd_arr_id = (link_id % SOC_DFE_DEFS_GET(unit, nof_links_in_fsrd))/SOC_DFE_DEFS_GET(unit, nof_links_in_quad); 
    SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_QUAD_STATUSr(unit, srd_id, srd_arr_id, &reg_val));
    *sig_lock = soc_reg_field_get(unit, FSRD_SRD_QUAD_STATUSr, reg_val, SRD_QUAD_N_SYNC_STATUSf);
    if(!(SHR_BITGET(sig_lock,inner_lnk))) {
        *link_status |= DCMN_FABRIC_LINK_STATUS_NO_SIG_LOCK;
    }
 
    
    rv = soc_dcmn_port_rx_locked_get(unit, link_id, &sig_acc);
    SOCDNX_IF_ERR_EXIT(rv);
    if(!sig_acc) {
        *link_status |= DCMN_FABRIC_LINK_STATUS_NO_SIG_ACCEP;
    }
    
    if(*errored_token_count < 63) {
       *link_status |= DCMN_FABRIC_LINK_STATUS_ERRORED_TOKENS;
    }
    
    
    rv = soc_fe3200_fabric_link_status_clear(unit, link_id);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
  
}


soc_error_t 
soc_fe3200_fabric_link_status_clear(int unit, soc_port_t link)
{
    int blk_id, inner_link;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    blk_id = link / 4;
    inner_link = link % 4;

    reg_val = 0x0;
    soc_reg_field_set(unit, FMAC_FMAC_INTERRUPT_REGISTER_1r, &reg_val, RX_CRC_ERR_N_INTf, 1 << inner_link);
    soc_reg_field_set(unit, FMAC_FMAC_INTERRUPT_REGISTER_1r, &reg_val, WRONG_SIZE_INTf, 1 << inner_link);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAC_INTERRUPT_REGISTER_1r(unit,blk_id,reg_val));

    reg_val = 0x0;
    soc_reg_field_set(unit, FMAC_FMAC_INTERRUPT_REGISTER_2r, &reg_val, RX_LOST_OF_SYNCf, 1 << inner_link);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAC_INTERRUPT_REGISTER_2r(unit,blk_id,reg_val));

    reg_val = 0x0;
    soc_reg_field_set(unit, FMAC_FMAC_INTERRUPT_REGISTER_4r, &reg_val, DEC_ERR_INTf, 1 << inner_link);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAC_INTERRUPT_REGISTER_4r(unit,blk_id,reg_val));
    
exit:
    SOCDNX_FUNC_RETURN;
  
}


#undef _ERR_MSG_MODULE_NAME
