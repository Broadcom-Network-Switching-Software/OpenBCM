
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_BIER_DB_DIAGNOSTIC_H__
#define __DNX_BIER_DB_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_bier_db_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_multicast.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_BIER_DB_INFO,
    DNX_BIER_DB_SET_SIZE_INFO,
    DNX_BIER_DB_INFO_NOF_ENTRIES
} sw_state_dnx_bier_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_bier_db_info[SOC_MAX_NUM_DEVICES][DNX_BIER_DB_INFO_NOF_ENTRIES];

extern const char* dnx_bier_db_layout_str[DNX_BIER_DB_INFO_NOF_ENTRIES];
int dnx_bier_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_bier_db_set_size_dump(
    int unit, int set_size_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
