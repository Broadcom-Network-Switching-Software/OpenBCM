
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_RAMON_LANE_MAP_DB_H_
#define _SOC_RAMON_LANE_MAP_DB_H_

#ifndef BCM_DNXF1_SUPPORT
#error "This file is for use by DNXF1 generation only!"
#endif

#include <soc/dnxc/error.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_types.h>

shr_error_e soc_ramon_lane_map_db_map_set(
    int unit,
    int map_size,
    soc_dnxc_lane_map_db_map_t * lane2serdes,
    int is_init_sequence);

shr_error_e soc_ramon_lane_map_db_map_get(
    int unit,
    int map_size,
    soc_dnxc_lane_map_db_map_t * lane2serdes);

shr_error_e soc_ramon_lane_map_db_pm_map_get(
    int unit,
    int pm_index,
    int nof_ports,
    soc_dnxc_lane_map_db_map_t * lane2serdes);

shr_error_e soc_ramon_lane_map_db_is_lane_mapped_get(
    int unit,
    int lane,
    int *is_lane_mapped);

#endif
