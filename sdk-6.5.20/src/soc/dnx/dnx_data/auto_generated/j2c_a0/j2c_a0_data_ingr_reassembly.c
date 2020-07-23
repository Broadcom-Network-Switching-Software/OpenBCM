

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_ingr_reassembly.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
j2c_a0_dnx_data_ingr_reassembly_context_rcy_interleaving_between_priorities_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_ingr_reassembly;
    int submodule_index = dnx_data_ingr_reassembly_submodule_context;
    int feature_index = dnx_data_ingr_reassembly_context_rcy_interleaving_between_priorities;
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
j2c_a0_dnx_data_ingr_reassembly_context_nof_contexts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_reassembly;
    int submodule_index = dnx_data_ingr_reassembly_submodule_context;
    int define_index = dnx_data_ingr_reassembly_context_define_nof_contexts;
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
j2c_a0_dnx_data_ingr_reassembly_context_context_map_set(
    int unit)
{
    int port_type_index;
    dnx_data_ingr_reassembly_context_context_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_ingr_reassembly;
    int submodule_index = dnx_data_ingr_reassembly_submodule_context;
    int table_index = dnx_data_ingr_reassembly_context_table_context_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_ALGO_PORT_TYPE_NOF;
    table->info_get.key_size[0] = DNX_ALGO_PORT_TYPE_NOF;

    
    table->values[0].default_val = "SAL_UINT32_MAX";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_ingr_reassembly_context_context_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_ingr_reassembly_context_table_context_map");

    
    default_data = (dnx_data_ingr_reassembly_context_context_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->start_index = SAL_UINT32_MAX;
    
    for (port_type_index = 0; port_type_index < table->keys[0].size; port_type_index++)
    {
        data = (dnx_data_ingr_reassembly_context_context_map_t *) dnxc_data_mgmt_table_data_get(unit, table, port_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_ALGO_PORT_TYPE_NIF_ILKN < table->keys[0].size)
    {
        data = (dnx_data_ingr_reassembly_context_context_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_ALGO_PORT_TYPE_NIF_ILKN, 0);
        data->start_index = 0;
    }
    if (DNX_ALGO_PORT_TYPE_NIF_ETH < table->keys[0].size)
    {
        data = (dnx_data_ingr_reassembly_context_context_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_ALGO_PORT_TYPE_NIF_ETH, 0);
        data->start_index = 0x1000;
    }
    if (DNX_ALGO_PORT_TYPE_RCY < table->keys[0].size)
    {
        data = (dnx_data_ingr_reassembly_context_context_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_ALGO_PORT_TYPE_RCY, 0);
        data->start_index = 0x1200;
    }
    if (DNX_ALGO_PORT_TYPE_RCY_MIRROR < table->keys[0].size)
    {
        data = (dnx_data_ingr_reassembly_context_context_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_ALGO_PORT_TYPE_RCY_MIRROR, 0);
        data->start_index = 0x1300;
    }
    if (DNX_ALGO_PORT_TYPE_CPU < table->keys[0].size)
    {
        data = (dnx_data_ingr_reassembly_context_context_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_ALGO_PORT_TYPE_CPU, 0);
        data->start_index = 0x1600;
    }
    if (DNX_ALGO_PORT_TYPE_SAT < table->keys[0].size)
    {
        data = (dnx_data_ingr_reassembly_context_context_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_ALGO_PORT_TYPE_SAT, 0);
        data->start_index = 0x1700;
    }
    if (DNX_ALGO_PORT_TYPE_OLP < table->keys[0].size)
    {
        data = (dnx_data_ingr_reassembly_context_context_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_ALGO_PORT_TYPE_OLP, 0);
        data->start_index = dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit) + 1;
    }
    if (DNX_ALGO_PORT_TYPE_OAMP < table->keys[0].size)
    {
        data = (dnx_data_ingr_reassembly_context_context_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_ALGO_PORT_TYPE_OAMP, 0);
        data->start_index = dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit) + 2;
    }
    if (DNX_ALGO_PORT_TYPE_EVENTOR < table->keys[0].size)
    {
        data = (dnx_data_ingr_reassembly_context_context_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_ALGO_PORT_TYPE_EVENTOR, 0);
        data->start_index = dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit) + 3;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2c_a0_dnx_data_ingr_reassembly_ilkn_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_ingr_reassembly;
    int submodule_index = dnx_data_ingr_reassembly_submodule_ilkn;
    int feature_index = dnx_data_ingr_reassembly_ilkn_is_supported;
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
j2c_a0_dnx_data_ingr_reassembly_dbal_reassembly_context_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_reassembly;
    int submodule_index = dnx_data_ingr_reassembly_submodule_dbal;
    int define_index = dnx_data_ingr_reassembly_dbal_define_reassembly_context_bits;
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
j2c_a0_dnx_data_ingr_reassembly_dbal_interface_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_reassembly;
    int submodule_index = dnx_data_ingr_reassembly_submodule_dbal;
    int define_index = dnx_data_ingr_reassembly_dbal_define_interface_bits;
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
j2c_a0_dnx_data_ingr_reassembly_dbal_interface_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_reassembly;
    int submodule_index = dnx_data_ingr_reassembly_submodule_dbal;
    int define_index = dnx_data_ingr_reassembly_dbal_define_interface_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 159;

    
    define->data = 159;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_ingr_reassembly_dbal_context_map_base_address_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_reassembly;
    int submodule_index = dnx_data_ingr_reassembly_submodule_dbal;
    int define_index = dnx_data_ingr_reassembly_dbal_define_context_map_base_address_bits;
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
j2c_a0_dnx_data_ingr_reassembly_dbal_context_map_base_address_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_reassembly;
    int submodule_index = dnx_data_ingr_reassembly_submodule_dbal;
    int define_index = dnx_data_ingr_reassembly_dbal_define_context_map_base_address_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6*1024-1;

    
    define->data = 6*1024-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_ingr_reassembly_dbal_interleaved_interface_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_reassembly;
    int submodule_index = dnx_data_ingr_reassembly_submodule_dbal;
    int define_index = dnx_data_ingr_reassembly_dbal_define_interleaved_interface_bits;
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
j2c_a0_data_ingr_reassembly_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_ingr_reassembly;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_ingr_reassembly_submodule_context;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ingr_reassembly_context_define_nof_contexts;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_ingr_reassembly_context_nof_contexts_set;

    
    data_index = dnx_data_ingr_reassembly_context_rcy_interleaving_between_priorities;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_ingr_reassembly_context_rcy_interleaving_between_priorities_set;

    
    data_index = dnx_data_ingr_reassembly_context_table_context_map;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_ingr_reassembly_context_context_map_set;
    
    submodule_index = dnx_data_ingr_reassembly_submodule_ilkn;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_ingr_reassembly_ilkn_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_ingr_reassembly_ilkn_is_supported_set;

    
    
    submodule_index = dnx_data_ingr_reassembly_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ingr_reassembly_dbal_define_reassembly_context_bits;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_ingr_reassembly_dbal_reassembly_context_bits_set;
    data_index = dnx_data_ingr_reassembly_dbal_define_interface_bits;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_ingr_reassembly_dbal_interface_bits_set;
    data_index = dnx_data_ingr_reassembly_dbal_define_interface_max;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_ingr_reassembly_dbal_interface_max_set;
    data_index = dnx_data_ingr_reassembly_dbal_define_context_map_base_address_bits;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_ingr_reassembly_dbal_context_map_base_address_bits_set;
    data_index = dnx_data_ingr_reassembly_dbal_define_context_map_base_address_max;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_ingr_reassembly_dbal_context_map_base_address_max_set;
    data_index = dnx_data_ingr_reassembly_dbal_define_interleaved_interface_bits;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_ingr_reassembly_dbal_interleaved_interface_bits_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

