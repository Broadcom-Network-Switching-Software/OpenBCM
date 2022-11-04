
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_ACCESS_MAPPER_TYPES_H__
#define __DNX_FIELD_TCAM_ACCESS_MAPPER_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>


#define DNX_FIELD_TCAM_ACCESS_HASH_NULL 0xDEADBEEF



typedef struct {
    uint32* head;
    uint32* next;
} dnx_field_tcam_access_mapper_hash;

typedef struct {
    dnx_field_tcam_access_mapper_hash** key_2_location_hash;
} dnx_field_tcam_access_mapper_t;


#endif 
