/*! \file bcmpmac_dcccport.h
 *
 * CDPORT_GEN4 driver.
 *
 * A DC3PORT contains 1 DC3PORT and 1 DC3MACs, and supports 8 ports at most.
 *
 * In this driver, we always use the port number 0~7 to access the CDPORT
 * and DC3MAC per-port registers
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef DCCCPORT_H
#define DCCCPORT_H

#include <soc/portmod/portmod.h>

#define DC3MAC_RSV_MASK_INCORR_SA            (0x1 << (0))
#define DC3MAC_RSV_MASK_STACK_VLAN_TAG_PKT   (0x1 << (1))
#define DC3MAC_RSV_MASK_INCORR_PAUSE_PFC_DA  (0x1 << (2))
#define DC3MAC_RSV_MASK_RX_TERM_ERR          (0x1 << (3))
#define DC3MAC_RSV_MASK_CRC_ERR              (0x1 << (4))
#define DC3MAC_RSV_MASK_FRAME_LEN_ERR   (0x1 << (5)) /* IEEE length check fail */
#define DC3MAC_RSV_MASK_LONG_PKT        (0x1 << (6)) /* truncated/out of range */
#define DC3MAC_RSV_MASK_GOOD_PKT             (0x1 << (7))
#define DC3MAC_RSV_MASK_MCAST_FRAME          (0x1 << (8))
#define DC3MAC_RSV_MASK_BCAST_FRAME          (0x1 << (9))
#define DC3MAC_RSV_MASK_PROMISCUOUS_FRAME    (0x1 << (10))
#define DC3MAC_RSV_MASK_CONTROL_FRAME        (0x1 << (11))
#define DC3MAC_RSV_MASK_PAUSE_FRAME          (0x1 << (12))
#define DC3MAC_RSV_MASK_OPCODE_ERR           (0x1 << (13))
#define DC3MAC_RSV_MASK_VLAN_TAG_DETECT      (0x1 << (14))
#define DC3MAC_RSV_MASK_UCAST_FRAME          (0x1 << (15))
#define DC3MAC_RSV_MASK_RESERVED_0           (0x1 << (16))
#define DC3MAC_RSV_MASK_RESERVED_1           (0x1 << (17))
#define DC3MAC_RSV_MASK_PFC_FRAME            (0x1 << (18))

#define DC3MAC_RSV_MASK_MIN           DC3MAC_RSV_MASK_INCORR_SA
#define DC3MAC_RSV_MASK_MAX           DC3MAC_RSV_MASK_PFC_FRAME
#define DC3MAC_RSV_MASK_ALL           ((DC3MAC_RSV_MASK_PFC_FRAME) | \
                                     ((DC3MAC_RSV_MASK_PFC_FRAME) - 1))

/******************************************************************************
 * Private functions
 ******************************************************************************/

int
dc3port_port_reset_set(int unit, soc_port_t port, uint32_t reset);

int
dc3port_port_reset_get(int unit, soc_port_t port, uint32_t *reset);

int
dc3port_port_enable_set(int unit, soc_port_t port, uint32_t enable);

int
dc3port_port_enable_get(int unit, soc_port_t port, uint32_t *enable);

int
dc3mac_reset_set(int unit, soc_port_t port, uint32_t reset);

int
dc3mac_reset_get(int unit, soc_port_t port, uint32_t *reset);

int
dc3mac_rx_enable_set(int unit, soc_port_t port, uint32_t enable);

int
dc3mac_rx_enable_get(int unit, soc_port_t port, uint32_t *enable);

int
dc3mac_tx_enable_set(int unit, soc_port_t port, uint32_t enable);

int
dc3mac_tx_enable_get(int unit, soc_port_t port, uint32_t *enable);

int
dc3mac_encap_set(int unit, soc_port_t port, portmod_encap_t encap);

int
dc3mac_encap_get(int unit, soc_port_t port, portmod_encap_t *encap);

int
dc3port_lpbk_set(int unit, soc_port_t port, uint32_t en);

int
dc3port_lpbk_get(int unit, soc_port_t port, uint32_t *en);

int
dc3mac_pause_set(int unit, soc_port_t port,
                 const portmod_pause_control_t *ctrl);

int
dc3mac_pause_get(int unit, soc_port_t port,
                 portmod_pause_control_t *ctrl);

int
dc3mac_pause_addr_set(int unit, soc_port_t port,
                      sal_mac_addr_t mac);

int
dc3mac_pause_addr_get(int unit, soc_port_t port,
                      sal_mac_addr_t mac);

int
dc3mac_frame_max_set(int unit, soc_port_t port,
                     uint32_t size);

int
dc3mac_frame_max_get(int unit, soc_port_t port,
                     uint32_t *size);

int
dc3mac_remote_fault_status_get(int unit, soc_port_t port,
                               int *status);

int
dc3mac_local_fault_status_get(int unit, soc_port_t port,
                              int *status);

int
dc3mac_pfc_control_set(int unit, soc_port_t port,
                       const portmod_pfc_control_t  *cfg);

int
dc3mac_pfc_control_get(int unit, soc_port_t port,
                       portmod_pfc_control_t  *cfg);

int
dc3mac_pfc_config_set(int unit, soc_port_t port,
                      const portmod_pfc_config_t *cfg);

int
dc3mac_pfc_config_get(int unit, soc_port_t port,
                      portmod_pfc_config_t *cfg);

int
dc3mac_local_fault_disable_set(int unit, soc_port_t port,
                               const portmod_local_fault_control_t *st);

int
dc3mac_local_fault_disable_get(int unit, soc_port_t port,
                               portmod_local_fault_control_t *st);

int
dc3mac_remote_fault_disable_set(int unit, soc_port_t port,
                                const portmod_remote_fault_control_t *st);

int
dc3mac_remote_fault_disable_get(int unit, soc_port_t port,
                                portmod_remote_fault_control_t *st);

int
dc3mac_avg_ipg_set(int unit, soc_port_t port,
                   uint8_t ipg_size);

int
dc3mac_avg_ipg_get(int unit, soc_port_t port,
                   uint8_t *ipg_size);

int dc3mac_interrupt_enable_get(int unit, int port, int intr_type, uint32 *value);

int dc3mac_interrupt_enable_set(int unit, int port, int intr_type, uint32 value);

int dc3mac_interrupt_status_get(int unit, int port, int intr_type, uint32 *value);

int dc3mac_interrupts_status_get(int unit, int port, int arr_max_size,
                                 uint32* intr_arr, uint32* size);
int
dc3mac_mib_counter_control_set(int unit, soc_port_t port,
                               int enable, int clear);

int
dc3mac_mib_oversize_set(int unit, soc_port_t port,
                        uint32_t size);

int
dc3mac_mib_oversize_get(int unit, soc_port_t port,
                        uint32_t *size);

int
dc3mac_pass_control_frame_set(int unit, soc_port_t port,
                              uint32_t enable);
int
dc3mac_pass_control_frame_get(int unit, soc_port_t port,
                              uint32_t *enable);
int
dc3mac_pass_pfc_frame_set(int unit, soc_port_t port,
                          uint32_t enable);
int
dc3mac_pass_pfc_frame_get(int unit, soc_port_t port,
                          uint32_t *enable);
int
dc3mac_pass_pause_frame_set(int unit, soc_port_t port,
                            uint32_t enable);
int
dc3mac_pass_pause_frame_get(int unit, soc_port_t port,
                            uint32_t *enable);

int
dc3mac_discard_set(int unit, soc_port_t port,
                   uint32_t discard);

int
dc3mac_discard_get(int unit, soc_port_t port,
                   uint32_t *discard);

int
dc3mac_stall_tx_enable_get(int unit, soc_port_t port,
                           int *enable);

int
dc3mac_stall_tx_enable_set(int unit, soc_port_t port,
                           int enable);

int
dc3mac_runt_threshold_get(int unit, soc_port_t port,
                          uint32_t *value);

int
dc3mac_port_init(int unit, soc_port_t port,
                 uint32_t init);

int
dc3mac_force_pfc_xon_set(int unit, soc_port_t port,
                         uint32_t value);

int
dc3mac_force_pfc_xon_get(int unit, soc_port_t port,
                         uint32_t *value);

int
dc3mac_rsv_mask_set(int unit, soc_port_t port,
                    uint32_t rsv_mask);

int
dc3mac_rsv_mask_get(int unit, soc_port_t port,
                    uint32_t *rsv_mask);

/*
 * This function controls which RSV(Receive statistics vector) event
 * causes a purge event that triggers RXERR to be set for the packet
 * sent by the MAC to the IP. These bits are used to mask RSV[34:16]
 * for DC3MAC; bit[18] of MASK maps to bit[34] of RSV, bit[0] of MASK
 * maps to bit[16] of RSV.
 * Enable : Set 0. Go through
 * Disable: Set 1. Purged.
 * bit[18] --> PFC frame detected
 * bit[17] --> Reserved
 * bit[16] --> Reserved
 * bit[15] --> Unicast detected
 * bit[14] --> VLAN tag detected
 * bit[13] --> Unsupported opcode detected
 * bit[12] --> Pause frame received
 * bit[11] --> Control frame received
 * bit[10] --> Promiscuous packet detected
 * bit[ 9] --> Broadcast detected
 * bit[ 8] --> Multicast detected
 * bit[ 7] --> Receive OK
 * bit[ 6] --> Truncated/Frame out of Range
 * bit[ 5] --> Frame length not out of range, but incorrect -
 *             IEEE length check failed
 * bit[ 4] --> CRC error
 * bit[ 3] --> Receive terminate/code error
 * bit[ 2] --> Unsupported DA for pause/PFC packets detected
 * bit[ 1] --> Stack VLAN detected
 * bit[ 0] --> Wrong SA
 */
int
dc3mac_rsv_selective_mask_set(int unit, soc_port_t port,
                              uint32_t flags, uint32_t value);

int
dc3mac_rsv_selective_mask_get(int unit, soc_port_t port,
                              uint32_t flags, uint32_t *value);

int
dc3mac_strip_crc_get(int unit, soc_port_t port,
                     uint32_t *enable);

int
dc3mac_strip_crc_set(int unit, soc_port_t port,
                     uint32_t enable);

int
dc3mac_tx_crc_mode_get(int unit, soc_port_t port,
                       uint32_t *crc_mode);

int
dc3mac_tx_crc_mode_set(int unit, soc_port_t port,
                       uint32_t crc_mode);

int
dc3mac_tx_threshold_get(int unit, soc_port_t port,
                        uint32_t *threshold);

int
dc3mac_tx_threshold_set(int unit, soc_port_t port,
                        uint32_t threshold);

int
dc3port_port_mode_set(int unit, soc_port_t port,
                      uint32_t flags, uint32_t lane_mask);

int
dc3port_dc3mac_control_set(int unit, soc_port_t port,
                           uint32_t reset);

int
dc3port_dc3mac_control_get(int unit, soc_port_t port,
                           uint32_t *reset);

int
dc3port_tsc_ctrl_get(int unit, soc_port_t port,
                     uint32_t *tsc_rstb, uint32_t *tsc_pwrdwn);

int
dc3port_tsc_ctrl_set(int unit, soc_port_t port,
                     int tsc_pwr_on);

int
dc3port_link_status_get(int unit, soc_port_t port,
                        uint32_t start_lane, int* link);

int
dc3mac_port_enable_set(int unit, soc_port_t port,
                       uint32_t enable);

int
dc3mac_port_fdr_symbol_error_window_size_set(int unit, soc_port_t port,
                                             uint32_t window_size);

int
dc3mac_port_fdr_symbol_error_window_size_get(int unit, soc_port_t port,
                                             uint32_t* window_size);

int
dc3mac_port_fdr_symbol_error_count_threshold_set(int unit, soc_port_t port,
                       uint32_t threshold);

int
dc3mac_port_fdr_symbol_error_count_threshold_get(int unit, soc_port_t port,
                                                 uint32_t* threshold);

int
dc3mac_port_mac_link_down_seq_enable_set(int unit, soc_port_t port,
                                         uint32_t enable);

int
dc3mac_rx_da_timestmap_enable_get(int unit, soc_port_t port,
                                  uint32_t *enable);

int
dc3mac_rx_da_timestmap_enable_set(int unit, soc_port_t port,
                                  uint32_t enable);

int
dc3mac_reset_check(int unit, int port, int enable, int *reset);

int
dc3mac_txfifo_cell_cnt_get(int unit, int port, uint32* val);

int
dc3mac_mac_ctrl_set(int unit, int port, uint64 ctrl);

int
dc3port_port_fault_link_status_set(int unit, int port, int enable);

int dc3mac_vlan_tag_set(int unit, soc_port_t port, int outer_vlan_tag,
                        int inner_vlan_tag);

int dc3mac_vlan_tag_get(int unit, soc_port_t port, int *outer_vlan_tag,
                        int *inner_vlan_tag);

int dc3mac_drain_cell_get(int unit, int port,
                          portmod_drain_cells_t *drain_cells);

int dc3mac_drain_cell_stop(int unit, int port,
                           const portmod_drain_cells_t *drain_cells);

int dc3mac_drain_cell_start(int unit, int port);

int dc3mac_drain_cells_rx_enable(int unit, int port, int rx_en);

int dc3mac_egress_queue_drain_rx_en(int unit, int port, int rx_en);

int dc3mac_egress_queue_drain_get(int unit, int port, uint64 *mac_ctrl,
                                  int *rx_en);

int dc3mac_link_fault_os_set(int unit, int port, int is_remote, uint32 enable);

int dc3mac_link_fault_os_get(int unit, int port, int is_remote, uint32 *enable);
int dc3port_timestamp_fifo_status_get(int unit, int port, int start_lane, uint32 *ts_valid);

#endif /*_PORTMOD_CDMAC_H_*/
