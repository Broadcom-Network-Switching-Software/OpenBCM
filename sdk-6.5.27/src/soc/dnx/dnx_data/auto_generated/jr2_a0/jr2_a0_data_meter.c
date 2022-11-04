
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_POLICER

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_meter.h>
#include <bcm/policer.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <shared/utilex/utilex_integer_arithmetic.h>







static shr_error_e
jr2_a0_dnx_data_meter_general_is_used_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_general;
    int feature_index = dnx_data_meter_general_is_used;
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
jr2_a0_dnx_data_meter_general_egr_rslv_color_counter_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_general;
    int feature_index = dnx_data_meter_general_egr_rslv_color_counter_support;
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
jr2_a0_dnx_data_meter_profile_large_bucket_mode_exp_add_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_profile;
    int define_index = dnx_data_meter_profile_define_large_bucket_mode_exp_add;
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
jr2_a0_dnx_data_meter_profile_max_regular_bucket_mode_burst_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_profile;
    int define_index = dnx_data_meter_profile_define_max_regular_bucket_mode_burst;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4161536;

    
    define->data = 4161536;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_meter_profile_rev_exp_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_profile;
    int define_index = dnx_data_meter_profile_define_rev_exp_min;
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
jr2_a0_dnx_data_meter_profile_rev_exp_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_profile;
    int define_index = dnx_data_meter_profile_define_rev_exp_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 31;

    
    define->data = 31;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_meter_profile_burst_size_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_profile;
    int define_index = dnx_data_meter_profile_define_burst_size_min;
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
jr2_a0_dnx_data_meter_profile_nof_valid_global_meters_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_profile;
    int define_index = dnx_data_meter_profile_define_nof_valid_global_meters;
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
jr2_a0_dnx_data_meter_profile_nof_dp_command_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_profile;
    int define_index = dnx_data_meter_profile_define_nof_dp_command_max;
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
jr2_a0_dnx_data_meter_profile_global_engine_hw_stat_database_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_profile;
    int define_index = dnx_data_meter_profile_define_global_engine_hw_stat_database_id;
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
jr2_a0_dnx_data_meter_profile_ir_mant_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_profile;
    int define_index = dnx_data_meter_profile_define_ir_mant_nof_bits;
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
jr2_a0_dnx_data_meter_diag_rate_measurements_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_diag;
    int feature_index = dnx_data_meter_diag_rate_measurements_support;
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
jr2_a0_dnx_data_meter_meter_db_refresh_disable_required_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_meter_db;
    int feature_index = dnx_data_meter_meter_db_refresh_disable_required;
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
jr2_a0_dnx_data_meter_meter_db_tcsm_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_meter_db;
    int feature_index = dnx_data_meter_meter_db_tcsm_support;
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
jr2_a0_dnx_data_meter_meter_db_fixed_cmd_id_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_meter_db;
    int feature_index = dnx_data_meter_meter_db_fixed_cmd_id;
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
jr2_a0_dnx_data_meter_meter_db_nof_ingress_db_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_meter_db;
    int define_index = dnx_data_meter_meter_db_define_nof_ingress_db;
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
jr2_a0_dnx_data_meter_meter_db_nof_egress_db_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_meter_db;
    int define_index = dnx_data_meter_meter_db_define_nof_egress_db;
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
jr2_a0_dnx_data_meter_meter_db_nof_meters_id_in_group_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_meter_db;
    int define_index = dnx_data_meter_meter_db_define_nof_meters_id_in_group;
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
jr2_a0_dnx_data_meter_mem_mgmt_is_ptr_map_to_bank_id_contains_bank_offset_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int feature_index = dnx_data_meter_mem_mgmt_is_ptr_map_to_bank_id_contains_bank_offset;
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
jr2_a0_dnx_data_meter_mem_mgmt_is_small_engine_support_dual_bucket_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int feature_index = dnx_data_meter_mem_mgmt_is_small_engine_support_dual_bucket;
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
jr2_a0_dnx_data_meter_mem_mgmt_ptr_map_table_has_invalid_mapping_option_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int feature_index = dnx_data_meter_mem_mgmt_ptr_map_table_has_invalid_mapping_option;
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
jr2_a0_dnx_data_meter_mem_mgmt_fixed_nof_profile_in_entry_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int feature_index = dnx_data_meter_mem_mgmt_fixed_nof_profile_in_entry;
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
jr2_a0_dnx_data_meter_mem_mgmt_adrress_range_resolution_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_adrress_range_resolution;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8192;

    
    define->data = 8192;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_meter_mem_mgmt_object_stat_pointer_base_resolution_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_object_stat_pointer_base_resolution;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16384;

    
    define->data = 16384;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_meter_mem_mgmt_base_big_engine_for_meter_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_base_big_engine_for_meter;
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
jr2_a0_dnx_data_meter_mem_mgmt_ingress_single_bucket_engine_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_ingress_single_bucket_engine;
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
jr2_a0_dnx_data_meter_mem_mgmt_egress_single_bucket_engine_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_egress_single_bucket_engine;
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
jr2_a0_dnx_data_meter_mem_mgmt_meter_pointer_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_meter_pointer_size;
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
jr2_a0_dnx_data_meter_mem_mgmt_meter_pointer_map_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_meter_pointer_map_size;
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
jr2_a0_dnx_data_meter_mem_mgmt_invalid_bank_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_invalid_bank_id;
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
jr2_a0_dnx_data_meter_mem_mgmt_ptr_map_table_resolution_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_ptr_map_table_resolution;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4096;

    
    define->data = 4096;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_meter_mem_mgmt_bank_id_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_bank_id_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = utilex_log2_round_up(dnx_data_crps.engine.nof_big_engines_get(unit)*2);

    
    define->data = utilex_log2_round_up(dnx_data_crps.engine.nof_big_engines_get(unit)*2);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_meter_mem_mgmt_nof_bank_ids_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_nof_bank_ids;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_crps.engine.nof_big_engines_get(unit)*2;

    
    define->data = dnx_data_crps.engine.nof_big_engines_get(unit)*2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_meter_mem_mgmt_sections_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_sections_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_crps.engine.nof_big_engines_get(unit);

    
    define->data = dnx_data_crps.engine.nof_big_engines_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_meter_mem_mgmt_nof_banks_in_big_engine_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_nof_banks_in_big_engine;
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
jr2_a0_dnx_data_meter_mem_mgmt_nof_single_buckets_per_entry_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_nof_single_buckets_per_entry;
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
jr2_a0_dnx_data_meter_mem_mgmt_nof_dual_buckets_per_entry_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_nof_dual_buckets_per_entry;
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
jr2_a0_dnx_data_meter_mem_mgmt_nof_profiles_per_entry_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_nof_profiles_per_entry;
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
jr2_a0_dnx_data_meter_mem_mgmt_nof_buckets_in_small_bank_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_nof_buckets_in_small_bank;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_crps.engine.engines_info_get(unit, dnx_data_meter.mem_mgmt.ingress_single_bucket_engine_get(unit))->nof_entries)*dnx_data_meter.mem_mgmt.nof_single_buckets_per_entry_get(unit);

    
    define->data = (dnx_data_crps.engine.engines_info_get(unit, dnx_data_meter.mem_mgmt.ingress_single_bucket_engine_get(unit))->nof_entries)*dnx_data_meter.mem_mgmt.nof_single_buckets_per_entry_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_meter_mem_mgmt_nof_buckets_in_big_bank_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_nof_buckets_in_big_bank;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_crps.engine.engines_info_get(unit, dnx_data_meter.mem_mgmt.base_big_engine_for_meter_get(unit))->nof_entries)*dnx_data_meter.mem_mgmt.nof_single_buckets_per_entry_get(unit);

    
    define->data = (dnx_data_crps.engine.engines_info_get(unit, dnx_data_meter.mem_mgmt.base_big_engine_for_meter_get(unit))->nof_entries)*dnx_data_meter.mem_mgmt.nof_single_buckets_per_entry_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_meter_mem_mgmt_bucket_size_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_bucket_size_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 23;

    
    define->data = 23;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_meter_compensation_nof_delta_compensation_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_compensation;
    int define_index = dnx_data_meter_compensation_define_nof_delta_compensation_profiles;
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
jr2_a0_dnx_data_meter_compensation_per_ingress_pp_port_delta_range_set(
    int unit)
{
    dnx_data_meter_compensation_per_ingress_pp_port_delta_range_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_compensation;
    int table_index = dnx_data_meter_compensation_table_per_ingress_pp_port_delta_range;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "-128";
    table->values[1].default_val = "127";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_meter_compensation_per_ingress_pp_port_delta_range_t, (1 + 1  ), "data of dnx_data_meter_compensation_table_per_ingress_pp_port_delta_range");

    
    default_data = (dnx_data_meter_compensation_per_ingress_pp_port_delta_range_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->min = -128;
    default_data->max = 127;
    
    data = (dnx_data_meter_compensation_per_ingress_pp_port_delta_range_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_meter_compensation_per_meter_delta_range_set(
    int unit)
{
    dnx_data_meter_compensation_per_meter_delta_range_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_compensation;
    int table_index = dnx_data_meter_compensation_table_per_meter_delta_range;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_meter_compensation_per_meter_delta_range_t, (1 + 1  ), "data of dnx_data_meter_compensation_table_per_meter_delta_range");

    
    default_data = (dnx_data_meter_compensation_per_meter_delta_range_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->min = 0;
    default_data->max = 0;
    
    data = (dnx_data_meter_compensation_per_meter_delta_range_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_meter_expansion_expansion_based_on_tc_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_expansion;
    int feature_index = dnx_data_meter_expansion_expansion_based_on_tc_support;
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
jr2_a0_dnx_data_meter_expansion_expansion_for_uuc_umc_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_expansion;
    int feature_index = dnx_data_meter_expansion_expansion_for_uuc_umc_support;
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
jr2_a0_dnx_data_meter_expansion_max_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_expansion;
    int define_index = dnx_data_meter_expansion_define_max_size;
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
jr2_a0_dnx_data_meter_expansion_nof_expansion_sets_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_expansion;
    int define_index = dnx_data_meter_expansion_define_nof_expansion_sets;
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
jr2_a0_dnx_data_meter_expansion_max_meter_set_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_expansion;
    int define_index = dnx_data_meter_expansion_define_max_meter_set_size;
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
jr2_a0_dnx_data_meter_expansion_max_meter_set_base_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_expansion;
    int define_index = dnx_data_meter_expansion_define_max_meter_set_base;
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
jr2_a0_dnx_data_meter_expansion_ingress_metadata_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_expansion;
    int define_index = dnx_data_meter_expansion_define_ingress_metadata_size;
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
jr2_a0_dnx_data_meter_expansion_egress_metadata_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_expansion;
    int define_index = dnx_data_meter_expansion_define_egress_metadata_size;
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
jr2_a0_dnx_data_meter_expansion_expansion_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_expansion;
    int define_index = dnx_data_meter_expansion_define_expansion_size;
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
jr2_a0_dnx_data_meter_expansion_expansion_offset_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_expansion;
    int define_index = dnx_data_meter_expansion_define_expansion_offset_size;
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
jr2_a0_dnx_data_meter_expansion_source_metadata_types_set(
    int unit)
{
    int metadata_type_index;
    dnx_data_meter_expansion_source_metadata_types_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_expansion;
    int table_index = dnx_data_meter_expansion_table_source_metadata_types;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_meter.expansion.ingress_metadata_size_get(unit);
    table->info_get.key_size[0] = dnx_data_meter.expansion.ingress_metadata_size_get(unit);

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_meter_expansion_source_metadata_types_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_meter_expansion_table_source_metadata_types");

    
    default_data = (dnx_data_meter_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->offset = -1;
    default_data->nof_bits = -1;
    
    for (metadata_type_index = 0; metadata_type_index < table->keys[0].size; metadata_type_index++)
    {
        data = (dnx_data_meter_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, metadata_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
jr2_a0_data_meter_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_meter;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_meter_submodule_general;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_meter_general_is_used;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_meter_general_is_used_set;
    data_index = dnx_data_meter_general_egr_rslv_color_counter_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_meter_general_egr_rslv_color_counter_support_set;

    
    
    submodule_index = dnx_data_meter_submodule_profile;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_meter_profile_define_large_bucket_mode_exp_add;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_profile_large_bucket_mode_exp_add_set;
    data_index = dnx_data_meter_profile_define_max_regular_bucket_mode_burst;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_profile_max_regular_bucket_mode_burst_set;
    data_index = dnx_data_meter_profile_define_rev_exp_min;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_profile_rev_exp_min_set;
    data_index = dnx_data_meter_profile_define_rev_exp_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_profile_rev_exp_max_set;
    data_index = dnx_data_meter_profile_define_burst_size_min;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_profile_burst_size_min_set;
    data_index = dnx_data_meter_profile_define_nof_valid_global_meters;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_profile_nof_valid_global_meters_set;
    data_index = dnx_data_meter_profile_define_nof_dp_command_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_profile_nof_dp_command_max_set;
    data_index = dnx_data_meter_profile_define_global_engine_hw_stat_database_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_profile_global_engine_hw_stat_database_id_set;
    data_index = dnx_data_meter_profile_define_ir_mant_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_profile_ir_mant_nof_bits_set;

    

    
    
    submodule_index = dnx_data_meter_submodule_diag;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_meter_diag_rate_measurements_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_meter_diag_rate_measurements_support_set;

    
    
    submodule_index = dnx_data_meter_submodule_meter_db;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_meter_meter_db_define_nof_ingress_db;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_meter_db_nof_ingress_db_set;
    data_index = dnx_data_meter_meter_db_define_nof_egress_db;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_meter_db_nof_egress_db_set;
    data_index = dnx_data_meter_meter_db_define_nof_meters_id_in_group;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_meter_db_nof_meters_id_in_group_set;

    
    data_index = dnx_data_meter_meter_db_refresh_disable_required;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_meter_meter_db_refresh_disable_required_set;
    data_index = dnx_data_meter_meter_db_tcsm_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_meter_meter_db_tcsm_support_set;
    data_index = dnx_data_meter_meter_db_fixed_cmd_id;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_meter_meter_db_fixed_cmd_id_set;

    
    
    submodule_index = dnx_data_meter_submodule_mem_mgmt;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_meter_mem_mgmt_define_adrress_range_resolution;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_mem_mgmt_adrress_range_resolution_set;
    data_index = dnx_data_meter_mem_mgmt_define_object_stat_pointer_base_resolution;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_mem_mgmt_object_stat_pointer_base_resolution_set;
    data_index = dnx_data_meter_mem_mgmt_define_base_big_engine_for_meter;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_mem_mgmt_base_big_engine_for_meter_set;
    data_index = dnx_data_meter_mem_mgmt_define_ingress_single_bucket_engine;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_mem_mgmt_ingress_single_bucket_engine_set;
    data_index = dnx_data_meter_mem_mgmt_define_egress_single_bucket_engine;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_mem_mgmt_egress_single_bucket_engine_set;
    data_index = dnx_data_meter_mem_mgmt_define_meter_pointer_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_mem_mgmt_meter_pointer_size_set;
    data_index = dnx_data_meter_mem_mgmt_define_meter_pointer_map_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_mem_mgmt_meter_pointer_map_size_set;
    data_index = dnx_data_meter_mem_mgmt_define_invalid_bank_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_mem_mgmt_invalid_bank_id_set;
    data_index = dnx_data_meter_mem_mgmt_define_ptr_map_table_resolution;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_mem_mgmt_ptr_map_table_resolution_set;
    data_index = dnx_data_meter_mem_mgmt_define_bank_id_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_mem_mgmt_bank_id_size_set;
    data_index = dnx_data_meter_mem_mgmt_define_nof_bank_ids;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_mem_mgmt_nof_bank_ids_set;
    data_index = dnx_data_meter_mem_mgmt_define_sections_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_mem_mgmt_sections_offset_set;
    data_index = dnx_data_meter_mem_mgmt_define_nof_banks_in_big_engine;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_mem_mgmt_nof_banks_in_big_engine_set;
    data_index = dnx_data_meter_mem_mgmt_define_nof_single_buckets_per_entry;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_mem_mgmt_nof_single_buckets_per_entry_set;
    data_index = dnx_data_meter_mem_mgmt_define_nof_dual_buckets_per_entry;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_mem_mgmt_nof_dual_buckets_per_entry_set;
    data_index = dnx_data_meter_mem_mgmt_define_nof_profiles_per_entry;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_mem_mgmt_nof_profiles_per_entry_set;
    data_index = dnx_data_meter_mem_mgmt_define_nof_buckets_in_small_bank;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_mem_mgmt_nof_buckets_in_small_bank_set;
    data_index = dnx_data_meter_mem_mgmt_define_nof_buckets_in_big_bank;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_mem_mgmt_nof_buckets_in_big_bank_set;
    data_index = dnx_data_meter_mem_mgmt_define_bucket_size_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_mem_mgmt_bucket_size_nof_bits_set;

    
    data_index = dnx_data_meter_mem_mgmt_is_ptr_map_to_bank_id_contains_bank_offset;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_meter_mem_mgmt_is_ptr_map_to_bank_id_contains_bank_offset_set;
    data_index = dnx_data_meter_mem_mgmt_is_small_engine_support_dual_bucket;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_meter_mem_mgmt_is_small_engine_support_dual_bucket_set;
    data_index = dnx_data_meter_mem_mgmt_ptr_map_table_has_invalid_mapping_option;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_meter_mem_mgmt_ptr_map_table_has_invalid_mapping_option_set;
    data_index = dnx_data_meter_mem_mgmt_fixed_nof_profile_in_entry;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_meter_mem_mgmt_fixed_nof_profile_in_entry_set;

    
    
    submodule_index = dnx_data_meter_submodule_compensation;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_meter_compensation_define_nof_delta_compensation_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_compensation_nof_delta_compensation_profiles_set;

    

    
    data_index = dnx_data_meter_compensation_table_per_ingress_pp_port_delta_range;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_meter_compensation_per_ingress_pp_port_delta_range_set;
    data_index = dnx_data_meter_compensation_table_per_meter_delta_range;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_meter_compensation_per_meter_delta_range_set;
    
    submodule_index = dnx_data_meter_submodule_expansion;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_meter_expansion_define_max_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_expansion_max_size_set;
    data_index = dnx_data_meter_expansion_define_nof_expansion_sets;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_expansion_nof_expansion_sets_set;
    data_index = dnx_data_meter_expansion_define_max_meter_set_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_expansion_max_meter_set_size_set;
    data_index = dnx_data_meter_expansion_define_max_meter_set_base;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_expansion_max_meter_set_base_set;
    data_index = dnx_data_meter_expansion_define_ingress_metadata_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_expansion_ingress_metadata_size_set;
    data_index = dnx_data_meter_expansion_define_egress_metadata_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_expansion_egress_metadata_size_set;
    data_index = dnx_data_meter_expansion_define_expansion_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_expansion_expansion_size_set;
    data_index = dnx_data_meter_expansion_define_expansion_offset_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_expansion_expansion_offset_size_set;

    
    data_index = dnx_data_meter_expansion_expansion_based_on_tc_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_meter_expansion_expansion_based_on_tc_support_set;
    data_index = dnx_data_meter_expansion_expansion_for_uuc_umc_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_meter_expansion_expansion_for_uuc_umc_support_set;

    
    data_index = dnx_data_meter_expansion_table_source_metadata_types;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_meter_expansion_source_metadata_types_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

