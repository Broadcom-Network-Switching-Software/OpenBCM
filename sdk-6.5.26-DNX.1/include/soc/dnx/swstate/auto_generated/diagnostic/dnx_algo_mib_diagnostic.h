
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_ALGO_MIB_DIAGNOSTIC_H__
#define __DNX_ALGO_MIB_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_algo_mib_types.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_ALGO_MIB_DB_INFO,
    DNX_ALGO_MIB_DB_MIB_INFO,
    DNX_ALGO_MIB_DB_MIB_COUNTER_PBMP_INFO,
    DNX_ALGO_MIB_DB_MIB_INTERVAL_INFO,
    DNX_ALGO_MIB_DB_INFO_NOF_ENTRIES
} sw_state_dnx_algo_mib_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_algo_mib_db_info[SOC_MAX_NUM_DEVICES][DNX_ALGO_MIB_DB_INFO_NOF_ENTRIES];

extern const char* dnx_algo_mib_db_layout_str[DNX_ALGO_MIB_DB_INFO_NOF_ENTRIES];
int dnx_algo_mib_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_algo_mib_db_mib_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_algo_mib_db_mib_counter_pbmp_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_algo_mib_db_mib_interval_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
