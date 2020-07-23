
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __SWITCH_TYPES_H__
#define __SWITCH_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <bcm/switch.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>



typedef struct {
    
    uint8 module_verification[bcmModuleCount];
    
    uint8 module_error_recovery[bcmModuleCount];
    
    uint8 l3mcastl2_fwd_type;
    
    uint32 header_enablers_hashing[DBAL_NOF_ENUM_HASH_FIELD_ENABLERS_HEADER_VALUES];
} switch_sw_state_t;


#endif 
