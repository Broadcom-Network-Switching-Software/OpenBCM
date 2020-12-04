
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_TIME_SW_STATE_TYPES_H__
#define __DNX_TIME_SW_STATE_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <include/bcm/types.h>



typedef struct {
    
    uint32 flags;
    
    int id;
    
    int heartbeat_hz;
    
    int clk_resolution;
    
    int bitclock_hz;
    
    int status;
} dnx_time_interface_sw_state_t;


#endif 
