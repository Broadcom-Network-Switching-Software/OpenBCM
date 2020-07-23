
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __KBP_COMMON_ACCESS_H__
#define __KBP_COMMON_ACCESS_H__

#ifdef BCM_DNX_SUPPORT
#if defined(INCLUDE_KBP)
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/kbp_common_types.h>



typedef int (*kbp_common_sw_state_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*kbp_common_sw_state_init_cb)(
    int unit);


typedef int (*kbp_common_sw_state_kbp_fwd_caching_enabled_set_cb)(
    int unit, int kbp_fwd_caching_enabled);


typedef int (*kbp_common_sw_state_kbp_fwd_caching_enabled_get_cb)(
    int unit, int *kbp_fwd_caching_enabled);


typedef int (*kbp_common_sw_state_kbp_acl_caching_enabled_set_cb)(
    int unit, int kbp_acl_caching_enabled);


typedef int (*kbp_common_sw_state_kbp_acl_caching_enabled_get_cb)(
    int unit, int *kbp_acl_caching_enabled);




typedef struct {
    kbp_common_sw_state_kbp_fwd_caching_enabled_set_cb set;
    kbp_common_sw_state_kbp_fwd_caching_enabled_get_cb get;
} kbp_common_sw_state_kbp_fwd_caching_enabled_cbs;


typedef struct {
    kbp_common_sw_state_kbp_acl_caching_enabled_set_cb set;
    kbp_common_sw_state_kbp_acl_caching_enabled_get_cb get;
} kbp_common_sw_state_kbp_acl_caching_enabled_cbs;


typedef struct {
    kbp_common_sw_state_is_init_cb is_init;
    kbp_common_sw_state_init_cb init;
    
    kbp_common_sw_state_kbp_fwd_caching_enabled_cbs kbp_fwd_caching_enabled;
    
    kbp_common_sw_state_kbp_acl_caching_enabled_cbs kbp_acl_caching_enabled;
} kbp_common_sw_state_cbs;





extern kbp_common_sw_state_cbs kbp_common_sw_state;
#endif  
#endif  

#endif 
