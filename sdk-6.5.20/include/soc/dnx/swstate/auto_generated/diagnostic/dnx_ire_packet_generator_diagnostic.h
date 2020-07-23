
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_IRE_PACKET_GENERATOR_DIAGNOSTIC_H__
#define __DNX_IRE_PACKET_GENERATOR_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_ire_packet_generator_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    IRE_PACKET_GENERATOR_INFO_INFO,
    IRE_PACKET_GENERATOR_INFO_IRE_DATAPATH_ENABLE_STATE_INFO,
    IRE_PACKET_GENERATOR_INFO_INFO_NOF_ENTRIES
} sw_state_ire_packet_generator_info_layout_e;


extern dnx_sw_state_diagnostic_info_t ire_packet_generator_info_info[SOC_MAX_NUM_DEVICES][IRE_PACKET_GENERATOR_INFO_INFO_NOF_ENTRIES];

extern const char* ire_packet_generator_info_layout_str[IRE_PACKET_GENERATOR_INFO_INFO_NOF_ENTRIES];
int ire_packet_generator_info_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int ire_packet_generator_info_ire_datapath_enable_state_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
