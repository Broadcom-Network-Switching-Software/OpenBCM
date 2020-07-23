
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_HARD_RESET_DIAGNOSTIC_H__
#define __DNX_HARD_RESET_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_hard_reset_types.h>
#include <include/bcm/switch.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_HARD_RESET_DB_INFO,
    DNX_HARD_RESET_DB_HARD_RESET_CALLBACK_INFO,
    DNX_HARD_RESET_DB_HARD_RESET_CALLBACK_CALLBACK_INFO,
    DNX_HARD_RESET_DB_HARD_RESET_CALLBACK_USERDATA_INFO,
    DNX_HARD_RESET_DB_INFO_NOF_ENTRIES
} sw_state_dnx_hard_reset_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_hard_reset_db_info[SOC_MAX_NUM_DEVICES][DNX_HARD_RESET_DB_INFO_NOF_ENTRIES];

extern const char* dnx_hard_reset_db_layout_str[DNX_HARD_RESET_DB_INFO_NOF_ENTRIES];
int dnx_hard_reset_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_hard_reset_db_hard_reset_callback_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_hard_reset_db_hard_reset_callback_callback_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_hard_reset_db_hard_reset_callback_userdata_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
