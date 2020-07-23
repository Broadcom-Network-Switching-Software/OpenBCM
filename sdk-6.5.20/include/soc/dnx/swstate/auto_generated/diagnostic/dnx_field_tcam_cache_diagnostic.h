
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_CACHE_DIAGNOSTIC_H__
#define __DNX_FIELD_TCAM_CACHE_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_cache_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_FIELD_TCAM_CACHE_SHADOW_INFO,
    DNX_FIELD_TCAM_CACHE_SHADOW_TCAM_BANK_INFO,
    DNX_FIELD_TCAM_CACHE_SHADOW_TCAM_BANK_ENTRY_KEY_INFO,
    DNX_FIELD_TCAM_CACHE_SHADOW_PAYLOAD_TABLE_INFO,
    DNX_FIELD_TCAM_CACHE_SHADOW_PAYLOAD_TABLE_ENTRY_PAYLOAD_INFO,
    DNX_FIELD_TCAM_CACHE_SHADOW_INFO_NOF_ENTRIES
} sw_state_dnx_field_tcam_cache_shadow_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_field_tcam_cache_shadow_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_TCAM_CACHE_SHADOW_INFO_NOF_ENTRIES];

extern const char* dnx_field_tcam_cache_shadow_layout_str[DNX_FIELD_TCAM_CACHE_SHADOW_INFO_NOF_ENTRIES];
int dnx_field_tcam_cache_shadow_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_cache_shadow_tcam_bank_dump(
    int unit, int tcam_bank_idx_0, int tcam_bank_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_cache_shadow_tcam_bank_entry_key_dump(
    int unit, int tcam_bank_idx_0, int tcam_bank_idx_1, int entry_key_idx_0, int entry_key_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_cache_shadow_payload_table_dump(
    int unit, int payload_table_idx_0, int payload_table_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_cache_shadow_payload_table_entry_payload_dump(
    int unit, int payload_table_idx_0, int payload_table_idx_1, int entry_payload_idx_0, int entry_payload_idx_1, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
