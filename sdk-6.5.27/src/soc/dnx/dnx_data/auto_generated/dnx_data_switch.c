
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCH

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_switch.h>




extern shr_error_e jr2_a0_data_switch_attach(
    int unit);


extern shr_error_e jr2_b0_data_switch_attach(
    int unit);


extern shr_error_e j2c_a0_data_switch_attach(
    int unit);


extern shr_error_e q2a_a0_data_switch_attach(
    int unit);


extern shr_error_e q2a_b0_data_switch_attach(
    int unit);


extern shr_error_e j2p_a0_data_switch_attach(
    int unit);


extern shr_error_e j2x_a0_data_switch_attach(
    int unit);




static shr_error_e
dnx_data_switch_load_balancing_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "load_balancing";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_switch_load_balancing_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data switch load_balancing features");

    submodule_data->features[dnx_data_switch_load_balancing_fwd_hashing_method].name = "fwd_hashing_method";
    submodule_data->features[dnx_data_switch_load_balancing_fwd_hashing_method].doc = "";
    submodule_data->features[dnx_data_switch_load_balancing_fwd_hashing_method].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_switch_load_balancing_layer_record_mode_selection].name = "layer_record_mode_selection";
    submodule_data->features[dnx_data_switch_load_balancing_layer_record_mode_selection].doc = "";
    submodule_data->features[dnx_data_switch_load_balancing_layer_record_mode_selection].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_switch_load_balancing_seed_config_per_pp_port].name = "seed_config_per_pp_port";
    submodule_data->features[dnx_data_switch_load_balancing_seed_config_per_pp_port].doc = "";
    submodule_data->features[dnx_data_switch_load_balancing_seed_config_per_pp_port].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_switch_load_balancing_paser_seed_update_supported].name = "paser_seed_update_supported";
    submodule_data->features[dnx_data_switch_load_balancing_paser_seed_update_supported].doc = "";
    submodule_data->features[dnx_data_switch_load_balancing_paser_seed_update_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_switch_load_balancing_ipv6_reparse_supported].name = "ipv6_reparse_supported";
    submodule_data->features[dnx_data_switch_load_balancing_ipv6_reparse_supported].doc = "";
    submodule_data->features[dnx_data_switch_load_balancing_ipv6_reparse_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_switch_load_balancing_ipv6_unsymmetrical_hashing_supported].name = "ipv6_unsymmetrical_hashing_supported";
    submodule_data->features[dnx_data_switch_load_balancing_ipv6_unsymmetrical_hashing_supported].doc = "";
    submodule_data->features[dnx_data_switch_load_balancing_ipv6_unsymmetrical_hashing_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_switch_load_balancing_cw_exclude_from_hash].name = "cw_exclude_from_hash";
    submodule_data->features[dnx_data_switch_load_balancing_cw_exclude_from_hash].doc = "";
    submodule_data->features[dnx_data_switch_load_balancing_cw_exclude_from_hash].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_switch_load_balancing_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data switch load_balancing defines");

    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_lb_clients].name = "nof_lb_clients";
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_lb_clients].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_lb_clients].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_lb_crc_sel_tcam_entries].name = "nof_lb_crc_sel_tcam_entries";
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_lb_crc_sel_tcam_entries].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_lb_crc_sel_tcam_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_crc_functions].name = "nof_crc_functions";
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_crc_functions].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_crc_functions].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_label].name = "initial_reserved_label";
    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_label].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_label].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_label_force].name = "initial_reserved_label_force";
    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_label_force].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_label_force].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_next_label].name = "initial_reserved_next_label";
    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_next_label].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_next_label].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_next_label_valid].name = "initial_reserved_next_label_valid";
    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_next_label_valid].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_next_label_valid].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_reserved_next_label_valid].name = "reserved_next_label_valid";
    submodule_data->defines[dnx_data_switch_load_balancing_define_reserved_next_label_valid].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_reserved_next_label_valid].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_num_valid_mpls_protocols].name = "num_valid_mpls_protocols";
    submodule_data->defines[dnx_data_switch_load_balancing_define_num_valid_mpls_protocols].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_num_valid_mpls_protocols].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_layer_records_from_parser].name = "nof_layer_records_from_parser";
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_layer_records_from_parser].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_layer_records_from_parser].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_seeds_per_crc_function].name = "nof_seeds_per_crc_function";
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_seeds_per_crc_function].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_seeds_per_crc_function].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_hashing_selection_per_layer].name = "hashing_selection_per_layer";
    submodule_data->defines[dnx_data_switch_load_balancing_define_hashing_selection_per_layer].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_hashing_selection_per_layer].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_mpls_split_stack_config].name = "mpls_split_stack_config";
    submodule_data->defines[dnx_data_switch_load_balancing_define_mpls_split_stack_config].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_mpls_split_stack_config].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_mpls_cam_next_label_valid_field_exists].name = "mpls_cam_next_label_valid_field_exists";
    submodule_data->defines[dnx_data_switch_load_balancing_define_mpls_cam_next_label_valid_field_exists].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_mpls_cam_next_label_valid_field_exists].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_padded_layers_crc_buffer].name = "padded_layers_crc_buffer";
    submodule_data->defines[dnx_data_switch_load_balancing_define_padded_layers_crc_buffer].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_padded_layers_crc_buffer].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_wide_hash_buffer].name = "wide_hash_buffer";
    submodule_data->defines[dnx_data_switch_load_balancing_define_wide_hash_buffer].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_wide_hash_buffer].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_lb_mpls_stack].name = "nof_lb_mpls_stack";
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_lb_mpls_stack].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_lb_mpls_stack].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_bits_mpls_protocol_identifier].name = "nof_bits_mpls_protocol_identifier";
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_bits_mpls_protocol_identifier].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_bits_mpls_protocol_identifier].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_seed_configuration_per_pp_port].name = "seed_configuration_per_pp_port";
    submodule_data->defines[dnx_data_switch_load_balancing_define_seed_configuration_per_pp_port].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_seed_configuration_per_pp_port].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_swig_base_offline_simulation].name = "swig_base_offline_simulation";
    submodule_data->defines[dnx_data_switch_load_balancing_define_swig_base_offline_simulation].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_swig_base_offline_simulation].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_pmf_simulation_support].name = "pmf_simulation_support";
    submodule_data->defines[dnx_data_switch_load_balancing_define_pmf_simulation_support].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_pmf_simulation_support].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_switch_load_balancing_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data switch load_balancing tables");

    
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].name = "lb_clients";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].doc = "";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].size_of_values = sizeof(dnx_data_switch_load_balancing_lb_clients_t);
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].entry_get = dnx_data_switch_load_balancing_lb_clients_entry_str_get;

    
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].nof_keys = 1;
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].keys[0].name = "client_hw_id";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].values, dnxc_data_value_t, submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].nof_values, "_dnx_data_switch_load_balancing_table_lb_clients table values");
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].values[0].name = "client_logical_id";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].values[0].type = "int";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].values[0].doc = "";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].values[0].offset = UTILEX_OFFSETOF(dnx_data_switch_load_balancing_lb_clients_t, client_logical_id);

    
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].name = "lb_client_crc";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].doc = "";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].size_of_values = sizeof(dnx_data_switch_load_balancing_lb_client_crc_t);
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].entry_get = dnx_data_switch_load_balancing_lb_client_crc_entry_str_get;

    
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].nof_keys = 1;
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].keys[0].name = "client_hw_id";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].values, dnxc_data_value_t, submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].nof_values, "_dnx_data_switch_load_balancing_table_lb_client_crc table values");
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].values[0].name = "crc_hash_func";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].values[0].type = "int";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].values[0].doc = "";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].values[0].offset = UTILEX_OFFSETOF(dnx_data_switch_load_balancing_lb_client_crc_t, crc_hash_func);

    
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].name = "lb_field_enablers";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].doc = "";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].size_of_values = sizeof(dnx_data_switch_load_balancing_lb_field_enablers_t);
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].entry_get = dnx_data_switch_load_balancing_lb_field_enablers_entry_str_get;

    
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].nof_keys = 1;
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].keys[0].name = "header_id";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].values, dnxc_data_value_t, submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].nof_values, "_dnx_data_switch_load_balancing_table_lb_field_enablers table values");
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].values[0].name = "field_enablers_mask";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].values[0].type = "uint32";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].values[0].doc = "";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].values[0].offset = UTILEX_OFFSETOF(dnx_data_switch_load_balancing_lb_field_enablers_t, field_enablers_mask);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_switch_load_balancing_feature_get(
    int unit,
    dnx_data_switch_load_balancing_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, feature);
}


uint32
dnx_data_switch_load_balancing_nof_lb_clients_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_nof_lb_clients);
}

uint32
dnx_data_switch_load_balancing_nof_lb_crc_sel_tcam_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_nof_lb_crc_sel_tcam_entries);
}

uint32
dnx_data_switch_load_balancing_nof_crc_functions_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_nof_crc_functions);
}

uint32
dnx_data_switch_load_balancing_initial_reserved_label_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_initial_reserved_label);
}

uint32
dnx_data_switch_load_balancing_initial_reserved_label_force_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_initial_reserved_label_force);
}

uint32
dnx_data_switch_load_balancing_initial_reserved_next_label_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_initial_reserved_next_label);
}

uint32
dnx_data_switch_load_balancing_initial_reserved_next_label_valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_initial_reserved_next_label_valid);
}

uint32
dnx_data_switch_load_balancing_reserved_next_label_valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_reserved_next_label_valid);
}

uint32
dnx_data_switch_load_balancing_num_valid_mpls_protocols_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_num_valid_mpls_protocols);
}

uint32
dnx_data_switch_load_balancing_nof_layer_records_from_parser_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_nof_layer_records_from_parser);
}

uint32
dnx_data_switch_load_balancing_nof_seeds_per_crc_function_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_nof_seeds_per_crc_function);
}

uint32
dnx_data_switch_load_balancing_hashing_selection_per_layer_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_hashing_selection_per_layer);
}

uint32
dnx_data_switch_load_balancing_mpls_split_stack_config_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_mpls_split_stack_config);
}

uint32
dnx_data_switch_load_balancing_mpls_cam_next_label_valid_field_exists_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_mpls_cam_next_label_valid_field_exists);
}

uint32
dnx_data_switch_load_balancing_padded_layers_crc_buffer_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_padded_layers_crc_buffer);
}

uint32
dnx_data_switch_load_balancing_wide_hash_buffer_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_wide_hash_buffer);
}

uint32
dnx_data_switch_load_balancing_nof_lb_mpls_stack_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_nof_lb_mpls_stack);
}

uint32
dnx_data_switch_load_balancing_nof_bits_mpls_protocol_identifier_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_nof_bits_mpls_protocol_identifier);
}

uint32
dnx_data_switch_load_balancing_seed_configuration_per_pp_port_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_seed_configuration_per_pp_port);
}

uint32
dnx_data_switch_load_balancing_swig_base_offline_simulation_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_swig_base_offline_simulation);
}

uint32
dnx_data_switch_load_balancing_pmf_simulation_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_pmf_simulation_support);
}



const dnx_data_switch_load_balancing_lb_clients_t *
dnx_data_switch_load_balancing_lb_clients_get(
    int unit,
    int client_hw_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_table_lb_clients);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, client_hw_id, 0);
    return (const dnx_data_switch_load_balancing_lb_clients_t *) data;

}

const dnx_data_switch_load_balancing_lb_client_crc_t *
dnx_data_switch_load_balancing_lb_client_crc_get(
    int unit,
    int client_hw_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_table_lb_client_crc);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, client_hw_id, 0);
    return (const dnx_data_switch_load_balancing_lb_client_crc_t *) data;

}

const dnx_data_switch_load_balancing_lb_field_enablers_t *
dnx_data_switch_load_balancing_lb_field_enablers_get(
    int unit,
    int header_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_table_lb_field_enablers);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, header_id, 0);
    return (const dnx_data_switch_load_balancing_lb_field_enablers_t *) data;

}


shr_error_e
dnx_data_switch_load_balancing_lb_clients_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_switch_load_balancing_lb_clients_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_table_lb_clients);
    data = (const dnx_data_switch_load_balancing_lb_clients_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->client_logical_id);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_switch_load_balancing_lb_client_crc_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_switch_load_balancing_lb_client_crc_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_table_lb_client_crc);
    data = (const dnx_data_switch_load_balancing_lb_client_crc_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->crc_hash_func);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_switch_load_balancing_lb_field_enablers_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_switch_load_balancing_lb_field_enablers_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_table_lb_field_enablers);
    data = (const dnx_data_switch_load_balancing_lb_field_enablers_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->field_enablers_mask);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_switch_load_balancing_lb_clients_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_table_lb_clients);

}

const dnxc_data_table_info_t *
dnx_data_switch_load_balancing_lb_client_crc_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_table_lb_client_crc);

}

const dnxc_data_table_info_t *
dnx_data_switch_load_balancing_lb_field_enablers_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_table_lb_field_enablers);

}






static shr_error_e
dnx_data_switch_load_balancing_sim_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "load_balancing_sim";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_switch_load_balancing_sim_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data switch load_balancing_sim features");

    
    submodule_data->nof_defines = _dnx_data_switch_load_balancing_sim_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data switch load_balancing_sim defines");

    submodule_data->defines[dnx_data_switch_load_balancing_sim_define_device_enum].name = "device_enum";
    submodule_data->defines[dnx_data_switch_load_balancing_sim_define_device_enum].doc = "";
    
    submodule_data->defines[dnx_data_switch_load_balancing_sim_define_device_enum].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_switch_load_balancing_sim_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data switch load_balancing_sim tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_switch_load_balancing_sim_feature_get(
    int unit,
    dnx_data_switch_load_balancing_sim_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing_sim, feature);
}


uint32
dnx_data_switch_load_balancing_sim_device_enum_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing_sim, dnx_data_switch_load_balancing_sim_define_device_enum);
}










static shr_error_e
dnx_data_switch_svtag_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "svtag";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_switch_svtag_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data switch svtag features");

    submodule_data->features[dnx_data_switch_svtag_egress_svtag_enable_by_context].name = "egress_svtag_enable_by_context";
    submodule_data->features[dnx_data_switch_svtag_egress_svtag_enable_by_context].doc = "";
    submodule_data->features[dnx_data_switch_svtag_egress_svtag_enable_by_context].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_switch_svtag_egress_svtag_legacy_api_support].name = "egress_svtag_legacy_api_support";
    submodule_data->features[dnx_data_switch_svtag_egress_svtag_legacy_api_support].doc = "";
    submodule_data->features[dnx_data_switch_svtag_egress_svtag_legacy_api_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_switch_svtag_enable_by_defualt].name = "enable_by_defualt";
    submodule_data->features[dnx_data_switch_svtag_enable_by_defualt].doc = "";
    submodule_data->features[dnx_data_switch_svtag_enable_by_defualt].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_switch_svtag_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data switch svtag defines");

    submodule_data->defines[dnx_data_switch_svtag_define_supported].name = "supported";
    submodule_data->defines[dnx_data_switch_svtag_define_supported].doc = "";
    
    submodule_data->defines[dnx_data_switch_svtag_define_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_svtag_label_size_bits].name = "svtag_label_size_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_svtag_label_size_bits].doc = "";
    
    submodule_data->defines[dnx_data_switch_svtag_define_svtag_label_size_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_offset_addr_size_bits].name = "egress_svtag_offset_addr_size_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_offset_addr_size_bits].doc = "";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_offset_addr_size_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_offset_addr_position_bits].name = "egress_svtag_offset_addr_position_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_offset_addr_position_bits].doc = "";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_offset_addr_position_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_signature_size_bits].name = "egress_svtag_signature_size_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_signature_size_bits].doc = "";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_signature_size_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_signature_position_bits].name = "egress_svtag_signature_position_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_signature_position_bits].doc = "";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_signature_position_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_sci_size_bits].name = "egress_svtag_sci_size_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_sci_size_bits].doc = "";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_sci_size_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_sci_position_bits].name = "egress_svtag_sci_position_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_sci_position_bits].doc = "";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_sci_position_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_pkt_type_size_bits].name = "egress_svtag_pkt_type_size_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_pkt_type_size_bits].doc = "";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_pkt_type_size_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_pkt_type_position_bits].name = "egress_svtag_pkt_type_position_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_pkt_type_position_bits].doc = "";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_pkt_type_position_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_ipv6_indication_position_bits].name = "egress_svtag_ipv6_indication_position_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_ipv6_indication_position_bits].doc = "";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_ipv6_indication_position_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_hw_field_position_bits].name = "egress_svtag_hw_field_position_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_hw_field_position_bits].doc = "";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_hw_field_position_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_accumulation_indication_hw_field_position_bits].name = "egress_svtag_accumulation_indication_hw_field_position_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_accumulation_indication_hw_field_position_bits].doc = "";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_accumulation_indication_hw_field_position_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_UDP_dedicated_port].name = "UDP_dedicated_port";
    submodule_data->defines[dnx_data_switch_svtag_define_UDP_dedicated_port].doc = "";
    
    submodule_data->defines[dnx_data_switch_svtag_define_UDP_dedicated_port].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_ingress_svtag_position_bytes].name = "ingress_svtag_position_bytes";
    submodule_data->defines[dnx_data_switch_svtag_define_ingress_svtag_position_bytes].doc = "";
    
    submodule_data->defines[dnx_data_switch_svtag_define_ingress_svtag_position_bytes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_svtag_physical_db].name = "svtag_physical_db";
    submodule_data->defines[dnx_data_switch_svtag_define_svtag_physical_db].doc = "";
    
    submodule_data->defines[dnx_data_switch_svtag_define_svtag_physical_db].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_switch_svtag_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data switch svtag tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_switch_svtag_feature_get(
    int unit,
    dnx_data_switch_svtag_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, feature);
}


uint32
dnx_data_switch_svtag_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_supported);
}

uint32
dnx_data_switch_svtag_svtag_label_size_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_svtag_label_size_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_offset_addr_size_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_offset_addr_size_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_offset_addr_position_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_offset_addr_position_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_signature_size_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_signature_size_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_signature_position_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_signature_position_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_sci_size_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_sci_size_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_sci_position_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_sci_position_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_pkt_type_size_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_pkt_type_size_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_pkt_type_position_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_pkt_type_position_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_ipv6_indication_position_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_ipv6_indication_position_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_hw_field_position_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_hw_field_position_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_accumulation_indication_hw_field_position_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_accumulation_indication_hw_field_position_bits);
}

uint32
dnx_data_switch_svtag_UDP_dedicated_port_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_UDP_dedicated_port);
}

uint32
dnx_data_switch_svtag_ingress_svtag_position_bytes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_ingress_svtag_position_bytes);
}

uint32
dnx_data_switch_svtag_svtag_physical_db_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_svtag_physical_db);
}










static shr_error_e
dnx_data_switch_feature_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "feature";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_switch_feature_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data switch feature features");

    submodule_data->features[dnx_data_switch_feature_mpls_labels_included_in_hash].name = "mpls_labels_included_in_hash";
    submodule_data->features[dnx_data_switch_feature_mpls_labels_included_in_hash].doc = "";
    submodule_data->features[dnx_data_switch_feature_mpls_labels_included_in_hash].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_switch_feature_excluded_header_bits_from_hash].name = "excluded_header_bits_from_hash";
    submodule_data->features[dnx_data_switch_feature_excluded_header_bits_from_hash].doc = "";
    submodule_data->features[dnx_data_switch_feature_excluded_header_bits_from_hash].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_switch_feature_silent_dummy_lif_lookup].name = "silent_dummy_lif_lookup";
    submodule_data->features[dnx_data_switch_feature_silent_dummy_lif_lookup].doc = "";
    submodule_data->features[dnx_data_switch_feature_silent_dummy_lif_lookup].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_switch_feature_fwd1_default_result_support].name = "fwd1_default_result_support";
    submodule_data->features[dnx_data_switch_feature_fwd1_default_result_support].doc = "";
    submodule_data->features[dnx_data_switch_feature_fwd1_default_result_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_switch_feature_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data switch feature defines");

    
    submodule_data->nof_tables = _dnx_data_switch_feature_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data switch feature tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_switch_feature_feature_get(
    int unit,
    dnx_data_switch_feature_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_feature, feature);
}











static shr_error_e
dnx_data_switch_wide_data_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "wide_data";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_switch_wide_data_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data switch wide_data features");

    submodule_data->features[dnx_data_switch_wide_data_ext_key_by_global_ac_lif].name = "ext_key_by_global_ac_lif";
    submodule_data->features[dnx_data_switch_wide_data_ext_key_by_global_ac_lif].doc = "";
    submodule_data->features[dnx_data_switch_wide_data_ext_key_by_global_ac_lif].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_switch_wide_data_ext_key_by_global_eth_rif].name = "ext_key_by_global_eth_rif";
    submodule_data->features[dnx_data_switch_wide_data_ext_key_by_global_eth_rif].doc = "";
    submodule_data->features[dnx_data_switch_wide_data_ext_key_by_global_eth_rif].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_switch_wide_data_ext_key_by_data].name = "ext_key_by_data";
    submodule_data->features[dnx_data_switch_wide_data_ext_key_by_data].doc = "";
    submodule_data->features[dnx_data_switch_wide_data_ext_key_by_data].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_switch_wide_data_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data switch wide_data defines");

    
    submodule_data->nof_tables = _dnx_data_switch_wide_data_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data switch wide_data tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_switch_wide_data_feature_get(
    int unit,
    dnx_data_switch_wide_data_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_wide_data, feature);
}








shr_error_e
dnx_data_switch_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "switch";
    module_data->nof_submodules = _dnx_data_switch_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data switch submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_switch_load_balancing_init(unit, &module_data->submodules[dnx_data_switch_submodule_load_balancing]));
    SHR_IF_ERR_EXIT(dnx_data_switch_load_balancing_sim_init(unit, &module_data->submodules[dnx_data_switch_submodule_load_balancing_sim]));
    SHR_IF_ERR_EXIT(dnx_data_switch_svtag_init(unit, &module_data->submodules[dnx_data_switch_submodule_svtag]));
    SHR_IF_ERR_EXIT(dnx_data_switch_feature_init(unit, &module_data->submodules[dnx_data_switch_submodule_feature]));
    SHR_IF_ERR_EXIT(dnx_data_switch_wide_data_init(unit, &module_data->submodules[dnx_data_switch_submodule_wide_data]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_switch_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_switch_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_switch_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_switch_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_switch_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_switch_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_switch_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_switch_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_switch_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_switch_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_switch_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_switch_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_switch_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_switch_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_switch_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_switch_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_switch_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_switch_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_switch_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_switch_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_switch_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_switch_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_switch_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_switch_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_switch_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

