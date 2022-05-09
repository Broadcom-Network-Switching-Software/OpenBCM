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
#include <phymod/phymod.h>
#include <phymod/phymod_dispatch.h>

#ifdef PHYMOD_D5_SUPPORT

int d5_core_identify(const phymod_core_access_t* core, uint32_t core_id,
                     uint32_t* is_identified)
{

    return PHYMOD_E_NONE;
}


int d5_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{

    return PHYMOD_E_NONE;
}


int d5_core_reset_set(const phymod_core_access_t* core,
                      phymod_reset_mode_t reset_mode,
                      phymod_reset_direction_t direction)
{

    return PHYMOD_E_NONE;
}

int d5_core_reset_get(const phymod_core_access_t* core,
                      phymod_reset_mode_t reset_mode,
                      phymod_reset_direction_t* direction)
{

    return PHYMOD_E_NONE;
}

int d5_core_pll_sequencer_restart(const phymod_core_access_t* core,
                                  uint32_t flags,
                                  phymod_sequencer_operation_t operation)
{

    return PHYMOD_E_NONE;
}

int d5_phy_rx_restart(const phymod_phy_access_t* phy)
{

    return PHYMOD_E_NONE;
}

int d5_phy_polarity_set(const phymod_phy_access_t* phy,
                        const phymod_polarity_t* polarity)
{

    return PHYMOD_E_NONE;
}

int d5_phy_polarity_get(const phymod_phy_access_t* phy,
                        phymod_polarity_t* polarity)
{

    return PHYMOD_E_NONE;
}

int d5_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{

    return PHYMOD_E_NONE;
}

int d5_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{

    return PHYMOD_E_NONE;
}

int d5_phy_tx_override_set(const phymod_phy_access_t* phy,
                           const phymod_tx_override_t* tx_override)
{

    return PHYMOD_E_NONE;
}

int d5_phy_tx_override_get(const phymod_phy_access_t* phy,
                           phymod_tx_override_t* tx_override)
{

    return PHYMOD_E_NONE;
}


int d5_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{

    return PHYMOD_E_NONE;
}

int d5_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{

    return PHYMOD_E_NONE;
}


int d5_phy_reset_set(const phymod_phy_access_t* phy,
                     const phymod_phy_reset_t* reset)
{

    return PHYMOD_E_NONE;
}

int d5_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{

    return PHYMOD_E_NONE;
}


int d5_phy_power_set(const phymod_phy_access_t* phy,
                     const phymod_phy_power_t* power)
{

    return PHYMOD_E_NONE;
}

int d5_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{

    return PHYMOD_E_NONE;
}


int d5_phy_tx_lane_control_set(const phymod_phy_access_t* phy,
                               phymod_phy_tx_lane_control_t tx_control)
{

    return PHYMOD_E_NONE;
}

int d5_phy_tx_lane_control_get(const phymod_phy_access_t* phy,
                               phymod_phy_tx_lane_control_t* tx_control)
{

    return PHYMOD_E_NONE;
}


int d5_phy_rx_lane_control_set(const phymod_phy_access_t* phy,
                               phymod_phy_rx_lane_control_t rx_control)
{

    return PHYMOD_E_NONE;
}

int d5_phy_rx_lane_control_get(const phymod_phy_access_t* phy,
                               phymod_phy_rx_lane_control_t* rx_control)
{

    return PHYMOD_E_NONE;
}


int d5_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags,
                                const phymod_phy_inf_config_t* config)
{

    return PHYMOD_E_NONE;
}

int d5_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags,
                                phymod_ref_clk_t ref_clock,
                                phymod_phy_inf_config_t* config)
{

    return PHYMOD_E_NONE;
}

int d5_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{

    return PHYMOD_E_NONE;
}

int d5_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{

    return PHYMOD_E_NONE;
}

int d5_phy_cl72_status_get(const phymod_phy_access_t* phy,
                           phymod_cl72_status_t* status)
{

    return PHYMOD_E_NONE;
}

int d5_core_init(const phymod_core_access_t* core,
                 const phymod_core_init_config_t* init_config,
                 const phymod_core_status_t* core_status)
{

    return PHYMOD_E_NONE;
}

int d5_phy_init(const phymod_phy_access_t* phy,
                const phymod_phy_init_config_t* init_config)
{

    return PHYMOD_E_NONE;
}

int d5_phy_loopback_set(const phymod_phy_access_t* phy,
                        phymod_loopback_mode_t loopback,
                        uint32_t enable)
{

    return PHYMOD_E_NONE;
}

int d5_phy_loopback_get(const phymod_phy_access_t* phy,
                        phymod_loopback_mode_t loopback,
                        uint32_t* enable)
{

    return PHYMOD_E_NONE;
}

int d5_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy,
                             uint32_t* rx_pmd_locked)
{

    return PHYMOD_E_NONE;
}

int d5_phy_rx_signal_detect_get(const phymod_phy_access_t* phy,
                                uint32_t* rx_signal_detect)
{

    return PHYMOD_E_NONE;
}

int d5_phy_reg_read(const phymod_phy_access_t* phy,
                    uint32_t reg_addr, uint32_t* val)
{

    return PHYMOD_E_NONE;
}

int d5_phy_reg_write(const phymod_phy_access_t* phy,
                     uint32_t reg_addr, uint32_t val)
{

    return PHYMOD_E_NONE;
}

#endif /* PHYMOD_D5_SUPPORT */
