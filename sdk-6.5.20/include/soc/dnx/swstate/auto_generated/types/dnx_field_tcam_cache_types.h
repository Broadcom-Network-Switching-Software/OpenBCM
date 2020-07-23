
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_CACHE_TYPES_H__
#define __DNX_FIELD_TCAM_CACHE_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>



typedef struct {
    
    uint8** entry_key;
} dnx_field_tcam_cach_shadow_bank_t;


typedef struct {
    
    uint8** entry_payload;
} dnx_field_tcam_cach_shadow_payload_table_t;


typedef struct {
    
    dnx_field_tcam_cach_shadow_bank_t** tcam_bank;
    
    dnx_field_tcam_cach_shadow_payload_table_t** payload_table;
} dnx_field_tcam_cach_shadow_t;


#endif 
