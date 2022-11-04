
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNXC_TYPES_H__
#define __DNXC_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/types/sw_state_pbmp.h>


typedef enum {
    soc_dnxc_isolation_status_active = 0,
    soc_dnxc_isolation_status_isolated = 1,
    
    SOC_DNXC_ISOLATION_STATUS_T_DUMMY
} soc_dnxc_isolation_status_t;



typedef struct {
    int rx_id;
    int tx_id;
} soc_dnxc_lane_map_db_map_t;

typedef struct {
    int map_size;
    soc_dnxc_lane_map_db_map_t* lane2serdes;
    soc_dnxc_lane_map_db_map_t* serdes2lane;
} soc_dnxc_lane_map_db_t;

typedef struct {
    int master_synce_enabled;
    int slave_synce_enabled;
    int master_set1_synce_enabled;
    int slave_set1_synce_enabled;
} soc_dnxc_synce_status_t;

typedef struct {
    bcm_pbmp_t counter_pbmp;
    int interval;
} soc_dnxc_mib_db_t;


#endif 
