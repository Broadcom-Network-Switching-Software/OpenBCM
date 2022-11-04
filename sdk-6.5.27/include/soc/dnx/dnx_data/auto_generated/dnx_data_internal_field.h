
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_FIELD_H_

#define _DNX_DATA_INTERNAL_FIELD_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_field_submodule_base_ipmf1,
    dnx_data_field_submodule_base_ipmf2,
    dnx_data_field_submodule_base_ipmf3,
    dnx_data_field_submodule_base_epmf,
    dnx_data_field_submodule_base_ifwd2,
    dnx_data_field_submodule_stage,
    dnx_data_field_submodule_external_tcam,
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
    dnx_data_field_submodule_sce,
    dnx_data_field_submodule_mdb_dt,
    dnx_data_field_submodule_hash,
    dnx_data_field_submodule_udh,
    dnx_data_field_submodule_system_headers,
    dnx_data_field_submodule_exem,
    dnx_data_field_submodule_exem_learn_flush_machine,
    dnx_data_field_submodule_ace,
    dnx_data_field_submodule_entry,
    dnx_data_field_submodule_L4_Ops,
    dnx_data_field_submodule_encoded_qual_actions_offset,
    dnx_data_field_submodule_compare,
    dnx_data_field_submodule_diag,
    dnx_data_field_submodule_general_data_qualifiers,
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
    dnx_data_field_base_ipmf1_define_nof_keys,
    dnx_data_field_base_ipmf1_define_nof_keys_alloc,
    dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_tcam,
    dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_exem,
    dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_dir_ext,
    dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_mdb_dt,
    dnx_data_field_base_ipmf1_define_nof_masks_per_fes,
    dnx_data_field_base_ipmf1_define_nof_fes_id_per_array,
    dnx_data_field_base_ipmf1_define_nof_fes_array,
    dnx_data_field_base_ipmf1_define_nof_fes_instruction_per_context,
    dnx_data_field_base_ipmf1_define_nof_fes_programs,
    dnx_data_field_base_ipmf1_define_nof_prog_per_fes,
    dnx_data_field_base_ipmf1_define_cs_container_5_selected_bits_size,
    dnx_data_field_base_ipmf1_define_nof_contexts,
    dnx_data_field_base_ipmf1_define_nof_link_profiles,
    dnx_data_field_base_ipmf1_define_nof_cs_lines,
    dnx_data_field_base_ipmf1_define_nof_actions,
    dnx_data_field_base_ipmf1_define_nof_qualifiers,
    dnx_data_field_base_ipmf1_define_nof_bits_in_fes_action,
    dnx_data_field_base_ipmf1_define_nof_bits_in_fes_key_select,
    dnx_data_field_base_ipmf1_define_fes_key_select_resolution_in_bits,
    dnx_data_field_base_ipmf1_define_fem_condition_ms_bit_min_value,
    dnx_data_field_base_ipmf1_define_nof_fem_programs,
    dnx_data_field_base_ipmf1_define_log_nof_bits_in_fem_key_select,
    dnx_data_field_base_ipmf1_define_nof_bits_in_fem_key_select,
    dnx_data_field_base_ipmf1_define_fem_key_select_resolution_in_bits,
    dnx_data_field_base_ipmf1_define_log_nof_bits_in_fem_map_data_field,
    dnx_data_field_base_ipmf1_define_nof_bits_in_fem_map_data_field,
    dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action_fems_2_15,
    dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action_fems_0_1,
    dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action,
    dnx_data_field_base_ipmf1_define_nof_fem_condition,
    dnx_data_field_base_ipmf1_define_nof_fem_map_index,
    dnx_data_field_base_ipmf1_define_nof_fem_id,
    dnx_data_field_base_ipmf1_define_num_fems_with_short_action,
    dnx_data_field_base_ipmf1_define_num_bits_in_fem_field_select,
    dnx_data_field_base_ipmf1_define_nof_fem_id_per_array,
    dnx_data_field_base_ipmf1_define_nof_fem_array,
    dnx_data_field_base_ipmf1_define_nof_compare_pairs_in_compare_mode,
    dnx_data_field_base_ipmf1_define_compare_key_size,
    dnx_data_field_base_ipmf1_define_nof_l4_ops_ranges_legacy,
    dnx_data_field_base_ipmf1_define_nof_ext_l4_ops_ranges,
    dnx_data_field_base_ipmf1_define_nof_pkt_hdr_ranges,
    dnx_data_field_base_ipmf1_define_nof_out_lif_ranges,
    dnx_data_field_base_ipmf1_define_cmp_selection,
    dnx_data_field_base_ipmf1_define_dir_ext_single_key_size,
    dnx_data_field_base_ipmf1_define_dir_ext_first_key,
    dnx_data_field_base_ipmf1_define_exem_max_result_size,
    dnx_data_field_base_ipmf1_define_fes_instruction_size,
    dnx_data_field_base_ipmf1_define_fes_pgm_id_offset,
    dnx_data_field_base_ipmf1_define_fes_key_select_for_zero_bit,
    dnx_data_field_base_ipmf1_define_fes_shift_for_zero_bit,
    dnx_data_field_base_ipmf1_define_exem_pdb,

    
    _dnx_data_field_base_ipmf1_define_nof
} dnx_data_field_base_ipmf1_define_e;



uint32 dnx_data_field_base_ipmf1_nof_ffc_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_keys_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_keys_alloc_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_keys_alloc_for_tcam_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_keys_alloc_for_exem_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_keys_alloc_for_dir_ext_get(
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


uint32 dnx_data_field_base_ipmf1_nof_bits_in_fes_action_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_bits_in_fes_key_select_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_fes_key_select_resolution_in_bits_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_fem_condition_ms_bit_min_value_get(
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


uint32 dnx_data_field_base_ipmf1_nof_fem_condition_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_nof_fem_map_index_get(
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


uint32 dnx_data_field_base_ipmf1_nof_compare_pairs_in_compare_mode_get(
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


uint32 dnx_data_field_base_ipmf1_cmp_selection_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_dir_ext_single_key_size_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_dir_ext_first_key_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_exem_max_result_size_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_fes_instruction_size_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_fes_pgm_id_offset_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_fes_key_select_for_zero_bit_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_fes_shift_for_zero_bit_get(
    int unit);


uint32 dnx_data_field_base_ipmf1_exem_pdb_get(
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
    dnx_data_field_base_ipmf2_define_nof_qualifiers,
    dnx_data_field_base_ipmf2_define_dir_ext_single_key_size,
    dnx_data_field_base_ipmf2_define_dir_ext_first_key,
    dnx_data_field_base_ipmf2_define_exem_max_result_size,
    dnx_data_field_base_ipmf2_define_fes_key_select_for_zero_bit,
    dnx_data_field_base_ipmf2_define_fes_shift_for_zero_bit,
    dnx_data_field_base_ipmf2_define_exem_pdb,

    
    _dnx_data_field_base_ipmf2_define_nof
} dnx_data_field_base_ipmf2_define_e;



uint32 dnx_data_field_base_ipmf2_nof_ffc_get(
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


uint32 dnx_data_field_base_ipmf2_nof_qualifiers_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_dir_ext_single_key_size_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_dir_ext_first_key_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_exem_max_result_size_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_fes_key_select_for_zero_bit_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_fes_shift_for_zero_bit_get(
    int unit);


uint32 dnx_data_field_base_ipmf2_exem_pdb_get(
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
    dnx_data_field_base_ipmf3_define_cs_scratch_pad_size,
    dnx_data_field_base_ipmf3_define_nof_contexts,
    dnx_data_field_base_ipmf3_define_nof_cs_lines,
    dnx_data_field_base_ipmf3_define_nof_actions,
    dnx_data_field_base_ipmf3_define_nof_qualifiers,
    dnx_data_field_base_ipmf3_define_nof_bits_in_fes_action,
    dnx_data_field_base_ipmf3_define_nof_bits_in_fes_key_select,
    dnx_data_field_base_ipmf3_define_fes_key_select_resolution_in_bits,
    dnx_data_field_base_ipmf3_define_nof_out_lif_ranges,
    dnx_data_field_base_ipmf3_define_fes_instruction_size,
    dnx_data_field_base_ipmf3_define_fes_pgm_id_offset,
    dnx_data_field_base_ipmf3_define_dir_ext_single_key_size,
    dnx_data_field_base_ipmf3_define_dir_ext_first_key,
    dnx_data_field_base_ipmf3_define_exem_max_result_size,
    dnx_data_field_base_ipmf3_define_nof_fes_used_by_sdk,
    dnx_data_field_base_ipmf3_define_fes_key_select_for_zero_bit,
    dnx_data_field_base_ipmf3_define_fes_shift_for_zero_bit,
    dnx_data_field_base_ipmf3_define_exem_pdb,

    
    _dnx_data_field_base_ipmf3_define_nof
} dnx_data_field_base_ipmf3_define_e;



uint32 dnx_data_field_base_ipmf3_nof_ffc_get(
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


uint32 dnx_data_field_base_ipmf3_nof_bits_in_fes_action_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_bits_in_fes_key_select_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_fes_key_select_resolution_in_bits_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_out_lif_ranges_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_fes_instruction_size_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_fes_pgm_id_offset_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_dir_ext_single_key_size_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_dir_ext_first_key_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_exem_max_result_size_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_nof_fes_used_by_sdk_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_fes_key_select_for_zero_bit_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_fes_shift_for_zero_bit_get(
    int unit);


uint32 dnx_data_field_base_ipmf3_exem_pdb_get(
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
    dnx_data_field_base_epmf_define_nof_cs_lines,
    dnx_data_field_base_epmf_define_nof_contexts,
    dnx_data_field_base_epmf_define_nof_actions,
    dnx_data_field_base_epmf_define_nof_qualifiers,
    dnx_data_field_base_epmf_define_nof_bits_in_fes_action,
    dnx_data_field_base_epmf_define_nof_bits_in_fes_key_select,
    dnx_data_field_base_epmf_define_fes_key_select_resolution_in_bits,
    dnx_data_field_base_epmf_define_nof_l4_ops_ranges_legacy,
    dnx_data_field_base_epmf_define_fes_instruction_size,
    dnx_data_field_base_epmf_define_fes_pgm_id_offset,
    dnx_data_field_base_epmf_define_dir_ext_single_key_size,
    dnx_data_field_base_epmf_define_dir_ext_first_key,
    dnx_data_field_base_epmf_define_exem_max_result_size,
    dnx_data_field_base_epmf_define_fes_key_select_for_zero_bit,
    dnx_data_field_base_epmf_define_fes_shift_for_zero_bit,
    dnx_data_field_base_epmf_define_exem_pdb,

    
    _dnx_data_field_base_epmf_define_nof
} dnx_data_field_base_epmf_define_e;



uint32 dnx_data_field_base_epmf_nof_ffc_get(
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


uint32 dnx_data_field_base_epmf_nof_cs_lines_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_contexts_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_actions_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_qualifiers_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_bits_in_fes_action_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_bits_in_fes_key_select_get(
    int unit);


uint32 dnx_data_field_base_epmf_fes_key_select_resolution_in_bits_get(
    int unit);


uint32 dnx_data_field_base_epmf_nof_l4_ops_ranges_legacy_get(
    int unit);


uint32 dnx_data_field_base_epmf_fes_instruction_size_get(
    int unit);


uint32 dnx_data_field_base_epmf_fes_pgm_id_offset_get(
    int unit);


uint32 dnx_data_field_base_epmf_dir_ext_single_key_size_get(
    int unit);


uint32 dnx_data_field_base_epmf_dir_ext_first_key_get(
    int unit);


uint32 dnx_data_field_base_epmf_exem_max_result_size_get(
    int unit);


uint32 dnx_data_field_base_epmf_fes_key_select_for_zero_bit_get(
    int unit);


uint32 dnx_data_field_base_epmf_fes_shift_for_zero_bit_get(
    int unit);


uint32 dnx_data_field_base_epmf_exem_pdb_get(
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
    dnx_data_field_base_ifwd2_define_nof_keys,
    dnx_data_field_base_ifwd2_define_nof_contexts,
    dnx_data_field_base_ifwd2_define_nof_cs_lines,

    
    _dnx_data_field_base_ifwd2_define_nof
} dnx_data_field_base_ifwd2_define_e;



uint32 dnx_data_field_base_ifwd2_nof_ffc_get(
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






int dnx_data_field_external_tcam_feature_get(
    int unit,
    dnx_data_field_external_tcam_feature_e feature);



typedef enum
{
    dnx_data_field_external_tcam_define_max_single_key_size,
    dnx_data_field_external_tcam_define_nof_acl_keys_master_max,
    dnx_data_field_external_tcam_define_nof_acl_keys_fg_max,
    dnx_data_field_external_tcam_define_nof_keys_total,
    dnx_data_field_external_tcam_define_min_acl_nof_ffc,
    dnx_data_field_external_tcam_define_max_fwd_context_num_for_one_apptype,
    dnx_data_field_external_tcam_define_max_acl_context_num,
    dnx_data_field_external_tcam_define_size_apptype_profile_id,
    dnx_data_field_external_tcam_define_key_bmp,
    dnx_data_field_external_tcam_define_apptype_user_1st,
    dnx_data_field_external_tcam_define_apptype_user_nof,
    dnx_data_field_external_tcam_define_max_payload_size_per_opcode,
    dnx_data_field_external_tcam_define_max_payload_size_per_opcode_epmf,
    dnx_data_field_external_tcam_define_max_epmf_nof_lookups,
    dnx_data_field_external_tcam_define_nof_total_lookups,
    dnx_data_field_external_tcam_define_multicore_fg_supports_core_all,
    dnx_data_field_external_tcam_define_type,

    
    _dnx_data_field_external_tcam_define_nof
} dnx_data_field_external_tcam_define_e;



uint32 dnx_data_field_external_tcam_max_single_key_size_get(
    int unit);


uint32 dnx_data_field_external_tcam_nof_acl_keys_master_max_get(
    int unit);


uint32 dnx_data_field_external_tcam_nof_acl_keys_fg_max_get(
    int unit);


uint32 dnx_data_field_external_tcam_nof_keys_total_get(
    int unit);


uint32 dnx_data_field_external_tcam_min_acl_nof_ffc_get(
    int unit);


uint32 dnx_data_field_external_tcam_max_fwd_context_num_for_one_apptype_get(
    int unit);


uint32 dnx_data_field_external_tcam_max_acl_context_num_get(
    int unit);


uint32 dnx_data_field_external_tcam_size_apptype_profile_id_get(
    int unit);


uint32 dnx_data_field_external_tcam_key_bmp_get(
    int unit);


uint32 dnx_data_field_external_tcam_apptype_user_1st_get(
    int unit);


uint32 dnx_data_field_external_tcam_apptype_user_nof_get(
    int unit);


uint32 dnx_data_field_external_tcam_max_payload_size_per_opcode_get(
    int unit);


uint32 dnx_data_field_external_tcam_max_payload_size_per_opcode_epmf_get(
    int unit);


uint32 dnx_data_field_external_tcam_max_epmf_nof_lookups_get(
    int unit);


uint32 dnx_data_field_external_tcam_nof_total_lookups_get(
    int unit);


uint32 dnx_data_field_external_tcam_multicore_fg_supports_core_all_get(
    int unit);


uint32 dnx_data_field_external_tcam_type_get(
    int unit);



typedef enum
{
    dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map,
    dnx_data_field_external_tcam_table_epmf_payload_sizes,

    
    _dnx_data_field_external_tcam_table_nof
} dnx_data_field_external_tcam_table_e;



const dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t * dnx_data_field_external_tcam_ffc_to_quad_and_group_map_get(
    int unit,
    int ffc_id);


const dnx_data_field_external_tcam_epmf_payload_sizes_t * dnx_data_field_external_tcam_epmf_payload_sizes_get(
    int unit,
    int result_id);



shr_error_e dnx_data_field_external_tcam_ffc_to_quad_and_group_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_field_external_tcam_epmf_payload_sizes_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_field_external_tcam_ffc_to_quad_and_group_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_field_external_tcam_epmf_payload_sizes_info_get(
    int unit);






int dnx_data_field_tcam_feature_get(
    int unit,
    dnx_data_field_tcam_feature_e feature);



typedef enum
{
    dnx_data_field_tcam_define_access_hw_mix_ratio_supported,
    dnx_data_field_tcam_define_direct_supported,
    dnx_data_field_tcam_define_key_size_half,
    dnx_data_field_tcam_define_key_size_single,
    dnx_data_field_tcam_define_key_size_double,
    dnx_data_field_tcam_define_dt_max_key_size,
    dnx_data_field_tcam_define_dt_max_key_size_small_bank,
    dnx_data_field_tcam_define_action_size_half,
    dnx_data_field_tcam_define_action_size_single,
    dnx_data_field_tcam_define_action_size_double,
    dnx_data_field_tcam_define_key_mode_size,
    dnx_data_field_tcam_define_entry_size_single_key_hw,
    dnx_data_field_tcam_define_entry_size_single_key_shadow,
    dnx_data_field_tcam_define_entry_size_single_valid_bits_hw,
    dnx_data_field_tcam_define_hw_bank_size,
    dnx_data_field_tcam_define_nof_big_bank_lines,
    dnx_data_field_tcam_define_nof_small_bank_lines,
    dnx_data_field_tcam_define_nof_big_banks,
    dnx_data_field_tcam_define_nof_small_banks,
    dnx_data_field_tcam_define_nof_banks,
    dnx_data_field_tcam_define_nof_payload_tables,
    dnx_data_field_tcam_define_nof_access_profiles,
    dnx_data_field_tcam_define_action_width_selector_size,
    dnx_data_field_tcam_define_nof_entries_80_bits,
    dnx_data_field_tcam_define_nof_entries_160_bits,
    dnx_data_field_tcam_define_tcam_banks_size,
    dnx_data_field_tcam_define_tcam_banks_last_index,
    dnx_data_field_tcam_define_nof_tcam_handlers,
    dnx_data_field_tcam_define_max_prefix_size,
    dnx_data_field_tcam_define_max_prefix_value,
    dnx_data_field_tcam_define_nof_keys_max,
    dnx_data_field_tcam_define_access_profile_half_key_mode,
    dnx_data_field_tcam_define_access_profile_single_key_mode,
    dnx_data_field_tcam_define_access_profile_double_key_mode,
    dnx_data_field_tcam_define_tcam_entries_per_hit_indication_entry,
    dnx_data_field_tcam_define_max_tcam_priority,
    dnx_data_field_tcam_define_nof_banks_hitbit_units,
    dnx_data_field_tcam_define_nof_big_bank_lines_per_hitbit_unit,
    dnx_data_field_tcam_define_nof_small_bank_lines_per_hitbit_unit,
    dnx_data_field_tcam_define_app_db_id_size,
    dnx_data_field_tcam_define_tcam_indirect_command_version,

    
    _dnx_data_field_tcam_define_nof
} dnx_data_field_tcam_define_e;



uint32 dnx_data_field_tcam_access_hw_mix_ratio_supported_get(
    int unit);


uint32 dnx_data_field_tcam_direct_supported_get(
    int unit);


uint32 dnx_data_field_tcam_key_size_half_get(
    int unit);


uint32 dnx_data_field_tcam_key_size_single_get(
    int unit);


uint32 dnx_data_field_tcam_key_size_double_get(
    int unit);


uint32 dnx_data_field_tcam_dt_max_key_size_get(
    int unit);


uint32 dnx_data_field_tcam_dt_max_key_size_small_bank_get(
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


uint32 dnx_data_field_tcam_entry_size_single_key_shadow_get(
    int unit);


uint32 dnx_data_field_tcam_entry_size_single_valid_bits_hw_get(
    int unit);


uint32 dnx_data_field_tcam_hw_bank_size_get(
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


uint32 dnx_data_field_tcam_nof_entries_80_bits_get(
    int unit);


uint32 dnx_data_field_tcam_nof_entries_160_bits_get(
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


uint32 dnx_data_field_tcam_nof_banks_hitbit_units_get(
    int unit);


uint32 dnx_data_field_tcam_nof_big_bank_lines_per_hitbit_unit_get(
    int unit);


uint32 dnx_data_field_tcam_nof_small_bank_lines_per_hitbit_unit_get(
    int unit);


uint32 dnx_data_field_tcam_app_db_id_size_get(
    int unit);


uint32 dnx_data_field_tcam_tcam_indirect_command_version_get(
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
    dnx_data_field_efes_define_max_nof_field_ios,
    dnx_data_field_efes_define_epmf_exem_zero_padding_added_to_field_io,

    
    _dnx_data_field_efes_define_nof
} dnx_data_field_efes_define_e;



uint32 dnx_data_field_efes_max_nof_key_selects_per_field_io_get(
    int unit);


uint32 dnx_data_field_efes_max_nof_field_ios_get(
    int unit);


uint32 dnx_data_field_efes_epmf_exem_zero_padding_added_to_field_io_get(
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
    dnx_data_field_fem_define_nof_condition_bits,

    
    _dnx_data_field_fem_define_nof
} dnx_data_field_fem_define_e;



uint32 dnx_data_field_fem_max_nof_key_selects_per_field_io_get(
    int unit);


uint32 dnx_data_field_fem_nof_condition_bits_get(
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
    dnx_data_field_context_define_default_ftmh_mc_context,
    dnx_data_field_context_define_default_itmh_context,
    dnx_data_field_context_define_default_itmh_pph_context,
    dnx_data_field_context_define_default_itmh_pph_fhei_context,
    dnx_data_field_context_define_default_itmh_1588_context,
    dnx_data_field_context_define_default_itmh_1588_pph_context,
    dnx_data_field_context_define_default_itmh_1588_pph_fhei_context,
    dnx_data_field_context_define_default_itmh_pph_routing_context,
    dnx_data_field_context_define_default_j1_itmh_context,
    dnx_data_field_context_define_default_j1_itmh_pph_context,
    dnx_data_field_context_define_default_oam_context,
    dnx_data_field_context_define_default_oam_reflector_context,
    dnx_data_field_context_define_default_oam_upmep_context,
    dnx_data_field_context_define_default_j1_learning_2ndpass_context,
    dnx_data_field_context_define_default_rch_remove_context,
    dnx_data_field_context_define_default_rch_extended_encap_context,
    dnx_data_field_context_define_default_nat_context,
    dnx_data_field_context_define_nof_hash_keys,
    dnx_data_field_context_define_default_itmh_pph_unsupported_ext_ipmf2_context,
    dnx_data_field_context_define_default_itmh_pph_fhei_unsupported_ext_ipmf2_context,
    dnx_data_field_context_define_default_itmh_pph_fhei_vlan_ipmf2_context,
    dnx_data_field_context_define_default_itmh_1588_pph_fhei_unsupported_ext_ipmf2_context,
    dnx_data_field_context_define_default_itmh_1588_pph_unsupported_ext_ipmf2_context,
    dnx_data_field_context_define_default_itmh_1588_pph_fhei_vlan_ipmf2_context,
    dnx_data_field_context_define_default_ftmh_mc_ipmf3_context,

    
    _dnx_data_field_context_define_nof
} dnx_data_field_context_define_e;



uint32 dnx_data_field_context_default_context_get(
    int unit);


uint32 dnx_data_field_context_default_ftmh_mc_context_get(
    int unit);


uint32 dnx_data_field_context_default_itmh_context_get(
    int unit);


uint32 dnx_data_field_context_default_itmh_pph_context_get(
    int unit);


uint32 dnx_data_field_context_default_itmh_pph_fhei_context_get(
    int unit);


uint32 dnx_data_field_context_default_itmh_1588_context_get(
    int unit);


uint32 dnx_data_field_context_default_itmh_1588_pph_context_get(
    int unit);


uint32 dnx_data_field_context_default_itmh_1588_pph_fhei_context_get(
    int unit);


uint32 dnx_data_field_context_default_itmh_pph_routing_context_get(
    int unit);


uint32 dnx_data_field_context_default_j1_itmh_context_get(
    int unit);


uint32 dnx_data_field_context_default_j1_itmh_pph_context_get(
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


uint32 dnx_data_field_context_default_rch_extended_encap_context_get(
    int unit);


uint32 dnx_data_field_context_default_nat_context_get(
    int unit);


uint32 dnx_data_field_context_nof_hash_keys_get(
    int unit);


uint32 dnx_data_field_context_default_itmh_pph_unsupported_ext_ipmf2_context_get(
    int unit);


uint32 dnx_data_field_context_default_itmh_pph_fhei_unsupported_ext_ipmf2_context_get(
    int unit);


uint32 dnx_data_field_context_default_itmh_pph_fhei_vlan_ipmf2_context_get(
    int unit);


uint32 dnx_data_field_context_default_itmh_1588_pph_fhei_unsupported_ext_ipmf2_context_get(
    int unit);


uint32 dnx_data_field_context_default_itmh_1588_pph_unsupported_ext_ipmf2_context_get(
    int unit);


uint32 dnx_data_field_context_default_itmh_1588_pph_fhei_vlan_ipmf2_context_get(
    int unit);


uint32 dnx_data_field_context_default_ftmh_mc_ipmf3_context_get(
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
    dnx_data_field_preselector_define_default_j1_itmh_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_j1_itmh_pph_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_j1_itmh_pph_oamp_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_j1_learn_presel_id_1st_pass_ipmf1,
    dnx_data_field_preselector_define_default_j1_learn_presel_id_2nd_pass_ipmf1,
    dnx_data_field_preselector_define_default_nat_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_ftmh_mc_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_itmh_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_itmh_pph_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_itmh_pph_fhei_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_itmh_1588_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_itmh_1588_pph_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_itmh_pph_eth_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_itmh_pph_ipv4_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_itmh_pph_ipv6_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_itmh_pph_mpls_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_itmh_y1711_rcpu_presel_id_ipmf1,
    dnx_data_field_preselector_define_srv6_endpoint_psp_presel_id_ipmf1,
    dnx_data_field_preselector_define_srv6_endpoint_psp_extended_cut_presel_id_ipmf1,
    dnx_data_field_preselector_define_srv6_endpoint_psp_extended_cut_presel_id_ipmf2,
    dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_presel_id_ipmf1,
    dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_trap_presel_id_ipmf1,
    dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_icmp_presel_id_ipmf1,
    dnx_data_field_preselector_define_srv6_egress_usd_eth_presel_id_ipmf1,
    dnx_data_field_preselector_define_srv6_egress_usd_ipv4_presel_id_ipmf1,
    dnx_data_field_preselector_define_srv6_egress_usd_ipv6_presel_id_ipmf1,
    dnx_data_field_preselector_define_srv6_egress_usd_mpls_presel_id_ipmf1,
    dnx_data_field_preselector_define_srv6_endpoint_usid_presel_id_ipmf1,
    dnx_data_field_preselector_define_srv6_endpoint_usid_no_default_presel_id_ipmf1,
    dnx_data_field_preselector_define_srv6_endpoint_gsid_presel_id_ipmf1,
    dnx_data_field_preselector_define_srv6_egress_usp_presel_id_ipmf1,
    dnx_data_field_preselector_define_srv6_egress_usp_trap_presel_id_ipmf1,
    dnx_data_field_preselector_define_srv6_egress_usp_fwd_trap_presel_id_ipmf1,
    dnx_data_field_preselector_define_srv6_egress_usp_icmp_presel_id_ipmf1,
    dnx_data_field_preselector_define_mpls_bier_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_rcy_mirror_sys_hdr_presel_id_ipmf1,
    dnx_data_field_preselector_define_default_itmh_pph_unsupported_ext_presel_id_ipmf2,
    dnx_data_field_preselector_define_default_itmh_pph_fhei_unsupported_ext_presel_id_ipmf2,
    dnx_data_field_preselector_define_default_itmh_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2,
    dnx_data_field_preselector_define_default_itmh_pph_fhei_trap_presel_id_ipmf2,
    dnx_data_field_preselector_define_default_itmh_pph_fhei_vlan_presel_id_ipmf2,
    dnx_data_field_preselector_define_default_itmh_1588_pph_unsupported_ext_presel_id_ipmf2,
    dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_ipmf2,
    dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2,
    dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_trap_presel_id_ipmf2,
    dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_vlan_presel_id_ipmf2,
    dnx_data_field_preselector_define_default_itmh_pph_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_itmh_pph_trap_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_itmh_pph_vlan_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_itmh_1588_pph_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_itmh_1588_pph_trap_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_itmh_1588_pph_vlan_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_itmh_pph_routing_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_j1_itmh_pph_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_ftmh_mc_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_oam_roo_ipv4_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_oam_roo_ipv6_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_oam_w_o_roo_no_rif_ipv4_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_oam_w_o_roo_no_rif_ipv6_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_oam_w_o_roo_with_rif_ipv4_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_oam_w_o_roo_with_rif_ipv6_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_j1_ipv4_mc_in_lif_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_j1_ipv6_mc_in_lif_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_rch_remove_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_rch_extended_encap_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_j1_php_presel_id_ipmf3,
    dnx_data_field_preselector_define_default_j1_swap_presel_id_ipmf3,
    dnx_data_field_preselector_define_srv6_partial_fwd_layer_presel_id_ipmf3,
    dnx_data_field_preselector_define_srv6_endpoint_usid_presel_id_ipmf3,
    dnx_data_field_preselector_define_srv6_endpoint_usid_no_default_presel_id_ipmf3,
    dnx_data_field_preselector_define_srv6_endpoint_gsid_presel_id_ipmf3,
    dnx_data_field_preselector_define_srv6_endpoint_psp_presel_id_ipmf3,
    dnx_data_field_preselector_define_srv6_psp_ext_to_endpoint_presel_id_ipmf3,
    dnx_data_field_preselector_define_srv6_egress_usp_presel_id_ipmf3,
    dnx_data_field_preselector_define_srv6_endpoint_psp_extended_cut_presel_id_ipmf3,
    dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_presel_id_ipmf3,
    dnx_data_field_preselector_define_srv6_egress_usd_presel_id_ipmf3,
    dnx_data_field_preselector_define_mpls_bier_presel_id_ipmf3,
    dnx_data_field_preselector_define_ebtr_const_fwd_layer_eth_epmf,
    dnx_data_field_preselector_define_srv6_ext_termination_btr_epmf,
    dnx_data_field_preselector_define_default_j1_same_port_presel_id_epmf,
    dnx_data_field_preselector_define_default_learn_limit_presel_id_epmf,
    dnx_data_field_preselector_define_srv6_endpoint_hbh_bta_presel_id_epmf,
    dnx_data_field_preselector_define_srv6_transit_hbh_bta_presel_id_epmf,
    dnx_data_field_preselector_define_srv6_endpoint_psp_bta_presel_id_epmf,
    dnx_data_field_preselector_define_default_mpls_8b_fhei_presel_id_epmf,
    dnx_data_field_preselector_define_default_non_pph_presel_id_epmf,
    dnx_data_field_preselector_define_default_nat_presel_id_epmf,
    dnx_data_field_preselector_define_num_cs_inlif_profile_entries,
    dnx_data_field_preselector_define_nof_uint32_in_cs_qual_hw,

    
    _dnx_data_field_preselector_define_nof
} dnx_data_field_preselector_define_e;



uint32 dnx_data_field_preselector_default_j1_itmh_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_j1_itmh_pph_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_j1_itmh_pph_oamp_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_j1_learn_presel_id_1st_pass_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_j1_learn_presel_id_2nd_pass_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_nat_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_ftmh_mc_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_pph_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_pph_fhei_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_1588_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_1588_pph_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_1588_pph_fhei_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_pph_eth_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_pph_ipv4_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_pph_ipv6_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_pph_mpls_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_y1711_rcpu_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_endpoint_psp_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_endpoint_psp_extended_cut_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_endpoint_psp_extended_cut_presel_id_ipmf2_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_trap_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_icmp_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_egress_usd_eth_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_egress_usd_ipv4_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_egress_usd_ipv6_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_egress_usd_mpls_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_endpoint_usid_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_endpoint_usid_no_default_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_endpoint_gsid_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_egress_usp_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_egress_usp_trap_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_egress_usp_fwd_trap_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_egress_usp_icmp_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_mpls_bier_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_rcy_mirror_sys_hdr_presel_id_ipmf1_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_pph_unsupported_ext_presel_id_ipmf2_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_pph_fhei_unsupported_ext_presel_id_ipmf2_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_pph_fhei_trap_presel_id_ipmf2_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_pph_fhei_vlan_presel_id_ipmf2_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_1588_pph_unsupported_ext_presel_id_ipmf2_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_ipmf2_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_1588_pph_fhei_trap_presel_id_ipmf2_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_1588_pph_fhei_vlan_presel_id_ipmf2_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_pph_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_pph_trap_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_pph_vlan_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_1588_pph_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_1588_pph_trap_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_1588_pph_vlan_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_itmh_pph_routing_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_j1_itmh_pph_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_ftmh_mc_presel_id_ipmf3_get(
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


uint32 dnx_data_field_preselector_default_rch_extended_encap_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_j1_php_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_default_j1_swap_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_partial_fwd_layer_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_endpoint_usid_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_endpoint_usid_no_default_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_endpoint_gsid_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_endpoint_psp_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_psp_ext_to_endpoint_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_egress_usp_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_endpoint_psp_extended_cut_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_egress_usd_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_mpls_bier_presel_id_ipmf3_get(
    int unit);


uint32 dnx_data_field_preselector_ebtr_const_fwd_layer_eth_epmf_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_ext_termination_btr_epmf_get(
    int unit);


uint32 dnx_data_field_preselector_default_j1_same_port_presel_id_epmf_get(
    int unit);


uint32 dnx_data_field_preselector_default_learn_limit_presel_id_epmf_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_endpoint_hbh_bta_presel_id_epmf_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_transit_hbh_bta_presel_id_epmf_get(
    int unit);


uint32 dnx_data_field_preselector_srv6_endpoint_psp_bta_presel_id_epmf_get(
    int unit);


uint32 dnx_data_field_preselector_default_mpls_8b_fhei_presel_id_epmf_get(
    int unit);


uint32 dnx_data_field_preselector_default_non_pph_presel_id_epmf_get(
    int unit);


uint32 dnx_data_field_preselector_default_nat_presel_id_epmf_get(
    int unit);


uint32 dnx_data_field_preselector_num_cs_inlif_profile_entries_get(
    int unit);


uint32 dnx_data_field_preselector_nof_uint32_in_cs_qual_hw_get(
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
    dnx_data_field_qual_define_predefined_nof,
    dnx_data_field_qual_define_layer_record_nof,
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
    dnx_data_field_qual_define_l4_ops_pbus_size,
    dnx_data_field_qual_define_ingress_nof_layer_records,
    dnx_data_field_qual_define_ingress_layer_record_size,
    dnx_data_field_qual_define_egress_nof_layer_records,
    dnx_data_field_qual_define_egress_layer_record_size,
    dnx_data_field_qual_define_kbp_extra_offset_after_layer_record_offset,
    dnx_data_field_qual_define_special_metadata_qual_max_parts,

    
    _dnx_data_field_qual_define_nof
} dnx_data_field_qual_define_e;



uint32 dnx_data_field_qual_user_1st_get(
    int unit);


uint32 dnx_data_field_qual_user_nof_get(
    int unit);


uint32 dnx_data_field_qual_predefined_nof_get(
    int unit);


uint32 dnx_data_field_qual_layer_record_nof_get(
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


uint32 dnx_data_field_qual_l4_ops_pbus_size_get(
    int unit);


uint32 dnx_data_field_qual_ingress_nof_layer_records_get(
    int unit);


uint32 dnx_data_field_qual_ingress_layer_record_size_get(
    int unit);


uint32 dnx_data_field_qual_egress_nof_layer_records_get(
    int unit);


uint32 dnx_data_field_qual_egress_layer_record_size_get(
    int unit);


uint32 dnx_data_field_qual_kbp_extra_offset_after_layer_record_offset_get(
    int unit);


uint32 dnx_data_field_qual_special_metadata_qual_max_parts_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_qual_table_nof
} dnx_data_field_qual_table_e;









int dnx_data_field_action_feature_get(
    int unit,
    dnx_data_field_action_feature_e feature);



typedef enum
{
    dnx_data_field_action_define_user_1st,
    dnx_data_field_action_define_user_nof,
    dnx_data_field_action_define_predefined_nof,
    dnx_data_field_action_define_vw_1st,
    dnx_data_field_action_define_vw_nof,

    
    _dnx_data_field_action_define_nof
} dnx_data_field_action_define_e;



uint32 dnx_data_field_action_user_1st_get(
    int unit);


uint32 dnx_data_field_action_user_nof_get(
    int unit);


uint32 dnx_data_field_action_predefined_nof_get(
    int unit);


uint32 dnx_data_field_action_vw_1st_get(
    int unit);


uint32 dnx_data_field_action_vw_nof_get(
    int unit);



typedef enum
{
    dnx_data_field_action_table_params,

    
    _dnx_data_field_action_table_nof
} dnx_data_field_action_table_e;



const dnx_data_field_action_params_t * dnx_data_field_action_params_get(
    int unit,
    int stage,
    int action);



shr_error_e dnx_data_field_action_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_field_action_params_info_get(
    int unit);






int dnx_data_field_virtual_wire_feature_get(
    int unit,
    dnx_data_field_virtual_wire_feature_e feature);



typedef enum
{
    dnx_data_field_virtual_wire_define_signals_nof,
    dnx_data_field_virtual_wire_define_actions_per_signal_nof,
    dnx_data_field_virtual_wire_define_general_data_user_general_containers_size,
    dnx_data_field_virtual_wire_define_general_data_nof_containers,
    dnx_data_field_virtual_wire_define_ipmf1_general_data_index,

    
    _dnx_data_field_virtual_wire_define_nof
} dnx_data_field_virtual_wire_define_e;



uint32 dnx_data_field_virtual_wire_signals_nof_get(
    int unit);


uint32 dnx_data_field_virtual_wire_actions_per_signal_nof_get(
    int unit);


uint32 dnx_data_field_virtual_wire_general_data_user_general_containers_size_get(
    int unit);


uint32 dnx_data_field_virtual_wire_general_data_nof_containers_get(
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
    dnx_data_field_profile_bits_define_ingress_pp_port_key_gen_var_size,
    dnx_data_field_profile_bits_define_max_port_profile_size,
    dnx_data_field_profile_bits_define_nof_bits_in_port_profile,
    dnx_data_field_profile_bits_define_nof_bits_in_ingress_pp_port_general_data,
    dnx_data_field_profile_bits_define_nof_ing_in_lif,
    dnx_data_field_profile_bits_define_nof_ing_eth_rif,

    
    _dnx_data_field_profile_bits_define_nof
} dnx_data_field_profile_bits_define_e;



uint32 dnx_data_field_profile_bits_ingress_pp_port_key_gen_var_size_get(
    int unit);


uint32 dnx_data_field_profile_bits_max_port_profile_size_get(
    int unit);


uint32 dnx_data_field_profile_bits_nof_bits_in_port_profile_get(
    int unit);


uint32 dnx_data_field_profile_bits_nof_bits_in_ingress_pp_port_general_data_get(
    int unit);


uint32 dnx_data_field_profile_bits_nof_ing_in_lif_get(
    int unit);


uint32 dnx_data_field_profile_bits_nof_ing_eth_rif_get(
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

    
    _dnx_data_field_dir_ext_define_nof
} dnx_data_field_dir_ext_define_e;



uint32 dnx_data_field_dir_ext_half_key_size_get(
    int unit);


uint32 dnx_data_field_dir_ext_single_key_size_get(
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
    dnx_data_field_state_table_define_supported,
    dnx_data_field_state_table_define_address_size_entry_max,
    dnx_data_field_state_table_define_data_size_entry_max,
    dnx_data_field_state_table_define_address_max_entry_max,
    dnx_data_field_state_table_define_wr_bit_size_rw,
    dnx_data_field_state_table_define_opcode_size_rmw,
    dnx_data_field_state_table_define_ipmf2_key_select_supported,
    dnx_data_field_state_table_define_data_size,
    dnx_data_field_state_table_define_state_table_stage_key,
    dnx_data_field_state_table_define_address_size,
    dnx_data_field_state_table_define_address_max,
    dnx_data_field_state_table_define_wr_bit_size,
    dnx_data_field_state_table_define_key_size,

    
    _dnx_data_field_state_table_define_nof
} dnx_data_field_state_table_define_e;



uint32 dnx_data_field_state_table_supported_get(
    int unit);


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


uint32 dnx_data_field_state_table_ipmf2_key_select_supported_get(
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









int dnx_data_field_sce_feature_get(
    int unit,
    dnx_data_field_sce_feature_e feature);



typedef enum
{

    
    _dnx_data_field_sce_define_nof
} dnx_data_field_sce_define_e;




typedef enum
{

    
    _dnx_data_field_sce_table_nof
} dnx_data_field_sce_table_e;









int dnx_data_field_mdb_dt_feature_get(
    int unit,
    dnx_data_field_mdb_dt_feature_e feature);



typedef enum
{
    dnx_data_field_mdb_dt_define_supported,
    dnx_data_field_mdb_dt_define_key_size,
    dnx_data_field_mdb_dt_define_action_size_large,
    dnx_data_field_mdb_dt_define_ipmf1_key_select_supported,
    dnx_data_field_mdb_dt_define_ipmf1_key_select,
    dnx_data_field_mdb_dt_define_ipmf2_key_select_supported,
    dnx_data_field_mdb_dt_define_ipmf2_key_select,
    dnx_data_field_mdb_dt_define_ipmf3_key_select_supported,
    dnx_data_field_mdb_dt_define_ipmf3_key_select,
    dnx_data_field_mdb_dt_define_ipmf1_ipmf2_concurrent_support,
    dnx_data_field_mdb_dt_define_signal_block_name_is_map,
    dnx_data_field_mdb_dt_define_signal_key_is_entire_key,
    dnx_data_field_mdb_dt_define_pmf_map_stage,

    
    _dnx_data_field_mdb_dt_define_nof
} dnx_data_field_mdb_dt_define_e;



uint32 dnx_data_field_mdb_dt_supported_get(
    int unit);


uint32 dnx_data_field_mdb_dt_key_size_get(
    int unit);


uint32 dnx_data_field_mdb_dt_action_size_large_get(
    int unit);


uint32 dnx_data_field_mdb_dt_ipmf1_key_select_supported_get(
    int unit);


uint32 dnx_data_field_mdb_dt_ipmf1_key_select_get(
    int unit);


uint32 dnx_data_field_mdb_dt_ipmf2_key_select_supported_get(
    int unit);


uint32 dnx_data_field_mdb_dt_ipmf2_key_select_get(
    int unit);


uint32 dnx_data_field_mdb_dt_ipmf3_key_select_supported_get(
    int unit);


uint32 dnx_data_field_mdb_dt_ipmf3_key_select_get(
    int unit);


uint32 dnx_data_field_mdb_dt_ipmf1_ipmf2_concurrent_support_get(
    int unit);


uint32 dnx_data_field_mdb_dt_signal_block_name_is_map_get(
    int unit);


uint32 dnx_data_field_mdb_dt_signal_key_is_entire_key_get(
    int unit);


uint32 dnx_data_field_mdb_dt_pmf_map_stage_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_mdb_dt_table_nof
} dnx_data_field_mdb_dt_table_e;









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
    dnx_data_field_udh_define_ext_supported,
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



uint32 dnx_data_field_udh_ext_supported_get(
    int unit);


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
    dnx_data_field_exem_define_max_result_size_all_stages,
    dnx_data_field_exem_define_small_max_container_size,
    dnx_data_field_exem_define_large_max_container_size,
    dnx_data_field_exem_define_vmv_hw_max_size,
    dnx_data_field_exem_define_small_min_app_db_id_range,
    dnx_data_field_exem_define_large_min_app_db_id_range,
    dnx_data_field_exem_define_small_max_app_db_id_range,
    dnx_data_field_exem_define_large_max_app_db_id_range,
    dnx_data_field_exem_define_small_ipmf2_key,
    dnx_data_field_exem_define_small_ipmf2_key_part,
    dnx_data_field_exem_define_small_ipmf2_key_hw_value,
    dnx_data_field_exem_define_small_ipmf2_key_hw_bits,
    dnx_data_field_exem_define_ipmf1_key_configurable,
    dnx_data_field_exem_define_ipmf2_key_configurable,
    dnx_data_field_exem_define_ipmf1_key,
    dnx_data_field_exem_define_debug_key_signal_includes_zero_padding,

    dnx_data_field_exem_define_pmf_sexem3_stage,


    
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


uint32 dnx_data_field_exem_max_result_size_all_stages_get(
    int unit);


uint32 dnx_data_field_exem_small_max_container_size_get(
    int unit);


uint32 dnx_data_field_exem_large_max_container_size_get(
    int unit);


uint32 dnx_data_field_exem_vmv_hw_max_size_get(
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


uint32 dnx_data_field_exem_ipmf1_key_configurable_get(
    int unit);


uint32 dnx_data_field_exem_ipmf2_key_configurable_get(
    int unit);


uint32 dnx_data_field_exem_ipmf1_key_get(
    int unit);


uint32 dnx_data_field_exem_debug_key_signal_includes_zero_padding_get(
    int unit);



uint32 dnx_data_field_exem_pmf_sexem3_stage_get(
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
    dnx_data_field_exem_learn_flush_machine_define_nof_flush_profiles,

    
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


uint32 dnx_data_field_exem_learn_flush_machine_nof_flush_profiles_get(
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
    dnx_data_field_ace_define_supported,
    dnx_data_field_ace_define_ace_id_size,
    dnx_data_field_ace_define_key_size,
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
    dnx_data_field_ace_define_hw_invalid_action_id,

    
    _dnx_data_field_ace_define_nof
} dnx_data_field_ace_define_e;



uint32 dnx_data_field_ace_supported_get(
    int unit);


uint32 dnx_data_field_ace_ace_id_size_get(
    int unit);


uint32 dnx_data_field_ace_key_size_get(
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


uint32 dnx_data_field_ace_hw_invalid_action_id_get(
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
    dnx_data_field_encoded_qual_actions_offset_define_forward_trap_qualifier_offset,

    
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


uint32 dnx_data_field_encoded_qual_actions_offset_forward_trap_qualifier_offset_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_encoded_qual_actions_offset_table_nof
} dnx_data_field_encoded_qual_actions_offset_table_e;









int dnx_data_field_compare_feature_get(
    int unit,
    dnx_data_field_compare_feature_e feature);



typedef enum
{
    dnx_data_field_compare_define_equal_offset,
    dnx_data_field_compare_define_not_equal_offset,
    dnx_data_field_compare_define_smaller_offset,
    dnx_data_field_compare_define_not_smaller_offset,
    dnx_data_field_compare_define_bigger_offset,
    dnx_data_field_compare_define_not_bigger_offset,
    dnx_data_field_compare_define_nof_keys,
    dnx_data_field_compare_define_nof_compare_pairs,

    
    _dnx_data_field_compare_define_nof
} dnx_data_field_compare_define_e;



uint32 dnx_data_field_compare_equal_offset_get(
    int unit);


uint32 dnx_data_field_compare_not_equal_offset_get(
    int unit);


uint32 dnx_data_field_compare_smaller_offset_get(
    int unit);


uint32 dnx_data_field_compare_not_smaller_offset_get(
    int unit);


uint32 dnx_data_field_compare_bigger_offset_get(
    int unit);


uint32 dnx_data_field_compare_not_bigger_offset_get(
    int unit);


uint32 dnx_data_field_compare_nof_keys_get(
    int unit);


uint32 dnx_data_field_compare_nof_compare_pairs_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_compare_table_nof
} dnx_data_field_compare_table_e;









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
    dnx_data_field_diag_define_key_signal_size_in_bytes,
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


uint32 dnx_data_field_diag_key_signal_size_in_bytes_get(
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









int dnx_data_field_general_data_qualifiers_feature_get(
    int unit,
    dnx_data_field_general_data_qualifiers_feature_e feature);



typedef enum
{
    dnx_data_field_general_data_qualifiers_define_vw_supported,
    dnx_data_field_general_data_qualifiers_define_ac_in_lif_wide_data_nof_bits,
    dnx_data_field_general_data_qualifiers_define_pwe_in_lif_wide_data_nof_bits,
    dnx_data_field_general_data_qualifiers_define_ip_tunnel_in_lif_wide_data_nof_bits,
    dnx_data_field_general_data_qualifiers_define_native_ac_in_lif_wide_data_nof_bits,
    dnx_data_field_general_data_qualifiers_define_ac_in_lif_wide_data_extended_nof_bits,
    dnx_data_field_general_data_qualifiers_define_mact_entry_grouping_nof_bits,
    dnx_data_field_general_data_qualifiers_define_vw_vip_valid_nof_bits,
    dnx_data_field_general_data_qualifiers_define_vw_vip_id_nof_bits,
    dnx_data_field_general_data_qualifiers_define_vw_member_reference_nof_bits,
    dnx_data_field_general_data_qualifiers_define_vw_pcc_hit_nof_bits,
    dnx_data_field_general_data_qualifiers_define_l3srcbind_hit_nof_bits,
    dnx_data_field_general_data_qualifiers_define_srv6_next_si,
    dnx_data_field_general_data_qualifiers_define_srv6_next_gsid,

    
    _dnx_data_field_general_data_qualifiers_define_nof
} dnx_data_field_general_data_qualifiers_define_e;



uint32 dnx_data_field_general_data_qualifiers_vw_supported_get(
    int unit);


uint32 dnx_data_field_general_data_qualifiers_ac_in_lif_wide_data_nof_bits_get(
    int unit);


uint32 dnx_data_field_general_data_qualifiers_pwe_in_lif_wide_data_nof_bits_get(
    int unit);


uint32 dnx_data_field_general_data_qualifiers_ip_tunnel_in_lif_wide_data_nof_bits_get(
    int unit);


uint32 dnx_data_field_general_data_qualifiers_native_ac_in_lif_wide_data_nof_bits_get(
    int unit);


uint32 dnx_data_field_general_data_qualifiers_ac_in_lif_wide_data_extended_nof_bits_get(
    int unit);


uint32 dnx_data_field_general_data_qualifiers_mact_entry_grouping_nof_bits_get(
    int unit);


uint32 dnx_data_field_general_data_qualifiers_vw_vip_valid_nof_bits_get(
    int unit);


uint32 dnx_data_field_general_data_qualifiers_vw_vip_id_nof_bits_get(
    int unit);


uint32 dnx_data_field_general_data_qualifiers_vw_member_reference_nof_bits_get(
    int unit);


uint32 dnx_data_field_general_data_qualifiers_vw_pcc_hit_nof_bits_get(
    int unit);


uint32 dnx_data_field_general_data_qualifiers_l3srcbind_hit_nof_bits_get(
    int unit);


uint32 dnx_data_field_general_data_qualifiers_srv6_next_si_get(
    int unit);


uint32 dnx_data_field_general_data_qualifiers_srv6_next_gsid_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_general_data_qualifiers_table_nof
} dnx_data_field_general_data_qualifiers_table_e;









int dnx_data_field_common_max_val_feature_get(
    int unit,
    dnx_data_field_common_max_val_feature_e feature);



typedef enum
{
    dnx_data_field_common_max_val_define_nof_ffc,
    dnx_data_field_common_max_val_define_nof_masks_per_fes,
    dnx_data_field_common_max_val_define_nof_fes_id_per_array,
    dnx_data_field_common_max_val_define_nof_fes_array,
    dnx_data_field_common_max_val_define_nof_fes_instruction_per_context,
    dnx_data_field_common_max_val_define_nof_fes_programs,
    dnx_data_field_common_max_val_define_nof_prog_per_fes,
    dnx_data_field_common_max_val_define_nof_cs_lines,
    dnx_data_field_common_max_val_define_nof_contexts,
    dnx_data_field_common_max_val_define_nof_actions,
    dnx_data_field_common_max_val_define_nof_qualifiers,
    dnx_data_field_common_max_val_define_nof_bits_in_fes_action,
    dnx_data_field_common_max_val_define_nof_bits_in_fes_key_select,
    dnx_data_field_common_max_val_define_nof_bits_in_ffc,
    dnx_data_field_common_max_val_define_nof_ffc_in_uint32,
    dnx_data_field_common_max_val_define_nof_action_per_group,
    dnx_data_field_common_max_val_define_nof_layer_records,
    dnx_data_field_common_max_val_define_layer_record_size,
    dnx_data_field_common_max_val_define_nof_l4_ops_ranges_legacy,
    dnx_data_field_common_max_val_define_nof_pkt_hdr_ranges,
    dnx_data_field_common_max_val_define_nof_out_lif_ranges,
    dnx_data_field_common_max_val_define_kbr_size,
    dnx_data_field_common_max_val_define_nof_compare_pairs_in_compare_mode,
    dnx_data_field_common_max_val_define_nof_bits_in_fem_action,
    dnx_data_field_common_max_val_define_nof_fem_condition,
    dnx_data_field_common_max_val_define_nof_fem_map_index,
    dnx_data_field_common_max_val_define_nof_fem_programs,
    dnx_data_field_common_max_val_define_nof_fem_id,
    dnx_data_field_common_max_val_define_nof_array_ids,
    dnx_data_field_common_max_val_define_dbal_pairs,

    
    _dnx_data_field_common_max_val_define_nof
} dnx_data_field_common_max_val_define_e;



uint32 dnx_data_field_common_max_val_nof_ffc_get(
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


uint32 dnx_data_field_common_max_val_nof_cs_lines_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_contexts_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_actions_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_qualifiers_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_bits_in_fes_action_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_bits_in_fes_key_select_get(
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


uint32 dnx_data_field_common_max_val_nof_compare_pairs_in_compare_mode_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_bits_in_fem_action_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_fem_condition_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_fem_map_index_get(
    int unit);


uint32 dnx_data_field_common_max_val_nof_fem_programs_get(
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
    dnx_data_field_init_define_roo,
    dnx_data_field_init_define_disable_erpp_counters,
    dnx_data_field_init_define_jr1_ipmc_inlif,
    dnx_data_field_init_define_j1_same_port,
    dnx_data_field_init_define_j1_learning,
    dnx_data_field_init_define_learn_limit,
    dnx_data_field_init_define_j1_php,
    dnx_data_field_init_define_ftmh_mc,
    dnx_data_field_init_define_flow_id,

    
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


uint32 dnx_data_field_init_roo_get(
    int unit);


uint32 dnx_data_field_init_disable_erpp_counters_get(
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


uint32 dnx_data_field_init_ftmh_mc_get(
    int unit);


uint32 dnx_data_field_init_flow_id_get(
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
    dnx_data_field_features_define_tcam_cs_is_tcam_direct_access,
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
    dnx_data_field_features_define_dir_ext_epmf,
    dnx_data_field_features_define_dir_ext_epmf_key_has_debug_signal,
    dnx_data_field_features_define_hashing_process_lsb_to_msb,
    dnx_data_field_features_define_exem_age_flush_scan,
    dnx_data_field_features_define_epmf_has_lookup_enabler,
    dnx_data_field_features_define_epmf_outlif_profile_map,
    dnx_data_field_features_define_tcam_full_key_half_payload_both_lsb_msb,
    dnx_data_field_features_define_epmf_exem_debug_signal_move_zero_padding_from_msb_to_lsb,
    dnx_data_field_features_define_ecmp_tunnel_priority,
    dnx_data_field_features_define_key_result_hit_signals_exist,
    dnx_data_field_features_define_external_cs_based_on_fwd2,
    dnx_data_field_features_define_udh_base_cs_is_mapped,
    dnx_data_field_features_define_state_collection_engine,
    dnx_data_field_features_define_cs_lif_profile,
    dnx_data_field_features_define_empf_trap_action_is_strength_based,
    dnx_data_field_features_define_has_last_keys,
    dnx_data_field_features_define_ipmf3_has_last_keys,
    dnx_data_field_features_define_ipmf3_context_signal_is_from_register,
    dnx_data_field_features_define_epmf_context_signal_exists,
    dnx_data_field_features_define_epmf_context_signal_from_phl_block,
    dnx_data_field_features_define_epmf_is_on_etpp,
    dnx_data_field_features_define_de_has_debug_signals,
    dnx_data_field_features_define_result_hit_signal_names_always_include_key,
    dnx_data_field_features_define_ipmf3_context_has_oam_stateless_property,

    
    _dnx_data_field_features_define_nof
} dnx_data_field_features_define_e;



uint32 dnx_data_field_features_tcam_cs_is_tcam_direct_access_get(
    int unit);


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


uint32 dnx_data_field_features_dir_ext_epmf_get(
    int unit);


uint32 dnx_data_field_features_dir_ext_epmf_key_has_debug_signal_get(
    int unit);


uint32 dnx_data_field_features_hashing_process_lsb_to_msb_get(
    int unit);


uint32 dnx_data_field_features_exem_age_flush_scan_get(
    int unit);


uint32 dnx_data_field_features_epmf_has_lookup_enabler_get(
    int unit);


uint32 dnx_data_field_features_epmf_outlif_profile_map_get(
    int unit);


uint32 dnx_data_field_features_tcam_full_key_half_payload_both_lsb_msb_get(
    int unit);


uint32 dnx_data_field_features_epmf_exem_debug_signal_move_zero_padding_from_msb_to_lsb_get(
    int unit);


uint32 dnx_data_field_features_ecmp_tunnel_priority_get(
    int unit);


uint32 dnx_data_field_features_key_result_hit_signals_exist_get(
    int unit);


uint32 dnx_data_field_features_external_cs_based_on_fwd2_get(
    int unit);


uint32 dnx_data_field_features_udh_base_cs_is_mapped_get(
    int unit);


uint32 dnx_data_field_features_state_collection_engine_get(
    int unit);


uint32 dnx_data_field_features_cs_lif_profile_get(
    int unit);


uint32 dnx_data_field_features_empf_trap_action_is_strength_based_get(
    int unit);


uint32 dnx_data_field_features_has_last_keys_get(
    int unit);


uint32 dnx_data_field_features_ipmf3_has_last_keys_get(
    int unit);


uint32 dnx_data_field_features_ipmf3_context_signal_is_from_register_get(
    int unit);


uint32 dnx_data_field_features_epmf_context_signal_exists_get(
    int unit);


uint32 dnx_data_field_features_epmf_context_signal_from_phl_block_get(
    int unit);


uint32 dnx_data_field_features_epmf_is_on_etpp_get(
    int unit);


uint32 dnx_data_field_features_de_has_debug_signals_get(
    int unit);


uint32 dnx_data_field_features_result_hit_signal_names_always_include_key_get(
    int unit);


uint32 dnx_data_field_features_ipmf3_context_has_oam_stateless_property_get(
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
    dnx_data_field_tests_define_epmf_debug_rely_on_tcam0,
    dnx_data_field_tests_define_epmf_tm_port_cs,
    dnx_data_field_tests_define_small_exem_addr_bits,
    dnx_data_field_tests_define_stub_adapter_supports_semantic_flush,
    dnx_data_field_tests_define_parsing_start_offset_exists,
    dnx_data_field_tests_define_first_part_of_acr_zero_padded_by_32,
    dnx_data_field_tests_define_ext_stat_valid_can_be_read_in_tm_cmd,

    
    _dnx_data_field_tests_define_nof
} dnx_data_field_tests_define_e;



uint32 dnx_data_field_tests_learn_info_actions_structure_change_get(
    int unit);


uint32 dnx_data_field_tests_ingress_time_stamp_increased_get(
    int unit);


uint32 dnx_data_field_tests_ace_debug_signals_exist_get(
    int unit);


uint32 dnx_data_field_tests_epmf_debug_rely_on_tcam0_get(
    int unit);


uint32 dnx_data_field_tests_epmf_tm_port_cs_get(
    int unit);


uint32 dnx_data_field_tests_small_exem_addr_bits_get(
    int unit);


uint32 dnx_data_field_tests_stub_adapter_supports_semantic_flush_get(
    int unit);


uint32 dnx_data_field_tests_parsing_start_offset_exists_get(
    int unit);


uint32 dnx_data_field_tests_first_part_of_acr_zero_padded_by_32_get(
    int unit);


uint32 dnx_data_field_tests_ext_stat_valid_can_be_read_in_tm_cmd_get(
    int unit);



typedef enum
{

    
    _dnx_data_field_tests_table_nof
} dnx_data_field_tests_table_e;






shr_error_e dnx_data_field_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

