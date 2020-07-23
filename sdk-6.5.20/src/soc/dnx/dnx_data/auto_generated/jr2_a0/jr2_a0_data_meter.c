

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_POLICER

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_meter.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <bcm/policer.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <shared/utilex/utilex_integer_arithmetic.h>








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
jr2_a0_dnx_data_meter_meter_db_meter_ids_in_group_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_meter_db;
    int define_index = dnx_data_meter_meter_db_define_meter_ids_in_group;
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
jr2_a0_dnx_data_meter_mem_mgmt_counters_buckets_ratio_per_engine_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_counters_buckets_ratio_per_engine;
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
jr2_a0_dnx_data_meter_compensation_ingress_port_delta_value_set(
    int unit)
{
    dnx_data_meter_compensation_ingress_port_delta_value_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_compensation;
    int table_index = dnx_data_meter_compensation_table_ingress_port_delta_value;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "-128";
    table->values[1].default_val = "127";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_meter_compensation_ingress_port_delta_value_t, (1 + 1  ), "data of dnx_data_meter_compensation_table_ingress_port_delta_value");

    
    default_data = (dnx_data_meter_compensation_ingress_port_delta_value_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->min = -128;
    default_data->max = 127;
    
    data = (dnx_data_meter_compensation_ingress_port_delta_value_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
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
    data_index = dnx_data_meter_meter_db_define_meter_ids_in_group;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_meter_db_meter_ids_in_group_set;

    
    data_index = dnx_data_meter_meter_db_refresh_disable_required;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_meter_meter_db_refresh_disable_required_set;
    data_index = dnx_data_meter_meter_db_tcsm_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_meter_meter_db_tcsm_support_set;

    
    
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
    data_index = dnx_data_meter_mem_mgmt_define_counters_buckets_ratio_per_engine;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_mem_mgmt_counters_buckets_ratio_per_engine_set;
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

    

    
    
    submodule_index = dnx_data_meter_submodule_compensation;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_meter_compensation_table_ingress_port_delta_value;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_meter_compensation_ingress_port_delta_value_set;
    
    submodule_index = dnx_data_meter_submodule_expansion;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_meter_expansion_define_max_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_meter_expansion_max_size_set;

    
    data_index = dnx_data_meter_expansion_expansion_based_on_tc_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_meter_expansion_expansion_based_on_tc_support_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

