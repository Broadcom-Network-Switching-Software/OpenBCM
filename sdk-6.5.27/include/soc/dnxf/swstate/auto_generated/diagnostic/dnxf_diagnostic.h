
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNXF_DIAGNOSTIC_H__
#define __DNXF_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxf/swstate/auto_generated/types/dnxf_types.h>
#include <include/bcm/port.h>
#include <include/bcm/switch.h>
#include <include/shared/fabric.h>
#include <include/shared/pkt.h>
#include <include/soc/dnxc/dnxc_fabric_source_routed_cell.h>
#include <include/soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_pbmp.h>
#include <soc/types.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNXF_STATE_INFO,
    DNXF_STATE_INFO_INFO,
    DNXF_STATE_INFO_ACTIVE_CORE_BMP_INFO,
    DNXF_STATE_INFO_VALID_FSRD_BMP_INFO,
    DNXF_STATE_INFO_VALID_DCH_DCML_CORE_BMP_INFO,
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
    DNXF_STATE_PORT_SPEED_INFO,
    DNXF_STATE_PORT_FEC_TYPE_INFO,
    DNXF_STATE_PORT_IS_CONNECTED_TO_REPEATER_INFO,
    DNXF_STATE_PORT_ISOLATION_STATUS_STORED_INFO,
    DNXF_STATE_PORT_ORIG_ISOLATED_STATUS_INFO,
    DNXF_STATE_CABLE_SWAP_INFO,
    DNXF_STATE_CABLE_SWAP_IS_ENABLE_INFO,
    DNXF_STATE_CABLE_SWAP_IS_MASTER_INFO,
    DNXF_STATE_INTR_INFO,
    DNXF_STATE_INTR_FLAGS_INFO,
    DNXF_STATE_INTR_STORM_TIMED_COUNT_INFO,
    DNXF_STATE_INTR_STORM_TIMED_PERIOD_INFO,
    DNXF_STATE_INTR_STORM_NOMINAL_INFO,
    DNXF_STATE_LANE_MAP_DB_INFO,
    DNXF_STATE_LANE_MAP_DB_SERDES_MAP_INFO,
    DNXF_STATE_LANE_MAP_DB_SERDES_MAP_MAP_SIZE_INFO,
    DNXF_STATE_LANE_MAP_DB_SERDES_MAP_LANE2SERDES_INFO,
    DNXF_STATE_LANE_MAP_DB_SERDES_MAP_LANE2SERDES_RX_ID_INFO,
    DNXF_STATE_LANE_MAP_DB_SERDES_MAP_LANE2SERDES_TX_ID_INFO,
    DNXF_STATE_LANE_MAP_DB_SERDES_MAP_SERDES2LANE_INFO,
    DNXF_STATE_LANE_MAP_DB_SERDES_MAP_SERDES2LANE_RX_ID_INFO,
    DNXF_STATE_LANE_MAP_DB_SERDES_MAP_SERDES2LANE_TX_ID_INFO,
    DNXF_STATE_LANE_MAP_DB_LINK_TO_FMAC_LANE_MAP_INFO,
    DNXF_STATE_SYNCE_INFO,
    DNXF_STATE_SYNCE_MASTER_SYNCE_ENABLED_INFO,
    DNXF_STATE_SYNCE_SLAVE_SYNCE_ENABLED_INFO,
    DNXF_STATE_SYNCE_MASTER_SET1_SYNCE_ENABLED_INFO,
    DNXF_STATE_SYNCE_SLAVE_SET1_SYNCE_ENABLED_INFO,
    DNXF_STATE_SYSTEM_UPGRADE_INFO,
    DNXF_STATE_SYSTEM_UPGRADE_STATE_0_INFO,
    DNXF_STATE_SYSTEM_UPGRADE_STATE_1_INFO,
    DNXF_STATE_SYSTEM_UPGRADE_STATE_2_INFO,
    DNXF_STATE_HARD_RESET_INFO,
    DNXF_STATE_HARD_RESET_CALLBACK_INFO,
    DNXF_STATE_HARD_RESET_USERDATA_INFO,
    DNXF_STATE_MIB_INFO,
    DNXF_STATE_MIB_COUNTER_PBMP_INFO,
    DNXF_STATE_MIB_INTERVAL_INFO,
    DNXF_STATE_SIGNAL_QUALITY_INTERRUPT_CALLBACK_INFO,
    DNXF_STATE_SIGNAL_QUALITY_INTERRUPT_USERDATA_INFO,
    DNXF_STATE_SIGNAL_QUALITY_FMAC_CONFIG_DEGRADE_THR_INFO,
    DNXF_STATE_SIGNAL_QUALITY_FMAC_CONFIG_DEGRADE_CU_THR_INFO,
    DNXF_STATE_SIGNAL_QUALITY_FMAC_CONFIG_FAIL_THR_INFO,
    DNXF_STATE_SIGNAL_QUALITY_FMAC_CONFIG_FAIL_CU_THR_INFO,
    DNXF_STATE_SIGNAL_QUALITY_FMAC_CONFIG_INTERVAL_INFO,
    DNXF_STATE_IP_CONFIG_SELF_ROUTED_CONFIG_DEST_MODID_INFO,
    DNXF_STATE_IP_CONFIG_SELF_ROUTED_CONFIG_DEST_SYSPORT_INFO,
    DNXF_STATE_IP_CONFIG_SOURCE_ROUTED_CONFIG_PATH_LINK_INFO,
    DNXF_STATE_IP_CONFIG_SOURCE_ROUTED_CONFIG_DEST_DEVICE_TYPE_INFO,
    DNXF_STATE_INFO_NOF_ENTRIES
} sw_state_dnxf_state_layout_e;


extern dnx_sw_state_diagnostic_info_t dnxf_state_info[SOC_MAX_NUM_DEVICES][DNXF_STATE_INFO_NOF_ENTRIES];

extern const char* dnxf_state_layout_str[DNXF_STATE_INFO_NOF_ENTRIES];
int dnxf_state_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_info_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_info_active_core_bmp_dump(
    int unit, int active_core_bmp_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_info_valid_fsrd_bmp_dump(
    int unit, int valid_fsrd_bmp_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_info_valid_dch_dcml_core_bmp_dump(
    int unit, int valid_dch_dcml_core_bmp_idx_0, dnx_sw_state_dump_filters_t filters);

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
    int unit, int group_to_first_link_idx_0, int group_to_first_link_idx_1, dnx_sw_state_dump_filters_t filters);

int dnxf_state_port_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_port_cl72_conf_dump(
    int unit, int cl72_conf_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_port_phy_lane_config_dump(
    int unit, int phy_lane_config_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_port_speed_dump(
    int unit, int speed_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_port_fec_type_dump(
    int unit, int fec_type_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_port_is_connected_to_repeater_dump(
    int unit, int is_connected_to_repeater_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_port_isolation_status_stored_dump(
    int unit, int isolation_status_stored_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_port_orig_isolated_status_dump(
    int unit, int orig_isolated_status_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_cable_swap_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_cable_swap_is_enable_dump(
    int unit, int is_enable_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_cable_swap_is_master_dump(
    int unit, int is_master_idx_0, dnx_sw_state_dump_filters_t filters);

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

int dnxf_state_lane_map_db_serdes_map_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_lane_map_db_serdes_map_map_size_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_lane_map_db_serdes_map_lane2serdes_dump(
    int unit, int lane2serdes_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_lane_map_db_serdes_map_lane2serdes_rx_id_dump(
    int unit, int lane2serdes_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_lane_map_db_serdes_map_lane2serdes_tx_id_dump(
    int unit, int lane2serdes_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_lane_map_db_serdes_map_serdes2lane_dump(
    int unit, int serdes2lane_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_lane_map_db_serdes_map_serdes2lane_rx_id_dump(
    int unit, int serdes2lane_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_lane_map_db_serdes_map_serdes2lane_tx_id_dump(
    int unit, int serdes2lane_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_lane_map_db_link_to_fmac_lane_map_dump(
    int unit, int link_to_fmac_lane_map_idx_0, dnx_sw_state_dump_filters_t filters);

int dnxf_state_synce_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_synce_master_synce_enabled_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_synce_slave_synce_enabled_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_synce_master_set1_synce_enabled_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_synce_slave_set1_synce_enabled_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_system_upgrade_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_system_upgrade_state_0_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_system_upgrade_state_1_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_system_upgrade_state_2_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_hard_reset_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_hard_reset_callback_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_hard_reset_userdata_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_mib_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_mib_counter_pbmp_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnxf_state_mib_interval_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
