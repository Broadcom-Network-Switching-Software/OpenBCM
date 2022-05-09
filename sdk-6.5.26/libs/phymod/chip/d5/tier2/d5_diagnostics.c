/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>
#include <phymod/phymod_util.h>

#ifdef PHYMOD_D5_SUPPORT

int d5_phy_rx_slicer_position_set(const phymod_phy_access_t* phy,
                                  uint32_t flags,
                                  const phymod_slicer_position_t* position)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int d5_phy_rx_slicer_position_get(const phymod_phy_access_t* phy,
                                  uint32_t flags,
                                  phymod_slicer_position_t* position)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int d5_phy_rx_slicer_position_max_get(const phymod_phy_access_t* phy,
                                uint32_t flags,
                                const phymod_slicer_position_t* position_min,
                                const phymod_slicer_position_t* position_max)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int d5_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags,
                           const phymod_prbs_t* prbs)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int d5_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags,
                           phymod_prbs_t* prbs)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int d5_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags,
                           uint32_t enable)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int d5_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags,
                           uint32_t* enable)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int d5_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags,
                           phymod_prbs_status_t* prbs_status)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int d5_phy_pattern_config_set(const phymod_phy_access_t* phy,
                              const phymod_pattern_t* pattern)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int d5_phy_pattern_config_get(const phymod_phy_access_t* phy,
                              phymod_pattern_t* pattern)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int d5_phy_pattern_enable_set(const phymod_phy_access_t* phy, uint32_t enable,
                              const phymod_pattern_t* pattern)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int d5_phy_pattern_enable_get(const phymod_phy_access_t* phy,
                              uint32_t* enable)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int d5_phy_diagnostics_get(const phymod_phy_access_t* phy,
                           phymod_phy_diagnostics_t* diag)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int d5_phy_pmd_info_dump(const phymod_phy_access_t* phy, const char* type)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int d5_diagnostics_eye_margin_proj(const phymod_phy_access_t* phy,
                            uint32_t flags,
                            const phymod_phy_eyescan_options_t* eyescan_options)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int d5_phy_eyescan_run(const phymod_phy_access_t* phy, uint32_t flags,
                       phymod_eyescan_mode_t mode,
                       const phymod_phy_eyescan_options_t* eyescan_options)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

#endif /* PHYMOD_D5_SUPPORT */
