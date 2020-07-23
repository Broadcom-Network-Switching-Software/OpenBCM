
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_ACTIONS_DIAGNOSTIC_H__
#define __DNX_FIELD_ACTIONS_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_actions_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_FIELD_ACTION_SW_DB_INFO,
    DNX_FIELD_ACTION_SW_DB_INFO_INFO,
    DNX_FIELD_ACTION_SW_DB_INFO_VALID_INFO,
    DNX_FIELD_ACTION_SW_DB_INFO_FIELD_ID_INFO,
    DNX_FIELD_ACTION_SW_DB_INFO_SIZE_INFO,
    DNX_FIELD_ACTION_SW_DB_INFO_PREFIX_INFO,
    DNX_FIELD_ACTION_SW_DB_INFO_PREFIX_SIZE_INFO,
    DNX_FIELD_ACTION_SW_DB_INFO_BCM_ID_INFO,
    DNX_FIELD_ACTION_SW_DB_INFO_REF_COUNT_INFO,
    DNX_FIELD_ACTION_SW_DB_INFO_FLAGS_INFO,
    DNX_FIELD_ACTION_SW_DB_INFO_BASE_DNX_ACTION_INFO,
    DNX_FIELD_ACTION_SW_DB_FEM_INFO_INFO,
    DNX_FIELD_ACTION_SW_DB_FEM_INFO_FG_ID_INFO_INFO,
    DNX_FIELD_ACTION_SW_DB_FEM_INFO_FG_ID_INFO_FG_ID_INFO,
    DNX_FIELD_ACTION_SW_DB_FEM_INFO_FG_ID_INFO_INPUT_OFFSET_INFO,
    DNX_FIELD_ACTION_SW_DB_FEM_INFO_FG_ID_INFO_INPUT_SIZE_INFO,
    DNX_FIELD_ACTION_SW_DB_FEM_INFO_FG_ID_INFO_SECOND_FG_ID_INFO,
    DNX_FIELD_ACTION_SW_DB_FEM_INFO_FG_ID_INFO_IGNORE_ACTIONS_INFO,
    DNX_FIELD_ACTION_SW_DB_FEM_INFO_FG_ID_INFO_FEM_ENCODED_ACTIONS_INFO,
    DNX_FIELD_ACTION_SW_DB_INFO_NOF_ENTRIES
} sw_state_dnx_field_action_sw_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_field_action_sw_db_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_ACTION_SW_DB_INFO_NOF_ENTRIES];

extern const char* dnx_field_action_sw_db_layout_str[DNX_FIELD_ACTION_SW_DB_INFO_NOF_ENTRIES];
int dnx_field_action_sw_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_field_action_sw_db_info_dump(
    int unit, int info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_action_sw_db_info_valid_dump(
    int unit, int info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_action_sw_db_info_field_id_dump(
    int unit, int info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_action_sw_db_info_size_dump(
    int unit, int info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_action_sw_db_info_prefix_dump(
    int unit, int info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_action_sw_db_info_prefix_size_dump(
    int unit, int info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_action_sw_db_info_bcm_id_dump(
    int unit, int info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_action_sw_db_info_ref_count_dump(
    int unit, int info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_action_sw_db_info_flags_dump(
    int unit, int info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_action_sw_db_info_base_dnx_action_dump(
    int unit, int info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_action_sw_db_fem_info_dump(
    int unit, int fem_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_action_sw_db_fem_info_fg_id_info_dump(
    int unit, int fem_info_idx_0, int fg_id_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_action_sw_db_fem_info_fg_id_info_fg_id_dump(
    int unit, int fem_info_idx_0, int fg_id_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_action_sw_db_fem_info_fg_id_info_input_offset_dump(
    int unit, int fem_info_idx_0, int fg_id_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_action_sw_db_fem_info_fg_id_info_input_size_dump(
    int unit, int fem_info_idx_0, int fg_id_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_action_sw_db_fem_info_fg_id_info_second_fg_id_dump(
    int unit, int fem_info_idx_0, int fg_id_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_action_sw_db_fem_info_fg_id_info_ignore_actions_dump(
    int unit, int fem_info_idx_0, int fg_id_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_action_sw_db_fem_info_fg_id_info_fem_encoded_actions_dump(
    int unit, int fem_info_idx_0, int fg_id_info_idx_0, int fem_encoded_actions_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
