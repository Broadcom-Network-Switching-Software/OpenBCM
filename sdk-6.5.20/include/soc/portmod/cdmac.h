/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    cdmac.h
 * Purpose: 
 */

#ifndef _PORTMOD_CDMAC_H_
#define _PORTMOD_CDMAC_H_

#include <soc/portmod/portmod.h>

#define CDMAC_NULL_FLAGS (0)

#define CDMAC_ENABLE_RX  (1)
#define CDMAC_ENABLE_TX  (2)

#define CDMAC_ENABLE_SET_FLAGS_SOFT_RESET         0x1
#define CDMAC_ENABLE_SET_FLAGS_TX_EN              0x2
#define CDMAC_ENABLE_SET_FLAGS_RX_EN              0x4

#define CDMAC_RUNT_THRESHOLD_MIN      64
#define CDMAC_RUNT_THRESHOLD_MAX      96
#define CDMAC_RUNT_THRESHOLD_DEFAULT  64

#define CDMAC_JUMBO_MAXSZ         0x3fe8

#define CDMAC_PAD_THRESHOLD_SIZE_MIN       64
#define CDMAC_PAD_THRESHOLD_SIZE_MAX       96
#define CDMAC_PAD_THRESHOLD_SIZE_DEFAULT   64

#define CDMAC_AVERAGE_IPG_DEFAULT  12

#define CDMAC_INIT_F_RX_STRIP_CRC               0x1
#define CDMAC_INIT_F_TX_APPEND_CRC              0x2
#define CDMAC_INIT_F_TX_REPLACE_CRC             0x4
#define CDMAC_INIT_F_TX_PASS_THROUGH_CRC_MODE   0x8

#define CDMAC_RSV_MASK_WRONG_SA             (0x1 << (0))
#define CDMAC_RSV_MASK_STACK_VLAN_DETECT    (0x1 << (1))
#define CDMAC_RSV_MASK_PFC_DA_ERR           (0x1 << (2)) /* unsupported PFC DA*/
#define CDMAC_RSV_MASK_PAUSE_DA_ERR         (0x1 << (2)) /* same bit as PFC */
#define CDMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR (0x1 << (3))
#define CDMAC_RSV_MASK_CRC_ERR              (0x1 << (4))
#define CDMAC_RSV_MASK_FRAME_LEN_ERR   (0x1 << (5)) /* IEEE length check fail */
#define CDMAC_RSV_MASK_TRUNCATED_FRAME (0x1 << (6)) /* truncated/out of range */
#define CDMAC_RSV_MASK_FRAME_RCV_OK    (0x1 << (7))
#define CDMAC_RSV_MASK_MCAST_FRAME          (0x1 << (8))
#define CDMAC_RSV_MASK_BCAST_FRAME          (0x1 << (9))
#define CDMAC_RSV_MASK_PROMISCUOUS_FRAME    (0x1 << (10))
#define CDMAC_RSV_MASK_CONTROL_FRAME        (0x1 << (11))
#define CDMAC_RSV_MASK_PAUSE_FRAME          (0x1 << (12))
#define CDMAC_RSV_MASK_OPCODE_ERR           (0x1 << (13))
#define CDMAC_RSV_MASK_VLAN_TAG_DETECT      (0x1 << (14))
#define CDMAC_RSV_MASK_UCAST_FRAME          (0x1 << (15))
#define CDMAC_RSV_MASK_RESERVED_0           (0x1 << (16))
#define CDMAC_RSV_MASK_RESERVED_1           (0x1 << (17))
#define CDMAC_RSV_MASK_PFC_FRAME            (0x1 << (18))

#define CDMAC_RSV_MASK_MIN           CDMAC_RSV_MASK_WRONG_SA
#define CDMAC_RSV_MASK_MAX           CDMAC_RSV_MASK_PFC_FRAME
#define CDMAC_RSV_MASK_ALL           ((CDMAC_RSV_MASK_PFC_FRAME) | \
                                     ((CDMAC_RSV_MASK_PFC_FRAME) - 1))

int cdmac_init(int unit, soc_port_t port, uint32 init_flags);
int cdmac_speed_set(int unit, soc_port_t port, int flags, int speed);
int cdmac_speed_get     (int unit, soc_port_t port, int *speed);
int cdmac_encap_set(int unit, soc_port_t port, int flags,
                    portmod_encap_t encap);
int cdmac_encap_get(int unit, soc_port_t port, int *flags,
                    portmod_encap_t *encap);
int cdmac_enable_set(int unit, soc_port_t port, int flags, int enable);
int cdmac_enable_get(int unit, soc_port_t port, int flags, int *enable);
int cdmac_duplex_set(int unit, soc_port_t port, int duplex);
int cdmac_duplex_get(int unit, soc_port_t port, int *duplex);

int cdmac_loopback_set(int unit, soc_port_t port, portmod_loopback_mode_t lb,
                       int enable);
int cdmac_loopback_get(int unit, soc_port_t port, portmod_loopback_mode_t lb,
                       int *enable);
int cdmac_discard_set   (int unit, soc_port_t port, int discard);
int cdmac_tx_enable_set (int unit, soc_port_t port, int enable);
int cdmac_tx_enable_get (int unit, soc_port_t port, int *enable);
int cdmac_rx_enable_set (int unit, soc_port_t port, int enable);
int cdmac_rx_enable_get (int unit, soc_port_t port, int *enable);
int cdmac_tx_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac);
int cdmac_tx_mac_sa_get(int unit, soc_port_t port, sal_mac_addr_t mac);
int cdmac_rx_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac);
int cdmac_rx_mac_sa_get(int unit, soc_port_t port, sal_mac_addr_t mac);
int cdmac_soft_reset_set(int unit, soc_port_t port, int enable);
int cdmac_soft_reset_get(int unit, soc_port_t port, int *enable);
int cdmac_rx_vlan_tag_set(int unit, soc_port_t port, int outer_vlan_tag,
                          int inner_vlan_tag);
int cdmac_rx_vlan_tag_get(int unit, soc_port_t port, int *outer_vlan_tag,
                          int *inner_vlan_tag);
int cdmac_rx_max_size_set(int unit, soc_port_t port, int value);
int cdmac_rx_max_size_get(int unit, soc_port_t port, int *value);
int cdmac_pad_size_set(int unit, soc_port_t port, int value);
int cdmac_pad_size_get(int unit, soc_port_t port, int *value);
int cdmac_tx_average_ipg_set(int unit, soc_port_t port, int val);
int cdmac_tx_average_ipg_get(int unit, soc_port_t port, int *val);
int cdmac_runt_threshold_set(int unit, soc_port_t port, int value);
int cdmac_runt_threshold_get(int unit, soc_port_t port, int *value);
int cdmac_remote_fault_control_get(int unit, soc_port_t port,
                            portmod_remote_fault_control_t *control);
int cdmac_remote_fault_control_set(int unit, soc_port_t port,
                            const portmod_remote_fault_control_t *control);
int cdmac_local_fault_control_get(int unit, soc_port_t port,
                            portmod_local_fault_control_t *control);
int cdmac_local_fault_control_set(int unit, soc_port_t port,
                             const portmod_local_fault_control_t *control);

int cdmac_local_fault_status_get  (int unit, soc_port_t port, int *status);
int cdmac_remote_fault_status_get (int unit, soc_port_t port, int *status);

int cdmac_pfc_control_set(int unit, soc_port_t port,
                          const portmod_pfc_control_t *control);
int cdmac_pfc_control_get(int unit, soc_port_t port,
                          portmod_pfc_control_t *control);
int cdmac_pause_control_set(int unit, soc_port_t port,
                            const portmod_pause_control_t *control);
int cdmac_pause_control_get(int unit, soc_port_t port,
                            portmod_pause_control_t *control);

int cdmac_pfc_config_set(int unit, int port,
                         const portmod_pfc_config_t* pfc_cfg);
int cdmac_pfc_config_get(int unit, int port,
                         portmod_pfc_config_t* pfc_cfg);

int cdmac_pass_control_frame_set(int unit, int port, int value);
int cdmac_pass_control_frame_get(int unit, int port, int *value);

int cdmac_pass_pfc_frame_set(int unit, int port, int value);
int cdmac_pass_pfc_frame_get(int unit, int port, int *value);

int cdmac_pass_pause_frame_set(int unit, int port, int value);
int cdmac_pass_pause_frame_get(int unit, int port, int *value);

int cdmac_lag_failover_loopback_set(int unit, int port, int val);
int cdmac_lag_failover_loopback_get(int unit, int port, int *val);
int cdmac_lag_failover_disable(int unit, int port);

int cdmac_lag_failover_en_get(int unit, int port, int *val);
int cdmac_lag_failover_en_set(int unit, int port, int val);

int cdmac_reset_fc_timers_on_link_dn_get (int unit, soc_port_t port, int *val);
int cdmac_reset_fc_timers_on_link_dn_set (int unit, soc_port_t port, int val);

int cdmac_lag_remove_failover_lpbk_get(int unit, int port, int *val);
int cdmac_lag_remove_failover_lpbk_set(int unit, int port, int val);

int cdmac_mac_ctrl_set(int unit, int port, uint64 ctrl);
int cdmac_drain_cell_get(int unit, int port,
                           portmod_drain_cells_t *drain_cells);
int cdmac_drain_cell_stop(int unit, int port,
                          const portmod_drain_cells_t *drain_cells);
int cdmac_drain_cell_start(int unit, int port);

int cdmac_txfifo_cell_cnt_get(int unit, int port, uint32* val);
int cdmac_egress_queue_drain_get(int unit, int port, uint64 *mac_ctrl,
                                 int *rx_en);

int cdmac_drain_cells_rx_enable(int unit, int port, int rx_en);
int cdmac_egress_queue_drain_rx_en(int unit, int port, int rx_en);
int cdmac_reset_check(int unit, int port, int enable, int *reset);

int cdmac_sw_link_status_select_set(int unit, soc_port_t port, int enable);
int cdmac_sw_link_status_select_get(int unit, soc_port_t port, int *enable);
int cdmac_sw_link_status_set (int unit, soc_port_t port, int link);
int cdmac_sw_link_status_get (int unit, soc_port_t port, int *link);

int cdmac_rsv_mask_control_set(int unit, int port, uint32 flags, uint32 value);
int cdmac_rsv_mask_control_get(int unit, int port, uint32 flags, uint32 *value);
int cdmac_rsv_mask_set(int unit, int port, uint32 rsv_mask);
int cdmac_rsv_mask_get(int unit, int port, uint32 *rsv_mask);

int cdmac_mib_counter_control_set(int unit, soc_port_t port,
                                  int enable, int clear);
int cdmac_cntmaxsize_set(int unit, int port, int val);
int cdmac_cntmaxsize_get(int unit, int port, int *val);

int cdmac_link_down_sequence_enable_set(int unit, int port, uint32 value);
int cdmac_link_down_sequence_enable_get(int unit, int port, uint32 *value);

int cdmac_interrupt_enable_set(int unit, int port, int intr_type, uint32 value);
int cdmac_interrupt_enable_get(int unit, int port, int intr_type, uint32 *value);
int cdmac_interrupt_status_get(int unit, int port, int intr_type, uint32 *value);
int cdmac_interrupts_status_get(int unit, int port, int arr_max_size,
                                uint32* intr_arr, uint32* size);
int cdmac_link_interrupt_ordered_set_enable(int unit, int port, uint32 enable);
int cdmac_link_interrupt_ordered_set_enable_get(int unit, int port, uint32* enable);
int cdmac_link_interruption_live_status_get(int unit, int port, uint32* status);
int cdmac_tx_stall_enable_set(int unit, int port, uint32 enable);

#endif /*_PORTMOD_CDMAC_H_*/
