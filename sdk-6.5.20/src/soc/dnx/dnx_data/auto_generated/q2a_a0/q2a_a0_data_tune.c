

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_tune.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
q2a_a0_dnx_data_tune_ecgm_port_uc_flow_control_min_divider_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_ecgm;
    int define_index = dnx_data_tune_ecgm_define_port_uc_flow_control_min_divider;
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
q2a_a0_dnx_data_tune_ecgm_port_dp_ratio_set(
    int unit)
{
    int dp_index;
    dnx_data_tune_ecgm_port_dp_ratio_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_ecgm;
    int table_index = dnx_data_tune_ecgm_table_port_dp_ratio;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;

    
    table->values[0].default_val = "100";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_tune_ecgm_port_dp_ratio_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_tune_ecgm_table_port_dp_ratio");

    
    default_data = (dnx_data_tune_ecgm_port_dp_ratio_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->percentage_ratio = 100;
    
    for (dp_index = 0; dp_index < table->keys[0].size; dp_index++)
    {
        data = (dnx_data_tune_ecgm_port_dp_ratio_t *) dnxc_data_mgmt_table_data_get(unit, table, dp_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_tune_ecgm_port_dp_ratio_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->percentage_ratio = 100;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_tune_ecgm_port_dp_ratio_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->percentage_ratio = 100;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_tune_ecgm_port_dp_ratio_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->percentage_ratio = 50;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_tune_ecgm_port_dp_ratio_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->percentage_ratio = 50;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
q2a_a0_dnx_data_tune_iqs_fabric_delay_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_iqs;
    int define_index = dnx_data_tune_iqs_define_fabric_delay;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 300;

    
    define->data = 300;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FABRIC_CONNECT_MODE;
    define->property.doc = 
        "\n"
        "See fabric module\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 1;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "SINGLE_FAP";
    define->property.mapping[0].val = 300;
    define->property.mapping[0].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}




shr_error_e
q2a_a0_data_tune_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_tune;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_tune_submodule_ecgm;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_tune_ecgm_define_port_uc_flow_control_min_divider;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_tune_ecgm_port_uc_flow_control_min_divider_set;

    

    
    data_index = dnx_data_tune_ecgm_table_port_dp_ratio;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_tune_ecgm_port_dp_ratio_set;
    
    submodule_index = dnx_data_tune_submodule_iqs;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_tune_iqs_define_fabric_delay;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_tune_iqs_fabric_delay_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

