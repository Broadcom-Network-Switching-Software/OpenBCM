
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __INTERRUPT_DIAGNOSTIC_H__
#define __INTERRUPT_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/interrupt_types.h>
#include <soc/types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    INTR_DB_INFO,
    INTR_DB_FLAGS_INFO,
    INTR_DB_STORM_TIMED_COUNT_INFO,
    INTR_DB_STORM_TIMED_PERIOD_INFO,
    INTR_DB_STORM_NOMINAL_INFO,
    INTR_DB_INFO_NOF_ENTRIES
} sw_state_intr_db_layout_e;


extern dnx_sw_state_diagnostic_info_t intr_db_info[SOC_MAX_NUM_DEVICES][INTR_DB_INFO_NOF_ENTRIES];

extern const char* intr_db_layout_str[INTR_DB_INFO_NOF_ENTRIES];
int intr_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int intr_db_flags_dump(
    int unit, int flags_idx_0, dnx_sw_state_dump_filters_t filters);

int intr_db_storm_timed_count_dump(
    int unit, int storm_timed_count_idx_0, dnx_sw_state_dump_filters_t filters);

int intr_db_storm_timed_period_dump(
    int unit, int storm_timed_period_idx_0, dnx_sw_state_dump_filters_t filters);

int intr_db_storm_nominal_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
