/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PORTMOD_CLMAC_H_
#define _PORTMOD_CLMAC_H_

#include <soc/portmod/portmod.h>

/*
 * version device
 * 0xA023  TD2P, Apache, Jericho, Jericho+, QAX_A0, QMX
 * 0xA027  QAX_B0
 * 0xA030  TH2, Apache_B0
 */
typedef enum {
    CLMAC_VERSION_A023 = 0xA023,
    CLMAC_VERSION_A027 = 0xA027,
    CLMAC_VERSION_A030 = 0xA030
}clmac_version_t;

#define CLMAC_ENCAP_SET_FLAGS_NO_SOP_FOR_CRC_HG  (1)
#define CLMAC_ENCAP_SET_FLAGS_EXTENDED_HIGIG2_EN  (2)
#define CLMAC_ENCAP_SET_FLAGS_SOFT_RESET_DIS      (4)
#define CLMAC_SPEED_SET_FLAGS_SOFT_RESET_DIS      (1)

#define CLMAC_ENABLE_SET_FLAGS_SOFT_RESET_DIS     0x1
#define CLMAC_ENABLE_SET_FLAGS_TX_EN              0x2
#define CLMAC_ENABLE_SET_FLAGS_RX_EN              0x4

#define CLMAC_ENABLE_RX  (1)
#define CLMAC_ENABLE_TX  (2)

#define CLMAC_INIT_F_RX_STRIP_CRC               0x1
#define CLMAC_INIT_F_TX_APPEND_CRC              0x2
#define CLMAC_INIT_F_TX_REPLACE_CRC             0x4
#define CLMAC_INIT_F_TX_PASS_THROUGH_CRC_MODE   0x8
#define CLMAC_INIT_F_IS_HIGIG                   0x10
#define CLMAC_INIT_F_IPG_CHECK_DISABLE          0x20

/* MAC Preemption TX status */
/*
 * if VERIFY_ENABLE = 1, but its status is failed or not initiaded
 * if VERIFY_ENABLE = 0, preemption is not enabled
 */
#define CLMAC_PREEMPT_TX_INACTIVE             0x0
/*
 * Preemption is enabled, Verify is enabled and its
 * operation is in progress.
 */
#define CLMAC_PREEMPT_TX_VERIFY_IN_PROGRESS   0x1
/*
 * Preemption is enabled and active.
 * if VERIFY_ENABLE = 1, verify is enabled and succeed
 * if VERIFY_ENABLE = 0, preemption is enabled
 */
#define CLMAC_PREEMPT_TX_ACTIVE               0x2

/* MAC preemption verification state */
#define CLMAC_PREEMPT_VERIFY_DISABLED    0x0 /* disabled */
#define CLMAC_PREEMPT_VERIFY_INITIAL     0x1 /* enabled but not started */
#define CLMAC_PREEMPT_VERIFY_VERIFYING   0x2 /* in progress */
#define CLMAC_PREEMPT_VERIFY_SUCCEED     0x3 /* successful */
#define CLMAC_PREEMPT_VERIFY_FAILED      0x4 /* failed */

/*
 * MAC Preemption Tx status flags
 * These macros are used as flags to retrieve the preemption status
 * on the transmit side.
 * Used by function clmac_preemption_tx_status_get() to get either
 * TX, Verify operation status or Number of Verify Messages sent
 * during preemption.
 */
#define CLMAC_PREEMPTION_TX_STATUS          0x1
#define CLMAC_PREEMPTION_VERIFY_STATUS      0x2
#define CLMAC_PREEMPTION_VERIFY_CNT_STATUS  0x3

/* MAC preemption force lost rx EOP flags */
#define CLMAC_PREEMPTION_FORCE_SW_RX_EOP       0x1
#define CLMAC_PREEMPTION_FORCE_AUTO_HW_RX_EOP  0x2

/* MAC timestamp byte adjust flags */
#define CLMAC_TIMESTAMP_BYTE_ADJUST_CLEAR 0x1

#define CLMAC_RUNT_THRESHOLD_MIN   0x31
#define CLMAC_RUNT_THRESHOLD_MAX   0x60

int clmac_version_get(int unit, soc_port_t port, int32 *version);
int clmac_init(int unit, soc_port_t port, uint32 init_flags);
int clmac_speed_set(int unit, soc_port_t port, int flags, int speed);
int clmac_speed_get     (int unit, soc_port_t port, int *speed);
int clmac_encap_set(int unit, soc_port_t port, int flags, portmod_encap_t encap);
int clmac_encap_get(int unit, soc_port_t port, int *flags, portmod_encap_t *encap);
int clmac_enable_set(int unit, soc_port_t port, int flags, int enable);
int clmac_enable_get(int unit, soc_port_t port, int flags, int *enable);
int clmac_duplex_set(int unit, soc_port_t port, int duplex);
int clmac_duplex_get(int unit, soc_port_t port, int *duplex);

int clmac_loopback_set(int unit, soc_port_t port, portmod_loopback_mode_t lb, int enable);
int clmac_loopback_get(int unit, soc_port_t port, portmod_loopback_mode_t lb, int *enable);
int clmac_discard_set   (int unit, soc_port_t port, int discard);
int clmac_tx_enable_set (int unit, soc_port_t port, int enable);
int clmac_tx_enable_get (int unit, soc_port_t port, int *enable);
int clmac_rx_enable_set (int unit, soc_port_t port, int enable);
int clmac_rx_enable_get (int unit, soc_port_t port, int *enable);
int clmac_tx_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac);
int clmac_tx_mac_sa_get(int unit, soc_port_t port, sal_mac_addr_t mac);
int clmac_rx_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac);
int clmac_rx_mac_sa_get(int unit, soc_port_t port, sal_mac_addr_t mac);
int clmac_soft_reset_set     (int unit, soc_port_t p, int enable);
int clmac_soft_reset_get     (int unit, soc_port_t p, int *enable);
int clmac_rx_vlan_tag_set(int unit, soc_port_t port, int outer_vlan_tag, int inner_vlan_tag);
int clmac_rx_vlan_tag_get(int unit, soc_port_t port, int *outer_vlan_tag, int *inner_vlan_tag);
int clmac_rx_max_size_set(int unit, soc_port_t port, int value);
int clmac_rx_max_size_get(int unit, soc_port_t port, int *value);
int clmac_pad_size_set(int unit, soc_port_t port, int value);
int clmac_pad_size_get(int unit, soc_port_t port, int *value);
int clmac_tx_average_ipg_set(int unit, soc_port_t port, int val);
int clmac_tx_average_ipg_get(int unit, soc_port_t port, int *val);
int clmac_runt_threshold_set(int unit, soc_port_t port, int value);
int clmac_runt_threshold_get(int unit, soc_port_t port, int *value);
int clmac_strict_preamble_set(int unit, soc_port_t port, int value);
int clmac_strict_preamble_get(int unit, soc_port_t port, int *value);
int clmac_sw_link_status_set (int unit, soc_port_t p, int link);

int xlmac_tx_preamble_length_set    (int unit, soc_port_t p, int length);
int xlmac_sw_link_status_select_set (int unit, soc_port_t p, int enable);
int xlmac_sw_link_status_select_get (int unit, soc_port_t p, int *enable);

int clmac_timestamp_delay_set(int unit, soc_port_t port, portmod_port_ts_adjust_t ts_adjust);
int clmac_timestamp_delay_get(int unit, soc_port_t port, portmod_port_ts_adjust_t *ts_adjust);
int clmac_timestamp_byte_adjust_set(int unit, soc_port_t port, int flags, int speed);

/******************************************************************************* 
 Remote/local Fault
 */
int clmac_remote_fault_control_get(int unit, soc_port_t port, portmod_remote_fault_control_t *control);
int clmac_remote_fault_control_set(int unit, soc_port_t port, const portmod_remote_fault_control_t *control);
int clmac_local_fault_control_get(int unit, soc_port_t port, portmod_local_fault_control_t *control);
int clmac_local_fault_control_set(int unit, soc_port_t port, const portmod_local_fault_control_t *control);
int clmac_local_fault_status_get  (int unit, soc_port_t port, int clear_status, int *status);
int clmac_remote_fault_status_get (int unit, soc_port_t port, int clear_status, int *status);
int clmac_clear_rx_lss_status_set (int unit, soc_port_t port, int lcl_fault, int rmt_fault);
int clmac_clear_rx_lss_status_get (int unit, soc_port_t port, int *lcl_fault, int *rmt_fault);

/******************************************************************************* 
 Flow Control
 */
int clmac_pfc_control_set(int unit, soc_port_t port,  const portmod_pfc_control_t *control);
int clmac_pfc_control_get(int unit, soc_port_t port, portmod_pfc_control_t *control);
int clmac_llfc_control_set(int unit, soc_port_t port,  const portmod_llfc_control_t *control);
int clmac_llfc_control_get(int unit, soc_port_t port, portmod_llfc_control_t *control);
int clmac_pause_control_set(int unit, soc_port_t port,  const portmod_pause_control_t *control);
int clmac_pause_control_get(int unit, soc_port_t port, portmod_pause_control_t *control);

/***************************************************************************** 
 SDK Support Functions 
 */
int clmac_eee_set(int u, int p, const portmod_eee_t* eee);
int clmac_eee_get(int u, int p, portmod_eee_t* eee);

int clmac_pfc_config_set (int u, int p, const portmod_pfc_config_t* pfc_cfg);
int clmac_pfc_config_get (int u, int p, const portmod_pfc_config_t* pfc_cfg);

int clmac_diag_fifo_status_get (int u, int p, const portmod_fifo_status_t* ts_info);
int clmac_timesync_tx_info_get (int unit, int port, portmod_fifo_status_t* tx_info);

int clmac_frame_spacing_stretch_set (int u, int p, int spacing);
int clmac_frame_spacing_stretch_get (int u, int p, int *spacing);

/*****************************************************************************
 send CTRL/PAUSE/PFC  frame to system side
 */
int clmac_pass_control_frame_set(int unit, int port, int value);
int clmac_pass_pfc_frame_set(int unit, int port, int value);
int clmac_pass_pause_frame_set(int unit, int port, int value);
int clmac_pass_control_frame_get(int unit, int port, int *value);
int clmac_pass_pfc_frame_get(int unit, int port, int *value);
int clmac_pass_pause_frame_get(int unit, int port, int *value);

int clmac_lag_failover_loopback_set(int unit, int port, int val);
int clmac_lag_failover_loopback_get(int unit, int port, int *val);

int clmac_lag_failover_en_get(int unit, int port, int *val);
int clmac_lag_failover_en_set(int unit, int port, int val);

int clmac_reset_fc_timers_on_link_dn_get (int unit, soc_port_t port, int *val);
int clmac_reset_fc_timers_on_link_dn_set (int unit, soc_port_t port, int val);

int clmac_lag_remove_failover_lpbk_get(int unit, int port, int *val);
int clmac_lag_remove_failover_lpbk_set(int unit, int port, int val);

int clmac_lag_failover_disable(int unit, int port);

int clmac_mac_ctrl_set    (int u, int p, uint64 ctrl);
int clmac_drain_cell_get  (int unit, int port, portmod_drain_cells_t *drain_cells);
int clmac_drain_cell_stop (int unit, int port, const portmod_drain_cells_t *drain_cells);
int clmac_drain_cell_start(int u, int p);

int clmac_txfifo_cell_cnt_get     (int u, int p, uint32* fval);
int clmac_egress_queue_drain_get  (int u, int p, uint64 *ctrl, int *rx);

int clmac_drain_cells_rx_enable   (int u, int p, int rx_en);
int clmac_egress_queue_drain_rx_en(int u, int p, int rx_en);
int clmac_reset_check(int u, int p, int enable, int *reset);

int clmac_e2ecc_hdr_get (int unit, int port, uint32_t *words);
int clmac_e2ecc_hdr_set (int unit, int port, uint32_t *words);

int clmac_e2e_enable_set(int unit, int port, int enable);
int clmac_e2e_enable_get(int unit, int port, int *nable);

int clmac_sw_link_status_select_set(int unit, soc_port_t port, int enable);
int clmac_sw_link_status_select_get(int unit, soc_port_t port, int *enable);

/*****************************************************************************
Interrupt enable and status
 */
int clmac_interrupt_enable_get(int unit, int port, int intr_type, uint32 *value);
int clmac_interrupt_enable_set(int unit, int port, int intr_type, uint32 value);
int clmac_interrupt_status_get(int unit, int port, int intr_type, uint32 *value);
int clmac_interrupts_status_get(int unit, int port, int arr_max_size, uint32* intr_arr, uint32* size);

int clmac_preemption_mode_set(int32 unit, soc_port_t port, uint32 enable);
int clmac_preemption_mode_get(int32 unit, soc_port_t port, uint32 *enable);

int clmac_preemption_tx_enable_set(int32 unit, soc_port_t port, uint32 enable);
int clmac_preemption_tx_enable_get(int32 unit, soc_port_t port, uint32 *enable);

int clmac_preemption_verify_enable_set(int32 unit, soc_port_t port,
                                       uint32 enable);
int clmac_preemption_verify_enable_get(int32 unit, soc_port_t port,
                                       uint32 *enable);

int clmac_preemption_verify_time_set(int32 unit, soc_port_t port,
                                     uint32 time_val);
int clmac_preemption_verify_time_get(int32 unit, soc_port_t port,
                                     uint32 *time_val);

int clmac_preemption_verify_attempts_set(int32 unit, soc_port_t port,
                                         uint32 num_attempts);
int clmac_preemption_verify_attempts_get(int32 unit, soc_port_t port,
                                         uint32 *num_attempts);

int clmac_preemption_tx_status_get(int unit, soc_port_t port,
                                   int flags, uint32 *value);

int clmac_preemption_force_rx_lost_eop_set(int unit, soc_port_t port,
                                           int flags, uint32 value);
int clmac_preemption_force_rx_lost_eop_get(int unit, soc_port_t port,
                                           int flags, uint32 *value);

int clmac_preemption_rx_timeout_info_set(int unit, soc_port_t port,
                                         uint32 enable, uint32 timeout_val);
int clmac_preemption_rx_timeout_info_get(int unit, soc_port_t port,
                                         uint32 *enable, uint32 *timeout_val);
int clmac_tx_stall_enable_set(int unit, int port, int enable);
int clmac_tx_stall_enable_get(int unit, int port, int* enable);

#endif /*_PORTMOD_CLMAC_H_*/
