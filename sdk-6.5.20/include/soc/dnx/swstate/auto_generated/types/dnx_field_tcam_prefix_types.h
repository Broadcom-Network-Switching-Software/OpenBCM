
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_PREFIX_TYPES_H__
#define __DNX_FIELD_TCAM_PREFIX_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_manager_types.h>


#define DNX_FIELD_TCAM_PREFIX_VAL_INVALID 0x511




typedef struct {
    
    uint32* prefix_handler_map;
} dnx_field_tcam_prefix_t;


typedef struct {
    
    dnx_field_tcam_prefix_t* banks_prefix;
} dnx_field_tcam_prefix_sw_t;


#endif 
