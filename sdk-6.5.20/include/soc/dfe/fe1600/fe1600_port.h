/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE1600 PORT H
 */
 
#ifndef _SOC_FE1600_PORT_H_
#define _SOC_FE1600_PORT_H_

#include <bcm/stat.h>
#include <soc/dcmn/dcmn_defs.h>
#include <soc/error.h>


extern uint8 wcmod_ucode_WarpLite_WarpCoreC0_revA0[];
extern int   wcmod_ucode_WarpLite_WarpCoreC0_revA0_len;


#define FE1600_PORT_PCS_8_10_EFFECTIVE_RATE_PERCENT (80)

soc_error_t soc_fe1600_port_soc_init(int unit);
soc_error_t soc_fe1600_port_init(int unit);
soc_error_t soc_fe1600_port_deinit(int unit);
soc_error_t soc_fe1600_port_detach(int unit, soc_port_t port);
soc_error_t soc_fe1600_port_probe(int unit, pbmp_t pbmp, pbmp_t *okay_pbmp, int is_init_sequence);
soc_error_t soc_fe1600_port_control_pcs_set(int unit, soc_port_t port, soc_dcmn_port_pcs_t pcs);
soc_error_t soc_fe1600_port_control_pcs_get(int unit, soc_port_t port, soc_dcmn_port_pcs_t* pcs);
soc_error_t soc_fe1600_port_control_power_set(int unit, soc_port_t port, uint32 flags, soc_dcmn_port_power_t power);
soc_error_t soc_fe1600_port_control_power_get(int unit, soc_port_t port, soc_dcmn_port_power_t* power);
soc_error_t soc_fe1600_port_control_strip_crc_set(int unit, soc_port_t port, int strip_crc);
soc_error_t soc_fe1600_port_control_strip_crc_get(int unit, soc_port_t port, int* strip_crc);
soc_error_t soc_fe1600_port_control_rx_enable_set(int unit, soc_port_t port, uint32 flags, int enable);
soc_error_t soc_fe1600_port_control_tx_enable_set(int unit, soc_port_t port, int enable);
soc_error_t soc_fe1600_port_control_rx_enable_get(int unit, soc_port_t port, int* enable);
soc_error_t soc_fe1600_port_control_tx_enable_get(int unit, soc_port_t port, int* enable);
soc_error_t soc_fe1600_port_control_low_latency_set(int unit,soc_port_t port,int value);
soc_error_t soc_fe1600_port_control_low_latency_get(int unit,soc_port_t port,int* value);
soc_error_t soc_fe1600_port_control_fec_error_detect_set(int unit,soc_port_t port,int value);
soc_error_t soc_fe1600_port_control_fec_error_detect_get(int unit,soc_port_t port,int* value);
soc_error_t soc_fe1600_port_loopback_set(int unit, soc_port_t port, soc_dcmn_loopback_mode_t loopback);
soc_error_t soc_fe1600_port_loopback_get(int unit, soc_port_t port, soc_dcmn_loopback_mode_t* loopback);
soc_error_t soc_fe1600_port_fault_get(int unit, soc_port_t port, uint32* flags);
soc_error_t soc_fe1600_port_speed_max(int unit, soc_port_t port, int *speed);
soc_error_t soc_fe1600_port_link_status_get(int unit, soc_port_t port, int *status);
soc_error_t soc_fe1600_port_bucket_fill_rate_validate(int unit, uint32 bucket_fill_rate);
soc_error_t soc_fe1600_port_prbs_tx_enable_set(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value);
soc_error_t soc_fe1600_port_prbs_tx_enable_get(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value);
soc_error_t soc_fe1600_port_prbs_rx_enable_set(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value);
soc_error_t soc_fe1600_port_prbs_rx_enable_get(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value);
soc_error_t soc_fe1600_port_prbs_rx_status_get(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value);
soc_error_t soc_fe1600_port_prbs_polynomial_set(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value);
soc_error_t soc_fe1600_port_prbs_polynomial_get(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value);
soc_error_t soc_fe1600_port_prbs_tx_invert_data_set(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value);
soc_error_t soc_fe1600_port_prbs_tx_invert_data_get(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value);
soc_error_t soc_fe1600_port_speed_set(int unit, soc_port_t port, int speed);
soc_error_t soc_fe1600_port_speed_get(int unit, soc_port_t port, int *speed);
soc_error_t soc_fe1600_port_interface_get(int unit, soc_port_t port, soc_port_if_t* intf);
soc_error_t soc_fe1600_port_rate_egress_ppt_set(int unit, soc_port_t port, uint32 burst, uint32 nof_tiks);
soc_error_t soc_fe1600_port_rate_egress_ppt_get(int unit, soc_port_t port, uint32 *burst, uint32 *nof_tiks);
soc_error_t soc_fe1600_port_wcmod_ucode_load(int unit , int port, uint8 *arr,int arr_len);
soc_error_t soc_fe1600_port_serdes_power_disable(int unit , soc_pbmp_t disable_pbmp);
soc_error_t soc_fe1600_port_phy_enable_set(int unit, soc_port_t port, int enable);
soc_error_t soc_fe1600_port_phy_enable_get(int unit, soc_port_t port, int *enable);
soc_error_t soc_fe1600_port_phy_cl72_set(int unit, soc_port_t port, int enable);
soc_error_t soc_fe1600_port_phy_cl72_get(int unit, soc_port_t port, int *enable);
soc_error_t soc_fe1600_port_phy_control_set(int unit, soc_port_t port, int phyn, int lane, int is_sys_side, soc_phy_control_t type, uint32 value);
soc_error_t soc_fe1600_port_phy_control_get(int unit, soc_port_t port, int phyn, int lane, int is_sys_side, soc_phy_control_t type, uint32 *value);
soc_error_t soc_fe1600_port_burst_control_set(int unit, soc_port_t port, soc_dcmn_loopback_mode_t loopback);
soc_error_t soc_fe1600_port_phy_get(int unit, soc_port_t port, uint32 flags, uint32 phy_reg_addr, uint32 *phy_data);
soc_error_t soc_fe1600_port_phy_set(int unit, soc_port_t port, uint32 flags, uint32 phy_reg_addr, uint32 phy_data);
soc_error_t soc_fe1600_port_phy_modify(int unit, soc_port_t port, uint32 flags, uint32 phy_reg_addr, uint32 phy_data, uint32 phy_mask);
soc_error_t soc_fe1600_port_dynamic_port_update(int unit, int port, int enable, soc_dfe_port_update_type_t type);
soc_error_t soc_fe1600_port_pump_enable_set(int unit, soc_port_t port, int enable);
soc_error_t soc_fe1600_port_enable_set(int unit, soc_port_t port, int enable);
soc_error_t soc_fe1600_port_enable_get(int unit, soc_port_t port, int *enable);
soc_error_t soc_fe1600_port_quad_disabled(int unit, int quad, int *disabled);


#endif 
