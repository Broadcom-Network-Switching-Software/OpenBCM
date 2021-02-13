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

#ifndef _FALCON_DPLL_H__H_
#define _FALCON_DPLL_H__H_

#include <phymod/phymod_definitions.h>
/*Set\get firmware operation mode*/
int falcon_dpll_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_lane_config);
int falcon_dpll_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_lane_config);

/*Set\Get TX Parameters*/
int falcon_dpll_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx);
int falcon_dpll_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx);

/*Request for default TX parameters configuration per media type*/
int falcon_dpll_phy_media_type_tx_get(const phymod_phy_access_t* phy, phymod_media_typed_t media, phymod_tx_t* tx);

/*Set/get TXPI config: enable/disable it and set work mode. */
int falcon_dpll_phy_txpi_config_set(const phymod_phy_access_t* phy, const phymod_txpi_config_t* config);
int falcon_dpll_phy_txpi_config_get(const phymod_phy_access_t* phy, phymod_txpi_config_t* config);

/*TX transmission control*/
int falcon_dpll_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control);
int falcon_dpll_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control);

/*Rx control*/
int falcon_dpll_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control);
int falcon_dpll_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control);

#endif /*_FALCON_DPLL_H_*/
