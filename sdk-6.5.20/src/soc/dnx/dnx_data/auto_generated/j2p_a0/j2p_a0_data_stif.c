

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_STAT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_stif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
j2p_a0_dnx_data_stif_config_mixing_ports_limitation_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int feature_index = dnx_data_stif_config_mixing_ports_limitation;
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
j2p_a0_dnx_data_stif_config_map_source_to_two_stif_ports_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int feature_index = dnx_data_stif_config_map_source_to_two_stif_ports;
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
j2p_a0_dnx_data_stif_config_only_billing_mode_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int feature_index = dnx_data_stif_config_only_billing_mode_support;
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
j2p_a0_dnx_data_stif_config_stif_report_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int define_index = dnx_data_stif_config_define_stif_report_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_stat_stif_mgmt_report_mode_billing;

    
    define->data = dnx_stat_stif_mgmt_report_mode_billing;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_STAT_IF_REPORT_MODE;
    define->property.doc = 
        "\n"
        "STIF report mode - billing\n"
        "Default: billing\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 1;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "BILLING";
    define->property.mapping[0].val = dnx_stat_stif_mgmt_report_mode_billing;
    define->property.mapping[0].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2p_a0_dnx_data_stif_port_max_nof_instances_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_port;
    int define_index = dnx_data_stif_port_define_max_nof_instances;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_stif_port_stif_ethu_select_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_port;
    int define_index = dnx_data_stif_port_define_stif_ethu_select_size;
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
j2p_a0_dnx_data_stif_port_stif_lane_mapping_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_port;
    int define_index = dnx_data_stif_port_define_stif_lane_mapping_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2p_a0_dnx_data_stif_port_ethu_id_stif_valid_set(
    int unit)
{
    int ethu_id_index;
    dnx_data_stif_port_ethu_id_stif_valid_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_port;
    int table_index = dnx_data_stif_port_table_ethu_id_stif_valid;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_nif.eth.ethu_nof_get(unit);
    table->info_get.key_size[0] = dnx_data_nif.eth.ethu_nof_get(unit);

    
    table->values[0].default_val = "1";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_stif_port_ethu_id_stif_valid_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_stif_port_table_ethu_id_stif_valid");

    
    default_data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = 1;
    default_data->hw_select_value = 0;
    
    for (ethu_id_index = 0; ethu_id_index < table->keys[0].size; ethu_id_index++)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, ethu_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->valid = 1;
        data->hw_select_value = 1;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->valid = 0;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->valid = 0;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->valid = 0;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->valid = 0;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->valid = 0;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->valid = 0;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->valid = 0;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->valid = 0;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->valid = 1;
        data->hw_select_value = 1;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->valid = 0;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->valid = 0;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->valid = 0;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->valid = 0;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->valid = 0;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->valid = 0;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->valid = 0;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->valid = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2p_a0_dnx_data_stif_report_etpp_core_in_portmetadata_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int feature_index = dnx_data_stif_report_etpp_core_in_portmetadata;
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
j2p_a0_dnx_data_stif_report_stif_record_is_last_copy_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int feature_index = dnx_data_stif_report_stif_record_is_last_copy;
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
j2p_a0_dnx_data_stif_flow_control_instance_0_must_be_on_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_flow_control;
    int feature_index = dnx_data_stif_flow_control_instance_0_must_be_on;
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
j2p_a0_data_stif_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_stif;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_stif_submodule_config;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_stif_config_define_stif_report_mode;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_stif_config_stif_report_mode_set;

    
    data_index = dnx_data_stif_config_mixing_ports_limitation;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_stif_config_mixing_ports_limitation_set;
    data_index = dnx_data_stif_config_map_source_to_two_stif_ports;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_stif_config_map_source_to_two_stif_ports_set;
    data_index = dnx_data_stif_config_only_billing_mode_support;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_stif_config_only_billing_mode_support_set;

    
    
    submodule_index = dnx_data_stif_submodule_port;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_stif_port_define_max_nof_instances;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_stif_port_max_nof_instances_set;
    data_index = dnx_data_stif_port_define_stif_ethu_select_size;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_stif_port_stif_ethu_select_size_set;
    data_index = dnx_data_stif_port_define_stif_lane_mapping_size;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_stif_port_stif_lane_mapping_size_set;

    

    
    data_index = dnx_data_stif_port_table_ethu_id_stif_valid;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_stif_port_ethu_id_stif_valid_set;
    
    submodule_index = dnx_data_stif_submodule_report;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_stif_report_etpp_core_in_portmetadata;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_stif_report_etpp_core_in_portmetadata_set;
    data_index = dnx_data_stif_report_stif_record_is_last_copy;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_stif_report_stif_record_is_last_copy_set;

    
    
    submodule_index = dnx_data_stif_submodule_flow_control;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_stif_flow_control_instance_0_must_be_on;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_stif_flow_control_instance_0_must_be_on_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

