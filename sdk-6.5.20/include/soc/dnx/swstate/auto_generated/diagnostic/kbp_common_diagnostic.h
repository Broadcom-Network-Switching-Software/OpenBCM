
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __KBP_COMMON_DIAGNOSTIC_H__
#define __KBP_COMMON_DIAGNOSTIC_H__

#ifdef BCM_DNX_SUPPORT
#if defined(INCLUDE_KBP)
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/kbp_common_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    KBP_COMMON_SW_STATE_INFO,
    KBP_COMMON_SW_STATE_KBP_FWD_CACHING_ENABLED_INFO,
    KBP_COMMON_SW_STATE_KBP_ACL_CACHING_ENABLED_INFO,
    KBP_COMMON_SW_STATE_INFO_NOF_ENTRIES
} sw_state_kbp_common_sw_state_layout_e;


extern dnx_sw_state_diagnostic_info_t kbp_common_sw_state_info[SOC_MAX_NUM_DEVICES][KBP_COMMON_SW_STATE_INFO_NOF_ENTRIES];

extern const char* kbp_common_sw_state_layout_str[KBP_COMMON_SW_STATE_INFO_NOF_ENTRIES];
int kbp_common_sw_state_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int kbp_common_sw_state_kbp_fwd_caching_enabled_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int kbp_common_sw_state_kbp_acl_caching_enabled_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 
#endif  
#endif  

#endif 
