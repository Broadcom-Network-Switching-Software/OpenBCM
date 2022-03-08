
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MDBDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_mdb.h>



#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_a0_data_mdb_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_b0_data_mdb_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_b1_data_mdb_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2c_a0_data_mdb_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2c_a1_data_mdb_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e q2a_a0_data_mdb_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e q2a_b1_data_mdb_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2p_a0_data_mdb_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2x_a0_data_mdb_attach(
    int unit);

#endif 



static shr_error_e
dnx_data_mdb_global_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "global";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_mdb_global_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data mdb global features");

    submodule_data->features[dnx_data_mdb_global_hitbit_support].name = "hitbit_support";
    submodule_data->features[dnx_data_mdb_global_hitbit_support].doc = "";
    submodule_data->features[dnx_data_mdb_global_hitbit_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_global_capacity_support].name = "capacity_support";
    submodule_data->features[dnx_data_mdb_global_capacity_support].doc = "";
    submodule_data->features[dnx_data_mdb_global_capacity_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_global_init_enable].name = "init_enable";
    submodule_data->features[dnx_data_mdb_global_init_enable].doc = "";
    submodule_data->features[dnx_data_mdb_global_init_enable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_global_block_id_fixed_mdb_value].name = "block_id_fixed_mdb_value";
    submodule_data->features[dnx_data_mdb_global_block_id_fixed_mdb_value].doc = "";
    submodule_data->features[dnx_data_mdb_global_block_id_fixed_mdb_value].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_global_lpm_acl_enable].name = "lpm_acl_enable";
    submodule_data->features[dnx_data_mdb_global_lpm_acl_enable].doc = "";
    submodule_data->features[dnx_data_mdb_global_lpm_acl_enable].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_mdb_global_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data mdb global defines");

    submodule_data->defines[dnx_data_mdb_global_define_utilization_api_nof_dbs].name = "utilization_api_nof_dbs";
    submodule_data->defines[dnx_data_mdb_global_define_utilization_api_nof_dbs].doc = "";
    
    submodule_data->defines[dnx_data_mdb_global_define_utilization_api_nof_dbs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_global_define_adapter_stub_enable].name = "adapter_stub_enable";
    submodule_data->defines[dnx_data_mdb_global_define_adapter_stub_enable].doc = "";
    
    submodule_data->defines[dnx_data_mdb_global_define_adapter_stub_enable].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_mdb_global_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data mdb global tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_mdb_global_feature_get(
    int unit,
    dnx_data_mdb_global_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_global, feature);
}


uint32
dnx_data_mdb_global_utilization_api_nof_dbs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_global, dnx_data_mdb_global_define_utilization_api_nof_dbs);
}

uint32
dnx_data_mdb_global_adapter_stub_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_global, dnx_data_mdb_global_define_adapter_stub_enable);
}










static shr_error_e
dnx_data_mdb_dh_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dh";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_mdb_dh_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data mdb dh features");

    submodule_data->features[dnx_data_mdb_dh_macro_A_half_nof_clusters_enable].name = "macro_A_half_nof_clusters_enable";
    submodule_data->features[dnx_data_mdb_dh_macro_A_half_nof_clusters_enable].doc = "";
    submodule_data->features[dnx_data_mdb_dh_macro_A_half_nof_clusters_enable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_dh_macro_A_75_nof_clusters_enable].name = "macro_A_75_nof_clusters_enable";
    submodule_data->features[dnx_data_mdb_dh_macro_A_75_nof_clusters_enable].doc = "";
    submodule_data->features[dnx_data_mdb_dh_macro_A_75_nof_clusters_enable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_dh_bpu_setup_extended_support].name = "bpu_setup_extended_support";
    submodule_data->features[dnx_data_mdb_dh_bpu_setup_extended_support].doc = "";
    submodule_data->features[dnx_data_mdb_dh_bpu_setup_extended_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_mdb_dh_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data mdb dh defines");

    submodule_data->defines[dnx_data_mdb_dh_define_total_nof_macroes].name = "total_nof_macroes";
    submodule_data->defines[dnx_data_mdb_dh_define_total_nof_macroes].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_total_nof_macroes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_total_nof_macroes_plus_em_ovf_or_eedb_bank].name = "total_nof_macroes_plus_em_ovf_or_eedb_bank";
    submodule_data->defines[dnx_data_mdb_dh_define_total_nof_macroes_plus_em_ovf_or_eedb_bank].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_total_nof_macroes_plus_em_ovf_or_eedb_bank].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_total_nof_macroes_plus_data_width].name = "total_nof_macroes_plus_data_width";
    submodule_data->defines[dnx_data_mdb_dh_define_total_nof_macroes_plus_data_width].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_total_nof_macroes_plus_data_width].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_max_nof_cluster_interfaces].name = "max_nof_cluster_interfaces";
    submodule_data->defines[dnx_data_mdb_dh_define_max_nof_cluster_interfaces].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_max_nof_cluster_interfaces].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_max_nof_clusters].name = "max_nof_clusters";
    submodule_data->defines[dnx_data_mdb_dh_define_max_nof_clusters].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_max_nof_clusters].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_max_nof_db_clusters].name = "max_nof_db_clusters";
    submodule_data->defines[dnx_data_mdb_dh_define_max_nof_db_clusters].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_max_nof_db_clusters].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_nof_bucket_clusters].name = "nof_bucket_clusters";
    submodule_data->defines[dnx_data_mdb_dh_define_nof_bucket_clusters].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_nof_bucket_clusters].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_nof_buckets_in_macro].name = "nof_buckets_in_macro";
    submodule_data->defines[dnx_data_mdb_dh_define_nof_buckets_in_macro].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_nof_buckets_in_macro].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_nof_pair_clusters].name = "nof_pair_clusters";
    submodule_data->defines[dnx_data_mdb_dh_define_nof_pair_clusters].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_nof_pair_clusters].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_nof_data_rows_per_hitbit_row].name = "nof_data_rows_per_hitbit_row";
    submodule_data->defines[dnx_data_mdb_dh_define_nof_data_rows_per_hitbit_row].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_nof_data_rows_per_hitbit_row].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_nof_ddha_blocks].name = "nof_ddha_blocks";
    submodule_data->defines[dnx_data_mdb_dh_define_nof_ddha_blocks].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_nof_ddha_blocks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_nof_ddhb_blocks].name = "nof_ddhb_blocks";
    submodule_data->defines[dnx_data_mdb_dh_define_nof_ddhb_blocks].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_nof_ddhb_blocks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_nof_dhc_blocks].name = "nof_dhc_blocks";
    submodule_data->defines[dnx_data_mdb_dh_define_nof_dhc_blocks].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_nof_dhc_blocks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_nof_ddhc_blocks].name = "nof_ddhc_blocks";
    submodule_data->defines[dnx_data_mdb_dh_define_nof_ddhc_blocks].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_nof_ddhc_blocks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_nof_ddhx_blocks].name = "nof_ddhx_blocks";
    submodule_data->defines[dnx_data_mdb_dh_define_nof_ddhx_blocks].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_nof_ddhx_blocks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_cluster_row_width_bits].name = "cluster_row_width_bits";
    submodule_data->defines[dnx_data_mdb_dh_define_cluster_row_width_bits].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_cluster_row_width_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_cluster_row_width_uint32].name = "cluster_row_width_uint32";
    submodule_data->defines[dnx_data_mdb_dh_define_cluster_row_width_uint32].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_cluster_row_width_uint32].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_data_out_granularity].name = "data_out_granularity";
    submodule_data->defines[dnx_data_mdb_dh_define_data_out_granularity].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_data_out_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_bb_connected].name = "bpu_setup_bb_connected";
    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_bb_connected].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_bb_connected].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_120_240_120].name = "bpu_setup_size_120_240_120";
    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_120_240_120].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_120_240_120].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_480_odd].name = "bpu_setup_size_480_odd";
    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_480_odd].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_480_odd].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_480].name = "bpu_setup_size_480";
    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_480].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_480].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_360_120].name = "bpu_setup_size_360_120";
    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_360_120].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_360_120].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_120_360].name = "bpu_setup_size_120_360";
    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_120_360].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_120_360].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_240_240].name = "bpu_setup_size_240_240";
    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_240_240].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_240_240].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_120_120_240].name = "bpu_setup_size_120_120_240";
    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_120_120_240].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_120_120_240].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_240_120_120].name = "bpu_setup_size_240_120_120";
    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_240_120_120].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_240_120_120].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_120_120_120_120].name = "bpu_setup_size_120_120_120_120";
    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_120_120_120_120].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_bpu_setup_size_120_120_120_120].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_ddha_dynamic_memory_access_dpc].name = "ddha_dynamic_memory_access_dpc";
    submodule_data->defines[dnx_data_mdb_dh_define_ddha_dynamic_memory_access_dpc].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_ddha_dynamic_memory_access_dpc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_dh_define_nof_fec_macro_types].name = "nof_fec_macro_types";
    submodule_data->defines[dnx_data_mdb_dh_define_nof_fec_macro_types].doc = "";
    
    submodule_data->defines[dnx_data_mdb_dh_define_nof_fec_macro_types].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_mdb_dh_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data mdb dh tables");

    
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].name = "dh_info";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].size_of_values = sizeof(dnx_data_mdb_dh_dh_info_t);
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].entry_get = dnx_data_mdb_dh_dh_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].keys[0].name = "mdb_table_id";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].nof_values = 7;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_dh_table_dh_info].nof_values, "_dnx_data_mdb_dh_table_dh_info table values");
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[0].name = "table_macro_interface_mapping";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[0].type = "uint32[DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK]";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_dh_info_t, table_macro_interface_mapping);
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[1].name = "cluster_if_offsets_values";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[1].type = "uint32[DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH]";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_dh_info_t, cluster_if_offsets_values);
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[2].name = "if_cluster_offsets_values";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[2].type = "uint32[DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH]";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_dh_info_t, if_cluster_offsets_values);
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[3].name = "macro_mapping";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[3].type = "uint32[DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH]";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_dh_info_t, macro_mapping);
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[4].name = "dh_in_width";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[4].type = "uint32";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[4].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[4].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_dh_info_t, dh_in_width);
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[5].name = "dh_out_width";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[5].type = "uint32";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[5].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[5].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_dh_info_t, dh_out_width);
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[6].name = "two_ways_connectivity_bm";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[6].type = "uint32";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[6].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[6].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_dh_info_t, two_ways_connectivity_bm);

    
    submodule_data->tables[dnx_data_mdb_dh_table_block_info].name = "block_info";
    submodule_data->tables[dnx_data_mdb_dh_table_block_info].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_block_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_dh_table_block_info].size_of_values = sizeof(dnx_data_mdb_dh_block_info_t);
    submodule_data->tables[dnx_data_mdb_dh_table_block_info].entry_get = dnx_data_mdb_dh_block_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_dh_table_block_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_dh_table_block_info].keys[0].name = "global_macro_index";
    submodule_data->tables[dnx_data_mdb_dh_table_block_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_dh_table_block_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_dh_table_block_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_dh_table_block_info].nof_values, "_dnx_data_mdb_dh_table_block_info table values");
    submodule_data->tables[dnx_data_mdb_dh_table_block_info].values[0].name = "block_type";
    submodule_data->tables[dnx_data_mdb_dh_table_block_info].values[0].type = "dbal_enum_value_field_mdb_block_types_e";
    submodule_data->tables[dnx_data_mdb_dh_table_block_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_block_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_block_info_t, block_type);
    submodule_data->tables[dnx_data_mdb_dh_table_block_info].values[1].name = "block_index";
    submodule_data->tables[dnx_data_mdb_dh_table_block_info].values[1].type = "int";
    submodule_data->tables[dnx_data_mdb_dh_table_block_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_block_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_block_info_t, block_index);

    
    submodule_data->tables[dnx_data_mdb_dh_table_logical_macro_info].name = "logical_macro_info";
    submodule_data->tables[dnx_data_mdb_dh_table_logical_macro_info].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_logical_macro_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_dh_table_logical_macro_info].size_of_values = sizeof(dnx_data_mdb_dh_logical_macro_info_t);
    submodule_data->tables[dnx_data_mdb_dh_table_logical_macro_info].entry_get = dnx_data_mdb_dh_logical_macro_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_dh_table_logical_macro_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_dh_table_logical_macro_info].keys[0].name = "global_macro_index";
    submodule_data->tables[dnx_data_mdb_dh_table_logical_macro_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_dh_table_logical_macro_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_dh_table_logical_macro_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_dh_table_logical_macro_info].nof_values, "_dnx_data_mdb_dh_table_logical_macro_info table values");
    submodule_data->tables[dnx_data_mdb_dh_table_logical_macro_info].values[0].name = "macro_type";
    submodule_data->tables[dnx_data_mdb_dh_table_logical_macro_info].values[0].type = "mdb_macro_types_e";
    submodule_data->tables[dnx_data_mdb_dh_table_logical_macro_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_logical_macro_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_logical_macro_info_t, macro_type);
    submodule_data->tables[dnx_data_mdb_dh_table_logical_macro_info].values[1].name = "macro_index";
    submodule_data->tables[dnx_data_mdb_dh_table_logical_macro_info].values[1].type = "int";
    submodule_data->tables[dnx_data_mdb_dh_table_logical_macro_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_logical_macro_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_logical_macro_info_t, macro_index);

    
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].name = "macro_type_info";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].size_of_values = sizeof(dnx_data_mdb_dh_macro_type_info_t);
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].entry_get = dnx_data_mdb_dh_macro_type_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].keys[0].name = "macro_type";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].nof_values = 8;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].nof_values, "_dnx_data_mdb_dh_table_macro_type_info table values");
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[0].name = "nof_macros";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[0].type = "uint8";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_macro_type_info_t, nof_macros);
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[1].name = "nof_clusters";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[1].type = "uint8";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_macro_type_info_t, nof_clusters);
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[2].name = "nof_rows";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_macro_type_info_t, nof_rows);
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[3].name = "nof_address_bits";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[3].type = "uint8";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_macro_type_info_t, nof_address_bits);
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[4].name = "global_start_index";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[4].type = "uint8";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[4].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[4].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_macro_type_info_t, global_start_index);
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[5].name = "xml_str";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[5].type = "char *";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[5].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[5].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_macro_type_info_t, xml_str);
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[6].name = "name";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[6].type = "char *";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[6].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[6].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_macro_type_info_t, name);
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[7].name = "hitbit_support";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[7].type = "uint8";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[7].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].values[7].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_macro_type_info_t, hitbit_support);

    
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_75_macro_halved].name = "mdb_75_macro_halved";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_75_macro_halved].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_75_macro_halved].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_75_macro_halved].size_of_values = sizeof(dnx_data_mdb_dh_mdb_75_macro_halved_t);
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_75_macro_halved].entry_get = dnx_data_mdb_dh_mdb_75_macro_halved_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_75_macro_halved].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_75_macro_halved].keys[0].name = "macro_index";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_75_macro_halved].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_75_macro_halved].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_dh_table_mdb_75_macro_halved].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_dh_table_mdb_75_macro_halved].nof_values, "_dnx_data_mdb_dh_table_mdb_75_macro_halved table values");
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_75_macro_halved].values[0].name = "macro_halved";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_75_macro_halved].values[0].type = "uint8";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_75_macro_halved].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_75_macro_halved].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_mdb_75_macro_halved_t, macro_halved);

    
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].name = "entry_banks_info";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].size_of_values = sizeof(dnx_data_mdb_dh_entry_banks_info_t);
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].entry_get = dnx_data_mdb_dh_entry_banks_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].keys[0].name = "global_macro_index";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].nof_values, "_dnx_data_mdb_dh_table_entry_banks_info table values");
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[0].name = "entry_bank";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[0].type = "uint32";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_entry_banks_info_t, entry_bank);
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[1].name = "overflow_reg";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_entry_banks_info_t, overflow_reg);
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[2].name = "abk_bank_a";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_entry_banks_info_t, abk_bank_a);
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[3].name = "abk_bank_b";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[3].type = "uint32";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_entry_banks_info_t, abk_bank_b);

    
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].name = "mdb_1_info";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].size_of_values = sizeof(dnx_data_mdb_dh_mdb_1_info_t);
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].entry_get = dnx_data_mdb_dh_mdb_1_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].keys[0].name = "global_macro_index";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].nof_values, "_dnx_data_mdb_dh_table_mdb_1_info table values");
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_mdb_1_info_t, dbal_table);
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[1].name = "mdb_item_1_field_size";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_mdb_1_info_t, mdb_item_1_field_size);
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[2].name = "mdb_item_2_array_size";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_mdb_1_info_t, mdb_item_2_array_size);
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[3].name = "mdb_item_3_field_size";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[3].type = "uint32";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_mdb_1_info_t, mdb_item_3_field_size);

    
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_2_info].name = "mdb_2_info";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_2_info].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_2_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_2_info].size_of_values = sizeof(dnx_data_mdb_dh_mdb_2_info_t);
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_2_info].entry_get = dnx_data_mdb_dh_mdb_2_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_2_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_2_info].keys[0].name = "global_macro_index";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_2_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_2_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_dh_table_mdb_2_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_dh_table_mdb_2_info].nof_values, "_dnx_data_mdb_dh_table_mdb_2_info table values");
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_2_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_2_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_2_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_2_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_mdb_2_info_t, dbal_table);
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_2_info].values[1].name = "mdb_item_1_field_size";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_2_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_2_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_2_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_mdb_2_info_t, mdb_item_1_field_size);

    
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_3_info].name = "mdb_3_info";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_3_info].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_3_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_3_info].size_of_values = sizeof(dnx_data_mdb_dh_mdb_3_info_t);
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_3_info].entry_get = dnx_data_mdb_dh_mdb_3_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_3_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_3_info].keys[0].name = "global_macro_index";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_3_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_3_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_dh_table_mdb_3_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_dh_table_mdb_3_info].nof_values, "_dnx_data_mdb_dh_table_mdb_3_info table values");
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_3_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_3_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_3_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_3_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_mdb_3_info_t, dbal_table);
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_3_info].values[1].name = "mdb_item_0_array_size";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_3_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_3_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_3_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_mdb_3_info_t, mdb_item_0_array_size);

    
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_fec_macro_types_info].name = "mdb_fec_macro_types_info";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_fec_macro_types_info].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_fec_macro_types_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_fec_macro_types_info].size_of_values = sizeof(dnx_data_mdb_dh_mdb_fec_macro_types_info_t);
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_fec_macro_types_info].entry_get = dnx_data_mdb_dh_mdb_fec_macro_types_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_fec_macro_types_info].nof_keys = 0;

    
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_fec_macro_types_info].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_dh_table_mdb_fec_macro_types_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_dh_table_mdb_fec_macro_types_info].nof_values, "_dnx_data_mdb_dh_table_mdb_fec_macro_types_info table values");
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_fec_macro_types_info].values[0].name = "macro_types";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_fec_macro_types_info].values[0].type = "mdb_macro_types_e[DNX_DATA_MAX_MDB_DH_NOF_FEC_MACRO_TYPES]";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_fec_macro_types_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_fec_macro_types_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_mdb_fec_macro_types_info_t, macro_types);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_mdb_dh_feature_get(
    int unit,
    dnx_data_mdb_dh_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, feature);
}


uint32
dnx_data_mdb_dh_total_nof_macroes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_total_nof_macroes);
}

uint32
dnx_data_mdb_dh_total_nof_macroes_plus_em_ovf_or_eedb_bank_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_total_nof_macroes_plus_em_ovf_or_eedb_bank);
}

uint32
dnx_data_mdb_dh_total_nof_macroes_plus_data_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_total_nof_macroes_plus_data_width);
}

uint32
dnx_data_mdb_dh_max_nof_cluster_interfaces_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_max_nof_cluster_interfaces);
}

uint32
dnx_data_mdb_dh_max_nof_clusters_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_max_nof_clusters);
}

uint32
dnx_data_mdb_dh_max_nof_db_clusters_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_max_nof_db_clusters);
}

uint32
dnx_data_mdb_dh_nof_bucket_clusters_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_nof_bucket_clusters);
}

uint32
dnx_data_mdb_dh_nof_buckets_in_macro_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_nof_buckets_in_macro);
}

uint32
dnx_data_mdb_dh_nof_pair_clusters_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_nof_pair_clusters);
}

uint32
dnx_data_mdb_dh_nof_data_rows_per_hitbit_row_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_nof_data_rows_per_hitbit_row);
}

uint32
dnx_data_mdb_dh_nof_ddha_blocks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_nof_ddha_blocks);
}

uint32
dnx_data_mdb_dh_nof_ddhb_blocks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_nof_ddhb_blocks);
}

uint32
dnx_data_mdb_dh_nof_dhc_blocks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_nof_dhc_blocks);
}

uint32
dnx_data_mdb_dh_nof_ddhc_blocks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_nof_ddhc_blocks);
}

uint32
dnx_data_mdb_dh_nof_ddhx_blocks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_nof_ddhx_blocks);
}

uint32
dnx_data_mdb_dh_cluster_row_width_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_cluster_row_width_bits);
}

uint32
dnx_data_mdb_dh_cluster_row_width_uint32_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_cluster_row_width_uint32);
}

uint32
dnx_data_mdb_dh_data_out_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_data_out_granularity);
}

uint32
dnx_data_mdb_dh_bpu_setup_bb_connected_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_bpu_setup_bb_connected);
}

uint32
dnx_data_mdb_dh_bpu_setup_size_120_240_120_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_bpu_setup_size_120_240_120);
}

uint32
dnx_data_mdb_dh_bpu_setup_size_480_odd_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_bpu_setup_size_480_odd);
}

uint32
dnx_data_mdb_dh_bpu_setup_size_480_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_bpu_setup_size_480);
}

uint32
dnx_data_mdb_dh_bpu_setup_size_360_120_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_bpu_setup_size_360_120);
}

uint32
dnx_data_mdb_dh_bpu_setup_size_120_360_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_bpu_setup_size_120_360);
}

uint32
dnx_data_mdb_dh_bpu_setup_size_240_240_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_bpu_setup_size_240_240);
}

uint32
dnx_data_mdb_dh_bpu_setup_size_120_120_240_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_bpu_setup_size_120_120_240);
}

uint32
dnx_data_mdb_dh_bpu_setup_size_240_120_120_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_bpu_setup_size_240_120_120);
}

uint32
dnx_data_mdb_dh_bpu_setup_size_120_120_120_120_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_bpu_setup_size_120_120_120_120);
}

uint32
dnx_data_mdb_dh_ddha_dynamic_memory_access_dpc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_ddha_dynamic_memory_access_dpc);
}

uint32
dnx_data_mdb_dh_nof_fec_macro_types_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_define_nof_fec_macro_types);
}



const dnx_data_mdb_dh_dh_info_t *
dnx_data_mdb_dh_dh_info_get(
    int unit,
    int mdb_table_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_dh_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id, 0);
    return (const dnx_data_mdb_dh_dh_info_t *) data;

}

const dnx_data_mdb_dh_block_info_t *
dnx_data_mdb_dh_block_info_get(
    int unit,
    int global_macro_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_block_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, global_macro_index, 0);
    return (const dnx_data_mdb_dh_block_info_t *) data;

}

const dnx_data_mdb_dh_logical_macro_info_t *
dnx_data_mdb_dh_logical_macro_info_get(
    int unit,
    int global_macro_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_logical_macro_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, global_macro_index, 0);
    return (const dnx_data_mdb_dh_logical_macro_info_t *) data;

}

const dnx_data_mdb_dh_macro_type_info_t *
dnx_data_mdb_dh_macro_type_info_get(
    int unit,
    int macro_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_macro_type_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, macro_type, 0);
    return (const dnx_data_mdb_dh_macro_type_info_t *) data;

}

const dnx_data_mdb_dh_mdb_75_macro_halved_t *
dnx_data_mdb_dh_mdb_75_macro_halved_get(
    int unit,
    int macro_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_mdb_75_macro_halved);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, macro_index, 0);
    return (const dnx_data_mdb_dh_mdb_75_macro_halved_t *) data;

}

const dnx_data_mdb_dh_entry_banks_info_t *
dnx_data_mdb_dh_entry_banks_info_get(
    int unit,
    int global_macro_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_entry_banks_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, global_macro_index, 0);
    return (const dnx_data_mdb_dh_entry_banks_info_t *) data;

}

const dnx_data_mdb_dh_mdb_1_info_t *
dnx_data_mdb_dh_mdb_1_info_get(
    int unit,
    int global_macro_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_mdb_1_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, global_macro_index, 0);
    return (const dnx_data_mdb_dh_mdb_1_info_t *) data;

}

const dnx_data_mdb_dh_mdb_2_info_t *
dnx_data_mdb_dh_mdb_2_info_get(
    int unit,
    int global_macro_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_mdb_2_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, global_macro_index, 0);
    return (const dnx_data_mdb_dh_mdb_2_info_t *) data;

}

const dnx_data_mdb_dh_mdb_3_info_t *
dnx_data_mdb_dh_mdb_3_info_get(
    int unit,
    int global_macro_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_mdb_3_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, global_macro_index, 0);
    return (const dnx_data_mdb_dh_mdb_3_info_t *) data;

}

const dnx_data_mdb_dh_mdb_fec_macro_types_info_t *
dnx_data_mdb_dh_mdb_fec_macro_types_info_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_mdb_fec_macro_types_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_mdb_dh_mdb_fec_macro_types_info_t *) data;

}


shr_error_e
dnx_data_mdb_dh_dh_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_dh_dh_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_dh_info);
    data = (const dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, data->table_macro_interface_mapping);
            break;
        case 1:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, data->cluster_if_offsets_values);
            break;
        case 2:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, data->if_cluster_offsets_values);
            break;
        case 3:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, data->macro_mapping);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dh_in_width);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dh_out_width);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->two_ways_connectivity_bm);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_dh_block_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_dh_block_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_block_info);
    data = (const dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->block_type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->block_index);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_dh_logical_macro_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_dh_logical_macro_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_logical_macro_info);
    data = (const dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->macro_type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->macro_index);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_dh_macro_type_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_dh_macro_type_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_macro_type_info);
    data = (const dnx_data_mdb_dh_macro_type_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_macros);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_clusters);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_rows);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_address_bits);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->global_start_index);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->xml_str == NULL ? "" : data->xml_str);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->name == NULL ? "" : data->name);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->hitbit_support);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_dh_mdb_75_macro_halved_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_dh_mdb_75_macro_halved_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_mdb_75_macro_halved);
    data = (const dnx_data_mdb_dh_mdb_75_macro_halved_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->macro_halved);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_dh_entry_banks_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_dh_entry_banks_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_entry_banks_info);
    data = (const dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->entry_bank);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->overflow_reg);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->abk_bank_a);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->abk_bank_b);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_dh_mdb_1_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_dh_mdb_1_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_mdb_1_info);
    data = (const dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_1_field_size);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_2_array_size);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_3_field_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_dh_mdb_2_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_dh_mdb_2_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_mdb_2_info);
    data = (const dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_1_field_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_dh_mdb_3_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_dh_mdb_3_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_mdb_3_info);
    data = (const dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_0_array_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_dh_mdb_fec_macro_types_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_dh_mdb_fec_macro_types_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_mdb_fec_macro_types_info);
    data = (const dnx_data_mdb_dh_mdb_fec_macro_types_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_MDB_DH_NOF_FEC_MACRO_TYPES, data->macro_types);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_mdb_dh_dh_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_dh_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_dh_block_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_block_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_dh_logical_macro_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_logical_macro_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_dh_macro_type_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_macro_type_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_dh_mdb_75_macro_halved_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_mdb_75_macro_halved);

}

const dnxc_data_table_info_t *
dnx_data_mdb_dh_entry_banks_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_entry_banks_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_dh_mdb_1_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_mdb_1_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_dh_mdb_2_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_mdb_2_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_dh_mdb_3_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_mdb_3_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_dh_mdb_fec_macro_types_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_mdb_fec_macro_types_info);

}






static shr_error_e
dnx_data_mdb_pdbs_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "pdbs";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_mdb_pdbs_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data mdb pdbs features");

    
    submodule_data->nof_defines = _dnx_data_mdb_pdbs_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data mdb pdbs defines");

    submodule_data->defines[dnx_data_mdb_pdbs_define_max_nof_interface_dhs].name = "max_nof_interface_dhs";
    submodule_data->defines[dnx_data_mdb_pdbs_define_max_nof_interface_dhs].doc = "";
    
    submodule_data->defines[dnx_data_mdb_pdbs_define_max_nof_interface_dhs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_pdbs_define_mesh_mode_support].name = "mesh_mode_support";
    submodule_data->defines[dnx_data_mdb_pdbs_define_mesh_mode_support].doc = "";
    
    submodule_data->defines[dnx_data_mdb_pdbs_define_mesh_mode_support].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_pdbs_define_max_key_size].name = "max_key_size";
    submodule_data->defines[dnx_data_mdb_pdbs_define_max_key_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_pdbs_define_max_key_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_pdbs_define_max_payload_size].name = "max_payload_size";
    submodule_data->defines[dnx_data_mdb_pdbs_define_max_payload_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_pdbs_define_max_payload_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_1_array_size].name = "table_mdb_9_mdb_item_1_array_size";
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_1_array_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_1_array_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_2_array_size].name = "table_mdb_9_mdb_item_2_array_size";
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_2_array_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_2_array_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_3_array_size].name = "table_mdb_9_mdb_item_3_array_size";
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_3_array_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_3_array_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_40_mdb_item_0_array_size].name = "table_mdb_40_mdb_item_0_array_size";
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_40_mdb_item_0_array_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_40_mdb_item_0_array_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_dynamic_memory_access_memory_access_field_size].name = "table_mdb_dynamic_memory_access_memory_access_field_size";
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_dynamic_memory_access_memory_access_field_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_dynamic_memory_access_memory_access_field_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_mact_dynamic_memory_access_memory_access_field_size].name = "table_mdb_mact_dynamic_memory_access_memory_access_field_size";
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_mact_dynamic_memory_access_memory_access_field_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_mact_dynamic_memory_access_memory_access_field_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_kaps_dynamic_memory_access_sbc_memory_access_field_size].name = "table_mdb_kaps_dynamic_memory_access_sbc_memory_access_field_size";
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_kaps_dynamic_memory_access_sbc_memory_access_field_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_kaps_dynamic_memory_access_sbc_memory_access_field_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_kaps_dynamic_memory_access_dpc_memory_access_field_size].name = "table_mdb_kaps_dynamic_memory_access_dpc_memory_access_field_size";
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_kaps_dynamic_memory_access_dpc_memory_access_field_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_kaps_dynamic_memory_access_dpc_memory_access_field_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_ddha_dynamic_memory_access_sbc_memory_access_field_size].name = "table_mdb_ddha_dynamic_memory_access_sbc_memory_access_field_size";
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_ddha_dynamic_memory_access_sbc_memory_access_field_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_ddha_dynamic_memory_access_sbc_memory_access_field_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_ddha_dynamic_memory_access_dpc_memory_access_field_size].name = "table_mdb_ddha_dynamic_memory_access_dpc_memory_access_field_size";
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_ddha_dynamic_memory_access_dpc_memory_access_field_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_ddha_dynamic_memory_access_dpc_memory_access_field_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_ddhb_dynamic_memory_access_memory_access_field_size].name = "table_mdb_ddhb_dynamic_memory_access_memory_access_field_size";
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_ddhb_dynamic_memory_access_memory_access_field_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_pdbs_define_table_mdb_ddhb_dynamic_memory_access_memory_access_field_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_pdbs_define_mdb_nof_profiles].name = "mdb_nof_profiles";
    submodule_data->defines[dnx_data_mdb_pdbs_define_mdb_nof_profiles].doc = "";
    
    submodule_data->defines[dnx_data_mdb_pdbs_define_mdb_nof_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_pdbs_define_adapter_mapping_crps_id].name = "adapter_mapping_crps_id";
    submodule_data->defines[dnx_data_mdb_pdbs_define_adapter_mapping_crps_id].doc = "";
    
    submodule_data->defines[dnx_data_mdb_pdbs_define_adapter_mapping_crps_id].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_mdb_pdbs_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data mdb pdbs tables");

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile].name = "mdb_profile";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile].size_of_values = sizeof(dnx_data_mdb_pdbs_mdb_profile_t);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile].entry_get = dnx_data_mdb_pdbs_mdb_profile_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile].nof_keys = 0;

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile].nof_values, "_dnx_data_mdb_pdbs_table_mdb_profile table values");
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile].values[0].name = "profile";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile].values[0].type = "char *";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_profile_t, profile);

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profiles_info].name = "mdb_profiles_info";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profiles_info].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profiles_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profiles_info].size_of_values = sizeof(dnx_data_mdb_pdbs_mdb_profiles_info_t);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profiles_info].entry_get = dnx_data_mdb_pdbs_mdb_profiles_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profiles_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profiles_info].keys[0].name = "mdb_profile_index";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profiles_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profiles_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profiles_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profiles_info].nof_values, "_dnx_data_mdb_pdbs_table_mdb_profiles_info table values");
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profiles_info].values[0].name = "supported";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profiles_info].values[0].type = "uint8";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profiles_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profiles_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_profiles_info_t, supported);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profiles_info].values[1].name = "name";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profiles_info].values[1].type = "char *";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profiles_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profiles_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_profiles_info_t, name);

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg].name = "mdb_profile_kaps_cfg";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg].size_of_values = sizeof(dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_t);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg].entry_get = dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg].nof_keys = 0;

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg].nof_values, "_dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg table values");
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg].values[0].name = "val";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg].values[0].type = "uint32";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_t, val);

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping].name = "mdb_profile_table_str_mapping";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping].size_of_values = sizeof(dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping].entry_get = dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping].keys[0].name = "mdb_table_id";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping].nof_values, "_dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping table values");
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping].values[0].name = "valid";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping].values[0].type = "uint8";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t, valid);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping].values[1].name = "xml_name";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping].values[1].type = "char *";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t, xml_name);

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].name = "dbal_vmv_str_mapping";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].size_of_values = sizeof(dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t);
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].entry_get = dnx_data_mdb_pdbs_dbal_vmv_str_mapping_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].keys[0].name = "dbal_table_id";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].nof_values, "_dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping table values");
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].values[0].name = "valid";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].values[0].type = "uint8";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t, valid);
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].values[1].name = "xml_name";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].values[1].type = "char *";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t, xml_name);
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].values[2].name = "vmv_xml_chain";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].values[2].type = "uint8";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t, vmv_xml_chain);

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].name = "mdb_pdb_info";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].size_of_values = sizeof(dnx_data_mdb_pdbs_mdb_pdb_info_t);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].entry_get = dnx_data_mdb_pdbs_mdb_pdb_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].keys[0].name = "mdb_table_id";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].nof_values = 8;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].nof_values, "_dnx_data_mdb_pdbs_table_mdb_pdb_info table values");
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[0].name = "physical_to_logical";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[0].type = "dbal_physical_tables_e";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_pdb_info_t, physical_to_logical);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[1].name = "db_type";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[1].type = "dbal_enum_value_field_mdb_db_type_e";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_pdb_info_t, db_type);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[2].name = "db_subtype";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[2].type = "uint8";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_pdb_info_t, db_subtype);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[3].name = "name";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[3].type = "char *";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_pdb_info_t, name);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[4].name = "row_width";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[4].type = "uint32";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[4].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[4].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_pdb_info_t, row_width);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[5].name = "macro_granularity";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[5].type = "mdb_macro_types_e";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[5].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[5].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_pdb_info_t, macro_granularity);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[6].name = "direct_payload_type";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[6].type = "dbal_enum_value_field_direct_payload_sizes_e";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[6].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[6].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_pdb_info_t, direct_payload_type);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[7].name = "direct_max_payload_type";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[7].type = "dbal_enum_value_field_direct_payload_sizes_e";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[7].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_pdb_info].values[7].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_pdb_info_t, direct_max_payload_type);

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].name = "dbal_pdb_info";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].size_of_values = sizeof(dnx_data_mdb_pdbs_dbal_pdb_info_t);
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].entry_get = dnx_data_mdb_pdbs_dbal_pdb_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].nof_values = 9;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].nof_values, "_dnx_data_mdb_pdbs_table_dbal_pdb_info table values");
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[0].name = "logical_to_physical";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[0].type = "mdb_physical_table_e";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_dbal_pdb_info_t, logical_to_physical);
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[1].name = "db_type";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[1].type = "dbal_enum_value_field_mdb_db_type_e";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_dbal_pdb_info_t, db_type);
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[2].name = "db_subtype";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[2].type = "uint8";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_dbal_pdb_info_t, db_subtype);
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[3].name = "name";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[3].type = "char *";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_dbal_pdb_info_t, name);
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[4].name = "dbal_str";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[4].type = "char *";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[4].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[4].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_dbal_pdb_info_t, dbal_str);
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[5].name = "core_mode";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[5].type = "dbal_core_mode_e";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[5].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[5].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_dbal_pdb_info_t, core_mode);
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[6].name = "row_width";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[6].type = "uint32";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[6].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[6].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_dbal_pdb_info_t, row_width);
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[7].name = "max_key_size";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[7].type = "uint32";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[7].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[7].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_dbal_pdb_info_t, max_key_size);
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[8].name = "max_payload_size";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[8].type = "uint32";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[8].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_dbal_pdb_info].values[8].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_dbal_pdb_info_t, max_payload_size);

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].name = "mdb_adapter_mapping";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].size_of_values = sizeof(dnx_data_mdb_pdbs_mdb_adapter_mapping_t);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].entry_get = dnx_data_mdb_pdbs_mdb_adapter_mapping_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].nof_values, "_dnx_data_mdb_pdbs_table_mdb_adapter_mapping table values");
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].values[0].name = "mdb_type";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].values[0].type = "uint8";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_adapter_mapping_t, mdb_type);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].values[1].name = "memory_id";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].values[1].type = "uint8";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_adapter_mapping_t, memory_id);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].values[2].name = "capacity";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].values[2].type = "int";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_adapter_mapping_t, capacity);

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info].name = "fec_physical_dbs_alloc_info";
    submodule_data->tables[dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info].size_of_values = sizeof(dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_t);
    submodule_data->tables[dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info].entry_get = dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info].keys[0].name = "fec_physical_db";
    submodule_data->tables[dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info].nof_values, "_dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info table values");
    submodule_data->tables[dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info].values[0].name = "bank_alloc_resolution";
    submodule_data->tables[dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info].values[0].type = "uint8";
    submodule_data->tables[dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_t, bank_alloc_resolution);
    submodule_data->tables[dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info].values[1].name = "base_macro_type";
    submodule_data->tables[dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info].values[1].type = "mdb_macro_types_e";
    submodule_data->tables[dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_t, base_macro_type);

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_51_info].name = "mdb_51_info";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_51_info].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_51_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_51_info].size_of_values = sizeof(dnx_data_mdb_pdbs_mdb_51_info_t);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_51_info].entry_get = dnx_data_mdb_pdbs_mdb_51_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_51_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_51_info].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_51_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_51_info].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_51_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_51_info].nof_values, "_dnx_data_mdb_pdbs_table_mdb_51_info table values");
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_51_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_51_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_51_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_51_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_51_info_t, dbal_table);

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_53_info].name = "mdb_53_info";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_53_info].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_53_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_53_info].size_of_values = sizeof(dnx_data_mdb_pdbs_mdb_53_info_t);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_53_info].entry_get = dnx_data_mdb_pdbs_mdb_53_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_53_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_53_info].keys[0].name = "mdb_table";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_53_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_53_info].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_53_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_53_info].nof_values, "_dnx_data_mdb_pdbs_table_mdb_53_info table values");
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_53_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_53_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_53_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_53_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_53_info_t, dbal_table);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_mdb_pdbs_feature_get(
    int unit,
    dnx_data_mdb_pdbs_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, feature);
}


uint32
dnx_data_mdb_pdbs_max_nof_interface_dhs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_define_max_nof_interface_dhs);
}

uint32
dnx_data_mdb_pdbs_mesh_mode_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_define_mesh_mode_support);
}

uint32
dnx_data_mdb_pdbs_max_key_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_define_max_key_size);
}

uint32
dnx_data_mdb_pdbs_max_payload_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_define_max_payload_size);
}

uint32
dnx_data_mdb_pdbs_table_mdb_9_mdb_item_1_array_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_1_array_size);
}

uint32
dnx_data_mdb_pdbs_table_mdb_9_mdb_item_2_array_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_2_array_size);
}

uint32
dnx_data_mdb_pdbs_table_mdb_9_mdb_item_3_array_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_3_array_size);
}

uint32
dnx_data_mdb_pdbs_table_mdb_40_mdb_item_0_array_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_define_table_mdb_40_mdb_item_0_array_size);
}

uint32
dnx_data_mdb_pdbs_table_mdb_dynamic_memory_access_memory_access_field_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_define_table_mdb_dynamic_memory_access_memory_access_field_size);
}

uint32
dnx_data_mdb_pdbs_table_mdb_mact_dynamic_memory_access_memory_access_field_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_define_table_mdb_mact_dynamic_memory_access_memory_access_field_size);
}

uint32
dnx_data_mdb_pdbs_table_mdb_kaps_dynamic_memory_access_sbc_memory_access_field_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_define_table_mdb_kaps_dynamic_memory_access_sbc_memory_access_field_size);
}

uint32
dnx_data_mdb_pdbs_table_mdb_kaps_dynamic_memory_access_dpc_memory_access_field_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_define_table_mdb_kaps_dynamic_memory_access_dpc_memory_access_field_size);
}

uint32
dnx_data_mdb_pdbs_table_mdb_ddha_dynamic_memory_access_sbc_memory_access_field_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_define_table_mdb_ddha_dynamic_memory_access_sbc_memory_access_field_size);
}

uint32
dnx_data_mdb_pdbs_table_mdb_ddha_dynamic_memory_access_dpc_memory_access_field_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_define_table_mdb_ddha_dynamic_memory_access_dpc_memory_access_field_size);
}

uint32
dnx_data_mdb_pdbs_table_mdb_ddhb_dynamic_memory_access_memory_access_field_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_define_table_mdb_ddhb_dynamic_memory_access_memory_access_field_size);
}

uint32
dnx_data_mdb_pdbs_mdb_nof_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_define_mdb_nof_profiles);
}

uint32
dnx_data_mdb_pdbs_adapter_mapping_crps_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_define_adapter_mapping_crps_id);
}



const dnx_data_mdb_pdbs_mdb_profile_t *
dnx_data_mdb_pdbs_mdb_profile_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_profile);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_mdb_pdbs_mdb_profile_t *) data;

}

const dnx_data_mdb_pdbs_mdb_profiles_info_t *
dnx_data_mdb_pdbs_mdb_profiles_info_get(
    int unit,
    int mdb_profile_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_profiles_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_profile_index, 0);
    return (const dnx_data_mdb_pdbs_mdb_profiles_info_t *) data;

}

const dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_t *
dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_t *) data;

}

const dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *
dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_get(
    int unit,
    int mdb_table_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id, 0);
    return (const dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) data;

}

const dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t *
dnx_data_mdb_pdbs_dbal_vmv_str_mapping_get(
    int unit,
    int dbal_table_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_table_id, 0);
    return (const dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t *) data;

}

const dnx_data_mdb_pdbs_mdb_pdb_info_t *
dnx_data_mdb_pdbs_mdb_pdb_info_get(
    int unit,
    int mdb_table_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_pdb_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id, 0);
    return (const dnx_data_mdb_pdbs_mdb_pdb_info_t *) data;

}

const dnx_data_mdb_pdbs_dbal_pdb_info_t *
dnx_data_mdb_pdbs_dbal_pdb_info_get(
    int unit,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_dbal_pdb_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_id, 0);
    return (const dnx_data_mdb_pdbs_dbal_pdb_info_t *) data;

}

const dnx_data_mdb_pdbs_mdb_adapter_mapping_t *
dnx_data_mdb_pdbs_mdb_adapter_mapping_get(
    int unit,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_adapter_mapping);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_id, 0);
    return (const dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) data;

}

const dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_t *
dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_get(
    int unit,
    int fec_physical_db)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, fec_physical_db, 0);
    return (const dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_t *) data;

}

const dnx_data_mdb_pdbs_mdb_51_info_t *
dnx_data_mdb_pdbs_mdb_51_info_get(
    int unit,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_51_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_id, 0);
    return (const dnx_data_mdb_pdbs_mdb_51_info_t *) data;

}

const dnx_data_mdb_pdbs_mdb_53_info_t *
dnx_data_mdb_pdbs_mdb_53_info_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_53_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_pdbs_mdb_53_info_t *) data;

}


shr_error_e
dnx_data_mdb_pdbs_mdb_profile_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_pdbs_mdb_profile_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_profile);
    data = (const dnx_data_mdb_pdbs_mdb_profile_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->profile == NULL ? "" : data->profile);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_pdbs_mdb_profiles_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_pdbs_mdb_profiles_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_profiles_info);
    data = (const dnx_data_mdb_pdbs_mdb_profiles_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->supported);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->name == NULL ? "" : data->name);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg);
    data = (const dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping);
    data = (const dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->xml_name == NULL ? "" : data->xml_name);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_pdbs_dbal_vmv_str_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping);
    data = (const dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->xml_name == NULL ? "" : data->xml_name);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->vmv_xml_chain);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_pdbs_mdb_pdb_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_pdbs_mdb_pdb_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_pdb_info);
    data = (const dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->physical_to_logical);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->db_type);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->db_subtype);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->name == NULL ? "" : data->name);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->row_width);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->macro_granularity);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->direct_payload_type);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->direct_max_payload_type);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_pdbs_dbal_pdb_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_pdbs_dbal_pdb_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_dbal_pdb_info);
    data = (const dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->logical_to_physical);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->db_type);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->db_subtype);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->name == NULL ? "" : data->name);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->dbal_str == NULL ? "" : data->dbal_str);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->core_mode);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->row_width);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max_key_size);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max_payload_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_pdbs_mdb_adapter_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_pdbs_mdb_adapter_mapping_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_adapter_mapping);
    data = (const dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->memory_id);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->capacity);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info);
    data = (const dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->bank_alloc_resolution);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->base_macro_type);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_pdbs_mdb_51_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_pdbs_mdb_51_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_51_info);
    data = (const dnx_data_mdb_pdbs_mdb_51_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_pdbs_mdb_53_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_pdbs_mdb_53_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_53_info);
    data = (const dnx_data_mdb_pdbs_mdb_53_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_mdb_pdbs_mdb_profile_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_profile);

}

const dnxc_data_table_info_t *
dnx_data_mdb_pdbs_mdb_profiles_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_profiles_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg);

}

const dnxc_data_table_info_t *
dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping);

}

const dnxc_data_table_info_t *
dnx_data_mdb_pdbs_dbal_vmv_str_mapping_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping);

}

const dnxc_data_table_info_t *
dnx_data_mdb_pdbs_mdb_pdb_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_pdb_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_pdbs_dbal_pdb_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_dbal_pdb_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_pdbs_mdb_adapter_mapping_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_adapter_mapping);

}

const dnxc_data_table_info_t *
dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_pdbs_mdb_51_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_51_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_pdbs_mdb_53_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_53_info);

}






static shr_error_e
dnx_data_mdb_em_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "em";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_mdb_em_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data mdb em features");

    submodule_data->features[dnx_data_mdb_em_entry_counter_decrease_support].name = "entry_counter_decrease_support";
    submodule_data->features[dnx_data_mdb_em_entry_counter_decrease_support].doc = "";
    submodule_data->features[dnx_data_mdb_em_entry_counter_decrease_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_em_entry_type_parser].name = "entry_type_parser";
    submodule_data->features[dnx_data_mdb_em_entry_type_parser].doc = "";
    submodule_data->features[dnx_data_mdb_em_entry_type_parser].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_em_step_table_any_cmds_support].name = "step_table_any_cmds_support";
    submodule_data->features[dnx_data_mdb_em_step_table_any_cmds_support].doc = "";
    submodule_data->features[dnx_data_mdb_em_step_table_any_cmds_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_em_multi_interface_blocks].name = "multi_interface_blocks";
    submodule_data->features[dnx_data_mdb_em_multi_interface_blocks].doc = "";
    submodule_data->features[dnx_data_mdb_em_multi_interface_blocks].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_em_is_isem_dpc_in_vtt5].name = "is_isem_dpc_in_vtt5";
    submodule_data->features[dnx_data_mdb_em_is_isem_dpc_in_vtt5].doc = "";
    submodule_data->features[dnx_data_mdb_em_is_isem_dpc_in_vtt5].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_mdb_em_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data mdb em defines");

    submodule_data->defines[dnx_data_mdb_em_define_age_row_size_bits].name = "age_row_size_bits";
    submodule_data->defines[dnx_data_mdb_em_define_age_row_size_bits].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_age_row_size_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_age_ovfcam_row_size_bits].name = "age_ovfcam_row_size_bits";
    submodule_data->defines[dnx_data_mdb_em_define_age_ovfcam_row_size_bits].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_age_ovfcam_row_size_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_ovf_cam_max_size].name = "ovf_cam_max_size";
    submodule_data->defines[dnx_data_mdb_em_define_ovf_cam_max_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_ovf_cam_max_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_age_support_per_entry_size_ratio].name = "age_support_per_entry_size_ratio";
    submodule_data->defines[dnx_data_mdb_em_define_age_support_per_entry_size_ratio].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_age_support_per_entry_size_ratio].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_flush_tcam_rule_counter_support].name = "flush_tcam_rule_counter_support";
    submodule_data->defines[dnx_data_mdb_em_define_flush_tcam_rule_counter_support].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_flush_tcam_rule_counter_support].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_nof_aging_profiles].name = "nof_aging_profiles";
    submodule_data->defines[dnx_data_mdb_em_define_nof_aging_profiles].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_nof_aging_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_aging_profiles_size_in_bits].name = "aging_profiles_size_in_bits";
    submodule_data->defines[dnx_data_mdb_em_define_aging_profiles_size_in_bits].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_aging_profiles_size_in_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_max_tid_size].name = "max_tid_size";
    submodule_data->defines[dnx_data_mdb_em_define_max_tid_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_max_tid_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_max_nof_tids].name = "max_nof_tids";
    submodule_data->defines[dnx_data_mdb_em_define_max_nof_tids].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_max_nof_tids].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_flush_support_tids].name = "flush_support_tids";
    submodule_data->defines[dnx_data_mdb_em_define_flush_support_tids].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_flush_support_tids].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_flush_max_supported_key].name = "flush_max_supported_key";
    submodule_data->defines[dnx_data_mdb_em_define_flush_max_supported_key].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_flush_max_supported_key].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_flush_max_supported_key_plus_payload].name = "flush_max_supported_key_plus_payload";
    submodule_data->defines[dnx_data_mdb_em_define_flush_max_supported_key_plus_payload].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_flush_max_supported_key_plus_payload].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_dh_120_entry_encoding_nof_bits].name = "dh_120_entry_encoding_nof_bits";
    submodule_data->defines[dnx_data_mdb_em_define_dh_120_entry_encoding_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_dh_120_entry_encoding_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_dh_240_entry_encoding_nof_bits].name = "dh_240_entry_encoding_nof_bits";
    submodule_data->defines[dnx_data_mdb_em_define_dh_240_entry_encoding_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_dh_240_entry_encoding_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_format_granularity].name = "format_granularity";
    submodule_data->defines[dnx_data_mdb_em_define_format_granularity].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_format_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_max_nof_vmv_size].name = "max_nof_vmv_size";
    submodule_data->defines[dnx_data_mdb_em_define_max_nof_vmv_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_max_nof_vmv_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_vmv_nof_values].name = "vmv_nof_values";
    submodule_data->defines[dnx_data_mdb_em_define_vmv_nof_values].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_vmv_nof_values].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_nof_vmv_size_nof_bits].name = "nof_vmv_size_nof_bits";
    submodule_data->defines[dnx_data_mdb_em_define_nof_vmv_size_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_nof_vmv_size_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_esem_nof_vmv_size].name = "esem_nof_vmv_size";
    submodule_data->defines[dnx_data_mdb_em_define_esem_nof_vmv_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_esem_nof_vmv_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_glem_nof_vmv_size].name = "glem_nof_vmv_size";
    submodule_data->defines[dnx_data_mdb_em_define_glem_nof_vmv_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_glem_nof_vmv_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_mact_nof_vmv_size].name = "mact_nof_vmv_size";
    submodule_data->defines[dnx_data_mdb_em_define_mact_nof_vmv_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_mact_nof_vmv_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_mact_max_payload_size].name = "mact_max_payload_size";
    submodule_data->defines[dnx_data_mdb_em_define_mact_max_payload_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_mact_max_payload_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_shift_vmv_max_size].name = "shift_vmv_max_size";
    submodule_data->defines[dnx_data_mdb_em_define_shift_vmv_max_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_shift_vmv_max_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_shift_vmv_max_regs_per_table].name = "shift_vmv_max_regs_per_table";
    submodule_data->defines[dnx_data_mdb_em_define_shift_vmv_max_regs_per_table].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_shift_vmv_max_regs_per_table].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_min_nof_app_id_bits].name = "min_nof_app_id_bits";
    submodule_data->defines[dnx_data_mdb_em_define_min_nof_app_id_bits].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_min_nof_app_id_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_max_nof_spn_sizes].name = "max_nof_spn_sizes";
    submodule_data->defines[dnx_data_mdb_em_define_max_nof_spn_sizes].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_max_nof_spn_sizes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_nof_lfsr_sizes].name = "nof_lfsr_sizes";
    submodule_data->defines[dnx_data_mdb_em_define_nof_lfsr_sizes].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_nof_lfsr_sizes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_nof_formats].name = "nof_formats";
    submodule_data->defines[dnx_data_mdb_em_define_nof_formats].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_nof_formats].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_defragmentation_priority_supported].name = "defragmentation_priority_supported";
    submodule_data->defines[dnx_data_mdb_em_define_defragmentation_priority_supported].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_defragmentation_priority_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_disable_cuckoo_loop_detection_support].name = "disable_cuckoo_loop_detection_support";
    submodule_data->defines[dnx_data_mdb_em_define_disable_cuckoo_loop_detection_support].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_disable_cuckoo_loop_detection_support].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_disable_cuckoo_hit_bit_sync].name = "disable_cuckoo_hit_bit_sync";
    submodule_data->defines[dnx_data_mdb_em_define_disable_cuckoo_hit_bit_sync].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_disable_cuckoo_hit_bit_sync].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_age_profile_per_ratio_support].name = "age_profile_per_ratio_support";
    submodule_data->defines[dnx_data_mdb_em_define_age_profile_per_ratio_support].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_age_profile_per_ratio_support].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_flex_mag_supported].name = "flex_mag_supported";
    submodule_data->defines[dnx_data_mdb_em_define_flex_mag_supported].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_flex_mag_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_flex_fully_supported].name = "flex_fully_supported";
    submodule_data->defines[dnx_data_mdb_em_define_flex_fully_supported].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_flex_fully_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_step_table_max_size].name = "step_table_max_size";
    submodule_data->defines[dnx_data_mdb_em_define_step_table_max_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_step_table_max_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_nof_encoding_values].name = "nof_encoding_values";
    submodule_data->defines[dnx_data_mdb_em_define_nof_encoding_values].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_nof_encoding_values].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_nof_encoding_types].name = "nof_encoding_types";
    submodule_data->defines[dnx_data_mdb_em_define_nof_encoding_types].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_nof_encoding_types].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_extra_vmv_shift_registers].name = "extra_vmv_shift_registers";
    submodule_data->defines[dnx_data_mdb_em_define_extra_vmv_shift_registers].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_extra_vmv_shift_registers].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_scan_bank_participate_in_cuckoo_support].name = "scan_bank_participate_in_cuckoo_support";
    submodule_data->defines[dnx_data_mdb_em_define_scan_bank_participate_in_cuckoo_support].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_scan_bank_participate_in_cuckoo_support].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_scan_bank_participate_in_cuckoo_partial_support].name = "scan_bank_participate_in_cuckoo_partial_support";
    submodule_data->defines[dnx_data_mdb_em_define_scan_bank_participate_in_cuckoo_partial_support].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_scan_bank_participate_in_cuckoo_partial_support].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_sbus_error_code_support].name = "sbus_error_code_support";
    submodule_data->defines[dnx_data_mdb_em_define_sbus_error_code_support].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_sbus_error_code_support].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_nof_emp_tables].name = "nof_emp_tables";
    submodule_data->defines[dnx_data_mdb_em_define_nof_emp_tables].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_nof_emp_tables].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_defrag_interrupt_support].name = "defrag_interrupt_support";
    submodule_data->defines[dnx_data_mdb_em_define_defrag_interrupt_support].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_defrag_interrupt_support].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_glem_bypass_option_support].name = "glem_bypass_option_support";
    submodule_data->defines[dnx_data_mdb_em_define_glem_bypass_option_support].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_glem_bypass_option_support].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_em_define_flush_max_supported_payload].name = "flush_max_supported_payload";
    submodule_data->defines[dnx_data_mdb_em_define_flush_max_supported_payload].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_flush_max_supported_payload].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_em_define_flush_payload_max_bus].name = "flush_payload_max_bus";
    submodule_data->defines[dnx_data_mdb_em_define_flush_payload_max_bus].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_flush_payload_max_bus].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_em_define_my_mac_ippa_em_dbal_id].name = "my_mac_ippa_em_dbal_id";
    submodule_data->defines[dnx_data_mdb_em_define_my_mac_ippa_em_dbal_id].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_my_mac_ippa_em_dbal_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_em_define_my_mac_ippf_em_dbal_id].name = "my_mac_ippf_em_dbal_id";
    submodule_data->defines[dnx_data_mdb_em_define_my_mac_ippf_em_dbal_id].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_my_mac_ippf_em_dbal_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_em_define_mact_dbal_id].name = "mact_dbal_id";
    submodule_data->defines[dnx_data_mdb_em_define_mact_dbal_id].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_mact_dbal_id].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_mdb_em_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data mdb em tables");

    
    submodule_data->tables[dnx_data_mdb_em_table_em_utilization_api_db_translation].name = "em_utilization_api_db_translation";
    submodule_data->tables[dnx_data_mdb_em_table_em_utilization_api_db_translation].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_utilization_api_db_translation].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_em_utilization_api_db_translation].size_of_values = sizeof(dnx_data_mdb_em_em_utilization_api_db_translation_t);
    submodule_data->tables[dnx_data_mdb_em_table_em_utilization_api_db_translation].entry_get = dnx_data_mdb_em_em_utilization_api_db_translation_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_em_utilization_api_db_translation].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_em_utilization_api_db_translation].keys[0].name = "resource_id";
    submodule_data->tables[dnx_data_mdb_em_table_em_utilization_api_db_translation].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_em_utilization_api_db_translation].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_em_utilization_api_db_translation].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_em_utilization_api_db_translation].nof_values, "_dnx_data_mdb_em_table_em_utilization_api_db_translation table values");
    submodule_data->tables[dnx_data_mdb_em_table_em_utilization_api_db_translation].values[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_em_table_em_utilization_api_db_translation].values[0].type = "uint8";
    submodule_data->tables[dnx_data_mdb_em_table_em_utilization_api_db_translation].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_utilization_api_db_translation].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_utilization_api_db_translation_t, dbal_id);

    
    submodule_data->tables[dnx_data_mdb_em_table_spn].name = "spn";
    submodule_data->tables[dnx_data_mdb_em_table_spn].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_spn].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_spn].size_of_values = sizeof(dnx_data_mdb_em_spn_t);
    submodule_data->tables[dnx_data_mdb_em_table_spn].entry_get = dnx_data_mdb_em_spn_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_spn].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_spn].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_em_table_spn].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_spn].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_spn].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_spn].nof_values, "_dnx_data_mdb_em_table_spn table values");
    submodule_data->tables[dnx_data_mdb_em_table_spn].values[0].name = "nof_spn_size";
    submodule_data->tables[dnx_data_mdb_em_table_spn].values[0].type = "uint8";
    submodule_data->tables[dnx_data_mdb_em_table_spn].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_spn].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_spn_t, nof_spn_size);
    submodule_data->tables[dnx_data_mdb_em_table_spn].values[1].name = "spn_array";
    submodule_data->tables[dnx_data_mdb_em_table_spn].values[1].type = "uint8[DNX_DATA_MAX_MDB_EM_MAX_NOF_SPN_SIZES]";
    submodule_data->tables[dnx_data_mdb_em_table_spn].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_spn].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_spn_t, spn_array);

    
    submodule_data->tables[dnx_data_mdb_em_table_lfsr].name = "lfsr";
    submodule_data->tables[dnx_data_mdb_em_table_lfsr].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_lfsr].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_lfsr].size_of_values = sizeof(dnx_data_mdb_em_lfsr_t);
    submodule_data->tables[dnx_data_mdb_em_table_lfsr].entry_get = dnx_data_mdb_em_lfsr_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_lfsr].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_lfsr].keys[0].name = "lfsr_index";
    submodule_data->tables[dnx_data_mdb_em_table_lfsr].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_lfsr].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_lfsr].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_lfsr].nof_values, "_dnx_data_mdb_em_table_lfsr table values");
    submodule_data->tables[dnx_data_mdb_em_table_lfsr].values[0].name = "lfsr_size";
    submodule_data->tables[dnx_data_mdb_em_table_lfsr].values[0].type = "int";
    submodule_data->tables[dnx_data_mdb_em_table_lfsr].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_lfsr].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_lfsr_t, lfsr_size);

    
    submodule_data->tables[dnx_data_mdb_em_table_emp_index_table_mapping].name = "emp_index_table_mapping";
    submodule_data->tables[dnx_data_mdb_em_table_emp_index_table_mapping].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_emp_index_table_mapping].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_emp_index_table_mapping].size_of_values = sizeof(dnx_data_mdb_em_emp_index_table_mapping_t);
    submodule_data->tables[dnx_data_mdb_em_table_emp_index_table_mapping].entry_get = dnx_data_mdb_em_emp_index_table_mapping_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_emp_index_table_mapping].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_emp_index_table_mapping].keys[0].name = "emp_table_idx";
    submodule_data->tables[dnx_data_mdb_em_table_emp_index_table_mapping].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_emp_index_table_mapping].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_emp_index_table_mapping].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_emp_index_table_mapping].nof_values, "_dnx_data_mdb_em_table_emp_index_table_mapping table values");
    submodule_data->tables[dnx_data_mdb_em_table_emp_index_table_mapping].values[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_em_table_emp_index_table_mapping].values[0].type = "dbal_physical_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_emp_index_table_mapping].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_emp_index_table_mapping].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_emp_index_table_mapping_t, dbal_id);

    
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].name = "em_aging_info";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].size_of_values = sizeof(dnx_data_mdb_em_em_aging_info_t);
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].entry_get = dnx_data_mdb_em_em_aging_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].nof_values, "_dnx_data_mdb_em_table_em_aging_info table values");
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[0].name = "max_nof_age_entry_bits";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[0].type = "uint8";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_aging_info_t, max_nof_age_entry_bits);
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[1].name = "rbd_size";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[1].type = "uint8";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_aging_info_t, rbd_size);
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[2].name = "nof_age_banks";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_aging_info_t, nof_age_banks);
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[3].name = "total_nof_aging_bits";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[3].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_aging_info_t, total_nof_aging_bits);

    
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].name = "em_aging_cfg";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].size_of_values = sizeof(dnx_data_mdb_em_em_aging_cfg_t);
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].entry_get = dnx_data_mdb_em_em_aging_cfg_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].nof_keys = 2;
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].keys[0].name = "profile";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].keys[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].keys[1].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].nof_values = 13;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].nof_values, "_dnx_data_mdb_em_table_em_aging_cfg table values");
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[0].name = "init_value";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[0].type = "uint8";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_aging_cfg_t, init_value);
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[1].name = "global_value";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[1].type = "uint8";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_aging_cfg_t, global_value);
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[2].name = "global_mask";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[2].type = "uint8";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_aging_cfg_t, global_mask);
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[3].name = "external_profile";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[3].type = "uint8";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_aging_cfg_t, external_profile);
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[4].name = "aging_disable";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[4].type = "uint8";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[4].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[4].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_aging_cfg_t, aging_disable);
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[5].name = "elephant_disable";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[5].type = "uint8";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[5].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[5].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_aging_cfg_t, elephant_disable);
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[6].name = "elephant_values";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[6].type = "uint8";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[6].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[6].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_aging_cfg_t, elephant_values);
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[7].name = "mouse_values";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[7].type = "uint8";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[7].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[7].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_aging_cfg_t, mouse_values);
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[8].name = "age_max_values";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[8].type = "uint8";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[8].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[8].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_aging_cfg_t, age_max_values);
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[9].name = "increment_values";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[9].type = "uint8";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[9].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[9].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_aging_cfg_t, increment_values);
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[10].name = "decrement_values";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[10].type = "uint8";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[10].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[10].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_aging_cfg_t, decrement_values);
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[11].name = "out_values";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[11].type = "uint8";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[11].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[11].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_aging_cfg_t, out_values);
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[12].name = "hit_bit_mode";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[12].type = "dbal_enum_value_field_mdb_em_aging_hit_bit_e";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[12].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].values[12].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_aging_cfg_t, hit_bit_mode);

    
    submodule_data->tables[dnx_data_mdb_em_table_em_info].name = "em_info";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_em_info].size_of_values = sizeof(dnx_data_mdb_em_em_info_t);
    submodule_data->tables[dnx_data_mdb_em_table_em_info].entry_get = dnx_data_mdb_em_em_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_em_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_em_info].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_em_info].nof_values = 6;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_em_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_em_info].nof_values, "_dnx_data_mdb_em_table_em_info table values");
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[0].name = "tid_size";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[0].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_info_t, tid_size);
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[1].name = "em_interface";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_info_t, em_interface);
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[2].name = "entry_size";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_info_t, entry_size);
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[3].name = "status_reg";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[3].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_info_t, status_reg);
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[4].name = "step_table_size";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[4].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[4].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[4].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_info_t, step_table_size);
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[5].name = "ovf_cam_size";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[5].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[5].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[5].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_info_t, ovf_cam_size);

    
    submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].name = "em_interrupt";
    submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].size_of_values = sizeof(dnx_data_mdb_em_em_interrupt_t);
    submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].entry_get = dnx_data_mdb_em_em_interrupt_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].nof_values, "_dnx_data_mdb_em_table_em_interrupt table values");
    submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].values[0].name = "instance";
    submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].values[0].type = "int";
    submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_interrupt_t, instance);
    submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].values[1].name = "interrupt_register";
    submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_interrupt_t, interrupt_register);
    submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].values[2].name = "interrupt_field";
    submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_interrupt].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_interrupt_t, interrupt_field);

    
    submodule_data->tables[dnx_data_mdb_em_table_step_table_pdb_max_depth].name = "step_table_pdb_max_depth";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_pdb_max_depth].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_pdb_max_depth].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_step_table_pdb_max_depth].size_of_values = sizeof(dnx_data_mdb_em_step_table_pdb_max_depth_t);
    submodule_data->tables[dnx_data_mdb_em_table_step_table_pdb_max_depth].entry_get = dnx_data_mdb_em_step_table_pdb_max_depth_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_step_table_pdb_max_depth].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_step_table_pdb_max_depth].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_pdb_max_depth].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_step_table_pdb_max_depth].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_step_table_pdb_max_depth].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_step_table_pdb_max_depth].nof_values, "_dnx_data_mdb_em_table_step_table_pdb_max_depth table values");
    submodule_data->tables[dnx_data_mdb_em_table_step_table_pdb_max_depth].values[0].name = "max_depth";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_pdb_max_depth].values[0].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_pdb_max_depth].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_pdb_max_depth].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_step_table_pdb_max_depth_t, max_depth);

    
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_possible].name = "step_table_max_depth_possible";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_possible].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_possible].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_possible].size_of_values = sizeof(dnx_data_mdb_em_step_table_max_depth_possible_t);
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_possible].entry_get = dnx_data_mdb_em_step_table_max_depth_possible_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_possible].nof_keys = 2;
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_possible].keys[0].name = "step_table_size_indication";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_possible].keys[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_possible].keys[1].name = "aspect_ratio_combination";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_possible].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_possible].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_possible].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_possible].nof_values, "_dnx_data_mdb_em_table_step_table_max_depth_possible table values");
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_possible].values[0].name = "max_depth";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_possible].values[0].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_possible].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_possible].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_step_table_max_depth_possible_t, max_depth);

    
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar].name = "step_table_max_depth_nof_ways_ar";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar].size_of_values = sizeof(dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t);
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar].entry_get = dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar].nof_keys = 2;
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar].keys[0].name = "aspect_ratio";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar].keys[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar].keys[1].name = "nof_ways";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar].nof_values, "_dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar table values");
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar].values[0].name = "max_depth";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar].values[0].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t, max_depth);

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].name = "mdb_15_info";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].size_of_values = sizeof(dnx_data_mdb_em_mdb_15_info_t);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].entry_get = dnx_data_mdb_em_mdb_15_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].nof_values, "_dnx_data_mdb_em_table_mdb_15_info table values");
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_15_info_t, dbal_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].values[1].name = "mdb_item_5_field_size";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_15_info_t, mdb_item_5_field_size);

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_16_info].name = "mdb_16_info";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_16_info].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_16_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_16_info].size_of_values = sizeof(dnx_data_mdb_em_mdb_16_info_t);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_16_info].entry_get = dnx_data_mdb_em_mdb_16_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_16_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_16_info].keys[0].name = "mdb_table_id";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_16_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_16_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_mdb_16_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_mdb_16_info].nof_values, "_dnx_data_mdb_em_table_mdb_16_info table values");
    submodule_data->tables[dnx_data_mdb_em_table_mdb_16_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_16_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_16_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_16_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_16_info_t, dbal_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_16_info].values[1].name = "mdb_item_0_field_size";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_16_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_16_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_16_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_16_info_t, mdb_item_0_field_size);

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].name = "mdb_21_info";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].size_of_values = sizeof(dnx_data_mdb_em_mdb_21_info_t);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].entry_get = dnx_data_mdb_em_mdb_21_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].keys[0].name = "mdb_table_id";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].nof_values, "_dnx_data_mdb_em_table_mdb_21_info table values");
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_21_info_t, dbal_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].values[1].name = "mdb_item_0_field_size";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_21_info_t, mdb_item_0_field_size);

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].name = "mdb_23_info";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].size_of_values = sizeof(dnx_data_mdb_em_mdb_23_info_t);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].entry_get = dnx_data_mdb_em_mdb_23_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].keys[0].name = "mdb_table_id";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].nof_values, "_dnx_data_mdb_em_table_mdb_23_info table values");
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_23_info_t, dbal_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].values[1].name = "mdb_item_0_field_size";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_23_info_t, mdb_item_0_field_size);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].values[2].name = "mdb_item_1_field_size";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_23_info_t, mdb_item_1_field_size);

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].name = "mdb_24_info";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].size_of_values = sizeof(dnx_data_mdb_em_mdb_24_info_t);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].entry_get = dnx_data_mdb_em_mdb_24_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].keys[0].name = "mdb_table_id";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].nof_values, "_dnx_data_mdb_em_table_mdb_24_info table values");
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_24_info_t, dbal_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].values[1].name = "mdb_item_0_array_size";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_24_info_t, mdb_item_0_array_size);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].values[2].name = "mdb_item_1_array_size";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_24_info_t, mdb_item_1_array_size);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].values[3].name = "mdb_item_1_field_size";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].values[3].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_24_info_t, mdb_item_1_field_size);

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].name = "mdb_29_info";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].size_of_values = sizeof(dnx_data_mdb_em_mdb_29_info_t);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].entry_get = dnx_data_mdb_em_mdb_29_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].keys[0].name = "mdb_table_id";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].nof_values, "_dnx_data_mdb_em_table_mdb_29_info table values");
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_29_info_t, dbal_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].values[1].name = "mdb_item_0_array_size";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_29_info_t, mdb_item_0_array_size);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].values[2].name = "mdb_item_1_array_size";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_29_info_t, mdb_item_1_array_size);

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].name = "mdb_31_info";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].size_of_values = sizeof(dnx_data_mdb_em_mdb_31_info_t);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].entry_get = dnx_data_mdb_em_mdb_31_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].nof_values, "_dnx_data_mdb_em_table_mdb_31_info table values");
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_31_info_t, dbal_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].values[1].name = "array_size";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_31_info_t, array_size);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].values[2].name = "mdb_item_0_field_size";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_31_info_t, mdb_item_0_field_size);

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_32_info].name = "mdb_32_info";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_32_info].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_32_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_32_info].size_of_values = sizeof(dnx_data_mdb_em_mdb_32_info_t);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_32_info].entry_get = dnx_data_mdb_em_mdb_32_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_32_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_32_info].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_32_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_32_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_mdb_32_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_mdb_32_info].nof_values, "_dnx_data_mdb_em_table_mdb_32_info table values");
    submodule_data->tables[dnx_data_mdb_em_table_mdb_32_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_32_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_32_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_32_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_32_info_t, dbal_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_32_info].values[1].name = "mdb_item_0_field_size";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_32_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_32_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_32_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_32_info_t, mdb_item_0_field_size);

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].name = "mdb_41_info";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].size_of_values = sizeof(dnx_data_mdb_em_mdb_41_info_t);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].entry_get = dnx_data_mdb_em_mdb_41_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].nof_values, "_dnx_data_mdb_em_table_mdb_41_info table values");
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_41_info_t, dbal_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].values[1].name = "array_size";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_41_info_t, array_size);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].values[2].name = "mdb_item_2_field_size";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_41_info_t, mdb_item_2_field_size);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].values[3].name = "mdb_item_3_field_size";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].values[3].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_41_info_t, mdb_item_3_field_size);

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].name = "mdb_45_info";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].size_of_values = sizeof(dnx_data_mdb_em_mdb_45_info_t);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].entry_get = dnx_data_mdb_em_mdb_45_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].nof_values, "_dnx_data_mdb_em_table_mdb_45_info table values");
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_45_info_t, dbal_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].values[1].name = "array_size";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_45_info_t, array_size);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].values[2].name = "mdb_item_3_field_size";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_45_info_t, mdb_item_3_field_size);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].values[3].name = "mdb_item_7_field_size";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].values[3].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_45_info_t, mdb_item_7_field_size);

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_mapping].name = "mdb_emp_tables_mapping";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_mapping].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_mapping].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_mapping].size_of_values = sizeof(dnx_data_mdb_em_mdb_emp_tables_mapping_t);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_mapping].entry_get = dnx_data_mdb_em_mdb_emp_tables_mapping_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_mapping].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_mapping].keys[0].name = "mdb_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_mapping].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_mapping].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_mapping].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_mapping].nof_values, "_dnx_data_mdb_em_table_mdb_emp_tables_mapping table values");
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_mapping].values[0].name = "emp_age_cfg_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_mapping].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_mapping].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_mapping].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_emp_tables_mapping_t, emp_age_cfg_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_mapping].values[1].name = "mdb_22_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_mapping].values[1].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_mapping].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_mapping].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_emp_tables_mapping_t, mdb_22_table);

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].name = "mdb_em_shift_vmv_regs";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].size_of_values = sizeof(dnx_data_mdb_em_mdb_em_shift_vmv_regs_t);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].entry_get = dnx_data_mdb_em_mdb_em_shift_vmv_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].nof_values = 6;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].nof_values, "_dnx_data_mdb_em_table_mdb_em_shift_vmv_regs table values");
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[0].name = "nof_valid_regs";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[0].type = "uint8";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_em_shift_vmv_regs_t, nof_valid_regs);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[1].name = "table_name_arr";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[1].type = "dbal_tables_e[DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE]";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_em_shift_vmv_regs_t, table_name_arr);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[2].name = "field_name_arr";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[2].type = "dbal_fields_e[DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE]";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_em_shift_vmv_regs_t, field_name_arr);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[3].name = "key_name_arr";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[3].type = "dbal_fields_e[DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE]";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_em_shift_vmv_regs_t, key_name_arr);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[4].name = "stage_index_arr";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[4].type = "uint8[DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE]";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[4].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[4].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_em_shift_vmv_regs_t, stage_index_arr);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[5].name = "esem_cmd_indication";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[5].type = "uint8[DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE]";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[5].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_shift_vmv_regs].values[5].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_em_shift_vmv_regs_t, esem_cmd_indication);

    
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].name = "em_emp";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].size_of_values = sizeof(dnx_data_mdb_em_em_emp_t);
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].entry_get = dnx_data_mdb_em_em_emp_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].nof_values = 10;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_em_emp].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_em_emp].nof_values, "_dnx_data_mdb_em_table_em_emp table values");
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[0].name = "age_mem";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[0].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_emp_t, age_mem);
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[1].name = "age_ovf_cam_mem";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_emp_t, age_ovf_cam_mem);
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[2].name = "emp_scan_status_field";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_emp_t, emp_scan_status_field);
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[3].name = "emp_age_cfg_reg";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[3].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_emp_t, emp_age_cfg_reg);
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[4].name = "global_value";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[4].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[4].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[4].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_emp_t, global_value);
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[5].name = "global_masks";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[5].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[5].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[5].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_emp_t, global_masks);
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[6].name = "external_profile";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[6].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[6].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[6].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_emp_t, external_profile);
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[7].name = "age_disable";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[7].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[7].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[7].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_emp_t, age_disable);
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[8].name = "elephant_disable";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[8].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[8].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[8].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_emp_t, elephant_disable);
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[9].name = "initial_values";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[9].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[9].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_emp].values[9].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_emp_t, initial_values);

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_step_table_map].name = "mdb_step_table_map";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_step_table_map].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_step_table_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_step_table_map].size_of_values = sizeof(dnx_data_mdb_em_mdb_step_table_map_t);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_step_table_map].entry_get = dnx_data_mdb_em_mdb_step_table_map_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_step_table_map].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_step_table_map].keys[0].name = "mdb_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_step_table_map].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_step_table_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_mdb_step_table_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_mdb_step_table_map].nof_values, "_dnx_data_mdb_em_table_mdb_step_table_map table values");
    submodule_data->tables[dnx_data_mdb_em_table_mdb_step_table_map].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_step_table_map].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_step_table_map].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_step_table_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_step_table_map_t, dbal_table);

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].name = "mdb_em_dbal_phy_tables_info";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].size_of_values = sizeof(dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].entry_get = dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].nof_values = 8;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].nof_values, "_dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info table values");
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[0].name = "mdb_13_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t, mdb_13_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[1].name = "mdb_14_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[1].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t, mdb_14_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[2].name = "mdb_17_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[2].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t, mdb_17_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[3].name = "mdb_18_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[3].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t, mdb_18_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[4].name = "mdb_33_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[4].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[4].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[4].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t, mdb_33_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[5].name = "mdb_34_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[5].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[5].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[5].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t, mdb_34_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[6].name = "mdb_46_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[6].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[6].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[6].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t, mdb_46_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[7].name = "mdb_52_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[7].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[7].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info].values[7].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t, mdb_52_table);

    
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].name = "filter_rules";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].size_of_values = sizeof(dnx_data_mdb_em_filter_rules_t);
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].entry_get = dnx_data_mdb_em_filter_rules_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].nof_values = 10;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_filter_rules].nof_values, "_dnx_data_mdb_em_table_filter_rules table values");
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[0].name = "table_name";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[0].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_filter_rules_t, table_name);
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[1].name = "valid";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_filter_rules_t, valid);
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[2].name = "entry_mask";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_filter_rules_t, entry_mask);
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[3].name = "entry_filter";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[3].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_filter_rules_t, entry_filter);
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[4].name = "src_mask";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[4].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[4].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[4].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_filter_rules_t, src_mask);
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[5].name = "src_filter";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[5].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[5].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[5].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_filter_rules_t, src_filter);
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[6].name = "appdb_id_mask";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[6].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[6].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[6].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_filter_rules_t, appdb_id_mask);
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[7].name = "appdb_id_filter";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[7].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[7].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[7].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_filter_rules_t, appdb_id_filter);
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[8].name = "accessed_mask";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[8].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[8].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[8].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_filter_rules_t, accessed_mask);
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[9].name = "accessed_filter";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[9].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[9].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_filter_rules].values[9].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_filter_rules_t, accessed_filter);

    
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].name = "data_rules";
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].size_of_values = sizeof(dnx_data_mdb_em_data_rules_t);
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].entry_get = dnx_data_mdb_em_data_rules_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_data_rules].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_data_rules].nof_values, "_dnx_data_mdb_em_table_data_rules table values");
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].values[0].name = "table_name";
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].values[0].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_data_rules_t, table_name);
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].values[1].name = "command";
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_data_rules_t, command);
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].values[2].name = "payload_mask";
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_data_rules_t, payload_mask);
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].values[3].name = "payload";
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].values[3].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_data_rules].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_data_rules_t, payload);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_mdb_em_feature_get(
    int unit,
    dnx_data_mdb_em_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, feature);
}


uint32
dnx_data_mdb_em_age_row_size_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_age_row_size_bits);
}

uint32
dnx_data_mdb_em_age_ovfcam_row_size_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_age_ovfcam_row_size_bits);
}

uint32
dnx_data_mdb_em_ovf_cam_max_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_ovf_cam_max_size);
}

uint32
dnx_data_mdb_em_age_support_per_entry_size_ratio_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_age_support_per_entry_size_ratio);
}

uint32
dnx_data_mdb_em_flush_tcam_rule_counter_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_flush_tcam_rule_counter_support);
}

uint32
dnx_data_mdb_em_nof_aging_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_nof_aging_profiles);
}

uint32
dnx_data_mdb_em_aging_profiles_size_in_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_aging_profiles_size_in_bits);
}

uint32
dnx_data_mdb_em_max_tid_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_max_tid_size);
}

uint32
dnx_data_mdb_em_max_nof_tids_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_max_nof_tids);
}

uint32
dnx_data_mdb_em_flush_support_tids_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_flush_support_tids);
}

uint32
dnx_data_mdb_em_flush_max_supported_key_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_flush_max_supported_key);
}

uint32
dnx_data_mdb_em_flush_max_supported_key_plus_payload_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_flush_max_supported_key_plus_payload);
}

uint32
dnx_data_mdb_em_dh_120_entry_encoding_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_dh_120_entry_encoding_nof_bits);
}

uint32
dnx_data_mdb_em_dh_240_entry_encoding_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_dh_240_entry_encoding_nof_bits);
}

uint32
dnx_data_mdb_em_format_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_format_granularity);
}

uint32
dnx_data_mdb_em_max_nof_vmv_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_max_nof_vmv_size);
}

uint32
dnx_data_mdb_em_vmv_nof_values_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_vmv_nof_values);
}

uint32
dnx_data_mdb_em_nof_vmv_size_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_nof_vmv_size_nof_bits);
}

uint32
dnx_data_mdb_em_esem_nof_vmv_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_esem_nof_vmv_size);
}

uint32
dnx_data_mdb_em_glem_nof_vmv_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_glem_nof_vmv_size);
}

uint32
dnx_data_mdb_em_mact_nof_vmv_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_mact_nof_vmv_size);
}

uint32
dnx_data_mdb_em_mact_max_payload_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_mact_max_payload_size);
}

uint32
dnx_data_mdb_em_shift_vmv_max_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_shift_vmv_max_size);
}

uint32
dnx_data_mdb_em_shift_vmv_max_regs_per_table_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_shift_vmv_max_regs_per_table);
}

uint32
dnx_data_mdb_em_min_nof_app_id_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_min_nof_app_id_bits);
}

uint32
dnx_data_mdb_em_max_nof_spn_sizes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_max_nof_spn_sizes);
}

uint32
dnx_data_mdb_em_nof_lfsr_sizes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_nof_lfsr_sizes);
}

uint32
dnx_data_mdb_em_nof_formats_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_nof_formats);
}

uint32
dnx_data_mdb_em_defragmentation_priority_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_defragmentation_priority_supported);
}

uint32
dnx_data_mdb_em_disable_cuckoo_loop_detection_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_disable_cuckoo_loop_detection_support);
}

uint32
dnx_data_mdb_em_disable_cuckoo_hit_bit_sync_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_disable_cuckoo_hit_bit_sync);
}

uint32
dnx_data_mdb_em_age_profile_per_ratio_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_age_profile_per_ratio_support);
}

uint32
dnx_data_mdb_em_flex_mag_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_flex_mag_supported);
}

uint32
dnx_data_mdb_em_flex_fully_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_flex_fully_supported);
}

uint32
dnx_data_mdb_em_step_table_max_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_step_table_max_size);
}

uint32
dnx_data_mdb_em_nof_encoding_values_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_nof_encoding_values);
}

uint32
dnx_data_mdb_em_nof_encoding_types_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_nof_encoding_types);
}

uint32
dnx_data_mdb_em_extra_vmv_shift_registers_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_extra_vmv_shift_registers);
}

uint32
dnx_data_mdb_em_scan_bank_participate_in_cuckoo_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_scan_bank_participate_in_cuckoo_support);
}

uint32
dnx_data_mdb_em_scan_bank_participate_in_cuckoo_partial_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_scan_bank_participate_in_cuckoo_partial_support);
}

uint32
dnx_data_mdb_em_sbus_error_code_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_sbus_error_code_support);
}

uint32
dnx_data_mdb_em_nof_emp_tables_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_nof_emp_tables);
}

uint32
dnx_data_mdb_em_defrag_interrupt_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_defrag_interrupt_support);
}

uint32
dnx_data_mdb_em_glem_bypass_option_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_glem_bypass_option_support);
}

uint32
dnx_data_mdb_em_flush_max_supported_payload_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_flush_max_supported_payload);
}

uint32
dnx_data_mdb_em_flush_payload_max_bus_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_flush_payload_max_bus);
}

uint32
dnx_data_mdb_em_my_mac_ippa_em_dbal_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_my_mac_ippa_em_dbal_id);
}

uint32
dnx_data_mdb_em_my_mac_ippf_em_dbal_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_my_mac_ippf_em_dbal_id);
}

uint32
dnx_data_mdb_em_mact_dbal_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_mact_dbal_id);
}



const dnx_data_mdb_em_em_utilization_api_db_translation_t *
dnx_data_mdb_em_em_utilization_api_db_translation_get(
    int unit,
    int resource_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_em_utilization_api_db_translation);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, resource_id, 0);
    return (const dnx_data_mdb_em_em_utilization_api_db_translation_t *) data;

}

const dnx_data_mdb_em_spn_t *
dnx_data_mdb_em_spn_get(
    int unit,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_spn);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_id, 0);
    return (const dnx_data_mdb_em_spn_t *) data;

}

const dnx_data_mdb_em_lfsr_t *
dnx_data_mdb_em_lfsr_get(
    int unit,
    int lfsr_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_lfsr);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, lfsr_index, 0);
    return (const dnx_data_mdb_em_lfsr_t *) data;

}

const dnx_data_mdb_em_emp_index_table_mapping_t *
dnx_data_mdb_em_emp_index_table_mapping_get(
    int unit,
    int emp_table_idx)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_emp_index_table_mapping);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, emp_table_idx, 0);
    return (const dnx_data_mdb_em_emp_index_table_mapping_t *) data;

}

const dnx_data_mdb_em_em_aging_info_t *
dnx_data_mdb_em_em_aging_info_get(
    int unit,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_em_aging_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_id, 0);
    return (const dnx_data_mdb_em_em_aging_info_t *) data;

}

const dnx_data_mdb_em_em_aging_cfg_t *
dnx_data_mdb_em_em_aging_cfg_get(
    int unit,
    int profile,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_em_aging_cfg);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, profile, dbal_id);
    return (const dnx_data_mdb_em_em_aging_cfg_t *) data;

}

const dnx_data_mdb_em_em_info_t *
dnx_data_mdb_em_em_info_get(
    int unit,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_em_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_id, 0);
    return (const dnx_data_mdb_em_em_info_t *) data;

}

const dnx_data_mdb_em_em_interrupt_t *
dnx_data_mdb_em_em_interrupt_get(
    int unit,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_em_interrupt);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_id, 0);
    return (const dnx_data_mdb_em_em_interrupt_t *) data;

}

const dnx_data_mdb_em_step_table_pdb_max_depth_t *
dnx_data_mdb_em_step_table_pdb_max_depth_get(
    int unit,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_step_table_pdb_max_depth);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_id, 0);
    return (const dnx_data_mdb_em_step_table_pdb_max_depth_t *) data;

}

const dnx_data_mdb_em_step_table_max_depth_possible_t *
dnx_data_mdb_em_step_table_max_depth_possible_get(
    int unit,
    int step_table_size_indication,
    int aspect_ratio_combination)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_step_table_max_depth_possible);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, step_table_size_indication, aspect_ratio_combination);
    return (const dnx_data_mdb_em_step_table_max_depth_possible_t *) data;

}

const dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *
dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_get(
    int unit,
    int aspect_ratio,
    int nof_ways)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, aspect_ratio, nof_ways);
    return (const dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) data;

}

const dnx_data_mdb_em_mdb_15_info_t *
dnx_data_mdb_em_mdb_15_info_get(
    int unit,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_15_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_id, 0);
    return (const dnx_data_mdb_em_mdb_15_info_t *) data;

}

const dnx_data_mdb_em_mdb_16_info_t *
dnx_data_mdb_em_mdb_16_info_get(
    int unit,
    int mdb_table_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_16_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id, 0);
    return (const dnx_data_mdb_em_mdb_16_info_t *) data;

}

const dnx_data_mdb_em_mdb_21_info_t *
dnx_data_mdb_em_mdb_21_info_get(
    int unit,
    int mdb_table_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_21_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id, 0);
    return (const dnx_data_mdb_em_mdb_21_info_t *) data;

}

const dnx_data_mdb_em_mdb_23_info_t *
dnx_data_mdb_em_mdb_23_info_get(
    int unit,
    int mdb_table_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_23_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id, 0);
    return (const dnx_data_mdb_em_mdb_23_info_t *) data;

}

const dnx_data_mdb_em_mdb_24_info_t *
dnx_data_mdb_em_mdb_24_info_get(
    int unit,
    int mdb_table_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_24_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id, 0);
    return (const dnx_data_mdb_em_mdb_24_info_t *) data;

}

const dnx_data_mdb_em_mdb_29_info_t *
dnx_data_mdb_em_mdb_29_info_get(
    int unit,
    int mdb_table_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_29_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id, 0);
    return (const dnx_data_mdb_em_mdb_29_info_t *) data;

}

const dnx_data_mdb_em_mdb_31_info_t *
dnx_data_mdb_em_mdb_31_info_get(
    int unit,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_31_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_id, 0);
    return (const dnx_data_mdb_em_mdb_31_info_t *) data;

}

const dnx_data_mdb_em_mdb_32_info_t *
dnx_data_mdb_em_mdb_32_info_get(
    int unit,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_32_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_id, 0);
    return (const dnx_data_mdb_em_mdb_32_info_t *) data;

}

const dnx_data_mdb_em_mdb_41_info_t *
dnx_data_mdb_em_mdb_41_info_get(
    int unit,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_41_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_id, 0);
    return (const dnx_data_mdb_em_mdb_41_info_t *) data;

}

const dnx_data_mdb_em_mdb_45_info_t *
dnx_data_mdb_em_mdb_45_info_get(
    int unit,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_45_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_id, 0);
    return (const dnx_data_mdb_em_mdb_45_info_t *) data;

}

const dnx_data_mdb_em_mdb_emp_tables_mapping_t *
dnx_data_mdb_em_mdb_emp_tables_mapping_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_emp_tables_mapping);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_em_mdb_emp_tables_mapping_t *) data;

}

const dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *
dnx_data_mdb_em_mdb_em_shift_vmv_regs_get(
    int unit,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_em_shift_vmv_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_id, 0);
    return (const dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *) data;

}

const dnx_data_mdb_em_em_emp_t *
dnx_data_mdb_em_em_emp_get(
    int unit,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_em_emp);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_id, 0);
    return (const dnx_data_mdb_em_em_emp_t *) data;

}

const dnx_data_mdb_em_mdb_step_table_map_t *
dnx_data_mdb_em_mdb_step_table_map_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_step_table_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_em_mdb_step_table_map_t *) data;

}

const dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *
dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_get(
    int unit,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_id, 0);
    return (const dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *) data;

}

const dnx_data_mdb_em_filter_rules_t *
dnx_data_mdb_em_filter_rules_get(
    int unit,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_filter_rules);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_id, 0);
    return (const dnx_data_mdb_em_filter_rules_t *) data;

}

const dnx_data_mdb_em_data_rules_t *
dnx_data_mdb_em_data_rules_get(
    int unit,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_data_rules);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_id, 0);
    return (const dnx_data_mdb_em_data_rules_t *) data;

}


shr_error_e
dnx_data_mdb_em_em_utilization_api_db_translation_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_em_utilization_api_db_translation_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_em_utilization_api_db_translation);
    data = (const dnx_data_mdb_em_em_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_id);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_spn_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_spn_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_spn);
    data = (const dnx_data_mdb_em_spn_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_spn_size);
            break;
        case 1:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_MDB_EM_MAX_NOF_SPN_SIZES, data->spn_array);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_lfsr_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_lfsr_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_lfsr);
    data = (const dnx_data_mdb_em_lfsr_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->lfsr_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_emp_index_table_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_emp_index_table_mapping_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_emp_index_table_mapping);
    data = (const dnx_data_mdb_em_emp_index_table_mapping_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_id);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_em_aging_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_em_aging_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_em_aging_info);
    data = (const dnx_data_mdb_em_em_aging_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max_nof_age_entry_bits);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->rbd_size);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_age_banks);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->total_nof_aging_bits);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_em_aging_cfg_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_em_aging_cfg_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_em_aging_cfg);
    data = (const dnx_data_mdb_em_em_aging_cfg_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->init_value);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->global_value);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->global_mask);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->external_profile);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->aging_disable);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->elephant_disable);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->elephant_values);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mouse_values);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->age_max_values);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->increment_values);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->decrement_values);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->out_values);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->hit_bit_mode);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_em_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_em_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_em_info);
    data = (const dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tid_size);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->em_interface);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->entry_size);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->status_reg);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->step_table_size);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ovf_cam_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_em_interrupt_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_em_interrupt_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_em_interrupt);
    data = (const dnx_data_mdb_em_em_interrupt_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->instance);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->interrupt_register);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->interrupt_field);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_step_table_pdb_max_depth_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_step_table_pdb_max_depth_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_step_table_pdb_max_depth);
    data = (const dnx_data_mdb_em_step_table_pdb_max_depth_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max_depth);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_step_table_max_depth_possible_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_step_table_max_depth_possible_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_step_table_max_depth_possible);
    data = (const dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max_depth);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar);
    data = (const dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max_depth);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_mdb_15_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_mdb_15_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_15_info);
    data = (const dnx_data_mdb_em_mdb_15_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_5_field_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_mdb_16_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_mdb_16_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_16_info);
    data = (const dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_0_field_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_mdb_21_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_mdb_21_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_21_info);
    data = (const dnx_data_mdb_em_mdb_21_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_0_field_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_mdb_23_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_mdb_23_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_23_info);
    data = (const dnx_data_mdb_em_mdb_23_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_0_field_size);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_1_field_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_mdb_24_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_mdb_24_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_24_info);
    data = (const dnx_data_mdb_em_mdb_24_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_0_array_size);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_1_array_size);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_1_field_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_mdb_29_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_mdb_29_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_29_info);
    data = (const dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_0_array_size);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_1_array_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_mdb_31_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_mdb_31_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_31_info);
    data = (const dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->array_size);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_0_field_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_mdb_32_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_mdb_32_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_32_info);
    data = (const dnx_data_mdb_em_mdb_32_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_0_field_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_mdb_41_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_mdb_41_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_41_info);
    data = (const dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->array_size);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_2_field_size);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_3_field_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_mdb_45_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_mdb_45_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_45_info);
    data = (const dnx_data_mdb_em_mdb_45_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->array_size);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_3_field_size);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_7_field_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_mdb_emp_tables_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_mdb_emp_tables_mapping_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_emp_tables_mapping);
    data = (const dnx_data_mdb_em_mdb_emp_tables_mapping_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->emp_age_cfg_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_22_table);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_mdb_em_shift_vmv_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_em_shift_vmv_regs);
    data = (const dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_valid_regs);
            break;
        case 1:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, data->table_name_arr);
            break;
        case 2:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, data->field_name_arr);
            break;
        case 3:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, data->key_name_arr);
            break;
        case 4:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, data->stage_index_arr);
            break;
        case 5:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, data->esem_cmd_indication);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_em_emp_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_em_emp_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_em_emp);
    data = (const dnx_data_mdb_em_em_emp_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->age_mem);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->age_ovf_cam_mem);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->emp_scan_status_field);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->emp_age_cfg_reg);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->global_value);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->global_masks);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->external_profile);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->age_disable);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->elephant_disable);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->initial_values);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_mdb_step_table_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_mdb_step_table_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_step_table_map);
    data = (const dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info);
    data = (const dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_13_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_14_table);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_17_table);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_18_table);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_33_table);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_34_table);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_46_table);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_52_table);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_filter_rules_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_filter_rules_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_filter_rules);
    data = (const dnx_data_mdb_em_filter_rules_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->table_name);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->entry_mask);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->entry_filter);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->src_mask);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->src_filter);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->appdb_id_mask);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->appdb_id_filter);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->accessed_mask);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->accessed_filter);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_data_rules_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_data_rules_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_data_rules);
    data = (const dnx_data_mdb_em_data_rules_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->table_name);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->command);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->payload_mask);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->payload);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_mdb_em_em_utilization_api_db_translation_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_em_utilization_api_db_translation);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_spn_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_spn);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_lfsr_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_lfsr);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_emp_index_table_mapping_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_emp_index_table_mapping);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_em_aging_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_em_aging_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_em_aging_cfg_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_em_aging_cfg);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_em_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_em_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_em_interrupt_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_em_interrupt);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_step_table_pdb_max_depth_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_step_table_pdb_max_depth);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_step_table_max_depth_possible_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_step_table_max_depth_possible);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_mdb_15_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_15_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_mdb_16_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_16_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_mdb_21_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_21_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_mdb_23_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_23_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_mdb_24_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_24_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_mdb_29_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_29_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_mdb_31_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_31_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_mdb_32_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_32_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_mdb_41_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_41_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_mdb_45_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_45_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_mdb_emp_tables_mapping_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_emp_tables_mapping);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_mdb_em_shift_vmv_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_em_shift_vmv_regs);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_em_emp_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_em_emp);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_mdb_step_table_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_step_table_map);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_filter_rules_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_filter_rules);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_data_rules_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_data_rules);

}






static shr_error_e
dnx_data_mdb_direct_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "direct";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_mdb_direct_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data mdb direct features");

    submodule_data->features[dnx_data_mdb_direct_fec_abk_mapping].name = "fec_abk_mapping";
    submodule_data->features[dnx_data_mdb_direct_fec_abk_mapping].doc = "";
    submodule_data->features[dnx_data_mdb_direct_fec_abk_mapping].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_direct_fec_abk_macro_mapping].name = "fec_abk_macro_mapping";
    submodule_data->features[dnx_data_mdb_direct_fec_abk_macro_mapping].doc = "";
    submodule_data->features[dnx_data_mdb_direct_fec_abk_macro_mapping].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_direct_vsi_config_size].name = "vsi_config_size";
    submodule_data->features[dnx_data_mdb_direct_vsi_config_size].doc = "";
    submodule_data->features[dnx_data_mdb_direct_vsi_config_size].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_direct_fec_address_remap].name = "fec_address_remap";
    submodule_data->features[dnx_data_mdb_direct_fec_address_remap].doc = "";
    submodule_data->features[dnx_data_mdb_direct_fec_address_remap].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_mdb_direct_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data mdb direct defines");

    submodule_data->defines[dnx_data_mdb_direct_define_physical_address_max_bits].name = "physical_address_max_bits";
    submodule_data->defines[dnx_data_mdb_direct_define_physical_address_max_bits].doc = "";
    
    submodule_data->defines[dnx_data_mdb_direct_define_physical_address_max_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_direct_define_physical_address_max_bits_support_single_macro_b_granularity_fec_alloc].name = "physical_address_max_bits_support_single_macro_b_granularity_fec_alloc";
    submodule_data->defines[dnx_data_mdb_direct_define_physical_address_max_bits_support_single_macro_b_granularity_fec_alloc].doc = "";
    
    submodule_data->defines[dnx_data_mdb_direct_define_physical_address_max_bits_support_single_macro_b_granularity_fec_alloc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_direct_define_vsi_physical_address_max_bits].name = "vsi_physical_address_max_bits";
    submodule_data->defines[dnx_data_mdb_direct_define_vsi_physical_address_max_bits].doc = "";
    
    submodule_data->defines[dnx_data_mdb_direct_define_vsi_physical_address_max_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_direct_define_fec_row_width].name = "fec_row_width";
    submodule_data->defines[dnx_data_mdb_direct_define_fec_row_width].doc = "";
    
    submodule_data->defines[dnx_data_mdb_direct_define_fec_row_width].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_direct_define_fec_address_mapping_bits].name = "fec_address_mapping_bits";
    submodule_data->defines[dnx_data_mdb_direct_define_fec_address_mapping_bits].doc = "";
    
    submodule_data->defines[dnx_data_mdb_direct_define_fec_address_mapping_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_direct_define_fec_address_mapping_bits_relevant].name = "fec_address_mapping_bits_relevant";
    submodule_data->defines[dnx_data_mdb_direct_define_fec_address_mapping_bits_relevant].doc = "";
    
    submodule_data->defines[dnx_data_mdb_direct_define_fec_address_mapping_bits_relevant].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_direct_define_fec_max_cluster_pairs].name = "fec_max_cluster_pairs";
    submodule_data->defines[dnx_data_mdb_direct_define_fec_max_cluster_pairs].doc = "";
    
    submodule_data->defines[dnx_data_mdb_direct_define_fec_max_cluster_pairs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_direct_define_max_payload_size].name = "max_payload_size";
    submodule_data->defines[dnx_data_mdb_direct_define_max_payload_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_direct_define_max_payload_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_direct_define_nof_fecs_in_super_fec].name = "nof_fecs_in_super_fec";
    submodule_data->defines[dnx_data_mdb_direct_define_nof_fecs_in_super_fec].doc = "";
    
    submodule_data->defines[dnx_data_mdb_direct_define_nof_fecs_in_super_fec].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_direct_define_nof_fec_ecmp_hierarchies].name = "nof_fec_ecmp_hierarchies";
    submodule_data->defines[dnx_data_mdb_direct_define_nof_fec_ecmp_hierarchies].doc = "";
    
    submodule_data->defines[dnx_data_mdb_direct_define_nof_fec_ecmp_hierarchies].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_direct_define_physical_address_ovf_enable_val].name = "physical_address_ovf_enable_val";
    submodule_data->defines[dnx_data_mdb_direct_define_physical_address_ovf_enable_val].doc = "";
    
    submodule_data->defines[dnx_data_mdb_direct_define_physical_address_ovf_enable_val].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_direct_define_nof_fec_dbs].name = "nof_fec_dbs";
    submodule_data->defines[dnx_data_mdb_direct_define_nof_fec_dbs].doc = "The NOF of FEC DBs";
    
    submodule_data->defines[dnx_data_mdb_direct_define_nof_fec_dbs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_direct_define_map_payload_size_enum].name = "map_payload_size_enum";
    submodule_data->defines[dnx_data_mdb_direct_define_map_payload_size_enum].doc = "";
    
    submodule_data->defines[dnx_data_mdb_direct_define_map_payload_size_enum].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_direct_define_map_payload_size].name = "map_payload_size";
    submodule_data->defines[dnx_data_mdb_direct_define_map_payload_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_direct_define_map_payload_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_direct_define_fec_allocation_base_macro_type].name = "fec_allocation_base_macro_type";
    submodule_data->defines[dnx_data_mdb_direct_define_fec_allocation_base_macro_type].doc = "";
    
    submodule_data->defines[dnx_data_mdb_direct_define_fec_allocation_base_macro_type].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_mdb_direct_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data mdb direct tables");

    
    submodule_data->tables[dnx_data_mdb_direct_table_vsi_payload_type_per_range].name = "vsi_payload_type_per_range";
    submodule_data->tables[dnx_data_mdb_direct_table_vsi_payload_type_per_range].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_vsi_payload_type_per_range].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_direct_table_vsi_payload_type_per_range].size_of_values = sizeof(dnx_data_mdb_direct_vsi_payload_type_per_range_t);
    submodule_data->tables[dnx_data_mdb_direct_table_vsi_payload_type_per_range].entry_get = dnx_data_mdb_direct_vsi_payload_type_per_range_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_direct_table_vsi_payload_type_per_range].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_direct_table_vsi_payload_type_per_range].keys[0].name = "range_idx";
    submodule_data->tables[dnx_data_mdb_direct_table_vsi_payload_type_per_range].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_direct_table_vsi_payload_type_per_range].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_direct_table_vsi_payload_type_per_range].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_direct_table_vsi_payload_type_per_range].nof_values, "_dnx_data_mdb_direct_table_vsi_payload_type_per_range table values");
    submodule_data->tables[dnx_data_mdb_direct_table_vsi_payload_type_per_range].values[0].name = "payload_type";
    submodule_data->tables[dnx_data_mdb_direct_table_vsi_payload_type_per_range].values[0].type = "uint32";
    submodule_data->tables[dnx_data_mdb_direct_table_vsi_payload_type_per_range].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_vsi_payload_type_per_range].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_direct_vsi_payload_type_per_range_t, payload_type);

    
    submodule_data->tables[dnx_data_mdb_direct_table_direct_utilization_api_db_translation].name = "direct_utilization_api_db_translation";
    submodule_data->tables[dnx_data_mdb_direct_table_direct_utilization_api_db_translation].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_direct_utilization_api_db_translation].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_direct_table_direct_utilization_api_db_translation].size_of_values = sizeof(dnx_data_mdb_direct_direct_utilization_api_db_translation_t);
    submodule_data->tables[dnx_data_mdb_direct_table_direct_utilization_api_db_translation].entry_get = dnx_data_mdb_direct_direct_utilization_api_db_translation_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_direct_table_direct_utilization_api_db_translation].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_direct_table_direct_utilization_api_db_translation].keys[0].name = "resource_id";
    submodule_data->tables[dnx_data_mdb_direct_table_direct_utilization_api_db_translation].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_direct_table_direct_utilization_api_db_translation].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_direct_table_direct_utilization_api_db_translation].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_direct_table_direct_utilization_api_db_translation].nof_values, "_dnx_data_mdb_direct_table_direct_utilization_api_db_translation table values");
    submodule_data->tables[dnx_data_mdb_direct_table_direct_utilization_api_db_translation].values[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_direct_table_direct_utilization_api_db_translation].values[0].type = "uint8";
    submodule_data->tables[dnx_data_mdb_direct_table_direct_utilization_api_db_translation].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_direct_utilization_api_db_translation].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_direct_direct_utilization_api_db_translation_t, dbal_id);

    
    submodule_data->tables[dnx_data_mdb_direct_table_direct_info].name = "direct_info";
    submodule_data->tables[dnx_data_mdb_direct_table_direct_info].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_direct_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_direct_table_direct_info].size_of_values = sizeof(dnx_data_mdb_direct_direct_info_t);
    submodule_data->tables[dnx_data_mdb_direct_table_direct_info].entry_get = dnx_data_mdb_direct_direct_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_direct_table_direct_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_direct_table_direct_info].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_direct_table_direct_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_direct_table_direct_info].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_direct_table_direct_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_direct_table_direct_info].nof_values, "_dnx_data_mdb_direct_table_direct_info table values");
    submodule_data->tables[dnx_data_mdb_direct_table_direct_info].values[0].name = "payload_type";
    submodule_data->tables[dnx_data_mdb_direct_table_direct_info].values[0].type = "dbal_enum_value_field_direct_payload_sizes_e";
    submodule_data->tables[dnx_data_mdb_direct_table_direct_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_direct_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_direct_direct_info_t, payload_type);

    
    submodule_data->tables[dnx_data_mdb_direct_table_hit_bit_pos_in_abk].name = "hit_bit_pos_in_abk";
    submodule_data->tables[dnx_data_mdb_direct_table_hit_bit_pos_in_abk].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_hit_bit_pos_in_abk].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_direct_table_hit_bit_pos_in_abk].size_of_values = sizeof(dnx_data_mdb_direct_hit_bit_pos_in_abk_t);
    submodule_data->tables[dnx_data_mdb_direct_table_hit_bit_pos_in_abk].entry_get = dnx_data_mdb_direct_hit_bit_pos_in_abk_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_direct_table_hit_bit_pos_in_abk].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_direct_table_hit_bit_pos_in_abk].keys[0].name = "hit_bit_index";
    submodule_data->tables[dnx_data_mdb_direct_table_hit_bit_pos_in_abk].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_direct_table_hit_bit_pos_in_abk].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_direct_table_hit_bit_pos_in_abk].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_direct_table_hit_bit_pos_in_abk].nof_values, "_dnx_data_mdb_direct_table_hit_bit_pos_in_abk table values");
    submodule_data->tables[dnx_data_mdb_direct_table_hit_bit_pos_in_abk].values[0].name = "hit_bit_position";
    submodule_data->tables[dnx_data_mdb_direct_table_hit_bit_pos_in_abk].values[0].type = "uint8";
    submodule_data->tables[dnx_data_mdb_direct_table_hit_bit_pos_in_abk].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_hit_bit_pos_in_abk].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_direct_hit_bit_pos_in_abk_t, hit_bit_position);

    
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].name = "mdb_11_info";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].size_of_values = sizeof(dnx_data_mdb_direct_mdb_11_info_t);
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].entry_get = dnx_data_mdb_direct_mdb_11_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].keys[0].name = "mdb_table_id";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].nof_values = 8;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].nof_values, "_dnx_data_mdb_direct_table_mdb_11_info table values");
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_direct_mdb_11_info_t, dbal_table);
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[1].name = "mdb_item_0_array_size";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_direct_mdb_11_info_t, mdb_item_0_array_size);
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[2].name = "mdb_item_0_field_size";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_direct_mdb_11_info_t, mdb_item_0_field_size);
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[3].name = "mdb_item_1_field_size";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[3].type = "uint32";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_direct_mdb_11_info_t, mdb_item_1_field_size);
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[4].name = "mdb_item_3_array_size";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[4].type = "uint32";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[4].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[4].offset = UTILEX_OFFSETOF(dnx_data_mdb_direct_mdb_11_info_t, mdb_item_3_array_size);
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[5].name = "mdb_item_3_field_size";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[5].type = "uint32";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[5].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[5].offset = UTILEX_OFFSETOF(dnx_data_mdb_direct_mdb_11_info_t, mdb_item_3_field_size);
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[6].name = "mdb_item_4_array_size";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[6].type = "uint32";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[6].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[6].offset = UTILEX_OFFSETOF(dnx_data_mdb_direct_mdb_11_info_t, mdb_item_4_array_size);
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[7].name = "mdb_item_4_field_size";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[7].type = "uint32";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[7].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_11_info].values[7].offset = UTILEX_OFFSETOF(dnx_data_mdb_direct_mdb_11_info_t, mdb_item_4_field_size);

    
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].name = "mdb_26_info";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].size_of_values = sizeof(dnx_data_mdb_direct_mdb_26_info_t);
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].entry_get = dnx_data_mdb_direct_mdb_26_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].keys[0].name = "mdb_table_id";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].nof_values, "_dnx_data_mdb_direct_table_mdb_26_info table values");
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_direct_mdb_26_info_t, dbal_table);
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].values[1].name = "mdb_item_0_field_size";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_direct_mdb_26_info_t, mdb_item_0_field_size);
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].values[2].name = "mdb_item_1_field_size";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_26_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_direct_mdb_26_info_t, mdb_item_1_field_size);

    
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info].name = "mdb_direct_mdb_phy_tables_info";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info].size_of_values = sizeof(dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_t);
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info].entry_get = dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info].keys[0].name = "mdb_table_id";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info].nof_values, "_dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info table values");
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info].values[0].name = "mdb_12_table";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_t, mdb_12_table);
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info].values[1].name = "mdb_42_table";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info].values[1].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_t, mdb_42_table);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_mdb_direct_feature_get(
    int unit,
    dnx_data_mdb_direct_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, feature);
}


uint32
dnx_data_mdb_direct_physical_address_max_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_define_physical_address_max_bits);
}

uint32
dnx_data_mdb_direct_physical_address_max_bits_support_single_macro_b_granularity_fec_alloc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_define_physical_address_max_bits_support_single_macro_b_granularity_fec_alloc);
}

uint32
dnx_data_mdb_direct_vsi_physical_address_max_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_define_vsi_physical_address_max_bits);
}

uint32
dnx_data_mdb_direct_fec_row_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_define_fec_row_width);
}

uint32
dnx_data_mdb_direct_fec_address_mapping_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_define_fec_address_mapping_bits);
}

uint32
dnx_data_mdb_direct_fec_address_mapping_bits_relevant_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_define_fec_address_mapping_bits_relevant);
}

uint32
dnx_data_mdb_direct_fec_max_cluster_pairs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_define_fec_max_cluster_pairs);
}

uint32
dnx_data_mdb_direct_max_payload_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_define_max_payload_size);
}

uint32
dnx_data_mdb_direct_nof_fecs_in_super_fec_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_define_nof_fecs_in_super_fec);
}

uint32
dnx_data_mdb_direct_nof_fec_ecmp_hierarchies_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_define_nof_fec_ecmp_hierarchies);
}

uint32
dnx_data_mdb_direct_physical_address_ovf_enable_val_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_define_physical_address_ovf_enable_val);
}

uint32
dnx_data_mdb_direct_nof_fec_dbs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_define_nof_fec_dbs);
}

uint32
dnx_data_mdb_direct_map_payload_size_enum_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_define_map_payload_size_enum);
}

uint32
dnx_data_mdb_direct_map_payload_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_define_map_payload_size);
}

uint32
dnx_data_mdb_direct_fec_allocation_base_macro_type_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_define_fec_allocation_base_macro_type);
}



const dnx_data_mdb_direct_vsi_payload_type_per_range_t *
dnx_data_mdb_direct_vsi_payload_type_per_range_get(
    int unit,
    int range_idx)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_vsi_payload_type_per_range);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, range_idx, 0);
    return (const dnx_data_mdb_direct_vsi_payload_type_per_range_t *) data;

}

const dnx_data_mdb_direct_direct_utilization_api_db_translation_t *
dnx_data_mdb_direct_direct_utilization_api_db_translation_get(
    int unit,
    int resource_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_direct_utilization_api_db_translation);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, resource_id, 0);
    return (const dnx_data_mdb_direct_direct_utilization_api_db_translation_t *) data;

}

const dnx_data_mdb_direct_direct_info_t *
dnx_data_mdb_direct_direct_info_get(
    int unit,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_direct_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_id, 0);
    return (const dnx_data_mdb_direct_direct_info_t *) data;

}

const dnx_data_mdb_direct_hit_bit_pos_in_abk_t *
dnx_data_mdb_direct_hit_bit_pos_in_abk_get(
    int unit,
    int hit_bit_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_hit_bit_pos_in_abk);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, hit_bit_index, 0);
    return (const dnx_data_mdb_direct_hit_bit_pos_in_abk_t *) data;

}

const dnx_data_mdb_direct_mdb_11_info_t *
dnx_data_mdb_direct_mdb_11_info_get(
    int unit,
    int mdb_table_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_mdb_11_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id, 0);
    return (const dnx_data_mdb_direct_mdb_11_info_t *) data;

}

const dnx_data_mdb_direct_mdb_26_info_t *
dnx_data_mdb_direct_mdb_26_info_get(
    int unit,
    int mdb_table_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_mdb_26_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id, 0);
    return (const dnx_data_mdb_direct_mdb_26_info_t *) data;

}

const dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_t *
dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_get(
    int unit,
    int mdb_table_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id, 0);
    return (const dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_t *) data;

}


shr_error_e
dnx_data_mdb_direct_vsi_payload_type_per_range_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_direct_vsi_payload_type_per_range_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_vsi_payload_type_per_range);
    data = (const dnx_data_mdb_direct_vsi_payload_type_per_range_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->payload_type);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_direct_direct_utilization_api_db_translation_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_direct_direct_utilization_api_db_translation_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_direct_utilization_api_db_translation);
    data = (const dnx_data_mdb_direct_direct_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_id);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_direct_direct_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_direct_direct_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_direct_info);
    data = (const dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->payload_type);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_direct_hit_bit_pos_in_abk_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_direct_hit_bit_pos_in_abk_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_hit_bit_pos_in_abk);
    data = (const dnx_data_mdb_direct_hit_bit_pos_in_abk_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->hit_bit_position);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_direct_mdb_11_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_direct_mdb_11_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_mdb_11_info);
    data = (const dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_0_array_size);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_0_field_size);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_1_field_size);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_3_array_size);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_3_field_size);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_4_array_size);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_4_field_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_direct_mdb_26_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_direct_mdb_26_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_mdb_26_info);
    data = (const dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_0_field_size);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_1_field_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info);
    data = (const dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_12_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_42_table);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_mdb_direct_vsi_payload_type_per_range_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_vsi_payload_type_per_range);

}

const dnxc_data_table_info_t *
dnx_data_mdb_direct_direct_utilization_api_db_translation_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_direct_utilization_api_db_translation);

}

const dnxc_data_table_info_t *
dnx_data_mdb_direct_direct_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_direct_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_direct_hit_bit_pos_in_abk_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_hit_bit_pos_in_abk);

}

const dnxc_data_table_info_t *
dnx_data_mdb_direct_mdb_11_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_mdb_11_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_direct_mdb_26_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_mdb_26_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info);

}






static shr_error_e
dnx_data_mdb_eedb_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "eedb";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_mdb_eedb_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data mdb eedb features");

    submodule_data->features[dnx_data_mdb_eedb_eedb_bank_hitbit_masked_clear].name = "eedb_bank_hitbit_masked_clear";
    submodule_data->features[dnx_data_mdb_eedb_eedb_bank_hitbit_masked_clear].doc = "";
    submodule_data->features[dnx_data_mdb_eedb_eedb_bank_hitbit_masked_clear].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_mdb_eedb_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data mdb eedb defines");

    submodule_data->defines[dnx_data_mdb_eedb_define_phase_map_bits].name = "phase_map_bits";
    submodule_data->defines[dnx_data_mdb_eedb_define_phase_map_bits].doc = "";
    
    submodule_data->defines[dnx_data_mdb_eedb_define_phase_map_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_eedb_define_phase_map_array_size].name = "phase_map_array_size";
    submodule_data->defines[dnx_data_mdb_eedb_define_phase_map_array_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_eedb_define_phase_map_array_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_eedb_define_nof_phase_per_mag].name = "nof_phase_per_mag";
    submodule_data->defines[dnx_data_mdb_eedb_define_nof_phase_per_mag].doc = "";
    
    submodule_data->defines[dnx_data_mdb_eedb_define_nof_phase_per_mag].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_eedb_define_nof_phases].name = "nof_phases";
    submodule_data->defines[dnx_data_mdb_eedb_define_nof_phases].doc = "";
    
    submodule_data->defines[dnx_data_mdb_eedb_define_nof_phases].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_eedb_define_nof_eedb_mags].name = "nof_eedb_mags";
    submodule_data->defines[dnx_data_mdb_eedb_define_nof_eedb_mags].doc = "";
    
    submodule_data->defines[dnx_data_mdb_eedb_define_nof_eedb_mags].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_eedb_define_nof_eedb_banks].name = "nof_eedb_banks";
    submodule_data->defines[dnx_data_mdb_eedb_define_nof_eedb_banks].doc = "";
    
    submodule_data->defines[dnx_data_mdb_eedb_define_nof_eedb_banks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_eedb_define_nof_phases_per_eedb_bank].name = "nof_phases_per_eedb_bank";
    submodule_data->defines[dnx_data_mdb_eedb_define_nof_phases_per_eedb_bank].doc = "";
    
    submodule_data->defines[dnx_data_mdb_eedb_define_nof_phases_per_eedb_bank].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_eedb_define_nof_phases_per_eedb_bank_size].name = "nof_phases_per_eedb_bank_size";
    submodule_data->defines[dnx_data_mdb_eedb_define_nof_phases_per_eedb_bank_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_eedb_define_nof_phases_per_eedb_bank_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_eedb_define_phase_bank_select_default].name = "phase_bank_select_default";
    submodule_data->defines[dnx_data_mdb_eedb_define_phase_bank_select_default].doc = "";
    
    submodule_data->defines[dnx_data_mdb_eedb_define_phase_bank_select_default].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_eedb_define_entry_format_bits].name = "entry_format_bits";
    submodule_data->defines[dnx_data_mdb_eedb_define_entry_format_bits].doc = "";
    
    submodule_data->defines[dnx_data_mdb_eedb_define_entry_format_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_eedb_define_entry_format_encoding_bits].name = "entry_format_encoding_bits";
    submodule_data->defines[dnx_data_mdb_eedb_define_entry_format_encoding_bits].doc = "";
    
    submodule_data->defines[dnx_data_mdb_eedb_define_entry_format_encoding_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_eedb_define_bank_id_bits].name = "bank_id_bits";
    submodule_data->defines[dnx_data_mdb_eedb_define_bank_id_bits].doc = "";
    
    submodule_data->defines[dnx_data_mdb_eedb_define_bank_id_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_eedb_define_entry_bank].name = "entry_bank";
    submodule_data->defines[dnx_data_mdb_eedb_define_entry_bank].doc = "";
    
    submodule_data->defines[dnx_data_mdb_eedb_define_entry_bank].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_eedb_define_abk_bank].name = "abk_bank";
    submodule_data->defines[dnx_data_mdb_eedb_define_abk_bank].doc = "";
    
    submodule_data->defines[dnx_data_mdb_eedb_define_abk_bank].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_mdb_eedb_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data mdb eedb tables");

    
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_info].name = "phase_info";
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_info].doc = "";
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_info].size_of_values = sizeof(dnx_data_mdb_eedb_phase_info_t);
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_info].entry_get = dnx_data_mdb_eedb_phase_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_info].keys[0].name = "phase_number";
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_info].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_eedb_table_phase_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_eedb_table_phase_info].nof_values, "_dnx_data_mdb_eedb_table_phase_info table values");
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_info].values[0].name = "nof_available_banks";
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_info].values[0].type = "uint32";
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_eedb_phase_info_t, nof_available_banks);

    
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_ll_bucket_enable].name = "phase_ll_bucket_enable";
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_ll_bucket_enable].doc = "";
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_ll_bucket_enable].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_ll_bucket_enable].size_of_values = sizeof(dnx_data_mdb_eedb_phase_ll_bucket_enable_t);
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_ll_bucket_enable].entry_get = dnx_data_mdb_eedb_phase_ll_bucket_enable_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_ll_bucket_enable].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_ll_bucket_enable].keys[0].name = "phase_number";
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_ll_bucket_enable].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_ll_bucket_enable].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_eedb_table_phase_ll_bucket_enable].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_eedb_table_phase_ll_bucket_enable].nof_values, "_dnx_data_mdb_eedb_table_phase_ll_bucket_enable table values");
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_ll_bucket_enable].values[0].name = "bucket_enable_dbal_table";
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_ll_bucket_enable].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_ll_bucket_enable].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_ll_bucket_enable].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_eedb_phase_ll_bucket_enable_t, bucket_enable_dbal_table);

    
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_to_table].name = "phase_to_table";
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_to_table].doc = "";
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_to_table].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_to_table].size_of_values = sizeof(dnx_data_mdb_eedb_phase_to_table_t);
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_to_table].entry_get = dnx_data_mdb_eedb_phase_to_table_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_to_table].nof_keys = 2;
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_to_table].keys[0].name = "eedb_mem_type";
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_to_table].keys[0].doc = "";
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_to_table].keys[1].name = "phase_number";
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_to_table].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_to_table].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_eedb_table_phase_to_table].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_eedb_table_phase_to_table].nof_values, "_dnx_data_mdb_eedb_table_phase_to_table table values");
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_to_table].values[0].name = "mdb_physical_table_id";
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_to_table].values[0].type = "mdb_physical_table_e";
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_to_table].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_eedb_table_phase_to_table].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_eedb_phase_to_table_t, mdb_physical_table_id);

    
    submodule_data->tables[dnx_data_mdb_eedb_table_outlif_physical_phase_granularity].name = "outlif_physical_phase_granularity";
    submodule_data->tables[dnx_data_mdb_eedb_table_outlif_physical_phase_granularity].doc = "";
    submodule_data->tables[dnx_data_mdb_eedb_table_outlif_physical_phase_granularity].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_eedb_table_outlif_physical_phase_granularity].size_of_values = sizeof(dnx_data_mdb_eedb_outlif_physical_phase_granularity_t);
    submodule_data->tables[dnx_data_mdb_eedb_table_outlif_physical_phase_granularity].entry_get = dnx_data_mdb_eedb_outlif_physical_phase_granularity_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_eedb_table_outlif_physical_phase_granularity].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_eedb_table_outlif_physical_phase_granularity].keys[0].name = "outlif_physical_phase";
    submodule_data->tables[dnx_data_mdb_eedb_table_outlif_physical_phase_granularity].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_eedb_table_outlif_physical_phase_granularity].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_eedb_table_outlif_physical_phase_granularity].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_eedb_table_outlif_physical_phase_granularity].nof_values, "_dnx_data_mdb_eedb_table_outlif_physical_phase_granularity table values");
    submodule_data->tables[dnx_data_mdb_eedb_table_outlif_physical_phase_granularity].values[0].name = "data_granularity";
    submodule_data->tables[dnx_data_mdb_eedb_table_outlif_physical_phase_granularity].values[0].type = "uint32";
    submodule_data->tables[dnx_data_mdb_eedb_table_outlif_physical_phase_granularity].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_eedb_table_outlif_physical_phase_granularity].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_eedb_outlif_physical_phase_granularity_t, data_granularity);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_mdb_eedb_feature_get(
    int unit,
    dnx_data_mdb_eedb_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, feature);
}


uint32
dnx_data_mdb_eedb_phase_map_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_define_phase_map_bits);
}

uint32
dnx_data_mdb_eedb_phase_map_array_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_define_phase_map_array_size);
}

uint32
dnx_data_mdb_eedb_nof_phase_per_mag_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_define_nof_phase_per_mag);
}

uint32
dnx_data_mdb_eedb_nof_phases_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_define_nof_phases);
}

uint32
dnx_data_mdb_eedb_nof_eedb_mags_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_define_nof_eedb_mags);
}

uint32
dnx_data_mdb_eedb_nof_eedb_banks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_define_nof_eedb_banks);
}

uint32
dnx_data_mdb_eedb_nof_phases_per_eedb_bank_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_define_nof_phases_per_eedb_bank);
}

uint32
dnx_data_mdb_eedb_nof_phases_per_eedb_bank_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_define_nof_phases_per_eedb_bank_size);
}

uint32
dnx_data_mdb_eedb_phase_bank_select_default_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_define_phase_bank_select_default);
}

uint32
dnx_data_mdb_eedb_entry_format_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_define_entry_format_bits);
}

uint32
dnx_data_mdb_eedb_entry_format_encoding_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_define_entry_format_encoding_bits);
}

uint32
dnx_data_mdb_eedb_bank_id_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_define_bank_id_bits);
}

uint32
dnx_data_mdb_eedb_entry_bank_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_define_entry_bank);
}

uint32
dnx_data_mdb_eedb_abk_bank_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_define_abk_bank);
}



const dnx_data_mdb_eedb_phase_info_t *
dnx_data_mdb_eedb_phase_info_get(
    int unit,
    int phase_number)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_table_phase_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, phase_number, 0);
    return (const dnx_data_mdb_eedb_phase_info_t *) data;

}

const dnx_data_mdb_eedb_phase_ll_bucket_enable_t *
dnx_data_mdb_eedb_phase_ll_bucket_enable_get(
    int unit,
    int phase_number)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_table_phase_ll_bucket_enable);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, phase_number, 0);
    return (const dnx_data_mdb_eedb_phase_ll_bucket_enable_t *) data;

}

const dnx_data_mdb_eedb_phase_to_table_t *
dnx_data_mdb_eedb_phase_to_table_get(
    int unit,
    int eedb_mem_type,
    int phase_number)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_table_phase_to_table);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, eedb_mem_type, phase_number);
    return (const dnx_data_mdb_eedb_phase_to_table_t *) data;

}

const dnx_data_mdb_eedb_outlif_physical_phase_granularity_t *
dnx_data_mdb_eedb_outlif_physical_phase_granularity_get(
    int unit,
    int outlif_physical_phase)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_table_outlif_physical_phase_granularity);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, outlif_physical_phase, 0);
    return (const dnx_data_mdb_eedb_outlif_physical_phase_granularity_t *) data;

}


shr_error_e
dnx_data_mdb_eedb_phase_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_eedb_phase_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_table_phase_info);
    data = (const dnx_data_mdb_eedb_phase_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_available_banks);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_eedb_phase_ll_bucket_enable_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_eedb_phase_ll_bucket_enable_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_table_phase_ll_bucket_enable);
    data = (const dnx_data_mdb_eedb_phase_ll_bucket_enable_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->bucket_enable_dbal_table);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_eedb_phase_to_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_eedb_phase_to_table_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_table_phase_to_table);
    data = (const dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_physical_table_id);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_eedb_outlif_physical_phase_granularity_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_eedb_outlif_physical_phase_granularity_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_table_outlif_physical_phase_granularity);
    data = (const dnx_data_mdb_eedb_outlif_physical_phase_granularity_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->data_granularity);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_mdb_eedb_phase_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_table_phase_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_eedb_phase_ll_bucket_enable_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_table_phase_ll_bucket_enable);

}

const dnxc_data_table_info_t *
dnx_data_mdb_eedb_phase_to_table_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_table_phase_to_table);

}

const dnxc_data_table_info_t *
dnx_data_mdb_eedb_outlif_physical_phase_granularity_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_eedb, dnx_data_mdb_eedb_table_outlif_physical_phase_granularity);

}






static shr_error_e
dnx_data_mdb_kaps_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "kaps";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_mdb_kaps_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data mdb kaps features");

    submodule_data->features[dnx_data_mdb_kaps_flex_bb_row].name = "flex_bb_row";
    submodule_data->features[dnx_data_mdb_kaps_flex_bb_row].doc = "";
    submodule_data->features[dnx_data_mdb_kaps_flex_bb_row].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_kaps_tcam_read_valid_bits_lsbits].name = "tcam_read_valid_bits_lsbits";
    submodule_data->features[dnx_data_mdb_kaps_tcam_read_valid_bits_lsbits].doc = "";
    submodule_data->features[dnx_data_mdb_kaps_tcam_read_valid_bits_lsbits].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_kaps_hitbit_support].name = "hitbit_support";
    submodule_data->features[dnx_data_mdb_kaps_hitbit_support].doc = "";
    submodule_data->features[dnx_data_mdb_kaps_hitbit_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_mdb_kaps_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data mdb kaps defines");

    submodule_data->defines[dnx_data_mdb_kaps_define_nof_dbs].name = "nof_dbs";
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_dbs].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_dbs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_nof_small_bbs].name = "nof_small_bbs";
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_small_bbs].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_small_bbs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_rpb_max].name = "nof_rows_in_rpb_max";
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_rpb_max].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_rpb_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_small_ads_max].name = "nof_rows_in_small_ads_max";
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_small_ads_max].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_small_ads_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_small_bb_max].name = "nof_rows_in_small_bb_max";
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_small_bb_max].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_small_bb_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_small_rpb_hitbits_max].name = "nof_rows_in_small_rpb_hitbits_max";
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_small_rpb_hitbits_max].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_small_rpb_hitbits_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_nof_big_bbs_blk_ids].name = "nof_big_bbs_blk_ids";
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_big_bbs_blk_ids].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_big_bbs_blk_ids].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_nof_bytes_in_hitbit_row].name = "nof_bytes_in_hitbit_row";
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_bytes_in_hitbit_row].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_bytes_in_hitbit_row].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_key_lsn_bits].name = "key_lsn_bits";
    submodule_data->defines[dnx_data_mdb_kaps_define_key_lsn_bits].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_key_lsn_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_key_prefix_length].name = "key_prefix_length";
    submodule_data->defines[dnx_data_mdb_kaps_define_key_prefix_length].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_key_prefix_length].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_max_prefix_in_bb_row].name = "max_prefix_in_bb_row";
    submodule_data->defines[dnx_data_mdb_kaps_define_max_prefix_in_bb_row].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_max_prefix_in_bb_row].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_key_width_in_bits].name = "key_width_in_bits";
    submodule_data->defines[dnx_data_mdb_kaps_define_key_width_in_bits].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_key_width_in_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_bb_byte_width].name = "bb_byte_width";
    submodule_data->defines[dnx_data_mdb_kaps_define_bb_byte_width].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_bb_byte_width].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_rpb_byte_width].name = "rpb_byte_width";
    submodule_data->defines[dnx_data_mdb_kaps_define_rpb_byte_width].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_rpb_byte_width].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_ads_byte_width].name = "ads_byte_width";
    submodule_data->defines[dnx_data_mdb_kaps_define_ads_byte_width].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_ads_byte_width].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_direct_byte_width].name = "direct_byte_width";
    submodule_data->defines[dnx_data_mdb_kaps_define_direct_byte_width].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_direct_byte_width].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_rpb_valid_bits].name = "rpb_valid_bits";
    submodule_data->defines[dnx_data_mdb_kaps_define_rpb_valid_bits].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_rpb_valid_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_dynamic_memory_access_dpc].name = "dynamic_memory_access_dpc";
    submodule_data->defines[dnx_data_mdb_kaps_define_dynamic_memory_access_dpc].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_dynamic_memory_access_dpc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_max_fmt].name = "max_fmt";
    submodule_data->defines[dnx_data_mdb_kaps_define_max_fmt].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_max_fmt].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_nof_app_ids].name = "nof_app_ids";
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_app_ids].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_app_ids].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_max_nof_result_types].name = "max_nof_result_types";
    submodule_data->defines[dnx_data_mdb_kaps_define_max_nof_result_types].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_max_nof_result_types].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_rpb_field].name = "rpb_field";
    submodule_data->defines[dnx_data_mdb_kaps_define_rpb_field].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_rpb_field].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_kaps_define_big_kaps_revision_val].name = "big_kaps_revision_val";
    submodule_data->defines[dnx_data_mdb_kaps_define_big_kaps_revision_val].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_big_kaps_revision_val].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_kaps_define_small_kaps_ad_size_e].name = "small_kaps_ad_size_e";
    submodule_data->defines[dnx_data_mdb_kaps_define_small_kaps_ad_size_e].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_small_kaps_ad_size_e].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_mdb_kaps_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data mdb kaps tables");

    
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].name = "big_bb_blk_id_mapping";
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].size_of_values = sizeof(dnx_data_mdb_kaps_big_bb_blk_id_mapping_t);
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].entry_get = dnx_data_mdb_kaps_big_bb_blk_id_mapping_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].nof_keys = 2;
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].keys[0].name = "pair_id";
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].keys[0].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].keys[1].name = "blk_id";
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].nof_values, "_dnx_data_mdb_kaps_table_big_bb_blk_id_mapping table values");
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].values[0].name = "global_macro_index";
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].values[0].type = "uint32";
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_big_bb_blk_id_mapping_t, global_macro_index);

    
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].name = "db_info";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].size_of_values = sizeof(dnx_data_mdb_kaps_db_info_t);
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].entry_get = dnx_data_mdb_kaps_db_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].keys[0].name = "db_idx";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].nof_values = 12;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_kaps_table_db_info].nof_values, "_dnx_data_mdb_kaps_table_db_info table values");
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[0].type = "dbal_physical_tables_e";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_db_info_t, dbal_id);
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[1].name = "mdb_bb_id";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[1].type = "mdb_physical_table_e";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_db_info_t, mdb_bb_id);
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[2].name = "mdb_ads_id";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[2].type = "mdb_physical_table_e";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_db_info_t, mdb_ads_id);
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[3].name = "mdb_direct_id";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[3].type = "mdb_physical_table_e";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_db_info_t, mdb_direct_id);
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[4].name = "name";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[4].type = "char *";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[4].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[4].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_db_info_t, name);
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[5].name = "dbal_table_db_atr";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[5].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[5].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[5].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_db_info_t, dbal_table_db_atr);
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[6].name = "nof_rows_in_rpb";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[6].type = "int";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[6].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[6].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_db_info_t, nof_rows_in_rpb);
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[7].name = "nof_rows_in_small_ads";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[7].type = "int";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[7].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[7].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_db_info_t, nof_rows_in_small_ads);
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[8].name = "nof_rows_in_small_bb";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[8].type = "int";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[8].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[8].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_db_info_t, nof_rows_in_small_bb);
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[9].name = "nof_rows_in_small_rpb_hitbits";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[9].type = "int";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[9].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[9].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_db_info_t, nof_rows_in_small_rpb_hitbits);
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[10].name = "nof_connected_big_bbs";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[10].type = "int";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[10].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[10].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_db_info_t, nof_connected_big_bbs);
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[11].name = "nof_result_types";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[11].type = "int";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[11].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_info].values[11].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_db_info_t, nof_result_types);

    
    submodule_data->tables[dnx_data_mdb_kaps_table_db_supported_ad_sizes].name = "db_supported_ad_sizes";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_supported_ad_sizes].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_supported_ad_sizes].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_kaps_table_db_supported_ad_sizes].size_of_values = sizeof(dnx_data_mdb_kaps_db_supported_ad_sizes_t);
    submodule_data->tables[dnx_data_mdb_kaps_table_db_supported_ad_sizes].entry_get = dnx_data_mdb_kaps_db_supported_ad_sizes_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_kaps_table_db_supported_ad_sizes].nof_keys = 2;
    submodule_data->tables[dnx_data_mdb_kaps_table_db_supported_ad_sizes].keys[0].name = "db_idx";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_supported_ad_sizes].keys[0].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_supported_ad_sizes].keys[1].name = "ad_size";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_supported_ad_sizes].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_mdb_kaps_table_db_supported_ad_sizes].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_kaps_table_db_supported_ad_sizes].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_kaps_table_db_supported_ad_sizes].nof_values, "_dnx_data_mdb_kaps_table_db_supported_ad_sizes table values");
    submodule_data->tables[dnx_data_mdb_kaps_table_db_supported_ad_sizes].values[0].name = "is_supported";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_supported_ad_sizes].values[0].type = "uint8";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_supported_ad_sizes].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_supported_ad_sizes].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_db_supported_ad_sizes_t, is_supported);

    
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].name = "db_access_info";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].size_of_values = sizeof(dnx_data_mdb_kaps_db_access_info_t);
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].entry_get = dnx_data_mdb_kaps_db_access_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].keys[0].name = "db_idx";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].nof_values = 8;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].nof_values, "_dnx_data_mdb_kaps_table_db_access_info table values");
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[0].name = "rpb_global_cfg_reg";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[0].type = "uint32";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_db_access_info_t, rpb_global_cfg_reg);
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[1].name = "tcam_scan_cfg_reg";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_db_access_info_t, tcam_scan_cfg_reg);
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[2].name = "bb_global_cfg_reg";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_db_access_info_t, bb_global_cfg_reg);
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[3].name = "bb_global_cfg_rpb_sel_field";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[3].type = "uint32";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_db_access_info_t, bb_global_cfg_rpb_sel_field);
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[4].name = "tcam_cmd_mem";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[4].type = "uint32";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[4].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[4].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_db_access_info_t, tcam_cmd_mem);
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[5].name = "small_ads_mem";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[5].type = "uint32";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[5].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[5].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_db_access_info_t, small_ads_mem);
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[6].name = "small_bb_mem";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[6].type = "uint32";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[6].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[6].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_db_access_info_t, small_bb_mem);
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[7].name = "search_cmd_mem";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[7].type = "uint32";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[7].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_db_access_info].values[7].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_db_access_info_t, search_cmd_mem);

    
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_28_info].name = "mdb_28_info";
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_28_info].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_28_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_28_info].size_of_values = sizeof(dnx_data_mdb_kaps_mdb_28_info_t);
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_28_info].entry_get = dnx_data_mdb_kaps_mdb_28_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_28_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_28_info].keys[0].name = "mdb_table";
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_28_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_28_info].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_kaps_table_mdb_28_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_kaps_table_mdb_28_info].nof_values, "_dnx_data_mdb_kaps_table_mdb_28_info table values");
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_28_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_28_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_28_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_28_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_mdb_28_info_t, dbal_table);

    
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_53_info].name = "mdb_53_info";
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_53_info].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_53_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_53_info].size_of_values = sizeof(dnx_data_mdb_kaps_mdb_53_info_t);
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_53_info].entry_get = dnx_data_mdb_kaps_mdb_53_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_53_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_53_info].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_53_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_53_info].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_kaps_table_mdb_53_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_kaps_table_mdb_53_info].nof_values, "_dnx_data_mdb_kaps_table_mdb_53_info table values");
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_53_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_53_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_53_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_mdb_53_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_mdb_53_info_t, dbal_table);

    
    submodule_data->tables[dnx_data_mdb_kaps_table_kaps_lookup_result].name = "kaps_lookup_result";
    submodule_data->tables[dnx_data_mdb_kaps_table_kaps_lookup_result].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_kaps_lookup_result].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_kaps_table_kaps_lookup_result].size_of_values = sizeof(dnx_data_mdb_kaps_kaps_lookup_result_t);
    submodule_data->tables[dnx_data_mdb_kaps_table_kaps_lookup_result].entry_get = dnx_data_mdb_kaps_kaps_lookup_result_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_kaps_table_kaps_lookup_result].nof_keys = 2;
    submodule_data->tables[dnx_data_mdb_kaps_table_kaps_lookup_result].keys[0].name = "db_idx";
    submodule_data->tables[dnx_data_mdb_kaps_table_kaps_lookup_result].keys[0].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_kaps_lookup_result].keys[1].name = "core";
    submodule_data->tables[dnx_data_mdb_kaps_table_kaps_lookup_result].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_mdb_kaps_table_kaps_lookup_result].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_kaps_table_kaps_lookup_result].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_kaps_table_kaps_lookup_result].nof_values, "_dnx_data_mdb_kaps_table_kaps_lookup_result table values");
    submodule_data->tables[dnx_data_mdb_kaps_table_kaps_lookup_result].values[0].name = "lookup_result";
    submodule_data->tables[dnx_data_mdb_kaps_table_kaps_lookup_result].values[0].type = "uint32";
    submodule_data->tables[dnx_data_mdb_kaps_table_kaps_lookup_result].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_kaps_lookup_result].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_kaps_lookup_result_t, lookup_result);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_mdb_kaps_feature_get(
    int unit,
    dnx_data_mdb_kaps_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, feature);
}


uint32
dnx_data_mdb_kaps_nof_dbs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_nof_dbs);
}

uint32
dnx_data_mdb_kaps_nof_small_bbs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_nof_small_bbs);
}

uint32
dnx_data_mdb_kaps_nof_rows_in_rpb_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_nof_rows_in_rpb_max);
}

uint32
dnx_data_mdb_kaps_nof_rows_in_small_ads_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_nof_rows_in_small_ads_max);
}

uint32
dnx_data_mdb_kaps_nof_rows_in_small_bb_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_nof_rows_in_small_bb_max);
}

uint32
dnx_data_mdb_kaps_nof_rows_in_small_rpb_hitbits_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_nof_rows_in_small_rpb_hitbits_max);
}

uint32
dnx_data_mdb_kaps_nof_big_bbs_blk_ids_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_nof_big_bbs_blk_ids);
}

uint32
dnx_data_mdb_kaps_nof_bytes_in_hitbit_row_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_nof_bytes_in_hitbit_row);
}

uint32
dnx_data_mdb_kaps_key_lsn_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_key_lsn_bits);
}

uint32
dnx_data_mdb_kaps_key_prefix_length_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_key_prefix_length);
}

uint32
dnx_data_mdb_kaps_max_prefix_in_bb_row_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_max_prefix_in_bb_row);
}

uint32
dnx_data_mdb_kaps_key_width_in_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_key_width_in_bits);
}

uint32
dnx_data_mdb_kaps_bb_byte_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_bb_byte_width);
}

uint32
dnx_data_mdb_kaps_rpb_byte_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_rpb_byte_width);
}

uint32
dnx_data_mdb_kaps_ads_byte_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_ads_byte_width);
}

uint32
dnx_data_mdb_kaps_direct_byte_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_direct_byte_width);
}

uint32
dnx_data_mdb_kaps_rpb_valid_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_rpb_valid_bits);
}

uint32
dnx_data_mdb_kaps_dynamic_memory_access_dpc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_dynamic_memory_access_dpc);
}

uint32
dnx_data_mdb_kaps_max_fmt_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_max_fmt);
}

uint32
dnx_data_mdb_kaps_nof_app_ids_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_nof_app_ids);
}

uint32
dnx_data_mdb_kaps_max_nof_result_types_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_max_nof_result_types);
}

uint32
dnx_data_mdb_kaps_rpb_field_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_rpb_field);
}

uint32
dnx_data_mdb_kaps_big_kaps_revision_val_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_big_kaps_revision_val);
}

uint32
dnx_data_mdb_kaps_small_kaps_ad_size_e_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_small_kaps_ad_size_e);
}



const dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *
dnx_data_mdb_kaps_big_bb_blk_id_mapping_get(
    int unit,
    int pair_id,
    int blk_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_big_bb_blk_id_mapping);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, pair_id, blk_id);
    return (const dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) data;

}

const dnx_data_mdb_kaps_db_info_t *
dnx_data_mdb_kaps_db_info_get(
    int unit,
    int db_idx)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_db_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, db_idx, 0);
    return (const dnx_data_mdb_kaps_db_info_t *) data;

}

const dnx_data_mdb_kaps_db_supported_ad_sizes_t *
dnx_data_mdb_kaps_db_supported_ad_sizes_get(
    int unit,
    int db_idx,
    int ad_size)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_db_supported_ad_sizes);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, db_idx, ad_size);
    return (const dnx_data_mdb_kaps_db_supported_ad_sizes_t *) data;

}

const dnx_data_mdb_kaps_db_access_info_t *
dnx_data_mdb_kaps_db_access_info_get(
    int unit,
    int db_idx)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_db_access_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, db_idx, 0);
    return (const dnx_data_mdb_kaps_db_access_info_t *) data;

}

const dnx_data_mdb_kaps_mdb_28_info_t *
dnx_data_mdb_kaps_mdb_28_info_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_mdb_28_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_kaps_mdb_28_info_t *) data;

}

const dnx_data_mdb_kaps_mdb_53_info_t *
dnx_data_mdb_kaps_mdb_53_info_get(
    int unit,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_mdb_53_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_id, 0);
    return (const dnx_data_mdb_kaps_mdb_53_info_t *) data;

}

const dnx_data_mdb_kaps_kaps_lookup_result_t *
dnx_data_mdb_kaps_kaps_lookup_result_get(
    int unit,
    int db_idx,
    int core)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_kaps_lookup_result);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, db_idx, core);
    return (const dnx_data_mdb_kaps_kaps_lookup_result_t *) data;

}


shr_error_e
dnx_data_mdb_kaps_big_bb_blk_id_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_big_bb_blk_id_mapping);
    data = (const dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->global_macro_index);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_kaps_db_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_kaps_db_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_db_info);
    data = (const dnx_data_mdb_kaps_db_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_id);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_bb_id);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_ads_id);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_direct_id);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->name == NULL ? "" : data->name);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table_db_atr);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_rows_in_rpb);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_rows_in_small_ads);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_rows_in_small_bb);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_rows_in_small_rpb_hitbits);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_connected_big_bbs);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_result_types);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_kaps_db_supported_ad_sizes_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_kaps_db_supported_ad_sizes_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_db_supported_ad_sizes);
    data = (const dnx_data_mdb_kaps_db_supported_ad_sizes_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_supported);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_kaps_db_access_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_kaps_db_access_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_db_access_info);
    data = (const dnx_data_mdb_kaps_db_access_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->rpb_global_cfg_reg);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tcam_scan_cfg_reg);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->bb_global_cfg_reg);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->bb_global_cfg_rpb_sel_field);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tcam_cmd_mem);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->small_ads_mem);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->small_bb_mem);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->search_cmd_mem);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_kaps_mdb_28_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_kaps_mdb_28_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_mdb_28_info);
    data = (const dnx_data_mdb_kaps_mdb_28_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_kaps_mdb_53_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_kaps_mdb_53_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_mdb_53_info);
    data = (const dnx_data_mdb_kaps_mdb_53_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_kaps_kaps_lookup_result_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_kaps_kaps_lookup_result_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_kaps_lookup_result);
    data = (const dnx_data_mdb_kaps_kaps_lookup_result_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->lookup_result);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_mdb_kaps_big_bb_blk_id_mapping_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_big_bb_blk_id_mapping);

}

const dnxc_data_table_info_t *
dnx_data_mdb_kaps_db_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_db_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_kaps_db_supported_ad_sizes_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_db_supported_ad_sizes);

}

const dnxc_data_table_info_t *
dnx_data_mdb_kaps_db_access_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_db_access_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_kaps_mdb_28_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_mdb_28_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_kaps_mdb_53_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_mdb_53_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_kaps_kaps_lookup_result_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_kaps_lookup_result);

}






static shr_error_e
dnx_data_mdb_feature_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "feature";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_mdb_feature_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data mdb feature features");

    submodule_data->features[dnx_data_mdb_feature_step_table_supports_mix_ar_algo].name = "step_table_supports_mix_ar_algo";
    submodule_data->features[dnx_data_mdb_feature_step_table_supports_mix_ar_algo].doc = "";
    submodule_data->features[dnx_data_mdb_feature_step_table_supports_mix_ar_algo].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_feature_eedb_bank_traffic_lock].name = "eedb_bank_traffic_lock";
    submodule_data->features[dnx_data_mdb_feature_eedb_bank_traffic_lock].doc = "";
    submodule_data->features[dnx_data_mdb_feature_eedb_bank_traffic_lock].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_feature_em_dfg_ovf_cam_disabled].name = "em_dfg_ovf_cam_disabled";
    submodule_data->features[dnx_data_mdb_feature_em_dfg_ovf_cam_disabled].doc = "";
    submodule_data->features[dnx_data_mdb_feature_em_dfg_ovf_cam_disabled].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_feature_em_mact_transplant_no_reply].name = "em_mact_transplant_no_reply";
    submodule_data->features[dnx_data_mdb_feature_em_mact_transplant_no_reply].doc = "";
    submodule_data->features[dnx_data_mdb_feature_em_mact_transplant_no_reply].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_feature_em_mact_use_refresh_on_insert].name = "em_mact_use_refresh_on_insert";
    submodule_data->features[dnx_data_mdb_feature_em_mact_use_refresh_on_insert].doc = "";
    submodule_data->features[dnx_data_mdb_feature_em_mact_use_refresh_on_insert].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_feature_em_mact_delete_transplant_no_reply].name = "em_mact_delete_transplant_no_reply";
    submodule_data->features[dnx_data_mdb_feature_em_mact_delete_transplant_no_reply].doc = "";
    submodule_data->features[dnx_data_mdb_feature_em_mact_delete_transplant_no_reply].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_feature_em_mact_insert_flush_drop_on_exceed_limit].name = "em_mact_insert_flush_drop_on_exceed_limit";
    submodule_data->features[dnx_data_mdb_feature_em_mact_insert_flush_drop_on_exceed_limit].doc = "";
    submodule_data->features[dnx_data_mdb_feature_em_mact_insert_flush_drop_on_exceed_limit].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_feature_em_sbus_interface_shutdown].name = "em_sbus_interface_shutdown";
    submodule_data->features[dnx_data_mdb_feature_em_sbus_interface_shutdown].doc = "";
    submodule_data->features[dnx_data_mdb_feature_em_sbus_interface_shutdown].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_feature_em_ecc_simple_command_deadlock].name = "em_ecc_simple_command_deadlock";
    submodule_data->features[dnx_data_mdb_feature_em_ecc_simple_command_deadlock].doc = "";
    submodule_data->features[dnx_data_mdb_feature_em_ecc_simple_command_deadlock].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_feature_em_compare_init].name = "em_compare_init";
    submodule_data->features[dnx_data_mdb_feature_em_compare_init].doc = "";
    submodule_data->features[dnx_data_mdb_feature_em_compare_init].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_feature_bubble_pulse_width_too_short].name = "bubble_pulse_width_too_short";
    submodule_data->features[dnx_data_mdb_feature_bubble_pulse_width_too_short].doc = "";
    submodule_data->features[dnx_data_mdb_feature_bubble_pulse_width_too_short].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_feature_kaps_rpb_ecc_valid_shared].name = "kaps_rpb_ecc_valid_shared";
    submodule_data->features[dnx_data_mdb_feature_kaps_rpb_ecc_valid_shared].doc = "";
    submodule_data->features[dnx_data_mdb_feature_kaps_rpb_ecc_valid_shared].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_feature_kaps_rpb_bubble_not_synced].name = "kaps_rpb_bubble_not_synced";
    submodule_data->features[dnx_data_mdb_feature_kaps_rpb_bubble_not_synced].doc = "";
    submodule_data->features[dnx_data_mdb_feature_kaps_rpb_bubble_not_synced].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_mdb_feature_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data mdb feature defines");

    
    submodule_data->nof_tables = _dnx_data_mdb_feature_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data mdb feature tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_mdb_feature_feature_get(
    int unit,
    dnx_data_mdb_feature_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_feature, feature);
}








shr_error_e
dnx_data_mdb_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "mdb";
    module_data->nof_submodules = _dnx_data_mdb_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data mdb submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_mdb_global_init(unit, &module_data->submodules[dnx_data_mdb_submodule_global]));
    SHR_IF_ERR_EXIT(dnx_data_mdb_dh_init(unit, &module_data->submodules[dnx_data_mdb_submodule_dh]));
    SHR_IF_ERR_EXIT(dnx_data_mdb_pdbs_init(unit, &module_data->submodules[dnx_data_mdb_submodule_pdbs]));
    SHR_IF_ERR_EXIT(dnx_data_mdb_em_init(unit, &module_data->submodules[dnx_data_mdb_submodule_em]));
    SHR_IF_ERR_EXIT(dnx_data_mdb_direct_init(unit, &module_data->submodules[dnx_data_mdb_submodule_direct]));
    SHR_IF_ERR_EXIT(dnx_data_mdb_eedb_init(unit, &module_data->submodules[dnx_data_mdb_submodule_eedb]));
    SHR_IF_ERR_EXIT(dnx_data_mdb_kaps_init(unit, &module_data->submodules[dnx_data_mdb_submodule_kaps]));
    SHR_IF_ERR_EXIT(dnx_data_mdb_feature_init(unit, &module_data->submodules[dnx_data_mdb_submodule_feature]));
    
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_mdb_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b1_data_mdb_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_mdb_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a1_data_mdb_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_mdb_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_mdb_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b1_data_mdb_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_mdb_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_mdb_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_mdb_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_mdb_attach(unit));
    }
    else

#endif 
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

