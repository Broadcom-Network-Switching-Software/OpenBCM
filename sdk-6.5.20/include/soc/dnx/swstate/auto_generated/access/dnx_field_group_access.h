
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_GROUP_ACCESS_H__
#define __DNX_FIELD_GROUP_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_group_types.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <include/soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <include/soc/dnx/dbal/dbal_structures.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_max_elk.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_actions_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_key_types.h>



typedef int (*dnx_field_group_sw_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_field_group_sw_init_cb)(
    int unit);


typedef int (*dnx_field_group_sw_fg_info_alloc_cb)(
    int unit);


typedef int (*dnx_field_group_sw_fg_info_field_stage_set_cb)(
    int unit, uint32 fg_info_idx_0, dnx_field_stage_e field_stage);


typedef int (*dnx_field_group_sw_fg_info_field_stage_get_cb)(
    int unit, uint32 fg_info_idx_0, dnx_field_stage_e *field_stage);


typedef int (*dnx_field_group_sw_fg_info_fg_type_set_cb)(
    int unit, uint32 fg_info_idx_0, dnx_field_group_type_e fg_type);


typedef int (*dnx_field_group_sw_fg_info_fg_type_get_cb)(
    int unit, uint32 fg_info_idx_0, dnx_field_group_type_e *fg_type);


typedef int (*dnx_field_group_sw_fg_info_name_set_cb)(
    int unit, uint32 fg_info_idx_0, CONST field_group_name_t *name);


typedef int (*dnx_field_group_sw_fg_info_name_get_cb)(
    int unit, uint32 fg_info_idx_0, field_group_name_t *name);


typedef int (*dnx_field_group_sw_fg_info_name_value_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 value_idx_0, char value);


typedef int (*dnx_field_group_sw_fg_info_name_value_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 value_idx_0, char *value);


typedef int (*dnx_field_group_sw_fg_info_context_info_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, CONST dnx_field_attach_context_info_t *context_info);


typedef int (*dnx_field_group_sw_fg_info_context_info_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, dnx_field_attach_context_info_t *context_info);


typedef int (*dnx_field_group_sw_fg_info_context_info_context_info_valid_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 context_info_valid);


typedef int (*dnx_field_group_sw_fg_info_context_info_context_info_valid_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 *context_info_valid);


typedef int (*dnx_field_group_sw_fg_info_context_info_key_id_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, CONST dnx_field_key_id_t *key_id);


typedef int (*dnx_field_group_sw_fg_info_context_info_key_id_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, dnx_field_key_id_t *key_id);


typedef int (*dnx_field_group_sw_fg_info_context_info_key_id_id_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e id);


typedef int (*dnx_field_group_sw_fg_info_context_info_key_id_id_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e *id);


typedef int (*dnx_field_group_sw_fg_info_context_info_key_id_full_key_allocated_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 full_key_allocated);


typedef int (*dnx_field_group_sw_fg_info_context_info_key_id_full_key_allocated_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 *full_key_allocated);


typedef int (*dnx_field_group_sw_fg_info_context_info_key_id_bit_range_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, CONST dnx_field_bit_range_t *bit_range);


typedef int (*dnx_field_group_sw_fg_info_context_info_key_id_bit_range_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, dnx_field_bit_range_t *bit_range);


typedef int (*dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_valid_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 bit_range_valid);


typedef int (*dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_valid_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 *bit_range_valid);


typedef int (*dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_offset_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 bit_range_offset);


typedef int (*dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_offset_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 *bit_range_offset);


typedef int (*dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_size_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 bit_range_size);


typedef int (*dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_size_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint8 *bit_range_size);


typedef int (*dnx_field_group_sw_fg_info_context_info_qual_attach_info_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 qual_attach_info_idx_0, CONST dnx_field_qual_attach_info_t *qual_attach_info);


typedef int (*dnx_field_group_sw_fg_info_context_info_qual_attach_info_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 qual_attach_info_idx_0, dnx_field_qual_attach_info_t *qual_attach_info);


typedef int (*dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_type_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 qual_attach_info_idx_0, dnx_field_input_type_e input_type);


typedef int (*dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_type_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 qual_attach_info_idx_0, dnx_field_input_type_e *input_type);


typedef int (*dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_arg_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 qual_attach_info_idx_0, int input_arg);


typedef int (*dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_arg_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 qual_attach_info_idx_0, int *input_arg);


typedef int (*dnx_field_group_sw_fg_info_context_info_qual_attach_info_offset_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 qual_attach_info_idx_0, int offset);


typedef int (*dnx_field_group_sw_fg_info_context_info_qual_attach_info_offset_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 qual_attach_info_idx_0, int *offset);


typedef int (*dnx_field_group_sw_fg_info_context_info_nof_cascading_refs_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint16 nof_cascading_refs);


typedef int (*dnx_field_group_sw_fg_info_context_info_nof_cascading_refs_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint16 *nof_cascading_refs);


typedef int (*dnx_field_group_sw_fg_info_context_info_payload_offset_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 payload_offset);


typedef int (*dnx_field_group_sw_fg_info_context_info_payload_offset_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 *payload_offset);


typedef int (*dnx_field_group_sw_fg_info_context_info_payload_id_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 payload_id);


typedef int (*dnx_field_group_sw_fg_info_context_info_payload_id_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, uint32 *payload_id);


typedef int (*dnx_field_group_sw_fg_info_context_info_flags_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, dnx_field_group_context_attach_flags_e flags);


typedef int (*dnx_field_group_sw_fg_info_context_info_flags_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 context_info_idx_0, dnx_field_group_context_attach_flags_e *flags);


typedef int (*dnx_field_group_sw_fg_info_key_template_set_cb)(
    int unit, uint32 fg_info_idx_0, CONST dnx_field_key_template_t *key_template);


typedef int (*dnx_field_group_sw_fg_info_key_template_get_cb)(
    int unit, uint32 fg_info_idx_0, dnx_field_key_template_t *key_template);


typedef int (*dnx_field_group_sw_fg_info_key_template_key_qual_map_qual_type_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t qual_type);


typedef int (*dnx_field_group_sw_fg_info_key_template_key_qual_map_qual_type_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t *qual_type);


typedef int (*dnx_field_group_sw_fg_info_key_template_key_qual_map_lsb_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 key_qual_map_idx_0, uint16 lsb);


typedef int (*dnx_field_group_sw_fg_info_key_template_key_qual_map_lsb_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 key_qual_map_idx_0, uint16 *lsb);


typedef int (*dnx_field_group_sw_fg_info_key_template_key_qual_map_size_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 key_qual_map_idx_0, uint8 size);


typedef int (*dnx_field_group_sw_fg_info_key_template_key_qual_map_size_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 key_qual_map_idx_0, uint8 *size);


typedef int (*dnx_field_group_sw_fg_info_key_length_type_set_cb)(
    int unit, uint32 fg_info_idx_0, dnx_field_key_length_type_e key_length_type);


typedef int (*dnx_field_group_sw_fg_info_key_length_type_get_cb)(
    int unit, uint32 fg_info_idx_0, dnx_field_key_length_type_e *key_length_type);


typedef int (*dnx_field_group_sw_fg_info_action_length_type_set_cb)(
    int unit, uint32 fg_info_idx_0, dnx_field_action_length_type_e action_length_type);


typedef int (*dnx_field_group_sw_fg_info_action_length_type_get_cb)(
    int unit, uint32 fg_info_idx_0, dnx_field_action_length_type_e *action_length_type);


typedef int (*dnx_field_group_sw_fg_info_actions_payload_info_set_cb)(
    int unit, uint32 fg_info_idx_0, CONST dnx_field_actions_fg_payload_sw_info_t *actions_payload_info);


typedef int (*dnx_field_group_sw_fg_info_actions_payload_info_get_cb)(
    int unit, uint32 fg_info_idx_0, dnx_field_actions_fg_payload_sw_info_t *actions_payload_info);


typedef int (*dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 actions_on_payload_info_idx_0, CONST dnx_field_action_in_group_info_t *actions_on_payload_info);


typedef int (*dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 actions_on_payload_info_idx_0, dnx_field_action_in_group_info_t *actions_on_payload_info);


typedef int (*dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dnx_action_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 actions_on_payload_info_idx_0, dnx_field_action_t dnx_action);


typedef int (*dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dnx_action_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 actions_on_payload_info_idx_0, dnx_field_action_t *dnx_action);


typedef int (*dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 actions_on_payload_info_idx_0, uint8 dont_use_valid_bit);


typedef int (*dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 actions_on_payload_info_idx_0, uint8 *dont_use_valid_bit);


typedef int (*dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_lsb_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 actions_on_payload_info_idx_0, uint8 lsb);


typedef int (*dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_lsb_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 actions_on_payload_info_idx_0, uint8 *lsb);


typedef int (*dnx_field_group_sw_fg_info_dbal_table_id_set_cb)(
    int unit, uint32 fg_info_idx_0, dbal_tables_e dbal_table_id);


typedef int (*dnx_field_group_sw_fg_info_dbal_table_id_get_cb)(
    int unit, uint32 fg_info_idx_0, dbal_tables_e *dbal_table_id);


typedef int (*dnx_field_group_sw_fg_info_flags_set_cb)(
    int unit, uint32 fg_info_idx_0, dnx_field_group_add_flags_e flags);


typedef int (*dnx_field_group_sw_fg_info_flags_get_cb)(
    int unit, uint32 fg_info_idx_0, dnx_field_group_add_flags_e *flags);


typedef int (*dnx_field_group_sw_fg_info_tcam_info_auto_tcam_bank_select_set_cb)(
    int unit, uint32 fg_info_idx_0, int auto_tcam_bank_select);


typedef int (*dnx_field_group_sw_fg_info_tcam_info_auto_tcam_bank_select_get_cb)(
    int unit, uint32 fg_info_idx_0, int *auto_tcam_bank_select);


typedef int (*dnx_field_group_sw_fg_info_tcam_info_nof_tcam_banks_set_cb)(
    int unit, uint32 fg_info_idx_0, int nof_tcam_banks);


typedef int (*dnx_field_group_sw_fg_info_tcam_info_nof_tcam_banks_get_cb)(
    int unit, uint32 fg_info_idx_0, int *nof_tcam_banks);


typedef int (*dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_set_cb)(
    int unit, uint32 fg_info_idx_0, uint32 tcam_bank_ids_idx_0, int tcam_bank_ids);


typedef int (*dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_get_cb)(
    int unit, uint32 fg_info_idx_0, uint32 tcam_bank_ids_idx_0, int *tcam_bank_ids);


typedef int (*dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_alloc_cb)(
    int unit, uint32 fg_info_idx_0);


typedef int (*dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_is_allocated_cb)(
    int unit, uint32 fg_info_idx_0, uint8 *is_allocated);


typedef int (*dnx_field_group_sw_ace_format_info_set_cb)(
    int unit, uint32 ace_format_info_idx_0, CONST dnx_field_ace_format_sw_info_t *ace_format_info);


typedef int (*dnx_field_group_sw_ace_format_info_get_cb)(
    int unit, uint32 ace_format_info_idx_0, dnx_field_ace_format_sw_info_t *ace_format_info);


typedef int (*dnx_field_group_sw_ace_format_info_alloc_cb)(
    int unit);


typedef int (*dnx_field_group_sw_ace_format_info_actions_payload_info_set_cb)(
    int unit, uint32 ace_format_info_idx_0, CONST dnx_field_actions_ace_payload_sw_info_t *actions_payload_info);


typedef int (*dnx_field_group_sw_ace_format_info_actions_payload_info_get_cb)(
    int unit, uint32 ace_format_info_idx_0, dnx_field_actions_ace_payload_sw_info_t *actions_payload_info);


typedef int (*dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dnx_action_set_cb)(
    int unit, uint32 ace_format_info_idx_0, uint32 actions_on_payload_info_idx_0, dnx_field_action_t dnx_action);


typedef int (*dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dnx_action_get_cb)(
    int unit, uint32 ace_format_info_idx_0, uint32 actions_on_payload_info_idx_0, dnx_field_action_t *dnx_action);


typedef int (*dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_set_cb)(
    int unit, uint32 ace_format_info_idx_0, uint32 actions_on_payload_info_idx_0, uint8 dont_use_valid_bit);


typedef int (*dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_get_cb)(
    int unit, uint32 ace_format_info_idx_0, uint32 actions_on_payload_info_idx_0, uint8 *dont_use_valid_bit);


typedef int (*dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_lsb_set_cb)(
    int unit, uint32 ace_format_info_idx_0, uint32 actions_on_payload_info_idx_0, uint8 lsb);


typedef int (*dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_lsb_get_cb)(
    int unit, uint32 ace_format_info_idx_0, uint32 actions_on_payload_info_idx_0, uint8 *lsb);


typedef int (*dnx_field_group_sw_ace_format_info_flags_set_cb)(
    int unit, uint32 ace_format_info_idx_0, dnx_field_ace_format_add_flags_e flags);


typedef int (*dnx_field_group_sw_ace_format_info_flags_get_cb)(
    int unit, uint32 ace_format_info_idx_0, dnx_field_ace_format_add_flags_e *flags);


typedef int (*dnx_field_group_sw_ace_format_info_name_set_cb)(
    int unit, uint32 ace_format_info_idx_0, CONST field_ace_format_name_t *name);


typedef int (*dnx_field_group_sw_ace_format_info_name_get_cb)(
    int unit, uint32 ace_format_info_idx_0, field_ace_format_name_t *name);


typedef int (*dnx_field_group_sw_ace_format_info_name_value_set_cb)(
    int unit, uint32 ace_format_info_idx_0, uint32 value_idx_0, char value);


typedef int (*dnx_field_group_sw_ace_format_info_name_value_get_cb)(
    int unit, uint32 ace_format_info_idx_0, uint32 value_idx_0, char *value);


typedef int (*dnx_field_group_sw_kbp_info_set_cb)(
    int unit, uint32 kbp_info_idx_0, CONST dnx_field_group_kbp_info_t *kbp_info);


typedef int (*dnx_field_group_sw_kbp_info_get_cb)(
    int unit, uint32 kbp_info_idx_0, dnx_field_group_kbp_info_t *kbp_info);


typedef int (*dnx_field_group_sw_kbp_info_master_key_info_set_cb)(
    int unit, uint32 kbp_info_idx_0, CONST dnx_field_group_kbp_master_key_info_t *master_key_info);


typedef int (*dnx_field_group_sw_kbp_info_master_key_info_get_cb)(
    int unit, uint32 kbp_info_idx_0, dnx_field_group_kbp_master_key_info_t *master_key_info);


typedef int (*dnx_field_group_sw_kbp_info_master_key_info_segment_info_set_cb)(
    int unit, uint32 kbp_info_idx_0, uint32 segment_info_idx_0, CONST dnx_field_group_kbp_segment_info_t *segment_info);


typedef int (*dnx_field_group_sw_kbp_info_master_key_info_segment_info_get_cb)(
    int unit, uint32 kbp_info_idx_0, uint32 segment_info_idx_0, dnx_field_group_kbp_segment_info_t *segment_info);


typedef int (*dnx_field_group_sw_kbp_info_master_key_info_segment_info_qual_idx_set_cb)(
    int unit, uint32 kbp_info_idx_0, uint32 segment_info_idx_0, uint32 qual_idx_idx_0, uint8 qual_idx);


typedef int (*dnx_field_group_sw_kbp_info_master_key_info_segment_info_qual_idx_get_cb)(
    int unit, uint32 kbp_info_idx_0, uint32 segment_info_idx_0, uint32 qual_idx_idx_0, uint8 *qual_idx);


typedef int (*dnx_field_group_sw_kbp_info_master_key_info_segment_info_fg_id_set_cb)(
    int unit, uint32 kbp_info_idx_0, uint32 segment_info_idx_0, dnx_field_group_t fg_id);


typedef int (*dnx_field_group_sw_kbp_info_master_key_info_segment_info_fg_id_get_cb)(
    int unit, uint32 kbp_info_idx_0, uint32 segment_info_idx_0, dnx_field_group_t *fg_id);


typedef int (*dnx_field_group_sw_kbp_info_master_key_info_nof_fwd_segments_set_cb)(
    int unit, uint32 kbp_info_idx_0, uint8 nof_fwd_segments);


typedef int (*dnx_field_group_sw_kbp_info_master_key_info_nof_fwd_segments_get_cb)(
    int unit, uint32 kbp_info_idx_0, uint8 *nof_fwd_segments);


typedef int (*dnx_field_group_sw_kbp_info_is_valid_set_cb)(
    int unit, uint32 kbp_info_idx_0, uint8 is_valid);


typedef int (*dnx_field_group_sw_kbp_info_is_valid_get_cb)(
    int unit, uint32 kbp_info_idx_0, uint8 *is_valid);




typedef struct {
    dnx_field_group_sw_fg_info_field_stage_set_cb set;
    dnx_field_group_sw_fg_info_field_stage_get_cb get;
} dnx_field_group_sw_fg_info_field_stage_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_fg_type_set_cb set;
    dnx_field_group_sw_fg_info_fg_type_get_cb get;
} dnx_field_group_sw_fg_info_fg_type_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_name_value_set_cb set;
    dnx_field_group_sw_fg_info_name_value_get_cb get;
} dnx_field_group_sw_fg_info_name_value_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_name_set_cb set;
    dnx_field_group_sw_fg_info_name_get_cb get;
    
    dnx_field_group_sw_fg_info_name_value_cbs value;
} dnx_field_group_sw_fg_info_name_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_context_info_context_info_valid_set_cb set;
    dnx_field_group_sw_fg_info_context_info_context_info_valid_get_cb get;
} dnx_field_group_sw_fg_info_context_info_context_info_valid_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_context_info_key_id_id_set_cb set;
    dnx_field_group_sw_fg_info_context_info_key_id_id_get_cb get;
} dnx_field_group_sw_fg_info_context_info_key_id_id_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_context_info_key_id_full_key_allocated_set_cb set;
    dnx_field_group_sw_fg_info_context_info_key_id_full_key_allocated_get_cb get;
} dnx_field_group_sw_fg_info_context_info_key_id_full_key_allocated_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_valid_set_cb set;
    dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_valid_get_cb get;
} dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_valid_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_offset_set_cb set;
    dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_offset_get_cb get;
} dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_offset_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_size_set_cb set;
    dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_size_get_cb get;
} dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_size_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_context_info_key_id_bit_range_set_cb set;
    dnx_field_group_sw_fg_info_context_info_key_id_bit_range_get_cb get;
    
    dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_valid_cbs bit_range_valid;
    
    dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_offset_cbs bit_range_offset;
    
    dnx_field_group_sw_fg_info_context_info_key_id_bit_range_bit_range_size_cbs bit_range_size;
} dnx_field_group_sw_fg_info_context_info_key_id_bit_range_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_context_info_key_id_set_cb set;
    dnx_field_group_sw_fg_info_context_info_key_id_get_cb get;
    
    dnx_field_group_sw_fg_info_context_info_key_id_id_cbs id;
    
    dnx_field_group_sw_fg_info_context_info_key_id_full_key_allocated_cbs full_key_allocated;
    
    dnx_field_group_sw_fg_info_context_info_key_id_bit_range_cbs bit_range;
} dnx_field_group_sw_fg_info_context_info_key_id_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_type_set_cb set;
    dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_type_get_cb get;
} dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_type_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_arg_set_cb set;
    dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_arg_get_cb get;
} dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_arg_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_context_info_qual_attach_info_offset_set_cb set;
    dnx_field_group_sw_fg_info_context_info_qual_attach_info_offset_get_cb get;
} dnx_field_group_sw_fg_info_context_info_qual_attach_info_offset_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_context_info_qual_attach_info_set_cb set;
    dnx_field_group_sw_fg_info_context_info_qual_attach_info_get_cb get;
    
    dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_type_cbs input_type;
    
    dnx_field_group_sw_fg_info_context_info_qual_attach_info_input_arg_cbs input_arg;
    
    dnx_field_group_sw_fg_info_context_info_qual_attach_info_offset_cbs offset;
} dnx_field_group_sw_fg_info_context_info_qual_attach_info_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_context_info_nof_cascading_refs_set_cb set;
    dnx_field_group_sw_fg_info_context_info_nof_cascading_refs_get_cb get;
} dnx_field_group_sw_fg_info_context_info_nof_cascading_refs_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_context_info_payload_offset_set_cb set;
    dnx_field_group_sw_fg_info_context_info_payload_offset_get_cb get;
} dnx_field_group_sw_fg_info_context_info_payload_offset_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_context_info_payload_id_set_cb set;
    dnx_field_group_sw_fg_info_context_info_payload_id_get_cb get;
} dnx_field_group_sw_fg_info_context_info_payload_id_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_context_info_flags_set_cb set;
    dnx_field_group_sw_fg_info_context_info_flags_get_cb get;
} dnx_field_group_sw_fg_info_context_info_flags_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_context_info_set_cb set;
    dnx_field_group_sw_fg_info_context_info_get_cb get;
    
    dnx_field_group_sw_fg_info_context_info_context_info_valid_cbs context_info_valid;
    
    dnx_field_group_sw_fg_info_context_info_key_id_cbs key_id;
    
    dnx_field_group_sw_fg_info_context_info_qual_attach_info_cbs qual_attach_info;
    
    dnx_field_group_sw_fg_info_context_info_nof_cascading_refs_cbs nof_cascading_refs;
    
    dnx_field_group_sw_fg_info_context_info_payload_offset_cbs payload_offset;
    
    dnx_field_group_sw_fg_info_context_info_payload_id_cbs payload_id;
    
    dnx_field_group_sw_fg_info_context_info_flags_cbs flags;
} dnx_field_group_sw_fg_info_context_info_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_key_template_key_qual_map_qual_type_set_cb set;
    dnx_field_group_sw_fg_info_key_template_key_qual_map_qual_type_get_cb get;
} dnx_field_group_sw_fg_info_key_template_key_qual_map_qual_type_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_key_template_key_qual_map_lsb_set_cb set;
    dnx_field_group_sw_fg_info_key_template_key_qual_map_lsb_get_cb get;
} dnx_field_group_sw_fg_info_key_template_key_qual_map_lsb_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_key_template_key_qual_map_size_set_cb set;
    dnx_field_group_sw_fg_info_key_template_key_qual_map_size_get_cb get;
} dnx_field_group_sw_fg_info_key_template_key_qual_map_size_cbs;


typedef struct {
    
    dnx_field_group_sw_fg_info_key_template_key_qual_map_qual_type_cbs qual_type;
    
    dnx_field_group_sw_fg_info_key_template_key_qual_map_lsb_cbs lsb;
    
    dnx_field_group_sw_fg_info_key_template_key_qual_map_size_cbs size;
} dnx_field_group_sw_fg_info_key_template_key_qual_map_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_key_template_set_cb set;
    dnx_field_group_sw_fg_info_key_template_get_cb get;
} dnx_field_group_sw_fg_info_key_template_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_key_length_type_set_cb set;
    dnx_field_group_sw_fg_info_key_length_type_get_cb get;
} dnx_field_group_sw_fg_info_key_length_type_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_action_length_type_set_cb set;
    dnx_field_group_sw_fg_info_action_length_type_get_cb get;
} dnx_field_group_sw_fg_info_action_length_type_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dnx_action_set_cb set;
    dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dnx_action_get_cb get;
} dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dnx_action_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_set_cb set;
    dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_get_cb get;
} dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_lsb_set_cb set;
    dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_lsb_get_cb get;
} dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_lsb_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_set_cb set;
    dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_get_cb get;
    
    dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dnx_action_cbs dnx_action;
    
    dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_cbs dont_use_valid_bit;
    
    dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_lsb_cbs lsb;
} dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_actions_payload_info_set_cb set;
    dnx_field_group_sw_fg_info_actions_payload_info_get_cb get;
    
    dnx_field_group_sw_fg_info_actions_payload_info_actions_on_payload_info_cbs actions_on_payload_info;
} dnx_field_group_sw_fg_info_actions_payload_info_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_dbal_table_id_set_cb set;
    dnx_field_group_sw_fg_info_dbal_table_id_get_cb get;
} dnx_field_group_sw_fg_info_dbal_table_id_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_flags_set_cb set;
    dnx_field_group_sw_fg_info_flags_get_cb get;
} dnx_field_group_sw_fg_info_flags_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_tcam_info_auto_tcam_bank_select_set_cb set;
    dnx_field_group_sw_fg_info_tcam_info_auto_tcam_bank_select_get_cb get;
} dnx_field_group_sw_fg_info_tcam_info_auto_tcam_bank_select_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_tcam_info_nof_tcam_banks_set_cb set;
    dnx_field_group_sw_fg_info_tcam_info_nof_tcam_banks_get_cb get;
} dnx_field_group_sw_fg_info_tcam_info_nof_tcam_banks_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_set_cb set;
    dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_get_cb get;
    dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_alloc_cb alloc;
    dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_is_allocated_cb is_allocated;
} dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_cbs;


typedef struct {
    
    dnx_field_group_sw_fg_info_tcam_info_auto_tcam_bank_select_cbs auto_tcam_bank_select;
    
    dnx_field_group_sw_fg_info_tcam_info_nof_tcam_banks_cbs nof_tcam_banks;
    
    dnx_field_group_sw_fg_info_tcam_info_tcam_bank_ids_cbs tcam_bank_ids;
} dnx_field_group_sw_fg_info_tcam_info_cbs;


typedef struct {
    dnx_field_group_sw_fg_info_alloc_cb alloc;
    
    dnx_field_group_sw_fg_info_field_stage_cbs field_stage;
    
    dnx_field_group_sw_fg_info_fg_type_cbs fg_type;
    
    dnx_field_group_sw_fg_info_name_cbs name;
    
    dnx_field_group_sw_fg_info_context_info_cbs context_info;
    
    dnx_field_group_sw_fg_info_key_template_cbs key_template;
    
    dnx_field_group_sw_fg_info_key_length_type_cbs key_length_type;
    
    dnx_field_group_sw_fg_info_action_length_type_cbs action_length_type;
    
    dnx_field_group_sw_fg_info_actions_payload_info_cbs actions_payload_info;
    
    dnx_field_group_sw_fg_info_dbal_table_id_cbs dbal_table_id;
    
    dnx_field_group_sw_fg_info_flags_cbs flags;
    
    dnx_field_group_sw_fg_info_tcam_info_cbs tcam_info;
} dnx_field_group_sw_fg_info_cbs;


typedef struct {
    dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dnx_action_set_cb set;
    dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dnx_action_get_cb get;
} dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dnx_action_cbs;


typedef struct {
    dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_set_cb set;
    dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_get_cb get;
} dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_cbs;


typedef struct {
    dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_lsb_set_cb set;
    dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_lsb_get_cb get;
} dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_lsb_cbs;


typedef struct {
    
    dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dnx_action_cbs dnx_action;
    
    dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_dont_use_valid_bit_cbs dont_use_valid_bit;
    
    dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_lsb_cbs lsb;
} dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_cbs;


typedef struct {
    dnx_field_group_sw_ace_format_info_actions_payload_info_set_cb set;
    dnx_field_group_sw_ace_format_info_actions_payload_info_get_cb get;
    
    dnx_field_group_sw_ace_format_info_actions_payload_info_actions_on_payload_info_cbs actions_on_payload_info;
} dnx_field_group_sw_ace_format_info_actions_payload_info_cbs;


typedef struct {
    dnx_field_group_sw_ace_format_info_flags_set_cb set;
    dnx_field_group_sw_ace_format_info_flags_get_cb get;
} dnx_field_group_sw_ace_format_info_flags_cbs;


typedef struct {
    dnx_field_group_sw_ace_format_info_name_value_set_cb set;
    dnx_field_group_sw_ace_format_info_name_value_get_cb get;
} dnx_field_group_sw_ace_format_info_name_value_cbs;


typedef struct {
    dnx_field_group_sw_ace_format_info_name_set_cb set;
    dnx_field_group_sw_ace_format_info_name_get_cb get;
    
    dnx_field_group_sw_ace_format_info_name_value_cbs value;
} dnx_field_group_sw_ace_format_info_name_cbs;


typedef struct {
    dnx_field_group_sw_ace_format_info_set_cb set;
    dnx_field_group_sw_ace_format_info_get_cb get;
    dnx_field_group_sw_ace_format_info_alloc_cb alloc;
    
    dnx_field_group_sw_ace_format_info_actions_payload_info_cbs actions_payload_info;
    
    dnx_field_group_sw_ace_format_info_flags_cbs flags;
    
    dnx_field_group_sw_ace_format_info_name_cbs name;
} dnx_field_group_sw_ace_format_info_cbs;


typedef struct {
    dnx_field_group_sw_kbp_info_master_key_info_segment_info_qual_idx_set_cb set;
    dnx_field_group_sw_kbp_info_master_key_info_segment_info_qual_idx_get_cb get;
} dnx_field_group_sw_kbp_info_master_key_info_segment_info_qual_idx_cbs;


typedef struct {
    dnx_field_group_sw_kbp_info_master_key_info_segment_info_fg_id_set_cb set;
    dnx_field_group_sw_kbp_info_master_key_info_segment_info_fg_id_get_cb get;
} dnx_field_group_sw_kbp_info_master_key_info_segment_info_fg_id_cbs;


typedef struct {
    dnx_field_group_sw_kbp_info_master_key_info_segment_info_set_cb set;
    dnx_field_group_sw_kbp_info_master_key_info_segment_info_get_cb get;
    
    dnx_field_group_sw_kbp_info_master_key_info_segment_info_qual_idx_cbs qual_idx;
    
    dnx_field_group_sw_kbp_info_master_key_info_segment_info_fg_id_cbs fg_id;
} dnx_field_group_sw_kbp_info_master_key_info_segment_info_cbs;


typedef struct {
    dnx_field_group_sw_kbp_info_master_key_info_nof_fwd_segments_set_cb set;
    dnx_field_group_sw_kbp_info_master_key_info_nof_fwd_segments_get_cb get;
} dnx_field_group_sw_kbp_info_master_key_info_nof_fwd_segments_cbs;


typedef struct {
    dnx_field_group_sw_kbp_info_master_key_info_set_cb set;
    dnx_field_group_sw_kbp_info_master_key_info_get_cb get;
    
    dnx_field_group_sw_kbp_info_master_key_info_segment_info_cbs segment_info;
    
    dnx_field_group_sw_kbp_info_master_key_info_nof_fwd_segments_cbs nof_fwd_segments;
} dnx_field_group_sw_kbp_info_master_key_info_cbs;


typedef struct {
    dnx_field_group_sw_kbp_info_is_valid_set_cb set;
    dnx_field_group_sw_kbp_info_is_valid_get_cb get;
} dnx_field_group_sw_kbp_info_is_valid_cbs;


typedef struct {
    dnx_field_group_sw_kbp_info_set_cb set;
    dnx_field_group_sw_kbp_info_get_cb get;
    
    dnx_field_group_sw_kbp_info_master_key_info_cbs master_key_info;
    
    dnx_field_group_sw_kbp_info_is_valid_cbs is_valid;
} dnx_field_group_sw_kbp_info_cbs;


typedef struct {
    dnx_field_group_sw_is_init_cb is_init;
    dnx_field_group_sw_init_cb init;
    
    dnx_field_group_sw_fg_info_cbs fg_info;
    
    dnx_field_group_sw_ace_format_info_cbs ace_format_info;
    
    dnx_field_group_sw_kbp_info_cbs kbp_info;
} dnx_field_group_sw_cbs;






const char *
dnx_field_group_add_flags_e_get_name(dnx_field_group_add_flags_e value);



const char *
dnx_field_group_context_attach_flags_e_get_name(dnx_field_group_context_attach_flags_e value);



const char *
dnx_field_group_compare_id_e_get_name(dnx_field_group_compare_id_e value);



const char *
dnx_field_ace_format_add_flags_e_get_name(dnx_field_ace_format_add_flags_e value);




extern dnx_field_group_sw_cbs dnx_field_group_sw;

#endif 
