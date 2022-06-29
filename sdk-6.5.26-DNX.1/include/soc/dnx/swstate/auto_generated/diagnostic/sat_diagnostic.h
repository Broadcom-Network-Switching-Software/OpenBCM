
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_SAT_DIAGNOSTIC_H__
#define __DNX_SAT_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnx/swstate/auto_generated/types/sat_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    SAT_PKT_HEADER_INFO_INFO,
    SAT_PKT_HEADER_INFO_PKT_HEADER_BASE_INFO,
    SAT_PKT_HEADER_INFO_INFO_NOF_ENTRIES
} sw_state_sat_pkt_header_info_layout_e;


extern dnx_sw_state_diagnostic_info_t sat_pkt_header_info_info[SOC_MAX_NUM_DEVICES][SAT_PKT_HEADER_INFO_INFO_NOF_ENTRIES];

extern const char* sat_pkt_header_info_layout_str[SAT_PKT_HEADER_INFO_INFO_NOF_ENTRIES];
int sat_pkt_header_info_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int sat_pkt_header_info_pkt_header_base_dump(
    int unit, int pkt_header_base_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
