
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_crps.h>
#include <bcm/stat.h>







static shr_error_e
jr2_a0_dnx_data_crps_engine_valid_address_admission_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int feature_index = dnx_data_crps_engine_valid_address_admission_is_supported;
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
jr2_a0_dnx_data_crps_engine_last_input_command_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int feature_index = dnx_data_crps_engine_last_input_command_is_supported;
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
jr2_a0_dnx_data_crps_engine_rate_calculation_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int feature_index = dnx_data_crps_engine_rate_calculation_is_supported;
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
jr2_a0_dnx_data_crps_engine_multi_sets_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int feature_index = dnx_data_crps_engine_multi_sets_is_supported;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = (dnx_data_crps.engine.max_nof_counter_sets_get(unit) > 1) ? 1 : 0;

    
    feature->data = (dnx_data_crps.engine.max_nof_counter_sets_get(unit) > 1) ? 1 : 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_crps_engine_dynamic_counter_mode_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int feature_index = dnx_data_crps_engine_dynamic_counter_mode_is_supported;
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
jr2_a0_dnx_data_crps_engine_nof_engines_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int define_index = dnx_data_crps_engine_define_nof_engines;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 22;

    
    define->data = 22;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_crps_engine_nof_mid_engines_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int define_index = dnx_data_crps_engine_define_nof_mid_engines;
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
jr2_a0_dnx_data_crps_engine_nof_big_engines_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int define_index = dnx_data_crps_engine_define_nof_big_engines;
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
jr2_a0_dnx_data_crps_engine_nof_databases_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int define_index = dnx_data_crps_engine_define_nof_databases;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 22;

    
    define->data = 22;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_crps_engine_data_mapping_table_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int define_index = dnx_data_crps_engine_define_data_mapping_table_size;
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
jr2_a0_dnx_data_crps_engine_max_counter_set_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int define_index = dnx_data_crps_engine_define_max_counter_set_size;
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
jr2_a0_dnx_data_crps_engine_oam_counter_set_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int define_index = dnx_data_crps_engine_define_oam_counter_set_size;
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
jr2_a0_dnx_data_crps_engine_max_nof_counter_sets_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int define_index = dnx_data_crps_engine_define_max_nof_counter_sets;
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
jr2_a0_dnx_data_crps_engine_last_address_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int define_index = dnx_data_crps_engine_define_last_address_nof_bits;
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
jr2_a0_dnx_data_crps_engine_counter_format_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int define_index = dnx_data_crps_engine_define_counter_format_nof_bits;
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
jr2_a0_dnx_data_crps_engine_counter_mode_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int define_index = dnx_data_crps_engine_define_counter_mode_nof_bits;
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
jr2_a0_dnx_data_crps_engine_nof_small_meter_shared_engines_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int define_index = dnx_data_crps_engine_define_nof_small_meter_shared_engines;
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
jr2_a0_dnx_data_crps_engine_nof_mid_meter_shared_engines_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int define_index = dnx_data_crps_engine_define_nof_mid_meter_shared_engines;
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
jr2_a0_dnx_data_crps_engine_nof_big_meter_shared_engines_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int define_index = dnx_data_crps_engine_define_nof_big_meter_shared_engines;
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
jr2_a0_dnx_data_crps_engine_engines_info_set(
    int unit)
{
    int engine_id_index;
    dnx_data_crps_engine_engines_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int table_index = dnx_data_crps_engine_table_engines_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 22;
    table->info_get.key_size[0] = 22;

    
    table->values[0].default_val = "2048";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_engine_engines_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_crps_engine_table_engines_info");

    
    default_data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nof_entries = 2048;
    default_data->meter_shared = 0;
    
    for (engine_id_index = 0; engine_id_index < table->keys[0].size; engine_id_index++)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, engine_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->nof_entries = 2048;
        data->meter_shared = 1;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->nof_entries = 2048;
        data->meter_shared = 1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->nof_entries = 2048;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->nof_entries = 2048;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->nof_entries = 2048;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->nof_entries = 2048;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->nof_entries = 2048;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->nof_entries = 2048;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->nof_entries = 4096;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->nof_entries = 4096;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->nof_entries = 4096;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->nof_entries = 4096;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->nof_entries = 4096;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->nof_entries = 4096;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->nof_entries = 4096;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->nof_entries = 4096;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->nof_entries = 8192;
        data->meter_shared = 1;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->nof_entries = 8192;
        data->meter_shared = 1;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->nof_entries = 8192;
        data->meter_shared = 1;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->nof_entries = 8192;
        data->meter_shared = 1;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->nof_entries = 8192;
        data->meter_shared = 1;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->nof_entries = 8192;
        data->meter_shared = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_crps_engine_source_base_set(
    int unit)
{
    int source_index;
    dnx_data_crps_engine_source_base_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int table_index = dnx_data_crps_engine_table_source_base;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = bcmStatCounterInterfaceCount;
    table->info_get.key_size[0] = bcmStatCounterInterfaceCount;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_engine_source_base_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_crps_engine_table_source_base");

    
    default_data = (dnx_data_crps_engine_source_base_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->base_val = -1;
    
    for (source_index = 0; source_index < table->keys[0].size; source_index++)
    {
        data = (dnx_data_crps_engine_source_base_t *) dnxc_data_mgmt_table_data_get(unit, table, source_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_source_base_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, 0);
        data->base_val = 2;
    }
    if (bcmStatCounterInterfaceIngressOam < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_source_base_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressOam, 0);
        data->base_val = 12;
    }
    if (bcmStatCounterInterfaceIngressVoq < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_source_base_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressVoq, 0);
        data->base_val = 0;
    }
    if (bcmStatCounterInterfaceIngressTransmitPp < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_source_base_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressTransmitPp, 0);
        data->base_val = 15;
    }
    if (bcmStatCounterInterfaceEgressReceivePp < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_source_base_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceivePp, 0);
        data->base_val = 17;
    }
    if (bcmStatCounterInterfaceEgressReceiveQueue < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_source_base_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceiveQueue, 0);
        data->base_val = 16;
    }
    if (bcmStatCounterInterfaceEgressTransmitPp < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_source_base_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressTransmitPp, 0);
        data->base_val = 19;
    }
    if (bcmStatCounterInterfaceEgressOam < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_source_base_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressOam, 0);
        data->base_val = 25;
    }
    if (bcmStatCounterInterfaceOamp < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_source_base_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceOamp, 0);
        data->base_val = 28;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_crps_engine_counter_format_types_set(
    int unit)
{
    int format_type_index;
    dnx_data_crps_engine_counter_format_types_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int table_index = dnx_data_crps_engine_table_counter_format_types;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = bcmStatCounterFormatCount;
    table->info_get.key_size[0] = bcmStatCounterFormatCount;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "bcmStatEvictionDestinationLocalHost, bcmStatEvictionDestinationSize";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_engine_counter_format_types_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_crps_engine_table_counter_format_types");

    
    default_data = (dnx_data_crps_engine_counter_format_types_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = 0;
    default_data->nof_bits_for_packets = 0;
    default_data->nof_bits_for_bytes = 0;
    default_data->nof_counters_per_entry = 0;
    SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_CRPS_EVICTION_NOF_EVICTION_DESTINATION_TYPES, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->valid_eviction_destinations[0] = bcmStatEvictionDestinationLocalHost;
    default_data->valid_eviction_destinations[1] = bcmStatEvictionDestinationSize;
    
    for (format_type_index = 0; format_type_index < table->keys[0].size; format_type_index++)
    {
        data = (dnx_data_crps_engine_counter_format_types_t *) dnxc_data_mgmt_table_data_get(unit, table, format_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (bcmStatCounterFormatPacketsAndBytes < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_counter_format_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterFormatPacketsAndBytes, 0);
        data->valid = 1;
        data->nof_bits_for_packets = 30;
        data->nof_bits_for_bytes = 38;
        data->nof_counters_per_entry = 2;
    }
    if (bcmStatCounterFormatPackets < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_counter_format_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterFormatPackets, 0);
        data->valid = 1;
        data->nof_bits_for_packets = 68;
        data->nof_counters_per_entry = 2;
    }
    if (bcmStatCounterFormatBytes < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_counter_format_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterFormatBytes, 0);
        data->valid = 1;
        data->nof_bits_for_bytes = 68;
        data->nof_counters_per_entry = 2;
    }
    if (bcmStatCounterFormatDoublePackets < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_counter_format_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterFormatDoublePackets, 0);
        data->valid = 1;
        data->nof_bits_for_packets = 34;
        data->nof_counters_per_entry = 4;
    }
    if (bcmStatCounterFormatMaxVoqSize < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_counter_format_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterFormatMaxVoqSize, 0);
        data->valid = 1;
        data->nof_counters_per_entry = 4;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_crps_eviction_algorithmic_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int feature_index = dnx_data_crps_eviction_algorithmic_is_supported;
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
jr2_a0_dnx_data_crps_eviction_probabilistic_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int feature_index = dnx_data_crps_eviction_probabilistic_is_supported;
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
jr2_a0_dnx_data_crps_eviction_low_rate_probabilistic_eviction_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int feature_index = dnx_data_crps_eviction_low_rate_probabilistic_eviction;
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
jr2_a0_dnx_data_crps_eviction_conditional_action_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int feature_index = dnx_data_crps_eviction_conditional_action_is_supported;
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
jr2_a0_dnx_data_crps_eviction_eviction_from_memory_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int feature_index = dnx_data_crps_eviction_eviction_from_memory_is_supported;
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
jr2_a0_dnx_data_crps_eviction_sequential_scan_speedup_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int feature_index = dnx_data_crps_eviction_sequential_scan_speedup_is_supported;
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
jr2_a0_dnx_data_crps_eviction_probabilistic_eviction_thresholds_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int feature_index = dnx_data_crps_eviction_probabilistic_eviction_thresholds_is_supported;
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
jr2_a0_dnx_data_crps_eviction_counters_fifo_depth_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int define_index = dnx_data_crps_eviction_define_counters_fifo_depth;
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
jr2_a0_dnx_data_crps_eviction_nof_counters_fifo_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int define_index = dnx_data_crps_eviction_define_nof_counters_fifo;
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
jr2_a0_dnx_data_crps_eviction_dma_record_entry_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int define_index = dnx_data_crps_eviction_define_dma_record_entry_nof_bits;
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
jr2_a0_dnx_data_crps_eviction_bubble_before_request_timeout_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int define_index = dnx_data_crps_eviction_define_bubble_before_request_timeout;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 500;

    
    define->data = 500;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_crps_eviction_bubble_after_request_timeout_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int define_index = dnx_data_crps_eviction_define_bubble_after_request_timeout;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 500;

    
    define->data = 500;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_crps_eviction_seq_address_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int define_index = dnx_data_crps_eviction_define_seq_address_nof_bits;
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
jr2_a0_dnx_data_crps_eviction_nof_eviction_destination_types_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int define_index = dnx_data_crps_eviction_define_nof_eviction_destination_types;
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
jr2_a0_dnx_data_crps_eviction_nof_crps_network_channels_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int define_index = dnx_data_crps_eviction_define_nof_crps_network_channels;
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
jr2_a0_dnx_data_crps_eviction_bg_thread_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int define_index = dnx_data_crps_eviction_define_bg_thread_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_COUNTER_ENGINE_SAMPLING_INTERVAL;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_enable;
    define->property.method_str = "enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_crps_eviction_phy_record_format_set(
    int unit)
{
    dnx_data_crps_eviction_phy_record_format_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int table_index = dnx_data_crps_eviction_table_phy_record_format;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "8";
    table->values[3].default_val = "104";
    table->values[4].default_val = "8";
    table->values[5].default_val = "96";
    table->values[6].default_val = "2";
    table->values[7].default_val = "94";
    table->values[8].default_val = "1";
    table->values[9].default_val = "93";
    table->values[10].default_val = "3";
    table->values[11].default_val = "90";
    table->values[12].default_val = "1";
    table->values[13].default_val = "89";
    table->values[14].default_val = "6";
    table->values[15].default_val = "83";
    table->values[16].default_val = "15";
    table->values[17].default_val = "68";
    table->values[18].default_val = "68";
    table->values[19].default_val = "0";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_eviction_phy_record_format_t, (1 + 1  ), "data of dnx_data_crps_eviction_table_phy_record_format");

    
    default_data = (dnx_data_crps_eviction_phy_record_format_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->is_probabilistic_size = -1;
    default_data->is_probabilistic_offset = -1;
    default_data->obj_type_size = 8;
    default_data->obj_type_offset = 104;
    default_data->reserved0_size = 8;
    default_data->reserved0_offset = 96;
    default_data->opcode_size = 2;
    default_data->opcode_offset = 94;
    default_data->err_ind_size = 1;
    default_data->err_ind_offset = 93;
    default_data->reserved1_size = 3;
    default_data->reserved1_offset = 90;
    default_data->core_id_size = 1;
    default_data->core_id_offset = 89;
    default_data->engine_id_size = 6;
    default_data->engine_id_offset = 83;
    default_data->local_ctr_id_size = 15;
    default_data->local_ctr_id_offset = 68;
    default_data->ctr_value_size = 68;
    default_data->ctr_value_offset = 0;
    
    data = (dnx_data_crps_eviction_phy_record_format_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_crps_eviction_condional_action_valid_set(
    int unit)
{
    int action_idx_index;
    dnx_data_crps_eviction_condional_action_valid_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int table_index = dnx_data_crps_eviction_table_condional_action_valid;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 5;
    table->info_get.key_size[0] = 5;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_eviction_condional_action_valid_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_crps_eviction_table_condional_action_valid");

    
    default_data = (dnx_data_crps_eviction_condional_action_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = 0;
    
    for (action_idx_index = 0; action_idx_index < table->keys[0].size; action_idx_index++)
    {
        data = (dnx_data_crps_eviction_condional_action_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, action_idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_crps_eviction_condional_action_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->valid = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_crps_eviction_condional_action_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->valid = 0;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_crps_eviction_condional_action_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->valid = 0;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_crps_eviction_condional_action_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->valid = 0;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_crps_eviction_condional_action_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->valid = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_crps_eviction_supported_eviction_destination_set(
    int unit)
{
    int destination_type_index;
    dnx_data_crps_eviction_supported_eviction_destination_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int table_index = dnx_data_crps_eviction_table_supported_eviction_destination;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = bcmStatEvictionDestinationSize;
    table->info_get.key_size[0] = bcmStatEvictionDestinationSize;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_eviction_supported_eviction_destination_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_crps_eviction_table_supported_eviction_destination");

    
    default_data = (dnx_data_crps_eviction_supported_eviction_destination_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = 0;
    
    for (destination_type_index = 0; destination_type_index < table->keys[0].size; destination_type_index++)
    {
        data = (dnx_data_crps_eviction_supported_eviction_destination_t *) dnxc_data_mgmt_table_data_get(unit, table, destination_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (bcmStatEvictionDestinationLocalHost < table->keys[0].size)
    {
        data = (dnx_data_crps_eviction_supported_eviction_destination_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatEvictionDestinationLocalHost, 0);
        data->valid = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_crps_expansion_expansion_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_expansion;
    int define_index = dnx_data_crps_expansion_define_expansion_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_crps_expansion_source_metadata_types_set(
    int unit)
{
    int source_index;
    int metadata_type_index;
    dnx_data_crps_expansion_source_metadata_types_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_expansion;
    int table_index = dnx_data_crps_expansion_table_source_metadata_types;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = bcmStatCounterInterfaceCount;
    table->info_get.key_size[0] = bcmStatCounterInterfaceCount;
    table->keys[1].size = bcmStatExpansionTypeMax;
    table->info_get.key_size[1] = bcmStatExpansionTypeMax;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_expansion_source_metadata_types_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_crps_expansion_table_source_metadata_types");

    
    default_data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->offset = -1;
    default_data->nof_bits = -1;
    
    for (source_index = 0; source_index < table->keys[0].size; source_index++)
    {
        for (metadata_type_index = 0; metadata_type_index < table->keys[1].size; metadata_type_index++)
        {
            data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, source_index, metadata_type_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypeDropPrecedenceMeter2Value < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypeDropPrecedenceMeter2Value);
        data->offset = 0;
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypeDropPrecedenceMeter2Valid < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypeDropPrecedenceMeter2Valid);
        data->offset = 2;
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypDropPrecedenceMeter1Value < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypDropPrecedenceMeter1Value);
        data->offset = 3;
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypeDropPrecedenceMeter1Valid < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypeDropPrecedenceMeter1Valid);
        data->offset = 5;
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypeDropPrecedenceMeter0Value < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypeDropPrecedenceMeter0Value);
        data->offset = 6;
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypeDropPrecedenceMeter0Valid < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypeDropPrecedenceMeter0Valid);
        data->offset = 8;
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypeTmDropReason < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypeTmDropReason);
        data->offset = 9;
        data->nof_bits = 3;
    }
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypeDispositionIsDrop < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypeDispositionIsDrop);
        data->offset = 13;
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypeDropPrecedenceMeterResolved < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypeDropPrecedenceMeterResolved);
        data->offset = 14;
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypeTrafficClass < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypeTrafficClass);
        data->offset = 16;
        data->nof_bits = 3;
    }
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypeMetaData < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypeMetaData);
        data->offset = 19;
        data->nof_bits = 16;
    }
    if (bcmStatCounterInterfaceIngressTransmitPp < table->keys[0].size && bcmStatExpansionTypeLatencyBin < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressTransmitPp, bcmStatExpansionTypeLatencyBin);
        data->offset = 0;
        data->nof_bits = 3;
    }
    if (bcmStatCounterInterfaceIngressTransmitPp < table->keys[0].size && bcmStatExpansionTypeTrafficClass < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressTransmitPp, bcmStatExpansionTypeTrafficClass);
        data->offset = 3;
        data->nof_bits = 3;
    }
    if (bcmStatCounterInterfaceIngressTransmitPp < table->keys[0].size && bcmStatExpansionTypeLatencyFlowProfile < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressTransmitPp, bcmStatExpansionTypeLatencyFlowProfile);
        data->offset = 6;
        data->nof_bits = 5;
    }
    if (bcmStatCounterInterfaceIngressVoq < table->keys[0].size && bcmStatExpansionTypeTmDropReason < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressVoq, bcmStatExpansionTypeTmDropReason);
        data->offset = 0;
        data->nof_bits = 3;
    }
    if (bcmStatCounterInterfaceIngressVoq < table->keys[0].size && bcmStatExpansionTypeDispositionIsDrop < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressVoq, bcmStatExpansionTypeDispositionIsDrop);
        data->offset = 3;
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceIngressVoq < table->keys[0].size && bcmStatExpansionTypeTrafficClass < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressVoq, bcmStatExpansionTypeTrafficClass);
        data->offset = 4;
        data->nof_bits = 3;
    }
    if (bcmStatCounterInterfaceIngressVoq < table->keys[0].size && bcmStatExpansionTypeDropPrecedenceMeterResolved < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressVoq, bcmStatExpansionTypeDropPrecedenceMeterResolved);
        data->offset = 7;
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceEgressReceivePp < table->keys[0].size && bcmStatExpansionTypeMetaData < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceivePp, bcmStatExpansionTypeMetaData);
        data->offset = 0;
        data->nof_bits = 4;
    }
    if (bcmStatCounterInterfaceEgressReceivePp < table->keys[0].size && bcmStatExpansionTypeDropPrecedenceMeterResolved < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceivePp, bcmStatExpansionTypeDropPrecedenceMeterResolved);
        data->offset = 4;
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceEgressReceivePp < table->keys[0].size && bcmStatExpansionTypeTrafficClass < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceivePp, bcmStatExpansionTypeTrafficClass);
        data->offset = 6;
        data->nof_bits = 3;
    }
    if (bcmStatCounterInterfaceEgressReceivePp < table->keys[0].size && bcmStatExpansionTypeSystemMultiCast < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceivePp, bcmStatExpansionTypeSystemMultiCast);
        data->offset = 9;
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceEgressReceivePp < table->keys[0].size && bcmStatExpansionTypeEgressTmActionType < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceivePp, bcmStatExpansionTypeEgressTmActionType);
        data->offset = 10;
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceEgressReceivePp < table->keys[0].size && bcmStatExpansionTypePpDropReason < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceivePp, bcmStatExpansionTypePpDropReason);
        data->offset = 12;
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceEgressReceivePp < table->keys[0].size && bcmStatExpansionTypeDiscardPp < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceivePp, bcmStatExpansionTypeDiscardPp);
        data->offset = 14;
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceEgressReceivePp < table->keys[0].size && bcmStatExpansionTypeDiscardTm < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceivePp, bcmStatExpansionTypeDiscardTm);
        data->offset = 15;
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceEgressReceiveQueue < table->keys[0].size && bcmStatExpansionTypeDiscardPp < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceiveQueue, bcmStatExpansionTypeDiscardPp);
        data->offset = 0;
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceEgressReceiveQueue < table->keys[0].size && bcmStatExpansionTypeDiscardTm < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceiveQueue, bcmStatExpansionTypeDiscardTm);
        data->offset = 1;
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceEgressReceiveQueue < table->keys[0].size && bcmStatExpansionTypeDropPrecedenceMeterResolved < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceiveQueue, bcmStatExpansionTypeDropPrecedenceMeterResolved);
        data->offset = 2;
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceEgressReceiveQueue < table->keys[0].size && bcmStatExpansionTypeTrafficClass < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceiveQueue, bcmStatExpansionTypeTrafficClass);
        data->offset = 4;
        data->nof_bits = 3;
    }
    if (bcmStatCounterInterfaceEgressReceiveQueue < table->keys[0].size && bcmStatExpansionTypeTmDropReason < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceiveQueue, bcmStatExpansionTypeTmDropReason);
        data->offset = 7;
        data->nof_bits = 3;
    }
    if (bcmStatCounterInterfaceEgressTransmitPp < table->keys[0].size && bcmStatExpansionTypeDropPrecedenceMeterResolved < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressTransmitPp, bcmStatExpansionTypeDropPrecedenceMeterResolved);
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceEgressTransmitPp < table->keys[0].size && bcmStatExpansionTypeTrafficClass < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressTransmitPp, bcmStatExpansionTypeTrafficClass);
        data->nof_bits = 3;
    }
    if (bcmStatCounterInterfaceEgressTransmitPp < table->keys[0].size && bcmStatExpansionTypeSystemMultiCast < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressTransmitPp, bcmStatExpansionTypeSystemMultiCast);
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceEgressTransmitPp < table->keys[0].size && bcmStatExpansionTypeEcnEligibleAndCni < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressTransmitPp, bcmStatExpansionTypeEcnEligibleAndCni);
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceEgressTransmitPp < table->keys[0].size && bcmStatExpansionTypePortMetaData < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressTransmitPp, bcmStatExpansionTypePortMetaData);
        data->nof_bits = 8;
    }
    if (bcmStatCounterInterfaceEgressTransmitPp < table->keys[0].size && bcmStatExpansionTypePpDropReason < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressTransmitPp, bcmStatExpansionTypePpDropReason);
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceEgressTransmitPp < table->keys[0].size && bcmStatExpansionTypeMetaData < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressTransmitPp, bcmStatExpansionTypeMetaData);
        data->nof_bits = 4;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_crps_expansion_source_expansion_info_set(
    int unit)
{
    int source_index;
    dnx_data_crps_expansion_source_expansion_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_expansion;
    int table_index = dnx_data_crps_expansion_table_source_expansion_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = bcmStatCounterInterfaceCount;
    table->info_get.key_size[0] = bcmStatCounterInterfaceCount;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_expansion_source_expansion_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_crps_expansion_table_source_expansion_info");

    
    default_data = (dnx_data_crps_expansion_source_expansion_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->expansion_size = 0;
    default_data->supports_expansion_per_type = 0;
    
    for (source_index = 0; source_index < table->keys[0].size; source_index++)
    {
        data = (dnx_data_crps_expansion_source_expansion_info_t *) dnxc_data_mgmt_table_data_get(unit, table, source_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size)
    {
        data = (dnx_data_crps_expansion_source_expansion_info_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, 0);
        data->expansion_size = 36;
        data->supports_expansion_per_type = 1;
    }
    if (bcmStatCounterInterfaceIngressTransmitPp < table->keys[0].size)
    {
        data = (dnx_data_crps_expansion_source_expansion_info_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressTransmitPp, 0);
        data->expansion_size = 11;
        data->supports_expansion_per_type = 0;
    }
    if (bcmStatCounterInterfaceIngressVoq < table->keys[0].size)
    {
        data = (dnx_data_crps_expansion_source_expansion_info_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressVoq, 0);
        data->expansion_size = 9;
        data->supports_expansion_per_type = 0;
    }
    if (bcmStatCounterInterfaceEgressReceivePp < table->keys[0].size)
    {
        data = (dnx_data_crps_expansion_source_expansion_info_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceivePp, 0);
        data->expansion_size = 16;
        data->supports_expansion_per_type = 1;
    }
    if (bcmStatCounterInterfaceEgressReceiveQueue < table->keys[0].size)
    {
        data = (dnx_data_crps_expansion_source_expansion_info_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceiveQueue, 0);
        data->expansion_size = 10;
        data->supports_expansion_per_type = 0;
    }
    if (bcmStatCounterInterfaceEgressTransmitPp < table->keys[0].size)
    {
        data = (dnx_data_crps_expansion_source_expansion_info_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressTransmitPp, 0);
        data->expansion_size = 24;
        data->supports_expansion_per_type = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_crps_src_interface_etm_count_pp_port_enable_table_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_src_interface;
    int feature_index = dnx_data_crps_src_interface_etm_count_pp_port_enable_table_is_supported;
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
jr2_a0_dnx_data_crps_src_interface_command_id_set(
    int unit)
{
    int source_index;
    dnx_data_crps_src_interface_command_id_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_src_interface;
    int table_index = dnx_data_crps_src_interface_table_command_id;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = bcmStatCounterInterfaceCount;
    table->info_get.key_size[0] = bcmStatCounterInterfaceCount;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_src_interface_command_id_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_crps_src_interface_table_command_id");

    
    default_data = (dnx_data_crps_src_interface_command_id_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->size = 0;
    default_data->base = -1;
    
    for (source_index = 0; source_index < table->keys[0].size; source_index++)
    {
        data = (dnx_data_crps_src_interface_command_id_t *) dnxc_data_mgmt_table_data_get(unit, table, source_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size)
    {
        data = (dnx_data_crps_src_interface_command_id_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, 0);
        data->size = 10;
        data->base = 0;
    }
    if (bcmStatCounterInterfaceIngressOam < table->keys[0].size)
    {
        data = (dnx_data_crps_src_interface_command_id_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressOam, 0);
        data->size = 3;
        data->base = 7;
    }
    if (bcmStatCounterInterfaceIngressVoq < table->keys[0].size)
    {
        data = (dnx_data_crps_src_interface_command_id_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressVoq, 0);
        data->size = 2;
        data->base = 0;
    }
    if (bcmStatCounterInterfaceIngressTransmitPp < table->keys[0].size)
    {
        data = (dnx_data_crps_src_interface_command_id_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressTransmitPp, 0);
        data->size = 1;
        data->base = 0;
    }
    if (bcmStatCounterInterfaceEgressReceivePp < table->keys[0].size)
    {
        data = (dnx_data_crps_src_interface_command_id_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceivePp, 0);
        data->size = 2;
        data->base = 0;
    }
    if (bcmStatCounterInterfaceEgressReceiveQueue < table->keys[0].size)
    {
        data = (dnx_data_crps_src_interface_command_id_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceiveQueue, 0);
        data->size = 1;
        data->base = 0;
    }
    if (bcmStatCounterInterfaceEgressTransmitPp < table->keys[0].size)
    {
        data = (dnx_data_crps_src_interface_command_id_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressTransmitPp, 0);
        data->size = 6;
        data->base = 0;
    }
    if (bcmStatCounterInterfaceEgressOam < table->keys[0].size)
    {
        data = (dnx_data_crps_src_interface_command_id_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressOam, 0);
        data->size = 3;
        data->base = 0;
    }
    if (bcmStatCounterInterfaceOamp < table->keys[0].size)
    {
        data = (dnx_data_crps_src_interface_command_id_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceOamp, 0);
        data->size = 1;
        data->base = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_crps_src_interface_crps_counting_set(
    int unit)
{
    int source_index;
    dnx_data_crps_src_interface_crps_counting_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_src_interface;
    int table_index = dnx_data_crps_src_interface_table_crps_counting;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = bcmStatCounterInterfaceCount;
    table->info_get.key_size[0] = bcmStatCounterInterfaceCount;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_src_interface_crps_counting_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_crps_src_interface_table_crps_counting");

    
    default_data = (dnx_data_crps_src_interface_crps_counting_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid_for_counting = 0;
    
    for (source_index = 0; source_index < table->keys[0].size; source_index++)
    {
        data = (dnx_data_crps_src_interface_crps_counting_t *) dnxc_data_mgmt_table_data_get(unit, table, source_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size)
    {
        data = (dnx_data_crps_src_interface_crps_counting_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, 0);
        data->valid_for_counting = 1;
    }
    if (bcmStatCounterInterfaceIngressOam < table->keys[0].size)
    {
        data = (dnx_data_crps_src_interface_crps_counting_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressOam, 0);
        data->valid_for_counting = 1;
    }
    if (bcmStatCounterInterfaceIngressVoq < table->keys[0].size)
    {
        data = (dnx_data_crps_src_interface_crps_counting_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressVoq, 0);
        data->valid_for_counting = 1;
    }
    if (bcmStatCounterInterfaceIngressTransmitPp < table->keys[0].size)
    {
        data = (dnx_data_crps_src_interface_crps_counting_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressTransmitPp, 0);
        data->valid_for_counting = 1;
    }
    if (bcmStatCounterInterfaceEgressReceivePp < table->keys[0].size)
    {
        data = (dnx_data_crps_src_interface_crps_counting_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceivePp, 0);
        data->valid_for_counting = 1;
    }
    if (bcmStatCounterInterfaceEgressReceiveQueue < table->keys[0].size)
    {
        data = (dnx_data_crps_src_interface_crps_counting_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceiveQueue, 0);
        data->valid_for_counting = 1;
    }
    if (bcmStatCounterInterfaceEgressTransmitPp < table->keys[0].size)
    {
        data = (dnx_data_crps_src_interface_crps_counting_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressTransmitPp, 0);
        data->valid_for_counting = 1;
    }
    if (bcmStatCounterInterfaceEgressOam < table->keys[0].size)
    {
        data = (dnx_data_crps_src_interface_crps_counting_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressOam, 0);
        data->valid_for_counting = 1;
    }
    if (bcmStatCounterInterfaceOamp < table->keys[0].size)
    {
        data = (dnx_data_crps_src_interface_crps_counting_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceOamp, 0);
        data->valid_for_counting = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_crps_latency_etpp_command_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_latency;
    int define_index = dnx_data_crps_latency_define_etpp_command_id;
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
jr2_a0_dnx_data_crps_latency_stat_id_port_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_latency;
    int define_index = dnx_data_crps_latency_define_stat_id_port_offset;
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
jr2_a0_dnx_data_crps_latency_stat_id_tc_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_latency;
    int define_index = dnx_data_crps_latency_define_stat_id_tc_offset;
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
jr2_a0_dnx_data_crps_latency_stat_id_multicast_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_latency;
    int define_index = dnx_data_crps_latency_define_stat_id_multicast_offset;
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
jr2_a0_dnx_data_crps_state_counters_max_wide_state_counters_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_state_counters;
    int define_index = dnx_data_crps_state_counters_define_max_wide_state_counters_size;
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
jr2_a0_dnx_data_crps_state_counters_max_narrow_state_counters_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_state_counters;
    int define_index = dnx_data_crps_state_counters_define_max_narrow_state_counters_size;
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
jr2_a0_dnx_data_crps_state_counters_state_counter_data_size_per_full_counter_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_state_counters;
    int define_index = dnx_data_crps_state_counters_define_state_counter_data_size_per_full_counter;
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
jr2_a0_dnx_data_crps_state_counters_counter_size_per_format_type_set(
    int unit)
{
    int format_type_index;
    int counter_size_index;
    dnx_data_crps_state_counters_counter_size_per_format_type_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_state_counters;
    int table_index = dnx_data_crps_state_counters_table_counter_size_per_format_type;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = bcmStatCounterFormatCount;
    table->info_get.key_size[0] = bcmStatCounterFormatCount;
    table->keys[1].size = DNX_DATA_MAX_CRPS_STATE_COUNTERS_MAX_WIDE_STATE_COUNTERS_SIZE+1;
    table->info_get.key_size[1] = DNX_DATA_MAX_CRPS_STATE_COUNTERS_MAX_WIDE_STATE_COUNTERS_SIZE+1;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_state_counters_counter_size_per_format_type_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_crps_state_counters_table_counter_size_per_format_type");

    
    default_data = (dnx_data_crps_state_counters_counter_size_per_format_type_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = 0;
    
    for (format_type_index = 0; format_type_index < table->keys[0].size; format_type_index++)
    {
        for (counter_size_index = 0; counter_size_index < table->keys[1].size; counter_size_index++)
        {
            data = (dnx_data_crps_state_counters_counter_size_per_format_type_t *) dnxc_data_mgmt_table_data_get(unit, table, format_type_index, counter_size_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_crps_general_crps_expansion_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_general;
    int feature_index = dnx_data_crps_general_crps_expansion_support;
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
jr2_a0_dnx_data_crps_general_count_mc_as_one_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_general;
    int feature_index = dnx_data_crps_general_count_mc_as_one_support;
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
jr2_a0_dnx_data_crps_general_crps_compensation_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_general;
    int feature_index = dnx_data_crps_general_crps_compensation_support;
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
jr2_a0_dnx_data_crps_general_crps_set_eviction_boundaries_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_general;
    int feature_index = dnx_data_crps_general_crps_set_eviction_boundaries_support;
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
jr2_a0_data_crps_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_crps;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_crps_submodule_engine;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_crps_engine_define_nof_engines;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_engine_nof_engines_set;
    data_index = dnx_data_crps_engine_define_nof_mid_engines;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_engine_nof_mid_engines_set;
    data_index = dnx_data_crps_engine_define_nof_big_engines;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_engine_nof_big_engines_set;
    data_index = dnx_data_crps_engine_define_nof_databases;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_engine_nof_databases_set;
    data_index = dnx_data_crps_engine_define_data_mapping_table_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_engine_data_mapping_table_size_set;
    data_index = dnx_data_crps_engine_define_max_counter_set_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_engine_max_counter_set_size_set;
    data_index = dnx_data_crps_engine_define_oam_counter_set_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_engine_oam_counter_set_size_set;
    data_index = dnx_data_crps_engine_define_max_nof_counter_sets;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_engine_max_nof_counter_sets_set;
    data_index = dnx_data_crps_engine_define_last_address_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_engine_last_address_nof_bits_set;
    data_index = dnx_data_crps_engine_define_counter_format_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_engine_counter_format_nof_bits_set;
    data_index = dnx_data_crps_engine_define_counter_mode_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_engine_counter_mode_nof_bits_set;
    data_index = dnx_data_crps_engine_define_nof_small_meter_shared_engines;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_engine_nof_small_meter_shared_engines_set;
    data_index = dnx_data_crps_engine_define_nof_mid_meter_shared_engines;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_engine_nof_mid_meter_shared_engines_set;
    data_index = dnx_data_crps_engine_define_nof_big_meter_shared_engines;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_engine_nof_big_meter_shared_engines_set;

    
    data_index = dnx_data_crps_engine_valid_address_admission_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_engine_valid_address_admission_is_supported_set;
    data_index = dnx_data_crps_engine_last_input_command_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_engine_last_input_command_is_supported_set;
    data_index = dnx_data_crps_engine_rate_calculation_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_engine_rate_calculation_is_supported_set;
    data_index = dnx_data_crps_engine_multi_sets_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_engine_multi_sets_is_supported_set;
    data_index = dnx_data_crps_engine_dynamic_counter_mode_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_engine_dynamic_counter_mode_is_supported_set;

    
    data_index = dnx_data_crps_engine_table_engines_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_engine_engines_info_set;
    data_index = dnx_data_crps_engine_table_source_base;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_engine_source_base_set;
    data_index = dnx_data_crps_engine_table_counter_format_types;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_engine_counter_format_types_set;
    
    submodule_index = dnx_data_crps_submodule_eviction;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_crps_eviction_define_counters_fifo_depth;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_eviction_counters_fifo_depth_set;
    data_index = dnx_data_crps_eviction_define_nof_counters_fifo;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_eviction_nof_counters_fifo_set;
    data_index = dnx_data_crps_eviction_define_dma_record_entry_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_eviction_dma_record_entry_nof_bits_set;
    data_index = dnx_data_crps_eviction_define_bubble_before_request_timeout;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_eviction_bubble_before_request_timeout_set;
    data_index = dnx_data_crps_eviction_define_bubble_after_request_timeout;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_eviction_bubble_after_request_timeout_set;
    data_index = dnx_data_crps_eviction_define_seq_address_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_eviction_seq_address_nof_bits_set;
    data_index = dnx_data_crps_eviction_define_nof_eviction_destination_types;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_eviction_nof_eviction_destination_types_set;
    data_index = dnx_data_crps_eviction_define_nof_crps_network_channels;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_eviction_nof_crps_network_channels_set;
    data_index = dnx_data_crps_eviction_define_bg_thread_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_eviction_bg_thread_enable_set;

    
    data_index = dnx_data_crps_eviction_algorithmic_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_eviction_algorithmic_is_supported_set;
    data_index = dnx_data_crps_eviction_probabilistic_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_eviction_probabilistic_is_supported_set;
    data_index = dnx_data_crps_eviction_low_rate_probabilistic_eviction;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_eviction_low_rate_probabilistic_eviction_set;
    data_index = dnx_data_crps_eviction_conditional_action_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_eviction_conditional_action_is_supported_set;
    data_index = dnx_data_crps_eviction_eviction_from_memory_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_eviction_eviction_from_memory_is_supported_set;
    data_index = dnx_data_crps_eviction_sequential_scan_speedup_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_eviction_sequential_scan_speedup_is_supported_set;
    data_index = dnx_data_crps_eviction_probabilistic_eviction_thresholds_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_eviction_probabilistic_eviction_thresholds_is_supported_set;

    
    data_index = dnx_data_crps_eviction_table_phy_record_format;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_eviction_phy_record_format_set;
    data_index = dnx_data_crps_eviction_table_condional_action_valid;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_eviction_condional_action_valid_set;
    data_index = dnx_data_crps_eviction_table_supported_eviction_destination;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_eviction_supported_eviction_destination_set;
    
    submodule_index = dnx_data_crps_submodule_expansion;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_crps_expansion_define_expansion_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_expansion_expansion_size_set;

    

    
    data_index = dnx_data_crps_expansion_table_source_metadata_types;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_expansion_source_metadata_types_set;
    data_index = dnx_data_crps_expansion_table_source_expansion_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_expansion_source_expansion_info_set;
    
    submodule_index = dnx_data_crps_submodule_src_interface;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_crps_src_interface_etm_count_pp_port_enable_table_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_src_interface_etm_count_pp_port_enable_table_is_supported_set;

    
    data_index = dnx_data_crps_src_interface_table_command_id;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_src_interface_command_id_set;
    data_index = dnx_data_crps_src_interface_table_crps_counting;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_src_interface_crps_counting_set;
    
    submodule_index = dnx_data_crps_submodule_latency;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_crps_latency_define_etpp_command_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_latency_etpp_command_id_set;
    data_index = dnx_data_crps_latency_define_stat_id_port_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_latency_stat_id_port_offset_set;
    data_index = dnx_data_crps_latency_define_stat_id_tc_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_latency_stat_id_tc_offset_set;
    data_index = dnx_data_crps_latency_define_stat_id_multicast_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_latency_stat_id_multicast_offset_set;

    

    
    
    submodule_index = dnx_data_crps_submodule_state_counters;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_crps_state_counters_define_max_wide_state_counters_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_state_counters_max_wide_state_counters_size_set;
    data_index = dnx_data_crps_state_counters_define_max_narrow_state_counters_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_state_counters_max_narrow_state_counters_size_set;
    data_index = dnx_data_crps_state_counters_define_state_counter_data_size_per_full_counter;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_state_counters_state_counter_data_size_per_full_counter_set;

    

    
    data_index = dnx_data_crps_state_counters_table_counter_size_per_format_type;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_state_counters_counter_size_per_format_type_set;
    
    submodule_index = dnx_data_crps_submodule_general;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_crps_general_crps_expansion_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_general_crps_expansion_support_set;
    data_index = dnx_data_crps_general_count_mc_as_one_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_general_count_mc_as_one_support_set;
    data_index = dnx_data_crps_general_crps_compensation_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_general_crps_compensation_support_set;
    data_index = dnx_data_crps_general_crps_set_eviction_boundaries_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_general_crps_set_eviction_boundaries_support_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

