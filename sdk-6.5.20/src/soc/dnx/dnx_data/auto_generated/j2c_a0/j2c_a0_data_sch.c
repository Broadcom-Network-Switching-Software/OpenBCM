

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <bcm_int/dnx/algo/sch/sch_alloc_mngr.h>







static shr_error_e
j2c_a0_dnx_data_sch_general_port_priority_propagation_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_general;
    int feature_index = dnx_data_sch_general_port_priority_propagation_supported;
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
j2c_a0_dnx_data_sch_general_port_priority_propagation_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_general;
    int define_index = dnx_data_sch_general_define_port_priority_propagation_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_PORT_SCH_PRIORITY_PROPAGATION_ENABLE;
    define->property.doc = 
        "\n"
        "Enable port priority propagation functionality.\n"
        "Default: 0 (disabled)\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_enable;
    define->property.method_str = "enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2c_a0_dnx_data_sch_ps_max_port_rate_mbps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_ps;
    int define_index = dnx_data_sch_ps_define_max_port_rate_mbps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 700000;

    
    define->data = 700000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_MAXIMAL_PORT_BANDWIDTH;
    define->property.doc = 
        "\n"
        "Maximal bandwidth of a single port in Mega-bits-per-second.\n"
        "Used to improve granularity of SCH port-TC and port-TCG shapers.\n"
        "Default: 700000 (700G)\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = dnx_data_sch.interface.max_if_rate_mbps_get(unit);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2c_a0_dnx_data_sch_flow_cl_fq_cl_fq_quartet_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int feature_index = dnx_data_sch_flow_cl_fq_cl_fq_quartet;
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
j2c_a0_dnx_data_sch_flow_nof_hr_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_nof_hr;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1024;

    
    define->data = 1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_sch_flow_nof_cl_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_nof_cl;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32*1024;

    
    define->data = 32*1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_sch_flow_nof_se_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_nof_se;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64*1024;

    
    define->data = 64*1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_sch_flow_nof_flows_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_nof_flows;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 192*1024;

    
    define->data = 192*1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2c_a0_dnx_data_sch_flow_cl_num_to_quartet_offset_set(
    int unit)
{
    int cl_num_index;
    dnx_data_sch_flow_cl_num_to_quartet_offset_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int table_index = dnx_data_sch_flow_table_cl_num_to_quartet_offset;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_flow_cl_num_to_quartet_offset_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_sch_flow_table_cl_num_to_quartet_offset");

    
    default_data = (dnx_data_sch_flow_cl_num_to_quartet_offset_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->quartet_offset = -1;
    
    for (cl_num_index = 0; cl_num_index < table->keys[0].size; cl_num_index++)
    {
        data = (dnx_data_sch_flow_cl_num_to_quartet_offset_t *) dnxc_data_mgmt_table_data_get(unit, table, cl_num_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_sch_flow_cl_num_to_quartet_offset_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->quartet_offset = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_sch_flow_cl_num_to_quartet_offset_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->quartet_offset = 2;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_sch_flow_quartet_offset_to_cl_num_set(
    int unit)
{
    int quartet_offset_index;
    dnx_data_sch_flow_quartet_offset_to_cl_num_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int table_index = dnx_data_sch_flow_table_quartet_offset_to_cl_num;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_flow_quartet_offset_to_cl_num_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_sch_flow_table_quartet_offset_to_cl_num");

    
    default_data = (dnx_data_sch_flow_quartet_offset_to_cl_num_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->cl_num = -1;
    
    for (quartet_offset_index = 0; quartet_offset_index < table->keys[0].size; quartet_offset_index++)
    {
        data = (dnx_data_sch_flow_quartet_offset_to_cl_num_t *) dnxc_data_mgmt_table_data_get(unit, table, quartet_offset_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_sch_flow_quartet_offset_to_cl_num_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->cl_num = 0;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_sch_flow_quartet_offset_to_cl_num_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->cl_num = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2c_a0_dnx_data_sch_interface_non_channelized_calendar_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_interface;
    int feature_index = dnx_data_sch_interface_non_channelized_calendar;
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
j2c_a0_dnx_data_sch_interface_nof_channelized_calendars_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_interface;
    int define_index = dnx_data_sch_interface_define_nof_channelized_calendars;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_sch_interface_nof_sch_interfaces_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_interface;
    int define_index = dnx_data_sch_interface_define_nof_sch_interfaces;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 136;

    
    define->data = 136;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_sch_interface_reserved_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_interface;
    int define_index = dnx_data_sch_interface_define_reserved;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_sch.interface.nof_sch_interfaces_get(unit);

    
    define->data = dnx_data_sch.interface.nof_sch_interfaces_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2c_a0_dnx_data_sch_interface_shaper_resolution_set(
    int unit)
{
    int channelized_index;
    dnx_data_sch_interface_shaper_resolution_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_interface;
    int table_index = dnx_data_sch_interface_table_shaper_resolution;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_interface_shaper_resolution_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_sch_interface_table_shaper_resolution");

    
    default_data = (dnx_data_sch_interface_shaper_resolution_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->resolution = 0;
    
    for (channelized_index = 0; channelized_index < table->keys[0].size; channelized_index++)
    {
        data = (dnx_data_sch_interface_shaper_resolution_t *) dnxc_data_mgmt_table_data_get(unit, table, channelized_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_sch_interface_shaper_resolution_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->resolution = 128;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_sch_interface_shaper_resolution_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->resolution = 4;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2c_a0_dnx_data_sch_device_shared_drm_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_device;
    int feature_index = dnx_data_sch_device_shared_drm;
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
j2c_a0_dnx_data_sch_sch_alloc_region_set(
    int unit)
{
    int region_type_index;
    dnx_data_sch_sch_alloc_region_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int table_index = dnx_data_sch_sch_alloc_table_region;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_SCH_REGION_TYPE_NOF;
    table->info_get.key_size[0] = DNX_SCH_REGION_TYPE_NOF;

    
    table->values[0].default_val = "DNX_SCH_ELEMENT_SE_TYPE_NONE";
    table->values[1].default_val = "DNX_SCHEDULER_REGION_EVEN";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_sch_alloc_region_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_sch_sch_alloc_table_region");

    
    default_data = (dnx_data_sch_sch_alloc_region_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->flow_type[0] = DNX_SCH_ELEMENT_SE_TYPE_NONE;
    default_data->odd_even_mode = DNX_SCHEDULER_REGION_EVEN;
    
    for (region_type_index = 0; region_type_index < table->keys[0].size; region_type_index++)
    {
        data = (dnx_data_sch_sch_alloc_region_t *) dnxc_data_mgmt_table_data_get(unit, table, region_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_SCH_REGION_TYPE_CON < table->keys[0].size)
    {
        data = (dnx_data_sch_sch_alloc_region_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_REGION_TYPE_CON, 0);
        data->flow_type[0] = DNX_SCH_ELEMENT_SE_TYPE_NONE;
        data->flow_type[1] = DNX_SCH_ELEMENT_SE_TYPE_NONE;
        data->flow_type[2] = DNX_SCH_ELEMENT_SE_TYPE_NONE;
        data->flow_type[3] = DNX_SCH_ELEMENT_SE_TYPE_NONE;
        data->odd_even_mode = DNX_SCHEDULER_REGION_ODD;
    }
    if (DNX_SCH_REGION_TYPE_INTERDIG_1 < table->keys[0].size)
    {
        data = (dnx_data_sch_sch_alloc_region_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_REGION_TYPE_INTERDIG_1, 0);
        data->flow_type[0] = DNX_SCH_ELEMENT_SE_TYPE_CL;
        data->flow_type[1] = DNX_SCH_ELEMENT_SE_TYPE_FQ;
        data->flow_type[2] = DNX_SCH_ELEMENT_SE_TYPE_NONE;
        data->flow_type[3] = DNX_SCH_ELEMENT_SE_TYPE_NONE;
        data->odd_even_mode = DNX_SCHEDULER_REGION_ODD;
    }
    if (DNX_SCH_REGION_TYPE_SE < table->keys[0].size)
    {
        data = (dnx_data_sch_sch_alloc_region_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_REGION_TYPE_SE, 0);
        data->flow_type[0] = DNX_SCH_ELEMENT_SE_TYPE_CL;
        data->flow_type[1] = DNX_SCH_ELEMENT_SE_TYPE_FQ;
        data->flow_type[2] = DNX_SCH_ELEMENT_SE_TYPE_CL;
        data->flow_type[3] = DNX_SCH_ELEMENT_SE_TYPE_FQ;
        data->odd_even_mode = DNX_SCHEDULER_REGION_ODD;
    }
    if (DNX_SCH_REGION_TYPE_SE_HR < table->keys[0].size)
    {
        data = (dnx_data_sch_sch_alloc_region_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_REGION_TYPE_SE_HR, 0);
        data->flow_type[0] = DNX_SCH_ELEMENT_SE_TYPE_CL;
        data->flow_type[1] = DNX_SCH_ELEMENT_SE_TYPE_HR;
        data->flow_type[2] = DNX_SCH_ELEMENT_SE_TYPE_CL;
        data->flow_type[3] = DNX_SCH_ELEMENT_SE_TYPE_FQ;
        data->odd_even_mode = DNX_SCHEDULER_REGION_EVEN;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_sch_sch_alloc_se_per_region_type_set(
    int unit)
{
    int flow_type_index;
    int region_type_index;
    dnx_data_sch_sch_alloc_se_per_region_type_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int table_index = dnx_data_sch_sch_alloc_table_se_per_region_type;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_SCH_ALLOC_FLOW_TYPE_NOF;
    table->info_get.key_size[0] = DNX_SCH_ALLOC_FLOW_TYPE_NOF;
    table->keys[1].size = DNX_SCH_REGION_TYPE_NOF;
    table->info_get.key_size[1] = DNX_SCH_REGION_TYPE_NOF;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "4";
    table->values[2].default_val = "1";
    table->values[3].default_val = "4";
    table->values[4].default_val = "1";
    table->values[5].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_sch_alloc_se_per_region_type_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_sch_sch_alloc_table_se_per_region_type");

    
    default_data = (dnx_data_sch_sch_alloc_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = 0;
    default_data->alignment = 4;
    default_data->pattern = 1;
    default_data->pattern_size = 4;
    default_data->nof_offsets = 1;
    default_data->offset[0] = 0;
    
    for (flow_type_index = 0; flow_type_index < table->keys[0].size; flow_type_index++)
    {
        for (region_type_index = 0; region_type_index < table->keys[1].size; region_type_index++)
        {
            data = (dnx_data_sch_sch_alloc_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, flow_type_index, region_type_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (DNX_SCH_ALLOC_FLOW_TYPE_FQ < table->keys[0].size && DNX_SCH_REGION_TYPE_INTERDIG_1 < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_FQ, DNX_SCH_REGION_TYPE_INTERDIG_1);
        data->valid = 1;
        data->pattern = 0x2;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_FQ < table->keys[0].size && DNX_SCH_REGION_TYPE_SE < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_FQ, DNX_SCH_REGION_TYPE_SE);
        data->valid = 1;
        data->alignment = 2;
        data->pattern = 0x2;
        data->pattern_size = 2;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_FQ < table->keys[0].size && DNX_SCH_REGION_TYPE_SE_HR < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_FQ, DNX_SCH_REGION_TYPE_SE_HR);
        data->valid = 1;
        data->pattern = 0x8;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_CL < table->keys[0].size && DNX_SCH_REGION_TYPE_INTERDIG_1 < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_CL, DNX_SCH_REGION_TYPE_INTERDIG_1);
        data->valid = 1;
        data->pattern = 0x1;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_CL < table->keys[0].size && DNX_SCH_REGION_TYPE_SE_HR < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_CL, DNX_SCH_REGION_TYPE_SE_HR);
        data->valid = 1;
        data->alignment = 2;
        data->pattern = 0x1;
        data->pattern_size = 2;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_CL < table->keys[0].size && DNX_SCH_REGION_TYPE_SE < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_CL, DNX_SCH_REGION_TYPE_SE);
        data->valid = 1;
        data->alignment = 2;
        data->pattern = 0x1;
        data->pattern_size = 2;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL < table->keys[0].size && DNX_SCH_REGION_TYPE_INTERDIG_1 < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL, DNX_SCH_REGION_TYPE_INTERDIG_1);
        data->valid = 1;
        data->pattern = 0x3;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL < table->keys[0].size && DNX_SCH_REGION_TYPE_SE_HR < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL, DNX_SCH_REGION_TYPE_SE_HR);
        data->valid = 1;
        data->pattern = 0xc;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL < table->keys[0].size && DNX_SCH_REGION_TYPE_SE < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL, DNX_SCH_REGION_TYPE_SE);
        data->valid = 1;
        data->alignment = 2;
        data->pattern = 0x3;
        data->pattern_size = 2;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_HR < table->keys[0].size && DNX_SCH_REGION_TYPE_SE_HR < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_HR, DNX_SCH_REGION_TYPE_SE_HR);
        data->valid = 1;
        data->pattern = 0x2;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_SHARED_CL_FQ < table->keys[0].size && DNX_SCH_REGION_TYPE_SE < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_SHARED_CL_FQ, DNX_SCH_REGION_TYPE_SE);
        data->valid = 1;
        data->alignment = 2;
        data->pattern = 0x3;
        data->pattern_size = 2;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_4 < table->keys[0].size && DNX_SCH_REGION_TYPE_SE < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_4, DNX_SCH_REGION_TYPE_SE);
        data->valid = 1;
        data->pattern = 0xf;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_8 < table->keys[0].size && DNX_SCH_REGION_TYPE_SE < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_8, DNX_SCH_REGION_TYPE_SE);
        data->valid = 1;
        data->alignment = 8;
        data->pattern = 0xff;
        data->pattern_size = 8;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_sch_sch_alloc_se_set(
    int unit)
{
    int flow_type_index;
    dnx_data_sch_sch_alloc_se_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int table_index = dnx_data_sch_sch_alloc_table_se;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_SCH_ALLOC_FLOW_TYPE_NOF;
    table->info_get.key_size[0] = DNX_SCH_ALLOC_FLOW_TYPE_NOF;

    
    table->values[0].default_val = "4";
    table->values[1].default_val = "0";
    table->values[2].default_val = "4";
    table->values[3].default_val = "1";
    table->values[4].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_sch_alloc_se_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_sch_sch_alloc_table_se");

    
    default_data = (dnx_data_sch_sch_alloc_se_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->alignment = 4;
    default_data->pattern = 0;
    default_data->pattern_size = 4;
    default_data->nof_offsets = 1;
    default_data->offset[0] = 0;
    
    for (flow_type_index = 0; flow_type_index < table->keys[0].size; flow_type_index++)
    {
        data = (dnx_data_sch_sch_alloc_se_t *) dnxc_data_mgmt_table_data_get(unit, table, flow_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_sch_sch_alloc_composite_se_per_region_type_set(
    int unit)
{
    int flow_type_index;
    int region_type_index;
    dnx_data_sch_sch_alloc_composite_se_per_region_type_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int table_index = dnx_data_sch_sch_alloc_table_composite_se_per_region_type;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_SCH_ALLOC_FLOW_TYPE_NOF;
    table->info_get.key_size[0] = DNX_SCH_ALLOC_FLOW_TYPE_NOF;
    table->keys[1].size = DNX_SCH_REGION_TYPE_NOF;
    table->info_get.key_size[1] = DNX_SCH_REGION_TYPE_NOF;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "4";
    table->values[2].default_val = "1";
    table->values[3].default_val = "4";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_sch_alloc_composite_se_per_region_type_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_sch_sch_alloc_table_composite_se_per_region_type");

    
    default_data = (dnx_data_sch_sch_alloc_composite_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = 0;
    default_data->alignment = 4;
    default_data->pattern = 1;
    default_data->pattern_size = 4;
    
    for (flow_type_index = 0; flow_type_index < table->keys[0].size; flow_type_index++)
    {
        for (region_type_index = 0; region_type_index < table->keys[1].size; region_type_index++)
        {
            data = (dnx_data_sch_sch_alloc_composite_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, flow_type_index, region_type_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL < table->keys[0].size && DNX_SCH_REGION_TYPE_INTERDIG_1 < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_composite_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL, DNX_SCH_REGION_TYPE_INTERDIG_1);
        data->valid = 1;
        data->pattern = 0x3;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL < table->keys[0].size && DNX_SCH_REGION_TYPE_SE < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_composite_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL, DNX_SCH_REGION_TYPE_SE);
        data->valid = 1;
        data->alignment = 2;
        data->pattern = 0x3;
        data->pattern_size = 2;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL < table->keys[0].size && DNX_SCH_REGION_TYPE_SE_HR < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_composite_se_per_region_type_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL, DNX_SCH_REGION_TYPE_SE_HR);
        data->valid = 1;
        data->pattern = 0x5;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_sch_sch_alloc_composite_se_set(
    int unit)
{
    int flow_type_index;
    int odd_even_mode_index;
    dnx_data_sch_sch_alloc_composite_se_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_sch_alloc;
    int table_index = dnx_data_sch_sch_alloc_table_composite_se;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_SCH_ALLOC_FLOW_TYPE_NOF;
    table->info_get.key_size[0] = DNX_SCH_ALLOC_FLOW_TYPE_NOF;
    table->keys[1].size = DNX_SCHEDULER_REGION_ODD_EVEN_MODE_NOF;
    table->info_get.key_size[1] = DNX_SCHEDULER_REGION_ODD_EVEN_MODE_NOF;

    
    table->values[0].default_val = "4";
    table->values[1].default_val = "1";
    table->values[2].default_val = "4";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_sch_alloc_composite_se_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_sch_sch_alloc_table_composite_se");

    
    default_data = (dnx_data_sch_sch_alloc_composite_se_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->alignment = 4;
    default_data->pattern = 1;
    default_data->pattern_size = 4;
    
    for (flow_type_index = 0; flow_type_index < table->keys[0].size; flow_type_index++)
    {
        for (odd_even_mode_index = 0; odd_even_mode_index < table->keys[1].size; odd_even_mode_index++)
        {
            data = (dnx_data_sch_sch_alloc_composite_se_t *) dnxc_data_mgmt_table_data_get(unit, table, flow_type_index, odd_even_mode_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (DNX_SCH_ALLOC_FLOW_TYPE_COMP_HR < table->keys[0].size && DNX_SCHEDULER_REGION_EVEN < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_composite_se_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_COMP_HR, DNX_SCHEDULER_REGION_EVEN);
        data->pattern = 0xa;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_COMP_HR < table->keys[0].size && DNX_SCHEDULER_REGION_ODD < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_composite_se_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_COMP_HR, DNX_SCHEDULER_REGION_ODD);
        data->pattern = 0x0;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ < table->keys[0].size && DNX_SCHEDULER_REGION_EVEN < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_composite_se_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ, DNX_SCHEDULER_REGION_EVEN);
        data->pattern = 0xa;
    }
    if (DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ < table->keys[0].size && DNX_SCHEDULER_REGION_ODD < table->keys[1].size)
    {
        data = (dnx_data_sch_sch_alloc_composite_se_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ, DNX_SCHEDULER_REGION_ODD);
        data->pattern = 0x0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2c_a0_dnx_data_sch_dbal_flow_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_flow_bits;
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
j2c_a0_dnx_data_sch_dbal_se_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_se_bits;
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
j2c_a0_dnx_data_sch_dbal_cl_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_cl_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 15;

    
    define->data = 15;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_sch_dbal_hr_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_hr_bits;
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
j2c_a0_dnx_data_sch_dbal_interface_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_interface_bits;
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
j2c_a0_dnx_data_sch_dbal_calendar_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_calendar_bits;
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
j2c_a0_dnx_data_sch_dbal_flow_shaper_mant_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_flow_shaper_mant_bits;
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
j2c_a0_dnx_data_sch_dbal_ps_shaper_quanta_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_ps_shaper_quanta_bits;
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
j2c_a0_dnx_data_sch_dbal_ps_shaper_max_burst_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_ps_shaper_max_burst_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 17;

    
    define->data = 17;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_sch_dbal_drm_nof_links_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_drm_nof_links_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 48;

    
    define->data = 48;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2c_a0_dnx_data_sch_features_dlm_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_features;
    int feature_index = dnx_data_sch_features_dlm;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}





shr_error_e
j2c_a0_data_sch_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_sch;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_sch_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sch_general_define_port_priority_propagation_enable;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_sch_general_port_priority_propagation_enable_set;

    
    data_index = dnx_data_sch_general_port_priority_propagation_supported;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_sch_general_port_priority_propagation_supported_set;

    
    
    submodule_index = dnx_data_sch_submodule_ps;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sch_ps_define_max_port_rate_mbps;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_sch_ps_max_port_rate_mbps_set;

    

    
    
    submodule_index = dnx_data_sch_submodule_flow;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sch_flow_define_nof_hr;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_sch_flow_nof_hr_set;
    data_index = dnx_data_sch_flow_define_nof_cl;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_sch_flow_nof_cl_set;
    data_index = dnx_data_sch_flow_define_nof_se;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_sch_flow_nof_se_set;
    data_index = dnx_data_sch_flow_define_nof_flows;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_sch_flow_nof_flows_set;

    
    data_index = dnx_data_sch_flow_cl_fq_cl_fq_quartet;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_sch_flow_cl_fq_cl_fq_quartet_set;

    
    data_index = dnx_data_sch_flow_table_cl_num_to_quartet_offset;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_sch_flow_cl_num_to_quartet_offset_set;
    data_index = dnx_data_sch_flow_table_quartet_offset_to_cl_num;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_sch_flow_quartet_offset_to_cl_num_set;
    
    submodule_index = dnx_data_sch_submodule_interface;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sch_interface_define_nof_channelized_calendars;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_sch_interface_nof_channelized_calendars_set;
    data_index = dnx_data_sch_interface_define_nof_sch_interfaces;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_sch_interface_nof_sch_interfaces_set;
    data_index = dnx_data_sch_interface_define_reserved;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_sch_interface_reserved_set;

    
    data_index = dnx_data_sch_interface_non_channelized_calendar;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_sch_interface_non_channelized_calendar_set;

    
    data_index = dnx_data_sch_interface_table_shaper_resolution;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_sch_interface_shaper_resolution_set;
    
    submodule_index = dnx_data_sch_submodule_device;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_sch_device_shared_drm;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_sch_device_shared_drm_set;

    
    
    submodule_index = dnx_data_sch_submodule_sch_alloc;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_sch_sch_alloc_table_region;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_sch_sch_alloc_region_set;
    data_index = dnx_data_sch_sch_alloc_table_se_per_region_type;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_sch_sch_alloc_se_per_region_type_set;
    data_index = dnx_data_sch_sch_alloc_table_se;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_sch_sch_alloc_se_set;
    data_index = dnx_data_sch_sch_alloc_table_composite_se_per_region_type;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_sch_sch_alloc_composite_se_per_region_type_set;
    data_index = dnx_data_sch_sch_alloc_table_composite_se;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_sch_sch_alloc_composite_se_set;
    
    submodule_index = dnx_data_sch_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sch_dbal_define_flow_bits;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_sch_dbal_flow_bits_set;
    data_index = dnx_data_sch_dbal_define_se_bits;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_sch_dbal_se_bits_set;
    data_index = dnx_data_sch_dbal_define_cl_bits;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_sch_dbal_cl_bits_set;
    data_index = dnx_data_sch_dbal_define_hr_bits;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_sch_dbal_hr_bits_set;
    data_index = dnx_data_sch_dbal_define_interface_bits;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_sch_dbal_interface_bits_set;
    data_index = dnx_data_sch_dbal_define_calendar_bits;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_sch_dbal_calendar_bits_set;
    data_index = dnx_data_sch_dbal_define_flow_shaper_mant_bits;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_sch_dbal_flow_shaper_mant_bits_set;
    data_index = dnx_data_sch_dbal_define_ps_shaper_quanta_bits;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_sch_dbal_ps_shaper_quanta_bits_set;
    data_index = dnx_data_sch_dbal_define_ps_shaper_max_burst_bits;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_sch_dbal_ps_shaper_max_burst_bits_set;
    data_index = dnx_data_sch_dbal_define_drm_nof_links_max;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_sch_dbal_drm_nof_links_max_set;

    

    
    
    submodule_index = dnx_data_sch_submodule_features;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_sch_features_dlm;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_sch_features_dlm_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

