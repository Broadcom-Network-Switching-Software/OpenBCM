
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __FLUSH_DIAGNOSTIC_H__
#define __FLUSH_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/flush_types.h>
#include <bcm/switch.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    FLUSH_DB_CONTEXT_INFO,
    FLUSH_DB_CONTEXT_NOF_VALID_RULES_INFO,
    FLUSH_DB_CONTEXT_FLUSH_IN_BULK_INFO,
    FLUSH_DB_CONTEXT_INFO_NOF_ENTRIES
} sw_state_flush_db_context_layout_e;


extern dnx_sw_state_diagnostic_info_t flush_db_context_info[SOC_MAX_NUM_DEVICES][FLUSH_DB_CONTEXT_INFO_NOF_ENTRIES];

extern const char* flush_db_context_layout_str[FLUSH_DB_CONTEXT_INFO_NOF_ENTRIES];
int flush_db_context_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int flush_db_context_nof_valid_rules_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int flush_db_context_flush_in_bulk_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
