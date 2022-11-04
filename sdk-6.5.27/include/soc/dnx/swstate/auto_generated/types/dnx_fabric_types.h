
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FABRIC_TYPES_H__
#define __DNX_FABRIC_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <include/bcm/port.h>
#include <include/shared/shr_thread_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>


typedef sw_state_user_def_t* dnx_fabric_signal_quality_callback_userdata_t;



typedef struct {
    int nof_modids;
    int* modids;
} dnx_fabric_modids_group_map_t;

typedef struct {
    int is_link_allowed;
    int is_link_isolated;
} dnx_fabric_link_t;

typedef struct {
    uint32 state_0;
    uint32 state_1;
    uint32 state_2;
} dnx_fabric_system_upgrade_t;

typedef struct {
    int* is_enable;
    int* is_master;
} dnx_fabric_cable_swap_t;

typedef struct {
    uint32 degrade_thr;
    uint32 degrade_cu_thr;
    uint32 fail_thr;
    uint32 fail_cu_thr;
    uint32 interval;
} dnx_fabric_signal_quality_fmac_config_t;

typedef struct {
    uint32 prev_error_rate;
    uint64 prev_cnt;
    uint64 prev_uncorrectable_cnt;
} dnx_fabric_signal_quality_lane_state_t;

typedef struct {
    bcm_port_signal_quality_callback_t callback;
    dnx_fabric_signal_quality_callback_userdata_t userdata;
} dnx_fabric_signal_quality_reset_t;

typedef struct {
    dnx_fabric_signal_quality_lane_state_t* lane_state;
} dnx_fabric_signal_quality_db_fabric_t;

typedef struct {
    int* links;
} dnx_fabric_mesh_topology_t;

typedef struct {
    dnx_fabric_modids_group_map_t* modids_to_group_map;
    dnx_fabric_link_t* links;
    shr_thread_manager_handler_t load_balancing_shr_thread_manager_handler;
    dnx_fabric_system_upgrade_t system_upgrade;
    dnx_fabric_cable_swap_t cable_swap;
    dnx_fabric_signal_quality_db_fabric_t signal_quality;
    dnx_fabric_mesh_topology_t* mesh_topology;
} dnx_fabric_db_t;


#endif 
