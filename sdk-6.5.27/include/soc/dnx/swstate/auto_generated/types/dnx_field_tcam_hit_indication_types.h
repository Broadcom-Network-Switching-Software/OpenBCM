
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_HIT_INDICATION_TYPES_H__
#define __DNX_FIELD_TCAM_HIT_INDICATION_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>


typedef struct {
    SHR_BITDCL* bmp;
} dnx_field_tcam_hit_indication_hitbit_unit_info_t;

typedef struct {
    dnx_field_tcam_hit_indication_hitbit_unit_info_t** tcam_action_hit_indication;
} dnx_field_tcam_hit_indication_t;


#endif 
