/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC NIF PORTS MANAGER H
 */
 
#ifndef _SOC_NIF_PORTS_MANAGER_H_
#define _SOC_NIF_PORTS_MANAGER_H_

#include <bcm/fabric.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/error.h>


soc_error_t soc_pm_init(int unit, soc_pbmp_t all_phy_pbmp, uint32 first_direct_port);
soc_error_t soc_pm_mac_reset_set(int unit, soc_port_t port, uint32 is_in_reset);


soc_error_t soc_pm_port_remove(int unit, soc_port_t port);
soc_error_t soc_pm_speed_set(int unit, soc_port_t port, int speed); 
soc_error_t soc_pm_higig_set(int unit, soc_port_t port, uint32 is_higig);
soc_error_t soc_pm_ports_init(int unit, soc_pbmp_t pbmp, soc_pbmp_t* okay_ports);
soc_error_t soc_pm_port_deinit(int unit, soc_port_t port);
soc_error_t soc_pm_enable_set(int unit, soc_port_t port, uint32 enable);
soc_error_t soc_pm_mac_loopback_set(int unit, soc_port_t port, uint32 lb);
soc_error_t soc_pm_port_clp_reset(int unit, soc_port_t port, int full_reset);
soc_error_t soc_pm_port_recover(int unit, soc_pbmp_t *okay_nif_ports);
soc_error_t soc_pm_frame_max_set(int unit, soc_port_t port, int size);
soc_error_t soc_pm_ilkn_dynamic_num_of_lanes_set(int unit, soc_port_t port, int lanes_number);
soc_error_t soc_pm_mac_enable_set(int unit, soc_port_t port, uint32 enable);
soc_error_t soc_pm_mac_speed_set(int unit, soc_port_t port, uint32 speed);
soc_error_t soc_pm_tx_remote_fault_enable_set(int unit, soc_port_t port, int enable);
soc_error_t soc_pm_fault_remote_enable_set(int unit, soc_port_t port, uint32 value);
soc_error_t soc_pm_fault_local_enable_set(int unit, soc_port_t port, uint32 value);


soc_error_t soc_pm_ilkn_id_get(int unit,soc_port_t port, uint32* ilkn_id);
soc_error_t soc_pm_default_speed_get(int unit, soc_port_t port, int* speed);
soc_error_t soc_pm_speed_get(int unit, soc_port_t port, int* speed);
soc_error_t soc_pm_enable_get(int unit, soc_port_t port, uint32* enable);
soc_error_t soc_pm_master_get(int unit, soc_port_t port, soc_port_t* master_port);
soc_error_t soc_pm_mac_loopback_get(int unit, soc_port_t port, uint32* lb);
soc_error_t soc_pm_frame_max_get(int unit, soc_port_t port, int* size);
soc_error_t soc_pm_local_fault_get(int unit, soc_port_t port, int* local_fault);
soc_error_t soc_pm_remote_fault_get(int unit, soc_port_t port, int* remote_fault);
soc_error_t soc_pm_local_fault_clear(int unit, soc_port_t port);
soc_error_t soc_pm_remote_fault_clear(int unit, soc_port_t port);
soc_error_t soc_pm_pad_size_set(int unit, soc_port_t port, int value);
soc_error_t soc_pm_pad_size_get(int unit, soc_port_t port, int *value);
soc_error_t soc_pm_mac_ability_get(int unit, soc_port_t port, soc_port_ability_t *mac_ability);
soc_error_t soc_pm_ilkn_dynamic_num_of_lanes_get(int unit, soc_port_t port, uint32 *lanes_number);
soc_error_t soc_pm_tx_remote_fault_enable_get(int unit, soc_port_t port, int *enable);
soc_error_t soc_pm_serdes_quads_in_use_get(int unit, soc_port_t port, soc_pbmp_t* quads_in_use);
soc_error_t soc_pm_pfc_refresh_set(int unit, soc_port_t port, int value);
soc_error_t soc_pm_pfc_refresh_get(int unit, soc_port_t port, int *value);
soc_error_t soc_pm_mac_sa_set(int unit, int port, sal_mac_addr_t mac_sa);
soc_error_t soc_pm_mac_sa_get(int unit, int port, sal_mac_addr_t mac_sa);
soc_error_t soc_pm_pause_set(int unit, soc_port_t port, int pause_tx, int pause_rx);
soc_error_t soc_pm_pause_get(int unit, soc_port_t port, int *pause_tx, int *pause_rx);
soc_error_t soc_pm_pfc_set(int unit, soc_port_t port, int is_rx , int pfc);
soc_error_t soc_pm_pfc_get(int unit, soc_port_t port, int is_rx , int *pfc);
soc_error_t soc_pm_llfc_set(int unit, soc_port_t port, int is_rx , int llfc);
soc_error_t soc_pm_llfc_get(int unit, soc_port_t port, int is_rx , int *llfc);
soc_error_t soc_pm_fault_remote_enable_get(int unit, soc_port_t port, uint32 *value);
soc_error_t soc_pm_fault_local_enable_get(int unit, soc_port_t port, uint32 *value);


soc_error_t soc_pm_eee_enable_get(int unit, soc_port_t port, uint32 *value);
soc_error_t soc_pm_eee_enable_set(int unit, soc_port_t port, uint32 value);
soc_error_t soc_pm_eee_tx_idle_time_get(int unit, soc_port_t port, uint32 *value);
soc_error_t soc_pm_eee_tx_idle_time_set(int unit, soc_port_t port, uint32 value);
soc_error_t soc_pm_eee_tx_wake_time_get(int unit, soc_port_t port, uint32 *value);
soc_error_t soc_pm_eee_tx_wake_time_set(int unit, soc_port_t port, uint32 value);
soc_error_t soc_pm_eee_link_active_duration_get(int unit, soc_port_t port, uint32* value);
soc_error_t soc_pm_eee_link_active_duration_set(int unit, soc_port_t port, uint32 value);


soc_error_t soc_pm_link_status_get(int unit, soc_port_t port, int *is_link_up, int *is_latch_down);
soc_error_t soc_pm_link_status_clear(int unit, soc_port_t port);


#endif 

