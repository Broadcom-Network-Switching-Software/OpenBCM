
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FABRIC_TYPES_H__
#define __DNX_FABRIC_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <include/shared/shr_thread_manager.h>
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
    
    uint32 state_0;
    
    uint32 state_1;
    
    uint32 state_2;
} dnx_fabric_system_upgrade_t;


typedef struct {
    
    dnx_fabric_modids_group_map_t* modids_to_group_map;
    
    dnx_fabric_link_t* links;
    
    shr_thread_manager_handler_t load_balancing_shr_thread_manager_handler;
    
    dnx_fabric_system_upgrade_t system_upgrade;
} dnx_fabric_db_t;


#endif 
