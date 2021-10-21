
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PVT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_pvt.h>







static shr_error_e
j2c_a0_dnx_data_pvt_general_pvt_peak_faulty_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_pvt;
    int submodule_index = dnx_data_pvt_submodule_general;
    int feature_index = dnx_data_pvt_general_pvt_peak_faulty;
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
j2c_a0_dnx_data_pvt_general_nof_pvt_monitors_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pvt;
    int submodule_index = dnx_data_pvt_submodule_general;
    int define_index = dnx_data_pvt_general_define_nof_pvt_monitors;
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
j2c_a0_dnx_data_pvt_general_pvt_base_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pvt;
    int submodule_index = dnx_data_pvt_submodule_general;
    int define_index = dnx_data_pvt_general_define_pvt_base;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 43410000;

    
    define->data = 43410000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_pvt_general_pvt_factor_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pvt;
    int submodule_index = dnx_data_pvt_submodule_general;
    int define_index = dnx_data_pvt_general_define_pvt_factor;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 53504;

    
    define->data = 53504;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2c_a0_dnx_data_pvt_general_pvt_name_set(
    int unit)
{
    int index_index;
    dnx_data_pvt_general_pvt_name_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_pvt;
    int submodule_index = dnx_data_pvt_submodule_general;
    int table_index = dnx_data_pvt_general_table_pvt_name;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_pvt.general.nof_pvt_monitors_get(unit) ;
    table->info_get.key_size[0] = dnx_data_pvt.general.nof_pvt_monitors_get(unit) ;

    
    table->values[0].default_val = "NA";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_pvt_general_pvt_name_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_pvt_general_table_pvt_name");

    
    default_data = (dnx_data_pvt_general_pvt_name_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->display_name = "NA";
    default_data->valid = 0;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_pvt_general_pvt_name_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_pvt_general_pvt_name_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->display_name = "FAB";
        data->valid = 1;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_pvt_general_pvt_name_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->display_name = "NIF0";
        data->valid = 1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_pvt_general_pvt_name_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->display_name = "NIF1";
        data->valid = 1;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_pvt_general_pvt_name_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->display_name = "NIF2";
        data->valid = 1;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_pvt_general_pvt_name_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->display_name = "PRM";
        data->valid = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
j2c_a0_data_pvt_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_pvt;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_pvt_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_pvt_general_define_nof_pvt_monitors;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_pvt_general_nof_pvt_monitors_set;
    data_index = dnx_data_pvt_general_define_pvt_base;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_pvt_general_pvt_base_set;
    data_index = dnx_data_pvt_general_define_pvt_factor;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_pvt_general_pvt_factor_set;

    
    data_index = dnx_data_pvt_general_pvt_peak_faulty;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_pvt_general_pvt_peak_faulty_set;

    
    data_index = dnx_data_pvt_general_table_pvt_name;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_pvt_general_pvt_name_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

