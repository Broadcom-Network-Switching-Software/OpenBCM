

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







static shr_error_e
j2c_a0_dnx_data_crps_engine_valid_address_admission_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int feature_index = dnx_data_crps_engine_valid_address_admission_is_supported;
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
j2c_a0_dnx_data_crps_engine_last_input_command_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int feature_index = dnx_data_crps_engine_last_input_command_is_supported;
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
j2c_a0_dnx_data_crps_engine_nof_engines_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int define_index = dnx_data_crps_engine_define_nof_engines;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 34;

    
    define->data = 34;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_crps_engine_nof_mid_engines_set(
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
j2c_a0_dnx_data_crps_engine_nof_big_engines_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int define_index = dnx_data_crps_engine_define_nof_big_engines;
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
j2c_a0_dnx_data_crps_engine_nof_databases_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int define_index = dnx_data_crps_engine_define_nof_databases;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 34;

    
    define->data = 34;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2c_a0_dnx_data_crps_engine_engines_info_set(
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

    
    table->keys[0].size = 34;
    table->info_get.key_size[0] = 34;

    
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
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->nof_counters = 16384;
        data->meter_shared = 1;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->nof_counters = 16384;
        data->meter_shared = 1;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->nof_counters = 16384;
        data->meter_shared = 1;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->nof_counters = 16384;
        data->meter_shared = 1;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->nof_counters = 16384;
        data->meter_shared = 1;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->nof_counters = 16384;
        data->meter_shared = 1;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->nof_counters = 16384;
        data->meter_shared = 1;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->nof_counters = 16384;
        data->meter_shared = 1;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->nof_counters = 16384;
        data->meter_shared = 1;
    }
    if (31 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->nof_counters = 16384;
        data->meter_shared = 1;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->nof_counters = 16384;
        data->meter_shared = 1;
    }
    if (33 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, 0);
        data->nof_counters = 16384;
        data->meter_shared = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2c_a0_dnx_data_crps_eviction_conditional_action_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int feature_index = dnx_data_crps_eviction_conditional_action_is_supported;
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
j2c_a0_dnx_data_crps_eviction_seq_address_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_eviction;
    int define_index = dnx_data_crps_eviction_define_seq_address_nof_bits;
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
j2c_a0_dnx_data_crps_eviction_condional_action_valid_set(
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
        data->valid = 1;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_crps_eviction_condional_action_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->valid = 1;
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
j2c_a0_dnx_data_crps_expansion_itm_types_set(
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

    
    table->values[0].default_val = "bcmStatExpansionTypeTmDropReason, bcmStatExpansionTypeDispositionIsDrop, bcmStatExpansionTypeTrafficClass,                  bcmStatExpansionTypeDropPrecedenceMeterResolved, bcmStatExpansionTypeTmLastCopy, bcmStatExpansionTypeMax";
    table->values[1].default_val = "10";
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
    default_data->valid_types[4] = bcmStatExpansionTypeTmLastCopy;
    default_data->valid_types[5] = bcmStatExpansionTypeMax;
    default_data->total_nof_bits = 10;
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
j2c_a0_dnx_data_crps_expansion_itpp_types_set(
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

    
    table->values[0].default_val = "bcmStatExpansionTypeLatencyBin, bcmStatExpansionTypeTrafficClass, bcmStatExpansionTypeLatencyFlowProfile,                  bcmStatExpansionTypeSystemMultiCast, bcmStatExpansionTypeMax";
    table->values[1].default_val = "12";
    table->values[2].default_val = "0";
    table->values[3].default_val = "3";
    table->values[4].default_val = "6";
    table->values[5].default_val = "11";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_expansion_itpp_types_t, (1 + 1  ), "data of dnx_data_crps_expansion_table_itpp_types");

    
    default_data = (dnx_data_crps_expansion_itpp_types_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid_types[0] = bcmStatExpansionTypeLatencyBin;
    default_data->valid_types[1] = bcmStatExpansionTypeTrafficClass;
    default_data->valid_types[2] = bcmStatExpansionTypeLatencyFlowProfile;
    default_data->valid_types[3] = bcmStatExpansionTypeSystemMultiCast;
    default_data->valid_types[4] = bcmStatExpansionTypeMax;
    default_data->total_nof_bits = 12;
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



shr_error_e
j2c_a0_data_crps_attach(
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
    define->set = j2c_a0_dnx_data_crps_engine_nof_engines_set;
    data_index = dnx_data_crps_engine_define_nof_mid_engines;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_crps_engine_nof_mid_engines_set;
    data_index = dnx_data_crps_engine_define_nof_big_engines;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_crps_engine_nof_big_engines_set;
    data_index = dnx_data_crps_engine_define_nof_databases;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_crps_engine_nof_databases_set;

    
    data_index = dnx_data_crps_engine_valid_address_admission_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_crps_engine_valid_address_admission_is_supported_set;
    data_index = dnx_data_crps_engine_last_input_command_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_crps_engine_last_input_command_is_supported_set;

    
    data_index = dnx_data_crps_engine_table_engines_info;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_crps_engine_engines_info_set;
    
    submodule_index = dnx_data_crps_submodule_eviction;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_crps_eviction_define_seq_address_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_crps_eviction_seq_address_nof_bits_set;

    
    data_index = dnx_data_crps_eviction_conditional_action_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_crps_eviction_conditional_action_is_supported_set;

    
    data_index = dnx_data_crps_eviction_table_condional_action_valid;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_crps_eviction_condional_action_valid_set;
    
    submodule_index = dnx_data_crps_submodule_expansion;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_crps_expansion_table_itm_types;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_crps_expansion_itm_types_set;
    data_index = dnx_data_crps_expansion_table_itpp_types;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_crps_expansion_itpp_types_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

