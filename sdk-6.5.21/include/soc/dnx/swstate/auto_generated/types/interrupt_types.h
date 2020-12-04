
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __INTERRUPT_TYPES_H__
#define __INTERRUPT_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/types.h>



typedef struct {
    
    uint32* flags;
    
    uint32* storm_timed_count;
    
    uint32* storm_timed_period;
    
    uint32 storm_nominal;
} soc_dnx_intr_t;


#endif 
