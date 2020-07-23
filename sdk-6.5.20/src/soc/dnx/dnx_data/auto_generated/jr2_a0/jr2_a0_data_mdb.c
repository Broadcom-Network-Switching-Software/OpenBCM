

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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
jr2_a0_dnx_data_mdb_global_adapter_use_stub_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_global;
    int feature_index = dnx_data_mdb_global_adapter_use_stub;
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

    
    table->keys[0].size = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;

    
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
    default_data->row_width = 0;
    default_data->dh_in_width = 0;
    default_data->dh_out_width = 0;
    default_data->two_ways_connectivity_bm = 0;
    
    for (mdb_table_id_index = 0; mdb_table_id_index < table->keys[0].size; mdb_table_id_index++)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 35;
        data->dh_out_width = 24;
        data->two_ways_connectivity_bm = 0x10000;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_1, 0);
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
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 180;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IVSI, 0);
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
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 90;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 75;
        data->dh_out_width = 24;
        data->two_ways_connectivity_bm = 0x40;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_2, 0);
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
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 180;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 75;
        data->dh_out_width = 24;
        data->two_ways_connectivity_bm = 0x40400;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_3, 0);
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
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 180;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM, 0);
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
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 147;
        data->dh_out_width = 120;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 120;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_2, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 120;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1, 0);
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
        data->row_width = 4*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 160;
        data->dh_out_width = 30;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2, 0);
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
        data->row_width = 4*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 160;
        data->dh_out_width = 30;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_0, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 19;
        data->dh_out_width = 48;
        data->two_ways_connectivity_bm = 0x10000;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_1, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 19;
        data->dh_out_width = 48;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MAP < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MAP, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 120;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1, 0);
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
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 150;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2, 0);
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
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 150;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3, 0);
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
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 150;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MC_ID < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MC_ID, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 11;
        data->dh_out_width = 110;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_0, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 11;
        data->dh_out_width = 24;
        data->two_ways_connectivity_bm = 0x8080;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_1, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 11;
        data->dh_out_width = 24;
        data->two_ways_connectivity_bm = 0x100;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_LL, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_DATA, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_LL, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_DATA, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 120;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_LL, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_DATA, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_LL, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_DATA, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 120;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_LL, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_DATA, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_LL, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_DATA, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 120;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_LL, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_DATA, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_LL, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_DATA, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 0;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 120;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_0, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 19;
        data->dh_out_width = 48;
        data->two_ways_connectivity_bm = 0x40000;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_1, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 19;
        data->dh_out_width = 48;
        data->two_ways_connectivity_bm = 0x40000;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ESEM, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 35;
        data->dh_out_width = 90;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EVSI, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 0;
        data->dh_out_width = 30;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_1, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 67;
        data->dh_out_width = 60;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_2, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 67;
        data->dh_out_width = 60;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 67;
        data->dh_out_width = 60;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4, 0);
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
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 147;
        data->dh_out_width = 60;
        data->two_ways_connectivity_bm = 0x0;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_RMEP < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_dh_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_RMEP, 0);
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
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->dh_in_width = 43;
        data->dh_out_width = 110;
        data->two_ways_connectivity_bm = 0x0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_macro_interface_mapping_set(
    int unit)
{
    int global_macro_index_index;
    dnx_data_mdb_dh_macro_interface_mapping_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int table_index = dnx_data_mdb_dh_table_macro_interface_mapping;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_mdb.dh.total_nof_macroes_get(unit);
    table->info_get.key_size[0] = dnx_data_mdb.dh.total_nof_macroes_get(unit);

    
    table->values[0].default_val = "DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_dh_macro_interface_mapping_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_dh_table_macro_interface_mapping");

    
    default_data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->interfaces[0] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    
    for (global_macro_index_index = 0; global_macro_index_index < table->keys[0].size; global_macro_index_index++)
    {
        data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, global_macro_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES, _SHR_E_INTERNAL, "out of bound access to array")
        data->interfaces[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM;
        data->interfaces[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1;
        data->interfaces[2] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_1;
        data->interfaces[3] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA;
        data->interfaces[4] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ESEM;
        data->interfaces[5] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_RMEP;
        data->interfaces[6] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[7] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[8] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES, _SHR_E_INTERNAL, "out of bound access to array")
        data->interfaces[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_1;
        data->interfaces[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IVSI;
        data->interfaces[2] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1;
        data->interfaces[3] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_1;
        data->interfaces[4] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA;
        data->interfaces[5] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA;
        data->interfaces[6] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4;
        data->interfaces[7] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[8] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES, _SHR_E_INTERNAL, "out of bound access to array")
        data->interfaces[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_2;
        data->interfaces[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM;
        data->interfaces[2] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1;
        data->interfaces[3] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_1;
        data->interfaces[4] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3;
        data->interfaces[5] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[6] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[7] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[8] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES, _SHR_E_INTERNAL, "out of bound access to array")
        data->interfaces[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM;
        data->interfaces[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1;
        data->interfaces[2] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_1;
        data->interfaces[3] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3;
        data->interfaces[4] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_1;
        data->interfaces[5] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3;
        data->interfaces[6] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[7] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[8] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES, _SHR_E_INTERNAL, "out of bound access to array")
        data->interfaces[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_1;
        data->interfaces[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM;
        data->interfaces[2] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1;
        data->interfaces[3] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2;
        data->interfaces[4] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MC_ID;
        data->interfaces[5] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_1;
        data->interfaces[6] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[7] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[8] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES, _SHR_E_INTERNAL, "out of bound access to array")
        data->interfaces[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1;
        data->interfaces[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2;
        data->interfaces[2] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM;
        data->interfaces[3] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2;
        data->interfaces[4] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3;
        data->interfaces[5] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_0;
        data->interfaces[6] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA;
        data->interfaces[7] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[8] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES, _SHR_E_INTERNAL, "out of bound access to array")
        data->interfaces[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1;
        data->interfaces[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2;
        data->interfaces[2] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2;
        data->interfaces[3] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3;
        data->interfaces[4] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM;
        data->interfaces[5] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1;
        data->interfaces[6] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA;
        data->interfaces[7] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ESEM;
        data->interfaces[8] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES, _SHR_E_INTERNAL, "out of bound access to array")
        data->interfaces[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3;
        data->interfaces[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM;
        data->interfaces[2] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1;
        data->interfaces[3] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MAP;
        data->interfaces[4] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MC_ID;
        data->interfaces[5] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_0;
        data->interfaces[6] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_0;
        data->interfaces[7] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_0;
        data->interfaces[8] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EVSI;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES, _SHR_E_INTERNAL, "out of bound access to array")
        data->interfaces[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1;
        data->interfaces[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_1;
        data->interfaces[2] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM;
        data->interfaces[3] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1;
        data->interfaces[4] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_1;
        data->interfaces[5] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_1;
        data->interfaces[6] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_2;
        data->interfaces[7] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[8] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES, _SHR_E_INTERNAL, "out of bound access to array")
        data->interfaces[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_3;
        data->interfaces[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM;
        data->interfaces[2] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1;
        data->interfaces[3] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA;
        data->interfaces[4] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_2;
        data->interfaces[5] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[6] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[7] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[8] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES, _SHR_E_INTERNAL, "out of bound access to array")
        data->interfaces[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2;
        data->interfaces[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3;
        data->interfaces[2] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3;
        data->interfaces[3] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM;
        data->interfaces[4] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_0;
        data->interfaces[5] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MAP;
        data->interfaces[6] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1;
        data->interfaces[7] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_0;
        data->interfaces[8] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES, _SHR_E_INTERNAL, "out of bound access to array")
        data->interfaces[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM;
        data->interfaces[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_2;
        data->interfaces[2] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1;
        data->interfaces[3] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3;
        data->interfaces[4] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_1;
        data->interfaces[5] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_1;
        data->interfaces[6] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ESEM;
        data->interfaces[7] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EVSI;
        data->interfaces[8] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES, _SHR_E_INTERNAL, "out of bound access to array")
        data->interfaces[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1;
        data->interfaces[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_1;
        data->interfaces[2] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1;
        data->interfaces[3] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2;
        data->interfaces[4] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA;
        data->interfaces[5] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[6] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[7] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[8] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES, _SHR_E_INTERNAL, "out of bound access to array")
        data->interfaces[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1;
        data->interfaces[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_1;
        data->interfaces[2] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2;
        data->interfaces[3] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2;
        data->interfaces[4] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_0;
        data->interfaces[5] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3;
        data->interfaces[6] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_1;
        data->interfaces[7] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EVSI;
        data->interfaces[8] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES, _SHR_E_INTERNAL, "out of bound access to array")
        data->interfaces[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2;
        data->interfaces[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1;
        data->interfaces[2] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2;
        data->interfaces[3] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MC_ID;
        data->interfaces[4] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA;
        data->interfaces[5] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ESEM;
        data->interfaces[6] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[7] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[8] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES, _SHR_E_INTERNAL, "out of bound access to array")
        data->interfaces[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_1;
        data->interfaces[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IVSI;
        data->interfaces[2] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3;
        data->interfaces[3] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2;
        data->interfaces[4] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_1;
        data->interfaces[5] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_0;
        data->interfaces[6] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_0;
        data->interfaces[7] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ESEM;
        data->interfaces[8] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES, _SHR_E_INTERNAL, "out of bound access to array")
        data->interfaces[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1;
        data->interfaces[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1;
        data->interfaces[2] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_0;
        data->interfaces[3] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_0;
        data->interfaces[4] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_1;
        data->interfaces[5] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MC_ID;
        data->interfaces[6] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA;
        data->interfaces[7] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_RMEP;
        data->interfaces[8] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES, _SHR_E_INTERNAL, "out of bound access to array")
        data->interfaces[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_3;
        data->interfaces[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1;
        data->interfaces[2] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2;
        data->interfaces[3] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MC_ID;
        data->interfaces[4] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_0;
        data->interfaces[5] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[6] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[7] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[8] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES, _SHR_E_INTERNAL, "out of bound access to array")
        data->interfaces[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1;
        data->interfaces[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3;
        data->interfaces[2] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3;
        data->interfaces[3] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_1;
        data->interfaces[4] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_0;
        data->interfaces[5] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_0;
        data->interfaces[6] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_1;
        data->interfaces[7] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_1;
        data->interfaces[8] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_interface_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        SHR_RANGE_VERIFY(9, 0, DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTER_INTERFACES, _SHR_E_INTERNAL, "out of bound access to array")
        data->interfaces[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2;
        data->interfaces[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1;
        data->interfaces[2] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2;
        data->interfaces[3] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1;
        data->interfaces[4] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2;
        data->interfaces[5] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[6] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[7] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->interfaces[8] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_dh_table_way_to_macro_mapping_set(
    int unit)
{
    int mdb_table_id_index;
    dnx_data_mdb_dh_table_way_to_macro_mapping_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_dh;
    int table_index = dnx_data_mdb_dh_table_table_way_to_macro_mapping;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;

    
    table->values[0].default_val = "DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_dh_table_way_to_macro_mapping_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_dh_table_table_way_to_macro_mapping");

    
    default_data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->table_way_to_macro_mapping[0] = DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES;
    
    for (mdb_table_id_index = 0; mdb_table_id_index < table->keys[0].size; mdb_table_id_index++)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 5;
        data->table_way_to_macro_mapping[1] = 6;
        data->table_way_to_macro_mapping[2] = 8;
        data->table_way_to_macro_mapping[3] = 12;
        data->table_way_to_macro_mapping[4] = 13;
        data->table_way_to_macro_mapping[5] = 16;
        data->table_way_to_macro_mapping[6] = 16;
        data->table_way_to_macro_mapping[7] = 18;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_1, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 1;
        data->table_way_to_macro_mapping[1] = 4;
        data->table_way_to_macro_mapping[2] = 8;
        data->table_way_to_macro_mapping[3] = 12;
        data->table_way_to_macro_mapping[4] = 13;
        data->table_way_to_macro_mapping[5] = 15;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IVSI, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 1;
        data->table_way_to_macro_mapping[1] = 15;
        data->table_way_to_macro_mapping[2] = 20;
        data->table_way_to_macro_mapping[3] = 20;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 5;
        data->table_way_to_macro_mapping[1] = 6;
        data->table_way_to_macro_mapping[2] = 6;
        data->table_way_to_macro_mapping[3] = 10;
        data->table_way_to_macro_mapping[4] = 13;
        data->table_way_to_macro_mapping[5] = 14;
        data->table_way_to_macro_mapping[6] = 19;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_2, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 2;
        data->table_way_to_macro_mapping[1] = 20;
        data->table_way_to_macro_mapping[2] = 20;
        data->table_way_to_macro_mapping[3] = 20;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 6;
        data->table_way_to_macro_mapping[1] = 7;
        data->table_way_to_macro_mapping[2] = 10;
        data->table_way_to_macro_mapping[3] = 10;
        data->table_way_to_macro_mapping[4] = 15;
        data->table_way_to_macro_mapping[5] = 18;
        data->table_way_to_macro_mapping[6] = 18;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_3, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 9;
        data->table_way_to_macro_mapping[1] = 17;
        data->table_way_to_macro_mapping[2] = 20;
        data->table_way_to_macro_mapping[3] = 20;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 0;
        data->table_way_to_macro_mapping[1] = 2;
        data->table_way_to_macro_mapping[2] = 3;
        data->table_way_to_macro_mapping[3] = 4;
        data->table_way_to_macro_mapping[4] = 5;
        data->table_way_to_macro_mapping[5] = 6;
        data->table_way_to_macro_mapping[6] = 7;
        data->table_way_to_macro_mapping[7] = 8;
        data->table_way_to_macro_mapping[8] = 9;
        data->table_way_to_macro_mapping[9] = 10;
        data->table_way_to_macro_mapping[10] = 11;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 6;
        data->table_way_to_macro_mapping[1] = 14;
        data->table_way_to_macro_mapping[2] = 20;
        data->table_way_to_macro_mapping[3] = 20;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_2, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 11;
        data->table_way_to_macro_mapping[1] = 20;
        data->table_way_to_macro_mapping[2] = 20;
        data->table_way_to_macro_mapping[3] = 20;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 1;
        data->table_way_to_macro_mapping[1] = 2;
        data->table_way_to_macro_mapping[2] = 3;
        data->table_way_to_macro_mapping[3] = 4;
        data->table_way_to_macro_mapping[4] = 7;
        data->table_way_to_macro_mapping[5] = 8;
        data->table_way_to_macro_mapping[6] = 9;
        data->table_way_to_macro_mapping[7] = 11;
        data->table_way_to_macro_mapping[8] = 12;
        data->table_way_to_macro_mapping[9] = 17;
        data->table_way_to_macro_mapping[10] = 19;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 12;
        data->table_way_to_macro_mapping[1] = 13;
        data->table_way_to_macro_mapping[2] = 14;
        data->table_way_to_macro_mapping[3] = 15;
        data->table_way_to_macro_mapping[4] = 17;
        data->table_way_to_macro_mapping[5] = 19;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_0, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 10;
        data->table_way_to_macro_mapping[1] = 13;
        data->table_way_to_macro_mapping[2] = 16;
        data->table_way_to_macro_mapping[3] = 16;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_1, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 3;
        data->table_way_to_macro_mapping[1] = 15;
        data->table_way_to_macro_mapping[2] = 16;
        data->table_way_to_macro_mapping[3] = 18;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MAP < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MAP, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 7;
        data->table_way_to_macro_mapping[1] = 10;
        data->table_way_to_macro_mapping[2] = 20;
        data->table_way_to_macro_mapping[3] = 20;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 0;
        data->table_way_to_macro_mapping[1] = 10;
        data->table_way_to_macro_mapping[2] = 19;
        data->table_way_to_macro_mapping[3] = 20;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 4;
        data->table_way_to_macro_mapping[1] = 5;
        data->table_way_to_macro_mapping[2] = 19;
        data->table_way_to_macro_mapping[3] = 20;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 3;
        data->table_way_to_macro_mapping[1] = 5;
        data->table_way_to_macro_mapping[2] = 11;
        data->table_way_to_macro_mapping[3] = 13;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MC_ID < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MC_ID, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 4;
        data->table_way_to_macro_mapping[1] = 7;
        data->table_way_to_macro_mapping[2] = 14;
        data->table_way_to_macro_mapping[3] = 16;
        data->table_way_to_macro_mapping[4] = 17;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_0, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 5;
        data->table_way_to_macro_mapping[1] = 7;
        data->table_way_to_macro_mapping[2] = 7;
        data->table_way_to_macro_mapping[3] = 10;
        data->table_way_to_macro_mapping[4] = 15;
        data->table_way_to_macro_mapping[5] = 15;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_1, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 0;
        data->table_way_to_macro_mapping[1] = 1;
        data->table_way_to_macro_mapping[2] = 4;
        data->table_way_to_macro_mapping[3] = 8;
        data->table_way_to_macro_mapping[4] = 8;
        data->table_way_to_macro_mapping[5] = 11;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 6;
        data->table_way_to_macro_mapping[1] = 14;
        data->table_way_to_macro_mapping[2] = 16;
        data->table_way_to_macro_mapping[3] = 20;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 5;
        data->table_way_to_macro_mapping[1] = 9;
        data->table_way_to_macro_mapping[2] = 20;
        data->table_way_to_macro_mapping[3] = 20;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 1;
        data->table_way_to_macro_mapping[1] = 20;
        data->table_way_to_macro_mapping[2] = 20;
        data->table_way_to_macro_mapping[3] = 20;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 0;
        data->table_way_to_macro_mapping[1] = 1;
        data->table_way_to_macro_mapping[2] = 12;
        data->table_way_to_macro_mapping[3] = 20;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_0, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 7;
        data->table_way_to_macro_mapping[1] = 17;
        data->table_way_to_macro_mapping[2] = 18;
        data->table_way_to_macro_mapping[3] = 18;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_1, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 11;
        data->table_way_to_macro_mapping[1] = 13;
        data->table_way_to_macro_mapping[2] = 18;
        data->table_way_to_macro_mapping[3] = 18;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ESEM, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 0;
        data->table_way_to_macro_mapping[1] = 6;
        data->table_way_to_macro_mapping[2] = 11;
        data->table_way_to_macro_mapping[3] = 14;
        data->table_way_to_macro_mapping[4] = 15;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EVSI, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 7;
        data->table_way_to_macro_mapping[1] = 11;
        data->table_way_to_macro_mapping[2] = 13;
        data->table_way_to_macro_mapping[3] = 20;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_1, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 2;
        data->table_way_to_macro_mapping[1] = 3;
        data->table_way_to_macro_mapping[2] = 20;
        data->table_way_to_macro_mapping[3] = 20;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_2, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 8;
        data->table_way_to_macro_mapping[1] = 9;
        data->table_way_to_macro_mapping[2] = 20;
        data->table_way_to_macro_mapping[3] = 20;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 2;
        data->table_way_to_macro_mapping[1] = 3;
        data->table_way_to_macro_mapping[2] = 20;
        data->table_way_to_macro_mapping[3] = 20;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 1;
        data->table_way_to_macro_mapping[1] = 10;
        data->table_way_to_macro_mapping[2] = 20;
        data->table_way_to_macro_mapping[3] = 20;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_RMEP < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_table_way_to_macro_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_RMEP, 0);
        SHR_RANGE_VERIFY(12, 0, DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS, _SHR_E_INTERNAL, "out of bound access to array")
        data->table_way_to_macro_mapping[0] = 0;
        data->table_way_to_macro_mapping[1] = 16;
        data->table_way_to_macro_mapping[2] = 20;
        data->table_way_to_macro_mapping[3] = 20;
        data->table_way_to_macro_mapping[4] = 20;
        data->table_way_to_macro_mapping[5] = 20;
        data->table_way_to_macro_mapping[6] = 20;
        data->table_way_to_macro_mapping[7] = 20;
        data->table_way_to_macro_mapping[8] = 20;
        data->table_way_to_macro_mapping[9] = 20;
        data->table_way_to_macro_mapping[10] = 20;
        data->table_way_to_macro_mapping[11] = 20;
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
    table->values[1].default_val = "INVALIDm";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_dh_block_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_dh_table_block_info");

    
    default_data = (dnx_data_mdb_dh_block_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->block_type = DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_NONE;
    default_data->block_index = INVALIDm;
    
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
    }
    if (MDB_MACRO_B < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_type_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_MACRO_B, 0);
        data->nof_macros = 8;
        data->nof_clusters = 8;
        data->nof_rows = 8192;
        data->nof_address_bits = 13;
        data->global_start_index = 12;
    }
    if (MDB_EEDB_BANK < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_macro_type_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EEDB_BANK, 0);
        data->nof_macros = 1;
        data->nof_clusters = 32;
        data->nof_rows = 2048;
        data->nof_address_bits = 11;
        data->global_start_index = 0;
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
    table->values[1].default_val = "INVALIDm";
    table->values[2].default_val = "INVALIDm";
    table->values[3].default_val = "INVALIDm";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_dh_entry_banks_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_dh_table_entry_banks_info");

    
    default_data = (dnx_data_mdb_dh_entry_banks_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->entry_bank = INVALIDm;
    default_data->overflow_reg = INVALIDm;
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

    
    table->values[0].default_val = "DBAL_TABLE_EMPTY";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_dh_mdb_1_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_dh_table_mdb_1_info");

    
    default_data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = DBAL_TABLE_EMPTY;
    default_data->mdb_item_2_field_size = 0;
    default_data->mdb_item_3_array_size = 0;
    default_data->mdb_item_4_field_size = 0;
    
    for (global_macro_index_index = 0; global_macro_index_index < table->keys[0].size; global_macro_index_index++)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, global_macro_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_0;
        data->mdb_item_2_field_size = 3;
        data->mdb_item_3_array_size = 7;
        data->mdb_item_4_field_size = 6;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_1;
        data->mdb_item_2_field_size = 3;
        data->mdb_item_3_array_size = 7;
        data->mdb_item_4_field_size = 6;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_2;
        data->mdb_item_2_field_size = 3;
        data->mdb_item_3_array_size = 8;
        data->mdb_item_4_field_size = 6;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_3;
        data->mdb_item_2_field_size = 3;
        data->mdb_item_3_array_size = 8;
        data->mdb_item_4_field_size = 6;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_4;
        data->mdb_item_2_field_size = 3;
        data->mdb_item_3_array_size = 8;
        data->mdb_item_4_field_size = 6;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_5;
        data->mdb_item_2_field_size = 3;
        data->mdb_item_3_array_size = 8;
        data->mdb_item_4_field_size = 6;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_6;
        data->mdb_item_2_field_size = 3;
        data->mdb_item_3_array_size = 8;
        data->mdb_item_4_field_size = 6;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_7;
        data->mdb_item_2_field_size = 4;
        data->mdb_item_3_array_size = 9;
        data->mdb_item_4_field_size = 7;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_8;
        data->mdb_item_2_field_size = 3;
        data->mdb_item_3_array_size = 8;
        data->mdb_item_4_field_size = 6;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_9;
        data->mdb_item_2_field_size = 3;
        data->mdb_item_3_array_size = 8;
        data->mdb_item_4_field_size = 6;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_10;
        data->mdb_item_2_field_size = 4;
        data->mdb_item_3_array_size = 9;
        data->mdb_item_4_field_size = 7;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_11;
        data->mdb_item_2_field_size = 3;
        data->mdb_item_3_array_size = 8;
        data->mdb_item_4_field_size = 6;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_12;
        data->mdb_item_2_field_size = 3;
        data->mdb_item_3_array_size = 8;
        data->mdb_item_4_field_size = 6;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_13;
        data->mdb_item_2_field_size = 3;
        data->mdb_item_3_array_size = 8;
        data->mdb_item_4_field_size = 6;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_14;
        data->mdb_item_2_field_size = 3;
        data->mdb_item_3_array_size = 8;
        data->mdb_item_4_field_size = 6;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_15;
        data->mdb_item_2_field_size = 3;
        data->mdb_item_3_array_size = 8;
        data->mdb_item_4_field_size = 6;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_16;
        data->mdb_item_2_field_size = 3;
        data->mdb_item_3_array_size = 8;
        data->mdb_item_4_field_size = 6;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_17;
        data->mdb_item_2_field_size = 3;
        data->mdb_item_3_array_size = 8;
        data->mdb_item_4_field_size = 6;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_18;
        data->mdb_item_2_field_size = 3;
        data->mdb_item_3_array_size = 8;
        data->mdb_item_4_field_size = 6;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_mdb_dh_mdb_1_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->dbal_table = DBAL_TABLE_MDB_1_GLOBAL_INDEX_19;
        data->mdb_item_2_field_size = 3;
        data->mdb_item_3_array_size = 8;
        data->mdb_item_4_field_size = 6;
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

    
    table->values[0].default_val = "DBAL_TABLE_EMPTY";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_dh_mdb_2_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_dh_table_mdb_2_info");

    
    default_data = (dnx_data_mdb_dh_mdb_2_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = DBAL_TABLE_EMPTY;
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

    
    table->values[0].default_val = "DBAL_TABLE_EMPTY";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_dh_mdb_3_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_dh_table_mdb_3_info");

    
    default_data = (dnx_data_mdb_dh_mdb_3_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = DBAL_TABLE_EMPTY;
    default_data->mdb_item_0_array_size = 0;
    
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
jr2_a0_dnx_data_mdb_pdbs_kaps_nof_blocks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_kaps_nof_blocks;
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
        "SoC property to select the MDB profile.\n"
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
jr2_a0_dnx_data_mdb_pdbs_pdbs_info_set(
    int unit)
{
    int mdb_table_id_index;
    dnx_data_mdb_pdbs_pdbs_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int table_index = dnx_data_mdb_pdbs_table_pdbs_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;

    
    table->values[0].default_val = "DBAL_NOF_ENUM_MDB_DB_TYPE_VALUES";
    table->values[1].default_val = "0";
    table->values[2].default_val = "DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES";
    table->values[3].default_val = "DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_pdbs_pdbs_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_pdbs_table_pdbs_info");

    
    default_data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->db_type = DBAL_NOF_ENUM_MDB_DB_TYPE_VALUES;
    default_data->row_width = 0;
    default_data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    default_data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    
    for (mdb_table_id_index = 0; mdb_table_id_index < table->keys[0].size; mdb_table_id_index++)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_1, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_60B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_180B;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IVSI, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_90B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_90B;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_2, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_60B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_180B;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_3, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_60B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_180B;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_2, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS;
        data->row_width = 4*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS;
        data->row_width = 4*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_0, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_1, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MAP < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MAP, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = dnx_data_mdb.direct.map_payload_size_enum_get(unit);
        data->direct_max_payload_type = dnx_data_mdb.direct.map_payload_size_enum_get(unit);
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_150B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_150B;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_150B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_150B;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_150B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_150B;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MC_ID < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MC_ID, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_0, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_1, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_LL, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_DATA, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_LL, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_DATA, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_LL, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_DATA, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_LL, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_DATA, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_LL, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_DATA, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_LL, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_DATA, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_LL, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_DATA, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_LL, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_DATA, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_0, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_1, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ESEM, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EVSI, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_30B;
        data->direct_max_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_30B;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_1, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_2, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_RMEP < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdbs_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_RMEP, 0);
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->direct_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
        data->direct_max_payload_type = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_pdb_info_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_pdbs_pdb_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int table_index = dnx_data_mdb_pdbs_table_pdb_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "DBAL_NOF_ENUM_MDB_DB_TYPE_VALUES";
    table->values[1].default_val = "DBAL_NOF_ENUM_MDB_DB_TYPE_VALUES";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_pdbs_pdb_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_pdbs_table_pdb_info");

    
    default_data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->logical_to_physical = DBAL_NOF_ENUM_MDB_DB_TYPE_VALUES;
    default_data->db_type = DBAL_NOF_ENUM_MDB_DB_TYPE_VALUES;
    default_data->row_width = 0;
    default_data->max_key_size = 0;
    default_data->max_payload_size = 0;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_NONE < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_NONE, 0);
        data->logical_to_physical = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->db_type = DBAL_NOF_ENUM_MDB_DB_TYPE_VALUES;
        data->row_width = 0;
        data->max_key_size = 0;
        data->max_payload_size = 0;
    }
    if (DBAL_PHYSICAL_TABLE_TCAM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_TCAM, 0);
        data->logical_to_physical = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_TCAM;
        data->row_width = 0;
        data->max_key_size = 160;
        data->max_payload_size = 64;
    }
    if (DBAL_PHYSICAL_TABLE_KBP < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_KBP, 0);
        data->logical_to_physical = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_TCAM;
        data->row_width = 0;
        data->max_key_size = 160;
        data->max_payload_size = 64;
    }
    if (DBAL_PHYSICAL_TABLE_KAPS_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_KAPS_1, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 160;
        data->max_payload_size = 20;
    }
    if (DBAL_PHYSICAL_TABLE_KAPS_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_KAPS_2, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_2;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 160;
        data->max_payload_size = 20;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_1, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 48;
        data->max_payload_size = 24;
    }
    if (DBAL_PHYSICAL_TABLE_INLIF_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_INLIF_1, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 180;
    }
    if (DBAL_PHYSICAL_TABLE_IVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IVSI, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IVSI;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 18;
        data->max_payload_size = 90;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_2, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 88;
        data->max_payload_size = 24;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_3, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 88;
        data->max_payload_size = 24;
    }
    if (DBAL_PHYSICAL_TABLE_INLIF_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_INLIF_2, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_2;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 180;
    }
    if (DBAL_PHYSICAL_TABLE_INLIF_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_INLIF_3, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_3;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 180;
    }
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 160;
        data->max_payload_size = 96;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_1, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_0;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 32;
        data->max_payload_size = 48;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_2, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 32;
        data->max_payload_size = 48;
    }
    if (DBAL_PHYSICAL_TABLE_MAP < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_MAP, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MAP;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 120;
    }
    if (DBAL_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_FEC_1, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_FEC_2, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_FEC_3, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_PPMC < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_PPMC, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MC_ID;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 24;
        data->max_payload_size = 110;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_1, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_0;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 24;
        data->max_payload_size = 24;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_2, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 24;
        data->max_payload_size = 24;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_1, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_2, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_3, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_4, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_5 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_5, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_6 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_6, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_7 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_7, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_8 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_8, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 20;
        data->max_payload_size = 150;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_0;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 32;
        data->max_payload_size = 48;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 32;
        data->max_payload_size = 48;
    }
    if (DBAL_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ESEM, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ESEM;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 48;
        data->max_payload_size = 90;
    }
    if (DBAL_PHYSICAL_TABLE_EVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EVSI, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EVSI;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 18;
        data->max_payload_size = 30;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_1, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_1;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 80;
        data->max_payload_size = 60;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_2, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_2;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 80;
        data->max_payload_size = 60;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 80;
        data->max_payload_size = 60;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = 2*dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 160;
        data->max_payload_size = 60;
    }
    if (DBAL_PHYSICAL_TABLE_RMEP_EM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_pdb_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_RMEP_EM, 0);
        data->logical_to_physical = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_RMEP;
        data->db_type = DBAL_ENUM_FVAL_MDB_DB_TYPE_EM;
        data->row_width = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        data->max_key_size = 56;
        data->max_payload_size = 110;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_mdb_11_info_set(
    int unit)
{
    int mdb_table_index;
    dnx_data_mdb_pdbs_mdb_11_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int table_index = dnx_data_mdb_pdbs_table_mdb_11_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;

    
    table->values[0].default_val = "DBAL_TABLE_EMPTY";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    table->values[6].default_val = "0";
    table->values[7].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_pdbs_mdb_11_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_pdbs_table_mdb_11_info");

    
    default_data = (dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = DBAL_TABLE_EMPTY;
    default_data->mdb_item_0_array_size = 0;
    default_data->mdb_item_0_field_size = 0;
    default_data->mdb_item_1_field_size = 0;
    default_data->mdb_item_3_array_size = 0;
    default_data->mdb_item_3_field_size = 0;
    default_data->mdb_item_4_array_size = 0;
    default_data->mdb_item_4_field_size = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_INLIF_1;
        data->mdb_item_0_array_size = 16;
        data->mdb_item_0_field_size = 3;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IVSI, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_IVSI;
        data->mdb_item_0_array_size = 16;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_INLIF_2;
        data->mdb_item_0_array_size = 16;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_INLIF_3;
        data->mdb_item_0_array_size = 16;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_ADS_1;
        data->mdb_item_0_array_size = 8;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_ADS_2;
        data->mdb_item_0_array_size = 8;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MAP < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MAP, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_MAP;
        data->mdb_item_0_array_size = 16;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_FEC_1;
        data->mdb_item_0_array_size = 32;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_FEC_2;
        data->mdb_item_0_array_size = 32;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_FEC_3;
        data->mdb_item_0_array_size = 32;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_LL, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_LL_1;
        data->mdb_item_0_array_size = 64;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_EEDB_1;
        data->mdb_item_0_array_size = 128;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 2;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 2;
        data->mdb_item_4_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_LL, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_LL_2;
        data->mdb_item_0_array_size = 64;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_EEDB_2;
        data->mdb_item_0_array_size = 128;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 2;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 2;
        data->mdb_item_4_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_LL, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_LL_3;
        data->mdb_item_0_array_size = 64;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_EEDB_3;
        data->mdb_item_0_array_size = 128;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 2;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 2;
        data->mdb_item_4_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_LL < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_LL, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_LL_4;
        data->mdb_item_0_array_size = 64;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 1;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 1;
        data->mdb_item_4_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA, 0);
        data->dbal_table = DBAL_TABLE_MDB_11_EEDB_4;
        data->mdb_item_0_array_size = 128;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 3;
        data->mdb_item_3_array_size = 2;
        data->mdb_item_3_field_size = 3;
        data->mdb_item_4_array_size = 2;
        data->mdb_item_4_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_11_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EVSI, 0);
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
jr2_a0_dnx_data_mdb_pdbs_mdb_12_info_set(
    int unit)
{
    int mdb_table_index;
    dnx_data_mdb_pdbs_mdb_12_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int table_index = dnx_data_mdb_pdbs_table_mdb_12_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;

    
    table->values[0].default_val = "DBAL_TABLE_EMPTY";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_pdbs_mdb_12_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_pdbs_table_mdb_12_info");

    
    default_data = (dnx_data_mdb_pdbs_mdb_12_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = DBAL_TABLE_EMPTY;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_pdbs_mdb_12_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_12_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_12_FEC_1;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_12_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_12_FEC_2;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_12_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_12_FEC_3;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_pdbs_mdb_26_info_set(
    int unit)
{
    int mdb_table_index;
    dnx_data_mdb_pdbs_mdb_26_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int table_index = dnx_data_mdb_pdbs_table_mdb_26_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_pdbs_mdb_26_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_pdbs_table_mdb_26_info");

    
    default_data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->mdb_item_0_field_size = 0;
    default_data->mdb_item_1_field_size = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_ISEM_1;
        data->mdb_item_0_field_size = 8;
        data->mdb_item_1_field_size = 8;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_INLIF_1;
        data->mdb_item_0_field_size = 6;
        data->mdb_item_1_field_size = 6;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IVSI, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_IVSI;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 2;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_ISEM_2;
        data->mdb_item_0_field_size = 7;
        data->mdb_item_1_field_size = 7;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_INLIF_2;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 1;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_ISEM_3;
        data->mdb_item_0_field_size = 7;
        data->mdb_item_1_field_size = 7;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_INLIF_3;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 2;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_LEM;
        data->mdb_item_0_field_size = 11;
        data->mdb_item_1_field_size = 11;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_ADS_1;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 2;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_ADS_2;
        data->mdb_item_0_field_size = 1;
        data->mdb_item_1_field_size = 1;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_KAPS_1;
        data->mdb_item_0_field_size = 11;
        data->mdb_item_1_field_size = 11;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_KAPS_2;
        data->mdb_item_0_field_size = 6;
        data->mdb_item_1_field_size = 6;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_IOEM_0;
        data->mdb_item_0_field_size = 4;
        data->mdb_item_1_field_size = 4;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_IOEM_1;
        data->mdb_item_0_field_size = 4;
        data->mdb_item_1_field_size = 4;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MAP < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MAP, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_MAP;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 2;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_FEC_1;
        data->mdb_item_0_field_size = 3;
        data->mdb_item_1_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_FEC_2;
        data->mdb_item_0_field_size = 3;
        data->mdb_item_1_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_FEC_3;
        data->mdb_item_0_field_size = 4;
        data->mdb_item_1_field_size = 4;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MC_ID < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MC_ID, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_MC_ID;
        data->mdb_item_0_field_size = 5;
        data->mdb_item_1_field_size = 5;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_GLEM_0;
        data->mdb_item_0_field_size = 6;
        data->mdb_item_1_field_size = 6;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_GLEM_1;
        data->mdb_item_0_field_size = 6;
        data->mdb_item_1_field_size = 6;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EEDB_1;
        data->mdb_item_0_field_size = 4;
        data->mdb_item_1_field_size = 4;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EEDB_2;
        data->mdb_item_0_field_size = 3;
        data->mdb_item_1_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EEDB_3;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 2;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EEDB_4;
        data->mdb_item_0_field_size = 4;
        data->mdb_item_1_field_size = 4;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EOEM_0;
        data->mdb_item_0_field_size = 4;
        data->mdb_item_1_field_size = 4;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EOEM_1;
        data->mdb_item_0_field_size = 4;
        data->mdb_item_1_field_size = 4;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ESEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_ESEM;
        data->mdb_item_0_field_size = 5;
        data->mdb_item_1_field_size = 5;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EVSI, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EVSI;
        data->mdb_item_0_field_size = 3;
        data->mdb_item_1_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EXEM_1;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 2;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EXEM_2;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 2;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EXEM_3;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 2;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_EXEM_4;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 2;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_RMEP < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_26_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_RMEP, 0);
        data->dbal_table = DBAL_TABLE_MDB_26_RMEP;
        data->mdb_item_0_field_size = 2;
        data->mdb_item_1_field_size = 2;
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
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_pdbs_mdb_adapter_mapping_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_pdbs_table_mdb_adapter_mapping");

    
    default_data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->memory_id = 0;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_NONE < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_NONE, 0);
        data->memory_id = 0;
    }
    if (DBAL_PHYSICAL_TABLE_TCAM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_TCAM, 0);
        data->memory_id = 0;
    }
    if (DBAL_PHYSICAL_TABLE_KBP < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_KBP, 0);
        data->memory_id = 0;
    }
    if (DBAL_PHYSICAL_TABLE_KAPS_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_KAPS_1, 0);
        data->memory_id = 185;
    }
    if (DBAL_PHYSICAL_TABLE_KAPS_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_KAPS_2, 0);
        data->memory_id = 186;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_1, 0);
        data->memory_id = 0;
    }
    if (DBAL_PHYSICAL_TABLE_INLIF_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_INLIF_1, 0);
        data->memory_id = 169;
    }
    if (DBAL_PHYSICAL_TABLE_IVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IVSI, 0);
        data->memory_id = 172;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_2, 0);
        data->memory_id = 1;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_3, 0);
        data->memory_id = 2;
    }
    if (DBAL_PHYSICAL_TABLE_INLIF_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_INLIF_2, 0);
        data->memory_id = 170;
    }
    if (DBAL_PHYSICAL_TABLE_INLIF_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_INLIF_3, 0);
        data->memory_id = 171;
    }
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->memory_id = 10;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_1, 0);
        data->memory_id = 5;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_2, 0);
        data->memory_id = 6;
    }
    if (DBAL_PHYSICAL_TABLE_MAP < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_MAP, 0);
        data->memory_id = 173;
    }
    if (DBAL_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_FEC_1, 0);
        data->memory_id = 174;
    }
    if (DBAL_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_FEC_2, 0);
        data->memory_id = 175;
    }
    if (DBAL_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_FEC_3, 0);
        data->memory_id = 176;
    }
    if (DBAL_PHYSICAL_TABLE_PPMC < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_PPMC, 0);
        data->memory_id = 158;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_1, 0);
        data->memory_id = 3;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_2, 0);
        data->memory_id = 4;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_1, 0);
        data->memory_id = 177;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_2, 0);
        data->memory_id = 177;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_3, 0);
        data->memory_id = 178;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_4, 0);
        data->memory_id = 178;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_5 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_5, 0);
        data->memory_id = 179;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_6 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_6, 0);
        data->memory_id = 179;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_7 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_7, 0);
        data->memory_id = 180;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_8 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_8, 0);
        data->memory_id = 180;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->memory_id = 7;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->memory_id = 8;
    }
    if (DBAL_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ESEM, 0);
        data->memory_id = 9;
    }
    if (DBAL_PHYSICAL_TABLE_EVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EVSI, 0);
        data->memory_id = 181;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_1, 0);
        data->memory_id = 79;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_2, 0);
        data->memory_id = 84;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->memory_id = 89;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->memory_id = 101;
    }
    if (DBAL_PHYSICAL_TABLE_RMEP_EM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_RMEP_EM, 0);
        data->memory_id = 12;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


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
jr2_a0_dnx_data_mdb_em_interrupt_register_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_interrupt_register;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = MDB_REG_0000r;

    
    define->data = MDB_REG_0000r;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


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
jr2_a0_dnx_data_mdb_em_em_aging_info_set(
    int unit)
{
    int emp_table_index;
    dnx_data_mdb_em_em_aging_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_em_aging_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_MDB_EMP_TABLE_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_MDB_EMP_TABLE_VALUES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_em_aging_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_em_aging_info");

    
    default_data = (dnx_data_mdb_em_em_aging_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->max_nof_age_entry_bits = 0;
    default_data->rbd_size = 0;
    default_data->total_nof_aging_bits = 0;
    
    for (emp_table_index = 0; emp_table_index < table->keys[0].size; emp_table_index++)
    {
        data = (dnx_data_mdb_em_em_aging_info_t *) dnxc_data_mgmt_table_data_get(unit, table, emp_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_MDB_EMP_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_aging_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_EMP_TABLE_LEM, 0);
        data->max_nof_age_entry_bits = 4;
        data->rbd_size = 1;
        data->total_nof_aging_bits = 5242752;
    }
    if (DBAL_ENUM_FVAL_MDB_EMP_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_aging_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_EMP_TABLE_EXEM_3, 0);
        data->max_nof_age_entry_bits = 8;
        data->rbd_size = 0;
        data->total_nof_aging_bits = 524288;
    }
    if (DBAL_ENUM_FVAL_MDB_EMP_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_aging_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_EMP_TABLE_EXEM_4, 0);
        data->max_nof_age_entry_bits = 8;
        data->rbd_size = 0;
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
    int emp_table_index;
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
    table->keys[1].size = DBAL_NOF_ENUM_MDB_EMP_TABLE_VALUES;
    table->info_get.key_size[1] = DBAL_NOF_ENUM_MDB_EMP_TABLE_VALUES;

    
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
        for (emp_table_index = 0; emp_table_index < table->keys[1].size; emp_table_index++)
        {
            data = (dnx_data_mdb_em_em_aging_cfg_t *) dnxc_data_mgmt_table_data_get(unit, table, profile_index, emp_table_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (1 < table->keys[0].size && DBAL_ENUM_FVAL_MDB_EMP_TABLE_LEM < table->keys[1].size)
    {
        data = (dnx_data_mdb_em_em_aging_cfg_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, DBAL_ENUM_FVAL_MDB_EMP_TABLE_LEM);
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
        data->hit_bit_mode = DBAL_ENUM_FVAL_MDB_EM_AGING_HIT_BIT_MODE_A_OR_B;
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
    table->values[2].default_val = "INVALIDm";
    table->values[3].default_val = "INVALIDm";
    table->values[4].default_val = "INVALIDr";
    table->values[5].default_val = "INVALIDf";
    table->values[6].default_val = "INVALIDf";
    table->values[7].default_val = "0";
    table->values[8].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_em_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_em_info");

    
    default_data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->tid_size = 0;
    default_data->em_interface = INVALIDm;
    default_data->age_mem = INVALIDm;
    default_data->age_ovf_cam_mem = INVALIDm;
    default_data->status_reg = INVALIDr;
    default_data->emp_scan_status_field = INVALIDf;
    default_data->interrupt_field = INVALIDf;
    default_data->step_table_size = 0;
    default_data->ovf_cam_size = 0;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_1, 0);
        data->tid_size = 6;
        data->em_interface = MDB_ISEM_1m;
        data->status_reg = MDB_REG_115r;
        data->interrupt_field = FIELD_1_1f;
        data->step_table_size = 1024;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_2, 0);
        data->tid_size = 6;
        data->em_interface = MDB_ISEM_2m;
        data->status_reg = MDB_REG_136r;
        data->interrupt_field = FIELD_2_2f;
        data->step_table_size = 1024;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_3, 0);
        data->tid_size = 6;
        data->em_interface = MDB_ISEM_3m;
        data->status_reg = MDB_REG_157r;
        data->interrupt_field = FIELD_3_3f;
        data->step_table_size = 1024;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->tid_size = 6;
        data->em_interface = MDB_LEMm;
        data->age_mem = MDB_MEM_0200000m;
        data->age_ovf_cam_mem = MDB_MEM_25000m;
        data->status_reg = MDB_REG_212r;
        data->emp_scan_status_field = ITEM_36_37f;
        data->interrupt_field = FIELD_4_4f;
        data->step_table_size = 2048;
        data->ovf_cam_size = 8;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_1, 0);
        data->tid_size = 6;
        data->em_interface = MDB_IOEM_0m;
        data->status_reg = MDB_REG_233r;
        data->interrupt_field = FIELD_5_5f;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_2, 0);
        data->tid_size = 6;
        data->em_interface = MDB_IOEM_1m;
        data->status_reg = MDB_REG_250r;
        data->interrupt_field = FIELD_6_6f;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_PPMC < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_PPMC, 0);
        data->tid_size = 2;
        data->em_interface = MDB_MC_IDm;
        data->status_reg = MDB_REG_274r;
        data->interrupt_field = FIELD_7_7f;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_1, 0);
        data->tid_size = 2;
        data->em_interface = MDB_GLEM_0m;
        data->status_reg = MDB_REG_294r;
        data->interrupt_field = FIELD_8_8f;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_2, 0);
        data->tid_size = 2;
        data->em_interface = MDB_GLEM_1m;
        data->status_reg = MDB_REG_310r;
        data->interrupt_field = FIELD_9_9f;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->tid_size = 6;
        data->em_interface = MDB_EOEM_0m;
        data->status_reg = MDB_REG_335r;
        data->interrupt_field = FIELD_10_10f;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->tid_size = 6;
        data->em_interface = MDB_EOEM_1m;
        data->status_reg = MDB_REG_352r;
        data->interrupt_field = FIELD_11_11f;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ESEM, 0);
        data->tid_size = 6;
        data->em_interface = MDB_ESEMm;
        data->status_reg = MDB_REG_377r;
        data->interrupt_field = FIELD_12_12f;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_1, 0);
        data->tid_size = 6;
        data->em_interface = MDB_EXEM_1m;
        data->status_reg = MDB_REG_398r;
        data->interrupt_field = FIELD_13_13f;
        data->step_table_size = 512;
        data->ovf_cam_size = 2;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_2, 0);
        data->tid_size = 6;
        data->em_interface = MDB_EXEM_2m;
        data->status_reg = MDB_REG_419r;
        data->interrupt_field = FIELD_14_14f;
        data->step_table_size = 512;
        data->ovf_cam_size = 2;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->tid_size = 6;
        data->em_interface = MDB_EXEM_3m;
        data->age_mem = MDB_MEM_210000m;
        data->age_ovf_cam_mem = MDB_MEM_94000m;
        data->status_reg = MDB_REG_472r;
        data->emp_scan_status_field = ITEM_36_37f;
        data->interrupt_field = FIELD_15_15f;
        data->step_table_size = 512;
        data->ovf_cam_size = 2;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->tid_size = 6;
        data->em_interface = MDB_EXEM_4m;
        data->age_mem = MDB_MEM_220000m;
        data->age_ovf_cam_mem = MDB_MEM_103000m;
        data->status_reg = MDB_REG_525r;
        data->emp_scan_status_field = ITEM_36_37f;
        data->interrupt_field = FIELD_16_16f;
        data->step_table_size = 512;
        data->ovf_cam_size = 2;
    }
    if (DBAL_PHYSICAL_TABLE_RMEP_EM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_RMEP_EM, 0);
        data->tid_size = 3;
        data->em_interface = MDB_RMEPm;
        data->status_reg = MDB_REG_545r;
        data->interrupt_field = FIELD_17_17f;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
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
jr2_a0_dnx_data_mdb_em_mdb_13_info_set(
    int unit)
{
    int mdb_table_index;
    dnx_data_mdb_em_mdb_13_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_13_info;
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

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_13_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_13_info");

    
    default_data = (dnx_data_mdb_em_mdb_13_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_13_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_13_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_13_ISEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_13_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_13_ISEM_2;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_13_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_13_ISEM_3;
    }
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_13_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_13_LEM;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_13_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_13_IOEM_0;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_13_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_13_IOEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_PPMC < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_13_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_PPMC, 0);
        data->dbal_table = DBAL_TABLE_MDB_13_MC_ID;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_13_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_13_GLEM_0;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_13_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_13_GLEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_13_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_13_EOEM_0;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_13_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_13_EOEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_13_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ESEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_13_ESEM;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_13_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_13_EXEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_13_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_13_EXEM_2;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_13_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_13_EXEM_3;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_13_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_13_EXEM_4;
    }
    if (DBAL_PHYSICAL_TABLE_RMEP_EM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_13_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_RMEP_EM, 0);
        data->dbal_table = DBAL_TABLE_MDB_13_RMEP;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_mdb_15_info_set(
    int unit)
{
    int mdb_table_index;
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
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_15_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
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
    int mdb_table_index;
    dnx_data_mdb_em_mdb_16_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_16_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;

    
    table->values[0].default_val = "DBAL_TABLE_EMPTY";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_16_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_16_info");

    
    default_data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = DBAL_TABLE_EMPTY;
    default_data->mdb_item_0_field_size = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_ISEM_1;
        data->mdb_item_0_field_size = 9;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_ISEM_2;
        data->mdb_item_0_field_size = 8;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_ISEM_3;
        data->mdb_item_0_field_size = 8;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_LEM;
        data->mdb_item_0_field_size = 12;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_IOEM_0;
        data->mdb_item_0_field_size = 5;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_IOEM_1;
        data->mdb_item_0_field_size = 5;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MC_ID < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MC_ID, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_MC_ID;
        data->mdb_item_0_field_size = 6;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_GLEM_0;
        data->mdb_item_0_field_size = 7;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_GLEM_1;
        data->mdb_item_0_field_size = 7;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_EOEM_0;
        data->mdb_item_0_field_size = 5;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_EOEM_1;
        data->mdb_item_0_field_size = 5;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ESEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_ESEM;
        data->mdb_item_0_field_size = 6;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_EXEM_1;
        data->mdb_item_0_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_EXEM_2;
        data->mdb_item_0_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_EXEM_3;
        data->mdb_item_0_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_EXEM_4;
        data->mdb_item_0_field_size = 3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_RMEP < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_RMEP, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_RMEP;
        data->mdb_item_0_field_size = 3;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_mdb_18_info_set(
    int unit)
{
    int mdb_table_index;
    dnx_data_mdb_em_mdb_18_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_18_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "DBAL_TABLE_EMPTY";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_18_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_18_info");

    
    default_data = (dnx_data_mdb_em_mdb_18_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = DBAL_TABLE_EMPTY;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_18_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_18_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_18_LEM;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_18_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_18_EXEM_3;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_18_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_18_EXEM_4;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_mdb_21_info_set(
    int unit)
{
    int mdb_table_index;
    dnx_data_mdb_em_mdb_21_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_21_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;

    
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
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_21_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_21_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_21_LEM;
        data->mdb_item_0_field_size = 1;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_21_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_21_EXEM_3;
        data->mdb_item_0_field_size = 2;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_21_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4, 0);
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
    int mdb_table_index;
    dnx_data_mdb_em_mdb_23_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_23_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;

    
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
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_23_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_23_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_23_LEM;
        data->mdb_item_0_field_size = 12;
        data->mdb_item_1_field_size = 4;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_23_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_23_EXEM_3;
        data->mdb_item_0_field_size = 3;
        data->mdb_item_1_field_size = 2;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_23_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4, 0);
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
    int mdb_table_index;
    dnx_data_mdb_em_mdb_24_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_24_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_24_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_24_info");

    
    default_data = (dnx_data_mdb_em_mdb_24_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->mdb_item_0_array_size = 0;
    default_data->mdb_item_1_array_size = 0;
    default_data->mdb_item_1_field_size = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_24_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_24_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_24_LEM;
        data->mdb_item_0_array_size = 12;
        data->mdb_item_1_array_size = 12;
        data->mdb_item_1_field_size = 18;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_24_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_24_EXEM_3;
        data->mdb_item_0_array_size = 3;
        data->mdb_item_1_array_size = 3;
        data->mdb_item_1_field_size = 16;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_24_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4, 0);
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
    int mdb_table_index;
    dnx_data_mdb_em_mdb_29_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_29_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_29_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_29_info");

    
    default_data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->mdb_item_0_array_size = 0;
    default_data->mdb_item_1_array_size = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_ISEM_1;
        data->mdb_item_0_array_size = 8;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_ISEM_2;
        data->mdb_item_0_array_size = 7;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_ISEM_3;
        data->mdb_item_0_array_size = 7;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_LEM;
        data->mdb_item_0_array_size = 11;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_IOEM_0;
        data->mdb_item_0_array_size = 4;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_IOEM_1;
        data->mdb_item_0_array_size = 4;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MC_ID < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MC_ID, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_MC_ID;
        data->mdb_item_0_array_size = 5;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_GLEM_0;
        data->mdb_item_0_array_size = 6;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_GLEM_1;
        data->mdb_item_0_array_size = 6;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_EOEM_0;
        data->mdb_item_0_array_size = 4;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_EOEM_1;
        data->mdb_item_0_array_size = 4;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ESEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_ESEM;
        data->mdb_item_0_array_size = 5;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_EXEM_1;
        data->mdb_item_0_array_size = 2;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_EXEM_2;
        data->mdb_item_0_array_size = 2;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_EXEM_3;
        data->mdb_item_0_array_size = 2;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_EXEM_4;
        data->mdb_item_0_array_size = 2;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_RMEP < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_RMEP, 0);
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
    int mdb_table_index;
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
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_31_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_31_info");

    
    default_data = (dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->array_size = 0;
    default_data->mdb_item_0_field_size = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_31_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
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
    int mdb_table_index;
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
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_32_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
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
    int mdb_table_index;
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
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_41_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_41_info");

    
    default_data = (dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->array_size = 0;
    default_data->mdb_item_2_field_size = 0;
    default_data->mdb_item_3_field_size = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_41_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
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
    int mdb_table_index;
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
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_45_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_45_info");

    
    default_data = (dnx_data_mdb_em_mdb_45_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->array_size = 0;
    default_data->mdb_item_3_field_size = 0;
    default_data->mdb_item_7_field_size = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_45_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
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
jr2_a0_dnx_data_mdb_em_mdb_em_tables_info_set(
    int unit)
{
    int mdb_table_index;
    dnx_data_mdb_em_mdb_em_tables_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_em_tables_info;
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
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_em_tables_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_em_tables_info");

    
    default_data = (dnx_data_mdb_em_mdb_em_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->mdb_14_table = 0;
    default_data->mdb_15_table = 0;
    default_data->mdb_17_table = 0;
    default_data->mdb_step_table = 0;
    default_data->mdb_32_table = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_em_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_1, 0);
        data->mdb_14_table = DBAL_TABLE_MDB_14_ISEM_1;
        data->mdb_15_table = DBAL_TABLE_MDB_15_ISEM_1;
        data->mdb_17_table = DBAL_TABLE_MDB_17_ISEM_1;
        data->mdb_step_table = DBAL_TABLE_MDB_31_ISEM_1;
        data->mdb_32_table = DBAL_TABLE_MDB_32_ISEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_2, 0);
        data->mdb_14_table = DBAL_TABLE_MDB_14_ISEM_2;
        data->mdb_15_table = DBAL_TABLE_MDB_15_ISEM_2;
        data->mdb_17_table = DBAL_TABLE_MDB_17_ISEM_2;
        data->mdb_step_table = DBAL_TABLE_MDB_31_ISEM_2;
        data->mdb_32_table = DBAL_TABLE_MDB_32_ISEM_2;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_3, 0);
        data->mdb_14_table = DBAL_TABLE_MDB_14_ISEM_3;
        data->mdb_15_table = DBAL_TABLE_MDB_15_ISEM_3;
        data->mdb_17_table = DBAL_TABLE_MDB_17_ISEM_3;
        data->mdb_step_table = DBAL_TABLE_MDB_31_ISEM_3;
        data->mdb_32_table = DBAL_TABLE_MDB_32_ISEM_3;
    }
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->mdb_14_table = DBAL_TABLE_MDB_14_LEM;
        data->mdb_15_table = DBAL_TABLE_MDB_15_LEM;
        data->mdb_17_table = DBAL_TABLE_MDB_17_LEM;
        data->mdb_step_table = DBAL_TABLE_MDB_41_LEM;
        data->mdb_32_table = DBAL_TABLE_MDB_32_LEM;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_1, 0);
        data->mdb_14_table = DBAL_TABLE_MDB_14_IOEM_0;
        data->mdb_15_table = DBAL_TABLE_MDB_15_IOEM_0;
        data->mdb_17_table = DBAL_TABLE_MDB_17_IOEM_0;
        data->mdb_step_table = DBAL_TABLE_MDB_31_IOEM_0;
        data->mdb_32_table = DBAL_TABLE_MDB_32_IOEM_0;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_2, 0);
        data->mdb_14_table = DBAL_TABLE_MDB_14_IOEM_1;
        data->mdb_15_table = DBAL_TABLE_MDB_15_IOEM_1;
        data->mdb_17_table = DBAL_TABLE_MDB_17_IOEM_1;
        data->mdb_step_table = DBAL_TABLE_MDB_31_IOEM_1;
        data->mdb_32_table = DBAL_TABLE_MDB_32_IOEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_PPMC < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_PPMC, 0);
        data->mdb_14_table = DBAL_TABLE_MDB_14_MC_ID;
        data->mdb_15_table = DBAL_TABLE_MDB_15_MC_ID;
        data->mdb_17_table = DBAL_TABLE_MDB_17_MC_ID;
        data->mdb_step_table = DBAL_TABLE_MDB_31_MC_ID;
        data->mdb_32_table = DBAL_TABLE_MDB_32_MC_ID;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_1, 0);
        data->mdb_14_table = DBAL_TABLE_MDB_14_GLEM_0;
        data->mdb_15_table = DBAL_TABLE_MDB_15_GLEM_0;
        data->mdb_17_table = DBAL_TABLE_MDB_17_GLEM_0;
        data->mdb_step_table = DBAL_TABLE_MDB_31_GLEM_0;
        data->mdb_32_table = DBAL_TABLE_MDB_32_GLEM_0;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_2, 0);
        data->mdb_14_table = DBAL_TABLE_MDB_14_GLEM_1;
        data->mdb_15_table = DBAL_TABLE_MDB_15_GLEM_1;
        data->mdb_17_table = DBAL_TABLE_MDB_17_GLEM_1;
        data->mdb_step_table = DBAL_TABLE_MDB_31_GLEM_1;
        data->mdb_32_table = DBAL_TABLE_MDB_32_GLEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->mdb_14_table = DBAL_TABLE_MDB_14_EOEM_0;
        data->mdb_15_table = DBAL_TABLE_MDB_15_EOEM_0;
        data->mdb_17_table = DBAL_TABLE_MDB_17_EOEM_0;
        data->mdb_step_table = DBAL_TABLE_MDB_31_EOEM_0;
        data->mdb_32_table = DBAL_TABLE_MDB_32_EOEM_0;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->mdb_14_table = DBAL_TABLE_MDB_14_EOEM_1;
        data->mdb_15_table = DBAL_TABLE_MDB_15_EOEM_1;
        data->mdb_17_table = DBAL_TABLE_MDB_17_EOEM_1;
        data->mdb_step_table = DBAL_TABLE_MDB_31_EOEM_1;
        data->mdb_32_table = DBAL_TABLE_MDB_32_EOEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ESEM, 0);
        data->mdb_14_table = DBAL_TABLE_MDB_14_ESEM;
        data->mdb_15_table = DBAL_TABLE_MDB_15_ESEM;
        data->mdb_17_table = DBAL_TABLE_MDB_17_ESEM;
        data->mdb_step_table = DBAL_TABLE_MDB_31_ESEM;
        data->mdb_32_table = DBAL_TABLE_MDB_32_ESEM;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_1, 0);
        data->mdb_14_table = DBAL_TABLE_MDB_14_EXEM_1;
        data->mdb_15_table = DBAL_TABLE_MDB_15_EXEM_1;
        data->mdb_17_table = DBAL_TABLE_MDB_17_EXEM_1;
        data->mdb_step_table = DBAL_TABLE_MDB_31_EXEM_1;
        data->mdb_32_table = DBAL_TABLE_MDB_32_EXEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_2, 0);
        data->mdb_14_table = DBAL_TABLE_MDB_14_EXEM_2;
        data->mdb_15_table = DBAL_TABLE_MDB_15_EXEM_2;
        data->mdb_17_table = DBAL_TABLE_MDB_17_EXEM_2;
        data->mdb_step_table = DBAL_TABLE_MDB_31_EXEM_2;
        data->mdb_32_table = DBAL_TABLE_MDB_32_EXEM_2;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->mdb_14_table = DBAL_TABLE_MDB_14_EXEM_3;
        data->mdb_15_table = DBAL_TABLE_MDB_15_EXEM_3;
        data->mdb_17_table = DBAL_TABLE_MDB_17_EXEM_3;
        data->mdb_step_table = DBAL_TABLE_MDB_31_EXEM_3;
        data->mdb_32_table = DBAL_TABLE_MDB_32_EXEM_3;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->mdb_14_table = DBAL_TABLE_MDB_14_EXEM_4;
        data->mdb_15_table = DBAL_TABLE_MDB_15_EXEM_4;
        data->mdb_17_table = DBAL_TABLE_MDB_17_EXEM_4;
        data->mdb_step_table = DBAL_TABLE_MDB_31_EXEM_4;
        data->mdb_32_table = DBAL_TABLE_MDB_32_EXEM_4;
    }
    if (DBAL_PHYSICAL_TABLE_RMEP_EM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_em_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_RMEP_EM, 0);
        data->mdb_14_table = DBAL_TABLE_MDB_14_RMEP;
        data->mdb_15_table = DBAL_TABLE_MDB_15_RMEP;
        data->mdb_17_table = DBAL_TABLE_MDB_17_RMEP;
        data->mdb_step_table = DBAL_TABLE_MDB_31_RMEP;
        data->mdb_32_table = DBAL_TABLE_MDB_32_RMEP;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_em_mdb_emp_tables_info_set(
    int unit)
{
    int mdb_table_index;
    dnx_data_mdb_em_mdb_emp_tables_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_emp_tables_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_emp_tables_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_emp_tables_info");

    
    default_data = (dnx_data_mdb_em_mdb_emp_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->emp_age_cfg_table = 0;
    default_data->mdb_22_table = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_emp_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_emp_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM, 0);
        data->emp_age_cfg_table = DBAL_TABLE_MDB_19_LEM;
        data->mdb_22_table = DBAL_TABLE_MDB_22_LEM;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_emp_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->emp_age_cfg_table = DBAL_TABLE_MDB_19_EXEM_3;
        data->mdb_22_table = DBAL_TABLE_MDB_22_EXEM_3;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_emp_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4, 0);
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
        data->table_name_arr[0] = DBAL_TABLE_MDB_VMV_SHIFT_WITH_STAGES;
        data->table_name_arr[1] = DBAL_TABLE_MDB_VMV_SHIFT_WITH_STAGES;
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
        data->table_name_arr[0] = DBAL_TABLE_MDB_VMV_SHIFT_WITH_STAGES;
        data->table_name_arr[1] = DBAL_TABLE_MDB_VMV_SHIFT_WITH_STAGES;
        data->table_name_arr[2] = DBAL_TABLE_MDB_VMV_SHIFT_WITH_STAGES;
        data->table_name_arr[3] = DBAL_TABLE_MDB_VMV_SHIFT_WITH_STAGES;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->field_name_arr[0] = DBAL_FIELD_VTT_ST_TOW_LE_VMV_SHIFT_VALUE_MAP;
        data->field_name_arr[1] = DBAL_FIELD_VTT_ST_TOW_LE_VMV_SHIFT_VALUE_MAP;
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
        data->table_name_arr[0] = DBAL_TABLE_MDB_VMV_SHIFT_WITH_STAGES;
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
        data->table_name_arr[0] = DBAL_TABLE_MDB_VMV_SHIFT_WITH_STAGES;
        data->table_name_arr[1] = DBAL_TABLE_MDB_VMV_SHIFT_WITH_STAGES;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->field_name_arr[0] = DBAL_FIELD_VTT_ST_ONE_LE_VMV_SHIFT_VALUE_MAP;
        data->field_name_arr[1] = DBAL_FIELD_VTT_ST_TOW_LE_VMV_SHIFT_VALUE_MAP;
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
        data->table_name_arr[0] = DBAL_TABLE_MDB_VMV_SHIFT_WITH_STAGES;
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
        data->table_name_arr[0] = DBAL_TABLE_MDB_VMV_SHIFT_WITH_STAGES;
        data->table_name_arr[1] = DBAL_TABLE_MDB_VMV_SHIFT_WITH_STAGES;
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
jr2_a0_dnx_data_mdb_direct_map_payload_type_enum_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int define_index = dnx_data_mdb_direct_define_map_payload_type_enum;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_30B;

    
    define->data = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_30B;

    
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
    define->property.mapping[0].val = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_30B;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "60";
    define->property.mapping[1].val = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_60B;
    define->property.mapping[2].name = "120";
    define->property.mapping[2].val = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_120B;

    
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

    
    table->values[0].default_val = "DBAL_NOF_ENUM_MDB_DIRECT_PAYLOAD_VALUES";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_direct_direct_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_direct_table_direct_info");

    
    default_data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->payload_type = DBAL_NOF_ENUM_MDB_DIRECT_PAYLOAD_VALUES;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_KAPS_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_KAPS_1, 0);
        data->payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_120B;
    }
    if (DBAL_PHYSICAL_TABLE_KAPS_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_KAPS_2, 0);
        data->payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_120B;
    }
    if (DBAL_PHYSICAL_TABLE_INLIF_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_INLIF_1, 0);
        data->payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_60B;
    }
    if (DBAL_PHYSICAL_TABLE_IVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IVSI, 0);
        data->payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_90B;
    }
    if (DBAL_PHYSICAL_TABLE_INLIF_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_INLIF_2, 0);
        data->payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_60B;
    }
    if (DBAL_PHYSICAL_TABLE_INLIF_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_INLIF_3, 0);
        data->payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_60B;
    }
    if (DBAL_PHYSICAL_TABLE_MAP < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_MAP, 0);
        data->payload_type = dnx_data_mdb.direct.map_payload_type_enum_get(unit);
    }
    if (DBAL_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_FEC_1, 0);
        data->payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_150B;
    }
    if (DBAL_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_FEC_2, 0);
        data->payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_150B;
    }
    if (DBAL_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_FEC_3, 0);
        data->payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_150B;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_1, 0);
        data->payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_120B;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_2, 0);
        data->payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_120B;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_3, 0);
        data->payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_120B;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_4, 0);
        data->payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_120B;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_5 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_5, 0);
        data->payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_120B;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_6 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_6, 0);
        data->payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_120B;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_7 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_7, 0);
        data->payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_120B;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_8 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_8, 0);
        data->payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_120B;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_120B;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_120B;
    }
    if (DBAL_PHYSICAL_TABLE_EVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_direct_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EVSI, 0);
        data->payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_30B;
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
jr2_a0_dnx_data_mdb_direct_mdb_42_info_set(
    int unit)
{
    int mdb_table_index;
    dnx_data_mdb_direct_mdb_42_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int table_index = dnx_data_mdb_direct_table_mdb_42_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_direct_mdb_42_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_direct_table_mdb_42_info");

    
    default_data = (dnx_data_mdb_direct_mdb_42_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_direct_mdb_42_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_42_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1, 0);
        data->dbal_table = DBAL_TABLE_EMPTY;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_42_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2, 0);
        data->dbal_table = DBAL_TABLE_EMPTY;
    }
    if (DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_42_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3, 0);
        data->dbal_table = DBAL_TABLE_EMPTY;
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
        "The suffix for this property is the physical phase, and the value is the data granularity.\n"
        "Valid data granularity values are 30/60/120 bits.\n"
        "Every physical phase must be configured.\n"
        "See related soc property outlif_logical_to_physical_phase_map.\n"
        "outlif_physical_phase_data_granularity_<Type>=<30|60|120>\n"
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
jr2_a0_dnx_data_mdb_kaps_nof_rows_in_rpb_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_nof_rows_in_rpb;
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
jr2_a0_dnx_data_mdb_kaps_nof_rows_in_small_ads_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_nof_rows_in_small_ads;
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
jr2_a0_dnx_data_mdb_kaps_nof_rows_in_small_bb_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_nof_rows_in_small_bb;
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
jr2_a0_dnx_data_mdb_kaps_nof_rows_in_small_rpb_hitbits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_nof_rows_in_small_rpb_hitbits;
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
jr2_a0_dnx_data_mdb_kaps_ad_width_in_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_ad_width_in_bits;
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
jr2_a0_dnx_data_mdb_kaps_result_core0_db0_reg_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_result_core0_db0_reg;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = MDB_REG_2002r;

    
    define->data = MDB_REG_2002r;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_result_core0_db1_reg_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_result_core0_db1_reg;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = MDB_REG_2016r;

    
    define->data = MDB_REG_2016r;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_result_core1_db0_reg_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_result_core1_db0_reg;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = MDB_REG_2005r;

    
    define->data = MDB_REG_2005r;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mdb_kaps_result_core1_db1_reg_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int define_index = dnx_data_mdb_kaps_define_result_core1_db1_reg;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = MDB_REG_2019r;

    
    define->data = MDB_REG_2019r;

    
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
jr2_a0_dnx_data_mdb_kaps_big_bb_blk_id_mapping_set(
    int unit)
{
    int blk_id_index;
    dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_kaps;
    int table_index = dnx_data_mdb_kaps_table_big_bb_blk_id_mapping;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_DATA_MAX_MDB_KAPS_NOF_BIG_BBS_BLK_IDS;
    table->info_get.key_size[0] = DNX_DATA_MAX_MDB_KAPS_NOF_BIG_BBS_BLK_IDS;

    
    table->values[0].default_val = "DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_kaps_big_bb_blk_id_mapping_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_kaps_table_big_bb_blk_id_mapping");

    
    default_data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->global_macro_index = DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES;
    
    for (blk_id_index = 0; blk_id_index < table->keys[0].size; blk_id_index++)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, blk_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->global_macro_index = 1;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->global_macro_index = 2;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->global_macro_index = 3;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->global_macro_index = 4;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->global_macro_index = 7;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->global_macro_index = 8;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->global_macro_index = 9;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->global_macro_index = 11;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->global_macro_index = 12;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->global_macro_index = 13;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->global_macro_index = 14;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->global_macro_index = 15;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->global_macro_index = 17;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->global_macro_index = 19;
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

    

    
    data_index = dnx_data_mdb_global_adapter_use_stub;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_global_adapter_use_stub_set;
    data_index = dnx_data_mdb_global_hitbit_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_global_hitbit_support_set;
    data_index = dnx_data_mdb_global_capacity_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_global_capacity_support_set;

    
    
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
    data_index = dnx_data_mdb_dh_table_macro_interface_mapping;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_dh_macro_interface_mapping_set;
    data_index = dnx_data_mdb_dh_table_table_way_to_macro_mapping;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_dh_table_way_to_macro_mapping_set;
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
    
    submodule_index = dnx_data_mdb_submodule_pdbs;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_mdb_pdbs_define_max_nof_interface_dhs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_pdbs_max_nof_interface_dhs_set;
    data_index = dnx_data_mdb_pdbs_define_kaps_nof_blocks;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_pdbs_kaps_nof_blocks_set;
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

    

    
    data_index = dnx_data_mdb_pdbs_table_mdb_profile;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_pdbs_mdb_profile_set;
    data_index = dnx_data_mdb_pdbs_table_mdb_profiles_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_pdbs_mdb_profiles_info_set;
    data_index = dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_set;
    data_index = dnx_data_mdb_pdbs_table_pdbs_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_pdbs_pdbs_info_set;
    data_index = dnx_data_mdb_pdbs_table_pdb_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_pdbs_pdb_info_set;
    data_index = dnx_data_mdb_pdbs_table_mdb_11_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_pdbs_mdb_11_info_set;
    data_index = dnx_data_mdb_pdbs_table_mdb_12_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_pdbs_mdb_12_info_set;
    data_index = dnx_data_mdb_pdbs_table_mdb_26_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_pdbs_mdb_26_info_set;
    data_index = dnx_data_mdb_pdbs_table_mdb_adapter_mapping;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_pdbs_mdb_adapter_mapping_set;
    
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
    data_index = dnx_data_mdb_em_define_flush_max_supported_payload;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_flush_max_supported_payload_set;
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
    data_index = dnx_data_mdb_em_define_interrupt_register;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_em_interrupt_register_set;

    
    data_index = dnx_data_mdb_em_entry_type_parser;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_em_entry_type_parser_set;

    
    data_index = dnx_data_mdb_em_table_spn;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_spn_set;
    data_index = dnx_data_mdb_em_table_lfsr;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_lfsr_set;
    data_index = dnx_data_mdb_em_table_em_aging_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_em_aging_info_set;
    data_index = dnx_data_mdb_em_table_em_aging_cfg;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_em_aging_cfg_set;
    data_index = dnx_data_mdb_em_table_em_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_em_info_set;
    data_index = dnx_data_mdb_em_table_step_table_pdb_max_depth;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_step_table_pdb_max_depth_set;
    data_index = dnx_data_mdb_em_table_step_table_max_depth_possible;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_step_table_max_depth_possible_set;
    data_index = dnx_data_mdb_em_table_mdb_13_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_mdb_13_info_set;
    data_index = dnx_data_mdb_em_table_mdb_15_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_mdb_15_info_set;
    data_index = dnx_data_mdb_em_table_mdb_16_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_mdb_16_info_set;
    data_index = dnx_data_mdb_em_table_mdb_18_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_mdb_18_info_set;
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
    data_index = dnx_data_mdb_em_table_mdb_em_tables_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_mdb_em_tables_info_set;
    data_index = dnx_data_mdb_em_table_mdb_emp_tables_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_mdb_emp_tables_info_set;
    data_index = dnx_data_mdb_em_table_mdb_em_shift_vmv_regs;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_em_mdb_em_shift_vmv_regs_set;
    
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
    data_index = dnx_data_mdb_direct_define_map_payload_size_enum;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_direct_map_payload_size_enum_set;
    data_index = dnx_data_mdb_direct_define_map_payload_type_enum;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_direct_map_payload_type_enum_set;
    data_index = dnx_data_mdb_direct_define_map_payload_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_direct_map_payload_size_set;

    

    
    data_index = dnx_data_mdb_direct_table_direct_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_direct_direct_info_set;
    data_index = dnx_data_mdb_direct_table_hit_bit_pos_in_abk;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_direct_hit_bit_pos_in_abk_set;
    data_index = dnx_data_mdb_direct_table_mdb_42_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_direct_mdb_42_info_set;
    
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
    data_index = dnx_data_mdb_eedb_table_outlif_physical_phase_granularity;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_eedb_outlif_physical_phase_granularity_set;
    
    submodule_index = dnx_data_mdb_submodule_kaps;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_mdb_kaps_define_nof_small_bbs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_nof_small_bbs_set;
    data_index = dnx_data_mdb_kaps_define_nof_big_bbs_blk_ids;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_nof_big_bbs_blk_ids_set;
    data_index = dnx_data_mdb_kaps_define_nof_rows_in_rpb;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_nof_rows_in_rpb_set;
    data_index = dnx_data_mdb_kaps_define_nof_rows_in_small_ads;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_nof_rows_in_small_ads_set;
    data_index = dnx_data_mdb_kaps_define_nof_rows_in_small_bb;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_nof_rows_in_small_bb_set;
    data_index = dnx_data_mdb_kaps_define_nof_rows_in_small_rpb_hitbits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_nof_rows_in_small_rpb_hitbits_set;
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
    data_index = dnx_data_mdb_kaps_define_ad_width_in_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_ad_width_in_bits_set;
    data_index = dnx_data_mdb_kaps_define_bb_byte_width;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_bb_byte_width_set;
    data_index = dnx_data_mdb_kaps_define_rpb_byte_width;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_rpb_byte_width_set;
    data_index = dnx_data_mdb_kaps_define_ads_byte_width;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_ads_byte_width_set;
    data_index = dnx_data_mdb_kaps_define_rpb_valid_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_rpb_valid_bits_set;
    data_index = dnx_data_mdb_kaps_define_dynamic_memory_access_dpc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_dynamic_memory_access_dpc_set;
    data_index = dnx_data_mdb_kaps_define_rpb_field;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_rpb_field_set;
    data_index = dnx_data_mdb_kaps_define_result_core0_db0_reg;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_result_core0_db0_reg_set;
    data_index = dnx_data_mdb_kaps_define_result_core0_db1_reg;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_result_core0_db1_reg_set;
    data_index = dnx_data_mdb_kaps_define_result_core1_db0_reg;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_result_core1_db0_reg_set;
    data_index = dnx_data_mdb_kaps_define_result_core1_db1_reg;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_result_core1_db1_reg_set;
    data_index = dnx_data_mdb_kaps_define_big_kaps_revision_val;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mdb_kaps_big_kaps_revision_val_set;

    
    data_index = dnx_data_mdb_kaps_flex_bb_row;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mdb_kaps_flex_bb_row_set;

    
    data_index = dnx_data_mdb_kaps_table_big_bb_blk_id_mapping;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mdb_kaps_big_bb_blk_id_mapping_set;
    
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

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

