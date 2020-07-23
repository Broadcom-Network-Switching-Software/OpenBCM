
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __SRV6_DIAGNOSTIC_H__
#define __SRV6_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/srv6_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    SRV6_MODES_INFO,
    SRV6_MODES_EGRESS_IS_PSP_INFO,
    SRV6_MODES_ENCAP_IS_REDUCED_INFO,
    SRV6_MODES_INFO_NOF_ENTRIES
} sw_state_srv6_modes_layout_e;


extern dnx_sw_state_diagnostic_info_t srv6_modes_info[SOC_MAX_NUM_DEVICES][SRV6_MODES_INFO_NOF_ENTRIES];

extern const char* srv6_modes_layout_str[SRV6_MODES_INFO_NOF_ENTRIES];
int srv6_modes_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int srv6_modes_egress_is_psp_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int srv6_modes_encap_is_reduced_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
