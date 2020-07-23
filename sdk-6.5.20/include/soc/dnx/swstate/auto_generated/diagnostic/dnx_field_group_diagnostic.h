
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_GROUP_DIAGNOSTIC_H__
#define __DNX_FIELD_GROUP_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_group_types.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <include/soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <include/soc/dnx/dbal/dbal_structures.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_max_elk.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_actions_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_key_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_FIELD_GROUP_SW_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_FIELD_STAGE_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_FG_TYPE_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_NAME_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_NAME_VALUE_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_CONTEXT_INFO_VALID_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_ID_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_FULL_KEY_ALLOCATED_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_BIT_RANGE_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_VALID_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_OFFSET_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_KEY_ID_BIT_RANGE_BIT_RANGE_SIZE_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_QUAL_ATTACH_INFO_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_QUAL_ATTACH_INFO_INPUT_TYPE_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_QUAL_ATTACH_INFO_INPUT_ARG_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_QUAL_ATTACH_INFO_OFFSET_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_NOF_CASCADING_REFS_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_PAYLOAD_OFFSET_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_PAYLOAD_ID_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_CONTEXT_INFO_FLAGS_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_KEY_TEMPLATE_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_QUAL_TYPE_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_LSB_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_KEY_TEMPLATE_KEY_QUAL_MAP_SIZE_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_KEY_TEMPLATE_KEY_SIZE_IN_BITS_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_KEY_LENGTH_TYPE_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_ACTION_LENGTH_TYPE_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_ACTIONS_PAYLOAD_INFO_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_DNX_ACTION_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_DONT_USE_VALID_BIT_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_LSB_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_DBAL_TABLE_ID_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_FLAGS_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_TCAM_INFO_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_TCAM_INFO_AUTO_TCAM_BANK_SELECT_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_TCAM_INFO_NOF_TCAM_BANKS_INFO,
    DNX_FIELD_GROUP_SW_FG_INFO_TCAM_INFO_TCAM_BANK_IDS_INFO,
    DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_INFO,
    DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_ACTIONS_PAYLOAD_INFO_INFO,
    DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_INFO,
    DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_DNX_ACTION_INFO,
    DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_DONT_USE_VALID_BIT_INFO,
    DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_ACTIONS_PAYLOAD_INFO_ACTIONS_ON_PAYLOAD_INFO_LSB_INFO,
    DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_FLAGS_INFO,
    DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_NAME_INFO,
    DNX_FIELD_GROUP_SW_ACE_FORMAT_INFO_NAME_VALUE_INFO,
    DNX_FIELD_GROUP_SW_KBP_INFO_INFO,
    DNX_FIELD_GROUP_SW_KBP_INFO_MASTER_KEY_INFO_INFO,
    DNX_FIELD_GROUP_SW_KBP_INFO_MASTER_KEY_INFO_SEGMENT_INFO_INFO,
    DNX_FIELD_GROUP_SW_KBP_INFO_MASTER_KEY_INFO_SEGMENT_INFO_QUAL_IDX_INFO,
    DNX_FIELD_GROUP_SW_KBP_INFO_MASTER_KEY_INFO_SEGMENT_INFO_FG_ID_INFO,
    DNX_FIELD_GROUP_SW_KBP_INFO_MASTER_KEY_INFO_NOF_FWD_SEGMENTS_INFO,
    DNX_FIELD_GROUP_SW_KBP_INFO_IS_VALID_INFO,
    DNX_FIELD_GROUP_SW_INFO_NOF_ENTRIES
} sw_state_dnx_field_group_sw_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_field_group_sw_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_GROUP_SW_INFO_NOF_ENTRIES];

extern const char* dnx_field_group_sw_layout_str[DNX_FIELD_GROUP_SW_INFO_NOF_ENTRIES];
int dnx_field_group_sw_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_dump(
    int unit, int fg_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_field_stage_dump(
    int unit, int fg_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_fg_type_dump(
    int unit, int fg_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_name_dump(
    int unit, int fg_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_name_value_dump(
    int unit, int fg_info_idx_0, int value_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_context_info_dump(
    int unit, int fg_info_idx_0, int context_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_context_info_context_info_valid_dump(
    int unit, int fg_info_idx_0, int context_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_context_info_key_id_dump(
    int unit, int fg_info_idx_0, int context_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_context_info_key_id_id_dump(
    int unit, int fg_info_idx_0, int context_info_idx_0, int id_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_context_info_key_id_full_key_allocated_dump(
    int unit, int fg_info_idx_0, int context_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_context_info_key_id_bit_range_dump(
    int unit, int fg_info_idx_0, int context_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_valid_dump(
    int unit, int fg_info_idx_0, int context_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_offset_dump(
    int unit, int fg_info_idx_0, int context_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_size_dump(
    int unit, int fg_info_idx_0, int context_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_context_info_qual_attach_info_dump(
    int unit, int fg_info_idx_0, int context_info_idx_0, int qual_attach_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_type_dump(
    int unit, int fg_info_idx_0, int context_info_idx_0, int qual_attach_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_arg_dump(
    int unit, int fg_info_idx_0, int context_info_idx_0, int qual_attach_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_context_info_qual_attach_info_offset_dump(
    int unit, int fg_info_idx_0, int context_info_idx_0, int qual_attach_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_context_info_nof_cascading_refs_dump(
    int unit, int fg_info_idx_0, int context_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_context_info_payload_offset_dump(
    int unit, int fg_info_idx_0, int context_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_context_info_payload_id_dump(
    int unit, int fg_info_idx_0, int context_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_context_info_flags_dump(
    int unit, int fg_info_idx_0, int context_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_key_template_dump(
    int unit, int fg_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_key_template_key_qual_map_dump(
    int unit, int fg_info_idx_0, int key_qual_map_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_key_template_key_qual_map_qual_type_dump(
    int unit, int fg_info_idx_0, int key_qual_map_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_key_template_key_qual_map_lsb_dump(
    int unit, int fg_info_idx_0, int key_qual_map_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_key_template_key_qual_map_size_dump(
    int unit, int fg_info_idx_0, int key_qual_map_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_key_template_key_size_in_bits_dump(
    int unit, int fg_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_key_length_type_dump(
    int unit, int fg_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_action_length_type_dump(
    int unit, int fg_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_actions_payload_info_dump(
    int unit, int fg_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dump(
    int unit, int fg_info_idx_0, int actions_on_payload_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dnx_action_dump(
    int unit, int fg_info_idx_0, int actions_on_payload_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_dump(
    int unit, int fg_info_idx_0, int actions_on_payload_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_lsb_dump(
    int unit, int fg_info_idx_0, int actions_on_payload_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_dbal_table_id_dump(
    int unit, int fg_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_flags_dump(
    int unit, int fg_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_tcam_info_dump(
    int unit, int fg_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_tcam_info_auto_tcam_bank_select_dump(
    int unit, int fg_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_tcam_info_nof_tcam_banks_dump(
    int unit, int fg_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_dump(
    int unit, int fg_info_idx_0, int tcam_bank_ids_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_ace_format_info_dump(
    int unit, int ace_format_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_ace_format_info_actions_payload_info_dump(
    int unit, int ace_format_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dump(
    int unit, int ace_format_info_idx_0, int actions_on_payload_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dnx_action_dump(
    int unit, int ace_format_info_idx_0, int actions_on_payload_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_dump(
    int unit, int ace_format_info_idx_0, int actions_on_payload_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_lsb_dump(
    int unit, int ace_format_info_idx_0, int actions_on_payload_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_ace_format_info_flags_dump(
    int unit, int ace_format_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_ace_format_info_name_dump(
    int unit, int ace_format_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_ace_format_info_name_value_dump(
    int unit, int ace_format_info_idx_0, int value_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_kbp_info_dump(
    int unit, int kbp_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_kbp_info_master_key_info_dump(
    int unit, int kbp_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_kbp_info_master_key_info_segment_info_dump(
    int unit, int kbp_info_idx_0, int segment_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_kbp_info_master_key_info_segment_info_qual_idx_dump(
    int unit, int kbp_info_idx_0, int segment_info_idx_0, int qual_idx_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_kbp_info_master_key_info_segment_info_fg_id_dump(
    int unit, int kbp_info_idx_0, int segment_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_kbp_info_master_key_info_nof_fwd_segments_dump(
    int unit, int kbp_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_group_sw_kbp_info_is_valid_dump(
    int unit, int kbp_info_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
