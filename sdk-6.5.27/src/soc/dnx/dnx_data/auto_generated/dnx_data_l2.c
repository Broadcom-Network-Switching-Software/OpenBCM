
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L2

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_l2.h>




extern shr_error_e jr2_a0_data_l2_attach(
    int unit);


extern shr_error_e jr2_b0_data_l2_attach(
    int unit);


extern shr_error_e j2c_a0_data_l2_attach(
    int unit);


extern shr_error_e j2c_a1_data_l2_attach(
    int unit);


extern shr_error_e q2a_a0_data_l2_attach(
    int unit);


extern shr_error_e q2a_b0_data_l2_attach(
    int unit);


extern shr_error_e q2a_b1_data_l2_attach(
    int unit);


extern shr_error_e j2p_a0_data_l2_attach(
    int unit);


extern shr_error_e j2x_a0_data_l2_attach(
    int unit);




static shr_error_e
dnx_data_l2_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l2_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l2 general features");

    submodule_data->features[dnx_data_l2_general_learning_use_insert_cmd].name = "learning_use_insert_cmd";
    submodule_data->features[dnx_data_l2_general_learning_use_insert_cmd].doc = "";
    submodule_data->features[dnx_data_l2_general_learning_use_insert_cmd].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_general_flush_machine_support].name = "flush_machine_support";
    submodule_data->features[dnx_data_l2_general_flush_machine_support].doc = "";
    submodule_data->features[dnx_data_l2_general_flush_machine_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_general_dynamic_entries_iteration_support].name = "dynamic_entries_iteration_support";
    submodule_data->features[dnx_data_l2_general_dynamic_entries_iteration_support].doc = "";
    submodule_data->features[dnx_data_l2_general_dynamic_entries_iteration_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_general_counters_support].name = "counters_support";
    submodule_data->features[dnx_data_l2_general_counters_support].doc = "";
    submodule_data->features[dnx_data_l2_general_counters_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_general_aging_support].name = "aging_support";
    submodule_data->features[dnx_data_l2_general_aging_support].doc = "";
    submodule_data->features[dnx_data_l2_general_aging_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_general_transplant_over_stronger_support].name = "transplant_over_stronger_support";
    submodule_data->features[dnx_data_l2_general_transplant_over_stronger_support].doc = "";
    submodule_data->features[dnx_data_l2_general_transplant_over_stronger_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_general_ivl_feature_support].name = "ivl_feature_support";
    submodule_data->features[dnx_data_l2_general_ivl_feature_support].doc = "";
    submodule_data->features[dnx_data_l2_general_ivl_feature_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_general_dma_support].name = "dma_support";
    submodule_data->features[dnx_data_l2_general_dma_support].doc = "";
    submodule_data->features[dnx_data_l2_general_dma_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_general_separate_fwd_learn_mact].name = "separate_fwd_learn_mact";
    submodule_data->features[dnx_data_l2_general_separate_fwd_learn_mact].doc = "";
    submodule_data->features[dnx_data_l2_general_separate_fwd_learn_mact].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_general_SLLB_v1_support].name = "SLLB_v1_support";
    submodule_data->features[dnx_data_l2_general_SLLB_v1_support].doc = "";
    submodule_data->features[dnx_data_l2_general_SLLB_v1_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_general_enhanced_learn_key_generation].name = "enhanced_learn_key_generation";
    submodule_data->features[dnx_data_l2_general_enhanced_learn_key_generation].doc = "";
    submodule_data->features[dnx_data_l2_general_enhanced_learn_key_generation].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_general_full_learn_action_resolution_support].name = "full_learn_action_resolution_support";
    submodule_data->features[dnx_data_l2_general_full_learn_action_resolution_support].doc = "";
    submodule_data->features[dnx_data_l2_general_full_learn_action_resolution_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_l2_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l2 general defines");

    submodule_data->defines[dnx_data_l2_general_define_vsi_offset_shift].name = "vsi_offset_shift";
    submodule_data->defines[dnx_data_l2_general_define_vsi_offset_shift].doc = "";
    
    submodule_data->defines[dnx_data_l2_general_define_vsi_offset_shift].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_general_define_lif_offset_shift].name = "lif_offset_shift";
    submodule_data->defines[dnx_data_l2_general_define_lif_offset_shift].doc = "";
    
    submodule_data->defines[dnx_data_l2_general_define_lif_offset_shift].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_general_define_lem_nof_dbs].name = "lem_nof_dbs";
    submodule_data->defines[dnx_data_l2_general_define_lem_nof_dbs].doc = "";
    
    submodule_data->defines[dnx_data_l2_general_define_lem_nof_dbs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_general_define_l2_learn_limit_mode].name = "l2_learn_limit_mode";
    submodule_data->defines[dnx_data_l2_general_define_l2_learn_limit_mode].doc = "";
    
    submodule_data->defines[dnx_data_l2_general_define_l2_learn_limit_mode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l2_general_define_jr_mode_nof_fec_bits].name = "jr_mode_nof_fec_bits";
    submodule_data->defines[dnx_data_l2_general_define_jr_mode_nof_fec_bits].doc = "";
    
    submodule_data->defines[dnx_data_l2_general_define_jr_mode_nof_fec_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l2_general_define_arad_plus_mode_nof_fec_bits].name = "arad_plus_mode_nof_fec_bits";
    submodule_data->defines[dnx_data_l2_general_define_arad_plus_mode_nof_fec_bits].doc = "";
    
    submodule_data->defines[dnx_data_l2_general_define_arad_plus_mode_nof_fec_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l2_general_define_l2_egress_max_extention_size_bytes].name = "l2_egress_max_extention_size_bytes";
    submodule_data->defines[dnx_data_l2_general_define_l2_egress_max_extention_size_bytes].doc = "";
    
    submodule_data->defines[dnx_data_l2_general_define_l2_egress_max_extention_size_bytes].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l2_general_define_l2_egress_max_additional_termination_size_bytes].name = "l2_egress_max_additional_termination_size_bytes";
    submodule_data->defines[dnx_data_l2_general_define_l2_egress_max_additional_termination_size_bytes].doc = "";
    
    submodule_data->defines[dnx_data_l2_general_define_l2_egress_max_additional_termination_size_bytes].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l2_general_define_mact_mngmnt_fid_exceed_limit_int].name = "mact_mngmnt_fid_exceed_limit_int";
    submodule_data->defines[dnx_data_l2_general_define_mact_mngmnt_fid_exceed_limit_int].doc = "";
    
    submodule_data->defines[dnx_data_l2_general_define_mact_mngmnt_fid_exceed_limit_int].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l2_general_define_mact_lela_fid_exceed_limit_int].name = "mact_lela_fid_exceed_limit_int";
    submodule_data->defines[dnx_data_l2_general_define_mact_lela_fid_exceed_limit_int].doc = "";
    
    submodule_data->defines[dnx_data_l2_general_define_mact_lela_fid_exceed_limit_int].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l2_general_define_l2_api_supported_flags2].name = "l2_api_supported_flags2";
    submodule_data->defines[dnx_data_l2_general_define_l2_api_supported_flags2].doc = "";
    
    submodule_data->defines[dnx_data_l2_general_define_l2_api_supported_flags2].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l2_general_define_l2_traverse_api_supported_flags].name = "l2_traverse_api_supported_flags";
    submodule_data->defines[dnx_data_l2_general_define_l2_traverse_api_supported_flags].doc = "";
    
    submodule_data->defines[dnx_data_l2_general_define_l2_traverse_api_supported_flags].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l2_general_define_l2_delete_api_supported_flags].name = "l2_delete_api_supported_flags";
    submodule_data->defines[dnx_data_l2_general_define_l2_delete_api_supported_flags].doc = "";
    
    submodule_data->defines[dnx_data_l2_general_define_l2_delete_api_supported_flags].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l2_general_define_l2_add_get_apis_supported_flags].name = "l2_add_get_apis_supported_flags";
    submodule_data->defines[dnx_data_l2_general_define_l2_add_get_apis_supported_flags].doc = "";
    
    submodule_data->defines[dnx_data_l2_general_define_l2_add_get_apis_supported_flags].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_l2_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l2 general tables");

    
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].name = "mact_result_type_map";
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].doc = "";
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].size_of_values = sizeof(dnx_data_l2_general_mact_result_type_map_t);
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].entry_get = dnx_data_l2_general_mact_result_type_map_entry_str_get;

    
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].nof_keys = 2;
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].keys[0].name = "forward_learning_db";
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].keys[0].doc = "";
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].keys[1].name = "vlan_learning_mode";
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].nof_values = 5;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].nof_values, "_dnx_data_l2_general_table_mact_result_type_map table values");
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].values[0].name = "dest_lif_stat";
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].values[0].type = "uint32";
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].values[0].doc = "";
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_l2_general_mact_result_type_map_t, dest_lif_stat);
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].values[1].name = "dest_stat";
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].values[1].type = "uint32";
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].values[1].doc = "";
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].values[1].offset = UTILEX_OFFSETOF(dnx_data_l2_general_mact_result_type_map_t, dest_stat);
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].values[2].name = "eei";
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].values[2].type = "uint32";
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].values[2].doc = "";
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].values[2].offset = UTILEX_OFFSETOF(dnx_data_l2_general_mact_result_type_map_t, eei);
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].values[3].name = "no_outlif";
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].values[3].type = "uint32";
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].values[3].doc = "";
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].values[3].offset = UTILEX_OFFSETOF(dnx_data_l2_general_mact_result_type_map_t, no_outlif);
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].values[4].name = "single_outlif";
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].values[4].type = "uint32";
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].values[4].doc = "";
    submodule_data->tables[dnx_data_l2_general_table_mact_result_type_map].values[4].offset = UTILEX_OFFSETOF(dnx_data_l2_general_mact_result_type_map_t, single_outlif);

    
    submodule_data->tables[dnx_data_l2_general_table_scan_data_bases_info].name = "scan_data_bases_info";
    submodule_data->tables[dnx_data_l2_general_table_scan_data_bases_info].doc = "";
    submodule_data->tables[dnx_data_l2_general_table_scan_data_bases_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l2_general_table_scan_data_bases_info].size_of_values = sizeof(dnx_data_l2_general_scan_data_bases_info_t);
    submodule_data->tables[dnx_data_l2_general_table_scan_data_bases_info].entry_get = dnx_data_l2_general_scan_data_bases_info_entry_str_get;

    
    submodule_data->tables[dnx_data_l2_general_table_scan_data_bases_info].nof_keys = 1;
    submodule_data->tables[dnx_data_l2_general_table_scan_data_bases_info].keys[0].name = "data_base";
    submodule_data->tables[dnx_data_l2_general_table_scan_data_bases_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_l2_general_table_scan_data_bases_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l2_general_table_scan_data_bases_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l2_general_table_scan_data_bases_info].nof_values, "_dnx_data_l2_general_table_scan_data_bases_info table values");
    submodule_data->tables[dnx_data_l2_general_table_scan_data_bases_info].values[0].name = "age_scan";
    submodule_data->tables[dnx_data_l2_general_table_scan_data_bases_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_l2_general_table_scan_data_bases_info].values[0].doc = "";
    submodule_data->tables[dnx_data_l2_general_table_scan_data_bases_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_l2_general_scan_data_bases_info_t, age_scan);
    submodule_data->tables[dnx_data_l2_general_table_scan_data_bases_info].values[1].name = "scan_cycles";
    submodule_data->tables[dnx_data_l2_general_table_scan_data_bases_info].values[1].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_l2_general_table_scan_data_bases_info].values[1].doc = "";
    submodule_data->tables[dnx_data_l2_general_table_scan_data_bases_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_l2_general_scan_data_bases_info_t, scan_cycles);

    
    submodule_data->tables[dnx_data_l2_general_table_fwd_mact_info].name = "fwd_mact_info";
    submodule_data->tables[dnx_data_l2_general_table_fwd_mact_info].doc = "";
    submodule_data->tables[dnx_data_l2_general_table_fwd_mact_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l2_general_table_fwd_mact_info].size_of_values = sizeof(dnx_data_l2_general_fwd_mact_info_t);
    submodule_data->tables[dnx_data_l2_general_table_fwd_mact_info].entry_get = dnx_data_l2_general_fwd_mact_info_entry_str_get;

    
    submodule_data->tables[dnx_data_l2_general_table_fwd_mact_info].nof_keys = 2;
    submodule_data->tables[dnx_data_l2_general_table_fwd_mact_info].keys[0].name = "l2_action_type";
    submodule_data->tables[dnx_data_l2_general_table_fwd_mact_info].keys[0].doc = "";
    submodule_data->tables[dnx_data_l2_general_table_fwd_mact_info].keys[1].name = "is_ivl";
    submodule_data->tables[dnx_data_l2_general_table_fwd_mact_info].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_l2_general_table_fwd_mact_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l2_general_table_fwd_mact_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l2_general_table_fwd_mact_info].nof_values, "_dnx_data_l2_general_table_fwd_mact_info table values");
    submodule_data->tables[dnx_data_l2_general_table_fwd_mact_info].values[0].name = "logical_data_base";
    submodule_data->tables[dnx_data_l2_general_table_fwd_mact_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_l2_general_table_fwd_mact_info].values[0].doc = "";
    submodule_data->tables[dnx_data_l2_general_table_fwd_mact_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_l2_general_fwd_mact_info_t, logical_data_base);
    submodule_data->tables[dnx_data_l2_general_table_fwd_mact_info].values[1].name = "physical_data_base";
    submodule_data->tables[dnx_data_l2_general_table_fwd_mact_info].values[1].type = "dbal_physical_tables_e";
    submodule_data->tables[dnx_data_l2_general_table_fwd_mact_info].values[1].doc = "";
    submodule_data->tables[dnx_data_l2_general_table_fwd_mact_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_l2_general_fwd_mact_info_t, physical_data_base);

    
    submodule_data->tables[dnx_data_l2_general_table_lem_table_map_l2_action].name = "lem_table_map_l2_action";
    submodule_data->tables[dnx_data_l2_general_table_lem_table_map_l2_action].doc = "";
    submodule_data->tables[dnx_data_l2_general_table_lem_table_map_l2_action].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l2_general_table_lem_table_map_l2_action].size_of_values = sizeof(dnx_data_l2_general_lem_table_map_l2_action_t);
    submodule_data->tables[dnx_data_l2_general_table_lem_table_map_l2_action].entry_get = dnx_data_l2_general_lem_table_map_l2_action_entry_str_get;

    
    submodule_data->tables[dnx_data_l2_general_table_lem_table_map_l2_action].nof_keys = 1;
    submodule_data->tables[dnx_data_l2_general_table_lem_table_map_l2_action].keys[0].name = "logical_data_base";
    submodule_data->tables[dnx_data_l2_general_table_lem_table_map_l2_action].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_l2_general_table_lem_table_map_l2_action].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l2_general_table_lem_table_map_l2_action].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l2_general_table_lem_table_map_l2_action].nof_values, "_dnx_data_l2_general_table_lem_table_map_l2_action table values");
    submodule_data->tables[dnx_data_l2_general_table_lem_table_map_l2_action].values[0].name = "l2_action_type";
    submodule_data->tables[dnx_data_l2_general_table_lem_table_map_l2_action].values[0].type = "dnx_l2_action_type_t";
    submodule_data->tables[dnx_data_l2_general_table_lem_table_map_l2_action].values[0].doc = "";
    submodule_data->tables[dnx_data_l2_general_table_lem_table_map_l2_action].values[0].offset = UTILEX_OFFSETOF(dnx_data_l2_general_lem_table_map_l2_action_t, l2_action_type);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l2_general_feature_get(
    int unit,
    dnx_data_l2_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, feature);
}


uint32
dnx_data_l2_general_vsi_offset_shift_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_define_vsi_offset_shift);
}

uint32
dnx_data_l2_general_lif_offset_shift_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_define_lif_offset_shift);
}

uint32
dnx_data_l2_general_lem_nof_dbs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_define_lem_nof_dbs);
}

uint32
dnx_data_l2_general_l2_learn_limit_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_define_l2_learn_limit_mode);
}

uint32
dnx_data_l2_general_jr_mode_nof_fec_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_define_jr_mode_nof_fec_bits);
}

uint32
dnx_data_l2_general_arad_plus_mode_nof_fec_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_define_arad_plus_mode_nof_fec_bits);
}

uint32
dnx_data_l2_general_l2_egress_max_extention_size_bytes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_define_l2_egress_max_extention_size_bytes);
}

uint32
dnx_data_l2_general_l2_egress_max_additional_termination_size_bytes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_define_l2_egress_max_additional_termination_size_bytes);
}

uint32
dnx_data_l2_general_mact_mngmnt_fid_exceed_limit_int_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_define_mact_mngmnt_fid_exceed_limit_int);
}

uint32
dnx_data_l2_general_mact_lela_fid_exceed_limit_int_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_define_mact_lela_fid_exceed_limit_int);
}

uint32
dnx_data_l2_general_l2_api_supported_flags2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_define_l2_api_supported_flags2);
}

uint32
dnx_data_l2_general_l2_traverse_api_supported_flags_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_define_l2_traverse_api_supported_flags);
}

uint32
dnx_data_l2_general_l2_delete_api_supported_flags_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_define_l2_delete_api_supported_flags);
}

uint32
dnx_data_l2_general_l2_add_get_apis_supported_flags_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_define_l2_add_get_apis_supported_flags);
}



const dnx_data_l2_general_mact_result_type_map_t *
dnx_data_l2_general_mact_result_type_map_get(
    int unit,
    int forward_learning_db,
    int vlan_learning_mode)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_table_mact_result_type_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, forward_learning_db, vlan_learning_mode);
    return (const dnx_data_l2_general_mact_result_type_map_t *) data;

}

const dnx_data_l2_general_scan_data_bases_info_t *
dnx_data_l2_general_scan_data_bases_info_get(
    int unit,
    int data_base)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_table_scan_data_bases_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, data_base, 0);
    return (const dnx_data_l2_general_scan_data_bases_info_t *) data;

}

const dnx_data_l2_general_fwd_mact_info_t *
dnx_data_l2_general_fwd_mact_info_get(
    int unit,
    int l2_action_type,
    int is_ivl)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_table_fwd_mact_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, l2_action_type, is_ivl);
    return (const dnx_data_l2_general_fwd_mact_info_t *) data;

}

const dnx_data_l2_general_lem_table_map_l2_action_t *
dnx_data_l2_general_lem_table_map_l2_action_get(
    int unit,
    int logical_data_base)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_table_lem_table_map_l2_action);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, logical_data_base, 0);
    return (const dnx_data_l2_general_lem_table_map_l2_action_t *) data;

}


shr_error_e
dnx_data_l2_general_mact_result_type_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l2_general_mact_result_type_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_table_mact_result_type_map);
    data = (const dnx_data_l2_general_mact_result_type_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dest_lif_stat);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dest_stat);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->eei);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->no_outlif);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->single_outlif);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_l2_general_scan_data_bases_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l2_general_scan_data_bases_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_table_scan_data_bases_info);
    data = (const dnx_data_l2_general_scan_data_bases_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->age_scan);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->scan_cycles);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_l2_general_fwd_mact_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l2_general_fwd_mact_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_table_fwd_mact_info);
    data = (const dnx_data_l2_general_fwd_mact_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->logical_data_base);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->physical_data_base);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_l2_general_lem_table_map_l2_action_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l2_general_lem_table_map_l2_action_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_table_lem_table_map_l2_action);
    data = (const dnx_data_l2_general_lem_table_map_l2_action_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->l2_action_type);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_l2_general_mact_result_type_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_table_mact_result_type_map);

}

const dnxc_data_table_info_t *
dnx_data_l2_general_scan_data_bases_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_table_scan_data_bases_info);

}

const dnxc_data_table_info_t *
dnx_data_l2_general_fwd_mact_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_table_fwd_mact_info);

}

const dnxc_data_table_info_t *
dnx_data_l2_general_lem_table_map_l2_action_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_table_lem_table_map_l2_action);

}






static shr_error_e
dnx_data_l2_feature_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "feature";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l2_feature_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l2 feature features");

    submodule_data->features[dnx_data_l2_feature_age_out_and_refresh_profile_selection].name = "age_out_and_refresh_profile_selection";
    submodule_data->features[dnx_data_l2_feature_age_out_and_refresh_profile_selection].doc = "";
    submodule_data->features[dnx_data_l2_feature_age_out_and_refresh_profile_selection].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_age_machine_pause_after_flush].name = "age_machine_pause_after_flush";
    submodule_data->features[dnx_data_l2_feature_age_machine_pause_after_flush].doc = "";
    submodule_data->features[dnx_data_l2_feature_age_machine_pause_after_flush].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_age_state_not_updated].name = "age_state_not_updated";
    submodule_data->features[dnx_data_l2_feature_age_state_not_updated].doc = "";
    submodule_data->features[dnx_data_l2_feature_age_state_not_updated].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_appdb_id_for_olp].name = "appdb_id_for_olp";
    submodule_data->features[dnx_data_l2_feature_appdb_id_for_olp].doc = "";
    submodule_data->features[dnx_data_l2_feature_appdb_id_for_olp].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_eth_qual_is_mc].name = "eth_qual_is_mc";
    submodule_data->features[dnx_data_l2_feature_eth_qual_is_mc].doc = "";
    submodule_data->features[dnx_data_l2_feature_eth_qual_is_mc].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_bc_same_as_unknown_mc].name = "bc_same_as_unknown_mc";
    submodule_data->features[dnx_data_l2_feature_bc_same_as_unknown_mc].doc = "";
    submodule_data->features[dnx_data_l2_feature_bc_same_as_unknown_mc].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_fid_mgmt_ecc_error].name = "fid_mgmt_ecc_error";
    submodule_data->features[dnx_data_l2_feature_fid_mgmt_ecc_error].doc = "";
    submodule_data->features[dnx_data_l2_feature_fid_mgmt_ecc_error].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_wrong_limit_interrupt_handling].name = "wrong_limit_interrupt_handling";
    submodule_data->features[dnx_data_l2_feature_wrong_limit_interrupt_handling].doc = "";
    submodule_data->features[dnx_data_l2_feature_wrong_limit_interrupt_handling].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_transplant_instead_of_refresh].name = "transplant_instead_of_refresh";
    submodule_data->features[dnx_data_l2_feature_transplant_instead_of_refresh].doc = "";
    submodule_data->features[dnx_data_l2_feature_transplant_instead_of_refresh].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_static_mac_age_out].name = "static_mac_age_out";
    submodule_data->features[dnx_data_l2_feature_static_mac_age_out].doc = "";
    submodule_data->features[dnx_data_l2_feature_static_mac_age_out].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_vmv_for_limit_in_wrong_location].name = "vmv_for_limit_in_wrong_location";
    submodule_data->features[dnx_data_l2_feature_vmv_for_limit_in_wrong_location].doc = "";
    submodule_data->features[dnx_data_l2_feature_vmv_for_limit_in_wrong_location].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_learn_limit].name = "learn_limit";
    submodule_data->features[dnx_data_l2_feature_learn_limit].doc = "";
    submodule_data->features[dnx_data_l2_feature_learn_limit].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_limit_per_lif_counters].name = "limit_per_lif_counters";
    submodule_data->features[dnx_data_l2_feature_limit_per_lif_counters].doc = "";
    submodule_data->features[dnx_data_l2_feature_limit_per_lif_counters].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_learn_events_wrong_command].name = "learn_events_wrong_command";
    submodule_data->features[dnx_data_l2_feature_learn_events_wrong_command].doc = "";
    submodule_data->features[dnx_data_l2_feature_learn_events_wrong_command].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_opportunistic_learning_always_transplant].name = "opportunistic_learning_always_transplant";
    submodule_data->features[dnx_data_l2_feature_opportunistic_learning_always_transplant].doc = "";
    submodule_data->features[dnx_data_l2_feature_opportunistic_learning_always_transplant].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_exceed_limit_interrupt_by_insert_cmd].name = "exceed_limit_interrupt_by_insert_cmd";
    submodule_data->features[dnx_data_l2_feature_exceed_limit_interrupt_by_insert_cmd].doc = "";
    submodule_data->features[dnx_data_l2_feature_exceed_limit_interrupt_by_insert_cmd].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_refresh_events_wrong_key_msbs].name = "refresh_events_wrong_key_msbs";
    submodule_data->features[dnx_data_l2_feature_refresh_events_wrong_key_msbs].doc = "";
    submodule_data->features[dnx_data_l2_feature_refresh_events_wrong_key_msbs].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_ignore_limit_check].name = "ignore_limit_check";
    submodule_data->features[dnx_data_l2_feature_ignore_limit_check].doc = "";
    submodule_data->features[dnx_data_l2_feature_ignore_limit_check].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_no_trap_for_unknown_destination].name = "no_trap_for_unknown_destination";
    submodule_data->features[dnx_data_l2_feature_no_trap_for_unknown_destination].doc = "";
    submodule_data->features[dnx_data_l2_feature_no_trap_for_unknown_destination].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_no_black_hole_for_unknown_destination].name = "no_black_hole_for_unknown_destination";
    submodule_data->features[dnx_data_l2_feature_no_black_hole_for_unknown_destination].doc = "";
    submodule_data->features[dnx_data_l2_feature_no_black_hole_for_unknown_destination].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_mact_access_by_opportunistic_learning].name = "mact_access_by_opportunistic_learning";
    submodule_data->features[dnx_data_l2_feature_mact_access_by_opportunistic_learning].doc = "";
    submodule_data->features[dnx_data_l2_feature_mact_access_by_opportunistic_learning].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_opportunistic_rejected].name = "opportunistic_rejected";
    submodule_data->features[dnx_data_l2_feature_opportunistic_rejected].doc = "";
    submodule_data->features[dnx_data_l2_feature_opportunistic_rejected].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_flush_drop_stuck].name = "flush_drop_stuck";
    submodule_data->features[dnx_data_l2_feature_flush_drop_stuck].doc = "";
    submodule_data->features[dnx_data_l2_feature_flush_drop_stuck].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_lif_flood_profile_always_update].name = "lif_flood_profile_always_update";
    submodule_data->features[dnx_data_l2_feature_lif_flood_profile_always_update].doc = "";
    submodule_data->features[dnx_data_l2_feature_lif_flood_profile_always_update].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_olp_always_enters_source_cpu].name = "olp_always_enters_source_cpu";
    submodule_data->features[dnx_data_l2_feature_olp_always_enters_source_cpu].doc = "";
    submodule_data->features[dnx_data_l2_feature_olp_always_enters_source_cpu].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_learn_payload_compatible_jr1].name = "learn_payload_compatible_jr1";
    submodule_data->features[dnx_data_l2_feature_learn_payload_compatible_jr1].doc = "";
    submodule_data->features[dnx_data_l2_feature_learn_payload_compatible_jr1].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_unified_vsi_info_db].name = "unified_vsi_info_db";
    submodule_data->features[dnx_data_l2_feature_unified_vsi_info_db].doc = "";
    submodule_data->features[dnx_data_l2_feature_unified_vsi_info_db].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_ingress_opportunistic_learning_support].name = "ingress_opportunistic_learning_support";
    submodule_data->features[dnx_data_l2_feature_ingress_opportunistic_learning_support].doc = "";
    submodule_data->features[dnx_data_l2_feature_ingress_opportunistic_learning_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_age_refresh_mode_support].name = "age_refresh_mode_support";
    submodule_data->features[dnx_data_l2_feature_age_refresh_mode_support].doc = "";
    submodule_data->features[dnx_data_l2_feature_age_refresh_mode_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_learn_payload_native_update_enable].name = "learn_payload_native_update_enable";
    submodule_data->features[dnx_data_l2_feature_learn_payload_native_update_enable].doc = "";
    submodule_data->features[dnx_data_l2_feature_learn_payload_native_update_enable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_accepted_data_with_vmv].name = "accepted_data_with_vmv";
    submodule_data->features[dnx_data_l2_feature_accepted_data_with_vmv].doc = "";
    submodule_data->features[dnx_data_l2_feature_accepted_data_with_vmv].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_l2_feature_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l2 feature defines");

    
    submodule_data->nof_tables = _dnx_data_l2_feature_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l2 feature tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l2_feature_feature_get(
    int unit,
    dnx_data_l2_feature_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_feature, feature);
}











static shr_error_e
dnx_data_l2_vsi_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "vsi";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l2_vsi_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l2 vsi features");

    
    submodule_data->nof_defines = _dnx_data_l2_vsi_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l2 vsi defines");

    submodule_data->defines[dnx_data_l2_vsi_define_nof_vsi_aging_profiles].name = "nof_vsi_aging_profiles";
    submodule_data->defines[dnx_data_l2_vsi_define_nof_vsi_aging_profiles].doc = "";
    
    submodule_data->defines[dnx_data_l2_vsi_define_nof_vsi_aging_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_vsi_define_nof_event_forwarding_profiles].name = "nof_event_forwarding_profiles";
    submodule_data->defines[dnx_data_l2_vsi_define_nof_event_forwarding_profiles].doc = "";
    
    submodule_data->defines[dnx_data_l2_vsi_define_nof_event_forwarding_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_vsi_define_nof_vsi_learning_profiles].name = "nof_vsi_learning_profiles";
    submodule_data->defines[dnx_data_l2_vsi_define_nof_vsi_learning_profiles].doc = "";
    
    submodule_data->defines[dnx_data_l2_vsi_define_nof_vsi_learning_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_vsi_define_vsi_table].name = "vsi_table";
    submodule_data->defines[dnx_data_l2_vsi_define_vsi_table].doc = "";
    
    submodule_data->defines[dnx_data_l2_vsi_define_vsi_table].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_l2_vsi_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l2 vsi tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l2_vsi_feature_get(
    int unit,
    dnx_data_l2_vsi_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_vsi, feature);
}


uint32
dnx_data_l2_vsi_nof_vsi_aging_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_vsi, dnx_data_l2_vsi_define_nof_vsi_aging_profiles);
}

uint32
dnx_data_l2_vsi_nof_event_forwarding_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_vsi, dnx_data_l2_vsi_define_nof_event_forwarding_profiles);
}

uint32
dnx_data_l2_vsi_nof_vsi_learning_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_vsi, dnx_data_l2_vsi_define_nof_vsi_learning_profiles);
}

uint32
dnx_data_l2_vsi_vsi_table_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_vsi, dnx_data_l2_vsi_define_vsi_table);
}










static shr_error_e
dnx_data_l2_vlan_domain_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "vlan_domain";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l2_vlan_domain_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l2 vlan_domain features");

    
    submodule_data->nof_defines = _dnx_data_l2_vlan_domain_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l2 vlan_domain defines");

    submodule_data->defines[dnx_data_l2_vlan_domain_define_nof_vlan_domains].name = "nof_vlan_domains";
    submodule_data->defines[dnx_data_l2_vlan_domain_define_nof_vlan_domains].doc = "";
    
    submodule_data->defines[dnx_data_l2_vlan_domain_define_nof_vlan_domains].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_vlan_domain_define_nof_bits_next_layer_network_domain].name = "nof_bits_next_layer_network_domain";
    submodule_data->defines[dnx_data_l2_vlan_domain_define_nof_bits_next_layer_network_domain].doc = "";
    
    submodule_data->defines[dnx_data_l2_vlan_domain_define_nof_bits_next_layer_network_domain].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l2_vlan_domain_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l2 vlan_domain tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l2_vlan_domain_feature_get(
    int unit,
    dnx_data_l2_vlan_domain_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_vlan_domain, feature);
}


uint32
dnx_data_l2_vlan_domain_nof_vlan_domains_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_vlan_domain, dnx_data_l2_vlan_domain_define_nof_vlan_domains);
}

uint32
dnx_data_l2_vlan_domain_nof_bits_next_layer_network_domain_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_vlan_domain, dnx_data_l2_vlan_domain_define_nof_bits_next_layer_network_domain);
}










static shr_error_e
dnx_data_l2_dma_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dma";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l2_dma_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l2 dma features");

    
    submodule_data->nof_defines = _dnx_data_l2_dma_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l2 dma defines");

    submodule_data->defines[dnx_data_l2_dma_define_flush_nof_dma_entries].name = "flush_nof_dma_entries";
    submodule_data->defines[dnx_data_l2_dma_define_flush_nof_dma_entries].doc = "";
    
    submodule_data->defines[dnx_data_l2_dma_define_flush_nof_dma_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_dma_define_flush_db_nof_dma_rules].name = "flush_db_nof_dma_rules";
    submodule_data->defines[dnx_data_l2_dma_define_flush_db_nof_dma_rules].doc = "";
    
    submodule_data->defines[dnx_data_l2_dma_define_flush_db_nof_dma_rules].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_dma_define_flush_db_nof_dma_rules_per_table].name = "flush_db_nof_dma_rules_per_table";
    submodule_data->defines[dnx_data_l2_dma_define_flush_db_nof_dma_rules_per_table].doc = "";
    
    submodule_data->defines[dnx_data_l2_dma_define_flush_db_nof_dma_rules_per_table].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_dma_define_flush_db_rule_size].name = "flush_db_rule_size";
    submodule_data->defines[dnx_data_l2_dma_define_flush_db_rule_size].doc = "";
    
    submodule_data->defines[dnx_data_l2_dma_define_flush_db_rule_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_dma_define_flush_db_data_size].name = "flush_db_data_size";
    submodule_data->defines[dnx_data_l2_dma_define_flush_db_data_size].doc = "";
    
    submodule_data->defines[dnx_data_l2_dma_define_flush_db_data_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_dma_define_flush_group_size].name = "flush_group_size";
    submodule_data->defines[dnx_data_l2_dma_define_flush_group_size].doc = "";
    
    submodule_data->defines[dnx_data_l2_dma_define_flush_group_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_dma_define_learning_fifo_dma_buffer_size].name = "learning_fifo_dma_buffer_size";
    submodule_data->defines[dnx_data_l2_dma_define_learning_fifo_dma_buffer_size].doc = "";
    
    submodule_data->defines[dnx_data_l2_dma_define_learning_fifo_dma_buffer_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l2_dma_define_learning_fifo_dma_timeout].name = "learning_fifo_dma_timeout";
    submodule_data->defines[dnx_data_l2_dma_define_learning_fifo_dma_timeout].doc = "";
    
    submodule_data->defines[dnx_data_l2_dma_define_learning_fifo_dma_timeout].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l2_dma_define_learning_fifo_dma_threshold].name = "learning_fifo_dma_threshold";
    submodule_data->defines[dnx_data_l2_dma_define_learning_fifo_dma_threshold].doc = "";
    
    submodule_data->defines[dnx_data_l2_dma_define_learning_fifo_dma_threshold].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l2_dma_define_l2_dma_cpu_learn_thread_priority].name = "l2_dma_cpu_learn_thread_priority";
    submodule_data->defines[dnx_data_l2_dma_define_l2_dma_cpu_learn_thread_priority].doc = "";
    
    submodule_data->defines[dnx_data_l2_dma_define_l2_dma_cpu_learn_thread_priority].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_l2_dma_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l2 dma tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l2_dma_feature_get(
    int unit,
    dnx_data_l2_dma_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_dma, feature);
}


uint32
dnx_data_l2_dma_flush_nof_dma_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_dma, dnx_data_l2_dma_define_flush_nof_dma_entries);
}

uint32
dnx_data_l2_dma_flush_db_nof_dma_rules_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_dma, dnx_data_l2_dma_define_flush_db_nof_dma_rules);
}

uint32
dnx_data_l2_dma_flush_db_nof_dma_rules_per_table_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_dma, dnx_data_l2_dma_define_flush_db_nof_dma_rules_per_table);
}

uint32
dnx_data_l2_dma_flush_db_rule_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_dma, dnx_data_l2_dma_define_flush_db_rule_size);
}

uint32
dnx_data_l2_dma_flush_db_data_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_dma, dnx_data_l2_dma_define_flush_db_data_size);
}

uint32
dnx_data_l2_dma_flush_group_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_dma, dnx_data_l2_dma_define_flush_group_size);
}

uint32
dnx_data_l2_dma_learning_fifo_dma_buffer_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_dma, dnx_data_l2_dma_define_learning_fifo_dma_buffer_size);
}

uint32
dnx_data_l2_dma_learning_fifo_dma_timeout_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_dma, dnx_data_l2_dma_define_learning_fifo_dma_timeout);
}

uint32
dnx_data_l2_dma_learning_fifo_dma_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_dma, dnx_data_l2_dma_define_learning_fifo_dma_threshold);
}

uint32
dnx_data_l2_dma_l2_dma_cpu_learn_thread_priority_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_dma, dnx_data_l2_dma_define_l2_dma_cpu_learn_thread_priority);
}










static shr_error_e
dnx_data_l2_age_and_flush_machine_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "age_and_flush_machine";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l2_age_and_flush_machine_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l2 age_and_flush_machine features");

    submodule_data->features[dnx_data_l2_age_and_flush_machine_flush_init_enable].name = "flush_init_enable";
    submodule_data->features[dnx_data_l2_age_and_flush_machine_flush_init_enable].doc = "";
    submodule_data->features[dnx_data_l2_age_and_flush_machine_flush_init_enable].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_l2_age_and_flush_machine_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l2 age_and_flush_machine defines");

    submodule_data->defines[dnx_data_l2_age_and_flush_machine_define_max_age_states].name = "max_age_states";
    submodule_data->defines[dnx_data_l2_age_and_flush_machine_define_max_age_states].doc = "";
    
    submodule_data->defines[dnx_data_l2_age_and_flush_machine_define_max_age_states].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_age_and_flush_machine_define_flush_buffer_nof_entries].name = "flush_buffer_nof_entries";
    submodule_data->defines[dnx_data_l2_age_and_flush_machine_define_flush_buffer_nof_entries].doc = "";
    
    submodule_data->defines[dnx_data_l2_age_and_flush_machine_define_flush_buffer_nof_entries].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l2_age_and_flush_machine_define_traverse_thread_priority].name = "traverse_thread_priority";
    submodule_data->defines[dnx_data_l2_age_and_flush_machine_define_traverse_thread_priority].doc = "";
    
    submodule_data->defines[dnx_data_l2_age_and_flush_machine_define_traverse_thread_priority].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_l2_age_and_flush_machine_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l2 age_and_flush_machine tables");

    
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush_pulse].name = "flush_pulse";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush_pulse].doc = "";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush_pulse].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush_pulse].size_of_values = sizeof(dnx_data_l2_age_and_flush_machine_flush_pulse_t);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush_pulse].entry_get = dnx_data_l2_age_and_flush_machine_flush_pulse_entry_str_get;

    
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush_pulse].nof_keys = 1;
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush_pulse].keys[0].name = "l2_action_type";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush_pulse].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush_pulse].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush_pulse].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush_pulse].nof_values, "_dnx_data_l2_age_and_flush_machine_table_flush_pulse table values");
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush_pulse].values[0].name = "flush_pulse_reg";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush_pulse].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush_pulse].values[0].doc = "";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush_pulse].values[0].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_flush_pulse_t, flush_pulse_reg);

    
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].name = "age";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].doc = "";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].size_of_values = sizeof(dnx_data_l2_age_and_flush_machine_age_t);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].entry_get = dnx_data_l2_age_and_flush_machine_age_entry_str_get;

    
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].nof_keys = 0;

    
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].nof_values, "_dnx_data_l2_age_and_flush_machine_table_age table values");
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[0].name = "age_config";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[0].doc = "";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[0].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_age_t, age_config);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[1].name = "disable_aging";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[1].type = "soc_field_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[1].doc = "";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[1].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_age_t, disable_aging);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[2].name = "scan_pulse";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[2].type = "soc_reg_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[2].doc = "";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[2].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_age_t, scan_pulse);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l2_age_and_flush_machine_feature_get(
    int unit,
    dnx_data_l2_age_and_flush_machine_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, feature);
}


uint32
dnx_data_l2_age_and_flush_machine_max_age_states_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_define_max_age_states);
}

uint32
dnx_data_l2_age_and_flush_machine_flush_buffer_nof_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_define_flush_buffer_nof_entries);
}

uint32
dnx_data_l2_age_and_flush_machine_traverse_thread_priority_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_define_traverse_thread_priority);
}



const dnx_data_l2_age_and_flush_machine_flush_pulse_t *
dnx_data_l2_age_and_flush_machine_flush_pulse_get(
    int unit,
    int l2_action_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_table_flush_pulse);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, l2_action_type, 0);
    return (const dnx_data_l2_age_and_flush_machine_flush_pulse_t *) data;

}

const dnx_data_l2_age_and_flush_machine_age_t *
dnx_data_l2_age_and_flush_machine_age_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_table_age);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_l2_age_and_flush_machine_age_t *) data;

}


shr_error_e
dnx_data_l2_age_and_flush_machine_flush_pulse_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l2_age_and_flush_machine_flush_pulse_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_table_flush_pulse);
    data = (const dnx_data_l2_age_and_flush_machine_flush_pulse_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->flush_pulse_reg);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_l2_age_and_flush_machine_age_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l2_age_and_flush_machine_age_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_table_age);
    data = (const dnx_data_l2_age_and_flush_machine_age_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->age_config);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->disable_aging);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->scan_pulse);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_l2_age_and_flush_machine_flush_pulse_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_table_flush_pulse);

}

const dnxc_data_table_info_t *
dnx_data_l2_age_and_flush_machine_age_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_table_age);

}






static shr_error_e
dnx_data_l2_olp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "olp";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l2_olp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l2 olp features");

    submodule_data->features[dnx_data_l2_olp_refresh_events_support].name = "refresh_events_support";
    submodule_data->features[dnx_data_l2_olp_refresh_events_support].doc = "";
    submodule_data->features[dnx_data_l2_olp_refresh_events_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_olp_olp_learn_payload_initial_value_supported].name = "olp_learn_payload_initial_value_supported";
    submodule_data->features[dnx_data_l2_olp_olp_learn_payload_initial_value_supported].doc = "";
    submodule_data->features[dnx_data_l2_olp_olp_learn_payload_initial_value_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_olp_dsp_messages_support].name = "dsp_messages_support";
    submodule_data->features[dnx_data_l2_olp_dsp_messages_support].doc = "";
    submodule_data->features[dnx_data_l2_olp_dsp_messages_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_l2_olp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l2 olp defines");

    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_shift].name = "lpkgv_shift";
    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_shift].doc = "";
    
    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_shift].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_mask].name = "lpkgv_mask";
    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_mask].doc = "";
    
    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_mask].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_with_outlif].name = "lpkgv_with_outlif";
    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_with_outlif].doc = "";
    
    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_with_outlif].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_wo_outlif].name = "lpkgv_wo_outlif";
    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_wo_outlif].doc = "";
    
    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_wo_outlif].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_olp_define_destination_offset].name = "destination_offset";
    submodule_data->defines[dnx_data_l2_olp_define_destination_offset].doc = "";
    
    submodule_data->defines[dnx_data_l2_olp_define_destination_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_olp_define_outlif_offset].name = "outlif_offset";
    submodule_data->defines[dnx_data_l2_olp_define_outlif_offset].doc = "";
    
    submodule_data->defines[dnx_data_l2_olp_define_outlif_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_olp_define_eei_offset].name = "eei_offset";
    submodule_data->defines[dnx_data_l2_olp_define_eei_offset].doc = "";
    
    submodule_data->defines[dnx_data_l2_olp_define_eei_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_olp_define_fec_offset].name = "fec_offset";
    submodule_data->defines[dnx_data_l2_olp_define_fec_offset].doc = "";
    
    submodule_data->defines[dnx_data_l2_olp_define_fec_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_olp_define_jr_mode_enhanced_performance_enable].name = "jr_mode_enhanced_performance_enable";
    submodule_data->defines[dnx_data_l2_olp_define_jr_mode_enhanced_performance_enable].doc = "";
    
    submodule_data->defines[dnx_data_l2_olp_define_jr_mode_enhanced_performance_enable].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_olp_define_olp_learning_block_id].name = "olp_learning_block_id";
    submodule_data->defines[dnx_data_l2_olp_define_olp_learning_block_id].doc = "";
    
    submodule_data->defines[dnx_data_l2_olp_define_olp_learning_block_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_olp_define_olp_fwd_block_id].name = "olp_fwd_block_id";
    submodule_data->defines[dnx_data_l2_olp_define_olp_fwd_block_id].doc = "";
    
    submodule_data->defines[dnx_data_l2_olp_define_olp_fwd_block_id].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l2_olp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l2 olp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l2_olp_feature_get(
    int unit,
    dnx_data_l2_olp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, feature);
}


uint32
dnx_data_l2_olp_lpkgv_shift_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, dnx_data_l2_olp_define_lpkgv_shift);
}

uint32
dnx_data_l2_olp_lpkgv_mask_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, dnx_data_l2_olp_define_lpkgv_mask);
}

uint32
dnx_data_l2_olp_lpkgv_with_outlif_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, dnx_data_l2_olp_define_lpkgv_with_outlif);
}

uint32
dnx_data_l2_olp_lpkgv_wo_outlif_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, dnx_data_l2_olp_define_lpkgv_wo_outlif);
}

uint32
dnx_data_l2_olp_destination_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, dnx_data_l2_olp_define_destination_offset);
}

uint32
dnx_data_l2_olp_outlif_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, dnx_data_l2_olp_define_outlif_offset);
}

uint32
dnx_data_l2_olp_eei_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, dnx_data_l2_olp_define_eei_offset);
}

uint32
dnx_data_l2_olp_fec_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, dnx_data_l2_olp_define_fec_offset);
}

uint32
dnx_data_l2_olp_jr_mode_enhanced_performance_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, dnx_data_l2_olp_define_jr_mode_enhanced_performance_enable);
}

uint32
dnx_data_l2_olp_olp_learning_block_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, dnx_data_l2_olp_define_olp_learning_block_id);
}

uint32
dnx_data_l2_olp_olp_fwd_block_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, dnx_data_l2_olp_define_olp_fwd_block_id);
}







shr_error_e
dnx_data_l2_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "l2";
    module_data->nof_submodules = _dnx_data_l2_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data l2 submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_l2_general_init(unit, &module_data->submodules[dnx_data_l2_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_l2_feature_init(unit, &module_data->submodules[dnx_data_l2_submodule_feature]));
    SHR_IF_ERR_EXIT(dnx_data_l2_vsi_init(unit, &module_data->submodules[dnx_data_l2_submodule_vsi]));
    SHR_IF_ERR_EXIT(dnx_data_l2_vlan_domain_init(unit, &module_data->submodules[dnx_data_l2_submodule_vlan_domain]));
    SHR_IF_ERR_EXIT(dnx_data_l2_dma_init(unit, &module_data->submodules[dnx_data_l2_submodule_dma]));
    SHR_IF_ERR_EXIT(dnx_data_l2_age_and_flush_machine_init(unit, &module_data->submodules[dnx_data_l2_submodule_age_and_flush_machine]));
    SHR_IF_ERR_EXIT(dnx_data_l2_olp_init(unit, &module_data->submodules[dnx_data_l2_submodule_olp]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l2_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_l2_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_l2_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_l2_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a1_data_l2_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_l2_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_l2_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b1_data_l2_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_l2_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_l2_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_l2_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_l2_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

