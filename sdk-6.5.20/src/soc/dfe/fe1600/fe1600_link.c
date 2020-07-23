/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE1600 STAT
 */
 

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <shared/bitop.h>

#if defined(BCM_88750_A0)

#include <soc/dfe/fe1600/fe1600_defs.h>
#include <soc/dfe/fe1600/fe1600_link.h>


STATIC int
_soc_fe1600_linkctrl_linkscan_hw_init(int unit)
{
    uint32 reg_val;

    SOCDNX_INIT_FUNC_DEFS;
	SOC_FE1600_ONLY(unit);

    
    if(SOC_IS_RELOADING(unit) || SOC_WARM_BOOT(unit)) {
        SOC_EXIT;
    }

    
    SOCDNX_IF_ERR_EXIT(soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_MIIM_LINK_SCAN_EN_CLR));

    
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_INT_SEL_MAPr(unit, 0xffffffff));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_INT_SEL_MAP_HIr(unit, 0xffffffff));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_INT_SEL_MAP_HI_2r(unit, 0xffffffff));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_INT_SEL_MAP_HI_3r(unit, 0xffffffff));

    SOCDNX_IF_ERR_EXIT(READ_CMIC_CONFIGr(unit, &reg_val));
    soc_reg_field_set(unit, CMIC_CONFIGr, &reg_val, OVER_RIDE_EXT_MDIO_MSTR_CNTRLf,1);
    soc_reg_field_set(unit, CMIC_CONFIGr, &reg_val, STOP_LS_ON_FIRST_CHANGEf,0);
    soc_reg_field_set(unit, CMIC_CONFIGr, &reg_val, STOP_LS_ON_CHANGEf,0);
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_CONFIGr(unit, reg_val));

  
    
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SCAN_PORTSr(unit, 0xffffffff));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SCAN_PORTS_HIr(unit, 0xffffffff));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SCAN_PORTS_HI_2r(unit, 0xffffffff));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SCAN_PORTS_HI_3r(unit, 0xffffffff));

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC int
_soc_fe1600_linkscan_ports_write(int unit, pbmp_t hw_mii_pbm)
{
    uint32      link_pbmp;
    SOCDNX_INIT_FUNC_DEFS;
	SOC_FE1600_ONLY(unit);

    link_pbmp = SOC_PBMP_WORD_GET(hw_mii_pbm, 0); 
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SCAN_PORTSr(unit, link_pbmp));

    link_pbmp = SOC_PBMP_WORD_GET(hw_mii_pbm, 1); 
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SCAN_PORTS_HIr(unit, link_pbmp));
        
    link_pbmp = SOC_PBMP_WORD_GET(hw_mii_pbm, 2); 
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SCAN_PORTS_HI_2r(unit, link_pbmp));
        
    link_pbmp = SOC_PBMP_WORD_GET(hw_mii_pbm, 3); 
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SCAN_PORTS_HI_3r(unit, link_pbmp));
        
exit:
    SOCDNX_FUNC_RETURN;  
}


STATIC int
_soc_fe1600_linkscan_pause(int unit)
{
    int stall_count;
    pbmp_t pbm;
    uint32 schan_ctrl;
    SOCDNX_INIT_FUNC_DEFS;
	SOC_FE1600_ONLY(unit);

    
    SOC_PBMP_CLEAR(pbm);
    SOCDNX_IF_ERR_EXIT(_soc_fe1600_linkscan_ports_write(unit, pbm));
        
    
    soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_MIIM_LINK_SCAN_EN_CLR);

    if (soc_feature(unit, soc_feature_linkscan_pause_timeout)) {
        soc_timeout_t to;
        
        soc_timeout_init(&to, 1000000 , 100);  
        while (soc_pci_read(unit, CMIC_SCHAN_CTRL) & SC_MIIM_SCAN_BUSY_TST) {
            if (soc_timeout_check(&to)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_TIMEOUT, (_BSL_SOCDNX_MSG("pausing hw linkscan failed")));
            }
        }
    } else {
        
        while (soc_pci_read(unit, CMIC_SCHAN_CTRL) & SC_MIIM_SCAN_BUSY_TST) {
           
        }
    }
            
    COMPILER_REFERENCE(schan_ctrl);
    
    for (stall_count = 0; stall_count < 4; stall_count++) {
        
        schan_ctrl = soc_pci_read(unit, CMIC_SCHAN_CTRL);
    }
   
exit:   
    SOCDNX_FUNC_RETURN;
}


STATIC int
_soc_fe1600_linkscan_continue(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    SOCDNX_INIT_FUNC_DEFS;
	SOC_FE1600_ONLY(unit);

    
    if (soc_feature(unit, soc_feature_phy_cl45))  {
        
        uint32 phy_miim_addr = 0;
        soc_reg_field_set(unit, CMIC_MIIM_ADDRESSr, &phy_miim_addr, CLAUSE_45_DTYPEf, 0x01);
        soc_reg_field_set(unit, CMIC_MIIM_ADDRESSr, &phy_miim_addr, CLAUSE_45_REGADRf, 0x01);
        soc_reg_field_set(unit, CMIC_MIIM_ADDRESSr, &phy_miim_addr, CLAUSE_22_REGADRf, 0x01);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_ADDRESSr(unit, phy_miim_addr));
    } else {
        soc_pci_write(unit, CMIC_MIIM_PARAM, (uint32) 0x01 << 24);
    }
    SOCDNX_IF_ERR_EXIT(_soc_fe1600_linkscan_ports_write(unit, soc->hw_linkscan_pbmp));
    soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_MIIM_LINK_SCAN_EN_SET);
    
            
exit:  
    SOCDNX_FUNC_RETURN;
}

STATIC int
_soc_fe1600_linkctrl_update(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;
	SOC_FE1600_ONLY(unit);

    SOCDNX_FUNC_RETURN;
}


STATIC int
_soc_fe1600_linkctrl_linkscan_config(int unit, pbmp_t hw_mii_pbm, pbmp_t hw_direct_pbm)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    pbmp_t pbm;
    SOCDNX_INIT_FUNC_DEFS;   
	SOC_FE1600_ONLY(unit);

    if (SOC_PBMP_NOT_NULL(hw_mii_pbm) || SOC_PBMP_NOT_NULL(hw_direct_pbm)) {

        
        soc_intr_enable(unit, IRQ_LINK_STAT_MOD);
        
     } else {

        
        soc_intr_disable(unit, IRQ_LINK_STAT_MOD);
     }   

    
    SOC_PBMP_ASSIGN(pbm, hw_mii_pbm);
    SOC_PBMP_OR(pbm, hw_direct_pbm);
    if (SOC_PBMP_NOT_NULL(pbm)) {
        
        soc->soc_flags |= SOC_F_LSE;
    } else {
        soc->soc_flags &= ~SOC_F_LSE;
    }


    
    SOC_PBMP_ASSIGN(soc->hw_linkscan_pbmp, hw_mii_pbm);

    SOCDNX_FUNC_RETURN;
}

STATIC int
_soc_fe1600_linkctrl_hw_link_get(int unit, soc_pbmp_t *hw_link)
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;
	SOC_FE1600_ONLY(unit);

    SOCDNX_NULL_CHECK(hw_link);

    SOC_PBMP_CLEAR(*hw_link);

    SOCDNX_IF_ERR_EXIT(READ_CMIC_LINK_STATr(unit, &reg_val));
    SOC_PBMP_WORD_SET(*hw_link,0,reg_val);

    SOCDNX_IF_ERR_EXIT(READ_CMIC_LINK_STAT_HIr(unit, &reg_val));
    SOC_PBMP_WORD_SET(*hw_link,1,reg_val);
    
     SOCDNX_IF_ERR_EXIT(READ_CMIC_LINK_STAT_HI_2r(unit, &reg_val));
    SOC_PBMP_WORD_SET(*hw_link,2,reg_val);
    
    SOCDNX_IF_ERR_EXIT(READ_CMIC_LINK_STAT_HI_3r(unit, &reg_val));
    SOC_PBMP_WORD_SET(*hw_link,3,reg_val);  

exit:
    SOCDNX_FUNC_RETURN;
}


soc_linkctrl_driver_t  soc_linkctrl_driver_fe1600 = {
    NULL,                                   
    _soc_fe1600_linkctrl_linkscan_hw_init,  
    _soc_fe1600_linkctrl_linkscan_config,   
    _soc_fe1600_linkscan_pause,             
    _soc_fe1600_linkscan_continue,          
    _soc_fe1600_linkctrl_update,            
    _soc_fe1600_linkctrl_hw_link_get        
};

int
soc_fe1600_linkctrl_init(int unit) 
{
    SOCDNX_INIT_FUNC_DEFS;
	SOC_FE1600_ONLY(unit);

    SOCDNX_IF_ERR_EXIT(soc_linkctrl_init(unit, &soc_linkctrl_driver_fe1600));  

exit:
    SOCDNX_FUNC_RETURN;
}

#endif 

#undef _ERR_MSG_MODULE_NAME

