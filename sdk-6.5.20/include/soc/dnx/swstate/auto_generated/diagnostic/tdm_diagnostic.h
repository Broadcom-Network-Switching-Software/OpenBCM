
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __TDM_DIAGNOSTIC_H__
#define __TDM_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/tdm_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    TDM_FTMH_INFO_INFO,
    TDM_FTMH_INFO_TDM_HEADER_IS_FTMH_INFO,
    TDM_FTMH_INFO_USE_OPTIMIZED_FTMH_INFO,
    TDM_FTMH_INFO_INFO_NOF_ENTRIES
} sw_state_tdm_ftmh_info_layout_e;


extern dnx_sw_state_diagnostic_info_t tdm_ftmh_info_info[SOC_MAX_NUM_DEVICES][TDM_FTMH_INFO_INFO_NOF_ENTRIES];

extern const char* tdm_ftmh_info_layout_str[TDM_FTMH_INFO_INFO_NOF_ENTRIES];
int tdm_ftmh_info_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int tdm_ftmh_info_tdm_header_is_ftmh_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int tdm_ftmh_info_use_optimized_ftmh_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
