/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *         
 *     
 * DO NOT EDIT THIS FILE!
 */

#ifndef _TSCBH_H__H_
#define _TSCBH_H__H_

#include <phymod/phymod_definitions.h>
/*Initialize phymod module*/
int tscbh_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified);

/*Retrive core information*/
int tscbh_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info);

/*Set\get lane mapping*/
int tscbh_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map);
int tscbh_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map);

/*Reset Core*/
int tscbh_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction);
int tscbh_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction);

/*Set\get firmware operation mode*/
int tscbh_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_lane_config);
int tscbh_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_lane_config);

/* re-tune rx path*/
int tscbh_phy_rx_restart(const phymod_phy_access_t* phy);

/*Set phy polarity*/
int tscbh_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity);
int tscbh_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity);

/*Set\Get TX Parameters*/
int tscbh_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx);
int tscbh_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx);

/*Request for default TX parameters configuration per media type*/
int tscbh_phy_media_type_tx_get(const phymod_phy_access_t* phy, phymod_media_typed_t media, phymod_tx_t* tx);

/*Reset phy*/
int tscbh_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset);
int tscbh_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset);

/*Control phy power*/
int tscbh_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power);
int tscbh_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power);

/*TX transmission control*/
int tscbh_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control);
int tscbh_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control);

/*Rx control*/
int tscbh_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control);
int tscbh_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control);

/*Autoneg FEC override control for 100G*/
int tscbh_phy_fec_override_set(const phymod_phy_access_t* phy, uint32_t enable);
int tscbh_phy_fec_override_get(const phymod_phy_access_t* phy, uint32_t* enable);

/*Set\Get CL72*/
int tscbh_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en);
int tscbh_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en);

/*Get CL72 status*/
int tscbh_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status);

/*Set\Get autoneg*/
int tscbh_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an);
int tscbh_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done);

/*Get Autoneg status*/
int tscbh_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status);

/*Core Initialization*/
int tscbh_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status);

/*Core vco freq get function*/
int tscbh_phy_pll_multiplier_get(const phymod_phy_access_t* phy, uint32_t* core_vco_pll_multiplier);

/*Phy Initialization*/
int tscbh_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config);

/*Set\get loopback mode*/
int tscbh_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable);
int tscbh_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable);

/*Get rx pmd locked indication*/
int tscbh_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_pmd_locked);

/*Get information required to determine local fault reasons*/
int tscbh_phy_local_fault_info_get(const phymod_phy_access_t* phy, phymod_phy_local_fault_info_t* local_fault_info);

/*Get rx pmd locked indication*/
int tscbh_phy_rx_signal_detect_get(const phymod_phy_access_t* phy, uint32_t* rx_signal_detect);

/*Get link up status indication*/
int tscbh_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status);

/*Read phymod register*/
int tscbh_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val);

/*Write phymod register*/
int tscbh_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val);

/*Set/Get timesync enable*/
int tscbh_timesync_enable_set(const phymod_phy_access_t* phy, uint32_t flags, uint32_t enable);
int tscbh_timesync_enable_get(const phymod_phy_access_t* phy, uint32_t flags, uint32_t* enable);

/*Set/Get timesync ts_offset*/
int tscbh_timesync_offset_set(const phymod_core_access_t* core, uint32_t ts_offset);
int tscbh_timesync_offset_get(const phymod_core_access_t* core, uint32_t* ts_offset);

/*Set timesync adjust*/
int tscbh_timesync_adjust_set(const phymod_phy_access_t* phy, uint32_t flags, phymod_timesync_compensation_mode_t ts_am_norm_mode);

/*Set the interface mode, speed and other configuration related to interface.*/
int tscbh_phy_speed_config_set(const phymod_phy_access_t* phy, const phymod_phy_speed_config_t* speed_config, const phymod_phy_pll_state_t* old_pll_state, phymod_phy_pll_state_t* new_pll_state);
int tscbh_phy_speed_config_get(const phymod_phy_access_t* phy, phymod_phy_speed_config_t* speed_config);

/* get default tx taps*/
int tscbh_phy_tx_taps_default_get(const phymod_phy_access_t* phy, phymod_phy_signalling_method_t mode, phymod_tx_t* tx);

/* get default tx taps*/
int tscbh_phy_lane_config_default_get(const phymod_phy_access_t* phy, phymod_phy_signalling_method_t mode, phymod_firmware_lane_config_t* lane_config);

/* get PMD micro code and length*/
int tscbh_phy_firmware_load_info_get(const phymod_phy_access_t* phy, phymod_firmware_load_info_t* info);

/* Set/Get autoneg advertising abilities*/
int tscbh_phy_autoneg_advert_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_advert_abilities_t* an_advert_abilities, const phymod_phy_pll_state_t* old_pll_adv_state, phymod_phy_pll_state_t* new_pll_adv_state);
int tscbh_phy_autoneg_advert_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_advert_abilities_t* an_advert_abilities);

/* Get remote autoneg advertising abilities*/
int tscbh_phy_autoneg_remote_advert_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_advert_abilities_t* an_advert_abilities);

/* enable tx pam4 precoder for extended reach channel*/
int tscbh_phy_tx_pam4_precoder_enable_set(const phymod_phy_access_t* phy, int enable);
int tscbh_phy_tx_pam4_precoder_enable_get(const phymod_phy_access_t* phy, int* enable);

/* reconfig pll on a specific serdes core*/
int tscbh_phy_pll_reconfig(const phymod_phy_access_t* phy, uint8_t pll_index, uint32_t pll_div, phymod_ref_clk_t ref_clk);

/* Set/Get synce clock config*/
int tscbh_phy_synce_clk_ctrl_set(const phymod_phy_access_t* phy, phymod_synce_clk_ctrl_t cfg);
int tscbh_phy_synce_clk_ctrl_get(const phymod_phy_access_t* phy, phymod_synce_clk_ctrl_t* cfg);

/* Enable/Disable PCS for the port*/
int tscbh_phy_pcs_enable_set(const phymod_phy_access_t* phy, uint32_t enable);

/*Get rx ppm*/
int tscbh_phy_rx_ppm_get(const phymod_phy_access_t* phy, int16_t* rx_ppm);

/*Serdes interrupt handler.*/
int tscbh_intr_handler(const phymod_phy_access_t* phy, phymod_interrupt_type_t type, uint32_t* is_handled);

/*Get timestamp tx info*/
int tscbh_timesync_tx_info_get(const phymod_phy_access_t* phy, phymod_ts_fifo_status_t* ts_tx_info);

/* pcs lane swap for RLM purpose*/
int tscbh_phy_pcs_lane_swap_adjust(const phymod_phy_access_t* phy, uint32_t active_lane_map, uint32_t original_tx_lane_map, uint32_t original_rx_lane_map);

/* speed id entry load for RLM only */
int tscbh_phy_load_speed_id_entry(const phymod_phy_access_t* phy, uint32_t speed, uint32_t num_lane, phymod_fec_type_t fec_type);

/* pmd_override_enable_set */
int tscbh_phy_pmd_override_enable_set(const phymod_phy_access_t* phy, phymod_override_type_t pmd_override_type, uint32_t override_enable, uint32_t override_val);

/*Get PLL power down*/
int tscbh_phy_pll_powerdown_get(const phymod_phy_access_t* phy, uint32_t pll_index, uint32_t* powerdown);

/*Set or get fec bypass indication. Valid for RS-FEC.*/
int tscbh_phy_fec_bypass_indication_set(const phymod_phy_access_t* phy, uint32_t enable);
int tscbh_phy_fec_bypass_indication_get(const phymod_phy_access_t* phy, uint32_t* enable);

/*Set or get codec mode */
int tscbh_phy_codec_mode_set(const phymod_phy_access_t* phy, phymod_phy_codec_mode_t codec_type);
int tscbh_phy_codec_mode_get(const phymod_phy_access_t* phy, phymod_phy_codec_mode_t* codec_type);

/*Get RS FEC hi_ser_LH and hi_ser_live status */
int tscbh_phy_rs_fec_rxp_get(const phymod_phy_access_t* phy, uint32_t* hi_ser_lh, uint32_t* hi_ser_live);

/* get the ref clk value */
int tscbh_phy_interrupt_enable_set(const phymod_phy_access_t* phy, phymod_interrupt_type_t intr_type, uint32_t enable);
int tscbh_phy_interrupt_enable_get(const phymod_phy_access_t* phy, phymod_interrupt_type_t intr_type, uint32_t* enable);

/* Get the fec three bad state */
int tscbh_phy_fec_three_cw_bad_state_get(const phymod_phy_access_t* phy, uint32_t* state);

#endif /*_TSCBH_H_*/
