/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PORTMOD_UNIMAC_H_
#define _PORTMOD_UNIMAC_H_

#include <soc/portmod/portmod.h>

#define UNIMAC_INIT_F_AUTO_CFG               0x1
#define UNIMAC_INIT_F_RX_STRIP_CRC           0x2

#define UNIMAC_ENABLE_SET_FLAGS_TX_EN              0x1
#define UNIMAC_ENABLE_SET_FLAGS_RX_EN              0x2


int unimac_init          (int unit, soc_port_t port, int init_flags);
int unimac_deinit        (int unit, soc_port_t port);
int unimac_speed_set     (int unit, soc_port_t port, int speed);
int unimac_speed_get     (int unit, soc_port_t port, int *speed);
int unimac_encap_set     (int unit, soc_port_t port, portmod_encap_t encap);
int unimac_encap_get     (int unit, soc_port_t port, portmod_encap_t *encap);
int unimac_enable_set    (int unit, soc_port_t port, int flags, int enable);
int unimac_enable_get    (int unit, soc_port_t port, int flags, int *enable);
int unimac_duplex_set    (int unit, soc_port_t port, int duplex);
int unimac_duplex_get    (int unit, soc_port_t port, int *duplex);
int unimac_loopback_set  (int unit, soc_port_t port, int enable);
int unimac_loopback_get  (int unit, soc_port_t port, int *enable);
int unimac_rx_enable_set (int unit, soc_port_t port, int enable);
int unimac_rx_enable_get (int unit, soc_port_t port, int *enable);

int unimac_mac_sa_set (int unit, soc_port_t port, sal_mac_addr_t mac);
int unimac_mac_sa_get (int unit, soc_port_t port, sal_mac_addr_t mac);

int unimac_soft_reset_set     (int unit, soc_port_t port, int enable);
int unimac_soft_reset_get     (int unit, soc_port_t port, int *enable);
int unimac_rx_max_size_set    (int unit, soc_port_t port, int value);
int unimac_rx_max_size_get    (int unit, soc_port_t port, int *value);
int unimac_tx_average_ipg_set (int unit, soc_port_t port, int ipg_val);
int unimac_tx_average_ipg_get (int unit, soc_port_t port, int *ipg_val);
int unimac_rx_vlan_tag_set    (int unit, soc_port_t port, int outer_vlan_tag, int inner_vlan_tag);
int unimac_rx_vlan_tag_get    (int unit, soc_port_t port, int *outer_vlan_tag, int *inner_vlan_tag);

int unimac_tx_preamble_length_set    (int unit, soc_port_t port, int length);

/***************************************************************************** 
 SDK Support Functions 
 */
int unimac_eee_set (int unit, int port, const portmod_eee_t* eee);
int unimac_eee_get (int unit, int port, const portmod_eee_t* eee);

int unimac_pfc_config_set (int unit, int port, const portmod_pfc_config_t* pfc_cfg);
int unimac_pfc_config_get (int unit, int port, const portmod_pfc_config_t* pfc_cfg);

int unimac_pause_control_set(int unit, soc_port_t port, const portmod_pause_control_t *control);
int unimac_pause_control_get(int unit, soc_port_t port, portmod_pause_control_t *control);

int unimac_pfc_control_set(int unit, soc_port_t port, const portmod_pfc_control_t *control);
int unimac_pfc_control_get(int unit, soc_port_t port, portmod_pfc_control_t *control);

int unimac_diag_fifo_status_get (int unit, int port, const portmod_fifo_status_t* inf);

int unimac_pass_control_frame_set(int unit, int port, int value);
int unimac_pass_control_frame_get(int unit, int port, int* value);

int unimac_pass_pause_frame_set(int unit, int port, uint32 value);
int unimac_pass_pause_frame_get(int unit, int port, uint32* value);

int unimac_mac_ctrl_set    (int unit, int port, uint32 ctrl);
int unimac_mac_ctrl_get(int unit, int port, uint32 *mac_ctrl, int *rx_en);
int unimac_drain_cell_get  (int unit, int port, portmod_drain_cells_t *drain_cells);

int unimac_tx_enable_set(int unit, soc_port_t port, int enable);
int unimac_tx_enable_get(int unit, soc_port_t port, int *enable);

int unimac_reset_check(int u, int p, int enable, int *reset);
#endif /*_PORTMOD_UNIMAC_H_*/
