

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
j2c_a0_dnx_data_tune_ecgm_port_uc_flow_control_min_divider_set(
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
j2c_a0_dnx_data_tune_ecgm_port_dp_ratio_set(
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
j2c_a0_dnx_data_tune_iqs_fmq_max_rate_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_iqs;
    int define_index = dnx_data_tune_iqs_define_fmq_max_rate;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 400;

    
    define->data = 400;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2c_a0_dnx_data_tune_fabric_cgm_llfc_pipe_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_fabric;
    int define_index = dnx_data_tune_fabric_define_cgm_llfc_pipe_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 376;

    
    define->data = 376;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_tune_fabric_cgm_drop_fabric_class_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_fabric;
    int define_index = dnx_data_tune_fabric_define_cgm_drop_fabric_class_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 192;

    
    define->data = 192;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_tune_fabric_cgm_rci_high_sev_min_links_param_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_fabric;
    int define_index = dnx_data_tune_fabric_define_cgm_rci_high_sev_min_links_param;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2c_a0_data_tune_attach(
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
    define->set = j2c_a0_dnx_data_tune_ecgm_port_uc_flow_control_min_divider_set;

    

    
    data_index = dnx_data_tune_ecgm_table_port_dp_ratio;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_tune_ecgm_port_dp_ratio_set;
    
    submodule_index = dnx_data_tune_submodule_iqs;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_tune_iqs_define_fmq_max_rate;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tune_iqs_fmq_max_rate_set;

    

    
    
    submodule_index = dnx_data_tune_submodule_fabric;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_tune_fabric_define_cgm_llfc_pipe_th;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tune_fabric_cgm_llfc_pipe_th_set;
    data_index = dnx_data_tune_fabric_define_cgm_drop_fabric_class_th;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tune_fabric_cgm_drop_fabric_class_th_set;
    data_index = dnx_data_tune_fabric_define_cgm_rci_high_sev_min_links_param;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tune_fabric_cgm_rci_high_sev_min_links_param_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

