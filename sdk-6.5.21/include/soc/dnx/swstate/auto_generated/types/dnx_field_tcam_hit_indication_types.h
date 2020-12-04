
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_HIT_INDICATION_TYPES_H__
#define __DNX_FIELD_TCAM_HIT_INDICATION_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>



typedef struct {
    
    uint8** bank_line_id;
} dnx_field_tcam_hit_indication_small_banks_sram_info_t;


typedef struct {
    
    uint8** bank_line_id;
} dnx_field_tcam_hit_indication_big_banks_sram_info_t;


typedef struct {
    
    dnx_field_tcam_hit_indication_big_banks_sram_info_t* tcam_action_hit_indication;
    
    dnx_field_tcam_hit_indication_small_banks_sram_info_t* tcam_action_hit_indication_small;
} dnx_field_tcam_hit_indication_t;


#endif 
