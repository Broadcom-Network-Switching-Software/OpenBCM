
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_SRV6_DIAGNOSTIC_H__
#define __DNX_SRV6_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnx/swstate/auto_generated/types/srv6_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    SRV6_MODES_INFO,
    SRV6_MODES_EGRESS_IS_PSP_INFO,
    SRV6_MODES_ENCAP_IS_REDUCED_INFO,
    SRV6_MODES_GSID_PREFIX_IS_64B_INFO,
    SRV6_MODES_USID_PREFIX_IS_48B_INFO,
    SRV6_MODES_LPM_DESTINATION_FOR_USP_TRAP_INFO,
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

int srv6_modes_gsid_prefix_is_64b_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int srv6_modes_usid_prefix_is_48b_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int srv6_modes_lpm_destination_for_usp_trap_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
