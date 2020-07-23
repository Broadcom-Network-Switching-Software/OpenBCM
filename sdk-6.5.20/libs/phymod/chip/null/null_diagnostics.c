/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <phymod/phymod.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>

#include <phymod/chip/null_diagnostics.h>

#ifdef PHYMOD_NULL_SUPPORT

int null_phy_PAM4_tx_pattern_enable_set(const phymod_phy_access_t* phy, phymod_PAM4_tx_pattern_t pattern_type, uint32_t enable)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_PAM4_tx_pattern_enable_get(const phymod_phy_access_t* phy, phymod_PAM4_tx_pattern_t pattern_type, uint32_t *enable)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

#endif /* PHYMOD_NULL_SUPPORT */
