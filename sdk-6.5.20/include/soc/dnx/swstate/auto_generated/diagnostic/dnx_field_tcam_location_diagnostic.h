
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_LOCATION_DIAGNOSTIC_H__
#define __DNX_FIELD_TCAM_LOCATION_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_location_types.h>
#include <soc/dnxc/swstate/callbacks/sw_state_ll_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>
#include <soc/dnxc/swstate/types/sw_state_occupation_bitmap.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_bank_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_FIELD_TCAM_LOCATION_SW_INFO,
    DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_INFO,
    DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_ENTRIES_OCCUPATION_BITMAP_INFO,
    DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_PRIORITIES_RANGE_INFO,
    DNX_FIELD_TCAM_LOCATION_SW_LOCATION_PRIORITY_ARR_INFO,
    DNX_FIELD_TCAM_LOCATION_SW_TCAM_BANKS_OCCUPATION_BITMAP_INFO,
    DNX_FIELD_TCAM_LOCATION_SW_INFO_NOF_ENTRIES
} sw_state_dnx_field_tcam_location_sw_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_field_tcam_location_sw_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_TCAM_LOCATION_SW_INFO_NOF_ENTRIES];

extern const char* dnx_field_tcam_location_sw_layout_str[DNX_FIELD_TCAM_LOCATION_SW_INFO_NOF_ENTRIES];
int dnx_field_tcam_location_sw_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_location_sw_tcam_handlers_info_dump(
    int unit, int tcam_handlers_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_dump(
    int unit, int tcam_handlers_info_idx_0, int entries_occupation_bitmap_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_dump(
    int unit, int tcam_handlers_info_idx_0, int priorities_range_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_location_sw_location_priority_arr_dump(
    int unit, int location_priority_arr_idx_0, int location_priority_arr_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_dump(
    int unit, int tcam_banks_occupation_bitmap_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
