
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_APPTYPE_DIAGNOSTIC_H__
#define __DNX_FIELD_APPTYPE_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_apptype_types.h>
#include <include/soc/dnx/dbal/dbal_structures.h>
#include <soc/dnxc/swstate/types/sw_state_string.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_FIELD_APPTYPE_SW_INFO,
    DNX_FIELD_APPTYPE_SW_PREDEF_INFO_INFO,
    DNX_FIELD_APPTYPE_SW_PREDEF_INFO_NOF_CONTEXT_IDS_INFO,
    DNX_FIELD_APPTYPE_SW_PREDEF_INFO_CONTEXT_IDS_INFO,
    DNX_FIELD_APPTYPE_SW_PREDEF_INFO_VALID_FOR_KBP_INFO,
    DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_INFO,
    DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_OPCODE_ID_INFO,
    DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_ACL_CONTEXT_INFO,
    DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_BASE_APPTYPE_INFO,
    DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_FLAGS_INFO,
    DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_NAME_INFO,
    DNX_FIELD_APPTYPE_SW_USER_DEF_INFO_PROFILE_ID_INFO,
    DNX_FIELD_APPTYPE_SW_INFO_NOF_ENTRIES
} sw_state_dnx_field_apptype_sw_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_field_apptype_sw_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_APPTYPE_SW_INFO_NOF_ENTRIES];

extern const char* dnx_field_apptype_sw_layout_str[DNX_FIELD_APPTYPE_SW_INFO_NOF_ENTRIES];
int dnx_field_apptype_sw_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_field_apptype_sw_predef_info_dump(
    int unit, int predef_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_apptype_sw_predef_info_nof_context_ids_dump(
    int unit, int predef_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_apptype_sw_predef_info_context_ids_dump(
    int unit, int predef_info_idx_0, int context_ids_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_apptype_sw_predef_info_valid_for_kbp_dump(
    int unit, int predef_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_apptype_sw_user_def_info_dump(
    int unit, int user_def_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_apptype_sw_user_def_info_opcode_id_dump(
    int unit, int user_def_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_apptype_sw_user_def_info_acl_context_dump(
    int unit, int user_def_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_apptype_sw_user_def_info_base_apptype_dump(
    int unit, int user_def_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_apptype_sw_user_def_info_flags_dump(
    int unit, int user_def_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_apptype_sw_user_def_info_name_dump(
    int unit, int user_def_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_apptype_sw_user_def_info_profile_id_dump(
    int unit, int user_def_info_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
