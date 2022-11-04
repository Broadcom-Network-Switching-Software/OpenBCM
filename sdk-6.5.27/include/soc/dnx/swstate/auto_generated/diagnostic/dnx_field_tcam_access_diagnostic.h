
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_ACCESS_DIAGNOSTIC_H__
#define __DNX_FIELD_TCAM_ACCESS_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_access_types.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_index_htb.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_manager_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_FIELD_TCAM_ACCESS_SW_INFO,
    DNX_FIELD_TCAM_ACCESS_SW_ENTRY_LOCATION_HASH_INFO,
    DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_INFO,
    DNX_FIELD_TCAM_ACCESS_SW_VALID_BMP_V_BIT_INFO,
    DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_INFO,
    DNX_FIELD_TCAM_ACCESS_SW_ENTRY_IN_USE_BMP_V_BIT_INFO,
    DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_INFO,
    DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_DT_BANK_ID_INFO,
    DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_STAGE_INFO,
    DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_BANK_ALLOCATION_MODE_INFO,
    DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_KEY_SIZE_INFO,
    DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_ACTION_SIZE_INFO,
    DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_ACTUAL_ACTION_SIZE_INFO,
    DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_PREFIX_SIZE_INFO,
    DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_DIRECT_TABLE_INFO,
    DNX_FIELD_TCAM_ACCESS_SW_FG_PARAMS_CONTEXT_SHARING_HANDLERS_CB_INFO,
    DNX_FIELD_TCAM_ACCESS_SW_FG_PREFIX_PER_CORE_INFO,
    DNX_FIELD_TCAM_ACCESS_SW_CACHE_INSTALL_IN_PROGRESS_INFO,
    DNX_FIELD_TCAM_ACCESS_SW_INFO_NOF_ENTRIES
} sw_state_dnx_field_tcam_access_sw_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_field_tcam_access_sw_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_TCAM_ACCESS_SW_INFO_NOF_ENTRIES];

extern const char* dnx_field_tcam_access_sw_layout_str[DNX_FIELD_TCAM_ACCESS_SW_INFO_NOF_ENTRIES];
int dnx_field_tcam_access_sw_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_access_sw_entry_location_hash_dump(
    int unit, int entry_location_hash_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_access_sw_valid_bmp_dump(
    int unit, int valid_bmp_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_access_sw_valid_bmp_v_bit_dump(
    int unit, int valid_bmp_idx_0, int v_bit_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_access_sw_entry_in_use_bmp_dump(
    int unit, int entry_in_use_bmp_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_dump(
    int unit, int entry_in_use_bmp_idx_0, int v_bit_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_access_sw_fg_params_dump(
    int unit, int fg_params_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_access_sw_fg_params_dt_bank_id_dump(
    int unit, int fg_params_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_access_sw_fg_params_stage_dump(
    int unit, int fg_params_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_access_sw_fg_params_bank_allocation_mode_dump(
    int unit, int fg_params_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_access_sw_fg_params_key_size_dump(
    int unit, int fg_params_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_access_sw_fg_params_action_size_dump(
    int unit, int fg_params_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_access_sw_fg_params_actual_action_size_dump(
    int unit, int fg_params_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_access_sw_fg_params_prefix_size_dump(
    int unit, int fg_params_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_access_sw_fg_params_direct_table_dump(
    int unit, int fg_params_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_access_sw_fg_params_context_sharing_handlers_cb_dump(
    int unit, int fg_params_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_access_sw_fg_prefix_per_core_dump(
    int unit, int fg_prefix_per_core_idx_0, int fg_prefix_per_core_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_access_sw_cache_install_in_progress_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
