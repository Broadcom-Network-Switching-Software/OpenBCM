
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_ALGO_FIELD_ACTION_DIAGNOSTIC_H__
#define __DNX_ALGO_FIELD_ACTION_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_algo_field_action_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_key_types.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_algo_field_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_ALGO_FIELD_ACTION_SW_INFO,
    DNX_ALGO_FIELD_ACTION_SW_IPMF2_INFO,
    DNX_ALGO_FIELD_ACTION_SW_IPMF2_FES_STATE_INFO,
    DNX_ALGO_FIELD_ACTION_SW_IPMF2_FES_STATE_FIELD_GROUP_INFO,
    DNX_ALGO_FIELD_ACTION_SW_IPMF2_FES_STATE_PLACE_IN_FG_INFO,
    DNX_ALGO_FIELD_ACTION_SW_IPMF2_FES_STATE_MASK_IS_ALLOC_INFO,
    DNX_ALGO_FIELD_ACTION_SW_IPMF2_CONTEXT_STATE_INFO,
    DNX_ALGO_FIELD_ACTION_SW_IPMF2_CONTEXT_STATE_PRIORITY_INFO,
    DNX_ALGO_FIELD_ACTION_SW_IPMF3_INFO,
    DNX_ALGO_FIELD_ACTION_SW_IPMF3_FES_STATE_INFO,
    DNX_ALGO_FIELD_ACTION_SW_IPMF3_FES_STATE_FIELD_GROUP_INFO,
    DNX_ALGO_FIELD_ACTION_SW_IPMF3_FES_STATE_PLACE_IN_FG_INFO,
    DNX_ALGO_FIELD_ACTION_SW_IPMF3_FES_STATE_MASK_IS_ALLOC_INFO,
    DNX_ALGO_FIELD_ACTION_SW_IPMF3_CONTEXT_STATE_INFO,
    DNX_ALGO_FIELD_ACTION_SW_IPMF3_CONTEXT_STATE_PRIORITY_INFO,
    DNX_ALGO_FIELD_ACTION_SW_EPMF_INFO,
    DNX_ALGO_FIELD_ACTION_SW_EPMF_FES_STATE_INFO,
    DNX_ALGO_FIELD_ACTION_SW_EPMF_FES_STATE_FIELD_GROUP_INFO,
    DNX_ALGO_FIELD_ACTION_SW_EPMF_FES_STATE_PLACE_IN_FG_INFO,
    DNX_ALGO_FIELD_ACTION_SW_EPMF_FES_STATE_MASK_IS_ALLOC_INFO,
    DNX_ALGO_FIELD_ACTION_SW_EPMF_CONTEXT_STATE_INFO,
    DNX_ALGO_FIELD_ACTION_SW_EPMF_CONTEXT_STATE_PRIORITY_INFO,
    DNX_ALGO_FIELD_ACTION_SW_INFO_NOF_ENTRIES
} sw_state_dnx_algo_field_action_sw_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_algo_field_action_sw_info[SOC_MAX_NUM_DEVICES][DNX_ALGO_FIELD_ACTION_SW_INFO_NOF_ENTRIES];

extern const char* dnx_algo_field_action_sw_layout_str[DNX_ALGO_FIELD_ACTION_SW_INFO_NOF_ENTRIES];
int dnx_algo_field_action_sw_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_algo_field_action_sw_ipmf2_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_algo_field_action_sw_ipmf2_fes_state_dump(
    int unit, int fes_state_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_field_action_sw_ipmf2_fes_state_field_group_dump(
    int unit, int fes_state_idx_0, int field_group_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_field_action_sw_ipmf2_fes_state_place_in_fg_dump(
    int unit, int fes_state_idx_0, int place_in_fg_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_field_action_sw_ipmf2_fes_state_mask_is_alloc_dump(
    int unit, int fes_state_idx_0, int mask_is_alloc_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_field_action_sw_ipmf2_context_state_dump(
    int unit, int context_state_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_field_action_sw_ipmf2_context_state_priority_dump(
    int unit, int context_state_idx_0, int priority_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_field_action_sw_ipmf3_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_algo_field_action_sw_ipmf3_fes_state_dump(
    int unit, int fes_state_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_field_action_sw_ipmf3_fes_state_field_group_dump(
    int unit, int fes_state_idx_0, int field_group_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_field_action_sw_ipmf3_fes_state_place_in_fg_dump(
    int unit, int fes_state_idx_0, int place_in_fg_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_field_action_sw_ipmf3_fes_state_mask_is_alloc_dump(
    int unit, int fes_state_idx_0, int mask_is_alloc_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_field_action_sw_ipmf3_context_state_dump(
    int unit, int context_state_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_field_action_sw_ipmf3_context_state_priority_dump(
    int unit, int context_state_idx_0, int priority_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_field_action_sw_epmf_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_algo_field_action_sw_epmf_fes_state_dump(
    int unit, int fes_state_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_field_action_sw_epmf_fes_state_field_group_dump(
    int unit, int fes_state_idx_0, int field_group_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_field_action_sw_epmf_fes_state_place_in_fg_dump(
    int unit, int fes_state_idx_0, int place_in_fg_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_field_action_sw_epmf_fes_state_mask_is_alloc_dump(
    int unit, int fes_state_idx_0, int mask_is_alloc_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_field_action_sw_epmf_context_state_dump(
    int unit, int context_state_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_algo_field_action_sw_epmf_context_state_priority_dump(
    int unit, int context_state_idx_0, int priority_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
