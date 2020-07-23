
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_ALGO_SYNCE_DIAGNOSTIC_H__
#define __DNX_ALGO_SYNCE_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_algo_synce_types.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_ALGO_SYNCE_DB_INFO,
    DNX_ALGO_SYNCE_DB_FABRIC_SYNCE_STATUS_INFO,
    DNX_ALGO_SYNCE_DB_FABRIC_SYNCE_STATUS_MASTER_SYNCE_ENABLED_INFO,
    DNX_ALGO_SYNCE_DB_FABRIC_SYNCE_STATUS_SLAVE_SYNCE_ENABLED_INFO,
    DNX_ALGO_SYNCE_DB_INFO_NOF_ENTRIES
} sw_state_dnx_algo_synce_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_algo_synce_db_info[SOC_MAX_NUM_DEVICES][DNX_ALGO_SYNCE_DB_INFO_NOF_ENTRIES];

extern const char* dnx_algo_synce_db_layout_str[DNX_ALGO_SYNCE_DB_INFO_NOF_ENTRIES];
int dnx_algo_synce_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_algo_synce_db_fabric_synce_status_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_algo_synce_db_fabric_synce_status_master_synce_enabled_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_algo_synce_db_fabric_synce_status_slave_synce_enabled_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
