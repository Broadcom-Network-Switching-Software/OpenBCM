
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_FLUSH_TYPES_H__
#define __DNX_FIELD_FLUSH_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>


typedef struct {
    dnx_field_group_t mapped_fg;
} dnx_field_flush_profile_info_t;

typedef struct {
    dnx_field_flush_profile_info_t* flush_profile;
} dnx_field_flush_sw_t;


#endif 
