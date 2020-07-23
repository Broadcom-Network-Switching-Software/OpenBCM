/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    arad_trunk.c
 * Purpose: SOC level functinos for Link Aggregation (a.k.a Trunking)
 */


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_TRUNK

#include <soc/drv.h>
#include <soc/register.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/ARAD/arad_trunk.h>


int soc_arad_trunk_resolve_ingress_mc_destination_method(int unit, int8 use_trunk_as_ingress_mc_dest)
{
    int value;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_ARADPLUS(unit) && !SOC_IS_ARDON(unit))
    {
        
        value = (use_trunk_as_ingress_mc_dest == 1) ? 0 : 1;
        SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, REG_PORT_ANY, CFG_BUG_FIX_86_DISABLEf, value));
    } else if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        
        value = (use_trunk_as_ingress_mc_dest == 1) ? 0 : 1;
        SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, EGQ_LAG_FILTER_ENABLEr, REG_PORT_ANY, ENABLE_LAG_FILTER_MCf, value));
    }
exit:
    SOCDNX_FUNC_RETURN;
}



#define _ERR_MSG_MODULE_NAME BSL_SOC_TRUNK
