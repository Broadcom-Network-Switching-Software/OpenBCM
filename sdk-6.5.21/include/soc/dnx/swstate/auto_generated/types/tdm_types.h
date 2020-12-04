
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __TDM_TYPES_H__
#define __TDM_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>



typedef struct {
    
    uint8 tdm_header_is_ftmh;
    
    uint8 use_optimized_ftmh;
} tdm_ftmh_headers_params;


#endif 
