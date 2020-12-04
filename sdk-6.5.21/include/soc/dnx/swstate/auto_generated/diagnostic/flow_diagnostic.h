
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __FLOW_DIAGNOSTIC_H__
#define __FLOW_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/flow_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    FLOW_DB_INFO,
    FLOW_DB_FLOW_APPLICATION_INFO_INFO,
    FLOW_DB_FLOW_APPLICATION_INFO_IS_VERIFY_DISABLED_INFO,
    FLOW_DB_FLOW_APPLICATION_INFO_IS_ERROR_RECOVERY_DISABLED_INFO,
    FLOW_DB_INFO_NOF_ENTRIES
} sw_state_flow_db_layout_e;


extern dnx_sw_state_diagnostic_info_t flow_db_info[SOC_MAX_NUM_DEVICES][FLOW_DB_INFO_NOF_ENTRIES];

extern const char* flow_db_layout_str[FLOW_DB_INFO_NOF_ENTRIES];
int flow_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int flow_db_flow_application_info_dump(
    int unit, int flow_application_info_idx_0, dnx_sw_state_dump_filters_t filters);

int flow_db_flow_application_info_is_verify_disabled_dump(
    int unit, int flow_application_info_idx_0, dnx_sw_state_dump_filters_t filters);

int flow_db_flow_application_info_is_error_recovery_disabled_dump(
    int unit, int flow_application_info_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
