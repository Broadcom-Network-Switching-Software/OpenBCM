

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MDBDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_mdb.h>



extern shr_error_e jr2_a0_data_mdb_attach(
    int unit);
extern shr_error_e jr2_b0_data_mdb_attach(
    int unit);
extern shr_error_e jr2_b1_data_mdb_attach(
    int unit);
extern shr_error_e j2c_a0_data_mdb_attach(
    int unit);
extern shr_error_e j2c_a1_data_mdb_attach(
    int unit);
extern shr_error_e q2a_a0_data_mdb_attach(
    int unit);
extern shr_error_e q2a_b1_data_mdb_attach(
    int unit);
extern shr_error_e j2p_a0_data_mdb_attach(
    int unit);



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

    submodule_data->features[dnx_data_mdb_global_adapter_use_stub].name = "adapter_use_stub";
    submodule_data->features[dnx_data_mdb_global_adapter_use_stub].doc = "";
    submodule_data->features[dnx_data_mdb_global_adapter_use_stub].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_global_hitbit_support].name = "hitbit_support";
    submodule_data->features[dnx_data_mdb_global_hitbit_support].doc = "";
    submodule_data->features[dnx_data_mdb_global_hitbit_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mdb_global_capacity_support].name = "capacity_support";
    submodule_data->features[dnx_data_mdb_global_capacity_support].doc = "";
    submodule_data->features[dnx_data_mdb_global_capacity_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_mdb_global_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data mdb global defines");

    
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
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[3].name = "row_width";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[3].type = "uint32";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_dh_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_dh_info_t, row_width);
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

    
    submodule_data->tables[dnx_data_mdb_dh_table_macro_interface_mapping].name = "macro_interface_mapping";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_interface_mapping].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_interface_mapping].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_dh_table_macro_interface_mapping].size_of_values = sizeof(dnx_data_mdb_dh_macro_interface_mapping_t);
    submodule_data->tables[dnx_data_mdb_dh_table_macro_interface_mapping].entry_get = dnx_data_mdb_dh_macro_interface_mapping_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_dh_table_macro_interface_mapping].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_dh_table_macro_interface_mapping].keys[0].name = "global_macro_index";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_interface_mapping].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_dh_table_macro_interface_mapping].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_dh_table_macro_interface_mapping].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_dh_table_macro_interface_mapping].nof_values, "_dnx_data_mdb_dh_table_macro_interface_mapping table values");
    submodule_data->tables[dnx_data_mdb_dh_table_macro_interface_mapping].values[0].name = "interfaces";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_interface_mapping].values[0].type = "dbal_enum_value_field_mdb_physical_table_e[DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES]";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_interface_mapping].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_macro_interface_mapping].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_macro_interface_mapping_t, interfaces);

    
    submodule_data->tables[dnx_data_mdb_dh_table_table_way_to_macro_mapping].name = "table_way_to_macro_mapping";
    submodule_data->tables[dnx_data_mdb_dh_table_table_way_to_macro_mapping].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_table_way_to_macro_mapping].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_dh_table_table_way_to_macro_mapping].size_of_values = sizeof(dnx_data_mdb_dh_table_way_to_macro_mapping_t);
    submodule_data->tables[dnx_data_mdb_dh_table_table_way_to_macro_mapping].entry_get = dnx_data_mdb_dh_table_way_to_macro_mapping_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_dh_table_table_way_to_macro_mapping].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_dh_table_table_way_to_macro_mapping].keys[0].name = "mdb_table_id";
    submodule_data->tables[dnx_data_mdb_dh_table_table_way_to_macro_mapping].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_dh_table_table_way_to_macro_mapping].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_dh_table_table_way_to_macro_mapping].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_dh_table_table_way_to_macro_mapping].nof_values, "_dnx_data_mdb_dh_table_table_way_to_macro_mapping table values");
    submodule_data->tables[dnx_data_mdb_dh_table_table_way_to_macro_mapping].values[0].name = "table_way_to_macro_mapping";
    submodule_data->tables[dnx_data_mdb_dh_table_table_way_to_macro_mapping].values[0].type = "uint32[DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS]";
    submodule_data->tables[dnx_data_mdb_dh_table_table_way_to_macro_mapping].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_table_way_to_macro_mapping].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_table_way_to_macro_mapping_t, table_way_to_macro_mapping);

    
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

    
    submodule_data->tables[dnx_data_mdb_dh_table_macro_type_info].nof_values = 5;
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
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[0].type = "soc_mem_t";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_entry_banks_info_t, entry_bank);
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[1].name = "overflow_reg";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[1].type = "soc_reg_t";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_entry_banks_info_t, overflow_reg);
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[2].name = "abk_bank_a";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[2].type = "soc_mem_t";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_entry_banks_info_t, abk_bank_a);
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[3].name = "abk_bank_b";
    submodule_data->tables[dnx_data_mdb_dh_table_entry_banks_info].values[3].type = "soc_mem_t";
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
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[1].name = "mdb_item_2_field_size";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_mdb_1_info_t, mdb_item_2_field_size);
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[2].name = "mdb_item_3_array_size";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_mdb_1_info_t, mdb_item_3_array_size);
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[3].name = "mdb_item_4_field_size";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[3].type = "uint32";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_dh_table_mdb_1_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_dh_mdb_1_info_t, mdb_item_4_field_size);

    
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

const dnx_data_mdb_dh_macro_interface_mapping_t *
dnx_data_mdb_dh_macro_interface_mapping_get(
    int unit,
    int global_macro_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_macro_interface_mapping);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, global_macro_index, 0);
    return (const dnx_data_mdb_dh_macro_interface_mapping_t *) data;

}

const dnx_data_mdb_dh_table_way_to_macro_mapping_t *
dnx_data_mdb_dh_table_way_to_macro_mapping_get(
    int unit,
    int mdb_table_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_table_way_to_macro_mapping);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id, 0);
    return (const dnx_data_mdb_dh_table_way_to_macro_mapping_t *) data;

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
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->row_width);
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
dnx_data_mdb_dh_macro_interface_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_dh_macro_interface_mapping_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_macro_interface_mapping);
    data = (const dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES, data->interfaces);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_dh_table_way_to_macro_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_dh_table_way_to_macro_mapping_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_table_way_to_macro_mapping);
    data = (const dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, data->table_way_to_macro_mapping);
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
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_2_field_size);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_3_array_size);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_item_4_field_size);
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


const dnxc_data_table_info_t *
dnx_data_mdb_dh_dh_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_dh_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_dh_macro_interface_mapping_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_macro_interface_mapping);

}

const dnxc_data_table_info_t *
dnx_data_mdb_dh_table_way_to_macro_mapping_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_dh, dnx_data_mdb_dh_table_table_way_to_macro_mapping);

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

    submodule_data->defines[dnx_data_mdb_pdbs_define_kaps_nof_blocks].name = "kaps_nof_blocks";
    submodule_data->defines[dnx_data_mdb_pdbs_define_kaps_nof_blocks].doc = "";
    
    submodule_data->defines[dnx_data_mdb_pdbs_define_kaps_nof_blocks].flags |= DNXC_DATA_F_DEFINE;

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

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].name = "pdbs_info";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].size_of_values = sizeof(dnx_data_mdb_pdbs_pdbs_info_t);
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].entry_get = dnx_data_mdb_pdbs_pdbs_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].keys[0].name = "mdb_table_id";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].nof_values, "_dnx_data_mdb_pdbs_table_pdbs_info table values");
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].values[0].name = "db_type";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].values[0].type = "dbal_enum_value_field_mdb_db_type_e";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_pdbs_info_t, db_type);
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].values[1].name = "row_width";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_pdbs_info_t, row_width);
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].values[2].name = "direct_payload_type";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].values[2].type = "dbal_enum_value_field_direct_payload_sizes_e";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_pdbs_info_t, direct_payload_type);
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].values[3].name = "direct_max_payload_type";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].values[3].type = "dbal_enum_value_field_direct_payload_sizes_e";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdbs_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_pdbs_info_t, direct_max_payload_type);

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].name = "pdb_info";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].size_of_values = sizeof(dnx_data_mdb_pdbs_pdb_info_t);
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].entry_get = dnx_data_mdb_pdbs_pdb_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].nof_values = 5;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].nof_values, "_dnx_data_mdb_pdbs_table_pdb_info table values");
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].values[0].name = "logical_to_physical";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].values[0].type = "dbal_enum_value_field_mdb_physical_table_e";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_pdb_info_t, logical_to_physical);
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].values[1].name = "db_type";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].values[1].type = "dbal_enum_value_field_mdb_db_type_e";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_pdb_info_t, db_type);
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].values[2].name = "row_width";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_pdb_info_t, row_width);
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].values[3].name = "max_key_size";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].values[3].type = "uint32";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_pdb_info_t, max_key_size);
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].values[4].name = "max_payload_size";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].values[4].type = "uint32";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].values[4].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_pdb_info].values[4].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_pdb_info_t, max_payload_size);

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].name = "mdb_11_info";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].size_of_values = sizeof(dnx_data_mdb_pdbs_mdb_11_info_t);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].entry_get = dnx_data_mdb_pdbs_mdb_11_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].keys[0].name = "mdb_table";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].nof_values = 8;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].nof_values, "_dnx_data_mdb_pdbs_table_mdb_11_info table values");
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_11_info_t, dbal_table);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[1].name = "mdb_item_0_array_size";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_11_info_t, mdb_item_0_array_size);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[2].name = "mdb_item_0_field_size";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_11_info_t, mdb_item_0_field_size);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[3].name = "mdb_item_1_field_size";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[3].type = "uint32";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_11_info_t, mdb_item_1_field_size);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[4].name = "mdb_item_3_array_size";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[4].type = "uint32";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[4].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[4].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_11_info_t, mdb_item_3_array_size);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[5].name = "mdb_item_3_field_size";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[5].type = "uint32";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[5].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[5].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_11_info_t, mdb_item_3_field_size);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[6].name = "mdb_item_4_array_size";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[6].type = "uint32";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[6].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[6].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_11_info_t, mdb_item_4_array_size);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[7].name = "mdb_item_4_field_size";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[7].type = "uint32";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[7].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_11_info].values[7].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_11_info_t, mdb_item_4_field_size);

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_12_info].name = "mdb_12_info";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_12_info].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_12_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_12_info].size_of_values = sizeof(dnx_data_mdb_pdbs_mdb_12_info_t);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_12_info].entry_get = dnx_data_mdb_pdbs_mdb_12_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_12_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_12_info].keys[0].name = "mdb_table";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_12_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_12_info].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_12_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_12_info].nof_values, "_dnx_data_mdb_pdbs_table_mdb_12_info table values");
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_12_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_12_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_12_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_12_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_12_info_t, dbal_table);

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].name = "mdb_26_info";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].size_of_values = sizeof(dnx_data_mdb_pdbs_mdb_26_info_t);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].entry_get = dnx_data_mdb_pdbs_mdb_26_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].keys[0].name = "mdb_table";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].nof_values, "_dnx_data_mdb_pdbs_table_mdb_26_info table values");
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_26_info_t, dbal_table);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].values[1].name = "mdb_item_0_field_size";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_26_info_t, mdb_item_0_field_size);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].values[2].name = "mdb_item_1_field_size";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_26_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_26_info_t, mdb_item_1_field_size);

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].name = "mdb_adapter_mapping";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].size_of_values = sizeof(dnx_data_mdb_pdbs_mdb_adapter_mapping_t);
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].entry_get = dnx_data_mdb_pdbs_mdb_adapter_mapping_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].keys[0].name = "dbal_id";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].nof_values, "_dnx_data_mdb_pdbs_table_mdb_adapter_mapping table values");
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].values[0].name = "memory_id";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].values[0].type = "uint8";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_pdbs_table_mdb_adapter_mapping].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_pdbs_mdb_adapter_mapping_t, memory_id);


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
dnx_data_mdb_pdbs_kaps_nof_blocks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_define_kaps_nof_blocks);
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

const dnx_data_mdb_pdbs_pdbs_info_t *
dnx_data_mdb_pdbs_pdbs_info_get(
    int unit,
    int mdb_table_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_pdbs_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id, 0);
    return (const dnx_data_mdb_pdbs_pdbs_info_t *) data;

}

const dnx_data_mdb_pdbs_pdb_info_t *
dnx_data_mdb_pdbs_pdb_info_get(
    int unit,
    int dbal_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_pdb_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_id, 0);
    return (const dnx_data_mdb_pdbs_pdb_info_t *) data;

}

const dnx_data_mdb_pdbs_mdb_11_info_t *
dnx_data_mdb_pdbs_mdb_11_info_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_11_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_pdbs_mdb_11_info_t *) data;

}

const dnx_data_mdb_pdbs_mdb_12_info_t *
dnx_data_mdb_pdbs_mdb_12_info_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_12_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_pdbs_mdb_12_info_t *) data;

}

const dnx_data_mdb_pdbs_mdb_26_info_t *
dnx_data_mdb_pdbs_mdb_26_info_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_26_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_pdbs_mdb_26_info_t *) data;

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
dnx_data_mdb_pdbs_pdbs_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_pdbs_pdbs_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_pdbs_info);
    data = (const dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->db_type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->row_width);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->direct_payload_type);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->direct_max_payload_type);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_pdbs_pdb_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_pdbs_pdb_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_pdb_info);
    data = (const dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->logical_to_physical);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->db_type);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->row_width);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max_key_size);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max_payload_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_pdbs_mdb_11_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_pdbs_mdb_11_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_11_info);
    data = (const dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
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
dnx_data_mdb_pdbs_mdb_12_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_pdbs_mdb_12_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_12_info);
    data = (const dnx_data_mdb_pdbs_mdb_12_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_pdbs_mdb_26_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_pdbs_mdb_26_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_26_info);
    data = (const dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
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
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->memory_id);
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
dnx_data_mdb_pdbs_pdbs_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_pdbs_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_pdbs_pdb_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_pdb_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_pdbs_mdb_11_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_11_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_pdbs_mdb_12_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_12_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_pdbs_mdb_26_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_26_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_pdbs_mdb_adapter_mapping_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_pdbs, dnx_data_mdb_pdbs_table_mdb_adapter_mapping);

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

    submodule_data->features[dnx_data_mdb_em_entry_type_parser].name = "entry_type_parser";
    submodule_data->features[dnx_data_mdb_em_entry_type_parser].doc = "";
    submodule_data->features[dnx_data_mdb_em_entry_type_parser].flags |= DNXC_DATA_F_FEATURE;

    
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

    submodule_data->defines[dnx_data_mdb_em_define_flush_max_supported_payload].name = "flush_max_supported_payload";
    submodule_data->defines[dnx_data_mdb_em_define_flush_max_supported_payload].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_flush_max_supported_payload].flags |= DNXC_DATA_F_DEFINE;

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

    submodule_data->defines[dnx_data_mdb_em_define_interrupt_register].name = "interrupt_register";
    submodule_data->defines[dnx_data_mdb_em_define_interrupt_register].doc = "";
    
    submodule_data->defines[dnx_data_mdb_em_define_interrupt_register].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_mdb_em_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data mdb em tables");

    
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

    
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].name = "em_aging_info";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].size_of_values = sizeof(dnx_data_mdb_em_em_aging_info_t);
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].entry_get = dnx_data_mdb_em_em_aging_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].keys[0].name = "emp_table";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].nof_values, "_dnx_data_mdb_em_table_em_aging_info table values");
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[0].name = "max_nof_age_entry_bits";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[0].type = "uint8";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_aging_info_t, max_nof_age_entry_bits);
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[1].name = "rbd_size";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[1].type = "uint8";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_aging_info_t, rbd_size);
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[2].name = "total_nof_aging_bits";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_aging_info_t, total_nof_aging_bits);

    
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].name = "em_aging_cfg";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].size_of_values = sizeof(dnx_data_mdb_em_em_aging_cfg_t);
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].entry_get = dnx_data_mdb_em_em_aging_cfg_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].nof_keys = 2;
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].keys[0].name = "profile";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].keys[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_aging_cfg].keys[1].name = "emp_table";
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

    
    submodule_data->tables[dnx_data_mdb_em_table_em_info].nof_values = 9;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_em_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_em_info].nof_values, "_dnx_data_mdb_em_table_em_info table values");
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[0].name = "tid_size";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[0].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_info_t, tid_size);
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[1].name = "em_interface";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[1].type = "soc_mem_t";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_info_t, em_interface);
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[2].name = "age_mem";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[2].type = "soc_mem_t";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_info_t, age_mem);
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[3].name = "age_ovf_cam_mem";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[3].type = "soc_mem_t";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_info_t, age_ovf_cam_mem);
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[4].name = "status_reg";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[4].type = "soc_reg_t";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[4].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[4].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_info_t, status_reg);
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[5].name = "emp_scan_status_field";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[5].type = "soc_field_t";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[5].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[5].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_info_t, emp_scan_status_field);
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[6].name = "interrupt_field";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[6].type = "soc_field_t";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[6].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[6].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_info_t, interrupt_field);
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[7].name = "step_table_size";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[7].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[7].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[7].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_info_t, step_table_size);
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[8].name = "ovf_cam_size";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[8].type = "uint32";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[8].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_em_info].values[8].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_em_info_t, ovf_cam_size);

    
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

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_13_info].name = "mdb_13_info";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_13_info].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_13_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_13_info].size_of_values = sizeof(dnx_data_mdb_em_mdb_13_info_t);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_13_info].entry_get = dnx_data_mdb_em_mdb_13_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_13_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_13_info].keys[0].name = "mdb_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_13_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_13_info].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_mdb_13_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_mdb_13_info].nof_values, "_dnx_data_mdb_em_table_mdb_13_info table values");
    submodule_data->tables[dnx_data_mdb_em_table_mdb_13_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_13_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_13_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_13_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_13_info_t, dbal_table);

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].name = "mdb_15_info";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].size_of_values = sizeof(dnx_data_mdb_em_mdb_15_info_t);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].entry_get = dnx_data_mdb_em_mdb_15_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_15_info].keys[0].name = "mdb_table";
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
    submodule_data->tables[dnx_data_mdb_em_table_mdb_16_info].keys[0].name = "mdb_table";
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

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_18_info].name = "mdb_18_info";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_18_info].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_18_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_18_info].size_of_values = sizeof(dnx_data_mdb_em_mdb_18_info_t);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_18_info].entry_get = dnx_data_mdb_em_mdb_18_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_18_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_18_info].keys[0].name = "mdb_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_18_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_18_info].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_mdb_18_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_mdb_18_info].nof_values, "_dnx_data_mdb_em_table_mdb_18_info table values");
    submodule_data->tables[dnx_data_mdb_em_table_mdb_18_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_18_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_18_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_18_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_18_info_t, dbal_table);

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].name = "mdb_21_info";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].size_of_values = sizeof(dnx_data_mdb_em_mdb_21_info_t);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].entry_get = dnx_data_mdb_em_mdb_21_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_21_info].keys[0].name = "mdb_table";
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
    submodule_data->tables[dnx_data_mdb_em_table_mdb_23_info].keys[0].name = "mdb_table";
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
    submodule_data->tables[dnx_data_mdb_em_table_mdb_24_info].keys[0].name = "mdb_table";
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
    submodule_data->tables[dnx_data_mdb_em_table_mdb_29_info].keys[0].name = "mdb_table";
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
    submodule_data->tables[dnx_data_mdb_em_table_mdb_31_info].keys[0].name = "mdb_table";
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
    submodule_data->tables[dnx_data_mdb_em_table_mdb_32_info].keys[0].name = "mdb_table";
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
    submodule_data->tables[dnx_data_mdb_em_table_mdb_41_info].keys[0].name = "mdb_table";
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
    submodule_data->tables[dnx_data_mdb_em_table_mdb_45_info].keys[0].name = "mdb_table";
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

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].name = "mdb_em_tables_info";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].size_of_values = sizeof(dnx_data_mdb_em_mdb_em_tables_info_t);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].entry_get = dnx_data_mdb_em_mdb_em_tables_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].keys[0].name = "mdb_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].nof_values = 5;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].nof_values, "_dnx_data_mdb_em_table_mdb_em_tables_info table values");
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].values[0].name = "mdb_14_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_em_tables_info_t, mdb_14_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].values[1].name = "mdb_15_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].values[1].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_em_tables_info_t, mdb_15_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].values[2].name = "mdb_17_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].values[2].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].values[2].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_em_tables_info_t, mdb_17_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].values[3].name = "mdb_step_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].values[3].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].values[3].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_em_tables_info_t, mdb_step_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].values[4].name = "mdb_32_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].values[4].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].values[4].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_em_tables_info].values[4].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_em_tables_info_t, mdb_32_table);

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_info].name = "mdb_emp_tables_info";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_info].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_info].size_of_values = sizeof(dnx_data_mdb_em_mdb_emp_tables_info_t);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_info].entry_get = dnx_data_mdb_em_mdb_emp_tables_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_info].keys[0].name = "mdb_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_info].nof_values, "_dnx_data_mdb_em_table_mdb_emp_tables_info table values");
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_info].values[0].name = "emp_age_cfg_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_emp_tables_info_t, emp_age_cfg_table);
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_info].values[1].name = "mdb_22_table";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_info].values[1].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_info].values[1].doc = "";
    submodule_data->tables[dnx_data_mdb_em_table_mdb_emp_tables_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_mdb_em_mdb_emp_tables_info_t, mdb_22_table);

    
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
dnx_data_mdb_em_flush_max_supported_payload_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_flush_max_supported_payload);
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
dnx_data_mdb_em_interrupt_register_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_define_interrupt_register);
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

const dnx_data_mdb_em_em_aging_info_t *
dnx_data_mdb_em_em_aging_info_get(
    int unit,
    int emp_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_em_aging_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, emp_table, 0);
    return (const dnx_data_mdb_em_em_aging_info_t *) data;

}

const dnx_data_mdb_em_em_aging_cfg_t *
dnx_data_mdb_em_em_aging_cfg_get(
    int unit,
    int profile,
    int emp_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_em_aging_cfg);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, profile, emp_table);
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

const dnx_data_mdb_em_mdb_13_info_t *
dnx_data_mdb_em_mdb_13_info_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_13_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_em_mdb_13_info_t *) data;

}

const dnx_data_mdb_em_mdb_15_info_t *
dnx_data_mdb_em_mdb_15_info_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_15_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_em_mdb_15_info_t *) data;

}

const dnx_data_mdb_em_mdb_16_info_t *
dnx_data_mdb_em_mdb_16_info_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_16_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_em_mdb_16_info_t *) data;

}

const dnx_data_mdb_em_mdb_18_info_t *
dnx_data_mdb_em_mdb_18_info_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_18_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_em_mdb_18_info_t *) data;

}

const dnx_data_mdb_em_mdb_21_info_t *
dnx_data_mdb_em_mdb_21_info_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_21_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_em_mdb_21_info_t *) data;

}

const dnx_data_mdb_em_mdb_23_info_t *
dnx_data_mdb_em_mdb_23_info_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_23_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_em_mdb_23_info_t *) data;

}

const dnx_data_mdb_em_mdb_24_info_t *
dnx_data_mdb_em_mdb_24_info_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_24_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_em_mdb_24_info_t *) data;

}

const dnx_data_mdb_em_mdb_29_info_t *
dnx_data_mdb_em_mdb_29_info_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_29_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_em_mdb_29_info_t *) data;

}

const dnx_data_mdb_em_mdb_31_info_t *
dnx_data_mdb_em_mdb_31_info_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_31_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_em_mdb_31_info_t *) data;

}

const dnx_data_mdb_em_mdb_32_info_t *
dnx_data_mdb_em_mdb_32_info_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_32_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_em_mdb_32_info_t *) data;

}

const dnx_data_mdb_em_mdb_41_info_t *
dnx_data_mdb_em_mdb_41_info_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_41_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_em_mdb_41_info_t *) data;

}

const dnx_data_mdb_em_mdb_45_info_t *
dnx_data_mdb_em_mdb_45_info_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_45_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_em_mdb_45_info_t *) data;

}

const dnx_data_mdb_em_mdb_em_tables_info_t *
dnx_data_mdb_em_mdb_em_tables_info_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_em_tables_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_em_mdb_em_tables_info_t *) data;

}

const dnx_data_mdb_em_mdb_emp_tables_info_t *
dnx_data_mdb_em_mdb_emp_tables_info_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_emp_tables_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_em_mdb_emp_tables_info_t *) data;

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
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->age_mem);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->age_ovf_cam_mem);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->status_reg);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->emp_scan_status_field);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->interrupt_field);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->step_table_size);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ovf_cam_size);
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
dnx_data_mdb_em_mdb_13_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_mdb_13_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_13_info);
    data = (const dnx_data_mdb_em_mdb_13_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
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
dnx_data_mdb_em_mdb_18_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_mdb_18_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_18_info);
    data = (const dnx_data_mdb_em_mdb_18_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
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
dnx_data_mdb_em_mdb_em_tables_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_mdb_em_tables_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_em_tables_info);
    data = (const dnx_data_mdb_em_mdb_em_tables_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_14_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_15_table);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_17_table);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_step_table);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb_32_table);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mdb_em_mdb_emp_tables_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_em_mdb_emp_tables_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_emp_tables_info);
    data = (const dnx_data_mdb_em_mdb_emp_tables_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
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
dnx_data_mdb_em_mdb_13_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_13_info);

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
dnx_data_mdb_em_mdb_18_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_18_info);

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
dnx_data_mdb_em_mdb_em_tables_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_em_tables_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_mdb_emp_tables_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_emp_tables_info);

}

const dnxc_data_table_info_t *
dnx_data_mdb_em_mdb_em_shift_vmv_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_em, dnx_data_mdb_em_table_mdb_em_shift_vmv_regs);

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

    submodule_data->defines[dnx_data_mdb_direct_define_map_payload_size_enum].name = "map_payload_size_enum";
    submodule_data->defines[dnx_data_mdb_direct_define_map_payload_size_enum].doc = "";
    
    submodule_data->defines[dnx_data_mdb_direct_define_map_payload_size_enum].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_direct_define_map_payload_type_enum].name = "map_payload_type_enum";
    submodule_data->defines[dnx_data_mdb_direct_define_map_payload_type_enum].doc = "";
    
    submodule_data->defines[dnx_data_mdb_direct_define_map_payload_type_enum].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_direct_define_map_payload_size].name = "map_payload_size";
    submodule_data->defines[dnx_data_mdb_direct_define_map_payload_size].doc = "";
    
    submodule_data->defines[dnx_data_mdb_direct_define_map_payload_size].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_mdb_direct_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data mdb direct tables");

    
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
    submodule_data->tables[dnx_data_mdb_direct_table_direct_info].values[0].type = "dbal_enum_value_field_mdb_direct_payload_e";
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

    
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_42_info].name = "mdb_42_info";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_42_info].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_42_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_42_info].size_of_values = sizeof(dnx_data_mdb_direct_mdb_42_info_t);
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_42_info].entry_get = dnx_data_mdb_direct_mdb_42_info_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_42_info].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_42_info].keys[0].name = "mdb_table";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_42_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_42_info].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_direct_table_mdb_42_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_direct_table_mdb_42_info].nof_values, "_dnx_data_mdb_direct_table_mdb_42_info table values");
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_42_info].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_42_info].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_42_info].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_direct_table_mdb_42_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_direct_mdb_42_info_t, dbal_table);


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
dnx_data_mdb_direct_map_payload_size_enum_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_define_map_payload_size_enum);
}

uint32
dnx_data_mdb_direct_map_payload_type_enum_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_define_map_payload_type_enum);
}

uint32
dnx_data_mdb_direct_map_payload_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_define_map_payload_size);
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

const dnx_data_mdb_direct_mdb_42_info_t *
dnx_data_mdb_direct_mdb_42_info_get(
    int unit,
    int mdb_table)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_mdb_42_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mdb_table, 0);
    return (const dnx_data_mdb_direct_mdb_42_info_t *) data;

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
dnx_data_mdb_direct_mdb_42_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mdb_direct_mdb_42_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_mdb_42_info);
    data = (const dnx_data_mdb_direct_mdb_42_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
    }

    SHR_FUNC_EXIT;
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
dnx_data_mdb_direct_mdb_42_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_direct, dnx_data_mdb_direct_table_mdb_42_info);

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

    
    submodule_data->nof_defines = _dnx_data_mdb_kaps_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data mdb kaps defines");

    submodule_data->defines[dnx_data_mdb_kaps_define_nof_small_bbs].name = "nof_small_bbs";
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_small_bbs].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_small_bbs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_nof_big_bbs_blk_ids].name = "nof_big_bbs_blk_ids";
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_big_bbs_blk_ids].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_big_bbs_blk_ids].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_rpb].name = "nof_rows_in_rpb";
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_rpb].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_rpb].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_small_ads].name = "nof_rows_in_small_ads";
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_small_ads].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_small_ads].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_small_bb].name = "nof_rows_in_small_bb";
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_small_bb].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_small_bb].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_small_rpb_hitbits].name = "nof_rows_in_small_rpb_hitbits";
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_small_rpb_hitbits].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_nof_rows_in_small_rpb_hitbits].flags |= DNXC_DATA_F_DEFINE;

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

    submodule_data->defines[dnx_data_mdb_kaps_define_ad_width_in_bits].name = "ad_width_in_bits";
    submodule_data->defines[dnx_data_mdb_kaps_define_ad_width_in_bits].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_ad_width_in_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_bb_byte_width].name = "bb_byte_width";
    submodule_data->defines[dnx_data_mdb_kaps_define_bb_byte_width].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_bb_byte_width].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_rpb_byte_width].name = "rpb_byte_width";
    submodule_data->defines[dnx_data_mdb_kaps_define_rpb_byte_width].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_rpb_byte_width].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_ads_byte_width].name = "ads_byte_width";
    submodule_data->defines[dnx_data_mdb_kaps_define_ads_byte_width].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_ads_byte_width].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_rpb_valid_bits].name = "rpb_valid_bits";
    submodule_data->defines[dnx_data_mdb_kaps_define_rpb_valid_bits].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_rpb_valid_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_dynamic_memory_access_dpc].name = "dynamic_memory_access_dpc";
    submodule_data->defines[dnx_data_mdb_kaps_define_dynamic_memory_access_dpc].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_dynamic_memory_access_dpc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mdb_kaps_define_rpb_field].name = "rpb_field";
    submodule_data->defines[dnx_data_mdb_kaps_define_rpb_field].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_rpb_field].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_kaps_define_result_core0_db0_reg].name = "result_core0_db0_reg";
    submodule_data->defines[dnx_data_mdb_kaps_define_result_core0_db0_reg].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_result_core0_db0_reg].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_kaps_define_result_core0_db1_reg].name = "result_core0_db1_reg";
    submodule_data->defines[dnx_data_mdb_kaps_define_result_core0_db1_reg].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_result_core0_db1_reg].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_kaps_define_result_core1_db0_reg].name = "result_core1_db0_reg";
    submodule_data->defines[dnx_data_mdb_kaps_define_result_core1_db0_reg].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_result_core1_db0_reg].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_kaps_define_result_core1_db1_reg].name = "result_core1_db1_reg";
    submodule_data->defines[dnx_data_mdb_kaps_define_result_core1_db1_reg].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_result_core1_db1_reg].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_kaps_define_big_kaps_revision_val].name = "big_kaps_revision_val";
    submodule_data->defines[dnx_data_mdb_kaps_define_big_kaps_revision_val].doc = "";
    
    submodule_data->defines[dnx_data_mdb_kaps_define_big_kaps_revision_val].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_mdb_kaps_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data mdb kaps tables");

    
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].name = "big_bb_blk_id_mapping";
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].size_of_values = sizeof(dnx_data_mdb_kaps_big_bb_blk_id_mapping_t);
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].entry_get = dnx_data_mdb_kaps_big_bb_blk_id_mapping_entry_str_get;

    
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].nof_keys = 1;
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].keys[0].name = "blk_id";
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].nof_values, "_dnx_data_mdb_kaps_table_big_bb_blk_id_mapping table values");
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].values[0].name = "global_macro_index";
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].values[0].type = "uint32";
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].values[0].doc = "";
    submodule_data->tables[dnx_data_mdb_kaps_table_big_bb_blk_id_mapping].values[0].offset = UTILEX_OFFSETOF(dnx_data_mdb_kaps_big_bb_blk_id_mapping_t, global_macro_index);


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
dnx_data_mdb_kaps_nof_small_bbs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_nof_small_bbs);
}

uint32
dnx_data_mdb_kaps_nof_big_bbs_blk_ids_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_nof_big_bbs_blk_ids);
}

uint32
dnx_data_mdb_kaps_nof_rows_in_rpb_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_nof_rows_in_rpb);
}

uint32
dnx_data_mdb_kaps_nof_rows_in_small_ads_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_nof_rows_in_small_ads);
}

uint32
dnx_data_mdb_kaps_nof_rows_in_small_bb_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_nof_rows_in_small_bb);
}

uint32
dnx_data_mdb_kaps_nof_rows_in_small_rpb_hitbits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_nof_rows_in_small_rpb_hitbits);
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
dnx_data_mdb_kaps_ad_width_in_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_ad_width_in_bits);
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
dnx_data_mdb_kaps_rpb_field_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_rpb_field);
}

uint32
dnx_data_mdb_kaps_result_core0_db0_reg_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_result_core0_db0_reg);
}

uint32
dnx_data_mdb_kaps_result_core0_db1_reg_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_result_core0_db1_reg);
}

uint32
dnx_data_mdb_kaps_result_core1_db0_reg_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_result_core1_db0_reg);
}

uint32
dnx_data_mdb_kaps_result_core1_db1_reg_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_result_core1_db1_reg);
}

uint32
dnx_data_mdb_kaps_big_kaps_revision_val_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_define_big_kaps_revision_val);
}



const dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *
dnx_data_mdb_kaps_big_bb_blk_id_mapping_get(
    int unit,
    int blk_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mdb, dnx_data_mdb_submodule_kaps, dnx_data_mdb_kaps_table_big_bb_blk_id_mapping);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, blk_id, 0);
    return (const dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) data;

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
    data = (const dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->global_macro_index);
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
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_mdb_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b1_data_mdb_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_mdb_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a1_data_mdb_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_mdb_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_mdb_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b1_data_mdb_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_mdb_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

