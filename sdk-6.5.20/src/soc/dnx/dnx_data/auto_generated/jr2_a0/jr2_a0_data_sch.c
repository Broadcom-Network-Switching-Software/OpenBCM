

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include <bcm_int/dnx/algo/sch/sch_alloc_mngr.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>







static shr_error_e
jr2_a0_dnx_data_sch_general_lag_scheduler_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_general;
    int feature_index = dnx_data_sch_general_lag_scheduler_supported;
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
jr2_a0_dnx_data_sch_general_fsm_modes_config_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_general;
    int feature_index = dnx_data_sch_general_fsm_modes_config_supported;
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
jr2_a0_dnx_data_sch_general_nof_slow_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_general;
    int define_index = dnx_data_sch_general_define_nof_slow_profiles;
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
jr2_a0_dnx_data_sch_general_nof_slow_levels_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_general;
    int define_index = dnx_data_sch_general_define_nof_slow_levels;
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
jr2_a0_dnx_data_sch_general_cycles_per_credit_token_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_general;
    int define_index = dnx_data_sch_general_define_cycles_per_credit_token;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12;

    
    define->data = 12;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_general_nof_fmq_class_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_general;
    int define_index = dnx_data_sch_general_define_nof_fmq_class;
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
jr2_a0_dnx_data_sch_general_port_priority_propagation_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_general;
    int define_index = dnx_data_sch_general_define_port_priority_propagation_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_PORT_SCH_PRIORITY_PROPAGATION_ENABLE;
    define->property.doc = 
        "\n"
        "Enable ports to be created with port priority propagation.\n"
        "Not supported for Jericho2.\n"
        "Default: 0 (disabled)\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 0;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_sch_general_low_rate_factor_to_dbal_enum_key_map(
    int unit,
    int key0_val,
    int key1_val,
    int *key0_index,
    int *key1_index)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (key0_val)
    {
        case 64:
            *key0_index = 0;
            break;
        case 32:
            *key0_index = 1;
            break;
        default:
            *key0_index = -1;
            break;
    }

    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_general_low_rate_factor_to_dbal_enum_key_reverse_map(
    int unit,
    int key0_index,
    int key1_index,
    int *key0_val,
    int *key1_val)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (key0_index)
    {
        case 0:
            *key0_val = 64;
            break;
        case 1:
            *key0_val = 32;
            break;
        default:
            *key0_val = -1;
            break;
    }

    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_general_low_rate_factor_to_dbal_enum_set(
    int unit)
{
    int low_rate_factor_index;
    dnx_data_sch_general_low_rate_factor_to_dbal_enum_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_general;
    int table_index = dnx_data_sch_general_table_low_rate_factor_to_dbal_enum;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_general_low_rate_factor_to_dbal_enum_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_sch_general_table_low_rate_factor_to_dbal_enum");

    
    default_data = (dnx_data_sch_general_low_rate_factor_to_dbal_enum_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_enum = -1;
    default_data->valid = 0;
    
    for (low_rate_factor_index = 0; low_rate_factor_index < table->keys[0].size; low_rate_factor_index++)
    {
        data = (dnx_data_sch_general_low_rate_factor_to_dbal_enum_t *) dnxc_data_mgmt_table_data_get(unit, table, low_rate_factor_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    table->key_map = jr2_a0_dnx_data_sch_general_low_rate_factor_to_dbal_enum_key_map;
    table->key_map_reverse = jr2_a0_dnx_data_sch_general_low_rate_factor_to_dbal_enum_key_reverse_map;
    
    data = (dnx_data_sch_general_low_rate_factor_to_dbal_enum_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
    data->dbal_enum = DBAL_ENUM_FVAL_LOW_RATE_FACTOR_LOW_RATE_FACTOR_32;
    data->valid = 1;
    data = (dnx_data_sch_general_low_rate_factor_to_dbal_enum_t *) dnxc_data_mgmt_table_data_get(unit, table, 64, 0);
    data->dbal_enum = DBAL_ENUM_FVAL_LOW_RATE_FACTOR_LOW_RATE_FACTOR_64;
    data->valid = 1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_general_dbal_enum_to_low_rate_factor_set(
    int unit)
{
    int dbal_enum_index;
    dnx_data_sch_general_dbal_enum_to_low_rate_factor_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_general;
    int table_index = dnx_data_sch_general_table_dbal_enum_to_low_rate_factor;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_LOW_RATE_FACTOR_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_LOW_RATE_FACTOR_VALUES;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_general_dbal_enum_to_low_rate_factor_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_sch_general_table_dbal_enum_to_low_rate_factor");

    
    default_data = (dnx_data_sch_general_dbal_enum_to_low_rate_factor_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->low_rate_factor = 0;
    
    for (dbal_enum_index = 0; dbal_enum_index < table->keys[0].size; dbal_enum_index++)
    {
        data = (dnx_data_sch_general_dbal_enum_to_low_rate_factor_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_enum_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_LOW_RATE_FACTOR_LOW_RATE_FACTOR_32 < table->keys[0].size)
    {
        data = (dnx_data_sch_general_dbal_enum_to_low_rate_factor_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_LOW_RATE_FACTOR_LOW_RATE_FACTOR_32, 0);
        data->low_rate_factor = 32;
    }
    if (DBAL_ENUM_FVAL_LOW_RATE_FACTOR_LOW_RATE_FACTOR_64 < table->keys[0].size)
    {
        data = (dnx_data_sch_general_dbal_enum_to_low_rate_factor_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_LOW_RATE_FACTOR_LOW_RATE_FACTOR_64, 0);
        data->low_rate_factor = 64;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_general_slow_rate_max_bucket_set(
    int unit)
{
    int idx_index;
    dnx_data_sch_general_slow_rate_max_bucket_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_general;
    int table_index = dnx_data_sch_general_table_slow_rate_max_bucket;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 7;
    table->info_get.key_size[0] = 7;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_general_slow_rate_max_bucket_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_sch_general_table_slow_rate_max_bucket");

    
    default_data = (dnx_data_sch_general_slow_rate_max_bucket_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->rate = -1;
    default_data->max_bucket = 0;
    
    for (idx_index = 0; idx_index < table->keys[0].size; idx_index++)
    {
        data = (dnx_data_sch_general_slow_rate_max_bucket_t *) dnxc_data_mgmt_table_data_get(unit, table, idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_sch_general_slow_rate_max_bucket_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->rate = 0;
        data->max_bucket = 1;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_sch_general_slow_rate_max_bucket_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->rate = 9781;
        data->max_bucket = 2;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_sch_general_slow_rate_max_bucket_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->rate = 18386;
        data->max_bucket = 3;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_sch_general_slow_rate_max_bucket_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->rate = 33271;
        data->max_bucket = 4;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_sch_general_slow_rate_max_bucket_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->rate = 57806;
        data->max_bucket = 5;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_sch_general_slow_rate_max_bucket_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->rate = 95507;
        data->max_bucket = 6;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_sch_general_slow_rate_max_bucket_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->rate = 146926;
        data->max_bucket = 7;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_sch_ps_min_priority_for_tcg_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_ps;
    int define_index = dnx_data_sch_ps_define_min_priority_for_tcg;
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
jr2_a0_dnx_data_sch_ps_max_nof_tcg_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_ps;
    int define_index = dnx_data_sch_ps_define_max_nof_tcg;
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
jr2_a0_dnx_data_sch_ps_nof_hrs_in_ps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_ps;
    int define_index = dnx_data_sch_ps_define_nof_hrs_in_ps;
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
jr2_a0_dnx_data_sch_ps_max_tcg_weight_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_ps;
    int define_index = dnx_data_sch_ps_define_max_tcg_weight;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1023;

    
    define->data = 1023;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_ps_tc_tcg_calendar_access_period_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_ps;
    int define_index = dnx_data_sch_ps_define_tc_tcg_calendar_access_period;
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
jr2_a0_dnx_data_sch_ps_max_port_rate_mbps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_ps;
    int define_index = dnx_data_sch_ps_define_max_port_rate_mbps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 600000;

    
    define->data = 600000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_MAXIMAL_PORT_BANDWIDTH;
    define->property.doc = 
        "\n"
        "Maximal bandwidth of a single port in Mega-bits-per-second.\n"
        "Used to improve granularity of SCH port-TC and port-TCG shapers.\n"
        "Default: 600000 (600G)\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = dnx_data_sch.interface.max_if_rate_mbps_get(unit);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_ps_max_burst_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_ps;
    int define_index = dnx_data_sch_ps_define_max_burst;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = utilex_power_of_2(dnx_data_sch.dbal.ps_shaper_max_burst_bits_get(unit)) - 1;

    
    define->data = utilex_power_of_2(dnx_data_sch.dbal.ps_shaper_max_burst_bits_get(unit)) - 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_sch_flow_nof_hr_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_nof_hr;
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
jr2_a0_dnx_data_sch_flow_nof_cl_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_nof_cl;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8*1024;

    
    define->data = 8*1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_flow_nof_se_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_nof_se;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32*1024;

    
    define->data = 32*1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_flow_nof_flows_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_nof_flows;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 96*1024;

    
    define->data = 96*1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_flow_region_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_region_size;
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
jr2_a0_dnx_data_sch_flow_nof_regions_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_nof_regions;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.flow.nof_flows_get(unit) / dnx_data_sch.flow.region_size_get(unit);

    
    define->data = dnx_data_sch.flow.nof_flows_get(unit) / dnx_data_sch.flow.region_size_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_flow_nof_hr_regions_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_nof_hr_regions;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.flow.nof_hr_get(unit) / 256;

    
    define->data = dnx_data_sch.flow.nof_hr_get(unit) / 256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_flow_nof_connectors_only_regions_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_nof_connectors_only_regions;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_sch.flow.nof_flows_get(unit) - dnx_data_sch.flow.nof_se_get(unit)) / dnx_data_sch.flow.region_size_get(unit);

    
    define->data = (dnx_data_sch.flow.nof_flows_get(unit) - dnx_data_sch.flow.nof_se_get(unit)) / dnx_data_sch.flow.region_size_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_flow_flow_in_region_mask_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_flow_in_region_mask;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x3ff;

    
    define->data = 0x3ff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_flow_min_connector_bundle_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_min_connector_bundle_size;
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
jr2_a0_dnx_data_sch_flow_conn_idx_interdig_flow_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_conn_idx_interdig_flow;
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
jr2_a0_dnx_data_sch_flow_shaper_flipflops_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_shaper_flipflops_nof;
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
jr2_a0_dnx_data_sch_flow_erp_hr_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_erp_hr;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.flow.nof_hr_get(unit)-8;

    
    define->data = dnx_data_sch.flow.nof_hr_get(unit)-8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_flow_reserved_hr_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_reserved_hr;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.flow.erp_hr_get(unit)-1;

    
    define->data = dnx_data_sch.flow.erp_hr_get(unit)-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_flow_max_se_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_max_se;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.flow.nof_se_get(unit)-1;

    
    define->data = dnx_data_sch.flow.nof_se_get(unit)-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_flow_max_flow_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_max_flow;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.flow.nof_flows_get(unit)-1;

    
    define->data = dnx_data_sch.flow.nof_flows_get(unit)-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_flow_first_hr_region_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_first_hr_region;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.flow.nof_regions_get(unit) - dnx_data_sch.flow.nof_hr_regions_get(unit);

    
    define->data = dnx_data_sch.flow.nof_regions_get(unit) - dnx_data_sch.flow.nof_hr_regions_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_flow_first_se_flow_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_first_se_flow_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.flow.nof_connectors_only_regions_get(unit) * dnx_data_sch.flow.region_size_get(unit);

    
    define->data = dnx_data_sch.flow.nof_connectors_only_regions_get(unit) * dnx_data_sch.flow.region_size_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_flow_hr_se_id_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_hr_se_id_min;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.flow.nof_se_get(unit)/2 - dnx_data_sch.flow.nof_hr_get(unit);

    
    define->data = dnx_data_sch.flow.nof_se_get(unit)/2 - dnx_data_sch.flow.nof_hr_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_flow_hr_se_id_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_hr_se_id_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.flow.nof_se_get(unit)/2-1;

    
    define->data = dnx_data_sch.flow.nof_se_get(unit)/2-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_flow_default_credit_source_se_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_default_credit_source_se_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.flow.nof_se_get(unit) / 4 * 3 +                      (dnx_data_sch.flow.nof_regions_get(unit) - dnx_data_sch.flow.nof_connectors_only_regions_get(unit) -dnx_data_sch.flow.nof_hr_regions_get(unit)) * dnx_data_sch.flow.region_size_get(unit)/4;

    
    define->data = dnx_data_sch.flow.nof_se_get(unit) / 4 * 3 +                      (dnx_data_sch.flow.nof_regions_get(unit) - dnx_data_sch.flow.nof_connectors_only_regions_get(unit) -dnx_data_sch.flow.nof_hr_regions_get(unit)) * dnx_data_sch.flow.region_size_get(unit)/4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_flow_runtime_performance_optimize_enable_sched_allocation_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_runtime_performance_optimize_enable_sched_allocation;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_RUNTIME_PERFORMANCE_OPTIMIZE_ENABLE;
    define->property.doc = 
        "\n"
        "Enable flow property access runtime optimization\n"
        "In Jericho2 optimization must be enabled\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 1;
    define->property.range_max = 1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_sch_flow_region_type_set(
    int unit)
{
    int core_index;
    int region_index;
    dnx_data_sch_flow_region_type_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int table_index = dnx_data_sch_flow_table_region_type;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_device.general.nof_cores_get(unit);
    table->info_get.key_size[0] = dnx_data_device.general.nof_cores_get(unit);
    table->keys[1].size = dnx_data_sch.flow.nof_regions_get(unit);
    table->info_get.key_size[1] = dnx_data_sch.flow.nof_regions_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_flow_region_type_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_sch_flow_table_region_type");

    
    default_data = (dnx_data_sch_flow_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->type = 0;
    default_data->propagation_direction = 0;
    
    for (core_index = 0; core_index < table->keys[0].size; core_index++)
    {
        for (region_index = 0; region_index < table->keys[1].size; region_index++)
        {
            data = (dnx_data_sch_flow_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, core_index, region_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    table->property.name = "dtm_flow_mapping_mode_region";
    table->property.doc =
        "\n"
        "Configure region mode per region id:\n"
        "0: connectors only\n"
        "1: interdigitated odd\n"
        "2: interdigitated even\n"
        "3: SE only, propagation direction 0\n"
        "7: SE only, propagation direction 1\n"
        "Default: 0\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (core_index = 0; core_index < table->keys[0].size; core_index++)
    {
        for (region_index = 0; region_index < table->keys[1].size; region_index++)
        {
            data = (dnx_data_sch_flow_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, core_index, region_index);
            SHR_IF_ERR_EXIT(dnx_data_property_sch_flow_region_type_read(unit, core_index, region_index, data));
        }
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_flow_nof_remote_cores_set(
    int unit)
{
    int core_index;
    int region_index;
    dnx_data_sch_flow_nof_remote_cores_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int table_index = dnx_data_sch_flow_table_nof_remote_cores;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_device.general.nof_cores_get(unit);
    table->info_get.key_size[0] = dnx_data_device.general.nof_cores_get(unit);
    table->keys[1].size = dnx_data_sch.flow.nof_regions_get(unit);
    table->info_get.key_size[1] = dnx_data_sch.flow.nof_regions_get(unit);

    
    table->values[0].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_flow_nof_remote_cores_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_sch_flow_table_nof_remote_cores");

    
    default_data = (dnx_data_sch_flow_nof_remote_cores_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = 1;
    
    for (core_index = 0; core_index < table->keys[0].size; core_index++)
    {
        for (region_index = 0; region_index < table->keys[1].size; region_index++)
        {
            data = (dnx_data_sch_flow_nof_remote_cores_t *) dnxc_data_mgmt_table_data_get(unit, table, core_index, region_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    table->property.name = "dtm_flow_nof_remote_cores_region";
    table->property.doc =
        "\n"
        "Configure region nof remote cores:\n"
        "1: one core\n"
        "2: two cores\n"
        "Default: 1\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (core_index = 0; core_index < table->keys[0].size; core_index++)
    {
        for (region_index = 0; region_index < table->keys[1].size; region_index++)
        {
            data = (dnx_data_sch_flow_nof_remote_cores_t *) dnxc_data_mgmt_table_data_get(unit, table, core_index, region_index);
            SHR_IF_ERR_EXIT(dnx_data_property_sch_flow_nof_remote_cores_read(unit, core_index, region_index, data));
        }
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_flow_cl_num_to_quartet_offset_set(
    int unit)
{
    int cl_num_index;
    dnx_data_sch_flow_cl_num_to_quartet_offset_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int table_index = dnx_data_sch_flow_table_cl_num_to_quartet_offset;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 1;
    table->info_get.key_size[0] = 1;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_flow_cl_num_to_quartet_offset_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_sch_flow_table_cl_num_to_quartet_offset");

    
    default_data = (dnx_data_sch_flow_cl_num_to_quartet_offset_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->quartet_offset = -1;
    
    for (cl_num_index = 0; cl_num_index < table->keys[0].size; cl_num_index++)
    {
        data = (dnx_data_sch_flow_cl_num_to_quartet_offset_t *) dnxc_data_mgmt_table_data_get(unit, table, cl_num_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_sch_flow_cl_num_to_quartet_offset_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->quartet_offset = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_flow_quartet_offset_to_cl_num_set(
    int unit)
{
    int quartet_offset_index;
    dnx_data_sch_flow_quartet_offset_to_cl_num_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int table_index = dnx_data_sch_flow_table_quartet_offset_to_cl_num;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_flow_quartet_offset_to_cl_num_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_sch_flow_table_quartet_offset_to_cl_num");

    
    default_data = (dnx_data_sch_flow_quartet_offset_to_cl_num_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->cl_num = -1;
    
    for (quartet_offset_index = 0; quartet_offset_index < table->keys[0].size; quartet_offset_index++)
    {
        data = (dnx_data_sch_flow_quartet_offset_to_cl_num_t *) dnxc_data_mgmt_table_data_get(unit, table, quartet_offset_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_sch_flow_quartet_offset_to_cl_num_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->cl_num = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_sch_se_default_cos_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_se;
    int define_index = dnx_data_sch_se_define_default_cos;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xfc;

    
    define->data = 0xfc;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_se_nof_color_group_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_se;
    int define_index = dnx_data_sch_se_define_nof_color_group;
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
jr2_a0_dnx_data_sch_se_max_hr_weight_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_se;
    int define_index = dnx_data_sch_se_define_max_hr_weight;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4*1024;

    
    define->data = 4*1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_se_cl_class_profile_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_se;
    int define_index = dnx_data_sch_se_define_cl_class_profile_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256;

    
    define->data = 256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_sch_interface_nof_big_calendars_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_interface;
    int define_index = dnx_data_sch_interface_define_nof_big_calendars;
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
jr2_a0_dnx_data_sch_interface_nof_channelized_calendars_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_interface;
    int define_index = dnx_data_sch_interface_define_nof_channelized_calendars;
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
jr2_a0_dnx_data_sch_interface_big_calendar_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_interface;
    int define_index = dnx_data_sch_interface_define_big_calendar_size;
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
jr2_a0_dnx_data_sch_interface_nof_sch_interfaces_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_interface;
    int define_index = dnx_data_sch_interface_define_nof_sch_interfaces;
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
jr2_a0_dnx_data_sch_interface_cal_speed_resolution_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_interface;
    int define_index = dnx_data_sch_interface_define_cal_speed_resolution;
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
jr2_a0_dnx_data_sch_interface_reserved_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_interface;
    int define_index = dnx_data_sch_interface_define_reserved;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.interface.nof_sch_interfaces_get(unit)-1;

    
    define->data = dnx_data_sch.interface.nof_sch_interfaces_get(unit)-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_interface_max_if_rate_mbps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_interface;
    int define_index = dnx_data_sch_interface_define_max_if_rate_mbps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_device.general.maximal_core_bandwidth_kbps_get(unit) / 1000;

    
    define->data = dnx_data_device.general.maximal_core_bandwidth_kbps_get(unit) / 1000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_sch_interface_shaper_resolution_set(
    int unit)
{
    int channelized_index;
    dnx_data_sch_interface_shaper_resolution_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_interface;
    int table_index = dnx_data_sch_interface_table_shaper_resolution;
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

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_interface_shaper_resolution_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_sch_interface_table_shaper_resolution");

    
    default_data = (dnx_data_sch_interface_shaper_resolution_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->resolution = 0;
    
    for (channelized_index = 0; channelized_index < table->keys[0].size; channelized_index++)
    {
        data = (dnx_data_sch_interface_shaper_resolution_t *) dnxc_data_mgmt_table_data_get(unit, table, channelized_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_sch_interface_shaper_resolution_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->resolution = 4;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_sch_device_shared_drm_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_device;
    int feature_index = dnx_data_sch_device_shared_drm;
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
jr2_a0_dnx_data_sch_device_drm_resolution_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_device;
    int define_index = dnx_data_sch_device_define_drm_resolution;
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
jr2_a0_dnx_data_sch_sch_alloc_tag_aggregate_se_2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int define_index = dnx_data_sch_sch_alloc_define_tag_aggregate_se_2;
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
jr2_a0_dnx_data_sch_sch_alloc_tag_aggregate_se_4_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int define_index = dnx_data_sch_sch_alloc_define_tag_aggregate_se_4;
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
jr2_a0_dnx_data_sch_sch_alloc_tag_aggregate_se_8_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int define_index = dnx_data_sch_sch_alloc_define_tag_aggregate_se_8;
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
jr2_a0_dnx_data_sch_sch_alloc_shared_shaper_max_tag_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int define_index = dnx_data_sch_sch_alloc_define_shared_shaper_max_tag_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.sch_alloc.tag_aggregate_se_8_get(unit);

    
    define->data = dnx_data_sch.sch_alloc.tag_aggregate_se_8_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_sch_alloc_tag_size_aggregate_se_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int define_index = dnx_data_sch_sch_alloc_define_tag_size_aggregate_se;
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
jr2_a0_dnx_data_sch_sch_alloc_tag_size_con_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int define_index = dnx_data_sch_sch_alloc_define_tag_size_con;
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
jr2_a0_dnx_data_sch_sch_alloc_type_con_reg_start_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int define_index = dnx_data_sch_sch_alloc_define_type_con_reg_start;
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
jr2_a0_dnx_data_sch_sch_alloc_alloc_invalid_flow_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int define_index = dnx_data_sch_sch_alloc_define_alloc_invalid_flow;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = -1;

    
    define->data = -1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_sch_alloc_dealloc_flow_ids_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int define_index = dnx_data_sch_sch_alloc_define_dealloc_flow_ids;
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
jr2_a0_dnx_data_sch_sch_alloc_type_con_reg_end_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int define_index = dnx_data_sch_sch_alloc_define_type_con_reg_end;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.flow.nof_connectors_only_regions_get(unit) - 1;

    
    define->data = dnx_data_sch.flow.nof_connectors_only_regions_get(unit) - 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_sch_alloc_type_hr_reg_start_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int define_index = dnx_data_sch_sch_alloc_define_type_hr_reg_start;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.flow.nof_regions_get(unit) - dnx_data_sch.flow.nof_hr_regions_get(unit);

    
    define->data = dnx_data_sch.flow.nof_regions_get(unit) - dnx_data_sch.flow.nof_hr_regions_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_sch_sch_alloc_region_set(
    int unit)
{
    int region_type_index;
    dnx_data_sch_sch_alloc_region_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int table_index = dnx_data_sch_sch_alloc_table_region;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_SCH_REGION_TYPE_NOF;
    table->info_get.key_size[0] = DNX_SCH_REGION_TYPE_NOF;

    
    table->values[0].default_val = "DNX_SCH_ELEMENT_SE_TYPE_NONE";
    table->values[1].default_val = "DNX_SCHEDULER_REGION_EVEN";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_sch_alloc_region_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_sch_sch_alloc_table_region");

    
    default_data = (dnx_data_sch_sch_alloc_region_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->flow_type[0] = DNX_SCH_ELEMENT_SE_TYPE_NONE;
    default_data->odd_even_mode = DNX_SCHEDULER_REGION_EVEN;
    
    for (region_type_index = 0; region_type_index < table->keys[0].size; region_type_index++)
    {
        data = (dnx_data_sch_sch_alloc_region_t *) dnxc_data_mgmt_table_data_get(unit, table, region_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_SCH_REGION_TYPE_CON < table->keys[0].size)
    {
        data = (dnx_data_sch_sch_alloc_region_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_REGION_TYPE_CON, 0);
        data->flow_type[0] = DNX_SCH_ELEMENT_SE_TYPE_NONE;
        data->flow_type[1] = DNX_SCH_ELEMENT_SE_TYPE_NONE;
        data->flow_type[2] = DNX_SCH_ELEMENT_SE_TYPE_NONE;
        data->flow_type[3] = DNX_SCH_ELEMENT_SE_TYPE_NONE;
        data->odd_even_mode = DNX_SCHEDULER_REGION_ODD;
    }
    if (DNX_SCH_REGION_TYPE_INTERDIG_1 < table->keys[0].size)
    {
        data = (dnx_data_sch_sch_alloc_region_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_REGION_TYPE_INTERDIG_1, 0);
        data->flow_type[0] = DNX_SCH_ELEMENT_SE_TYPE_CL;
        data->flow_type[1] = DNX_SCH_ELEMENT_SE_TYPE_FQ;
        data->flow_type[2] = DNX_SCH_ELEMENT_SE_TYPE_NONE;
        data->flow_type[3] = DNX_SCH_ELEMENT_SE_TYPE_NONE;
        data->odd_even_mode = DNX_SCHEDULER_REGION_ODD;
    }
    if (DNX_SCH_REGION_TYPE_SE < table->keys[0].size)
    {
        data = (dnx_data_sch_sch_alloc_region_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_REGION_TYPE_SE, 0);
        data->flow_type[0] = DNX_SCH_ELEMENT_SE_TYPE_CL;
        data->flow_type[1] = DNX_SCH_ELEMENT_SE_TYPE_FQ;
        data->flow_type[2] = DNX_SCH_ELEMENT_SE_TYPE_FQ;
        data->flow_type[3] = DNX_SCH_ELEMENT_SE_TYPE_FQ;
        data->odd_even_mode = DNX_SCHEDULER_REGION_ODD;
    }
    if (DNX_SCH_REGION_TYPE_SE_HR < table->keys[0].size)
    {
        data = (dnx_data_sch_sch_alloc_region_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_REGION_TYPE_SE_HR, 0);
        data->flow_type[0] = DNX_SCH_ELEMENT_SE_TYPE_CL;
        data->flow_type[1] = DNX_SCH_ELEMENT_SE_TYPE_HR;
        data->flow_type[2] = DNX_SCH_ELEMENT_SE_TYPE_FQ;
        data->flow_type[3] = DNX_SCH_ELEMENT_SE_TYPE_FQ;
        data->odd_even_mode = DNX_SCHEDULER_REGION_EVEN;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_sch_alloc_connector_set(
    int unit)
{
    int composite_index;
    int interdigitated_index;
    dnx_data_sch_sch_alloc_connector_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int table_index = dnx_data_sch_sch_alloc_table_connector;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;
    table->keys[1].size = 2;
    table->info_get.key_size[1] = 2;

    
    table->values[0].default_val = "4";
    table->values[1].default_val = "1";
    table->values[2].default_val = "1";
    table->values[3].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_sch_alloc_connector_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_sch_sch_alloc_table_connector");

    
    default_data = (dnx_data_sch_sch_alloc_connector_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->alignment = 4;
    default_data->pattern = 1;
    default_data->pattern_size = 1;
    default_data->nof_in_pattern = 1;
    
    for (composite_index = 0; composite_index < table->keys[0].size; composite_index++)
    {
        for (interdigitated_index = 0; interdigitated_index < table->keys[1].size; interdigitated_index++)
        {
            data = (dnx_data_sch_sch_alloc_connector_t *) dnxc_data_mgmt_table_data_get(unit, table, composite_index, interdigitated_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_connector_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->alignment = 4;
        data->pattern = 0x1;
        data->pattern_size = 1;
    }
    if (1 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_connector_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->alignment = 8;
        data->pattern = 0x3;
        data->pattern_size = 2;
    }
    if (0 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_connector_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 1);
        data->alignment = 8;
        data->pattern = 0xc;
        data->pattern_size = 4;
        data->nof_in_pattern = 2;
    }
    if (1 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_connector_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 1);
        data->alignment = 16;
        data->pattern = 0xc;
        data->pattern_size = 4;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_sch_alloc_se_per_region_type_set(
    int unit)
{
    int flow_type_index;
    int region_type_index;
    dnx_data_sch_sch_alloc_se_per_region_type_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int table_index = dnx_data_sch_sch_alloc_table_se_per_region_type;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_SCH_ALLOC_FLOW_TYPE_NOF;
    table->info_get.key_size[0] = DNX_SCH_ALLOC_FLOW_TYPE_NOF;
    table->keys[1].size = DNX_SCH_REGION_TYPE_NOF;
    table->info_get.key_size[1] = DNX_SCH_REGION_TYPE_NOF;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "4";
    table->values[2].default_val = "1";
    table->values[3].default_val = "4";
    table->values[4].default_val = "1";
    table->values[5].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_sch_alloc_se_per_region_type_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_sch_sch_alloc_table_se_per_region_type");

    
    default_data = (dnx_data_sch_sch_alloc_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = 0;
    default_data->alignment = 4;
    default_data->pattern = 1;
    default_data->pattern_size = 4;
    default_data->nof_offsets = 1;
    default_data->offset[0] = 0;
    
    for (flow_type_index = 0; flow_type_index < table->keys[0].size; flow_type_index++)
    {
        for (region_type_index = 0; region_type_index < table->keys[1].size; region_type_index++)
        {
            data = (dnx_data_sch_sch_alloc_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, flow_type_index, region_type_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (DNX_SCH_ALLOC_FLOW_TYPE_FQ < table->keys[0].size && DNX_SCH_REGION_TYPE_INTERDIG_1 < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_FQ, DNX_SCH_REGION_TYPE_INTERDIG_1);
        data->valid = 1;
        data->pattern = 0x2;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_FQ < table->keys[0].size && DNX_SCH_REGION_TYPE_SE < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_FQ, DNX_SCH_REGION_TYPE_SE);
        data->valid = 1;
        data->pattern = 0x1;
        data->nof_offsets = 3;
        data->offset[0] = 1;
        data->offset[1] = 2;
        data->offset[2] = 3;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_FQ < table->keys[0].size && DNX_SCH_REGION_TYPE_SE_HR < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_FQ, DNX_SCH_REGION_TYPE_SE_HR);
        data->valid = 1;
        data->pattern = 0x1;
        data->nof_offsets = 2;
        data->offset[0] = 2;
        data->offset[1] = 3;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_sch_alloc_se_set(
    int unit)
{
    int flow_type_index;
    dnx_data_sch_sch_alloc_se_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int table_index = dnx_data_sch_sch_alloc_table_se;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_SCH_ALLOC_FLOW_TYPE_NOF;
    table->info_get.key_size[0] = DNX_SCH_ALLOC_FLOW_TYPE_NOF;

    
    table->values[0].default_val = "4";
    table->values[1].default_val = "1";
    table->values[2].default_val = "4";
    table->values[3].default_val = "1";
    table->values[4].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_sch_alloc_se_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_sch_sch_alloc_table_se");

    
    default_data = (dnx_data_sch_sch_alloc_se_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->alignment = 4;
    default_data->pattern = 1;
    default_data->pattern_size = 4;
    default_data->nof_offsets = 1;
    default_data->offset[0] = 0;
    
    for (flow_type_index = 0; flow_type_index < table->keys[0].size; flow_type_index++)
    {
        data = (dnx_data_sch_sch_alloc_se_t *) dnxc_data_mgmt_table_data_get(unit, table, flow_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_SCH_ALLOC_FLOW_TYPE_CL < table->keys[0].size)
    {
        data = (dnx_data_sch_sch_alloc_se_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_CL, 0);
        data->pattern = 0x1;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL < table->keys[0].size)
    {
        data = (dnx_data_sch_sch_alloc_se_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL, 0);
        data->pattern = 0x3;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_HR < table->keys[0].size)
    {
        data = (dnx_data_sch_sch_alloc_se_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_HR, 0);
        data->pattern = 0x2;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_SHARED_CL_FQ < table->keys[0].size)
    {
        data = (dnx_data_sch_sch_alloc_se_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_SHARED_CL_FQ, 0);
        data->pattern = 0x3;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_4 < table->keys[0].size)
    {
        data = (dnx_data_sch_sch_alloc_se_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_4, 0);
        data->pattern = 0xf;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_8 < table->keys[0].size)
    {
        data = (dnx_data_sch_sch_alloc_se_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_8, 0);
        data->alignment = 8;
        data->pattern = 0xff;
        data->pattern_size = 8;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_sch_alloc_composite_se_per_region_type_set(
    int unit)
{
    int flow_type_index;
    int region_type_index;
    dnx_data_sch_sch_alloc_composite_se_per_region_type_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int table_index = dnx_data_sch_sch_alloc_table_composite_se_per_region_type;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_SCH_ALLOC_FLOW_TYPE_NOF;
    table->info_get.key_size[0] = DNX_SCH_ALLOC_FLOW_TYPE_NOF;
    table->keys[1].size = DNX_SCH_REGION_TYPE_NOF;
    table->info_get.key_size[1] = DNX_SCH_REGION_TYPE_NOF;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "4";
    table->values[2].default_val = "1";
    table->values[3].default_val = "4";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_sch_alloc_composite_se_per_region_type_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_sch_sch_alloc_table_composite_se_per_region_type");

    
    default_data = (dnx_data_sch_sch_alloc_composite_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = 0;
    default_data->alignment = 4;
    default_data->pattern = 1;
    default_data->pattern_size = 4;
    
    for (flow_type_index = 0; flow_type_index < table->keys[0].size; flow_type_index++)
    {
        for (region_type_index = 0; region_type_index < table->keys[1].size; region_type_index++)
        {
            data = (dnx_data_sch_sch_alloc_composite_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, flow_type_index, region_type_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_sch_alloc_composite_se_set(
    int unit)
{
    int flow_type_index;
    int odd_even_mode_index;
    dnx_data_sch_sch_alloc_composite_se_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int table_index = dnx_data_sch_sch_alloc_table_composite_se;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_SCH_ALLOC_FLOW_TYPE_NOF;
    table->info_get.key_size[0] = DNX_SCH_ALLOC_FLOW_TYPE_NOF;
    table->keys[1].size = DNX_SCHEDULER_REGION_ODD_EVEN_MODE_NOF;
    table->info_get.key_size[1] = DNX_SCHEDULER_REGION_ODD_EVEN_MODE_NOF;

    
    table->values[0].default_val = "4";
    table->values[1].default_val = "1";
    table->values[2].default_val = "4";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_sch_alloc_composite_se_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_sch_sch_alloc_table_composite_se");

    
    default_data = (dnx_data_sch_sch_alloc_composite_se_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->alignment = 4;
    default_data->pattern = 1;
    default_data->pattern_size = 4;
    
    for (flow_type_index = 0; flow_type_index < table->keys[0].size; flow_type_index++)
    {
        for (odd_even_mode_index = 0; odd_even_mode_index < table->keys[1].size; odd_even_mode_index++)
        {
            data = (dnx_data_sch_sch_alloc_composite_se_t *) dnxc_data_mgmt_table_data_get(unit, table, flow_type_index, odd_even_mode_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL < table->keys[0].size && DNX_SCHEDULER_REGION_EVEN < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_composite_se_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL, DNX_SCHEDULER_REGION_EVEN);
        data->pattern = 0x5;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL < table->keys[0].size && DNX_SCHEDULER_REGION_ODD < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_composite_se_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL, DNX_SCHEDULER_REGION_ODD);
        data->pattern = 0x3;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_COMP_HR < table->keys[0].size && DNX_SCHEDULER_REGION_EVEN < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_composite_se_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_COMP_HR, DNX_SCHEDULER_REGION_EVEN);
        data->pattern = 0xa;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_COMP_HR < table->keys[0].size && DNX_SCHEDULER_REGION_ODD < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_composite_se_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_COMP_HR, DNX_SCHEDULER_REGION_ODD);
        data->pattern = 0x0;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ < table->keys[0].size && DNX_SCHEDULER_REGION_EVEN < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_composite_se_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ, DNX_SCHEDULER_REGION_EVEN);
        data->pattern = 0xa;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ < table->keys[0].size && DNX_SCHEDULER_REGION_ODD < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_composite_se_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ, DNX_SCHEDULER_REGION_ODD);
        data->pattern = 0xc;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_sch_dbal_scheduler_enable_grouping_factor_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_scheduler_enable_grouping_factor;
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
jr2_a0_dnx_data_sch_dbal_scheduler_enable_dbal_mult_factor_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_scheduler_enable_dbal_mult_factor;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (1 << dnx_data_sch.dbal.scheduler_enable_grouping_factor_get(unit))-1;

    
    define->data = (1 << dnx_data_sch.dbal.scheduler_enable_grouping_factor_get(unit))-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_dbal_flow_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_flow_bits;
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
jr2_a0_dnx_data_sch_dbal_se_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_se_bits;
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
jr2_a0_dnx_data_sch_dbal_cl_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_cl_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 13;

    
    define->data = 13;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_dbal_hr_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_hr_bits;
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
jr2_a0_dnx_data_sch_dbal_interface_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_interface_bits;
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
jr2_a0_dnx_data_sch_dbal_calendar_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_calendar_bits;
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
jr2_a0_dnx_data_sch_dbal_flow_shaper_mant_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_flow_shaper_mant_bits;
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
jr2_a0_dnx_data_sch_dbal_ps_shaper_quanta_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_ps_shaper_quanta_bits;
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
jr2_a0_dnx_data_sch_dbal_ps_shaper_max_burst_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_ps_shaper_max_burst_bits;
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
jr2_a0_dnx_data_sch_dbal_drm_nof_links_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_drm_nof_links_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_fabric.links.nof_links_get(unit);

    
    define->data = dnx_data_fabric.links.nof_links_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_dbal_scheduler_enable_key_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_scheduler_enable_key_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.dbal.se_bits_get(unit) - 3;

    
    define->data = dnx_data_sch.dbal.se_bits_get(unit) - 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_dbal_flow_id_pair_key_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_flow_id_pair_key_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.dbal.flow_bits_get(unit) - 1;

    
    define->data = dnx_data_sch.dbal.flow_bits_get(unit) - 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_dbal_flow_id_pair_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_flow_id_pair_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.flow.nof_flows_get(unit)/2 - 1;

    
    define->data = dnx_data_sch.flow.nof_flows_get(unit)/2 - 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_dbal_ps_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_ps_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.dbal.hr_bits_get(unit) - 3;

    
    define->data = dnx_data_sch.dbal.hr_bits_get(unit) - 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_dbal_flow_shaper_descr_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_flow_shaper_descr_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.dbal.flow_shaper_mant_bits_get(unit) + 4 + 9 + 1;

    
    define->data = dnx_data_sch.dbal.flow_shaper_mant_bits_get(unit) + 4 + 9 + 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_dbal_ps_shaper_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_ps_shaper_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.dbal.ps_shaper_quanta_bits_get(unit) +dnx_data_sch.dbal.ps_shaper_max_burst_bits_get(unit);

    
    define->data = dnx_data_sch.dbal.ps_shaper_quanta_bits_get(unit) +dnx_data_sch.dbal.ps_shaper_max_burst_bits_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sch_dbal_cl_profile_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_cl_profile_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = utilex_log2_round_up(dnx_data_sch.se.cl_class_profile_nof_get(unit));

    
    define->data = utilex_log2_round_up(dnx_data_sch.se.cl_class_profile_nof_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_sch_features_dlm_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_features;
    int feature_index = dnx_data_sch_features_dlm;
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
jr2_a0_data_sch_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_sch;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_sch_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sch_general_define_nof_slow_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_general_nof_slow_profiles_set;
    data_index = dnx_data_sch_general_define_nof_slow_levels;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_general_nof_slow_levels_set;
    data_index = dnx_data_sch_general_define_cycles_per_credit_token;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_general_cycles_per_credit_token_set;
    data_index = dnx_data_sch_general_define_nof_fmq_class;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_general_nof_fmq_class_set;
    data_index = dnx_data_sch_general_define_port_priority_propagation_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_general_port_priority_propagation_enable_set;

    
    data_index = dnx_data_sch_general_lag_scheduler_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_sch_general_lag_scheduler_supported_set;
    data_index = dnx_data_sch_general_fsm_modes_config_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_sch_general_fsm_modes_config_supported_set;

    
    data_index = dnx_data_sch_general_table_low_rate_factor_to_dbal_enum;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_sch_general_low_rate_factor_to_dbal_enum_set;
    data_index = dnx_data_sch_general_table_dbal_enum_to_low_rate_factor;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_sch_general_dbal_enum_to_low_rate_factor_set;
    data_index = dnx_data_sch_general_table_slow_rate_max_bucket;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_sch_general_slow_rate_max_bucket_set;
    
    submodule_index = dnx_data_sch_submodule_ps;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sch_ps_define_min_priority_for_tcg;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_ps_min_priority_for_tcg_set;
    data_index = dnx_data_sch_ps_define_max_nof_tcg;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_ps_max_nof_tcg_set;
    data_index = dnx_data_sch_ps_define_nof_hrs_in_ps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_ps_nof_hrs_in_ps_set;
    data_index = dnx_data_sch_ps_define_max_tcg_weight;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_ps_max_tcg_weight_set;
    data_index = dnx_data_sch_ps_define_tc_tcg_calendar_access_period;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_ps_tc_tcg_calendar_access_period_set;
    data_index = dnx_data_sch_ps_define_max_port_rate_mbps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_ps_max_port_rate_mbps_set;
    data_index = dnx_data_sch_ps_define_max_burst;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_ps_max_burst_set;

    

    
    
    submodule_index = dnx_data_sch_submodule_flow;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sch_flow_define_nof_hr;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_nof_hr_set;
    data_index = dnx_data_sch_flow_define_nof_cl;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_nof_cl_set;
    data_index = dnx_data_sch_flow_define_nof_se;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_nof_se_set;
    data_index = dnx_data_sch_flow_define_nof_flows;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_nof_flows_set;
    data_index = dnx_data_sch_flow_define_region_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_region_size_set;
    data_index = dnx_data_sch_flow_define_nof_regions;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_nof_regions_set;
    data_index = dnx_data_sch_flow_define_nof_hr_regions;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_nof_hr_regions_set;
    data_index = dnx_data_sch_flow_define_nof_connectors_only_regions;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_nof_connectors_only_regions_set;
    data_index = dnx_data_sch_flow_define_flow_in_region_mask;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_flow_in_region_mask_set;
    data_index = dnx_data_sch_flow_define_min_connector_bundle_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_min_connector_bundle_size_set;
    data_index = dnx_data_sch_flow_define_conn_idx_interdig_flow;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_conn_idx_interdig_flow_set;
    data_index = dnx_data_sch_flow_define_shaper_flipflops_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_shaper_flipflops_nof_set;
    data_index = dnx_data_sch_flow_define_erp_hr;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_erp_hr_set;
    data_index = dnx_data_sch_flow_define_reserved_hr;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_reserved_hr_set;
    data_index = dnx_data_sch_flow_define_max_se;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_max_se_set;
    data_index = dnx_data_sch_flow_define_max_flow;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_max_flow_set;
    data_index = dnx_data_sch_flow_define_first_hr_region;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_first_hr_region_set;
    data_index = dnx_data_sch_flow_define_first_se_flow_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_first_se_flow_id_set;
    data_index = dnx_data_sch_flow_define_hr_se_id_min;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_hr_se_id_min_set;
    data_index = dnx_data_sch_flow_define_hr_se_id_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_hr_se_id_max_set;
    data_index = dnx_data_sch_flow_define_default_credit_source_se_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_default_credit_source_se_id_set;
    data_index = dnx_data_sch_flow_define_runtime_performance_optimize_enable_sched_allocation;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_flow_runtime_performance_optimize_enable_sched_allocation_set;

    

    
    data_index = dnx_data_sch_flow_table_region_type;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_sch_flow_region_type_set;
    data_index = dnx_data_sch_flow_table_nof_remote_cores;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_sch_flow_nof_remote_cores_set;
    data_index = dnx_data_sch_flow_table_cl_num_to_quartet_offset;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_sch_flow_cl_num_to_quartet_offset_set;
    data_index = dnx_data_sch_flow_table_quartet_offset_to_cl_num;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_sch_flow_quartet_offset_to_cl_num_set;
    
    submodule_index = dnx_data_sch_submodule_se;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sch_se_define_default_cos;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_se_default_cos_set;
    data_index = dnx_data_sch_se_define_nof_color_group;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_se_nof_color_group_set;
    data_index = dnx_data_sch_se_define_max_hr_weight;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_se_max_hr_weight_set;
    data_index = dnx_data_sch_se_define_cl_class_profile_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_se_cl_class_profile_nof_set;

    

    
    
    submodule_index = dnx_data_sch_submodule_interface;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sch_interface_define_nof_big_calendars;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_interface_nof_big_calendars_set;
    data_index = dnx_data_sch_interface_define_nof_channelized_calendars;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_interface_nof_channelized_calendars_set;
    data_index = dnx_data_sch_interface_define_big_calendar_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_interface_big_calendar_size_set;
    data_index = dnx_data_sch_interface_define_nof_sch_interfaces;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_interface_nof_sch_interfaces_set;
    data_index = dnx_data_sch_interface_define_cal_speed_resolution;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_interface_cal_speed_resolution_set;
    data_index = dnx_data_sch_interface_define_reserved;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_interface_reserved_set;
    data_index = dnx_data_sch_interface_define_max_if_rate_mbps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_interface_max_if_rate_mbps_set;

    

    
    data_index = dnx_data_sch_interface_table_shaper_resolution;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_sch_interface_shaper_resolution_set;
    
    submodule_index = dnx_data_sch_submodule_device;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sch_device_define_drm_resolution;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_device_drm_resolution_set;

    
    data_index = dnx_data_sch_device_shared_drm;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_sch_device_shared_drm_set;

    
    
    submodule_index = dnx_data_sch_submodule_sch_alloc;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sch_sch_alloc_define_tag_aggregate_se_2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_sch_alloc_tag_aggregate_se_2_set;
    data_index = dnx_data_sch_sch_alloc_define_tag_aggregate_se_4;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_sch_alloc_tag_aggregate_se_4_set;
    data_index = dnx_data_sch_sch_alloc_define_tag_aggregate_se_8;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_sch_alloc_tag_aggregate_se_8_set;
    data_index = dnx_data_sch_sch_alloc_define_shared_shaper_max_tag_value;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_sch_alloc_shared_shaper_max_tag_value_set;
    data_index = dnx_data_sch_sch_alloc_define_tag_size_aggregate_se;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_sch_alloc_tag_size_aggregate_se_set;
    data_index = dnx_data_sch_sch_alloc_define_tag_size_con;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_sch_alloc_tag_size_con_set;
    data_index = dnx_data_sch_sch_alloc_define_type_con_reg_start;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_sch_alloc_type_con_reg_start_set;
    data_index = dnx_data_sch_sch_alloc_define_alloc_invalid_flow;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_sch_alloc_alloc_invalid_flow_set;
    data_index = dnx_data_sch_sch_alloc_define_dealloc_flow_ids;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_sch_alloc_dealloc_flow_ids_set;
    data_index = dnx_data_sch_sch_alloc_define_type_con_reg_end;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_sch_alloc_type_con_reg_end_set;
    data_index = dnx_data_sch_sch_alloc_define_type_hr_reg_start;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_sch_alloc_type_hr_reg_start_set;

    

    
    data_index = dnx_data_sch_sch_alloc_table_region;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_sch_sch_alloc_region_set;
    data_index = dnx_data_sch_sch_alloc_table_connector;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_sch_sch_alloc_connector_set;
    data_index = dnx_data_sch_sch_alloc_table_se_per_region_type;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_sch_sch_alloc_se_per_region_type_set;
    data_index = dnx_data_sch_sch_alloc_table_se;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_sch_sch_alloc_se_set;
    data_index = dnx_data_sch_sch_alloc_table_composite_se_per_region_type;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_sch_sch_alloc_composite_se_per_region_type_set;
    data_index = dnx_data_sch_sch_alloc_table_composite_se;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_sch_sch_alloc_composite_se_set;
    
    submodule_index = dnx_data_sch_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sch_dbal_define_scheduler_enable_grouping_factor;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_dbal_scheduler_enable_grouping_factor_set;
    data_index = dnx_data_sch_dbal_define_scheduler_enable_dbal_mult_factor;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_dbal_scheduler_enable_dbal_mult_factor_set;
    data_index = dnx_data_sch_dbal_define_flow_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_dbal_flow_bits_set;
    data_index = dnx_data_sch_dbal_define_se_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_dbal_se_bits_set;
    data_index = dnx_data_sch_dbal_define_cl_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_dbal_cl_bits_set;
    data_index = dnx_data_sch_dbal_define_hr_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_dbal_hr_bits_set;
    data_index = dnx_data_sch_dbal_define_interface_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_dbal_interface_bits_set;
    data_index = dnx_data_sch_dbal_define_calendar_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_dbal_calendar_bits_set;
    data_index = dnx_data_sch_dbal_define_flow_shaper_mant_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_dbal_flow_shaper_mant_bits_set;
    data_index = dnx_data_sch_dbal_define_ps_shaper_quanta_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_dbal_ps_shaper_quanta_bits_set;
    data_index = dnx_data_sch_dbal_define_ps_shaper_max_burst_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_dbal_ps_shaper_max_burst_bits_set;
    data_index = dnx_data_sch_dbal_define_drm_nof_links_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_dbal_drm_nof_links_max_set;
    data_index = dnx_data_sch_dbal_define_scheduler_enable_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_dbal_scheduler_enable_key_size_set;
    data_index = dnx_data_sch_dbal_define_flow_id_pair_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_dbal_flow_id_pair_key_size_set;
    data_index = dnx_data_sch_dbal_define_flow_id_pair_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_dbal_flow_id_pair_max_set;
    data_index = dnx_data_sch_dbal_define_ps_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_dbal_ps_bits_set;
    data_index = dnx_data_sch_dbal_define_flow_shaper_descr_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_dbal_flow_shaper_descr_bits_set;
    data_index = dnx_data_sch_dbal_define_ps_shaper_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_dbal_ps_shaper_bits_set;
    data_index = dnx_data_sch_dbal_define_cl_profile_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sch_dbal_cl_profile_bits_set;

    

    
    
    submodule_index = dnx_data_sch_submodule_features;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_sch_features_dlm;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_sch_features_dlm_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

