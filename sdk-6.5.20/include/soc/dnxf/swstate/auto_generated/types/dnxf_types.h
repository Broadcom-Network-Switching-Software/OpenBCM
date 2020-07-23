
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNXF_TYPES_H__
#define __DNXF_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/types.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_types.h>


#define SOC_DNXF_MODID_NOF 2048

#define SOC_DNXF_MODID_NOF_UINT32_SIZE (SOC_DNXF_MODID_NOF/32)



typedef struct {
    
    uint32 raw[SOC_DNXF_MODID_NOF_UINT32_SIZE];
} soc_dnxf_modid_group_map_t;

typedef struct {
    
    uint32 valid;
    
    uint32 module_id;
} soc_dnxf_modid_local_map_t;

typedef struct {
    
    soc_dnxf_modid_local_map_t* local_map;
    
    soc_dnxf_modid_group_map_t* group_map;
    
    soc_module_t modid_to_group_map[SOC_DNXF_MODID_NOF];
} soc_dnxf_modid_t;

typedef struct {
    
    uint32 mode;
    
    uint32* id_map;
    
    uint32 nof_mc_groups_created;
} soc_dnxf_mc_t;

typedef struct {
    
    soc_port_t* group_to_first_link;
} soc_dnxf_lb_t;

typedef struct {
    
    uint32* cl72_conf;
    
    int* phy_lane_config;
    
    SHR_BITDCL* is_connected_to_repeater;
    
    int* isolation_status_stored;
    
    int* orig_isolated_status;
} soc_dnxf_port_t;

typedef struct {
    
    uint32* flags;
    
    uint32* storm_timed_count;
    
    uint32* storm_timed_period;
    
    uint32 storm_nominal;
} soc_dnxf_intr_t;

typedef struct {
    
    soc_dnxf_modid_t modid;
    
    soc_dnxf_mc_t mc;
    
    soc_dnxf_lb_t lb;
    
    soc_dnxf_port_t port;
    
    soc_dnxf_intr_t intr;
    
    soc_dnxc_lane_map_db_t lane_map_db;
    
    soc_dnxc_synce_status_t synce;
} dnxf_state_t;


#endif 
