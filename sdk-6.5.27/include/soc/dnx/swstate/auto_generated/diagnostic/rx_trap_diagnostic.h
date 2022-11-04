
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_RX_TRAP_DIAGNOSTIC_H__
#define __DNX_RX_TRAP_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnx/swstate/auto_generated/types/rx_trap_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    RX_TRAP_INFO_INFO,
    RX_TRAP_INFO_USER_TRAP_INFO_INFO,
    RX_TRAP_INFO_USER_TRAP_INFO_NAME_INFO,
    RX_TRAP_INFO_INFO_NOF_ENTRIES
} sw_state_rx_trap_info_layout_e;


extern dnx_sw_state_diagnostic_info_t rx_trap_info_info[SOC_MAX_NUM_DEVICES][RX_TRAP_INFO_INFO_NOF_ENTRIES];

extern const char* rx_trap_info_layout_str[RX_TRAP_INFO_INFO_NOF_ENTRIES];
int rx_trap_info_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int rx_trap_info_user_trap_info_dump(
    int unit, int user_trap_info_idx_0, dnx_sw_state_dump_filters_t filters);

int rx_trap_info_user_trap_info_name_dump(
    int unit, int user_trap_info_idx_0, int name_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
