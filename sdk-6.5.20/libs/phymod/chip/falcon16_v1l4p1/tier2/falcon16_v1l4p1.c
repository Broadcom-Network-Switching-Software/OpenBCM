/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod.h>
#include <phymod/phymod_dispatch.h>

#ifdef PHYMOD_FALCON16_V1L4P1_SUPPORT

int falcon16_v1l4p1_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int falcon16_v1l4p1_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int falcon16_v1l4p1_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_core_config)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int falcon16_v1l4p1_phy_firmware_core_config_get(const phymod_phy_access_t* phy, phymod_firmware_core_config_t* fw_core_config)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_lane_config)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int falcon16_v1l4p1_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_lane_config)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_core_wait_event(const phymod_core_access_t* core, phymod_core_event_t event, uint32_t timeout)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_rx_restart(const phymod_phy_access_t* phy)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int falcon16_v1l4p1_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int falcon16_v1l4p1_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_media_type_tx_get(const phymod_phy_access_t* phy, phymod_media_typed_t media, phymod_tx_t* tx)
{
    switch (media) {
    case phymodMediaTypeChipToChip:
      tx->pre   = 0x0;
      tx->main  = 0x64;
      tx->post  = 0x0;
      tx->post2 = 0x0;
      tx->post3 = 0x0;
      tx->amp   = 0xc;
      break;
    case phymodMediaTypeShort:
      tx->pre   = 0x0;
      tx->main  = 0x64;
      tx->post  = 0x0;
      tx->post2 = 0x0;
      tx->post3 = 0x0;
      tx->amp   = 0xc;
      break;
    case phymodMediaTypeMid:
      tx->pre   = 0x0;
      tx->main  = 0x64;
      tx->post  = 0x0;
      tx->post2 = 0x0;
      tx->post3 = 0x0;
      tx->amp   = 0xc;
      break;
    case phymodMediaTypeLong:
      tx->pre   = 0x0;
      tx->main  = 0x64;
      tx->post  = 0x0;
      tx->post2 = 0x0;
      tx->post3 = 0x0;
      tx->amp   = 0xc;
      break;
    default:
      tx->pre   = 0x0;
      tx->main  = 0x64;
      tx->post  = 0x0;
      tx->post2 = 0x0;
      tx->post3 = 0x0;
      tx->amp   = 0xc;
      break;
    }

    return PHYMOD_E_NONE;
}

int falcon16_v1l4p1_phy_tx_override_set(const phymod_phy_access_t* phy, const phymod_tx_override_t* tx_override)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int falcon16_v1l4p1_phy_tx_override_get(const phymod_phy_access_t* phy, phymod_tx_override_t* tx_override)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int falcon16_v1l4p1_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int falcon16_v1l4p1_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int falcon16_v1l4p1_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int falcon16_v1l4p1_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int falcon16_v1l4p1_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int falcon16_v1l4p1_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_ref_clk_t ref_clock, phymod_phy_inf_config_t* config)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int falcon16_v1l4p1_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability_set_type)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int falcon16_v1l4p1_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int falcon16_v1l4p1_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int falcon16_v1l4p1_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_pmd_locked)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_rx_signal_detect_get(const phymod_phy_access_t* phy, uint32_t* rx_signal_detect)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


#endif /* PHYMOD_FALCON16_V1L4P1_SUPPORT */
