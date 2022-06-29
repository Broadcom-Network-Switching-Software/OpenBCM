
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







static shr_error_e
j2x_a0_dnx_data_crps_engine_valid_address_admission_is_supported_set(
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
j2x_a0_dnx_data_crps_engine_last_input_command_is_supported_set(
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
j2x_a0_dnx_data_crps_engine_nof_engines_set(
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
j2x_a0_dnx_data_crps_engine_nof_mid_engines_set(
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
j2x_a0_dnx_data_crps_engine_nof_big_engines_set(
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
j2x_a0_dnx_data_crps_engine_nof_databases_set(
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
j2x_a0_dnx_data_crps_engine_engines_info_set(
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
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->nof_entries = 8192;
        data->meter_shared = 1;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->nof_entries = 8192;
        data->meter_shared = 1;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->nof_entries = 8192;
        data->meter_shared = 1;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->nof_entries = 8192;
        data->meter_shared = 1;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->nof_entries = 8192;
        data->meter_shared = 1;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->nof_entries = 8192;
        data->meter_shared = 1;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->nof_entries = 8192;
        data->meter_shared = 1;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->nof_entries = 8192;
        data->meter_shared = 1;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->nof_entries = 8192;
        data->meter_shared = 1;
    }
    if (31 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->nof_entries = 8192;
        data->meter_shared = 1;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->nof_entries = 8192;
        data->meter_shared = 1;
    }
    if (33 < table->keys[0].size)
    {
        data = (dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, 0);
        data->nof_entries = 8192;
        data->meter_shared = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2x_a0_dnx_data_crps_eviction_conditional_action_is_supported_set(
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
j2x_a0_dnx_data_crps_eviction_seq_address_nof_bits_set(
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
j2x_a0_dnx_data_crps_eviction_condional_action_valid_set(
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
j2x_a0_dnx_data_crps_expansion_source_metadata_types_set(
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
    if (bcmStatCounterInterfaceIngressTransmitPp < table->keys[0].size && bcmStatExpansionTypeSystemMultiCast < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressTransmitPp, bcmStatExpansionTypeSystemMultiCast);
        data->offset = 11;
        data->nof_bits = 1;
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
    if (bcmStatCounterInterfaceIngressVoq < table->keys[0].size && bcmStatExpansionTypeTmLastCopy < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressVoq, bcmStatExpansionTypeTmLastCopy);
        data->offset = 9;
        data->nof_bits = 1;
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
    if (bcmStatCounterInterfaceEgressReceiveQueue < table->keys[0].size &&  bcmStatExpansionTypeDiscardTm < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceiveQueue,  bcmStatExpansionTypeDiscardTm);
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
    if (bcmStatCounterInterfaceEgressTransmitPp < table->keys[0].size && bcmStatExpansionTypePortRateBin < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressTransmitPp, bcmStatExpansionTypePortRateBin);
        data->nof_bits = 3;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_crps_expansion_source_expansion_info_set(
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
        data->expansion_size = 12;
        data->supports_expansion_per_type = 0;
    }
    if (bcmStatCounterInterfaceIngressVoq < table->keys[0].size)
    {
        data = (dnx_data_crps_expansion_source_expansion_info_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressVoq, 0);
        data->expansion_size = 10;
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
j2x_a0_dnx_data_crps_general_is_used_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_general;
    int feature_index = dnx_data_crps_general_is_used;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = dnx_data_nif.global.l1_only_mode_get(unit) == DNX_PORT_NIF_L1_ONLY_MODE_DISABLED;

    
    feature->data = dnx_data_nif.global.l1_only_mode_get(unit) == DNX_PORT_NIF_L1_ONLY_MODE_DISABLED;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_crps_general_power_down_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_general;
    int feature_index = dnx_data_crps_general_power_down_supported;
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
j2x_a0_data_crps_attach(
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
    define->set = j2x_a0_dnx_data_crps_engine_nof_engines_set;
    data_index = dnx_data_crps_engine_define_nof_mid_engines;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_crps_engine_nof_mid_engines_set;
    data_index = dnx_data_crps_engine_define_nof_big_engines;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_crps_engine_nof_big_engines_set;
    data_index = dnx_data_crps_engine_define_nof_databases;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_crps_engine_nof_databases_set;

    
    data_index = dnx_data_crps_engine_valid_address_admission_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_crps_engine_valid_address_admission_is_supported_set;
    data_index = dnx_data_crps_engine_last_input_command_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_crps_engine_last_input_command_is_supported_set;

    
    data_index = dnx_data_crps_engine_table_engines_info;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_crps_engine_engines_info_set;
    
    submodule_index = dnx_data_crps_submodule_eviction;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_crps_eviction_define_seq_address_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_crps_eviction_seq_address_nof_bits_set;

    
    data_index = dnx_data_crps_eviction_conditional_action_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_crps_eviction_conditional_action_is_supported_set;

    
    data_index = dnx_data_crps_eviction_table_condional_action_valid;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_crps_eviction_condional_action_valid_set;
    
    submodule_index = dnx_data_crps_submodule_expansion;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_crps_expansion_table_source_metadata_types;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_crps_expansion_source_metadata_types_set;
    data_index = dnx_data_crps_expansion_table_source_expansion_info;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_crps_expansion_source_expansion_info_set;
    
    submodule_index = dnx_data_crps_submodule_general;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_crps_general_is_used;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_crps_general_is_used_set;
    data_index = dnx_data_crps_general_power_down_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_crps_general_power_down_supported_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

