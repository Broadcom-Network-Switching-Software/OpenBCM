
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_TRAPS_DIAGNOSTIC_H__
#define __DNX_TRAPS_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_traps_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_RX_TRAPS_INFO_INFO,
    DNX_RX_TRAPS_INFO_ETPP_KEEP_FABRIC_HEADERS_PROFILE_INFO,
    DNX_RX_TRAPS_INFO_INFO_NOF_ENTRIES
} sw_state_dnx_rx_traps_info_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_rx_traps_info_info[SOC_MAX_NUM_DEVICES][DNX_RX_TRAPS_INFO_INFO_NOF_ENTRIES];

extern const char* dnx_rx_traps_info_layout_str[DNX_RX_TRAPS_INFO_INFO_NOF_ENTRIES];
int dnx_rx_traps_info_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_rx_traps_info_etpp_keep_fabric_headers_profile_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
