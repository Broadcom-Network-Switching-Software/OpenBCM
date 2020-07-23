
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __SWITCH_DIAGNOSTIC_H__
#define __SWITCH_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/switch_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    SWITCH_DB_INFO,
    SWITCH_DB_MODULE_VERIFICATION_INFO,
    SWITCH_DB_MODULE_ERROR_RECOVERY_INFO,
    SWITCH_DB_L3MCASTL2_FWD_TYPE_INFO,
    SWITCH_DB_HEADER_ENABLERS_HASHING_INFO,
    SWITCH_DB_INFO_NOF_ENTRIES
} sw_state_switch_db_layout_e;


extern dnx_sw_state_diagnostic_info_t switch_db_info[SOC_MAX_NUM_DEVICES][SWITCH_DB_INFO_NOF_ENTRIES];

extern const char* switch_db_layout_str[SWITCH_DB_INFO_NOF_ENTRIES];
int switch_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int switch_db_module_verification_dump(
    int unit, int module_verification_idx_0, dnx_sw_state_dump_filters_t filters);

int switch_db_module_error_recovery_dump(
    int unit, int module_error_recovery_idx_0, dnx_sw_state_dump_filters_t filters);

int switch_db_l3mcastl2_fwd_type_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int switch_db_header_enablers_hashing_dump(
    int unit, int header_enablers_hashing_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
