/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *
 * Its contents are not used directly by applications; it is used only
 * by header files of parent APIs which need to define port modes.
 */

#ifndef _SOC_OAM_H
#define _SOC_OAM_H

#include <shared/port.h>


#define SOC_PORT_PHY_OAM_DM_MAC_CHECK_ENABLE _SHR_PORT_PHY_OAM_MAC_CHECK_ENABLE /* Enable MAC check for
                                                          Delay Measurement */
#define SOC_PORT_PHY_OAM_DM_CONTROL_WORD_ENABLE _SHR_PORT_PHY_OAM_CONTROL_WORD_ENABLE /* Enable Control Word
                                                          for Delay Measurement */

#define SOC_PORT_PHY_OAM_DM_ENTROPY_ENABLE  _SHR_PORT_PHY_OAM_DM_ENTROPY_ENABLE /* Enable entropy for
                                                          Delay Measurement */
#define SOC_PORT_PHY_OAM_DM_TS_FORMAT       _SHR_PORT_PHY_OAM_DM_TS_FORMAT /* Select timestamp
                                                          format PTP(0)/NTP(1)
                                                          for Delay Measurement */

/* Port controls for phy OAM */
typedef _shr_port_control_phy_oam_t soc_port_control_phy_oam_t;
#define SOC_PORT_CONTROL_PHY_OAM_DM_TX_ETHERTYPE _SHR_PORT_CONTROL_PHY_OAM_DM_TX_ETHERTYPE /* OAM Delay measurement Tx Ethertype */
#define SOC_PORT_CONTROL_PHY_OAM_DM_RX_ETHERTYPE _SHR_PORT_CONTROL_PHY_OAM_DM_RX_ETHERTYPE /* OAM Delay measurement Rx Ethertype */
#define SOC_PORT_CONTROL_PHY_OAM_DM_TX_PORT_MAC_ADDRESS_INDEX _SHR_PORT_CONTROL_PHY_OAM_DM_TX_PORT_MAC_ADDRESS_INDEX /* Index to select the MAC address. Use
                                           index of 1 to select MacAddress1,
                                           index of 2 for MacAddress2 and so
                                           forth. */
#define SOC_PORT_CONTROL_PHY_OAM_DM_RX_PORT_MAC_ADDRESS_INDEX _SHR_PORT_CONTROL_PHY_OAM_DM_RX_PORT_MAC_ADDRESS_INDEX /* Index to select the MAC address. Use
                                           index of 1 to select MacAddress1,
                                           index of 2 for MacAddress2 and so
                                           forth. */
#define SOC_PORT_CONTROL_PHY_OAM_DM_MAC_ADDRESS_1 _SHR_PORT_CONTROL_PHY_OAM_DM_MAC_ADDRESS_1 /* OAM Delay measurement MAC address 1.
                                           There can be multiple MAC address to
                                           choose from in a PHY depending upon
                                           the design */
#define SOC_PORT_CONTROL_PHY_OAM_DM_MAC_ADDRESS_2 _SHR_PORT_CONTROL_PHY_OAM_DM_MAC_ADDRESS_2 /* OAM Delay measurement MAC address 2.
                                           There can be multiple MAC address to
                                           choose from in a PHY depending upon
                                           the design */
#define SOC_PORT_CONTROL_PHY_OAM_DM_MAC_ADDRESS_3 _SHR_PORT_CONTROL_PHY_OAM_DM_MAC_ADDRESS_3 /* OAM Delay measurement MAC address 3.
                                           There can be multiple MAC address to
                                           choose from in a PHY depending upon
                                           the design */

/* Phy OAM Delay Measurement Modes */
typedef  _shr_port_config_phy_oam_dm_mode_t soc_port_config_phy_oam_dm_mode_t;
#define SOC_PORT_CONFIG_PHY_OAM_DM_Y1731 _SHR_PORT_CONFIG_PHY_OAM_DM_Y1731
#define SOC_PORT_CONFIG_PHY_OAM_DM_BHH _SHR_PORT_CONFIG_PHY_OAM_DM_BHH
#define SOC_PORT_CONFIG_PHY_OAM_DM_IETF _SHR_PORT_CONFIG_PHY_OAM_DM_IETF

/* Phy OAM Delay measurement config type. */
typedef struct soc_port_config_phy_oam_dm_s {
    uint32 flags;                    /* OAM Delay Measurement config flags */
    soc_port_config_phy_oam_dm_mode_t mode; /* OAM Delay Measurement mode Y.1731,
                                           BHH or IETF */
} soc_port_config_phy_oam_dm_t;

/* Phy OAM config type. */
typedef struct soc_port_config_phy_oam_s {
    soc_port_config_phy_oam_dm_t tx_dm_config; /* OAM delay measurement config for Tx */
    soc_port_config_phy_oam_dm_t rx_dm_config; /* OAM delay measurement config for Rx */
} soc_port_config_phy_oam_t;

/* soc_port_config_phy_oam_set */
extern int soc_port_config_phy_oam_set(
    int unit, 
    soc_port_t port, 
    soc_port_config_phy_oam_t *conf);

/* soc_port_config_phy_oam_get */
extern int soc_port_config_phy_oam_get(
    int unit, 
    soc_port_t port, 
    soc_port_config_phy_oam_t *conf);

/* soc_port_control_phy_oam_set */
extern int soc_port_control_phy_oam_set(
    int unit, 
    soc_port_t port, 
    soc_port_control_phy_oam_t type, 
    uint64 value);

/* soc_port_control_phy_oam_get */
extern int soc_port_control_phy_oam_get(
    int unit, 
    soc_port_t port, 
    soc_port_control_phy_oam_t type, 
    uint64 *value);

#endif  /* !_SOC_OAM_H */

