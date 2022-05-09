
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_field.h>




extern shr_error_e jr2_a0_data_field_attach(
    int unit);


extern shr_error_e jr2_b0_data_field_attach(
    int unit);


extern shr_error_e j2c_a0_data_field_attach(
    int unit);


extern shr_error_e q2a_a0_data_field_attach(
    int unit);


extern shr_error_e j2p_a0_data_field_attach(
    int unit);


extern shr_error_e j2x_a0_data_field_attach(
    int unit);




static shr_error_e
dnx_data_field_base_ipmf1_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "base_ipmf1";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_base_ipmf1_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field base_ipmf1 features");

    
    submodule_data->nof_defines = _dnx_data_field_base_ipmf1_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field base_ipmf1 defines");

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_ffc].name = "nof_ffc";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_ffc].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_ffc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_keys].name = "nof_keys";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_keys].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_keys].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_keys_alloc].name = "nof_keys_alloc";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_keys_alloc].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_keys_alloc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_tcam].name = "nof_keys_alloc_for_tcam";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_tcam].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_tcam].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_exem].name = "nof_keys_alloc_for_exem";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_exem].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_exem].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_dir_ext].name = "nof_keys_alloc_for_dir_ext";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_dir_ext].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_dir_ext].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_mdb_dt].name = "nof_keys_alloc_for_mdb_dt";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_mdb_dt].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_mdb_dt].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_masks_per_fes].name = "nof_masks_per_fes";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_masks_per_fes].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_masks_per_fes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fes_id_per_array].name = "nof_fes_id_per_array";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fes_id_per_array].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fes_id_per_array].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fes_array].name = "nof_fes_array";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fes_array].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fes_array].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fes_instruction_per_context].name = "nof_fes_instruction_per_context";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fes_instruction_per_context].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fes_instruction_per_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fes_programs].name = "nof_fes_programs";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fes_programs].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fes_programs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_prog_per_fes].name = "nof_prog_per_fes";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_prog_per_fes].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_prog_per_fes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_cs_container_5_selected_bits_size].name = "cs_container_5_selected_bits_size";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_cs_container_5_selected_bits_size].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_cs_container_5_selected_bits_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_contexts].name = "nof_contexts";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_contexts].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_contexts].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_link_profiles].name = "nof_link_profiles";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_link_profiles].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_link_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_cs_lines].name = "nof_cs_lines";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_cs_lines].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_cs_lines].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_actions].name = "nof_actions";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_actions].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_actions].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_qualifiers].name = "nof_qualifiers";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_qualifiers].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_qualifiers].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_bits_in_fes_action].name = "nof_bits_in_fes_action";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_bits_in_fes_action].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_bits_in_fes_action].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_bits_in_fes_key_select].name = "nof_bits_in_fes_key_select";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_bits_in_fes_key_select].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_bits_in_fes_key_select].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_fes_key_select_resolution_in_bits].name = "fes_key_select_resolution_in_bits";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_fes_key_select_resolution_in_bits].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_fes_key_select_resolution_in_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_fem_condition_ms_bit_min_value].name = "fem_condition_ms_bit_min_value";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_fem_condition_ms_bit_min_value].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_fem_condition_ms_bit_min_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fem_programs].name = "nof_fem_programs";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fem_programs].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fem_programs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_log_nof_bits_in_fem_key_select].name = "log_nof_bits_in_fem_key_select";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_log_nof_bits_in_fem_key_select].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_log_nof_bits_in_fem_key_select].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_bits_in_fem_key_select].name = "nof_bits_in_fem_key_select";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_bits_in_fem_key_select].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_bits_in_fem_key_select].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_fem_key_select_resolution_in_bits].name = "fem_key_select_resolution_in_bits";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_fem_key_select_resolution_in_bits].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_fem_key_select_resolution_in_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_log_nof_bits_in_fem_map_data_field].name = "log_nof_bits_in_fem_map_data_field";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_log_nof_bits_in_fem_map_data_field].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_log_nof_bits_in_fem_map_data_field].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_bits_in_fem_map_data_field].name = "nof_bits_in_fem_map_data_field";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_bits_in_fem_map_data_field].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_bits_in_fem_map_data_field].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action_fems_2_15].name = "nof_bits_in_fem_action_fems_2_15";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action_fems_2_15].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action_fems_2_15].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action_fems_0_1].name = "nof_bits_in_fem_action_fems_0_1";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action_fems_0_1].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action_fems_0_1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action].name = "nof_bits_in_fem_action";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fem_condition].name = "nof_fem_condition";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fem_condition].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fem_condition].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fem_map_index].name = "nof_fem_map_index";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fem_map_index].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fem_map_index].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fem_id].name = "nof_fem_id";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fem_id].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fem_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_num_fems_with_short_action].name = "num_fems_with_short_action";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_num_fems_with_short_action].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_num_fems_with_short_action].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_num_bits_in_fem_field_select].name = "num_bits_in_fem_field_select";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_num_bits_in_fem_field_select].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_num_bits_in_fem_field_select].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fem_id_per_array].name = "nof_fem_id_per_array";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fem_id_per_array].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fem_id_per_array].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fem_array].name = "nof_fem_array";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fem_array].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_fem_array].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_compare_pairs_in_compare_mode].name = "nof_compare_pairs_in_compare_mode";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_compare_pairs_in_compare_mode].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_compare_pairs_in_compare_mode].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_compare_key_size].name = "compare_key_size";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_compare_key_size].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_compare_key_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_l4_ops_ranges_legacy].name = "nof_l4_ops_ranges_legacy";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_l4_ops_ranges_legacy].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_l4_ops_ranges_legacy].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_ext_l4_ops_ranges].name = "nof_ext_l4_ops_ranges";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_ext_l4_ops_ranges].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_ext_l4_ops_ranges].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_pkt_hdr_ranges].name = "nof_pkt_hdr_ranges";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_pkt_hdr_ranges].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_pkt_hdr_ranges].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_out_lif_ranges].name = "nof_out_lif_ranges";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_out_lif_ranges].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_nof_out_lif_ranges].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_cmp_selection].name = "cmp_selection";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_cmp_selection].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_cmp_selection].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_dir_ext_single_key_size].name = "dir_ext_single_key_size";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_dir_ext_single_key_size].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_dir_ext_single_key_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_dir_ext_first_key].name = "dir_ext_first_key";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_dir_ext_first_key].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_dir_ext_first_key].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_exem_max_result_size].name = "exem_max_result_size";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_exem_max_result_size].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_exem_max_result_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_fes_instruction_size].name = "fes_instruction_size";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_fes_instruction_size].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_fes_instruction_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_fes_pgm_id_offset].name = "fes_pgm_id_offset";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_fes_pgm_id_offset].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_fes_pgm_id_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_fes_key_select_for_zero_bit].name = "fes_key_select_for_zero_bit";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_fes_key_select_for_zero_bit].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_fes_key_select_for_zero_bit].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_fes_shift_for_zero_bit].name = "fes_shift_for_zero_bit";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_fes_shift_for_zero_bit].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_fes_shift_for_zero_bit].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_field_base_ipmf1_define_exem_pdb].name = "exem_pdb";
    submodule_data->defines[dnx_data_field_base_ipmf1_define_exem_pdb].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf1_define_exem_pdb].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_field_base_ipmf1_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field base_ipmf1 tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_base_ipmf1_feature_get(
    int unit,
    dnx_data_field_base_ipmf1_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, feature);
}


uint32
dnx_data_field_base_ipmf1_nof_ffc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_ffc);
}

uint32
dnx_data_field_base_ipmf1_nof_keys_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_keys);
}

uint32
dnx_data_field_base_ipmf1_nof_keys_alloc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_keys_alloc);
}

uint32
dnx_data_field_base_ipmf1_nof_keys_alloc_for_tcam_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_tcam);
}

uint32
dnx_data_field_base_ipmf1_nof_keys_alloc_for_exem_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_exem);
}

uint32
dnx_data_field_base_ipmf1_nof_keys_alloc_for_dir_ext_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_dir_ext);
}

uint32
dnx_data_field_base_ipmf1_nof_keys_alloc_for_mdb_dt_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_keys_alloc_for_mdb_dt);
}

uint32
dnx_data_field_base_ipmf1_nof_masks_per_fes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_masks_per_fes);
}

uint32
dnx_data_field_base_ipmf1_nof_fes_id_per_array_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_fes_id_per_array);
}

uint32
dnx_data_field_base_ipmf1_nof_fes_array_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_fes_array);
}

uint32
dnx_data_field_base_ipmf1_nof_fes_instruction_per_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_fes_instruction_per_context);
}

uint32
dnx_data_field_base_ipmf1_nof_fes_programs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_fes_programs);
}

uint32
dnx_data_field_base_ipmf1_nof_prog_per_fes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_prog_per_fes);
}

uint32
dnx_data_field_base_ipmf1_cs_container_5_selected_bits_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_cs_container_5_selected_bits_size);
}

uint32
dnx_data_field_base_ipmf1_nof_contexts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_contexts);
}

uint32
dnx_data_field_base_ipmf1_nof_link_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_link_profiles);
}

uint32
dnx_data_field_base_ipmf1_nof_cs_lines_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_cs_lines);
}

uint32
dnx_data_field_base_ipmf1_nof_actions_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_actions);
}

uint32
dnx_data_field_base_ipmf1_nof_qualifiers_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_qualifiers);
}

uint32
dnx_data_field_base_ipmf1_nof_bits_in_fes_action_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_bits_in_fes_action);
}

uint32
dnx_data_field_base_ipmf1_nof_bits_in_fes_key_select_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_bits_in_fes_key_select);
}

uint32
dnx_data_field_base_ipmf1_fes_key_select_resolution_in_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_fes_key_select_resolution_in_bits);
}

uint32
dnx_data_field_base_ipmf1_fem_condition_ms_bit_min_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_fem_condition_ms_bit_min_value);
}

uint32
dnx_data_field_base_ipmf1_nof_fem_programs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_fem_programs);
}

uint32
dnx_data_field_base_ipmf1_log_nof_bits_in_fem_key_select_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_log_nof_bits_in_fem_key_select);
}

uint32
dnx_data_field_base_ipmf1_nof_bits_in_fem_key_select_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_bits_in_fem_key_select);
}

uint32
dnx_data_field_base_ipmf1_fem_key_select_resolution_in_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_fem_key_select_resolution_in_bits);
}

uint32
dnx_data_field_base_ipmf1_log_nof_bits_in_fem_map_data_field_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_log_nof_bits_in_fem_map_data_field);
}

uint32
dnx_data_field_base_ipmf1_nof_bits_in_fem_map_data_field_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_bits_in_fem_map_data_field);
}

uint32
dnx_data_field_base_ipmf1_nof_bits_in_fem_action_fems_2_15_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action_fems_2_15);
}

uint32
dnx_data_field_base_ipmf1_nof_bits_in_fem_action_fems_0_1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action_fems_0_1);
}

uint32
dnx_data_field_base_ipmf1_nof_bits_in_fem_action_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_bits_in_fem_action);
}

uint32
dnx_data_field_base_ipmf1_nof_fem_condition_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_fem_condition);
}

uint32
dnx_data_field_base_ipmf1_nof_fem_map_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_fem_map_index);
}

uint32
dnx_data_field_base_ipmf1_nof_fem_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_fem_id);
}

uint32
dnx_data_field_base_ipmf1_num_fems_with_short_action_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_num_fems_with_short_action);
}

uint32
dnx_data_field_base_ipmf1_num_bits_in_fem_field_select_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_num_bits_in_fem_field_select);
}

uint32
dnx_data_field_base_ipmf1_nof_fem_id_per_array_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_fem_id_per_array);
}

uint32
dnx_data_field_base_ipmf1_nof_fem_array_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_fem_array);
}

uint32
dnx_data_field_base_ipmf1_nof_compare_pairs_in_compare_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_compare_pairs_in_compare_mode);
}

uint32
dnx_data_field_base_ipmf1_compare_key_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_compare_key_size);
}

uint32
dnx_data_field_base_ipmf1_nof_l4_ops_ranges_legacy_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_l4_ops_ranges_legacy);
}

uint32
dnx_data_field_base_ipmf1_nof_ext_l4_ops_ranges_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_ext_l4_ops_ranges);
}

uint32
dnx_data_field_base_ipmf1_nof_pkt_hdr_ranges_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_pkt_hdr_ranges);
}

uint32
dnx_data_field_base_ipmf1_nof_out_lif_ranges_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_nof_out_lif_ranges);
}

uint32
dnx_data_field_base_ipmf1_cmp_selection_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_cmp_selection);
}

uint32
dnx_data_field_base_ipmf1_dir_ext_single_key_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_dir_ext_single_key_size);
}

uint32
dnx_data_field_base_ipmf1_dir_ext_first_key_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_dir_ext_first_key);
}

uint32
dnx_data_field_base_ipmf1_exem_max_result_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_exem_max_result_size);
}

uint32
dnx_data_field_base_ipmf1_fes_instruction_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_fes_instruction_size);
}

uint32
dnx_data_field_base_ipmf1_fes_pgm_id_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_fes_pgm_id_offset);
}

uint32
dnx_data_field_base_ipmf1_fes_key_select_for_zero_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_fes_key_select_for_zero_bit);
}

uint32
dnx_data_field_base_ipmf1_fes_shift_for_zero_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_fes_shift_for_zero_bit);
}

uint32
dnx_data_field_base_ipmf1_exem_pdb_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf1, dnx_data_field_base_ipmf1_define_exem_pdb);
}










static shr_error_e
dnx_data_field_base_ipmf2_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "base_ipmf2";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_base_ipmf2_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field base_ipmf2 features");

    
    submodule_data->nof_defines = _dnx_data_field_base_ipmf2_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field base_ipmf2 defines");

    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_ffc].name = "nof_ffc";
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_ffc].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_ffc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_keys].name = "nof_keys";
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_keys].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_keys].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_keys_alloc].name = "nof_keys_alloc";
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_keys_alloc].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_keys_alloc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_tcam].name = "nof_keys_alloc_for_tcam";
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_tcam].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_tcam].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_exem].name = "nof_keys_alloc_for_exem";
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_exem].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_exem].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_dir_ext].name = "nof_keys_alloc_for_dir_ext";
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_dir_ext].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_dir_ext].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_mdb_dt].name = "nof_keys_alloc_for_mdb_dt";
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_mdb_dt].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_mdb_dt].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_masks_per_fes].name = "nof_masks_per_fes";
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_masks_per_fes].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_masks_per_fes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_fes_id_per_array].name = "nof_fes_id_per_array";
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_fes_id_per_array].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_fes_id_per_array].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_fes_array].name = "nof_fes_array";
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_fes_array].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_fes_array].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_fes_instruction_per_context].name = "nof_fes_instruction_per_context";
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_fes_instruction_per_context].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_fes_instruction_per_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_cs_lines].name = "nof_cs_lines";
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_cs_lines].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_cs_lines].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_contexts].name = "nof_contexts";
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_contexts].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_contexts].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_qualifiers].name = "nof_qualifiers";
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_qualifiers].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf2_define_nof_qualifiers].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf2_define_dir_ext_single_key_size].name = "dir_ext_single_key_size";
    submodule_data->defines[dnx_data_field_base_ipmf2_define_dir_ext_single_key_size].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf2_define_dir_ext_single_key_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf2_define_dir_ext_first_key].name = "dir_ext_first_key";
    submodule_data->defines[dnx_data_field_base_ipmf2_define_dir_ext_first_key].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf2_define_dir_ext_first_key].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf2_define_exem_max_result_size].name = "exem_max_result_size";
    submodule_data->defines[dnx_data_field_base_ipmf2_define_exem_max_result_size].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf2_define_exem_max_result_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf2_define_fes_key_select_for_zero_bit].name = "fes_key_select_for_zero_bit";
    submodule_data->defines[dnx_data_field_base_ipmf2_define_fes_key_select_for_zero_bit].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf2_define_fes_key_select_for_zero_bit].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_field_base_ipmf2_define_fes_shift_for_zero_bit].name = "fes_shift_for_zero_bit";
    submodule_data->defines[dnx_data_field_base_ipmf2_define_fes_shift_for_zero_bit].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf2_define_fes_shift_for_zero_bit].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_field_base_ipmf2_define_exem_pdb].name = "exem_pdb";
    submodule_data->defines[dnx_data_field_base_ipmf2_define_exem_pdb].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf2_define_exem_pdb].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_field_base_ipmf2_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field base_ipmf2 tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_base_ipmf2_feature_get(
    int unit,
    dnx_data_field_base_ipmf2_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf2, feature);
}


uint32
dnx_data_field_base_ipmf2_nof_ffc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf2, dnx_data_field_base_ipmf2_define_nof_ffc);
}

uint32
dnx_data_field_base_ipmf2_nof_keys_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf2, dnx_data_field_base_ipmf2_define_nof_keys);
}

uint32
dnx_data_field_base_ipmf2_nof_keys_alloc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf2, dnx_data_field_base_ipmf2_define_nof_keys_alloc);
}

uint32
dnx_data_field_base_ipmf2_nof_keys_alloc_for_tcam_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf2, dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_tcam);
}

uint32
dnx_data_field_base_ipmf2_nof_keys_alloc_for_exem_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf2, dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_exem);
}

uint32
dnx_data_field_base_ipmf2_nof_keys_alloc_for_dir_ext_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf2, dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_dir_ext);
}

uint32
dnx_data_field_base_ipmf2_nof_keys_alloc_for_mdb_dt_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf2, dnx_data_field_base_ipmf2_define_nof_keys_alloc_for_mdb_dt);
}

uint32
dnx_data_field_base_ipmf2_nof_masks_per_fes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf2, dnx_data_field_base_ipmf2_define_nof_masks_per_fes);
}

uint32
dnx_data_field_base_ipmf2_nof_fes_id_per_array_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf2, dnx_data_field_base_ipmf2_define_nof_fes_id_per_array);
}

uint32
dnx_data_field_base_ipmf2_nof_fes_array_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf2, dnx_data_field_base_ipmf2_define_nof_fes_array);
}

uint32
dnx_data_field_base_ipmf2_nof_fes_instruction_per_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf2, dnx_data_field_base_ipmf2_define_nof_fes_instruction_per_context);
}

uint32
dnx_data_field_base_ipmf2_nof_cs_lines_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf2, dnx_data_field_base_ipmf2_define_nof_cs_lines);
}

uint32
dnx_data_field_base_ipmf2_nof_contexts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf2, dnx_data_field_base_ipmf2_define_nof_contexts);
}

uint32
dnx_data_field_base_ipmf2_nof_qualifiers_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf2, dnx_data_field_base_ipmf2_define_nof_qualifiers);
}

uint32
dnx_data_field_base_ipmf2_dir_ext_single_key_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf2, dnx_data_field_base_ipmf2_define_dir_ext_single_key_size);
}

uint32
dnx_data_field_base_ipmf2_dir_ext_first_key_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf2, dnx_data_field_base_ipmf2_define_dir_ext_first_key);
}

uint32
dnx_data_field_base_ipmf2_exem_max_result_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf2, dnx_data_field_base_ipmf2_define_exem_max_result_size);
}

uint32
dnx_data_field_base_ipmf2_fes_key_select_for_zero_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf2, dnx_data_field_base_ipmf2_define_fes_key_select_for_zero_bit);
}

uint32
dnx_data_field_base_ipmf2_fes_shift_for_zero_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf2, dnx_data_field_base_ipmf2_define_fes_shift_for_zero_bit);
}

uint32
dnx_data_field_base_ipmf2_exem_pdb_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf2, dnx_data_field_base_ipmf2_define_exem_pdb);
}










static shr_error_e
dnx_data_field_base_ipmf3_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "base_ipmf3";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_base_ipmf3_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field base_ipmf3 features");

    
    submodule_data->nof_defines = _dnx_data_field_base_ipmf3_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field base_ipmf3 defines");

    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_ffc].name = "nof_ffc";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_ffc].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_ffc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_keys].name = "nof_keys";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_keys].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_keys].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_keys_alloc].name = "nof_keys_alloc";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_keys_alloc].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_keys_alloc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_tcam].name = "nof_keys_alloc_for_tcam";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_tcam].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_tcam].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_exem].name = "nof_keys_alloc_for_exem";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_exem].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_exem].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_dir_ext].name = "nof_keys_alloc_for_dir_ext";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_dir_ext].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_dir_ext].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_mdb_dt].name = "nof_keys_alloc_for_mdb_dt";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_mdb_dt].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_mdb_dt].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_masks_per_fes].name = "nof_masks_per_fes";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_masks_per_fes].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_masks_per_fes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_fes_id_per_array].name = "nof_fes_id_per_array";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_fes_id_per_array].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_fes_id_per_array].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_fes_array].name = "nof_fes_array";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_fes_array].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_fes_array].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_fes_instruction_per_context].name = "nof_fes_instruction_per_context";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_fes_instruction_per_context].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_fes_instruction_per_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_fes_programs].name = "nof_fes_programs";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_fes_programs].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_fes_programs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_prog_per_fes].name = "nof_prog_per_fes";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_prog_per_fes].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_prog_per_fes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_cs_scratch_pad_size].name = "cs_scratch_pad_size";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_cs_scratch_pad_size].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_cs_scratch_pad_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_contexts].name = "nof_contexts";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_contexts].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_contexts].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_cs_lines].name = "nof_cs_lines";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_cs_lines].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_cs_lines].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_actions].name = "nof_actions";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_actions].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_actions].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_qualifiers].name = "nof_qualifiers";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_qualifiers].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_qualifiers].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_bits_in_fes_action].name = "nof_bits_in_fes_action";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_bits_in_fes_action].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_bits_in_fes_action].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_bits_in_fes_key_select].name = "nof_bits_in_fes_key_select";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_bits_in_fes_key_select].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_bits_in_fes_key_select].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_fes_key_select_resolution_in_bits].name = "fes_key_select_resolution_in_bits";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_fes_key_select_resolution_in_bits].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_fes_key_select_resolution_in_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_out_lif_ranges].name = "nof_out_lif_ranges";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_out_lif_ranges].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_out_lif_ranges].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_fes_instruction_size].name = "fes_instruction_size";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_fes_instruction_size].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_fes_instruction_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_fes_pgm_id_offset].name = "fes_pgm_id_offset";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_fes_pgm_id_offset].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_fes_pgm_id_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_dir_ext_single_key_size].name = "dir_ext_single_key_size";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_dir_ext_single_key_size].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_dir_ext_single_key_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_dir_ext_first_key].name = "dir_ext_first_key";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_dir_ext_first_key].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_dir_ext_first_key].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_exem_max_result_size].name = "exem_max_result_size";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_exem_max_result_size].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_exem_max_result_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_fes_used_by_sdk].name = "nof_fes_used_by_sdk";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_fes_used_by_sdk].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_nof_fes_used_by_sdk].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_fes_key_select_for_zero_bit].name = "fes_key_select_for_zero_bit";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_fes_key_select_for_zero_bit].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_fes_key_select_for_zero_bit].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_fes_shift_for_zero_bit].name = "fes_shift_for_zero_bit";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_fes_shift_for_zero_bit].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_fes_shift_for_zero_bit].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_field_base_ipmf3_define_exem_pdb].name = "exem_pdb";
    submodule_data->defines[dnx_data_field_base_ipmf3_define_exem_pdb].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ipmf3_define_exem_pdb].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_field_base_ipmf3_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field base_ipmf3 tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_base_ipmf3_feature_get(
    int unit,
    dnx_data_field_base_ipmf3_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, feature);
}


uint32
dnx_data_field_base_ipmf3_nof_ffc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_nof_ffc);
}

uint32
dnx_data_field_base_ipmf3_nof_keys_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_nof_keys);
}

uint32
dnx_data_field_base_ipmf3_nof_keys_alloc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_nof_keys_alloc);
}

uint32
dnx_data_field_base_ipmf3_nof_keys_alloc_for_tcam_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_tcam);
}

uint32
dnx_data_field_base_ipmf3_nof_keys_alloc_for_exem_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_exem);
}

uint32
dnx_data_field_base_ipmf3_nof_keys_alloc_for_dir_ext_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_dir_ext);
}

uint32
dnx_data_field_base_ipmf3_nof_keys_alloc_for_mdb_dt_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_nof_keys_alloc_for_mdb_dt);
}

uint32
dnx_data_field_base_ipmf3_nof_masks_per_fes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_nof_masks_per_fes);
}

uint32
dnx_data_field_base_ipmf3_nof_fes_id_per_array_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_nof_fes_id_per_array);
}

uint32
dnx_data_field_base_ipmf3_nof_fes_array_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_nof_fes_array);
}

uint32
dnx_data_field_base_ipmf3_nof_fes_instruction_per_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_nof_fes_instruction_per_context);
}

uint32
dnx_data_field_base_ipmf3_nof_fes_programs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_nof_fes_programs);
}

uint32
dnx_data_field_base_ipmf3_nof_prog_per_fes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_nof_prog_per_fes);
}

uint32
dnx_data_field_base_ipmf3_cs_scratch_pad_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_cs_scratch_pad_size);
}

uint32
dnx_data_field_base_ipmf3_nof_contexts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_nof_contexts);
}

uint32
dnx_data_field_base_ipmf3_nof_cs_lines_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_nof_cs_lines);
}

uint32
dnx_data_field_base_ipmf3_nof_actions_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_nof_actions);
}

uint32
dnx_data_field_base_ipmf3_nof_qualifiers_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_nof_qualifiers);
}

uint32
dnx_data_field_base_ipmf3_nof_bits_in_fes_action_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_nof_bits_in_fes_action);
}

uint32
dnx_data_field_base_ipmf3_nof_bits_in_fes_key_select_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_nof_bits_in_fes_key_select);
}

uint32
dnx_data_field_base_ipmf3_fes_key_select_resolution_in_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_fes_key_select_resolution_in_bits);
}

uint32
dnx_data_field_base_ipmf3_nof_out_lif_ranges_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_nof_out_lif_ranges);
}

uint32
dnx_data_field_base_ipmf3_fes_instruction_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_fes_instruction_size);
}

uint32
dnx_data_field_base_ipmf3_fes_pgm_id_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_fes_pgm_id_offset);
}

uint32
dnx_data_field_base_ipmf3_dir_ext_single_key_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_dir_ext_single_key_size);
}

uint32
dnx_data_field_base_ipmf3_dir_ext_first_key_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_dir_ext_first_key);
}

uint32
dnx_data_field_base_ipmf3_exem_max_result_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_exem_max_result_size);
}

uint32
dnx_data_field_base_ipmf3_nof_fes_used_by_sdk_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_nof_fes_used_by_sdk);
}

uint32
dnx_data_field_base_ipmf3_fes_key_select_for_zero_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_fes_key_select_for_zero_bit);
}

uint32
dnx_data_field_base_ipmf3_fes_shift_for_zero_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_fes_shift_for_zero_bit);
}

uint32
dnx_data_field_base_ipmf3_exem_pdb_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ipmf3, dnx_data_field_base_ipmf3_define_exem_pdb);
}










static shr_error_e
dnx_data_field_base_epmf_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "base_epmf";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_base_epmf_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field base_epmf features");

    
    submodule_data->nof_defines = _dnx_data_field_base_epmf_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field base_epmf defines");

    submodule_data->defines[dnx_data_field_base_epmf_define_nof_ffc].name = "nof_ffc";
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_ffc].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_ffc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_nof_keys].name = "nof_keys";
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_keys].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_keys].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_nof_keys_alloc].name = "nof_keys_alloc";
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_keys_alloc].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_keys_alloc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_nof_keys_alloc_for_tcam].name = "nof_keys_alloc_for_tcam";
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_keys_alloc_for_tcam].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_keys_alloc_for_tcam].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_nof_keys_alloc_for_exem].name = "nof_keys_alloc_for_exem";
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_keys_alloc_for_exem].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_keys_alloc_for_exem].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_nof_keys_alloc_for_dir_ext].name = "nof_keys_alloc_for_dir_ext";
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_keys_alloc_for_dir_ext].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_keys_alloc_for_dir_ext].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_nof_masks_per_fes].name = "nof_masks_per_fes";
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_masks_per_fes].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_masks_per_fes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_nof_fes_id_per_array].name = "nof_fes_id_per_array";
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_fes_id_per_array].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_fes_id_per_array].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_nof_fes_array].name = "nof_fes_array";
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_fes_array].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_fes_array].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_nof_fes_instruction_per_context].name = "nof_fes_instruction_per_context";
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_fes_instruction_per_context].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_fes_instruction_per_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_nof_fes_programs].name = "nof_fes_programs";
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_fes_programs].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_fes_programs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_nof_prog_per_fes].name = "nof_prog_per_fes";
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_prog_per_fes].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_prog_per_fes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_nof_cs_lines].name = "nof_cs_lines";
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_cs_lines].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_cs_lines].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_nof_contexts].name = "nof_contexts";
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_contexts].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_contexts].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_nof_actions].name = "nof_actions";
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_actions].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_actions].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_nof_qualifiers].name = "nof_qualifiers";
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_qualifiers].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_qualifiers].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_nof_bits_in_fes_action].name = "nof_bits_in_fes_action";
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_bits_in_fes_action].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_bits_in_fes_action].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_nof_bits_in_fes_key_select].name = "nof_bits_in_fes_key_select";
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_bits_in_fes_key_select].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_bits_in_fes_key_select].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_fes_key_select_resolution_in_bits].name = "fes_key_select_resolution_in_bits";
    submodule_data->defines[dnx_data_field_base_epmf_define_fes_key_select_resolution_in_bits].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_fes_key_select_resolution_in_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_nof_l4_ops_ranges_legacy].name = "nof_l4_ops_ranges_legacy";
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_l4_ops_ranges_legacy].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_nof_l4_ops_ranges_legacy].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_fes_instruction_size].name = "fes_instruction_size";
    submodule_data->defines[dnx_data_field_base_epmf_define_fes_instruction_size].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_fes_instruction_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_fes_pgm_id_offset].name = "fes_pgm_id_offset";
    submodule_data->defines[dnx_data_field_base_epmf_define_fes_pgm_id_offset].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_fes_pgm_id_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_dir_ext_single_key_size].name = "dir_ext_single_key_size";
    submodule_data->defines[dnx_data_field_base_epmf_define_dir_ext_single_key_size].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_dir_ext_single_key_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_dir_ext_first_key].name = "dir_ext_first_key";
    submodule_data->defines[dnx_data_field_base_epmf_define_dir_ext_first_key].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_dir_ext_first_key].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_exem_max_result_size].name = "exem_max_result_size";
    submodule_data->defines[dnx_data_field_base_epmf_define_exem_max_result_size].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_exem_max_result_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_epmf_define_fes_key_select_for_zero_bit].name = "fes_key_select_for_zero_bit";
    submodule_data->defines[dnx_data_field_base_epmf_define_fes_key_select_for_zero_bit].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_fes_key_select_for_zero_bit].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_field_base_epmf_define_fes_shift_for_zero_bit].name = "fes_shift_for_zero_bit";
    submodule_data->defines[dnx_data_field_base_epmf_define_fes_shift_for_zero_bit].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_fes_shift_for_zero_bit].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_field_base_epmf_define_exem_pdb].name = "exem_pdb";
    submodule_data->defines[dnx_data_field_base_epmf_define_exem_pdb].doc = "";
    
    submodule_data->defines[dnx_data_field_base_epmf_define_exem_pdb].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_field_base_epmf_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field base_epmf tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_base_epmf_feature_get(
    int unit,
    dnx_data_field_base_epmf_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, feature);
}


uint32
dnx_data_field_base_epmf_nof_ffc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_nof_ffc);
}

uint32
dnx_data_field_base_epmf_nof_keys_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_nof_keys);
}

uint32
dnx_data_field_base_epmf_nof_keys_alloc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_nof_keys_alloc);
}

uint32
dnx_data_field_base_epmf_nof_keys_alloc_for_tcam_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_nof_keys_alloc_for_tcam);
}

uint32
dnx_data_field_base_epmf_nof_keys_alloc_for_exem_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_nof_keys_alloc_for_exem);
}

uint32
dnx_data_field_base_epmf_nof_keys_alloc_for_dir_ext_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_nof_keys_alloc_for_dir_ext);
}

uint32
dnx_data_field_base_epmf_nof_masks_per_fes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_nof_masks_per_fes);
}

uint32
dnx_data_field_base_epmf_nof_fes_id_per_array_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_nof_fes_id_per_array);
}

uint32
dnx_data_field_base_epmf_nof_fes_array_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_nof_fes_array);
}

uint32
dnx_data_field_base_epmf_nof_fes_instruction_per_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_nof_fes_instruction_per_context);
}

uint32
dnx_data_field_base_epmf_nof_fes_programs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_nof_fes_programs);
}

uint32
dnx_data_field_base_epmf_nof_prog_per_fes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_nof_prog_per_fes);
}

uint32
dnx_data_field_base_epmf_nof_cs_lines_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_nof_cs_lines);
}

uint32
dnx_data_field_base_epmf_nof_contexts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_nof_contexts);
}

uint32
dnx_data_field_base_epmf_nof_actions_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_nof_actions);
}

uint32
dnx_data_field_base_epmf_nof_qualifiers_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_nof_qualifiers);
}

uint32
dnx_data_field_base_epmf_nof_bits_in_fes_action_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_nof_bits_in_fes_action);
}

uint32
dnx_data_field_base_epmf_nof_bits_in_fes_key_select_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_nof_bits_in_fes_key_select);
}

uint32
dnx_data_field_base_epmf_fes_key_select_resolution_in_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_fes_key_select_resolution_in_bits);
}

uint32
dnx_data_field_base_epmf_nof_l4_ops_ranges_legacy_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_nof_l4_ops_ranges_legacy);
}

uint32
dnx_data_field_base_epmf_fes_instruction_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_fes_instruction_size);
}

uint32
dnx_data_field_base_epmf_fes_pgm_id_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_fes_pgm_id_offset);
}

uint32
dnx_data_field_base_epmf_dir_ext_single_key_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_dir_ext_single_key_size);
}

uint32
dnx_data_field_base_epmf_dir_ext_first_key_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_dir_ext_first_key);
}

uint32
dnx_data_field_base_epmf_exem_max_result_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_exem_max_result_size);
}

uint32
dnx_data_field_base_epmf_fes_key_select_for_zero_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_fes_key_select_for_zero_bit);
}

uint32
dnx_data_field_base_epmf_fes_shift_for_zero_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_fes_shift_for_zero_bit);
}

uint32
dnx_data_field_base_epmf_exem_pdb_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_epmf, dnx_data_field_base_epmf_define_exem_pdb);
}










static shr_error_e
dnx_data_field_base_ifwd2_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "base_ifwd2";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_base_ifwd2_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field base_ifwd2 features");

    
    submodule_data->nof_defines = _dnx_data_field_base_ifwd2_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field base_ifwd2 defines");

    submodule_data->defines[dnx_data_field_base_ifwd2_define_nof_ffc].name = "nof_ffc";
    submodule_data->defines[dnx_data_field_base_ifwd2_define_nof_ffc].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ifwd2_define_nof_ffc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ifwd2_define_nof_keys].name = "nof_keys";
    submodule_data->defines[dnx_data_field_base_ifwd2_define_nof_keys].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ifwd2_define_nof_keys].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ifwd2_define_nof_contexts].name = "nof_contexts";
    submodule_data->defines[dnx_data_field_base_ifwd2_define_nof_contexts].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ifwd2_define_nof_contexts].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_base_ifwd2_define_nof_cs_lines].name = "nof_cs_lines";
    submodule_data->defines[dnx_data_field_base_ifwd2_define_nof_cs_lines].doc = "";
    
    submodule_data->defines[dnx_data_field_base_ifwd2_define_nof_cs_lines].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_base_ifwd2_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field base_ifwd2 tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_base_ifwd2_feature_get(
    int unit,
    dnx_data_field_base_ifwd2_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ifwd2, feature);
}


uint32
dnx_data_field_base_ifwd2_nof_ffc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ifwd2, dnx_data_field_base_ifwd2_define_nof_ffc);
}

uint32
dnx_data_field_base_ifwd2_nof_keys_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ifwd2, dnx_data_field_base_ifwd2_define_nof_keys);
}

uint32
dnx_data_field_base_ifwd2_nof_contexts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ifwd2, dnx_data_field_base_ifwd2_define_nof_contexts);
}

uint32
dnx_data_field_base_ifwd2_nof_cs_lines_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_base_ifwd2, dnx_data_field_base_ifwd2_define_nof_cs_lines);
}










static shr_error_e
dnx_data_field_stage_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "stage";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_stage_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field stage features");

    
    submodule_data->nof_defines = _dnx_data_field_stage_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field stage defines");

    
    submodule_data->nof_tables = _dnx_data_field_stage_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field stage tables");

    
    submodule_data->tables[dnx_data_field_stage_table_stage_info].name = "stage_info";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].doc = "";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_field_stage_table_stage_info].size_of_values = sizeof(dnx_data_field_stage_stage_info_t);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].entry_get = dnx_data_field_stage_stage_info_entry_str_get;

    
    submodule_data->tables[dnx_data_field_stage_table_stage_info].nof_keys = 1;
    submodule_data->tables[dnx_data_field_stage_table_stage_info].keys[0].name = "stage";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_field_stage_table_stage_info].nof_values = 59;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_field_stage_table_stage_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_field_stage_table_stage_info].nof_values, "_dnx_data_field_stage_table_stage_info table values");
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[0].name = "valid";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[0].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[0].doc = "Whether the field_stage is supported on the device";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, valid);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[1].name = "nof_ffc";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[1].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[1].doc = "Number of FFC's per context";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_ffc);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[2].name = "nof_keys";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[2].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[2].doc = "Number of keys for the this PMF stage";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_keys);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[3].name = "nof_keys_alloc";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[3].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[3].doc = "Number of keys available for allocation for use by this specific PMF stage";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_keys_alloc);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[4].name = "nof_keys_alloc_for_tcam";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[4].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[4].doc = "Number of keys available for allocation for use by this specific PMF stage for TCAM access";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[4].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_keys_alloc_for_tcam);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[5].name = "nof_keys_alloc_for_exem";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[5].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[5].doc = "Number of keys available for allocation for use by this specific PMF stage for EXEM access";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[5].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_keys_alloc_for_exem);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[6].name = "nof_keys_alloc_for_dir_ext";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[6].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[6].doc = "Number of keys available for allocation for use by this specific PMF stage for DIRECT EXTRACTION";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[6].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_keys_alloc_for_dir_ext);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[7].name = "nof_keys_alloc_for_mdb_dt";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[7].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[7].doc = "Number of keys available for allocation for use by this specific PMF stage for Direct Table MDB access";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[7].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_keys_alloc_for_mdb_dt);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[8].name = "nof_masks_per_fes";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[8].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[8].doc = "Number of masks per FES";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[8].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_masks_per_fes);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[9].name = "nof_fes_id_per_array";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[9].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[9].doc = "Number FESes in each FES array";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[9].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_fes_id_per_array);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[10].name = "nof_fes_array";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[10].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[10].doc = "Number of FES arrays";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[10].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_fes_array);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[11].name = "nof_fes_instruction_per_context";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[11].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[11].doc = "Number of FES INSTRUCTIONs per context";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[11].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_fes_instruction_per_context);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[12].name = "nof_fes_programs";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[12].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[12].doc = "Number of FES programs";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[12].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_fes_programs);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[13].name = "nof_prog_per_fes";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[13].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[13].doc = "Number of MS bits instruction programs per FES";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[13].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_prog_per_fes);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[14].name = "cs_scratch_pad_size";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[14].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[14].doc = "Number of bits taken from general data for context selection.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[14].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, cs_scratch_pad_size);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[15].name = "cs_container_5_selected_bits_size";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[15].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[15].doc = "Number of bits taken from general data for context selection.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[15].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, cs_container_5_selected_bits_size);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[16].name = "nof_contexts";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[16].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[16].doc = "Number of Ingress PMF_A programs (contexts)";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[16].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_contexts);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[17].name = "nof_link_profiles";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[17].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[17].doc = "Number of profiles assigned to iPMF1 contexts in order to create links between iPMF1 and iPMF2 contexts";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[17].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_link_profiles);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[18].name = "nof_cs_lines";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[18].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[18].doc = "Number of program selection lines";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[18].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_cs_lines);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[19].name = "nof_actions";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[19].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[19].doc = "Number of actions";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[19].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_actions);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[20].name = "nof_qualifiers";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[20].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[20].doc = "Number of qualifiers";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[20].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_qualifiers);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[21].name = "nof_bits_in_fes_action";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[21].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[21].doc = "Number of bits on the output of a FES.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[21].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_bits_in_fes_action);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[22].name = "nof_bits_in_fes_key_select";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[22].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[22].doc = "Number of bits on input to FES.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[22].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_bits_in_fes_key_select);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[23].name = "fes_key_select_resolution_in_bits";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[23].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[23].doc = "Number of bits on input to FES.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[23].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, fes_key_select_resolution_in_bits);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[24].name = "fem_condition_ms_bit_min_value";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[24].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[24].doc = "The minumum legal value to be set in bit select for a FEM.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[24].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, fem_condition_ms_bit_min_value);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[25].name = "nof_fem_programs";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[25].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[25].doc = "Total number FEM programs available.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[25].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_fem_programs);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[26].name = "log_nof_bits_in_fem_key_select";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[26].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[26].doc = "Log2 of number of bits on one 'chunk' of input to FEM.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[26].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, log_nof_bits_in_fem_key_select);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[27].name = "nof_bits_in_fem_key_select";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[27].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[27].doc = "Number of bits on one 'chunk' of input to FEM.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[27].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_bits_in_fem_key_select);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[28].name = "fem_key_select_resolution_in_bits";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[28].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[28].doc = "Number of bits in the 'step' from one 'key select' to the following. See dbal_enum_value_field_field_pmf_a_fem_key_select_e.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[28].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, fem_key_select_resolution_in_bits);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[29].name = "log_nof_bits_in_fem_map_data_field";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[29].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[29].doc = "Log2 of number of bits on MAP_DATA field in IPPC_FEM_MAP_INDEX_TABLE (See FIELD_PMF_A_FEM_MAP_INDEX dbal table).";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[29].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, log_nof_bits_in_fem_map_data_field);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[30].name = "nof_bits_in_fem_map_data_field";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[30].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[30].doc = "Number of bits on MAP_DATA field in IPPC_FEM_MAP_INDEX_TABLE (See FIELD_PMF_A_FEM_MAP_INDEX dbal table).";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[30].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_bits_in_fem_map_data_field);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[31].name = "nof_bits_in_fem_action_fems_2_15";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[31].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[31].doc = "Number of bits on action value in IPPC_FEM_*_24B_MAP_TABLE.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[31].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_bits_in_fem_action_fems_2_15);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[32].name = "nof_bits_in_fem_action_fems_0_1";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[32].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[32].doc = "Number of bits on action value in IPPC_FEM_*_4B_MAP_TABLE.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[32].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_bits_in_fem_action_fems_0_1);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[33].name = "nof_bits_in_fem_action";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[33].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[33].doc = "Maximal number of bits on action value in IPPC_FEM_*_*_MAP_TABLE.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[33].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_bits_in_fem_action);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[34].name = "nof_fem_condition";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[34].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[34].doc = "Total number of fem conditions. This is the number of conditions that may be assigned to each (fem_id,fem_program) combination.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[34].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_fem_condition);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[35].name = "nof_fem_map_index";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[35].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[35].doc = "Total number fem map indices. This represents the number of actions that may be assigned to each condition.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[35].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_fem_map_index);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[36].name = "nof_fem_id";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[36].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[36].doc = "Total number 'FEM id's available.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[36].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_fem_id);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[37].name = "num_fems_with_short_action";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[37].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[37].doc = "Number of FEMs, starting from 'fem_id=0', which have only 4 bits on action value. The rest have 24 bits on action value";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[37].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, num_fems_with_short_action);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[38].name = "num_bits_in_fem_field_select";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[38].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[38].doc = "Number of bits on each of the HW field marked FIELD_SELECT_MAP_*";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[38].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, num_bits_in_fem_field_select);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[39].name = "nof_fem_id_per_array";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[39].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[39].doc = "Number of FEMs in each FEM array";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[39].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_fem_id_per_array);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[40].name = "nof_fem_array";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[40].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[40].doc = "Number of FEM arrays";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[40].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_fem_array);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[41].name = "pbus_header_length";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[41].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[41].doc = "Number of bits reserved in Ingress PBUS for header";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[41].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, pbus_header_length);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[42].name = "nof_layer_records";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[42].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[42].doc = "Number of layer records taken from the parser.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[42].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_layer_records);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[43].name = "layer_record_size";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[43].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[43].doc = "Number of bits in each layer record.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[43].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, layer_record_size);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[44].name = "nof_compare_pairs_in_compare_mode";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[44].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[44].doc = "Number compare pairs in compare mode";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[44].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_compare_pairs_in_compare_mode);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[45].name = "compare_key_size";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[45].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[45].doc = "Size of the compare key in the system";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[45].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, compare_key_size);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[46].name = "nof_l4_ops_ranges_legacy";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[46].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[46].doc = "Number of ranges for L4 Ops legacy";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[46].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_l4_ops_ranges_legacy);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[47].name = "nof_ext_l4_ops_ranges";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[47].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[47].doc = "Number of ranges for Extended L4 Ops";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[47].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_ext_l4_ops_ranges);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[48].name = "nof_pkt_hdr_ranges";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[48].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[48].doc = "Number of ranges for PKT HDR";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[48].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_pkt_hdr_ranges);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[49].name = "nof_out_lif_ranges";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[49].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[49].doc = "Number of ranges for Out Lif";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[49].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_out_lif_ranges);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[50].name = "fes_key_select_for_zero_bit";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[50].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[50].doc = "The key select used for obtaining an always zero bit for input to FES (for example, using a zero padded bit).";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[50].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, fes_key_select_for_zero_bit);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[51].name = "fes_shift_for_zero_bit";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[51].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[51].doc = "The shift used for obtaining an always zero bit for input to FES (for example, using a zero padded bit).";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[51].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, fes_shift_for_zero_bit);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[52].name = "cmp_selection";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[52].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[52].doc = "Compare selection for both keys of 2nd compare, See the CMP_SELECTION field in IPPC_PMF_GENERAL register for additional information";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[52].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, cmp_selection);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[53].name = "nof_bits_main_pbus";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[53].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[53].doc = "Number of bits in the main PBUS of the stage (the one used by METADATA qualifiers). Only iPMF2 uses the iPMF1 PBUs as it's main PBUS.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[53].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_bits_main_pbus);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[54].name = "nof_bits_native_pbus";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[54].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[54].doc = "Number of bits in the native PBUS of the stage, if it is different from the main PBUS. Only iPMF2 has a separate native PBUS.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[54].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, nof_bits_native_pbus);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[55].name = "dir_ext_single_key_size";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[55].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[55].doc = "Single key size used for Direct Extraction.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[55].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, dir_ext_single_key_size);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[56].name = "dir_ext_first_key";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[56].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[56].doc = "First key used for direct extraction.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[56].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, dir_ext_first_key);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[57].name = "exem_max_result_size";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[57].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[57].doc = "EXEM result (payload) size in bits, including VMV";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[57].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, exem_max_result_size);
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[58].name = "exem_pdb";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[58].type = "int";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[58].doc = "EXEM physical database type in the form of dbal_physical_tables_e.";
    submodule_data->tables[dnx_data_field_stage_table_stage_info].values[58].offset = UTILEX_OFFSETOF(dnx_data_field_stage_stage_info_t, exem_pdb);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_stage_feature_get(
    int unit,
    dnx_data_field_stage_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_stage, feature);
}




const dnx_data_field_stage_stage_info_t *
dnx_data_field_stage_stage_info_get(
    int unit,
    int stage)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_stage, dnx_data_field_stage_table_stage_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, stage, 0);
    return (const dnx_data_field_stage_stage_info_t *) data;

}


shr_error_e
dnx_data_field_stage_stage_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_field_stage_stage_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_stage, dnx_data_field_stage_table_stage_info);
    data = (const dnx_data_field_stage_stage_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_ffc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_keys);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_keys_alloc);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_keys_alloc_for_tcam);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_keys_alloc_for_exem);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_keys_alloc_for_dir_ext);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_keys_alloc_for_mdb_dt);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_masks_per_fes);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_fes_id_per_array);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_fes_array);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_fes_instruction_per_context);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_fes_programs);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_prog_per_fes);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->cs_scratch_pad_size);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->cs_container_5_selected_bits_size);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_contexts);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_link_profiles);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_cs_lines);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_actions);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_qualifiers);
            break;
        case 21:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_bits_in_fes_action);
            break;
        case 22:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_bits_in_fes_key_select);
            break;
        case 23:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fes_key_select_resolution_in_bits);
            break;
        case 24:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fem_condition_ms_bit_min_value);
            break;
        case 25:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_fem_programs);
            break;
        case 26:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->log_nof_bits_in_fem_key_select);
            break;
        case 27:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_bits_in_fem_key_select);
            break;
        case 28:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fem_key_select_resolution_in_bits);
            break;
        case 29:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->log_nof_bits_in_fem_map_data_field);
            break;
        case 30:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_bits_in_fem_map_data_field);
            break;
        case 31:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_bits_in_fem_action_fems_2_15);
            break;
        case 32:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_bits_in_fem_action_fems_0_1);
            break;
        case 33:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_bits_in_fem_action);
            break;
        case 34:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_fem_condition);
            break;
        case 35:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_fem_map_index);
            break;
        case 36:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_fem_id);
            break;
        case 37:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->num_fems_with_short_action);
            break;
        case 38:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->num_bits_in_fem_field_select);
            break;
        case 39:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_fem_id_per_array);
            break;
        case 40:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_fem_array);
            break;
        case 41:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pbus_header_length);
            break;
        case 42:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_layer_records);
            break;
        case 43:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->layer_record_size);
            break;
        case 44:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_compare_pairs_in_compare_mode);
            break;
        case 45:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->compare_key_size);
            break;
        case 46:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_l4_ops_ranges_legacy);
            break;
        case 47:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_ext_l4_ops_ranges);
            break;
        case 48:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_pkt_hdr_ranges);
            break;
        case 49:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_out_lif_ranges);
            break;
        case 50:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fes_key_select_for_zero_bit);
            break;
        case 51:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fes_shift_for_zero_bit);
            break;
        case 52:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->cmp_selection);
            break;
        case 53:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_bits_main_pbus);
            break;
        case 54:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_bits_native_pbus);
            break;
        case 55:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dir_ext_single_key_size);
            break;
        case 56:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dir_ext_first_key);
            break;
        case 57:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->exem_max_result_size);
            break;
        case 58:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->exem_pdb);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_field_stage_stage_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_field, dnx_data_field_submodule_stage, dnx_data_field_stage_table_stage_info);

}






static shr_error_e
dnx_data_field_external_tcam_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "external_tcam";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_external_tcam_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field external_tcam features");

    submodule_data->features[dnx_data_field_external_tcam_has_acl_context_profile_mapping].name = "has_acl_context_profile_mapping";
    submodule_data->features[dnx_data_field_external_tcam_has_acl_context_profile_mapping].doc = "";
    submodule_data->features[dnx_data_field_external_tcam_has_acl_context_profile_mapping].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_field_external_tcam_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field external_tcam defines");

    submodule_data->defines[dnx_data_field_external_tcam_define_max_single_key_size].name = "max_single_key_size";
    submodule_data->defines[dnx_data_field_external_tcam_define_max_single_key_size].doc = "";
    
    submodule_data->defines[dnx_data_field_external_tcam_define_max_single_key_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_external_tcam_define_nof_acl_keys_master_max].name = "nof_acl_keys_master_max";
    submodule_data->defines[dnx_data_field_external_tcam_define_nof_acl_keys_master_max].doc = "";
    
    submodule_data->defines[dnx_data_field_external_tcam_define_nof_acl_keys_master_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_external_tcam_define_nof_acl_keys_fg_max].name = "nof_acl_keys_fg_max";
    submodule_data->defines[dnx_data_field_external_tcam_define_nof_acl_keys_fg_max].doc = "";
    
    submodule_data->defines[dnx_data_field_external_tcam_define_nof_acl_keys_fg_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_external_tcam_define_nof_keys_total].name = "nof_keys_total";
    submodule_data->defines[dnx_data_field_external_tcam_define_nof_keys_total].doc = "";
    
    submodule_data->defines[dnx_data_field_external_tcam_define_nof_keys_total].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_external_tcam_define_min_acl_nof_ffc].name = "min_acl_nof_ffc";
    submodule_data->defines[dnx_data_field_external_tcam_define_min_acl_nof_ffc].doc = "";
    
    submodule_data->defines[dnx_data_field_external_tcam_define_min_acl_nof_ffc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_external_tcam_define_max_fwd_context_num_for_one_apptype].name = "max_fwd_context_num_for_one_apptype";
    submodule_data->defines[dnx_data_field_external_tcam_define_max_fwd_context_num_for_one_apptype].doc = "";
    
    submodule_data->defines[dnx_data_field_external_tcam_define_max_fwd_context_num_for_one_apptype].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_external_tcam_define_max_acl_context_num].name = "max_acl_context_num";
    submodule_data->defines[dnx_data_field_external_tcam_define_max_acl_context_num].doc = "";
    
    submodule_data->defines[dnx_data_field_external_tcam_define_max_acl_context_num].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_external_tcam_define_size_apptype_profile_id].name = "size_apptype_profile_id";
    submodule_data->defines[dnx_data_field_external_tcam_define_size_apptype_profile_id].doc = "";
    
    submodule_data->defines[dnx_data_field_external_tcam_define_size_apptype_profile_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_external_tcam_define_key_bmp].name = "key_bmp";
    submodule_data->defines[dnx_data_field_external_tcam_define_key_bmp].doc = "";
    
    submodule_data->defines[dnx_data_field_external_tcam_define_key_bmp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_external_tcam_define_apptype_user_1st].name = "apptype_user_1st";
    submodule_data->defines[dnx_data_field_external_tcam_define_apptype_user_1st].doc = "";
    
    submodule_data->defines[dnx_data_field_external_tcam_define_apptype_user_1st].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_external_tcam_define_apptype_user_nof].name = "apptype_user_nof";
    submodule_data->defines[dnx_data_field_external_tcam_define_apptype_user_nof].doc = "";
    
    submodule_data->defines[dnx_data_field_external_tcam_define_apptype_user_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_external_tcam_define_max_payload_size_per_opcode].name = "max_payload_size_per_opcode";
    submodule_data->defines[dnx_data_field_external_tcam_define_max_payload_size_per_opcode].doc = "";
    
    submodule_data->defines[dnx_data_field_external_tcam_define_max_payload_size_per_opcode].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_external_tcam_define_max_payload_size_per_opcode_epmf].name = "max_payload_size_per_opcode_epmf";
    submodule_data->defines[dnx_data_field_external_tcam_define_max_payload_size_per_opcode_epmf].doc = "";
    
    submodule_data->defines[dnx_data_field_external_tcam_define_max_payload_size_per_opcode_epmf].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_external_tcam_define_max_epmf_nof_lookups].name = "max_epmf_nof_lookups";
    submodule_data->defines[dnx_data_field_external_tcam_define_max_epmf_nof_lookups].doc = "";
    
    submodule_data->defines[dnx_data_field_external_tcam_define_max_epmf_nof_lookups].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_external_tcam_define_nof_total_lookups].name = "nof_total_lookups";
    submodule_data->defines[dnx_data_field_external_tcam_define_nof_total_lookups].doc = "";
    
    submodule_data->defines[dnx_data_field_external_tcam_define_nof_total_lookups].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_external_tcam_define_type].name = "type";
    submodule_data->defines[dnx_data_field_external_tcam_define_type].doc = "";
    
    submodule_data->defines[dnx_data_field_external_tcam_define_type].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_field_external_tcam_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field external_tcam tables");

    
    submodule_data->tables[dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map].name = "ffc_to_quad_and_group_map";
    submodule_data->tables[dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map].doc = "";
    submodule_data->tables[dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map].size_of_values = sizeof(dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t);
    submodule_data->tables[dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map].entry_get = dnx_data_field_external_tcam_ffc_to_quad_and_group_map_entry_str_get;

    
    submodule_data->tables[dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map].nof_keys = 1;
    submodule_data->tables[dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map].keys[0].name = "ffc_id";
    submodule_data->tables[dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map].nof_values, "_dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map table values");
    submodule_data->tables[dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map].values[0].name = "quad_id";
    submodule_data->tables[dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map].values[0].type = "uint32";
    submodule_data->tables[dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map].values[0].doc = "Id of the relevant quad for the given ffc id";
    submodule_data->tables[dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t, quad_id);
    submodule_data->tables[dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map].values[1].name = "group_id";
    submodule_data->tables[dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map].values[1].type = "uint32";
    submodule_data->tables[dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map].values[1].doc = "Id of the relevant group for the given ffc id";
    submodule_data->tables[dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map].values[1].offset = UTILEX_OFFSETOF(dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t, group_id);

    
    submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].name = "epmf_payload_sizes";
    submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].doc = "";
    submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].size_of_values = sizeof(dnx_data_field_external_tcam_epmf_payload_sizes_t);
    submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].entry_get = dnx_data_field_external_tcam_epmf_payload_sizes_entry_str_get;

    
    submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].nof_keys = 1;
    submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].keys[0].name = "result_id";
    submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].values, dnxc_data_value_t, submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].nof_values, "_dnx_data_field_external_tcam_table_epmf_payload_sizes table values");
    submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].values[0].name = "payload_large_size";
    submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].values[0].type = "int";
    submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].values[0].doc = "Max size of larger option.";
    submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].values[0].offset = UTILEX_OFFSETOF(dnx_data_field_external_tcam_epmf_payload_sizes_t, payload_large_size);
    submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].values[1].name = "payload_small_size";
    submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].values[1].type = "int";
    submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].values[1].doc = "Max size of smaller option. -1 if not applicable.";
    submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].values[1].offset = UTILEX_OFFSETOF(dnx_data_field_external_tcam_epmf_payload_sizes_t, payload_small_size);
    submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].values[2].name = "payload_first_bit";
    submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].values[2].type = "int";
    submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].values[2].doc = "First bit on result buffer.";
    submodule_data->tables[dnx_data_field_external_tcam_table_epmf_payload_sizes].values[2].offset = UTILEX_OFFSETOF(dnx_data_field_external_tcam_epmf_payload_sizes_t, payload_first_bit);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_external_tcam_feature_get(
    int unit,
    dnx_data_field_external_tcam_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, feature);
}


uint32
dnx_data_field_external_tcam_max_single_key_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_define_max_single_key_size);
}

uint32
dnx_data_field_external_tcam_nof_acl_keys_master_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_define_nof_acl_keys_master_max);
}

uint32
dnx_data_field_external_tcam_nof_acl_keys_fg_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_define_nof_acl_keys_fg_max);
}

uint32
dnx_data_field_external_tcam_nof_keys_total_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_define_nof_keys_total);
}

uint32
dnx_data_field_external_tcam_min_acl_nof_ffc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_define_min_acl_nof_ffc);
}

uint32
dnx_data_field_external_tcam_max_fwd_context_num_for_one_apptype_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_define_max_fwd_context_num_for_one_apptype);
}

uint32
dnx_data_field_external_tcam_max_acl_context_num_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_define_max_acl_context_num);
}

uint32
dnx_data_field_external_tcam_size_apptype_profile_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_define_size_apptype_profile_id);
}

uint32
dnx_data_field_external_tcam_key_bmp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_define_key_bmp);
}

uint32
dnx_data_field_external_tcam_apptype_user_1st_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_define_apptype_user_1st);
}

uint32
dnx_data_field_external_tcam_apptype_user_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_define_apptype_user_nof);
}

uint32
dnx_data_field_external_tcam_max_payload_size_per_opcode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_define_max_payload_size_per_opcode);
}

uint32
dnx_data_field_external_tcam_max_payload_size_per_opcode_epmf_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_define_max_payload_size_per_opcode_epmf);
}

uint32
dnx_data_field_external_tcam_max_epmf_nof_lookups_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_define_max_epmf_nof_lookups);
}

uint32
dnx_data_field_external_tcam_nof_total_lookups_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_define_nof_total_lookups);
}

uint32
dnx_data_field_external_tcam_type_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_define_type);
}



const dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *
dnx_data_field_external_tcam_ffc_to_quad_and_group_map_get(
    int unit,
    int ffc_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ffc_id, 0);
    return (const dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) data;

}

const dnx_data_field_external_tcam_epmf_payload_sizes_t *
dnx_data_field_external_tcam_epmf_payload_sizes_get(
    int unit,
    int result_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_table_epmf_payload_sizes);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, result_id, 0);
    return (const dnx_data_field_external_tcam_epmf_payload_sizes_t *) data;

}


shr_error_e
dnx_data_field_external_tcam_ffc_to_quad_and_group_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map);
    data = (const dnx_data_field_external_tcam_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->quad_id);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->group_id);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_field_external_tcam_epmf_payload_sizes_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_field_external_tcam_epmf_payload_sizes_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_table_epmf_payload_sizes);
    data = (const dnx_data_field_external_tcam_epmf_payload_sizes_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->payload_large_size);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->payload_small_size);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->payload_first_bit);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_field_external_tcam_ffc_to_quad_and_group_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_table_ffc_to_quad_and_group_map);

}

const dnxc_data_table_info_t *
dnx_data_field_external_tcam_epmf_payload_sizes_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_field, dnx_data_field_submodule_external_tcam, dnx_data_field_external_tcam_table_epmf_payload_sizes);

}






static shr_error_e
dnx_data_field_tcam_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "tcam";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_tcam_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field tcam features");

    submodule_data->features[dnx_data_field_tcam_tcam_80b_traffic_safe_write].name = "tcam_80b_traffic_safe_write";
    submodule_data->features[dnx_data_field_tcam_tcam_80b_traffic_safe_write].doc = "";
    submodule_data->features[dnx_data_field_tcam_tcam_80b_traffic_safe_write].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_field_tcam_mixed_ratio_supported].name = "mixed_ratio_supported";
    submodule_data->features[dnx_data_field_tcam_mixed_ratio_supported].doc = "";
    submodule_data->features[dnx_data_field_tcam_mixed_ratio_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_field_tcam_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field tcam defines");

    submodule_data->defines[dnx_data_field_tcam_define_access_hw_mix_ratio_supported].name = "access_hw_mix_ratio_supported";
    submodule_data->defines[dnx_data_field_tcam_define_access_hw_mix_ratio_supported].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_access_hw_mix_ratio_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_direct_supported].name = "direct_supported";
    submodule_data->defines[dnx_data_field_tcam_define_direct_supported].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_direct_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_key_size_half].name = "key_size_half";
    submodule_data->defines[dnx_data_field_tcam_define_key_size_half].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_key_size_half].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_key_size_single].name = "key_size_single";
    submodule_data->defines[dnx_data_field_tcam_define_key_size_single].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_key_size_single].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_key_size_double].name = "key_size_double";
    submodule_data->defines[dnx_data_field_tcam_define_key_size_double].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_key_size_double].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_dt_max_key_size].name = "dt_max_key_size";
    submodule_data->defines[dnx_data_field_tcam_define_dt_max_key_size].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_dt_max_key_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_dt_max_key_size_small_bank].name = "dt_max_key_size_small_bank";
    submodule_data->defines[dnx_data_field_tcam_define_dt_max_key_size_small_bank].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_dt_max_key_size_small_bank].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_action_size_half].name = "action_size_half";
    submodule_data->defines[dnx_data_field_tcam_define_action_size_half].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_action_size_half].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_action_size_single].name = "action_size_single";
    submodule_data->defines[dnx_data_field_tcam_define_action_size_single].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_action_size_single].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_action_size_double].name = "action_size_double";
    submodule_data->defines[dnx_data_field_tcam_define_action_size_double].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_action_size_double].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_key_mode_size].name = "key_mode_size";
    submodule_data->defines[dnx_data_field_tcam_define_key_mode_size].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_key_mode_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_entry_size_single_key_hw].name = "entry_size_single_key_hw";
    submodule_data->defines[dnx_data_field_tcam_define_entry_size_single_key_hw].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_entry_size_single_key_hw].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_entry_size_single_key_shadow].name = "entry_size_single_key_shadow";
    submodule_data->defines[dnx_data_field_tcam_define_entry_size_single_key_shadow].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_entry_size_single_key_shadow].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_entry_size_single_valid_bits_hw].name = "entry_size_single_valid_bits_hw";
    submodule_data->defines[dnx_data_field_tcam_define_entry_size_single_valid_bits_hw].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_entry_size_single_valid_bits_hw].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_hw_bank_size].name = "hw_bank_size";
    submodule_data->defines[dnx_data_field_tcam_define_hw_bank_size].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_hw_bank_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_nof_big_bank_lines].name = "nof_big_bank_lines";
    submodule_data->defines[dnx_data_field_tcam_define_nof_big_bank_lines].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_nof_big_bank_lines].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_nof_small_bank_lines].name = "nof_small_bank_lines";
    submodule_data->defines[dnx_data_field_tcam_define_nof_small_bank_lines].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_nof_small_bank_lines].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_nof_big_banks].name = "nof_big_banks";
    submodule_data->defines[dnx_data_field_tcam_define_nof_big_banks].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_nof_big_banks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_nof_small_banks].name = "nof_small_banks";
    submodule_data->defines[dnx_data_field_tcam_define_nof_small_banks].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_nof_small_banks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_nof_banks].name = "nof_banks";
    submodule_data->defines[dnx_data_field_tcam_define_nof_banks].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_nof_banks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_nof_payload_tables].name = "nof_payload_tables";
    submodule_data->defines[dnx_data_field_tcam_define_nof_payload_tables].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_nof_payload_tables].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_nof_access_profiles].name = "nof_access_profiles";
    submodule_data->defines[dnx_data_field_tcam_define_nof_access_profiles].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_nof_access_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_action_width_selector_size].name = "action_width_selector_size";
    submodule_data->defines[dnx_data_field_tcam_define_action_width_selector_size].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_action_width_selector_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_nof_entries_160_bits].name = "nof_entries_160_bits";
    submodule_data->defines[dnx_data_field_tcam_define_nof_entries_160_bits].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_nof_entries_160_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_nof_entries_80_bits].name = "nof_entries_80_bits";
    submodule_data->defines[dnx_data_field_tcam_define_nof_entries_80_bits].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_nof_entries_80_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_tcam_banks_size].name = "tcam_banks_size";
    submodule_data->defines[dnx_data_field_tcam_define_tcam_banks_size].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_tcam_banks_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_tcam_banks_last_index].name = "tcam_banks_last_index";
    submodule_data->defines[dnx_data_field_tcam_define_tcam_banks_last_index].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_tcam_banks_last_index].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_nof_tcam_handlers].name = "nof_tcam_handlers";
    submodule_data->defines[dnx_data_field_tcam_define_nof_tcam_handlers].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_nof_tcam_handlers].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_max_prefix_size].name = "max_prefix_size";
    submodule_data->defines[dnx_data_field_tcam_define_max_prefix_size].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_max_prefix_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_max_prefix_value].name = "max_prefix_value";
    submodule_data->defines[dnx_data_field_tcam_define_max_prefix_value].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_max_prefix_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_nof_keys_max].name = "nof_keys_max";
    submodule_data->defines[dnx_data_field_tcam_define_nof_keys_max].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_nof_keys_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_access_profile_half_key_mode].name = "access_profile_half_key_mode";
    submodule_data->defines[dnx_data_field_tcam_define_access_profile_half_key_mode].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_access_profile_half_key_mode].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_access_profile_single_key_mode].name = "access_profile_single_key_mode";
    submodule_data->defines[dnx_data_field_tcam_define_access_profile_single_key_mode].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_access_profile_single_key_mode].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_access_profile_double_key_mode].name = "access_profile_double_key_mode";
    submodule_data->defines[dnx_data_field_tcam_define_access_profile_double_key_mode].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_access_profile_double_key_mode].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_tcam_entries_per_hit_indication_entry].name = "tcam_entries_per_hit_indication_entry";
    submodule_data->defines[dnx_data_field_tcam_define_tcam_entries_per_hit_indication_entry].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_tcam_entries_per_hit_indication_entry].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_max_tcam_priority].name = "max_tcam_priority";
    submodule_data->defines[dnx_data_field_tcam_define_max_tcam_priority].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_max_tcam_priority].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_nof_big_banks_srams].name = "nof_big_banks_srams";
    submodule_data->defines[dnx_data_field_tcam_define_nof_big_banks_srams].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_nof_big_banks_srams].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_nof_small_banks_srams].name = "nof_small_banks_srams";
    submodule_data->defines[dnx_data_field_tcam_define_nof_small_banks_srams].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_nof_small_banks_srams].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_nof_big_bank_lines_per_sram].name = "nof_big_bank_lines_per_sram";
    submodule_data->defines[dnx_data_field_tcam_define_nof_big_bank_lines_per_sram].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_nof_big_bank_lines_per_sram].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_nof_small_bank_lines_per_sram].name = "nof_small_bank_lines_per_sram";
    submodule_data->defines[dnx_data_field_tcam_define_nof_small_bank_lines_per_sram].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_nof_small_bank_lines_per_sram].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_app_db_id_size].name = "app_db_id_size";
    submodule_data->defines[dnx_data_field_tcam_define_app_db_id_size].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_app_db_id_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tcam_define_tcam_indirect_command_version].name = "tcam_indirect_command_version";
    submodule_data->defines[dnx_data_field_tcam_define_tcam_indirect_command_version].doc = "";
    
    submodule_data->defines[dnx_data_field_tcam_define_tcam_indirect_command_version].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_tcam_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field tcam tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_tcam_feature_get(
    int unit,
    dnx_data_field_tcam_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, feature);
}


uint32
dnx_data_field_tcam_access_hw_mix_ratio_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_access_hw_mix_ratio_supported);
}

uint32
dnx_data_field_tcam_direct_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_direct_supported);
}

uint32
dnx_data_field_tcam_key_size_half_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_key_size_half);
}

uint32
dnx_data_field_tcam_key_size_single_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_key_size_single);
}

uint32
dnx_data_field_tcam_key_size_double_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_key_size_double);
}

uint32
dnx_data_field_tcam_dt_max_key_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_dt_max_key_size);
}

uint32
dnx_data_field_tcam_dt_max_key_size_small_bank_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_dt_max_key_size_small_bank);
}

uint32
dnx_data_field_tcam_action_size_half_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_action_size_half);
}

uint32
dnx_data_field_tcam_action_size_single_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_action_size_single);
}

uint32
dnx_data_field_tcam_action_size_double_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_action_size_double);
}

uint32
dnx_data_field_tcam_key_mode_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_key_mode_size);
}

uint32
dnx_data_field_tcam_entry_size_single_key_hw_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_entry_size_single_key_hw);
}

uint32
dnx_data_field_tcam_entry_size_single_key_shadow_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_entry_size_single_key_shadow);
}

uint32
dnx_data_field_tcam_entry_size_single_valid_bits_hw_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_entry_size_single_valid_bits_hw);
}

uint32
dnx_data_field_tcam_hw_bank_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_hw_bank_size);
}

uint32
dnx_data_field_tcam_nof_big_bank_lines_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_nof_big_bank_lines);
}

uint32
dnx_data_field_tcam_nof_small_bank_lines_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_nof_small_bank_lines);
}

uint32
dnx_data_field_tcam_nof_big_banks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_nof_big_banks);
}

uint32
dnx_data_field_tcam_nof_small_banks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_nof_small_banks);
}

uint32
dnx_data_field_tcam_nof_banks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_nof_banks);
}

uint32
dnx_data_field_tcam_nof_payload_tables_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_nof_payload_tables);
}

uint32
dnx_data_field_tcam_nof_access_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_nof_access_profiles);
}

uint32
dnx_data_field_tcam_action_width_selector_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_action_width_selector_size);
}

uint32
dnx_data_field_tcam_nof_entries_160_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_nof_entries_160_bits);
}

uint32
dnx_data_field_tcam_nof_entries_80_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_nof_entries_80_bits);
}

uint32
dnx_data_field_tcam_tcam_banks_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_tcam_banks_size);
}

uint32
dnx_data_field_tcam_tcam_banks_last_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_tcam_banks_last_index);
}

uint32
dnx_data_field_tcam_nof_tcam_handlers_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_nof_tcam_handlers);
}

uint32
dnx_data_field_tcam_max_prefix_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_max_prefix_size);
}

uint32
dnx_data_field_tcam_max_prefix_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_max_prefix_value);
}

uint32
dnx_data_field_tcam_nof_keys_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_nof_keys_max);
}

uint32
dnx_data_field_tcam_access_profile_half_key_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_access_profile_half_key_mode);
}

uint32
dnx_data_field_tcam_access_profile_single_key_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_access_profile_single_key_mode);
}

uint32
dnx_data_field_tcam_access_profile_double_key_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_access_profile_double_key_mode);
}

uint32
dnx_data_field_tcam_tcam_entries_per_hit_indication_entry_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_tcam_entries_per_hit_indication_entry);
}

uint32
dnx_data_field_tcam_max_tcam_priority_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_max_tcam_priority);
}

uint32
dnx_data_field_tcam_nof_big_banks_srams_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_nof_big_banks_srams);
}

uint32
dnx_data_field_tcam_nof_small_banks_srams_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_nof_small_banks_srams);
}

uint32
dnx_data_field_tcam_nof_big_bank_lines_per_sram_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_nof_big_bank_lines_per_sram);
}

uint32
dnx_data_field_tcam_nof_small_bank_lines_per_sram_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_nof_small_bank_lines_per_sram);
}

uint32
dnx_data_field_tcam_app_db_id_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_app_db_id_size);
}

uint32
dnx_data_field_tcam_tcam_indirect_command_version_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tcam, dnx_data_field_tcam_define_tcam_indirect_command_version);
}










static shr_error_e
dnx_data_field_group_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "group";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_group_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field group features");

    
    submodule_data->nof_defines = _dnx_data_field_group_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field group defines");

    submodule_data->defines[dnx_data_field_group_define_nof_fgs].name = "nof_fgs";
    submodule_data->defines[dnx_data_field_group_define_nof_fgs].doc = "";
    
    submodule_data->defines[dnx_data_field_group_define_nof_fgs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_group_define_nof_action_per_fg].name = "nof_action_per_fg";
    submodule_data->defines[dnx_data_field_group_define_nof_action_per_fg].doc = "";
    
    submodule_data->defines[dnx_data_field_group_define_nof_action_per_fg].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_group_define_nof_quals_per_fg].name = "nof_quals_per_fg";
    submodule_data->defines[dnx_data_field_group_define_nof_quals_per_fg].doc = "";
    
    submodule_data->defines[dnx_data_field_group_define_nof_quals_per_fg].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_group_define_nof_keys_per_fg_max].name = "nof_keys_per_fg_max";
    submodule_data->defines[dnx_data_field_group_define_nof_keys_per_fg_max].doc = "";
    
    submodule_data->defines[dnx_data_field_group_define_nof_keys_per_fg_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_group_define_id_fec].name = "id_fec";
    submodule_data->defines[dnx_data_field_group_define_id_fec].doc = "";
    
    submodule_data->defines[dnx_data_field_group_define_id_fec].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_group_define_payload_max_size].name = "payload_max_size";
    submodule_data->defines[dnx_data_field_group_define_payload_max_size].doc = "";
    
    submodule_data->defines[dnx_data_field_group_define_payload_max_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_group_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field group tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_group_feature_get(
    int unit,
    dnx_data_field_group_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_group, feature);
}


uint32
dnx_data_field_group_nof_fgs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_group, dnx_data_field_group_define_nof_fgs);
}

uint32
dnx_data_field_group_nof_action_per_fg_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_group, dnx_data_field_group_define_nof_action_per_fg);
}

uint32
dnx_data_field_group_nof_quals_per_fg_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_group, dnx_data_field_group_define_nof_quals_per_fg);
}

uint32
dnx_data_field_group_nof_keys_per_fg_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_group, dnx_data_field_group_define_nof_keys_per_fg_max);
}

uint32
dnx_data_field_group_id_fec_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_group, dnx_data_field_group_define_id_fec);
}

uint32
dnx_data_field_group_payload_max_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_group, dnx_data_field_group_define_payload_max_size);
}










static shr_error_e
dnx_data_field_efes_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "efes";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_efes_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field efes features");

    
    submodule_data->nof_defines = _dnx_data_field_efes_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field efes defines");

    submodule_data->defines[dnx_data_field_efes_define_max_nof_key_selects_per_field_io].name = "max_nof_key_selects_per_field_io";
    submodule_data->defines[dnx_data_field_efes_define_max_nof_key_selects_per_field_io].doc = "";
    
    submodule_data->defines[dnx_data_field_efes_define_max_nof_key_selects_per_field_io].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_efes_define_max_nof_field_ios].name = "max_nof_field_ios";
    submodule_data->defines[dnx_data_field_efes_define_max_nof_field_ios].doc = "";
    
    submodule_data->defines[dnx_data_field_efes_define_max_nof_field_ios].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_efes_define_epmf_exem_zero_padding_added_to_field_io].name = "epmf_exem_zero_padding_added_to_field_io";
    submodule_data->defines[dnx_data_field_efes_define_epmf_exem_zero_padding_added_to_field_io].doc = "";
    
    submodule_data->defines[dnx_data_field_efes_define_epmf_exem_zero_padding_added_to_field_io].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_efes_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field efes tables");

    
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].name = "key_select_properties";
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].doc = "";
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].size_of_values = sizeof(dnx_data_field_efes_key_select_properties_t);
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].entry_get = dnx_data_field_efes_key_select_properties_entry_str_get;

    
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].nof_keys = 2;
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].keys[0].name = "stage";
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].keys[0].doc = "";
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].keys[1].name = "field_io";
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_field_efes_table_key_select_properties].values, dnxc_data_value_t, submodule_data->tables[dnx_data_field_efes_table_key_select_properties].nof_values, "_dnx_data_field_efes_table_key_select_properties table values");
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].values[0].name = "key_select";
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].values[0].type = "uint8[DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO]";
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].values[0].doc = "Represents the DBAL key select enums of the different stages";
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].values[0].offset = UTILEX_OFFSETOF(dnx_data_field_efes_key_select_properties_t, key_select);
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].values[1].name = "num_bits";
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].values[1].type = "uint8[DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO]";
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].values[1].doc = "Number of bits on key select for field IO. If zero than entry is invalid.";
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].values[1].offset = UTILEX_OFFSETOF(dnx_data_field_efes_key_select_properties_t, num_bits);
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].values[2].name = "lsb";
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].values[2].type = "uint16[DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO]";
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].values[2].doc = "The offset on the field IO of the key select";
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].values[2].offset = UTILEX_OFFSETOF(dnx_data_field_efes_key_select_properties_t, lsb);
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].values[3].name = "num_bits_not_on_key";
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].values[3].type = "uint8[DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO]";
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].values[3].doc = "The number of LS bits on key select not on the field IO";
    submodule_data->tables[dnx_data_field_efes_table_key_select_properties].values[3].offset = UTILEX_OFFSETOF(dnx_data_field_efes_key_select_properties_t, num_bits_not_on_key);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_efes_feature_get(
    int unit,
    dnx_data_field_efes_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_efes, feature);
}


uint32
dnx_data_field_efes_max_nof_key_selects_per_field_io_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_efes, dnx_data_field_efes_define_max_nof_key_selects_per_field_io);
}

uint32
dnx_data_field_efes_max_nof_field_ios_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_efes, dnx_data_field_efes_define_max_nof_field_ios);
}

uint32
dnx_data_field_efes_epmf_exem_zero_padding_added_to_field_io_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_efes, dnx_data_field_efes_define_epmf_exem_zero_padding_added_to_field_io);
}



const dnx_data_field_efes_key_select_properties_t *
dnx_data_field_efes_key_select_properties_get(
    int unit,
    int stage,
    int field_io)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_efes, dnx_data_field_efes_table_key_select_properties);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, stage, field_io);
    return (const dnx_data_field_efes_key_select_properties_t *) data;

}


shr_error_e
dnx_data_field_efes_key_select_properties_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_field_efes_key_select_properties_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_efes, dnx_data_field_efes_table_key_select_properties);
    data = (const dnx_data_field_efes_key_select_properties_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, data->key_select);
            break;
        case 1:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, data->num_bits);
            break;
        case 2:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, data->lsb);
            break;
        case 3:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, data->num_bits_not_on_key);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_field_efes_key_select_properties_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_field, dnx_data_field_submodule_efes, dnx_data_field_efes_table_key_select_properties);

}






static shr_error_e
dnx_data_field_fem_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "fem";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_fem_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field fem features");

    
    submodule_data->nof_defines = _dnx_data_field_fem_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field fem defines");

    submodule_data->defines[dnx_data_field_fem_define_max_nof_key_selects_per_field_io].name = "max_nof_key_selects_per_field_io";
    submodule_data->defines[dnx_data_field_fem_define_max_nof_key_selects_per_field_io].doc = "";
    
    submodule_data->defines[dnx_data_field_fem_define_max_nof_key_selects_per_field_io].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_fem_define_nof_condition_bits].name = "nof_condition_bits";
    submodule_data->defines[dnx_data_field_fem_define_nof_condition_bits].doc = "";
    
    submodule_data->defines[dnx_data_field_fem_define_nof_condition_bits].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_field_fem_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field fem tables");

    
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].name = "key_select_properties";
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].doc = "";
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].size_of_values = sizeof(dnx_data_field_fem_key_select_properties_t);
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].entry_get = dnx_data_field_fem_key_select_properties_entry_str_get;

    
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].nof_keys = 1;
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].keys[0].name = "field_io";
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_field_fem_table_key_select_properties].values, dnxc_data_value_t, submodule_data->tables[dnx_data_field_fem_table_key_select_properties].nof_values, "_dnx_data_field_fem_table_key_select_properties table values");
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].values[0].name = "key_select";
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].values[0].type = "uint8[DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO]";
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].values[0].doc = "Represents the DBAL key select enums of the different stages";
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].values[0].offset = UTILEX_OFFSETOF(dnx_data_field_fem_key_select_properties_t, key_select);
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].values[1].name = "num_bits";
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].values[1].type = "uint8[DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO]";
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].values[1].doc = "Number of bits on key select for field IO. If zero than entry is invalid.";
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].values[1].offset = UTILEX_OFFSETOF(dnx_data_field_fem_key_select_properties_t, num_bits);
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].values[2].name = "lsb";
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].values[2].type = "uint16[DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO]";
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].values[2].doc = "The offset on the field IO of the key select";
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].values[2].offset = UTILEX_OFFSETOF(dnx_data_field_fem_key_select_properties_t, lsb);
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].values[3].name = "num_bits_not_on_key";
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].values[3].type = "uint8[DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO]";
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].values[3].doc = "The number of LS bits on key select not on the field IO";
    submodule_data->tables[dnx_data_field_fem_table_key_select_properties].values[3].offset = UTILEX_OFFSETOF(dnx_data_field_fem_key_select_properties_t, num_bits_not_on_key);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_fem_feature_get(
    int unit,
    dnx_data_field_fem_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_fem, feature);
}


uint32
dnx_data_field_fem_max_nof_key_selects_per_field_io_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_fem, dnx_data_field_fem_define_max_nof_key_selects_per_field_io);
}

uint32
dnx_data_field_fem_nof_condition_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_fem, dnx_data_field_fem_define_nof_condition_bits);
}



const dnx_data_field_fem_key_select_properties_t *
dnx_data_field_fem_key_select_properties_get(
    int unit,
    int field_io)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_fem, dnx_data_field_fem_table_key_select_properties);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, field_io, 0);
    return (const dnx_data_field_fem_key_select_properties_t *) data;

}


shr_error_e
dnx_data_field_fem_key_select_properties_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_field_fem_key_select_properties_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_fem, dnx_data_field_fem_table_key_select_properties);
    data = (const dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, data->key_select);
            break;
        case 1:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, data->num_bits);
            break;
        case 2:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, data->lsb);
            break;
        case 3:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO, data->num_bits_not_on_key);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_field_fem_key_select_properties_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_field, dnx_data_field_submodule_fem, dnx_data_field_fem_table_key_select_properties);

}






static shr_error_e
dnx_data_field_context_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "context";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_context_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field context features");

    
    submodule_data->nof_defines = _dnx_data_field_context_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field context defines");

    submodule_data->defines[dnx_data_field_context_define_default_context].name = "default_context";
    submodule_data->defines[dnx_data_field_context_define_default_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_ftmh_mc_context].name = "default_ftmh_mc_context";
    submodule_data->defines[dnx_data_field_context_define_default_ftmh_mc_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_ftmh_mc_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_itmh_context].name = "default_itmh_context";
    submodule_data->defines[dnx_data_field_context_define_default_itmh_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_itmh_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_itmh_pph_context].name = "default_itmh_pph_context";
    submodule_data->defines[dnx_data_field_context_define_default_itmh_pph_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_itmh_pph_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_itmh_pph_fhei_context].name = "default_itmh_pph_fhei_context";
    submodule_data->defines[dnx_data_field_context_define_default_itmh_pph_fhei_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_itmh_pph_fhei_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_itmh_1588_context].name = "default_itmh_1588_context";
    submodule_data->defines[dnx_data_field_context_define_default_itmh_1588_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_itmh_1588_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_itmh_1588_pph_context].name = "default_itmh_1588_pph_context";
    submodule_data->defines[dnx_data_field_context_define_default_itmh_1588_pph_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_itmh_1588_pph_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_itmh_1588_pph_fhei_context].name = "default_itmh_1588_pph_fhei_context";
    submodule_data->defines[dnx_data_field_context_define_default_itmh_1588_pph_fhei_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_itmh_1588_pph_fhei_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_j1_itmh_context].name = "default_j1_itmh_context";
    submodule_data->defines[dnx_data_field_context_define_default_j1_itmh_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_j1_itmh_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_j1_itmh_pph_context].name = "default_j1_itmh_pph_context";
    submodule_data->defines[dnx_data_field_context_define_default_j1_itmh_pph_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_j1_itmh_pph_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_oam_context].name = "default_oam_context";
    submodule_data->defines[dnx_data_field_context_define_default_oam_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_oam_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_oam_reflector_context].name = "default_oam_reflector_context";
    submodule_data->defines[dnx_data_field_context_define_default_oam_reflector_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_oam_reflector_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_oam_upmep_context].name = "default_oam_upmep_context";
    submodule_data->defines[dnx_data_field_context_define_default_oam_upmep_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_oam_upmep_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_j1_learning_2ndpass_context].name = "default_j1_learning_2ndpass_context";
    submodule_data->defines[dnx_data_field_context_define_default_j1_learning_2ndpass_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_j1_learning_2ndpass_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_rch_remove_context].name = "default_rch_remove_context";
    submodule_data->defines[dnx_data_field_context_define_default_rch_remove_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_rch_remove_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_rch_extended_encap_context].name = "default_rch_extended_encap_context";
    submodule_data->defines[dnx_data_field_context_define_default_rch_extended_encap_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_rch_extended_encap_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_nat_context].name = "default_nat_context";
    submodule_data->defines[dnx_data_field_context_define_default_nat_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_nat_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_nof_hash_keys].name = "nof_hash_keys";
    submodule_data->defines[dnx_data_field_context_define_nof_hash_keys].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_nof_hash_keys].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_itmh_pph_unsupported_ext_ipmf2_context].name = "default_itmh_pph_unsupported_ext_ipmf2_context";
    submodule_data->defines[dnx_data_field_context_define_default_itmh_pph_unsupported_ext_ipmf2_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_itmh_pph_unsupported_ext_ipmf2_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_itmh_pph_fhei_unsupported_ext_ipmf2_context].name = "default_itmh_pph_fhei_unsupported_ext_ipmf2_context";
    submodule_data->defines[dnx_data_field_context_define_default_itmh_pph_fhei_unsupported_ext_ipmf2_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_itmh_pph_fhei_unsupported_ext_ipmf2_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_itmh_pph_fhei_vlan_ipmf2_context].name = "default_itmh_pph_fhei_vlan_ipmf2_context";
    submodule_data->defines[dnx_data_field_context_define_default_itmh_pph_fhei_vlan_ipmf2_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_itmh_pph_fhei_vlan_ipmf2_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_itmh_1588_pph_fhei_unsupported_ext_ipmf2_context].name = "default_itmh_1588_pph_fhei_unsupported_ext_ipmf2_context";
    submodule_data->defines[dnx_data_field_context_define_default_itmh_1588_pph_fhei_unsupported_ext_ipmf2_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_itmh_1588_pph_fhei_unsupported_ext_ipmf2_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_itmh_1588_pph_unsupported_ext_ipmf2_context].name = "default_itmh_1588_pph_unsupported_ext_ipmf2_context";
    submodule_data->defines[dnx_data_field_context_define_default_itmh_1588_pph_unsupported_ext_ipmf2_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_itmh_1588_pph_unsupported_ext_ipmf2_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_context_define_default_itmh_1588_pph_fhei_vlan_ipmf2_context].name = "default_itmh_1588_pph_fhei_vlan_ipmf2_context";
    submodule_data->defines[dnx_data_field_context_define_default_itmh_1588_pph_fhei_vlan_ipmf2_context].doc = "";
    
    submodule_data->defines[dnx_data_field_context_define_default_itmh_1588_pph_fhei_vlan_ipmf2_context].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_context_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field context tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_context_feature_get(
    int unit,
    dnx_data_field_context_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, feature);
}


uint32
dnx_data_field_context_default_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_context);
}

uint32
dnx_data_field_context_default_ftmh_mc_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_ftmh_mc_context);
}

uint32
dnx_data_field_context_default_itmh_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_itmh_context);
}

uint32
dnx_data_field_context_default_itmh_pph_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_itmh_pph_context);
}

uint32
dnx_data_field_context_default_itmh_pph_fhei_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_itmh_pph_fhei_context);
}

uint32
dnx_data_field_context_default_itmh_1588_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_itmh_1588_context);
}

uint32
dnx_data_field_context_default_itmh_1588_pph_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_itmh_1588_pph_context);
}

uint32
dnx_data_field_context_default_itmh_1588_pph_fhei_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_itmh_1588_pph_fhei_context);
}

uint32
dnx_data_field_context_default_j1_itmh_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_j1_itmh_context);
}

uint32
dnx_data_field_context_default_j1_itmh_pph_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_j1_itmh_pph_context);
}

uint32
dnx_data_field_context_default_oam_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_oam_context);
}

uint32
dnx_data_field_context_default_oam_reflector_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_oam_reflector_context);
}

uint32
dnx_data_field_context_default_oam_upmep_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_oam_upmep_context);
}

uint32
dnx_data_field_context_default_j1_learning_2ndpass_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_j1_learning_2ndpass_context);
}

uint32
dnx_data_field_context_default_rch_remove_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_rch_remove_context);
}

uint32
dnx_data_field_context_default_rch_extended_encap_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_rch_extended_encap_context);
}

uint32
dnx_data_field_context_default_nat_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_nat_context);
}

uint32
dnx_data_field_context_nof_hash_keys_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_nof_hash_keys);
}

uint32
dnx_data_field_context_default_itmh_pph_unsupported_ext_ipmf2_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_itmh_pph_unsupported_ext_ipmf2_context);
}

uint32
dnx_data_field_context_default_itmh_pph_fhei_unsupported_ext_ipmf2_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_itmh_pph_fhei_unsupported_ext_ipmf2_context);
}

uint32
dnx_data_field_context_default_itmh_pph_fhei_vlan_ipmf2_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_itmh_pph_fhei_vlan_ipmf2_context);
}

uint32
dnx_data_field_context_default_itmh_1588_pph_fhei_unsupported_ext_ipmf2_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_itmh_1588_pph_fhei_unsupported_ext_ipmf2_context);
}

uint32
dnx_data_field_context_default_itmh_1588_pph_unsupported_ext_ipmf2_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_itmh_1588_pph_unsupported_ext_ipmf2_context);
}

uint32
dnx_data_field_context_default_itmh_1588_pph_fhei_vlan_ipmf2_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_context, dnx_data_field_context_define_default_itmh_1588_pph_fhei_vlan_ipmf2_context);
}










static shr_error_e
dnx_data_field_preselector_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "preselector";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_preselector_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field preselector features");

    
    submodule_data->nof_defines = _dnx_data_field_preselector_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field preselector defines");

    submodule_data->defines[dnx_data_field_preselector_define_default_j1_itmh_presel_id_ipmf1].name = "default_j1_itmh_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_itmh_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_itmh_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_j1_itmh_pph_presel_id_ipmf1].name = "default_j1_itmh_pph_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_itmh_pph_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_itmh_pph_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_j1_itmh_pph_oamp_presel_id_ipmf1].name = "default_j1_itmh_pph_oamp_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_itmh_pph_oamp_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_itmh_pph_oamp_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_oamp_presel_id_ipmf1].name = "default_itmh_pph_oamp_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_oamp_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_oamp_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_j1_learn_presel_id_1st_pass_ipmf1].name = "default_j1_learn_presel_id_1st_pass_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_learn_presel_id_1st_pass_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_learn_presel_id_1st_pass_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_j1_learn_presel_id_2nd_pass_ipmf1].name = "default_j1_learn_presel_id_2nd_pass_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_learn_presel_id_2nd_pass_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_learn_presel_id_2nd_pass_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_nat_presel_id_ipmf1].name = "default_nat_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_default_nat_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_nat_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_ftmh_mc_presel_id_ipmf1].name = "default_ftmh_mc_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_default_ftmh_mc_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_ftmh_mc_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_presel_id_ipmf1].name = "default_itmh_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_presel_id_ipmf1].name = "default_itmh_pph_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_fhei_presel_id_ipmf1].name = "default_itmh_pph_fhei_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_fhei_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_fhei_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_presel_id_ipmf1].name = "default_itmh_1588_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_presel_id_ipmf1].name = "default_itmh_1588_pph_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_presel_id_ipmf1].name = "default_itmh_1588_pph_fhei_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_psp_presel_id_ipmf1].name = "srv6_endpoint_psp_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_psp_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_psp_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_psp_extended_cut_presel_id_ipmf1].name = "srv6_endpoint_psp_extended_cut_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_psp_extended_cut_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_psp_extended_cut_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_psp_extended_cut_presel_id_ipmf2].name = "srv6_endpoint_psp_extended_cut_presel_id_ipmf2";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_psp_extended_cut_presel_id_ipmf2].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_psp_extended_cut_presel_id_ipmf2].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_presel_id_ipmf1].name = "srv6_endpoint_usp_extended_cut_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_trap_presel_id_ipmf1].name = "srv6_endpoint_usp_extended_cut_trap_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_trap_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_trap_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_icmp_presel_id_ipmf1].name = "srv6_endpoint_usp_extended_cut_icmp_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_icmp_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_icmp_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usd_eth_presel_id_ipmf1].name = "srv6_egress_usd_eth_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usd_eth_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usd_eth_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usd_ipv4_presel_id_ipmf1].name = "srv6_egress_usd_ipv4_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usd_ipv4_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usd_ipv4_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usd_ipv6_presel_id_ipmf1].name = "srv6_egress_usd_ipv6_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usd_ipv6_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usd_ipv6_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usd_mpls_presel_id_ipmf1].name = "srv6_egress_usd_mpls_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usd_mpls_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usd_mpls_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usid_presel_id_ipmf1].name = "srv6_endpoint_usid_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usid_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usid_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usid_no_default_presel_id_ipmf1].name = "srv6_endpoint_usid_no_default_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usid_no_default_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usid_no_default_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_gsid_presel_id_ipmf1].name = "srv6_endpoint_gsid_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_gsid_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_gsid_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usp_presel_id_ipmf1].name = "srv6_egress_usp_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usp_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usp_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usp_trap_presel_id_ipmf1].name = "srv6_egress_usp_trap_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usp_trap_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usp_trap_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usp_fwd_trap_presel_id_ipmf1].name = "srv6_egress_usp_fwd_trap_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usp_fwd_trap_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usp_fwd_trap_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usp_icmp_presel_id_ipmf1].name = "srv6_egress_usp_icmp_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usp_icmp_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usp_icmp_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_mpls_bier_presel_id_ipmf1].name = "mpls_bier_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_mpls_bier_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_mpls_bier_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_rcy_mirror_sys_hdr_presel_id_ipmf1].name = "default_rcy_mirror_sys_hdr_presel_id_ipmf1";
    submodule_data->defines[dnx_data_field_preselector_define_default_rcy_mirror_sys_hdr_presel_id_ipmf1].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_rcy_mirror_sys_hdr_presel_id_ipmf1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_unsupported_ext_presel_id_ipmf2].name = "default_itmh_pph_unsupported_ext_presel_id_ipmf2";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_unsupported_ext_presel_id_ipmf2].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_unsupported_ext_presel_id_ipmf2].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_fhei_unsupported_ext_presel_id_ipmf2].name = "default_itmh_pph_fhei_unsupported_ext_presel_id_ipmf2";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_fhei_unsupported_ext_presel_id_ipmf2].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_fhei_unsupported_ext_presel_id_ipmf2].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2].name = "default_itmh_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_fhei_trap_presel_id_ipmf2].name = "default_itmh_pph_fhei_trap_presel_id_ipmf2";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_fhei_trap_presel_id_ipmf2].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_fhei_trap_presel_id_ipmf2].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_fhei_vlan_presel_id_ipmf2].name = "default_itmh_pph_fhei_vlan_presel_id_ipmf2";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_fhei_vlan_presel_id_ipmf2].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_fhei_vlan_presel_id_ipmf2].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_unsupported_ext_presel_id_ipmf2].name = "default_itmh_1588_pph_unsupported_ext_presel_id_ipmf2";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_unsupported_ext_presel_id_ipmf2].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_unsupported_ext_presel_id_ipmf2].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_ipmf2].name = "default_itmh_1588_pph_fhei_unsupported_ext_presel_id_ipmf2";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_ipmf2].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_ipmf2].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2].name = "default_itmh_1588_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_trap_presel_id_ipmf2].name = "default_itmh_1588_pph_fhei_trap_presel_id_ipmf2";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_trap_presel_id_ipmf2].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_trap_presel_id_ipmf2].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_vlan_presel_id_ipmf2].name = "default_itmh_1588_pph_fhei_vlan_presel_id_ipmf2";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_vlan_presel_id_ipmf2].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_vlan_presel_id_ipmf2].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_presel_id_ipmf3].name = "default_itmh_pph_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_trap_presel_id_ipmf3].name = "default_itmh_pph_trap_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_trap_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_trap_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_vlan_presel_id_ipmf3].name = "default_itmh_pph_vlan_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_vlan_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_pph_vlan_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_presel_id_ipmf3].name = "default_itmh_1588_pph_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_trap_presel_id_ipmf3].name = "default_itmh_1588_pph_trap_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_trap_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_trap_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_vlan_presel_id_ipmf3].name = "default_itmh_1588_pph_vlan_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_vlan_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_itmh_1588_pph_vlan_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_j1_itmh_pph_presel_id_ipmf3].name = "default_j1_itmh_pph_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_itmh_pph_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_itmh_pph_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_ftmh_mc_presel_id_ipmf3].name = "default_ftmh_mc_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_default_ftmh_mc_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_ftmh_mc_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_oam_roo_ipv4_presel_id_ipmf3].name = "default_oam_roo_ipv4_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_default_oam_roo_ipv4_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_oam_roo_ipv4_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_oam_roo_ipv6_presel_id_ipmf3].name = "default_oam_roo_ipv6_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_default_oam_roo_ipv6_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_oam_roo_ipv6_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_oam_w_o_roo_no_rif_ipv4_presel_id_ipmf3].name = "default_oam_w_o_roo_no_rif_ipv4_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_default_oam_w_o_roo_no_rif_ipv4_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_oam_w_o_roo_no_rif_ipv4_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_oam_w_o_roo_no_rif_ipv6_presel_id_ipmf3].name = "default_oam_w_o_roo_no_rif_ipv6_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_default_oam_w_o_roo_no_rif_ipv6_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_oam_w_o_roo_no_rif_ipv6_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_oam_w_o_roo_with_rif_ipv4_presel_id_ipmf3].name = "default_oam_w_o_roo_with_rif_ipv4_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_default_oam_w_o_roo_with_rif_ipv4_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_oam_w_o_roo_with_rif_ipv4_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_oam_w_o_roo_with_rif_ipv6_presel_id_ipmf3].name = "default_oam_w_o_roo_with_rif_ipv6_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_default_oam_w_o_roo_with_rif_ipv6_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_oam_w_o_roo_with_rif_ipv6_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_j1_ipv4_mc_in_lif_presel_id_ipmf3].name = "default_j1_ipv4_mc_in_lif_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_ipv4_mc_in_lif_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_ipv4_mc_in_lif_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_j1_ipv6_mc_in_lif_presel_id_ipmf3].name = "default_j1_ipv6_mc_in_lif_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_ipv6_mc_in_lif_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_ipv6_mc_in_lif_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_rch_remove_presel_id_ipmf3].name = "default_rch_remove_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_default_rch_remove_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_rch_remove_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_rch_extended_encap_presel_id_ipmf3].name = "default_rch_extended_encap_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_default_rch_extended_encap_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_rch_extended_encap_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_j1_php_presel_id_ipmf3].name = "default_j1_php_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_php_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_php_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_j1_swap_presel_id_ipmf3].name = "default_j1_swap_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_swap_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_swap_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_partial_fwd_layer_presel_id_ipmf3].name = "srv6_partial_fwd_layer_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_partial_fwd_layer_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_partial_fwd_layer_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usid_presel_id_ipmf3].name = "srv6_endpoint_usid_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usid_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usid_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usid_no_default_presel_id_ipmf3].name = "srv6_endpoint_usid_no_default_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usid_no_default_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usid_no_default_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_gsid_presel_id_ipmf3].name = "srv6_endpoint_gsid_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_gsid_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_gsid_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_psp_presel_id_ipmf3].name = "srv6_endpoint_psp_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_psp_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_psp_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_psp_ext_to_endpoint_presel_id_ipmf3].name = "srv6_psp_ext_to_endpoint_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_psp_ext_to_endpoint_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_psp_ext_to_endpoint_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usp_presel_id_ipmf3].name = "srv6_egress_usp_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usp_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usp_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_psp_extended_cut_presel_id_ipmf3].name = "srv6_endpoint_psp_extended_cut_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_psp_extended_cut_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_psp_extended_cut_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_presel_id_ipmf3].name = "srv6_endpoint_usp_extended_cut_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usd_presel_id_ipmf3].name = "srv6_egress_usd_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usd_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_egress_usd_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_mpls_bier_presel_id_ipmf3].name = "mpls_bier_presel_id_ipmf3";
    submodule_data->defines[dnx_data_field_preselector_define_mpls_bier_presel_id_ipmf3].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_mpls_bier_presel_id_ipmf3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_ebtr_const_fwd_layer_eth_epmf].name = "ebtr_const_fwd_layer_eth_epmf";
    submodule_data->defines[dnx_data_field_preselector_define_ebtr_const_fwd_layer_eth_epmf].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_ebtr_const_fwd_layer_eth_epmf].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_ext_termination_btr_epmf].name = "srv6_ext_termination_btr_epmf";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_ext_termination_btr_epmf].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_ext_termination_btr_epmf].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_j1_same_port_presel_id_epmf].name = "default_j1_same_port_presel_id_epmf";
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_same_port_presel_id_epmf].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_j1_same_port_presel_id_epmf].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_learn_limit_presel_id_epmf].name = "default_learn_limit_presel_id_epmf";
    submodule_data->defines[dnx_data_field_preselector_define_default_learn_limit_presel_id_epmf].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_learn_limit_presel_id_epmf].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_hbh_bta_presel_id_epmf].name = "srv6_endpoint_hbh_bta_presel_id_epmf";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_hbh_bta_presel_id_epmf].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_hbh_bta_presel_id_epmf].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_transit_hbh_bta_presel_id_epmf].name = "srv6_transit_hbh_bta_presel_id_epmf";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_transit_hbh_bta_presel_id_epmf].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_transit_hbh_bta_presel_id_epmf].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_psp_bta_presel_id_epmf].name = "srv6_endpoint_psp_bta_presel_id_epmf";
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_psp_bta_presel_id_epmf].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_srv6_endpoint_psp_bta_presel_id_epmf].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_default_mpls_8b_fhei_presel_id_epmf].name = "default_mpls_8b_fhei_presel_id_epmf";
    submodule_data->defines[dnx_data_field_preselector_define_default_mpls_8b_fhei_presel_id_epmf].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_default_mpls_8b_fhei_presel_id_epmf].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_preselector_define_num_cs_inlif_profile_entries].name = "num_cs_inlif_profile_entries";
    submodule_data->defines[dnx_data_field_preselector_define_num_cs_inlif_profile_entries].doc = "";
    
    submodule_data->defines[dnx_data_field_preselector_define_num_cs_inlif_profile_entries].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_preselector_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field preselector tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_preselector_feature_get(
    int unit,
    dnx_data_field_preselector_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, feature);
}


uint32
dnx_data_field_preselector_default_j1_itmh_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_j1_itmh_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_default_j1_itmh_pph_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_j1_itmh_pph_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_default_j1_itmh_pph_oamp_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_j1_itmh_pph_oamp_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_default_itmh_pph_oamp_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_pph_oamp_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_default_j1_learn_presel_id_1st_pass_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_j1_learn_presel_id_1st_pass_ipmf1);
}

uint32
dnx_data_field_preselector_default_j1_learn_presel_id_2nd_pass_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_j1_learn_presel_id_2nd_pass_ipmf1);
}

uint32
dnx_data_field_preselector_default_nat_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_nat_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_default_ftmh_mc_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_ftmh_mc_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_default_itmh_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_default_itmh_pph_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_pph_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_default_itmh_pph_fhei_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_pph_fhei_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_default_itmh_1588_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_1588_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_default_itmh_1588_pph_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_1588_pph_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_default_itmh_1588_pph_fhei_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_srv6_endpoint_psp_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_endpoint_psp_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_srv6_endpoint_psp_extended_cut_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_endpoint_psp_extended_cut_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_srv6_endpoint_psp_extended_cut_presel_id_ipmf2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_endpoint_psp_extended_cut_presel_id_ipmf2);
}

uint32
dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_trap_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_trap_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_icmp_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_icmp_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_srv6_egress_usd_eth_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_egress_usd_eth_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_srv6_egress_usd_ipv4_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_egress_usd_ipv4_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_srv6_egress_usd_ipv6_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_egress_usd_ipv6_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_srv6_egress_usd_mpls_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_egress_usd_mpls_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_srv6_endpoint_usid_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_endpoint_usid_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_srv6_endpoint_usid_no_default_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_endpoint_usid_no_default_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_srv6_endpoint_gsid_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_endpoint_gsid_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_srv6_egress_usp_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_egress_usp_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_srv6_egress_usp_trap_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_egress_usp_trap_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_srv6_egress_usp_fwd_trap_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_egress_usp_fwd_trap_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_srv6_egress_usp_icmp_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_egress_usp_icmp_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_mpls_bier_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_mpls_bier_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_default_rcy_mirror_sys_hdr_presel_id_ipmf1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_rcy_mirror_sys_hdr_presel_id_ipmf1);
}

uint32
dnx_data_field_preselector_default_itmh_pph_unsupported_ext_presel_id_ipmf2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_pph_unsupported_ext_presel_id_ipmf2);
}

uint32
dnx_data_field_preselector_default_itmh_pph_fhei_unsupported_ext_presel_id_ipmf2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_pph_fhei_unsupported_ext_presel_id_ipmf2);
}

uint32
dnx_data_field_preselector_default_itmh_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2);
}

uint32
dnx_data_field_preselector_default_itmh_pph_fhei_trap_presel_id_ipmf2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_pph_fhei_trap_presel_id_ipmf2);
}

uint32
dnx_data_field_preselector_default_itmh_pph_fhei_vlan_presel_id_ipmf2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_pph_fhei_vlan_presel_id_ipmf2);
}

uint32
dnx_data_field_preselector_default_itmh_1588_pph_unsupported_ext_presel_id_ipmf2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_1588_pph_unsupported_ext_presel_id_ipmf2);
}

uint32
dnx_data_field_preselector_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_ipmf2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_ipmf2);
}

uint32
dnx_data_field_preselector_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2);
}

uint32
dnx_data_field_preselector_default_itmh_1588_pph_fhei_trap_presel_id_ipmf2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_trap_presel_id_ipmf2);
}

uint32
dnx_data_field_preselector_default_itmh_1588_pph_fhei_vlan_presel_id_ipmf2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_1588_pph_fhei_vlan_presel_id_ipmf2);
}

uint32
dnx_data_field_preselector_default_itmh_pph_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_pph_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_default_itmh_pph_trap_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_pph_trap_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_default_itmh_pph_vlan_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_pph_vlan_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_default_itmh_1588_pph_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_1588_pph_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_default_itmh_1588_pph_trap_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_1588_pph_trap_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_default_itmh_1588_pph_vlan_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_itmh_1588_pph_vlan_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_default_j1_itmh_pph_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_j1_itmh_pph_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_default_ftmh_mc_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_ftmh_mc_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_default_oam_roo_ipv4_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_oam_roo_ipv4_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_default_oam_roo_ipv6_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_oam_roo_ipv6_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_default_oam_w_o_roo_no_rif_ipv4_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_oam_w_o_roo_no_rif_ipv4_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_default_oam_w_o_roo_no_rif_ipv6_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_oam_w_o_roo_no_rif_ipv6_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_default_oam_w_o_roo_with_rif_ipv4_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_oam_w_o_roo_with_rif_ipv4_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_default_oam_w_o_roo_with_rif_ipv6_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_oam_w_o_roo_with_rif_ipv6_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_default_j1_ipv4_mc_in_lif_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_j1_ipv4_mc_in_lif_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_default_j1_ipv6_mc_in_lif_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_j1_ipv6_mc_in_lif_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_default_rch_remove_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_rch_remove_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_default_rch_extended_encap_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_rch_extended_encap_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_default_j1_php_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_j1_php_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_default_j1_swap_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_j1_swap_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_srv6_partial_fwd_layer_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_partial_fwd_layer_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_srv6_endpoint_usid_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_endpoint_usid_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_srv6_endpoint_usid_no_default_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_endpoint_usid_no_default_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_srv6_endpoint_gsid_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_endpoint_gsid_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_srv6_endpoint_psp_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_endpoint_psp_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_srv6_psp_ext_to_endpoint_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_psp_ext_to_endpoint_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_srv6_egress_usp_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_egress_usp_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_srv6_endpoint_psp_extended_cut_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_endpoint_psp_extended_cut_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_endpoint_usp_extended_cut_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_srv6_egress_usd_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_egress_usd_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_mpls_bier_presel_id_ipmf3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_mpls_bier_presel_id_ipmf3);
}

uint32
dnx_data_field_preselector_ebtr_const_fwd_layer_eth_epmf_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_ebtr_const_fwd_layer_eth_epmf);
}

uint32
dnx_data_field_preselector_srv6_ext_termination_btr_epmf_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_ext_termination_btr_epmf);
}

uint32
dnx_data_field_preselector_default_j1_same_port_presel_id_epmf_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_j1_same_port_presel_id_epmf);
}

uint32
dnx_data_field_preselector_default_learn_limit_presel_id_epmf_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_learn_limit_presel_id_epmf);
}

uint32
dnx_data_field_preselector_srv6_endpoint_hbh_bta_presel_id_epmf_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_endpoint_hbh_bta_presel_id_epmf);
}

uint32
dnx_data_field_preselector_srv6_transit_hbh_bta_presel_id_epmf_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_transit_hbh_bta_presel_id_epmf);
}

uint32
dnx_data_field_preselector_srv6_endpoint_psp_bta_presel_id_epmf_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_srv6_endpoint_psp_bta_presel_id_epmf);
}

uint32
dnx_data_field_preselector_default_mpls_8b_fhei_presel_id_epmf_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_default_mpls_8b_fhei_presel_id_epmf);
}

uint32
dnx_data_field_preselector_num_cs_inlif_profile_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_preselector, dnx_data_field_preselector_define_num_cs_inlif_profile_entries);
}










static shr_error_e
dnx_data_field_qual_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "qual";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_qual_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field qual features");

    
    submodule_data->nof_defines = _dnx_data_field_qual_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field qual defines");

    submodule_data->defines[dnx_data_field_qual_define_user_1st].name = "user_1st";
    submodule_data->defines[dnx_data_field_qual_define_user_1st].doc = "";
    
    submodule_data->defines[dnx_data_field_qual_define_user_1st].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_qual_define_user_nof].name = "user_nof";
    submodule_data->defines[dnx_data_field_qual_define_user_nof].doc = "";
    
    submodule_data->defines[dnx_data_field_qual_define_user_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_qual_define_predefined_nof].name = "predefined_nof";
    submodule_data->defines[dnx_data_field_qual_define_predefined_nof].doc = "";
    
    submodule_data->defines[dnx_data_field_qual_define_predefined_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_qual_define_layer_record_nof].name = "layer_record_nof";
    submodule_data->defines[dnx_data_field_qual_define_layer_record_nof].doc = "";
    
    submodule_data->defines[dnx_data_field_qual_define_layer_record_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_qual_define_vw_1st].name = "vw_1st";
    submodule_data->defines[dnx_data_field_qual_define_vw_1st].doc = "";
    
    submodule_data->defines[dnx_data_field_qual_define_vw_1st].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_qual_define_vw_nof].name = "vw_nof";
    submodule_data->defines[dnx_data_field_qual_define_vw_nof].doc = "";
    
    submodule_data->defines[dnx_data_field_qual_define_vw_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_qual_define_max_bits_in_qual].name = "max_bits_in_qual";
    submodule_data->defines[dnx_data_field_qual_define_max_bits_in_qual].doc = "";
    
    submodule_data->defines[dnx_data_field_qual_define_max_bits_in_qual].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_qual_define_ingress_pbus_header_length].name = "ingress_pbus_header_length";
    submodule_data->defines[dnx_data_field_qual_define_ingress_pbus_header_length].doc = "";
    
    submodule_data->defines[dnx_data_field_qual_define_ingress_pbus_header_length].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_qual_define_egress_pbus_header_length].name = "egress_pbus_header_length";
    submodule_data->defines[dnx_data_field_qual_define_egress_pbus_header_length].doc = "";
    
    submodule_data->defines[dnx_data_field_qual_define_egress_pbus_header_length].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_qual_define_ifwd2_pbus_size].name = "ifwd2_pbus_size";
    submodule_data->defines[dnx_data_field_qual_define_ifwd2_pbus_size].doc = "";
    
    submodule_data->defines[dnx_data_field_qual_define_ifwd2_pbus_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_qual_define_ipmf1_pbus_size].name = "ipmf1_pbus_size";
    submodule_data->defines[dnx_data_field_qual_define_ipmf1_pbus_size].doc = "";
    
    submodule_data->defines[dnx_data_field_qual_define_ipmf1_pbus_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_qual_define_ipmf2_pbus_size].name = "ipmf2_pbus_size";
    submodule_data->defines[dnx_data_field_qual_define_ipmf2_pbus_size].doc = "";
    
    submodule_data->defines[dnx_data_field_qual_define_ipmf2_pbus_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_qual_define_ipmf3_pbus_size].name = "ipmf3_pbus_size";
    submodule_data->defines[dnx_data_field_qual_define_ipmf3_pbus_size].doc = "";
    
    submodule_data->defines[dnx_data_field_qual_define_ipmf3_pbus_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_qual_define_epmf_pbus_size].name = "epmf_pbus_size";
    submodule_data->defines[dnx_data_field_qual_define_epmf_pbus_size].doc = "";
    
    submodule_data->defines[dnx_data_field_qual_define_epmf_pbus_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_qual_define_l4_ops_pbus_size].name = "l4_ops_pbus_size";
    submodule_data->defines[dnx_data_field_qual_define_l4_ops_pbus_size].doc = "";
    
    submodule_data->defines[dnx_data_field_qual_define_l4_ops_pbus_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_qual_define_ingress_nof_layer_records].name = "ingress_nof_layer_records";
    submodule_data->defines[dnx_data_field_qual_define_ingress_nof_layer_records].doc = "";
    
    submodule_data->defines[dnx_data_field_qual_define_ingress_nof_layer_records].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_qual_define_ingress_layer_record_size].name = "ingress_layer_record_size";
    submodule_data->defines[dnx_data_field_qual_define_ingress_layer_record_size].doc = "";
    
    submodule_data->defines[dnx_data_field_qual_define_ingress_layer_record_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_qual_define_egress_nof_layer_records].name = "egress_nof_layer_records";
    submodule_data->defines[dnx_data_field_qual_define_egress_nof_layer_records].doc = "";
    
    submodule_data->defines[dnx_data_field_qual_define_egress_nof_layer_records].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_qual_define_egress_layer_record_size].name = "egress_layer_record_size";
    submodule_data->defines[dnx_data_field_qual_define_egress_layer_record_size].doc = "";
    
    submodule_data->defines[dnx_data_field_qual_define_egress_layer_record_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_qual_define_kbp_extra_offset_after_layer_record_offset].name = "kbp_extra_offset_after_layer_record_offset";
    submodule_data->defines[dnx_data_field_qual_define_kbp_extra_offset_after_layer_record_offset].doc = "";
    
    submodule_data->defines[dnx_data_field_qual_define_kbp_extra_offset_after_layer_record_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_qual_define_special_metadata_qual_max_parts].name = "special_metadata_qual_max_parts";
    submodule_data->defines[dnx_data_field_qual_define_special_metadata_qual_max_parts].doc = "";
    
    submodule_data->defines[dnx_data_field_qual_define_special_metadata_qual_max_parts].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_qual_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field qual tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_qual_feature_get(
    int unit,
    dnx_data_field_qual_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, feature);
}


uint32
dnx_data_field_qual_user_1st_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, dnx_data_field_qual_define_user_1st);
}

uint32
dnx_data_field_qual_user_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, dnx_data_field_qual_define_user_nof);
}

uint32
dnx_data_field_qual_predefined_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, dnx_data_field_qual_define_predefined_nof);
}

uint32
dnx_data_field_qual_layer_record_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, dnx_data_field_qual_define_layer_record_nof);
}

uint32
dnx_data_field_qual_vw_1st_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, dnx_data_field_qual_define_vw_1st);
}

uint32
dnx_data_field_qual_vw_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, dnx_data_field_qual_define_vw_nof);
}

uint32
dnx_data_field_qual_max_bits_in_qual_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, dnx_data_field_qual_define_max_bits_in_qual);
}

uint32
dnx_data_field_qual_ingress_pbus_header_length_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, dnx_data_field_qual_define_ingress_pbus_header_length);
}

uint32
dnx_data_field_qual_egress_pbus_header_length_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, dnx_data_field_qual_define_egress_pbus_header_length);
}

uint32
dnx_data_field_qual_ifwd2_pbus_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, dnx_data_field_qual_define_ifwd2_pbus_size);
}

uint32
dnx_data_field_qual_ipmf1_pbus_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, dnx_data_field_qual_define_ipmf1_pbus_size);
}

uint32
dnx_data_field_qual_ipmf2_pbus_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, dnx_data_field_qual_define_ipmf2_pbus_size);
}

uint32
dnx_data_field_qual_ipmf3_pbus_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, dnx_data_field_qual_define_ipmf3_pbus_size);
}

uint32
dnx_data_field_qual_epmf_pbus_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, dnx_data_field_qual_define_epmf_pbus_size);
}

uint32
dnx_data_field_qual_l4_ops_pbus_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, dnx_data_field_qual_define_l4_ops_pbus_size);
}

uint32
dnx_data_field_qual_ingress_nof_layer_records_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, dnx_data_field_qual_define_ingress_nof_layer_records);
}

uint32
dnx_data_field_qual_ingress_layer_record_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, dnx_data_field_qual_define_ingress_layer_record_size);
}

uint32
dnx_data_field_qual_egress_nof_layer_records_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, dnx_data_field_qual_define_egress_nof_layer_records);
}

uint32
dnx_data_field_qual_egress_layer_record_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, dnx_data_field_qual_define_egress_layer_record_size);
}

uint32
dnx_data_field_qual_kbp_extra_offset_after_layer_record_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, dnx_data_field_qual_define_kbp_extra_offset_after_layer_record_offset);
}

uint32
dnx_data_field_qual_special_metadata_qual_max_parts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_qual, dnx_data_field_qual_define_special_metadata_qual_max_parts);
}










static shr_error_e
dnx_data_field_action_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "action";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_action_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field action features");

    
    submodule_data->nof_defines = _dnx_data_field_action_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field action defines");

    submodule_data->defines[dnx_data_field_action_define_user_1st].name = "user_1st";
    submodule_data->defines[dnx_data_field_action_define_user_1st].doc = "";
    
    submodule_data->defines[dnx_data_field_action_define_user_1st].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_action_define_user_nof].name = "user_nof";
    submodule_data->defines[dnx_data_field_action_define_user_nof].doc = "";
    
    submodule_data->defines[dnx_data_field_action_define_user_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_action_define_predefined_nof].name = "predefined_nof";
    submodule_data->defines[dnx_data_field_action_define_predefined_nof].doc = "";
    
    submodule_data->defines[dnx_data_field_action_define_predefined_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_action_define_vw_1st].name = "vw_1st";
    submodule_data->defines[dnx_data_field_action_define_vw_1st].doc = "";
    
    submodule_data->defines[dnx_data_field_action_define_vw_1st].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_action_define_vw_nof].name = "vw_nof";
    submodule_data->defines[dnx_data_field_action_define_vw_nof].doc = "";
    
    submodule_data->defines[dnx_data_field_action_define_vw_nof].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_action_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field action tables");

    
    submodule_data->tables[dnx_data_field_action_table_params].name = "params";
    submodule_data->tables[dnx_data_field_action_table_params].doc = "";
    submodule_data->tables[dnx_data_field_action_table_params].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_field_action_table_params].size_of_values = sizeof(dnx_data_field_action_params_t);
    submodule_data->tables[dnx_data_field_action_table_params].entry_get = dnx_data_field_action_params_entry_str_get;

    
    submodule_data->tables[dnx_data_field_action_table_params].nof_keys = 2;
    submodule_data->tables[dnx_data_field_action_table_params].keys[0].name = "stage";
    submodule_data->tables[dnx_data_field_action_table_params].keys[0].doc = "";
    submodule_data->tables[dnx_data_field_action_table_params].keys[1].name = "action";
    submodule_data->tables[dnx_data_field_action_table_params].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_field_action_table_params].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_field_action_table_params].values, dnxc_data_value_t, submodule_data->tables[dnx_data_field_action_table_params].nof_values, "_dnx_data_field_action_table_params table values");
    submodule_data->tables[dnx_data_field_action_table_params].values[0].name = "id";
    submodule_data->tables[dnx_data_field_action_table_params].values[0].type = "int";
    submodule_data->tables[dnx_data_field_action_table_params].values[0].doc = "DBAL Enum ID";
    submodule_data->tables[dnx_data_field_action_table_params].values[0].offset = UTILEX_OFFSETOF(dnx_data_field_action_params_t, id);
    submodule_data->tables[dnx_data_field_action_table_params].values[1].name = "size";
    submodule_data->tables[dnx_data_field_action_table_params].values[1].type = "int";
    submodule_data->tables[dnx_data_field_action_table_params].values[1].doc = "action size";
    submodule_data->tables[dnx_data_field_action_table_params].values[1].offset = UTILEX_OFFSETOF(dnx_data_field_action_params_t, size);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_action_feature_get(
    int unit,
    dnx_data_field_action_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_action, feature);
}


uint32
dnx_data_field_action_user_1st_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_action, dnx_data_field_action_define_user_1st);
}

uint32
dnx_data_field_action_user_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_action, dnx_data_field_action_define_user_nof);
}

uint32
dnx_data_field_action_predefined_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_action, dnx_data_field_action_define_predefined_nof);
}

uint32
dnx_data_field_action_vw_1st_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_action, dnx_data_field_action_define_vw_1st);
}

uint32
dnx_data_field_action_vw_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_action, dnx_data_field_action_define_vw_nof);
}



const dnx_data_field_action_params_t *
dnx_data_field_action_params_get(
    int unit,
    int stage,
    int action)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_action, dnx_data_field_action_table_params);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, stage, action);
    return (const dnx_data_field_action_params_t *) data;

}


shr_error_e
dnx_data_field_action_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_field_action_params_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_action, dnx_data_field_action_table_params);
    data = (const dnx_data_field_action_params_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->id);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->size);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_field_action_params_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_field, dnx_data_field_submodule_action, dnx_data_field_action_table_params);

}






static shr_error_e
dnx_data_field_virtual_wire_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "virtual_wire";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_virtual_wire_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field virtual_wire features");

    submodule_data->features[dnx_data_field_virtual_wire_in_lif_ac_wide_is_vw].name = "in_lif_ac_wide_is_vw";
    submodule_data->features[dnx_data_field_virtual_wire_in_lif_ac_wide_is_vw].doc = "";
    submodule_data->features[dnx_data_field_virtual_wire_in_lif_ac_wide_is_vw].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_field_virtual_wire_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field virtual_wire defines");

    submodule_data->defines[dnx_data_field_virtual_wire_define_signals_nof].name = "signals_nof";
    submodule_data->defines[dnx_data_field_virtual_wire_define_signals_nof].doc = "";
    
    submodule_data->defines[dnx_data_field_virtual_wire_define_signals_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_virtual_wire_define_actions_per_signal_nof].name = "actions_per_signal_nof";
    submodule_data->defines[dnx_data_field_virtual_wire_define_actions_per_signal_nof].doc = "";
    
    submodule_data->defines[dnx_data_field_virtual_wire_define_actions_per_signal_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_virtual_wire_define_general_data_user_general_containers_size].name = "general_data_user_general_containers_size";
    submodule_data->defines[dnx_data_field_virtual_wire_define_general_data_user_general_containers_size].doc = "";
    
    submodule_data->defines[dnx_data_field_virtual_wire_define_general_data_user_general_containers_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_virtual_wire_define_ipmf1_general_data_index].name = "ipmf1_general_data_index";
    submodule_data->defines[dnx_data_field_virtual_wire_define_ipmf1_general_data_index].doc = "";
    
    submodule_data->defines[dnx_data_field_virtual_wire_define_ipmf1_general_data_index].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_virtual_wire_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field virtual_wire tables");

    
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].name = "signal_mapping";
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].doc = "";
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].size_of_values = sizeof(dnx_data_field_virtual_wire_signal_mapping_t);
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].entry_get = dnx_data_field_virtual_wire_signal_mapping_entry_str_get;

    
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].nof_keys = 2;
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].keys[0].name = "stage";
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].keys[0].doc = "";
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].keys[1].name = "signal_id";
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].nof_values = 5;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].values, dnxc_data_value_t, submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].nof_values, "_dnx_data_field_virtual_wire_table_signal_mapping table values");
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].values[0].name = "signal_name";
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].values[0].type = "char *";
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].values[0].doc = "Name of the signal.";
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].values[0].offset = UTILEX_OFFSETOF(dnx_data_field_virtual_wire_signal_mapping_t, signal_name);
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].values[1].name = "mapped_qual";
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].values[1].type = "int";
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].values[1].doc = "Standard 1 qualifier that correlates to the signal";
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].values[1].offset = UTILEX_OFFSETOF(dnx_data_field_virtual_wire_signal_mapping_t, mapped_qual);
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].values[2].name = "offset_from_qual";
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].values[2].type = "int";
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].values[2].doc = "Offset on from the start of the standard 1 qualifier";
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].values[2].offset = UTILEX_OFFSETOF(dnx_data_field_virtual_wire_signal_mapping_t, offset_from_qual);
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].values[3].name = "mapped_action";
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].values[3].type = "int[DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF]";
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].values[3].doc = "Standard 1 action that correlates to the signal";
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].values[3].offset = UTILEX_OFFSETOF(dnx_data_field_virtual_wire_signal_mapping_t, mapped_action);
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].values[4].name = "action_offset";
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].values[4].type = "int[DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF]";
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].values[4].doc = "Offset on from the start of the signal for the action";
    submodule_data->tables[dnx_data_field_virtual_wire_table_signal_mapping].values[4].offset = UTILEX_OFFSETOF(dnx_data_field_virtual_wire_signal_mapping_t, action_offset);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_virtual_wire_feature_get(
    int unit,
    dnx_data_field_virtual_wire_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_virtual_wire, feature);
}


uint32
dnx_data_field_virtual_wire_signals_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_virtual_wire, dnx_data_field_virtual_wire_define_signals_nof);
}

uint32
dnx_data_field_virtual_wire_actions_per_signal_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_virtual_wire, dnx_data_field_virtual_wire_define_actions_per_signal_nof);
}

uint32
dnx_data_field_virtual_wire_general_data_user_general_containers_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_virtual_wire, dnx_data_field_virtual_wire_define_general_data_user_general_containers_size);
}

uint32
dnx_data_field_virtual_wire_ipmf1_general_data_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_virtual_wire, dnx_data_field_virtual_wire_define_ipmf1_general_data_index);
}



const dnx_data_field_virtual_wire_signal_mapping_t *
dnx_data_field_virtual_wire_signal_mapping_get(
    int unit,
    int stage,
    int signal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_virtual_wire, dnx_data_field_virtual_wire_table_signal_mapping);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, stage, signal_id);
    return (const dnx_data_field_virtual_wire_signal_mapping_t *) data;

}


shr_error_e
dnx_data_field_virtual_wire_signal_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_field_virtual_wire_signal_mapping_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_virtual_wire, dnx_data_field_virtual_wire_table_signal_mapping);
    data = (const dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->signal_name == NULL ? "" : data->signal_name);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mapped_qual);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->offset_from_qual);
            break;
        case 3:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, data->mapped_action);
            break;
        case 4:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, data->action_offset);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_field_virtual_wire_signal_mapping_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_field, dnx_data_field_submodule_virtual_wire, dnx_data_field_virtual_wire_table_signal_mapping);

}






static shr_error_e
dnx_data_field_profile_bits_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "profile_bits";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_profile_bits_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field profile_bits features");

    
    submodule_data->nof_defines = _dnx_data_field_profile_bits_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field profile_bits defines");

    submodule_data->defines[dnx_data_field_profile_bits_define_ingress_pp_port_key_gen_var_size].name = "ingress_pp_port_key_gen_var_size";
    submodule_data->defines[dnx_data_field_profile_bits_define_ingress_pp_port_key_gen_var_size].doc = "";
    
    submodule_data->defines[dnx_data_field_profile_bits_define_ingress_pp_port_key_gen_var_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_profile_bits_define_max_port_profile_size].name = "max_port_profile_size";
    submodule_data->defines[dnx_data_field_profile_bits_define_max_port_profile_size].doc = "";
    
    submodule_data->defines[dnx_data_field_profile_bits_define_max_port_profile_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_profile_bits_define_nof_bits_in_port_profile].name = "nof_bits_in_port_profile";
    submodule_data->defines[dnx_data_field_profile_bits_define_nof_bits_in_port_profile].doc = "";
    
    submodule_data->defines[dnx_data_field_profile_bits_define_nof_bits_in_port_profile].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_profile_bits_define_nof_bits_in_ingress_pp_port_general_data].name = "nof_bits_in_ingress_pp_port_general_data";
    submodule_data->defines[dnx_data_field_profile_bits_define_nof_bits_in_ingress_pp_port_general_data].doc = "";
    
    submodule_data->defines[dnx_data_field_profile_bits_define_nof_bits_in_ingress_pp_port_general_data].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_profile_bits_define_nof_ing_in_lif].name = "nof_ing_in_lif";
    submodule_data->defines[dnx_data_field_profile_bits_define_nof_ing_in_lif].doc = "";
    
    submodule_data->defines[dnx_data_field_profile_bits_define_nof_ing_in_lif].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_field_profile_bits_define_nof_ing_eth_rif].name = "nof_ing_eth_rif";
    submodule_data->defines[dnx_data_field_profile_bits_define_nof_ing_eth_rif].doc = "";
    
    submodule_data->defines[dnx_data_field_profile_bits_define_nof_ing_eth_rif].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_field_profile_bits_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field profile_bits tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_profile_bits_feature_get(
    int unit,
    dnx_data_field_profile_bits_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_profile_bits, feature);
}


uint32
dnx_data_field_profile_bits_ingress_pp_port_key_gen_var_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_profile_bits, dnx_data_field_profile_bits_define_ingress_pp_port_key_gen_var_size);
}

uint32
dnx_data_field_profile_bits_max_port_profile_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_profile_bits, dnx_data_field_profile_bits_define_max_port_profile_size);
}

uint32
dnx_data_field_profile_bits_nof_bits_in_port_profile_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_profile_bits, dnx_data_field_profile_bits_define_nof_bits_in_port_profile);
}

uint32
dnx_data_field_profile_bits_nof_bits_in_ingress_pp_port_general_data_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_profile_bits, dnx_data_field_profile_bits_define_nof_bits_in_ingress_pp_port_general_data);
}

uint32
dnx_data_field_profile_bits_nof_ing_in_lif_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_profile_bits, dnx_data_field_profile_bits_define_nof_ing_in_lif);
}

uint32
dnx_data_field_profile_bits_nof_ing_eth_rif_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_profile_bits, dnx_data_field_profile_bits_define_nof_ing_eth_rif);
}










static shr_error_e
dnx_data_field_dir_ext_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dir_ext";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_dir_ext_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field dir_ext features");

    
    submodule_data->nof_defines = _dnx_data_field_dir_ext_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field dir_ext defines");

    submodule_data->defines[dnx_data_field_dir_ext_define_half_key_size].name = "half_key_size";
    submodule_data->defines[dnx_data_field_dir_ext_define_half_key_size].doc = "";
    
    submodule_data->defines[dnx_data_field_dir_ext_define_half_key_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_dir_ext_define_single_key_size].name = "single_key_size";
    submodule_data->defines[dnx_data_field_dir_ext_define_single_key_size].doc = "";
    
    submodule_data->defines[dnx_data_field_dir_ext_define_single_key_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_dir_ext_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field dir_ext tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_dir_ext_feature_get(
    int unit,
    dnx_data_field_dir_ext_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_dir_ext, feature);
}


uint32
dnx_data_field_dir_ext_half_key_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_dir_ext, dnx_data_field_dir_ext_define_half_key_size);
}

uint32
dnx_data_field_dir_ext_single_key_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_dir_ext, dnx_data_field_dir_ext_define_single_key_size);
}










static shr_error_e
dnx_data_field_state_table_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "state_table";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_state_table_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field state_table features");

    
    submodule_data->nof_defines = _dnx_data_field_state_table_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field state_table defines");

    submodule_data->defines[dnx_data_field_state_table_define_supported].name = "supported";
    submodule_data->defines[dnx_data_field_state_table_define_supported].doc = "";
    
    submodule_data->defines[dnx_data_field_state_table_define_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_state_table_define_address_size_entry_max].name = "address_size_entry_max";
    submodule_data->defines[dnx_data_field_state_table_define_address_size_entry_max].doc = "";
    
    submodule_data->defines[dnx_data_field_state_table_define_address_size_entry_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_state_table_define_data_size_entry_max].name = "data_size_entry_max";
    submodule_data->defines[dnx_data_field_state_table_define_data_size_entry_max].doc = "";
    
    submodule_data->defines[dnx_data_field_state_table_define_data_size_entry_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_state_table_define_address_max_entry_max].name = "address_max_entry_max";
    submodule_data->defines[dnx_data_field_state_table_define_address_max_entry_max].doc = "";
    
    submodule_data->defines[dnx_data_field_state_table_define_address_max_entry_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_state_table_define_wr_bit_size_rw].name = "wr_bit_size_rw";
    submodule_data->defines[dnx_data_field_state_table_define_wr_bit_size_rw].doc = "";
    
    submodule_data->defines[dnx_data_field_state_table_define_wr_bit_size_rw].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_state_table_define_opcode_size_rmw].name = "opcode_size_rmw";
    submodule_data->defines[dnx_data_field_state_table_define_opcode_size_rmw].doc = "";
    
    submodule_data->defines[dnx_data_field_state_table_define_opcode_size_rmw].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_state_table_define_ipmf2_key_select_supported].name = "ipmf2_key_select_supported";
    submodule_data->defines[dnx_data_field_state_table_define_ipmf2_key_select_supported].doc = "";
    
    submodule_data->defines[dnx_data_field_state_table_define_ipmf2_key_select_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_state_table_define_data_size].name = "data_size";
    submodule_data->defines[dnx_data_field_state_table_define_data_size].doc = "";
    
    submodule_data->defines[dnx_data_field_state_table_define_data_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_field_state_table_define_state_table_stage_key].name = "state_table_stage_key";
    submodule_data->defines[dnx_data_field_state_table_define_state_table_stage_key].doc = "";
    
    submodule_data->defines[dnx_data_field_state_table_define_state_table_stage_key].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_field_state_table_define_address_size].name = "address_size";
    submodule_data->defines[dnx_data_field_state_table_define_address_size].doc = "";
    
    submodule_data->defines[dnx_data_field_state_table_define_address_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_field_state_table_define_address_max].name = "address_max";
    submodule_data->defines[dnx_data_field_state_table_define_address_max].doc = "";
    
    submodule_data->defines[dnx_data_field_state_table_define_address_max].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_field_state_table_define_wr_bit_size].name = "wr_bit_size";
    submodule_data->defines[dnx_data_field_state_table_define_wr_bit_size].doc = "";
    
    submodule_data->defines[dnx_data_field_state_table_define_wr_bit_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_field_state_table_define_key_size].name = "key_size";
    submodule_data->defines[dnx_data_field_state_table_define_key_size].doc = "";
    
    submodule_data->defines[dnx_data_field_state_table_define_key_size].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_field_state_table_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field state_table tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_state_table_feature_get(
    int unit,
    dnx_data_field_state_table_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_state_table, feature);
}


uint32
dnx_data_field_state_table_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_state_table, dnx_data_field_state_table_define_supported);
}

uint32
dnx_data_field_state_table_address_size_entry_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_state_table, dnx_data_field_state_table_define_address_size_entry_max);
}

uint32
dnx_data_field_state_table_data_size_entry_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_state_table, dnx_data_field_state_table_define_data_size_entry_max);
}

uint32
dnx_data_field_state_table_address_max_entry_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_state_table, dnx_data_field_state_table_define_address_max_entry_max);
}

uint32
dnx_data_field_state_table_wr_bit_size_rw_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_state_table, dnx_data_field_state_table_define_wr_bit_size_rw);
}

uint32
dnx_data_field_state_table_opcode_size_rmw_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_state_table, dnx_data_field_state_table_define_opcode_size_rmw);
}

uint32
dnx_data_field_state_table_ipmf2_key_select_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_state_table, dnx_data_field_state_table_define_ipmf2_key_select_supported);
}

uint32
dnx_data_field_state_table_data_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_state_table, dnx_data_field_state_table_define_data_size);
}

uint32
dnx_data_field_state_table_state_table_stage_key_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_state_table, dnx_data_field_state_table_define_state_table_stage_key);
}

uint32
dnx_data_field_state_table_address_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_state_table, dnx_data_field_state_table_define_address_size);
}

uint32
dnx_data_field_state_table_address_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_state_table, dnx_data_field_state_table_define_address_max);
}

uint32
dnx_data_field_state_table_wr_bit_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_state_table, dnx_data_field_state_table_define_wr_bit_size);
}

uint32
dnx_data_field_state_table_key_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_state_table, dnx_data_field_state_table_define_key_size);
}










static shr_error_e
dnx_data_field_sce_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "sce";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_sce_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field sce features");

    
    submodule_data->nof_defines = _dnx_data_field_sce_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field sce defines");

    
    submodule_data->nof_tables = _dnx_data_field_sce_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field sce tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_sce_feature_get(
    int unit,
    dnx_data_field_sce_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_sce, feature);
}











static shr_error_e
dnx_data_field_mdb_dt_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "mdb_dt";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_mdb_dt_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field mdb_dt features");

    
    submodule_data->nof_defines = _dnx_data_field_mdb_dt_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field mdb_dt defines");

    submodule_data->defines[dnx_data_field_mdb_dt_define_supported].name = "supported";
    submodule_data->defines[dnx_data_field_mdb_dt_define_supported].doc = "";
    
    submodule_data->defines[dnx_data_field_mdb_dt_define_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_mdb_dt_define_key_size].name = "key_size";
    submodule_data->defines[dnx_data_field_mdb_dt_define_key_size].doc = "";
    
    submodule_data->defines[dnx_data_field_mdb_dt_define_key_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_mdb_dt_define_action_size_large].name = "action_size_large";
    submodule_data->defines[dnx_data_field_mdb_dt_define_action_size_large].doc = "";
    
    submodule_data->defines[dnx_data_field_mdb_dt_define_action_size_large].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_mdb_dt_define_ipmf1_key_select_supported].name = "ipmf1_key_select_supported";
    submodule_data->defines[dnx_data_field_mdb_dt_define_ipmf1_key_select_supported].doc = "";
    
    submodule_data->defines[dnx_data_field_mdb_dt_define_ipmf1_key_select_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_mdb_dt_define_ipmf1_key_select].name = "ipmf1_key_select";
    submodule_data->defines[dnx_data_field_mdb_dt_define_ipmf1_key_select].doc = "";
    
    submodule_data->defines[dnx_data_field_mdb_dt_define_ipmf1_key_select].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_mdb_dt_define_ipmf2_key_select_supported].name = "ipmf2_key_select_supported";
    submodule_data->defines[dnx_data_field_mdb_dt_define_ipmf2_key_select_supported].doc = "";
    
    submodule_data->defines[dnx_data_field_mdb_dt_define_ipmf2_key_select_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_mdb_dt_define_ipmf2_key_select].name = "ipmf2_key_select";
    submodule_data->defines[dnx_data_field_mdb_dt_define_ipmf2_key_select].doc = "";
    
    submodule_data->defines[dnx_data_field_mdb_dt_define_ipmf2_key_select].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_mdb_dt_define_ipmf3_key_select_supported].name = "ipmf3_key_select_supported";
    submodule_data->defines[dnx_data_field_mdb_dt_define_ipmf3_key_select_supported].doc = "";
    
    submodule_data->defines[dnx_data_field_mdb_dt_define_ipmf3_key_select_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_mdb_dt_define_ipmf3_key_select].name = "ipmf3_key_select";
    submodule_data->defines[dnx_data_field_mdb_dt_define_ipmf3_key_select].doc = "";
    
    submodule_data->defines[dnx_data_field_mdb_dt_define_ipmf3_key_select].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_mdb_dt_define_pmf_map_stage].name = "pmf_map_stage";
    submodule_data->defines[dnx_data_field_mdb_dt_define_pmf_map_stage].doc = "";
    
    submodule_data->defines[dnx_data_field_mdb_dt_define_pmf_map_stage].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_field_mdb_dt_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field mdb_dt tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_mdb_dt_feature_get(
    int unit,
    dnx_data_field_mdb_dt_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_mdb_dt, feature);
}


uint32
dnx_data_field_mdb_dt_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_mdb_dt, dnx_data_field_mdb_dt_define_supported);
}

uint32
dnx_data_field_mdb_dt_key_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_mdb_dt, dnx_data_field_mdb_dt_define_key_size);
}

uint32
dnx_data_field_mdb_dt_action_size_large_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_mdb_dt, dnx_data_field_mdb_dt_define_action_size_large);
}

uint32
dnx_data_field_mdb_dt_ipmf1_key_select_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_mdb_dt, dnx_data_field_mdb_dt_define_ipmf1_key_select_supported);
}

uint32
dnx_data_field_mdb_dt_ipmf1_key_select_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_mdb_dt, dnx_data_field_mdb_dt_define_ipmf1_key_select);
}

uint32
dnx_data_field_mdb_dt_ipmf2_key_select_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_mdb_dt, dnx_data_field_mdb_dt_define_ipmf2_key_select_supported);
}

uint32
dnx_data_field_mdb_dt_ipmf2_key_select_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_mdb_dt, dnx_data_field_mdb_dt_define_ipmf2_key_select);
}

uint32
dnx_data_field_mdb_dt_ipmf3_key_select_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_mdb_dt, dnx_data_field_mdb_dt_define_ipmf3_key_select_supported);
}

uint32
dnx_data_field_mdb_dt_ipmf3_key_select_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_mdb_dt, dnx_data_field_mdb_dt_define_ipmf3_key_select);
}

uint32
dnx_data_field_mdb_dt_pmf_map_stage_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_mdb_dt, dnx_data_field_mdb_dt_define_pmf_map_stage);
}










static shr_error_e
dnx_data_field_hash_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "hash";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_hash_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field hash features");

    
    submodule_data->nof_defines = _dnx_data_field_hash_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field hash defines");

    submodule_data->defines[dnx_data_field_hash_define_max_key_size].name = "max_key_size";
    submodule_data->defines[dnx_data_field_hash_define_max_key_size].doc = "";
    
    submodule_data->defines[dnx_data_field_hash_define_max_key_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_hash_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field hash tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_hash_feature_get(
    int unit,
    dnx_data_field_hash_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_hash, feature);
}


uint32
dnx_data_field_hash_max_key_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_hash, dnx_data_field_hash_define_max_key_size);
}










static shr_error_e
dnx_data_field_udh_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "udh";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_udh_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field udh features");

    
    submodule_data->nof_defines = _dnx_data_field_udh_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field udh defines");

    submodule_data->defines[dnx_data_field_udh_define_ext_supported].name = "ext_supported";
    submodule_data->defines[dnx_data_field_udh_define_ext_supported].doc = "";
    
    submodule_data->defines[dnx_data_field_udh_define_ext_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_udh_define_type_count].name = "type_count";
    submodule_data->defines[dnx_data_field_udh_define_type_count].doc = "";
    
    submodule_data->defines[dnx_data_field_udh_define_type_count].flags |= DNXC_DATA_F_DEFINE;


    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_field_udh_define_type_0_length].name = "type_0_length";
        submodule_data->defines[dnx_data_field_udh_define_type_0_length].doc = "";
        
        submodule_data->defines[dnx_data_field_udh_define_type_0_length].flags |= DNXC_DATA_F_DEFINE;
    }


    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_field_udh_define_type_1_length].name = "type_1_length";
        submodule_data->defines[dnx_data_field_udh_define_type_1_length].doc = "";
        
        submodule_data->defines[dnx_data_field_udh_define_type_1_length].flags |= DNXC_DATA_F_DEFINE;
    }


    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_field_udh_define_type_2_length].name = "type_2_length";
        submodule_data->defines[dnx_data_field_udh_define_type_2_length].doc = "";
        
        submodule_data->defines[dnx_data_field_udh_define_type_2_length].flags |= DNXC_DATA_F_DEFINE;
    }


    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_field_udh_define_type_3_length].name = "type_3_length";
        submodule_data->defines[dnx_data_field_udh_define_type_3_length].doc = "";
        
        submodule_data->defines[dnx_data_field_udh_define_type_3_length].flags |= DNXC_DATA_F_DEFINE;
    }

    submodule_data->defines[dnx_data_field_udh_define_field_class_id_size_0].name = "field_class_id_size_0";
    submodule_data->defines[dnx_data_field_udh_define_field_class_id_size_0].doc = "";
    
    submodule_data->defines[dnx_data_field_udh_define_field_class_id_size_0].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_field_udh_define_field_class_id_size_1].name = "field_class_id_size_1";
    submodule_data->defines[dnx_data_field_udh_define_field_class_id_size_1].doc = "";
    
    submodule_data->defines[dnx_data_field_udh_define_field_class_id_size_1].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_field_udh_define_field_class_id_size_2].name = "field_class_id_size_2";
    submodule_data->defines[dnx_data_field_udh_define_field_class_id_size_2].doc = "";
    
    submodule_data->defines[dnx_data_field_udh_define_field_class_id_size_2].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_field_udh_define_field_class_id_size_3].name = "field_class_id_size_3";
    submodule_data->defines[dnx_data_field_udh_define_field_class_id_size_3].doc = "";
    
    submodule_data->defines[dnx_data_field_udh_define_field_class_id_size_3].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_field_udh_define_field_class_id_total_size].name = "field_class_id_total_size";
    submodule_data->defines[dnx_data_field_udh_define_field_class_id_total_size].doc = "";
    
    submodule_data->defines[dnx_data_field_udh_define_field_class_id_total_size].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_field_udh_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field udh tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_udh_feature_get(
    int unit,
    dnx_data_field_udh_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_udh, feature);
}


uint32
dnx_data_field_udh_ext_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_udh, dnx_data_field_udh_define_ext_supported);
}

uint32
dnx_data_field_udh_type_count_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_udh, dnx_data_field_udh_define_type_count);
}


uint32
dnx_data_field_udh_type_0_length_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "type_0_length", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_udh, dnx_data_field_udh_define_type_0_length);
}



uint32
dnx_data_field_udh_type_1_length_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "type_1_length", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_udh, dnx_data_field_udh_define_type_1_length);
}



uint32
dnx_data_field_udh_type_2_length_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "type_2_length", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_udh, dnx_data_field_udh_define_type_2_length);
}



uint32
dnx_data_field_udh_type_3_length_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "type_3_length", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_udh, dnx_data_field_udh_define_type_3_length);
}


uint32
dnx_data_field_udh_field_class_id_size_0_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_udh, dnx_data_field_udh_define_field_class_id_size_0);
}

uint32
dnx_data_field_udh_field_class_id_size_1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_udh, dnx_data_field_udh_define_field_class_id_size_1);
}

uint32
dnx_data_field_udh_field_class_id_size_2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_udh, dnx_data_field_udh_define_field_class_id_size_2);
}

uint32
dnx_data_field_udh_field_class_id_size_3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_udh, dnx_data_field_udh_define_field_class_id_size_3);
}

uint32
dnx_data_field_udh_field_class_id_total_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_udh, dnx_data_field_udh_define_field_class_id_total_size);
}










static shr_error_e
dnx_data_field_system_headers_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "system_headers";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_system_headers_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field system_headers features");

    
    submodule_data->nof_defines = _dnx_data_field_system_headers_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field system_headers defines");

    submodule_data->defines[dnx_data_field_system_headers_define_nof_profiles].name = "nof_profiles";
    submodule_data->defines[dnx_data_field_system_headers_define_nof_profiles].doc = "";
    
    submodule_data->defines[dnx_data_field_system_headers_define_nof_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_system_headers_define_pph_learn_ext_disable].name = "pph_learn_ext_disable";
    submodule_data->defines[dnx_data_field_system_headers_define_pph_learn_ext_disable].doc = "";
    
    submodule_data->defines[dnx_data_field_system_headers_define_pph_learn_ext_disable].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_field_system_headers_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field system_headers tables");

    
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].name = "system_header_profiles";
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].doc = "";
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].size_of_values = sizeof(dnx_data_field_system_headers_system_header_profiles_t);
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].entry_get = dnx_data_field_system_headers_system_header_profiles_entry_str_get;

    
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].nof_keys = 1;
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].keys[0].name = "system_header_profile";
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].nof_values = 6;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values, dnxc_data_value_t, submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].nof_values, "_dnx_data_field_system_headers_table_system_header_profiles table values");
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[0].name = "name";
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[0].type = "char *";
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[0].doc = "profile name";
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[0].offset = UTILEX_OFFSETOF(dnx_data_field_system_headers_system_header_profiles_t, name);
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[1].name = "build_ftmh";
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[1].type = "uint8";
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[1].doc = "Defines whether to build FTMH header or not";
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[1].offset = UTILEX_OFFSETOF(dnx_data_field_system_headers_system_header_profiles_t, build_ftmh);
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[2].name = "build_tsh";
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[2].type = "uint8";
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[2].doc = "Defines whether to build TSH header or not";
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[2].offset = UTILEX_OFFSETOF(dnx_data_field_system_headers_system_header_profiles_t, build_tsh);
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[3].name = "build_pph";
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[3].type = "uint8";
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[3].doc = "Defines whether to build PPH header or not";
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[3].offset = UTILEX_OFFSETOF(dnx_data_field_system_headers_system_header_profiles_t, build_pph);
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[4].name = "build_udh";
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[4].type = "uint8";
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[4].doc = "Defines whether to build UDH header or not";
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[4].offset = UTILEX_OFFSETOF(dnx_data_field_system_headers_system_header_profiles_t, build_udh);
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[5].name = "never_add_pph_learn_ext";
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[5].type = "uint8";
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[5].doc = "Defines whether to never add PPH Learn Ext.";
    submodule_data->tables[dnx_data_field_system_headers_table_system_header_profiles].values[5].offset = UTILEX_OFFSETOF(dnx_data_field_system_headers_system_header_profiles_t, never_add_pph_learn_ext);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_system_headers_feature_get(
    int unit,
    dnx_data_field_system_headers_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_system_headers, feature);
}


uint32
dnx_data_field_system_headers_nof_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_system_headers, dnx_data_field_system_headers_define_nof_profiles);
}

uint32
dnx_data_field_system_headers_pph_learn_ext_disable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_system_headers, dnx_data_field_system_headers_define_pph_learn_ext_disable);
}



const dnx_data_field_system_headers_system_header_profiles_t *
dnx_data_field_system_headers_system_header_profiles_get(
    int unit,
    int system_header_profile)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_system_headers, dnx_data_field_system_headers_table_system_header_profiles);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, system_header_profile, 0);
    return (const dnx_data_field_system_headers_system_header_profiles_t *) data;

}


shr_error_e
dnx_data_field_system_headers_system_header_profiles_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_field_system_headers_system_header_profiles_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_system_headers, dnx_data_field_system_headers_table_system_header_profiles);
    data = (const dnx_data_field_system_headers_system_header_profiles_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->name == NULL ? "" : data->name);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->build_ftmh);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->build_tsh);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->build_pph);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->build_udh);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->never_add_pph_learn_ext);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_field_system_headers_system_header_profiles_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_field, dnx_data_field_submodule_system_headers, dnx_data_field_system_headers_table_system_header_profiles);

}






static shr_error_e
dnx_data_field_exem_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "exem";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_exem_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field exem features");

    submodule_data->features[dnx_data_field_exem_exem_vmv_padded_for_2msb].name = "exem_vmv_padded_for_2msb";
    submodule_data->features[dnx_data_field_exem_exem_vmv_padded_for_2msb].doc = "";
    submodule_data->features[dnx_data_field_exem_exem_vmv_padded_for_2msb].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_field_exem_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field exem defines");

    submodule_data->defines[dnx_data_field_exem_define_small_app_db_id_size].name = "small_app_db_id_size";
    submodule_data->defines[dnx_data_field_exem_define_small_app_db_id_size].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_define_small_app_db_id_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_define_large_app_db_id_size].name = "large_app_db_id_size";
    submodule_data->defines[dnx_data_field_exem_define_large_app_db_id_size].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_define_large_app_db_id_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_define_small_max_key_size].name = "small_max_key_size";
    submodule_data->defines[dnx_data_field_exem_define_small_max_key_size].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_define_small_max_key_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_define_large_max_key_size].name = "large_max_key_size";
    submodule_data->defines[dnx_data_field_exem_define_large_max_key_size].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_define_large_max_key_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_define_max_result_size_all_stages].name = "max_result_size_all_stages";
    submodule_data->defines[dnx_data_field_exem_define_max_result_size_all_stages].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_define_max_result_size_all_stages].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_define_small_max_container_size].name = "small_max_container_size";
    submodule_data->defines[dnx_data_field_exem_define_small_max_container_size].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_define_small_max_container_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_define_large_max_container_size].name = "large_max_container_size";
    submodule_data->defines[dnx_data_field_exem_define_large_max_container_size].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_define_large_max_container_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_define_vmv_hw_max_size].name = "vmv_hw_max_size";
    submodule_data->defines[dnx_data_field_exem_define_vmv_hw_max_size].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_define_vmv_hw_max_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_define_small_min_app_db_id_range].name = "small_min_app_db_id_range";
    submodule_data->defines[dnx_data_field_exem_define_small_min_app_db_id_range].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_define_small_min_app_db_id_range].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_define_large_min_app_db_id_range].name = "large_min_app_db_id_range";
    submodule_data->defines[dnx_data_field_exem_define_large_min_app_db_id_range].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_define_large_min_app_db_id_range].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_define_small_max_app_db_id_range].name = "small_max_app_db_id_range";
    submodule_data->defines[dnx_data_field_exem_define_small_max_app_db_id_range].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_define_small_max_app_db_id_range].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_define_large_max_app_db_id_range].name = "large_max_app_db_id_range";
    submodule_data->defines[dnx_data_field_exem_define_large_max_app_db_id_range].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_define_large_max_app_db_id_range].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_define_small_ipmf2_key].name = "small_ipmf2_key";
    submodule_data->defines[dnx_data_field_exem_define_small_ipmf2_key].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_define_small_ipmf2_key].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_define_small_ipmf2_key_part].name = "small_ipmf2_key_part";
    submodule_data->defines[dnx_data_field_exem_define_small_ipmf2_key_part].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_define_small_ipmf2_key_part].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_define_small_ipmf2_key_hw_value].name = "small_ipmf2_key_hw_value";
    submodule_data->defines[dnx_data_field_exem_define_small_ipmf2_key_hw_value].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_define_small_ipmf2_key_hw_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_define_small_ipmf2_key_hw_bits].name = "small_ipmf2_key_hw_bits";
    submodule_data->defines[dnx_data_field_exem_define_small_ipmf2_key_hw_bits].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_define_small_ipmf2_key_hw_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_define_ipmf1_key_configurable].name = "ipmf1_key_configurable";
    submodule_data->defines[dnx_data_field_exem_define_ipmf1_key_configurable].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_define_ipmf1_key_configurable].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_define_ipmf2_key_configurable].name = "ipmf2_key_configurable";
    submodule_data->defines[dnx_data_field_exem_define_ipmf2_key_configurable].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_define_ipmf2_key_configurable].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_define_ipmf1_key].name = "ipmf1_key";
    submodule_data->defines[dnx_data_field_exem_define_ipmf1_key].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_define_ipmf1_key].flags |= DNXC_DATA_F_DEFINE;


    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_field_exem_define_pmf_sexem3_stage].name = "pmf_sexem3_stage";
        submodule_data->defines[dnx_data_field_exem_define_pmf_sexem3_stage].doc = "";
        
        submodule_data->defines[dnx_data_field_exem_define_pmf_sexem3_stage].flags |= DNXC_DATA_F_NUMERIC;
    }

    
    submodule_data->nof_tables = _dnx_data_field_exem_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field exem tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_exem_feature_get(
    int unit,
    dnx_data_field_exem_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem, feature);
}


uint32
dnx_data_field_exem_small_app_db_id_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem, dnx_data_field_exem_define_small_app_db_id_size);
}

uint32
dnx_data_field_exem_large_app_db_id_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem, dnx_data_field_exem_define_large_app_db_id_size);
}

uint32
dnx_data_field_exem_small_max_key_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem, dnx_data_field_exem_define_small_max_key_size);
}

uint32
dnx_data_field_exem_large_max_key_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem, dnx_data_field_exem_define_large_max_key_size);
}

uint32
dnx_data_field_exem_max_result_size_all_stages_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem, dnx_data_field_exem_define_max_result_size_all_stages);
}

uint32
dnx_data_field_exem_small_max_container_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem, dnx_data_field_exem_define_small_max_container_size);
}

uint32
dnx_data_field_exem_large_max_container_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem, dnx_data_field_exem_define_large_max_container_size);
}

uint32
dnx_data_field_exem_vmv_hw_max_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem, dnx_data_field_exem_define_vmv_hw_max_size);
}

uint32
dnx_data_field_exem_small_min_app_db_id_range_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem, dnx_data_field_exem_define_small_min_app_db_id_range);
}

uint32
dnx_data_field_exem_large_min_app_db_id_range_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem, dnx_data_field_exem_define_large_min_app_db_id_range);
}

uint32
dnx_data_field_exem_small_max_app_db_id_range_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem, dnx_data_field_exem_define_small_max_app_db_id_range);
}

uint32
dnx_data_field_exem_large_max_app_db_id_range_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem, dnx_data_field_exem_define_large_max_app_db_id_range);
}

uint32
dnx_data_field_exem_small_ipmf2_key_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem, dnx_data_field_exem_define_small_ipmf2_key);
}

uint32
dnx_data_field_exem_small_ipmf2_key_part_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem, dnx_data_field_exem_define_small_ipmf2_key_part);
}

uint32
dnx_data_field_exem_small_ipmf2_key_hw_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem, dnx_data_field_exem_define_small_ipmf2_key_hw_value);
}

uint32
dnx_data_field_exem_small_ipmf2_key_hw_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem, dnx_data_field_exem_define_small_ipmf2_key_hw_bits);
}

uint32
dnx_data_field_exem_ipmf1_key_configurable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem, dnx_data_field_exem_define_ipmf1_key_configurable);
}

uint32
dnx_data_field_exem_ipmf2_key_configurable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem, dnx_data_field_exem_define_ipmf2_key_configurable);
}

uint32
dnx_data_field_exem_ipmf1_key_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem, dnx_data_field_exem_define_ipmf1_key);
}


uint32
dnx_data_field_exem_pmf_sexem3_stage_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "pmf_sexem3_stage", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem, dnx_data_field_exem_define_pmf_sexem3_stage);
}











static shr_error_e
dnx_data_field_exem_learn_flush_machine_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "exem_learn_flush_machine";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_exem_learn_flush_machine_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field exem_learn_flush_machine features");

    submodule_data->features[dnx_data_field_exem_learn_flush_machine_has_elephant_signals].name = "has_elephant_signals";
    submodule_data->features[dnx_data_field_exem_learn_flush_machine_has_elephant_signals].doc = "";
    submodule_data->features[dnx_data_field_exem_learn_flush_machine_has_elephant_signals].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_field_exem_learn_flush_machine_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field exem_learn_flush_machine defines");

    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_flush_machine_nof_entries].name = "flush_machine_nof_entries";
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_flush_machine_nof_entries].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_flush_machine_nof_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_entry_extra_bits_for_hash].name = "entry_extra_bits_for_hash";
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_entry_extra_bits_for_hash].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_entry_extra_bits_for_hash].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_sexem_entry_max_size].name = "sexem_entry_max_size";
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_sexem_entry_max_size].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_sexem_entry_max_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_lexem_entry_max_size].name = "lexem_entry_max_size";
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_lexem_entry_max_size].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_lexem_entry_max_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_command_bit_transplant].name = "command_bit_transplant";
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_command_bit_transplant].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_command_bit_transplant].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_command_bit_delete].name = "command_bit_delete";
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_command_bit_delete].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_command_bit_delete].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_command_bit_clear_hit_bit].name = "command_bit_clear_hit_bit";
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_command_bit_clear_hit_bit].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_command_bit_clear_hit_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_nof_bits_source].name = "nof_bits_source";
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_nof_bits_source].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_nof_bits_source].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_source_bit_for_scan].name = "source_bit_for_scan";
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_source_bit_for_scan].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_source_bit_for_scan].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_source_bit_for_pipe].name = "source_bit_for_pipe";
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_source_bit_for_pipe].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_source_bit_for_pipe].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_source_bit_for_mrq].name = "source_bit_for_mrq";
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_source_bit_for_mrq].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_source_bit_for_mrq].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_nof_bits_accessed].name = "nof_bits_accessed";
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_nof_bits_accessed].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_nof_bits_accessed].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_nof_flush_profiles].name = "nof_flush_profiles";
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_nof_flush_profiles].doc = "";
    
    submodule_data->defines[dnx_data_field_exem_learn_flush_machine_define_nof_flush_profiles].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_exem_learn_flush_machine_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field exem_learn_flush_machine tables");

    
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].name = "sexem_flush_tcam_tcam_memories";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].doc = "";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].size_of_values = sizeof(dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].entry_get = dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_entry_str_get;

    
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].nof_keys = 0;

    
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].nof_values = 10;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values, dnxc_data_value_t, submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].nof_values, "_dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories table values");
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[0].name = "table_name";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[0].type = "soc_mem_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[0].doc = "memory name";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[0].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t, table_name);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[1].name = "valid";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[1].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[1].doc = "1 means rule is valid and should be used. 0 rule should not be checked.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[1].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t, valid);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[2].name = "entry_data";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[2].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[2].doc = "The key value for entry to be compared.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[2].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t, entry_data);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[3].name = "entry_mask";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[3].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[3].doc = "The mask value for entry to be compared.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[3].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t, entry_mask);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[4].name = "src_data";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[4].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[4].doc = "The key value for SRC bits {MRQ,PIPE,SCAN}.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[4].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t, src_data);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[5].name = "src_mask";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[5].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[5].doc = "The mask value for SRC bits {MRQ,PIPE,SCAN}.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[5].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t, src_mask);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[6].name = "appdb_id_data";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[6].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[6].doc = "The key value for the app_db_id of the entry.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[6].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t, appdb_id_data);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[7].name = "appdb_id_mask";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[7].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[7].doc = "The mask value for the app_db_id of the entry.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[7].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t, appdb_id_mask);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[8].name = "accessed_data";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[8].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[8].doc = "The key value for accessed.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[8].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t, accessed_data);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[9].name = "accessed_mask";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[9].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[9].doc = "The mask value for accessed.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories].values[9].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t, accessed_mask);

    
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].name = "lexem_flush_tcam_tcam_memories";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].doc = "";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].size_of_values = sizeof(dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].entry_get = dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_entry_str_get;

    
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].nof_keys = 0;

    
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].nof_values = 10;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values, dnxc_data_value_t, submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].nof_values, "_dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories table values");
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[0].name = "table_name";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[0].type = "soc_mem_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[0].doc = "memory name";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[0].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t, table_name);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[1].name = "valid";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[1].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[1].doc = "1 means rule is valid and should be used. 0 rule should not be checked.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[1].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t, valid);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[2].name = "entry_data";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[2].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[2].doc = "The key value for entry to be compared.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[2].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t, entry_data);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[3].name = "entry_mask";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[3].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[3].doc = "The mask value for entry to be compared.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[3].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t, entry_mask);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[4].name = "src_data";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[4].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[4].doc = "The key value for SRC bits {MRQ,PIPE,SCAN}.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[4].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t, src_data);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[5].name = "src_mask";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[5].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[5].doc = "The mask value for SRC bits {MRQ,PIPE,SCAN}.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[5].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t, src_mask);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[6].name = "appdb_id_data";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[6].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[6].doc = "The key value for the app_db_id of the entry.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[6].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t, appdb_id_data);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[7].name = "appdb_id_mask";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[7].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[7].doc = "The mask value for the app_db_id of the entry.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[7].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t, appdb_id_mask);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[8].name = "accessed_data";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[8].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[8].doc = "The key value for accessed.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[8].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t, accessed_data);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[9].name = "accessed_mask";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[9].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[9].doc = "The mask value for accessed.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories].values[9].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t, accessed_mask);

    
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].name = "sexem_flush_tcam_data_rules";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].doc = "";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].size_of_values = sizeof(dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_t);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].entry_get = dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_entry_str_get;

    
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].nof_keys = 0;

    
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].values, dnxc_data_value_t, submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].nof_values, "_dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules table values");
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].values[0].name = "table_name";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].values[0].type = "soc_mem_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].values[0].doc = "memory name";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].values[0].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_t, table_name);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].values[1].name = "command";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].values[1].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].values[1].doc = "command type.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].values[1].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_t, command);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].values[2].name = "payload";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].values[2].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].values[2].doc = "Payload for transplant action.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].values[2].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_t, payload);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].values[3].name = "payload_mask";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].values[3].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].values[3].doc = "Mask for the payload in transplant action.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules].values[3].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_t, payload_mask);

    
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].name = "lexem_flush_tcam_data_rules";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].doc = "";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].size_of_values = sizeof(dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_t);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].entry_get = dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_entry_str_get;

    
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].nof_keys = 0;

    
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].values, dnxc_data_value_t, submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].nof_values, "_dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules table values");
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].values[0].name = "table_name";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].values[0].type = "soc_mem_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].values[0].doc = "memory name";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].values[0].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_t, table_name);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].values[1].name = "command";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].values[1].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].values[1].doc = "command type.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].values[1].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_t, command);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].values[2].name = "payload";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].values[2].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].values[2].doc = "Payload for transplant action.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].values[2].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_t, payload);
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].values[3].name = "payload_mask";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].values[3].type = "soc_field_t";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].values[3].doc = "Mask for the payload in transplant action.";
    submodule_data->tables[dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules].values[3].offset = UTILEX_OFFSETOF(dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_t, payload_mask);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_exem_learn_flush_machine_feature_get(
    int unit,
    dnx_data_field_exem_learn_flush_machine_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, feature);
}


uint32
dnx_data_field_exem_learn_flush_machine_flush_machine_nof_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_define_flush_machine_nof_entries);
}

uint32
dnx_data_field_exem_learn_flush_machine_entry_extra_bits_for_hash_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_define_entry_extra_bits_for_hash);
}

uint32
dnx_data_field_exem_learn_flush_machine_sexem_entry_max_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_define_sexem_entry_max_size);
}

uint32
dnx_data_field_exem_learn_flush_machine_lexem_entry_max_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_define_lexem_entry_max_size);
}

uint32
dnx_data_field_exem_learn_flush_machine_command_bit_transplant_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_define_command_bit_transplant);
}

uint32
dnx_data_field_exem_learn_flush_machine_command_bit_delete_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_define_command_bit_delete);
}

uint32
dnx_data_field_exem_learn_flush_machine_command_bit_clear_hit_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_define_command_bit_clear_hit_bit);
}

uint32
dnx_data_field_exem_learn_flush_machine_nof_bits_source_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_define_nof_bits_source);
}

uint32
dnx_data_field_exem_learn_flush_machine_source_bit_for_scan_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_define_source_bit_for_scan);
}

uint32
dnx_data_field_exem_learn_flush_machine_source_bit_for_pipe_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_define_source_bit_for_pipe);
}

uint32
dnx_data_field_exem_learn_flush_machine_source_bit_for_mrq_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_define_source_bit_for_mrq);
}

uint32
dnx_data_field_exem_learn_flush_machine_nof_bits_accessed_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_define_nof_bits_accessed);
}

uint32
dnx_data_field_exem_learn_flush_machine_nof_flush_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_define_nof_flush_profiles);
}



const dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t *
dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t *) data;

}

const dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t *
dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t *) data;

}

const dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_t *
dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_t *) data;

}

const dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_t *
dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_t *) data;

}


shr_error_e
dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories);
    data = (const dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->table_name);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->entry_data);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->entry_mask);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->src_data);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->src_mask);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->appdb_id_data);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->appdb_id_mask);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->accessed_data);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->accessed_mask);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories);
    data = (const dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->table_name);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->entry_data);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->entry_mask);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->src_data);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->src_mask);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->appdb_id_data);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->appdb_id_mask);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->accessed_data);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->accessed_mask);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules);
    data = (const dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->table_name);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->command);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->payload);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->payload_mask);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules);
    data = (const dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->table_name);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->command);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->payload);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->payload_mask);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_tcam_memories);

}

const dnxc_data_table_info_t *
dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_tcam_memories);

}

const dnxc_data_table_info_t *
dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_table_sexem_flush_tcam_data_rules);

}

const dnxc_data_table_info_t *
dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_field, dnx_data_field_submodule_exem_learn_flush_machine, dnx_data_field_exem_learn_flush_machine_table_lexem_flush_tcam_data_rules);

}






static shr_error_e
dnx_data_field_ace_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ace";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_ace_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field ace features");

    
    submodule_data->nof_defines = _dnx_data_field_ace_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field ace defines");

    submodule_data->defines[dnx_data_field_ace_define_supported].name = "supported";
    submodule_data->defines[dnx_data_field_ace_define_supported].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_ace_id_size].name = "ace_id_size";
    submodule_data->defines[dnx_data_field_ace_define_ace_id_size].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_ace_id_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_key_size].name = "key_size";
    submodule_data->defines[dnx_data_field_ace_define_key_size].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_key_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_app_db_id_size].name = "app_db_id_size";
    submodule_data->defines[dnx_data_field_ace_define_app_db_id_size].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_app_db_id_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_payload_size].name = "payload_size";
    submodule_data->defines[dnx_data_field_ace_define_payload_size].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_payload_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_min_key_range_pmf].name = "min_key_range_pmf";
    submodule_data->defines[dnx_data_field_ace_define_min_key_range_pmf].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_min_key_range_pmf].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_max_key_range_pmf].name = "max_key_range_pmf";
    submodule_data->defines[dnx_data_field_ace_define_max_key_range_pmf].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_max_key_range_pmf].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_min_ace_id_dynamic_range].name = "min_ace_id_dynamic_range";
    submodule_data->defines[dnx_data_field_ace_define_min_ace_id_dynamic_range].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_min_ace_id_dynamic_range].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_nof_ace_id].name = "nof_ace_id";
    submodule_data->defines[dnx_data_field_ace_define_nof_ace_id].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_nof_ace_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_nof_action_per_ace_format].name = "nof_action_per_ace_format";
    submodule_data->defines[dnx_data_field_ace_define_nof_action_per_ace_format].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_nof_action_per_ace_format].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_row_size].name = "row_size";
    submodule_data->defines[dnx_data_field_ace_define_row_size].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_row_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_min_entry_size].name = "min_entry_size";
    submodule_data->defines[dnx_data_field_ace_define_min_entry_size].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_min_entry_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_ace_id_pmf_alloc_first].name = "ace_id_pmf_alloc_first";
    submodule_data->defines[dnx_data_field_ace_define_ace_id_pmf_alloc_first].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_ace_id_pmf_alloc_first].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_ace_id_pmf_alloc_last].name = "ace_id_pmf_alloc_last";
    submodule_data->defines[dnx_data_field_ace_define_ace_id_pmf_alloc_last].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_ace_id_pmf_alloc_last].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_nof_masks_per_fes].name = "nof_masks_per_fes";
    submodule_data->defines[dnx_data_field_ace_define_nof_masks_per_fes].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_nof_masks_per_fes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_nof_fes_id_per_array].name = "nof_fes_id_per_array";
    submodule_data->defines[dnx_data_field_ace_define_nof_fes_id_per_array].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_nof_fes_id_per_array].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_nof_fes_array].name = "nof_fes_array";
    submodule_data->defines[dnx_data_field_ace_define_nof_fes_array].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_nof_fes_array].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_nof_fes_instruction_per_context].name = "nof_fes_instruction_per_context";
    submodule_data->defines[dnx_data_field_ace_define_nof_fes_instruction_per_context].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_nof_fes_instruction_per_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_nof_fes_programs].name = "nof_fes_programs";
    submodule_data->defines[dnx_data_field_ace_define_nof_fes_programs].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_nof_fes_programs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_nof_prog_per_fes].name = "nof_prog_per_fes";
    submodule_data->defines[dnx_data_field_ace_define_nof_prog_per_fes].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_nof_prog_per_fes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_nof_bits_in_fes_action].name = "nof_bits_in_fes_action";
    submodule_data->defines[dnx_data_field_ace_define_nof_bits_in_fes_action].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_nof_bits_in_fes_action].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_fes_shift_for_zero_bit].name = "fes_shift_for_zero_bit";
    submodule_data->defines[dnx_data_field_ace_define_fes_shift_for_zero_bit].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_fes_shift_for_zero_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_fes_instruction_size].name = "fes_instruction_size";
    submodule_data->defines[dnx_data_field_ace_define_fes_instruction_size].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_fes_instruction_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_fes_shift_offset].name = "fes_shift_offset";
    submodule_data->defines[dnx_data_field_ace_define_fes_shift_offset].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_fes_shift_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_fes_invalid_bits_offset].name = "fes_invalid_bits_offset";
    submodule_data->defines[dnx_data_field_ace_define_fes_invalid_bits_offset].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_fes_invalid_bits_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_fes_type_offset].name = "fes_type_offset";
    submodule_data->defines[dnx_data_field_ace_define_fes_type_offset].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_fes_type_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_fes_ace_action_offset].name = "fes_ace_action_offset";
    submodule_data->defines[dnx_data_field_ace_define_fes_ace_action_offset].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_fes_ace_action_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_ace_define_fes_chosen_mask_offset].name = "fes_chosen_mask_offset";
    submodule_data->defines[dnx_data_field_ace_define_fes_chosen_mask_offset].doc = "";
    
    submodule_data->defines[dnx_data_field_ace_define_fes_chosen_mask_offset].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_ace_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field ace tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_ace_feature_get(
    int unit,
    dnx_data_field_ace_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, feature);
}


uint32
dnx_data_field_ace_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_supported);
}

uint32
dnx_data_field_ace_ace_id_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_ace_id_size);
}

uint32
dnx_data_field_ace_key_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_key_size);
}

uint32
dnx_data_field_ace_app_db_id_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_app_db_id_size);
}

uint32
dnx_data_field_ace_payload_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_payload_size);
}

uint32
dnx_data_field_ace_min_key_range_pmf_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_min_key_range_pmf);
}

uint32
dnx_data_field_ace_max_key_range_pmf_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_max_key_range_pmf);
}

uint32
dnx_data_field_ace_min_ace_id_dynamic_range_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_min_ace_id_dynamic_range);
}

uint32
dnx_data_field_ace_nof_ace_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_nof_ace_id);
}

uint32
dnx_data_field_ace_nof_action_per_ace_format_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_nof_action_per_ace_format);
}

uint32
dnx_data_field_ace_row_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_row_size);
}

uint32
dnx_data_field_ace_min_entry_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_min_entry_size);
}

uint32
dnx_data_field_ace_ace_id_pmf_alloc_first_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_ace_id_pmf_alloc_first);
}

uint32
dnx_data_field_ace_ace_id_pmf_alloc_last_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_ace_id_pmf_alloc_last);
}

uint32
dnx_data_field_ace_nof_masks_per_fes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_nof_masks_per_fes);
}

uint32
dnx_data_field_ace_nof_fes_id_per_array_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_nof_fes_id_per_array);
}

uint32
dnx_data_field_ace_nof_fes_array_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_nof_fes_array);
}

uint32
dnx_data_field_ace_nof_fes_instruction_per_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_nof_fes_instruction_per_context);
}

uint32
dnx_data_field_ace_nof_fes_programs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_nof_fes_programs);
}

uint32
dnx_data_field_ace_nof_prog_per_fes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_nof_prog_per_fes);
}

uint32
dnx_data_field_ace_nof_bits_in_fes_action_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_nof_bits_in_fes_action);
}

uint32
dnx_data_field_ace_fes_shift_for_zero_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_fes_shift_for_zero_bit);
}

uint32
dnx_data_field_ace_fes_instruction_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_fes_instruction_size);
}

uint32
dnx_data_field_ace_fes_shift_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_fes_shift_offset);
}

uint32
dnx_data_field_ace_fes_invalid_bits_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_fes_invalid_bits_offset);
}

uint32
dnx_data_field_ace_fes_type_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_fes_type_offset);
}

uint32
dnx_data_field_ace_fes_ace_action_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_fes_ace_action_offset);
}

uint32
dnx_data_field_ace_fes_chosen_mask_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_ace, dnx_data_field_ace_define_fes_chosen_mask_offset);
}










static shr_error_e
dnx_data_field_entry_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "entry";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_entry_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field entry features");

    
    submodule_data->nof_defines = _dnx_data_field_entry_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field entry defines");

    submodule_data->defines[dnx_data_field_entry_define_dir_ext_nof_fields].name = "dir_ext_nof_fields";
    submodule_data->defines[dnx_data_field_entry_define_dir_ext_nof_fields].doc = "";
    
    submodule_data->defines[dnx_data_field_entry_define_dir_ext_nof_fields].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_entry_define_nof_action_params_per_entry].name = "nof_action_params_per_entry";
    submodule_data->defines[dnx_data_field_entry_define_nof_action_params_per_entry].doc = "";
    
    submodule_data->defines[dnx_data_field_entry_define_nof_action_params_per_entry].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_entry_define_nof_qual_params_per_entry].name = "nof_qual_params_per_entry";
    submodule_data->defines[dnx_data_field_entry_define_nof_qual_params_per_entry].doc = "";
    
    submodule_data->defines[dnx_data_field_entry_define_nof_qual_params_per_entry].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_entry_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field entry tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_entry_feature_get(
    int unit,
    dnx_data_field_entry_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_entry, feature);
}


uint32
dnx_data_field_entry_dir_ext_nof_fields_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_entry, dnx_data_field_entry_define_dir_ext_nof_fields);
}

uint32
dnx_data_field_entry_nof_action_params_per_entry_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_entry, dnx_data_field_entry_define_nof_action_params_per_entry);
}

uint32
dnx_data_field_entry_nof_qual_params_per_entry_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_entry, dnx_data_field_entry_define_nof_qual_params_per_entry);
}










static shr_error_e
dnx_data_field_L4_Ops_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "L4_Ops";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_L4_Ops_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field L4_Ops features");

    
    submodule_data->nof_defines = _dnx_data_field_L4_Ops_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field L4_Ops defines");

    submodule_data->defines[dnx_data_field_L4_Ops_define_udp_position].name = "udp_position";
    submodule_data->defines[dnx_data_field_L4_Ops_define_udp_position].doc = "";
    
    submodule_data->defines[dnx_data_field_L4_Ops_define_udp_position].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_L4_Ops_define_tcp_position].name = "tcp_position";
    submodule_data->defines[dnx_data_field_L4_Ops_define_tcp_position].doc = "";
    
    submodule_data->defines[dnx_data_field_L4_Ops_define_tcp_position].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_L4_Ops_define_nof_range_entries].name = "nof_range_entries";
    submodule_data->defines[dnx_data_field_L4_Ops_define_nof_range_entries].doc = "";
    
    submodule_data->defines[dnx_data_field_L4_Ops_define_nof_range_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_L4_Ops_define_nof_ext_encoders].name = "nof_ext_encoders";
    submodule_data->defines[dnx_data_field_L4_Ops_define_nof_ext_encoders].doc = "";
    
    submodule_data->defines[dnx_data_field_L4_Ops_define_nof_ext_encoders].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_L4_Ops_define_nof_ext_types].name = "nof_ext_types";
    submodule_data->defines[dnx_data_field_L4_Ops_define_nof_ext_types].doc = "";
    
    submodule_data->defines[dnx_data_field_L4_Ops_define_nof_ext_types].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_L4_Ops_define_nof_configurable_ranges].name = "nof_configurable_ranges";
    submodule_data->defines[dnx_data_field_L4_Ops_define_nof_configurable_ranges].doc = "";
    
    submodule_data->defines[dnx_data_field_L4_Ops_define_nof_configurable_ranges].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_L4_Ops_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field L4_Ops tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_L4_Ops_feature_get(
    int unit,
    dnx_data_field_L4_Ops_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_L4_Ops, feature);
}


uint32
dnx_data_field_L4_Ops_udp_position_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_L4_Ops, dnx_data_field_L4_Ops_define_udp_position);
}

uint32
dnx_data_field_L4_Ops_tcp_position_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_L4_Ops, dnx_data_field_L4_Ops_define_tcp_position);
}

uint32
dnx_data_field_L4_Ops_nof_range_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_L4_Ops, dnx_data_field_L4_Ops_define_nof_range_entries);
}

uint32
dnx_data_field_L4_Ops_nof_ext_encoders_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_L4_Ops, dnx_data_field_L4_Ops_define_nof_ext_encoders);
}

uint32
dnx_data_field_L4_Ops_nof_ext_types_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_L4_Ops, dnx_data_field_L4_Ops_define_nof_ext_types);
}

uint32
dnx_data_field_L4_Ops_nof_configurable_ranges_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_L4_Ops, dnx_data_field_L4_Ops_define_nof_configurable_ranges);
}










static shr_error_e
dnx_data_field_encoded_qual_actions_offset_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "encoded_qual_actions_offset";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_encoded_qual_actions_offset_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field encoded_qual_actions_offset features");

    
    submodule_data->nof_defines = _dnx_data_field_encoded_qual_actions_offset_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field encoded_qual_actions_offset defines");

    submodule_data->defines[dnx_data_field_encoded_qual_actions_offset_define_trap_strength_offset].name = "trap_strength_offset";
    submodule_data->defines[dnx_data_field_encoded_qual_actions_offset_define_trap_strength_offset].doc = "";
    
    submodule_data->defines[dnx_data_field_encoded_qual_actions_offset_define_trap_strength_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_encoded_qual_actions_offset_define_trap_qualifier_offset].name = "trap_qualifier_offset";
    submodule_data->defines[dnx_data_field_encoded_qual_actions_offset_define_trap_qualifier_offset].doc = "";
    
    submodule_data->defines[dnx_data_field_encoded_qual_actions_offset_define_trap_qualifier_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_encoded_qual_actions_offset_define_sniff_qualifier_offset].name = "sniff_qualifier_offset";
    submodule_data->defines[dnx_data_field_encoded_qual_actions_offset_define_sniff_qualifier_offset].doc = "";
    
    submodule_data->defines[dnx_data_field_encoded_qual_actions_offset_define_sniff_qualifier_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_encoded_qual_actions_offset_define_mirror_qualifier_offset].name = "mirror_qualifier_offset";
    submodule_data->defines[dnx_data_field_encoded_qual_actions_offset_define_mirror_qualifier_offset].doc = "";
    
    submodule_data->defines[dnx_data_field_encoded_qual_actions_offset_define_mirror_qualifier_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_encoded_qual_actions_offset_define_forward_trap_qualifier_offset].name = "forward_trap_qualifier_offset";
    submodule_data->defines[dnx_data_field_encoded_qual_actions_offset_define_forward_trap_qualifier_offset].doc = "";
    
    submodule_data->defines[dnx_data_field_encoded_qual_actions_offset_define_forward_trap_qualifier_offset].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_encoded_qual_actions_offset_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field encoded_qual_actions_offset tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_encoded_qual_actions_offset_feature_get(
    int unit,
    dnx_data_field_encoded_qual_actions_offset_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_encoded_qual_actions_offset, feature);
}


uint32
dnx_data_field_encoded_qual_actions_offset_trap_strength_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_encoded_qual_actions_offset, dnx_data_field_encoded_qual_actions_offset_define_trap_strength_offset);
}

uint32
dnx_data_field_encoded_qual_actions_offset_trap_qualifier_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_encoded_qual_actions_offset, dnx_data_field_encoded_qual_actions_offset_define_trap_qualifier_offset);
}

uint32
dnx_data_field_encoded_qual_actions_offset_sniff_qualifier_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_encoded_qual_actions_offset, dnx_data_field_encoded_qual_actions_offset_define_sniff_qualifier_offset);
}

uint32
dnx_data_field_encoded_qual_actions_offset_mirror_qualifier_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_encoded_qual_actions_offset, dnx_data_field_encoded_qual_actions_offset_define_mirror_qualifier_offset);
}

uint32
dnx_data_field_encoded_qual_actions_offset_forward_trap_qualifier_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_encoded_qual_actions_offset, dnx_data_field_encoded_qual_actions_offset_define_forward_trap_qualifier_offset);
}










static shr_error_e
dnx_data_field_compare_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "compare";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_compare_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field compare features");

    
    submodule_data->nof_defines = _dnx_data_field_compare_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field compare defines");

    submodule_data->defines[dnx_data_field_compare_define_equal_offset].name = "equal_offset";
    submodule_data->defines[dnx_data_field_compare_define_equal_offset].doc = "";
    
    submodule_data->defines[dnx_data_field_compare_define_equal_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_compare_define_not_equal_offset].name = "not_equal_offset";
    submodule_data->defines[dnx_data_field_compare_define_not_equal_offset].doc = "";
    
    submodule_data->defines[dnx_data_field_compare_define_not_equal_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_compare_define_smaller_offset].name = "smaller_offset";
    submodule_data->defines[dnx_data_field_compare_define_smaller_offset].doc = "";
    
    submodule_data->defines[dnx_data_field_compare_define_smaller_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_compare_define_not_smaller_offset].name = "not_smaller_offset";
    submodule_data->defines[dnx_data_field_compare_define_not_smaller_offset].doc = "";
    
    submodule_data->defines[dnx_data_field_compare_define_not_smaller_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_compare_define_bigger_offset].name = "bigger_offset";
    submodule_data->defines[dnx_data_field_compare_define_bigger_offset].doc = "";
    
    submodule_data->defines[dnx_data_field_compare_define_bigger_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_compare_define_not_bigger_offset].name = "not_bigger_offset";
    submodule_data->defines[dnx_data_field_compare_define_not_bigger_offset].doc = "";
    
    submodule_data->defines[dnx_data_field_compare_define_not_bigger_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_compare_define_nof_keys].name = "nof_keys";
    submodule_data->defines[dnx_data_field_compare_define_nof_keys].doc = "";
    
    submodule_data->defines[dnx_data_field_compare_define_nof_keys].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_compare_define_nof_compare_pairs].name = "nof_compare_pairs";
    submodule_data->defines[dnx_data_field_compare_define_nof_compare_pairs].doc = "";
    
    submodule_data->defines[dnx_data_field_compare_define_nof_compare_pairs].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_compare_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field compare tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_compare_feature_get(
    int unit,
    dnx_data_field_compare_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_compare, feature);
}


uint32
dnx_data_field_compare_equal_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_compare, dnx_data_field_compare_define_equal_offset);
}

uint32
dnx_data_field_compare_not_equal_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_compare, dnx_data_field_compare_define_not_equal_offset);
}

uint32
dnx_data_field_compare_smaller_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_compare, dnx_data_field_compare_define_smaller_offset);
}

uint32
dnx_data_field_compare_not_smaller_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_compare, dnx_data_field_compare_define_not_smaller_offset);
}

uint32
dnx_data_field_compare_bigger_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_compare, dnx_data_field_compare_define_bigger_offset);
}

uint32
dnx_data_field_compare_not_bigger_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_compare, dnx_data_field_compare_define_not_bigger_offset);
}

uint32
dnx_data_field_compare_nof_keys_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_compare, dnx_data_field_compare_define_nof_keys);
}

uint32
dnx_data_field_compare_nof_compare_pairs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_compare, dnx_data_field_compare_define_nof_compare_pairs);
}










static shr_error_e
dnx_data_field_diag_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "diag";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_diag_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field diag features");

    submodule_data->features[dnx_data_field_diag_tcam_cs_hit_bit_support].name = "tcam_cs_hit_bit_support";
    submodule_data->features[dnx_data_field_diag_tcam_cs_hit_bit_support].doc = "";
    submodule_data->features[dnx_data_field_diag_tcam_cs_hit_bit_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_field_diag_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field diag defines");

    submodule_data->defines[dnx_data_field_diag_define_bytes_to_remove_mask].name = "bytes_to_remove_mask";
    submodule_data->defines[dnx_data_field_diag_define_bytes_to_remove_mask].doc = "";
    
    submodule_data->defines[dnx_data_field_diag_define_bytes_to_remove_mask].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_diag_define_layers_to_remove_size_in_bit].name = "layers_to_remove_size_in_bit";
    submodule_data->defines[dnx_data_field_diag_define_layers_to_remove_size_in_bit].doc = "";
    
    submodule_data->defines[dnx_data_field_diag_define_layers_to_remove_size_in_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_diag_define_layers_to_remove_mask].name = "layers_to_remove_mask";
    submodule_data->defines[dnx_data_field_diag_define_layers_to_remove_mask].doc = "";
    
    submodule_data->defines[dnx_data_field_diag_define_layers_to_remove_mask].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_diag_define_nof_signals_per_action].name = "nof_signals_per_action";
    submodule_data->defines[dnx_data_field_diag_define_nof_signals_per_action].doc = "";
    
    submodule_data->defines[dnx_data_field_diag_define_nof_signals_per_action].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_diag_define_nof_signals_per_qualifier].name = "nof_signals_per_qualifier";
    submodule_data->defines[dnx_data_field_diag_define_nof_signals_per_qualifier].doc = "";
    
    submodule_data->defines[dnx_data_field_diag_define_nof_signals_per_qualifier].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_diag_define_key_signal_size_in_bytes].name = "key_signal_size_in_bytes";
    submodule_data->defines[dnx_data_field_diag_define_key_signal_size_in_bytes].doc = "";
    
    submodule_data->defines[dnx_data_field_diag_define_key_signal_size_in_bytes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_diag_define_result_signal_size_in_words].name = "result_signal_size_in_words";
    submodule_data->defines[dnx_data_field_diag_define_result_signal_size_in_words].doc = "";
    
    submodule_data->defines[dnx_data_field_diag_define_result_signal_size_in_words].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_diag_define_dt_result_signal_size_in_words].name = "dt_result_signal_size_in_words";
    submodule_data->defines[dnx_data_field_diag_define_dt_result_signal_size_in_words].doc = "";
    
    submodule_data->defines[dnx_data_field_diag_define_dt_result_signal_size_in_words].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_diag_define_hit_signal_size_in_words].name = "hit_signal_size_in_words";
    submodule_data->defines[dnx_data_field_diag_define_hit_signal_size_in_words].doc = "";
    
    submodule_data->defines[dnx_data_field_diag_define_hit_signal_size_in_words].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_diag_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field diag tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_diag_feature_get(
    int unit,
    dnx_data_field_diag_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_diag, feature);
}


uint32
dnx_data_field_diag_bytes_to_remove_mask_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_diag, dnx_data_field_diag_define_bytes_to_remove_mask);
}

uint32
dnx_data_field_diag_layers_to_remove_size_in_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_diag, dnx_data_field_diag_define_layers_to_remove_size_in_bit);
}

uint32
dnx_data_field_diag_layers_to_remove_mask_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_diag, dnx_data_field_diag_define_layers_to_remove_mask);
}

uint32
dnx_data_field_diag_nof_signals_per_action_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_diag, dnx_data_field_diag_define_nof_signals_per_action);
}

uint32
dnx_data_field_diag_nof_signals_per_qualifier_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_diag, dnx_data_field_diag_define_nof_signals_per_qualifier);
}

uint32
dnx_data_field_diag_key_signal_size_in_bytes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_diag, dnx_data_field_diag_define_key_signal_size_in_bytes);
}

uint32
dnx_data_field_diag_result_signal_size_in_words_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_diag, dnx_data_field_diag_define_result_signal_size_in_words);
}

uint32
dnx_data_field_diag_dt_result_signal_size_in_words_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_diag, dnx_data_field_diag_define_dt_result_signal_size_in_words);
}

uint32
dnx_data_field_diag_hit_signal_size_in_words_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_diag, dnx_data_field_diag_define_hit_signal_size_in_words);
}










static shr_error_e
dnx_data_field_general_data_qualifiers_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general_data_qualifiers";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_general_data_qualifiers_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field general_data_qualifiers features");

    
    submodule_data->nof_defines = _dnx_data_field_general_data_qualifiers_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field general_data_qualifiers defines");

    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_vw_supported].name = "vw_supported";
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_vw_supported].doc = "";
    
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_vw_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_ac_in_lif_wide_data_nof_bits].name = "ac_in_lif_wide_data_nof_bits";
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_ac_in_lif_wide_data_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_ac_in_lif_wide_data_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_pwe_in_lif_wide_data_nof_bits].name = "pwe_in_lif_wide_data_nof_bits";
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_pwe_in_lif_wide_data_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_pwe_in_lif_wide_data_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_ip_tunnel_in_lif_wide_data_nof_bits].name = "ip_tunnel_in_lif_wide_data_nof_bits";
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_ip_tunnel_in_lif_wide_data_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_ip_tunnel_in_lif_wide_data_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_native_ac_in_lif_wide_data_nof_bits].name = "native_ac_in_lif_wide_data_nof_bits";
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_native_ac_in_lif_wide_data_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_native_ac_in_lif_wide_data_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_ac_in_lif_wide_data_extended_nof_bits].name = "ac_in_lif_wide_data_extended_nof_bits";
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_ac_in_lif_wide_data_extended_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_ac_in_lif_wide_data_extended_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_mact_entry_grouping_nof_bits].name = "mact_entry_grouping_nof_bits";
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_mact_entry_grouping_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_mact_entry_grouping_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_vw_vip_valid_nof_bits].name = "vw_vip_valid_nof_bits";
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_vw_vip_valid_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_vw_vip_valid_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_vw_vip_id_nof_bits].name = "vw_vip_id_nof_bits";
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_vw_vip_id_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_vw_vip_id_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_vw_member_reference_nof_bits].name = "vw_member_reference_nof_bits";
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_vw_member_reference_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_vw_member_reference_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_vw_pcc_hit_nof_bits].name = "vw_pcc_hit_nof_bits";
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_vw_pcc_hit_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_vw_pcc_hit_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_l3srcbind_hit_nof_bits].name = "l3srcbind_hit_nof_bits";
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_l3srcbind_hit_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_l3srcbind_hit_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_srv6_next_si].name = "srv6_next_si";
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_srv6_next_si].doc = "";
    
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_srv6_next_si].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_srv6_next_gsid].name = "srv6_next_gsid";
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_srv6_next_gsid].doc = "";
    
    submodule_data->defines[dnx_data_field_general_data_qualifiers_define_srv6_next_gsid].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_general_data_qualifiers_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field general_data_qualifiers tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_general_data_qualifiers_feature_get(
    int unit,
    dnx_data_field_general_data_qualifiers_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_general_data_qualifiers, feature);
}


uint32
dnx_data_field_general_data_qualifiers_vw_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_general_data_qualifiers, dnx_data_field_general_data_qualifiers_define_vw_supported);
}

uint32
dnx_data_field_general_data_qualifiers_ac_in_lif_wide_data_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_general_data_qualifiers, dnx_data_field_general_data_qualifiers_define_ac_in_lif_wide_data_nof_bits);
}

uint32
dnx_data_field_general_data_qualifiers_pwe_in_lif_wide_data_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_general_data_qualifiers, dnx_data_field_general_data_qualifiers_define_pwe_in_lif_wide_data_nof_bits);
}

uint32
dnx_data_field_general_data_qualifiers_ip_tunnel_in_lif_wide_data_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_general_data_qualifiers, dnx_data_field_general_data_qualifiers_define_ip_tunnel_in_lif_wide_data_nof_bits);
}

uint32
dnx_data_field_general_data_qualifiers_native_ac_in_lif_wide_data_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_general_data_qualifiers, dnx_data_field_general_data_qualifiers_define_native_ac_in_lif_wide_data_nof_bits);
}

uint32
dnx_data_field_general_data_qualifiers_ac_in_lif_wide_data_extended_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_general_data_qualifiers, dnx_data_field_general_data_qualifiers_define_ac_in_lif_wide_data_extended_nof_bits);
}

uint32
dnx_data_field_general_data_qualifiers_mact_entry_grouping_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_general_data_qualifiers, dnx_data_field_general_data_qualifiers_define_mact_entry_grouping_nof_bits);
}

uint32
dnx_data_field_general_data_qualifiers_vw_vip_valid_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_general_data_qualifiers, dnx_data_field_general_data_qualifiers_define_vw_vip_valid_nof_bits);
}

uint32
dnx_data_field_general_data_qualifiers_vw_vip_id_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_general_data_qualifiers, dnx_data_field_general_data_qualifiers_define_vw_vip_id_nof_bits);
}

uint32
dnx_data_field_general_data_qualifiers_vw_member_reference_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_general_data_qualifiers, dnx_data_field_general_data_qualifiers_define_vw_member_reference_nof_bits);
}

uint32
dnx_data_field_general_data_qualifiers_vw_pcc_hit_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_general_data_qualifiers, dnx_data_field_general_data_qualifiers_define_vw_pcc_hit_nof_bits);
}

uint32
dnx_data_field_general_data_qualifiers_l3srcbind_hit_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_general_data_qualifiers, dnx_data_field_general_data_qualifiers_define_l3srcbind_hit_nof_bits);
}

uint32
dnx_data_field_general_data_qualifiers_srv6_next_si_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_general_data_qualifiers, dnx_data_field_general_data_qualifiers_define_srv6_next_si);
}

uint32
dnx_data_field_general_data_qualifiers_srv6_next_gsid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_general_data_qualifiers, dnx_data_field_general_data_qualifiers_define_srv6_next_gsid);
}










static shr_error_e
dnx_data_field_common_max_val_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "common_max_val";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_common_max_val_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field common_max_val features");

    
    submodule_data->nof_defines = _dnx_data_field_common_max_val_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field common_max_val defines");

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_ffc].name = "nof_ffc";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_ffc].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_ffc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_masks_per_fes].name = "nof_masks_per_fes";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_masks_per_fes].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_masks_per_fes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fes_id_per_array].name = "nof_fes_id_per_array";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fes_id_per_array].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fes_id_per_array].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fes_array].name = "nof_fes_array";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fes_array].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fes_array].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fes_instruction_per_context].name = "nof_fes_instruction_per_context";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fes_instruction_per_context].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fes_instruction_per_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fes_programs].name = "nof_fes_programs";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fes_programs].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fes_programs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_prog_per_fes].name = "nof_prog_per_fes";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_prog_per_fes].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_prog_per_fes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_cs_lines].name = "nof_cs_lines";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_cs_lines].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_cs_lines].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_contexts].name = "nof_contexts";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_contexts].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_contexts].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_actions].name = "nof_actions";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_actions].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_actions].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_qualifiers].name = "nof_qualifiers";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_qualifiers].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_qualifiers].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_bits_in_fes_action].name = "nof_bits_in_fes_action";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_bits_in_fes_action].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_bits_in_fes_action].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_bits_in_fes_key_select].name = "nof_bits_in_fes_key_select";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_bits_in_fes_key_select].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_bits_in_fes_key_select].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_bits_in_ffc].name = "nof_bits_in_ffc";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_bits_in_ffc].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_bits_in_ffc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_ffc_in_uint32].name = "nof_ffc_in_uint32";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_ffc_in_uint32].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_ffc_in_uint32].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_action_per_group].name = "nof_action_per_group";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_action_per_group].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_action_per_group].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_layer_records].name = "nof_layer_records";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_layer_records].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_layer_records].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_layer_record_size].name = "layer_record_size";
    submodule_data->defines[dnx_data_field_common_max_val_define_layer_record_size].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_layer_record_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_l4_ops_ranges_legacy].name = "nof_l4_ops_ranges_legacy";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_l4_ops_ranges_legacy].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_l4_ops_ranges_legacy].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_pkt_hdr_ranges].name = "nof_pkt_hdr_ranges";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_pkt_hdr_ranges].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_pkt_hdr_ranges].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_out_lif_ranges].name = "nof_out_lif_ranges";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_out_lif_ranges].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_out_lif_ranges].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_kbr_size].name = "kbr_size";
    submodule_data->defines[dnx_data_field_common_max_val_define_kbr_size].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_kbr_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_compare_pairs_in_compare_mode].name = "nof_compare_pairs_in_compare_mode";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_compare_pairs_in_compare_mode].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_compare_pairs_in_compare_mode].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_bits_in_fem_action].name = "nof_bits_in_fem_action";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_bits_in_fem_action].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_bits_in_fem_action].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fem_condition].name = "nof_fem_condition";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fem_condition].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fem_condition].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fem_map_index].name = "nof_fem_map_index";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fem_map_index].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fem_map_index].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fem_programs].name = "nof_fem_programs";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fem_programs].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fem_programs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fem_id].name = "nof_fem_id";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fem_id].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_fem_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_nof_array_ids].name = "nof_array_ids";
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_array_ids].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_nof_array_ids].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_common_max_val_define_dbal_pairs].name = "dbal_pairs";
    submodule_data->defines[dnx_data_field_common_max_val_define_dbal_pairs].doc = "";
    
    submodule_data->defines[dnx_data_field_common_max_val_define_dbal_pairs].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_common_max_val_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field common_max_val tables");

    
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].name = "array_id_type";
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].doc = "";
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].size_of_values = sizeof(dnx_data_field_common_max_val_array_id_type_t);
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].entry_get = dnx_data_field_common_max_val_array_id_type_entry_str_get;

    
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].nof_keys = 1;
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].keys[0].name = "array_id";
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].values, dnxc_data_value_t, submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].nof_values, "_dnx_data_field_common_max_val_table_array_id_type table values");
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].values[0].name = "is_fes";
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].values[0].type = "int";
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].values[0].doc = "Non-zero if 'array_id' is for FESes. Zero otherwise";
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].values[0].offset = UTILEX_OFFSETOF(dnx_data_field_common_max_val_array_id_type_t, is_fes);
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].values[1].name = "is_fem";
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].values[1].type = "int";
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].values[1].doc = "Non-zero if 'array_id' is for FEMs. Zero otherwise";
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].values[1].offset = UTILEX_OFFSETOF(dnx_data_field_common_max_val_array_id_type_t, is_fem);
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].values[2].name = "fes_array_index";
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].values[2].type = "int";
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].values[2].doc = "Converts 'array_id' to number of FES arrays so far. 'array_id' 0 converts to 0, 2 converts to 1";
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].values[2].offset = UTILEX_OFFSETOF(dnx_data_field_common_max_val_array_id_type_t, fes_array_index);
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].values[3].name = "fem_array_index";
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].values[3].type = "int";
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].values[3].doc = "Converts 'array_id' to number of FEM arrays so far. 'array_id' 1 converts to 0, 3 converts to 1";
    submodule_data->tables[dnx_data_field_common_max_val_table_array_id_type].values[3].offset = UTILEX_OFFSETOF(dnx_data_field_common_max_val_array_id_type_t, fem_array_index);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_common_max_val_feature_get(
    int unit,
    dnx_data_field_common_max_val_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, feature);
}


uint32
dnx_data_field_common_max_val_nof_ffc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_ffc);
}

uint32
dnx_data_field_common_max_val_nof_masks_per_fes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_masks_per_fes);
}

uint32
dnx_data_field_common_max_val_nof_fes_id_per_array_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_fes_id_per_array);
}

uint32
dnx_data_field_common_max_val_nof_fes_array_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_fes_array);
}

uint32
dnx_data_field_common_max_val_nof_fes_instruction_per_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_fes_instruction_per_context);
}

uint32
dnx_data_field_common_max_val_nof_fes_programs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_fes_programs);
}

uint32
dnx_data_field_common_max_val_nof_prog_per_fes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_prog_per_fes);
}

uint32
dnx_data_field_common_max_val_nof_cs_lines_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_cs_lines);
}

uint32
dnx_data_field_common_max_val_nof_contexts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_contexts);
}

uint32
dnx_data_field_common_max_val_nof_actions_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_actions);
}

uint32
dnx_data_field_common_max_val_nof_qualifiers_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_qualifiers);
}

uint32
dnx_data_field_common_max_val_nof_bits_in_fes_action_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_bits_in_fes_action);
}

uint32
dnx_data_field_common_max_val_nof_bits_in_fes_key_select_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_bits_in_fes_key_select);
}

uint32
dnx_data_field_common_max_val_nof_bits_in_ffc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_bits_in_ffc);
}

uint32
dnx_data_field_common_max_val_nof_ffc_in_uint32_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_ffc_in_uint32);
}

uint32
dnx_data_field_common_max_val_nof_action_per_group_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_action_per_group);
}

uint32
dnx_data_field_common_max_val_nof_layer_records_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_layer_records);
}

uint32
dnx_data_field_common_max_val_layer_record_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_layer_record_size);
}

uint32
dnx_data_field_common_max_val_nof_l4_ops_ranges_legacy_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_l4_ops_ranges_legacy);
}

uint32
dnx_data_field_common_max_val_nof_pkt_hdr_ranges_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_pkt_hdr_ranges);
}

uint32
dnx_data_field_common_max_val_nof_out_lif_ranges_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_out_lif_ranges);
}

uint32
dnx_data_field_common_max_val_kbr_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_kbr_size);
}

uint32
dnx_data_field_common_max_val_nof_compare_pairs_in_compare_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_compare_pairs_in_compare_mode);
}

uint32
dnx_data_field_common_max_val_nof_bits_in_fem_action_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_bits_in_fem_action);
}

uint32
dnx_data_field_common_max_val_nof_fem_condition_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_fem_condition);
}

uint32
dnx_data_field_common_max_val_nof_fem_map_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_fem_map_index);
}

uint32
dnx_data_field_common_max_val_nof_fem_programs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_fem_programs);
}

uint32
dnx_data_field_common_max_val_nof_fem_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_fem_id);
}

uint32
dnx_data_field_common_max_val_nof_array_ids_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_nof_array_ids);
}

uint32
dnx_data_field_common_max_val_dbal_pairs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_define_dbal_pairs);
}



const dnx_data_field_common_max_val_array_id_type_t *
dnx_data_field_common_max_val_array_id_type_get(
    int unit,
    int array_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_table_array_id_type);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, array_id, 0);
    return (const dnx_data_field_common_max_val_array_id_type_t *) data;

}


shr_error_e
dnx_data_field_common_max_val_array_id_type_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_field_common_max_val_array_id_type_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_table_array_id_type);
    data = (const dnx_data_field_common_max_val_array_id_type_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_fes);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_fem);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fes_array_index);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fem_array_index);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_field_common_max_val_array_id_type_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_field, dnx_data_field_submodule_common_max_val, dnx_data_field_common_max_val_table_array_id_type);

}






static shr_error_e
dnx_data_field_init_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "init";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_init_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field init features");

    
    submodule_data->nof_defines = _dnx_data_field_init_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field init defines");

    submodule_data->defines[dnx_data_field_init_define_fec_dest].name = "fec_dest";
    submodule_data->defines[dnx_data_field_init_define_fec_dest].doc = "";
    
    submodule_data->defines[dnx_data_field_init_define_fec_dest].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_init_define_l4_trap].name = "l4_trap";
    submodule_data->defines[dnx_data_field_init_define_l4_trap].doc = "";
    
    submodule_data->defines[dnx_data_field_init_define_l4_trap].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_init_define_oam_layer_index].name = "oam_layer_index";
    submodule_data->defines[dnx_data_field_init_define_oam_layer_index].doc = "";
    
    submodule_data->defines[dnx_data_field_init_define_oam_layer_index].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_init_define_oam_stat].name = "oam_stat";
    submodule_data->defines[dnx_data_field_init_define_oam_stat].doc = "";
    
    submodule_data->defines[dnx_data_field_init_define_oam_stat].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_init_define_roo].name = "roo";
    submodule_data->defines[dnx_data_field_init_define_roo].doc = "";
    
    submodule_data->defines[dnx_data_field_init_define_roo].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_init_define_disable_erpp_counters].name = "disable_erpp_counters";
    submodule_data->defines[dnx_data_field_init_define_disable_erpp_counters].doc = "";
    
    submodule_data->defines[dnx_data_field_init_define_disable_erpp_counters].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_init_define_jr1_ipmc_inlif].name = "jr1_ipmc_inlif";
    submodule_data->defines[dnx_data_field_init_define_jr1_ipmc_inlif].doc = "";
    
    submodule_data->defines[dnx_data_field_init_define_jr1_ipmc_inlif].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_init_define_j1_same_port].name = "j1_same_port";
    submodule_data->defines[dnx_data_field_init_define_j1_same_port].doc = "";
    
    submodule_data->defines[dnx_data_field_init_define_j1_same_port].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_init_define_j1_learning].name = "j1_learning";
    submodule_data->defines[dnx_data_field_init_define_j1_learning].doc = "";
    
    submodule_data->defines[dnx_data_field_init_define_j1_learning].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_init_define_learn_limit].name = "learn_limit";
    submodule_data->defines[dnx_data_field_init_define_learn_limit].doc = "";
    
    submodule_data->defines[dnx_data_field_init_define_learn_limit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_init_define_j1_php].name = "j1_php";
    submodule_data->defines[dnx_data_field_init_define_j1_php].doc = "";
    
    submodule_data->defines[dnx_data_field_init_define_j1_php].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_init_define_ftmh_mc].name = "ftmh_mc";
    submodule_data->defines[dnx_data_field_init_define_ftmh_mc].doc = "";
    
    submodule_data->defines[dnx_data_field_init_define_ftmh_mc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_init_define_flow_id].name = "flow_id";
    submodule_data->defines[dnx_data_field_init_define_flow_id].doc = "";
    
    submodule_data->defines[dnx_data_field_init_define_flow_id].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_init_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field init tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_init_feature_get(
    int unit,
    dnx_data_field_init_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_init, feature);
}


uint32
dnx_data_field_init_fec_dest_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_init, dnx_data_field_init_define_fec_dest);
}

uint32
dnx_data_field_init_l4_trap_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_init, dnx_data_field_init_define_l4_trap);
}

uint32
dnx_data_field_init_oam_layer_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_init, dnx_data_field_init_define_oam_layer_index);
}

uint32
dnx_data_field_init_oam_stat_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_init, dnx_data_field_init_define_oam_stat);
}

uint32
dnx_data_field_init_roo_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_init, dnx_data_field_init_define_roo);
}

uint32
dnx_data_field_init_disable_erpp_counters_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_init, dnx_data_field_init_define_disable_erpp_counters);
}

uint32
dnx_data_field_init_jr1_ipmc_inlif_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_init, dnx_data_field_init_define_jr1_ipmc_inlif);
}

uint32
dnx_data_field_init_j1_same_port_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_init, dnx_data_field_init_define_j1_same_port);
}

uint32
dnx_data_field_init_j1_learning_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_init, dnx_data_field_init_define_j1_learning);
}

uint32
dnx_data_field_init_learn_limit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_init, dnx_data_field_init_define_learn_limit);
}

uint32
dnx_data_field_init_j1_php_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_init, dnx_data_field_init_define_j1_php);
}

uint32
dnx_data_field_init_ftmh_mc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_init, dnx_data_field_init_define_ftmh_mc);
}

uint32
dnx_data_field_init_flow_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_init, dnx_data_field_init_define_flow_id);
}










static shr_error_e
dnx_data_field_features_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "features";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_features_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field features features");

    
    submodule_data->nof_defines = _dnx_data_field_features_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field features defines");

    submodule_data->defines[dnx_data_field_features_define_tcam_cs_inlif_profile_supported].name = "tcam_cs_inlif_profile_supported";
    submodule_data->defines[dnx_data_field_features_define_tcam_cs_inlif_profile_supported].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_tcam_cs_inlif_profile_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_tcam_cs_is_tcam_direct_access].name = "tcam_cs_is_tcam_direct_access";
    submodule_data->defines[dnx_data_field_features_define_tcam_cs_is_tcam_direct_access].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_tcam_cs_is_tcam_direct_access].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_ecc_enable].name = "ecc_enable";
    submodule_data->defines[dnx_data_field_features_define_ecc_enable].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_ecc_enable].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_switch_to_acl_context].name = "switch_to_acl_context";
    submodule_data->defines[dnx_data_field_features_define_switch_to_acl_context].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_switch_to_acl_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_per_pp_port_pmf_profile_cs_offset].name = "per_pp_port_pmf_profile_cs_offset";
    submodule_data->defines[dnx_data_field_features_define_per_pp_port_pmf_profile_cs_offset].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_per_pp_port_pmf_profile_cs_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_tcam_result_flip_eco].name = "tcam_result_flip_eco";
    submodule_data->defines[dnx_data_field_features_define_tcam_result_flip_eco].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_tcam_result_flip_eco].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_tcam_result_half_payload_on_msb].name = "tcam_result_half_payload_on_msb";
    submodule_data->defines[dnx_data_field_features_define_tcam_result_half_payload_on_msb].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_tcam_result_half_payload_on_msb].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_parsing_start_offset_msb_meaningless].name = "parsing_start_offset_msb_meaningless";
    submodule_data->defines[dnx_data_field_features_define_parsing_start_offset_msb_meaningless].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_parsing_start_offset_msb_meaningless].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_kbp_opcode_in_ipmf1_cs].name = "kbp_opcode_in_ipmf1_cs";
    submodule_data->defines[dnx_data_field_features_define_kbp_opcode_in_ipmf1_cs].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_kbp_opcode_in_ipmf1_cs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_kbp_hitbits_correct_in_ipmf1_cs].name = "kbp_hitbits_correct_in_ipmf1_cs";
    submodule_data->defines[dnx_data_field_features_define_kbp_hitbits_correct_in_ipmf1_cs].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_kbp_hitbits_correct_in_ipmf1_cs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_exem_vmv_removable_from_payload].name = "exem_vmv_removable_from_payload";
    submodule_data->defines[dnx_data_field_features_define_exem_vmv_removable_from_payload].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_exem_vmv_removable_from_payload].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_multiple_dynamic_mem_enablers].name = "multiple_dynamic_mem_enablers";
    submodule_data->defines[dnx_data_field_features_define_multiple_dynamic_mem_enablers].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_multiple_dynamic_mem_enablers].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_aacl_super_group_handler_enable].name = "aacl_super_group_handler_enable";
    submodule_data->defines[dnx_data_field_features_define_aacl_super_group_handler_enable].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_aacl_super_group_handler_enable].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_aacl_tcam_swap_enable].name = "aacl_tcam_swap_enable";
    submodule_data->defines[dnx_data_field_features_define_aacl_tcam_swap_enable].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_aacl_tcam_swap_enable].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_extended_l4_ops].name = "extended_l4_ops";
    submodule_data->defines[dnx_data_field_features_define_extended_l4_ops].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_extended_l4_ops].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_state_table_ipmf1_key_select].name = "state_table_ipmf1_key_select";
    submodule_data->defines[dnx_data_field_features_define_state_table_ipmf1_key_select].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_state_table_ipmf1_key_select].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_state_table_acr_bus].name = "state_table_acr_bus";
    submodule_data->defines[dnx_data_field_features_define_state_table_acr_bus].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_state_table_acr_bus].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_state_table_atomic_rmw].name = "state_table_atomic_rmw";
    submodule_data->defines[dnx_data_field_features_define_state_table_atomic_rmw].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_state_table_atomic_rmw].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_hitbit_volatile].name = "hitbit_volatile";
    submodule_data->defines[dnx_data_field_features_define_hitbit_volatile].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_hitbit_volatile].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_fem_replace_msb_instead_lsb].name = "fem_replace_msb_instead_lsb";
    submodule_data->defines[dnx_data_field_features_define_fem_replace_msb_instead_lsb].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_fem_replace_msb_instead_lsb].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_dir_ext_epmf].name = "dir_ext_epmf";
    submodule_data->defines[dnx_data_field_features_define_dir_ext_epmf].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_dir_ext_epmf].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_hashing_process_lsb_to_msb].name = "hashing_process_lsb_to_msb";
    submodule_data->defines[dnx_data_field_features_define_hashing_process_lsb_to_msb].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_hashing_process_lsb_to_msb].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_exem_age_flush_scan].name = "exem_age_flush_scan";
    submodule_data->defines[dnx_data_field_features_define_exem_age_flush_scan].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_exem_age_flush_scan].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_epmf_has_lookup_enabler].name = "epmf_has_lookup_enabler";
    submodule_data->defines[dnx_data_field_features_define_epmf_has_lookup_enabler].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_epmf_has_lookup_enabler].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_epmf_outlif_profile_map].name = "epmf_outlif_profile_map";
    submodule_data->defines[dnx_data_field_features_define_epmf_outlif_profile_map].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_epmf_outlif_profile_map].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_epmf_inlif_profile_map].name = "epmf_inlif_profile_map";
    submodule_data->defines[dnx_data_field_features_define_epmf_inlif_profile_map].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_epmf_inlif_profile_map].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_tcam_full_key_half_payload_both_lsb_msb].name = "tcam_full_key_half_payload_both_lsb_msb";
    submodule_data->defines[dnx_data_field_features_define_tcam_full_key_half_payload_both_lsb_msb].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_tcam_full_key_half_payload_both_lsb_msb].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_epmf_exem_debug_signal_move_zero_padding_from_msb_to_lsb].name = "epmf_exem_debug_signal_move_zero_padding_from_msb_to_lsb";
    submodule_data->defines[dnx_data_field_features_define_epmf_exem_debug_signal_move_zero_padding_from_msb_to_lsb].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_epmf_exem_debug_signal_move_zero_padding_from_msb_to_lsb].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_ecmp_tunnel_priority].name = "ecmp_tunnel_priority";
    submodule_data->defines[dnx_data_field_features_define_ecmp_tunnel_priority].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_ecmp_tunnel_priority].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_result_signal_exists].name = "result_signal_exists";
    submodule_data->defines[dnx_data_field_features_define_result_signal_exists].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_result_signal_exists].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_apptype_based_on_fwd2].name = "apptype_based_on_fwd2";
    submodule_data->defines[dnx_data_field_features_define_apptype_based_on_fwd2].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_apptype_based_on_fwd2].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_udh_base_cs_is_mapped].name = "udh_base_cs_is_mapped";
    submodule_data->defines[dnx_data_field_features_define_udh_base_cs_is_mapped].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_udh_base_cs_is_mapped].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_state_collection_engine].name = "state_collection_engine";
    submodule_data->defines[dnx_data_field_features_define_state_collection_engine].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_state_collection_engine].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_features_define_empf_trap_action_is_strength_based].name = "empf_trap_action_is_strength_based";
    submodule_data->defines[dnx_data_field_features_define_empf_trap_action_is_strength_based].doc = "";
    
    submodule_data->defines[dnx_data_field_features_define_empf_trap_action_is_strength_based].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_features_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field features tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_features_feature_get(
    int unit,
    dnx_data_field_features_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, feature);
}


uint32
dnx_data_field_features_tcam_cs_inlif_profile_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_tcam_cs_inlif_profile_supported);
}

uint32
dnx_data_field_features_tcam_cs_is_tcam_direct_access_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_tcam_cs_is_tcam_direct_access);
}

uint32
dnx_data_field_features_ecc_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_ecc_enable);
}

uint32
dnx_data_field_features_switch_to_acl_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_switch_to_acl_context);
}

uint32
dnx_data_field_features_per_pp_port_pmf_profile_cs_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_per_pp_port_pmf_profile_cs_offset);
}

uint32
dnx_data_field_features_tcam_result_flip_eco_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_tcam_result_flip_eco);
}

uint32
dnx_data_field_features_tcam_result_half_payload_on_msb_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_tcam_result_half_payload_on_msb);
}

uint32
dnx_data_field_features_parsing_start_offset_msb_meaningless_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_parsing_start_offset_msb_meaningless);
}

uint32
dnx_data_field_features_kbp_opcode_in_ipmf1_cs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_kbp_opcode_in_ipmf1_cs);
}

uint32
dnx_data_field_features_kbp_hitbits_correct_in_ipmf1_cs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_kbp_hitbits_correct_in_ipmf1_cs);
}

uint32
dnx_data_field_features_exem_vmv_removable_from_payload_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_exem_vmv_removable_from_payload);
}

uint32
dnx_data_field_features_multiple_dynamic_mem_enablers_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_multiple_dynamic_mem_enablers);
}

uint32
dnx_data_field_features_aacl_super_group_handler_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_aacl_super_group_handler_enable);
}

uint32
dnx_data_field_features_aacl_tcam_swap_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_aacl_tcam_swap_enable);
}

uint32
dnx_data_field_features_extended_l4_ops_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_extended_l4_ops);
}

uint32
dnx_data_field_features_state_table_ipmf1_key_select_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_state_table_ipmf1_key_select);
}

uint32
dnx_data_field_features_state_table_acr_bus_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_state_table_acr_bus);
}

uint32
dnx_data_field_features_state_table_atomic_rmw_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_state_table_atomic_rmw);
}

uint32
dnx_data_field_features_hitbit_volatile_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_hitbit_volatile);
}

uint32
dnx_data_field_features_fem_replace_msb_instead_lsb_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_fem_replace_msb_instead_lsb);
}

uint32
dnx_data_field_features_dir_ext_epmf_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_dir_ext_epmf);
}

uint32
dnx_data_field_features_hashing_process_lsb_to_msb_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_hashing_process_lsb_to_msb);
}

uint32
dnx_data_field_features_exem_age_flush_scan_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_exem_age_flush_scan);
}

uint32
dnx_data_field_features_epmf_has_lookup_enabler_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_epmf_has_lookup_enabler);
}

uint32
dnx_data_field_features_epmf_outlif_profile_map_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_epmf_outlif_profile_map);
}

uint32
dnx_data_field_features_epmf_inlif_profile_map_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_epmf_inlif_profile_map);
}

uint32
dnx_data_field_features_tcam_full_key_half_payload_both_lsb_msb_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_tcam_full_key_half_payload_both_lsb_msb);
}

uint32
dnx_data_field_features_epmf_exem_debug_signal_move_zero_padding_from_msb_to_lsb_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_epmf_exem_debug_signal_move_zero_padding_from_msb_to_lsb);
}

uint32
dnx_data_field_features_ecmp_tunnel_priority_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_ecmp_tunnel_priority);
}

uint32
dnx_data_field_features_result_signal_exists_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_result_signal_exists);
}

uint32
dnx_data_field_features_apptype_based_on_fwd2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_apptype_based_on_fwd2);
}

uint32
dnx_data_field_features_udh_base_cs_is_mapped_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_udh_base_cs_is_mapped);
}

uint32
dnx_data_field_features_state_collection_engine_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_state_collection_engine);
}

uint32
dnx_data_field_features_empf_trap_action_is_strength_based_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_features, dnx_data_field_features_define_empf_trap_action_is_strength_based);
}










static shr_error_e
dnx_data_field_signal_sizes_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "signal_sizes";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_signal_sizes_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field signal_sizes features");

    
    submodule_data->nof_defines = _dnx_data_field_signal_sizes_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field signal_sizes defines");

    submodule_data->defines[dnx_data_field_signal_sizes_define_dual_queue_size].name = "dual_queue_size";
    submodule_data->defines[dnx_data_field_signal_sizes_define_dual_queue_size].doc = "";
    
    submodule_data->defines[dnx_data_field_signal_sizes_define_dual_queue_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_signal_sizes_define_packet_header_size].name = "packet_header_size";
    submodule_data->defines[dnx_data_field_signal_sizes_define_packet_header_size].doc = "";
    
    submodule_data->defines[dnx_data_field_signal_sizes_define_packet_header_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_signal_sizes_define_ecn].name = "ecn";
    submodule_data->defines[dnx_data_field_signal_sizes_define_ecn].doc = "";
    
    submodule_data->defines[dnx_data_field_signal_sizes_define_ecn].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_signal_sizes_define_congestion_info].name = "congestion_info";
    submodule_data->defines[dnx_data_field_signal_sizes_define_congestion_info].doc = "";
    
    submodule_data->defines[dnx_data_field_signal_sizes_define_congestion_info].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_signal_sizes_define_parsing_start_offset_size].name = "parsing_start_offset_size";
    submodule_data->defines[dnx_data_field_signal_sizes_define_parsing_start_offset_size].doc = "";
    
    submodule_data->defines[dnx_data_field_signal_sizes_define_parsing_start_offset_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_signal_sizes_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field signal_sizes tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_signal_sizes_feature_get(
    int unit,
    dnx_data_field_signal_sizes_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_signal_sizes, feature);
}


uint32
dnx_data_field_signal_sizes_dual_queue_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_signal_sizes, dnx_data_field_signal_sizes_define_dual_queue_size);
}

uint32
dnx_data_field_signal_sizes_packet_header_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_signal_sizes, dnx_data_field_signal_sizes_define_packet_header_size);
}

uint32
dnx_data_field_signal_sizes_ecn_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_signal_sizes, dnx_data_field_signal_sizes_define_ecn);
}

uint32
dnx_data_field_signal_sizes_congestion_info_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_signal_sizes, dnx_data_field_signal_sizes_define_congestion_info);
}

uint32
dnx_data_field_signal_sizes_parsing_start_offset_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_signal_sizes, dnx_data_field_signal_sizes_define_parsing_start_offset_size);
}










static shr_error_e
dnx_data_field_dnx_data_internal_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dnx_data_internal";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_dnx_data_internal_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field dnx_data_internal features");

    
    submodule_data->nof_defines = _dnx_data_field_dnx_data_internal_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field dnx_data_internal defines");

    submodule_data->defines[dnx_data_field_dnx_data_internal_define_lr_eth_is_da_mac_valid].name = "lr_eth_is_da_mac_valid";
    submodule_data->defines[dnx_data_field_dnx_data_internal_define_lr_eth_is_da_mac_valid].doc = "";
    
    submodule_data->defines[dnx_data_field_dnx_data_internal_define_lr_eth_is_da_mac_valid].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_dnx_data_internal_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field dnx_data_internal tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_dnx_data_internal_feature_get(
    int unit,
    dnx_data_field_dnx_data_internal_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_dnx_data_internal, feature);
}


uint32
dnx_data_field_dnx_data_internal_lr_eth_is_da_mac_valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_dnx_data_internal, dnx_data_field_dnx_data_internal_define_lr_eth_is_da_mac_valid);
}










static shr_error_e
dnx_data_field_tests_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "tests";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_tests_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field tests features");

    
    submodule_data->nof_defines = _dnx_data_field_tests_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field tests defines");

    submodule_data->defines[dnx_data_field_tests_define_learn_info_actions_structure_change].name = "learn_info_actions_structure_change";
    submodule_data->defines[dnx_data_field_tests_define_learn_info_actions_structure_change].doc = "";
    
    submodule_data->defines[dnx_data_field_tests_define_learn_info_actions_structure_change].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tests_define_ingress_time_stamp_increased].name = "ingress_time_stamp_increased";
    submodule_data->defines[dnx_data_field_tests_define_ingress_time_stamp_increased].doc = "";
    
    submodule_data->defines[dnx_data_field_tests_define_ingress_time_stamp_increased].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tests_define_ace_debug_signals_exist].name = "ace_debug_signals_exist";
    submodule_data->defines[dnx_data_field_tests_define_ace_debug_signals_exist].doc = "";
    
    submodule_data->defines[dnx_data_field_tests_define_ace_debug_signals_exist].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tests_define_epmf_debug_rely_on_tcam0].name = "epmf_debug_rely_on_tcam0";
    submodule_data->defines[dnx_data_field_tests_define_epmf_debug_rely_on_tcam0].doc = "";
    
    submodule_data->defines[dnx_data_field_tests_define_epmf_debug_rely_on_tcam0].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tests_define_epmf_tm_port_cs].name = "epmf_tm_port_cs";
    submodule_data->defines[dnx_data_field_tests_define_epmf_tm_port_cs].doc = "";
    
    submodule_data->defines[dnx_data_field_tests_define_epmf_tm_port_cs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tests_define_small_exem_addr_bits].name = "small_exem_addr_bits";
    submodule_data->defines[dnx_data_field_tests_define_small_exem_addr_bits].doc = "";
    
    submodule_data->defines[dnx_data_field_tests_define_small_exem_addr_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tests_define_stub_adapter_supports_semantic_flush].name = "stub_adapter_supports_semantic_flush";
    submodule_data->defines[dnx_data_field_tests_define_stub_adapter_supports_semantic_flush].doc = "";
    
    submodule_data->defines[dnx_data_field_tests_define_stub_adapter_supports_semantic_flush].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_field_tests_define_parsing_start_offset_exists].name = "parsing_start_offset_exists";
    submodule_data->defines[dnx_data_field_tests_define_parsing_start_offset_exists].doc = "";
    
    submodule_data->defines[dnx_data_field_tests_define_parsing_start_offset_exists].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_field_tests_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field tests tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_tests_feature_get(
    int unit,
    dnx_data_field_tests_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tests, feature);
}


uint32
dnx_data_field_tests_learn_info_actions_structure_change_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tests, dnx_data_field_tests_define_learn_info_actions_structure_change);
}

uint32
dnx_data_field_tests_ingress_time_stamp_increased_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tests, dnx_data_field_tests_define_ingress_time_stamp_increased);
}

uint32
dnx_data_field_tests_ace_debug_signals_exist_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tests, dnx_data_field_tests_define_ace_debug_signals_exist);
}

uint32
dnx_data_field_tests_epmf_debug_rely_on_tcam0_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tests, dnx_data_field_tests_define_epmf_debug_rely_on_tcam0);
}

uint32
dnx_data_field_tests_epmf_tm_port_cs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tests, dnx_data_field_tests_define_epmf_tm_port_cs);
}

uint32
dnx_data_field_tests_small_exem_addr_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tests, dnx_data_field_tests_define_small_exem_addr_bits);
}

uint32
dnx_data_field_tests_stub_adapter_supports_semantic_flush_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tests, dnx_data_field_tests_define_stub_adapter_supports_semantic_flush);
}

uint32
dnx_data_field_tests_parsing_start_offset_exists_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_field, dnx_data_field_submodule_tests, dnx_data_field_tests_define_parsing_start_offset_exists);
}







shr_error_e
dnx_data_field_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "field";
    module_data->nof_submodules = _dnx_data_field_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data field submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_field_base_ipmf1_init(unit, &module_data->submodules[dnx_data_field_submodule_base_ipmf1]));
    SHR_IF_ERR_EXIT(dnx_data_field_base_ipmf2_init(unit, &module_data->submodules[dnx_data_field_submodule_base_ipmf2]));
    SHR_IF_ERR_EXIT(dnx_data_field_base_ipmf3_init(unit, &module_data->submodules[dnx_data_field_submodule_base_ipmf3]));
    SHR_IF_ERR_EXIT(dnx_data_field_base_epmf_init(unit, &module_data->submodules[dnx_data_field_submodule_base_epmf]));
    SHR_IF_ERR_EXIT(dnx_data_field_base_ifwd2_init(unit, &module_data->submodules[dnx_data_field_submodule_base_ifwd2]));
    SHR_IF_ERR_EXIT(dnx_data_field_stage_init(unit, &module_data->submodules[dnx_data_field_submodule_stage]));
    SHR_IF_ERR_EXIT(dnx_data_field_external_tcam_init(unit, &module_data->submodules[dnx_data_field_submodule_external_tcam]));
    SHR_IF_ERR_EXIT(dnx_data_field_tcam_init(unit, &module_data->submodules[dnx_data_field_submodule_tcam]));
    SHR_IF_ERR_EXIT(dnx_data_field_group_init(unit, &module_data->submodules[dnx_data_field_submodule_group]));
    SHR_IF_ERR_EXIT(dnx_data_field_efes_init(unit, &module_data->submodules[dnx_data_field_submodule_efes]));
    SHR_IF_ERR_EXIT(dnx_data_field_fem_init(unit, &module_data->submodules[dnx_data_field_submodule_fem]));
    SHR_IF_ERR_EXIT(dnx_data_field_context_init(unit, &module_data->submodules[dnx_data_field_submodule_context]));
    SHR_IF_ERR_EXIT(dnx_data_field_preselector_init(unit, &module_data->submodules[dnx_data_field_submodule_preselector]));
    SHR_IF_ERR_EXIT(dnx_data_field_qual_init(unit, &module_data->submodules[dnx_data_field_submodule_qual]));
    SHR_IF_ERR_EXIT(dnx_data_field_action_init(unit, &module_data->submodules[dnx_data_field_submodule_action]));
    SHR_IF_ERR_EXIT(dnx_data_field_virtual_wire_init(unit, &module_data->submodules[dnx_data_field_submodule_virtual_wire]));
    SHR_IF_ERR_EXIT(dnx_data_field_profile_bits_init(unit, &module_data->submodules[dnx_data_field_submodule_profile_bits]));
    SHR_IF_ERR_EXIT(dnx_data_field_dir_ext_init(unit, &module_data->submodules[dnx_data_field_submodule_dir_ext]));
    SHR_IF_ERR_EXIT(dnx_data_field_state_table_init(unit, &module_data->submodules[dnx_data_field_submodule_state_table]));
    SHR_IF_ERR_EXIT(dnx_data_field_sce_init(unit, &module_data->submodules[dnx_data_field_submodule_sce]));
    SHR_IF_ERR_EXIT(dnx_data_field_mdb_dt_init(unit, &module_data->submodules[dnx_data_field_submodule_mdb_dt]));
    SHR_IF_ERR_EXIT(dnx_data_field_hash_init(unit, &module_data->submodules[dnx_data_field_submodule_hash]));
    SHR_IF_ERR_EXIT(dnx_data_field_udh_init(unit, &module_data->submodules[dnx_data_field_submodule_udh]));
    SHR_IF_ERR_EXIT(dnx_data_field_system_headers_init(unit, &module_data->submodules[dnx_data_field_submodule_system_headers]));
    SHR_IF_ERR_EXIT(dnx_data_field_exem_init(unit, &module_data->submodules[dnx_data_field_submodule_exem]));
    SHR_IF_ERR_EXIT(dnx_data_field_exem_learn_flush_machine_init(unit, &module_data->submodules[dnx_data_field_submodule_exem_learn_flush_machine]));
    SHR_IF_ERR_EXIT(dnx_data_field_ace_init(unit, &module_data->submodules[dnx_data_field_submodule_ace]));
    SHR_IF_ERR_EXIT(dnx_data_field_entry_init(unit, &module_data->submodules[dnx_data_field_submodule_entry]));
    SHR_IF_ERR_EXIT(dnx_data_field_L4_Ops_init(unit, &module_data->submodules[dnx_data_field_submodule_L4_Ops]));
    SHR_IF_ERR_EXIT(dnx_data_field_encoded_qual_actions_offset_init(unit, &module_data->submodules[dnx_data_field_submodule_encoded_qual_actions_offset]));
    SHR_IF_ERR_EXIT(dnx_data_field_compare_init(unit, &module_data->submodules[dnx_data_field_submodule_compare]));
    SHR_IF_ERR_EXIT(dnx_data_field_diag_init(unit, &module_data->submodules[dnx_data_field_submodule_diag]));
    SHR_IF_ERR_EXIT(dnx_data_field_general_data_qualifiers_init(unit, &module_data->submodules[dnx_data_field_submodule_general_data_qualifiers]));
    SHR_IF_ERR_EXIT(dnx_data_field_common_max_val_init(unit, &module_data->submodules[dnx_data_field_submodule_common_max_val]));
    SHR_IF_ERR_EXIT(dnx_data_field_init_init(unit, &module_data->submodules[dnx_data_field_submodule_init]));
    SHR_IF_ERR_EXIT(dnx_data_field_features_init(unit, &module_data->submodules[dnx_data_field_submodule_features]));
    SHR_IF_ERR_EXIT(dnx_data_field_signal_sizes_init(unit, &module_data->submodules[dnx_data_field_submodule_signal_sizes]));
    SHR_IF_ERR_EXIT(dnx_data_field_dnx_data_internal_init(unit, &module_data->submodules[dnx_data_field_submodule_dnx_data_internal]));
    SHR_IF_ERR_EXIT(dnx_data_field_tests_init(unit, &module_data->submodules[dnx_data_field_submodule_tests]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_field_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_field_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_field_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_field_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_field_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_field_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_field_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_field_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_field_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_field_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_field_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

