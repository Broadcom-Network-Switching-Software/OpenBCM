

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_regression.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
q2a_a0_dnx_data_regression_dvapi_topology_nof_pm_ext_loops_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_regression;
    int submodule_index = dnx_data_regression_submodule_dvapi_topology;
    int define_index = dnx_data_regression_dvapi_topology_define_nof_pm_ext_loops;
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
q2a_a0_dnx_data_regression_dvapi_topology_nof_pm_p2p_links_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_regression;
    int submodule_index = dnx_data_regression_submodule_dvapi_topology;
    int define_index = dnx_data_regression_dvapi_topology_define_nof_pm_p2p_links;
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
q2a_a0_dnx_data_regression_dvapi_topology_pm_ext_loops_set(
    int unit)
{
    int index_index;
    dnx_data_regression_dvapi_topology_pm_ext_loops_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_regression;
    int submodule_index = dnx_data_regression_submodule_dvapi_topology;
    int table_index = dnx_data_regression_dvapi_topology_table_pm_ext_loops;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 7;
    table->info_get.key_size[0] = 7;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_regression_dvapi_topology_pm_ext_loops_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_regression_dvapi_topology_table_pm_ext_loops");

    
    default_data = (dnx_data_regression_dvapi_topology_pm_ext_loops_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->pm_id = -1;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_regression_dvapi_topology_pm_ext_loops_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_regression_dvapi_topology_pm_ext_loops_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->pm_id = 2;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_regression_dvapi_topology_pm_ext_loops_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->pm_id = 3;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_regression_dvapi_topology_pm_ext_loops_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->pm_id = 4;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_regression_dvapi_topology_pm_ext_loops_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->pm_id = 7;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_regression_dvapi_topology_pm_ext_loops_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->pm_id = 8;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_regression_dvapi_topology_pm_ext_loops_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->pm_id = 9;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_regression_dvapi_topology_pm_ext_loops_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->pm_id = 10;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_regression_dvapi_topology_pm_p2p_links_set(
    int unit)
{
    int index_index;
    dnx_data_regression_dvapi_topology_pm_p2p_links_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_regression;
    int submodule_index = dnx_data_regression_submodule_dvapi_topology;
    int table_index = dnx_data_regression_dvapi_topology_table_pm_p2p_links;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_regression_dvapi_topology_pm_p2p_links_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_regression_dvapi_topology_table_pm_p2p_links");

    
    default_data = (dnx_data_regression_dvapi_topology_pm_p2p_links_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->pm_id = -1;
    default_data->peer_pm_id = -1;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_regression_dvapi_topology_pm_p2p_links_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_regression_dvapi_topology_pm_p2p_links_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->pm_id = 0;
        data->peer_pm_id = 1;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_regression_dvapi_topology_pm_p2p_links_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->pm_id = 5;
        data->peer_pm_id = 6;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_regression_dvapi_topology_pm_test_limitations_set(
    int unit)
{
    int pm_index_index;
    dnx_data_regression_dvapi_topology_pm_test_limitations_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_regression;
    int submodule_index = dnx_data_regression_submodule_dvapi_topology;
    int table_index = dnx_data_regression_dvapi_topology_table_pm_test_limitations;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_nif.eth.total_nof_ethu_pms_in_device_get(unit);
    table->info_get.key_size[0] = dnx_data_nif.eth.total_nof_ethu_pms_in_device_get(unit);

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_regression_dvapi_topology_pm_test_limitations_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_regression_dvapi_topology_table_pm_test_limitations");

    
    default_data = (dnx_data_regression_dvapi_topology_pm_test_limitations_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->max_speed = -1;
    
    for (pm_index_index = 0; pm_index_index < table->keys[0].size; pm_index_index++)
    {
        data = (dnx_data_regression_dvapi_topology_pm_test_limitations_t *) dnxc_data_mgmt_table_data_get(unit, table, pm_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_regression_dvapi_topology_pm_test_limitations_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->max_speed = 10312;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
q2a_a0_data_regression_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_regression;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_regression_submodule_dvapi_topology;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_regression_dvapi_topology_define_nof_pm_ext_loops;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_regression_dvapi_topology_nof_pm_ext_loops_set;
    data_index = dnx_data_regression_dvapi_topology_define_nof_pm_p2p_links;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_regression_dvapi_topology_nof_pm_p2p_links_set;

    

    
    data_index = dnx_data_regression_dvapi_topology_table_pm_ext_loops;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_regression_dvapi_topology_pm_ext_loops_set;
    data_index = dnx_data_regression_dvapi_topology_table_pm_p2p_links;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_regression_dvapi_topology_pm_p2p_links_set;
    data_index = dnx_data_regression_dvapi_topology_table_pm_test_limitations;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_regression_dvapi_topology_pm_test_limitations_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

