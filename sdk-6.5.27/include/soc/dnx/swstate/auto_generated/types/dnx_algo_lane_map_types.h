
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_ALGO_LANE_MAP_TYPES_H__
#define __DNX_ALGO_LANE_MAP_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <soc/dnxc/swstate/types/sw_state_pbmp.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_types.h>


typedef struct {
    uint16 logical_otn_lane;
    uint16 nof_lanes;
} dnx_algo_lane_map_db_otn_logical_lane_map_t;

typedef struct {
    bcm_pbmp_t logical_lanes;
    dnx_algo_lane_map_db_otn_logical_lane_map_t* logical_lane_map;
} dnx_algo_lane_map_db_otn_t;

typedef struct {
    soc_dnxc_lane_map_db_t lane_map[DNX_ALGO_NOF_LANE_MAP_TYPES];
    dnx_algo_lane_map_db_otn_t otn;
} dnx_algo_lane_map_db_t;


#endif 
