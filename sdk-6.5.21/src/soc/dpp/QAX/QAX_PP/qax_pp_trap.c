/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: qax_pp_trap.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_TRAP




#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/mcm/memregs.h>
#include <soc/mcm/memacc.h>
#include <soc/mem.h>

#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/mbcm_pp.h>

#include <soc/dpp/PPC/ppc_api_general.h>

#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPC/ppc_api_trap_mgmt.h>
#include <soc/dpp/PPC/ppc_api_eg_mirror.h>

#include <soc/dpp/QAX/QAX_PP/qax_pp_trap.h>





























STATIC soc_error_t _soc_qax_pp_trap_ser_trap_code_set(int unit,int trap_hw_id)
{
   uint32 regData = 0;
   int rv;
   SOCDNX_INIT_FUNC_DEFS;


    
    rv = soc_reg32_get(unit,IHB_ACTION_PROFILE_PACKET_HAS_SERr,REG_PORT_ANY,0,&regData);
    SOCDNX_IF_ERR_EXIT(rv);
    
    
    soc_reg_field_set(unit,IHB_ACTION_PROFILE_PACKET_HAS_SERr,&regData,
                        ACTION_PROFILE_PACKET_HAS_SER_CPU_TRAP_CODEf,trap_hw_id);
    
    
    rv =soc_reg32_set(unit,IHB_ACTION_PROFILE_PACKET_HAS_SERr,REG_PORT_ANY,0,regData);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_EXIT;; 
exit:
    SOCDNX_FUNC_RETURN;
}


STATIC soc_error_t _soc_qax_pp_trap_ser_set_unset(int unit,int enable)
{
   uint32 regData = 0;
   int rv;
   SOCDNX_INIT_FUNC_DEFS;

   
    rv = soc_reg32_get(unit,IHB_FER_GENERAL_CONFIGURATIONSr,REG_PORT_ANY,0,&regData);
    SOCDNX_IF_ERR_EXIT(rv);
    
    
    soc_reg_field_set(unit,IHB_FER_GENERAL_CONFIGURATIONSr,&regData,
                        ENABLE_TRAP_SER_PACKETf,enable);
    
    
    rv = soc_reg32_set(unit,IHB_FER_GENERAL_CONFIGURATIONSr,REG_PORT_ANY,0,regData);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_EXIT;
exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t soc_qax_pp_trap_ingress_ser_set(int unit,int enable,int trap_hw_id)
{
    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    rv = _soc_qax_pp_trap_ser_set_unset(unit,enable);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = _soc_qax_pp_trap_ser_trap_code_set(unit,trap_hw_id);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}




#include <soc/dpp/SAND/Utils/sand_footer.h>

