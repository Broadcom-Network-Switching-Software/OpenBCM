 /* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * File:        jer_link.c
 * Purpose:     Hardware Linkscan module
 *
 * These routines will be called by the linkscan module,
 * so they need to be defined and return SOC_E_NONE.
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT


#include <shared/bsl.h>
#include <soc/linkctrl.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/cmicm.h>
#include <soc/mcm/memregs.h>
#include <soc/mcm/cmicm.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/JER/jer_link.h>
#include <soc/dpp/port_sw_db.h>
#if defined(PORTMOD_SUPPORT)
#include <soc/portmod/portmod.h>
#endif


STATIC int
_soc_jer_linkctrl_linkscan_hw_init(int unit)
{

    uint32 reg32_val;
    SOCDNX_INIT_FUNC_DEFS;

    
    if(SOC_WARM_BOOT(unit)) {
        SOC_EXIT;
    }

    
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_INT_SEL_MAP_0r(unit,  0xffffffff));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_INT_SEL_MAP_1r(unit,  0xffffffff));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_INT_SEL_MAP_2r(unit,  0xffffffff));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_INT_SEL_MAP_3r(unit,  0xffffffff));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_INT_SEL_MAP_4r(unit,  0xffffffff));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_INT_SEL_MAP_5r(unit,  0xffffffff));

    
    SOCDNX_IF_ERR_EXIT(READ_CMIC_MIIM_SCAN_CTRLr(unit, &reg32_val));
    
    soc_reg_field_set(unit, CMIC_MIIM_SCAN_CTRLr, &reg32_val, MIIM_LINK_SCAN_ENf, 0);
    soc_reg_field_set(unit, CMIC_MIIM_SCAN_CTRLr, &reg32_val, STOP_LS_ON_FIRST_CHANGEf, 0);
    soc_reg_field_set(unit, CMIC_MIIM_SCAN_CTRLr, &reg32_val, STOP_LS_ON_CHANGEf, 0);
    soc_reg_field_set(unit, CMIC_MIIM_SCAN_CTRLr, &reg32_val, OVER_RIDE_EXT_MDIO_MSTR_CNTRLf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_SCAN_CTRLr(unit, reg32_val));

    
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_SCAN_PORTS_0r(unit, 0x0));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_SCAN_PORTS_1r(unit, 0x0));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_SCAN_PORTS_2r(unit, 0x0));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_SCAN_PORTS_3r(unit, 0x0));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_SCAN_PORTS_4r(unit, 0x0));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_SCAN_PORTS_5r(unit, 0x0));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
_soc_jer_ports_phy_to_cmic_bitmap_map(int unit, pbmp_t hw_mii_pbm, soc_pbmp_t  *cmic_bitmap)
{
    soc_port_t port;
    uint32 cmic_port, phy_port;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_PBMP_CLEAR(*cmic_bitmap);
    PBMP_ITER(hw_mii_pbm, port)
    {
        if (IS_SFI_PORT(unit, port))
        {
            phy_port = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
            cmic_port = phy_port + 144;
        }
        else
        {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port));
            cmic_port = phy_port - 1;
            
            if (SOC_IS_QAX(unit) && !SOC_IS_QUX(unit)) {
                cmic_port += 64;
            }
        }
        
        if (cmic_port > _SHR_PBMP_PORT_MAX) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Port %d is invalid phy %d"),port, phy_port));
        }
        
        SOC_PBMP_PORT_ADD(*cmic_bitmap, cmic_port);
    }

exit:
    SOCDNX_FUNC_RETURN;

}

STATIC int
_soc_jer_linkscan_ports_write(int unit, pbmp_t hw_mii_pbm)
{
    uint32      link_pbmp;
    pbmp_t cmic_bmp;
     
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(_soc_jer_ports_phy_to_cmic_bitmap_map(unit, hw_mii_pbm, &cmic_bmp));
    link_pbmp = SOC_PBMP_WORD_GET(cmic_bmp, 0); 
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_SCAN_PORTS_0r(unit, link_pbmp));
    link_pbmp = SOC_PBMP_WORD_GET(cmic_bmp, 1); 
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_SCAN_PORTS_1r(unit, link_pbmp));
    link_pbmp = SOC_PBMP_WORD_GET(cmic_bmp, 2); 
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_SCAN_PORTS_2r(unit, link_pbmp));
    link_pbmp = SOC_PBMP_WORD_GET(cmic_bmp, 3); 
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_SCAN_PORTS_3r(unit, link_pbmp));
    link_pbmp = SOC_PBMP_WORD_GET(cmic_bmp, 4); 
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_SCAN_PORTS_4r(unit, link_pbmp));
    link_pbmp = SOC_PBMP_WORD_GET(cmic_bmp, 5); 
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_SCAN_PORTS_5r(unit, link_pbmp));
  
exit:
    SOCDNX_FUNC_RETURN;  
}


STATIC int
_soc_jer_linkctrl_linkscan_pause(int unit)
{
    int stall_count;
    pbmp_t pbm;
    uint32 schan_ctrl;
    soc_timeout_t to;
    uint32 busy_status, reg32_val;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_PBMP_CLEAR(pbm);
    SOCDNX_IF_ERR_EXIT(_soc_jer_linkscan_ports_write(unit, pbm));

    
    SOCDNX_IF_ERR_EXIT(READ_CMIC_MIIM_SCAN_CTRLr(unit,&schan_ctrl));
    soc_reg_field_set(unit, CMIC_MIIM_SCAN_CTRLr, &schan_ctrl, MIIM_LINK_SCAN_ENf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_SCAN_CTRLr(unit,schan_ctrl));
    
    
    soc_timeout_init(&to, 1000000 , 100);
    busy_status = 1;
    while (busy_status) 
    {
        if (soc_timeout_check(&to)) {
           SOCDNX_EXIT_WITH_ERR(SOC_E_TIMEOUT, (_BSL_SOCDNX_MSG("pausing hw linkscan failed")));
        }

        SOCDNX_IF_ERR_EXIT(READ_CMIC_MIIM_SCAN_STATUSr(unit, &reg32_val));
        busy_status = soc_reg_field_get(unit, CMIC_MIIM_SCAN_STATUSr, reg32_val, MIIM_SCAN_BUSYf);
    }

    COMPILER_REFERENCE(schan_ctrl);
    
    for (stall_count = 0; stall_count < 4; stall_count++) {
        
        schan_ctrl = soc_pci_read(unit, CMIC_MIIM_SCAN_STATUS_OFFSET);
    }
   
exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
_soc_jer_linkctrl_linkscan_continue(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    uint32 schan_ctrl;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(_soc_jer_linkscan_ports_write(unit, soc->hw_linkscan_pbmp));
    SOCDNX_IF_ERR_EXIT(READ_CMIC_MIIM_SCAN_CTRLr(unit,&schan_ctrl));
    soc_reg_field_set(unit, CMIC_MIIM_SCAN_CTRLr, &schan_ctrl, MIIM_LINK_SCAN_ENf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_SCAN_CTRLr(unit,schan_ctrl));
          
exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
_soc_jer_linkctrl_linkscan_config(int unit, pbmp_t hw_mii_pbm,
                                    pbmp_t hw_direct_pbm)
{
    uint32 reg32_val;
    soc_control_t *soc = SOC_CONTROL(unit);
    pbmp_t pbm;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_PBMP_NOT_NULL(hw_mii_pbm) || SOC_PBMP_NOT_NULL(hw_direct_pbm)) {

        
        soc_cmicm_intr1_enable(unit, IRQ_CMCx_LINK_STAT_MOD);

        
        reg32_val = 0;
        soc_reg_field_set(unit, CMIC_MIIM_CLR_SCAN_STATUSr, &reg32_val, CLR_LINK_STATUS_CHANGEf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_CLR_SCAN_STATUSr(unit, reg32_val));
        
     } else {

        
        soc_cmicm_intr1_disable(unit, IRQ_CMCx_LINK_STAT_MOD);

        
        reg32_val = 0;
        soc_reg_field_set(unit, CMIC_MIIM_CLR_SCAN_STATUSr, &reg32_val, CLR_LINK_STATUS_CHANGEf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MIIM_CLR_SCAN_STATUSr(unit, reg32_val));
     }   

     

     SOC_PBMP_ASSIGN(pbm, hw_mii_pbm);
     SOC_PBMP_OR(pbm, hw_direct_pbm);
     if (SOC_PBMP_NOT_NULL(pbm)) {
        soc->soc_flags |= SOC_F_LSE;
    } else {
        soc->soc_flags &= ~SOC_F_LSE;
    }

    
    SOC_PBMP_ASSIGN(soc->hw_linkscan_pbmp, hw_mii_pbm);

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
_soc_jer_linkctrl_update(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_FUNC_RETURN;
}

STATIC int
_soc_jer_linkctrl_hw_link_get(int unit, soc_pbmp_t *hw_link)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(hw_link);

    SOC_PBMP_ASSIGN(*hw_link, PBMP_PORT_ALL(unit));

exit:
    SOCDNX_FUNC_RETURN;
    
}


CONST soc_linkctrl_driver_t  soc_linkctrl_driver_jer = {
    NULL,                                       
    _soc_jer_linkctrl_linkscan_hw_init,      
    _soc_jer_linkctrl_linkscan_config,       
    _soc_jer_linkctrl_linkscan_pause,        
    _soc_jer_linkctrl_linkscan_continue,     
    _soc_jer_linkctrl_update,                
    _soc_jer_linkctrl_hw_link_get            

};


int
soc_jer_linkcan_fault_to_cmiclink_enable_set(int unit, pbmp_t *pbm,
                                           int remote_fault_enable, int local_fault_enable)
{
    pbmp_t qsgmii;
    pbmp_t sfi;
#ifdef PORTMOD_SUPPORT
    soc_port_t port;
    uint32 fault_link;
#endif
    SOCDNX_INIT_FUNC_DEFS;

    BCM_PBMP_ASSIGN(qsgmii, PBMP_QSGMII_ALL(unit));
    BCM_PBMP_AND(qsgmii, *pbm);
    if (BCM_PBMP_NOT_NULL(qsgmii)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                             (_BSL_SOCDNX_MSG("Fault won't bring link down on QSGMII\n")));

    }
    BCM_PBMP_ASSIGN(sfi, PBMP_SFI_ALL(unit));
    BCM_PBMP_AND(sfi, *pbm);
    if (BCM_PBMP_NOT_NULL(sfi)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                             (_BSL_SOCDNX_MSG("Fault won't bring link down on SFI\n")));
    }

#ifdef PORTMOD_SUPPORT
    PBMP_ITER(*pbm, port)
    {
        int real_port = 0;
        portmod_dispatch_type_t pm_type = portmodDispatchTypeCount;
        SOCDNX_IF_ERR_EXIT(portmod_port_pm_type_get(unit, port, &real_port, &pm_type));
        switch (pm_type)
        {
            case portmodDispatchTypePm4x25:
                SOCDNX_IF_ERR_EXIT(READ_CLPORT_FAULT_LINK_STATUSr(unit, port, &fault_link));
                soc_reg_field_set(unit, CLPORT_FAULT_LINK_STATUSr, &fault_link,
                                            REMOTE_FAULTf,(remote_fault_enable != 0));
                soc_reg_field_set(unit, CLPORT_FAULT_LINK_STATUSr, &fault_link,
                                            LOCAL_FAULTf,(local_fault_enable != 0));
                SOCDNX_IF_ERR_EXIT(WRITE_CLPORT_FAULT_LINK_STATUSr(unit, port, fault_link));
                break;

            case portmodDispatchTypePm4x10:
                SOCDNX_IF_ERR_EXIT(READ_XLPORT_FAULT_LINK_STATUSr(unit, port, &fault_link));
                soc_reg_field_set(unit, XLPORT_FAULT_LINK_STATUSr, &fault_link,
                                            REMOTE_FAULTf,(remote_fault_enable != 0));
                soc_reg_field_set(unit, XLPORT_FAULT_LINK_STATUSr, &fault_link,
                                            LOCAL_FAULTf,(local_fault_enable != 0));
                SOCDNX_IF_ERR_EXIT(WRITE_XLPORT_FAULT_LINK_STATUSr(unit, port, fault_link));
                break;

            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                                     (_BSL_SOCDNX_MSG("Unsupported pm_type[%d] to config fault to cmic link.\n"),
                                      pm_type));
                break;
        }
        
        if (local_fault_enable) {
            SOCDNX_IF_ERR_EXIT(bcm_port_control_set(unit,
                                                    port,
                                                    bcmPortControlLinkFaultLocal, 0));
        }
        if (remote_fault_enable) {
            SOCDNX_IF_ERR_EXIT(bcm_port_control_set(unit,
                                                    port,
                                                    bcmPortControlLinkFaultRemote, 0));
        }

    }

exit:
#endif

    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

