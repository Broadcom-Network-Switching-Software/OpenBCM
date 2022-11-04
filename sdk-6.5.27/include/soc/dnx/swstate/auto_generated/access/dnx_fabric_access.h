
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FABRIC_ACCESS_H__
#define __DNX_FABRIC_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/dnx_fabric_types.h>
#include <include/bcm/port.h>
#include <include/shared/shr_thread_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>


typedef int (*dnx_fabric_db_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*dnx_fabric_db_init_cb)(
    int unit);

typedef int (*dnx_fabric_db_modids_to_group_map_alloc_cb)(
    int unit);

typedef int (*dnx_fabric_db_modids_to_group_map_nof_modids_set_cb)(
    int unit, uint32 modids_to_group_map_idx_0, int nof_modids);

typedef int (*dnx_fabric_db_modids_to_group_map_nof_modids_get_cb)(
    int unit, uint32 modids_to_group_map_idx_0, int *nof_modids);

typedef int (*dnx_fabric_db_modids_to_group_map_modids_set_cb)(
    int unit, uint32 modids_to_group_map_idx_0, uint32 modids_idx_0, int modids);

typedef int (*dnx_fabric_db_modids_to_group_map_modids_get_cb)(
    int unit, uint32 modids_to_group_map_idx_0, uint32 modids_idx_0, int *modids);

typedef int (*dnx_fabric_db_modids_to_group_map_modids_alloc_cb)(
    int unit, uint32 modids_to_group_map_idx_0);

typedef int (*dnx_fabric_db_links_alloc_cb)(
    int unit);

typedef int (*dnx_fabric_db_links_is_link_allowed_set_cb)(
    int unit, uint32 links_idx_0, int is_link_allowed);

typedef int (*dnx_fabric_db_links_is_link_allowed_get_cb)(
    int unit, uint32 links_idx_0, int *is_link_allowed);

typedef int (*dnx_fabric_db_links_is_link_isolated_set_cb)(
    int unit, uint32 links_idx_0, int is_link_isolated);

typedef int (*dnx_fabric_db_links_is_link_isolated_get_cb)(
    int unit, uint32 links_idx_0, int *is_link_isolated);

typedef int (*dnx_fabric_db_load_balancing_shr_thread_manager_handler_set_cb)(
    int unit, shr_thread_manager_handler_t load_balancing_shr_thread_manager_handler);

typedef int (*dnx_fabric_db_load_balancing_shr_thread_manager_handler_get_cb)(
    int unit, shr_thread_manager_handler_t *load_balancing_shr_thread_manager_handler);

typedef int (*dnx_fabric_db_system_upgrade_state_0_set_cb)(
    int unit, uint32 state_0);

typedef int (*dnx_fabric_db_system_upgrade_state_0_get_cb)(
    int unit, uint32 *state_0);

typedef int (*dnx_fabric_db_system_upgrade_state_1_set_cb)(
    int unit, uint32 state_1);

typedef int (*dnx_fabric_db_system_upgrade_state_1_get_cb)(
    int unit, uint32 *state_1);

typedef int (*dnx_fabric_db_system_upgrade_state_2_set_cb)(
    int unit, uint32 state_2);

typedef int (*dnx_fabric_db_system_upgrade_state_2_get_cb)(
    int unit, uint32 *state_2);

typedef int (*dnx_fabric_db_cable_swap_is_enable_set_cb)(
    int unit, uint32 is_enable_idx_0, int is_enable);

typedef int (*dnx_fabric_db_cable_swap_is_enable_get_cb)(
    int unit, uint32 is_enable_idx_0, int *is_enable);

typedef int (*dnx_fabric_db_cable_swap_is_enable_alloc_cb)(
    int unit);

typedef int (*dnx_fabric_db_cable_swap_is_master_set_cb)(
    int unit, uint32 is_master_idx_0, int is_master);

typedef int (*dnx_fabric_db_cable_swap_is_master_get_cb)(
    int unit, uint32 is_master_idx_0, int *is_master);

typedef int (*dnx_fabric_db_cable_swap_is_master_alloc_cb)(
    int unit);

typedef int (*dnx_fabric_db_signal_quality_lane_state_set_cb)(
    int unit, uint32 lane_state_idx_0, CONST dnx_fabric_signal_quality_lane_state_t *lane_state);

typedef int (*dnx_fabric_db_signal_quality_lane_state_get_cb)(
    int unit, uint32 lane_state_idx_0, dnx_fabric_signal_quality_lane_state_t *lane_state);

typedef int (*dnx_fabric_db_signal_quality_lane_state_alloc_cb)(
    int unit);

typedef int (*dnx_fabric_db_signal_quality_lane_state_prev_error_rate_set_cb)(
    int unit, uint32 lane_state_idx_0, uint32 prev_error_rate);

typedef int (*dnx_fabric_db_signal_quality_lane_state_prev_error_rate_get_cb)(
    int unit, uint32 lane_state_idx_0, uint32 *prev_error_rate);

typedef int (*dnx_fabric_db_signal_quality_lane_state_prev_cnt_set_cb)(
    int unit, uint32 lane_state_idx_0, uint64 prev_cnt);

typedef int (*dnx_fabric_db_signal_quality_lane_state_prev_cnt_get_cb)(
    int unit, uint32 lane_state_idx_0, uint64 *prev_cnt);

typedef int (*dnx_fabric_db_signal_quality_lane_state_prev_uncorrectable_cnt_set_cb)(
    int unit, uint32 lane_state_idx_0, uint64 prev_uncorrectable_cnt);

typedef int (*dnx_fabric_db_signal_quality_lane_state_prev_uncorrectable_cnt_get_cb)(
    int unit, uint32 lane_state_idx_0, uint64 *prev_uncorrectable_cnt);

typedef int (*dnx_fabric_db_mesh_topology_alloc_cb)(
    int unit);

typedef int (*dnx_fabric_db_mesh_topology_links_set_cb)(
    int unit, uint32 mesh_topology_idx_0, uint32 links_idx_0, int links);

typedef int (*dnx_fabric_db_mesh_topology_links_get_cb)(
    int unit, uint32 mesh_topology_idx_0, uint32 links_idx_0, int *links);

typedef int (*dnx_fabric_db_mesh_topology_links_alloc_cb)(
    int unit, uint32 mesh_topology_idx_0);



typedef struct {
    dnx_fabric_db_modids_to_group_map_nof_modids_set_cb set;
    dnx_fabric_db_modids_to_group_map_nof_modids_get_cb get;
} dnx_fabric_db_modids_to_group_map_nof_modids_cbs;

typedef struct {
    dnx_fabric_db_modids_to_group_map_modids_set_cb set;
    dnx_fabric_db_modids_to_group_map_modids_get_cb get;
    dnx_fabric_db_modids_to_group_map_modids_alloc_cb alloc;
} dnx_fabric_db_modids_to_group_map_modids_cbs;

typedef struct {
    dnx_fabric_db_modids_to_group_map_alloc_cb alloc;
    dnx_fabric_db_modids_to_group_map_nof_modids_cbs nof_modids;
    dnx_fabric_db_modids_to_group_map_modids_cbs modids;
} dnx_fabric_db_modids_to_group_map_cbs;

typedef struct {
    dnx_fabric_db_links_is_link_allowed_set_cb set;
    dnx_fabric_db_links_is_link_allowed_get_cb get;
} dnx_fabric_db_links_is_link_allowed_cbs;

typedef struct {
    dnx_fabric_db_links_is_link_isolated_set_cb set;
    dnx_fabric_db_links_is_link_isolated_get_cb get;
} dnx_fabric_db_links_is_link_isolated_cbs;

typedef struct {
    dnx_fabric_db_links_alloc_cb alloc;
    dnx_fabric_db_links_is_link_allowed_cbs is_link_allowed;
    dnx_fabric_db_links_is_link_isolated_cbs is_link_isolated;
} dnx_fabric_db_links_cbs;

typedef struct {
    dnx_fabric_db_load_balancing_shr_thread_manager_handler_set_cb set;
    dnx_fabric_db_load_balancing_shr_thread_manager_handler_get_cb get;
} dnx_fabric_db_load_balancing_shr_thread_manager_handler_cbs;

typedef struct {
    dnx_fabric_db_system_upgrade_state_0_set_cb set;
    dnx_fabric_db_system_upgrade_state_0_get_cb get;
} dnx_fabric_db_system_upgrade_state_0_cbs;

typedef struct {
    dnx_fabric_db_system_upgrade_state_1_set_cb set;
    dnx_fabric_db_system_upgrade_state_1_get_cb get;
} dnx_fabric_db_system_upgrade_state_1_cbs;

typedef struct {
    dnx_fabric_db_system_upgrade_state_2_set_cb set;
    dnx_fabric_db_system_upgrade_state_2_get_cb get;
} dnx_fabric_db_system_upgrade_state_2_cbs;

typedef struct {
    dnx_fabric_db_system_upgrade_state_0_cbs state_0;
    dnx_fabric_db_system_upgrade_state_1_cbs state_1;
    dnx_fabric_db_system_upgrade_state_2_cbs state_2;
} dnx_fabric_db_system_upgrade_cbs;

typedef struct {
    dnx_fabric_db_cable_swap_is_enable_set_cb set;
    dnx_fabric_db_cable_swap_is_enable_get_cb get;
    dnx_fabric_db_cable_swap_is_enable_alloc_cb alloc;
} dnx_fabric_db_cable_swap_is_enable_cbs;

typedef struct {
    dnx_fabric_db_cable_swap_is_master_set_cb set;
    dnx_fabric_db_cable_swap_is_master_get_cb get;
    dnx_fabric_db_cable_swap_is_master_alloc_cb alloc;
} dnx_fabric_db_cable_swap_is_master_cbs;

typedef struct {
    dnx_fabric_db_cable_swap_is_enable_cbs is_enable;
    dnx_fabric_db_cable_swap_is_master_cbs is_master;
} dnx_fabric_db_cable_swap_cbs;

typedef struct {
    dnx_fabric_db_signal_quality_lane_state_prev_error_rate_set_cb set;
    dnx_fabric_db_signal_quality_lane_state_prev_error_rate_get_cb get;
} dnx_fabric_db_signal_quality_lane_state_prev_error_rate_cbs;

typedef struct {
    dnx_fabric_db_signal_quality_lane_state_prev_cnt_set_cb set;
    dnx_fabric_db_signal_quality_lane_state_prev_cnt_get_cb get;
} dnx_fabric_db_signal_quality_lane_state_prev_cnt_cbs;

typedef struct {
    dnx_fabric_db_signal_quality_lane_state_prev_uncorrectable_cnt_set_cb set;
    dnx_fabric_db_signal_quality_lane_state_prev_uncorrectable_cnt_get_cb get;
} dnx_fabric_db_signal_quality_lane_state_prev_uncorrectable_cnt_cbs;

typedef struct {
    dnx_fabric_db_signal_quality_lane_state_set_cb set;
    dnx_fabric_db_signal_quality_lane_state_get_cb get;
    dnx_fabric_db_signal_quality_lane_state_alloc_cb alloc;
    dnx_fabric_db_signal_quality_lane_state_prev_error_rate_cbs prev_error_rate;
    dnx_fabric_db_signal_quality_lane_state_prev_cnt_cbs prev_cnt;
    dnx_fabric_db_signal_quality_lane_state_prev_uncorrectable_cnt_cbs prev_uncorrectable_cnt;
} dnx_fabric_db_signal_quality_lane_state_cbs;

typedef struct {
    dnx_fabric_db_signal_quality_lane_state_cbs lane_state;
} dnx_fabric_db_signal_quality_cbs;

typedef struct {
    dnx_fabric_db_mesh_topology_links_set_cb set;
    dnx_fabric_db_mesh_topology_links_get_cb get;
    dnx_fabric_db_mesh_topology_links_alloc_cb alloc;
} dnx_fabric_db_mesh_topology_links_cbs;

typedef struct {
    dnx_fabric_db_mesh_topology_alloc_cb alloc;
    dnx_fabric_db_mesh_topology_links_cbs links;
} dnx_fabric_db_mesh_topology_cbs;

typedef struct {
    dnx_fabric_db_is_init_cb is_init;
    dnx_fabric_db_init_cb init;
    dnx_fabric_db_modids_to_group_map_cbs modids_to_group_map;
    dnx_fabric_db_links_cbs links;
    dnx_fabric_db_load_balancing_shr_thread_manager_handler_cbs load_balancing_shr_thread_manager_handler;
    dnx_fabric_db_system_upgrade_cbs system_upgrade;
    dnx_fabric_db_cable_swap_cbs cable_swap;
    dnx_fabric_db_signal_quality_cbs signal_quality;
    dnx_fabric_db_mesh_topology_cbs mesh_topology;
} dnx_fabric_db_cbs;





extern dnx_fabric_db_cbs dnx_fabric_db;

#endif 
