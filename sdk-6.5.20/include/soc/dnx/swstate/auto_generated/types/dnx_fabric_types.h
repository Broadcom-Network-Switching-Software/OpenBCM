
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FABRIC_TYPES_H__
#define __DNX_FABRIC_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <include/shared/periodic_event.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>



typedef struct {
    
    int nof_modids;
    
    int* modids;
} dnx_fabric_modids_group_map_t;


typedef struct {
    
    int is_link_allowed;
    
    int is_link_isolated;
} dnx_fabric_link_t;


typedef struct {
    
    dnx_fabric_modids_group_map_t* modids_to_group_map;
    
    dnx_fabric_link_t* links;
    
    periodic_event_handler_t load_balancing_periodic_event_handler;
} dnx_fabric_db_t;


#endif 
