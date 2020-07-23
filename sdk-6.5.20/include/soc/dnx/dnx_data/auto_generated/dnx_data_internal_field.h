

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_FIELD_H_

#define _DNX_DATA_INTERNAL_FIELD_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_field_submodule_base_ipmf1,
    dnx_data_field_submodule_base_ipmf2,
    dnx_data_field_submodule_base_ipmf3,
    dnx_data_field_submodule_base_epmf,
    dnx_data_field_submodule_base_ifwd2,
    dnx_data_field_submodule_stage,
    dnx_data_field_submodule_kbp,
    dnx_data_field_submodule_tcam,
    dnx_data_field_submodule_group,
    dnx_data_field_submodule_efes,
    dnx_data_field_submodule_fem,
    dnx_data_field_submodule_context,
    dnx_data_field_submodule_preselector,
    dnx_data_field_submodule_qual,
    dnx_data_field_submodule_action,
    dnx_data_field_submodule_virtual_wire,
    dnx_data_field_submodule_profile_bits,
    dnx_data_field_submodule_dir_ext,
    dnx_data_field_submodule_state_table,
    dnx_data_field_submodule_map,
    dnx_data_field_submodule_hash,
    dnx_data_field_submodule_udh,
    dnx_data_field_submodule_system_headers,
    dnx_data_field_submodule_exem,
    dnx_data_field_submodule_exem_learn_flush_machine,
    dnx_data_field_submodule_ace,
    dnx_data_field_submodule_entry,
    dnx_data_field_submodule_L4_Ops,
    dnx_data_field_submodule_encoded_qual_actions_offset,
    dnx_data_field_submodule_Compare_operand,
    dnx_data_field_submodule_diag,
    dnx_data_field_submodule_common_max_val,
    dnx_data_field_submodule_init,
    dnx_data_field_submodule_features,
    dnx_data_field_submodule_signal_sizes,
    dnx_data_field_submodule_dnx_data_internal,
    dnx_data_field_submodule_tests,

    
    _dnx_data_field_submodule_nof
} dnx_data_field_submodule_e;








int dnx_data_field_base_ipmf1_feature_get(
    int unit,
    dnx_data_field_base_ipmf1_feature_e feature);



typedef enum
{
    dnx_data_field_base_ipmf1_define_nof_ffc,
    dnx_data_field_base_ipmf1_define_nof_ffc_groups,
    dnx_data_field_base_ipmf1_define_ffc_group_one_lower,
    dnx_data_field_base_ipmf1_define_ffc_group_one_upper,
    dnx_data_field_base_ipmf1_define_ffc_group_two_lower,
    dnx_data_field_base_ipmf1_define_ffc_group_two_upper,
    dnx_data_field_base_ipmf1_define_ffc_group_three_lower,
    dnx_data_field_base_ipmf1_define_ffc_group_three_upper,
    dnx_data_field_base_ipmf1_define_ffc_group_four_lower,
    dnx_data_field_base_ipmf1_define_ffc_group_four_upper,
    dnx_data_field_base_ipmf1_define_nof_keys,
    dnx_data_field_base_ipmf1_define_nof_keys_alloc,
    dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_tcam,
    dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_exem,
    dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_mdb_dt,
    dnx_data_field_base_ipmf1_define_nof_masks_per_fes,
    dnx_data_field_base_ipmf1_define_nof_fes_id_per_array,
    dnx_data_field_base_ipmf1_define_nof_fes_array,
    dnx_data_field_base_ipmf1_define_nof_fes_instruction_per_context,
    dnx_data_field_base_ipmf1_define_nof_fes_programs,
    dnx_data_field_base_ipmf1_define_nof_prog_per_fes,
    dnx_data_field_base_ipmf1_define_program_selection_cam_mask_nof_bits,
    dnx_data_field_base_ipmf1_define_cs_container_5_selected_bits_size,
    dnx_data_field_base_ipmf1_define_nof_contexts,
    dnx_data_field_base_ipmf1_define_nof_link_profiles,
    dnx_data_field_base_ipmf1_define_nof_cs_lines,
    dnx_data_field_base_ipmf1_define_nof_actions,
    dnx_data_field_base_ipmf1_define_nof_qualifiers,
    dnx_data_field_base_ipmf1_define_nof_80B_zones,
    dnx_data_field_base_ipmf1_define_nof_key_zones,
    dnx_data_field_base_ipmf1_define_nof_key_zone_bits,
    dnx_data_field_base_ipmf1_define_nof_bits_in_fes_action,
    dnx_data_field_base_ipmf1_define_nof_bits_in_fes_key_select,
    dnx_data_field_base_ipmf1_define_nof_fes_key_selects_on_one_actions_line,
    dnx_data_field_base_ipmf1_define_fem_condition_ms_bit_min_value,
    dnx_data_field_base_ipmf1_define_fem_condition_ms_bit_max_value,
    dnx_data_field_base_ipmf1_define_nof_bits_in_fem_programs,
    dnx_data_field_base_ipmf1_define_nof_fem_programs,
    dnx_data_field_base_ipmf1_define_log_nof_bits_in_fem_key_select,
    dnx_data_field_base_ipmf1_define_nof_bits_in_fem_key_select,
    dnx_data_field_base_ipmf1_define_fem_key_select_resolution_in_bits,
    dnx_data_field_base_ipmf1_define_log_nof_bits_in_fem_map_data_field,
    dnx_data_field_base_ipmf1_define_nof_bits_in_fem_map_data_field,
    dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action_fems_2_15,
    dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action_fems_0_1,
    dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action,
    dnx_data_field_base_ipmf1_define_nof_bits_in_fem_condition,
    dnx_data_field_base_ipmf1_define_nof_fem_condition,
    dnx_data_field_base_ipmf1_define_nof_bits_in_fem_map_index,
    dnx_data_field_base_ipmf1_define_nof_fem_map_index,
    dnx_data_field_base_ipmf1_define_nof_bits_in_fem_id,
    dnx_data_field_base_ipmf1_define_nof_fem_id,
    dnx_data_field_base_ipmf1_define_num_fems_with_short_action,
    dnx_data_field_base_ipmf1_define_num_bits_in_fem_field_select,
    dnx_data_field_base_ipmf1_define_nof_fem_id_per_array,
    dnx_data_field_base_ipmf1_define_nof_fem_array,
    dnx_data_field_base_ipmf1_define_nof_fems_per_context,
    dnx_data_field_base_ipmf1_define_nof_fem_action_overriding_bits,
    dnx_data_field_base_ipmf1_define_default_strength,
    dnx_data_field_base_ipmf1_define_nof_compare_pairs_in_compare_mode,
    dnx_data_field_base_ipmf1_define_nof_compare_keys_in_compare_mode,
    dnx_data_field_base_ipmf1_define_compare_key_size,
    dnx_data_field_base_ipmf1_define_nof_l4_ops_ranges_legacy,
    dnx_data_field_base_ipmf1_define_nof_ext_l4_ops_ranges,
    dnx_data_field_base_ipmf1_define_nof_pkt_hdr_ranges,
    dnx_data_field_base_ipmf1_define_nof_out_lif_ranges,
    dnx_data_field_base_ipmf1_define_fes_key_select_for_zero_bit,
    dnx_data_field_base_ipmf1_define_fes_shift_for_zero_bit,
    dnx_data_field_base_ipmf1_define_uses_small_exem,
    dnx_data_field_base_ipmf1_define_uses_large_exem,
    dnx_data_field_base_ipmf1_define_cmp_selection,
    dnx_data_field_base_ipmf1_define_fes_instruction_size,
    dnx_data_field_base_ipmf1_define_fes_pgm_id_offset,
    dnx_data_field_base_ipmf1_define_dir_ext_single_key_size,

    
    _dnx_data_field_base_ipmf1_define_nof
} dnx_data_field_base_ipmf1_define_e;



uint32 dnx_data_field_base_ipmf1_nof_ffc_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_ffc_groups_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_ffc_group_one_lower_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_ffc_group_one_upper_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_ffc_group_two_lower_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_ffc_group_two_upper_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_ffc_group_three_lower_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_ffc_group_three_upper_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_ffc_group_four_lower_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_ffc_group_four_upper_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_keys_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_keys_alloc_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_keys_alloc_for_tcam_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_keys_alloc_for_exem_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_keys_alloc_for_mdb_dt_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_masks_per_fes_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_fes_id_per_array_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_fes_array_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_fes_instruction_per_context_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_fes_programs_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_prog_per_fes_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_program_selection_cam_mask_nof_bits_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_cs_container_5_selected_bits_size_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_contexts_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_link_profiles_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_cs_lines_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_actions_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_qualifiers_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_80B_zones_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_key_zones_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_key_zone_bits_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_bits_in_fes_action_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_bits_in_fes_key_select_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_fes_key_selects_on_one_actions_line_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_fem_condition_ms_bit_min_value_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_fem_condition_ms_bit_max_value_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_bits_in_fem_programs_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_fem_programs_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_log_nof_bits_in_fem_key_select_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_bits_in_fem_key_select_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_fem_key_select_resolution_in_bits_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_log_nof_bits_in_fem_map_data_field_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_bits_in_fem_map_data_field_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_bits_in_fem_action_fems_2_15_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_bits_in_fem_action_fems_0_1_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_bits_in_fem_action_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_bits_in_fem_condition_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_fem_condition_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_bits_in_fem_map_index_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_fem_map_index_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_bits_in_fem_id_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_fem_id_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_num_fems_with_short_action_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_num_bits_in_fem_field_select_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_fem_id_per_array_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_fem_array_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_fems_per_context_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_fem_action_overriding_bits_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_default_strength_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_compare_pairs_in_compare_mode_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_compare_keys_in_compare_mode_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_compare_key_size_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_l4_ops_ranges_legacy_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_ext_l4_ops_ranges_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_pkt_hdr_ranges_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_out_lif_ranges_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_fes_key_select_for_zero_bit_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_fes_shift_for_zero_bit_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_uses_small_exem_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_uses_large_exem_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_cmp_selection_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_fes_instruction_size_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_fes_pgm_id_offset_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_dir_ext_single_key_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_base_ipmf1_table_nof
} dnx_data_field_base_ipmf1_table_e;









int dnx_data_field_base_ipmf2_feature_get(
    int unit,
    dnx_data_field_base_ipmf2_feature_e feature);



typedef enum
{
    dnx_data_field_base_ipmf2_define_nof_ffc,
    dnx_data_field_base_ipmf2_define_nof_ffc_groups,
    dnx_data_field_base_ipmf2_define_ffc_group_one_lower,
    dnx_data_field_base_ipmf2_define_ffc_group_one_upper,
    dnx_data_field_base_ipmf2_define_nof_keys,
    dnx_data_field_base_ipmf2_define_nof_keys_alloc,
    dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_tcam,
    dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_exem,
    dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_dir_ext,
    dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_mdb_dt,
    dnx_data_field_base_ipmf2_define_nof_masks_per_fes,
    dnx_data_field_base_ipmf2_define_nof_fes_id_per_array,
    dnx_data_field_base_ipmf2_define_nof_fes_array,
    dnx_data_field_base_ipmf2_define_nof_fes_instruction_per_context,
    dnx_data_field_base_ipmf2_define_nof_cs_lines,
    dnx_data_field_base_ipmf2_define_nof_contexts,
    dnx_data_field_base_ipmf2_define_program_selection_cam_mask_nof_bits,
    dnx_data_field_base_ipmf2_define_nof_qualifiers,
    dnx_data_field_base_ipmf2_define_nof_80B_zones,
    dnx_data_field_base_ipmf2_define_nof_key_zones,
    dnx_data_field_base_ipmf2_define_nof_key_zone_bits,
    dnx_data_field_base_ipmf2_define_fes_key_select_for_zero_bit,
    dnx_data_field_base_ipmf2_define_fes_shift_for_zero_bit,
    dnx_data_field_base_ipmf2_define_uses_small_exem,
    dnx_data_field_base_ipmf2_define_uses_large_exem,
    dnx_data_field_base_ipmf2_define_dir_ext_single_key_size,

    
    _dnx_data_field_base_ipmf2_define_nof
} dnx_data_field_base_ipmf2_define_e;



uint32 dnx_data_field_base_ipmf2_nof_ffc_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_nof_ffc_groups_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_ffc_group_one_lower_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_ffc_group_one_upper_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_nof_keys_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_nof_keys_alloc_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_nof_keys_alloc_for_tcam_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_nof_keys_alloc_for_exem_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_nof_keys_alloc_for_dir_ext_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_nof_keys_alloc_for_mdb_dt_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_nof_masks_per_fes_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_nof_fes_id_per_array_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_nof_fes_array_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_nof_fes_instruction_per_context_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_nof_cs_lines_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_nof_contexts_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_program_selection_cam_mask_nof_bits_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_nof_qualifiers_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_nof_80B_zones_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_nof_key_zones_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_nof_key_zone_bits_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_fes_key_select_for_zero_bit_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_fes_shift_for_zero_bit_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_uses_small_exem_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_uses_large_exem_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_dir_ext_single_key_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_base_ipmf2_table_nof
} dnx_data_field_base_ipmf2_table_e;









int dnx_data_field_base_ipmf3_feature_get(
    int unit,
    dnx_data_field_base_ipmf3_feature_e feature);



typedef enum
{
    dnx_data_field_base_ipmf3_define_nof_ffc,
    dnx_data_field_base_ipmf3_define_nof_ffc_groups,
    dnx_data_field_base_ipmf3_define_ffc_group_one_lower,
    dnx_data_field_base_ipmf3_define_ffc_group_one_upper,
    dnx_data_field_base_ipmf3_define_nof_keys,
    dnx_data_field_base_ipmf3_define_nof_keys_alloc,
    dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_tcam,
    dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_exem,
    dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_dir_ext,
    dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_mdb_dt,
    dnx_data_field_base_ipmf3_define_nof_masks_per_fes,
    dnx_data_field_base_ipmf3_define_nof_fes_id_per_array,
    dnx_data_field_base_ipmf3_define_nof_fes_array,
    dnx_data_field_base_ipmf3_define_nof_fes_instruction_per_context,
    dnx_data_field_base_ipmf3_define_nof_fes_programs,
    dnx_data_field_base_ipmf3_define_nof_prog_per_fes,
    dnx_data_field_base_ipmf3_define_program_selection_cam_mask_nof_bits,
    dnx_data_field_base_ipmf3_define_cs_scratch_pad_size,
    dnx_data_field_base_ipmf3_define_nof_contexts,
    dnx_data_field_base_ipmf3_define_nof_cs_lines,
    dnx_data_field_base_ipmf3_define_nof_actions,
    dnx_data_field_base_ipmf3_define_nof_qualifiers,
    dnx_data_field_base_ipmf3_define_nof_80B_zones,
    dnx_data_field_base_ipmf3_define_nof_key_zones,
    dnx_data_field_base_ipmf3_define_nof_key_zone_bits,
    dnx_data_field_base_ipmf3_define_nof_bits_in_fes_action,
    dnx_data_field_base_ipmf3_define_nof_bits_in_fes_key_select,
    dnx_data_field_base_ipmf3_define_nof_fes_key_selects_on_one_actions_line,
    dnx_data_field_base_ipmf3_define_default_strength,
    dnx_data_field_base_ipmf3_define_nof_out_lif_ranges,
    dnx_data_field_base_ipmf3_define_fes_key_select_for_zero_bit,
    dnx_data_field_base_ipmf3_define_fes_shift_for_zero_bit,
    dnx_data_field_base_ipmf3_define_uses_small_exem,
    dnx_data_field_base_ipmf3_define_uses_large_exem,
    dnx_data_field_base_ipmf3_define_fes_instruction_size,
    dnx_data_field_base_ipmf3_define_fes_pgm_id_offset,
    dnx_data_field_base_ipmf3_define_dir_ext_single_key_size,
    dnx_data_field_base_ipmf3_define_nof_fes_used_by_sdk,

    
    _dnx_data_field_base_ipmf3_define_nof
} dnx_data_field_base_ipmf3_define_e;



uint32 dnx_data_field_base_ipmf3_nof_ffc_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_ffc_groups_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_ffc_group_one_lower_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_ffc_group_one_upper_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_keys_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_keys_alloc_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_keys_alloc_for_tcam_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_keys_alloc_for_exem_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_keys_alloc_for_dir_ext_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_keys_alloc_for_mdb_dt_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_masks_per_fes_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_fes_id_per_array_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_fes_array_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_fes_instruction_per_context_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_fes_programs_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_prog_per_fes_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_program_selection_cam_mask_nof_bits_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_cs_scratch_pad_size_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_contexts_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_cs_lines_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_actions_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_qualifiers_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_80B_zones_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_key_zones_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_key_zone_bits_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_bits_in_fes_action_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_bits_in_fes_key_select_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_fes_key_selects_on_one_actions_line_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_default_strength_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_out_lif_ranges_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_fes_key_select_for_zero_bit_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_fes_shift_for_zero_bit_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_uses_small_exem_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_uses_large_exem_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_fes_instruction_size_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_fes_pgm_id_offset_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_dir_ext_single_key_size_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_fes_used_by_sdk_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_base_ipmf3_table_nof
} dnx_data_field_base_ipmf3_table_e;









int dnx_data_field_base_epmf_feature_get(
    int unit,
    dnx_data_field_base_epmf_feature_e feature);



typedef enum
{
    dnx_data_field_base_epmf_define_nof_ffc,
    dnx_data_field_base_epmf_define_nof_ffc_groups,
    dnx_data_field_base_epmf_define_ffc_group_one_lower,
    dnx_data_field_base_epmf_define_ffc_group_one_upper,
    dnx_data_field_base_epmf_define_nof_keys,
    dnx_data_field_base_epmf_define_nof_keys_alloc,
    dnx_data_field_base_epmf_define_nof_keys_alloc_for_tcam,
    dnx_data_field_base_epmf_define_nof_keys_alloc_for_exem,
    dnx_data_field_base_epmf_define_nof_keys_alloc_for_dir_ext,
    dnx_data_field_base_epmf_define_nof_masks_per_fes,
    dnx_data_field_base_epmf_define_nof_fes_id_per_array,
    dnx_data_field_base_epmf_define_nof_fes_array,
    dnx_data_field_base_epmf_define_nof_fes_instruction_per_context,
    dnx_data_field_base_epmf_define_nof_fes_programs,
    dnx_data_field_base_epmf_define_nof_prog_per_fes,
    dnx_data_field_base_epmf_define_program_selection_cam_mask_nof_bits,
    dnx_data_field_base_epmf_define_nof_cs_lines,
    dnx_data_field_base_epmf_define_nof_contexts,
    dnx_data_field_base_epmf_define_nof_actions,
    dnx_data_field_base_epmf_define_nof_qualifiers,
    dnx_data_field_base_epmf_define_nof_80B_zones,
    dnx_data_field_base_epmf_define_nof_key_zones,
    dnx_data_field_base_epmf_define_nof_key_zone_bits,
    dnx_data_field_base_epmf_define_nof_bits_in_fes_action,
    dnx_data_field_base_epmf_define_nof_bits_in_fes_key_select,
    dnx_data_field_base_epmf_define_nof_fes_key_selects_on_one_actions_line,
    dnx_data_field_base_epmf_define_nof_l4_ops_ranges_legacy,
    dnx_data_field_base_epmf_define_fes_key_select_for_zero_bit,
    dnx_data_field_base_epmf_define_fes_shift_for_zero_bit,
    dnx_data_field_base_epmf_define_uses_small_exem,
    dnx_data_field_base_epmf_define_uses_large_exem,
    dnx_data_field_base_epmf_define_fes_instruction_size,
    dnx_data_field_base_epmf_define_fes_pgm_id_offset,
    dnx_data_field_base_epmf_define_dir_ext_single_key_size,

    
    _dnx_data_field_base_epmf_define_nof
} dnx_data_field_base_epmf_define_e;



uint32 dnx_data_field_base_epmf_nof_ffc_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_ffc_groups_get(
    int unit);


uint32 dnx_data_field_base_epmf_ffc_group_one_lower_get(
    int unit);


uint32 dnx_data_field_base_epmf_ffc_group_one_upper_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_keys_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_keys_alloc_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_keys_alloc_for_tcam_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_keys_alloc_for_exem_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_keys_alloc_for_dir_ext_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_masks_per_fes_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_fes_id_per_array_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_fes_array_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_fes_instruction_per_context_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_fes_programs_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_prog_per_fes_get(
    int unit);


uint32 dnx_data_field_base_epmf_program_selection_cam_mask_nof_bits_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_cs_lines_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_contexts_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_actions_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_qualifiers_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_80B_zones_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_key_zones_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_key_zone_bits_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_bits_in_fes_action_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_bits_in_fes_key_select_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_fes_key_selects_on_one_actions_line_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_l4_ops_ranges_legacy_get(
    int unit);


uint32 dnx_data_field_base_epmf_fes_key_select_for_zero_bit_get(
    int unit);


uint32 dnx_data_field_base_epmf_fes_shift_for_zero_bit_get(
    int unit);


uint32 dnx_data_field_base_epmf_uses_small_exem_get(
    int unit);


uint32 dnx_data_field_base_epmf_uses_large_exem_get(
    int unit);


uint32 dnx_data_field_base_epmf_fes_instruction_size_get(
    int unit);


uint32 dnx_data_field_base_epmf_fes_pgm_id_offset_get(
    int unit);


uint32 dnx_data_field_base_epmf_dir_ext_single_key_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_base_epmf_table_nof
} dnx_data_field_base_epmf_table_e;









int dnx_data_field_base_ifwd2_feature_get(
    int unit,
    dnx_data_field_base_ifwd2_feature_e feature);



typedef enum
{
    dnx_data_field_base_ifwd2_define_nof_ffc,
    dnx_data_field_base_ifwd2_define_nof_ffc_groups,
    dnx_data_field_base_ifwd2_define_ffc_group_one_lower,
    dnx_data_field_base_ifwd2_define_ffc_group_one_upper,
    dnx_data_field_base_ifwd2_define_nof_keys,
    dnx_data_field_base_ifwd2_define_nof_contexts,
    dnx_data_field_base_ifwd2_define_nof_cs_lines,

    
    _dnx_data_field_base_ifwd2_define_nof
} dnx_data_field_base_ifwd2_define_e;



uint32 dnx_data_field_base_ifwd2_nof_ffc_get(
    int unit);


uint32 dnx_data_field_base_ifwd2_nof_ffc_groups_get(
    int unit);


uint32 dnx_data_field_base_ifwd2_ffc_group_one_lower_get(
    int unit);


uint32 dnx_data_field_base_ifwd2_ffc_group_one_upper_get(
    int unit);


uint32 dnx_data_field_base_ifwd2_nof_keys_get(
    int unit);


uint32 dnx_data_field_base_ifwd2_nof_contexts_get(
    int unit);


uint32 dnx_data_field_base_ifwd2_nof_cs_lines_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_base_ifwd2_table_nof
} dnx_data_field_base_ifwd2_table_e;









int dnx_data_field_stage_feature_get(
    int unit,
    dnx_data_field_stage_feature_e feature);



typedef enum
{

    
    _dnx_data_field_stage_define_nof
} dnx_data_field_stage_define_e;




typedef enum
{
    dnx_data_field_stage_table_stage_info,

    
    _dnx_data_field_stage_table_nof
} dnx_data_field_stage_table_e;



const dnx_data_field_stage_stage_info_t * dnx_data_field_stage_stage_info_get(
    int unit,
    int stage);



shr_error_e dnx_data_field_stage_stage_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_field_stage_stage_info_info_get(
    int unit);






int dnx_data_field_kbp_feature_get(
    int unit,
    dnx_data_field_kbp_feature_e feature);



typedef enum
{
    dnx_data_field_kbp_define_nof_fgs,
    dnx_data_field_kbp_define_max_single_key_size,
    dnx_data_field_kbp_define_nof_acl_keys_master_max,
    dnx_data_field_kbp_define_nof_acl_keys_fg_max,
    dnx_data_field_kbp_define_min_acl_nof_ffc,
    dnx_data_field_kbp_define_max_fwd_context_num_for_one_apptype,
    dnx_data_field_kbp_define_max_acl_context_num,
    dnx_data_field_kbp_define_size_apptype_profile_id,
    dnx_data_field_kbp_define_key_bmp,
    dnx_data_field_kbp_define_apptype_user_1st,
    dnx_data_field_kbp_define_apptype_user_nof,
    dnx_data_field_kbp_define_max_payload_size_per_opcode,

    
    _dnx_data_field_kbp_define_nof
} dnx_data_field_kbp_define_e;



uint32 dnx_data_field_kbp_nof_fgs_get(
    int unit);


uint32 dnx_data_field_kbp_max_single_key_size_get(
    int unit);


uint32 dnx_data_field_kbp_nof_acl_keys_master_max_get(
    int unit);


uint32 dnx_data_field_kbp_nof_acl_keys_fg_max_get(
    int unit);


uint32 dnx_data_field_kbp_min_acl_nof_ffc_get(
    int unit);


uint32 dnx_data_field_kbp_max_fwd_context_num_for_one_apptype_get(
    int unit);


uint32 dnx_data_field_kbp_max_acl_context_num_get(
    int unit);


uint32 dnx_data_field_kbp_size_apptype_profile_id_get(
    int unit);


uint32 dnx_data_field_kbp_key_bmp_get(
    int unit);


uint32 dnx_data_field_kbp_apptype_user_1st_get(
    int unit);


uint32 dnx_data_field_kbp_apptype_user_nof_get(
    int unit);


uint32 dnx_data_field_kbp_max_payload_size_per_opcode_get(
    int unit);



typedef enum
{
    dnx_data_field_kbp_table_ffc_to_quad_and_group_map,

    
    _dnx_data_field_kbp_table_nof
} dnx_data_field_kbp_table_e;



const dnx_data_field_kbp_ffc_to_quad_and_group_map_t * dnx_data_field_kbp_ffc_to_quad_and_group_map_get(
    int unit,
    int ffc_id);



shr_error_e dnx_data_field_kbp_ffc_to_quad_and_group_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_field_kbp_ffc_to_quad_and_group_map_info_get(
    int unit);






int dnx_data_field_tcam_feature_get(
    int unit,
    dnx_data_field_tcam_feature_e feature);



typedef enum
{
    dnx_data_field_tcam_define_key_size_half,
    dnx_data_field_tcam_define_key_size_single,
    dnx_data_field_tcam_define_key_size_double,
    dnx_data_field_tcam_define_dt_max_key_size,
    dnx_data_field_tcam_define_action_size_half,
    dnx_data_field_tcam_define_action_size_single,
    dnx_data_field_tcam_define_action_size_double,
    dnx_data_field_tcam_define_key_mode_size,
    dnx_data_field_tcam_define_entry_size_single_key_hw,
    dnx_data_field_tcam_define_entry_size_single_valid_bits_hw,
    dnx_data_field_tcam_define_entry_size_half_payload_hw,
    dnx_data_field_tcam_define_hw_bank_size,
    dnx_data_field_tcam_define_small_bank_size,
    dnx_data_field_tcam_define_nof_big_bank_lines,
    dnx_data_field_tcam_define_nof_small_bank_lines,
    dnx_data_field_tcam_define_nof_big_banks,
    dnx_data_field_tcam_define_nof_small_banks,
    dnx_data_field_tcam_define_nof_banks,
    dnx_data_field_tcam_define_nof_payload_tables,
    dnx_data_field_tcam_define_nof_access_profiles,
    dnx_data_field_tcam_define_action_width_selector_size,
    dnx_data_field_tcam_define_cascaded_data_nof_bits,
    dnx_data_field_tcam_define_big_bank_key_nof_bits,
    dnx_data_field_tcam_define_nof_entries_160_bits,
    dnx_data_field_tcam_define_nof_entries_80_bits,
    dnx_data_field_tcam_define_tcam_banks_size,
    dnx_data_field_tcam_define_tcam_banks_last_index,
    dnx_data_field_tcam_define_nof_tcam_handlers,
    dnx_data_field_tcam_define_max_prefix_size,
    dnx_data_field_tcam_define_max_prefix_value,
    dnx_data_field_tcam_define_nof_keys_in_double_key,
    dnx_data_field_tcam_define_nof_keys_max,
    dnx_data_field_tcam_define_access_profile_half_key_mode,
    dnx_data_field_tcam_define_access_profile_single_key_mode,
    dnx_data_field_tcam_define_access_profile_double_key_mode,
    dnx_data_field_tcam_define_tcam_entries_per_hit_indication_entry,
    dnx_data_field_tcam_define_max_tcam_priority,
    dnx_data_field_tcam_define_nof_big_banks_srams,
    dnx_data_field_tcam_define_nof_small_banks_srams,
    dnx_data_field_tcam_define_nof_big_bank_lines_per_sram,
    dnx_data_field_tcam_define_nof_small_bank_lines_per_sram,
    dnx_data_field_tcam_define_app_db_id_size,

    
    _dnx_data_field_tcam_define_nof
} dnx_data_field_tcam_define_e;



uint32 dnx_data_field_tcam_key_size_half_get(
    int unit);


uint32 dnx_data_field_tcam_key_size_single_get(
    int unit);


uint32 dnx_data_field_tcam_key_size_double_get(
    int unit);


uint32 dnx_data_field_tcam_dt_max_key_size_get(
    int unit);


uint32 dnx_data_field_tcam_action_size_half_get(
    int unit);


uint32 dnx_data_field_tcam_action_size_single_get(
    int unit);


uint32 dnx_data_field_tcam_action_size_double_get(
    int unit);


uint32 dnx_data_field_tcam_key_mode_size_get(
    int unit);


uint32 dnx_data_field_tcam_entry_size_single_key_hw_get(
    int unit);


uint32 dnx_data_field_tcam_entry_size_single_valid_bits_hw_get(
    int unit);


uint32 dnx_data_field_tcam_entry_size_half_payload_hw_get(
    int unit);


uint32 dnx_data_field_tcam_hw_bank_size_get(
    int unit);


uint32 dnx_data_field_tcam_small_bank_size_get(
    int unit);


uint32 dnx_data_field_tcam_nof_big_bank_lines_get(
    int unit);


uint32 dnx_data_field_tcam_nof_small_bank_lines_get(
    int unit);


uint32 dnx_data_field_tcam_nof_big_banks_get(
    int unit);


uint32 dnx_data_field_tcam_nof_small_banks_get(
    int unit);


uint32 dnx_data_field_tcam_nof_banks_get(
    int unit);


uint32 dnx_data_field_tcam_nof_payload_tables_get(
    int unit);


uint32 dnx_data_field_tcam_nof_access_profiles_get(
    int unit);


uint32 dnx_data_field_tcam_action_width_selector_size_get(
    int unit);


uint32 dnx_data_field_tcam_cascaded_data_nof_bits_get(
    int unit);


uint32 dnx_data_field_tcam_big_bank_key_nof_bits_get(
    int unit);


uint32 dnx_data_field_tcam_nof_entries_160_bits_get(
    int unit);


uint32 dnx_data_field_tcam_nof_entries_80_bits_get(
    int unit);


uint32 dnx_data_field_tcam_tcam_banks_size_get(
    int unit);


uint32 dnx_data_field_tcam_tcam_banks_last_index_get(
    int unit);


uint32 dnx_data_field_tcam_nof_tcam_handlers_get(
    int unit);


uint32 dnx_data_field_tcam_max_prefix_size_get(
    int unit);


uint32 dnx_data_field_tcam_max_prefix_value_get(
    int unit);


uint32 dnx_data_field_tcam_nof_keys_in_double_key_get(
    int unit);


uint32 dnx_data_field_tcam_nof_keys_max_get(
    int unit);


uint32 dnx_data_field_tcam_access_profile_half_key_mode_get(
    int unit);


uint32 dnx_data_field_tcam_access_profile_single_key_mode_get(
    int unit);


uint32 dnx_data_field_tcam_access_profile_double_key_mode_get(
    int unit);


uint32 dnx_data_field_tcam_tcam_entries_per_hit_indication_entry_get(
    int unit);


uint32 dnx_data_field_tcam_max_tcam_priority_get(
    int unit);


uint32 dnx_data_field_tcam_nof_big_banks_srams_get(
    int unit);


uint32 dnx_data_field_tcam_nof_small_banks_srams_get(
    int unit);


uint32 dnx_data_field_tcam_nof_big_bank_lines_per_sram_get(
    int unit);


uint32 dnx_data_field_tcam_nof_small_bank_lines_per_sram_get(
    int unit);


uint32 dnx_data_field_tcam_app_db_id_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_tcam_table_nof
} dnx_data_field_tcam_table_e;









int dnx_data_field_group_feature_get(
    int unit,
    dnx_data_field_group_feature_e feature);



typedef enum
{
    dnx_data_field_group_define_nof_fgs,
    dnx_data_field_group_define_nof_action_per_fg,
    dnx_data_field_group_define_nof_quals_per_fg,
    dnx_data_field_group_define_nof_keys_per_fg_max,
    dnx_data_field_group_define_id_fec,
    dnx_data_field_group_define_payload_max_size,

    
    _dnx_data_field_group_define_nof
} dnx_data_field_group_define_e;



uint32 dnx_data_field_group_nof_fgs_get(
    int unit);


uint32 dnx_data_field_group_nof_action_per_fg_get(
    int unit);


uint32 dnx_data_field_group_nof_quals_per_fg_get(
    int unit);


uint32 dnx_data_field_group_nof_keys_per_fg_max_get(
    int unit);


uint32 dnx_data_field_group_id_fec_get(
    int unit);


uint32 dnx_data_field_group_payload_max_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_group_table_nof
} dnx_data_field_group_table_e;









int dnx_data_field_efes_feature_get(
    int unit,
    dnx_data_field_efes_feature_e feature);



typedef enum
{
    dnx_data_field_efes_define_max_nof_key_selects_per_field_io,

    
    _dnx_data_field_efes_define_nof
} dnx_data_field_efes_define_e;



uint32 dnx_data_field_efes_max_nof_key_selects_per_field_io_get(
    int unit);



typedef enum
{
    dnx_data_field_efes_table_key_select_properties,

    
    _dnx_data_field_efes_table_nof
} dnx_data_field_efes_table_e;



const dnx_data_field_efes_key_select_properties_t * dnx_data_field_efes_key_select_properties_get(
    int unit,
    int stage,
    int field_io);



shr_error_e dnx_data_field_efes_key_select_properties_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_field_efes_key_select_properties_info_get(
    int unit);






int dnx_data_field_fem_feature_get(
    int unit,
    dnx_data_field_fem_feature_e feature);



typedef enum
{
    dnx_data_field_fem_define_max_nof_key_selects_per_field_io,

    
    _dnx_data_field_fem_define_nof
} dnx_data_field_fem_define_e;



uint32 dnx_data_field_fem_max_nof_key_selects_per_field_io_get(
    int unit);



typedef enum
{
    dnx_data_field_fem_table_key_select_properties,

    
    _dnx_data_field_fem_table_nof
} dnx_data_field_fem_table_e;



const dnx_data_field_fem_key_select_properties_t * dnx_data_field_fem_key_select_properties_get(
    int unit,
    int field_io);



shr_error_e dnx_data_field_fem_key_select_properties_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_field_fem_key_select_properties_info_get(
    int unit);






int dnx_data_field_context_feature_get(
    int unit,
    dnx_data_field_context_feature_e feature);



typedef enum
{
    dnx_data_field_context_define_default_context,
    dnx_data_field_context_define_default_itmh_context,
    dnx_data_field_context_define_default_itmh_pph_context,
    dnx_data_field_context_define_default_j1_itmh_context,
    dnx_data_field_context_define_default_j1_itmh_pph_context,
    dnx_data_field_context_define_default_stacking_context,
    dnx_data_field_context_define_default_oam_context,
    dnx_data_field_context_define_default_oam_reflector_context,
    dnx_data_field_context_define_default_oam_upmep_context,
    dnx_data_field_context_define_default_j1_learning_2ndpass_context,
    dnx_data_field_context_define_default_rch_remove_context,
    dnx_data_field_context_define_default_nat_context,
    dnx_data_field_context_define_nof_hash_keys,

    
    _dnx_data_field_context_define_nof
} dnx_data_field_context_define_e;



uint32 dnx_data_field_context_default_context_get(
    int unit);


uint32 dnx_data_field_context_default_itmh_context_get(
    int unit);


uint32 dnx_data_field_context_default_itmh_pph_context_get(
    int unit);


uint32 dnx_data_field_context_default_j1_itmh_context_get(
    int unit);


uint32 dnx_data_field_context_default_j1_itmh_pph_context_get(
    int unit);


uint32 dnx_data_field_context_default_stacking_context_get(
    int unit);


uint32 dnx_data_field_context_default_oam_context_get(
    int unit);


uint32 dnx_data_field_context_default_oam_reflector_context_get(
    int unit);


uint32 dnx_data_field_context_default_oam_upmep_context_get(
    int unit);


uint32 dnx_data_field_context_default_j1_learning_2ndpass_context_get(
    int unit);


uint32 dnx_data_field_context_default_rch_remove_context_get(
    int unit);


uint32 dnx_data_field_context_default_nat_context_get(
    int unit);


uint32 dnx_data_field_context_nof_hash_keys_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_context_table_nof
} dnx_data_field_context_table_e;









int dnx_data_field_preselector_feature_get(
    int unit,
    dnx_data_field_preselector_feature_e feature);



typedef enum
{
    dnx_data_field_preselector_define_default_oam_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_itmh_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_itmh_pph_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_itmh_pph_oamp_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_j1_itmh_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_j1_itmh_pph_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_j1_itmh_pph_oamp_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_stacking_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_j1_learn_presel_id_1st_pass_ipmf1,
    dnx_data_field_preselector_define_default_j1_learn_presel_id_2nd_pass_ipmf1,
    dnx_data_field_preselector_define_default_nat_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_itmh_pph_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_j1_itmh_pph_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_oam_roo_ipv4_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_oam_roo_ipv6_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_oam_w_o_roo_no_rif_ipv4_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_oam_w_o_roo_no_rif_ipv6_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_oam_w_o_roo_with_rif_ipv4_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_oam_w_o_roo_with_rif_ipv6_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_j1_ipv4_mc_in_lif_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_j1_ipv6_mc_in_lif_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_rch_remove_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_j1_php_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_j1_swap_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_j1_same_port_presel_id_epmf,
    dnx_data_field_preselector_define_default_learn_limit_presel_id_epmf,
    dnx_data_field_preselector_define_num_cs_inlif_profile_entries,

    
    _dnx_data_field_preselector_define_nof
} dnx_data_field_preselector_define_e;



uint32 dnx_data_field_preselector_default_oam_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_pph_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_pph_oamp_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_j1_itmh_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_j1_itmh_pph_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_j1_itmh_pph_oamp_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_stacking_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_j1_learn_presel_id_1st_pass_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_j1_learn_presel_id_2nd_pass_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_nat_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_pph_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_j1_itmh_pph_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_oam_roo_ipv4_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_oam_roo_ipv6_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_oam_w_o_roo_no_rif_ipv4_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_oam_w_o_roo_no_rif_ipv6_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_oam_w_o_roo_with_rif_ipv4_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_oam_w_o_roo_with_rif_ipv6_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_j1_ipv4_mc_in_lif_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_j1_ipv6_mc_in_lif_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_rch_remove_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_j1_php_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_j1_swap_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_j1_same_port_presel_id_epmf_get(
    int unit);


uint32 dnx_data_field_preselector_default_learn_limit_presel_id_epmf_get(
    int unit);


uint32 dnx_data_field_preselector_num_cs_inlif_profile_entries_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_preselector_table_nof
} dnx_data_field_preselector_table_e;









int dnx_data_field_qual_feature_get(
    int unit,
    dnx_data_field_qual_feature_e feature);



typedef enum
{
    dnx_data_field_qual_define_user_1st,
    dnx_data_field_qual_define_user_nof,
    dnx_data_field_qual_define_vw_1st,
    dnx_data_field_qual_define_vw_nof,
    dnx_data_field_qual_define_max_bits_in_qual,
    dnx_data_field_qual_define_ingress_pbus_header_length,
    dnx_data_field_qual_define_egress_pbus_header_length,
    dnx_data_field_qual_define_ifwd2_pbus_size,
    dnx_data_field_qual_define_ipmf1_pbus_size,
    dnx_data_field_qual_define_ipmf2_pbus_size,
    dnx_data_field_qual_define_ipmf3_pbus_size,
    dnx_data_field_qual_define_epmf_pbus_size,
    dnx_data_field_qual_define_ingress_nof_layer_records,
    dnx_data_field_qual_define_ingress_layer_record_size,
    dnx_data_field_qual_define_egress_nof_layer_records,
    dnx_data_field_qual_define_egress_layer_record_size,
    dnx_data_field_qual_define_ac_lif_wide_size,

    
    _dnx_data_field_qual_define_nof
} dnx_data_field_qual_define_e;



uint32 dnx_data_field_qual_user_1st_get(
    int unit);


uint32 dnx_data_field_qual_user_nof_get(
    int unit);


uint32 dnx_data_field_qual_vw_1st_get(
    int unit);


uint32 dnx_data_field_qual_vw_nof_get(
    int unit);


uint32 dnx_data_field_qual_max_bits_in_qual_get(
    int unit);


uint32 dnx_data_field_qual_ingress_pbus_header_length_get(
    int unit);


uint32 dnx_data_field_qual_egress_pbus_header_length_get(
    int unit);


uint32 dnx_data_field_qual_ifwd2_pbus_size_get(
    int unit);


uint32 dnx_data_field_qual_ipmf1_pbus_size_get(
    int unit);


uint32 dnx_data_field_qual_ipmf2_pbus_size_get(
    int unit);


uint32 dnx_data_field_qual_ipmf3_pbus_size_get(
    int unit);


uint32 dnx_data_field_qual_epmf_pbus_size_get(
    int unit);


uint32 dnx_data_field_qual_ingress_nof_layer_records_get(
    int unit);


uint32 dnx_data_field_qual_ingress_layer_record_size_get(
    int unit);


uint32 dnx_data_field_qual_egress_nof_layer_records_get(
    int unit);


uint32 dnx_data_field_qual_egress_layer_record_size_get(
    int unit);


uint32 dnx_data_field_qual_ac_lif_wide_size_get(
    int unit);



typedef enum
{
    dnx_data_field_qual_table_params,
    dnx_data_field_qual_table_layer_record_info_ingress,
    dnx_data_field_qual_table_layer_record_info_egress,

    
    _dnx_data_field_qual_table_nof
} dnx_data_field_qual_table_e;



const dnx_data_field_qual_params_t * dnx_data_field_qual_params_get(
    int unit,
    int stage,
    int qual);


const dnx_data_field_qual_layer_record_info_ingress_t * dnx_data_field_qual_layer_record_info_ingress_get(
    int unit,
    int layer_record);


const dnx_data_field_qual_layer_record_info_egress_t * dnx_data_field_qual_layer_record_info_egress_get(
    int unit,
    int layer_record);



shr_error_e dnx_data_field_qual_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_field_qual_layer_record_info_ingress_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_field_qual_layer_record_info_egress_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_field_qual_params_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_field_qual_layer_record_info_ingress_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_field_qual_layer_record_info_egress_info_get(
    int unit);






int dnx_data_field_action_feature_get(
    int unit,
    dnx_data_field_action_feature_e feature);



typedef enum
{
    dnx_data_field_action_define_user_1st,
    dnx_data_field_action_define_user_nof,
    dnx_data_field_action_define_vw_1st,
    dnx_data_field_action_define_vw_nof,

    
    _dnx_data_field_action_define_nof
} dnx_data_field_action_define_e;



uint32 dnx_data_field_action_user_1st_get(
    int unit);


uint32 dnx_data_field_action_user_nof_get(
    int unit);


uint32 dnx_data_field_action_vw_1st_get(
    int unit);


uint32 dnx_data_field_action_vw_nof_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_action_table_nof
} dnx_data_field_action_table_e;









int dnx_data_field_virtual_wire_feature_get(
    int unit,
    dnx_data_field_virtual_wire_feature_e feature);



typedef enum
{
    dnx_data_field_virtual_wire_define_signals_nof,
    dnx_data_field_virtual_wire_define_actions_per_signal_nof,
    dnx_data_field_virtual_wire_define_general_data_user_general_containers_size,
    dnx_data_field_virtual_wire_define_ipmf1_general_data_index,

    
    _dnx_data_field_virtual_wire_define_nof
} dnx_data_field_virtual_wire_define_e;



uint32 dnx_data_field_virtual_wire_signals_nof_get(
    int unit);


uint32 dnx_data_field_virtual_wire_actions_per_signal_nof_get(
    int unit);


uint32 dnx_data_field_virtual_wire_general_data_user_general_containers_size_get(
    int unit);


uint32 dnx_data_field_virtual_wire_ipmf1_general_data_index_get(
    int unit);



typedef enum
{
    dnx_data_field_virtual_wire_table_signal_mapping,

    
    _dnx_data_field_virtual_wire_table_nof
} dnx_data_field_virtual_wire_table_e;



const dnx_data_field_virtual_wire_signal_mapping_t * dnx_data_field_virtual_wire_signal_mapping_get(
    int unit,
    int stage,
    int signal_id);



shr_error_e dnx_data_field_virtual_wire_signal_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_field_virtual_wire_signal_mapping_info_get(
    int unit);






int dnx_data_field_profile_bits_feature_get(
    int unit,
    dnx_data_field_profile_bits_feature_e feature);



typedef enum
{
    dnx_data_field_profile_bits_define_nof_ing_in_lif,
    dnx_data_field_profile_bits_define_nof_eg_in_lif,
    dnx_data_field_profile_bits_define_nof_ing_eth_rif,
    dnx_data_field_profile_bits_define_nof_eg_eth_rif,
    dnx_data_field_profile_bits_define_ingress_pp_port_key_gen_var_size,
    dnx_data_field_profile_bits_define_max_port_profile_size,
    dnx_data_field_profile_bits_define_nof_bits_in_port_profile,
    dnx_data_field_profile_bits_define_nof_bits_in_ingress_pp_port_general_data,
    dnx_data_field_profile_bits_define_pmf_sexem3_stage,
    dnx_data_field_profile_bits_define_pmf_map_stage,

    
    _dnx_data_field_profile_bits_define_nof
} dnx_data_field_profile_bits_define_e;



uint32 dnx_data_field_profile_bits_nof_ing_in_lif_get(
    int unit);


uint32 dnx_data_field_profile_bits_nof_eg_in_lif_get(
    int unit);


uint32 dnx_data_field_profile_bits_nof_ing_eth_rif_get(
    int unit);


uint32 dnx_data_field_profile_bits_nof_eg_eth_rif_get(
    int unit);


uint32 dnx_data_field_profile_bits_ingress_pp_port_key_gen_var_size_get(
    int unit);


uint32 dnx_data_field_profile_bits_max_port_profile_size_get(
    int unit);


uint32 dnx_data_field_profile_bits_nof_bits_in_port_profile_get(
    int unit);


uint32 dnx_data_field_profile_bits_nof_bits_in_ingress_pp_port_general_data_get(
    int unit);


uint32 dnx_data_field_profile_bits_pmf_sexem3_stage_get(
    int unit);


uint32 dnx_data_field_profile_bits_pmf_map_stage_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_profile_bits_table_nof
} dnx_data_field_profile_bits_table_e;









int dnx_data_field_dir_ext_feature_get(
    int unit,
    dnx_data_field_dir_ext_feature_e feature);



typedef enum
{
    dnx_data_field_dir_ext_define_half_key_size,
    dnx_data_field_dir_ext_define_single_key_size,
    dnx_data_field_dir_ext_define_double_key_size,

    
    _dnx_data_field_dir_ext_define_nof
} dnx_data_field_dir_ext_define_e;



uint32 dnx_data_field_dir_ext_half_key_size_get(
    int unit);


uint32 dnx_data_field_dir_ext_single_key_size_get(
    int unit);


uint32 dnx_data_field_dir_ext_double_key_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_dir_ext_table_nof
} dnx_data_field_dir_ext_table_e;









int dnx_data_field_state_table_feature_get(
    int unit,
    dnx_data_field_state_table_feature_e feature);



typedef enum
{
    dnx_data_field_state_table_define_address_size_entry_max,
    dnx_data_field_state_table_define_data_size_entry_max,
    dnx_data_field_state_table_define_address_max_entry_max,
    dnx_data_field_state_table_define_wr_bit_size_rw,
    dnx_data_field_state_table_define_opcode_size_rmw,
    dnx_data_field_state_table_define_data_size,
    dnx_data_field_state_table_define_state_table_stage_key,
    dnx_data_field_state_table_define_address_size,
    dnx_data_field_state_table_define_address_max,
    dnx_data_field_state_table_define_wr_bit_size,
    dnx_data_field_state_table_define_key_size,

    
    _dnx_data_field_state_table_define_nof
} dnx_data_field_state_table_define_e;



uint32 dnx_data_field_state_table_address_size_entry_max_get(
    int unit);


uint32 dnx_data_field_state_table_data_size_entry_max_get(
    int unit);


uint32 dnx_data_field_state_table_address_max_entry_max_get(
    int unit);


uint32 dnx_data_field_state_table_wr_bit_size_rw_get(
    int unit);


uint32 dnx_data_field_state_table_opcode_size_rmw_get(
    int unit);


uint32 dnx_data_field_state_table_data_size_get(
    int unit);


uint32 dnx_data_field_state_table_state_table_stage_key_get(
    int unit);


uint32 dnx_data_field_state_table_address_size_get(
    int unit);


uint32 dnx_data_field_state_table_address_max_get(
    int unit);


uint32 dnx_data_field_state_table_wr_bit_size_get(
    int unit);


uint32 dnx_data_field_state_table_key_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_state_table_table_nof
} dnx_data_field_state_table_table_e;









int dnx_data_field_map_feature_get(
    int unit,
    dnx_data_field_map_feature_e feature);



typedef enum
{
    dnx_data_field_map_define_key_size,
    dnx_data_field_map_define_action_size_small,
    dnx_data_field_map_define_action_size_mid,
    dnx_data_field_map_define_action_size_large,
    dnx_data_field_map_define_ipmf1_key_select,
    dnx_data_field_map_define_ipmf2_key_select,
    dnx_data_field_map_define_ipmf3_key_select,

    
    _dnx_data_field_map_define_nof
} dnx_data_field_map_define_e;



uint32 dnx_data_field_map_key_size_get(
    int unit);


uint32 dnx_data_field_map_action_size_small_get(
    int unit);


uint32 dnx_data_field_map_action_size_mid_get(
    int unit);


uint32 dnx_data_field_map_action_size_large_get(
    int unit);


uint32 dnx_data_field_map_ipmf1_key_select_get(
    int unit);


uint32 dnx_data_field_map_ipmf2_key_select_get(
    int unit);


uint32 dnx_data_field_map_ipmf3_key_select_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_map_table_nof
} dnx_data_field_map_table_e;









int dnx_data_field_hash_feature_get(
    int unit,
    dnx_data_field_hash_feature_e feature);



typedef enum
{
    dnx_data_field_hash_define_max_key_size,

    
    _dnx_data_field_hash_define_nof
} dnx_data_field_hash_define_e;



uint32 dnx_data_field_hash_max_key_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_hash_table_nof
} dnx_data_field_hash_table_e;









int dnx_data_field_udh_feature_get(
    int unit,
    dnx_data_field_udh_feature_e feature);



typedef enum
{
    dnx_data_field_udh_define_type_count,
    dnx_data_field_udh_define_type_0_length,
    dnx_data_field_udh_define_type_1_length,
    dnx_data_field_udh_define_type_2_length,
    dnx_data_field_udh_define_type_3_length,
    dnx_data_field_udh_define_field_class_id_size_0,
    dnx_data_field_udh_define_field_class_id_size_1,
    dnx_data_field_udh_define_field_class_id_size_2,
    dnx_data_field_udh_define_field_class_id_size_3,
    dnx_data_field_udh_define_field_class_id_total_size,

    
    _dnx_data_field_udh_define_nof
} dnx_data_field_udh_define_e;



uint32 dnx_data_field_udh_type_count_get(
    int unit);


uint32 dnx_data_field_udh_type_0_length_get(
    int unit);


uint32 dnx_data_field_udh_type_1_length_get(
    int unit);


uint32 dnx_data_field_udh_type_2_length_get(
    int unit);


uint32 dnx_data_field_udh_type_3_length_get(
    int unit);


uint32 dnx_data_field_udh_field_class_id_size_0_get(
    int unit);


uint32 dnx_data_field_udh_field_class_id_size_1_get(
    int unit);


uint32 dnx_data_field_udh_field_class_id_size_2_get(
    int unit);


uint32 dnx_data_field_udh_field_class_id_size_3_get(
    int unit);


uint32 dnx_data_field_udh_field_class_id_total_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_udh_table_nof
} dnx_data_field_udh_table_e;









int dnx_data_field_system_headers_feature_get(
    int unit,
    dnx_data_field_system_headers_feature_e feature);



typedef enum
{
    dnx_data_field_system_headers_define_nof_profiles,
    dnx_data_field_system_headers_define_pph_learn_ext_disable,

    
    _dnx_data_field_system_headers_define_nof
} dnx_data_field_system_headers_define_e;



uint32 dnx_data_field_system_headers_nof_profiles_get(
    int unit);


uint32 dnx_data_field_system_headers_pph_learn_ext_disable_get(
    int unit);



typedef enum
{
    dnx_data_field_system_headers_table_system_header_profiles,

    
    _dnx_data_field_system_headers_table_nof
} dnx_data_field_system_headers_table_e;



const dnx_data_field_system_headers_system_header_profiles_t * dnx_data_field_system_headers_system_header_profiles_get(
    int unit,
    int system_header_profile);



shr_error_e dnx_data_field_system_headers_system_header_profiles_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_field_system_headers_system_header_profiles_info_get(
    int unit);






int dnx_data_field_exem_feature_get(
    int unit,
    dnx_data_field_exem_feature_e feature);



typedef enum
{
    dnx_data_field_exem_define_small_app_db_id_size,
    dnx_data_field_exem_define_large_app_db_id_size,
    dnx_data_field_exem_define_small_max_key_size,
    dnx_data_field_exem_define_large_max_key_size,
    dnx_data_field_exem_define_small_max_result_size,
    dnx_data_field_exem_define_large_max_result_size,
    dnx_data_field_exem_define_small_max_container_size,
    dnx_data_field_exem_define_large_max_container_size,
    dnx_data_field_exem_define_small_key_hash_size,
    dnx_data_field_exem_define_large_key_hash_size,
    dnx_data_field_exem_define_small_min_app_db_id_range,
    dnx_data_field_exem_define_large_min_app_db_id_range,
    dnx_data_field_exem_define_small_max_app_db_id_range,
    dnx_data_field_exem_define_large_max_app_db_id_range,
    dnx_data_field_exem_define_small_ipmf2_key,
    dnx_data_field_exem_define_small_ipmf2_key_part,
    dnx_data_field_exem_define_small_ipmf2_key_hw_value,
    dnx_data_field_exem_define_small_ipmf2_key_hw_bits,
    dnx_data_field_exem_define_large_ipmf1_key_configurable,
    dnx_data_field_exem_define_large_ipmf1_key,
    dnx_data_field_exem_define_small_nof_flush_profiles,

    
    _dnx_data_field_exem_define_nof
} dnx_data_field_exem_define_e;



uint32 dnx_data_field_exem_small_app_db_id_size_get(
    int unit);


uint32 dnx_data_field_exem_large_app_db_id_size_get(
    int unit);


uint32 dnx_data_field_exem_small_max_key_size_get(
    int unit);


uint32 dnx_data_field_exem_large_max_key_size_get(
    int unit);


uint32 dnx_data_field_exem_small_max_result_size_get(
    int unit);


uint32 dnx_data_field_exem_large_max_result_size_get(
    int unit);


uint32 dnx_data_field_exem_small_max_container_size_get(
    int unit);


uint32 dnx_data_field_exem_large_max_container_size_get(
    int unit);


uint32 dnx_data_field_exem_small_key_hash_size_get(
    int unit);


uint32 dnx_data_field_exem_large_key_hash_size_get(
    int unit);


uint32 dnx_data_field_exem_small_min_app_db_id_range_get(
    int unit);


uint32 dnx_data_field_exem_large_min_app_db_id_range_get(
    int unit);


uint32 dnx_data_field_exem_small_max_app_db_id_range_get(
    int unit);


uint32 dnx_data_field_exem_large_max_app_db_id_range_get(
    int unit);


uint32 dnx_data_field_exem_small_ipmf2_key_get(
    int unit);


uint32 dnx_data_field_exem_small_ipmf2_key_part_get(
    int unit);


uint32 dnx_data_field_exem_small_ipmf2_key_hw_value_get(
    int unit);


uint32 dnx_data_field_exem_small_ipmf2_key_hw_bits_get(
    int unit);


uint32 dnx_data_field_exem_large_ipmf1_key_configurable_get(
    int unit);


uint32 dnx_data_field_exem_large_ipmf1_key_get(
    int unit);


uint32 dnx_data_field_exem_small_nof_flush_profiles_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_exem_table_nof
} dnx_data_field_exem_table_e;









int dnx_data_field_exem_learn_flush_machine_feature_get(
    int unit,
    dnx_data_field_exem_learn_flush_machine_feature_e feature);



typedef enum
{
    dnx_data_field_exem_learn_flush_machine_define_flush_machine_nof_entries,
    dnx_data_field_exem_learn_flush_machine_define_entry_extra_bits_for_hash,
    dnx_data_field_exem_learn_flush_machine_define_sexem_entry_max_size,
    dnx_data_field_exem_learn_flush_machine_define_lexem_entry_max_size,
    dnx_data_field_exem_learn_flush_machine_define_command_bit_transplant,
    dnx_data_field_exem_learn_flush_machine_define_command_bit_delete,
    dnx_data_field_exem_learn_flush_machine_define_command_bit_clear_hit_bit,
    dnx_data_field_exem_learn_flush_machine_define_nof_bits_source,
    dnx_data_field_exem_learn_flush_machine_define_source_bit_for_scan,
    dnx_data_field_exem_learn_flush_machine_define_source_bit_for_pipe,
    dnx_data_field_exem_learn_flush_machine_define_source_bit_for_mrq,
    dnx_data_field_exem_learn_flush_machine_define_nof_bits_accessed,

    
    _dnx_data_field_exem_learn_flush_machine_define_nof
} dnx_data_field_exem_learn_flush_machine_define_e;



uint32 dnx_data_field_exem_learn_flush_machine_flush_machine_nof_entries_get(
    int unit);


uint32 dnx_data_field_exem_learn_flush_machine_entry_extra_bits_for_hash_get(
    int unit);


uint32 dnx_data_field_exem_learn_flush_machine_sexem_entry_max_size_get(
    int unit);


uint32 dnx_data_field_exem_learn_flush_machine_lexem_entry_max_size_get(
    int unit);


uint32 dnx_data_field_exem_learn_flush_machine_command_bit_transplant_get(
    int unit);


uint32 dnx_data_field_exem_learn_flush_machine_command_bit_delete_get(
    int unit);


uint32 dnx_data_field_exem_learn_flush_machine_command_bit_clear_hit_bit_get(
    int unit);


uint32 dnx_data_field_exem_learn_flush_machine_nof_bits_source_get(
    int unit);


uint32 dnx_data_field_exem_learn_flush_machine_source_bit_for_scan_get(
    int unit);


uint32 dnx_data_field_exem_learn_flush_machine_source_bit_for_pipe_get(
    int unit);


uint32 dnx_data_field_exem_learn_flush_machine_source_bit_for_mrq_get(
    int unit);


uint32 dnx_data_field_exem_learn_flush_machine_nof_bits_accessed_get(
    int unit);



typedef enum
{
    dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories,
    dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories,
    dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules,
    dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules,

    
    _dnx_data_field_exem_learn_flush_machine_table_nof
} dnx_data_field_exem_learn_flush_machine_table_e;



const dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t * dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_get(
    int unit);


const dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t * dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_get(
    int unit);


const dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_t * dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_get(
    int unit);


const dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_t * dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_get(
    int unit);



shr_error_e dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_info_get(
    int unit);






int dnx_data_field_ace_feature_get(
    int unit,
    dnx_data_field_ace_feature_e feature);



typedef enum
{
    dnx_data_field_ace_define_ace_id_size,
    dnx_data_field_ace_define_key_size,
    dnx_data_field_ace_define_key_hash_size,
    dnx_data_field_ace_define_app_db_id_size,
    dnx_data_field_ace_define_payload_size,
    dnx_data_field_ace_define_min_key_range_pmf,
    dnx_data_field_ace_define_max_key_range_pmf,
    dnx_data_field_ace_define_min_ace_id_dynamic_range,
    dnx_data_field_ace_define_nof_ace_id,
    dnx_data_field_ace_define_nof_action_per_ace_format,
    dnx_data_field_ace_define_row_size,
    dnx_data_field_ace_define_min_entry_size,
    dnx_data_field_ace_define_ace_id_pmf_alloc_first,
    dnx_data_field_ace_define_ace_id_pmf_alloc_last,
    dnx_data_field_ace_define_nof_masks_per_fes,
    dnx_data_field_ace_define_nof_fes_id_per_array,
    dnx_data_field_ace_define_nof_fes_array,
    dnx_data_field_ace_define_nof_fes_instruction_per_context,
    dnx_data_field_ace_define_nof_fes_programs,
    dnx_data_field_ace_define_nof_prog_per_fes,
    dnx_data_field_ace_define_nof_bits_in_fes_action,
    dnx_data_field_ace_define_fes_shift_for_zero_bit,
    dnx_data_field_ace_define_fes_instruction_size,
    dnx_data_field_ace_define_fes_shift_offset,
    dnx_data_field_ace_define_fes_invalid_bits_offset,
    dnx_data_field_ace_define_fes_type_offset,
    dnx_data_field_ace_define_fes_ace_action_offset,
    dnx_data_field_ace_define_fes_chosen_mask_offset,

    
    _dnx_data_field_ace_define_nof
} dnx_data_field_ace_define_e;



uint32 dnx_data_field_ace_ace_id_size_get(
    int unit);


uint32 dnx_data_field_ace_key_size_get(
    int unit);


uint32 dnx_data_field_ace_key_hash_size_get(
    int unit);


uint32 dnx_data_field_ace_app_db_id_size_get(
    int unit);


uint32 dnx_data_field_ace_payload_size_get(
    int unit);


uint32 dnx_data_field_ace_min_key_range_pmf_get(
    int unit);


uint32 dnx_data_field_ace_max_key_range_pmf_get(
    int unit);


uint32 dnx_data_field_ace_min_ace_id_dynamic_range_get(
    int unit);


uint32 dnx_data_field_ace_nof_ace_id_get(
    int unit);


uint32 dnx_data_field_ace_nof_action_per_ace_format_get(
    int unit);


uint32 dnx_data_field_ace_row_size_get(
    int unit);


uint32 dnx_data_field_ace_min_entry_size_get(
    int unit);


uint32 dnx_data_field_ace_ace_id_pmf_alloc_first_get(
    int unit);


uint32 dnx_data_field_ace_ace_id_pmf_alloc_last_get(
    int unit);


uint32 dnx_data_field_ace_nof_masks_per_fes_get(
    int unit);


uint32 dnx_data_field_ace_nof_fes_id_per_array_get(
    int unit);


uint32 dnx_data_field_ace_nof_fes_array_get(
    int unit);


uint32 dnx_data_field_ace_nof_fes_instruction_per_context_get(
    int unit);


uint32 dnx_data_field_ace_nof_fes_programs_get(
    int unit);


uint32 dnx_data_field_ace_nof_prog_per_fes_get(
    int unit);


uint32 dnx_data_field_ace_nof_bits_in_fes_action_get(
    int unit);


uint32 dnx_data_field_ace_fes_shift_for_zero_bit_get(
    int unit);


uint32 dnx_data_field_ace_fes_instruction_size_get(
    int unit);


uint32 dnx_data_field_ace_fes_shift_offset_get(
    int unit);


uint32 dnx_data_field_ace_fes_invalid_bits_offset_get(
    int unit);


uint32 dnx_data_field_ace_fes_type_offset_get(
    int unit);


uint32 dnx_data_field_ace_fes_ace_action_offset_get(
    int unit);


uint32 dnx_data_field_ace_fes_chosen_mask_offset_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_ace_table_nof
} dnx_data_field_ace_table_e;









int dnx_data_field_entry_feature_get(
    int unit,
    dnx_data_field_entry_feature_e feature);



typedef enum
{
    dnx_data_field_entry_define_dir_ext_nof_fields,
    dnx_data_field_entry_define_nof_action_params_per_entry,
    dnx_data_field_entry_define_nof_qual_params_per_entry,

    
    _dnx_data_field_entry_define_nof
} dnx_data_field_entry_define_e;



uint32 dnx_data_field_entry_dir_ext_nof_fields_get(
    int unit);


uint32 dnx_data_field_entry_nof_action_params_per_entry_get(
    int unit);


uint32 dnx_data_field_entry_nof_qual_params_per_entry_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_entry_table_nof
} dnx_data_field_entry_table_e;









int dnx_data_field_L4_Ops_feature_get(
    int unit,
    dnx_data_field_L4_Ops_feature_e feature);



typedef enum
{
    dnx_data_field_L4_Ops_define_udp_position,
    dnx_data_field_L4_Ops_define_tcp_position,
    dnx_data_field_L4_Ops_define_nof_range_entries,
    dnx_data_field_L4_Ops_define_nof_ext_encoders,
    dnx_data_field_L4_Ops_define_nof_ext_types,
    dnx_data_field_L4_Ops_define_nof_configurable_ranges,

    
    _dnx_data_field_L4_Ops_define_nof
} dnx_data_field_L4_Ops_define_e;



uint32 dnx_data_field_L4_Ops_udp_position_get(
    int unit);


uint32 dnx_data_field_L4_Ops_tcp_position_get(
    int unit);


uint32 dnx_data_field_L4_Ops_nof_range_entries_get(
    int unit);


uint32 dnx_data_field_L4_Ops_nof_ext_encoders_get(
    int unit);


uint32 dnx_data_field_L4_Ops_nof_ext_types_get(
    int unit);


uint32 dnx_data_field_L4_Ops_nof_configurable_ranges_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_L4_Ops_table_nof
} dnx_data_field_L4_Ops_table_e;









int dnx_data_field_encoded_qual_actions_offset_feature_get(
    int unit,
    dnx_data_field_encoded_qual_actions_offset_feature_e feature);



typedef enum
{
    dnx_data_field_encoded_qual_actions_offset_define_trap_strength_offset,
    dnx_data_field_encoded_qual_actions_offset_define_trap_qualifier_offset,
    dnx_data_field_encoded_qual_actions_offset_define_sniff_qualifier_offset,
    dnx_data_field_encoded_qual_actions_offset_define_mirror_qualifier_offset,

    
    _dnx_data_field_encoded_qual_actions_offset_define_nof
} dnx_data_field_encoded_qual_actions_offset_define_e;



uint32 dnx_data_field_encoded_qual_actions_offset_trap_strength_offset_get(
    int unit);


uint32 dnx_data_field_encoded_qual_actions_offset_trap_qualifier_offset_get(
    int unit);


uint32 dnx_data_field_encoded_qual_actions_offset_sniff_qualifier_offset_get(
    int unit);


uint32 dnx_data_field_encoded_qual_actions_offset_mirror_qualifier_offset_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_encoded_qual_actions_offset_table_nof
} dnx_data_field_encoded_qual_actions_offset_table_e;









int dnx_data_field_Compare_operand_feature_get(
    int unit,
    dnx_data_field_Compare_operand_feature_e feature);



typedef enum
{
    dnx_data_field_Compare_operand_define_equal,
    dnx_data_field_Compare_operand_define_not_equal,
    dnx_data_field_Compare_operand_define_smaller,
    dnx_data_field_Compare_operand_define_not_smaller,
    dnx_data_field_Compare_operand_define_bigger,
    dnx_data_field_Compare_operand_define_not_bigger,
    dnx_data_field_Compare_operand_define_nof_operands,

    
    _dnx_data_field_Compare_operand_define_nof
} dnx_data_field_Compare_operand_define_e;



uint32 dnx_data_field_Compare_operand_equal_get(
    int unit);


uint32 dnx_data_field_Compare_operand_not_equal_get(
    int unit);


uint32 dnx_data_field_Compare_operand_smaller_get(
    int unit);


uint32 dnx_data_field_Compare_operand_not_smaller_get(
    int unit);


uint32 dnx_data_field_Compare_operand_bigger_get(
    int unit);


uint32 dnx_data_field_Compare_operand_not_bigger_get(
    int unit);


uint32 dnx_data_field_Compare_operand_nof_operands_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_Compare_operand_table_nof
} dnx_data_field_Compare_operand_table_e;









int dnx_data_field_diag_feature_get(
    int unit,
    dnx_data_field_diag_feature_e feature);



typedef enum
{
    dnx_data_field_diag_define_bytes_to_remove_mask,
    dnx_data_field_diag_define_layers_to_remove_size_in_bit,
    dnx_data_field_diag_define_layers_to_remove_mask,
    dnx_data_field_diag_define_nof_signals_per_action,
    dnx_data_field_diag_define_nof_signals_per_qualifier,
    dnx_data_field_diag_define_key_signal_size_in_words,
    dnx_data_field_diag_define_result_signal_size_in_words,
    dnx_data_field_diag_define_dt_result_signal_size_in_words,
    dnx_data_field_diag_define_hit_signal_size_in_words,

    
    _dnx_data_field_diag_define_nof
} dnx_data_field_diag_define_e;



uint32 dnx_data_field_diag_bytes_to_remove_mask_get(
    int unit);


uint32 dnx_data_field_diag_layers_to_remove_size_in_bit_get(
    int unit);


uint32 dnx_data_field_diag_layers_to_remove_mask_get(
    int unit);


uint32 dnx_data_field_diag_nof_signals_per_action_get(
    int unit);


uint32 dnx_data_field_diag_nof_signals_per_qualifier_get(
    int unit);


uint32 dnx_data_field_diag_key_signal_size_in_words_get(
    int unit);


uint32 dnx_data_field_diag_result_signal_size_in_words_get(
    int unit);


uint32 dnx_data_field_diag_dt_result_signal_size_in_words_get(
    int unit);


uint32 dnx_data_field_diag_hit_signal_size_in_words_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_diag_table_nof
} dnx_data_field_diag_table_e;









int dnx_data_field_common_max_val_feature_get(
    int unit,
    dnx_data_field_common_max_val_feature_e feature);



typedef enum
{
    dnx_data_field_common_max_val_define_nof_ffc,
    dnx_data_field_common_max_val_define_nof_keys,
    dnx_data_field_common_max_val_define_nof_masks_per_fes,
    dnx_data_field_common_max_val_define_nof_fes_id_per_array,
    dnx_data_field_common_max_val_define_nof_fes_array,
    dnx_data_field_common_max_val_define_nof_fes_instruction_per_context,
    dnx_data_field_common_max_val_define_nof_fes_programs,
    dnx_data_field_common_max_val_define_nof_prog_per_fes,
    dnx_data_field_common_max_val_define_nof_program_selection_lines,
    dnx_data_field_common_max_val_define_program_selection_cam_mask_nof_bits,
    dnx_data_field_common_max_val_define_nof_cs_lines,
    dnx_data_field_common_max_val_define_nof_contexts,
    dnx_data_field_common_max_val_define_nof_actions,
    dnx_data_field_common_max_val_define_nof_qualifiers,
    dnx_data_field_common_max_val_define_nof_80B_zones,
    dnx_data_field_common_max_val_define_nof_key_zones,
    dnx_data_field_common_max_val_define_nof_key_zone_bits,
    dnx_data_field_common_max_val_define_nof_bits_in_fes_action,
    dnx_data_field_common_max_val_define_nof_bits_in_fes_key_select,
    dnx_data_field_common_max_val_define_nof_fes_key_selects_on_one_actions_line,
    dnx_data_field_common_max_val_define_nof_ffc_in_qual,
    dnx_data_field_common_max_val_define_nof_bits_in_ffc,
    dnx_data_field_common_max_val_define_nof_ffc_in_uint32,
    dnx_data_field_common_max_val_define_nof_action_per_group,
    dnx_data_field_common_max_val_define_nof_layer_records,
    dnx_data_field_common_max_val_define_layer_record_size,
    dnx_data_field_common_max_val_define_nof_l4_ops_ranges_legacy,
    dnx_data_field_common_max_val_define_nof_pkt_hdr_ranges,
    dnx_data_field_common_max_val_define_nof_out_lif_ranges,
    dnx_data_field_common_max_val_define_kbr_size,
    dnx_data_field_common_max_val_define_nof_compare_pairs,
    dnx_data_field_common_max_val_define_nof_compare_pairs_in_compare_mode,
    dnx_data_field_common_max_val_define_nof_compare_keys_in_compare_mode,
    dnx_data_field_common_max_val_define_nof_bits_in_fem_action,
    dnx_data_field_common_max_val_define_nof_fem_condition,
    dnx_data_field_common_max_val_define_nof_fem_map_index,
    dnx_data_field_common_max_val_define_nof_fem_id,
    dnx_data_field_common_max_val_define_nof_array_ids,
    dnx_data_field_common_max_val_define_dbal_pairs,

    
    _dnx_data_field_common_max_val_define_nof
} dnx_data_field_common_max_val_define_e;



uint32 dnx_data_field_common_max_val_nof_ffc_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_keys_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_masks_per_fes_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_fes_id_per_array_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_fes_array_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_fes_instruction_per_context_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_fes_programs_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_prog_per_fes_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_program_selection_lines_get(
    int unit);


uint32 dnx_data_field_common_max_val_program_selection_cam_mask_nof_bits_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_cs_lines_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_contexts_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_actions_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_qualifiers_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_80B_zones_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_key_zones_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_key_zone_bits_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_bits_in_fes_action_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_bits_in_fes_key_select_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_fes_key_selects_on_one_actions_line_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_ffc_in_qual_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_bits_in_ffc_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_ffc_in_uint32_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_action_per_group_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_layer_records_get(
    int unit);


uint32 dnx_data_field_common_max_val_layer_record_size_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_l4_ops_ranges_legacy_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_pkt_hdr_ranges_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_out_lif_ranges_get(
    int unit);


uint32 dnx_data_field_common_max_val_kbr_size_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_compare_pairs_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_compare_pairs_in_compare_mode_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_compare_keys_in_compare_mode_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_bits_in_fem_action_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_fem_condition_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_fem_map_index_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_fem_id_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_array_ids_get(
    int unit);


uint32 dnx_data_field_common_max_val_dbal_pairs_get(
    int unit);



typedef enum
{
    dnx_data_field_common_max_val_table_array_id_type,

    
    _dnx_data_field_common_max_val_table_nof
} dnx_data_field_common_max_val_table_e;



const dnx_data_field_common_max_val_array_id_type_t * dnx_data_field_common_max_val_array_id_type_get(
    int unit,
    int array_id);



shr_error_e dnx_data_field_common_max_val_array_id_type_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_field_common_max_val_array_id_type_info_get(
    int unit);






int dnx_data_field_init_feature_get(
    int unit,
    dnx_data_field_init_feature_e feature);



typedef enum
{
    dnx_data_field_init_define_fec_dest,
    dnx_data_field_init_define_l4_trap,
    dnx_data_field_init_define_oam_layer_index,
    dnx_data_field_init_define_oam_stat,
    dnx_data_field_init_define_flow_id,
    dnx_data_field_init_define_roo,
    dnx_data_field_init_define_jr1_ipmc_inlif,
    dnx_data_field_init_define_j1_same_port,
    dnx_data_field_init_define_j1_learning,
    dnx_data_field_init_define_learn_limit,
    dnx_data_field_init_define_j1_php,

    
    _dnx_data_field_init_define_nof
} dnx_data_field_init_define_e;



uint32 dnx_data_field_init_fec_dest_get(
    int unit);


uint32 dnx_data_field_init_l4_trap_get(
    int unit);


uint32 dnx_data_field_init_oam_layer_index_get(
    int unit);


uint32 dnx_data_field_init_oam_stat_get(
    int unit);


uint32 dnx_data_field_init_flow_id_get(
    int unit);


uint32 dnx_data_field_init_roo_get(
    int unit);


uint32 dnx_data_field_init_jr1_ipmc_inlif_get(
    int unit);


uint32 dnx_data_field_init_j1_same_port_get(
    int unit);


uint32 dnx_data_field_init_j1_learning_get(
    int unit);


uint32 dnx_data_field_init_learn_limit_get(
    int unit);


uint32 dnx_data_field_init_j1_php_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_init_table_nof
} dnx_data_field_init_table_e;









int dnx_data_field_features_feature_get(
    int unit,
    dnx_data_field_features_feature_e feature);



typedef enum
{
    dnx_data_field_features_define_ecc_enable,
    dnx_data_field_features_define_switch_to_acl_context,
    dnx_data_field_features_define_per_pp_port_pmf_profile_cs_offset,
    dnx_data_field_features_define_tcam_result_flip_eco,
    dnx_data_field_features_define_tcam_result_half_payload_on_msb,
    dnx_data_field_features_define_parsing_start_offset_msb_meaningless,
    dnx_data_field_features_define_kbp_opcode_in_ipmf1_cs,
    dnx_data_field_features_define_kbp_hitbits_correct_in_ipmf1_cs,
    dnx_data_field_features_define_exem_vmv_removable_from_payload,
    dnx_data_field_features_define_multiple_dynamic_mem_enablers,
    dnx_data_field_features_define_aacl_super_group_handler_enable,
    dnx_data_field_features_define_aacl_tcam_swap_enable,
    dnx_data_field_features_define_extended_l4_ops,
    dnx_data_field_features_define_state_table_ipmf1_key_select,
    dnx_data_field_features_define_state_table_acr_bus,
    dnx_data_field_features_define_state_table_atomic_rmw,
    dnx_data_field_features_define_hitbit_volatile,
    dnx_data_field_features_define_fem_replace_msb_instead_lsb,
    dnx_data_field_features_define_no_parser_resources,
    dnx_data_field_features_define_dir_ext_epmf,
    dnx_data_field_features_define_hashing_process_lsb_to_msb,
    dnx_data_field_features_define_exem_age_flush_scan,

    
    _dnx_data_field_features_define_nof
} dnx_data_field_features_define_e;



uint32 dnx_data_field_features_ecc_enable_get(
    int unit);


uint32 dnx_data_field_features_switch_to_acl_context_get(
    int unit);


uint32 dnx_data_field_features_per_pp_port_pmf_profile_cs_offset_get(
    int unit);


uint32 dnx_data_field_features_tcam_result_flip_eco_get(
    int unit);


uint32 dnx_data_field_features_tcam_result_half_payload_on_msb_get(
    int unit);


uint32 dnx_data_field_features_parsing_start_offset_msb_meaningless_get(
    int unit);


uint32 dnx_data_field_features_kbp_opcode_in_ipmf1_cs_get(
    int unit);


uint32 dnx_data_field_features_kbp_hitbits_correct_in_ipmf1_cs_get(
    int unit);


uint32 dnx_data_field_features_exem_vmv_removable_from_payload_get(
    int unit);


uint32 dnx_data_field_features_multiple_dynamic_mem_enablers_get(
    int unit);


uint32 dnx_data_field_features_aacl_super_group_handler_enable_get(
    int unit);


uint32 dnx_data_field_features_aacl_tcam_swap_enable_get(
    int unit);


uint32 dnx_data_field_features_extended_l4_ops_get(
    int unit);


uint32 dnx_data_field_features_state_table_ipmf1_key_select_get(
    int unit);


uint32 dnx_data_field_features_state_table_acr_bus_get(
    int unit);


uint32 dnx_data_field_features_state_table_atomic_rmw_get(
    int unit);


uint32 dnx_data_field_features_hitbit_volatile_get(
    int unit);


uint32 dnx_data_field_features_fem_replace_msb_instead_lsb_get(
    int unit);


uint32 dnx_data_field_features_no_parser_resources_get(
    int unit);


uint32 dnx_data_field_features_dir_ext_epmf_get(
    int unit);


uint32 dnx_data_field_features_hashing_process_lsb_to_msb_get(
    int unit);


uint32 dnx_data_field_features_exem_age_flush_scan_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_features_table_nof
} dnx_data_field_features_table_e;









int dnx_data_field_signal_sizes_feature_get(
    int unit,
    dnx_data_field_signal_sizes_feature_e feature);



typedef enum
{
    dnx_data_field_signal_sizes_define_dual_queue_size,
    dnx_data_field_signal_sizes_define_packet_header_size,
    dnx_data_field_signal_sizes_define_ecn,
    dnx_data_field_signal_sizes_define_congestion_info,
    dnx_data_field_signal_sizes_define_parsing_start_offset_size,

    
    _dnx_data_field_signal_sizes_define_nof
} dnx_data_field_signal_sizes_define_e;



uint32 dnx_data_field_signal_sizes_dual_queue_size_get(
    int unit);


uint32 dnx_data_field_signal_sizes_packet_header_size_get(
    int unit);


uint32 dnx_data_field_signal_sizes_ecn_get(
    int unit);


uint32 dnx_data_field_signal_sizes_congestion_info_get(
    int unit);


uint32 dnx_data_field_signal_sizes_parsing_start_offset_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_signal_sizes_table_nof
} dnx_data_field_signal_sizes_table_e;









int dnx_data_field_dnx_data_internal_feature_get(
    int unit,
    dnx_data_field_dnx_data_internal_feature_e feature);



typedef enum
{
    dnx_data_field_dnx_data_internal_define_lr_eth_is_da_mac_valid,

    
    _dnx_data_field_dnx_data_internal_define_nof
} dnx_data_field_dnx_data_internal_define_e;



uint32 dnx_data_field_dnx_data_internal_lr_eth_is_da_mac_valid_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_dnx_data_internal_table_nof
} dnx_data_field_dnx_data_internal_table_e;









int dnx_data_field_tests_feature_get(
    int unit,
    dnx_data_field_tests_feature_e feature);



typedef enum
{
    dnx_data_field_tests_define_learn_info_actions_structure_change,
    dnx_data_field_tests_define_ingress_time_stamp_increased,
    dnx_data_field_tests_define_ace_debug_signals_exist,

    
    _dnx_data_field_tests_define_nof
} dnx_data_field_tests_define_e;



uint32 dnx_data_field_tests_learn_info_actions_structure_change_get(
    int unit);


uint32 dnx_data_field_tests_ingress_time_stamp_increased_get(
    int unit);


uint32 dnx_data_field_tests_ace_debug_signals_exist_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_tests_table_nof
} dnx_data_field_tests_table_e;






shr_error_e dnx_data_field_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

