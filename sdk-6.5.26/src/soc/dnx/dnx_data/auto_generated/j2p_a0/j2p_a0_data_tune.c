
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_tune.h>









static shr_error_e
j2p_a0_dnx_data_tune_ecgm_port_dp_ratio_set(
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
j2p_a0_dnx_data_tune_fabric_cgm_rci_update_egress_th_values_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_fabric;
    int feature_index = dnx_data_tune_fabric_cgm_rci_update_egress_th_values;
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
j2p_a0_dnx_data_tune_fabric_cgm_rci_high_sev_min_links_param_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_fabric;
    int define_index = dnx_data_tune_fabric_define_cgm_rci_high_sev_min_links_param;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_tune_fabric_cgm_rci_egress_pipe_level_th_base_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_fabric;
    int define_index = dnx_data_tune_fabric_define_cgm_rci_egress_pipe_level_th_base;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1000;

    
    define->data = 1000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_tune_fabric_cgm_rci_egress_pipe_level_th_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_fabric;
    int define_index = dnx_data_tune_fabric_define_cgm_rci_egress_pipe_level_th_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 300;

    
    define->data = 300;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_tune_fabric_cgm_rci_total_egress_pipe_level_th_base_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_fabric;
    int define_index = dnx_data_tune_fabric_define_cgm_rci_total_egress_pipe_level_th_base;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4000;

    
    define->data = 4000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_tune_fabric_cgm_rci_total_egress_pipe_level_th_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_fabric;
    int define_index = dnx_data_tune_fabric_define_cgm_rci_total_egress_pipe_level_th_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1000;

    
    define->data = 1000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2p_a0_dnx_data_tune_fabric_cgm_drop_fabric_multicast_th_set(
    int unit)
{
    int priority_index;
    dnx_data_tune_fabric_cgm_drop_fabric_multicast_th_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_fabric;
    int table_index = dnx_data_tune_fabric_table_cgm_drop_fabric_multicast_th;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;

    
    table->values[0].default_val = "DATA(tune, fabric, cgm_drop_fabric_class_th)";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_tune_fabric_cgm_drop_fabric_multicast_th_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_tune_fabric_table_cgm_drop_fabric_multicast_th");

    
    default_data = (dnx_data_tune_fabric_cgm_drop_fabric_multicast_th_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->threshold = dnx_data_tune.fabric.cgm_drop_fabric_class_th_get(unit);
    
    for (priority_index = 0; priority_index < table->keys[0].size; priority_index++)
    {
        data = (dnx_data_tune_fabric_cgm_drop_fabric_multicast_th_t *) dnxc_data_mgmt_table_data_get(unit, table, priority_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_tune_fabric_cgm_drop_fabric_multicast_th_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->threshold = 1900;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_tune_fabric_cgm_drop_fabric_multicast_th_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->threshold = 1600;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_tune_fabric_cgm_drop_fabric_multicast_th_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->threshold = 1400;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_tune_fabric_cgm_drop_fabric_multicast_th_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->threshold = 1200;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_tune_fabric_cgm_rci_egress_level_factor_set(
    int unit)
{
    int level_index;
    dnx_data_tune_fabric_cgm_rci_egress_level_factor_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_tune;
    int submodule_index = dnx_data_tune_submodule_fabric;
    int table_index = dnx_data_tune_fabric_table_cgm_rci_egress_level_factor;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 3;
    table->info_get.key_size[0] = 3;

    
    table->values[0].default_val = "500";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_tune_fabric_cgm_rci_egress_level_factor_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_tune_fabric_table_cgm_rci_egress_level_factor");

    
    default_data = (dnx_data_tune_fabric_cgm_rci_egress_level_factor_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->score = 500;
    
    for (level_index = 0; level_index < table->keys[0].size; level_index++)
    {
        data = (dnx_data_tune_fabric_cgm_rci_egress_level_factor_t *) dnxc_data_mgmt_table_data_get(unit, table, level_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_tune_fabric_cgm_rci_egress_level_factor_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->score = 250;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_tune_fabric_cgm_rci_egress_level_factor_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->score = 450;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_tune_fabric_cgm_rci_egress_level_factor_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->score = 650;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
j2p_a0_data_tune_attach(
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

    

    

    
    data_index = dnx_data_tune_ecgm_table_port_dp_ratio;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_tune_ecgm_port_dp_ratio_set;
    
    submodule_index = dnx_data_tune_submodule_fabric;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_tune_fabric_define_cgm_rci_high_sev_min_links_param;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_tune_fabric_cgm_rci_high_sev_min_links_param_set;
    data_index = dnx_data_tune_fabric_define_cgm_rci_egress_pipe_level_th_base;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_tune_fabric_cgm_rci_egress_pipe_level_th_base_set;
    data_index = dnx_data_tune_fabric_define_cgm_rci_egress_pipe_level_th_offset;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_tune_fabric_cgm_rci_egress_pipe_level_th_offset_set;
    data_index = dnx_data_tune_fabric_define_cgm_rci_total_egress_pipe_level_th_base;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_tune_fabric_cgm_rci_total_egress_pipe_level_th_base_set;
    data_index = dnx_data_tune_fabric_define_cgm_rci_total_egress_pipe_level_th_offset;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_tune_fabric_cgm_rci_total_egress_pipe_level_th_offset_set;

    
    data_index = dnx_data_tune_fabric_cgm_rci_update_egress_th_values;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_tune_fabric_cgm_rci_update_egress_th_values_set;

    
    data_index = dnx_data_tune_fabric_table_cgm_drop_fabric_multicast_th;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_tune_fabric_cgm_drop_fabric_multicast_th_set;
    data_index = dnx_data_tune_fabric_table_cgm_rci_egress_level_factor;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_tune_fabric_cgm_rci_egress_level_factor_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

