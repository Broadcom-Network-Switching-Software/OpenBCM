
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_DNXF_LANE_MAP_DB_H_
#define _SOC_DNXF_LANE_MAP_DB_H_

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF generation only!"
#endif

#include <soc/dnxc/error.h>
#include <bcm/port.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_types.h>

#define SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED (-1)

shr_error_e soc_dnxf_lane_map_db_map_input_check(
    int unit,
    int map_size,
    soc_dnxc_lane_map_db_map_t * lane2serdes);

shr_error_e soc_dnxf_lane_map_db_link_to_fmac_lane_get(
    int unit,
    bcm_port_t link,
    int *fmac_lane);

shr_error_e soc_dnxf_lane_map_db_fmac_lane_to_link_get(
    int unit,
    int fmac_lane,
    bcm_port_t * link);

shr_error_e soc_dnxf_lane_map_db_map_size_align_get(
    int unit,
    int map_size,
    int *map_size_aligned);

#endif
