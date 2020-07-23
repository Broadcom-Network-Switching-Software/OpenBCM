

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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>








static shr_error_e
jr2_a0_dnx_data_tune_ecgm_core_flow_control_percentage_ratio_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_ecgm;
    int define_index = dnx_data_tune_ecgm_define_core_flow_control_percentage_ratio;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 92;

    
    define->data = 92;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_tune_ecgm_port_uc_flow_control_min_divider_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_ecgm;
    int define_index = dnx_data_tune_ecgm_define_port_uc_flow_control_min_divider;
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
jr2_a0_dnx_data_tune_ecgm_port_dp_ratio_set(
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
        data->percentage_ratio = 50;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_tune_ecgm_port_dp_ratio_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->percentage_ratio = 50;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_tune_ecgm_port_dp_ratio_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->percentage_ratio = 10;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_tune_iqs_credit_resolution_up_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_iqs;
    int define_index = dnx_data_tune_iqs_define_credit_resolution_up_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128 * 1024;

    
    define->data = 128 * 1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_tune_iqs_fabric_delay_set(
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
    define->property.nof_mapping = 3;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "FE";
    define->property.mapping[0].val = 1000;
    define->property.mapping[1].name = "SINGLE_FAP";
    define->property.mapping[1].val = 300;
    define->property.mapping[1].is_default = 1 ;
    define->property.mapping[2].name = "MESH";
    define->property.mapping[2].val = 500;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_tune_iqs_fabric_multicast_delay_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_iqs;
    int define_index = dnx_data_tune_iqs_define_fabric_multicast_delay;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 200;

    
    define->data = 200;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_tune_iqs_fmq_max_rate_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_iqs;
    int define_index = dnx_data_tune_iqs_define_fmq_max_rate;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 600;

    
    define->data = 600;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_tune_iqs_bw_level_rate_set(
    int unit)
{
    int bw_level_index;
    dnx_data_tune_iqs_bw_level_rate_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_iqs;
    int table_index = dnx_data_tune_iqs_table_bw_level_rate;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_iqs.credit.nof_bw_levels_get(unit);
    table->info_get.key_size[0] = dnx_data_iqs.credit.nof_bw_levels_get(unit);

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_tune_iqs_bw_level_rate_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_tune_iqs_table_bw_level_rate");

    
    default_data = (dnx_data_tune_iqs_bw_level_rate_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->max = -1;
    default_data->min = -1;
    
    for (bw_level_index = 0; bw_level_index < table->keys[0].size; bw_level_index++)
    {
        data = (dnx_data_tune_iqs_bw_level_rate_t *) dnxc_data_mgmt_table_data_get(unit, table, bw_level_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_tune_iqs_bw_level_rate_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->max = 75;
        data->min = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_tune_iqs_bw_level_rate_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->max = 300;
        data->min = 75;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_tune_iqs_bw_level_rate_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->max = 400;
        data->min = 300;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_tune_iqs_bw_level_rate_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->max = -1;
        data->min = 400;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_tune_fabric_cgm_llfc_pipe_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_fabric;
    int define_index = dnx_data_tune_fabric_define_cgm_llfc_pipe_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 248;

    
    define->data = 248;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_tune_fabric_cgm_drop_fabric_class_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_fabric;
    int define_index = dnx_data_tune_fabric_define_cgm_drop_fabric_class_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2048;

    
    define->data = 2048;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_tune_fabric_cgm_rci_high_sev_min_links_param_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_fabric;
    int define_index = dnx_data_tune_fabric_define_cgm_rci_high_sev_min_links_param;
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
jr2_a0_data_tune_attach(
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

    
    data_index = dnx_data_tune_ecgm_define_core_flow_control_percentage_ratio;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tune_ecgm_core_flow_control_percentage_ratio_set;
    data_index = dnx_data_tune_ecgm_define_port_uc_flow_control_min_divider;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tune_ecgm_port_uc_flow_control_min_divider_set;

    

    
    data_index = dnx_data_tune_ecgm_table_port_dp_ratio;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_tune_ecgm_port_dp_ratio_set;
    
    submodule_index = dnx_data_tune_submodule_iqs;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_tune_iqs_define_credit_resolution_up_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tune_iqs_credit_resolution_up_th_set;
    data_index = dnx_data_tune_iqs_define_fabric_delay;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tune_iqs_fabric_delay_set;
    data_index = dnx_data_tune_iqs_define_fabric_multicast_delay;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tune_iqs_fabric_multicast_delay_set;
    data_index = dnx_data_tune_iqs_define_fmq_max_rate;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tune_iqs_fmq_max_rate_set;

    

    
    data_index = dnx_data_tune_iqs_table_bw_level_rate;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_tune_iqs_bw_level_rate_set;
    
    submodule_index = dnx_data_tune_submodule_fabric;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_tune_fabric_define_cgm_llfc_pipe_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tune_fabric_cgm_llfc_pipe_th_set;
    data_index = dnx_data_tune_fabric_define_cgm_drop_fabric_class_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tune_fabric_cgm_drop_fabric_class_th_set;
    data_index = dnx_data_tune_fabric_define_cgm_rci_high_sev_min_links_param;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tune_fabric_cgm_rci_high_sev_min_links_param_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

