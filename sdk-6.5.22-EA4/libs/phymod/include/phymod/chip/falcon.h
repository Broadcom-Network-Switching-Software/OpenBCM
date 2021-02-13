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

#ifndef _FALCON_H__H_
#define _FALCON_H__H_

#include <phymod/phymod_definitions.h>
/*Initialize phymod module*/
int falcon_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified);

/*Retrive core information*/
int falcon_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info);

/*Set\get lane mapping*/
int falcon_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map);
int falcon_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map);

/*Reset Core*/
int falcon_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction);
int falcon_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction);

/*Retrive firmware information*/
int falcon_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info);

/*Set\get firmware operation mode*/
int falcon_phy_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_core_config);
int falcon_phy_firmware_core_config_get(const phymod_phy_access_t* phy, phymod_firmware_core_config_t* fw_core_config);

/*Set\get firmware operation mode*/
int falcon_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_lane_config);
int falcon_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_lane_config);

/*Start\Stop the sequencer*/
int falcon_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation);

/*Wait for core event*/
int falcon_core_wait_event(const phymod_core_access_t* core, phymod_core_event_t event, uint32_t timeout);

/* re-tune rx path*/
int falcon_phy_rx_restart(const phymod_phy_access_t* phy);

/*Set phy polarity*/
int falcon_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity);
int falcon_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity);

/*Set\Get TX Parameters*/
int falcon_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx);
int falcon_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx);

/*Request for default TX parameters configuration per media type*/
int falcon_phy_media_type_tx_get(const phymod_phy_access_t* phy, phymod_media_typed_t media, phymod_tx_t* tx);

/*Set\Get TX override Parameters*/
int falcon_phy_tx_override_set(const phymod_phy_access_t* phy, const phymod_tx_override_t* tx_override);
int falcon_phy_tx_override_get(const phymod_phy_access_t* phy, phymod_tx_override_t* tx_override);

/*Set\Get RX Parameters*/
int falcon_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx);
int falcon_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx);

/*PHY Rx adaptation resume*/
int falcon_phy_rx_adaptation_resume(const phymod_phy_access_t* phy);

/*Reset phy*/
int falcon_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset);
int falcon_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset);

/*Control phy power*/
int falcon_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power);
int falcon_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power);

/*TX transmission control*/
int falcon_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control);
int falcon_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control);

/*Rx control*/
int falcon_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control);
int falcon_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control);

/*Set the interface mode, speed and other configuration related to interface.*/
int falcon_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config);
int falcon_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_ref_clk_t ref_clock, phymod_phy_inf_config_t* config);

/*Set\Get CL72*/
int falcon_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en);
int falcon_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en);

/*Get CL72 status*/
int falcon_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status);

/*Set\Get autoneg*/
int falcon_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability_set_type);
int falcon_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type);

/*Set\Get autoneg*/
int falcon_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an);
int falcon_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done);

/*Get Autoneg status*/
int falcon_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status);

/*Core Initialization*/
int falcon_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status);

/*Phy Initialization*/
int falcon_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config);

/*Set\get loopback mode*/
int falcon_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable);
int falcon_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable);

/*Get rx pmd locked indication*/
int falcon_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_pmd_locked);

/*Get rx pmd locked indication*/
int falcon_phy_rx_signal_detect_get(const phymod_phy_access_t* phy, uint32_t* rx_signal_detect);

/*Get link up status indication*/
int falcon_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status);

/*Read phymod register*/
int falcon_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val);

/*Write phymod register*/
int falcon_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val);

/*eye margin estimate get*/
int falcon_phy_eye_margin_est_get(const phymod_phy_access_t* phy, phymod_eye_margin_mode_t eye_margin_mode, uint32_t* value);

#endif /*_FALCON_H_*/
