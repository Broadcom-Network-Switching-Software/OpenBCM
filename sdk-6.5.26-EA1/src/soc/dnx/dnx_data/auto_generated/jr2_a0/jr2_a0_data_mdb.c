
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







static shr_error_e
jr2_a0_dnx_data_mdb_global_hitbit_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_global;
    int feature_index = dnx_data_mdb_global_hitbit_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_global_capacity_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_global;
    int feature_index = dnx_data_mdb_global_capacity_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_global_init_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_global;
    int feature_index = dnx_data_mdb_global_init_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_global_block_id_fixed_mdb_value_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_global;
    int feature_index = dnx_data_mdb_global_block_id_fixed_mdb_value;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_global_lpm_acl_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_global;
    int feature_index = dnx_data_mdb_global_lpm_acl_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_mdb_global_utilization_api_nof_dbs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_global;
    int define_index = dnx_data_mdb_global_define_utilization_api_nof_dbs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 17;

    
    define->data = 17;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_global_adapter_stub_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_global;
    int define_index = dnx_data_mdb_global_define_adapter_stub_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "custom_feature";
    define->property.doc = 
        "\n"
        "Enable/Disable MDB logical stub mode\n"
        "Default: 0\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "adapter_use_stub";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_mdb_dh_macro_A_half_nof_clusters_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int feature_index = dnx_data_mdb_dh_macro_A_half_nof_clusters_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_macro_A_75_nof_clusters_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int feature_index = dnx_data_mdb_dh_macro_A_75_nof_clusters_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_bpu_setup_extended_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int feature_index = dnx_data_mdb_dh_bpu_setup_extended_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_mdb_dh_total_nof_macroes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_total_nof_macroes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20;

    
    define->data = 20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_total_nof_macroes_plus_em_ovf_or_eedb_bank_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_total_nof_macroes_plus_em_ovf_or_eedb_bank;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_mdb.dh.total_nof_macroes_get(unit) + 1;

    
    define->data = dnx_data_mdb.dh.total_nof_macroes_get(unit) + 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_total_nof_macroes_plus_data_width_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_total_nof_macroes_plus_data_width;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_mdb.dh.total_nof_macroes_get(unit) + 1;

    
    define->data = dnx_data_mdb.dh.total_nof_macroes_get(unit) + 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_max_nof_cluster_interfaces_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_max_nof_cluster_interfaces;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 9;

    
    define->data = 9;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_max_nof_clusters_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_max_nof_clusters;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_max_nof_db_clusters_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_max_nof_db_clusters;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 100;

    
    define->data = 100;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_nof_bucket_clusters_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_nof_bucket_clusters;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_nof_buckets_in_macro_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_nof_buckets_in_macro;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_nof_pair_clusters_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_nof_pair_clusters;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_nof_data_rows_per_hitbit_row_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_nof_data_rows_per_hitbit_row;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_nof_ddha_blocks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_nof_ddha_blocks;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_nof_ddhb_blocks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_nof_ddhb_blocks;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_nof_dhc_blocks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_nof_dhc_blocks;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_nof_ddhc_blocks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_nof_ddhc_blocks;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_nof_ddhx_blocks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_nof_ddhx_blocks;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_cluster_row_width_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_cluster_row_width_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 120;

    
    define->data = 120;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_cluster_row_width_uint32_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_cluster_row_width_uint32;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_data_out_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_data_out_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 30;

    
    define->data = 30;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_bpu_setup_bb_connected_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_bpu_setup_bb_connected;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_bpu_setup_size_120_240_120_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_bpu_setup_size_120_240_120;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_bpu_setup_size_480_odd_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_bpu_setup_size_480_odd;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_bpu_setup_size_480_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_bpu_setup_size_480;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_bpu_setup_size_360_120_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_bpu_setup_size_360_120;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_bpu_setup_size_120_360_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_bpu_setup_size_120_360;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_bpu_setup_size_240_240_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_bpu_setup_size_240_240;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_bpu_setup_size_120_120_240_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_bpu_setup_size_120_120_240;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_bpu_setup_size_240_120_120_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_bpu_setup_size_240_120_120;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_bpu_setup_size_120_120_120_120_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_bpu_setup_size_120_120_120_120;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_ddha_dynamic_memory_access_dpc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_ddha_dynamic_memory_access_dpc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_nof_fec_macro_types_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int define_index = dnx_data_mdb_dh_define_nof_fec_macro_types;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_mdb_dh_dh_info_set(
    int unit)
{
    int mdb_table_id_index;
    dnx_data_mdb_dh_dh_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int table_index = dnx_data_mdb_dh_table_dh_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    table->values[6].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_dh_dh_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_dh_table_dh_info");

    
    default_data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->table_macro_interface_mapping[0] = 0;
    default_data->cluster_if_offsets_values[0] = 0;
    default_data->if_cluster_offsets_values[0] = 0;
    default_data->macro_mapping[0] = 0;
    default_data->dh_in_width = 0;
    default_data->dh_out_width = 0;
    default_data->two_ways_connectivity_bm = 0;
    
    for (mdb_table_id_index = 0; mdb_table_id_index < table->keys[0].size; mdb_table_id_index++)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_1, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0x0;
        data->table_macro_interface_mapping[6] = 0x1;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0x2;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0x3;
        data->table_macro_interface_mapping[13] = 0x4;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0x5;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0x7;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0x8;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x05;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x01;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0x00;
        data->macro_mapping[6] = 0x00;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0x00;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0x00;
        data->macro_mapping[13] = 0x00;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0x00;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0x00;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 35;
        data->dh_out_width = 24;
        data->two_ways_connectivity_bm = 0x20;
    }
    if (MDB_PHYSICAL_TABLE_INLIF_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_INLIF_1, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0x0;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0x1;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0x2;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0x3;
        data->table_macro_interface_mapping[13] = 0x4;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0x5;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x05;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x05;
        data->cluster_if_offsets_values[13] = 0x05;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x01;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x01;
        data->if_cluster_offsets_values[13] = 0x01;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x06;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0x00;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0x00;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0x01;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0x01;
        data->macro_mapping[13] = 0x01;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0x00;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 180;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_IVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_IVSI, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0x0;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0x1;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x06;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x06;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x03;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0x01;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0x01;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 90;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_2, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0x0;
        data->table_macro_interface_mapping[6] = 0x1;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0x3;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0x4;
        data->table_macro_interface_mapping[14] = 0x5;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0x6;
        data->table_macro_interface_mapping[20] = 0x7;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x05;
        data->cluster_if_offsets_values[6] = 0x05;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x05;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x0a;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x01;
        data->if_cluster_offsets_values[6] = 0x01;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x07;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x01;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0x01;
        data->macro_mapping[6] = 0x01;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0x00;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0x02;
        data->macro_mapping[14] = 0x00;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0x00;
        data->dh_in_width = 75;
        data->dh_out_width = 24;
        data->two_ways_connectivity_bm = 0x2;
    }
    if (MDB_PHYSICAL_TABLE_INLIF_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_INLIF_2, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0x0;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x06;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0x00;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 180;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_3, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0x0;
        data->table_macro_interface_mapping[7] = 0x1;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0x2;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0x4;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0x5;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0x7;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x19;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x0a;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x05;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x0a;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x03;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x01;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x09;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x01;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x01;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0x03;
        data->macro_mapping[7] = 0x00;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0x01;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0x02;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0x01;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 75;
        data->dh_out_width = 24;
        data->two_ways_connectivity_bm = 0x24;
    }
    if (MDB_PHYSICAL_TABLE_INLIF_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_INLIF_3, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0x0;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0x1;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x06;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0x00;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0x00;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 180;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_LEM, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0x0;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0x1;
        data->table_macro_interface_mapping[3] = 0x2;
        data->table_macro_interface_mapping[4] = 0x3;
        data->table_macro_interface_mapping[5] = 0x4;
        data->table_macro_interface_mapping[6] = 0x5;
        data->table_macro_interface_mapping[7] = 0x6;
        data->table_macro_interface_mapping[8] = 0x7;
        data->table_macro_interface_mapping[9] = 0x8;
        data->table_macro_interface_mapping[10] = 0x9;
        data->table_macro_interface_mapping[11] = 0xA;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xB;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x0f;
        data->cluster_if_offsets_values[6] = 0x23;
        data->cluster_if_offsets_values[7] = 0x0a;
        data->cluster_if_offsets_values[8] = 0x05;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x1e;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x13;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x06;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x06;
        data->if_cluster_offsets_values[5] = 0x02;
        data->if_cluster_offsets_values[6] = 0x04;
        data->if_cluster_offsets_values[7] = 0x01;
        data->if_cluster_offsets_values[8] = 0x07;
        data->if_cluster_offsets_values[9] = 0x06;
        data->if_cluster_offsets_values[10] = 0x03;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x04;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0x00;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0x01;
        data->macro_mapping[3] = 0x00;
        data->macro_mapping[4] = 0x01;
        data->macro_mapping[5] = 0x02;
        data->macro_mapping[6] = 0x04;
        data->macro_mapping[7] = 0x01;
        data->macro_mapping[8] = 0x02;
        data->macro_mapping[9] = 0x01;
        data->macro_mapping[10] = 0x03;
        data->macro_mapping[11] = 0x00;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 147;
        data->dh_out_width = 120;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_ADS_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ADS_1, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0x0;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0x1;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x36;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x0a;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x08;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x01;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x04;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0x05;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0x01;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 120;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_ADS_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ADS_2, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0x0;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x13;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x04;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x04;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0x01;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 120;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0x0;
        data->table_macro_interface_mapping[2] = 0x1;
        data->table_macro_interface_mapping[3] = 0x2;
        data->table_macro_interface_mapping[4] = 0x3;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0x4;
        data->table_macro_interface_mapping[8] = 0x5;
        data->table_macro_interface_mapping[9] = 0x6;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0x7;
        data->table_macro_interface_mapping[12] = 0x8;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0x9;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xA;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x13;
        data->cluster_if_offsets_values[3] = 0x13;
        data->cluster_if_offsets_values[4] = 0x13;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x1d;
        data->cluster_if_offsets_values[8] = 0x18;
        data->cluster_if_offsets_values[9] = 0x13;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x13;
        data->cluster_if_offsets_values[12] = 0x05;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x0a;
        data->cluster_if_offsets_values[20] = 0x14;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x09;
        data->if_cluster_offsets_values[2] = 0x0a;
        data->if_cluster_offsets_values[3] = 0x04;
        data->if_cluster_offsets_values[4] = 0x0a;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x05;
        data->if_cluster_offsets_values[8] = 0x0b;
        data->if_cluster_offsets_values[9] = 0x0a;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x08;
        data->if_cluster_offsets_values[12] = 0x07;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x06;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x01;
        data->if_cluster_offsets_values[20] = 0x01;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0x02;
        data->macro_mapping[2] = 0x02;
        data->macro_mapping[3] = 0x01;
        data->macro_mapping[4] = 0x02;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0x02;
        data->macro_mapping[8] = 0x03;
        data->macro_mapping[9] = 0x02;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0x02;
        data->macro_mapping[12] = 0x02;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0x01;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0x01;
        data->dh_in_width = 160;
        data->dh_out_width = 30;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0x0;
        data->table_macro_interface_mapping[13] = 0x1;
        data->table_macro_interface_mapping[14] = 0x2;
        data->table_macro_interface_mapping[15] = 0x3;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0x4;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0x5;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x19;
        data->cluster_if_offsets_values[13] = 0x0f;
        data->cluster_if_offsets_values[14] = 0x0a;
        data->cluster_if_offsets_values[15] = 0x0a;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x14;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x1e;
        data->cluster_if_offsets_values[20] = 0x14;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x08;
        data->if_cluster_offsets_values[13] = 0x08;
        data->if_cluster_offsets_values[14] = 0x05;
        data->if_cluster_offsets_values[15] = 0x0a;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x07;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x02;
        data->if_cluster_offsets_values[20] = 0x01;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0x03;
        data->macro_mapping[13] = 0x03;
        data->macro_mapping[14] = 0x02;
        data->macro_mapping[15] = 0x03;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0x02;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0x02;
        data->dh_in_width = 160;
        data->dh_out_width = 30;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_IOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_IOEM_0, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0x0;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0x1;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0x2;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0x4;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x31;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x23;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x0a;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x03;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x07;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x09;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x02;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x02;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0x04;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0x04;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0x02;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 19;
        data->dh_out_width = 48;
        data->two_ways_connectivity_bm = 0x4;
    }
    if (MDB_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_IOEM_1, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0x0;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0x1;
        data->table_macro_interface_mapping[16] = 0x2;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0x3;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0x4;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x27;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x1e;
        data->cluster_if_offsets_values[16] = 0x10;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x19;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x03;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x05;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x0b;
        data->if_cluster_offsets_values[16] = 0x06;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x03;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x02;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0x02;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0x04;
        data->macro_mapping[16] = 0x04;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0x03;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 19;
        data->dh_out_width = 48;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_MAP < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_MAP, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0x0;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0x1;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x31;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x34;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x06;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x09;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x04;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0x03;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0x05;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 120;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_1, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0x0;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0x1;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0x2;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x13;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x34;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x32;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x04;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x0d;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x03;
        data->if_cluster_offsets_values[20] = 0x05;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0x01;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0x06;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0x03;
        data->dh_in_width = 0;
        data->dh_out_width = 150;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_2, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0x0;
        data->table_macro_interface_mapping[5] = 0x1;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0x2;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x27;
        data->cluster_if_offsets_values[5] = 0x22;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x32;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x0b;
        data->if_cluster_offsets_values[5] = 0x06;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x08;
        data->if_cluster_offsets_values[20] = 0x05;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0x03;
        data->macro_mapping[5] = 0x03;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0x04;
        data->dh_in_width = 0;
        data->dh_out_width = 150;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_3, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0x0;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0x1;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0x2;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0x3;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x2a;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x22;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x27;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x26;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x07;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x0b;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x09;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x0b;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x05;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0x03;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0x04;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0x03;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0x05;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 150;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_MC_ID < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_MC_ID, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0x0;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0x1;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0x2;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0x3;
        data->table_macro_interface_mapping[17] = 0x4;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0x5;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x27;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x31;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x1e;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x13;
        data->cluster_if_offsets_values[17] = 0x28;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x02;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x10;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x0a;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x06;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x08;
        data->if_cluster_offsets_values[17] = 0x08;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x04;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0x04;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0x04;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0x03;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0x05;
        data->macro_mapping[17] = 0x03;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 11;
        data->dh_out_width = 110;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_GLEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_GLEM_0, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0x0;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0x1;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0x3;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0x4;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0x6;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x22;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x33;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x34;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x21;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x02;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x10;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x0e;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x12;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x0d;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x01;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0x05;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0x05;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0x07;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0x05;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 11;
        data->dh_out_width = 24;
        data->two_ways_connectivity_bm = 0x12;
    }
    if (MDB_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_GLEM_1, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0x0;
        data->table_macro_interface_mapping[1] = 0x1;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0x2;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0x3;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0x5;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0x6;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x13;
        data->cluster_if_offsets_values[1] = 0x14;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x29;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x2c;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x27;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x02;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x09;
        data->if_cluster_offsets_values[1] = 0x0a;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x14;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x0c;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x0e;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x01;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0x02;
        data->macro_mapping[1] = 0x03;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0x05;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0x04;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0x04;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 11;
        data->dh_out_width = 24;
        data->two_ways_connectivity_bm = 0x8;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_1_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_1_LL, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0x0;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0x1;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0x2;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0x06;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0x04;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0x06;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_1_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_1_DATA, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0x0;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0x1;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0x2;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0x06;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0x04;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0x06;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_2_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_2_LL, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0x0;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0x1;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0x2;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0x06;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0x04;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0x06;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_2_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_2_DATA, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0x0;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0x1;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0x2;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0x06;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0x04;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0x06;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_1_2_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_1_2_DATA, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0x0;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0x1;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0x2;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0x3;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x36;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x20;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x15;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x0c;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x0a;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x0c;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x04;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0x06;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0x04;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0x06;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 120;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_3_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_3_LL, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0x0;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0x1;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0x06;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0x03;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_3_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_3_DATA, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0x0;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0x1;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0x06;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0x03;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_4_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_4_LL, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0x0;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0x1;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0x06;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0x03;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_4_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_4_DATA, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0x0;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0x1;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0x06;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0x03;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_3_4_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_3_4_DATA, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0x0;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0x1;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0x2;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x24;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x27;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x11;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x0b;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x04;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0x06;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0x03;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 120;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_5_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_5_LL, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0x0;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0x04;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_5_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_5_DATA, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0x0;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0x04;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_6_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_6_LL, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0x0;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0x04;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_6_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_6_DATA, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0x0;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0x04;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_5_6_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_5_6_DATA, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0x0;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0x1;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x16;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x0b;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x04;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0x04;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 120;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_7_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_7_LL, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0x0;
        data->table_macro_interface_mapping[1] = 0x1;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0x2;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0x03;
        data->macro_mapping[1] = 0x05;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0x04;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_7_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_7_DATA, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0x0;
        data->table_macro_interface_mapping[1] = 0x1;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0x2;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0x03;
        data->macro_mapping[1] = 0x05;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0x04;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_8_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_8_LL, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0x0;
        data->table_macro_interface_mapping[1] = 0x1;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0x2;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0x03;
        data->macro_mapping[1] = 0x05;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0x04;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_8_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_8_DATA, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0x0;
        data->table_macro_interface_mapping[1] = 0x1;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0x2;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0x03;
        data->macro_mapping[1] = 0x05;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0x04;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_7_8_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_7_8_DATA, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0x0;
        data->table_macro_interface_mapping[1] = 0x1;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0x2;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0x3;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x15;
        data->cluster_if_offsets_values[1] = 0x16;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x2d;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x0a;
        data->if_cluster_offsets_values[1] = 0x0f;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x09;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x04;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0x03;
        data->macro_mapping[1] = 0x05;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0x04;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 120;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EOEM_0, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0x0;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0x1;
        data->table_macro_interface_mapping[18] = 0x2;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0x4;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x37;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x2a;
        data->cluster_if_offsets_values[18] = 0x1c;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x03;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x10;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x0c;
        data->if_cluster_offsets_values[18] = 0x05;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x02;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0x07;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0x04;
        data->macro_mapping[18] = 0x04;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 19;
        data->dh_out_width = 48;
        data->two_ways_connectivity_bm = 0x4;
    }
    if (MDB_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EOEM_1, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0x0;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0x1;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0x2;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0x4;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x29;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x26;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x22;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x03;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x0f;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x10;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x09;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x02;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0x05;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0x06;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0x06;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 19;
        data->dh_out_width = 48;
        data->two_ways_connectivity_bm = 0x4;
    }
    if (MDB_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ESEM, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0x0;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0x1;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0x2;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0x3;
        data->table_macro_interface_mapping[15] = 0x4;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0x5;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x15;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x36;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x2c;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x20;
        data->cluster_if_offsets_values[15] = 0x25;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x05;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x0e;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x10;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x11;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x0e;
        data->if_cluster_offsets_values[15] = 0x0f;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x03;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0x04;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0x07;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0x06;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0x05;
        data->macro_mapping[15] = 0x07;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 35;
        data->dh_out_width = 90;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EVSI, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0x0;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0x1;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0x2;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0xF;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x3a;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x31;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x29;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x00;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x12;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x14;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x12;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x01;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0x08;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0x07;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0x07;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 0;
        data->dh_out_width = 30;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_1, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0x0;
        data->table_macro_interface_mapping[3] = 0x1;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0x2;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x27;
        data->cluster_if_offsets_values[3] = 0x2a;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x09;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x0b;
        data->if_cluster_offsets_values[3] = 0x0c;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x02;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0x03;
        data->macro_mapping[3] = 0x04;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 67;
        data->dh_out_width = 60;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_2, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0x0;
        data->table_macro_interface_mapping[9] = 0x1;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0x2;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x30;
        data->cluster_if_offsets_values[9] = 0x27;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x09;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x0e;
        data->if_cluster_offsets_values[9] = 0x0f;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x02;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0x06;
        data->macro_mapping[9] = 0x04;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 67;
        data->dh_out_width = 60;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_3, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0x0;
        data->table_macro_interface_mapping[3] = 0x1;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0x2;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x30;
        data->cluster_if_offsets_values[3] = 0x33;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x09;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x0d;
        data->if_cluster_offsets_values[3] = 0x0e;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x02;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0x04;
        data->macro_mapping[3] = 0x05;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 67;
        data->dh_out_width = 60;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_4, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0xF;
        data->table_macro_interface_mapping[1] = 0x0;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0x1;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0xF;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0x2;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x00;
        data->cluster_if_offsets_values[1] = 0x16;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x36;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x00;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x13;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x00;
        data->if_cluster_offsets_values[1] = 0x13;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x13;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x00;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x02;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0xFF;
        data->macro_mapping[1] = 0x06;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0x08;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0xFF;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 147;
        data->dh_out_width = 60;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (MDB_PHYSICAL_TABLE_RMEP < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_RMEP, 0);
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_macro_interface_mapping[0] = 0x0;
        data->table_macro_interface_mapping[1] = 0xF;
        data->table_macro_interface_mapping[2] = 0xF;
        data->table_macro_interface_mapping[3] = 0xF;
        data->table_macro_interface_mapping[4] = 0xF;
        data->table_macro_interface_mapping[5] = 0xF;
        data->table_macro_interface_mapping[6] = 0xF;
        data->table_macro_interface_mapping[7] = 0xF;
        data->table_macro_interface_mapping[8] = 0xF;
        data->table_macro_interface_mapping[9] = 0xF;
        data->table_macro_interface_mapping[10] = 0xF;
        data->table_macro_interface_mapping[11] = 0xF;
        data->table_macro_interface_mapping[12] = 0xF;
        data->table_macro_interface_mapping[13] = 0xF;
        data->table_macro_interface_mapping[14] = 0xF;
        data->table_macro_interface_mapping[15] = 0xF;
        data->table_macro_interface_mapping[16] = 0x1;
        data->table_macro_interface_mapping[17] = 0xF;
        data->table_macro_interface_mapping[18] = 0xF;
        data->table_macro_interface_mapping[19] = 0xF;
        data->table_macro_interface_mapping[20] = 0x2;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->cluster_if_offsets_values[0] = 0x1a;
        data->cluster_if_offsets_values[1] = 0x00;
        data->cluster_if_offsets_values[2] = 0x00;
        data->cluster_if_offsets_values[3] = 0x00;
        data->cluster_if_offsets_values[4] = 0x00;
        data->cluster_if_offsets_values[5] = 0x00;
        data->cluster_if_offsets_values[6] = 0x00;
        data->cluster_if_offsets_values[7] = 0x00;
        data->cluster_if_offsets_values[8] = 0x00;
        data->cluster_if_offsets_values[9] = 0x00;
        data->cluster_if_offsets_values[10] = 0x00;
        data->cluster_if_offsets_values[11] = 0x00;
        data->cluster_if_offsets_values[12] = 0x00;
        data->cluster_if_offsets_values[13] = 0x00;
        data->cluster_if_offsets_values[14] = 0x00;
        data->cluster_if_offsets_values[15] = 0x00;
        data->cluster_if_offsets_values[16] = 0x15;
        data->cluster_if_offsets_values[17] = 0x00;
        data->cluster_if_offsets_values[18] = 0x00;
        data->cluster_if_offsets_values[19] = 0x00;
        data->cluster_if_offsets_values[20] = 0x06;
        SHR_RANGE_VERIFY(21, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->if_cluster_offsets_values[0] = 0x11;
        data->if_cluster_offsets_values[1] = 0x00;
        data->if_cluster_offsets_values[2] = 0x00;
        data->if_cluster_offsets_values[3] = 0x00;
        data->if_cluster_offsets_values[4] = 0x00;
        data->if_cluster_offsets_values[5] = 0x00;
        data->if_cluster_offsets_values[6] = 0x00;
        data->if_cluster_offsets_values[7] = 0x00;
        data->if_cluster_offsets_values[8] = 0x00;
        data->if_cluster_offsets_values[9] = 0x00;
        data->if_cluster_offsets_values[10] = 0x00;
        data->if_cluster_offsets_values[11] = 0x00;
        data->if_cluster_offsets_values[12] = 0x00;
        data->if_cluster_offsets_values[13] = 0x00;
        data->if_cluster_offsets_values[14] = 0x00;
        data->if_cluster_offsets_values[15] = 0x00;
        data->if_cluster_offsets_values[16] = 0x10;
        data->if_cluster_offsets_values[17] = 0x00;
        data->if_cluster_offsets_values[18] = 0x00;
        data->if_cluster_offsets_values[19] = 0x00;
        data->if_cluster_offsets_values[20] = 0x04;
        SHR_RANGE_VERIFY(20, 0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH, _SHR_E_INTERNAL, "out of bound access to array")
        data->macro_mapping[0] = 0x05;
        data->macro_mapping[1] = 0xFF;
        data->macro_mapping[2] = 0xFF;
        data->macro_mapping[3] = 0xFF;
        data->macro_mapping[4] = 0xFF;
        data->macro_mapping[5] = 0xFF;
        data->macro_mapping[6] = 0xFF;
        data->macro_mapping[7] = 0xFF;
        data->macro_mapping[8] = 0xFF;
        data->macro_mapping[9] = 0xFF;
        data->macro_mapping[10] = 0xFF;
        data->macro_mapping[11] = 0xFF;
        data->macro_mapping[12] = 0xFF;
        data->macro_mapping[13] = 0xFF;
        data->macro_mapping[14] = 0xFF;
        data->macro_mapping[15] = 0xFF;
        data->macro_mapping[16] = 0x07;
        data->macro_mapping[17] = 0xFF;
        data->macro_mapping[18] = 0xFF;
        data->macro_mapping[19] = 0xFF;
        data->dh_in_width = 43;
        data->dh_out_width = 110;
        data->two_ways_connectivity_bm = 0x0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_block_info_set(
    int unit)
{
    int global_macro_index_index;
    dnx_data_mdb_dh_block_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int table_index = dnx_data_mdb_dh_table_block_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_mdb.dh.total_nof_macroes_get(unit);
    table->info_get.key_size[0] = dnx_data_mdb.dh.total_nof_macroes_get(unit);

    
    table->values[0].default_val = "DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_NONE";
    table->values[1].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_dh_block_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_dh_table_block_info");

    
    default_data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_NONE;
    default_data->block_index = -1;
    
    for (global_macro_index_index = 0; global_macro_index_index < table->keys[0].size; global_macro_index_index++)
    {
        data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, global_macro_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DDHA;
        data->block_index = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DDHA;
        data->block_index = 1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DHC;
        data->block_index = 2;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DHC;
        data->block_index = 7;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DHC;
        data->block_index = 3;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DHC;
        data->block_index = 6;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DHC;
        data->block_index = 0;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DDHA;
        data->block_index = 0;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DHC;
        data->block_index = 4;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DHC;
        data->block_index = 1;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DDHA;
        data->block_index = 1;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DHC;
        data->block_index = 5;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DDHB;
        data->block_index = 0;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DDHB;
        data->block_index = 0;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DDHB;
        data->block_index = 2;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DDHB;
        data->block_index = 1;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DDHB;
        data->block_index = 3;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DDHB;
        data->block_index = 2;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DDHB;
        data->block_index = 1;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DDHB;
        data->block_index = 3;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_logical_macro_info_set(
    int unit)
{
    int global_macro_index_index;
    dnx_data_mdb_dh_logical_macro_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int table_index = dnx_data_mdb_dh_table_logical_macro_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_mdb.dh.total_nof_macroes_get(unit);
    table->info_get.key_size[0] = dnx_data_mdb.dh.total_nof_macroes_get(unit);

    
    table->values[0].default_val = "MDB_NOF_MACRO_TYPES";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_dh_logical_macro_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_dh_table_logical_macro_info");

    
    default_data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->macro_type = MDB_NOF_MACRO_TYPES;
    default_data->macro_index = 0;
    
    for (global_macro_index_index = 0; global_macro_index_index < table->keys[0].size; global_macro_index_index++)
    {
        data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, global_macro_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->macro_type = MDB_MACRO_A;
        data->macro_index = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->macro_type = MDB_MACRO_A;
        data->macro_index = 1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->macro_type = MDB_MACRO_A;
        data->macro_index = 2;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->macro_type = MDB_MACRO_A;
        data->macro_index = 3;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->macro_type = MDB_MACRO_A;
        data->macro_index = 4;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->macro_type = MDB_MACRO_A;
        data->macro_index = 5;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->macro_type = MDB_MACRO_A;
        data->macro_index = 6;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->macro_type = MDB_MACRO_A;
        data->macro_index = 7;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->macro_type = MDB_MACRO_A;
        data->macro_index = 8;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->macro_type = MDB_MACRO_A;
        data->macro_index = 9;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->macro_type = MDB_MACRO_A;
        data->macro_index = 10;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->macro_type = MDB_MACRO_A;
        data->macro_index = 11;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->macro_type = MDB_MACRO_B;
        data->macro_index = 0;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->macro_type = MDB_MACRO_B;
        data->macro_index = 1;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->macro_type = MDB_MACRO_B;
        data->macro_index = 2;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->macro_type = MDB_MACRO_B;
        data->macro_index = 3;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->macro_type = MDB_MACRO_B;
        data->macro_index = 4;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->macro_type = MDB_MACRO_B;
        data->macro_index = 5;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->macro_type = MDB_MACRO_B;
        data->macro_index = 6;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_logical_macro_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->macro_type = MDB_MACRO_B;
        data->macro_index = 7;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_macro_type_info_set(
    int unit)
{
    int macro_type_index;
    dnx_data_mdb_dh_macro_type_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int table_index = dnx_data_mdb_dh_table_macro_type_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_MACRO_TYPES;
    table->info_get.key_size[0] = MDB_NOF_MACRO_TYPES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "invalid";
    table->values[6].default_val = "invalid";
    table->values[7].default_val = "TRUE";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_dh_macro_type_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_dh_table_macro_type_info");

    
    default_data = (dnx_data_mdb_dh_macro_type_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nof_macros = 0;
    default_data->nof_clusters = 0;
    default_data->nof_rows = 0;
    default_data->nof_address_bits = 0;
    default_data->global_start_index = 0;
    default_data->xml_str = "invalid";
    default_data->name = "invalid";
    default_data->hitbit_support = TRUE;
    
    for (macro_type_index = 0; macro_type_index < table->keys[0].size; macro_type_index++)
    {
        data = (dnx_data_mdb_dh_macro_type_info_t *) dnxc_data_mgmt_table_data_get(unit, table, macro_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_MACRO_A < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_type_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_MACRO_A, 0);
        data->nof_macros = 12;
        data->nof_clusters = 8;
        data->nof_rows = 16384;
        data->nof_address_bits = 14;
        data->global_start_index = 0;
        data->xml_str = "dha";
        data->name = "MACRO_A";
    }
    if (MDB_MACRO_B < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_type_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_MACRO_B, 0);
        data->nof_macros = 8;
        data->nof_clusters = 8;
        data->nof_rows = 8192;
        data->nof_address_bits = 13;
        data->global_start_index = 12;
        data->xml_str = "dhb";
        data->name = "MACRO_B";
    }
    if (MDB_EEDB_BANK < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_type_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_BANK, 0);
        data->nof_macros = 1;
        data->nof_clusters = 32;
        data->nof_rows = 2048;
        data->nof_address_bits = 11;
        data->global_start_index = 0;
        data->xml_str = "eedb_bank";
        data->name = "EEDB_BANK";
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_mdb_75_macro_halved_set(
    int unit)
{
    int macro_index_index;
    dnx_data_mdb_dh_mdb_75_macro_halved_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int table_index = dnx_data_mdb_dh_table_mdb_75_macro_halved;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_mdb.dh.total_nof_macroes_get(unit);
    table->info_get.key_size[0] = dnx_data_mdb.dh.total_nof_macroes_get(unit);

    
    table->values[0].default_val = "FALSE";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_dh_mdb_75_macro_halved_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_dh_table_mdb_75_macro_halved");

    
    default_data = (dnx_data_mdb_dh_mdb_75_macro_halved_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->macro_halved = FALSE;
    
    for (macro_index_index = 0; macro_index_index < table->keys[0].size; macro_index_index++)
    {
        data = (dnx_data_mdb_dh_mdb_75_macro_halved_t *) dnxc_data_mgmt_table_data_get(unit, table, macro_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_75_macro_halved_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->macro_halved = TRUE;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_75_macro_halved_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->macro_halved = TRUE;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_75_macro_halved_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->macro_halved = TRUE;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_75_macro_halved_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->macro_halved = TRUE;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_75_macro_halved_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->macro_halved = TRUE;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_75_macro_halved_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->macro_halved = TRUE;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_75_macro_halved_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->macro_halved = TRUE;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_75_macro_halved_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->macro_halved = TRUE;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_entry_banks_info_set(
    int unit)
{
    int global_macro_index_index;
    dnx_data_mdb_dh_entry_banks_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int table_index = dnx_data_mdb_dh_table_entry_banks_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_mdb.dh.total_nof_macroes_get(unit);
    table->info_get.key_size[0] = dnx_data_mdb.dh.total_nof_macroes_get(unit);

    
    table->values[0].default_val = "INVALIDm";
    table->values[1].default_val = "INVALIDr";
    table->values[2].default_val = "INVALIDm";
    table->values[3].default_val = "INVALIDm";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_dh_entry_banks_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_dh_table_entry_banks_info");

    
    default_data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->entry_bank = INVALIDm;
    default_data->overflow_reg = INVALIDr;
    default_data->abk_bank_a = INVALIDm;
    default_data->abk_bank_b = INVALIDm;
    
    for (global_macro_index_index = 0; global_macro_index_index < table->keys[0].size; global_macro_index_index++)
    {
        data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, global_macro_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->entry_bank = DDHA_MACRO_0_ENTRY_BANKm;
        data->overflow_reg = DDHA_REG_0134r;
        data->abk_bank_a = DDHA_MACRO_0_ABK_BANK_Am;
        data->abk_bank_b = DDHA_MACRO_0_ABK_BANK_Bm;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->entry_bank = DDHA_MACRO_0_ENTRY_BANKm;
        data->overflow_reg = DDHA_REG_0134r;
        data->abk_bank_a = DDHA_MACRO_0_ABK_BANK_Am;
        data->abk_bank_b = DDHA_MACRO_0_ABK_BANK_Bm;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->entry_bank = DHC_MACRO_ENTRY_BANKm;
        data->overflow_reg = DHC_REG_0134r;
        data->abk_bank_a = DHC_MACRO_ABK_BANK_Am;
        data->abk_bank_b = DHC_MACRO_ABK_BANK_Bm;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->entry_bank = DHC_MACRO_ENTRY_BANKm;
        data->overflow_reg = DHC_REG_0134r;
        data->abk_bank_a = DHC_MACRO_ABK_BANK_Am;
        data->abk_bank_b = DHC_MACRO_ABK_BANK_Bm;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->entry_bank = DHC_MACRO_ENTRY_BANKm;
        data->overflow_reg = DHC_REG_0134r;
        data->abk_bank_a = DHC_MACRO_ABK_BANK_Am;
        data->abk_bank_b = DHC_MACRO_ABK_BANK_Bm;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->entry_bank = DHC_MACRO_ENTRY_BANKm;
        data->overflow_reg = DHC_REG_0134r;
        data->abk_bank_a = DHC_MACRO_ABK_BANK_Am;
        data->abk_bank_b = DHC_MACRO_ABK_BANK_Bm;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->entry_bank = DHC_MACRO_ENTRY_BANKm;
        data->overflow_reg = DHC_REG_0134r;
        data->abk_bank_a = DHC_MACRO_ABK_BANK_Am;
        data->abk_bank_b = DHC_MACRO_ABK_BANK_Bm;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->entry_bank = DDHA_MACRO_1_ENTRY_BANKm;
        data->overflow_reg = DDHA_REG_0234r;
        data->abk_bank_a = DDHA_MACRO_1_ABK_BANK_Am;
        data->abk_bank_b = DDHA_MACRO_1_ABK_BANK_Bm;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->entry_bank = DHC_MACRO_ENTRY_BANKm;
        data->overflow_reg = DHC_REG_0134r;
        data->abk_bank_a = DHC_MACRO_ABK_BANK_Am;
        data->abk_bank_b = DHC_MACRO_ABK_BANK_Bm;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->entry_bank = DHC_MACRO_ENTRY_BANKm;
        data->overflow_reg = DHC_REG_0134r;
        data->abk_bank_a = DHC_MACRO_ABK_BANK_Am;
        data->abk_bank_b = DHC_MACRO_ABK_BANK_Bm;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->entry_bank = DDHA_MACRO_1_ENTRY_BANKm;
        data->overflow_reg = DDHA_REG_0234r;
        data->abk_bank_a = DDHA_MACRO_1_ABK_BANK_Am;
        data->abk_bank_b = DDHA_MACRO_1_ABK_BANK_Bm;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->entry_bank = DHC_MACRO_ENTRY_BANKm;
        data->overflow_reg = DHC_REG_0134r;
        data->abk_bank_a = DHC_MACRO_ABK_BANK_Am;
        data->abk_bank_b = DHC_MACRO_ABK_BANK_Bm;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->entry_bank = DDHB_MACRO_0_ENTRY_BANKm;
        data->overflow_reg = DDHB_REG_0133r;
        data->abk_bank_a = DDHB_MACRO_0_ABK_BANK_Am;
        data->abk_bank_b = DDHB_MACRO_0_ABK_BANK_Bm;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->entry_bank = DDHB_MACRO_1_ENTRY_BANKm;
        data->overflow_reg = DDHB_REG_0234r;
        data->abk_bank_a = DDHB_MACRO_1_ABK_BANK_Am;
        data->abk_bank_b = DDHB_MACRO_1_ABK_BANK_Bm;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->entry_bank = DDHB_MACRO_1_ENTRY_BANKm;
        data->overflow_reg = DDHB_REG_0234r;
        data->abk_bank_a = DDHB_MACRO_1_ABK_BANK_Am;
        data->abk_bank_b = DDHB_MACRO_1_ABK_BANK_Bm;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->entry_bank = DDHB_MACRO_1_ENTRY_BANKm;
        data->overflow_reg = DDHB_REG_0234r;
        data->abk_bank_a = DDHB_MACRO_1_ABK_BANK_Am;
        data->abk_bank_b = DDHB_MACRO_1_ABK_BANK_Bm;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->entry_bank = DDHB_MACRO_1_ENTRY_BANKm;
        data->overflow_reg = DDHB_REG_0234r;
        data->abk_bank_a = DDHB_MACRO_1_ABK_BANK_Am;
        data->abk_bank_b = DDHB_MACRO_1_ABK_BANK_Bm;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->entry_bank = DDHB_MACRO_0_ENTRY_BANKm;
        data->overflow_reg = DDHB_REG_0133r;
        data->abk_bank_a = DDHB_MACRO_0_ABK_BANK_Am;
        data->abk_bank_b = DDHB_MACRO_0_ABK_BANK_Bm;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->entry_bank = DDHB_MACRO_0_ENTRY_BANKm;
        data->overflow_reg = DDHB_REG_0133r;
        data->abk_bank_a = DDHB_MACRO_0_ABK_BANK_Am;
        data->abk_bank_b = DDHB_MACRO_0_ABK_BANK_Bm;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->entry_bank = DDHB_MACRO_0_ENTRY_BANKm;
        data->overflow_reg = DDHB_REG_0133r;
        data->abk_bank_a = DDHB_MACRO_0_ABK_BANK_Am;
        data->abk_bank_b = DDHB_MACRO_0_ABK_BANK_Bm;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_mdb_1_info_set(
    int unit)
{
    int global_macro_index_index;
    dnx_data_mdb_dh_mdb_1_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int table_index = dnx_data_mdb_dh_table_mdb_1_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_mdb.dh.total_nof_macroes_get(unit);
    table->info_get.key_size[0] = dnx_data_mdb.dh.total_nof_macroes_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "1";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_dh_mdb_1_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_dh_table_mdb_1_info");

    
    default_data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->mdb_item_1_field_size = 0;
    default_data->mdb_item_2_array_size = 1;
    default_data->mdb_item_3_field_size = 0;
    
    for (global_macro_index_index = 0; global_macro_index_index < table->keys[0].size; global_macro_index_index++)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, global_macro_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_0;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_2_array_size = 7;
        data->mdb_item_3_field_size = 6;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_2_array_size = 7;
        data->mdb_item_3_field_size = 6;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_2;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_2_array_size = 8;
        data->mdb_item_3_field_size = 6;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_3;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_2_array_size = 8;
        data->mdb_item_3_field_size = 6;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_4;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_2_array_size = 8;
        data->mdb_item_3_field_size = 6;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_5;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_2_array_size = 8;
        data->mdb_item_3_field_size = 6;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_6;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_2_array_size = 8;
        data->mdb_item_3_field_size = 6;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_7;
        data->mdb_item_1_field_size = 4;
        data->mdb_item_2_array_size = 9;
        data->mdb_item_3_field_size = 7;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_8;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_2_array_size = 8;
        data->mdb_item_3_field_size = 6;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_9;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_2_array_size = 8;
        data->mdb_item_3_field_size = 6;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_10;
        data->mdb_item_1_field_size = 4;
        data->mdb_item_2_array_size = 9;
        data->mdb_item_3_field_size = 7;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_11;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_2_array_size = 8;
        data->mdb_item_3_field_size = 6;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_12;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_2_array_size = 8;
        data->mdb_item_3_field_size = 6;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_13;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_2_array_size = 8;
        data->mdb_item_3_field_size = 6;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_14;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_2_array_size = 8;
        data->mdb_item_3_field_size = 6;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_15;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_2_array_size = 8;
        data->mdb_item_3_field_size = 6;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_16;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_2_array_size = 8;
        data->mdb_item_3_field_size = 6;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_17;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_2_array_size = 8;
        data->mdb_item_3_field_size = 6;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_18;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_2_array_size = 8;
        data->mdb_item_3_field_size = 6;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_19;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_2_array_size = 8;
        data->mdb_item_3_field_size = 6;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_mdb_2_info_set(
    int unit)
{
    int global_macro_index_index;
    dnx_data_mdb_dh_mdb_2_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int table_index = dnx_data_mdb_dh_table_mdb_2_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_mdb.dh.total_nof_macroes_get(unit);
    table->info_get.key_size[0] = dnx_data_mdb.dh.total_nof_macroes_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_dh_mdb_2_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_dh_table_mdb_2_info");

    
    default_data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->mdb_item_1_field_size = 0;
    
    for (global_macro_index_index = 0; global_macro_index_index < table->keys[0].size; global_macro_index_index++)
    {
        data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, global_macro_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->dbal_table = DBAL_TABLE_MDB_2_GLOBAL_INDEX_0;
        data->mdb_item_1_field_size = 4;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->dbal_table = DBAL_TABLE_MDB_2_GLOBAL_INDEX_1;
        data->mdb_item_1_field_size = 4;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->dbal_table = DBAL_TABLE_MDB_2_GLOBAL_INDEX_2;
        data->mdb_item_1_field_size = 4;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->dbal_table = DBAL_TABLE_MDB_2_GLOBAL_INDEX_3;
        data->mdb_item_1_field_size = 4;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->dbal_table = DBAL_TABLE_MDB_2_GLOBAL_INDEX_4;
        data->mdb_item_1_field_size = 4;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->dbal_table = DBAL_TABLE_MDB_2_GLOBAL_INDEX_5;
        data->mdb_item_1_field_size = 4;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->dbal_table = DBAL_TABLE_MDB_2_GLOBAL_INDEX_6;
        data->mdb_item_1_field_size = 4;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->dbal_table = DBAL_TABLE_MDB_2_GLOBAL_INDEX_7;
        data->mdb_item_1_field_size = 4;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->dbal_table = DBAL_TABLE_MDB_2_GLOBAL_INDEX_8;
        data->mdb_item_1_field_size = 4;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->dbal_table = DBAL_TABLE_MDB_2_GLOBAL_INDEX_9;
        data->mdb_item_1_field_size = 4;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->dbal_table = DBAL_TABLE_MDB_2_GLOBAL_INDEX_10;
        data->mdb_item_1_field_size = 4;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->dbal_table = DBAL_TABLE_MDB_2_GLOBAL_INDEX_11;
        data->mdb_item_1_field_size = 4;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->dbal_table = DBAL_TABLE_MDB_2_GLOBAL_INDEX_12;
        data->mdb_item_1_field_size = 4;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->dbal_table = DBAL_TABLE_MDB_2_GLOBAL_INDEX_13;
        data->mdb_item_1_field_size = 4;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->dbal_table = DBAL_TABLE_MDB_2_GLOBAL_INDEX_14;
        data->mdb_item_1_field_size = 4;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->dbal_table = DBAL_TABLE_MDB_2_GLOBAL_INDEX_15;
        data->mdb_item_1_field_size = 4;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->dbal_table = DBAL_TABLE_MDB_2_GLOBAL_INDEX_16;
        data->mdb_item_1_field_size = 4;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->dbal_table = DBAL_TABLE_MDB_2_GLOBAL_INDEX_17;
        data->mdb_item_1_field_size = 4;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->dbal_table = DBAL_TABLE_MDB_2_GLOBAL_INDEX_18;
        data->mdb_item_1_field_size = 4;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->dbal_table = DBAL_TABLE_MDB_2_GLOBAL_INDEX_19;
        data->mdb_item_1_field_size = 4;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_mdb_3_info_set(
    int unit)
{
    int global_macro_index_index;
    dnx_data_mdb_dh_mdb_3_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int table_index = dnx_data_mdb_dh_table_mdb_3_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_mdb.dh.total_nof_macroes_get(unit);
    table->info_get.key_size[0] = dnx_data_mdb.dh.total_nof_macroes_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_dh_mdb_3_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_dh_table_mdb_3_info");

    
    default_data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->mdb_item_0_array_size = 1;
    
    for (global_macro_index_index = 0; global_macro_index_index < table->keys[0].size; global_macro_index_index++)
    {
        data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, global_macro_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->dbal_table = DBAL_TABLE_MDB_3_GLOBAL_INDEX_0;
        data->mdb_item_0_array_size = 21;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->dbal_table = DBAL_TABLE_MDB_3_GLOBAL_INDEX_1;
        data->mdb_item_0_array_size = 21;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->dbal_table = DBAL_TABLE_MDB_3_GLOBAL_INDEX_2;
        data->mdb_item_0_array_size = 21;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->dbal_table = DBAL_TABLE_MDB_3_GLOBAL_INDEX_3;
        data->mdb_item_0_array_size = 21;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->dbal_table = DBAL_TABLE_MDB_3_GLOBAL_INDEX_4;
        data->mdb_item_0_array_size = 21;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->dbal_table = DBAL_TABLE_MDB_3_GLOBAL_INDEX_5;
        data->mdb_item_0_array_size = 21;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->dbal_table = DBAL_TABLE_MDB_3_GLOBAL_INDEX_6;
        data->mdb_item_0_array_size = 21;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->dbal_table = DBAL_TABLE_MDB_3_GLOBAL_INDEX_7;
        data->mdb_item_0_array_size = 21;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->dbal_table = DBAL_TABLE_MDB_3_GLOBAL_INDEX_8;
        data->mdb_item_0_array_size = 21;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->dbal_table = DBAL_TABLE_MDB_3_GLOBAL_INDEX_9;
        data->mdb_item_0_array_size = 21;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->dbal_table = DBAL_TABLE_MDB_3_GLOBAL_INDEX_10;
        data->mdb_item_0_array_size = 21;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->dbal_table = DBAL_TABLE_MDB_3_GLOBAL_INDEX_11;
        data->mdb_item_0_array_size = 21;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->dbal_table = DBAL_TABLE_MDB_3_GLOBAL_INDEX_12;
        data->mdb_item_0_array_size = 14;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->dbal_table = DBAL_TABLE_MDB_3_GLOBAL_INDEX_13;
        data->mdb_item_0_array_size = 20;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->dbal_table = DBAL_TABLE_MDB_3_GLOBAL_INDEX_14;
        data->mdb_item_0_array_size = 20;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->dbal_table = DBAL_TABLE_MDB_3_GLOBAL_INDEX_15;
        data->mdb_item_0_array_size = 20;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->dbal_table = DBAL_TABLE_MDB_3_GLOBAL_INDEX_16;
        data->mdb_item_0_array_size = 20;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->dbal_table = DBAL_TABLE_MDB_3_GLOBAL_INDEX_17;
        data->mdb_item_0_array_size = 14;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->dbal_table = DBAL_TABLE_MDB_3_GLOBAL_INDEX_18;
        data->mdb_item_0_array_size = 14;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->dbal_table = DBAL_TABLE_MDB_3_GLOBAL_INDEX_19;
        data->mdb_item_0_array_size = 14;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_mdb_fec_macro_types_info_set(
    int unit)
{
    dnx_data_mdb_dh_mdb_fec_macro_types_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int table_index = dnx_data_mdb_dh_table_mdb_fec_macro_types_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "MDB_NOF_MACRO_TYPES";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_dh_mdb_fec_macro_types_info_t, (1 + 1  ), "data of dnx_data_mdb_dh_table_mdb_fec_macro_types_info");

    
    default_data = (dnx_data_mdb_dh_mdb_fec_macro_types_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->macro_types[0] = MDB_NOF_MACRO_TYPES;
    
    data = (dnx_data_mdb_dh_mdb_fec_macro_types_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    data = (dnx_data_mdb_dh_mdb_fec_macro_types_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_DH_NOF_FEC_MACRO_TYPES, _SHR_E_INTERNAL, "out of bound access to array")
    data->macro_types[0] = MDB_MACRO_A;
    data->macro_types[1] = MDB_MACRO_B;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_max_nof_interface_dhs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_max_nof_interface_dhs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 11;

    
    define->data = 11;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_mesh_mode_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_mesh_mode_support;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_max_key_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_max_key_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 160;

    
    define->data = 160;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_max_payload_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_max_payload_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 180;

    
    define->data = 180;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_table_mdb_9_mdb_item_1_array_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_1_array_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_table_mdb_9_mdb_item_2_array_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_2_array_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_table_mdb_9_mdb_item_3_array_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_3_array_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_table_mdb_40_mdb_item_0_array_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_40_mdb_item_0_array_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_table_mdb_dynamic_memory_access_memory_access_field_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_dynamic_memory_access_memory_access_field_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_table_mdb_mact_dynamic_memory_access_memory_access_field_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_mact_dynamic_memory_access_memory_access_field_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_table_mdb_kaps_dynamic_memory_access_sbc_memory_access_field_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_kaps_dynamic_memory_access_sbc_memory_access_field_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_table_mdb_kaps_dynamic_memory_access_dpc_memory_access_field_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_kaps_dynamic_memory_access_dpc_memory_access_field_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_table_mdb_ddha_dynamic_memory_access_sbc_memory_access_field_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_ddha_dynamic_memory_access_sbc_memory_access_field_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_table_mdb_ddha_dynamic_memory_access_dpc_memory_access_field_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_ddha_dynamic_memory_access_dpc_memory_access_field_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_table_mdb_ddhb_dynamic_memory_access_memory_access_field_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_ddhb_dynamic_memory_access_memory_access_field_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_mdb_nof_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_mdb_nof_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_adapter_mapping_crps_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_adapter_mapping_crps_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_mdb_profile_set(
    int unit)
{
    dnx_data_mdb_pdbs_mdb_profile_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int table_index = dnx_data_mdb_pdbs_table_mdb_profile;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "Balanced";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_pdbs_mdb_profile_t, (1 + 1  ), "data of dnx_data_mdb_pdbs_table_mdb_profile");

    
    default_data = (dnx_data_mdb_pdbs_mdb_profile_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->profile = "Balanced";
    
    data = (dnx_data_mdb_pdbs_mdb_profile_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[0].property.name = spn_MDB_PROFILE;
    table->values[0].property.doc =
        "SoC property to select the MDB profile: balanced,\n"
        "balanced-exem, balanced-p-and-p, l2-xl, l3-xl, ext-kbp.\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_str;
    table->values[0].property.method_str = "str";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_mdb_pdbs_mdb_profile_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, -1, &data->profile));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_mdb_profiles_info_set(
    int unit)
{
    int mdb_profile_index_index;
    dnx_data_mdb_pdbs_mdb_profiles_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int table_index = dnx_data_mdb_pdbs_table_mdb_profiles_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_DATA_MAX_MDB_PDBS_MDB_NOF_PROFILES;
    table->info_get.key_size[0] = DNX_DATA_MAX_MDB_PDBS_MDB_NOF_PROFILES;

    
    table->values[0].default_val = "FALSE";
    table->values[1].default_val = "NULL";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_pdbs_mdb_profiles_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_pdbs_table_mdb_profiles_info");

    
    default_data = (dnx_data_mdb_pdbs_mdb_profiles_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->supported = FALSE;
    default_data->name = NULL;
    
    for (mdb_profile_index_index = 0; mdb_profile_index_index < table->keys[0].size; mdb_profile_index_index++)
    {
        data = (dnx_data_mdb_pdbs_mdb_profiles_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_profile_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profiles_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->supported = TRUE;
        data->name = "balanced";
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profiles_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->supported = TRUE;
        data->name = "balanced-exem";
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profiles_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->supported = TRUE;
        data->name = "balanced-p-and-p";
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profiles_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->supported = TRUE;
        data->name = "l2-xl";
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profiles_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->supported = TRUE;
        data->name = "l3-xl";
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profiles_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->supported = TRUE;
        data->name = "ext-kbp";
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profiles_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->supported = TRUE;
        data->name = "custom";
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_set(
    int unit)
{
    dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int table_index = dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "2";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_t, (1 + 1  ), "data of dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg");

    
    default_data = (dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = 2;
    
    data = (dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[0].property.name = spn_MDB_PROFILE_KAPS_CFG;
    table->values[0].property.doc =
        "SoC property to select the KAPS configuration.\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_range;
    table->values[0].property.method_str = "range";
    table->values[0].property.range_min = 1;
    table->values[0].property.range_max = 256;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, -1, &data->val));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_set(
    int unit)
{
    int mdb_table_id_index;
    dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int table_index = dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "FALSE";
    table->values[1].default_val = "default";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping");

    
    default_data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = FALSE;
    default_data->xml_name = "default";
    
    for (mdb_table_id_index = 0; mdb_table_id_index < table->keys[0].size; mdb_table_id_index++)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_1, 0);
        data->valid = TRUE;
        data->xml_name = "isem_1";
    }
    if (MDB_PHYSICAL_TABLE_INLIF_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_INLIF_1, 0);
        data->valid = TRUE;
        data->xml_name = "inlif_1";
    }
    if (MDB_PHYSICAL_TABLE_IVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_IVSI, 0);
        data->valid = TRUE;
        data->xml_name = "ivsi";
    }
    if (MDB_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_2, 0);
        data->valid = TRUE;
        data->xml_name = "isem_2";
    }
    if (MDB_PHYSICAL_TABLE_INLIF_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_INLIF_2, 0);
        data->valid = TRUE;
        data->xml_name = "inlif_2";
    }
    if (MDB_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_3, 0);
        data->valid = TRUE;
        data->xml_name = "isem_3";
    }
    if (MDB_PHYSICAL_TABLE_INLIF_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_INLIF_3, 0);
        data->valid = TRUE;
        data->xml_name = "inlif_3";
    }
    if (MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_LEM, 0);
        data->valid = TRUE;
        data->xml_name = "lem";
    }
    if (MDB_PHYSICAL_TABLE_ADS_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ADS_1, 0);
        data->valid = TRUE;
        data->xml_name = "ads_1";
    }
    if (MDB_PHYSICAL_TABLE_ADS_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ADS_2, 0);
        data->valid = TRUE;
        data->xml_name = "ads_2";
    }
    if (MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1, 0);
        data->valid = TRUE;
        data->xml_name = "kaps_1";
    }
    if (MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2, 0);
        data->valid = TRUE;
        data->xml_name = "kaps_2";
    }
    if (MDB_PHYSICAL_TABLE_IOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_IOEM_0, 0);
        data->valid = TRUE;
        data->xml_name = "ioem_0";
    }
    if (MDB_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_IOEM_1, 0);
        data->valid = TRUE;
        data->xml_name = "ioem_1";
    }
    if (MDB_PHYSICAL_TABLE_MAP < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_MAP, 0);
        data->valid = TRUE;
        data->xml_name = "map";
    }
    if (MDB_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_1, 0);
        data->valid = TRUE;
        data->xml_name = "fec_1";
    }
    if (MDB_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_2, 0);
        data->valid = TRUE;
        data->xml_name = "fec_2";
    }
    if (MDB_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_3, 0);
        data->valid = TRUE;
        data->xml_name = "fec_3";
    }
    if (MDB_PHYSICAL_TABLE_MC_ID < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_MC_ID, 0);
        data->valid = TRUE;
        data->xml_name = "mc_id";
    }
    if (MDB_PHYSICAL_TABLE_GLEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_GLEM_0, 0);
        data->valid = TRUE;
        data->xml_name = "glem_0";
    }
    if (MDB_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_GLEM_1, 0);
        data->valid = TRUE;
        data->xml_name = "glem_1";
    }
    if (MDB_PHYSICAL_TABLE_EEDB_1_2_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_1_2_DATA, 0);
        data->valid = TRUE;
        data->xml_name = "eedb_1";
    }
    if (MDB_PHYSICAL_TABLE_EEDB_3_4_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_3_4_DATA, 0);
        data->valid = TRUE;
        data->xml_name = "eedb_2";
    }
    if (MDB_PHYSICAL_TABLE_EEDB_5_6_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_5_6_DATA, 0);
        data->valid = TRUE;
        data->xml_name = "eedb_3";
    }
    if (MDB_PHYSICAL_TABLE_EEDB_7_8_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_7_8_DATA, 0);
        data->valid = TRUE;
        data->xml_name = "eedb_4";
    }
    if (MDB_PHYSICAL_TABLE_EOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EOEM_0, 0);
        data->valid = TRUE;
        data->xml_name = "eoem_0";
    }
    if (MDB_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EOEM_1, 0);
        data->valid = TRUE;
        data->xml_name = "eoem_1";
    }
    if (MDB_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ESEM, 0);
        data->valid = TRUE;
        data->xml_name = "esem";
    }
    if (MDB_PHYSICAL_TABLE_EVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EVSI, 0);
        data->valid = TRUE;
        data->xml_name = "evsi";
    }
    if (MDB_PHYSICAL_TABLE_EXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_1, 0);
        data->valid = TRUE;
        data->xml_name = "exem_1";
    }
    if (MDB_PHYSICAL_TABLE_EXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_2, 0);
        data->valid = TRUE;
        data->xml_name = "exem_2";
    }
    if (MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->valid = TRUE;
        data->xml_name = "exem_3";
    }
    if (MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_4, 0);
        data->valid = TRUE;
        data->xml_name = "exem_4";
    }
    if (MDB_PHYSICAL_TABLE_RMEP < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_RMEP, 0);
        data->valid = TRUE;
        data->xml_name = "rmep";
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_dbal_vmv_str_mapping_set(
    int unit)
{
    int dbal_table_id_index;
    dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int table_index = dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "FALSE";
    table->values[1].default_val = "default";
    table->values[2].default_val = "FALSE";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping");

    
    default_data = (dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = FALSE;
    default_data->xml_name = "default";
    default_data->vmv_xml_chain = FALSE;
    
    for (dbal_table_id_index = 0; dbal_table_id_index < table->keys[0].size; dbal_table_id_index++)
    {
        data = (dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_table_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_1, 0);
        data->valid = TRUE;
        data->xml_name = "ISEM1";
        data->vmv_xml_chain = FALSE;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_2, 0);
        data->valid = TRUE;
        data->xml_name = "ISEM2";
        data->vmv_xml_chain = TRUE;
    }
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->valid = TRUE;
        data->xml_name = "LEM";
        data->vmv_xml_chain = FALSE;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_1, 0);
        data->valid = TRUE;
        data->xml_name = "IOEM1";
        data->vmv_xml_chain = TRUE;
    }
    if (DBAL_PHYSICAL_TABLE_PPMC < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_PPMC, 0);
        data->valid = TRUE;
        data->xml_name = "PPMC";
        data->vmv_xml_chain = FALSE;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_1, 0);
        data->valid = TRUE;
        data->xml_name = "GLEM1";
        data->vmv_xml_chain = TRUE;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->valid = TRUE;
        data->xml_name = "EOEM1";
        data->vmv_xml_chain = TRUE;
    }
    if (DBAL_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ESEM, 0);
        data->valid = TRUE;
        data->xml_name = "ESEM";
        data->vmv_xml_chain = FALSE;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_1, 0);
        data->valid = TRUE;
        data->xml_name = "SEXEM1";
        data->vmv_xml_chain = FALSE;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_2, 0);
        data->valid = TRUE;
        data->xml_name = "SEXEM2";
        data->vmv_xml_chain = FALSE;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->valid = TRUE;
        data->xml_name = "SEXEM3";
        data->vmv_xml_chain = FALSE;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->valid = TRUE;
        data->xml_name = "LEXEM";
        data->vmv_xml_chain = FALSE;
    }
    if (DBAL_PHYSICAL_TABLE_RMEP_EM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_RMEP_EM, 0);
        data->valid = TRUE;
        data->xml_name = "RMEP_EM";
        data->vmv_xml_chain = FALSE;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_mdb_pdb_info_set(
    int unit)
{
    int mdb_table_id_index;
    dnx_data_mdb_pdbs_mdb_pdb_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int table_index = dnx_data_mdb_pdbs_table_mdb_pdb_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "DBAL_NOF_PHYSICAL_TABLES";
    table->values[1].default_val = "DBAL_NOF_ENUM_MDB_DB_TYPE_VALUES";
    table->values[2].default_val = "0";
    table->values[3].default_val = "DEFAULT_NAME";
    table->values[4].default_val = "0";
    table->values[5].default_val = "MDB_NOF_MACRO_TYPES";
    table->values[6].default_val = "DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES";
    table->values[7].default_val = "DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_pdbs_mdb_pdb_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_pdbs_table_mdb_pdb_info");

    
    default_data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->physical_to_logical = DBAL_NOF_PHYSICAL_TABLES;
    default_data->db_type = DBAL_NOF_ENUM_MDB_DB_TYPE_VALUES;
    default_data->db_subtype = 0;
    default_data->name = "DEFAULT_NAME";
    default_data->row_width = 0;
    default_data->macro_granularity = MDB_NOF_MACRO_TYPES;
    default_data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    default_data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    
    for (mdb_table_id_index = 0; mdb_table_id_index < table->keys[0].size; mdb_table_id_index++)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_1, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_ISEM_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_ISEM;
        data->name = "ISEM_1";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_INLIF_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_INLIF_1, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_INLIF_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->db_subtype = MDB_DIRECT_TYPE_INLIF;
        data->name = "INLIF_1";
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_60B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_180B;
    }
    if (MDB_PHYSICAL_TABLE_IVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_IVSI, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_IVSI;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->db_subtype = MDB_DIRECT_TYPE_VSI;
        data->name = "IVSI";
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_90B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_90B;
    }
    if (MDB_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_2, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_ISEM_2;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_ISEM;
        data->name = "ISEM_2";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_INLIF_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_INLIF_2, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_INLIF_2;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->db_subtype = MDB_DIRECT_TYPE_INLIF;
        data->name = "INLIF_2";
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_60B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_180B;
    }
    if (MDB_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_3, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_ISEM_3;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_ISEM;
        data->name = "ISEM_3";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_INLIF_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_INLIF_3, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_INLIF_3;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->db_subtype = MDB_DIRECT_TYPE_INLIF;
        data->name = "INLIF_3";
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_60B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_180B;
    }
    if (MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_LEM, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_LEM;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_LEM;
        data->name = "LEM";
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_ADS_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ADS_1, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_KAPS_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->db_subtype = MDB_DIRECT_TYPE_KAPS;
        data->name = "ADS_1";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (MDB_PHYSICAL_TABLE_ADS_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ADS_2, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_KAPS_2;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->db_subtype = MDB_DIRECT_TYPE_KAPS;
        data->name = "ADS_2";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_KAPS_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS;
        data->db_subtype = 0;
        data->name = "BIG_KAPS_BB_1";
        data->row_width = 4*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_KAPS_2;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS;
        data->db_subtype = 1;
        data->name = "BIG_KAPS_BB_2";
        data->row_width = 4*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_IOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_IOEM_0, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_IOEM_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_OEM;
        data->name = "IOEM_0";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_IOEM_1, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_IOEM_2;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_OEM;
        data->name = "IOEM_1";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_MAP < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_MAP, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_MAP;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->db_subtype = MDB_DIRECT_TYPE_MAP;
        data->name = "MAP";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = dnx_data_mdb.direct.map_payload_size_enum_get(unit);
        data->direct_max_payload_type = dnx_data_mdb.direct.map_payload_size_enum_get(unit);
    }
    if (MDB_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_1, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_FEC_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->db_subtype = MDB_DIRECT_TYPE_FEC;
        data->name = "FEC_1";
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_150B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_150B;
    }
    if (MDB_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_2, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_FEC_2;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->db_subtype = MDB_DIRECT_TYPE_FEC;
        data->name = "FEC_2";
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_150B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_150B;
    }
    if (MDB_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_3, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_FEC_3;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->db_subtype = MDB_DIRECT_TYPE_FEC;
        data->name = "FEC_3";
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_150B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_150B;
    }
    if (MDB_PHYSICAL_TABLE_MC_ID < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_MC_ID, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_PPMC;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_PPMC;
        data->name = "MC_ID";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_GLEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_GLEM_0, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_GLEM_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_GLEM;
        data->name = "GLEM_0";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_GLEM_1, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_GLEM_2;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_GLEM;
        data->name = "GLEM_1";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_1_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_1_LL, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EEDB_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1;
        data->name = "EEDB_1_LL";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_1_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_1_DATA, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EEDB_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1;
        data->name = "EEDB_1_DATA";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_2_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_2_LL, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EEDB_2;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_2;
        data->name = "EEDB_2_LL";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_2_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_2_DATA, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EEDB_2;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_2;
        data->name = "EEDB_2_DATA";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_1_2_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_1_2_DATA, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EEDB_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1;
        data->name = "EEDB_1_2_DATA";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_3_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_3_LL, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EEDB_3;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_3;
        data->name = "EEDB_3_LL";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_3_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_3_DATA, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EEDB_3;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_3;
        data->name = "EEDB_3_DATA";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_4_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_4_LL, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EEDB_4;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_4;
        data->name = "EEDB_4_LL";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_4_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_4_DATA, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EEDB_4;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_4;
        data->name = "EEDB_4_DATA";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_3_4_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_3_4_DATA, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EEDB_3;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_3;
        data->name = "EEDB_3_4_DATA";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_5_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_5_LL, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EEDB_5;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5;
        data->name = "EEDB_5_LL";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_5_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_5_DATA, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EEDB_5;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5;
        data->name = "EEDB_5_DATA";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_6_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_6_LL, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EEDB_6;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_6;
        data->name = "EEDB_6_LL";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_6_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_6_DATA, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EEDB_6;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_6;
        data->name = "EEDB_6_DATA";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_5_6_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_5_6_DATA, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EEDB_5;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5;
        data->name = "EEDB_5_6_DATA";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_7_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_7_LL, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EEDB_7;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_7;
        data->name = "EEDB_7_LL";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_7_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_7_DATA, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EEDB_7;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_7;
        data->name = "EEDB_7_DATA";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_8_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_8_LL, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EEDB_8;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_8;
        data->name = "EEDB_8_LL";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_8_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_8_DATA, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EEDB_8;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_8;
        data->name = "EEDB_8_DATA";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_7_8_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_7_8_DATA, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EEDB_7;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_7;
        data->name = "EEDB_7_8_DATA";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (MDB_PHYSICAL_TABLE_EOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EOEM_0, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EOEM_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_OEM;
        data->name = "EOEM_0";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EOEM_1, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EOEM_2;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_OEM;
        data->name = "EOEM_1";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ESEM, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_ESEM;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_ESEM;
        data->name = "ESEM";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EVSI, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_EVSI;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->db_subtype = MDB_DIRECT_TYPE_VSI;
        data->name = "EVSI";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_30B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_30B;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_1, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_SEXEM_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_EXEM;
        data->name = "EXEM_1";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_2, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_SEXEM_2;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_EXEM;
        data->name = "EXEM_2";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_SEXEM_3;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_EXEM;
        data->name = "EXEM_3";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_4, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_LEXEM;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_EXEM;
        data->name = "EXEM_4";
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (MDB_PHYSICAL_TABLE_RMEP < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_RMEP, 0);
        data->physical_to_logical = DBAL_PHYSICAL_TABLE_RMEP_EM;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_RMEP_EM;
        data->name = "RMEP";
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->macro_granularity = MDB_MACRO_B;
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_dbal_pdb_info_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_pdbs_dbal_pdb_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int table_index = dnx_data_mdb_pdbs_table_dbal_pdb_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "MDB_NOF_PHYSICAL_TABLES";
    table->values[1].default_val = "DBAL_ENUM_FVAL_MDB_DB_TYPE_NONE";
    table->values[2].default_val = "0";
    table->values[3].default_val = "DEFAULT_NAME";
    table->values[4].default_val = "DEFAULT_DBAL_STR";
    table->values[5].default_val = "DBAL_CORE_MODE_SBC";
    table->values[6].default_val = "0";
    table->values[7].default_val = "0";
    table->values[8].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_pdbs_dbal_pdb_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_pdbs_table_dbal_pdb_info");

    
    default_data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->logical_to_physical = MDB_NOF_PHYSICAL_TABLES;
    default_data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_NONE;
    default_data->db_subtype = 0;
    default_data->name = "DEFAULT_NAME";
    default_data->dbal_str = "DEFAULT_DBAL_STR";
    default_data->core_mode = DBAL_CORE_MODE_SBC;
    default_data->row_width = 0;
    default_data->max_key_size = 0;
    default_data->max_payload_size = 0;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_NONE < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_NONE, 0);
        data->logical_to_physical = MDB_NOF_PHYSICAL_TABLES;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_NONE;
        data->db_subtype = 0;
        data->name = "MDB TABLE NONE";
        data->dbal_str = "NONE";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = 0;
        data->max_key_size = 0;
        data->max_payload_size = 0;
    }
    if (DBAL_PHYSICAL_TABLE_TCAM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_TCAM, 0);
        data->logical_to_physical = MDB_NOF_PHYSICAL_TABLES;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_TCAM;
        data->db_subtype = 0;
        data->name = "MDB TABLE TCAM";
        data->dbal_str = "TCAM";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = 0;
        data->max_key_size = 160;
        data->max_payload_size = 64;
    }
    if (DBAL_PHYSICAL_TABLE_KBP < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_KBP, 0);
        data->logical_to_physical = MDB_NOF_PHYSICAL_TABLES;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_NONE;
        data->db_subtype = 0;
        data->name = "MDB TABLE KBP";
        data->dbal_str = "KBP";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = 0;
        data->max_key_size = 160;
        data->max_payload_size = 64;
    }
    if (DBAL_PHYSICAL_TABLE_KAPS_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_KAPS_1, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_ADS_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS;
        data->db_subtype = 0;
        data->name = "MDB TABLE LPM PRIVATE";
        data->dbal_str = "KAPS1";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 160;
        data->max_payload_size = 20;
    }
    if (DBAL_PHYSICAL_TABLE_KAPS_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_KAPS_2, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_ADS_2;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS;
        data->db_subtype = 1;
        data->name = "MDB TABLE LPM PUBLIC";
        data->dbal_str = "KAPS2";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 160;
        data->max_payload_size = 20;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_1, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_ISEM_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_ISEM;
        data->name = "MDB TABLE ISEM 1";
        data->dbal_str = "ISEM1";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 48;
        data->max_payload_size = 24;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_2, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_ISEM_2;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_ISEM;
        data->name = "MDB TABLE ISEM 2";
        data->dbal_str = "ISEM2";
        data->core_mode = DBAL_CORE_MODE_DPC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 88;
        data->max_payload_size = 24;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_3, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_ISEM_3;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_ISEM;
        data->name = "MDB TABLE ISEM 3";
        data->dbal_str = "ISEM3";
        data->core_mode = DBAL_CORE_MODE_DPC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 88;
        data->max_payload_size = 24;
    }
    if (DBAL_PHYSICAL_TABLE_INLIF_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_INLIF_1, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_INLIF_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->db_subtype = MDB_DIRECT_TYPE_INLIF;
        data->name = "MDB TABLE INLIF 1";
        data->dbal_str = "INLIF1";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 180;
    }
    if (DBAL_PHYSICAL_TABLE_INLIF_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_INLIF_2, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_INLIF_2;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->db_subtype = MDB_DIRECT_TYPE_INLIF;
        data->name = "MDB TABLE INLIF 2";
        data->dbal_str = "INLIF2";
        data->core_mode = DBAL_CORE_MODE_DPC;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 180;
    }
    if (DBAL_PHYSICAL_TABLE_INLIF_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_INLIF_3, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_INLIF_3;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->db_subtype = MDB_DIRECT_TYPE_INLIF;
        data->name = "MDB TABLE INLIF 3";
        data->dbal_str = "INLIF3";
        data->core_mode = DBAL_CORE_MODE_DPC;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 180;
    }
    if (DBAL_PHYSICAL_TABLE_IVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IVSI, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_IVSI;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->db_subtype = MDB_DIRECT_TYPE_VSI;
        data->name = "MDB TABLE IVSI";
        data->dbal_str = "IVSI";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 18;
        data->max_payload_size = 90;
    }
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_LEM;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_LEM;
        data->name = "MDB TABLE LEM";
        data->dbal_str = "LEM";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 160;
        data->max_payload_size = 96;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_1, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_IOEM_0;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_OEM;
        data->name = "MDB TABLE IOEM 0";
        data->dbal_str = "IOEM1";
        data->core_mode = DBAL_CORE_MODE_DPC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 32;
        data->max_payload_size = 48;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_2, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_IOEM_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_OEM;
        data->name = "MDB TABLE IOEM 1";
        data->dbal_str = "IOEM2";
        data->core_mode = DBAL_CORE_MODE_DPC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 32;
        data->max_payload_size = 48;
    }
    if (DBAL_PHYSICAL_TABLE_MAP < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_MAP, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_MAP;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->db_subtype = MDB_DIRECT_TYPE_MAP;
        data->name = "MDB TABLE MAP";
        data->dbal_str = "MAP";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 120;
    }
    if (DBAL_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_FEC_1, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_FEC_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->db_subtype = MDB_DIRECT_TYPE_FEC;
        data->name = "MDB TABLE FEC 1";
        data->dbal_str = "FEC1";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_FEC_2, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_FEC_2;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->db_subtype = MDB_DIRECT_TYPE_FEC;
        data->name = "MDB TABLE FEC 2";
        data->dbal_str = "FEC2";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_FEC_3, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_FEC_3;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->db_subtype = MDB_DIRECT_TYPE_FEC;
        data->name = "MDB TABLE FEC 3";
        data->dbal_str = "FEC3";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_PPMC < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_PPMC, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_MC_ID;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_PPMC;
        data->name = "MDB TABLE MC ID";
        data->dbal_str = "PPMC";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 24;
        data->max_payload_size = 110;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_1, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_GLEM_0;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_GLEM;
        data->name = "MDB TABLE GLEM 0";
        data->dbal_str = "GLEM1";
        data->core_mode = DBAL_CORE_MODE_DPC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 24;
        data->max_payload_size = 24;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_2, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_GLEM_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_GLEM;
        data->name = "MDB TABLE GLEM 1";
        data->dbal_str = "GLEM2";
        data->core_mode = DBAL_CORE_MODE_DPC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 24;
        data->max_payload_size = 24;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_1, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_EEDB_1_2_DATA;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1;
        data->name = "MDB TABLE EEDB 1";
        data->dbal_str = "EEDB1";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_2, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_EEDB_1_2_DATA;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_2;
        data->name = "MDB TABLE EEDB 2";
        data->dbal_str = "EEDB2";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_3, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_EEDB_3_4_DATA;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_3;
        data->name = "MDB TABLE EEDB 3";
        data->dbal_str = "EEDB3";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_4, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_EEDB_3_4_DATA;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_4;
        data->name = "MDB TABLE EEDB 4";
        data->dbal_str = "EEDB4";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_5 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_5, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_EEDB_5_6_DATA;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5;
        data->name = "MDB TABLE EEDB 5";
        data->dbal_str = "EEDB5";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_6 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_6, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_EEDB_5_6_DATA;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_6;
        data->name = "MDB TABLE EEDB 6";
        data->dbal_str = "EEDB6";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_7 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_7, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_EEDB_7_8_DATA;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_7;
        data->name = "MDB TABLE EEDB 7";
        data->dbal_str = "EEDB7";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_8 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_8, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_EEDB_7_8_DATA;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->db_subtype = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_8;
        data->name = "MDB TABLE EEDB 8";
        data->dbal_str = "EEDB8";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_EOEM_0;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_OEM;
        data->name = "MDB TABLE EOEM 0";
        data->dbal_str = "EOEM1";
        data->core_mode = DBAL_CORE_MODE_DPC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 32;
        data->max_payload_size = 48;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_EOEM_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_OEM;
        data->name = "MDB TABLE EOEM 1";
        data->dbal_str = "EOEM2";
        data->core_mode = DBAL_CORE_MODE_DPC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 32;
        data->max_payload_size = 48;
    }
    if (DBAL_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ESEM, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_ESEM;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_ESEM;
        data->name = "MDB TABLE ESEM";
        data->dbal_str = "ESEM";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 48;
        data->max_payload_size = 90;
    }
    if (DBAL_PHYSICAL_TABLE_EVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EVSI, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_EVSI;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->db_subtype = MDB_DIRECT_TYPE_VSI;
        data->name = "MDB TABLE EVSI";
        data->dbal_str = "EVSI";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 18;
        data->max_payload_size = 30;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_1, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_EXEM_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_EXEM;
        data->name = "MDB TABLE EXEM 1";
        data->dbal_str = "SEXEM1";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 80;
        data->max_payload_size = 60;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_2, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_EXEM_2;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_EXEM;
        data->name = "MDB TABLE EXEM 2";
        data->dbal_str = "SEXEM2";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 80;
        data->max_payload_size = 60;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_EXEM_3;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_EXEM;
        data->name = "MDB TABLE EXEM 3";
        data->dbal_str = "SEXEM3";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 80;
        data->max_payload_size = 60;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_EXEM_4;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_EXEM;
        data->name = "MDB TABLE EXEM 4";
        data->dbal_str = "LEXEM";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 160;
        data->max_payload_size = 60;
    }
    if (DBAL_PHYSICAL_TABLE_RMEP_EM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_dbal_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_RMEP_EM, 0);
        data->logical_to_physical = MDB_PHYSICAL_TABLE_RMEP;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->db_subtype = MDB_EM_TYPE_RMEP_EM;
        data->name = "MDB TABLE RMEP";
        data->dbal_str = "RMEP_EM";
        data->core_mode = DBAL_CORE_MODE_SBC;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 56;
        data->max_payload_size = 110;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_mdb_adapter_mapping_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_pdbs_mdb_adapter_mapping_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int table_index = dnx_data_mdb_pdbs_table_mdb_adapter_mapping;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "4096";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_pdbs_mdb_adapter_mapping_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_pdbs_table_mdb_adapter_mapping");

    
    default_data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->mdb_type = 0;
    default_data->memory_id = 0;
    default_data->capacity = 4096;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_NONE < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_NONE, 0);
        data->mdb_type = -1;
        data->memory_id = 0;
    }
    if (DBAL_PHYSICAL_TABLE_TCAM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_TCAM, 0);
        data->mdb_type = 4;
        data->memory_id = 0;
    }
    if (DBAL_PHYSICAL_TABLE_KBP < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_KBP, 0);
        data->mdb_type = 0;
        data->memory_id = 0;
    }
    if (DBAL_PHYSICAL_TABLE_KAPS_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_KAPS_1, 0);
        data->mdb_type = 3;
        data->memory_id = 0;
    }
    if (DBAL_PHYSICAL_TABLE_KAPS_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_KAPS_2, 0);
        data->mdb_type = 3;
        data->memory_id = 1;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_1, 0);
        data->mdb_type = 1;
        data->memory_id = 0;
        data->capacity = 16384;
    }
    if (DBAL_PHYSICAL_TABLE_INLIF_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_INLIF_1, 0);
        data->mdb_type = 0;
        data->memory_id = 172;
        data->capacity = 131072;
    }
    if (DBAL_PHYSICAL_TABLE_IVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IVSI, 0);
        data->mdb_type = 0;
        data->memory_id = 175;
        data->capacity = 43691;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_2, 0);
        data->mdb_type = 1;
        data->memory_id = 1;
        data->capacity = 16384;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_3, 0);
        data->mdb_type = 1;
        data->memory_id = 2;
        data->capacity = 16384;
    }
    if (DBAL_PHYSICAL_TABLE_INLIF_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_INLIF_2, 0);
        data->mdb_type = 0;
        data->memory_id = 173;
        data->capacity = 131072;
    }
    if (DBAL_PHYSICAL_TABLE_INLIF_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_INLIF_3, 0);
        data->mdb_type = 0;
        data->memory_id = 174;
        data->capacity = 131072;
    }
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->mdb_type = 1;
        data->memory_id = 10;
        data->capacity = 16384;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_1, 0);
        data->mdb_type = 1;
        data->memory_id = 5;
        data->capacity = 4096;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_2, 0);
        data->mdb_type = 1;
        data->memory_id = 6;
        data->capacity = 4096;
    }
    if (DBAL_PHYSICAL_TABLE_MAP < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_MAP, 0);
        data->mdb_type = 0;
        data->memory_id = 176;
        data->capacity = 65536;
    }
    if (DBAL_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_FEC_1, 0);
        data->mdb_type = 0;
        data->memory_id = 177;
        data->capacity = 104758;
    }
    if (DBAL_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_FEC_2, 0);
        data->mdb_type = 0;
        data->memory_id = 178;
        data->capacity = 78644;
    }
    if (DBAL_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_FEC_3, 0);
        data->mdb_type = 0;
        data->memory_id = 179;
        data->capacity = 52430;
    }
    if (DBAL_PHYSICAL_TABLE_PPMC < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_PPMC, 0);
        data->mdb_type = 1;
        data->memory_id = 11;
        data->capacity = 4096;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_1, 0);
        data->mdb_type = 1;
        data->memory_id = 3;
        data->capacity = 1048576;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_2, 0);
        data->mdb_type = 1;
        data->memory_id = 4;
        data->capacity = 1048576;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_1, 0);
        data->mdb_type = 2;
        data->memory_id = 180;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_2, 0);
        data->mdb_type = 2;
        data->memory_id = 180;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_3, 0);
        data->mdb_type = 2;
        data->memory_id = 181;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_4, 0);
        data->mdb_type = 2;
        data->memory_id = 181;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_5 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_5, 0);
        data->mdb_type = 2;
        data->memory_id = 182;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_6 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_6, 0);
        data->mdb_type = 2;
        data->memory_id = 182;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_7 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_7, 0);
        data->mdb_type = 2;
        data->memory_id = 183;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_8 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_8, 0);
        data->mdb_type = 2;
        data->memory_id = 183;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->mdb_type = 1;
        data->memory_id = 7;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->mdb_type = 1;
        data->memory_id = 8;
    }
    if (DBAL_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ESEM, 0);
        data->mdb_type = 1;
        data->memory_id = 9;
        data->capacity = 16384;
    }
    if (DBAL_PHYSICAL_TABLE_EVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EVSI, 0);
        data->mdb_type = 0;
        data->memory_id = 184;
        data->capacity = 65536;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_1, 0);
        data->mdb_type = 1;
        data->memory_id = 13;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_2, 0);
        data->mdb_type = 1;
        data->memory_id = 14;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->mdb_type = 1;
        data->memory_id = 15;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->mdb_type = 1;
        data->memory_id = 16;
    }
    if (DBAL_PHYSICAL_TABLE_RMEP_EM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_RMEP_EM, 0);
        data->mdb_type = 1;
        data->memory_id = 12;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_set(
    int unit)
{
    int fec_physical_db_index;
    dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int table_index = dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "MDB_NOF_MACRO_TYPES";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info");

    
    default_data = (dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->bank_alloc_resolution = 0;
    default_data->base_macro_type = MDB_NOF_MACRO_TYPES;
    
    for (fec_physical_db_index = 0; fec_physical_db_index < table->keys[0].size; fec_physical_db_index++)
    {
        data = (dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_t *) dnxc_data_mgmt_table_data_get(unit, table, fec_physical_db_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_1, 0);
        data->bank_alloc_resolution = 1;
        data->base_macro_type = MDB_MACRO_B;
    }
    if (MDB_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_2, 0);
        data->bank_alloc_resolution = 1;
        data->base_macro_type = MDB_MACRO_B;
    }
    if (MDB_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_3, 0);
        data->bank_alloc_resolution = 1;
        data->base_macro_type = MDB_MACRO_B;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_mdb_em_entry_counter_decrease_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int feature_index = dnx_data_mdb_em_entry_counter_decrease_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_entry_type_parser_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int feature_index = dnx_data_mdb_em_entry_type_parser;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_step_table_any_cmds_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int feature_index = dnx_data_mdb_em_step_table_any_cmds_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_multi_interface_blocks_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int feature_index = dnx_data_mdb_em_multi_interface_blocks;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_is_isem_dpc_in_vtt5_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int feature_index = dnx_data_mdb_em_is_isem_dpc_in_vtt5;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_mdb_em_age_row_size_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_age_row_size_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128;

    
    define->data = 128;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_age_ovfcam_row_size_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_age_ovfcam_row_size_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_ovf_cam_max_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_ovf_cam_max_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_age_support_per_entry_size_ratio_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_age_support_per_entry_size_ratio;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_flush_tcam_rule_counter_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_flush_tcam_rule_counter_support;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_nof_aging_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_nof_aging_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_aging_profiles_size_in_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_aging_profiles_size_in_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_max_tid_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_max_tid_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_max_nof_tids_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_max_nof_tids;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_flush_support_tids_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_flush_support_tids;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_flush_max_supported_key_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_flush_max_supported_key;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 80;

    
    define->data = 80;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_flush_max_supported_key_plus_payload_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_flush_max_supported_key_plus_payload;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 144;

    
    define->data = 144;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_dh_120_entry_encoding_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_dh_120_entry_encoding_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_dh_240_entry_encoding_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_dh_240_entry_encoding_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_format_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_format_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 30;

    
    define->data = 30;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_max_nof_vmv_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_max_nof_vmv_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_vmv_nof_values_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_vmv_nof_values;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_nof_vmv_size_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_nof_vmv_size_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_esem_nof_vmv_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_esem_nof_vmv_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_glem_nof_vmv_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_glem_nof_vmv_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_mact_nof_vmv_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_mact_nof_vmv_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_mact_max_payload_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_mact_max_payload_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_shift_vmv_max_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_shift_vmv_max_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_shift_vmv_max_regs_per_table_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_shift_vmv_max_regs_per_table;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_min_nof_app_id_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_min_nof_app_id_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_max_nof_spn_sizes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_max_nof_spn_sizes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_nof_lfsr_sizes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_nof_lfsr_sizes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_nof_formats_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_nof_formats;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_defragmentation_priority_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_defragmentation_priority_supported;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_disable_cuckoo_loop_detection_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_disable_cuckoo_loop_detection_support;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_disable_cuckoo_hit_bit_sync_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_disable_cuckoo_hit_bit_sync;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_age_profile_per_ratio_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_age_profile_per_ratio_support;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_flex_mag_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_flex_mag_supported;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_flex_fully_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_flex_fully_supported;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_step_table_max_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_step_table_max_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2048;

    
    define->data = 2048;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_nof_encoding_values_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_nof_encoding_values;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_nof_encoding_types_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_nof_encoding_types;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_extra_vmv_shift_registers_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_extra_vmv_shift_registers;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_scan_bank_participate_in_cuckoo_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_scan_bank_participate_in_cuckoo_support;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_scan_bank_participate_in_cuckoo_partial_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_scan_bank_participate_in_cuckoo_partial_support;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_sbus_error_code_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_sbus_error_code_support;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_nof_emp_tables_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_nof_emp_tables;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_defrag_interrupt_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_defrag_interrupt_support;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_glem_bypass_option_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_glem_bypass_option_support;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_flush_max_supported_payload_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_flush_max_supported_payload;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_flush_payload_max_bus_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_flush_payload_max_bus;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_my_mac_ippa_em_dbal_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_my_mac_ippa_em_dbal_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_PHYSICAL_TABLE_SEXEM_3;

    
    define->data = DBAL_PHYSICAL_TABLE_SEXEM_3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_my_mac_ippf_em_dbal_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_my_mac_ippf_em_dbal_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_PHYSICAL_TABLE_SEXEM_1;

    
    define->data = DBAL_PHYSICAL_TABLE_SEXEM_1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_mact_dbal_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_mact_dbal_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_PHYSICAL_TABLE_LEM;

    
    define->data = DBAL_PHYSICAL_TABLE_LEM;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_mdb_em_em_utilization_api_db_translation_set(
    int unit)
{
    int resource_id_index;
    dnx_data_mdb_em_em_utilization_api_db_translation_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_em_utilization_api_db_translation;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_mdb.global.utilization_api_nof_dbs_get(unit);
    table->info_get.key_size[0] = dnx_data_mdb.global.utilization_api_nof_dbs_get(unit);

    
    table->values[0].default_val = "DBAL_NOF_PHYSICAL_TABLES";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_em_utilization_api_db_translation_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_em_utilization_api_db_translation");

    
    default_data = (dnx_data_mdb_em_em_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_id = DBAL_NOF_PHYSICAL_TABLES;
    
    for (resource_id_index = 0; resource_id_index < table->keys[0].size; resource_id_index++)
    {
        data = (dnx_data_mdb_em_em_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, resource_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_ISEM_1;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_ISEM_2;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_ISEM_3;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_LEM;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_IOEM_1;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_IOEM_2;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_GLEM_1;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_GLEM_2;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_EOEM_1;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_EOEM_2;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_ESEM;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_SEXEM_1;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_SEXEM_2;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_SEXEM_3;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_LEXEM;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_RMEP_EM;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_PPMC;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_spn_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_em_spn_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_spn;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_spn_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_spn");

    
    default_data = (dnx_data_mdb_em_spn_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nof_spn_size = 0;
    default_data->spn_array[0] = 0;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_em_spn_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_spn_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_1, 0);
        data->nof_spn_size = 6;
        SHR_RANGE_VERIFY(6, 0, DNX_DATA_MAX_MDB_EM_MAX_NOF_SPN_SIZES, _SHR_E_INTERNAL, "out of bound access to array")
        data->spn_array[0] = 20;
        data->spn_array[1] = 24;
        data->spn_array[2] = 32;
        data->spn_array[3] = 40;
        data->spn_array[4] = 44;
        data->spn_array[5] = 48;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_spn_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_2, 0);
        data->nof_spn_size = 5;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_MDB_EM_MAX_NOF_SPN_SIZES, _SHR_E_INTERNAL, "out of bound access to array")
        data->spn_array[0] = 20;
        data->spn_array[1] = 32;
        data->spn_array[2] = 40;
        data->spn_array[3] = 64;
        data->spn_array[4] = 80;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_spn_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_3, 0);
        data->nof_spn_size = 5;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_MDB_EM_MAX_NOF_SPN_SIZES, _SHR_E_INTERNAL, "out of bound access to array")
        data->spn_array[0] = 20;
        data->spn_array[1] = 32;
        data->spn_array[2] = 40;
        data->spn_array[3] = 64;
        data->spn_array[4] = 80;
    }
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_spn_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->nof_spn_size = 6;
        SHR_RANGE_VERIFY(6, 0, DNX_DATA_MAX_MDB_EM_MAX_NOF_SPN_SIZES, _SHR_E_INTERNAL, "out of bound access to array")
        data->spn_array[0] = 24;
        data->spn_array[1] = 32;
        data->spn_array[2] = 48;
        data->spn_array[3] = 64;
        data->spn_array[4] = 80;
        data->spn_array[5] = 144;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_spn_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_1, 0);
        data->nof_spn_size = 3;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_MDB_EM_MAX_NOF_SPN_SIZES, _SHR_E_INTERNAL, "out of bound access to array")
        data->spn_array[0] = 24;
        data->spn_array[1] = 28;
        data->spn_array[2] = 32;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_spn_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_2, 0);
        data->nof_spn_size = 3;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_MDB_EM_MAX_NOF_SPN_SIZES, _SHR_E_INTERNAL, "out of bound access to array")
        data->spn_array[0] = 24;
        data->spn_array[1] = 28;
        data->spn_array[2] = 32;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_spn_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->nof_spn_size = 3;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_MDB_EM_MAX_NOF_SPN_SIZES, _SHR_E_INTERNAL, "out of bound access to array")
        data->spn_array[0] = 24;
        data->spn_array[1] = 28;
        data->spn_array[2] = 32;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_spn_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->nof_spn_size = 3;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_MDB_EM_MAX_NOF_SPN_SIZES, _SHR_E_INTERNAL, "out of bound access to array")
        data->spn_array[0] = 24;
        data->spn_array[1] = 28;
        data->spn_array[2] = 32;
    }
    if (DBAL_PHYSICAL_TABLE_PPMC < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_spn_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_PPMC, 0);
        data->nof_spn_size = 2;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_MAX_NOF_SPN_SIZES, _SHR_E_INTERNAL, "out of bound access to array")
        data->spn_array[0] = 20;
        data->spn_array[1] = 24;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_spn_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_1, 0);
        data->nof_spn_size = 2;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_MAX_NOF_SPN_SIZES, _SHR_E_INTERNAL, "out of bound access to array")
        data->spn_array[0] = 20;
        data->spn_array[1] = 24;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_spn_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_2, 0);
        data->nof_spn_size = 2;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_MAX_NOF_SPN_SIZES, _SHR_E_INTERNAL, "out of bound access to array")
        data->spn_array[0] = 20;
        data->spn_array[1] = 24;
    }
    if (DBAL_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_spn_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ESEM, 0);
        data->nof_spn_size = 5;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_MDB_EM_MAX_NOF_SPN_SIZES, _SHR_E_INTERNAL, "out of bound access to array")
        data->spn_array[0] = 24;
        data->spn_array[1] = 32;
        data->spn_array[2] = 40;
        data->spn_array[3] = 44;
        data->spn_array[4] = 48;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_spn_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_1, 0);
        data->nof_spn_size = 5;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_MDB_EM_MAX_NOF_SPN_SIZES, _SHR_E_INTERNAL, "out of bound access to array")
        data->spn_array[0] = 20;
        data->spn_array[1] = 24;
        data->spn_array[2] = 32;
        data->spn_array[3] = 40;
        data->spn_array[4] = 48;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_spn_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_2, 0);
        data->nof_spn_size = 5;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_MDB_EM_MAX_NOF_SPN_SIZES, _SHR_E_INTERNAL, "out of bound access to array")
        data->spn_array[0] = 20;
        data->spn_array[1] = 24;
        data->spn_array[2] = 32;
        data->spn_array[3] = 40;
        data->spn_array[4] = 48;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_spn_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->nof_spn_size = 5;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_MDB_EM_MAX_NOF_SPN_SIZES, _SHR_E_INTERNAL, "out of bound access to array")
        data->spn_array[0] = 20;
        data->spn_array[1] = 24;
        data->spn_array[2] = 32;
        data->spn_array[3] = 40;
        data->spn_array[4] = 48;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_spn_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->nof_spn_size = 5;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_MDB_EM_MAX_NOF_SPN_SIZES, _SHR_E_INTERNAL, "out of bound access to array")
        data->spn_array[0] = 24;
        data->spn_array[1] = 32;
        data->spn_array[2] = 48;
        data->spn_array[3] = 64;
        data->spn_array[4] = 80;
    }
    if (DBAL_PHYSICAL_TABLE_RMEP_EM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_spn_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_RMEP_EM, 0);
        data->nof_spn_size = 4;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_MDB_EM_MAX_NOF_SPN_SIZES, _SHR_E_INTERNAL, "out of bound access to array")
        data->spn_array[0] = 20;
        data->spn_array[1] = 32;
        data->spn_array[2] = 40;
        data->spn_array[3] = 52;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_lfsr_set(
    int unit)
{
    int lfsr_index_index;
    dnx_data_mdb_em_lfsr_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_lfsr;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_mdb.em.nof_lfsr_sizes_get(unit);
    table->info_get.key_size[0] = dnx_data_mdb.em.nof_lfsr_sizes_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_lfsr_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_lfsr");

    
    default_data = (dnx_data_mdb_em_lfsr_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->lfsr_size = 0;
    
    for (lfsr_index_index = 0; lfsr_index_index < table->keys[0].size; lfsr_index_index++)
    {
        data = (dnx_data_mdb_em_lfsr_t *) dnxc_data_mgmt_table_data_get(unit, table, lfsr_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_lfsr_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->lfsr_size = 20;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_lfsr_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->lfsr_size = 24;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_lfsr_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->lfsr_size = 28;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_lfsr_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->lfsr_size = 32;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_emp_index_table_mapping_set(
    int unit)
{
    int emp_table_idx_index;
    dnx_data_mdb_em_emp_index_table_mapping_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_emp_index_table_mapping;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_mdb.em.nof_emp_tables_get(unit);
    table->info_get.key_size[0] = dnx_data_mdb.em.nof_emp_tables_get(unit);

    
    table->values[0].default_val = "DBAL_NOF_PHYSICAL_TABLES";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_emp_index_table_mapping_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_emp_index_table_mapping");

    
    default_data = (dnx_data_mdb_em_emp_index_table_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_id = DBAL_NOF_PHYSICAL_TABLES;
    
    for (emp_table_idx_index = 0; emp_table_idx_index < table->keys[0].size; emp_table_idx_index++)
    {
        data = (dnx_data_mdb_em_emp_index_table_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, emp_table_idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_emp_index_table_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_LEM;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_emp_index_table_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_SEXEM_3;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_emp_index_table_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_LEXEM;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_em_aging_info_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_em_em_aging_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_em_aging_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_em_aging_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_em_aging_info");

    
    default_data = (dnx_data_mdb_em_em_aging_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->max_nof_age_entry_bits = 0;
    default_data->rbd_size = 0;
    default_data->nof_age_banks = 0;
    default_data->total_nof_aging_bits = 0;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_em_em_aging_info_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_aging_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->max_nof_age_entry_bits = 4;
        data->rbd_size = 1;
        data->nof_age_banks = 1;
        data->total_nof_aging_bits = 5242752;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_aging_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->max_nof_age_entry_bits = 8;
        data->rbd_size = 0;
        data->nof_age_banks = 1;
        data->total_nof_aging_bits = 524288;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_aging_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->max_nof_age_entry_bits = 8;
        data->rbd_size = 0;
        data->nof_age_banks = 1;
        data->total_nof_aging_bits = 524288;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_em_aging_cfg_set(
    int unit)
{
    int profile_index;
    int dbal_id_index;
    dnx_data_mdb_em_em_aging_cfg_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_em_aging_cfg;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_mdb.em.nof_aging_profiles_get(unit);
    table->info_get.key_size[0] = dnx_data_mdb.em.nof_aging_profiles_get(unit);
    table->keys[1].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[1] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "1";
    table->values[5].default_val = "1";
    table->values[6].default_val = "0";
    table->values[7].default_val = "0";
    table->values[8].default_val = "0";
    table->values[9].default_val = "0";
    table->values[10].default_val = "0";
    table->values[11].default_val = "0";
    table->values[12].default_val = "DBAL_ENUM_FVAL_MDB_EM_AGING_HIT_BIT_MODE_A_OR_B";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_em_aging_cfg_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_mdb_em_table_em_aging_cfg");

    
    default_data = (dnx_data_mdb_em_em_aging_cfg_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->init_value = 0;
    default_data->global_value = 0;
    default_data->global_mask = 0;
    default_data->external_profile = 0;
    default_data->aging_disable = 1;
    default_data->elephant_disable = 1;
    default_data->elephant_values = 0;
    default_data->mouse_values = 0;
    default_data->age_max_values = 0;
    default_data->increment_values = 0;
    default_data->decrement_values = 0;
    default_data->out_values = 0;
    default_data->hit_bit_mode = DBAL_ENUM_FVAL_MDB_EM_AGING_HIT_BIT_MODE_A_OR_B;
    
    for (profile_index = 0; profile_index < table->keys[0].size; profile_index++)
    {
        for (dbal_id_index = 0; dbal_id_index < table->keys[1].size; dbal_id_index++)
        {
            data = (dnx_data_mdb_em_em_aging_cfg_t *) dnxc_data_mgmt_table_data_get(unit, table, profile_index, dbal_id_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (1 < table->keys[0].size && DBAL_PHYSICAL_TABLE_LEM < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_em_aging_cfg_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, DBAL_PHYSICAL_TABLE_LEM);
        data->init_value = 0;
        data->global_value = 0;
        data->global_mask = 0;
        data->external_profile = 1;
        data->aging_disable = 0;
        data->elephant_disable = 1;
        data->elephant_values = 0;
        data->mouse_values = 0;
        data->age_max_values = 0;
        data->increment_values = 0;
        data->decrement_values = 0;
        data->out_values = 0;
        data->hit_bit_mode = DBAL_ENUM_FVAL_MDB_EM_AGING_HIT_BIT_MODE_A;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_em_info_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_em_em_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_em_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "INVALIDm";
    table->values[2].default_val = "0";
    table->values[3].default_val = "INVALIDr";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_em_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_em_info");

    
    default_data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->tid_size = 0;
    default_data->em_interface = INVALIDm;
    default_data->entry_size = 0;
    default_data->status_reg = INVALIDr;
    default_data->step_table_size = 0;
    default_data->ovf_cam_size = 0;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_PPMC < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_PPMC, 0);
        data->tid_size = 2;
        data->em_interface = MDB_MC_IDm;
        data->entry_size = 137;
        data->status_reg = MDB_REG_274r;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_RMEP_EM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_RMEP_EM, 0);
        data->tid_size = 3;
        data->em_interface = MDB_RMEPm;
        data->entry_size = 137;
        data->status_reg = MDB_REG_545r;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_1, 0);
        data->tid_size = 6;
        data->em_interface = MDB_ISEM_1m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_115r;
        data->step_table_size = 1024;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_2, 0);
        data->tid_size = 6;
        data->em_interface = MDB_ISEM_2m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_136r;
        data->step_table_size = 1024;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_3, 0);
        data->tid_size = 6;
        data->em_interface = MDB_ISEM_3m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_157r;
        data->step_table_size = 1024;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_1, 0);
        data->tid_size = 2;
        data->em_interface = MDB_GLEM_0m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_294r;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_2, 0);
        data->tid_size = 2;
        data->em_interface = MDB_GLEM_1m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_310r;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->tid_size = 6;
        data->em_interface = MDB_LEMm;
        data->entry_size = 257;
        data->status_reg = MDB_REG_212r;
        data->step_table_size = 2048;
        data->ovf_cam_size = 8;
    }
    if (DBAL_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ESEM, 0);
        data->tid_size = 6;
        data->em_interface = MDB_ESEMm;
        data->entry_size = 137;
        data->status_reg = MDB_REG_377r;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_1, 0);
        data->tid_size = 6;
        data->em_interface = MDB_EXEM_1m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_398r;
        data->step_table_size = 512;
        data->ovf_cam_size = 2;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_2, 0);
        data->tid_size = 6;
        data->em_interface = MDB_EXEM_2m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_419r;
        data->step_table_size = 512;
        data->ovf_cam_size = 2;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->tid_size = 6;
        data->em_interface = MDB_EXEM_3m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_472r;
        data->step_table_size = 512;
        data->ovf_cam_size = 2;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->tid_size = 6;
        data->em_interface = MDB_EXEM_4m;
        data->entry_size = 257;
        data->status_reg = MDB_REG_525r;
        data->step_table_size = 512;
        data->ovf_cam_size = 2;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->tid_size = 6;
        data->em_interface = MDB_EOEM_0m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_335r;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->tid_size = 6;
        data->em_interface = MDB_EOEM_1m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_352r;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_1, 0);
        data->tid_size = 6;
        data->em_interface = MDB_IOEM_0m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_233r;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_2, 0);
        data->tid_size = 6;
        data->em_interface = MDB_IOEM_1m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_250r;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_em_interrupt_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_em_em_interrupt_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_em_interrupt;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "INVALIDr";
    table->values[2].default_val = "INVALIDf";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_em_interrupt_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_em_interrupt");

    
    default_data = (dnx_data_mdb_em_em_interrupt_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->instance = 0;
    default_data->interrupt_register = INVALIDr;
    default_data->interrupt_field = INVALIDf;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_em_em_interrupt_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_interrupt_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_1, 0);
        data->instance = 0;
        data->interrupt_register = MDB_REG_0000r;
        data->interrupt_field = FIELD_1_1f;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_interrupt_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_2, 0);
        data->instance = 0;
        data->interrupt_register = MDB_REG_0000r;
        data->interrupt_field = FIELD_2_2f;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_interrupt_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_3, 0);
        data->instance = 0;
        data->interrupt_register = MDB_REG_0000r;
        data->interrupt_field = FIELD_3_3f;
    }
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_interrupt_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->instance = 0;
        data->interrupt_register = MDB_REG_0000r;
        data->interrupt_field = FIELD_4_4f;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_interrupt_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_1, 0);
        data->instance = 0;
        data->interrupt_register = MDB_REG_0000r;
        data->interrupt_field = FIELD_5_5f;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_interrupt_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_2, 0);
        data->instance = 0;
        data->interrupt_register = MDB_REG_0000r;
        data->interrupt_field = FIELD_6_6f;
    }
    if (DBAL_PHYSICAL_TABLE_PPMC < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_interrupt_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_PPMC, 0);
        data->instance = 0;
        data->interrupt_register = MDB_REG_0000r;
        data->interrupt_field = FIELD_7_7f;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_interrupt_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_1, 0);
        data->instance = 0;
        data->interrupt_register = MDB_REG_0000r;
        data->interrupt_field = FIELD_8_8f;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_interrupt_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_2, 0);
        data->instance = 0;
        data->interrupt_register = MDB_REG_0000r;
        data->interrupt_field = FIELD_9_9f;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_interrupt_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->instance = 0;
        data->interrupt_register = MDB_REG_0000r;
        data->interrupt_field = FIELD_10_10f;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_interrupt_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->instance = 0;
        data->interrupt_register = MDB_REG_0000r;
        data->interrupt_field = FIELD_11_11f;
    }
    if (DBAL_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_interrupt_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ESEM, 0);
        data->instance = 0;
        data->interrupt_register = MDB_REG_0000r;
        data->interrupt_field = FIELD_12_12f;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_interrupt_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_1, 0);
        data->instance = 0;
        data->interrupt_register = MDB_REG_0000r;
        data->interrupt_field = FIELD_13_13f;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_interrupt_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_2, 0);
        data->instance = 0;
        data->interrupt_register = MDB_REG_0000r;
        data->interrupt_field = FIELD_14_14f;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_interrupt_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->instance = 0;
        data->interrupt_register = MDB_REG_0000r;
        data->interrupt_field = FIELD_15_15f;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_interrupt_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->instance = 0;
        data->interrupt_register = MDB_REG_0000r;
        data->interrupt_field = FIELD_16_16f;
    }
    if (DBAL_PHYSICAL_TABLE_RMEP_EM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_interrupt_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_RMEP_EM, 0);
        data->instance = 0;
        data->interrupt_register = MDB_REG_0000r;
        data->interrupt_field = FIELD_17_17f;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_step_table_pdb_max_depth_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_em_step_table_pdb_max_depth_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_step_table_pdb_max_depth;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "3";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_step_table_pdb_max_depth_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_step_table_pdb_max_depth");

    
    default_data = (dnx_data_mdb_em_step_table_pdb_max_depth_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->max_depth = 3;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_em_step_table_pdb_max_depth_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_step_table_max_depth_possible_set(
    int unit)
{
    int step_table_size_indication_index;
    int aspect_ratio_combination_index;
    dnx_data_mdb_em_step_table_max_depth_possible_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_step_table_max_depth_possible;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 3;
    table->info_get.key_size[0] = 3;
    table->keys[1].size = 16;
    table->info_get.key_size[1] = 16;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_step_table_max_depth_possible_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_mdb_em_table_step_table_max_depth_possible");

    
    default_data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->max_depth = 0;
    
    for (step_table_size_indication_index = 0; step_table_size_indication_index < table->keys[0].size; step_table_size_indication_index++)
    {
        for (aspect_ratio_combination_index = 0; aspect_ratio_combination_index < table->keys[1].size; aspect_ratio_combination_index++)
        {
            data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, step_table_size_indication_index, aspect_ratio_combination_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 1);
        data->max_depth = 22;
    }
    if (0 < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 2);
        data->max_depth = 18;
    }
    if (0 < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 3);
        data->max_depth = 6;
    }
    if (0 < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 4);
        data->max_depth = 15;
    }
    if (0 < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 5);
        data->max_depth = 6;
    }
    if (0 < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 6);
        data->max_depth = 6;
    }
    if (0 < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 7);
        data->max_depth = 4;
    }
    if (0 < table->keys[0].size && 8 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 8);
        data->max_depth = 14;
    }
    if (0 < table->keys[0].size && 9 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 9);
        data->max_depth = 6;
    }
    if (0 < table->keys[0].size && 10 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 10);
        data->max_depth = 6;
    }
    if (0 < table->keys[0].size && 11 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 11);
        data->max_depth = 4;
    }
    if (0 < table->keys[0].size && 12 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 12);
        data->max_depth = 5;
    }
    if (0 < table->keys[0].size && 13 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 13);
        data->max_depth = 4;
    }
    if (0 < table->keys[0].size && 14 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 14);
        data->max_depth = 4;
    }
    if (0 < table->keys[0].size && 15 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 15);
        data->max_depth = 4;
    }
    if (1 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 1);
        data->max_depth = 31;
    }
    if (1 < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 2);
        data->max_depth = 25;
    }
    if (1 < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 3);
        data->max_depth = 7;
    }
    if (1 < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 4);
        data->max_depth = 22;
    }
    if (1 < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 5);
        data->max_depth = 7;
    }
    if (1 < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 6);
        data->max_depth = 7;
    }
    if (1 < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 7);
        data->max_depth = 5;
    }
    if (1 < table->keys[0].size && 8 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 8);
        data->max_depth = 19;
    }
    if (1 < table->keys[0].size && 9 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 9);
        data->max_depth = 7;
    }
    if (1 < table->keys[0].size && 10 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 10);
        data->max_depth = 7;
    }
    if (1 < table->keys[0].size && 11 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 11);
        data->max_depth = 5;
    }
    if (1 < table->keys[0].size && 12 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 12);
        data->max_depth = 6;
    }
    if (1 < table->keys[0].size && 13 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 13);
        data->max_depth = 5;
    }
    if (1 < table->keys[0].size && 14 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 14);
        data->max_depth = 5;
    }
    if (1 < table->keys[0].size && 15 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 15);
        data->max_depth = 4;
    }
    if (2 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 1);
        data->max_depth = 45;
    }
    if (2 < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 2);
        data->max_depth = 36;
    }
    if (2 < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 3);
        data->max_depth = 8;
    }
    if (2 < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 4);
        data->max_depth = 31;
    }
    if (2 < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 5);
        data->max_depth = 8;
    }
    if (2 < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 6);
        data->max_depth = 8;
    }
    if (2 < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 7);
        data->max_depth = 6;
    }
    if (2 < table->keys[0].size && 8 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 8);
        data->max_depth = 28;
    }
    if (2 < table->keys[0].size && 9 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 9);
        data->max_depth = 8;
    }
    if (2 < table->keys[0].size && 10 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 10);
        data->max_depth = 8;
    }
    if (2 < table->keys[0].size && 11 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 11);
        data->max_depth = 6;
    }
    if (2 < table->keys[0].size && 12 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 12);
        data->max_depth = 7;
    }
    if (2 < table->keys[0].size && 13 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 13);
        data->max_depth = 5;
    }
    if (2 < table->keys[0].size && 14 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 14);
        data->max_depth = 5;
    }
    if (2 < table->keys[0].size && 15 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_possible_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 15);
        data->max_depth = 5;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_set(
    int unit)
{
    int aspect_ratio_index;
    int nof_ways_index;
    dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 5;
    table->info_get.key_size[0] = 5;
    table->keys[1].size = DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS;
    table->info_get.key_size[1] = DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS;

    
    table->values[0].default_val = "3";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar");

    
    default_data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->max_depth = 3;
    
    for (aspect_ratio_index = 0; aspect_ratio_index < table->keys[0].size; aspect_ratio_index++)
    {
        for (nof_ways_index = 0; nof_ways_index < table->keys[1].size; nof_ways_index++)
        {
            data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, aspect_ratio_index, nof_ways_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 2);
        data->max_depth = 8;
    }
    if (0 < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 3);
        data->max_depth = 8;
    }
    if (0 < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 4);
        data->max_depth = 7;
    }
    if (0 < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 5);
        data->max_depth = 6;
    }
    if (0 < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 6);
        data->max_depth = 6;
    }
    if (0 < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 7);
        data->max_depth = 5;
    }
    if (0 < table->keys[0].size && 8 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 8);
        data->max_depth = 5;
    }
    if (0 < table->keys[0].size && 9 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 9);
        data->max_depth = 5;
    }
    if (0 < table->keys[0].size && 10 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 10);
        data->max_depth = 4;
    }
    if (0 < table->keys[0].size && 11 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 11);
        data->max_depth = 4;
    }
    if (1 < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 2);
        data->max_depth = 8;
    }
    if (1 < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 3);
        data->max_depth = 6;
    }
    if (1 < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 4);
        data->max_depth = 5;
    }
    if (1 < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 5);
        data->max_depth = 4;
    }
    if (1 < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 6);
        data->max_depth = 4;
    }
    if (1 < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 7);
        data->max_depth = 4;
    }
    if (1 < table->keys[0].size && 8 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 8);
        data->max_depth = 4;
    }
    if (1 < table->keys[0].size && 9 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 9);
        data->max_depth = 3;
    }
    if (1 < table->keys[0].size && 10 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 10);
        data->max_depth = 3;
    }
    if (1 < table->keys[0].size && 11 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 11);
        data->max_depth = 3;
    }
    if (2 < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 2);
        data->max_depth = 6;
    }
    if (2 < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 3);
        data->max_depth = 4;
    }
    if (2 < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 4);
        data->max_depth = 4;
    }
    if (2 < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 5);
        data->max_depth = 3;
    }
    if (2 < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 6);
        data->max_depth = 3;
    }
    if (2 < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 7);
        data->max_depth = 3;
    }
    if (2 < table->keys[0].size && 8 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 8);
        data->max_depth = 3;
    }
    if (2 < table->keys[0].size && 9 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 9);
        data->max_depth = 3;
    }
    if (2 < table->keys[0].size && 10 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 10);
        data->max_depth = 3;
    }
    if (2 < table->keys[0].size && 11 < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 11);
        data->max_depth = 3;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_mdb_15_info_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_em_mdb_15_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_15_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_15_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_15_info");

    
    default_data = (dnx_data_mdb_em_mdb_15_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->mdb_item_5_field_size = 0;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_em_mdb_15_info_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_15_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_15_ISEM_1;
        data->mdb_item_5_field_size = 3;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_15_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_15_ISEM_2;
        data->mdb_item_5_field_size = 3;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_15_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_15_ISEM_3;
        data->mdb_item_5_field_size = 3;
    }
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_15_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_15_LEM;
        data->mdb_item_5_field_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_15_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_15_IOEM_0;
        data->mdb_item_5_field_size = 3;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_15_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_15_IOEM_1;
        data->mdb_item_5_field_size = 3;
    }
    if (DBAL_PHYSICAL_TABLE_PPMC < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_15_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_PPMC, 0);
        data->dbal_table = DBAL_TABLE_MDB_15_MC_ID;
        data->mdb_item_5_field_size = 3;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_15_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_15_GLEM_0;
        data->mdb_item_5_field_size = 3;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_15_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_15_GLEM_1;
        data->mdb_item_5_field_size = 3;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_15_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_15_EOEM_0;
        data->mdb_item_5_field_size = 3;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_15_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_15_EOEM_1;
        data->mdb_item_5_field_size = 3;
    }
    if (DBAL_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_15_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ESEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_15_ESEM;
        data->mdb_item_5_field_size = 3;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_15_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_15_EXEM_1;
        data->mdb_item_5_field_size = 2;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_15_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_15_EXEM_2;
        data->mdb_item_5_field_size = 2;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_15_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_15_EXEM_3;
        data->mdb_item_5_field_size = 2;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_15_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_15_EXEM_4;
        data->mdb_item_5_field_size = 2;
    }
    if (DBAL_PHYSICAL_TABLE_RMEP_EM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_15_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_RMEP_EM, 0);
        data->dbal_table = DBAL_TABLE_MDB_15_RMEP;
        data->mdb_item_5_field_size = 3;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_mdb_16_info_set(
    int unit)
{
    int mdb_table_id_index;
    dnx_data_mdb_em_mdb_16_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_16_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_16_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_16_info");

    
    default_data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->mdb_item_0_field_size = 0;
    
    for (mdb_table_id_index = 0; mdb_table_id_index < table->keys[0].size; mdb_table_id_index++)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_ISEM_1;
        data->mdb_item_0_field_size = 9;
    }
    if (MDB_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_ISEM_2;
        data->mdb_item_0_field_size = 8;
    }
    if (MDB_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_ISEM_3;
        data->mdb_item_0_field_size = 8;
    }
    if (MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_LEM;
        data->mdb_item_0_field_size = 12;
    }
    if (MDB_PHYSICAL_TABLE_IOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_IOEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_IOEM_0;
        data->mdb_item_0_field_size = 5;
    }
    if (MDB_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_IOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_IOEM_1;
        data->mdb_item_0_field_size = 5;
    }
    if (MDB_PHYSICAL_TABLE_EOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EOEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_EOEM_0;
        data->mdb_item_0_field_size = 5;
    }
    if (MDB_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_EOEM_1;
        data->mdb_item_0_field_size = 5;
    }
    if (MDB_PHYSICAL_TABLE_MC_ID < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_MC_ID, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_MC_ID;
        data->mdb_item_0_field_size = 6;
    }
    if (MDB_PHYSICAL_TABLE_GLEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_GLEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_GLEM_0;
        data->mdb_item_0_field_size = 7;
    }
    if (MDB_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_GLEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_GLEM_1;
        data->mdb_item_0_field_size = 7;
    }
    if (MDB_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ESEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_ESEM;
        data->mdb_item_0_field_size = 6;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_EXEM_1;
        data->mdb_item_0_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_EXEM_2;
        data->mdb_item_0_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_EXEM_3;
        data->mdb_item_0_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_4, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_EXEM_4;
        data->mdb_item_0_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_RMEP < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_RMEP, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_RMEP;
        data->mdb_item_0_field_size = 3;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_mdb_21_info_set(
    int unit)
{
    int mdb_table_id_index;
    dnx_data_mdb_em_mdb_21_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_21_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_21_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_21_info");

    
    default_data = (dnx_data_mdb_em_mdb_21_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->mdb_item_0_field_size = 0;
    
    for (mdb_table_id_index = 0; mdb_table_id_index < table->keys[0].size; mdb_table_id_index++)
    {
        data = (dnx_data_mdb_em_mdb_21_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_21_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_21_LEM;
        data->mdb_item_0_field_size = 1;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_21_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_21_EXEM_3;
        data->mdb_item_0_field_size = 2;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_21_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_4, 0);
        data->dbal_table = DBAL_TABLE_MDB_21_EXEM_4;
        data->mdb_item_0_field_size = 2;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_mdb_23_info_set(
    int unit)
{
    int mdb_table_id_index;
    dnx_data_mdb_em_mdb_23_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_23_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_23_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_23_info");

    
    default_data = (dnx_data_mdb_em_mdb_23_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->mdb_item_0_field_size = 0;
    default_data->mdb_item_1_field_size = 0;
    
    for (mdb_table_id_index = 0; mdb_table_id_index < table->keys[0].size; mdb_table_id_index++)
    {
        data = (dnx_data_mdb_em_mdb_23_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_23_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_23_LEM;
        data->mdb_item_0_field_size = 12;
        data->mdb_item_1_field_size = 4;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_23_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_23_EXEM_3;
        data->mdb_item_0_field_size = 3;
        data->mdb_item_1_field_size = 2;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_23_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_4, 0);
        data->dbal_table = DBAL_TABLE_MDB_23_EXEM_4;
        data->mdb_item_0_field_size = 3;
        data->mdb_item_1_field_size = 2;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_mdb_24_info_set(
    int unit)
{
    int mdb_table_id_index;
    dnx_data_mdb_em_mdb_24_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_24_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "1";
    table->values[2].default_val = "1";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_24_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_24_info");

    
    default_data = (dnx_data_mdb_em_mdb_24_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->mdb_item_0_array_size = 1;
    default_data->mdb_item_1_array_size = 1;
    default_data->mdb_item_1_field_size = 0;
    
    for (mdb_table_id_index = 0; mdb_table_id_index < table->keys[0].size; mdb_table_id_index++)
    {
        data = (dnx_data_mdb_em_mdb_24_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_24_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_24_LEM;
        data->mdb_item_0_array_size = 12;
        data->mdb_item_1_array_size = 12;
        data->mdb_item_1_field_size = 18;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_24_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_24_EXEM_3;
        data->mdb_item_0_array_size = 3;
        data->mdb_item_1_array_size = 3;
        data->mdb_item_1_field_size = 16;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_24_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_4, 0);
        data->dbal_table = DBAL_TABLE_MDB_24_EXEM_4;
        data->mdb_item_0_array_size = 3;
        data->mdb_item_1_array_size = 3;
        data->mdb_item_1_field_size = 16;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_mdb_29_info_set(
    int unit)
{
    int mdb_table_id_index;
    dnx_data_mdb_em_mdb_29_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_29_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "1";
    table->values[2].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_29_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_29_info");

    
    default_data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->mdb_item_0_array_size = 1;
    default_data->mdb_item_1_array_size = 1;
    
    for (mdb_table_id_index = 0; mdb_table_id_index < table->keys[0].size; mdb_table_id_index++)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_ISEM_1;
        data->mdb_item_0_array_size = 8;
    }
    if (MDB_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_ISEM_2;
        data->mdb_item_0_array_size = 7;
    }
    if (MDB_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_ISEM_3;
        data->mdb_item_0_array_size = 7;
    }
    if (MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_LEM;
        data->mdb_item_0_array_size = 11;
    }
    if (MDB_PHYSICAL_TABLE_IOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_IOEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_IOEM_0;
        data->mdb_item_0_array_size = 4;
    }
    if (MDB_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_IOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_IOEM_1;
        data->mdb_item_0_array_size = 4;
    }
    if (MDB_PHYSICAL_TABLE_EOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EOEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_EOEM_0;
        data->mdb_item_0_array_size = 4;
    }
    if (MDB_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_EOEM_1;
        data->mdb_item_0_array_size = 4;
    }
    if (MDB_PHYSICAL_TABLE_MC_ID < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_MC_ID, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_MC_ID;
        data->mdb_item_0_array_size = 5;
    }
    if (MDB_PHYSICAL_TABLE_GLEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_GLEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_GLEM_0;
        data->mdb_item_0_array_size = 6;
    }
    if (MDB_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_GLEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_GLEM_1;
        data->mdb_item_0_array_size = 6;
    }
    if (MDB_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ESEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_ESEM;
        data->mdb_item_0_array_size = 5;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_EXEM_1;
        data->mdb_item_0_array_size = 2;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_EXEM_2;
        data->mdb_item_0_array_size = 2;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_EXEM_3;
        data->mdb_item_0_array_size = 2;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_4, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_EXEM_4;
        data->mdb_item_0_array_size = 2;
    }
    if (MDB_PHYSICAL_TABLE_RMEP < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_RMEP, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_RMEP;
        data->mdb_item_0_array_size = 2;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_mdb_31_info_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_em_mdb_31_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_31_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "1";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_31_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_31_info");

    
    default_data = (dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->array_size = 1;
    default_data->mdb_item_0_field_size = 0;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_ISEM_1;
        data->array_size = 10;
        data->mdb_item_0_field_size = 25;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_ISEM_2;
        data->array_size = 10;
        data->mdb_item_0_field_size = 25;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_ISEM_3;
        data->array_size = 10;
        data->mdb_item_0_field_size = 25;
    }
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_LEM;
        data->array_size = 11;
        data->mdb_item_0_field_size = 27;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_IOEM_0;
        data->array_size = 9;
        data->mdb_item_0_field_size = 23;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_IOEM_1;
        data->array_size = 9;
        data->mdb_item_0_field_size = 23;
    }
    if (DBAL_PHYSICAL_TABLE_PPMC < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_PPMC, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_MC_ID;
        data->array_size = 9;
        data->mdb_item_0_field_size = 23;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_GLEM_0;
        data->array_size = 9;
        data->mdb_item_0_field_size = 23;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_GLEM_1;
        data->array_size = 9;
        data->mdb_item_0_field_size = 23;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_EOEM_0;
        data->array_size = 9;
        data->mdb_item_0_field_size = 23;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_EOEM_1;
        data->array_size = 9;
        data->mdb_item_0_field_size = 23;
    }
    if (DBAL_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ESEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_ESEM;
        data->array_size = 9;
        data->mdb_item_0_field_size = 23;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_EXEM_1;
        data->array_size = 9;
        data->mdb_item_0_field_size = 23;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_EXEM_2;
        data->array_size = 9;
        data->mdb_item_0_field_size = 23;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_EXEM_3;
        data->array_size = 9;
        data->mdb_item_0_field_size = 23;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_EXEM_4;
        data->array_size = 9;
        data->mdb_item_0_field_size = 23;
    }
    if (DBAL_PHYSICAL_TABLE_RMEP_EM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_RMEP_EM, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_RMEP;
        data->array_size = 9;
        data->mdb_item_0_field_size = 23;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_mdb_32_info_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_em_mdb_32_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_32_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_32_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_32_info");

    
    default_data = (dnx_data_mdb_em_mdb_32_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->mdb_item_0_field_size = 0;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_em_mdb_32_info_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_32_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_32_ISEM_1;
        data->mdb_item_0_field_size = 10;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_32_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_32_ISEM_2;
        data->mdb_item_0_field_size = 10;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_32_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_32_ISEM_3;
        data->mdb_item_0_field_size = 10;
    }
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_32_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_32_LEM;
        data->mdb_item_0_field_size = 11;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_32_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_32_IOEM_0;
        data->mdb_item_0_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_32_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_32_IOEM_1;
        data->mdb_item_0_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_PPMC < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_32_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_PPMC, 0);
        data->dbal_table = DBAL_TABLE_MDB_32_MC_ID;
        data->mdb_item_0_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_32_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_32_GLEM_0;
        data->mdb_item_0_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_32_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_32_GLEM_1;
        data->mdb_item_0_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_32_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_32_EOEM_0;
        data->mdb_item_0_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_32_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_32_EOEM_1;
        data->mdb_item_0_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_32_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ESEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_32_ESEM;
        data->mdb_item_0_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_32_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_32_EXEM_1;
        data->mdb_item_0_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_32_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_32_EXEM_2;
        data->mdb_item_0_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_32_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_32_EXEM_3;
        data->mdb_item_0_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_32_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_32_EXEM_4;
        data->mdb_item_0_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_RMEP_EM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_32_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_RMEP_EM, 0);
        data->dbal_table = DBAL_TABLE_MDB_32_RMEP;
        data->mdb_item_0_field_size = 9;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_mdb_41_info_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_em_mdb_41_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_41_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "1";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_41_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_41_info");

    
    default_data = (dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->array_size = 1;
    default_data->mdb_item_2_field_size = 0;
    default_data->mdb_item_3_field_size = 0;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_ISEM_1;
        data->array_size = 10;
        data->mdb_item_2_field_size = 10;
        data->mdb_item_3_field_size = 10;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_ISEM_2;
        data->array_size = 10;
        data->mdb_item_2_field_size = 10;
        data->mdb_item_3_field_size = 10;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_ISEM_3;
        data->array_size = 10;
        data->mdb_item_2_field_size = 10;
        data->mdb_item_3_field_size = 10;
    }
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_LEM;
        data->array_size = 11;
        data->mdb_item_2_field_size = 11;
        data->mdb_item_3_field_size = 11;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_IOEM_0;
        data->array_size = 9;
        data->mdb_item_2_field_size = 9;
        data->mdb_item_3_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_IOEM_1;
        data->array_size = 9;
        data->mdb_item_2_field_size = 9;
        data->mdb_item_3_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_PPMC < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_PPMC, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_MC_ID;
        data->array_size = 9;
        data->mdb_item_2_field_size = 9;
        data->mdb_item_3_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_GLEM_0;
        data->array_size = 9;
        data->mdb_item_2_field_size = 9;
        data->mdb_item_3_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_GLEM_1;
        data->array_size = 9;
        data->mdb_item_2_field_size = 9;
        data->mdb_item_3_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_EOEM_0;
        data->array_size = 9;
        data->mdb_item_2_field_size = 9;
        data->mdb_item_3_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_EOEM_1;
        data->array_size = 9;
        data->mdb_item_2_field_size = 9;
        data->mdb_item_3_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ESEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_ESEM;
        data->array_size = 9;
        data->mdb_item_2_field_size = 9;
        data->mdb_item_3_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_EXEM_1;
        data->array_size = 9;
        data->mdb_item_2_field_size = 9;
        data->mdb_item_3_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_EXEM_2;
        data->array_size = 9;
        data->mdb_item_2_field_size = 9;
        data->mdb_item_3_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_EXEM_3;
        data->array_size = 9;
        data->mdb_item_2_field_size = 9;
        data->mdb_item_3_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_EXEM_4;
        data->array_size = 9;
        data->mdb_item_2_field_size = 9;
        data->mdb_item_3_field_size = 9;
    }
    if (DBAL_PHYSICAL_TABLE_RMEP_EM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_RMEP_EM, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_RMEP;
        data->array_size = 9;
        data->mdb_item_2_field_size = 9;
        data->mdb_item_3_field_size = 9;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_mdb_45_info_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_em_mdb_45_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_45_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "1";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_45_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_45_info");

    
    default_data = (dnx_data_mdb_em_mdb_45_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->array_size = 1;
    default_data->mdb_item_3_field_size = 0;
    default_data->mdb_item_7_field_size = 0;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_em_mdb_45_info_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_45_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_45_LEM;
        data->array_size = 3;
        data->mdb_item_3_field_size = 257;
        data->mdb_item_7_field_size = 257;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_45_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_45_EXEM_3;
        data->array_size = 2;
        data->mdb_item_3_field_size = 137;
        data->mdb_item_7_field_size = 137;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_45_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_45_EXEM_4;
        data->array_size = 2;
        data->mdb_item_3_field_size = 257;
        data->mdb_item_7_field_size = 257;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_mdb_emp_tables_mapping_set(
    int unit)
{
    int mdb_table_index;
    dnx_data_mdb_em_mdb_emp_tables_mapping_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_emp_tables_mapping;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_emp_tables_mapping_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_emp_tables_mapping");

    
    default_data = (dnx_data_mdb_em_mdb_emp_tables_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->emp_age_cfg_table = 0;
    default_data->mdb_22_table = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_emp_tables_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_emp_tables_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_LEM, 0);
        data->emp_age_cfg_table = DBAL_TABLE_MDB_19_LEM;
        data->mdb_22_table = DBAL_TABLE_MDB_22_LEM;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_emp_tables_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->emp_age_cfg_table = DBAL_TABLE_MDB_19_EXEM_3;
        data->mdb_22_table = DBAL_TABLE_MDB_22_EXEM_3;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_emp_tables_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_4, 0);
        data->emp_age_cfg_table = DBAL_TABLE_MDB_19_EXEM_4;
        data->mdb_22_table = DBAL_TABLE_MDB_22_EXEM_4;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_mdb_em_shift_vmv_regs_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_em_shift_vmv_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_em_shift_vmv_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_em_shift_vmv_regs");

    
    default_data = (dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nof_valid_regs = 0;
    default_data->table_name_arr[0] = 0;
    default_data->field_name_arr[0] = 0;
    default_data->key_name_arr[0] = 0;
    default_data->stage_index_arr[0] = 0;
    default_data->esem_cmd_indication[0] = 0;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_1, 0);
        data->nof_valid_regs = 2;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_name_arr[0] = DBAL_TABLE_MDB_VTT_ST_ONE_LE_VMV_SHIFT;
        data->table_name_arr[1] = DBAL_TABLE_MDB_VTT_ST_ONE_LE_VMV_SHIFT;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->field_name_arr[0] = DBAL_FIELD_VTT_ST_ONE_LE_VMV_SHIFT_VALUE_MAP;
        data->field_name_arr[1] = DBAL_FIELD_VTT_ST_ONE_LE_VMV_SHIFT_VALUE_MAP;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_name_arr[0] = DBAL_FIELD_STAGE_INDEX;
        data->key_name_arr[1] = DBAL_FIELD_STAGE_INDEX;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->stage_index_arr[0] = 1;
        data->stage_index_arr[1] = 2;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_2, 0);
        data->nof_valid_regs = 4;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_name_arr[0] = DBAL_TABLE_MDB_VTT_ST_TWO_LE_VMV_SHIFT;
        data->table_name_arr[1] = DBAL_TABLE_MDB_VTT_ST_TWO_LE_VMV_SHIFT;
        data->table_name_arr[2] = DBAL_TABLE_MDB_VTT_ST_TH_LE_VMV_SHIFT;
        data->table_name_arr[3] = DBAL_TABLE_MDB_VTT_ST_TH_LE_VMV_SHIFT;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->field_name_arr[0] = DBAL_FIELD_VTT_ST_TWO_LE_VMV_SHIFT_VALUE_MAP;
        data->field_name_arr[1] = DBAL_FIELD_VTT_ST_TWO_LE_VMV_SHIFT_VALUE_MAP;
        data->field_name_arr[2] = DBAL_FIELD_VTT_ST_TH_LE_VMV_SHIFT_VALUE_MAP;
        data->field_name_arr[3] = DBAL_FIELD_VTT_ST_TH_LE_VMV_SHIFT_VALUE_MAP;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_name_arr[0] = DBAL_FIELD_STAGE_INDEX;
        data->key_name_arr[1] = DBAL_FIELD_STAGE_INDEX;
        data->key_name_arr[2] = DBAL_FIELD_STAGE_INDEX;
        data->key_name_arr[3] = DBAL_FIELD_STAGE_INDEX;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->stage_index_arr[0] = 1;
        data->stage_index_arr[1] = 2;
        data->stage_index_arr[2] = 1;
        data->stage_index_arr[3] = 2;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_3, 0);
        data->nof_valid_regs = 0;
    }
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->nof_valid_regs = 1;
        data->table_name_arr[0] = DBAL_TABLE_MDB_FLPLE_VMV_SHIFT;
        data->field_name_arr[0] = DBAL_FIELD_FLPLE_VMV_SHIFT_VALUE;
        data->key_name_arr[0] = DBAL_FIELD_STAGE_INDEX;
        data->stage_index_arr[0] = 0;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_1, 0);
        data->nof_valid_regs = 1;
        data->table_name_arr[0] = DBAL_TABLE_MDB_VMV_SHIFT;
        data->field_name_arr[0] = DBAL_FIELD_IPPB_OAM_VMV_SHIFT_VALUE;
        data->key_name_arr[0] = DBAL_FIELD_EMPTY;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_2, 0);
        data->nof_valid_regs = 0;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->nof_valid_regs = 1;
        data->table_name_arr[0] = DBAL_TABLE_MDB_VMV_SHIFT;
        data->field_name_arr[0] = DBAL_FIELD_ETPPC_OAM_VMV_SHIFT_VALUE;
        data->key_name_arr[0] = DBAL_FIELD_EMPTY;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->nof_valid_regs = 0;
    }
    if (DBAL_PHYSICAL_TABLE_PPMC < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_PPMC, 0);
        data->nof_valid_regs = 0;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_1, 0);
        data->nof_valid_regs = 0;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_2, 0);
        data->nof_valid_regs = 0;
    }
    if (DBAL_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ESEM, 0);
        data->nof_valid_regs = 2;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_name_arr[0] = DBAL_TABLE_MDB_VMV_SHIFT;
        data->table_name_arr[1] = DBAL_TABLE_MDB_VMV_SHIFT;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->field_name_arr[0] = DBAL_FIELD_ESEM_1_SHIFT_VAL;
        data->field_name_arr[1] = DBAL_FIELD_ESEM_2_SHIFT_VAL;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_name_arr[0] = DBAL_FIELD_EMPTY;
        data->key_name_arr[1] = DBAL_FIELD_EMPTY;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->stage_index_arr[0] = 0;
        data->stage_index_arr[1] = 0;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->esem_cmd_indication[0] = 1;
        data->esem_cmd_indication[1] = 1;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_1, 0);
        data->nof_valid_regs = 2;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_name_arr[0] = DBAL_TABLE_MDB_VTT_ST_ONE_LE_VMV_SHIFT;
        data->table_name_arr[1] = DBAL_TABLE_MDB_VTT_ST_TWO_LE_VMV_SHIFT;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->field_name_arr[0] = DBAL_FIELD_VTT_ST_ONE_LE_VMV_SHIFT_VALUE_MAP;
        data->field_name_arr[1] = DBAL_FIELD_VTT_ST_TWO_LE_VMV_SHIFT_VALUE_MAP;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_name_arr[0] = DBAL_FIELD_STAGE_INDEX;
        data->key_name_arr[1] = DBAL_FIELD_STAGE_INDEX;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->stage_index_arr[0] = 0;
        data->stage_index_arr[1] = 0;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_2, 0);
        data->nof_valid_regs = 2;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_name_arr[0] = DBAL_TABLE_MDB_FLPLE_VMV_SHIFT;
        data->table_name_arr[1] = DBAL_TABLE_MDB_VMV_SHIFT;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->field_name_arr[0] = DBAL_FIELD_FLPLE_VMV_SHIFT_VALUE;
        data->field_name_arr[1] = DBAL_FIELD_ESEM_3_SHIFT_VAL;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_name_arr[0] = DBAL_FIELD_STAGE_INDEX;
        data->key_name_arr[1] = DBAL_FIELD_EMPTY;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->stage_index_arr[0] = 2;
        data->stage_index_arr[1] = 0;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->esem_cmd_indication[0] = 0;
        data->esem_cmd_indication[1] = 1;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->nof_valid_regs = 2;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_name_arr[0] = DBAL_TABLE_MDB_FLPLE_VMV_SHIFT;
        data->table_name_arr[1] = DBAL_TABLE_MDB_VTT_ST_TH_LE_VMV_SHIFT;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->field_name_arr[0] = DBAL_FIELD_FLPLE_VMV_SHIFT_VALUE;
        data->field_name_arr[1] = DBAL_FIELD_VTT_ST_TH_LE_VMV_SHIFT_VALUE_MAP;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->key_name_arr[0] = DBAL_FIELD_STAGE_INDEX;
        data->key_name_arr[1] = DBAL_FIELD_STAGE_INDEX;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->stage_index_arr[0] = 1;
        data->stage_index_arr[1] = 0;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->nof_valid_regs = 0;
    }
    if (DBAL_PHYSICAL_TABLE_RMEP_EM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_RMEP_EM, 0);
        data->nof_valid_regs = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_em_emp_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_em_em_emp_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_em_emp;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "INVALIDm";
    table->values[1].default_val = "INVALIDm";
    table->values[2].default_val = "INVALIDf";
    table->values[3].default_val = "INVALIDm";
    table->values[4].default_val = "INVALIDm";
    table->values[5].default_val = "INVALIDm";
    table->values[6].default_val = "INVALIDm";
    table->values[7].default_val = "INVALIDm";
    table->values[8].default_val = "INVALIDm";
    table->values[9].default_val = "INVALIDm";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_em_emp_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_em_emp");

    
    default_data = (dnx_data_mdb_em_em_emp_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->age_mem = INVALIDm;
    default_data->age_ovf_cam_mem = INVALIDm;
    default_data->emp_scan_status_field = INVALIDf;
    default_data->emp_age_cfg_reg = INVALIDm;
    default_data->global_value = INVALIDm;
    default_data->global_masks = INVALIDm;
    default_data->external_profile = INVALIDm;
    default_data->age_disable = INVALIDm;
    default_data->elephant_disable = INVALIDm;
    default_data->initial_values = INVALIDm;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_em_em_emp_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_emp_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->age_mem = MDB_MEM_0200000m;
        data->age_ovf_cam_mem = MDB_MEM_25000m;
        data->emp_scan_status_field = ITEM_36_37f;
        data->emp_age_cfg_reg = MDB_REG_181r;
        data->global_value = ITEM_224_287f;
        data->global_masks = ITEM_288_351f;
        data->external_profile = ITEM_352_359f;
        data->age_disable = ITEM_360_367f;
        data->elephant_disable = ITEM_368_375f;
        data->initial_values = ITEM_192_223f;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_emp_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->age_mem = MDB_MEM_210000m;
        data->age_ovf_cam_mem = MDB_MEM_94000m;
        data->emp_scan_status_field = ITEM_36_37f;
        data->emp_age_cfg_reg = MDB_REG_436r;
        data->global_value = ITEM_448_511f;
        data->global_masks = ITEM_512_575f;
        data->external_profile = ITEM_576_583f;
        data->age_disable = ITEM_584_591f;
        data->elephant_disable = ITEM_592_599f;
        data->initial_values = ITEM_384_447f;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_emp_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->age_mem = MDB_MEM_220000m;
        data->age_ovf_cam_mem = MDB_MEM_103000m;
        data->emp_scan_status_field = ITEM_36_37f;
        data->emp_age_cfg_reg = MDB_REG_489r;
        data->global_value = ITEM_448_511f;
        data->global_masks = ITEM_512_575f;
        data->external_profile = ITEM_576_583f;
        data->age_disable = ITEM_584_591f;
        data->elephant_disable = ITEM_592_599f;
        data->initial_values = ITEM_384_447f;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_mdb_step_table_map_set(
    int unit)
{
    int mdb_table_index;
    dnx_data_mdb_em_mdb_step_table_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_step_table_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_step_table_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_step_table_map");

    
    default_data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_ISEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_ISEM_2;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_ISEM_3;
    }
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_LEM;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_IOEM_0;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_IOEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_PPMC < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_PPMC, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_MC_ID;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_GLEM_0;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_GLEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_EOEM_0;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_EOEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ESEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_ESEM;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_EXEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_EXEM_2;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_EXEM_3;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_EXEM_4;
    }
    if (DBAL_PHYSICAL_TABLE_RMEP_EM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_RMEP_EM, 0);
        data->dbal_table = DBAL_TABLE_MDB_31_RMEP;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "DBAL_TABLE_EMPTY";
    table->values[1].default_val = "DBAL_TABLE_EMPTY";
    table->values[2].default_val = "DBAL_TABLE_EMPTY";
    table->values[3].default_val = "DBAL_TABLE_EMPTY";
    table->values[4].default_val = "DBAL_TABLE_EMPTY";
    table->values[5].default_val = "DBAL_TABLE_EMPTY";
    table->values[6].default_val = "DBAL_TABLE_EMPTY";
    table->values[7].default_val = "DBAL_TABLE_EMPTY";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info");

    
    default_data = (dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->mdb_13_table = DBAL_TABLE_EMPTY;
    default_data->mdb_14_table = DBAL_TABLE_EMPTY;
    default_data->mdb_17_table = DBAL_TABLE_EMPTY;
    default_data->mdb_18_table = DBAL_TABLE_EMPTY;
    default_data->mdb_33_table = DBAL_TABLE_EMPTY;
    default_data->mdb_34_table = DBAL_TABLE_EMPTY;
    default_data->mdb_46_table = DBAL_TABLE_EMPTY;
    default_data->mdb_52_table = DBAL_TABLE_EMPTY;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_1, 0);
        data->mdb_13_table = DBAL_TABLE_MDB_13_ISEM_1;
        data->mdb_14_table = DBAL_TABLE_MDB_14_ISEM_1;
        data->mdb_17_table = DBAL_TABLE_MDB_17_ISEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_2, 0);
        data->mdb_13_table = DBAL_TABLE_MDB_13_ISEM_2;
        data->mdb_14_table = DBAL_TABLE_MDB_14_ISEM_2;
        data->mdb_17_table = DBAL_TABLE_MDB_17_ISEM_2;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_3, 0);
        data->mdb_13_table = DBAL_TABLE_MDB_13_ISEM_3;
        data->mdb_14_table = DBAL_TABLE_MDB_14_ISEM_3;
        data->mdb_17_table = DBAL_TABLE_MDB_17_ISEM_3;
    }
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->mdb_13_table = DBAL_TABLE_MDB_13_LEM;
        data->mdb_14_table = DBAL_TABLE_MDB_14_LEM;
        data->mdb_17_table = DBAL_TABLE_MDB_17_LEM;
        data->mdb_18_table = DBAL_TABLE_MDB_18_LEM;
        data->mdb_33_table = DBAL_TABLE_MDB_33_LEM;
        data->mdb_46_table = DBAL_TABLE_MDB_46_LEM;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_1, 0);
        data->mdb_13_table = DBAL_TABLE_MDB_13_IOEM_0;
        data->mdb_14_table = DBAL_TABLE_MDB_14_IOEM_0;
        data->mdb_17_table = DBAL_TABLE_MDB_17_IOEM_0;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_2, 0);
        data->mdb_13_table = DBAL_TABLE_MDB_13_IOEM_1;
        data->mdb_14_table = DBAL_TABLE_MDB_14_IOEM_1;
        data->mdb_17_table = DBAL_TABLE_MDB_17_IOEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_PPMC < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_PPMC, 0);
        data->mdb_13_table = DBAL_TABLE_MDB_13_MC_ID;
        data->mdb_14_table = DBAL_TABLE_MDB_14_MC_ID;
        data->mdb_17_table = DBAL_TABLE_MDB_17_MC_ID;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_1, 0);
        data->mdb_13_table = DBAL_TABLE_MDB_13_GLEM_0;
        data->mdb_14_table = DBAL_TABLE_MDB_14_GLEM_0;
        data->mdb_17_table = DBAL_TABLE_MDB_17_GLEM_0;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_2, 0);
        data->mdb_13_table = DBAL_TABLE_MDB_13_GLEM_1;
        data->mdb_14_table = DBAL_TABLE_MDB_14_GLEM_1;
        data->mdb_17_table = DBAL_TABLE_MDB_17_GLEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->mdb_13_table = DBAL_TABLE_MDB_13_EOEM_0;
        data->mdb_14_table = DBAL_TABLE_MDB_14_EOEM_0;
        data->mdb_17_table = DBAL_TABLE_MDB_17_EOEM_0;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->mdb_13_table = DBAL_TABLE_MDB_13_EOEM_1;
        data->mdb_14_table = DBAL_TABLE_MDB_14_EOEM_1;
        data->mdb_17_table = DBAL_TABLE_MDB_17_EOEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ESEM, 0);
        data->mdb_13_table = DBAL_TABLE_MDB_13_ESEM;
        data->mdb_14_table = DBAL_TABLE_MDB_14_ESEM;
        data->mdb_17_table = DBAL_TABLE_MDB_17_ESEM;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_1, 0);
        data->mdb_13_table = DBAL_TABLE_MDB_13_EXEM_1;
        data->mdb_14_table = DBAL_TABLE_MDB_14_EXEM_1;
        data->mdb_17_table = DBAL_TABLE_MDB_17_EXEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_2, 0);
        data->mdb_13_table = DBAL_TABLE_MDB_13_EXEM_2;
        data->mdb_14_table = DBAL_TABLE_MDB_14_EXEM_2;
        data->mdb_17_table = DBAL_TABLE_MDB_17_EXEM_2;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->mdb_13_table = DBAL_TABLE_MDB_13_EXEM_3;
        data->mdb_14_table = DBAL_TABLE_MDB_14_EXEM_3;
        data->mdb_17_table = DBAL_TABLE_MDB_17_EXEM_3;
        data->mdb_18_table = DBAL_TABLE_MDB_18_EXEM_3;
        data->mdb_33_table = DBAL_TABLE_MDB_33_EXEM_3;
        data->mdb_46_table = DBAL_TABLE_MDB_46_EXEM_3;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->mdb_13_table = DBAL_TABLE_MDB_13_EXEM_4;
        data->mdb_14_table = DBAL_TABLE_MDB_14_EXEM_4;
        data->mdb_17_table = DBAL_TABLE_MDB_17_EXEM_4;
        data->mdb_18_table = DBAL_TABLE_MDB_18_EXEM_4;
        data->mdb_33_table = DBAL_TABLE_MDB_33_EXEM_4;
        data->mdb_46_table = DBAL_TABLE_MDB_46_EXEM_4;
    }
    if (DBAL_PHYSICAL_TABLE_RMEP_EM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_RMEP_EM, 0);
        data->mdb_13_table = DBAL_TABLE_MDB_13_RMEP;
        data->mdb_14_table = DBAL_TABLE_MDB_14_RMEP;
        data->mdb_17_table = DBAL_TABLE_MDB_17_RMEP;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_filter_rules_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_em_filter_rules_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_filter_rules;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    table->values[6].default_val = "0";
    table->values[7].default_val = "0";
    table->values[8].default_val = "0";
    table->values[9].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_filter_rules_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_filter_rules");

    
    default_data = (dnx_data_mdb_em_filter_rules_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->table_name = 0;
    default_data->valid = 0;
    default_data->entry_mask = 0;
    default_data->entry_filter = 0;
    default_data->src_mask = 0;
    default_data->src_filter = 0;
    default_data->appdb_id_mask = 0;
    default_data->appdb_id_filter = 0;
    default_data->accessed_mask = 0;
    default_data->accessed_filter = 0;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_em_filter_rules_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_filter_rules_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->table_name = MDB_MEM_23000m;
        data->valid = ITEM_0_0f;
        data->entry_mask = ITEM_8_264f;
        data->entry_filter = ITEM_275_531f;
        data->src_mask = ITEM_265_267f;
        data->src_filter = ITEM_532_534f;
        data->appdb_id_mask = ITEM_2_7f;
        data->appdb_id_filter = ITEM_269_274f;
        data->accessed_mask = ITEM_1_1f;
        data->accessed_filter = ITEM_268_268f;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_data_rules_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_em_data_rules_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_data_rules;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_data_rules_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_data_rules");

    
    default_data = (dnx_data_mdb_em_data_rules_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->table_name = 0;
    default_data->command = 0;
    default_data->payload_mask = 0;
    default_data->payload = 0;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_em_data_rules_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_data_rules_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->table_name = MDB_MEM_24000m;
        data->command = ITEM_0_3f;
        data->payload_mask = ITEM_4_99f;
        data->payload = ITEM_100_195f;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_mdb_direct_fec_abk_mapping_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int feature_index = dnx_data_mdb_direct_fec_abk_mapping;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_direct_fec_abk_macro_mapping_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int feature_index = dnx_data_mdb_direct_fec_abk_macro_mapping;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_direct_vsi_config_size_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int feature_index = dnx_data_mdb_direct_vsi_config_size;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_mdb_direct_physical_address_max_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int define_index = dnx_data_mdb_direct_define_physical_address_max_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20;

    
    define->data = 20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_direct_physical_address_max_bits_support_single_macro_b_granularity_fec_alloc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int define_index = dnx_data_mdb_direct_define_physical_address_max_bits_support_single_macro_b_granularity_fec_alloc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 19;

    
    define->data = 19;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_direct_vsi_physical_address_max_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int define_index = dnx_data_mdb_direct_define_vsi_physical_address_max_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 18;

    
    define->data = 18;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_direct_fec_row_width_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int define_index = dnx_data_mdb_direct_define_fec_row_width;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);

    
    define->data = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_direct_fec_address_mapping_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int define_index = dnx_data_mdb_direct_define_fec_address_mapping_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_direct_fec_address_mapping_bits_relevant_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int define_index = dnx_data_mdb_direct_define_fec_address_mapping_bits_relevant;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_direct_fec_max_cluster_pairs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int define_index = dnx_data_mdb_direct_define_fec_max_cluster_pairs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_direct_max_payload_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int define_index = dnx_data_mdb_direct_define_max_payload_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 180;

    
    define->data = 180;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_direct_nof_fecs_in_super_fec_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int define_index = dnx_data_mdb_direct_define_nof_fecs_in_super_fec;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_direct_nof_fec_ecmp_hierarchies_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int define_index = dnx_data_mdb_direct_define_nof_fec_ecmp_hierarchies;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_direct_physical_address_ovf_enable_val_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int define_index = dnx_data_mdb_direct_define_physical_address_ovf_enable_val;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_direct_nof_fec_dbs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int define_index = dnx_data_mdb_direct_define_nof_fec_dbs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit);

    
    define->data = dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_direct_map_payload_size_enum_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int define_index = dnx_data_mdb_direct_define_map_payload_size_enum;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_30B;

    
    define->data = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_30B;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_PMF_MAPS_PAYLOAD_SIZE;
    define->property.doc = 
        "\n"
        "pmf_maps_payload_size determines the payload size for PMF MAPS DB,\n"
        "user can choose between 30/60/120 bits payload size values.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 3;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "30";
    define->property.mapping[0].val = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_30B;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "60";
    define->property.mapping[1].val = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_60B;
    define->property.mapping[2].name = "120";
    define->property.mapping[2].val = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_direct_map_payload_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int define_index = dnx_data_mdb_direct_define_map_payload_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 30;

    
    define->data = 30;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_PMF_MAPS_PAYLOAD_SIZE;
    define->property.doc = 
        "\n"
        "pmf_maps_payload_size determines the payload size for PMF MAPS DB,\n"
        "user can choose between 30/60/120 bits payload size values.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 3;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "30";
    define->property.mapping[0].val = 30;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "60";
    define->property.mapping[1].val = 60;
    define->property.mapping[2].name = "120";
    define->property.mapping[2].val = 120;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_direct_fec_allocation_base_macro_type_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int define_index = dnx_data_mdb_direct_define_fec_allocation_base_macro_type;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = MDB_MACRO_B;

    
    define->data = MDB_MACRO_B;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_mdb_direct_direct_utilization_api_db_translation_set(
    int unit)
{
    int resource_id_index;
    dnx_data_mdb_direct_direct_utilization_api_db_translation_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int table_index = dnx_data_mdb_direct_table_direct_utilization_api_db_translation;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_mdb.global.utilization_api_nof_dbs_get(unit);
    table->info_get.key_size[0] = dnx_data_mdb.global.utilization_api_nof_dbs_get(unit);

    
    table->values[0].default_val = "DBAL_NOF_PHYSICAL_TABLES";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_direct_direct_utilization_api_db_translation_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_direct_table_direct_utilization_api_db_translation");

    
    default_data = (dnx_data_mdb_direct_direct_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_id = DBAL_NOF_PHYSICAL_TABLES;
    
    for (resource_id_index = 0; resource_id_index < table->keys[0].size; resource_id_index++)
    {
        data = (dnx_data_mdb_direct_direct_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, resource_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_INLIF_1;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_INLIF_2;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_INLIF_3;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_FEC_1;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_FEC_2;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_FEC_3;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_IVSI;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_EVSI;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_MAP;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_EEDB_1;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_EEDB_2;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_EEDB_3;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_EEDB_4;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_EEDB_5;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_EEDB_6;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_EEDB_7;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_utilization_api_db_translation_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_EEDB_8;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_direct_direct_info_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_direct_direct_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int table_index = dnx_data_mdb_direct_table_direct_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_direct_direct_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_direct_table_direct_info");

    
    default_data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_KAPS_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_KAPS_1, 0);
        data->payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (DBAL_PHYSICAL_TABLE_KAPS_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_KAPS_2, 0);
        data->payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (DBAL_PHYSICAL_TABLE_INLIF_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_INLIF_1, 0);
        data->payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_60B;
    }
    if (DBAL_PHYSICAL_TABLE_IVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IVSI, 0);
        data->payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_90B;
    }
    if (DBAL_PHYSICAL_TABLE_INLIF_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_INLIF_2, 0);
        data->payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_60B;
    }
    if (DBAL_PHYSICAL_TABLE_INLIF_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_INLIF_3, 0);
        data->payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_60B;
    }
    if (DBAL_PHYSICAL_TABLE_MAP < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_MAP, 0);
        data->payload_type = dnx_data_mdb.direct.map_payload_size_enum_get(unit);
    }
    if (DBAL_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_FEC_1, 0);
        data->payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_150B;
    }
    if (DBAL_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_FEC_2, 0);
        data->payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_150B;
    }
    if (DBAL_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_FEC_3, 0);
        data->payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_150B;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_1, 0);
        data->payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_2, 0);
        data->payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_3, 0);
        data->payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_4, 0);
        data->payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_5 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_5, 0);
        data->payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_6 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_6, 0);
        data->payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_7 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_7, 0);
        data->payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_8 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_8, 0);
        data->payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (DBAL_PHYSICAL_TABLE_EVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EVSI, 0);
        data->payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_30B;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_direct_hit_bit_pos_in_abk_set(
    int unit)
{
    int hit_bit_index_index;
    dnx_data_mdb_direct_hit_bit_pos_in_abk_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int table_index = dnx_data_mdb_direct_table_hit_bit_pos_in_abk;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_direct_hit_bit_pos_in_abk_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_direct_table_hit_bit_pos_in_abk");

    
    default_data = (dnx_data_mdb_direct_hit_bit_pos_in_abk_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->hit_bit_position = 0;
    
    for (hit_bit_index_index = 0; hit_bit_index_index < table->keys[0].size; hit_bit_index_index++)
    {
        data = (dnx_data_mdb_direct_hit_bit_pos_in_abk_t *) dnxc_data_mgmt_table_data_get(unit, table, hit_bit_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_hit_bit_pos_in_abk_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->hit_bit_position = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_hit_bit_pos_in_abk_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->hit_bit_position = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_direct_mdb_11_info_set(
    int unit)
{
    int mdb_table_id_index;
    dnx_data_mdb_direct_mdb_11_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int table_index = dnx_data_mdb_direct_table_mdb_11_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "1";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "1";
    table->values[5].default_val = "0";
    table->values[6].default_val = "1";
    table->values[7].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_direct_mdb_11_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_direct_table_mdb_11_info");

    
    default_data = (dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->mdb_item_0_array_size = 1;
    default_data->mdb_item_0_field_size = 0;
    default_data->mdb_item_1_field_size = 0;
    default_data->mdb_item_3_array_size = 1;
    default_data->mdb_item_3_field_size = 0;
    default_data->mdb_item_4_array_size = 1;
    default_data->mdb_item_4_field_size = 0;
    
    for (mdb_table_id_index = 0; mdb_table_id_index < table->keys[0].size; mdb_table_id_index++)
    {
        data = (dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_PHYSICAL_TABLE_INLIF_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_INLIF_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_INLIF_1;
        data->mdb_item_0_array_size = 16;
        data->mdb_item_0_field_size = 3;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_INLIF_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_INLIF_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_INLIF_2;
        data->mdb_item_0_array_size = 16;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_INLIF_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_INLIF_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_INLIF_3;
        data->mdb_item_0_array_size = 16;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_IVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_IVSI, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_IVSI;
        data->mdb_item_0_array_size = 16;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_ADS_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ADS_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_ADS_1;
        data->mdb_item_0_array_size = 8;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_ADS_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ADS_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_ADS_2;
        data->mdb_item_0_array_size = 8;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_MAP < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_MAP, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_MAP;
        data->mdb_item_0_array_size = 16;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_FEC_1;
        data->mdb_item_0_array_size = 32;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_FEC_2;
        data->mdb_item_0_array_size = 32;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_FEC_3;
        data->mdb_item_0_array_size = 32;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_1_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_1_LL, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_EEDB_1_LL;
        data->mdb_item_0_array_size = 64;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_1_2_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_1_2_DATA, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_EEDB_1_2_DATA;
        data->mdb_item_0_array_size = 128;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 2;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 2;
        data->mdb_item_4_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_3_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_3_LL, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_EEDB_3_LL;
        data->mdb_item_0_array_size = 64;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_3_4_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_3_4_DATA, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_EEDB_3_4_DATA;
        data->mdb_item_0_array_size = 128;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 2;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 2;
        data->mdb_item_4_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_5_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_5_LL, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_EEDB_5_LL;
        data->mdb_item_0_array_size = 64;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_5_6_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_5_6_DATA, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_EEDB_5_6_DATA;
        data->mdb_item_0_array_size = 128;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 2;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 2;
        data->mdb_item_4_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_7_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_7_LL, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_EEDB_7_LL;
        data->mdb_item_0_array_size = 64;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_7_8_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_7_8_DATA, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_EEDB_7_8_DATA;
        data->mdb_item_0_array_size = 128;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 2;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 2;
        data->mdb_item_4_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_EVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EVSI, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_EVSI;
        data->mdb_item_0_array_size = 16;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_direct_mdb_26_info_set(
    int unit)
{
    int mdb_table_id_index;
    dnx_data_mdb_direct_mdb_26_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int table_index = dnx_data_mdb_direct_table_mdb_26_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_direct_mdb_26_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_direct_table_mdb_26_info");

    
    default_data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->mdb_item_0_field_size = 0;
    default_data->mdb_item_1_field_size = 0;
    
    for (mdb_table_id_index = 0; mdb_table_id_index < table->keys[0].size; mdb_table_id_index++)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_ISEM_1;
        data->mdb_item_0_field_size = 8;
        data->mdb_item_1_field_size = 8;
    }
    if (MDB_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_ISEM_2;
        data->mdb_item_0_field_size = 7;
        data->mdb_item_1_field_size = 7;
    }
    if (MDB_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_ISEM_3;
        data->mdb_item_0_field_size = 7;
        data->mdb_item_1_field_size = 7;
    }
    if (MDB_PHYSICAL_TABLE_INLIF_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_INLIF_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_INLIF_1;
        data->mdb_item_0_field_size = 6;
        data->mdb_item_1_field_size = 6;
    }
    if (MDB_PHYSICAL_TABLE_INLIF_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_INLIF_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_INLIF_2;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 1;
    }
    if (MDB_PHYSICAL_TABLE_INLIF_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_INLIF_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_INLIF_3;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 2;
    }
    if (MDB_PHYSICAL_TABLE_IVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_IVSI, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_IVSI;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 2;
    }
    if (MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_LEM;
        data->mdb_item_0_field_size = 11;
        data->mdb_item_1_field_size = 11;
    }
    if (MDB_PHYSICAL_TABLE_ADS_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ADS_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_ADS_1;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 2;
    }
    if (MDB_PHYSICAL_TABLE_ADS_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ADS_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_ADS_2;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 1;
    }
    if (MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_KAPS_1;
        data->mdb_item_0_field_size = 11;
        data->mdb_item_1_field_size = 11;
    }
    if (MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_KAPS_2;
        data->mdb_item_0_field_size = 6;
        data->mdb_item_1_field_size = 6;
    }
    if (MDB_PHYSICAL_TABLE_IOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_IOEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_IOEM_0;
        data->mdb_item_0_field_size = 4;
        data->mdb_item_1_field_size = 4;
    }
    if (MDB_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_IOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_IOEM_1;
        data->mdb_item_0_field_size = 4;
        data->mdb_item_1_field_size = 4;
    }
    if (MDB_PHYSICAL_TABLE_EOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EOEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EOEM_0;
        data->mdb_item_0_field_size = 4;
        data->mdb_item_1_field_size = 4;
    }
    if (MDB_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EOEM_1;
        data->mdb_item_0_field_size = 4;
        data->mdb_item_1_field_size = 4;
    }
    if (MDB_PHYSICAL_TABLE_MAP < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_MAP, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_MAP;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 2;
    }
    if (MDB_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_FEC_1;
        data->mdb_item_0_field_size = 3;
        data->mdb_item_1_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_FEC_2;
        data->mdb_item_0_field_size = 3;
        data->mdb_item_1_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_FEC_3;
        data->mdb_item_0_field_size = 4;
        data->mdb_item_1_field_size = 4;
    }
    if (MDB_PHYSICAL_TABLE_MC_ID < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_MC_ID, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_MC_ID;
        data->mdb_item_0_field_size = 5;
        data->mdb_item_1_field_size = 5;
    }
    if (MDB_PHYSICAL_TABLE_GLEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_GLEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_GLEM_0;
        data->mdb_item_0_field_size = 6;
        data->mdb_item_1_field_size = 6;
    }
    if (MDB_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_GLEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_GLEM_1;
        data->mdb_item_0_field_size = 6;
        data->mdb_item_1_field_size = 6;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_1_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_1_LL, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EEDB_1_LL;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 1;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_1_2_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_1_2_DATA, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EEDB_1_2_DATA;
        data->mdb_item_0_field_size = 4;
        data->mdb_item_1_field_size = 4;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_3_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_3_LL, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EEDB_3_LL;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 1;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_3_4_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_3_4_DATA, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EEDB_3_4_DATA;
        data->mdb_item_0_field_size = 3;
        data->mdb_item_1_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_5_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_5_LL, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EEDB_5_LL;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 1;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_5_6_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_5_6_DATA, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EEDB_5_6_DATA;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 2;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_7_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_7_LL, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EEDB_7_LL;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 1;
    }
    if (MDB_PHYSICAL_TABLE_EEDB_7_8_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EEDB_7_8_DATA, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EEDB_7_8_DATA;
        data->mdb_item_0_field_size = 4;
        data->mdb_item_1_field_size = 4;
    }
    if (MDB_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ESEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_ESEM;
        data->mdb_item_0_field_size = 5;
        data->mdb_item_1_field_size = 5;
    }
    if (MDB_PHYSICAL_TABLE_EVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EVSI, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EVSI;
        data->mdb_item_0_field_size = 3;
        data->mdb_item_1_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EXEM_1;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 2;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EXEM_2;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 2;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EXEM_3;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 2;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_4, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EXEM_4;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 2;
    }
    if (MDB_PHYSICAL_TABLE_RMEP < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_RMEP, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_RMEP;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 2;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_set(
    int unit)
{
    int mdb_table_id_index;
    dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int table_index = dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "DBAL_TABLE_EMPTY";
    table->values[1].default_val = "DBAL_TABLE_EMPTY";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info");

    
    default_data = (dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->mdb_12_table = DBAL_TABLE_EMPTY;
    default_data->mdb_42_table = DBAL_TABLE_EMPTY;
    
    for (mdb_table_id_index = 0; mdb_table_id_index < table->keys[0].size; mdb_table_id_index++)
    {
        data = (dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_1, 0);
        data->mdb_12_table = DBAL_TABLE_MDB_12_FEC_1;
    }
    if (MDB_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_2, 0);
        data->mdb_12_table = DBAL_TABLE_MDB_12_FEC_2;
    }
    if (MDB_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_3, 0);
        data->mdb_12_table = DBAL_TABLE_MDB_12_FEC_3;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_mdb_eedb_eedb_bank_hitbit_masked_clear_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_eedb;
    int feature_index = dnx_data_mdb_eedb_eedb_bank_hitbit_masked_clear;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_mdb_eedb_phase_map_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_eedb;
    int define_index = dnx_data_mdb_eedb_define_phase_map_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_eedb_phase_map_array_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_eedb;
    int define_index = dnx_data_mdb_eedb_define_phase_map_array_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x80;

    
    define->data = 0x80;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_eedb_nof_phase_per_mag_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_eedb;
    int define_index = dnx_data_mdb_eedb_define_nof_phase_per_mag;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_eedb_nof_phases_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_eedb;
    int define_index = dnx_data_mdb_eedb_define_nof_phases;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_eedb_nof_eedb_mags_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_eedb;
    int define_index = dnx_data_mdb_eedb_define_nof_eedb_mags;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_mdb.eedb.nof_phases_get(unit)/dnx_data_mdb.eedb.nof_phase_per_mag_get(unit);

    
    define->data = dnx_data_mdb.eedb.nof_phases_get(unit)/dnx_data_mdb.eedb.nof_phase_per_mag_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_eedb_nof_eedb_banks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_eedb;
    int define_index = dnx_data_mdb_eedb_define_nof_eedb_banks;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_eedb_nof_phases_per_eedb_bank_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_eedb;
    int define_index = dnx_data_mdb_eedb_define_nof_phases_per_eedb_bank;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_eedb_nof_phases_per_eedb_bank_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_eedb;
    int define_index = dnx_data_mdb_eedb_define_nof_phases_per_eedb_bank_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_eedb_phase_bank_select_default_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_eedb;
    int define_index = dnx_data_mdb_eedb_define_phase_bank_select_default;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_eedb_entry_format_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_eedb;
    int define_index = dnx_data_mdb_eedb_define_entry_format_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_eedb_entry_format_encoding_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_eedb;
    int define_index = dnx_data_mdb_eedb_define_entry_format_encoding_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_eedb_bank_id_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_eedb;
    int define_index = dnx_data_mdb_eedb_define_bank_id_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_eedb_entry_bank_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_eedb;
    int define_index = dnx_data_mdb_eedb_define_entry_bank;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = MDB_EEDB_ENTRY_BANKm;

    
    define->data = MDB_EEDB_ENTRY_BANKm;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_eedb_abk_bank_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_eedb;
    int define_index = dnx_data_mdb_eedb_define_abk_bank;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = MDB_EEDB_ABK_BANKm;

    
    define->data = MDB_EEDB_ABK_BANKm;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_mdb_eedb_phase_info_set(
    int unit)
{
    int phase_number_index;
    dnx_data_mdb_eedb_phase_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_eedb;
    int table_index = dnx_data_mdb_eedb_table_phase_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_eedb_phase_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_eedb_table_phase_info");

    
    default_data = (dnx_data_mdb_eedb_phase_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nof_available_banks = 0;
    
    for (phase_number_index = 0; phase_number_index < table->keys[0].size; phase_number_index++)
    {
        data = (dnx_data_mdb_eedb_phase_info_t *) dnxc_data_mgmt_table_data_get(unit, table, phase_number_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_eedb_phase_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1, 0);
        data->nof_available_banks = 32;
    }
    if (DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_eedb_phase_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_2, 0);
        data->nof_available_banks = 4;
    }
    if (DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_eedb_phase_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_3, 0);
        data->nof_available_banks = 16;
    }
    if (DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_eedb_phase_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_4, 0);
        data->nof_available_banks = 4;
    }
    if (DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5 < table->keys[0].size)
    {
        data = (dnx_data_mdb_eedb_phase_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5, 0);
        data->nof_available_banks = 16;
    }
    if (DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_6 < table->keys[0].size)
    {
        data = (dnx_data_mdb_eedb_phase_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_6, 0);
        data->nof_available_banks = 8;
    }
    if (DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_7 < table->keys[0].size)
    {
        data = (dnx_data_mdb_eedb_phase_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_7, 0);
        data->nof_available_banks = 8;
    }
    if (DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_8 < table->keys[0].size)
    {
        data = (dnx_data_mdb_eedb_phase_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_8, 0);
        data->nof_available_banks = 8;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_eedb_phase_ll_bucket_enable_set(
    int unit)
{
    int phase_number_index;
    dnx_data_mdb_eedb_phase_ll_bucket_enable_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_eedb;
    int table_index = dnx_data_mdb_eedb_table_phase_ll_bucket_enable;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;

    
    table->values[0].default_val = "DBAL_NOF_TABLES";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_eedb_phase_ll_bucket_enable_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_eedb_table_phase_ll_bucket_enable");

    
    default_data = (dnx_data_mdb_eedb_phase_ll_bucket_enable_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->bucket_enable_dbal_table = DBAL_NOF_TABLES;
    
    for (phase_number_index = 0; phase_number_index < table->keys[0].size; phase_number_index++)
    {
        data = (dnx_data_mdb_eedb_phase_ll_bucket_enable_t *) dnxc_data_mgmt_table_data_get(unit, table, phase_number_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_eedb_phase_ll_bucket_enable_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1, 0);
        data->bucket_enable_dbal_table = DBAL_TABLE_MDB_26_EEDB_1_LL;
    }
    if (DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_eedb_phase_ll_bucket_enable_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_2, 0);
        data->bucket_enable_dbal_table = DBAL_NOF_TABLES;
    }
    if (DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_eedb_phase_ll_bucket_enable_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_3, 0);
        data->bucket_enable_dbal_table = DBAL_TABLE_MDB_26_EEDB_3_LL;
    }
    if (DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_eedb_phase_ll_bucket_enable_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_4, 0);
        data->bucket_enable_dbal_table = DBAL_NOF_TABLES;
    }
    if (DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5 < table->keys[0].size)
    {
        data = (dnx_data_mdb_eedb_phase_ll_bucket_enable_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5, 0);
        data->bucket_enable_dbal_table = DBAL_TABLE_MDB_26_EEDB_5_LL;
    }
    if (DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_6 < table->keys[0].size)
    {
        data = (dnx_data_mdb_eedb_phase_ll_bucket_enable_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_6, 0);
        data->bucket_enable_dbal_table = DBAL_NOF_TABLES;
    }
    if (DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_7 < table->keys[0].size)
    {
        data = (dnx_data_mdb_eedb_phase_ll_bucket_enable_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_7, 0);
        data->bucket_enable_dbal_table = DBAL_TABLE_MDB_26_EEDB_7_LL;
    }
    if (DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_8 < table->keys[0].size)
    {
        data = (dnx_data_mdb_eedb_phase_ll_bucket_enable_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_8, 0);
        data->bucket_enable_dbal_table = DBAL_NOF_TABLES;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_eedb_phase_to_table_set(
    int unit)
{
    int eedb_mem_type_index;
    int phase_number_index;
    dnx_data_mdb_eedb_phase_to_table_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_eedb;
    int table_index = dnx_data_mdb_eedb_table_phase_to_table;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_EEDB_MEM_TYPES;
    table->info_get.key_size[0] = MDB_NOF_EEDB_MEM_TYPES;
    table->keys[1].size = DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;
    table->info_get.key_size[1] = DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;

    
    table->values[0].default_val = "MDB_NOF_PHYSICAL_TABLES";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_eedb_phase_to_table_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_mdb_eedb_table_phase_to_table");

    
    default_data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->mdb_physical_table_id = MDB_NOF_PHYSICAL_TABLES;
    
    for (eedb_mem_type_index = 0; eedb_mem_type_index < table->keys[0].size; eedb_mem_type_index++)
    {
        for (phase_number_index = 0; phase_number_index < table->keys[1].size; phase_number_index++)
        {
            data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, eedb_mem_type_index, phase_number_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (MDB_EEDB_MEM_TYPE_LL < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_LL, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_1_LL;
    }
    if (MDB_EEDB_MEM_TYPE_LL < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_2 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_LL, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_2);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_2_LL;
    }
    if (MDB_EEDB_MEM_TYPE_LL < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_3 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_LL, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_3);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_3_LL;
    }
    if (MDB_EEDB_MEM_TYPE_LL < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_4 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_LL, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_4);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_4_LL;
    }
    if (MDB_EEDB_MEM_TYPE_LL < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_LL, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_5_LL;
    }
    if (MDB_EEDB_MEM_TYPE_LL < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_6 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_LL, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_6);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_6_LL;
    }
    if (MDB_EEDB_MEM_TYPE_LL < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_7 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_LL, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_7);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_7_LL;
    }
    if (MDB_EEDB_MEM_TYPE_LL < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_8 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_LL, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_8);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_8_LL;
    }
    if (MDB_EEDB_MEM_TYPE_DATA_BANK < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_DATA_BANK, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_1_DATA;
    }
    if (MDB_EEDB_MEM_TYPE_DATA_BANK < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_2 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_DATA_BANK, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_2);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_2_DATA;
    }
    if (MDB_EEDB_MEM_TYPE_DATA_BANK < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_3 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_DATA_BANK, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_3);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_3_DATA;
    }
    if (MDB_EEDB_MEM_TYPE_DATA_BANK < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_4 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_DATA_BANK, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_4);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_4_DATA;
    }
    if (MDB_EEDB_MEM_TYPE_DATA_BANK < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_DATA_BANK, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_5_DATA;
    }
    if (MDB_EEDB_MEM_TYPE_DATA_BANK < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_6 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_DATA_BANK, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_6);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_6_DATA;
    }
    if (MDB_EEDB_MEM_TYPE_DATA_BANK < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_7 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_DATA_BANK, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_7);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_7_DATA;
    }
    if (MDB_EEDB_MEM_TYPE_DATA_BANK < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_8 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_DATA_BANK, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_8);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_8_DATA;
    }
    if (MDB_EEDB_MEM_TYPE_DATA_CLUSTER < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_DATA_CLUSTER, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_1_2_DATA;
    }
    if (MDB_EEDB_MEM_TYPE_DATA_CLUSTER < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_2 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_DATA_CLUSTER, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_2);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_1_2_DATA;
    }
    if (MDB_EEDB_MEM_TYPE_DATA_CLUSTER < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_3 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_DATA_CLUSTER, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_3);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_3_4_DATA;
    }
    if (MDB_EEDB_MEM_TYPE_DATA_CLUSTER < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_4 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_DATA_CLUSTER, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_4);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_3_4_DATA;
    }
    if (MDB_EEDB_MEM_TYPE_DATA_CLUSTER < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_DATA_CLUSTER, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_5_6_DATA;
    }
    if (MDB_EEDB_MEM_TYPE_DATA_CLUSTER < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_6 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_DATA_CLUSTER, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_6);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_5_6_DATA;
    }
    if (MDB_EEDB_MEM_TYPE_DATA_CLUSTER < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_7 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_DATA_CLUSTER, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_7);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_7_8_DATA;
    }
    if (MDB_EEDB_MEM_TYPE_DATA_CLUSTER < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_8 < table->keys[1].size)
    {
        data = (dnx_data_mdb_eedb_phase_to_table_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_MEM_TYPE_DATA_CLUSTER, DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_8);
        data->mdb_physical_table_id = MDB_PHYSICAL_TABLE_EEDB_7_8_DATA;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_eedb_outlif_physical_phase_granularity_set(
    int unit)
{
    int outlif_physical_phase_index;
    dnx_data_mdb_eedb_outlif_physical_phase_granularity_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_eedb;
    int table_index = dnx_data_mdb_eedb_table_outlif_physical_phase_granularity;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_eedb_outlif_physical_phase_granularity_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_eedb_table_outlif_physical_phase_granularity");

    
    default_data = (dnx_data_mdb_eedb_outlif_physical_phase_granularity_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->data_granularity = 0;
    
    for (outlif_physical_phase_index = 0; outlif_physical_phase_index < table->keys[0].size; outlif_physical_phase_index++)
    {
        data = (dnx_data_mdb_eedb_outlif_physical_phase_granularity_t *) dnxc_data_mgmt_table_data_get(unit, table, outlif_physical_phase_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = "outlif_physical_phase_data_granularity";
    table->values[0].property.doc =
        "\n"
        "Assign a data granularity to each outlif physical phase.\n"
        "The suffix for this property is the physical phase, and the value is\n"
        "the data granularity.\n"
        "Valid data granularity values are 30/60/120 bits.\n"
        "Every physical phase must be configured.\n"
        "See related soc property outlif_logical_to_physical_phase_map.\n"
        "outlif_physical_phase_data_granularity_Type= 30|60|120\n"
        "Default: 60.\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_custom;
    table->values[0].property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (outlif_physical_phase_index = 0; outlif_physical_phase_index < table->keys[0].size; outlif_physical_phase_index++)
    {
        data = (dnx_data_mdb_eedb_outlif_physical_phase_granularity_t *) dnxc_data_mgmt_table_data_get(unit, table, outlif_physical_phase_index, 0);
        SHR_IF_ERR_EXIT(dnx_data_property_mdb_eedb_outlif_physical_phase_granularity_data_granularity_read(unit, outlif_physical_phase_index, &data->data_granularity));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_mdb_kaps_flex_bb_row_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int feature_index = dnx_data_mdb_kaps_flex_bb_row;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_tcam_read_valid_bits_lsbits_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int feature_index = dnx_data_mdb_kaps_tcam_read_valid_bits_lsbits;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_hitbit_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int feature_index = dnx_data_mdb_kaps_hitbit_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_mdb_kaps_nof_dbs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_nof_dbs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_nof_small_bbs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_nof_small_bbs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_nof_rows_in_rpb_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_nof_rows_in_rpb_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2048;

    
    define->data = 2048;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_nof_rows_in_small_ads_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_nof_rows_in_small_ads_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1024;

    
    define->data = 1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_nof_rows_in_small_bb_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_nof_rows_in_small_bb_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 512;

    
    define->data = 512;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_nof_rows_in_small_rpb_hitbits_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_nof_rows_in_small_rpb_hitbits_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_nof_big_bbs_blk_ids_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_nof_big_bbs_blk_ids;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 14;

    
    define->data = 14;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_nof_bytes_in_hitbit_row_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_nof_bytes_in_hitbit_row;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_key_lsn_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_key_lsn_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_key_prefix_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_key_prefix_length;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_max_prefix_in_bb_row_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_max_prefix_in_bb_row;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_key_width_in_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_key_width_in_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 160;

    
    define->data = 160;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_bb_byte_width_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_bb_byte_width;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 60;

    
    define->data = 60;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_rpb_byte_width_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_rpb_byte_width;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 21;

    
    define->data = 21;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_ads_byte_width_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_ads_byte_width;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_direct_byte_width_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_direct_byte_width;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 15;

    
    define->data = 15;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_rpb_valid_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_rpb_valid_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_dynamic_memory_access_dpc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_dynamic_memory_access_dpc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_max_fmt_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_max_fmt;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 11;

    
    define->data = 11;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_nof_app_ids_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_nof_app_ids;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_mdb.kaps.key_prefix_length_get(unit);

    
    define->data = 1 << dnx_data_mdb.kaps.key_prefix_length_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_max_nof_result_types_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_max_nof_result_types;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_rpb_field_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_rpb_field;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = RPB_RESET_Nf;

    
    define->data = RPB_RESET_Nf;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_big_kaps_revision_val_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_big_kaps_revision_val;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x00050001;

    
    define->data = 0x00050001;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_small_kaps_ad_size_e_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_small_kaps_ad_size_e;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = MDB_LPM_AD_SIZE_20;

    
    define->data = MDB_LPM_AD_SIZE_20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_mdb_kaps_big_bb_blk_id_mapping_set(
    int unit)
{
    int pair_id_index;
    int blk_id_index;
    dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int table_index = dnx_data_mdb_kaps_table_big_bb_blk_id_mapping;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_DATA_MAX_MDB_KAPS_NOF_DBS/MDB_LPM_DBS_IN_PAIR;
    table->info_get.key_size[0] = DNX_DATA_MAX_MDB_KAPS_NOF_DBS/MDB_LPM_DBS_IN_PAIR;
    table->keys[1].size = DNX_DATA_MAX_MDB_KAPS_NOF_BIG_BBS_BLK_IDS;
    table->info_get.key_size[1] = DNX_DATA_MAX_MDB_KAPS_NOF_BIG_BBS_BLK_IDS;

    
    table->values[0].default_val = "DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_kaps_big_bb_blk_id_mapping_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_mdb_kaps_table_big_bb_blk_id_mapping");

    
    default_data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->global_macro_index = DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES;
    
    for (pair_id_index = 0; pair_id_index < table->keys[0].size; pair_id_index++)
    {
        for (blk_id_index = 0; blk_id_index < table->keys[1].size; blk_id_index++)
        {
            data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, pair_id_index, blk_id_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->global_macro_index = 1;
    }
    if (0 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 1);
        data->global_macro_index = 2;
    }
    if (0 < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 2);
        data->global_macro_index = 3;
    }
    if (0 < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 3);
        data->global_macro_index = 4;
    }
    if (0 < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 4);
        data->global_macro_index = 7;
    }
    if (0 < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 5);
        data->global_macro_index = 8;
    }
    if (0 < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 6);
        data->global_macro_index = 9;
    }
    if (0 < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 7);
        data->global_macro_index = 11;
    }
    if (0 < table->keys[0].size && 8 < table->keys[1].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 8);
        data->global_macro_index = 12;
    }
    if (0 < table->keys[0].size && 9 < table->keys[1].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 9);
        data->global_macro_index = 13;
    }
    if (0 < table->keys[0].size && 10 < table->keys[1].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 10);
        data->global_macro_index = 14;
    }
    if (0 < table->keys[0].size && 11 < table->keys[1].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 11);
        data->global_macro_index = 15;
    }
    if (0 < table->keys[0].size && 12 < table->keys[1].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 12);
        data->global_macro_index = 17;
    }
    if (0 < table->keys[0].size && 13 < table->keys[1].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 13);
        data->global_macro_index = 19;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_db_info_set(
    int unit)
{
    int db_idx_index;
    dnx_data_mdb_kaps_db_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int table_index = dnx_data_mdb_kaps_table_db_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_mdb.kaps.nof_dbs_get(unit);
    table->info_get.key_size[0] = dnx_data_mdb.kaps.nof_dbs_get(unit);

    
    table->values[0].default_val = "DBAL_NOF_PHYSICAL_TABLES";
    table->values[1].default_val = "MDB_NOF_PHYSICAL_TABLES";
    table->values[2].default_val = "MDB_NOF_PHYSICAL_TABLES";
    table->values[3].default_val = "MDB_NOF_PHYSICAL_TABLES";
    table->values[4].default_val = "DEFAULT_KAPS_DB_NAME";
    table->values[5].default_val = "DBAL_NOF_DYNAMIC_AND_STATIC_TABLES";
    table->values[6].default_val = "0";
    table->values[7].default_val = "0";
    table->values[8].default_val = "0";
    table->values[9].default_val = "0";
    table->values[10].default_val = "0";
    table->values[11].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_kaps_db_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_kaps_table_db_info");

    
    default_data = (dnx_data_mdb_kaps_db_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_id = DBAL_NOF_PHYSICAL_TABLES;
    default_data->mdb_bb_id = MDB_NOF_PHYSICAL_TABLES;
    default_data->mdb_ads_id = MDB_NOF_PHYSICAL_TABLES;
    default_data->mdb_direct_id = MDB_NOF_PHYSICAL_TABLES;
    default_data->name = "DEFAULT_KAPS_DB_NAME";
    default_data->dbal_table_db_atr = DBAL_NOF_DYNAMIC_AND_STATIC_TABLES;
    default_data->nof_rows_in_rpb = 0;
    default_data->nof_rows_in_small_ads = 0;
    default_data->nof_rows_in_small_bb = 0;
    default_data->nof_rows_in_small_rpb_hitbits = 0;
    default_data->nof_connected_big_bbs = 0;
    default_data->nof_result_types = 0;
    
    for (db_idx_index = 0; db_idx_index < table->keys[0].size; db_idx_index++)
    {
        data = (dnx_data_mdb_kaps_db_info_t *) dnxc_data_mgmt_table_data_get(unit, table, db_idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_kaps_db_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_KAPS_1;
        data->mdb_bb_id = MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1;
        data->mdb_ads_id = MDB_PHYSICAL_TABLE_ADS_1;
        data->mdb_direct_id = MDB_NOF_PHYSICAL_TABLES;
        data->name = "PRIVATE";
        data->dbal_table_db_atr = DBAL_TABLE_MDB_28_KAPS_1;
        data->nof_rows_in_rpb = 2048;
        data->nof_rows_in_small_ads = 1024;
        data->nof_rows_in_small_bb = 512;
        data->nof_rows_in_small_rpb_hitbits = 64;
        data->nof_connected_big_bbs = 11;
        data->nof_result_types = 1;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_kaps_db_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->dbal_id = DBAL_PHYSICAL_TABLE_KAPS_2;
        data->mdb_bb_id = MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2;
        data->mdb_ads_id = MDB_PHYSICAL_TABLE_ADS_2;
        data->mdb_direct_id = MDB_NOF_PHYSICAL_TABLES;
        data->name = "PUBLIC";
        data->dbal_table_db_atr = DBAL_TABLE_MDB_28_KAPS_2;
        data->nof_rows_in_rpb = 2048;
        data->nof_rows_in_small_ads = 1024;
        data->nof_rows_in_small_bb = 512;
        data->nof_rows_in_small_rpb_hitbits = 64;
        data->nof_connected_big_bbs = 6;
        data->nof_result_types = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_db_supported_ad_sizes_set(
    int unit)
{
    int db_idx_index;
    int ad_size_index;
    dnx_data_mdb_kaps_db_supported_ad_sizes_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int table_index = dnx_data_mdb_kaps_table_db_supported_ad_sizes;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_mdb.kaps.nof_dbs_get(unit);
    table->info_get.key_size[0] = dnx_data_mdb.kaps.nof_dbs_get(unit);
    table->keys[1].size = MDB_NOF_LPM_AD_SIZES;
    table->info_get.key_size[1] = MDB_NOF_LPM_AD_SIZES;

    
    table->values[0].default_val = "FALSE";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_kaps_db_supported_ad_sizes_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_mdb_kaps_table_db_supported_ad_sizes");

    
    default_data = (dnx_data_mdb_kaps_db_supported_ad_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->is_supported = FALSE;
    
    for (db_idx_index = 0; db_idx_index < table->keys[0].size; db_idx_index++)
    {
        for (ad_size_index = 0; ad_size_index < table->keys[1].size; ad_size_index++)
        {
            data = (dnx_data_mdb_kaps_db_supported_ad_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, db_idx_index, ad_size_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && MDB_LPM_AD_SIZE_20 < table->keys[1].size)
    {
        data = (dnx_data_mdb_kaps_db_supported_ad_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, MDB_LPM_AD_SIZE_20);
        data->is_supported = TRUE;
    }
    if (1 < table->keys[0].size && MDB_LPM_AD_SIZE_20 < table->keys[1].size)
    {
        data = (dnx_data_mdb_kaps_db_supported_ad_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, MDB_LPM_AD_SIZE_20);
        data->is_supported = TRUE;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_db_access_info_set(
    int unit)
{
    int db_idx_index;
    dnx_data_mdb_kaps_db_access_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int table_index = dnx_data_mdb_kaps_table_db_access_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_mdb.kaps.nof_dbs_get(unit);
    table->info_get.key_size[0] = dnx_data_mdb.kaps.nof_dbs_get(unit);

    
    table->values[0].default_val = "INVALIDr";
    table->values[1].default_val = "INVALIDr";
    table->values[2].default_val = "INVALIDr";
    table->values[3].default_val = "INVALIDf";
    table->values[4].default_val = "INVALIDm";
    table->values[5].default_val = "INVALIDm";
    table->values[6].default_val = "INVALIDm";
    table->values[7].default_val = "INVALIDm";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_kaps_db_access_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_kaps_table_db_access_info");

    
    default_data = (dnx_data_mdb_kaps_db_access_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->rpb_global_cfg_reg = INVALIDr;
    default_data->tcam_scan_cfg_reg = INVALIDr;
    default_data->bb_global_cfg_reg = INVALIDr;
    default_data->bb_global_cfg_rpb_sel_field = INVALIDf;
    default_data->tcam_cmd_mem = INVALIDm;
    default_data->small_ads_mem = INVALIDm;
    default_data->small_bb_mem = INVALIDm;
    default_data->search_cmd_mem = INVALIDm;
    
    for (db_idx_index = 0; db_idx_index < table->keys[0].size; db_idx_index++)
    {
        data = (dnx_data_mdb_kaps_db_access_info_t *) dnxc_data_mgmt_table_data_get(unit, table, db_idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_kaps_db_access_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->rpb_global_cfg_reg = KAPS_RPB_GLOBAL_CONFIGr;
        data->tcam_scan_cfg_reg = KAPS_RPB_TCAM_SCAN_CFGSr;
        data->bb_global_cfg_reg = KAPS_BB_GLOBAL_CONFIGr;
        data->bb_global_cfg_rpb_sel_field = BB_RPB_SEL_Nf;
        data->tcam_cmd_mem = KAPS_RPB_TCAM_CPU_COMMANDm;
        data->small_ads_mem = KAPS_RPB_ADSm;
        data->small_bb_mem = KAPS_BUCKET_MEMORYm;
        data->search_cmd_mem = KAPS_RPB_TCAM_CPU_COMMANDm;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_kaps_db_access_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->rpb_global_cfg_reg = KAPS_RPB_GLOBAL_CONFIGr;
        data->tcam_scan_cfg_reg = KAPS_RPB_TCAM_SCAN_CFGSr;
        data->bb_global_cfg_reg = KAPS_BB_GLOBAL_CONFIGr;
        data->bb_global_cfg_rpb_sel_field = BB_RPB_SEL_Nf;
        data->tcam_cmd_mem = KAPS_RPB_TCAM_CPU_COMMANDm;
        data->small_ads_mem = KAPS_RPB_ADSm;
        data->small_bb_mem = KAPS_BUCKET_MEMORYm;
        data->search_cmd_mem = KAPS_RPB_TCAM_CPU_COMMANDm;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_kaps_lookup_result_set(
    int unit)
{
    int db_idx_index;
    int core_index;
    dnx_data_mdb_kaps_kaps_lookup_result_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int table_index = dnx_data_mdb_kaps_table_kaps_lookup_result;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;
    table->keys[1].size = dnx_data_device.general.nof_cores_get(unit);
    table->info_get.key_size[1] = dnx_data_device.general.nof_cores_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_kaps_kaps_lookup_result_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_mdb_kaps_table_kaps_lookup_result");

    
    default_data = (dnx_data_mdb_kaps_kaps_lookup_result_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->lookup_result = 0;
    
    for (db_idx_index = 0; db_idx_index < table->keys[0].size; db_idx_index++)
    {
        for (core_index = 0; core_index < table->keys[1].size; core_index++)
        {
            data = (dnx_data_mdb_kaps_kaps_lookup_result_t *) dnxc_data_mgmt_table_data_get(unit, table, db_idx_index, core_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_mdb_kaps_kaps_lookup_result_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->lookup_result = MDB_REG_2002r;
    }
    if (1 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_mdb_kaps_kaps_lookup_result_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->lookup_result = MDB_REG_2016r;
    }
    if (0 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_mdb_kaps_kaps_lookup_result_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 1);
        data->lookup_result = MDB_REG_2005r;
    }
    if (1 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_mdb_kaps_kaps_lookup_result_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 1);
        data->lookup_result = MDB_REG_2019r;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_mdb_feature_step_table_supports_mix_ar_algo_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_feature;
    int feature_index = dnx_data_mdb_feature_step_table_supports_mix_ar_algo;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_feature_eedb_bank_traffic_lock_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_feature;
    int feature_index = dnx_data_mdb_feature_eedb_bank_traffic_lock;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_feature_em_dfg_ovf_cam_disabled_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_feature;
    int feature_index = dnx_data_mdb_feature_em_dfg_ovf_cam_disabled;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_feature_em_mact_transplant_no_reply_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_feature;
    int feature_index = dnx_data_mdb_feature_em_mact_transplant_no_reply;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_feature_em_mact_use_refresh_on_insert_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_feature;
    int feature_index = dnx_data_mdb_feature_em_mact_use_refresh_on_insert;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_feature_em_mact_delete_transplant_no_reply_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_feature;
    int feature_index = dnx_data_mdb_feature_em_mact_delete_transplant_no_reply;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_feature_em_mact_insert_flush_drop_on_exceed_limit_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_feature;
    int feature_index = dnx_data_mdb_feature_em_mact_insert_flush_drop_on_exceed_limit;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_feature_em_sbus_interface_shutdown_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_feature;
    int feature_index = dnx_data_mdb_feature_em_sbus_interface_shutdown;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_feature_em_ecc_simple_command_deadlock_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_feature;
    int feature_index = dnx_data_mdb_feature_em_ecc_simple_command_deadlock;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_feature_em_compare_init_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_feature;
    int feature_index = dnx_data_mdb_feature_em_compare_init;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_feature_bubble_pulse_width_too_short_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_feature;
    int feature_index = dnx_data_mdb_feature_bubble_pulse_width_too_short;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_feature_kaps_rpb_ecc_valid_shared_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_feature;
    int feature_index = dnx_data_mdb_feature_kaps_rpb_ecc_valid_shared;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_feature_kaps_rpb_bubble_not_synced_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_feature;
    int feature_index = dnx_data_mdb_feature_kaps_rpb_bubble_not_synced;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}





shr_error_e
jr2_a0_data_mdb_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_mdb;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_mdb_submodule_global;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_mdb_global_define_utilization_api_nof_dbs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_global_utilization_api_nof_dbs_set;
    data_index = dnx_data_mdb_global_define_adapter_stub_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_global_adapter_stub_enable_set;

    
    data_index = dnx_data_mdb_global_hitbit_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_global_hitbit_support_set;
    data_index = dnx_data_mdb_global_capacity_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_global_capacity_support_set;
    data_index = dnx_data_mdb_global_init_enable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_global_init_enable_set;
    data_index = dnx_data_mdb_global_block_id_fixed_mdb_value;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_global_block_id_fixed_mdb_value_set;
    data_index = dnx_data_mdb_global_lpm_acl_enable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_global_lpm_acl_enable_set;

    
    
    submodule_index = dnx_data_mdb_submodule_dh;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_mdb_dh_define_total_nof_macroes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_total_nof_macroes_set;
    data_index = dnx_data_mdb_dh_define_total_nof_macroes_plus_em_ovf_or_eedb_bank;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_total_nof_macroes_plus_em_ovf_or_eedb_bank_set;
    data_index = dnx_data_mdb_dh_define_total_nof_macroes_plus_data_width;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_total_nof_macroes_plus_data_width_set;
    data_index = dnx_data_mdb_dh_define_max_nof_cluster_interfaces;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_max_nof_cluster_interfaces_set;
    data_index = dnx_data_mdb_dh_define_max_nof_clusters;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_max_nof_clusters_set;
    data_index = dnx_data_mdb_dh_define_max_nof_db_clusters;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_max_nof_db_clusters_set;
    data_index = dnx_data_mdb_dh_define_nof_bucket_clusters;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_nof_bucket_clusters_set;
    data_index = dnx_data_mdb_dh_define_nof_buckets_in_macro;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_nof_buckets_in_macro_set;
    data_index = dnx_data_mdb_dh_define_nof_pair_clusters;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_nof_pair_clusters_set;
    data_index = dnx_data_mdb_dh_define_nof_data_rows_per_hitbit_row;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_nof_data_rows_per_hitbit_row_set;
    data_index = dnx_data_mdb_dh_define_nof_ddha_blocks;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_nof_ddha_blocks_set;
    data_index = dnx_data_mdb_dh_define_nof_ddhb_blocks;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_nof_ddhb_blocks_set;
    data_index = dnx_data_mdb_dh_define_nof_dhc_blocks;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_nof_dhc_blocks_set;
    data_index = dnx_data_mdb_dh_define_nof_ddhc_blocks;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_nof_ddhc_blocks_set;
    data_index = dnx_data_mdb_dh_define_nof_ddhx_blocks;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_nof_ddhx_blocks_set;
    data_index = dnx_data_mdb_dh_define_cluster_row_width_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_cluster_row_width_bits_set;
    data_index = dnx_data_mdb_dh_define_cluster_row_width_uint32;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_cluster_row_width_uint32_set;
    data_index = dnx_data_mdb_dh_define_data_out_granularity;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_data_out_granularity_set;
    data_index = dnx_data_mdb_dh_define_bpu_setup_bb_connected;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_bpu_setup_bb_connected_set;
    data_index = dnx_data_mdb_dh_define_bpu_setup_size_120_240_120;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_bpu_setup_size_120_240_120_set;
    data_index = dnx_data_mdb_dh_define_bpu_setup_size_480_odd;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_bpu_setup_size_480_odd_set;
    data_index = dnx_data_mdb_dh_define_bpu_setup_size_480;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_bpu_setup_size_480_set;
    data_index = dnx_data_mdb_dh_define_bpu_setup_size_360_120;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_bpu_setup_size_360_120_set;
    data_index = dnx_data_mdb_dh_define_bpu_setup_size_120_360;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_bpu_setup_size_120_360_set;
    data_index = dnx_data_mdb_dh_define_bpu_setup_size_240_240;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_bpu_setup_size_240_240_set;
    data_index = dnx_data_mdb_dh_define_bpu_setup_size_120_120_240;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_bpu_setup_size_120_120_240_set;
    data_index = dnx_data_mdb_dh_define_bpu_setup_size_240_120_120;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_bpu_setup_size_240_120_120_set;
    data_index = dnx_data_mdb_dh_define_bpu_setup_size_120_120_120_120;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_bpu_setup_size_120_120_120_120_set;
    data_index = dnx_data_mdb_dh_define_ddha_dynamic_memory_access_dpc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_ddha_dynamic_memory_access_dpc_set;
    data_index = dnx_data_mdb_dh_define_nof_fec_macro_types;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_dh_nof_fec_macro_types_set;

    
    data_index = dnx_data_mdb_dh_macro_A_half_nof_clusters_enable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_dh_macro_A_half_nof_clusters_enable_set;
    data_index = dnx_data_mdb_dh_macro_A_75_nof_clusters_enable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_dh_macro_A_75_nof_clusters_enable_set;
    data_index = dnx_data_mdb_dh_bpu_setup_extended_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_dh_bpu_setup_extended_support_set;

    
    data_index = dnx_data_mdb_dh_table_dh_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_dh_dh_info_set;
    data_index = dnx_data_mdb_dh_table_block_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_dh_block_info_set;
    data_index = dnx_data_mdb_dh_table_logical_macro_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_dh_logical_macro_info_set;
    data_index = dnx_data_mdb_dh_table_macro_type_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_dh_macro_type_info_set;
    data_index = dnx_data_mdb_dh_table_mdb_75_macro_halved;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_dh_mdb_75_macro_halved_set;
    data_index = dnx_data_mdb_dh_table_entry_banks_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_dh_entry_banks_info_set;
    data_index = dnx_data_mdb_dh_table_mdb_1_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_dh_mdb_1_info_set;
    data_index = dnx_data_mdb_dh_table_mdb_2_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_dh_mdb_2_info_set;
    data_index = dnx_data_mdb_dh_table_mdb_3_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_dh_mdb_3_info_set;
    data_index = dnx_data_mdb_dh_table_mdb_fec_macro_types_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_dh_mdb_fec_macro_types_info_set;
    
    submodule_index = dnx_data_mdb_submodule_pdbs;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_mdb_pdbs_define_max_nof_interface_dhs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_pdbs_max_nof_interface_dhs_set;
    data_index = dnx_data_mdb_pdbs_define_mesh_mode_support;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_pdbs_mesh_mode_support_set;
    data_index = dnx_data_mdb_pdbs_define_max_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_pdbs_max_key_size_set;
    data_index = dnx_data_mdb_pdbs_define_max_payload_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_pdbs_max_payload_size_set;
    data_index = dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_1_array_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_pdbs_table_mdb_9_mdb_item_1_array_size_set;
    data_index = dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_2_array_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_pdbs_table_mdb_9_mdb_item_2_array_size_set;
    data_index = dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_3_array_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_pdbs_table_mdb_9_mdb_item_3_array_size_set;
    data_index = dnx_data_mdb_pdbs_define_table_mdb_40_mdb_item_0_array_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_pdbs_table_mdb_40_mdb_item_0_array_size_set;
    data_index = dnx_data_mdb_pdbs_define_table_mdb_dynamic_memory_access_memory_access_field_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_pdbs_table_mdb_dynamic_memory_access_memory_access_field_size_set;
    data_index = dnx_data_mdb_pdbs_define_table_mdb_mact_dynamic_memory_access_memory_access_field_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_pdbs_table_mdb_mact_dynamic_memory_access_memory_access_field_size_set;
    data_index = dnx_data_mdb_pdbs_define_table_mdb_kaps_dynamic_memory_access_sbc_memory_access_field_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_pdbs_table_mdb_kaps_dynamic_memory_access_sbc_memory_access_field_size_set;
    data_index = dnx_data_mdb_pdbs_define_table_mdb_kaps_dynamic_memory_access_dpc_memory_access_field_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_pdbs_table_mdb_kaps_dynamic_memory_access_dpc_memory_access_field_size_set;
    data_index = dnx_data_mdb_pdbs_define_table_mdb_ddha_dynamic_memory_access_sbc_memory_access_field_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_pdbs_table_mdb_ddha_dynamic_memory_access_sbc_memory_access_field_size_set;
    data_index = dnx_data_mdb_pdbs_define_table_mdb_ddha_dynamic_memory_access_dpc_memory_access_field_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_pdbs_table_mdb_ddha_dynamic_memory_access_dpc_memory_access_field_size_set;
    data_index = dnx_data_mdb_pdbs_define_table_mdb_ddhb_dynamic_memory_access_memory_access_field_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_pdbs_table_mdb_ddhb_dynamic_memory_access_memory_access_field_size_set;
    data_index = dnx_data_mdb_pdbs_define_mdb_nof_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_pdbs_mdb_nof_profiles_set;
    data_index = dnx_data_mdb_pdbs_define_adapter_mapping_crps_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_pdbs_adapter_mapping_crps_id_set;

    

    
    data_index = dnx_data_mdb_pdbs_table_mdb_profile;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_pdbs_mdb_profile_set;
    data_index = dnx_data_mdb_pdbs_table_mdb_profiles_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_pdbs_mdb_profiles_info_set;
    data_index = dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_set;
    data_index = dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_set;
    data_index = dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_pdbs_dbal_vmv_str_mapping_set;
    data_index = dnx_data_mdb_pdbs_table_mdb_pdb_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_pdbs_mdb_pdb_info_set;
    data_index = dnx_data_mdb_pdbs_table_dbal_pdb_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_pdbs_dbal_pdb_info_set;
    data_index = dnx_data_mdb_pdbs_table_mdb_adapter_mapping;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_pdbs_mdb_adapter_mapping_set;
    data_index = dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_set;
    
    submodule_index = dnx_data_mdb_submodule_em;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_mdb_em_define_age_row_size_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_age_row_size_bits_set;
    data_index = dnx_data_mdb_em_define_age_ovfcam_row_size_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_age_ovfcam_row_size_bits_set;
    data_index = dnx_data_mdb_em_define_ovf_cam_max_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_ovf_cam_max_size_set;
    data_index = dnx_data_mdb_em_define_age_support_per_entry_size_ratio;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_age_support_per_entry_size_ratio_set;
    data_index = dnx_data_mdb_em_define_flush_tcam_rule_counter_support;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_flush_tcam_rule_counter_support_set;
    data_index = dnx_data_mdb_em_define_nof_aging_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_nof_aging_profiles_set;
    data_index = dnx_data_mdb_em_define_aging_profiles_size_in_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_aging_profiles_size_in_bits_set;
    data_index = dnx_data_mdb_em_define_max_tid_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_max_tid_size_set;
    data_index = dnx_data_mdb_em_define_max_nof_tids;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_max_nof_tids_set;
    data_index = dnx_data_mdb_em_define_flush_support_tids;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_flush_support_tids_set;
    data_index = dnx_data_mdb_em_define_flush_max_supported_key;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_flush_max_supported_key_set;
    data_index = dnx_data_mdb_em_define_flush_max_supported_key_plus_payload;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_flush_max_supported_key_plus_payload_set;
    data_index = dnx_data_mdb_em_define_dh_120_entry_encoding_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_dh_120_entry_encoding_nof_bits_set;
    data_index = dnx_data_mdb_em_define_dh_240_entry_encoding_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_dh_240_entry_encoding_nof_bits_set;
    data_index = dnx_data_mdb_em_define_format_granularity;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_format_granularity_set;
    data_index = dnx_data_mdb_em_define_max_nof_vmv_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_max_nof_vmv_size_set;
    data_index = dnx_data_mdb_em_define_vmv_nof_values;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_vmv_nof_values_set;
    data_index = dnx_data_mdb_em_define_nof_vmv_size_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_nof_vmv_size_nof_bits_set;
    data_index = dnx_data_mdb_em_define_esem_nof_vmv_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_esem_nof_vmv_size_set;
    data_index = dnx_data_mdb_em_define_glem_nof_vmv_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_glem_nof_vmv_size_set;
    data_index = dnx_data_mdb_em_define_mact_nof_vmv_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_mact_nof_vmv_size_set;
    data_index = dnx_data_mdb_em_define_mact_max_payload_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_mact_max_payload_size_set;
    data_index = dnx_data_mdb_em_define_shift_vmv_max_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_shift_vmv_max_size_set;
    data_index = dnx_data_mdb_em_define_shift_vmv_max_regs_per_table;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_shift_vmv_max_regs_per_table_set;
    data_index = dnx_data_mdb_em_define_min_nof_app_id_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_min_nof_app_id_bits_set;
    data_index = dnx_data_mdb_em_define_max_nof_spn_sizes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_max_nof_spn_sizes_set;
    data_index = dnx_data_mdb_em_define_nof_lfsr_sizes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_nof_lfsr_sizes_set;
    data_index = dnx_data_mdb_em_define_nof_formats;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_nof_formats_set;
    data_index = dnx_data_mdb_em_define_defragmentation_priority_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_defragmentation_priority_supported_set;
    data_index = dnx_data_mdb_em_define_disable_cuckoo_loop_detection_support;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_disable_cuckoo_loop_detection_support_set;
    data_index = dnx_data_mdb_em_define_disable_cuckoo_hit_bit_sync;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_disable_cuckoo_hit_bit_sync_set;
    data_index = dnx_data_mdb_em_define_age_profile_per_ratio_support;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_age_profile_per_ratio_support_set;
    data_index = dnx_data_mdb_em_define_flex_mag_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_flex_mag_supported_set;
    data_index = dnx_data_mdb_em_define_flex_fully_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_flex_fully_supported_set;
    data_index = dnx_data_mdb_em_define_step_table_max_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_step_table_max_size_set;
    data_index = dnx_data_mdb_em_define_nof_encoding_values;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_nof_encoding_values_set;
    data_index = dnx_data_mdb_em_define_nof_encoding_types;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_nof_encoding_types_set;
    data_index = dnx_data_mdb_em_define_extra_vmv_shift_registers;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_extra_vmv_shift_registers_set;
    data_index = dnx_data_mdb_em_define_scan_bank_participate_in_cuckoo_support;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_scan_bank_participate_in_cuckoo_support_set;
    data_index = dnx_data_mdb_em_define_scan_bank_participate_in_cuckoo_partial_support;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_scan_bank_participate_in_cuckoo_partial_support_set;
    data_index = dnx_data_mdb_em_define_sbus_error_code_support;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_sbus_error_code_support_set;
    data_index = dnx_data_mdb_em_define_nof_emp_tables;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_nof_emp_tables_set;
    data_index = dnx_data_mdb_em_define_defrag_interrupt_support;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_defrag_interrupt_support_set;
    data_index = dnx_data_mdb_em_define_glem_bypass_option_support;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_glem_bypass_option_support_set;
    data_index = dnx_data_mdb_em_define_flush_max_supported_payload;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_flush_max_supported_payload_set;
    data_index = dnx_data_mdb_em_define_flush_payload_max_bus;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_flush_payload_max_bus_set;
    data_index = dnx_data_mdb_em_define_my_mac_ippa_em_dbal_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_my_mac_ippa_em_dbal_id_set;
    data_index = dnx_data_mdb_em_define_my_mac_ippf_em_dbal_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_my_mac_ippf_em_dbal_id_set;
    data_index = dnx_data_mdb_em_define_mact_dbal_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_mact_dbal_id_set;

    
    data_index = dnx_data_mdb_em_entry_counter_decrease_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_em_entry_counter_decrease_support_set;
    data_index = dnx_data_mdb_em_entry_type_parser;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_em_entry_type_parser_set;
    data_index = dnx_data_mdb_em_step_table_any_cmds_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_em_step_table_any_cmds_support_set;
    data_index = dnx_data_mdb_em_multi_interface_blocks;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_em_multi_interface_blocks_set;
    data_index = dnx_data_mdb_em_is_isem_dpc_in_vtt5;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_em_is_isem_dpc_in_vtt5_set;

    
    data_index = dnx_data_mdb_em_table_em_utilization_api_db_translation;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_em_utilization_api_db_translation_set;
    data_index = dnx_data_mdb_em_table_spn;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_spn_set;
    data_index = dnx_data_mdb_em_table_lfsr;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_lfsr_set;
    data_index = dnx_data_mdb_em_table_emp_index_table_mapping;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_emp_index_table_mapping_set;
    data_index = dnx_data_mdb_em_table_em_aging_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_em_aging_info_set;
    data_index = dnx_data_mdb_em_table_em_aging_cfg;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_em_aging_cfg_set;
    data_index = dnx_data_mdb_em_table_em_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_em_info_set;
    data_index = dnx_data_mdb_em_table_em_interrupt;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_em_interrupt_set;
    data_index = dnx_data_mdb_em_table_step_table_pdb_max_depth;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_step_table_pdb_max_depth_set;
    data_index = dnx_data_mdb_em_table_step_table_max_depth_possible;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_step_table_max_depth_possible_set;
    data_index = dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_set;
    data_index = dnx_data_mdb_em_table_mdb_15_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_mdb_15_info_set;
    data_index = dnx_data_mdb_em_table_mdb_16_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_mdb_16_info_set;
    data_index = dnx_data_mdb_em_table_mdb_21_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_mdb_21_info_set;
    data_index = dnx_data_mdb_em_table_mdb_23_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_mdb_23_info_set;
    data_index = dnx_data_mdb_em_table_mdb_24_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_mdb_24_info_set;
    data_index = dnx_data_mdb_em_table_mdb_29_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_mdb_29_info_set;
    data_index = dnx_data_mdb_em_table_mdb_31_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_mdb_31_info_set;
    data_index = dnx_data_mdb_em_table_mdb_32_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_mdb_32_info_set;
    data_index = dnx_data_mdb_em_table_mdb_41_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_mdb_41_info_set;
    data_index = dnx_data_mdb_em_table_mdb_45_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_mdb_45_info_set;
    data_index = dnx_data_mdb_em_table_mdb_emp_tables_mapping;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_mdb_emp_tables_mapping_set;
    data_index = dnx_data_mdb_em_table_mdb_em_shift_vmv_regs;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_mdb_em_shift_vmv_regs_set;
    data_index = dnx_data_mdb_em_table_em_emp;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_em_emp_set;
    data_index = dnx_data_mdb_em_table_mdb_step_table_map;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_mdb_step_table_map_set;
    data_index = dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_set;
    data_index = dnx_data_mdb_em_table_filter_rules;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_filter_rules_set;
    data_index = dnx_data_mdb_em_table_data_rules;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_data_rules_set;
    
    submodule_index = dnx_data_mdb_submodule_direct;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_mdb_direct_define_physical_address_max_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_direct_physical_address_max_bits_set;
    data_index = dnx_data_mdb_direct_define_physical_address_max_bits_support_single_macro_b_granularity_fec_alloc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_direct_physical_address_max_bits_support_single_macro_b_granularity_fec_alloc_set;
    data_index = dnx_data_mdb_direct_define_vsi_physical_address_max_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_direct_vsi_physical_address_max_bits_set;
    data_index = dnx_data_mdb_direct_define_fec_row_width;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_direct_fec_row_width_set;
    data_index = dnx_data_mdb_direct_define_fec_address_mapping_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_direct_fec_address_mapping_bits_set;
    data_index = dnx_data_mdb_direct_define_fec_address_mapping_bits_relevant;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_direct_fec_address_mapping_bits_relevant_set;
    data_index = dnx_data_mdb_direct_define_fec_max_cluster_pairs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_direct_fec_max_cluster_pairs_set;
    data_index = dnx_data_mdb_direct_define_max_payload_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_direct_max_payload_size_set;
    data_index = dnx_data_mdb_direct_define_nof_fecs_in_super_fec;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_direct_nof_fecs_in_super_fec_set;
    data_index = dnx_data_mdb_direct_define_nof_fec_ecmp_hierarchies;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_direct_nof_fec_ecmp_hierarchies_set;
    data_index = dnx_data_mdb_direct_define_physical_address_ovf_enable_val;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_direct_physical_address_ovf_enable_val_set;
    data_index = dnx_data_mdb_direct_define_nof_fec_dbs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_direct_nof_fec_dbs_set;
    data_index = dnx_data_mdb_direct_define_map_payload_size_enum;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_direct_map_payload_size_enum_set;
    data_index = dnx_data_mdb_direct_define_map_payload_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_direct_map_payload_size_set;
    data_index = dnx_data_mdb_direct_define_fec_allocation_base_macro_type;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_direct_fec_allocation_base_macro_type_set;

    
    data_index = dnx_data_mdb_direct_fec_abk_mapping;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_direct_fec_abk_mapping_set;
    data_index = dnx_data_mdb_direct_fec_abk_macro_mapping;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_direct_fec_abk_macro_mapping_set;
    data_index = dnx_data_mdb_direct_vsi_config_size;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_direct_vsi_config_size_set;

    
    data_index = dnx_data_mdb_direct_table_direct_utilization_api_db_translation;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_direct_direct_utilization_api_db_translation_set;
    data_index = dnx_data_mdb_direct_table_direct_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_direct_direct_info_set;
    data_index = dnx_data_mdb_direct_table_hit_bit_pos_in_abk;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_direct_hit_bit_pos_in_abk_set;
    data_index = dnx_data_mdb_direct_table_mdb_11_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_direct_mdb_11_info_set;
    data_index = dnx_data_mdb_direct_table_mdb_26_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_direct_mdb_26_info_set;
    data_index = dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_set;
    
    submodule_index = dnx_data_mdb_submodule_eedb;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_mdb_eedb_define_phase_map_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_eedb_phase_map_bits_set;
    data_index = dnx_data_mdb_eedb_define_phase_map_array_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_eedb_phase_map_array_size_set;
    data_index = dnx_data_mdb_eedb_define_nof_phase_per_mag;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_eedb_nof_phase_per_mag_set;
    data_index = dnx_data_mdb_eedb_define_nof_phases;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_eedb_nof_phases_set;
    data_index = dnx_data_mdb_eedb_define_nof_eedb_mags;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_eedb_nof_eedb_mags_set;
    data_index = dnx_data_mdb_eedb_define_nof_eedb_banks;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_eedb_nof_eedb_banks_set;
    data_index = dnx_data_mdb_eedb_define_nof_phases_per_eedb_bank;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_eedb_nof_phases_per_eedb_bank_set;
    data_index = dnx_data_mdb_eedb_define_nof_phases_per_eedb_bank_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_eedb_nof_phases_per_eedb_bank_size_set;
    data_index = dnx_data_mdb_eedb_define_phase_bank_select_default;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_eedb_phase_bank_select_default_set;
    data_index = dnx_data_mdb_eedb_define_entry_format_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_eedb_entry_format_bits_set;
    data_index = dnx_data_mdb_eedb_define_entry_format_encoding_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_eedb_entry_format_encoding_bits_set;
    data_index = dnx_data_mdb_eedb_define_bank_id_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_eedb_bank_id_bits_set;
    data_index = dnx_data_mdb_eedb_define_entry_bank;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_eedb_entry_bank_set;
    data_index = dnx_data_mdb_eedb_define_abk_bank;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_eedb_abk_bank_set;

    
    data_index = dnx_data_mdb_eedb_eedb_bank_hitbit_masked_clear;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_eedb_eedb_bank_hitbit_masked_clear_set;

    
    data_index = dnx_data_mdb_eedb_table_phase_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_eedb_phase_info_set;
    data_index = dnx_data_mdb_eedb_table_phase_ll_bucket_enable;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_eedb_phase_ll_bucket_enable_set;
    data_index = dnx_data_mdb_eedb_table_phase_to_table;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_eedb_phase_to_table_set;
    data_index = dnx_data_mdb_eedb_table_outlif_physical_phase_granularity;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_eedb_outlif_physical_phase_granularity_set;
    
    submodule_index = dnx_data_mdb_submodule_kaps;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_mdb_kaps_define_nof_dbs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_nof_dbs_set;
    data_index = dnx_data_mdb_kaps_define_nof_small_bbs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_nof_small_bbs_set;
    data_index = dnx_data_mdb_kaps_define_nof_rows_in_rpb_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_nof_rows_in_rpb_max_set;
    data_index = dnx_data_mdb_kaps_define_nof_rows_in_small_ads_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_nof_rows_in_small_ads_max_set;
    data_index = dnx_data_mdb_kaps_define_nof_rows_in_small_bb_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_nof_rows_in_small_bb_max_set;
    data_index = dnx_data_mdb_kaps_define_nof_rows_in_small_rpb_hitbits_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_nof_rows_in_small_rpb_hitbits_max_set;
    data_index = dnx_data_mdb_kaps_define_nof_big_bbs_blk_ids;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_nof_big_bbs_blk_ids_set;
    data_index = dnx_data_mdb_kaps_define_nof_bytes_in_hitbit_row;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_nof_bytes_in_hitbit_row_set;
    data_index = dnx_data_mdb_kaps_define_key_lsn_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_key_lsn_bits_set;
    data_index = dnx_data_mdb_kaps_define_key_prefix_length;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_key_prefix_length_set;
    data_index = dnx_data_mdb_kaps_define_max_prefix_in_bb_row;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_max_prefix_in_bb_row_set;
    data_index = dnx_data_mdb_kaps_define_key_width_in_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_key_width_in_bits_set;
    data_index = dnx_data_mdb_kaps_define_bb_byte_width;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_bb_byte_width_set;
    data_index = dnx_data_mdb_kaps_define_rpb_byte_width;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_rpb_byte_width_set;
    data_index = dnx_data_mdb_kaps_define_ads_byte_width;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_ads_byte_width_set;
    data_index = dnx_data_mdb_kaps_define_direct_byte_width;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_direct_byte_width_set;
    data_index = dnx_data_mdb_kaps_define_rpb_valid_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_rpb_valid_bits_set;
    data_index = dnx_data_mdb_kaps_define_dynamic_memory_access_dpc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_dynamic_memory_access_dpc_set;
    data_index = dnx_data_mdb_kaps_define_max_fmt;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_max_fmt_set;
    data_index = dnx_data_mdb_kaps_define_nof_app_ids;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_nof_app_ids_set;
    data_index = dnx_data_mdb_kaps_define_max_nof_result_types;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_max_nof_result_types_set;
    data_index = dnx_data_mdb_kaps_define_rpb_field;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_rpb_field_set;
    data_index = dnx_data_mdb_kaps_define_big_kaps_revision_val;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_big_kaps_revision_val_set;
    data_index = dnx_data_mdb_kaps_define_small_kaps_ad_size_e;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_small_kaps_ad_size_e_set;

    
    data_index = dnx_data_mdb_kaps_flex_bb_row;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_kaps_flex_bb_row_set;
    data_index = dnx_data_mdb_kaps_tcam_read_valid_bits_lsbits;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_kaps_tcam_read_valid_bits_lsbits_set;
    data_index = dnx_data_mdb_kaps_hitbit_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_kaps_hitbit_support_set;

    
    data_index = dnx_data_mdb_kaps_table_big_bb_blk_id_mapping;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_kaps_big_bb_blk_id_mapping_set;
    data_index = dnx_data_mdb_kaps_table_db_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_kaps_db_info_set;
    data_index = dnx_data_mdb_kaps_table_db_supported_ad_sizes;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_kaps_db_supported_ad_sizes_set;
    data_index = dnx_data_mdb_kaps_table_db_access_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_kaps_db_access_info_set;
    data_index = dnx_data_mdb_kaps_table_kaps_lookup_result;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_kaps_kaps_lookup_result_set;
    
    submodule_index = dnx_data_mdb_submodule_feature;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_mdb_feature_step_table_supports_mix_ar_algo;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_feature_step_table_supports_mix_ar_algo_set;
    data_index = dnx_data_mdb_feature_eedb_bank_traffic_lock;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_feature_eedb_bank_traffic_lock_set;
    data_index = dnx_data_mdb_feature_em_dfg_ovf_cam_disabled;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_feature_em_dfg_ovf_cam_disabled_set;
    data_index = dnx_data_mdb_feature_em_mact_transplant_no_reply;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_feature_em_mact_transplant_no_reply_set;
    data_index = dnx_data_mdb_feature_em_mact_use_refresh_on_insert;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_feature_em_mact_use_refresh_on_insert_set;
    data_index = dnx_data_mdb_feature_em_mact_delete_transplant_no_reply;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_feature_em_mact_delete_transplant_no_reply_set;
    data_index = dnx_data_mdb_feature_em_mact_insert_flush_drop_on_exceed_limit;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_feature_em_mact_insert_flush_drop_on_exceed_limit_set;
    data_index = dnx_data_mdb_feature_em_sbus_interface_shutdown;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_feature_em_sbus_interface_shutdown_set;
    data_index = dnx_data_mdb_feature_em_ecc_simple_command_deadlock;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_feature_em_ecc_simple_command_deadlock_set;
    data_index = dnx_data_mdb_feature_em_compare_init;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_feature_em_compare_init_set;
    data_index = dnx_data_mdb_feature_bubble_pulse_width_too_short;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_feature_bubble_pulse_width_too_short_set;
    data_index = dnx_data_mdb_feature_kaps_rpb_ecc_valid_shared;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_feature_kaps_rpb_ecc_valid_shared_set;
    data_index = dnx_data_mdb_feature_kaps_rpb_bubble_not_synced;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_feature_kaps_rpb_bubble_not_synced_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

