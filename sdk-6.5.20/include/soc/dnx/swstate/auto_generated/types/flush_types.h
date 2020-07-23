
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __FLUSH_TYPES_H__
#define __FLUSH_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <bcm/switch.h>



typedef struct {
    
    uint32 nof_valid_rules;
    
    uint8 flush_in_bulk;
} flush_db_t;


#endif 
