
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_TIME_SW_STATE_DIAGNOSTIC_H__
#define __DNX_TIME_SW_STATE_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_time_sw_state_types.h>
#include <include/bcm/types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_TIME_INTERFACE_DB_INFO,
    DNX_TIME_INTERFACE_DB_FLAGS_INFO,
    DNX_TIME_INTERFACE_DB_ID_INFO,
    DNX_TIME_INTERFACE_DB_HEARTBEAT_HZ_INFO,
    DNX_TIME_INTERFACE_DB_CLK_RESOLUTION_INFO,
    DNX_TIME_INTERFACE_DB_BITCLOCK_HZ_INFO,
    DNX_TIME_INTERFACE_DB_STATUS_INFO,
    DNX_TIME_INTERFACE_DB_INFO_NOF_ENTRIES
} sw_state_dnx_time_interface_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_time_interface_db_info[SOC_MAX_NUM_DEVICES][DNX_TIME_INTERFACE_DB_INFO_NOF_ENTRIES];

extern const char* dnx_time_interface_db_layout_str[DNX_TIME_INTERFACE_DB_INFO_NOF_ENTRIES];
int dnx_time_interface_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_time_interface_db_flags_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_time_interface_db_id_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_time_interface_db_heartbeat_hz_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_time_interface_db_clk_resolution_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_time_interface_db_bitclock_hz_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_time_interface_db_status_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
