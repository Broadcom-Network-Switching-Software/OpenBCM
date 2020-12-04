
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_ECGM_TYPES_H__
#define __DNX_ECGM_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <bcm_int/dnx/algo/ecgm/algo_ecgm.h>



typedef struct {
    
    bcm_port_t port;
    
    dnx_ecgm_interface_profile_info_t info;
    
    int profile_id;
    
    int valid;
} dnx_ecgm_interface_data_t;


typedef struct {
    
    bcm_port_t port;
    
    dnx_ecgm_port_profile_info_t info;
    
    int profile_id;
    
    int valid;
} dnx_ecgm_port_data_t;


typedef struct {
    
    dnx_ecgm_interface_data_t interface_caching;
    
    dnx_ecgm_port_data_t port_caching;
} dnx_ecgm_sw_state_t;


#endif 
