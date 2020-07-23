
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNXF_DIAGNOSTIC_H__
#define __DNXF_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnxf/swstate/auto_generated/types/dnxf_types.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/types.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNXF_STATE_INFO,
    DNXF_STATE_MODID_INFO,
    DNXF_STATE_MODID_LOCAL_MAP_INFO,
    DNXF_STATE_MODID_LOCAL_MAP_VALID_INFO,
    DNXF_STATE_MODID_LOCAL_MAP_MODULE_ID_INFO,
    DNXF_STATE_MODID_GROUP_MAP_INFO,
    DNXF_STATE_MODID_GROUP_MAP_RAW_INFO,
    DNXF_STATE_MODID_MODID_TO_GROUP_MAP_INFO,
    DNXF_STATE_MC_INFO,
    DNXF_STATE_MC_MODE_INFO,
    DNXF_STATE_MC_ID_MAP_INFO,
    DNXF_STATE_MC_NOF_MC_GROUPS_CREATED_INFO,
    DNXF_STATE_LB_INFO,
    DNXF_STATE_LB_GROUP_TO_FIRST_LINK_INFO,
    DNXF_STATE_PORT_INFO,
    DNXF_STATE_PORT_CL72_CONF_INFO,
    DNXF_STATE_PORT_PHY_LANE_CONFIG_INFO,
    DNXF_STATE_PORT_IS_CONNECTED_TO_REPEATER_INFO,
    DNXF_STATE_PORT_ISOLATION_STATUS_STORED_INFO,
    DNXF_STATE_PORT_ORIG_ISOLATED_STATUS_INFO,
    DNXF_STATE_INTR_INFO,
    DNXF_STATE_INTR_FLAGS_INFO,
    DNXF_STATE_INTR_STORM_TIMED_COUNT_INFO,
    DNXF_STATE_INTR_STORM_TIMED_PERIOD_INFO,
    DNXF_STATE_INTR_STORM_NOMINAL_INFO,
    DNXF_STATE_LANE_MAP_DB_INFO,
    DNXF_STATE_LANE_MAP_DB_MAP_SIZE_INFO,
    DNXF_STATE_LANE_MAP_DB_LANE2SERDES_INFO,
    DNXF_STATE_LANE_MAP_DB_LANE2SERDES_RX_ID_INFO,
    DNXF_STATE_LANE_MAP_DB_LANE2SERDES_TX_ID_INFO,
    DNXF_STATE_LANE_MAP_DB_SERDES2LANE_INFO,
    DNXF_STATE_LANE_MAP_DB_SERDES2LANE_RX_ID_INFO,
    DNXF_STATE_LANE_MAP_DB_SERDES2LANE_TX_ID_INFO,
    DNXF_STATE_SYNCE_INFO,
    DNXF_STATE_SYNCE_MASTER_SYNCE_ENABLED_INFO,
    DNXF_STATE_SYNCE_SLAVE_SYNCE_ENABLED_INFO,
    DNXF_STATE_INFO_NOF_ENTRIES
} sw_state_dnxf_state_layout_e;


extern dnx_sw_state_diagnostic_info_t dnxf_state_info[SOC_MAX_NUM_DEVICES][DNXF_STATE_INFO_NOF_ENTRIES];

extern const char* dnxf_state_layout_str[DNXF_STATE_INFO_NOF_ENTRIES];
int dnxf_state_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_modid_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_modid_local_map_dump(
    int unit, int local_map_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_modid_local_map_valid_dump(
    int unit, int local_map_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_modid_local_map_module_id_dump(
    int unit, int local_map_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_modid_group_map_dump(
    int unit, int group_map_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_modid_group_map_raw_dump(
    int unit, int group_map_idx_0, int raw_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_modid_modid_to_group_map_dump(
    int unit, int modid_to_group_map_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_mc_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_mc_mode_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_mc_id_map_dump(
    int unit, int id_map_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_mc_nof_mc_groups_created_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_lb_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_lb_group_to_first_link_dump(
    int unit, int group_to_first_link_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_port_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_port_cl72_conf_dump(
    int unit, int cl72_conf_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_port_phy_lane_config_dump(
    int unit, int phy_lane_config_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_port_is_connected_to_repeater_dump(
    int unit, int is_connected_to_repeater_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_port_isolation_status_stored_dump(
    int unit, int isolation_status_stored_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_port_orig_isolated_status_dump(
    int unit, int orig_isolated_status_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_intr_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_intr_flags_dump(
    int unit, int flags_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_intr_storm_timed_count_dump(
    int unit, int storm_timed_count_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_intr_storm_timed_period_dump(
    int unit, int storm_timed_period_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_intr_storm_nominal_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_lane_map_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_lane_map_db_map_size_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_lane_map_db_lane2serdes_dump(
    int unit, int lane2serdes_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_lane_map_db_lane2serdes_rx_id_dump(
    int unit, int lane2serdes_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_lane_map_db_lane2serdes_tx_id_dump(
    int unit, int lane2serdes_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_lane_map_db_serdes2lane_dump(
    int unit, int serdes2lane_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_lane_map_db_serdes2lane_rx_id_dump(
    int unit, int serdes2lane_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_lane_map_db_serdes2lane_tx_id_dump(
    int unit, int serdes2lane_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_synce_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_synce_master_synce_enabled_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_synce_slave_synce_enabled_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
