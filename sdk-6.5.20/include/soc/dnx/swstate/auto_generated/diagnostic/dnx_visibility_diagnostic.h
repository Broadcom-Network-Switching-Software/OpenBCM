
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_VISIBILITY_DIAGNOSTIC_H__
#define __DNX_VISIBILITY_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_visibility_types.h>
#include <bcm/instru.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    VISIBILITY_INFO_DB_INFO,
    VISIBILITY_INFO_DB_VISIB_MODE_INFO,
    VISIBILITY_INFO_DB_INFO_NOF_ENTRIES
} sw_state_visibility_info_db_layout_e;


extern dnx_sw_state_diagnostic_info_t visibility_info_db_info[SOC_MAX_NUM_DEVICES][VISIBILITY_INFO_DB_INFO_NOF_ENTRIES];

extern const char* visibility_info_db_layout_str[VISIBILITY_INFO_DB_INFO_NOF_ENTRIES];
int visibility_info_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int visibility_info_db_visib_mode_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
