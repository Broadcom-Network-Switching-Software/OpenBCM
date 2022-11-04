
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FABRIC_DIAGNOSTIC_H__
#define __DNX_FABRIC_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_fabric_types.h>
#include <include/bcm/port.h>
#include <include/shared/shr_thread_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_FABRIC_DB_INFO,
    DNX_FABRIC_DB_MODIDS_TO_GROUP_MAP_INFO,
    DNX_FABRIC_DB_MODIDS_TO_GROUP_MAP_NOF_MODIDS_INFO,
    DNX_FABRIC_DB_MODIDS_TO_GROUP_MAP_MODIDS_INFO,
    DNX_FABRIC_DB_LINKS_INFO,
    DNX_FABRIC_DB_LINKS_IS_LINK_ALLOWED_INFO,
    DNX_FABRIC_DB_LINKS_IS_LINK_ISOLATED_INFO,
    DNX_FABRIC_DB_LOAD_BALANCING_SHR_THREAD_MANAGER_HANDLER_INFO,
    DNX_FABRIC_DB_SYSTEM_UPGRADE_INFO,
    DNX_FABRIC_DB_SYSTEM_UPGRADE_STATE_0_INFO,
    DNX_FABRIC_DB_SYSTEM_UPGRADE_STATE_1_INFO,
    DNX_FABRIC_DB_SYSTEM_UPGRADE_STATE_2_INFO,
    DNX_FABRIC_DB_CABLE_SWAP_INFO,
    DNX_FABRIC_DB_CABLE_SWAP_IS_ENABLE_INFO,
    DNX_FABRIC_DB_CABLE_SWAP_IS_MASTER_INFO,
    DNX_FABRIC_DB_SIGNAL_QUALITY_INFO,
    DNX_FABRIC_DB_SIGNAL_QUALITY_LANE_STATE_INFO,
    DNX_FABRIC_DB_SIGNAL_QUALITY_LANE_STATE_PREV_ERROR_RATE_INFO,
    DNX_FABRIC_DB_SIGNAL_QUALITY_LANE_STATE_PREV_CNT_INFO,
    DNX_FABRIC_DB_SIGNAL_QUALITY_LANE_STATE_PREV_UNCORRECTABLE_CNT_INFO,
    DNX_FABRIC_DB_MESH_TOPOLOGY_INFO,
    DNX_FABRIC_DB_MESH_TOPOLOGY_LINKS_INFO,
    DNX_FABRIC_DB_INFO_NOF_ENTRIES
} sw_state_dnx_fabric_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_fabric_db_info[SOC_MAX_NUM_DEVICES][DNX_FABRIC_DB_INFO_NOF_ENTRIES];

extern const char* dnx_fabric_db_layout_str[DNX_FABRIC_DB_INFO_NOF_ENTRIES];
int dnx_fabric_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_modids_to_group_map_dump(
    int unit, int modids_to_group_map_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_modids_to_group_map_nof_modids_dump(
    int unit, int modids_to_group_map_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_modids_to_group_map_modids_dump(
    int unit, int modids_to_group_map_idx_0, int modids_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_links_dump(
    int unit, int links_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_links_is_link_allowed_dump(
    int unit, int links_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_links_is_link_isolated_dump(
    int unit, int links_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_load_balancing_shr_thread_manager_handler_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_system_upgrade_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_system_upgrade_state_0_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_system_upgrade_state_1_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_system_upgrade_state_2_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_cable_swap_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_cable_swap_is_enable_dump(
    int unit, int is_enable_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_cable_swap_is_master_dump(
    int unit, int is_master_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_signal_quality_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_signal_quality_lane_state_dump(
    int unit, int lane_state_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_signal_quality_lane_state_prev_error_rate_dump(
    int unit, int lane_state_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_signal_quality_lane_state_prev_cnt_dump(
    int unit, int lane_state_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_signal_quality_lane_state_prev_uncorrectable_cnt_dump(
    int unit, int lane_state_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_mesh_topology_dump(
    int unit, int mesh_topology_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fabric_db_mesh_topology_links_dump(
    int unit, int mesh_topology_idx_0, int links_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
