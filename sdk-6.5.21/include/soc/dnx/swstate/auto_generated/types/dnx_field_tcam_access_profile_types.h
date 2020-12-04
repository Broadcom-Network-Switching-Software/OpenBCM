
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_ACCESS_PROFILE_TYPES_H__
#define __DNX_FIELD_TCAM_ACCESS_PROFILE_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>



#define DNX_FIELD_TCAM_ACCESS_PROFILE_PAIRED_ID_GET(profile_id) ((profile_id) + 1)




typedef struct {
    
    uint8 occupied;
    
    dnx_field_key_length_type_e key_size;
    
    dnx_field_action_length_type_e action_size;
    
    uint16 bank_ids_bmp;
    
    uint8 is_direct;
    
    uint8 prefix;
    
    uint32 handler_id;
} dnx_field_tcam_access_profile_t;


typedef struct {
    
    dnx_field_tcam_access_profile_t* access_profiles;
} dnx_field_tcam_access_profile_sw_t;


#endif 
