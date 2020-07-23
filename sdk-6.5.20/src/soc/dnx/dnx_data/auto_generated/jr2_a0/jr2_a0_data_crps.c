

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_crps.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
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

    
    table->values[0].default_val = "4096";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_engine_engines_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_crps_engine_table_engines_info");

    
    default_data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nof_counters = 4096;
    default_data->meter_shared = 0;
    
    for (engine_id_index = 0; engine_id_index < table->keys[0].size; engine_id_index++)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, engine_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->nof_counters = 4096;
        data->meter_shared = 1;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->nof_counters = 4096;
        data->meter_shared = 1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->nof_counters = 4096;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->nof_counters = 4096;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->nof_counters = 4096;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->nof_counters = 4096;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->nof_counters = 4096;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->nof_counters = 4096;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->nof_counters = 8192;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->nof_counters = 8192;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->nof_counters = 8192;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->nof_counters = 8192;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->nof_counters = 8192;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->nof_counters = 8192;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->nof_counters = 8192;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->nof_counters = 8192;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->nof_counters = 16384;
        data->meter_shared = 1;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->nof_counters = 16384;
        data->meter_shared = 1;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->nof_counters = 16384;
        data->meter_shared = 1;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->nof_counters = 16384;
        data->meter_shared = 1;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->nof_counters = 16384;
        data->meter_shared = 1;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->nof_counters = 16384;
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

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_engine_source_base_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_crps_engine_table_source_base");

    
    default_data = (dnx_data_crps_engine_source_base_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->base_val = 0;
    
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
jr2_a0_dnx_data_crps_eviction_packet_and_bytes_packet_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int define_index = dnx_data_crps_eviction_define_packet_and_bytes_packet_size;
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
        "enable/disable counters bg thread operation. possible values: counter_engine_sampling_interval=0/1\n"
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

    
    table->values[0].default_val = "8";
    table->values[1].default_val = "104";
    table->values[2].default_val = "8";
    table->values[3].default_val = "96";
    table->values[4].default_val = "2";
    table->values[5].default_val = "94";
    table->values[6].default_val = "1";
    table->values[7].default_val = "93";
    table->values[8].default_val = "3";
    table->values[9].default_val = "90";
    table->values[10].default_val = "1";
    table->values[11].default_val = "89";
    table->values[12].default_val = "6";
    table->values[13].default_val = "83";
    table->values[14].default_val = "15";
    table->values[15].default_val = "68";
    table->values[16].default_val = "68";
    table->values[17].default_val = "0";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_eviction_phy_record_format_t, (1 + 1  ), "data of dnx_data_crps_eviction_table_phy_record_format");

    
    default_data = (dnx_data_crps_eviction_phy_record_format_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
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
jr2_a0_dnx_data_crps_expansion_irpp_types_set(
    int unit)
{
    dnx_data_crps_expansion_irpp_types_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_expansion;
    int table_index = dnx_data_crps_expansion_table_irpp_types;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "bcmStatExpansionTypeTmDropReason, bcmStatExpansionTypeDispositionIsDrop,                   bcmStatExpansionTypeTrafficClass, bcmStatExpansionTypeDropPrecedenceMeter0Valid,                   bcmStatExpansionTypeDropPrecedenceMeter0Value, bcmStatExpansionTypeDropPrecedenceMeter1Valid,                   bcmStatExpansionTypDropPrecedenceMeter1Value, bcmStatExpansionTypeDropPrecedenceMeter2Valid,                   bcmStatExpansionTypeDropPrecedenceMeter2Value, bcmStatExpansionTypeDropPrecedenceMeterResolved,                   bcmStatExpansionTypeMetaData, bcmStatExpansionTypeMax";
    table->values[1].default_val = "36";
    table->values[2].default_val = "9";
    table->values[3].default_val = "13";
    table->values[4].default_val = "16";
    table->values[5].default_val = "8";
    table->values[6].default_val = "6";
    table->values[7].default_val = "5";
    table->values[8].default_val = "3";
    table->values[9].default_val = "2";
    table->values[10].default_val = "0";
    table->values[11].default_val = "14";
    table->values[12].default_val = "12";
    table->values[13].default_val = "19";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_expansion_irpp_types_t, (1 + 1  ), "data of dnx_data_crps_expansion_table_irpp_types");

    
    default_data = (dnx_data_crps_expansion_irpp_types_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid_types[0] = bcmStatExpansionTypeTmDropReason;
    default_data->valid_types[1] = bcmStatExpansionTypeDispositionIsDrop;
    default_data->valid_types[2] = bcmStatExpansionTypeTrafficClass;
    default_data->valid_types[3] = bcmStatExpansionTypeDropPrecedenceMeter0Valid;
    default_data->valid_types[4] = bcmStatExpansionTypeDropPrecedenceMeter0Value;
    default_data->valid_types[5] = bcmStatExpansionTypeDropPrecedenceMeter1Valid;
    default_data->valid_types[6] = bcmStatExpansionTypDropPrecedenceMeter1Value;
    default_data->valid_types[7] = bcmStatExpansionTypeDropPrecedenceMeter2Valid;
    default_data->valid_types[8] = bcmStatExpansionTypeDropPrecedenceMeter2Value;
    default_data->valid_types[9] = bcmStatExpansionTypeDropPrecedenceMeterResolved;
    default_data->valid_types[10] = bcmStatExpansionTypeMetaData;
    default_data->valid_types[11] = bcmStatExpansionTypeMax;
    default_data->total_nof_bits = 36;
    default_data->drop_reason_offset = 9;
    default_data->disposition_offset = 13;
    default_data->tc_offset = 16;
    default_data->dp0_valid_offset = 8;
    default_data->dp0_value_offset = 6;
    default_data->dp1_valid_offset = 5;
    default_data->dp1_value_offset = 3;
    default_data->dp2_valid_offset = 2;
    default_data->dp2_value_offset = 0;
    default_data->final_dp_offset = 14;
    default_data->reject_all_copies_offset = 12;
    default_data->metadata_offset = 19;
    
    data = (dnx_data_crps_expansion_irpp_types_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_crps_expansion_itm_types_set(
    int unit)
{
    dnx_data_crps_expansion_itm_types_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_expansion;
    int table_index = dnx_data_crps_expansion_table_itm_types;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "bcmStatExpansionTypeTmDropReason, bcmStatExpansionTypeDispositionIsDrop, bcmStatExpansionTypeTrafficClass,                  bcmStatExpansionTypeDropPrecedenceMeterResolved, bcmStatExpansionTypeMax";
    table->values[1].default_val = "9";
    table->values[2].default_val = "0";
    table->values[3].default_val = "3";
    table->values[4].default_val = "4";
    table->values[5].default_val = "7";
    table->values[6].default_val = "9";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_expansion_itm_types_t, (1 + 1  ), "data of dnx_data_crps_expansion_table_itm_types");

    
    default_data = (dnx_data_crps_expansion_itm_types_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid_types[0] = bcmStatExpansionTypeTmDropReason;
    default_data->valid_types[1] = bcmStatExpansionTypeDispositionIsDrop;
    default_data->valid_types[2] = bcmStatExpansionTypeTrafficClass;
    default_data->valid_types[3] = bcmStatExpansionTypeDropPrecedenceMeterResolved;
    default_data->valid_types[4] = bcmStatExpansionTypeMax;
    default_data->total_nof_bits = 9;
    default_data->drop_reason_offset = 0;
    default_data->disposition_offset = 3;
    default_data->tc_offset = 4;
    default_data->final_dp_offset = 7;
    default_data->last_copy_offset = 9;
    
    data = (dnx_data_crps_expansion_itm_types_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_crps_expansion_itpp_types_set(
    int unit)
{
    dnx_data_crps_expansion_itpp_types_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_expansion;
    int table_index = dnx_data_crps_expansion_table_itpp_types;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "bcmStatExpansionTypeLatencyBin, bcmStatExpansionTypeTrafficClass, bcmStatExpansionTypeLatencyFlowProfile, bcmStatExpansionTypeMax";
    table->values[1].default_val = "11";
    table->values[2].default_val = "0";
    table->values[3].default_val = "3";
    table->values[4].default_val = "6";
    table->values[5].default_val = "11";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_expansion_itpp_types_t, (1 + 1  ), "data of dnx_data_crps_expansion_table_itpp_types");

    
    default_data = (dnx_data_crps_expansion_itpp_types_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid_types[0] = bcmStatExpansionTypeLatencyBin;
    default_data->valid_types[1] = bcmStatExpansionTypeTrafficClass;
    default_data->valid_types[2] = bcmStatExpansionTypeLatencyFlowProfile;
    default_data->valid_types[3] = bcmStatExpansionTypeMax;
    default_data->total_nof_bits = 11;
    default_data->latency_bin_offset = 0;
    default_data->tc_offset = 3;
    default_data->latency_profile_offset = 6;
    default_data->mc_offset = 11;
    
    data = (dnx_data_crps_expansion_itpp_types_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_crps_expansion_erpp_types_set(
    int unit)
{
    dnx_data_crps_expansion_erpp_types_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_expansion;
    int table_index = dnx_data_crps_expansion_table_erpp_types;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "bcmStatExpansionTypeMetaData, bcmStatExpansionTypeDropPrecedenceMeterResolved,                  bcmStatExpansionTypeTrafficClass, bcmStatExpansionTypeSystemMultiCast, bcmStatExpansionTypeEgressTmActionType,                  bcmStatExpansionTypePpDropReason, bcmStatExpansionTypeMax";
    table->values[1].default_val = "16";
    table->values[2].default_val = "0";
    table->values[3].default_val = "4";
    table->values[4].default_val = "6";
    table->values[5].default_val = "9";
    table->values[6].default_val = "10";
    table->values[7].default_val = "12";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_expansion_erpp_types_t, (1 + 1  ), "data of dnx_data_crps_expansion_table_erpp_types");

    
    default_data = (dnx_data_crps_expansion_erpp_types_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid_types[0] = bcmStatExpansionTypeMetaData;
    default_data->valid_types[1] = bcmStatExpansionTypeDropPrecedenceMeterResolved;
    default_data->valid_types[2] = bcmStatExpansionTypeTrafficClass;
    default_data->valid_types[3] = bcmStatExpansionTypeSystemMultiCast;
    default_data->valid_types[4] = bcmStatExpansionTypeEgressTmActionType;
    default_data->valid_types[5] = bcmStatExpansionTypePpDropReason;
    default_data->valid_types[6] = bcmStatExpansionTypeMax;
    default_data->total_nof_bits = 16;
    default_data->metadata_offset = 0;
    default_data->dp_offset = 4;
    default_data->tc_offset = 6;
    default_data->mc_offset = 9;
    default_data->tm_action_type_offset = 10;
    default_data->pp_drop_reason_offset = 12;
    
    data = (dnx_data_crps_expansion_erpp_types_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_crps_expansion_etm_types_set(
    int unit)
{
    dnx_data_crps_expansion_etm_types_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_expansion;
    int table_index = dnx_data_crps_expansion_table_etm_types;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "bcmStatExpansionTypeDropPrecedenceMeterResolved, bcmStatExpansionTypeTrafficClass,                  bcmStatExpansionTypeDiscardPp, bcmStatExpansionTypeDiscardTm,                 bcmStatExpansionTypeTmDropReason, bcmStatExpansionTypeMax";
    table->values[1].default_val = "10";
    table->values[2].default_val = "7";
    table->values[3].default_val = "1";
    table->values[4].default_val = "0";
    table->values[5].default_val = "4";
    table->values[6].default_val = "2";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_expansion_etm_types_t, (1 + 1  ), "data of dnx_data_crps_expansion_table_etm_types");

    
    default_data = (dnx_data_crps_expansion_etm_types_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid_types[0] = bcmStatExpansionTypeDropPrecedenceMeterResolved;
    default_data->valid_types[1] = bcmStatExpansionTypeTrafficClass;
    default_data->valid_types[2] = bcmStatExpansionTypeDiscardPp;
    default_data->valid_types[3] = bcmStatExpansionTypeDiscardTm;
    default_data->valid_types[4] = bcmStatExpansionTypeTmDropReason;
    default_data->valid_types[5] = bcmStatExpansionTypeMax;
    default_data->total_nof_bits = 10;
    default_data->drop_reason_offset = 7;
    default_data->tm_discard_offset = 1;
    default_data->pp_discard_offset = 0;
    default_data->tc_offset = 4;
    default_data->dp_offset = 2;
    
    data = (dnx_data_crps_expansion_etm_types_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_crps_expansion_etpp_types_set(
    int unit)
{
    dnx_data_crps_expansion_etpp_types_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_expansion;
    int table_index = dnx_data_crps_expansion_table_etpp_types;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "bcmStatExpansionTypeDropPrecedenceMeterResolved, bcmStatExpansionTypeTrafficClass,                  bcmStatExpansionTypeSystemMultiCast, bcmStatExpansionTypeEcnEligibleAndCni, bcmStatExpansionTypePortMetaData,                  bcmStatExpansionTypePpDropReason, bcmStatExpansionTypeMetaData, bcmStatExpansionTypeMax";
    table->values[1].default_val = "24";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_expansion_etpp_types_t, (1 + 1  ), "data of dnx_data_crps_expansion_table_etpp_types");

    
    default_data = (dnx_data_crps_expansion_etpp_types_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid_types[0] = bcmStatExpansionTypeDropPrecedenceMeterResolved;
    default_data->valid_types[1] = bcmStatExpansionTypeTrafficClass;
    default_data->valid_types[2] = bcmStatExpansionTypeSystemMultiCast;
    default_data->valid_types[3] = bcmStatExpansionTypeEcnEligibleAndCni;
    default_data->valid_types[4] = bcmStatExpansionTypePortMetaData;
    default_data->valid_types[5] = bcmStatExpansionTypePpDropReason;
    default_data->valid_types[6] = bcmStatExpansionTypeMetaData;
    default_data->valid_types[7] = bcmStatExpansionTypeMax;
    default_data->total_nof_bits = 24;
    
    data = (dnx_data_crps_expansion_etpp_types_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


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
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_src_interface_command_id_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_crps_src_interface_table_command_id");

    
    default_data = (dnx_data_crps_src_interface_command_id_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->size = 0;
    default_data->base = 0;
    
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
    data_index = dnx_data_crps_engine_define_last_address_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_engine_last_address_nof_bits_set;

    
    data_index = dnx_data_crps_engine_valid_address_admission_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_engine_valid_address_admission_is_supported_set;
    data_index = dnx_data_crps_engine_last_input_command_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_engine_last_input_command_is_supported_set;

    
    data_index = dnx_data_crps_engine_table_engines_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_engine_engines_info_set;
    data_index = dnx_data_crps_engine_table_source_base;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_engine_source_base_set;
    
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
    data_index = dnx_data_crps_eviction_define_packet_and_bytes_packet_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_eviction_packet_and_bytes_packet_size_set;
    data_index = dnx_data_crps_eviction_define_bubble_before_request_timeout;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_eviction_bubble_before_request_timeout_set;
    data_index = dnx_data_crps_eviction_define_bubble_after_request_timeout;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_eviction_bubble_after_request_timeout_set;
    data_index = dnx_data_crps_eviction_define_seq_address_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_eviction_seq_address_nof_bits_set;
    data_index = dnx_data_crps_eviction_define_bg_thread_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_eviction_bg_thread_enable_set;

    
    data_index = dnx_data_crps_eviction_algorithmic_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_eviction_algorithmic_is_supported_set;
    data_index = dnx_data_crps_eviction_probabilistic_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_eviction_probabilistic_is_supported_set;
    data_index = dnx_data_crps_eviction_conditional_action_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_crps_eviction_conditional_action_is_supported_set;

    
    data_index = dnx_data_crps_eviction_table_phy_record_format;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_eviction_phy_record_format_set;
    data_index = dnx_data_crps_eviction_table_condional_action_valid;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_eviction_condional_action_valid_set;
    
    submodule_index = dnx_data_crps_submodule_expansion;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_crps_expansion_define_expansion_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_crps_expansion_expansion_size_set;

    

    
    data_index = dnx_data_crps_expansion_table_irpp_types;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_expansion_irpp_types_set;
    data_index = dnx_data_crps_expansion_table_itm_types;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_expansion_itm_types_set;
    data_index = dnx_data_crps_expansion_table_itpp_types;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_expansion_itpp_types_set;
    data_index = dnx_data_crps_expansion_table_erpp_types;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_expansion_erpp_types_set;
    data_index = dnx_data_crps_expansion_table_etm_types;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_expansion_etm_types_set;
    data_index = dnx_data_crps_expansion_table_etpp_types;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_expansion_etpp_types_set;
    
    submodule_index = dnx_data_crps_submodule_src_interface;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_crps_src_interface_table_command_id;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_crps_src_interface_command_id_set;
    
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

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

