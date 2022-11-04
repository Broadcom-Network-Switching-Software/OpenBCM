
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_FIELD_H_

#define _DNX_DATA_FIELD_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_field_init(
    int unit);







typedef enum
{

    
    _dnx_data_field_base_ipmf1_feature_nof
} dnx_data_field_base_ipmf1_feature_e;



typedef int(
    *dnx_data_field_base_ipmf1_feature_get_f) (
    int unit,
    dnx_data_field_base_ipmf1_feature_e feature);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_ffc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_keys_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_keys_alloc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_keys_alloc_for_tcam_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_keys_alloc_for_exem_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_keys_alloc_for_dir_ext_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_keys_alloc_for_mdb_dt_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_masks_per_fes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_fes_id_per_array_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_fes_array_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_fes_instruction_per_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_fes_programs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_prog_per_fes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_cs_container_5_selected_bits_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_contexts_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_link_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_cs_lines_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_actions_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_qualifiers_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_bits_in_fes_action_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_bits_in_fes_key_select_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_fes_key_select_resolution_in_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_fem_condition_ms_bit_min_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_fem_programs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_log_nof_bits_in_fem_key_select_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_bits_in_fem_key_select_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_fem_key_select_resolution_in_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_log_nof_bits_in_fem_map_data_field_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_bits_in_fem_map_data_field_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_bits_in_fem_action_fems_2_15_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_bits_in_fem_action_fems_0_1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_bits_in_fem_action_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_fem_condition_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_fem_map_index_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_fem_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_num_fems_with_short_action_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_num_bits_in_fem_field_select_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_fem_id_per_array_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_fem_array_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_compare_pairs_in_compare_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_compare_key_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_l4_ops_ranges_legacy_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_ext_l4_ops_ranges_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_pkt_hdr_ranges_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_nof_out_lif_ranges_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_cmp_selection_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_dir_ext_single_key_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_dir_ext_first_key_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_exem_max_result_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_fes_instruction_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_fes_pgm_id_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_fes_key_select_for_zero_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_fes_shift_for_zero_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf1_exem_pdb_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_base_ipmf1_feature_get_f feature_get;
    
    dnx_data_field_base_ipmf1_nof_ffc_get_f nof_ffc_get;
    
    dnx_data_field_base_ipmf1_nof_keys_get_f nof_keys_get;
    
    dnx_data_field_base_ipmf1_nof_keys_alloc_get_f nof_keys_alloc_get;
    
    dnx_data_field_base_ipmf1_nof_keys_alloc_for_tcam_get_f nof_keys_alloc_for_tcam_get;
    
    dnx_data_field_base_ipmf1_nof_keys_alloc_for_exem_get_f nof_keys_alloc_for_exem_get;
    
    dnx_data_field_base_ipmf1_nof_keys_alloc_for_dir_ext_get_f nof_keys_alloc_for_dir_ext_get;
    
    dnx_data_field_base_ipmf1_nof_keys_alloc_for_mdb_dt_get_f nof_keys_alloc_for_mdb_dt_get;
    
    dnx_data_field_base_ipmf1_nof_masks_per_fes_get_f nof_masks_per_fes_get;
    
    dnx_data_field_base_ipmf1_nof_fes_id_per_array_get_f nof_fes_id_per_array_get;
    
    dnx_data_field_base_ipmf1_nof_fes_array_get_f nof_fes_array_get;
    
    dnx_data_field_base_ipmf1_nof_fes_instruction_per_context_get_f nof_fes_instruction_per_context_get;
    
    dnx_data_field_base_ipmf1_nof_fes_programs_get_f nof_fes_programs_get;
    
    dnx_data_field_base_ipmf1_nof_prog_per_fes_get_f nof_prog_per_fes_get;
    
    dnx_data_field_base_ipmf1_cs_container_5_selected_bits_size_get_f cs_container_5_selected_bits_size_get;
    
    dnx_data_field_base_ipmf1_nof_contexts_get_f nof_contexts_get;
    
    dnx_data_field_base_ipmf1_nof_link_profiles_get_f nof_link_profiles_get;
    
    dnx_data_field_base_ipmf1_nof_cs_lines_get_f nof_cs_lines_get;
    
    dnx_data_field_base_ipmf1_nof_actions_get_f nof_actions_get;
    
    dnx_data_field_base_ipmf1_nof_qualifiers_get_f nof_qualifiers_get;
    
    dnx_data_field_base_ipmf1_nof_bits_in_fes_action_get_f nof_bits_in_fes_action_get;
    
    dnx_data_field_base_ipmf1_nof_bits_in_fes_key_select_get_f nof_bits_in_fes_key_select_get;
    
    dnx_data_field_base_ipmf1_fes_key_select_resolution_in_bits_get_f fes_key_select_resolution_in_bits_get;
    
    dnx_data_field_base_ipmf1_fem_condition_ms_bit_min_value_get_f fem_condition_ms_bit_min_value_get;
    
    dnx_data_field_base_ipmf1_nof_fem_programs_get_f nof_fem_programs_get;
    
    dnx_data_field_base_ipmf1_log_nof_bits_in_fem_key_select_get_f log_nof_bits_in_fem_key_select_get;
    
    dnx_data_field_base_ipmf1_nof_bits_in_fem_key_select_get_f nof_bits_in_fem_key_select_get;
    
    dnx_data_field_base_ipmf1_fem_key_select_resolution_in_bits_get_f fem_key_select_resolution_in_bits_get;
    
    dnx_data_field_base_ipmf1_log_nof_bits_in_fem_map_data_field_get_f log_nof_bits_in_fem_map_data_field_get;
    
    dnx_data_field_base_ipmf1_nof_bits_in_fem_map_data_field_get_f nof_bits_in_fem_map_data_field_get;
    
    dnx_data_field_base_ipmf1_nof_bits_in_fem_action_fems_2_15_get_f nof_bits_in_fem_action_fems_2_15_get;
    
    dnx_data_field_base_ipmf1_nof_bits_in_fem_action_fems_0_1_get_f nof_bits_in_fem_action_fems_0_1_get;
    
    dnx_data_field_base_ipmf1_nof_bits_in_fem_action_get_f nof_bits_in_fem_action_get;
    
    dnx_data_field_base_ipmf1_nof_fem_condition_get_f nof_fem_condition_get;
    
    dnx_data_field_base_ipmf1_nof_fem_map_index_get_f nof_fem_map_index_get;
    
    dnx_data_field_base_ipmf1_nof_fem_id_get_f nof_fem_id_get;
    
    dnx_data_field_base_ipmf1_num_fems_with_short_action_get_f num_fems_with_short_action_get;
    
    dnx_data_field_base_ipmf1_num_bits_in_fem_field_select_get_f num_bits_in_fem_field_select_get;
    
    dnx_data_field_base_ipmf1_nof_fem_id_per_array_get_f nof_fem_id_per_array_get;
    
    dnx_data_field_base_ipmf1_nof_fem_array_get_f nof_fem_array_get;
    
    dnx_data_field_base_ipmf1_nof_compare_pairs_in_compare_mode_get_f nof_compare_pairs_in_compare_mode_get;
    
    dnx_data_field_base_ipmf1_compare_key_size_get_f compare_key_size_get;
    
    dnx_data_field_base_ipmf1_nof_l4_ops_ranges_legacy_get_f nof_l4_ops_ranges_legacy_get;
    
    dnx_data_field_base_ipmf1_nof_ext_l4_ops_ranges_get_f nof_ext_l4_ops_ranges_get;
    
    dnx_data_field_base_ipmf1_nof_pkt_hdr_ranges_get_f nof_pkt_hdr_ranges_get;
    
    dnx_data_field_base_ipmf1_nof_out_lif_ranges_get_f nof_out_lif_ranges_get;
    
    dnx_data_field_base_ipmf1_cmp_selection_get_f cmp_selection_get;
    
    dnx_data_field_base_ipmf1_dir_ext_single_key_size_get_f dir_ext_single_key_size_get;
    
    dnx_data_field_base_ipmf1_dir_ext_first_key_get_f dir_ext_first_key_get;
    
    dnx_data_field_base_ipmf1_exem_max_result_size_get_f exem_max_result_size_get;
    
    dnx_data_field_base_ipmf1_fes_instruction_size_get_f fes_instruction_size_get;
    
    dnx_data_field_base_ipmf1_fes_pgm_id_offset_get_f fes_pgm_id_offset_get;
    
    dnx_data_field_base_ipmf1_fes_key_select_for_zero_bit_get_f fes_key_select_for_zero_bit_get;
    
    dnx_data_field_base_ipmf1_fes_shift_for_zero_bit_get_f fes_shift_for_zero_bit_get;
    
    dnx_data_field_base_ipmf1_exem_pdb_get_f exem_pdb_get;
} dnx_data_if_field_base_ipmf1_t;







typedef enum
{

    
    _dnx_data_field_base_ipmf2_feature_nof
} dnx_data_field_base_ipmf2_feature_e;



typedef int(
    *dnx_data_field_base_ipmf2_feature_get_f) (
    int unit,
    dnx_data_field_base_ipmf2_feature_e feature);


typedef uint32(
    *dnx_data_field_base_ipmf2_nof_ffc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf2_nof_keys_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf2_nof_keys_alloc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf2_nof_keys_alloc_for_tcam_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf2_nof_keys_alloc_for_exem_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf2_nof_keys_alloc_for_dir_ext_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf2_nof_keys_alloc_for_mdb_dt_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf2_nof_masks_per_fes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf2_nof_fes_id_per_array_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf2_nof_fes_array_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf2_nof_fes_instruction_per_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf2_nof_cs_lines_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf2_nof_contexts_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf2_nof_qualifiers_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf2_dir_ext_single_key_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf2_dir_ext_first_key_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf2_exem_max_result_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf2_fes_key_select_for_zero_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf2_fes_shift_for_zero_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf2_exem_pdb_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_base_ipmf2_feature_get_f feature_get;
    
    dnx_data_field_base_ipmf2_nof_ffc_get_f nof_ffc_get;
    
    dnx_data_field_base_ipmf2_nof_keys_get_f nof_keys_get;
    
    dnx_data_field_base_ipmf2_nof_keys_alloc_get_f nof_keys_alloc_get;
    
    dnx_data_field_base_ipmf2_nof_keys_alloc_for_tcam_get_f nof_keys_alloc_for_tcam_get;
    
    dnx_data_field_base_ipmf2_nof_keys_alloc_for_exem_get_f nof_keys_alloc_for_exem_get;
    
    dnx_data_field_base_ipmf2_nof_keys_alloc_for_dir_ext_get_f nof_keys_alloc_for_dir_ext_get;
    
    dnx_data_field_base_ipmf2_nof_keys_alloc_for_mdb_dt_get_f nof_keys_alloc_for_mdb_dt_get;
    
    dnx_data_field_base_ipmf2_nof_masks_per_fes_get_f nof_masks_per_fes_get;
    
    dnx_data_field_base_ipmf2_nof_fes_id_per_array_get_f nof_fes_id_per_array_get;
    
    dnx_data_field_base_ipmf2_nof_fes_array_get_f nof_fes_array_get;
    
    dnx_data_field_base_ipmf2_nof_fes_instruction_per_context_get_f nof_fes_instruction_per_context_get;
    
    dnx_data_field_base_ipmf2_nof_cs_lines_get_f nof_cs_lines_get;
    
    dnx_data_field_base_ipmf2_nof_contexts_get_f nof_contexts_get;
    
    dnx_data_field_base_ipmf2_nof_qualifiers_get_f nof_qualifiers_get;
    
    dnx_data_field_base_ipmf2_dir_ext_single_key_size_get_f dir_ext_single_key_size_get;
    
    dnx_data_field_base_ipmf2_dir_ext_first_key_get_f dir_ext_first_key_get;
    
    dnx_data_field_base_ipmf2_exem_max_result_size_get_f exem_max_result_size_get;
    
    dnx_data_field_base_ipmf2_fes_key_select_for_zero_bit_get_f fes_key_select_for_zero_bit_get;
    
    dnx_data_field_base_ipmf2_fes_shift_for_zero_bit_get_f fes_shift_for_zero_bit_get;
    
    dnx_data_field_base_ipmf2_exem_pdb_get_f exem_pdb_get;
} dnx_data_if_field_base_ipmf2_t;







typedef enum
{

    
    _dnx_data_field_base_ipmf3_feature_nof
} dnx_data_field_base_ipmf3_feature_e;



typedef int(
    *dnx_data_field_base_ipmf3_feature_get_f) (
    int unit,
    dnx_data_field_base_ipmf3_feature_e feature);


typedef uint32(
    *dnx_data_field_base_ipmf3_nof_ffc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_nof_keys_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_nof_keys_alloc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_nof_keys_alloc_for_tcam_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_nof_keys_alloc_for_exem_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_nof_keys_alloc_for_dir_ext_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_nof_keys_alloc_for_mdb_dt_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_nof_masks_per_fes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_nof_fes_id_per_array_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_nof_fes_array_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_nof_fes_instruction_per_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_nof_fes_programs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_nof_prog_per_fes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_cs_scratch_pad_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_nof_contexts_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_nof_cs_lines_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_nof_actions_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_nof_qualifiers_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_nof_bits_in_fes_action_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_nof_bits_in_fes_key_select_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_fes_key_select_resolution_in_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_nof_out_lif_ranges_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_fes_instruction_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_fes_pgm_id_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_dir_ext_single_key_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_dir_ext_first_key_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_exem_max_result_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_nof_fes_used_by_sdk_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_fes_key_select_for_zero_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_fes_shift_for_zero_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ipmf3_exem_pdb_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_base_ipmf3_feature_get_f feature_get;
    
    dnx_data_field_base_ipmf3_nof_ffc_get_f nof_ffc_get;
    
    dnx_data_field_base_ipmf3_nof_keys_get_f nof_keys_get;
    
    dnx_data_field_base_ipmf3_nof_keys_alloc_get_f nof_keys_alloc_get;
    
    dnx_data_field_base_ipmf3_nof_keys_alloc_for_tcam_get_f nof_keys_alloc_for_tcam_get;
    
    dnx_data_field_base_ipmf3_nof_keys_alloc_for_exem_get_f nof_keys_alloc_for_exem_get;
    
    dnx_data_field_base_ipmf3_nof_keys_alloc_for_dir_ext_get_f nof_keys_alloc_for_dir_ext_get;
    
    dnx_data_field_base_ipmf3_nof_keys_alloc_for_mdb_dt_get_f nof_keys_alloc_for_mdb_dt_get;
    
    dnx_data_field_base_ipmf3_nof_masks_per_fes_get_f nof_masks_per_fes_get;
    
    dnx_data_field_base_ipmf3_nof_fes_id_per_array_get_f nof_fes_id_per_array_get;
    
    dnx_data_field_base_ipmf3_nof_fes_array_get_f nof_fes_array_get;
    
    dnx_data_field_base_ipmf3_nof_fes_instruction_per_context_get_f nof_fes_instruction_per_context_get;
    
    dnx_data_field_base_ipmf3_nof_fes_programs_get_f nof_fes_programs_get;
    
    dnx_data_field_base_ipmf3_nof_prog_per_fes_get_f nof_prog_per_fes_get;
    
    dnx_data_field_base_ipmf3_cs_scratch_pad_size_get_f cs_scratch_pad_size_get;
    
    dnx_data_field_base_ipmf3_nof_contexts_get_f nof_contexts_get;
    
    dnx_data_field_base_ipmf3_nof_cs_lines_get_f nof_cs_lines_get;
    
    dnx_data_field_base_ipmf3_nof_actions_get_f nof_actions_get;
    
    dnx_data_field_base_ipmf3_nof_qualifiers_get_f nof_qualifiers_get;
    
    dnx_data_field_base_ipmf3_nof_bits_in_fes_action_get_f nof_bits_in_fes_action_get;
    
    dnx_data_field_base_ipmf3_nof_bits_in_fes_key_select_get_f nof_bits_in_fes_key_select_get;
    
    dnx_data_field_base_ipmf3_fes_key_select_resolution_in_bits_get_f fes_key_select_resolution_in_bits_get;
    
    dnx_data_field_base_ipmf3_nof_out_lif_ranges_get_f nof_out_lif_ranges_get;
    
    dnx_data_field_base_ipmf3_fes_instruction_size_get_f fes_instruction_size_get;
    
    dnx_data_field_base_ipmf3_fes_pgm_id_offset_get_f fes_pgm_id_offset_get;
    
    dnx_data_field_base_ipmf3_dir_ext_single_key_size_get_f dir_ext_single_key_size_get;
    
    dnx_data_field_base_ipmf3_dir_ext_first_key_get_f dir_ext_first_key_get;
    
    dnx_data_field_base_ipmf3_exem_max_result_size_get_f exem_max_result_size_get;
    
    dnx_data_field_base_ipmf3_nof_fes_used_by_sdk_get_f nof_fes_used_by_sdk_get;
    
    dnx_data_field_base_ipmf3_fes_key_select_for_zero_bit_get_f fes_key_select_for_zero_bit_get;
    
    dnx_data_field_base_ipmf3_fes_shift_for_zero_bit_get_f fes_shift_for_zero_bit_get;
    
    dnx_data_field_base_ipmf3_exem_pdb_get_f exem_pdb_get;
} dnx_data_if_field_base_ipmf3_t;







typedef enum
{

    
    _dnx_data_field_base_epmf_feature_nof
} dnx_data_field_base_epmf_feature_e;



typedef int(
    *dnx_data_field_base_epmf_feature_get_f) (
    int unit,
    dnx_data_field_base_epmf_feature_e feature);


typedef uint32(
    *dnx_data_field_base_epmf_nof_ffc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_nof_keys_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_nof_keys_alloc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_nof_keys_alloc_for_tcam_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_nof_keys_alloc_for_exem_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_nof_keys_alloc_for_dir_ext_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_nof_masks_per_fes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_nof_fes_id_per_array_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_nof_fes_array_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_nof_fes_instruction_per_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_nof_fes_programs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_nof_prog_per_fes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_nof_cs_lines_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_nof_contexts_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_nof_actions_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_nof_qualifiers_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_nof_bits_in_fes_action_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_nof_bits_in_fes_key_select_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_fes_key_select_resolution_in_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_nof_l4_ops_ranges_legacy_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_fes_instruction_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_fes_pgm_id_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_dir_ext_single_key_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_dir_ext_first_key_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_exem_max_result_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_fes_key_select_for_zero_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_fes_shift_for_zero_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_epmf_exem_pdb_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_base_epmf_feature_get_f feature_get;
    
    dnx_data_field_base_epmf_nof_ffc_get_f nof_ffc_get;
    
    dnx_data_field_base_epmf_nof_keys_get_f nof_keys_get;
    
    dnx_data_field_base_epmf_nof_keys_alloc_get_f nof_keys_alloc_get;
    
    dnx_data_field_base_epmf_nof_keys_alloc_for_tcam_get_f nof_keys_alloc_for_tcam_get;
    
    dnx_data_field_base_epmf_nof_keys_alloc_for_exem_get_f nof_keys_alloc_for_exem_get;
    
    dnx_data_field_base_epmf_nof_keys_alloc_for_dir_ext_get_f nof_keys_alloc_for_dir_ext_get;
    
    dnx_data_field_base_epmf_nof_masks_per_fes_get_f nof_masks_per_fes_get;
    
    dnx_data_field_base_epmf_nof_fes_id_per_array_get_f nof_fes_id_per_array_get;
    
    dnx_data_field_base_epmf_nof_fes_array_get_f nof_fes_array_get;
    
    dnx_data_field_base_epmf_nof_fes_instruction_per_context_get_f nof_fes_instruction_per_context_get;
    
    dnx_data_field_base_epmf_nof_fes_programs_get_f nof_fes_programs_get;
    
    dnx_data_field_base_epmf_nof_prog_per_fes_get_f nof_prog_per_fes_get;
    
    dnx_data_field_base_epmf_nof_cs_lines_get_f nof_cs_lines_get;
    
    dnx_data_field_base_epmf_nof_contexts_get_f nof_contexts_get;
    
    dnx_data_field_base_epmf_nof_actions_get_f nof_actions_get;
    
    dnx_data_field_base_epmf_nof_qualifiers_get_f nof_qualifiers_get;
    
    dnx_data_field_base_epmf_nof_bits_in_fes_action_get_f nof_bits_in_fes_action_get;
    
    dnx_data_field_base_epmf_nof_bits_in_fes_key_select_get_f nof_bits_in_fes_key_select_get;
    
    dnx_data_field_base_epmf_fes_key_select_resolution_in_bits_get_f fes_key_select_resolution_in_bits_get;
    
    dnx_data_field_base_epmf_nof_l4_ops_ranges_legacy_get_f nof_l4_ops_ranges_legacy_get;
    
    dnx_data_field_base_epmf_fes_instruction_size_get_f fes_instruction_size_get;
    
    dnx_data_field_base_epmf_fes_pgm_id_offset_get_f fes_pgm_id_offset_get;
    
    dnx_data_field_base_epmf_dir_ext_single_key_size_get_f dir_ext_single_key_size_get;
    
    dnx_data_field_base_epmf_dir_ext_first_key_get_f dir_ext_first_key_get;
    
    dnx_data_field_base_epmf_exem_max_result_size_get_f exem_max_result_size_get;
    
    dnx_data_field_base_epmf_fes_key_select_for_zero_bit_get_f fes_key_select_for_zero_bit_get;
    
    dnx_data_field_base_epmf_fes_shift_for_zero_bit_get_f fes_shift_for_zero_bit_get;
    
    dnx_data_field_base_epmf_exem_pdb_get_f exem_pdb_get;
} dnx_data_if_field_base_epmf_t;







typedef enum
{

    
    _dnx_data_field_base_ifwd2_feature_nof
} dnx_data_field_base_ifwd2_feature_e;



typedef int(
    *dnx_data_field_base_ifwd2_feature_get_f) (
    int unit,
    dnx_data_field_base_ifwd2_feature_e feature);


typedef uint32(
    *dnx_data_field_base_ifwd2_nof_ffc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ifwd2_nof_keys_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ifwd2_nof_contexts_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_base_ifwd2_nof_cs_lines_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_base_ifwd2_feature_get_f feature_get;
    
    dnx_data_field_base_ifwd2_nof_ffc_get_f nof_ffc_get;
    
    dnx_data_field_base_ifwd2_nof_keys_get_f nof_keys_get;
    
    dnx_data_field_base_ifwd2_nof_contexts_get_f nof_contexts_get;
    
    dnx_data_field_base_ifwd2_nof_cs_lines_get_f nof_cs_lines_get;
} dnx_data_if_field_base_ifwd2_t;






typedef struct
{
    int valid;
    int nof_ffc;
    int nof_keys;
    int nof_keys_alloc;
    int nof_keys_alloc_for_tcam;
    int nof_keys_alloc_for_exem;
    int nof_keys_alloc_for_dir_ext;
    int nof_keys_alloc_for_mdb_dt;
    int nof_masks_per_fes;
    int nof_fes_id_per_array;
    int nof_fes_array;
    int nof_fes_instruction_per_context;
    int nof_fes_programs;
    int nof_prog_per_fes;
    int cs_scratch_pad_size;
    int cs_container_5_selected_bits_size;
    int nof_contexts;
    int nof_link_profiles;
    int nof_cs_lines;
    int nof_actions;
    int nof_qualifiers;
    int nof_bits_in_fes_action;
    int nof_bits_in_fes_key_select;
    int fes_key_select_resolution_in_bits;
    int fem_condition_ms_bit_min_value;
    int nof_fem_programs;
    int log_nof_bits_in_fem_key_select;
    int nof_bits_in_fem_key_select;
    int fem_key_select_resolution_in_bits;
    int log_nof_bits_in_fem_map_data_field;
    int nof_bits_in_fem_map_data_field;
    int nof_bits_in_fem_action_fems_2_15;
    int nof_bits_in_fem_action_fems_0_1;
    int nof_bits_in_fem_action;
    int nof_fem_condition;
    int nof_fem_map_index;
    int nof_fem_id;
    int num_fems_with_short_action;
    int num_bits_in_fem_field_select;
    int nof_fem_id_per_array;
    int nof_fem_array;
    int pbus_header_length;
    int nof_layer_records;
    int layer_record_size;
    int nof_compare_pairs_in_compare_mode;
    int compare_key_size;
    int nof_l4_ops_ranges_legacy;
    int nof_ext_l4_ops_ranges;
    int nof_pkt_hdr_ranges;
    int nof_out_lif_ranges;
    int fes_key_select_for_zero_bit;
    int fes_shift_for_zero_bit;
    int cmp_selection;
    int nof_bits_main_pbus;
    int nof_bits_native_pbus;
    int dir_ext_single_key_size;
    int dir_ext_first_key;
    int exem_max_result_size;
    int exem_pdb;
} dnx_data_field_stage_stage_info_t;



typedef enum
{

    
    _dnx_data_field_stage_feature_nof
} dnx_data_field_stage_feature_e;



typedef int(
    *dnx_data_field_stage_feature_get_f) (
    int unit,
    dnx_data_field_stage_feature_e feature);


typedef const dnx_data_field_stage_stage_info_t *(
    *dnx_data_field_stage_stage_info_get_f) (
    int unit,
    int stage);



typedef struct
{
    
    dnx_data_field_stage_feature_get_f feature_get;
    
    dnx_data_field_stage_stage_info_get_f stage_info_get;
    
    dnxc_data_table_info_get_f stage_info_info_get;
} dnx_data_if_field_stage_t;






typedef struct
{
    uint32 quad_id;
    uint32 group_id;
} dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t;


typedef struct
{
    int payload_large_size;
    int payload_small_size;
    int payload_first_bit;
} dnx_data_field_external_tcam_epmf_payload_sizes_t;



typedef enum
{
    dnx_data_field_external_tcam_has_acl_context_profile_mapping,

    
    _dnx_data_field_external_tcam_feature_nof
} dnx_data_field_external_tcam_feature_e;



typedef int(
    *dnx_data_field_external_tcam_feature_get_f) (
    int unit,
    dnx_data_field_external_tcam_feature_e feature);


typedef uint32(
    *dnx_data_field_external_tcam_max_single_key_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_external_tcam_nof_acl_keys_master_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_external_tcam_nof_acl_keys_fg_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_external_tcam_nof_keys_total_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_external_tcam_min_acl_nof_ffc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_external_tcam_max_fwd_context_num_for_one_apptype_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_external_tcam_max_acl_context_num_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_external_tcam_size_apptype_profile_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_external_tcam_key_bmp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_external_tcam_apptype_user_1st_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_external_tcam_apptype_user_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_external_tcam_max_payload_size_per_opcode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_external_tcam_max_payload_size_per_opcode_epmf_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_external_tcam_max_epmf_nof_lookups_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_external_tcam_nof_total_lookups_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_external_tcam_multicore_fg_supports_core_all_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_external_tcam_type_get_f) (
    int unit);


typedef const dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *(
    *dnx_data_field_external_tcam_ffc_to_quad_and_group_map_get_f) (
    int unit,
    int ffc_id);


typedef const dnx_data_field_external_tcam_epmf_payload_sizes_t *(
    *dnx_data_field_external_tcam_epmf_payload_sizes_get_f) (
    int unit,
    int result_id);



typedef struct
{
    
    dnx_data_field_external_tcam_feature_get_f feature_get;
    
    dnx_data_field_external_tcam_max_single_key_size_get_f max_single_key_size_get;
    
    dnx_data_field_external_tcam_nof_acl_keys_master_max_get_f nof_acl_keys_master_max_get;
    
    dnx_data_field_external_tcam_nof_acl_keys_fg_max_get_f nof_acl_keys_fg_max_get;
    
    dnx_data_field_external_tcam_nof_keys_total_get_f nof_keys_total_get;
    
    dnx_data_field_external_tcam_min_acl_nof_ffc_get_f min_acl_nof_ffc_get;
    
    dnx_data_field_external_tcam_max_fwd_context_num_for_one_apptype_get_f max_fwd_context_num_for_one_apptype_get;
    
    dnx_data_field_external_tcam_max_acl_context_num_get_f max_acl_context_num_get;
    
    dnx_data_field_external_tcam_size_apptype_profile_id_get_f size_apptype_profile_id_get;
    
    dnx_data_field_external_tcam_key_bmp_get_f key_bmp_get;
    
    dnx_data_field_external_tcam_apptype_user_1st_get_f apptype_user_1st_get;
    
    dnx_data_field_external_tcam_apptype_user_nof_get_f apptype_user_nof_get;
    
    dnx_data_field_external_tcam_max_payload_size_per_opcode_get_f max_payload_size_per_opcode_get;
    
    dnx_data_field_external_tcam_max_payload_size_per_opcode_epmf_get_f max_payload_size_per_opcode_epmf_get;
    
    dnx_data_field_external_tcam_max_epmf_nof_lookups_get_f max_epmf_nof_lookups_get;
    
    dnx_data_field_external_tcam_nof_total_lookups_get_f nof_total_lookups_get;
    
    dnx_data_field_external_tcam_multicore_fg_supports_core_all_get_f multicore_fg_supports_core_all_get;
    
    dnx_data_field_external_tcam_type_get_f type_get;
    
    dnx_data_field_external_tcam_ffc_to_quad_and_group_map_get_f ffc_to_quad_and_group_map_get;
    
    dnxc_data_table_info_get_f ffc_to_quad_and_group_map_info_get;
    
    dnx_data_field_external_tcam_epmf_payload_sizes_get_f epmf_payload_sizes_get;
    
    dnxc_data_table_info_get_f epmf_payload_sizes_info_get;
} dnx_data_if_field_external_tcam_t;







typedef enum
{
    dnx_data_field_tcam_tcam_80b_traffic_safe_write,
    dnx_data_field_tcam_mixed_ratio_supported,
    dnx_data_field_tcam_hitbit_splits_banks_like_sram,

    
    _dnx_data_field_tcam_feature_nof
} dnx_data_field_tcam_feature_e;



typedef int(
    *dnx_data_field_tcam_feature_get_f) (
    int unit,
    dnx_data_field_tcam_feature_e feature);


typedef uint32(
    *dnx_data_field_tcam_access_hw_mix_ratio_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_direct_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_key_size_half_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_key_size_single_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_key_size_double_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_dt_max_key_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_dt_max_key_size_small_bank_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_action_size_half_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_action_size_single_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_action_size_double_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_key_mode_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_entry_size_single_key_hw_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_entry_size_single_key_shadow_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_entry_size_single_valid_bits_hw_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_hw_bank_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_nof_big_bank_lines_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_nof_small_bank_lines_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_nof_big_banks_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_nof_small_banks_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_nof_banks_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_nof_payload_tables_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_nof_access_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_action_width_selector_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_nof_entries_80_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_nof_entries_160_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_tcam_banks_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_tcam_banks_last_index_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_nof_tcam_handlers_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_max_prefix_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_max_prefix_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_nof_keys_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_access_profile_half_key_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_access_profile_single_key_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_access_profile_double_key_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_tcam_entries_per_hit_indication_entry_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_max_tcam_priority_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_nof_banks_hitbit_units_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_nof_big_bank_lines_per_hitbit_unit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_nof_small_bank_lines_per_hitbit_unit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_app_db_id_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tcam_tcam_indirect_command_version_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_tcam_feature_get_f feature_get;
    
    dnx_data_field_tcam_access_hw_mix_ratio_supported_get_f access_hw_mix_ratio_supported_get;
    
    dnx_data_field_tcam_direct_supported_get_f direct_supported_get;
    
    dnx_data_field_tcam_key_size_half_get_f key_size_half_get;
    
    dnx_data_field_tcam_key_size_single_get_f key_size_single_get;
    
    dnx_data_field_tcam_key_size_double_get_f key_size_double_get;
    
    dnx_data_field_tcam_dt_max_key_size_get_f dt_max_key_size_get;
    
    dnx_data_field_tcam_dt_max_key_size_small_bank_get_f dt_max_key_size_small_bank_get;
    
    dnx_data_field_tcam_action_size_half_get_f action_size_half_get;
    
    dnx_data_field_tcam_action_size_single_get_f action_size_single_get;
    
    dnx_data_field_tcam_action_size_double_get_f action_size_double_get;
    
    dnx_data_field_tcam_key_mode_size_get_f key_mode_size_get;
    
    dnx_data_field_tcam_entry_size_single_key_hw_get_f entry_size_single_key_hw_get;
    
    dnx_data_field_tcam_entry_size_single_key_shadow_get_f entry_size_single_key_shadow_get;
    
    dnx_data_field_tcam_entry_size_single_valid_bits_hw_get_f entry_size_single_valid_bits_hw_get;
    
    dnx_data_field_tcam_hw_bank_size_get_f hw_bank_size_get;
    
    dnx_data_field_tcam_nof_big_bank_lines_get_f nof_big_bank_lines_get;
    
    dnx_data_field_tcam_nof_small_bank_lines_get_f nof_small_bank_lines_get;
    
    dnx_data_field_tcam_nof_big_banks_get_f nof_big_banks_get;
    
    dnx_data_field_tcam_nof_small_banks_get_f nof_small_banks_get;
    
    dnx_data_field_tcam_nof_banks_get_f nof_banks_get;
    
    dnx_data_field_tcam_nof_payload_tables_get_f nof_payload_tables_get;
    
    dnx_data_field_tcam_nof_access_profiles_get_f nof_access_profiles_get;
    
    dnx_data_field_tcam_action_width_selector_size_get_f action_width_selector_size_get;
    
    dnx_data_field_tcam_nof_entries_80_bits_get_f nof_entries_80_bits_get;
    
    dnx_data_field_tcam_nof_entries_160_bits_get_f nof_entries_160_bits_get;
    
    dnx_data_field_tcam_tcam_banks_size_get_f tcam_banks_size_get;
    
    dnx_data_field_tcam_tcam_banks_last_index_get_f tcam_banks_last_index_get;
    
    dnx_data_field_tcam_nof_tcam_handlers_get_f nof_tcam_handlers_get;
    
    dnx_data_field_tcam_max_prefix_size_get_f max_prefix_size_get;
    
    dnx_data_field_tcam_max_prefix_value_get_f max_prefix_value_get;
    
    dnx_data_field_tcam_nof_keys_max_get_f nof_keys_max_get;
    
    dnx_data_field_tcam_access_profile_half_key_mode_get_f access_profile_half_key_mode_get;
    
    dnx_data_field_tcam_access_profile_single_key_mode_get_f access_profile_single_key_mode_get;
    
    dnx_data_field_tcam_access_profile_double_key_mode_get_f access_profile_double_key_mode_get;
    
    dnx_data_field_tcam_tcam_entries_per_hit_indication_entry_get_f tcam_entries_per_hit_indication_entry_get;
    
    dnx_data_field_tcam_max_tcam_priority_get_f max_tcam_priority_get;
    
    dnx_data_field_tcam_nof_banks_hitbit_units_get_f nof_banks_hitbit_units_get;
    
    dnx_data_field_tcam_nof_big_bank_lines_per_hitbit_unit_get_f nof_big_bank_lines_per_hitbit_unit_get;
    
    dnx_data_field_tcam_nof_small_bank_lines_per_hitbit_unit_get_f nof_small_bank_lines_per_hitbit_unit_get;
    
    dnx_data_field_tcam_app_db_id_size_get_f app_db_id_size_get;
    
    dnx_data_field_tcam_tcam_indirect_command_version_get_f tcam_indirect_command_version_get;
} dnx_data_if_field_tcam_t;







typedef enum
{

    
    _dnx_data_field_group_feature_nof
} dnx_data_field_group_feature_e;



typedef int(
    *dnx_data_field_group_feature_get_f) (
    int unit,
    dnx_data_field_group_feature_e feature);


typedef uint32(
    *dnx_data_field_group_nof_fgs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_group_nof_action_per_fg_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_group_nof_quals_per_fg_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_group_nof_keys_per_fg_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_group_id_fec_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_group_payload_max_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_group_feature_get_f feature_get;
    
    dnx_data_field_group_nof_fgs_get_f nof_fgs_get;
    
    dnx_data_field_group_nof_action_per_fg_get_f nof_action_per_fg_get;
    
    dnx_data_field_group_nof_quals_per_fg_get_f nof_quals_per_fg_get;
    
    dnx_data_field_group_nof_keys_per_fg_max_get_f nof_keys_per_fg_max_get;
    
    dnx_data_field_group_id_fec_get_f id_fec_get;
    
    dnx_data_field_group_payload_max_size_get_f payload_max_size_get;
} dnx_data_if_field_group_t;






typedef struct
{
    uint8 key_select[DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO];
    uint8 num_bits[DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO];
    uint16 lsb[DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO];
    uint8 num_bits_not_on_key[DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO];
} dnx_data_field_efes_key_select_properties_t;



typedef enum
{

    
    _dnx_data_field_efes_feature_nof
} dnx_data_field_efes_feature_e;



typedef int(
    *dnx_data_field_efes_feature_get_f) (
    int unit,
    dnx_data_field_efes_feature_e feature);


typedef uint32(
    *dnx_data_field_efes_max_nof_key_selects_per_field_io_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_efes_max_nof_field_ios_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_efes_epmf_exem_zero_padding_added_to_field_io_get_f) (
    int unit);


typedef const dnx_data_field_efes_key_select_properties_t *(
    *dnx_data_field_efes_key_select_properties_get_f) (
    int unit,
    int stage,
    int field_io);



typedef struct
{
    
    dnx_data_field_efes_feature_get_f feature_get;
    
    dnx_data_field_efes_max_nof_key_selects_per_field_io_get_f max_nof_key_selects_per_field_io_get;
    
    dnx_data_field_efes_max_nof_field_ios_get_f max_nof_field_ios_get;
    
    dnx_data_field_efes_epmf_exem_zero_padding_added_to_field_io_get_f epmf_exem_zero_padding_added_to_field_io_get;
    
    dnx_data_field_efes_key_select_properties_get_f key_select_properties_get;
    
    dnxc_data_table_info_get_f key_select_properties_info_get;
} dnx_data_if_field_efes_t;






typedef struct
{
    uint8 key_select[DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO];
    uint8 num_bits[DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO];
    uint16 lsb[DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO];
    uint8 num_bits_not_on_key[DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO];
} dnx_data_field_fem_key_select_properties_t;



typedef enum
{

    
    _dnx_data_field_fem_feature_nof
} dnx_data_field_fem_feature_e;



typedef int(
    *dnx_data_field_fem_feature_get_f) (
    int unit,
    dnx_data_field_fem_feature_e feature);


typedef uint32(
    *dnx_data_field_fem_max_nof_key_selects_per_field_io_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_fem_nof_condition_bits_get_f) (
    int unit);


typedef const dnx_data_field_fem_key_select_properties_t *(
    *dnx_data_field_fem_key_select_properties_get_f) (
    int unit,
    int field_io);



typedef struct
{
    
    dnx_data_field_fem_feature_get_f feature_get;
    
    dnx_data_field_fem_max_nof_key_selects_per_field_io_get_f max_nof_key_selects_per_field_io_get;
    
    dnx_data_field_fem_nof_condition_bits_get_f nof_condition_bits_get;
    
    dnx_data_field_fem_key_select_properties_get_f key_select_properties_get;
    
    dnxc_data_table_info_get_f key_select_properties_info_get;
} dnx_data_if_field_fem_t;







typedef enum
{

    
    _dnx_data_field_context_feature_nof
} dnx_data_field_context_feature_e;



typedef int(
    *dnx_data_field_context_feature_get_f) (
    int unit,
    dnx_data_field_context_feature_e feature);


typedef uint32(
    *dnx_data_field_context_default_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_ftmh_mc_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_itmh_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_itmh_pph_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_itmh_pph_fhei_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_itmh_1588_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_itmh_1588_pph_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_itmh_1588_pph_fhei_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_itmh_pph_routing_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_j1_itmh_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_j1_itmh_pph_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_oam_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_oam_reflector_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_oam_upmep_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_j1_learning_2ndpass_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_rch_remove_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_rch_extended_encap_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_nat_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_nof_hash_keys_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_itmh_pph_unsupported_ext_ipmf2_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_itmh_pph_fhei_unsupported_ext_ipmf2_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_itmh_pph_fhei_vlan_ipmf2_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_itmh_1588_pph_fhei_unsupported_ext_ipmf2_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_itmh_1588_pph_unsupported_ext_ipmf2_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_itmh_1588_pph_fhei_vlan_ipmf2_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_context_default_ftmh_mc_ipmf3_context_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_context_feature_get_f feature_get;
    
    dnx_data_field_context_default_context_get_f default_context_get;
    
    dnx_data_field_context_default_ftmh_mc_context_get_f default_ftmh_mc_context_get;
    
    dnx_data_field_context_default_itmh_context_get_f default_itmh_context_get;
    
    dnx_data_field_context_default_itmh_pph_context_get_f default_itmh_pph_context_get;
    
    dnx_data_field_context_default_itmh_pph_fhei_context_get_f default_itmh_pph_fhei_context_get;
    
    dnx_data_field_context_default_itmh_1588_context_get_f default_itmh_1588_context_get;
    
    dnx_data_field_context_default_itmh_1588_pph_context_get_f default_itmh_1588_pph_context_get;
    
    dnx_data_field_context_default_itmh_1588_pph_fhei_context_get_f default_itmh_1588_pph_fhei_context_get;
    
    dnx_data_field_context_default_itmh_pph_routing_context_get_f default_itmh_pph_routing_context_get;
    
    dnx_data_field_context_default_j1_itmh_context_get_f default_j1_itmh_context_get;
    
    dnx_data_field_context_default_j1_itmh_pph_context_get_f default_j1_itmh_pph_context_get;
    
    dnx_data_field_context_default_oam_context_get_f default_oam_context_get;
    
    dnx_data_field_context_default_oam_reflector_context_get_f default_oam_reflector_context_get;
    
    dnx_data_field_context_default_oam_upmep_context_get_f default_oam_upmep_context_get;
    
    dnx_data_field_context_default_j1_learning_2ndpass_context_get_f default_j1_learning_2ndpass_context_get;
    
    dnx_data_field_context_default_rch_remove_context_get_f default_rch_remove_context_get;
    
    dnx_data_field_context_default_rch_extended_encap_context_get_f default_rch_extended_encap_context_get;
    
    dnx_data_field_context_default_nat_context_get_f default_nat_context_get;
    
    dnx_data_field_context_nof_hash_keys_get_f nof_hash_keys_get;
    
    dnx_data_field_context_default_itmh_pph_unsupported_ext_ipmf2_context_get_f default_itmh_pph_unsupported_ext_ipmf2_context_get;
    
    dnx_data_field_context_default_itmh_pph_fhei_unsupported_ext_ipmf2_context_get_f default_itmh_pph_fhei_unsupported_ext_ipmf2_context_get;
    
    dnx_data_field_context_default_itmh_pph_fhei_vlan_ipmf2_context_get_f default_itmh_pph_fhei_vlan_ipmf2_context_get;
    
    dnx_data_field_context_default_itmh_1588_pph_fhei_unsupported_ext_ipmf2_context_get_f default_itmh_1588_pph_fhei_unsupported_ext_ipmf2_context_get;
    
    dnx_data_field_context_default_itmh_1588_pph_unsupported_ext_ipmf2_context_get_f default_itmh_1588_pph_unsupported_ext_ipmf2_context_get;
    
    dnx_data_field_context_default_itmh_1588_pph_fhei_vlan_ipmf2_context_get_f default_itmh_1588_pph_fhei_vlan_ipmf2_context_get;
    
    dnx_data_field_context_default_ftmh_mc_ipmf3_context_get_f default_ftmh_mc_ipmf3_context_get;
} dnx_data_if_field_context_t;







typedef enum
{

    
    _dnx_data_field_preselector_feature_nof
} dnx_data_field_preselector_feature_e;



typedef int(
    *dnx_data_field_preselector_feature_get_f) (
    int unit,
    dnx_data_field_preselector_feature_e feature);


typedef uint32(
    *dnx_data_field_preselector_default_j1_itmh_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_j1_itmh_pph_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_j1_itmh_pph_oamp_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_j1_learn_presel_id_1st_pass_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_j1_learn_presel_id_2nd_pass_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_nat_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_ftmh_mc_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_pph_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_pph_fhei_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_1588_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_1588_pph_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_1588_pph_fhei_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_pph_eth_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_pph_ipv4_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_pph_ipv6_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_pph_mpls_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_y1711_rcpu_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_endpoint_psp_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_endpoint_psp_extended_cut_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_endpoint_psp_extended_cut_presel_id_ipmf2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_trap_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_icmp_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_egress_usd_eth_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_egress_usd_ipv4_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_egress_usd_ipv6_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_egress_usd_mpls_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_endpoint_usid_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_endpoint_usid_no_default_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_endpoint_gsid_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_egress_usp_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_egress_usp_trap_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_egress_usp_fwd_trap_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_egress_usp_icmp_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_mpls_bier_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_rcy_mirror_sys_hdr_presel_id_ipmf1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_pph_unsupported_ext_presel_id_ipmf2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_pph_fhei_unsupported_ext_presel_id_ipmf2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_pph_fhei_trap_presel_id_ipmf2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_pph_fhei_vlan_presel_id_ipmf2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_1588_pph_unsupported_ext_presel_id_ipmf2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_ipmf2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_1588_pph_fhei_trap_presel_id_ipmf2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_1588_pph_fhei_vlan_presel_id_ipmf2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_pph_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_pph_trap_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_pph_vlan_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_1588_pph_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_1588_pph_trap_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_1588_pph_vlan_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_itmh_pph_routing_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_j1_itmh_pph_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_ftmh_mc_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_oam_roo_ipv4_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_oam_roo_ipv6_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_oam_w_o_roo_no_rif_ipv4_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_oam_w_o_roo_no_rif_ipv6_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_oam_w_o_roo_with_rif_ipv4_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_oam_w_o_roo_with_rif_ipv6_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_j1_ipv4_mc_in_lif_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_j1_ipv6_mc_in_lif_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_rch_remove_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_rch_extended_encap_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_j1_php_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_j1_swap_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_partial_fwd_layer_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_endpoint_usid_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_endpoint_usid_no_default_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_endpoint_gsid_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_endpoint_psp_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_psp_ext_to_endpoint_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_egress_usp_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_endpoint_psp_extended_cut_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_egress_usd_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_mpls_bier_presel_id_ipmf3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_ebtr_const_fwd_layer_eth_epmf_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_ext_termination_btr_epmf_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_j1_same_port_presel_id_epmf_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_learn_limit_presel_id_epmf_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_endpoint_hbh_bta_presel_id_epmf_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_transit_hbh_bta_presel_id_epmf_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_srv6_endpoint_psp_bta_presel_id_epmf_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_mpls_8b_fhei_presel_id_epmf_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_non_pph_presel_id_epmf_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_default_nat_presel_id_epmf_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_num_cs_inlif_profile_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_preselector_nof_uint32_in_cs_qual_hw_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_preselector_feature_get_f feature_get;
    
    dnx_data_field_preselector_default_j1_itmh_presel_id_ipmf1_get_f default_j1_itmh_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_default_j1_itmh_pph_presel_id_ipmf1_get_f default_j1_itmh_pph_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_default_j1_itmh_pph_oamp_presel_id_ipmf1_get_f default_j1_itmh_pph_oamp_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_default_j1_learn_presel_id_1st_pass_ipmf1_get_f default_j1_learn_presel_id_1st_pass_ipmf1_get;
    
    dnx_data_field_preselector_default_j1_learn_presel_id_2nd_pass_ipmf1_get_f default_j1_learn_presel_id_2nd_pass_ipmf1_get;
    
    dnx_data_field_preselector_default_nat_presel_id_ipmf1_get_f default_nat_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_default_ftmh_mc_presel_id_ipmf1_get_f default_ftmh_mc_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_default_itmh_presel_id_ipmf1_get_f default_itmh_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_default_itmh_pph_presel_id_ipmf1_get_f default_itmh_pph_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_default_itmh_pph_fhei_presel_id_ipmf1_get_f default_itmh_pph_fhei_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_default_itmh_1588_presel_id_ipmf1_get_f default_itmh_1588_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_default_itmh_1588_pph_presel_id_ipmf1_get_f default_itmh_1588_pph_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_default_itmh_1588_pph_fhei_presel_id_ipmf1_get_f default_itmh_1588_pph_fhei_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_default_itmh_pph_eth_presel_id_ipmf1_get_f default_itmh_pph_eth_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_default_itmh_pph_ipv4_presel_id_ipmf1_get_f default_itmh_pph_ipv4_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_default_itmh_pph_ipv6_presel_id_ipmf1_get_f default_itmh_pph_ipv6_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_default_itmh_pph_mpls_presel_id_ipmf1_get_f default_itmh_pph_mpls_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_default_itmh_y1711_rcpu_presel_id_ipmf1_get_f default_itmh_y1711_rcpu_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_srv6_endpoint_psp_presel_id_ipmf1_get_f srv6_endpoint_psp_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_srv6_endpoint_psp_extended_cut_presel_id_ipmf1_get_f srv6_endpoint_psp_extended_cut_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_srv6_endpoint_psp_extended_cut_presel_id_ipmf2_get_f srv6_endpoint_psp_extended_cut_presel_id_ipmf2_get;
    
    dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_presel_id_ipmf1_get_f srv6_endpoint_usp_extended_cut_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_trap_presel_id_ipmf1_get_f srv6_endpoint_usp_extended_cut_trap_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_icmp_presel_id_ipmf1_get_f srv6_endpoint_usp_extended_cut_icmp_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_srv6_egress_usd_eth_presel_id_ipmf1_get_f srv6_egress_usd_eth_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_srv6_egress_usd_ipv4_presel_id_ipmf1_get_f srv6_egress_usd_ipv4_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_srv6_egress_usd_ipv6_presel_id_ipmf1_get_f srv6_egress_usd_ipv6_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_srv6_egress_usd_mpls_presel_id_ipmf1_get_f srv6_egress_usd_mpls_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_srv6_endpoint_usid_presel_id_ipmf1_get_f srv6_endpoint_usid_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_srv6_endpoint_usid_no_default_presel_id_ipmf1_get_f srv6_endpoint_usid_no_default_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_srv6_endpoint_gsid_presel_id_ipmf1_get_f srv6_endpoint_gsid_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_srv6_egress_usp_presel_id_ipmf1_get_f srv6_egress_usp_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_srv6_egress_usp_trap_presel_id_ipmf1_get_f srv6_egress_usp_trap_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_srv6_egress_usp_fwd_trap_presel_id_ipmf1_get_f srv6_egress_usp_fwd_trap_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_srv6_egress_usp_icmp_presel_id_ipmf1_get_f srv6_egress_usp_icmp_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_mpls_bier_presel_id_ipmf1_get_f mpls_bier_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_default_rcy_mirror_sys_hdr_presel_id_ipmf1_get_f default_rcy_mirror_sys_hdr_presel_id_ipmf1_get;
    
    dnx_data_field_preselector_default_itmh_pph_unsupported_ext_presel_id_ipmf2_get_f default_itmh_pph_unsupported_ext_presel_id_ipmf2_get;
    
    dnx_data_field_preselector_default_itmh_pph_fhei_unsupported_ext_presel_id_ipmf2_get_f default_itmh_pph_fhei_unsupported_ext_presel_id_ipmf2_get;
    
    dnx_data_field_preselector_default_itmh_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2_get_f default_itmh_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2_get;
    
    dnx_data_field_preselector_default_itmh_pph_fhei_trap_presel_id_ipmf2_get_f default_itmh_pph_fhei_trap_presel_id_ipmf2_get;
    
    dnx_data_field_preselector_default_itmh_pph_fhei_vlan_presel_id_ipmf2_get_f default_itmh_pph_fhei_vlan_presel_id_ipmf2_get;
    
    dnx_data_field_preselector_default_itmh_1588_pph_unsupported_ext_presel_id_ipmf2_get_f default_itmh_1588_pph_unsupported_ext_presel_id_ipmf2_get;
    
    dnx_data_field_preselector_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_ipmf2_get_f default_itmh_1588_pph_fhei_unsupported_ext_presel_id_ipmf2_get;
    
    dnx_data_field_preselector_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2_get_f default_itmh_1588_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2_get;
    
    dnx_data_field_preselector_default_itmh_1588_pph_fhei_trap_presel_id_ipmf2_get_f default_itmh_1588_pph_fhei_trap_presel_id_ipmf2_get;
    
    dnx_data_field_preselector_default_itmh_1588_pph_fhei_vlan_presel_id_ipmf2_get_f default_itmh_1588_pph_fhei_vlan_presel_id_ipmf2_get;
    
    dnx_data_field_preselector_default_itmh_pph_presel_id_ipmf3_get_f default_itmh_pph_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_default_itmh_pph_trap_presel_id_ipmf3_get_f default_itmh_pph_trap_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_default_itmh_pph_vlan_presel_id_ipmf3_get_f default_itmh_pph_vlan_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_default_itmh_1588_pph_presel_id_ipmf3_get_f default_itmh_1588_pph_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_default_itmh_1588_pph_trap_presel_id_ipmf3_get_f default_itmh_1588_pph_trap_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_default_itmh_1588_pph_vlan_presel_id_ipmf3_get_f default_itmh_1588_pph_vlan_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_default_itmh_pph_routing_presel_id_ipmf3_get_f default_itmh_pph_routing_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_default_j1_itmh_pph_presel_id_ipmf3_get_f default_j1_itmh_pph_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_default_ftmh_mc_presel_id_ipmf3_get_f default_ftmh_mc_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_default_oam_roo_ipv4_presel_id_ipmf3_get_f default_oam_roo_ipv4_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_default_oam_roo_ipv6_presel_id_ipmf3_get_f default_oam_roo_ipv6_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_default_oam_w_o_roo_no_rif_ipv4_presel_id_ipmf3_get_f default_oam_w_o_roo_no_rif_ipv4_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_default_oam_w_o_roo_no_rif_ipv6_presel_id_ipmf3_get_f default_oam_w_o_roo_no_rif_ipv6_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_default_oam_w_o_roo_with_rif_ipv4_presel_id_ipmf3_get_f default_oam_w_o_roo_with_rif_ipv4_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_default_oam_w_o_roo_with_rif_ipv6_presel_id_ipmf3_get_f default_oam_w_o_roo_with_rif_ipv6_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_default_j1_ipv4_mc_in_lif_presel_id_ipmf3_get_f default_j1_ipv4_mc_in_lif_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_default_j1_ipv6_mc_in_lif_presel_id_ipmf3_get_f default_j1_ipv6_mc_in_lif_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_default_rch_remove_presel_id_ipmf3_get_f default_rch_remove_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_default_rch_extended_encap_presel_id_ipmf3_get_f default_rch_extended_encap_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_default_j1_php_presel_id_ipmf3_get_f default_j1_php_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_default_j1_swap_presel_id_ipmf3_get_f default_j1_swap_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_srv6_partial_fwd_layer_presel_id_ipmf3_get_f srv6_partial_fwd_layer_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_srv6_endpoint_usid_presel_id_ipmf3_get_f srv6_endpoint_usid_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_srv6_endpoint_usid_no_default_presel_id_ipmf3_get_f srv6_endpoint_usid_no_default_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_srv6_endpoint_gsid_presel_id_ipmf3_get_f srv6_endpoint_gsid_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_srv6_endpoint_psp_presel_id_ipmf3_get_f srv6_endpoint_psp_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_srv6_psp_ext_to_endpoint_presel_id_ipmf3_get_f srv6_psp_ext_to_endpoint_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_srv6_egress_usp_presel_id_ipmf3_get_f srv6_egress_usp_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_srv6_endpoint_psp_extended_cut_presel_id_ipmf3_get_f srv6_endpoint_psp_extended_cut_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_presel_id_ipmf3_get_f srv6_endpoint_usp_extended_cut_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_srv6_egress_usd_presel_id_ipmf3_get_f srv6_egress_usd_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_mpls_bier_presel_id_ipmf3_get_f mpls_bier_presel_id_ipmf3_get;
    
    dnx_data_field_preselector_ebtr_const_fwd_layer_eth_epmf_get_f ebtr_const_fwd_layer_eth_epmf_get;
    
    dnx_data_field_preselector_srv6_ext_termination_btr_epmf_get_f srv6_ext_termination_btr_epmf_get;
    
    dnx_data_field_preselector_default_j1_same_port_presel_id_epmf_get_f default_j1_same_port_presel_id_epmf_get;
    
    dnx_data_field_preselector_default_learn_limit_presel_id_epmf_get_f default_learn_limit_presel_id_epmf_get;
    
    dnx_data_field_preselector_srv6_endpoint_hbh_bta_presel_id_epmf_get_f srv6_endpoint_hbh_bta_presel_id_epmf_get;
    
    dnx_data_field_preselector_srv6_transit_hbh_bta_presel_id_epmf_get_f srv6_transit_hbh_bta_presel_id_epmf_get;
    
    dnx_data_field_preselector_srv6_endpoint_psp_bta_presel_id_epmf_get_f srv6_endpoint_psp_bta_presel_id_epmf_get;
    
    dnx_data_field_preselector_default_mpls_8b_fhei_presel_id_epmf_get_f default_mpls_8b_fhei_presel_id_epmf_get;
    
    dnx_data_field_preselector_default_non_pph_presel_id_epmf_get_f default_non_pph_presel_id_epmf_get;
    
    dnx_data_field_preselector_default_nat_presel_id_epmf_get_f default_nat_presel_id_epmf_get;
    
    dnx_data_field_preselector_num_cs_inlif_profile_entries_get_f num_cs_inlif_profile_entries_get;
    
    dnx_data_field_preselector_nof_uint32_in_cs_qual_hw_get_f nof_uint32_in_cs_qual_hw_get;
} dnx_data_if_field_preselector_t;







typedef enum
{

    
    _dnx_data_field_qual_feature_nof
} dnx_data_field_qual_feature_e;



typedef int(
    *dnx_data_field_qual_feature_get_f) (
    int unit,
    dnx_data_field_qual_feature_e feature);


typedef uint32(
    *dnx_data_field_qual_user_1st_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_qual_user_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_qual_predefined_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_qual_layer_record_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_qual_vw_1st_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_qual_vw_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_qual_max_bits_in_qual_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_qual_ingress_pbus_header_length_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_qual_egress_pbus_header_length_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_qual_ifwd2_pbus_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_qual_ipmf1_pbus_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_qual_ipmf2_pbus_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_qual_ipmf3_pbus_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_qual_epmf_pbus_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_qual_l4_ops_pbus_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_qual_ingress_nof_layer_records_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_qual_ingress_layer_record_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_qual_egress_nof_layer_records_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_qual_egress_layer_record_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_qual_kbp_extra_offset_after_layer_record_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_qual_special_metadata_qual_max_parts_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_qual_feature_get_f feature_get;
    
    dnx_data_field_qual_user_1st_get_f user_1st_get;
    
    dnx_data_field_qual_user_nof_get_f user_nof_get;
    
    dnx_data_field_qual_predefined_nof_get_f predefined_nof_get;
    
    dnx_data_field_qual_layer_record_nof_get_f layer_record_nof_get;
    
    dnx_data_field_qual_vw_1st_get_f vw_1st_get;
    
    dnx_data_field_qual_vw_nof_get_f vw_nof_get;
    
    dnx_data_field_qual_max_bits_in_qual_get_f max_bits_in_qual_get;
    
    dnx_data_field_qual_ingress_pbus_header_length_get_f ingress_pbus_header_length_get;
    
    dnx_data_field_qual_egress_pbus_header_length_get_f egress_pbus_header_length_get;
    
    dnx_data_field_qual_ifwd2_pbus_size_get_f ifwd2_pbus_size_get;
    
    dnx_data_field_qual_ipmf1_pbus_size_get_f ipmf1_pbus_size_get;
    
    dnx_data_field_qual_ipmf2_pbus_size_get_f ipmf2_pbus_size_get;
    
    dnx_data_field_qual_ipmf3_pbus_size_get_f ipmf3_pbus_size_get;
    
    dnx_data_field_qual_epmf_pbus_size_get_f epmf_pbus_size_get;
    
    dnx_data_field_qual_l4_ops_pbus_size_get_f l4_ops_pbus_size_get;
    
    dnx_data_field_qual_ingress_nof_layer_records_get_f ingress_nof_layer_records_get;
    
    dnx_data_field_qual_ingress_layer_record_size_get_f ingress_layer_record_size_get;
    
    dnx_data_field_qual_egress_nof_layer_records_get_f egress_nof_layer_records_get;
    
    dnx_data_field_qual_egress_layer_record_size_get_f egress_layer_record_size_get;
    
    dnx_data_field_qual_kbp_extra_offset_after_layer_record_offset_get_f kbp_extra_offset_after_layer_record_offset_get;
    
    dnx_data_field_qual_special_metadata_qual_max_parts_get_f special_metadata_qual_max_parts_get;
} dnx_data_if_field_qual_t;






typedef struct
{
    int id;
    int size;
} dnx_data_field_action_params_t;



typedef enum
{

    
    _dnx_data_field_action_feature_nof
} dnx_data_field_action_feature_e;



typedef int(
    *dnx_data_field_action_feature_get_f) (
    int unit,
    dnx_data_field_action_feature_e feature);


typedef uint32(
    *dnx_data_field_action_user_1st_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_action_user_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_action_predefined_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_action_vw_1st_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_action_vw_nof_get_f) (
    int unit);


typedef const dnx_data_field_action_params_t *(
    *dnx_data_field_action_params_get_f) (
    int unit,
    int stage,
    int action);



typedef struct
{
    
    dnx_data_field_action_feature_get_f feature_get;
    
    dnx_data_field_action_user_1st_get_f user_1st_get;
    
    dnx_data_field_action_user_nof_get_f user_nof_get;
    
    dnx_data_field_action_predefined_nof_get_f predefined_nof_get;
    
    dnx_data_field_action_vw_1st_get_f vw_1st_get;
    
    dnx_data_field_action_vw_nof_get_f vw_nof_get;
    
    dnx_data_field_action_params_get_f params_get;
    
    dnxc_data_table_info_get_f params_info_get;
} dnx_data_if_field_action_t;






typedef struct
{
    char *signal_name;
    int mapped_qual;
    int offset_from_qual;
    int mapped_action[DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF];
    int action_offset[DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF];
} dnx_data_field_virtual_wire_signal_mapping_t;



typedef enum
{
    dnx_data_field_virtual_wire_in_lif_ac_wide_is_vw,

    
    _dnx_data_field_virtual_wire_feature_nof
} dnx_data_field_virtual_wire_feature_e;



typedef int(
    *dnx_data_field_virtual_wire_feature_get_f) (
    int unit,
    dnx_data_field_virtual_wire_feature_e feature);


typedef uint32(
    *dnx_data_field_virtual_wire_signals_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_virtual_wire_actions_per_signal_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_virtual_wire_general_data_user_general_containers_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_virtual_wire_general_data_nof_containers_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_virtual_wire_ipmf1_general_data_index_get_f) (
    int unit);


typedef const dnx_data_field_virtual_wire_signal_mapping_t *(
    *dnx_data_field_virtual_wire_signal_mapping_get_f) (
    int unit,
    int stage,
    int signal_id);



typedef struct
{
    
    dnx_data_field_virtual_wire_feature_get_f feature_get;
    
    dnx_data_field_virtual_wire_signals_nof_get_f signals_nof_get;
    
    dnx_data_field_virtual_wire_actions_per_signal_nof_get_f actions_per_signal_nof_get;
    
    dnx_data_field_virtual_wire_general_data_user_general_containers_size_get_f general_data_user_general_containers_size_get;
    
    dnx_data_field_virtual_wire_general_data_nof_containers_get_f general_data_nof_containers_get;
    
    dnx_data_field_virtual_wire_ipmf1_general_data_index_get_f ipmf1_general_data_index_get;
    
    dnx_data_field_virtual_wire_signal_mapping_get_f signal_mapping_get;
    
    dnxc_data_table_info_get_f signal_mapping_info_get;
} dnx_data_if_field_virtual_wire_t;







typedef enum
{

    
    _dnx_data_field_profile_bits_feature_nof
} dnx_data_field_profile_bits_feature_e;



typedef int(
    *dnx_data_field_profile_bits_feature_get_f) (
    int unit,
    dnx_data_field_profile_bits_feature_e feature);


typedef uint32(
    *dnx_data_field_profile_bits_ingress_pp_port_key_gen_var_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_profile_bits_max_port_profile_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_profile_bits_nof_bits_in_port_profile_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_profile_bits_nof_bits_in_ingress_pp_port_general_data_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_profile_bits_nof_ing_in_lif_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_profile_bits_nof_ing_eth_rif_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_profile_bits_feature_get_f feature_get;
    
    dnx_data_field_profile_bits_ingress_pp_port_key_gen_var_size_get_f ingress_pp_port_key_gen_var_size_get;
    
    dnx_data_field_profile_bits_max_port_profile_size_get_f max_port_profile_size_get;
    
    dnx_data_field_profile_bits_nof_bits_in_port_profile_get_f nof_bits_in_port_profile_get;
    
    dnx_data_field_profile_bits_nof_bits_in_ingress_pp_port_general_data_get_f nof_bits_in_ingress_pp_port_general_data_get;
    
    dnx_data_field_profile_bits_nof_ing_in_lif_get_f nof_ing_in_lif_get;
    
    dnx_data_field_profile_bits_nof_ing_eth_rif_get_f nof_ing_eth_rif_get;
} dnx_data_if_field_profile_bits_t;







typedef enum
{

    
    _dnx_data_field_dir_ext_feature_nof
} dnx_data_field_dir_ext_feature_e;



typedef int(
    *dnx_data_field_dir_ext_feature_get_f) (
    int unit,
    dnx_data_field_dir_ext_feature_e feature);


typedef uint32(
    *dnx_data_field_dir_ext_half_key_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_dir_ext_single_key_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_dir_ext_feature_get_f feature_get;
    
    dnx_data_field_dir_ext_half_key_size_get_f half_key_size_get;
    
    dnx_data_field_dir_ext_single_key_size_get_f single_key_size_get;
} dnx_data_if_field_dir_ext_t;







typedef enum
{

    
    _dnx_data_field_state_table_feature_nof
} dnx_data_field_state_table_feature_e;



typedef int(
    *dnx_data_field_state_table_feature_get_f) (
    int unit,
    dnx_data_field_state_table_feature_e feature);


typedef uint32(
    *dnx_data_field_state_table_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_state_table_address_size_entry_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_state_table_data_size_entry_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_state_table_address_max_entry_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_state_table_wr_bit_size_rw_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_state_table_opcode_size_rmw_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_state_table_ipmf2_key_select_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_state_table_data_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_state_table_state_table_stage_key_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_state_table_address_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_state_table_address_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_state_table_wr_bit_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_state_table_key_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_state_table_feature_get_f feature_get;
    
    dnx_data_field_state_table_supported_get_f supported_get;
    
    dnx_data_field_state_table_address_size_entry_max_get_f address_size_entry_max_get;
    
    dnx_data_field_state_table_data_size_entry_max_get_f data_size_entry_max_get;
    
    dnx_data_field_state_table_address_max_entry_max_get_f address_max_entry_max_get;
    
    dnx_data_field_state_table_wr_bit_size_rw_get_f wr_bit_size_rw_get;
    
    dnx_data_field_state_table_opcode_size_rmw_get_f opcode_size_rmw_get;
    
    dnx_data_field_state_table_ipmf2_key_select_supported_get_f ipmf2_key_select_supported_get;
    
    dnx_data_field_state_table_data_size_get_f data_size_get;
    
    dnx_data_field_state_table_state_table_stage_key_get_f state_table_stage_key_get;
    
    dnx_data_field_state_table_address_size_get_f address_size_get;
    
    dnx_data_field_state_table_address_max_get_f address_max_get;
    
    dnx_data_field_state_table_wr_bit_size_get_f wr_bit_size_get;
    
    dnx_data_field_state_table_key_size_get_f key_size_get;
} dnx_data_if_field_state_table_t;







typedef enum
{

    
    _dnx_data_field_sce_feature_nof
} dnx_data_field_sce_feature_e;



typedef int(
    *dnx_data_field_sce_feature_get_f) (
    int unit,
    dnx_data_field_sce_feature_e feature);



typedef struct
{
    
    dnx_data_field_sce_feature_get_f feature_get;
} dnx_data_if_field_sce_t;







typedef enum
{

    
    _dnx_data_field_mdb_dt_feature_nof
} dnx_data_field_mdb_dt_feature_e;



typedef int(
    *dnx_data_field_mdb_dt_feature_get_f) (
    int unit,
    dnx_data_field_mdb_dt_feature_e feature);


typedef uint32(
    *dnx_data_field_mdb_dt_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_mdb_dt_key_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_mdb_dt_action_size_large_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_mdb_dt_ipmf1_key_select_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_mdb_dt_ipmf1_key_select_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_mdb_dt_ipmf2_key_select_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_mdb_dt_ipmf2_key_select_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_mdb_dt_ipmf3_key_select_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_mdb_dt_ipmf3_key_select_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_mdb_dt_ipmf1_ipmf2_concurrent_support_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_mdb_dt_signal_block_name_is_map_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_mdb_dt_signal_key_is_entire_key_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_mdb_dt_pmf_map_stage_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_mdb_dt_feature_get_f feature_get;
    
    dnx_data_field_mdb_dt_supported_get_f supported_get;
    
    dnx_data_field_mdb_dt_key_size_get_f key_size_get;
    
    dnx_data_field_mdb_dt_action_size_large_get_f action_size_large_get;
    
    dnx_data_field_mdb_dt_ipmf1_key_select_supported_get_f ipmf1_key_select_supported_get;
    
    dnx_data_field_mdb_dt_ipmf1_key_select_get_f ipmf1_key_select_get;
    
    dnx_data_field_mdb_dt_ipmf2_key_select_supported_get_f ipmf2_key_select_supported_get;
    
    dnx_data_field_mdb_dt_ipmf2_key_select_get_f ipmf2_key_select_get;
    
    dnx_data_field_mdb_dt_ipmf3_key_select_supported_get_f ipmf3_key_select_supported_get;
    
    dnx_data_field_mdb_dt_ipmf3_key_select_get_f ipmf3_key_select_get;
    
    dnx_data_field_mdb_dt_ipmf1_ipmf2_concurrent_support_get_f ipmf1_ipmf2_concurrent_support_get;
    
    dnx_data_field_mdb_dt_signal_block_name_is_map_get_f signal_block_name_is_map_get;
    
    dnx_data_field_mdb_dt_signal_key_is_entire_key_get_f signal_key_is_entire_key_get;
    
    dnx_data_field_mdb_dt_pmf_map_stage_get_f pmf_map_stage_get;
} dnx_data_if_field_mdb_dt_t;







typedef enum
{

    
    _dnx_data_field_hash_feature_nof
} dnx_data_field_hash_feature_e;



typedef int(
    *dnx_data_field_hash_feature_get_f) (
    int unit,
    dnx_data_field_hash_feature_e feature);


typedef uint32(
    *dnx_data_field_hash_max_key_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_hash_feature_get_f feature_get;
    
    dnx_data_field_hash_max_key_size_get_f max_key_size_get;
} dnx_data_if_field_hash_t;







typedef enum
{

    
    _dnx_data_field_udh_feature_nof
} dnx_data_field_udh_feature_e;



typedef int(
    *dnx_data_field_udh_feature_get_f) (
    int unit,
    dnx_data_field_udh_feature_e feature);


typedef uint32(
    *dnx_data_field_udh_ext_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_udh_type_count_get_f) (
    int unit);



typedef uint32(
    *dnx_data_field_udh_type_0_length_get_f) (
    int unit);



typedef uint32(
    *dnx_data_field_udh_type_1_length_get_f) (
    int unit);



typedef uint32(
    *dnx_data_field_udh_type_2_length_get_f) (
    int unit);



typedef uint32(
    *dnx_data_field_udh_type_3_length_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_udh_field_class_id_size_0_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_udh_field_class_id_size_1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_udh_field_class_id_size_2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_udh_field_class_id_size_3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_udh_field_class_id_total_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_udh_feature_get_f feature_get;
    
    dnx_data_field_udh_ext_supported_get_f ext_supported_get;
    
    dnx_data_field_udh_type_count_get_f type_count_get;

    
    dnx_data_field_udh_type_0_length_get_f type_0_length_get;


    
    dnx_data_field_udh_type_1_length_get_f type_1_length_get;


    
    dnx_data_field_udh_type_2_length_get_f type_2_length_get;


    
    dnx_data_field_udh_type_3_length_get_f type_3_length_get;

    
    dnx_data_field_udh_field_class_id_size_0_get_f field_class_id_size_0_get;
    
    dnx_data_field_udh_field_class_id_size_1_get_f field_class_id_size_1_get;
    
    dnx_data_field_udh_field_class_id_size_2_get_f field_class_id_size_2_get;
    
    dnx_data_field_udh_field_class_id_size_3_get_f field_class_id_size_3_get;
    
    dnx_data_field_udh_field_class_id_total_size_get_f field_class_id_total_size_get;
} dnx_data_if_field_udh_t;






typedef struct
{
    char *name;
    uint8 build_ftmh;
    uint8 build_tsh;
    uint8 build_pph;
    uint8 build_udh;
    uint8 never_add_pph_learn_ext;
} dnx_data_field_system_headers_system_header_profiles_t;



typedef enum
{

    
    _dnx_data_field_system_headers_feature_nof
} dnx_data_field_system_headers_feature_e;



typedef int(
    *dnx_data_field_system_headers_feature_get_f) (
    int unit,
    dnx_data_field_system_headers_feature_e feature);


typedef uint32(
    *dnx_data_field_system_headers_nof_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_system_headers_pph_learn_ext_disable_get_f) (
    int unit);


typedef const dnx_data_field_system_headers_system_header_profiles_t *(
    *dnx_data_field_system_headers_system_header_profiles_get_f) (
    int unit,
    int system_header_profile);



typedef struct
{
    
    dnx_data_field_system_headers_feature_get_f feature_get;
    
    dnx_data_field_system_headers_nof_profiles_get_f nof_profiles_get;
    
    dnx_data_field_system_headers_pph_learn_ext_disable_get_f pph_learn_ext_disable_get;
    
    dnx_data_field_system_headers_system_header_profiles_get_f system_header_profiles_get;
    
    dnxc_data_table_info_get_f system_header_profiles_info_get;
} dnx_data_if_field_system_headers_t;







typedef enum
{
    dnx_data_field_exem_stage_selection_includes_ipmf1,
    dnx_data_field_exem_exem_vmv_padded_for_2msb,

    
    _dnx_data_field_exem_feature_nof
} dnx_data_field_exem_feature_e;



typedef int(
    *dnx_data_field_exem_feature_get_f) (
    int unit,
    dnx_data_field_exem_feature_e feature);


typedef uint32(
    *dnx_data_field_exem_small_app_db_id_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_large_app_db_id_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_small_max_key_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_large_max_key_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_max_result_size_all_stages_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_small_max_container_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_large_max_container_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_vmv_hw_max_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_small_min_app_db_id_range_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_large_min_app_db_id_range_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_small_max_app_db_id_range_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_large_max_app_db_id_range_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_small_ipmf2_key_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_small_ipmf2_key_part_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_small_ipmf2_key_hw_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_small_ipmf2_key_hw_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_ipmf1_key_configurable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_ipmf2_key_configurable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_ipmf1_key_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_debug_key_signal_includes_zero_padding_get_f) (
    int unit);



typedef uint32(
    *dnx_data_field_exem_pmf_sexem3_stage_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_exem_feature_get_f feature_get;
    
    dnx_data_field_exem_small_app_db_id_size_get_f small_app_db_id_size_get;
    
    dnx_data_field_exem_large_app_db_id_size_get_f large_app_db_id_size_get;
    
    dnx_data_field_exem_small_max_key_size_get_f small_max_key_size_get;
    
    dnx_data_field_exem_large_max_key_size_get_f large_max_key_size_get;
    
    dnx_data_field_exem_max_result_size_all_stages_get_f max_result_size_all_stages_get;
    
    dnx_data_field_exem_small_max_container_size_get_f small_max_container_size_get;
    
    dnx_data_field_exem_large_max_container_size_get_f large_max_container_size_get;
    
    dnx_data_field_exem_vmv_hw_max_size_get_f vmv_hw_max_size_get;
    
    dnx_data_field_exem_small_min_app_db_id_range_get_f small_min_app_db_id_range_get;
    
    dnx_data_field_exem_large_min_app_db_id_range_get_f large_min_app_db_id_range_get;
    
    dnx_data_field_exem_small_max_app_db_id_range_get_f small_max_app_db_id_range_get;
    
    dnx_data_field_exem_large_max_app_db_id_range_get_f large_max_app_db_id_range_get;
    
    dnx_data_field_exem_small_ipmf2_key_get_f small_ipmf2_key_get;
    
    dnx_data_field_exem_small_ipmf2_key_part_get_f small_ipmf2_key_part_get;
    
    dnx_data_field_exem_small_ipmf2_key_hw_value_get_f small_ipmf2_key_hw_value_get;
    
    dnx_data_field_exem_small_ipmf2_key_hw_bits_get_f small_ipmf2_key_hw_bits_get;
    
    dnx_data_field_exem_ipmf1_key_configurable_get_f ipmf1_key_configurable_get;
    
    dnx_data_field_exem_ipmf2_key_configurable_get_f ipmf2_key_configurable_get;
    
    dnx_data_field_exem_ipmf1_key_get_f ipmf1_key_get;
    
    dnx_data_field_exem_debug_key_signal_includes_zero_padding_get_f debug_key_signal_includes_zero_padding_get;

    
    dnx_data_field_exem_pmf_sexem3_stage_get_f pmf_sexem3_stage_get;

} dnx_data_if_field_exem_t;






typedef struct
{
    soc_mem_t table_name;
    soc_field_t valid;
    soc_field_t entry_data;
    soc_field_t entry_mask;
    soc_field_t src_data;
    soc_field_t src_mask;
    soc_field_t appdb_id_data;
    soc_field_t appdb_id_mask;
    soc_field_t accessed_data;
    soc_field_t accessed_mask;
} dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t;


typedef struct
{
    soc_mem_t table_name;
    soc_field_t valid;
    soc_field_t entry_data;
    soc_field_t entry_mask;
    soc_field_t src_data;
    soc_field_t src_mask;
    soc_field_t appdb_id_data;
    soc_field_t appdb_id_mask;
    soc_field_t accessed_data;
    soc_field_t accessed_mask;
} dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t;


typedef struct
{
    soc_mem_t table_name;
    soc_field_t command;
    soc_field_t payload;
    soc_field_t payload_mask;
} dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_t;


typedef struct
{
    soc_mem_t table_name;
    soc_field_t command;
    soc_field_t payload;
    soc_field_t payload_mask;
} dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_t;



typedef enum
{
    dnx_data_field_exem_learn_flush_machine_has_elephant_signals,

    
    _dnx_data_field_exem_learn_flush_machine_feature_nof
} dnx_data_field_exem_learn_flush_machine_feature_e;



typedef int(
    *dnx_data_field_exem_learn_flush_machine_feature_get_f) (
    int unit,
    dnx_data_field_exem_learn_flush_machine_feature_e feature);


typedef uint32(
    *dnx_data_field_exem_learn_flush_machine_flush_machine_nof_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_learn_flush_machine_entry_extra_bits_for_hash_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_learn_flush_machine_sexem_entry_max_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_learn_flush_machine_lexem_entry_max_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_learn_flush_machine_command_bit_transplant_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_learn_flush_machine_command_bit_delete_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_learn_flush_machine_command_bit_clear_hit_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_learn_flush_machine_nof_bits_source_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_learn_flush_machine_source_bit_for_scan_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_learn_flush_machine_source_bit_for_pipe_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_learn_flush_machine_source_bit_for_mrq_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_learn_flush_machine_nof_bits_accessed_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_exem_learn_flush_machine_nof_flush_profiles_get_f) (
    int unit);


typedef const dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t *(
    *dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_get_f) (
    int unit);


typedef const dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t *(
    *dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_get_f) (
    int unit);


typedef const dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_t *(
    *dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_get_f) (
    int unit);


typedef const dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_t *(
    *dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_exem_learn_flush_machine_feature_get_f feature_get;
    
    dnx_data_field_exem_learn_flush_machine_flush_machine_nof_entries_get_f flush_machine_nof_entries_get;
    
    dnx_data_field_exem_learn_flush_machine_entry_extra_bits_for_hash_get_f entry_extra_bits_for_hash_get;
    
    dnx_data_field_exem_learn_flush_machine_sexem_entry_max_size_get_f sexem_entry_max_size_get;
    
    dnx_data_field_exem_learn_flush_machine_lexem_entry_max_size_get_f lexem_entry_max_size_get;
    
    dnx_data_field_exem_learn_flush_machine_command_bit_transplant_get_f command_bit_transplant_get;
    
    dnx_data_field_exem_learn_flush_machine_command_bit_delete_get_f command_bit_delete_get;
    
    dnx_data_field_exem_learn_flush_machine_command_bit_clear_hit_bit_get_f command_bit_clear_hit_bit_get;
    
    dnx_data_field_exem_learn_flush_machine_nof_bits_source_get_f nof_bits_source_get;
    
    dnx_data_field_exem_learn_flush_machine_source_bit_for_scan_get_f source_bit_for_scan_get;
    
    dnx_data_field_exem_learn_flush_machine_source_bit_for_pipe_get_f source_bit_for_pipe_get;
    
    dnx_data_field_exem_learn_flush_machine_source_bit_for_mrq_get_f source_bit_for_mrq_get;
    
    dnx_data_field_exem_learn_flush_machine_nof_bits_accessed_get_f nof_bits_accessed_get;
    
    dnx_data_field_exem_learn_flush_machine_nof_flush_profiles_get_f nof_flush_profiles_get;
    
    dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_get_f sexem_flush_tcam_tcam_memories_get;
    
    dnxc_data_table_info_get_f sexem_flush_tcam_tcam_memories_info_get;
    
    dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_get_f lexem_flush_tcam_tcam_memories_get;
    
    dnxc_data_table_info_get_f lexem_flush_tcam_tcam_memories_info_get;
    
    dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_get_f sexem_flush_tcam_data_rules_get;
    
    dnxc_data_table_info_get_f sexem_flush_tcam_data_rules_info_get;
    
    dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_get_f lexem_flush_tcam_data_rules_get;
    
    dnxc_data_table_info_get_f lexem_flush_tcam_data_rules_info_get;
} dnx_data_if_field_exem_learn_flush_machine_t;







typedef enum
{

    
    _dnx_data_field_ace_feature_nof
} dnx_data_field_ace_feature_e;



typedef int(
    *dnx_data_field_ace_feature_get_f) (
    int unit,
    dnx_data_field_ace_feature_e feature);


typedef uint32(
    *dnx_data_field_ace_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_ace_id_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_key_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_app_db_id_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_payload_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_min_key_range_pmf_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_max_key_range_pmf_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_min_ace_id_dynamic_range_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_nof_ace_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_nof_action_per_ace_format_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_row_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_min_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_ace_id_pmf_alloc_first_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_ace_id_pmf_alloc_last_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_nof_masks_per_fes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_nof_fes_id_per_array_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_nof_fes_array_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_nof_fes_instruction_per_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_nof_fes_programs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_nof_prog_per_fes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_nof_bits_in_fes_action_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_fes_shift_for_zero_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_fes_instruction_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_fes_shift_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_fes_invalid_bits_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_fes_type_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_fes_ace_action_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_fes_chosen_mask_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_ace_hw_invalid_action_id_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_ace_feature_get_f feature_get;
    
    dnx_data_field_ace_supported_get_f supported_get;
    
    dnx_data_field_ace_ace_id_size_get_f ace_id_size_get;
    
    dnx_data_field_ace_key_size_get_f key_size_get;
    
    dnx_data_field_ace_app_db_id_size_get_f app_db_id_size_get;
    
    dnx_data_field_ace_payload_size_get_f payload_size_get;
    
    dnx_data_field_ace_min_key_range_pmf_get_f min_key_range_pmf_get;
    
    dnx_data_field_ace_max_key_range_pmf_get_f max_key_range_pmf_get;
    
    dnx_data_field_ace_min_ace_id_dynamic_range_get_f min_ace_id_dynamic_range_get;
    
    dnx_data_field_ace_nof_ace_id_get_f nof_ace_id_get;
    
    dnx_data_field_ace_nof_action_per_ace_format_get_f nof_action_per_ace_format_get;
    
    dnx_data_field_ace_row_size_get_f row_size_get;
    
    dnx_data_field_ace_min_entry_size_get_f min_entry_size_get;
    
    dnx_data_field_ace_ace_id_pmf_alloc_first_get_f ace_id_pmf_alloc_first_get;
    
    dnx_data_field_ace_ace_id_pmf_alloc_last_get_f ace_id_pmf_alloc_last_get;
    
    dnx_data_field_ace_nof_masks_per_fes_get_f nof_masks_per_fes_get;
    
    dnx_data_field_ace_nof_fes_id_per_array_get_f nof_fes_id_per_array_get;
    
    dnx_data_field_ace_nof_fes_array_get_f nof_fes_array_get;
    
    dnx_data_field_ace_nof_fes_instruction_per_context_get_f nof_fes_instruction_per_context_get;
    
    dnx_data_field_ace_nof_fes_programs_get_f nof_fes_programs_get;
    
    dnx_data_field_ace_nof_prog_per_fes_get_f nof_prog_per_fes_get;
    
    dnx_data_field_ace_nof_bits_in_fes_action_get_f nof_bits_in_fes_action_get;
    
    dnx_data_field_ace_fes_shift_for_zero_bit_get_f fes_shift_for_zero_bit_get;
    
    dnx_data_field_ace_fes_instruction_size_get_f fes_instruction_size_get;
    
    dnx_data_field_ace_fes_shift_offset_get_f fes_shift_offset_get;
    
    dnx_data_field_ace_fes_invalid_bits_offset_get_f fes_invalid_bits_offset_get;
    
    dnx_data_field_ace_fes_type_offset_get_f fes_type_offset_get;
    
    dnx_data_field_ace_fes_ace_action_offset_get_f fes_ace_action_offset_get;
    
    dnx_data_field_ace_fes_chosen_mask_offset_get_f fes_chosen_mask_offset_get;
    
    dnx_data_field_ace_hw_invalid_action_id_get_f hw_invalid_action_id_get;
} dnx_data_if_field_ace_t;







typedef enum
{

    
    _dnx_data_field_entry_feature_nof
} dnx_data_field_entry_feature_e;



typedef int(
    *dnx_data_field_entry_feature_get_f) (
    int unit,
    dnx_data_field_entry_feature_e feature);


typedef uint32(
    *dnx_data_field_entry_dir_ext_nof_fields_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_entry_nof_action_params_per_entry_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_entry_nof_qual_params_per_entry_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_entry_feature_get_f feature_get;
    
    dnx_data_field_entry_dir_ext_nof_fields_get_f dir_ext_nof_fields_get;
    
    dnx_data_field_entry_nof_action_params_per_entry_get_f nof_action_params_per_entry_get;
    
    dnx_data_field_entry_nof_qual_params_per_entry_get_f nof_qual_params_per_entry_get;
} dnx_data_if_field_entry_t;







typedef enum
{

    
    _dnx_data_field_L4_Ops_feature_nof
} dnx_data_field_L4_Ops_feature_e;



typedef int(
    *dnx_data_field_L4_Ops_feature_get_f) (
    int unit,
    dnx_data_field_L4_Ops_feature_e feature);


typedef uint32(
    *dnx_data_field_L4_Ops_udp_position_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_L4_Ops_tcp_position_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_L4_Ops_nof_range_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_L4_Ops_nof_ext_encoders_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_L4_Ops_nof_ext_types_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_L4_Ops_nof_configurable_ranges_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_L4_Ops_feature_get_f feature_get;
    
    dnx_data_field_L4_Ops_udp_position_get_f udp_position_get;
    
    dnx_data_field_L4_Ops_tcp_position_get_f tcp_position_get;
    
    dnx_data_field_L4_Ops_nof_range_entries_get_f nof_range_entries_get;
    
    dnx_data_field_L4_Ops_nof_ext_encoders_get_f nof_ext_encoders_get;
    
    dnx_data_field_L4_Ops_nof_ext_types_get_f nof_ext_types_get;
    
    dnx_data_field_L4_Ops_nof_configurable_ranges_get_f nof_configurable_ranges_get;
} dnx_data_if_field_L4_Ops_t;







typedef enum
{

    
    _dnx_data_field_encoded_qual_actions_offset_feature_nof
} dnx_data_field_encoded_qual_actions_offset_feature_e;



typedef int(
    *dnx_data_field_encoded_qual_actions_offset_feature_get_f) (
    int unit,
    dnx_data_field_encoded_qual_actions_offset_feature_e feature);


typedef uint32(
    *dnx_data_field_encoded_qual_actions_offset_trap_strength_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_encoded_qual_actions_offset_trap_qualifier_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_encoded_qual_actions_offset_sniff_qualifier_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_encoded_qual_actions_offset_mirror_qualifier_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_encoded_qual_actions_offset_forward_trap_qualifier_offset_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_encoded_qual_actions_offset_feature_get_f feature_get;
    
    dnx_data_field_encoded_qual_actions_offset_trap_strength_offset_get_f trap_strength_offset_get;
    
    dnx_data_field_encoded_qual_actions_offset_trap_qualifier_offset_get_f trap_qualifier_offset_get;
    
    dnx_data_field_encoded_qual_actions_offset_sniff_qualifier_offset_get_f sniff_qualifier_offset_get;
    
    dnx_data_field_encoded_qual_actions_offset_mirror_qualifier_offset_get_f mirror_qualifier_offset_get;
    
    dnx_data_field_encoded_qual_actions_offset_forward_trap_qualifier_offset_get_f forward_trap_qualifier_offset_get;
} dnx_data_if_field_encoded_qual_actions_offset_t;







typedef enum
{

    
    _dnx_data_field_compare_feature_nof
} dnx_data_field_compare_feature_e;



typedef int(
    *dnx_data_field_compare_feature_get_f) (
    int unit,
    dnx_data_field_compare_feature_e feature);


typedef uint32(
    *dnx_data_field_compare_equal_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_compare_not_equal_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_compare_smaller_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_compare_not_smaller_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_compare_bigger_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_compare_not_bigger_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_compare_nof_keys_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_compare_nof_compare_pairs_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_compare_feature_get_f feature_get;
    
    dnx_data_field_compare_equal_offset_get_f equal_offset_get;
    
    dnx_data_field_compare_not_equal_offset_get_f not_equal_offset_get;
    
    dnx_data_field_compare_smaller_offset_get_f smaller_offset_get;
    
    dnx_data_field_compare_not_smaller_offset_get_f not_smaller_offset_get;
    
    dnx_data_field_compare_bigger_offset_get_f bigger_offset_get;
    
    dnx_data_field_compare_not_bigger_offset_get_f not_bigger_offset_get;
    
    dnx_data_field_compare_nof_keys_get_f nof_keys_get;
    
    dnx_data_field_compare_nof_compare_pairs_get_f nof_compare_pairs_get;
} dnx_data_if_field_compare_t;







typedef enum
{
    dnx_data_field_diag_tcam_cs_hit_bit_support,

    
    _dnx_data_field_diag_feature_nof
} dnx_data_field_diag_feature_e;



typedef int(
    *dnx_data_field_diag_feature_get_f) (
    int unit,
    dnx_data_field_diag_feature_e feature);


typedef uint32(
    *dnx_data_field_diag_bytes_to_remove_mask_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_diag_layers_to_remove_size_in_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_diag_layers_to_remove_mask_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_diag_nof_signals_per_action_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_diag_nof_signals_per_qualifier_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_diag_key_signal_size_in_bytes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_diag_result_signal_size_in_words_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_diag_dt_result_signal_size_in_words_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_diag_hit_signal_size_in_words_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_diag_feature_get_f feature_get;
    
    dnx_data_field_diag_bytes_to_remove_mask_get_f bytes_to_remove_mask_get;
    
    dnx_data_field_diag_layers_to_remove_size_in_bit_get_f layers_to_remove_size_in_bit_get;
    
    dnx_data_field_diag_layers_to_remove_mask_get_f layers_to_remove_mask_get;
    
    dnx_data_field_diag_nof_signals_per_action_get_f nof_signals_per_action_get;
    
    dnx_data_field_diag_nof_signals_per_qualifier_get_f nof_signals_per_qualifier_get;
    
    dnx_data_field_diag_key_signal_size_in_bytes_get_f key_signal_size_in_bytes_get;
    
    dnx_data_field_diag_result_signal_size_in_words_get_f result_signal_size_in_words_get;
    
    dnx_data_field_diag_dt_result_signal_size_in_words_get_f dt_result_signal_size_in_words_get;
    
    dnx_data_field_diag_hit_signal_size_in_words_get_f hit_signal_size_in_words_get;
} dnx_data_if_field_diag_t;







typedef enum
{

    
    _dnx_data_field_general_data_qualifiers_feature_nof
} dnx_data_field_general_data_qualifiers_feature_e;



typedef int(
    *dnx_data_field_general_data_qualifiers_feature_get_f) (
    int unit,
    dnx_data_field_general_data_qualifiers_feature_e feature);


typedef uint32(
    *dnx_data_field_general_data_qualifiers_vw_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_general_data_qualifiers_ac_in_lif_wide_data_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_general_data_qualifiers_pwe_in_lif_wide_data_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_general_data_qualifiers_ip_tunnel_in_lif_wide_data_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_general_data_qualifiers_native_ac_in_lif_wide_data_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_general_data_qualifiers_ac_in_lif_wide_data_extended_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_general_data_qualifiers_mact_entry_grouping_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_general_data_qualifiers_vw_vip_valid_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_general_data_qualifiers_vw_vip_id_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_general_data_qualifiers_vw_member_reference_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_general_data_qualifiers_vw_pcc_hit_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_general_data_qualifiers_l3srcbind_hit_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_general_data_qualifiers_srv6_next_si_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_general_data_qualifiers_srv6_next_gsid_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_general_data_qualifiers_feature_get_f feature_get;
    
    dnx_data_field_general_data_qualifiers_vw_supported_get_f vw_supported_get;
    
    dnx_data_field_general_data_qualifiers_ac_in_lif_wide_data_nof_bits_get_f ac_in_lif_wide_data_nof_bits_get;
    
    dnx_data_field_general_data_qualifiers_pwe_in_lif_wide_data_nof_bits_get_f pwe_in_lif_wide_data_nof_bits_get;
    
    dnx_data_field_general_data_qualifiers_ip_tunnel_in_lif_wide_data_nof_bits_get_f ip_tunnel_in_lif_wide_data_nof_bits_get;
    
    dnx_data_field_general_data_qualifiers_native_ac_in_lif_wide_data_nof_bits_get_f native_ac_in_lif_wide_data_nof_bits_get;
    
    dnx_data_field_general_data_qualifiers_ac_in_lif_wide_data_extended_nof_bits_get_f ac_in_lif_wide_data_extended_nof_bits_get;
    
    dnx_data_field_general_data_qualifiers_mact_entry_grouping_nof_bits_get_f mact_entry_grouping_nof_bits_get;
    
    dnx_data_field_general_data_qualifiers_vw_vip_valid_nof_bits_get_f vw_vip_valid_nof_bits_get;
    
    dnx_data_field_general_data_qualifiers_vw_vip_id_nof_bits_get_f vw_vip_id_nof_bits_get;
    
    dnx_data_field_general_data_qualifiers_vw_member_reference_nof_bits_get_f vw_member_reference_nof_bits_get;
    
    dnx_data_field_general_data_qualifiers_vw_pcc_hit_nof_bits_get_f vw_pcc_hit_nof_bits_get;
    
    dnx_data_field_general_data_qualifiers_l3srcbind_hit_nof_bits_get_f l3srcbind_hit_nof_bits_get;
    
    dnx_data_field_general_data_qualifiers_srv6_next_si_get_f srv6_next_si_get;
    
    dnx_data_field_general_data_qualifiers_srv6_next_gsid_get_f srv6_next_gsid_get;
} dnx_data_if_field_general_data_qualifiers_t;






typedef struct
{
    int is_fes;
    int is_fem;
    int fes_array_index;
    int fem_array_index;
} dnx_data_field_common_max_val_array_id_type_t;



typedef enum
{

    
    _dnx_data_field_common_max_val_feature_nof
} dnx_data_field_common_max_val_feature_e;



typedef int(
    *dnx_data_field_common_max_val_feature_get_f) (
    int unit,
    dnx_data_field_common_max_val_feature_e feature);


typedef uint32(
    *dnx_data_field_common_max_val_nof_ffc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_masks_per_fes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_fes_id_per_array_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_fes_array_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_fes_instruction_per_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_fes_programs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_prog_per_fes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_cs_lines_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_contexts_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_actions_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_qualifiers_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_bits_in_fes_action_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_bits_in_fes_key_select_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_bits_in_ffc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_ffc_in_uint32_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_action_per_group_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_layer_records_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_layer_record_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_l4_ops_ranges_legacy_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_pkt_hdr_ranges_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_out_lif_ranges_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_kbr_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_compare_pairs_in_compare_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_bits_in_fem_action_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_fem_condition_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_fem_map_index_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_fem_programs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_fem_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_nof_array_ids_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_common_max_val_dbal_pairs_get_f) (
    int unit);


typedef const dnx_data_field_common_max_val_array_id_type_t *(
    *dnx_data_field_common_max_val_array_id_type_get_f) (
    int unit,
    int array_id);



typedef struct
{
    
    dnx_data_field_common_max_val_feature_get_f feature_get;
    
    dnx_data_field_common_max_val_nof_ffc_get_f nof_ffc_get;
    
    dnx_data_field_common_max_val_nof_masks_per_fes_get_f nof_masks_per_fes_get;
    
    dnx_data_field_common_max_val_nof_fes_id_per_array_get_f nof_fes_id_per_array_get;
    
    dnx_data_field_common_max_val_nof_fes_array_get_f nof_fes_array_get;
    
    dnx_data_field_common_max_val_nof_fes_instruction_per_context_get_f nof_fes_instruction_per_context_get;
    
    dnx_data_field_common_max_val_nof_fes_programs_get_f nof_fes_programs_get;
    
    dnx_data_field_common_max_val_nof_prog_per_fes_get_f nof_prog_per_fes_get;
    
    dnx_data_field_common_max_val_nof_cs_lines_get_f nof_cs_lines_get;
    
    dnx_data_field_common_max_val_nof_contexts_get_f nof_contexts_get;
    
    dnx_data_field_common_max_val_nof_actions_get_f nof_actions_get;
    
    dnx_data_field_common_max_val_nof_qualifiers_get_f nof_qualifiers_get;
    
    dnx_data_field_common_max_val_nof_bits_in_fes_action_get_f nof_bits_in_fes_action_get;
    
    dnx_data_field_common_max_val_nof_bits_in_fes_key_select_get_f nof_bits_in_fes_key_select_get;
    
    dnx_data_field_common_max_val_nof_bits_in_ffc_get_f nof_bits_in_ffc_get;
    
    dnx_data_field_common_max_val_nof_ffc_in_uint32_get_f nof_ffc_in_uint32_get;
    
    dnx_data_field_common_max_val_nof_action_per_group_get_f nof_action_per_group_get;
    
    dnx_data_field_common_max_val_nof_layer_records_get_f nof_layer_records_get;
    
    dnx_data_field_common_max_val_layer_record_size_get_f layer_record_size_get;
    
    dnx_data_field_common_max_val_nof_l4_ops_ranges_legacy_get_f nof_l4_ops_ranges_legacy_get;
    
    dnx_data_field_common_max_val_nof_pkt_hdr_ranges_get_f nof_pkt_hdr_ranges_get;
    
    dnx_data_field_common_max_val_nof_out_lif_ranges_get_f nof_out_lif_ranges_get;
    
    dnx_data_field_common_max_val_kbr_size_get_f kbr_size_get;
    
    dnx_data_field_common_max_val_nof_compare_pairs_in_compare_mode_get_f nof_compare_pairs_in_compare_mode_get;
    
    dnx_data_field_common_max_val_nof_bits_in_fem_action_get_f nof_bits_in_fem_action_get;
    
    dnx_data_field_common_max_val_nof_fem_condition_get_f nof_fem_condition_get;
    
    dnx_data_field_common_max_val_nof_fem_map_index_get_f nof_fem_map_index_get;
    
    dnx_data_field_common_max_val_nof_fem_programs_get_f nof_fem_programs_get;
    
    dnx_data_field_common_max_val_nof_fem_id_get_f nof_fem_id_get;
    
    dnx_data_field_common_max_val_nof_array_ids_get_f nof_array_ids_get;
    
    dnx_data_field_common_max_val_dbal_pairs_get_f dbal_pairs_get;
    
    dnx_data_field_common_max_val_array_id_type_get_f array_id_type_get;
    
    dnxc_data_table_info_get_f array_id_type_info_get;
} dnx_data_if_field_common_max_val_t;







typedef enum
{

    
    _dnx_data_field_init_feature_nof
} dnx_data_field_init_feature_e;



typedef int(
    *dnx_data_field_init_feature_get_f) (
    int unit,
    dnx_data_field_init_feature_e feature);


typedef uint32(
    *dnx_data_field_init_fec_dest_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_init_l4_trap_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_init_oam_layer_index_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_init_oam_stat_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_init_roo_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_init_disable_erpp_counters_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_init_jr1_ipmc_inlif_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_init_j1_same_port_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_init_j1_learning_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_init_learn_limit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_init_j1_php_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_init_ftmh_mc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_init_flow_id_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_init_feature_get_f feature_get;
    
    dnx_data_field_init_fec_dest_get_f fec_dest_get;
    
    dnx_data_field_init_l4_trap_get_f l4_trap_get;
    
    dnx_data_field_init_oam_layer_index_get_f oam_layer_index_get;
    
    dnx_data_field_init_oam_stat_get_f oam_stat_get;
    
    dnx_data_field_init_roo_get_f roo_get;
    
    dnx_data_field_init_disable_erpp_counters_get_f disable_erpp_counters_get;
    
    dnx_data_field_init_jr1_ipmc_inlif_get_f jr1_ipmc_inlif_get;
    
    dnx_data_field_init_j1_same_port_get_f j1_same_port_get;
    
    dnx_data_field_init_j1_learning_get_f j1_learning_get;
    
    dnx_data_field_init_learn_limit_get_f learn_limit_get;
    
    dnx_data_field_init_j1_php_get_f j1_php_get;
    
    dnx_data_field_init_ftmh_mc_get_f ftmh_mc_get;
    
    dnx_data_field_init_flow_id_get_f flow_id_get;
} dnx_data_if_field_init_t;







typedef enum
{

    
    _dnx_data_field_features_feature_nof
} dnx_data_field_features_feature_e;



typedef int(
    *dnx_data_field_features_feature_get_f) (
    int unit,
    dnx_data_field_features_feature_e feature);


typedef uint32(
    *dnx_data_field_features_tcam_cs_is_tcam_direct_access_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_ecc_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_switch_to_acl_context_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_per_pp_port_pmf_profile_cs_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_tcam_result_flip_eco_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_tcam_result_half_payload_on_msb_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_parsing_start_offset_msb_meaningless_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_kbp_opcode_in_ipmf1_cs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_kbp_hitbits_correct_in_ipmf1_cs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_exem_vmv_removable_from_payload_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_multiple_dynamic_mem_enablers_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_aacl_super_group_handler_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_aacl_tcam_swap_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_extended_l4_ops_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_state_table_ipmf1_key_select_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_state_table_acr_bus_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_state_table_atomic_rmw_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_hitbit_volatile_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_fem_replace_msb_instead_lsb_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_dir_ext_epmf_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_dir_ext_epmf_key_has_debug_signal_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_hashing_process_lsb_to_msb_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_exem_age_flush_scan_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_epmf_has_lookup_enabler_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_epmf_outlif_profile_map_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_tcam_full_key_half_payload_both_lsb_msb_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_epmf_exem_debug_signal_move_zero_padding_from_msb_to_lsb_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_ecmp_tunnel_priority_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_key_result_hit_signals_exist_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_external_cs_based_on_fwd2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_udh_base_cs_is_mapped_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_state_collection_engine_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_cs_lif_profile_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_empf_trap_action_is_strength_based_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_has_last_keys_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_ipmf3_has_last_keys_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_ipmf3_context_signal_is_from_register_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_epmf_context_signal_exists_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_epmf_context_signal_from_phl_block_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_epmf_is_on_etpp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_de_has_debug_signals_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_result_hit_signal_names_always_include_key_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_features_ipmf3_context_has_oam_stateless_property_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_features_feature_get_f feature_get;
    
    dnx_data_field_features_tcam_cs_is_tcam_direct_access_get_f tcam_cs_is_tcam_direct_access_get;
    
    dnx_data_field_features_ecc_enable_get_f ecc_enable_get;
    
    dnx_data_field_features_switch_to_acl_context_get_f switch_to_acl_context_get;
    
    dnx_data_field_features_per_pp_port_pmf_profile_cs_offset_get_f per_pp_port_pmf_profile_cs_offset_get;
    
    dnx_data_field_features_tcam_result_flip_eco_get_f tcam_result_flip_eco_get;
    
    dnx_data_field_features_tcam_result_half_payload_on_msb_get_f tcam_result_half_payload_on_msb_get;
    
    dnx_data_field_features_parsing_start_offset_msb_meaningless_get_f parsing_start_offset_msb_meaningless_get;
    
    dnx_data_field_features_kbp_opcode_in_ipmf1_cs_get_f kbp_opcode_in_ipmf1_cs_get;
    
    dnx_data_field_features_kbp_hitbits_correct_in_ipmf1_cs_get_f kbp_hitbits_correct_in_ipmf1_cs_get;
    
    dnx_data_field_features_exem_vmv_removable_from_payload_get_f exem_vmv_removable_from_payload_get;
    
    dnx_data_field_features_multiple_dynamic_mem_enablers_get_f multiple_dynamic_mem_enablers_get;
    
    dnx_data_field_features_aacl_super_group_handler_enable_get_f aacl_super_group_handler_enable_get;
    
    dnx_data_field_features_aacl_tcam_swap_enable_get_f aacl_tcam_swap_enable_get;
    
    dnx_data_field_features_extended_l4_ops_get_f extended_l4_ops_get;
    
    dnx_data_field_features_state_table_ipmf1_key_select_get_f state_table_ipmf1_key_select_get;
    
    dnx_data_field_features_state_table_acr_bus_get_f state_table_acr_bus_get;
    
    dnx_data_field_features_state_table_atomic_rmw_get_f state_table_atomic_rmw_get;
    
    dnx_data_field_features_hitbit_volatile_get_f hitbit_volatile_get;
    
    dnx_data_field_features_fem_replace_msb_instead_lsb_get_f fem_replace_msb_instead_lsb_get;
    
    dnx_data_field_features_dir_ext_epmf_get_f dir_ext_epmf_get;
    
    dnx_data_field_features_dir_ext_epmf_key_has_debug_signal_get_f dir_ext_epmf_key_has_debug_signal_get;
    
    dnx_data_field_features_hashing_process_lsb_to_msb_get_f hashing_process_lsb_to_msb_get;
    
    dnx_data_field_features_exem_age_flush_scan_get_f exem_age_flush_scan_get;
    
    dnx_data_field_features_epmf_has_lookup_enabler_get_f epmf_has_lookup_enabler_get;
    
    dnx_data_field_features_epmf_outlif_profile_map_get_f epmf_outlif_profile_map_get;
    
    dnx_data_field_features_tcam_full_key_half_payload_both_lsb_msb_get_f tcam_full_key_half_payload_both_lsb_msb_get;
    
    dnx_data_field_features_epmf_exem_debug_signal_move_zero_padding_from_msb_to_lsb_get_f epmf_exem_debug_signal_move_zero_padding_from_msb_to_lsb_get;
    
    dnx_data_field_features_ecmp_tunnel_priority_get_f ecmp_tunnel_priority_get;
    
    dnx_data_field_features_key_result_hit_signals_exist_get_f key_result_hit_signals_exist_get;
    
    dnx_data_field_features_external_cs_based_on_fwd2_get_f external_cs_based_on_fwd2_get;
    
    dnx_data_field_features_udh_base_cs_is_mapped_get_f udh_base_cs_is_mapped_get;
    
    dnx_data_field_features_state_collection_engine_get_f state_collection_engine_get;
    
    dnx_data_field_features_cs_lif_profile_get_f cs_lif_profile_get;
    
    dnx_data_field_features_empf_trap_action_is_strength_based_get_f empf_trap_action_is_strength_based_get;
    
    dnx_data_field_features_has_last_keys_get_f has_last_keys_get;
    
    dnx_data_field_features_ipmf3_has_last_keys_get_f ipmf3_has_last_keys_get;
    
    dnx_data_field_features_ipmf3_context_signal_is_from_register_get_f ipmf3_context_signal_is_from_register_get;
    
    dnx_data_field_features_epmf_context_signal_exists_get_f epmf_context_signal_exists_get;
    
    dnx_data_field_features_epmf_context_signal_from_phl_block_get_f epmf_context_signal_from_phl_block_get;
    
    dnx_data_field_features_epmf_is_on_etpp_get_f epmf_is_on_etpp_get;
    
    dnx_data_field_features_de_has_debug_signals_get_f de_has_debug_signals_get;
    
    dnx_data_field_features_result_hit_signal_names_always_include_key_get_f result_hit_signal_names_always_include_key_get;
    
    dnx_data_field_features_ipmf3_context_has_oam_stateless_property_get_f ipmf3_context_has_oam_stateless_property_get;
} dnx_data_if_field_features_t;







typedef enum
{

    
    _dnx_data_field_signal_sizes_feature_nof
} dnx_data_field_signal_sizes_feature_e;



typedef int(
    *dnx_data_field_signal_sizes_feature_get_f) (
    int unit,
    dnx_data_field_signal_sizes_feature_e feature);


typedef uint32(
    *dnx_data_field_signal_sizes_dual_queue_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_signal_sizes_packet_header_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_signal_sizes_ecn_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_signal_sizes_congestion_info_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_signal_sizes_parsing_start_offset_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_signal_sizes_feature_get_f feature_get;
    
    dnx_data_field_signal_sizes_dual_queue_size_get_f dual_queue_size_get;
    
    dnx_data_field_signal_sizes_packet_header_size_get_f packet_header_size_get;
    
    dnx_data_field_signal_sizes_ecn_get_f ecn_get;
    
    dnx_data_field_signal_sizes_congestion_info_get_f congestion_info_get;
    
    dnx_data_field_signal_sizes_parsing_start_offset_size_get_f parsing_start_offset_size_get;
} dnx_data_if_field_signal_sizes_t;







typedef enum
{

    
    _dnx_data_field_dnx_data_internal_feature_nof
} dnx_data_field_dnx_data_internal_feature_e;



typedef int(
    *dnx_data_field_dnx_data_internal_feature_get_f) (
    int unit,
    dnx_data_field_dnx_data_internal_feature_e feature);


typedef uint32(
    *dnx_data_field_dnx_data_internal_lr_eth_is_da_mac_valid_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_dnx_data_internal_feature_get_f feature_get;
    
    dnx_data_field_dnx_data_internal_lr_eth_is_da_mac_valid_get_f lr_eth_is_da_mac_valid_get;
} dnx_data_if_field_dnx_data_internal_t;







typedef enum
{

    
    _dnx_data_field_tests_feature_nof
} dnx_data_field_tests_feature_e;



typedef int(
    *dnx_data_field_tests_feature_get_f) (
    int unit,
    dnx_data_field_tests_feature_e feature);


typedef uint32(
    *dnx_data_field_tests_learn_info_actions_structure_change_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tests_ingress_time_stamp_increased_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tests_ace_debug_signals_exist_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tests_epmf_debug_rely_on_tcam0_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tests_epmf_tm_port_cs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tests_small_exem_addr_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tests_stub_adapter_supports_semantic_flush_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tests_parsing_start_offset_exists_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tests_first_part_of_acr_zero_padded_by_32_get_f) (
    int unit);


typedef uint32(
    *dnx_data_field_tests_ext_stat_valid_can_be_read_in_tm_cmd_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_field_tests_feature_get_f feature_get;
    
    dnx_data_field_tests_learn_info_actions_structure_change_get_f learn_info_actions_structure_change_get;
    
    dnx_data_field_tests_ingress_time_stamp_increased_get_f ingress_time_stamp_increased_get;
    
    dnx_data_field_tests_ace_debug_signals_exist_get_f ace_debug_signals_exist_get;
    
    dnx_data_field_tests_epmf_debug_rely_on_tcam0_get_f epmf_debug_rely_on_tcam0_get;
    
    dnx_data_field_tests_epmf_tm_port_cs_get_f epmf_tm_port_cs_get;
    
    dnx_data_field_tests_small_exem_addr_bits_get_f small_exem_addr_bits_get;
    
    dnx_data_field_tests_stub_adapter_supports_semantic_flush_get_f stub_adapter_supports_semantic_flush_get;
    
    dnx_data_field_tests_parsing_start_offset_exists_get_f parsing_start_offset_exists_get;
    
    dnx_data_field_tests_first_part_of_acr_zero_padded_by_32_get_f first_part_of_acr_zero_padded_by_32_get;
    
    dnx_data_field_tests_ext_stat_valid_can_be_read_in_tm_cmd_get_f ext_stat_valid_can_be_read_in_tm_cmd_get;
} dnx_data_if_field_tests_t;





typedef struct
{
    
    dnx_data_if_field_base_ipmf1_t base_ipmf1;
    
    dnx_data_if_field_base_ipmf2_t base_ipmf2;
    
    dnx_data_if_field_base_ipmf3_t base_ipmf3;
    
    dnx_data_if_field_base_epmf_t base_epmf;
    
    dnx_data_if_field_base_ifwd2_t base_ifwd2;
    
    dnx_data_if_field_stage_t stage;
    
    dnx_data_if_field_external_tcam_t external_tcam;
    
    dnx_data_if_field_tcam_t tcam;
    
    dnx_data_if_field_group_t group;
    
    dnx_data_if_field_efes_t efes;
    
    dnx_data_if_field_fem_t fem;
    
    dnx_data_if_field_context_t context;
    
    dnx_data_if_field_preselector_t preselector;
    
    dnx_data_if_field_qual_t qual;
    
    dnx_data_if_field_action_t action;
    
    dnx_data_if_field_virtual_wire_t virtual_wire;
    
    dnx_data_if_field_profile_bits_t profile_bits;
    
    dnx_data_if_field_dir_ext_t dir_ext;
    
    dnx_data_if_field_state_table_t state_table;
    
    dnx_data_if_field_sce_t sce;
    
    dnx_data_if_field_mdb_dt_t mdb_dt;
    
    dnx_data_if_field_hash_t hash;
    
    dnx_data_if_field_udh_t udh;
    
    dnx_data_if_field_system_headers_t system_headers;
    
    dnx_data_if_field_exem_t exem;
    
    dnx_data_if_field_exem_learn_flush_machine_t exem_learn_flush_machine;
    
    dnx_data_if_field_ace_t ace;
    
    dnx_data_if_field_entry_t entry;
    
    dnx_data_if_field_L4_Ops_t L4_Ops;
    
    dnx_data_if_field_encoded_qual_actions_offset_t encoded_qual_actions_offset;
    
    dnx_data_if_field_compare_t compare;
    
    dnx_data_if_field_diag_t diag;
    
    dnx_data_if_field_general_data_qualifiers_t general_data_qualifiers;
    
    dnx_data_if_field_common_max_val_t common_max_val;
    
    dnx_data_if_field_init_t init;
    
    dnx_data_if_field_features_t features;
    
    dnx_data_if_field_signal_sizes_t signal_sizes;
    
    dnx_data_if_field_dnx_data_internal_t dnx_data_internal;
    
    dnx_data_if_field_tests_t tests;
} dnx_data_if_field_t;




extern dnx_data_if_field_t dnx_data_field;


#endif 

