
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __FLOW_TYPES_H__
#define __FLOW_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>



typedef struct {
    
    uint8 is_verify_disabled;
    
    uint8 is_error_recovery_disabled;
} dnx_flow_app_common_config_t;


typedef struct {
    
    dnx_flow_app_common_config_t* flow_application_info;
} flow_sw_state_t;


#endif 
