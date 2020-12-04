 /* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ramon_link.c
 * Purpose:     Hardware Linkscan module
 *
 * These routines will be called by the linkscan module,
 * so they need to be defined and return _SHR_E_NONE.
 */
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_PORT


#include <shared/bsl.h>
#include <soc/linkctrl.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/cmicm.h>
#include <soc/mcm/memregs.h>
#include <soc/mcm/cmicm.h>

#include <soc/dnxc/error.h>

#include <soc/dnxf/ramon/ramon_link.h>
#include <shared/cmicfw/cmicx_link.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_linkscan.h>



STATIC int
_soc_ramon_linkctrl_linkscan_hw_init(int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_cmicx_linkscan_hw_init(unit));

exit:
    SHR_FUNC_EXIT;
}

STATIC int
_soc_ramon_linkscan_ports_write(int unit, pbmp_t hw_mii_pbm)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_cmicx_linkscan_config(unit, &hw_mii_pbm));

exit:
    SHR_FUNC_EXIT;  
}


STATIC int
_soc_ramon_linkctrl_linkscan_pause(int unit)
{
    uint32 m0_pause_enable;
    SHR_FUNC_INIT_VARS(unit);

    m0_pause_enable = dnxf_data_linkscan.general.m0_pause_enable_get(unit);
    if (!m0_pause_enable)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(soc_cmicx_linkscan_pause(unit));

exit:
    SHR_FUNC_EXIT;
}


STATIC int
_soc_ramon_linkctrl_linkscan_continue(int unit)
{
    uint32 m0_pause_enable;
    SHR_FUNC_INIT_VARS(unit);

    m0_pause_enable = dnxf_data_linkscan.general.m0_pause_enable_get(unit);
   if (!m0_pause_enable)
    {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(soc_cmicx_linkscan_continue(unit));

exit:
    SHR_FUNC_EXIT;
}


STATIC int
_soc_ramon_linkctrl_linkscan_config(int unit, pbmp_t hw_mii_pbm,
                                    pbmp_t hw_direct_pbm)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    pbmp_t pbm;

    SHR_FUNC_INIT_VARS(unit);

    

    SOC_PBMP_ASSIGN(pbm, hw_mii_pbm);
    SOC_PBMP_OR(pbm, hw_direct_pbm);
    if (SOC_PBMP_NOT_NULL(pbm)) {
       
       soc->soc_flags |= SOC_F_LSE;
    } else {
        soc->soc_flags &= ~SOC_F_LSE;
    }

    SHR_IF_ERR_EXIT(_soc_ramon_linkscan_ports_write(unit, hw_mii_pbm));

    SOC_PBMP_ASSIGN(soc->hw_linkscan_pbmp, hw_mii_pbm);

exit:
    SHR_FUNC_EXIT;
}


STATIC int
_soc_ramon_linkctrl_update(int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

STATIC int
_soc_ramon_linkctrl_hw_link_get(int unit, soc_pbmp_t *hw_link)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(hw_link, _SHR_E_PARAM, "hw_link");
    
    SOC_PBMP_CLEAR(*hw_link);
    SHR_IF_ERR_EXIT(soc_cmicx_linkscan_hw_link_cache_get(unit, hw_link));

exit:
    SHR_FUNC_EXIT;
}


CONST soc_linkctrl_driver_t  soc_linkctrl_driver_ramon = {
    NULL,                                       
    _soc_ramon_linkctrl_linkscan_hw_init,      
    _soc_ramon_linkctrl_linkscan_config,       
    _soc_ramon_linkctrl_linkscan_pause,        
    _soc_ramon_linkctrl_linkscan_continue,     
    _soc_ramon_linkctrl_update,                
    _soc_ramon_linkctrl_hw_link_get            

};

 
shr_error_e
soc_ramon_linkctrl_init(int unit) 
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_linkctrl_init(unit, &soc_linkctrl_driver_ramon));

exit:
    SHR_FUNC_EXIT;
}




#undef BSL_LOG_MODULE
