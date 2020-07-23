/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_PORT
#include <shared/bsl.h>

#include <shared/shrextend/shrextend_error.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/portmod/portmod.h>

#include "phy_power_up.h"


shr_error_e
dnx_phy_power_up(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!soc_is(unit, J2C_DEVICE))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Heat Up is not supported for this device.\n");
    }

#ifdef PHYMOD_TSCBH_SUPPORT
     
    SHR_IF_ERR_EXIT(dnx_phy_nif_bh_power_up(unit));
#endif

#ifdef PHYMOD_BLACKHAWK_SUPPORT
      
    SHR_IF_ERR_EXIT(dnx_phy_fabric_bh_power_up(unit));
#endif

#ifdef PHYMOD_TSCF16_GEN3_SUPPORT
     
    SHR_IF_ERR_EXIT(dnx_phy_nif_falcon_power_up(unit));
#endif

#if !defined(PHYMOD_TSCBH_SUPPORT) && !defined(PHYMOD_BLACKHAWK_SUPPORT) && !defined(PHYMOD_TSCF16_GEN3_SUPPORT)
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Heat Up is not supported for this device.\n");
#endif

exit:
    SHR_FUNC_EXIT;

}
