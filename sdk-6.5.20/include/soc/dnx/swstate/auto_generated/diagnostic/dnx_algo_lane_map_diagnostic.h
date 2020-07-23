
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_ALGO_LANE_MAP_DIAGNOSTIC_H__
#define __DNX_ALGO_LANE_MAP_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_algo_lane_map_types.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_ALGO_LANE_MAP_DB_INFO,
    DNX_ALGO_LANE_MAP_DB_LANE_MAP_INFO,
    DNX_ALGO_LANE_MAP_DB_LANE_MAP_MAP_SIZE_INFO,
    DNX_ALGO_LANE_MAP_DB_LANE_MAP_LANE2SERDES_INFO,
    DNX_ALGO_LANE_MAP_DB_LANE_MAP_LANE2SERDES_RX_ID_INFO,
    DNX_ALGO_LANE_MAP_DB_LANE_MAP_LANE2SERDES_TX_ID_INFO,
    DNX_ALGO_LANE_MAP_DB_LANE_MAP_SERDES2LANE_INFO,
    DNX_ALGO_LANE_MAP_DB_LANE_MAP_SERDES2LANE_RX_ID_INFO,
    DNX_ALGO_LANE_MAP_DB_LANE_MAP_SERDES2LANE_TX_ID_INFO,
    DNX_ALGO_LANE_MAP_DB_INFO_NOF_ENTRIES
} sw_state_dnx_algo_lane_map_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_algo_lane_map_db_info[SOC_MAX_NUM_DEVICES][DNX_ALGO_LANE_MAP_DB_INFO_NOF_ENTRIES];

extern const char* dnx_algo_lane_map_db_layout_str[DNX_ALGO_LANE_MAP_DB_INFO_NOF_ENTRIES];
int dnx_algo_lane_map_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_algo_lane_map_db_lane_map_dump(
    int unit, int lane_map_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_lane_map_db_lane_map_map_size_dump(
    int unit, int lane_map_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_lane_map_db_lane_map_lane2serdes_dump(
    int unit, int lane_map_idx_0, int lane2serdes_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_lane_map_db_lane_map_lane2serdes_rx_id_dump(
    int unit, int lane_map_idx_0, int lane2serdes_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_lane_map_db_lane_map_lane2serdes_tx_id_dump(
    int unit, int lane_map_idx_0, int lane2serdes_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_lane_map_db_lane_map_serdes2lane_dump(
    int unit, int lane_map_idx_0, int serdes2lane_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_lane_map_db_lane_map_serdes2lane_rx_id_dump(
    int unit, int lane_map_idx_0, int serdes2lane_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_lane_map_db_lane_map_serdes2lane_tx_id_dump(
    int unit, int lane_map_idx_0, int serdes2lane_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
