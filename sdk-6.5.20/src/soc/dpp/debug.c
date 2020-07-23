/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_DIAG
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/debug.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/ARAD/arad_debug.h>

soc_error_t soc_dpp_dbg_egress_shaping_enable_get(const unsigned int unit, uint8 *enable)
{
  int rv = SOC_E_NONE;
    rv = arad_dbg_egress_shaping_enable_get_unsafe(unit, enable);
    SOCDNX_SAND_IF_ERR_RETURN(rv);
    return SOC_E_NONE;
}

soc_error_t soc_dpp_dbg_egress_shaping_enable_set(const unsigned int unit, const uint8 enable)
{
  int rv = SOC_E_NONE;
  rv = arad_dbg_egress_shaping_enable_set_unsafe(unit, enable);
  SOCDNX_SAND_IF_ERR_RETURN(rv);
  return SOC_E_NONE;
}

soc_error_t soc_dpp_dbg_flow_control_enable_get(const unsigned int unit, uint8 *enable)
{
  int rv = SOC_E_NONE;
  rv = arad_dbg_flow_control_enable_get_unsafe(unit, enable);
  SOCDNX_SAND_IF_ERR_RETURN(rv);
  return SOC_E_NONE;
}

soc_error_t soc_dpp_dbg_flow_control_enable_set(const unsigned int unit, const uint8 enable)
{
  int rv = SOC_E_NONE;
    rv = arad_dbg_flow_control_enable_set_unsafe(unit, enable);
    SOCDNX_SAND_IF_ERR_RETURN(rv);
    return SOC_E_NONE;
}

soc_error_t soc_dpp_compilation_vendor_valid(const unsigned int unit, const unsigned int val)
{

    return SOC_E_UNAVAIL;
}

#undef _ERR_MSG_MODULE_NAME

