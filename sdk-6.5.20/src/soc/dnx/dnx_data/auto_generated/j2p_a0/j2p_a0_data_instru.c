

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INSTRU

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_instru.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
j2p_a0_dnx_data_instru_ipt_advanced_ipt_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_ipt;
    int feature_index = dnx_data_instru_ipt_advanced_ipt;
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
j2p_a0_dnx_data_instru_ipt_profile_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_ipt;
    int define_index = dnx_data_instru_ipt_define_profile_size;
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
j2p_a0_dnx_data_instru_ipt_metadata_bitmap_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_ipt;
    int define_index = dnx_data_instru_ipt_define_metadata_bitmap_size;
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
j2p_a0_dnx_data_instru_ipt_metadata_edit_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_ipt;
    int define_index = dnx_data_instru_ipt_define_metadata_edit_size;
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
j2p_a0_dnx_data_instru_ipt_node_id_padding_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_ipt;
    int define_index = dnx_data_instru_ipt_define_node_id_padding_size;
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
j2p_a0_dnx_data_instru_ipt_profile_info_set(
    int unit)
{
    int profile_id_index;
    dnx_data_instru_ipt_profile_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_ipt;
    int table_index = dnx_data_instru_ipt_table_profile_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 8;
    table->info_get.key_size[0] = 8;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_instru_ipt_profile_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_instru_ipt_table_profile_info");

    
    default_data = (dnx_data_instru_ipt_profile_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->type = -1;
    
    for (profile_id_index = 0; profile_id_index < table->keys[0].size; profile_id_index++)
    {
        data = (dnx_data_instru_ipt_profile_info_t *) dnxc_data_mgmt_table_data_get(unit, table, profile_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_profile_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->type = bcmInstruIptNodeTypeTail;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_profile_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->type = bcmInstruIptNodeTypeIntOverVxlanIntermediate;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_profile_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->type = bcmInstruIptNodeTypeTail;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_profile_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->type = bcmInstruIptNodeTypeIntOverVxlanFirst;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_profile_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->type = bcmInstruIptNodeTypeTail;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_profile_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->type = bcmInstruIptNodeTypeIntOverVxlanLast;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_profile_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->type = bcmInstruIptNodeTypeTail;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_instru_ipt_metadata_set(
    int unit)
{
    int flag_id_index;
    dnx_data_instru_ipt_metadata_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_ipt;
    int table_index = dnx_data_instru_ipt_table_metadata;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 32;
    table->info_get.key_size[0] = 32;

    
    table->values[0].default_val = "SAL_UINT32_MAX";
    table->values[1].default_val = "SAL_UINT32_MAX";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_instru_ipt_metadata_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_instru_ipt_table_metadata");

    
    default_data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->bit = SAL_UINT32_MAX;
    default_data->size = SAL_UINT32_MAX;
    
    for (flag_id_index = 0; flag_id_index < table->keys[0].size; flag_id_index++)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, flag_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->bit = 0;
        data->size = 4;
    }
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->bit = 1;
        data->size = 6;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->bit = 2;
        data->size = 6;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->bit = 3;
        data->size = 2;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->bit = 4;
        data->size = 2;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->bit = 5;
        data->size = 2;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->bit = 6;
        data->size = 3;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->bit = 7;
        data->size = 2;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->bit = 8;
        data->size = 1;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->bit = 9;
        data->size = 2;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2p_a0_dnx_data_instru_ifa_max_lenght_check_for_ifa2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_ifa;
    int define_index = dnx_data_instru_ifa_define_max_lenght_check_for_ifa2;
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
j2p_a0_dnx_data_instru_synced_counters_interval_period_size_hw_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_synced_counters;
    int define_index = dnx_data_instru_synced_counters_define_interval_period_size_hw;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 48;

    
    define->data = 48;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2p_a0_data_instru_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_instru;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_instru_submodule_ipt;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_instru_ipt_define_profile_size;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_instru_ipt_profile_size_set;
    data_index = dnx_data_instru_ipt_define_metadata_bitmap_size;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_instru_ipt_metadata_bitmap_size_set;
    data_index = dnx_data_instru_ipt_define_metadata_edit_size;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_instru_ipt_metadata_edit_size_set;
    data_index = dnx_data_instru_ipt_define_node_id_padding_size;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_instru_ipt_node_id_padding_size_set;

    
    data_index = dnx_data_instru_ipt_advanced_ipt;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_instru_ipt_advanced_ipt_set;

    
    data_index = dnx_data_instru_ipt_table_profile_info;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_instru_ipt_profile_info_set;
    data_index = dnx_data_instru_ipt_table_metadata;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_instru_ipt_metadata_set;
    
    submodule_index = dnx_data_instru_submodule_ifa;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_instru_ifa_define_max_lenght_check_for_ifa2;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_instru_ifa_max_lenght_check_for_ifa2_set;

    

    
    
    submodule_index = dnx_data_instru_submodule_synced_counters;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_instru_synced_counters_define_interval_period_size_hw;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_instru_synced_counters_interval_period_size_hw_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

