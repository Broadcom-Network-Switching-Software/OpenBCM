/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <phymod/phymod.h>
#include <phymod/phymod.h>
#include <phymod/phymod_dispatch.h>

#include <phymod/chip/null.h>

#ifdef PHYMOD_NULL_SUPPORT

int null_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{

    *is_identified = 1;

    return PHYMOD_E_NONE;

}


int null_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{

    /* Place your code here */
    return phymod_core_info_t_init(info);
}


int null_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{

    /* Place your code here */
    return phymod_lane_map_t_init(lane_map);
}


int null_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{

    /* Place your code here */
    *direction = 0;

    return PHYMOD_E_NONE;

}


int null_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{

    /* Place your code here */
    return phymod_core_firmware_info_t_init(fw_info);
}


int null_phy_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_core_config)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_firmware_core_config_get(const phymod_phy_access_t* phy, phymod_firmware_core_config_t* fw_core_config)
{

    /* Place your code here */
    return phymod_firmware_core_config_t_init(fw_core_config);
}


int null_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_lane_config)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_lane_config)
{

    /* Place your code here */
    return phymod_firmware_lane_config_t_init(fw_lane_config);

}


int null_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int null_core_wait_event(const phymod_core_access_t* core, phymod_core_event_t event, uint32_t timeout)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int null_phy_rx_restart(const phymod_phy_access_t* phy)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int null_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{

    /* Place your code here */
    return phymod_polarity_t_init(polarity);

}


int null_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{

    /* Place your code here */
    return phymod_tx_t_init(tx);

}


int null_phy_media_type_tx_get(const phymod_phy_access_t* phy, phymod_media_typed_t media, phymod_tx_t* tx)
{

    /* Place your code here */
    return phymod_tx_t_init(tx);

}


int null_phy_tx_override_set(const phymod_phy_access_t* phy, const phymod_tx_override_t* tx_override)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_tx_override_get(const phymod_phy_access_t* phy, phymod_tx_override_t* tx_override)
{

    /* Place your code here */
    return phymod_tx_override_t_init(tx_override);

}


int null_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{

    /* Place your code here */
    return phymod_rx_t_init(rx);

}


int null_phy_rx_adaptation_resume(const phymod_phy_access_t* phy)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int null_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{

    /* Place your code here */
    return phymod_phy_reset_t_init(reset);

}


int null_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{

    /* Place your code here */
    return phymod_phy_power_t_init(power);

}


int null_phy_hg2_codec_control_set(const phymod_phy_access_t* phy, phymod_phy_hg2_codec_t hg2_codec)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_hg2_codec_control_get(const phymod_phy_access_t* phy, phymod_phy_hg2_codec_t* hg2_codec)
{

    /* Place your code here */
    *hg2_codec = 0;


    return PHYMOD_E_NONE;

}


int null_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{

    /* Place your code here */
    *tx_control = 0;


    return PHYMOD_E_NONE;

}


int null_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{

    /* Place your code here */
    *rx_control = 0;


    return PHYMOD_E_NONE;

}


int null_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{

    /* Place your code here */
    *enable = 0;


    return PHYMOD_E_NONE;

}


int null_phy_autoneg_oui_set(const phymod_phy_access_t* phy, phymod_autoneg_oui_t an_oui)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_autoneg_oui_get(const phymod_phy_access_t* phy, phymod_autoneg_oui_t* an_oui)
{

    /* Place your code here */
    return phymod_autoneg_oui_t_init(an_oui);

}


int null_phy_eee_set(const phymod_phy_access_t* phy, uint32_t enable)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_eee_get(const phymod_phy_access_t* phy, uint32_t* enable)
{

    /* Place your code here */
    *enable = 0;


    return PHYMOD_E_NONE;

}


int null_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_ref_clk_t ref_clock, phymod_phy_inf_config_t* config)
{

    /* Place your code here */
    return phymod_phy_inf_config_t_init(config);

}


int null_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{

    /* Place your code here */
    *cl72_en = 0;


    return PHYMOD_E_NONE;

}


int null_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{

    /* Place your code here */
    return phymod_cl72_status_t_init(status);

}


int null_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability_set_type)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{

    /* Place your code here */
    return phymod_autoneg_ability_t_init(an_ability_get_type);

}


int null_phy_autoneg_remote_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{

    /* Place your code here */
    return phymod_autoneg_ability_t_init(an_ability_get_type);

}


int null_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{

    /* Place your code here */
    *an_done = 1;
    return phymod_autoneg_control_t_init(an);
}


int null_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{

    /* Place your code here */
    return phymod_autoneg_status_t_init(status);

}


int null_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int null_phy_pll_multiplier_get(const phymod_phy_access_t* phy, uint32_t* core_vco_pll_multiplier)
{

    /* Place your code here */
    *core_vco_pll_multiplier = 0;


    return PHYMOD_E_NONE;

}


int null_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int null_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{

    /* Place your code here */
    *enable = 0;


    return PHYMOD_E_NONE;

}


int null_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_pmd_locked)
{

    /* Place your code here */
    *rx_pmd_locked = 1;


    return PHYMOD_E_NONE;

}


int null_phy_rx_signal_detect_get(const phymod_phy_access_t* phy, uint32_t* rx_signal_detect)
{

    /* Place your code here */
    *rx_signal_detect = 1;

    return PHYMOD_E_NONE;

}


int null_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{

    /* Place your code here */
    *link_status = 1;


    return PHYMOD_E_NONE;

}


int null_phy_pcs_userspeed_set(const phymod_phy_access_t* phy, const phymod_pcs_userspeed_config_t* config)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_pcs_userspeed_get(const phymod_phy_access_t* phy, phymod_pcs_userspeed_config_t* config)
{

    /* Place your code here */
    return phymod_pcs_userspeed_config_t_init(config);

}


int null_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int null_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int null_phy_eye_margin_est_get(const phymod_phy_access_t* phy, phymod_eye_margin_mode_t eye_margin_mode, uint32_t* value)
{

    /* Place your code here */
    *value = 0;


    return PHYMOD_E_NONE;

}


int null_phy_sw_autoneg_enable(const phymod_phy_access_t* phy, int enable)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_speed_config_set(const phymod_phy_access_t* phy,
                              const phymod_phy_speed_config_t* speed_config,
                              const phymod_phy_pll_state_t* old_pll_state,
                              phymod_phy_pll_state_t* new_pll_state)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_speed_config_get(const phymod_phy_access_t* phy, phymod_phy_speed_config_t* speed_config)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_tx_taps_default_get(const phymod_phy_access_t* phy, phymod_phy_signalling_method_t mode, phymod_tx_t* tx)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_lane_config_default_get(const phymod_phy_access_t* phy, phymod_phy_signalling_method_t mode, phymod_firmware_lane_config_t* lane_config)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_firmware_load_info_get(const phymod_phy_access_t* phy, phymod_firmware_load_info_t* info)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_autoneg_advert_ability_set(const phymod_phy_access_t* phy,
                                         const phymod_autoneg_advert_abilities_t* an_advert_abilities,
                                         const phymod_phy_pll_state_t* old_pll_adv_state,
                                         phymod_phy_pll_state_t* new_pll_adv_state)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_autoneg_advert_ability_get(const phymod_phy_access_t* phy,
                                         phymod_autoneg_advert_abilities_t* an_advert_abilities)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_autoneg_remote_advert_ability_get(const phymod_phy_access_t* phy,
                                                phymod_autoneg_advert_abilities_t* an_advert_abilities)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_plls_reconfig(const phymod_phy_access_t* phy, uint32_t num_plls, const uint32_t* plls)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_tx_pam4_precoder_enable_set(const phymod_phy_access_t* phy, int enable)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_tx_pam4_precoder_enable_get(const phymod_phy_access_t* phy, int* enable)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

#endif /* PHYMOD_NULL_SUPPORT */
