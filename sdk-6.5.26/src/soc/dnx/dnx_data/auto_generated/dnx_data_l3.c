
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_l3.h>




extern shr_error_e jr2_a0_data_l3_attach(
    int unit);


extern shr_error_e jr2_b0_data_l3_attach(
    int unit);


extern shr_error_e j2c_a0_data_l3_attach(
    int unit);


extern shr_error_e q2a_a0_data_l3_attach(
    int unit);


extern shr_error_e j2p_a0_data_l3_attach(
    int unit);


extern shr_error_e j2x_a0_data_l3_attach(
    int unit);




static shr_error_e
dnx_data_l3_egr_pointed_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "egr_pointed";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l3_egr_pointed_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 egr_pointed features");

    
    submodule_data->nof_defines = _dnx_data_l3_egr_pointed_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 egr_pointed defines");

    submodule_data->defines[dnx_data_l3_egr_pointed_define_first_egr_pointed_id].name = "first_egr_pointed_id";
    submodule_data->defines[dnx_data_l3_egr_pointed_define_first_egr_pointed_id].doc = "";
    
    submodule_data->defines[dnx_data_l3_egr_pointed_define_first_egr_pointed_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_egr_pointed_define_nof_egr_pointed_ids].name = "nof_egr_pointed_ids";
    submodule_data->defines[dnx_data_l3_egr_pointed_define_nof_egr_pointed_ids].doc = "";
    
    submodule_data->defines[dnx_data_l3_egr_pointed_define_nof_egr_pointed_ids].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l3_egr_pointed_define_nof_bits_in_egr_pointed_id].name = "nof_bits_in_egr_pointed_id";
    submodule_data->defines[dnx_data_l3_egr_pointed_define_nof_bits_in_egr_pointed_id].doc = "";
    
    submodule_data->defines[dnx_data_l3_egr_pointed_define_nof_bits_in_egr_pointed_id].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_l3_egr_pointed_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 egr_pointed tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_egr_pointed_feature_get(
    int unit,
    dnx_data_l3_egr_pointed_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_egr_pointed, feature);
}


uint32
dnx_data_l3_egr_pointed_first_egr_pointed_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_egr_pointed, dnx_data_l3_egr_pointed_define_first_egr_pointed_id);
}

uint32
dnx_data_l3_egr_pointed_nof_egr_pointed_ids_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_egr_pointed, dnx_data_l3_egr_pointed_define_nof_egr_pointed_ids);
}

uint32
dnx_data_l3_egr_pointed_nof_bits_in_egr_pointed_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_egr_pointed, dnx_data_l3_egr_pointed_define_nof_bits_in_egr_pointed_id);
}










static shr_error_e
dnx_data_l3_fec_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "fec";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l3_fec_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 fec features");

    submodule_data->features[dnx_data_l3_fec_uneven_bank_sizes].name = "uneven_bank_sizes";
    submodule_data->features[dnx_data_l3_fec_uneven_bank_sizes].doc = "";
    submodule_data->features[dnx_data_l3_fec_uneven_bank_sizes].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_fec_mc_rpf_sip_based_supported].name = "mc_rpf_sip_based_supported";
    submodule_data->features[dnx_data_l3_fec_mc_rpf_sip_based_supported].doc = "";
    submodule_data->features[dnx_data_l3_fec_mc_rpf_sip_based_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_l3_fec_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 fec defines");

    submodule_data->defines[dnx_data_l3_fec_define_nof_fecs].name = "nof_fecs";
    submodule_data->defines[dnx_data_l3_fec_define_nof_fecs].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_nof_fecs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_first_valid_fec_ecmp_id].name = "first_valid_fec_ecmp_id";
    submodule_data->defines[dnx_data_l3_fec_define_first_valid_fec_ecmp_id].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_first_valid_fec_ecmp_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_max_fec_id_for_single_dhb_cluster_pair_granularity].name = "max_fec_id_for_single_dhb_cluster_pair_granularity";
    submodule_data->defines[dnx_data_l3_fec_define_max_fec_id_for_single_dhb_cluster_pair_granularity].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_max_fec_id_for_single_dhb_cluster_pair_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_max_fec_id_for_double_dhb_cluster_pair_granularity].name = "max_fec_id_for_double_dhb_cluster_pair_granularity";
    submodule_data->defines[dnx_data_l3_fec_define_max_fec_id_for_double_dhb_cluster_pair_granularity].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_max_fec_id_for_double_dhb_cluster_pair_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_max_default_fec].name = "max_default_fec";
    submodule_data->defines[dnx_data_l3_fec_define_max_default_fec].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_max_default_fec].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_bank_size].name = "bank_size";
    submodule_data->defines[dnx_data_l3_fec_define_bank_size].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_bank_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_bank_size_round_up].name = "bank_size_round_up";
    submodule_data->defines[dnx_data_l3_fec_define_bank_size_round_up].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_bank_size_round_up].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_max_nof_super_fecs_per_bank].name = "max_nof_super_fecs_per_bank";
    submodule_data->defines[dnx_data_l3_fec_define_max_nof_super_fecs_per_bank].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_max_nof_super_fecs_per_bank].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_nof_physical_fecs_per_bank].name = "nof_physical_fecs_per_bank";
    submodule_data->defines[dnx_data_l3_fec_define_nof_physical_fecs_per_bank].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_nof_physical_fecs_per_bank].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_bank_nof_physical_rows].name = "bank_nof_physical_rows";
    submodule_data->defines[dnx_data_l3_fec_define_bank_nof_physical_rows].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_bank_nof_physical_rows].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_super_fec_size].name = "super_fec_size";
    submodule_data->defines[dnx_data_l3_fec_define_super_fec_size].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_super_fec_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_max_nof_banks].name = "max_nof_banks";
    submodule_data->defines[dnx_data_l3_fec_define_max_nof_banks].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_max_nof_banks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_first_bank_without_id_alloc].name = "first_bank_without_id_alloc";
    submodule_data->defines[dnx_data_l3_fec_define_first_bank_without_id_alloc].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_first_bank_without_id_alloc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_first_shared_bank].name = "first_shared_bank";
    submodule_data->defines[dnx_data_l3_fec_define_first_shared_bank].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_first_shared_bank].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_max_super_fec_id].name = "max_super_fec_id";
    submodule_data->defines[dnx_data_l3_fec_define_max_super_fec_id].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_max_super_fec_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_nof_fec_directions].name = "nof_fec_directions";
    submodule_data->defines[dnx_data_l3_fec_define_nof_fec_directions].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_nof_fec_directions].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_fec_z_max_fec_id].name = "fec_z_max_fec_id";
    submodule_data->defines[dnx_data_l3_fec_define_fec_z_max_fec_id].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_fec_z_max_fec_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_nof_dpc_fec_db].name = "nof_dpc_fec_db";
    submodule_data->defines[dnx_data_l3_fec_define_nof_dpc_fec_db].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_nof_dpc_fec_db].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_nof_fec_dbs_all_cores].name = "nof_fec_dbs_all_cores";
    submodule_data->defines[dnx_data_l3_fec_define_nof_fec_dbs_all_cores].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_nof_fec_dbs_all_cores].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_nof_fec_result_types].name = "nof_fec_result_types";
    submodule_data->defines[dnx_data_l3_fec_define_nof_fec_result_types].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_nof_fec_result_types].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_fer_hw_version].name = "fer_hw_version";
    submodule_data->defines[dnx_data_l3_fec_define_fer_hw_version].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_fer_hw_version].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l3_fec_define_fec_property_supported_flags].name = "fec_property_supported_flags";
    submodule_data->defines[dnx_data_l3_fec_define_fec_property_supported_flags].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_fec_property_supported_flags].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l3_fec_define_supported_fec_flags].name = "supported_fec_flags";
    submodule_data->defines[dnx_data_l3_fec_define_supported_fec_flags].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_supported_fec_flags].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l3_fec_define_supported_fec_flags2].name = "supported_fec_flags2";
    submodule_data->defines[dnx_data_l3_fec_define_supported_fec_flags2].doc = "";
    
    submodule_data->defines[dnx_data_l3_fec_define_supported_fec_flags2].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_l3_fec_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 fec tables");

    
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].name = "fec_resource_info";
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].doc = "";
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].size_of_values = sizeof(dnx_data_l3_fec_fec_resource_info_t);
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].entry_get = dnx_data_l3_fec_fec_resource_info_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].nof_keys = 1;
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].keys[0].name = "resource_type";
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].nof_values, "_dnx_data_l3_fec_table_fec_resource_info table values");
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].values[0].name = "is_protection_type";
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].values[0].type = "uint8";
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].values[0].doc = "FEC protection state information.";
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_fec_fec_resource_info_t, is_protection_type);
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].values[1].name = "is_statistic_type";
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].values[1].type = "uint8";
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].values[1].doc = "FEC statistic state information.";
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_l3_fec_fec_resource_info_t, is_statistic_type);
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].values[2].name = "nof_statistics";
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].values[2].type = "uint8";
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].values[2].doc = "Number of FEC statistic instances";
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_l3_fec_fec_resource_info_t, nof_statistics);
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].values[3].name = "dbal_result_type";
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].values[3].type = "dbal_result_type_t";
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].values[3].doc = "FEC dbal result type.";
    submodule_data->tables[dnx_data_l3_fec_table_fec_resource_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_l3_fec_fec_resource_info_t, dbal_result_type);

    
    submodule_data->tables[dnx_data_l3_fec_table_super_fec_result_types_map].name = "super_fec_result_types_map";
    submodule_data->tables[dnx_data_l3_fec_table_super_fec_result_types_map].doc = "";
    submodule_data->tables[dnx_data_l3_fec_table_super_fec_result_types_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_fec_table_super_fec_result_types_map].size_of_values = sizeof(dnx_data_l3_fec_super_fec_result_types_map_t);
    submodule_data->tables[dnx_data_l3_fec_table_super_fec_result_types_map].entry_get = dnx_data_l3_fec_super_fec_result_types_map_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_fec_table_super_fec_result_types_map].nof_keys = 1;
    submodule_data->tables[dnx_data_l3_fec_table_super_fec_result_types_map].keys[0].name = "dbal_result_type";
    submodule_data->tables[dnx_data_l3_fec_table_super_fec_result_types_map].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_l3_fec_table_super_fec_result_types_map].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_fec_table_super_fec_result_types_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_fec_table_super_fec_result_types_map].nof_values, "_dnx_data_l3_fec_table_super_fec_result_types_map table values");
    submodule_data->tables[dnx_data_l3_fec_table_super_fec_result_types_map].values[0].name = "fec_type_in_super_fec";
    submodule_data->tables[dnx_data_l3_fec_table_super_fec_result_types_map].values[0].type = "dbal_fields_e";
    submodule_data->tables[dnx_data_l3_fec_table_super_fec_result_types_map].values[0].doc = "FEC type in Super FEC.";
    submodule_data->tables[dnx_data_l3_fec_table_super_fec_result_types_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_fec_super_fec_result_types_map_t, fec_type_in_super_fec);
    submodule_data->tables[dnx_data_l3_fec_table_super_fec_result_types_map].values[1].name = "fec_resource_type";
    submodule_data->tables[dnx_data_l3_fec_table_super_fec_result_types_map].values[1].type = "uint32";
    submodule_data->tables[dnx_data_l3_fec_table_super_fec_result_types_map].values[1].doc = "FEC resource type.";
    submodule_data->tables[dnx_data_l3_fec_table_super_fec_result_types_map].values[1].offset = UTILEX_OFFSETOF(dnx_data_l3_fec_super_fec_result_types_map_t, fec_resource_type);

    
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].name = "fec_result_type_fields";
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].doc = "";
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].size_of_values = sizeof(dnx_data_l3_fec_fec_result_type_fields_t);
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].entry_get = dnx_data_l3_fec_fec_result_type_fields_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].nof_keys = 1;
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].keys[0].name = "result_type";
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].nof_values = 5;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].nof_values, "_dnx_data_l3_fec_table_fec_result_type_fields table values");
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].values[0].name = "has_global_outlif";
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].values[0].type = "uint8";
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].values[0].doc = "Indication if GLOBAL_OUT_LIF field exists in this result type.";
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_fec_fec_result_type_fields_t, has_global_outlif);
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].values[1].name = "has_global_outlif_second";
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].values[1].type = "uint8";
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].values[1].doc = "Indication if GLOBAL_OUT_LIF_2ND field exists in this result type.";
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].values[1].offset = UTILEX_OFFSETOF(dnx_data_l3_fec_fec_result_type_fields_t, has_global_outlif_second);
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].values[2].name = "has_mc_rpf";
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].values[2].type = "uint8";
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].values[2].doc = "Indication if MC_RPF_MODE field exists in this result type.";
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].values[2].offset = UTILEX_OFFSETOF(dnx_data_l3_fec_fec_result_type_fields_t, has_mc_rpf);
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].values[3].name = "has_eei";
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].values[3].type = "uint8";
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].values[3].doc = "Indication if EEI field exists in this result type.";
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].values[3].offset = UTILEX_OFFSETOF(dnx_data_l3_fec_fec_result_type_fields_t, has_eei);
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].values[4].name = "has_htm";
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].values[4].type = "uint8";
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].values[4].doc = "Indication if HIERARCHICAL_TM_FLOW field exists in this result type.";
    submodule_data->tables[dnx_data_l3_fec_table_fec_result_type_fields].values[4].offset = UTILEX_OFFSETOF(dnx_data_l3_fec_fec_result_type_fields_t, has_htm);

    
    submodule_data->tables[dnx_data_l3_fec_table_fec_physical_db].name = "fec_physical_db";
    submodule_data->tables[dnx_data_l3_fec_table_fec_physical_db].doc = "";
    submodule_data->tables[dnx_data_l3_fec_table_fec_physical_db].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_fec_table_fec_physical_db].size_of_values = sizeof(dnx_data_l3_fec_fec_physical_db_t);
    submodule_data->tables[dnx_data_l3_fec_table_fec_physical_db].entry_get = dnx_data_l3_fec_fec_physical_db_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_fec_table_fec_physical_db].nof_keys = 0;

    
    submodule_data->tables[dnx_data_l3_fec_table_fec_physical_db].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_fec_table_fec_physical_db].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_fec_table_fec_physical_db].nof_values, "_dnx_data_l3_fec_table_fec_physical_db table values");
    submodule_data->tables[dnx_data_l3_fec_table_fec_physical_db].values[0].name = "physical_table";
    submodule_data->tables[dnx_data_l3_fec_table_fec_physical_db].values[0].type = "mdb_physical_table_e[6]";
    submodule_data->tables[dnx_data_l3_fec_table_fec_physical_db].values[0].doc = "An array of FEC physical DBs.";
    submodule_data->tables[dnx_data_l3_fec_table_fec_physical_db].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_fec_fec_physical_db_t, physical_table);

    
    submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].name = "fec_tables_info";
    submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].doc = "";
    submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].size_of_values = sizeof(dnx_data_l3_fec_fec_tables_info_t);
    submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].entry_get = dnx_data_l3_fec_fec_tables_info_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].nof_keys = 1;
    submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].keys[0].name = "mdb_table";
    submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].nof_values, "_dnx_data_l3_fec_table_fec_tables_info table values");
    submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].values[0].doc = "The FEC DBAL table name that should be used for the given key.";
    submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_fec_fec_tables_info_t, dbal_table);
    submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].values[1].name = "hierarchy";
    submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].values[1].doc = "Hierarchy information.";
    submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_l3_fec_fec_tables_info_t, hierarchy);
    submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].values[2].name = "index";
    submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].values[2].type = "int";
    submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].values[2].doc = "Consecutive indexes of the FEC physical databases.";
    submodule_data->tables[dnx_data_l3_fec_table_fec_tables_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_l3_fec_fec_tables_info_t, index);

    
    submodule_data->tables[dnx_data_l3_fec_table_fec_hierachy_position_in_range].name = "fec_hierachy_position_in_range";
    submodule_data->tables[dnx_data_l3_fec_table_fec_hierachy_position_in_range].doc = "";
    submodule_data->tables[dnx_data_l3_fec_table_fec_hierachy_position_in_range].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_fec_table_fec_hierachy_position_in_range].size_of_values = sizeof(dnx_data_l3_fec_fec_hierachy_position_in_range_t);
    submodule_data->tables[dnx_data_l3_fec_table_fec_hierachy_position_in_range].entry_get = dnx_data_l3_fec_fec_hierachy_position_in_range_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_fec_table_fec_hierachy_position_in_range].nof_keys = 1;
    submodule_data->tables[dnx_data_l3_fec_table_fec_hierachy_position_in_range].keys[0].name = "hierarchy";
    submodule_data->tables[dnx_data_l3_fec_table_fec_hierachy_position_in_range].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_l3_fec_table_fec_hierachy_position_in_range].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_fec_table_fec_hierachy_position_in_range].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_fec_table_fec_hierachy_position_in_range].nof_values, "_dnx_data_l3_fec_table_fec_hierachy_position_in_range table values");
    submodule_data->tables[dnx_data_l3_fec_table_fec_hierachy_position_in_range].values[0].name = "forced_position";
    submodule_data->tables[dnx_data_l3_fec_table_fec_hierachy_position_in_range].values[0].type = "uint32";
    submodule_data->tables[dnx_data_l3_fec_table_fec_hierachy_position_in_range].values[0].doc = "The position number of the hierarchy";
    submodule_data->tables[dnx_data_l3_fec_table_fec_hierachy_position_in_range].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_fec_fec_hierachy_position_in_range_t, forced_position);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_fec_feature_get(
    int unit,
    dnx_data_l3_fec_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, feature);
}


uint32
dnx_data_l3_fec_nof_fecs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_nof_fecs);
}

uint32
dnx_data_l3_fec_first_valid_fec_ecmp_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_first_valid_fec_ecmp_id);
}

uint32
dnx_data_l3_fec_max_fec_id_for_single_dhb_cluster_pair_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_max_fec_id_for_single_dhb_cluster_pair_granularity);
}

uint32
dnx_data_l3_fec_max_fec_id_for_double_dhb_cluster_pair_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_max_fec_id_for_double_dhb_cluster_pair_granularity);
}

uint32
dnx_data_l3_fec_max_default_fec_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_max_default_fec);
}

uint32
dnx_data_l3_fec_bank_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_bank_size);
}

uint32
dnx_data_l3_fec_bank_size_round_up_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_bank_size_round_up);
}

uint32
dnx_data_l3_fec_max_nof_super_fecs_per_bank_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_max_nof_super_fecs_per_bank);
}

uint32
dnx_data_l3_fec_nof_physical_fecs_per_bank_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_nof_physical_fecs_per_bank);
}

uint32
dnx_data_l3_fec_bank_nof_physical_rows_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_bank_nof_physical_rows);
}

uint32
dnx_data_l3_fec_super_fec_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_super_fec_size);
}

uint32
dnx_data_l3_fec_max_nof_banks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_max_nof_banks);
}

uint32
dnx_data_l3_fec_first_bank_without_id_alloc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_first_bank_without_id_alloc);
}

uint32
dnx_data_l3_fec_first_shared_bank_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_first_shared_bank);
}

uint32
dnx_data_l3_fec_max_super_fec_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_max_super_fec_id);
}

uint32
dnx_data_l3_fec_nof_fec_directions_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_nof_fec_directions);
}

uint32
dnx_data_l3_fec_fec_z_max_fec_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_fec_z_max_fec_id);
}

uint32
dnx_data_l3_fec_nof_dpc_fec_db_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_nof_dpc_fec_db);
}

uint32
dnx_data_l3_fec_nof_fec_dbs_all_cores_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_nof_fec_dbs_all_cores);
}

uint32
dnx_data_l3_fec_nof_fec_result_types_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_nof_fec_result_types);
}

uint32
dnx_data_l3_fec_fer_hw_version_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_fer_hw_version);
}

uint32
dnx_data_l3_fec_fec_property_supported_flags_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_fec_property_supported_flags);
}

uint32
dnx_data_l3_fec_supported_fec_flags_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_supported_fec_flags);
}

uint32
dnx_data_l3_fec_supported_fec_flags2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_supported_fec_flags2);
}



const dnx_data_l3_fec_fec_resource_info_t *
dnx_data_l3_fec_fec_resource_info_get(
    int unit,
    int resource_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_table_fec_resource_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, resource_type, 0);
    return (const dnx_data_l3_fec_fec_resource_info_t *) data;

}

const dnx_data_l3_fec_super_fec_result_types_map_t *
dnx_data_l3_fec_super_fec_result_types_map_get(
    int unit,
    int dbal_result_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_table_super_fec_result_types_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_result_type, 0);
    return (const dnx_data_l3_fec_super_fec_result_types_map_t *) data;

}

const dnx_data_l3_fec_fec_result_type_fields_t *
dnx_data_l3_fec_fec_result_type_fields_get(
    int unit,
    int result_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_table_fec_result_type_fields);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, result_type, 0);
    return (const dnx_data_l3_fec_fec_result_type_fields_t *) data;

}

const dnx_data_l3_fec_fec_physical_db_t *
dnx_data_l3_fec_fec_physical_db_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_table_fec_physical_db);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_l3_fec_fec_physical_db_t *) data;

}

const dnx_data_l3_fec_fec_tables_info_t *
dnx_data_l3_fec_fec_tables_info_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_table_fec_tables_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_l3_fec_fec_tables_info_t *) data;

}

const dnx_data_l3_fec_fec_hierachy_position_in_range_t *
dnx_data_l3_fec_fec_hierachy_position_in_range_get(
    int unit,
    int hierarchy)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_table_fec_hierachy_position_in_range);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, hierarchy, 0);
    return (const dnx_data_l3_fec_fec_hierachy_position_in_range_t *) data;

}


shr_error_e
dnx_data_l3_fec_fec_resource_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_fec_fec_resource_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_table_fec_resource_info);
    data = (const dnx_data_l3_fec_fec_resource_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_protection_type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_statistic_type);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_statistics);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_result_type);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_l3_fec_super_fec_result_types_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_fec_super_fec_result_types_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_table_super_fec_result_types_map);
    data = (const dnx_data_l3_fec_super_fec_result_types_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fec_type_in_super_fec);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fec_resource_type);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_l3_fec_fec_result_type_fields_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_fec_fec_result_type_fields_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_table_fec_result_type_fields);
    data = (const dnx_data_l3_fec_fec_result_type_fields_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->has_global_outlif);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->has_global_outlif_second);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->has_mc_rpf);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->has_eei);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->has_htm);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_l3_fec_fec_physical_db_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_fec_fec_physical_db_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_table_fec_physical_db);
    data = (const dnx_data_l3_fec_fec_physical_db_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, 6, data->physical_table);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_l3_fec_fec_tables_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_fec_fec_tables_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_table_fec_tables_info);
    data = (const dnx_data_l3_fec_fec_tables_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->hierarchy);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->index);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_l3_fec_fec_hierachy_position_in_range_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_fec_fec_hierachy_position_in_range_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_table_fec_hierachy_position_in_range);
    data = (const dnx_data_l3_fec_fec_hierachy_position_in_range_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->forced_position);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_l3_fec_fec_resource_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_table_fec_resource_info);

}

const dnxc_data_table_info_t *
dnx_data_l3_fec_super_fec_result_types_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_table_super_fec_result_types_map);

}

const dnxc_data_table_info_t *
dnx_data_l3_fec_fec_result_type_fields_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_table_fec_result_type_fields);

}

const dnxc_data_table_info_t *
dnx_data_l3_fec_fec_physical_db_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_table_fec_physical_db);

}

const dnxc_data_table_info_t *
dnx_data_l3_fec_fec_tables_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_table_fec_tables_info);

}

const dnxc_data_table_info_t *
dnx_data_l3_fec_fec_hierachy_position_in_range_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_table_fec_hierachy_position_in_range);

}






static shr_error_e
dnx_data_l3_fer_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "fer";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l3_fer_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 fer features");

    submodule_data->features[dnx_data_l3_fer_mux_connection_supported].name = "mux_connection_supported";
    submodule_data->features[dnx_data_l3_fer_mux_connection_supported].doc = "";
    submodule_data->features[dnx_data_l3_fer_mux_connection_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_l3_fer_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 fer defines");

    submodule_data->defines[dnx_data_l3_fer_define_mdb_granularity_ratio].name = "mdb_granularity_ratio";
    submodule_data->defines[dnx_data_l3_fer_define_mdb_granularity_ratio].doc = "";
    
    submodule_data->defines[dnx_data_l3_fer_define_mdb_granularity_ratio].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fer_define_nof_fer_sort_to_mdb_table_muxes].name = "nof_fer_sort_to_mdb_table_muxes";
    submodule_data->defines[dnx_data_l3_fer_define_nof_fer_sort_to_mdb_table_muxes].doc = "";
    
    submodule_data->defines[dnx_data_l3_fer_define_nof_fer_sort_to_mdb_table_muxes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fer_define_max_nof_sorting_blocks_per_fec_db].name = "max_nof_sorting_blocks_per_fec_db";
    submodule_data->defines[dnx_data_l3_fer_define_max_nof_sorting_blocks_per_fec_db].doc = "";
    
    submodule_data->defines[dnx_data_l3_fer_define_max_nof_sorting_blocks_per_fec_db].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l3_fer_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 fer tables");

    
    submodule_data->tables[dnx_data_l3_fer_table_mux_configuration].name = "mux_configuration";
    submodule_data->tables[dnx_data_l3_fer_table_mux_configuration].doc = "";
    submodule_data->tables[dnx_data_l3_fer_table_mux_configuration].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_fer_table_mux_configuration].size_of_values = sizeof(dnx_data_l3_fer_mux_configuration_t);
    submodule_data->tables[dnx_data_l3_fer_table_mux_configuration].entry_get = dnx_data_l3_fer_mux_configuration_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_fer_table_mux_configuration].nof_keys = 1;
    submodule_data->tables[dnx_data_l3_fer_table_mux_configuration].keys[0].name = "mux_id";
    submodule_data->tables[dnx_data_l3_fer_table_mux_configuration].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_l3_fer_table_mux_configuration].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_fer_table_mux_configuration].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_fer_table_mux_configuration].nof_values, "_dnx_data_l3_fer_table_mux_configuration table values");
    submodule_data->tables[dnx_data_l3_fer_table_mux_configuration].values[0].name = "direction";
    submodule_data->tables[dnx_data_l3_fer_table_mux_configuration].values[0].type = "uint32";
    submodule_data->tables[dnx_data_l3_fer_table_mux_configuration].values[0].doc = "The FEC direction(FWD or RPF) for which the MUX is configured.";
    submodule_data->tables[dnx_data_l3_fer_table_mux_configuration].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_fer_mux_configuration_t, direction);
    submodule_data->tables[dnx_data_l3_fer_table_mux_configuration].values[1].name = "hierarchy";
    submodule_data->tables[dnx_data_l3_fer_table_mux_configuration].values[1].type = "uint32";
    submodule_data->tables[dnx_data_l3_fer_table_mux_configuration].values[1].doc = "The FEC hierarchy for which the MUX is configured.";
    submodule_data->tables[dnx_data_l3_fer_table_mux_configuration].values[1].offset = UTILEX_OFFSETOF(dnx_data_l3_fer_mux_configuration_t, hierarchy);

    
    submodule_data->tables[dnx_data_l3_fer_table_mux_connection].name = "mux_connection";
    submodule_data->tables[dnx_data_l3_fer_table_mux_connection].doc = "";
    submodule_data->tables[dnx_data_l3_fer_table_mux_connection].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_fer_table_mux_connection].size_of_values = sizeof(dnx_data_l3_fer_mux_connection_t);
    submodule_data->tables[dnx_data_l3_fer_table_mux_connection].entry_get = dnx_data_l3_fer_mux_connection_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_fer_table_mux_connection].nof_keys = 1;
    submodule_data->tables[dnx_data_l3_fer_table_mux_connection].keys[0].name = "physical_db";
    submodule_data->tables[dnx_data_l3_fer_table_mux_connection].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_l3_fer_table_mux_connection].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_fer_table_mux_connection].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_fer_table_mux_connection].nof_values, "_dnx_data_l3_fer_table_mux_connection table values");
    submodule_data->tables[dnx_data_l3_fer_table_mux_connection].values[0].name = "mux_ids";
    submodule_data->tables[dnx_data_l3_fer_table_mux_connection].values[0].type = "uint32[3]";
    submodule_data->tables[dnx_data_l3_fer_table_mux_connection].values[0].doc = "The ID(s) of the MUX(es) that each FEC DB is connected to";
    submodule_data->tables[dnx_data_l3_fer_table_mux_connection].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_fer_mux_connection_t, mux_ids);
    submodule_data->tables[dnx_data_l3_fer_table_mux_connection].values[1].name = "nof_muxes";
    submodule_data->tables[dnx_data_l3_fer_table_mux_connection].values[1].type = "uint32";
    submodule_data->tables[dnx_data_l3_fer_table_mux_connection].values[1].doc = "Number of muxes connected to the physical FEC DB";
    submodule_data->tables[dnx_data_l3_fer_table_mux_connection].values[1].offset = UTILEX_OFFSETOF(dnx_data_l3_fer_mux_connection_t, nof_muxes);

    
    submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].name = "sorting_stages_info";
    submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].doc = "";
    submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].size_of_values = sizeof(dnx_data_l3_fer_sorting_stages_info_t);
    submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].entry_get = dnx_data_l3_fer_sorting_stages_info_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].nof_keys = 1;
    submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].keys[0].name = "sorting_stage";
    submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].nof_values, "_dnx_data_l3_fer_table_sorting_stages_info table values");
    submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].values[0].name = "bank_mapping_table";
    submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].values[0].doc = "FEC bank mapping table for the FER stage";
    submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_fer_sorting_stages_info_t, bank_mapping_table);
    submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].values[1].name = "direction";
    submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].values[1].doc = "The FEC direction(FWD or RPF) of the FER stage.";
    submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_l3_fer_sorting_stages_info_t, direction);
    submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].values[2].name = "hierarchy";
    submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].values[2].doc = "The FEC hierarchy associated with the FER stage.";
    submodule_data->tables[dnx_data_l3_fer_table_sorting_stages_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_l3_fer_sorting_stages_info_t, hierarchy);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_fer_feature_get(
    int unit,
    dnx_data_l3_fer_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fer, feature);
}


uint32
dnx_data_l3_fer_mdb_granularity_ratio_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fer, dnx_data_l3_fer_define_mdb_granularity_ratio);
}

uint32
dnx_data_l3_fer_nof_fer_sort_to_mdb_table_muxes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fer, dnx_data_l3_fer_define_nof_fer_sort_to_mdb_table_muxes);
}

uint32
dnx_data_l3_fer_max_nof_sorting_blocks_per_fec_db_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fer, dnx_data_l3_fer_define_max_nof_sorting_blocks_per_fec_db);
}



const dnx_data_l3_fer_mux_configuration_t *
dnx_data_l3_fer_mux_configuration_get(
    int unit,
    int mux_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fer, dnx_data_l3_fer_table_mux_configuration);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mux_id, 0);
    return (const dnx_data_l3_fer_mux_configuration_t *) data;

}

const dnx_data_l3_fer_mux_connection_t *
dnx_data_l3_fer_mux_connection_get(
    int unit,
    int physical_db)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fer, dnx_data_l3_fer_table_mux_connection);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, physical_db, 0);
    return (const dnx_data_l3_fer_mux_connection_t *) data;

}

const dnx_data_l3_fer_sorting_stages_info_t *
dnx_data_l3_fer_sorting_stages_info_get(
    int unit,
    int sorting_stage)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fer, dnx_data_l3_fer_table_sorting_stages_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, sorting_stage, 0);
    return (const dnx_data_l3_fer_sorting_stages_info_t *) data;

}


shr_error_e
dnx_data_l3_fer_mux_configuration_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_fer_mux_configuration_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fer, dnx_data_l3_fer_table_mux_configuration);
    data = (const dnx_data_l3_fer_mux_configuration_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->direction);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->hierarchy);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_l3_fer_mux_connection_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_fer_mux_connection_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fer, dnx_data_l3_fer_table_mux_connection);
    data = (const dnx_data_l3_fer_mux_connection_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, 3, data->mux_ids);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_muxes);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_l3_fer_sorting_stages_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_fer_sorting_stages_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fer, dnx_data_l3_fer_table_sorting_stages_info);
    data = (const dnx_data_l3_fer_sorting_stages_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->bank_mapping_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->direction);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->hierarchy);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_l3_fer_mux_configuration_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fer, dnx_data_l3_fer_table_mux_configuration);

}

const dnxc_data_table_info_t *
dnx_data_l3_fer_mux_connection_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fer, dnx_data_l3_fer_table_mux_connection);

}

const dnxc_data_table_info_t *
dnx_data_l3_fer_sorting_stages_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fer, dnx_data_l3_fer_table_sorting_stages_info);

}






static shr_error_e
dnx_data_l3_source_address_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "source_address";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l3_source_address_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 source_address features");

    
    submodule_data->nof_defines = _dnx_data_l3_source_address_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 source_address defines");

    submodule_data->defines[dnx_data_l3_source_address_define_source_address_table_size].name = "source_address_table_size";
    submodule_data->defines[dnx_data_l3_source_address_define_source_address_table_size].doc = "";
    
    submodule_data->defines[dnx_data_l3_source_address_define_source_address_table_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_source_address_define_nof_mymac_prefixes].name = "nof_mymac_prefixes";
    submodule_data->defines[dnx_data_l3_source_address_define_nof_mymac_prefixes].doc = "";
    
    submodule_data->defines[dnx_data_l3_source_address_define_nof_mymac_prefixes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_source_address_define_max_nof_mac_addresses_used_by_arp].name = "max_nof_mac_addresses_used_by_arp";
    submodule_data->defines[dnx_data_l3_source_address_define_max_nof_mac_addresses_used_by_arp].doc = "";
    
    submodule_data->defines[dnx_data_l3_source_address_define_max_nof_mac_addresses_used_by_arp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_source_address_define_custom_sa_use_dual_homing].name = "custom_sa_use_dual_homing";
    submodule_data->defines[dnx_data_l3_source_address_define_custom_sa_use_dual_homing].doc = "";
    
    submodule_data->defines[dnx_data_l3_source_address_define_custom_sa_use_dual_homing].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l3_source_address_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 source_address tables");

    
    submodule_data->tables[dnx_data_l3_source_address_table_address_type_info].name = "address_type_info";
    submodule_data->tables[dnx_data_l3_source_address_table_address_type_info].doc = "";
    submodule_data->tables[dnx_data_l3_source_address_table_address_type_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_source_address_table_address_type_info].size_of_values = sizeof(dnx_data_l3_source_address_address_type_info_t);
    submodule_data->tables[dnx_data_l3_source_address_table_address_type_info].entry_get = dnx_data_l3_source_address_address_type_info_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_source_address_table_address_type_info].nof_keys = 1;
    submodule_data->tables[dnx_data_l3_source_address_table_address_type_info].keys[0].name = "address_type";
    submodule_data->tables[dnx_data_l3_source_address_table_address_type_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_l3_source_address_table_address_type_info].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_source_address_table_address_type_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_source_address_table_address_type_info].nof_values, "_dnx_data_l3_source_address_table_address_type_info table values");
    submodule_data->tables[dnx_data_l3_source_address_table_address_type_info].values[0].name = "nof_entries";
    submodule_data->tables[dnx_data_l3_source_address_table_address_type_info].values[0].type = "uint32";
    submodule_data->tables[dnx_data_l3_source_address_table_address_type_info].values[0].doc = "The number of entries.";
    submodule_data->tables[dnx_data_l3_source_address_table_address_type_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_source_address_address_type_info_t, nof_entries);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_source_address_feature_get(
    int unit,
    dnx_data_l3_source_address_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_source_address, feature);
}


uint32
dnx_data_l3_source_address_source_address_table_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_source_address, dnx_data_l3_source_address_define_source_address_table_size);
}

uint32
dnx_data_l3_source_address_nof_mymac_prefixes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_source_address, dnx_data_l3_source_address_define_nof_mymac_prefixes);
}

uint32
dnx_data_l3_source_address_max_nof_mac_addresses_used_by_arp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_source_address, dnx_data_l3_source_address_define_max_nof_mac_addresses_used_by_arp);
}

uint32
dnx_data_l3_source_address_custom_sa_use_dual_homing_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_source_address, dnx_data_l3_source_address_define_custom_sa_use_dual_homing);
}



const dnx_data_l3_source_address_address_type_info_t *
dnx_data_l3_source_address_address_type_info_get(
    int unit,
    int address_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_source_address, dnx_data_l3_source_address_table_address_type_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, address_type, 0);
    return (const dnx_data_l3_source_address_address_type_info_t *) data;

}


shr_error_e
dnx_data_l3_source_address_address_type_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_source_address_address_type_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_source_address, dnx_data_l3_source_address_table_address_type_info);
    data = (const dnx_data_l3_source_address_address_type_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_entries);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_l3_source_address_address_type_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_source_address, dnx_data_l3_source_address_table_address_type_info);

}






static shr_error_e
dnx_data_l3_vrf_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "vrf";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l3_vrf_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 vrf features");

    
    submodule_data->nof_defines = _dnx_data_l3_vrf_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 vrf defines");

    submodule_data->defines[dnx_data_l3_vrf_define_nof_vrf].name = "nof_vrf";
    submodule_data->defines[dnx_data_l3_vrf_define_nof_vrf].doc = "";
    
    submodule_data->defines[dnx_data_l3_vrf_define_nof_vrf].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_vrf_define_nof_vrf_ipv6].name = "nof_vrf_ipv6";
    submodule_data->defines[dnx_data_l3_vrf_define_nof_vrf_ipv6].doc = "";
    
    submodule_data->defines[dnx_data_l3_vrf_define_nof_vrf_ipv6].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l3_vrf_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 vrf tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_vrf_feature_get(
    int unit,
    dnx_data_l3_vrf_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrf, feature);
}


uint32
dnx_data_l3_vrf_nof_vrf_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrf, dnx_data_l3_vrf_define_nof_vrf);
}

uint32
dnx_data_l3_vrf_nof_vrf_ipv6_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrf, dnx_data_l3_vrf_define_nof_vrf_ipv6);
}










static shr_error_e
dnx_data_l3_routing_enablers_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "routing_enablers";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l3_routing_enablers_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 routing_enablers features");

    submodule_data->features[dnx_data_l3_routing_enablers_ptc_routing_enable_profile_support].name = "ptc_routing_enable_profile_support";
    submodule_data->features[dnx_data_l3_routing_enablers_ptc_routing_enable_profile_support].doc = "";
    submodule_data->features[dnx_data_l3_routing_enablers_ptc_routing_enable_profile_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_l3_routing_enablers_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 routing_enablers defines");

    submodule_data->defines[dnx_data_l3_routing_enablers_define_routing_enable_vector_length].name = "routing_enable_vector_length";
    submodule_data->defines[dnx_data_l3_routing_enablers_define_routing_enable_vector_length].doc = "";
    
    submodule_data->defines[dnx_data_l3_routing_enablers_define_routing_enable_vector_length].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_routing_enablers_define_routing_enable_action_support].name = "routing_enable_action_support";
    submodule_data->defines[dnx_data_l3_routing_enablers_define_routing_enable_action_support].doc = "";
    
    submodule_data->defines[dnx_data_l3_routing_enablers_define_routing_enable_action_support].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_routing_enablers_define_routing_enable_action_profile_size].name = "routing_enable_action_profile_size";
    submodule_data->defines[dnx_data_l3_routing_enablers_define_routing_enable_action_profile_size].doc = "";
    
    submodule_data->defines[dnx_data_l3_routing_enablers_define_routing_enable_action_profile_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_routing_enablers_define_routing_enable_indication_nof_bits_per_layer].name = "routing_enable_indication_nof_bits_per_layer";
    submodule_data->defines[dnx_data_l3_routing_enablers_define_routing_enable_indication_nof_bits_per_layer].doc = "";
    
    submodule_data->defines[dnx_data_l3_routing_enablers_define_routing_enable_indication_nof_bits_per_layer].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_routing_enablers_define_routing_enable_nof_profiles_per_layer].name = "routing_enable_nof_profiles_per_layer";
    submodule_data->defines[dnx_data_l3_routing_enablers_define_routing_enable_nof_profiles_per_layer].doc = "";
    
    submodule_data->defines[dnx_data_l3_routing_enablers_define_routing_enable_nof_profiles_per_layer].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_routing_enablers_define_routing_enabled_action_profile_id].name = "routing_enabled_action_profile_id";
    submodule_data->defines[dnx_data_l3_routing_enablers_define_routing_enabled_action_profile_id].doc = "";
    
    submodule_data->defines[dnx_data_l3_routing_enablers_define_routing_enabled_action_profile_id].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l3_routing_enablers_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 routing_enablers tables");

    
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers_by_id].name = "layer_enablers_by_id";
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers_by_id].doc = "";
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers_by_id].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers_by_id].size_of_values = sizeof(dnx_data_l3_routing_enablers_layer_enablers_by_id_t);
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers_by_id].entry_get = dnx_data_l3_routing_enablers_layer_enablers_by_id_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers_by_id].nof_keys = 1;
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers_by_id].keys[0].name = "idx";
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers_by_id].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers_by_id].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers_by_id].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers_by_id].nof_values, "_dnx_data_l3_routing_enablers_table_layer_enablers_by_id table values");
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers_by_id].values[0].name = "layer_type";
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers_by_id].values[0].type = "uint32";
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers_by_id].values[0].doc = "Layer type protocol";
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers_by_id].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_routing_enablers_layer_enablers_by_id_t, layer_type);

    
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].name = "layer_enablers";
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].doc = "";
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].size_of_values = sizeof(dnx_data_l3_routing_enablers_layer_enablers_t);
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].entry_get = dnx_data_l3_routing_enablers_layer_enablers_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].nof_keys = 1;
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].keys[0].name = "layer_type";
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].nof_values, "_dnx_data_l3_routing_enablers_table_layer_enablers table values");
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].values[0].name = "uc_enable";
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].values[0].type = "uint8";
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].values[0].doc = "UC enable value.";
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_routing_enablers_layer_enablers_t, uc_enable);
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].values[1].name = "mc_enable";
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].values[1].type = "uint8";
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].values[1].doc = "MC enable value.";
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].values[1].offset = UTILEX_OFFSETOF(dnx_data_l3_routing_enablers_layer_enablers_t, mc_enable);
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].values[2].name = "disable_profile";
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].values[2].type = "uint8";
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].values[2].doc = "Profile when routing is disabled.";
    submodule_data->tables[dnx_data_l3_routing_enablers_table_layer_enablers].values[2].offset = UTILEX_OFFSETOF(dnx_data_l3_routing_enablers_layer_enablers_t, disable_profile);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_routing_enablers_feature_get(
    int unit,
    dnx_data_l3_routing_enablers_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_routing_enablers, feature);
}


uint32
dnx_data_l3_routing_enablers_routing_enable_vector_length_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_routing_enablers, dnx_data_l3_routing_enablers_define_routing_enable_vector_length);
}

uint32
dnx_data_l3_routing_enablers_routing_enable_action_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_routing_enablers, dnx_data_l3_routing_enablers_define_routing_enable_action_support);
}

uint32
dnx_data_l3_routing_enablers_routing_enable_action_profile_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_routing_enablers, dnx_data_l3_routing_enablers_define_routing_enable_action_profile_size);
}

uint32
dnx_data_l3_routing_enablers_routing_enable_indication_nof_bits_per_layer_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_routing_enablers, dnx_data_l3_routing_enablers_define_routing_enable_indication_nof_bits_per_layer);
}

uint32
dnx_data_l3_routing_enablers_routing_enable_nof_profiles_per_layer_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_routing_enablers, dnx_data_l3_routing_enablers_define_routing_enable_nof_profiles_per_layer);
}

uint32
dnx_data_l3_routing_enablers_routing_enabled_action_profile_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_routing_enablers, dnx_data_l3_routing_enablers_define_routing_enabled_action_profile_id);
}



const dnx_data_l3_routing_enablers_layer_enablers_by_id_t *
dnx_data_l3_routing_enablers_layer_enablers_by_id_get(
    int unit,
    int idx)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_routing_enablers, dnx_data_l3_routing_enablers_table_layer_enablers_by_id);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, idx, 0);
    return (const dnx_data_l3_routing_enablers_layer_enablers_by_id_t *) data;

}

const dnx_data_l3_routing_enablers_layer_enablers_t *
dnx_data_l3_routing_enablers_layer_enablers_get(
    int unit,
    int layer_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_routing_enablers, dnx_data_l3_routing_enablers_table_layer_enablers);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, layer_type, 0);
    return (const dnx_data_l3_routing_enablers_layer_enablers_t *) data;

}


shr_error_e
dnx_data_l3_routing_enablers_layer_enablers_by_id_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_routing_enablers_layer_enablers_by_id_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_routing_enablers, dnx_data_l3_routing_enablers_table_layer_enablers_by_id);
    data = (const dnx_data_l3_routing_enablers_layer_enablers_by_id_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->layer_type);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_l3_routing_enablers_layer_enablers_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_routing_enablers_layer_enablers_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_routing_enablers, dnx_data_l3_routing_enablers_table_layer_enablers);
    data = (const dnx_data_l3_routing_enablers_layer_enablers_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->uc_enable);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mc_enable);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->disable_profile);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_l3_routing_enablers_layer_enablers_by_id_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_routing_enablers, dnx_data_l3_routing_enablers_table_layer_enablers_by_id);

}

const dnxc_data_table_info_t *
dnx_data_l3_routing_enablers_layer_enablers_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_routing_enablers, dnx_data_l3_routing_enablers_table_layer_enablers);

}






static shr_error_e
dnx_data_l3_rif_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "rif";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l3_rif_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 rif features");

    submodule_data->features[dnx_data_l3_rif_out_rif_part_of_lif_mngr].name = "out_rif_part_of_lif_mngr";
    submodule_data->features[dnx_data_l3_rif_out_rif_part_of_lif_mngr].doc = "";
    submodule_data->features[dnx_data_l3_rif_out_rif_part_of_lif_mngr].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_l3_rif_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 rif defines");

    submodule_data->defines[dnx_data_l3_rif_define_max_nof_rifs].name = "max_nof_rifs";
    submodule_data->defines[dnx_data_l3_rif_define_max_nof_rifs].doc = "";
    
    submodule_data->defines[dnx_data_l3_rif_define_max_nof_rifs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_rif_define_nof_rifs].name = "nof_rifs";
    submodule_data->defines[dnx_data_l3_rif_define_nof_rifs].doc = "";
    
    submodule_data->defines[dnx_data_l3_rif_define_nof_rifs].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_l3_rif_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 rif tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_rif_feature_get(
    int unit,
    dnx_data_l3_rif_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_rif, feature);
}


uint32
dnx_data_l3_rif_max_nof_rifs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_rif, dnx_data_l3_rif_define_max_nof_rifs);
}

uint32
dnx_data_l3_rif_nof_rifs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_rif, dnx_data_l3_rif_define_nof_rifs);
}










static shr_error_e
dnx_data_l3_fwd_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "fwd";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l3_fwd_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 fwd features");

    submodule_data->features[dnx_data_l3_fwd_ipmc_rif_key_participation].name = "ipmc_rif_key_participation";
    submodule_data->features[dnx_data_l3_fwd_ipmc_rif_key_participation].doc = "";
    submodule_data->features[dnx_data_l3_fwd_ipmc_rif_key_participation].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_fwd_host_entry_support].name = "host_entry_support";
    submodule_data->features[dnx_data_l3_fwd_host_entry_support].doc = "";
    submodule_data->features[dnx_data_l3_fwd_host_entry_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_fwd_tcam_entry_support].name = "tcam_entry_support";
    submodule_data->features[dnx_data_l3_fwd_tcam_entry_support].doc = "";
    submodule_data->features[dnx_data_l3_fwd_tcam_entry_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_fwd_ipmc_config_cmprs_vrf_group].name = "ipmc_config_cmprs_vrf_group";
    submodule_data->features[dnx_data_l3_fwd_ipmc_config_cmprs_vrf_group].doc = "";
    submodule_data->features[dnx_data_l3_fwd_ipmc_config_cmprs_vrf_group].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_fwd_lpm_default_entry_set_by_prefix].name = "lpm_default_entry_set_by_prefix";
    submodule_data->features[dnx_data_l3_fwd_lpm_default_entry_set_by_prefix].doc = "";
    submodule_data->features[dnx_data_l3_fwd_lpm_default_entry_set_by_prefix].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_fwd_kaps_lpm_strength_profile_map_support].name = "kaps_lpm_strength_profile_map_support";
    submodule_data->features[dnx_data_l3_fwd_kaps_lpm_strength_profile_map_support].doc = "";
    submodule_data->features[dnx_data_l3_fwd_kaps_lpm_strength_profile_map_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_fwd_native_arp_rif_encap_access_support].name = "native_arp_rif_encap_access_support";
    submodule_data->features[dnx_data_l3_fwd_native_arp_rif_encap_access_support].doc = "";
    submodule_data->features[dnx_data_l3_fwd_native_arp_rif_encap_access_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_l3_fwd_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 fwd defines");

    submodule_data->defines[dnx_data_l3_fwd_define_flp_fragment_support].name = "flp_fragment_support";
    submodule_data->defines[dnx_data_l3_fwd_define_flp_fragment_support].doc = "";
    
    submodule_data->defines[dnx_data_l3_fwd_define_flp_fragment_support].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fwd_define_max_mc_group_lpm].name = "max_mc_group_lpm";
    submodule_data->defines[dnx_data_l3_fwd_define_max_mc_group_lpm].doc = "";
    
    submodule_data->defines[dnx_data_l3_fwd_define_max_mc_group_lpm].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fwd_define_max_mc_group_em].name = "max_mc_group_em";
    submodule_data->defines[dnx_data_l3_fwd_define_max_mc_group_em].doc = "";
    
    submodule_data->defines[dnx_data_l3_fwd_define_max_mc_group_em].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fwd_define_nof_my_mac_prefixes].name = "nof_my_mac_prefixes";
    submodule_data->defines[dnx_data_l3_fwd_define_nof_my_mac_prefixes].doc = "";
    
    submodule_data->defines[dnx_data_l3_fwd_define_nof_my_mac_prefixes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fwd_define_compressed_sip_svl_size].name = "compressed_sip_svl_size";
    submodule_data->defines[dnx_data_l3_fwd_define_compressed_sip_svl_size].doc = "";
    
    submodule_data->defines[dnx_data_l3_fwd_define_compressed_sip_svl_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fwd_define_nof_compressed_svl_sip].name = "nof_compressed_svl_sip";
    submodule_data->defines[dnx_data_l3_fwd_define_nof_compressed_svl_sip].doc = "";
    
    submodule_data->defines[dnx_data_l3_fwd_define_nof_compressed_svl_sip].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fwd_define_lpm_strength_profiles_support].name = "lpm_strength_profiles_support";
    submodule_data->defines[dnx_data_l3_fwd_define_lpm_strength_profiles_support].doc = "";
    
    submodule_data->defines[dnx_data_l3_fwd_define_lpm_strength_profiles_support].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fwd_define_nof_lpm_strength_profiles].name = "nof_lpm_strength_profiles";
    submodule_data->defines[dnx_data_l3_fwd_define_nof_lpm_strength_profiles].doc = "";
    
    submodule_data->defines[dnx_data_l3_fwd_define_nof_lpm_strength_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fwd_define_compressed_sip_ivl_size].name = "compressed_sip_ivl_size";
    submodule_data->defines[dnx_data_l3_fwd_define_compressed_sip_ivl_size].doc = "";
    
    submodule_data->defines[dnx_data_l3_fwd_define_compressed_sip_ivl_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fwd_define_nof_compressed_ivl_sip].name = "nof_compressed_ivl_sip";
    submodule_data->defines[dnx_data_l3_fwd_define_nof_compressed_ivl_sip].doc = "";
    
    submodule_data->defines[dnx_data_l3_fwd_define_nof_compressed_ivl_sip].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fwd_define_uc_supported_route_flags].name = "uc_supported_route_flags";
    submodule_data->defines[dnx_data_l3_fwd_define_uc_supported_route_flags].doc = "";
    
    submodule_data->defines[dnx_data_l3_fwd_define_uc_supported_route_flags].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l3_fwd_define_uc_supported_route_flags2].name = "uc_supported_route_flags2";
    submodule_data->defines[dnx_data_l3_fwd_define_uc_supported_route_flags2].doc = "";
    
    submodule_data->defines[dnx_data_l3_fwd_define_uc_supported_route_flags2].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l3_fwd_define_uc_supported_kbp_route_flags].name = "uc_supported_kbp_route_flags";
    submodule_data->defines[dnx_data_l3_fwd_define_uc_supported_kbp_route_flags].doc = "";
    
    submodule_data->defines[dnx_data_l3_fwd_define_uc_supported_kbp_route_flags].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l3_fwd_define_uc_supported_kbp_route_flags2].name = "uc_supported_kbp_route_flags2";
    submodule_data->defines[dnx_data_l3_fwd_define_uc_supported_kbp_route_flags2].doc = "";
    
    submodule_data->defines[dnx_data_l3_fwd_define_uc_supported_kbp_route_flags2].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l3_fwd_define_ipmc_supported_flags].name = "ipmc_supported_flags";
    submodule_data->defines[dnx_data_l3_fwd_define_ipmc_supported_flags].doc = "";
    
    submodule_data->defines[dnx_data_l3_fwd_define_ipmc_supported_flags].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l3_fwd_define_ipmc_config_supported_flags].name = "ipmc_config_supported_flags";
    submodule_data->defines[dnx_data_l3_fwd_define_ipmc_config_supported_flags].doc = "";
    
    submodule_data->defines[dnx_data_l3_fwd_define_ipmc_config_supported_flags].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l3_fwd_define_ipmc_config_supported_traverse_flags].name = "ipmc_config_supported_traverse_flags";
    submodule_data->defines[dnx_data_l3_fwd_define_ipmc_config_supported_traverse_flags].doc = "";
    
    submodule_data->defines[dnx_data_l3_fwd_define_ipmc_config_supported_traverse_flags].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l3_fwd_define_last_kaps_intf_id].name = "last_kaps_intf_id";
    submodule_data->defines[dnx_data_l3_fwd_define_last_kaps_intf_id].doc = "";
    
    submodule_data->defines[dnx_data_l3_fwd_define_last_kaps_intf_id].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_l3_fwd_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 fwd tables");

    
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].name = "lpm_profile_to_entry_strength";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].doc = "";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].size_of_values = sizeof(dnx_data_l3_fwd_lpm_profile_to_entry_strength_t);
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].entry_get = dnx_data_l3_fwd_lpm_profile_to_entry_strength_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].nof_keys = 2;
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].keys[0].name = "lpm_profile";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].keys[0].doc = "";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].keys[1].name = "kaps_intf";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].nof_values, "_dnx_data_l3_fwd_table_lpm_profile_to_entry_strength table values");
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[0].name = "prefix_len_non_def";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[0].type = "int[DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES]";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[0].doc = "Prefix length ranges for non-default entries.The value in each array cell is the highest value of the range.";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_fwd_lpm_profile_to_entry_strength_t, prefix_len_non_def);
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[1].name = "prefix_len_def";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[1].type = "int[DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES]";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[1].doc = "Prefix length ranges for non-default entries.The value in each array cell is the highest value of the range.";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[1].offset = UTILEX_OFFSETOF(dnx_data_l3_fwd_lpm_profile_to_entry_strength_t, prefix_len_def);
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[2].name = "entry_strength_non_def";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[2].type = "int[DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES]";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[2].doc = "Entry strength.";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[2].offset = UTILEX_OFFSETOF(dnx_data_l3_fwd_lpm_profile_to_entry_strength_t, entry_strength_non_def);
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[3].name = "entry_strength_def";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[3].type = "int[DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES]";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[3].doc = "Default entry strength.";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[3].offset = UTILEX_OFFSETOF(dnx_data_l3_fwd_lpm_profile_to_entry_strength_t, entry_strength_def);

    
    submodule_data->tables[dnx_data_l3_fwd_table_routing_tables].name = "routing_tables";
    submodule_data->tables[dnx_data_l3_fwd_table_routing_tables].doc = "";
    submodule_data->tables[dnx_data_l3_fwd_table_routing_tables].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_fwd_table_routing_tables].size_of_values = sizeof(dnx_data_l3_fwd_routing_tables_t);
    submodule_data->tables[dnx_data_l3_fwd_table_routing_tables].entry_get = dnx_data_l3_fwd_routing_tables_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_fwd_table_routing_tables].nof_keys = 1;
    submodule_data->tables[dnx_data_l3_fwd_table_routing_tables].keys[0].name = "ip_version";
    submodule_data->tables[dnx_data_l3_fwd_table_routing_tables].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_l3_fwd_table_routing_tables].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_fwd_table_routing_tables].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_fwd_table_routing_tables].nof_values, "_dnx_data_l3_fwd_table_routing_tables table values");
    submodule_data->tables[dnx_data_l3_fwd_table_routing_tables].values[0].name = "tables";
    submodule_data->tables[dnx_data_l3_fwd_table_routing_tables].values[0].type = "dbal_tables_e[L3_MAX_NOF_ROUTING_TABLES_PER_PROTOCOL_VERSION+1]";
    submodule_data->tables[dnx_data_l3_fwd_table_routing_tables].values[0].doc = "An array of routing tables";
    submodule_data->tables[dnx_data_l3_fwd_table_routing_tables].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_fwd_routing_tables_t, tables);

    
    submodule_data->tables[dnx_data_l3_fwd_table_mc_tables].name = "mc_tables";
    submodule_data->tables[dnx_data_l3_fwd_table_mc_tables].doc = "";
    submodule_data->tables[dnx_data_l3_fwd_table_mc_tables].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_fwd_table_mc_tables].size_of_values = sizeof(dnx_data_l3_fwd_mc_tables_t);
    submodule_data->tables[dnx_data_l3_fwd_table_mc_tables].entry_get = dnx_data_l3_fwd_mc_tables_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_fwd_table_mc_tables].nof_keys = 2;
    submodule_data->tables[dnx_data_l3_fwd_table_mc_tables].keys[0].name = "ip_version";
    submodule_data->tables[dnx_data_l3_fwd_table_mc_tables].keys[0].doc = "";
    submodule_data->tables[dnx_data_l3_fwd_table_mc_tables].keys[1].name = "route_enable";
    submodule_data->tables[dnx_data_l3_fwd_table_mc_tables].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_l3_fwd_table_mc_tables].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_fwd_table_mc_tables].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_fwd_table_mc_tables].nof_values, "_dnx_data_l3_fwd_table_mc_tables table values");
    submodule_data->tables[dnx_data_l3_fwd_table_mc_tables].values[0].name = "tables";
    submodule_data->tables[dnx_data_l3_fwd_table_mc_tables].values[0].type = "dbal_tables_e[6]";
    submodule_data->tables[dnx_data_l3_fwd_table_mc_tables].values[0].doc = "An array of tables";
    submodule_data->tables[dnx_data_l3_fwd_table_mc_tables].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_fwd_mc_tables_t, tables);
    submodule_data->tables[dnx_data_l3_fwd_table_mc_tables].values[1].name = "count";
    submodule_data->tables[dnx_data_l3_fwd_table_mc_tables].values[1].type = "uint32";
    submodule_data->tables[dnx_data_l3_fwd_table_mc_tables].values[1].doc = "The number of tables in the array";
    submodule_data->tables[dnx_data_l3_fwd_table_mc_tables].values[1].offset = UTILEX_OFFSETOF(dnx_data_l3_fwd_mc_tables_t, count);

    
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].name = "ipmc_compression_mode";
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].doc = "";
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].size_of_values = sizeof(dnx_data_l3_fwd_ipmc_compression_mode_t);
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].entry_get = dnx_data_l3_fwd_ipmc_compression_mode_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].nof_keys = 2;
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].keys[0].name = "route_enable";
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].keys[0].doc = "";
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].keys[1].name = "dbal_table_type";
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].nof_values, "_dnx_data_l3_fwd_table_ipmc_compression_mode table values");
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].values[0].name = "mode";
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].values[0].type = "dnx_ipmc_config_compression_mode_e";
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].values[0].doc = "The IPMC configuration compression mode.";
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_fwd_ipmc_compression_mode_t, mode);
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].values[1].name = "dbal_table";
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].values[1].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].values[1].doc = "The DBAL tabled used by this mode.";
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].values[1].offset = UTILEX_OFFSETOF(dnx_data_l3_fwd_ipmc_compression_mode_t, dbal_table);
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].values[2].name = "physical_table";
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].values[2].type = "dbal_physical_tables_e";
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].values[2].doc = "The physical table that is accessed.";
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].values[2].offset = UTILEX_OFFSETOF(dnx_data_l3_fwd_ipmc_compression_mode_t, physical_table);
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].values[3].name = "result_field";
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].values[3].type = "dbal_fields_e";
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].values[3].doc = "The result field in the dbal table.";
    submodule_data->tables[dnx_data_l3_fwd_table_ipmc_compression_mode].values[3].offset = UTILEX_OFFSETOF(dnx_data_l3_fwd_ipmc_compression_mode_t, result_field);

    
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_default_prefix_length_info].name = "lpm_default_prefix_length_info";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_default_prefix_length_info].doc = "";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_default_prefix_length_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_default_prefix_length_info].size_of_values = sizeof(dnx_data_l3_fwd_lpm_default_prefix_length_info_t);
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_default_prefix_length_info].entry_get = dnx_data_l3_fwd_lpm_default_prefix_length_info_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_default_prefix_length_info].nof_keys = 1;
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_default_prefix_length_info].keys[0].name = "physical_db_id";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_default_prefix_length_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_default_prefix_length_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_fwd_table_lpm_default_prefix_length_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_fwd_table_lpm_default_prefix_length_info].nof_values, "_dnx_data_l3_fwd_table_lpm_default_prefix_length_info table values");
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_default_prefix_length_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_default_prefix_length_info].values[0].type = "uint32[DNX_DATA_MAX_MDB_KAPS_NOF_APP_IDS]";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_default_prefix_length_info].values[0].doc = "The IPv4/IPv6 UC/MC routing table APP DB ID";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_default_prefix_length_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_fwd_lpm_default_prefix_length_info_t, dbal_table);
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_default_prefix_length_info].values[1].name = "default_prefix_length";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_default_prefix_length_info].values[1].type = "uint32[DNX_DATA_MAX_MDB_KAPS_NOF_APP_IDS]";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_default_prefix_length_info].values[1].doc = "The associated LPM default prefix length";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_default_prefix_length_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_l3_fwd_lpm_default_prefix_length_info_t, default_prefix_length);

    
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile].name = "lpm_app_db_to_strength_profile";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile].doc = "";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile].size_of_values = sizeof(dnx_data_l3_fwd_lpm_app_db_to_strength_profile_t);
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile].entry_get = dnx_data_l3_fwd_lpm_app_db_to_strength_profile_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile].nof_keys = 1;
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile].keys[0].name = "kaps_intf";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile].nof_values, "_dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile table values");
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile].values[0].type = "uint32[DNX_DATA_MAX_MDB_KAPS_NOF_APP_IDS]";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile].values[0].doc = "LPM APP DB ID";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_fwd_lpm_app_db_to_strength_profile_t, dbal_table);
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile].values[1].name = "lpm_profile";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile].values[1].type = "dnx_l3_kaps_lpm_strength_profiles_e[DNX_DATA_MAX_MDB_KAPS_NOF_APP_IDS]";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile].values[1].doc = "LPM strength profile";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile].values[1].offset = UTILEX_OFFSETOF(dnx_data_l3_fwd_lpm_app_db_to_strength_profile_t, lpm_profile);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_fwd_feature_get(
    int unit,
    dnx_data_l3_fwd_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, feature);
}


uint32
dnx_data_l3_fwd_flp_fragment_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_flp_fragment_support);
}

uint32
dnx_data_l3_fwd_max_mc_group_lpm_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_max_mc_group_lpm);
}

uint32
dnx_data_l3_fwd_max_mc_group_em_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_max_mc_group_em);
}

uint32
dnx_data_l3_fwd_nof_my_mac_prefixes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_nof_my_mac_prefixes);
}

uint32
dnx_data_l3_fwd_compressed_sip_svl_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_compressed_sip_svl_size);
}

uint32
dnx_data_l3_fwd_nof_compressed_svl_sip_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_nof_compressed_svl_sip);
}

uint32
dnx_data_l3_fwd_lpm_strength_profiles_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_lpm_strength_profiles_support);
}

uint32
dnx_data_l3_fwd_nof_lpm_strength_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_nof_lpm_strength_profiles);
}

uint32
dnx_data_l3_fwd_compressed_sip_ivl_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_compressed_sip_ivl_size);
}

uint32
dnx_data_l3_fwd_nof_compressed_ivl_sip_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_nof_compressed_ivl_sip);
}

uint32
dnx_data_l3_fwd_uc_supported_route_flags_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_uc_supported_route_flags);
}

uint32
dnx_data_l3_fwd_uc_supported_route_flags2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_uc_supported_route_flags2);
}

uint32
dnx_data_l3_fwd_uc_supported_kbp_route_flags_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_uc_supported_kbp_route_flags);
}

uint32
dnx_data_l3_fwd_uc_supported_kbp_route_flags2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_uc_supported_kbp_route_flags2);
}

uint32
dnx_data_l3_fwd_ipmc_supported_flags_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_ipmc_supported_flags);
}

uint32
dnx_data_l3_fwd_ipmc_config_supported_flags_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_ipmc_config_supported_flags);
}

uint32
dnx_data_l3_fwd_ipmc_config_supported_traverse_flags_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_ipmc_config_supported_traverse_flags);
}

uint32
dnx_data_l3_fwd_last_kaps_intf_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_last_kaps_intf_id);
}



const dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *
dnx_data_l3_fwd_lpm_profile_to_entry_strength_get(
    int unit,
    int lpm_profile,
    int kaps_intf)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_table_lpm_profile_to_entry_strength);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, lpm_profile, kaps_intf);
    return (const dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *) data;

}

const dnx_data_l3_fwd_routing_tables_t *
dnx_data_l3_fwd_routing_tables_get(
    int unit,
    int ip_version)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_table_routing_tables);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ip_version, 0);
    return (const dnx_data_l3_fwd_routing_tables_t *) data;

}

const dnx_data_l3_fwd_mc_tables_t *
dnx_data_l3_fwd_mc_tables_get(
    int unit,
    int ip_version,
    int route_enable)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_table_mc_tables);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ip_version, route_enable);
    return (const dnx_data_l3_fwd_mc_tables_t *) data;

}

const dnx_data_l3_fwd_ipmc_compression_mode_t *
dnx_data_l3_fwd_ipmc_compression_mode_get(
    int unit,
    int route_enable,
    int dbal_table_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_table_ipmc_compression_mode);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, route_enable, dbal_table_type);
    return (const dnx_data_l3_fwd_ipmc_compression_mode_t *) data;

}

const dnx_data_l3_fwd_lpm_default_prefix_length_info_t *
dnx_data_l3_fwd_lpm_default_prefix_length_info_get(
    int unit,
    int physical_db_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_table_lpm_default_prefix_length_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, physical_db_id, 0);
    return (const dnx_data_l3_fwd_lpm_default_prefix_length_info_t *) data;

}

const dnx_data_l3_fwd_lpm_app_db_to_strength_profile_t *
dnx_data_l3_fwd_lpm_app_db_to_strength_profile_get(
    int unit,
    int kaps_intf)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, kaps_intf, 0);
    return (const dnx_data_l3_fwd_lpm_app_db_to_strength_profile_t *) data;

}


shr_error_e
dnx_data_l3_fwd_lpm_profile_to_entry_strength_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_table_lpm_profile_to_entry_strength);
    data = (const dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, data->prefix_len_non_def);
            break;
        case 1:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, data->prefix_len_def);
            break;
        case 2:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, data->entry_strength_non_def);
            break;
        case 3:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, data->entry_strength_def);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_l3_fwd_routing_tables_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_fwd_routing_tables_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_table_routing_tables);
    data = (const dnx_data_l3_fwd_routing_tables_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, L3_MAX_NOF_ROUTING_TABLES_PER_PROTOCOL_VERSION+1, data->tables);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_l3_fwd_mc_tables_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_fwd_mc_tables_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_table_mc_tables);
    data = (const dnx_data_l3_fwd_mc_tables_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, 6, data->tables);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->count);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_l3_fwd_ipmc_compression_mode_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_fwd_ipmc_compression_mode_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_table_ipmc_compression_mode);
    data = (const dnx_data_l3_fwd_ipmc_compression_mode_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mode);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->physical_table);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->result_field);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_l3_fwd_lpm_default_prefix_length_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_fwd_lpm_default_prefix_length_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_table_lpm_default_prefix_length_info);
    data = (const dnx_data_l3_fwd_lpm_default_prefix_length_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_MDB_KAPS_NOF_APP_IDS, data->dbal_table);
            break;
        case 1:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_MDB_KAPS_NOF_APP_IDS, data->default_prefix_length);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_l3_fwd_lpm_app_db_to_strength_profile_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_fwd_lpm_app_db_to_strength_profile_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile);
    data = (const dnx_data_l3_fwd_lpm_app_db_to_strength_profile_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_MDB_KAPS_NOF_APP_IDS, data->dbal_table);
            break;
        case 1:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_MDB_KAPS_NOF_APP_IDS, data->lpm_profile);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_l3_fwd_lpm_profile_to_entry_strength_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_table_lpm_profile_to_entry_strength);

}

const dnxc_data_table_info_t *
dnx_data_l3_fwd_routing_tables_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_table_routing_tables);

}

const dnxc_data_table_info_t *
dnx_data_l3_fwd_mc_tables_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_table_mc_tables);

}

const dnxc_data_table_info_t *
dnx_data_l3_fwd_ipmc_compression_mode_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_table_ipmc_compression_mode);

}

const dnxc_data_table_info_t *
dnx_data_l3_fwd_lpm_default_prefix_length_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_table_lpm_default_prefix_length_info);

}

const dnxc_data_table_info_t *
dnx_data_l3_fwd_lpm_app_db_to_strength_profile_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_table_lpm_app_db_to_strength_profile);

}






static shr_error_e
dnx_data_l3_ecmp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ecmp";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l3_ecmp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 ecmp features");

    submodule_data->features[dnx_data_l3_ecmp_ecmp_group_profile_multiple_result_types_support].name = "ecmp_group_profile_multiple_result_types_support";
    submodule_data->features[dnx_data_l3_ecmp_ecmp_group_profile_multiple_result_types_support].doc = "";
    submodule_data->features[dnx_data_l3_ecmp_ecmp_group_profile_multiple_result_types_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_l3_ecmp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 ecmp defines");

    submodule_data->defines[dnx_data_l3_ecmp_define_group_size_multiply_and_divide_nof_bits].name = "group_size_multiply_and_divide_nof_bits";
    submodule_data->defines[dnx_data_l3_ecmp_define_group_size_multiply_and_divide_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_group_size_multiply_and_divide_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_group_size_consistent_nof_bits].name = "group_size_consistent_nof_bits";
    submodule_data->defines[dnx_data_l3_ecmp_define_group_size_consistent_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_group_size_consistent_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_group_size_nof_bits].name = "group_size_nof_bits";
    submodule_data->defines[dnx_data_l3_ecmp_define_group_size_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_group_size_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_max_group_size].name = "max_group_size";
    submodule_data->defines[dnx_data_l3_ecmp_define_max_group_size].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_max_group_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_basic_mode].name = "nof_ecmp_basic_mode";
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_basic_mode].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_basic_mode].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_total_nof_ecmp].name = "total_nof_ecmp";
    submodule_data->defines[dnx_data_l3_ecmp_define_total_nof_ecmp].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_total_nof_ecmp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_max_ecmp_basic_mode].name = "max_ecmp_basic_mode";
    submodule_data->defines[dnx_data_l3_ecmp_define_max_ecmp_basic_mode].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_max_ecmp_basic_mode].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_max_ecmp_extended_mode].name = "max_ecmp_extended_mode";
    submodule_data->defines[dnx_data_l3_ecmp_define_max_ecmp_extended_mode].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_max_ecmp_extended_mode].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_nof_extended_ecmp_per_failover_bank].name = "nof_extended_ecmp_per_failover_bank";
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_extended_ecmp_per_failover_bank].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_extended_ecmp_per_failover_bank].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_nof_failover_banks_per_extended_ecmp_bank].name = "nof_failover_banks_per_extended_ecmp_bank";
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_failover_banks_per_extended_ecmp_bank].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_failover_banks_per_extended_ecmp_bank].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_nof_group_profiles_per_hierarchy].name = "nof_group_profiles_per_hierarchy";
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_group_profiles_per_hierarchy].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_group_profiles_per_hierarchy].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_member_table_nof_rows_size_in_bits].name = "member_table_nof_rows_size_in_bits";
    submodule_data->defines[dnx_data_l3_ecmp_define_member_table_nof_rows_size_in_bits].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_member_table_nof_rows_size_in_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_member_table_address_size_in_bits].name = "member_table_address_size_in_bits";
    submodule_data->defines[dnx_data_l3_ecmp_define_member_table_address_size_in_bits].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_member_table_address_size_in_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_ecmp_bank_size].name = "ecmp_bank_size";
    submodule_data->defines[dnx_data_l3_ecmp_define_ecmp_bank_size].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_ecmp_bank_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_banks_basic].name = "nof_ecmp_banks_basic";
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_banks_basic].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_banks_basic].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_banks_extended].name = "nof_ecmp_banks_extended";
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_banks_extended].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_banks_extended].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp].name = "nof_ecmp";
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_profile_id_size].name = "profile_id_size";
    submodule_data->defines[dnx_data_l3_ecmp_define_profile_id_size].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_profile_id_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_profile_id_offset].name = "profile_id_offset";
    submodule_data->defines[dnx_data_l3_ecmp_define_profile_id_offset].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_profile_id_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_tunnel_priority_support].name = "tunnel_priority_support";
    submodule_data->defines[dnx_data_l3_ecmp_define_tunnel_priority_support].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_tunnel_priority_support].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_nof_tunnel_priority_map_profiles].name = "nof_tunnel_priority_map_profiles";
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_tunnel_priority_map_profiles].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_tunnel_priority_map_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_tunnel_priority_field_width].name = "tunnel_priority_field_width";
    submodule_data->defines[dnx_data_l3_ecmp_define_tunnel_priority_field_width].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_tunnel_priority_field_width].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_tunnel_priority_index_field_width].name = "tunnel_priority_index_field_width";
    submodule_data->defines[dnx_data_l3_ecmp_define_tunnel_priority_index_field_width].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_tunnel_priority_index_field_width].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_consistent_mem_row_size_in_bits].name = "consistent_mem_row_size_in_bits";
    submodule_data->defines[dnx_data_l3_ecmp_define_consistent_mem_row_size_in_bits].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_consistent_mem_row_size_in_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_consistent_bank_size_in_bits].name = "consistent_bank_size_in_bits";
    submodule_data->defines[dnx_data_l3_ecmp_define_consistent_bank_size_in_bits].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_consistent_bank_size_in_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_ecmp_small_consistent_group_nof_entries].name = "ecmp_small_consistent_group_nof_entries";
    submodule_data->defines[dnx_data_l3_ecmp_define_ecmp_small_consistent_group_nof_entries].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_ecmp_small_consistent_group_nof_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_ecmp_small_consistent_group_entry_size_in_bits].name = "ecmp_small_consistent_group_entry_size_in_bits";
    submodule_data->defines[dnx_data_l3_ecmp_define_ecmp_small_consistent_group_entry_size_in_bits].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_ecmp_small_consistent_group_entry_size_in_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_nof_consistent_resources].name = "nof_consistent_resources";
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_consistent_resources].doc = "";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_consistent_resources].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l3_ecmp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 ecmp tables");

    
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].name = "consistent_tables_info";
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].doc = "";
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].size_of_values = sizeof(dnx_data_l3_ecmp_consistent_tables_info_t);
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].entry_get = dnx_data_l3_ecmp_consistent_tables_info_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].nof_keys = 1;
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].keys[0].name = "table_type";
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].nof_values, "_dnx_data_l3_ecmp_table_consistent_tables_info table values");
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values[0].name = "nof_entries";
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values[0].type = "uint32";
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values[0].doc = "The NOF entries this tables holds";
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_ecmp_consistent_tables_info_t, nof_entries);
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values[1].name = "entry_size_in_bits";
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values[1].doc = "A single table entry size in bits.";
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_l3_ecmp_consistent_tables_info_t, entry_size_in_bits);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_ecmp_feature_get(
    int unit,
    dnx_data_l3_ecmp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, feature);
}


uint32
dnx_data_l3_ecmp_group_size_multiply_and_divide_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_group_size_multiply_and_divide_nof_bits);
}

uint32
dnx_data_l3_ecmp_group_size_consistent_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_group_size_consistent_nof_bits);
}

uint32
dnx_data_l3_ecmp_group_size_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_group_size_nof_bits);
}

uint32
dnx_data_l3_ecmp_max_group_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_max_group_size);
}

uint32
dnx_data_l3_ecmp_nof_ecmp_basic_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_nof_ecmp_basic_mode);
}

uint32
dnx_data_l3_ecmp_total_nof_ecmp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_total_nof_ecmp);
}

uint32
dnx_data_l3_ecmp_max_ecmp_basic_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_max_ecmp_basic_mode);
}

uint32
dnx_data_l3_ecmp_max_ecmp_extended_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_max_ecmp_extended_mode);
}

uint32
dnx_data_l3_ecmp_nof_extended_ecmp_per_failover_bank_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_nof_extended_ecmp_per_failover_bank);
}

uint32
dnx_data_l3_ecmp_nof_failover_banks_per_extended_ecmp_bank_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_nof_failover_banks_per_extended_ecmp_bank);
}

uint32
dnx_data_l3_ecmp_nof_group_profiles_per_hierarchy_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_nof_group_profiles_per_hierarchy);
}

uint32
dnx_data_l3_ecmp_member_table_nof_rows_size_in_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_member_table_nof_rows_size_in_bits);
}

uint32
dnx_data_l3_ecmp_member_table_address_size_in_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_member_table_address_size_in_bits);
}

uint32
dnx_data_l3_ecmp_ecmp_bank_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_ecmp_bank_size);
}

uint32
dnx_data_l3_ecmp_nof_ecmp_banks_basic_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_nof_ecmp_banks_basic);
}

uint32
dnx_data_l3_ecmp_nof_ecmp_banks_extended_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_nof_ecmp_banks_extended);
}

uint32
dnx_data_l3_ecmp_nof_ecmp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_nof_ecmp);
}

uint32
dnx_data_l3_ecmp_profile_id_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_profile_id_size);
}

uint32
dnx_data_l3_ecmp_profile_id_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_profile_id_offset);
}

uint32
dnx_data_l3_ecmp_tunnel_priority_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_tunnel_priority_support);
}

uint32
dnx_data_l3_ecmp_nof_tunnel_priority_map_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_nof_tunnel_priority_map_profiles);
}

uint32
dnx_data_l3_ecmp_tunnel_priority_field_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_tunnel_priority_field_width);
}

uint32
dnx_data_l3_ecmp_tunnel_priority_index_field_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_tunnel_priority_index_field_width);
}

uint32
dnx_data_l3_ecmp_consistent_mem_row_size_in_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_consistent_mem_row_size_in_bits);
}

uint32
dnx_data_l3_ecmp_consistent_bank_size_in_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_consistent_bank_size_in_bits);
}

uint32
dnx_data_l3_ecmp_ecmp_small_consistent_group_nof_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_ecmp_small_consistent_group_nof_entries);
}

uint32
dnx_data_l3_ecmp_ecmp_small_consistent_group_entry_size_in_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_ecmp_small_consistent_group_entry_size_in_bits);
}

uint32
dnx_data_l3_ecmp_nof_consistent_resources_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_nof_consistent_resources);
}



const dnx_data_l3_ecmp_consistent_tables_info_t *
dnx_data_l3_ecmp_consistent_tables_info_get(
    int unit,
    int table_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_table_consistent_tables_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, table_type, 0);
    return (const dnx_data_l3_ecmp_consistent_tables_info_t *) data;

}


shr_error_e
dnx_data_l3_ecmp_consistent_tables_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_ecmp_consistent_tables_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_table_consistent_tables_info);
    data = (const dnx_data_l3_ecmp_consistent_tables_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_entries);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->entry_size_in_bits);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_l3_ecmp_consistent_tables_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_table_consistent_tables_info);

}






static shr_error_e
dnx_data_l3_vip_ecmp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "vip_ecmp";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l3_vip_ecmp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 vip_ecmp features");

    submodule_data->features[dnx_data_l3_vip_ecmp_supported].name = "supported";
    submodule_data->features[dnx_data_l3_vip_ecmp_supported].doc = "";
    submodule_data->features[dnx_data_l3_vip_ecmp_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_l3_vip_ecmp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 vip_ecmp defines");

    submodule_data->defines[dnx_data_l3_vip_ecmp_define_nof_vip_ecmp].name = "nof_vip_ecmp";
    submodule_data->defines[dnx_data_l3_vip_ecmp_define_nof_vip_ecmp].doc = "";
    
    submodule_data->defines[dnx_data_l3_vip_ecmp_define_nof_vip_ecmp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_vip_ecmp_define_max_vip_ecmp_table_size].name = "max_vip_ecmp_table_size";
    submodule_data->defines[dnx_data_l3_vip_ecmp_define_max_vip_ecmp_table_size].doc = "";
    
    submodule_data->defines[dnx_data_l3_vip_ecmp_define_max_vip_ecmp_table_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l3_vip_ecmp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 vip_ecmp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_vip_ecmp_feature_get(
    int unit,
    dnx_data_l3_vip_ecmp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vip_ecmp, feature);
}


uint32
dnx_data_l3_vip_ecmp_nof_vip_ecmp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vip_ecmp, dnx_data_l3_vip_ecmp_define_nof_vip_ecmp);
}

uint32
dnx_data_l3_vip_ecmp_max_vip_ecmp_table_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vip_ecmp, dnx_data_l3_vip_ecmp_define_max_vip_ecmp_table_size);
}










static shr_error_e
dnx_data_l3_wcmp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "wcmp";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l3_wcmp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 wcmp features");

    
    submodule_data->nof_defines = _dnx_data_l3_wcmp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 wcmp defines");

    submodule_data->defines[dnx_data_l3_wcmp_define_group_size_nof_bits].name = "group_size_nof_bits";
    submodule_data->defines[dnx_data_l3_wcmp_define_group_size_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_l3_wcmp_define_group_size_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_wcmp_define_max_member_weight].name = "max_member_weight";
    submodule_data->defines[dnx_data_l3_wcmp_define_max_member_weight].doc = "";
    
    submodule_data->defines[dnx_data_l3_wcmp_define_max_member_weight].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_wcmp_define_member_weight_quant_factor].name = "member_weight_quant_factor";
    submodule_data->defines[dnx_data_l3_wcmp_define_member_weight_quant_factor].doc = "";
    
    submodule_data->defines[dnx_data_l3_wcmp_define_member_weight_quant_factor].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l3_wcmp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 wcmp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_wcmp_feature_get(
    int unit,
    dnx_data_l3_wcmp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_wcmp, feature);
}


uint32
dnx_data_l3_wcmp_group_size_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_wcmp, dnx_data_l3_wcmp_define_group_size_nof_bits);
}

uint32
dnx_data_l3_wcmp_max_member_weight_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_wcmp, dnx_data_l3_wcmp_define_max_member_weight);
}

uint32
dnx_data_l3_wcmp_member_weight_quant_factor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_wcmp, dnx_data_l3_wcmp_define_member_weight_quant_factor);
}










static shr_error_e
dnx_data_l3_vrrp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "vrrp";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l3_vrrp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 vrrp features");

    
    submodule_data->nof_defines = _dnx_data_l3_vrrp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 vrrp defines");

    submodule_data->defines[dnx_data_l3_vrrp_define_nof_protocol_groups].name = "nof_protocol_groups";
    submodule_data->defines[dnx_data_l3_vrrp_define_nof_protocol_groups].doc = "";
    
    submodule_data->defines[dnx_data_l3_vrrp_define_nof_protocol_groups].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_vrrp_define_nof_tcam_entries].name = "nof_tcam_entries";
    submodule_data->defines[dnx_data_l3_vrrp_define_nof_tcam_entries].doc = "";
    
    submodule_data->defines[dnx_data_l3_vrrp_define_nof_tcam_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_vrrp_define_nof_vsi_tcam_entries].name = "nof_vsi_tcam_entries";
    submodule_data->defines[dnx_data_l3_vrrp_define_nof_vsi_tcam_entries].doc = "";
    
    submodule_data->defines[dnx_data_l3_vrrp_define_nof_vsi_tcam_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_vrrp_define_nof_tcam_entries_used_by_exem].name = "nof_tcam_entries_used_by_exem";
    submodule_data->defines[dnx_data_l3_vrrp_define_nof_tcam_entries_used_by_exem].doc = "";
    
    submodule_data->defines[dnx_data_l3_vrrp_define_nof_tcam_entries_used_by_exem].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_vrrp_define_exem_vrid_ipv4_tcam_index].name = "exem_vrid_ipv4_tcam_index";
    submodule_data->defines[dnx_data_l3_vrrp_define_exem_vrid_ipv4_tcam_index].doc = "";
    
    submodule_data->defines[dnx_data_l3_vrrp_define_exem_vrid_ipv4_tcam_index].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_vrrp_define_exem_vrid_ipv6_tcam_index].name = "exem_vrid_ipv6_tcam_index";
    submodule_data->defines[dnx_data_l3_vrrp_define_exem_vrid_ipv6_tcam_index].doc = "";
    
    submodule_data->defines[dnx_data_l3_vrrp_define_exem_vrid_ipv6_tcam_index].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_vrrp_define_exem_default_tcam_index].name = "exem_default_tcam_index";
    submodule_data->defines[dnx_data_l3_vrrp_define_exem_default_tcam_index].doc = "";
    
    submodule_data->defines[dnx_data_l3_vrrp_define_exem_default_tcam_index].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_vrrp_define_exem_default_tcam_index_msb].name = "exem_default_tcam_index_msb";
    submodule_data->defines[dnx_data_l3_vrrp_define_exem_default_tcam_index_msb].doc = "";
    
    submodule_data->defines[dnx_data_l3_vrrp_define_exem_default_tcam_index_msb].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_vrrp_define_vrrp_default_num_entries].name = "vrrp_default_num_entries";
    submodule_data->defines[dnx_data_l3_vrrp_define_vrrp_default_num_entries].doc = "";
    
    submodule_data->defines[dnx_data_l3_vrrp_define_vrrp_default_num_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_vrrp_define_is_mapped_pp_port_used].name = "is_mapped_pp_port_used";
    submodule_data->defines[dnx_data_l3_vrrp_define_is_mapped_pp_port_used].doc = "";
    
    submodule_data->defines[dnx_data_l3_vrrp_define_is_mapped_pp_port_used].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_vrrp_define_nof_dbs].name = "nof_dbs";
    submodule_data->defines[dnx_data_l3_vrrp_define_nof_dbs].doc = "";
    
    submodule_data->defines[dnx_data_l3_vrrp_define_nof_dbs].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l3_vrrp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 vrrp tables");

    
    submodule_data->tables[dnx_data_l3_vrrp_table_vrrp_tables].name = "vrrp_tables";
    submodule_data->tables[dnx_data_l3_vrrp_table_vrrp_tables].doc = "";
    submodule_data->tables[dnx_data_l3_vrrp_table_vrrp_tables].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_vrrp_table_vrrp_tables].size_of_values = sizeof(dnx_data_l3_vrrp_vrrp_tables_t);
    submodule_data->tables[dnx_data_l3_vrrp_table_vrrp_tables].entry_get = dnx_data_l3_vrrp_vrrp_tables_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_vrrp_table_vrrp_tables].nof_keys = 1;
    submodule_data->tables[dnx_data_l3_vrrp_table_vrrp_tables].keys[0].name = "table_type";
    submodule_data->tables[dnx_data_l3_vrrp_table_vrrp_tables].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_l3_vrrp_table_vrrp_tables].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_vrrp_table_vrrp_tables].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_vrrp_table_vrrp_tables].nof_values, "_dnx_data_l3_vrrp_table_vrrp_tables table values");
    submodule_data->tables[dnx_data_l3_vrrp_table_vrrp_tables].values[0].name = "table";
    submodule_data->tables[dnx_data_l3_vrrp_table_vrrp_tables].values[0].type = "dbal_tables_e[2]";
    submodule_data->tables[dnx_data_l3_vrrp_table_vrrp_tables].values[0].doc = "An array of tables";
    submodule_data->tables[dnx_data_l3_vrrp_table_vrrp_tables].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_vrrp_vrrp_tables_t, table);

    
    submodule_data->tables[dnx_data_l3_vrrp_table_data_bases].name = "data_bases";
    submodule_data->tables[dnx_data_l3_vrrp_table_data_bases].doc = "";
    submodule_data->tables[dnx_data_l3_vrrp_table_data_bases].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_vrrp_table_data_bases].size_of_values = sizeof(dnx_data_l3_vrrp_data_bases_t);
    submodule_data->tables[dnx_data_l3_vrrp_table_data_bases].entry_get = dnx_data_l3_vrrp_data_bases_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_vrrp_table_data_bases].nof_keys = 0;

    
    submodule_data->tables[dnx_data_l3_vrrp_table_data_bases].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_vrrp_table_data_bases].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_vrrp_table_data_bases].nof_values, "_dnx_data_l3_vrrp_table_data_bases table values");
    submodule_data->tables[dnx_data_l3_vrrp_table_data_bases].values[0].name = "data_base";
    submodule_data->tables[dnx_data_l3_vrrp_table_data_bases].values[0].type = "dbal_physical_tables_e[DNX_DATA_MAX_L3_VRRP_NOF_DBS]";
    submodule_data->tables[dnx_data_l3_vrrp_table_data_bases].values[0].doc = "An array of physical data bases";
    submodule_data->tables[dnx_data_l3_vrrp_table_data_bases].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_vrrp_data_bases_t, data_base);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_vrrp_feature_get(
    int unit,
    dnx_data_l3_vrrp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, feature);
}


uint32
dnx_data_l3_vrrp_nof_protocol_groups_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_define_nof_protocol_groups);
}

uint32
dnx_data_l3_vrrp_nof_tcam_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_define_nof_tcam_entries);
}

uint32
dnx_data_l3_vrrp_nof_vsi_tcam_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_define_nof_vsi_tcam_entries);
}

uint32
dnx_data_l3_vrrp_nof_tcam_entries_used_by_exem_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_define_nof_tcam_entries_used_by_exem);
}

uint32
dnx_data_l3_vrrp_exem_vrid_ipv4_tcam_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_define_exem_vrid_ipv4_tcam_index);
}

uint32
dnx_data_l3_vrrp_exem_vrid_ipv6_tcam_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_define_exem_vrid_ipv6_tcam_index);
}

uint32
dnx_data_l3_vrrp_exem_default_tcam_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_define_exem_default_tcam_index);
}

uint32
dnx_data_l3_vrrp_exem_default_tcam_index_msb_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_define_exem_default_tcam_index_msb);
}

uint32
dnx_data_l3_vrrp_vrrp_default_num_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_define_vrrp_default_num_entries);
}

uint32
dnx_data_l3_vrrp_is_mapped_pp_port_used_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_define_is_mapped_pp_port_used);
}

uint32
dnx_data_l3_vrrp_nof_dbs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_define_nof_dbs);
}



const dnx_data_l3_vrrp_vrrp_tables_t *
dnx_data_l3_vrrp_vrrp_tables_get(
    int unit,
    int table_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_table_vrrp_tables);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, table_type, 0);
    return (const dnx_data_l3_vrrp_vrrp_tables_t *) data;

}

const dnx_data_l3_vrrp_data_bases_t *
dnx_data_l3_vrrp_data_bases_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_table_data_bases);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_l3_vrrp_data_bases_t *) data;

}


shr_error_e
dnx_data_l3_vrrp_vrrp_tables_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_vrrp_vrrp_tables_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_table_vrrp_tables);
    data = (const dnx_data_l3_vrrp_vrrp_tables_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, 2, data->table);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_l3_vrrp_data_bases_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_vrrp_data_bases_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_table_data_bases);
    data = (const dnx_data_l3_vrrp_data_bases_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_L3_VRRP_NOF_DBS, data->data_base);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_l3_vrrp_vrrp_tables_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_table_vrrp_tables);

}

const dnxc_data_table_info_t *
dnx_data_l3_vrrp_data_bases_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_table_data_bases);

}






static shr_error_e
dnx_data_l3_feature_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "feature";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l3_feature_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 feature features");

    submodule_data->features[dnx_data_l3_feature_ipv6_mc_compatible_dmac].name = "ipv6_mc_compatible_dmac";
    submodule_data->features[dnx_data_l3_feature_ipv6_mc_compatible_dmac].doc = "";
    submodule_data->features[dnx_data_l3_feature_ipv6_mc_compatible_dmac].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_fec_hit_bit].name = "fec_hit_bit";
    submodule_data->features[dnx_data_l3_feature_fec_hit_bit].doc = "";
    submodule_data->features[dnx_data_l3_feature_fec_hit_bit].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_fer_fec_granularity_double_size].name = "fer_fec_granularity_double_size";
    submodule_data->features[dnx_data_l3_feature_fer_fec_granularity_double_size].doc = "";
    submodule_data->features[dnx_data_l3_feature_fer_fec_granularity_double_size].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_mc_bridge_fallback].name = "mc_bridge_fallback";
    submodule_data->features[dnx_data_l3_feature_mc_bridge_fallback].doc = "";
    submodule_data->features[dnx_data_l3_feature_mc_bridge_fallback].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_nat_on_a_stick].name = "nat_on_a_stick";
    submodule_data->features[dnx_data_l3_feature_nat_on_a_stick].doc = "";
    submodule_data->features[dnx_data_l3_feature_nat_on_a_stick].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_routed_learning].name = "routed_learning";
    submodule_data->features[dnx_data_l3_feature_routed_learning].doc = "";
    submodule_data->features[dnx_data_l3_feature_routed_learning].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_ingress_frag_not_first].name = "ingress_frag_not_first";
    submodule_data->features[dnx_data_l3_feature_ingress_frag_not_first].doc = "";
    submodule_data->features[dnx_data_l3_feature_ingress_frag_not_first].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_fec_init_enable].name = "fec_init_enable";
    submodule_data->features[dnx_data_l3_feature_fec_init_enable].doc = "";
    submodule_data->features[dnx_data_l3_feature_fec_init_enable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_public_routing_support].name = "public_routing_support";
    submodule_data->features[dnx_data_l3_feature_public_routing_support].doc = "";
    submodule_data->features[dnx_data_l3_feature_public_routing_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_separate_fwd_rpf_dbs].name = "separate_fwd_rpf_dbs";
    submodule_data->features[dnx_data_l3_feature_separate_fwd_rpf_dbs].doc = "";
    submodule_data->features[dnx_data_l3_feature_separate_fwd_rpf_dbs].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_default_fec_limitation].name = "default_fec_limitation";
    submodule_data->features[dnx_data_l3_feature_default_fec_limitation].doc = "";
    submodule_data->features[dnx_data_l3_feature_default_fec_limitation].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_wcmp_support].name = "wcmp_support";
    submodule_data->features[dnx_data_l3_feature_wcmp_support].doc = "";
    submodule_data->features[dnx_data_l3_feature_wcmp_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_default_kaps_interface].name = "default_kaps_interface";
    submodule_data->features[dnx_data_l3_feature_default_kaps_interface].doc = "";
    submodule_data->features[dnx_data_l3_feature_default_kaps_interface].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_lpm_uses_lart].name = "lpm_uses_lart";
    submodule_data->features[dnx_data_l3_feature_lpm_uses_lart].doc = "";
    submodule_data->features[dnx_data_l3_feature_lpm_uses_lart].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_my_mac_prefix_0_is_invalid].name = "my_mac_prefix_0_is_invalid";
    submodule_data->features[dnx_data_l3_feature_my_mac_prefix_0_is_invalid].doc = "";
    submodule_data->features[dnx_data_l3_feature_my_mac_prefix_0_is_invalid].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_vpbr_used_for_bfd].name = "vpbr_used_for_bfd";
    submodule_data->features[dnx_data_l3_feature_vpbr_used_for_bfd].doc = "";
    submodule_data->features[dnx_data_l3_feature_vpbr_used_for_bfd].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_vpbr_tunnel_available].name = "vpbr_tunnel_available";
    submodule_data->features[dnx_data_l3_feature_vpbr_tunnel_available].doc = "";
    submodule_data->features[dnx_data_l3_feature_vpbr_tunnel_available].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_mc_bridge_fallback_outlifs_set].name = "mc_bridge_fallback_outlifs_set";
    submodule_data->features[dnx_data_l3_feature_mc_bridge_fallback_outlifs_set].doc = "";
    submodule_data->features[dnx_data_l3_feature_mc_bridge_fallback_outlifs_set].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_rpf_default_route_supported].name = "rpf_default_route_supported";
    submodule_data->features[dnx_data_l3_feature_rpf_default_route_supported].doc = "";
    submodule_data->features[dnx_data_l3_feature_rpf_default_route_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_anti_spoofing_support].name = "anti_spoofing_support";
    submodule_data->features[dnx_data_l3_feature_anti_spoofing_support].doc = "";
    submodule_data->features[dnx_data_l3_feature_anti_spoofing_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_l3_ipv6_mc_tcam_not_used].name = "l3_ipv6_mc_tcam_not_used";
    submodule_data->features[dnx_data_l3_feature_l3_ipv6_mc_tcam_not_used].doc = "";
    submodule_data->features[dnx_data_l3_feature_l3_ipv6_mc_tcam_not_used].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_l3_feature_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 feature defines");

    
    submodule_data->nof_tables = _dnx_data_l3_feature_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 feature tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_feature_feature_get(
    int unit,
    dnx_data_l3_feature_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_feature, feature);
}








shr_error_e
dnx_data_l3_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "l3";
    module_data->nof_submodules = _dnx_data_l3_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data l3 submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_l3_egr_pointed_init(unit, &module_data->submodules[dnx_data_l3_submodule_egr_pointed]));
    SHR_IF_ERR_EXIT(dnx_data_l3_fec_init(unit, &module_data->submodules[dnx_data_l3_submodule_fec]));
    SHR_IF_ERR_EXIT(dnx_data_l3_fer_init(unit, &module_data->submodules[dnx_data_l3_submodule_fer]));
    SHR_IF_ERR_EXIT(dnx_data_l3_source_address_init(unit, &module_data->submodules[dnx_data_l3_submodule_source_address]));
    SHR_IF_ERR_EXIT(dnx_data_l3_vrf_init(unit, &module_data->submodules[dnx_data_l3_submodule_vrf]));
    SHR_IF_ERR_EXIT(dnx_data_l3_routing_enablers_init(unit, &module_data->submodules[dnx_data_l3_submodule_routing_enablers]));
    SHR_IF_ERR_EXIT(dnx_data_l3_rif_init(unit, &module_data->submodules[dnx_data_l3_submodule_rif]));
    SHR_IF_ERR_EXIT(dnx_data_l3_fwd_init(unit, &module_data->submodules[dnx_data_l3_submodule_fwd]));
    SHR_IF_ERR_EXIT(dnx_data_l3_ecmp_init(unit, &module_data->submodules[dnx_data_l3_submodule_ecmp]));
    SHR_IF_ERR_EXIT(dnx_data_l3_vip_ecmp_init(unit, &module_data->submodules[dnx_data_l3_submodule_vip_ecmp]));
    SHR_IF_ERR_EXIT(dnx_data_l3_wcmp_init(unit, &module_data->submodules[dnx_data_l3_submodule_wcmp]));
    SHR_IF_ERR_EXIT(dnx_data_l3_vrrp_init(unit, &module_data->submodules[dnx_data_l3_submodule_vrrp]));
    SHR_IF_ERR_EXIT(dnx_data_l3_feature_init(unit, &module_data->submodules[dnx_data_l3_submodule_feature]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l3_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l3_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_l3_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l3_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_l3_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l3_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_l3_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l3_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_l3_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l3_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_l3_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l3_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_l3_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l3_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_l3_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l3_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_l3_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l3_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_l3_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l3_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_l3_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l3_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_l3_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

