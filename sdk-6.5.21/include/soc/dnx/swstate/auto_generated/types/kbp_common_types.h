
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __KBP_COMMON_TYPES_H__
#define __KBP_COMMON_TYPES_H__

#ifdef BCM_DNX_SUPPORT
#if defined(INCLUDE_KBP)
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>



typedef struct {
    int kbp_fwd_caching_enabled;
    int kbp_acl_caching_enabled;
} kbp_common_sw_state_t;

#endif  
#endif  

#endif 
