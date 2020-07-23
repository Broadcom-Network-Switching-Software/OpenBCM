
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_EGR_DB_TYPES_H__
#define __DNX_EGR_DB_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_egr_queuing.h>



typedef struct {
    
    uint32 allocation_bmap[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS];
    
    uint32 prio_mode[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS];
    
    int if_idx[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS];
} dnx_ps_db_t;


typedef struct {
    
    dnx_ps_db_t ps;
    
    int interface_occ[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES];
    
    int total_egr_if_credits;
} dnx_egr_db_t;


#endif 
