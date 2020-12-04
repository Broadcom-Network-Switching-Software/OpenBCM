

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
#include <bcm_int/dnx/algo/sch/sch_alloc_mngr.h>







static shr_error_e
j2p_a0_dnx_data_sch_general_fsm_modes_config_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_general;
    int feature_index = dnx_data_sch_general_fsm_modes_config_supported;
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
j2p_a0_dnx_data_sch_general_calrx_rxi_enable_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_general;
    int feature_index = dnx_data_sch_general_calrx_rxi_enable_supported;
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
j2p_a0_dnx_data_sch_general_port_priority_propagation_version_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_general;
    int define_index = dnx_data_sch_general_define_port_priority_propagation_version;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DNX_SCHEDULER_PORT_PRIORITY_PROPAGATION_V2;

    
    define->data = DNX_SCHEDULER_PORT_PRIORITY_PROPAGATION_V2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2p_a0_dnx_data_sch_ps_max_burst_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_ps;
    int define_index = dnx_data_sch_ps_define_max_burst;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = utilex_power_of_2(dnx_data_sch.dbal.ps_shaper_max_burst_bits_get(unit)) - utilex_power_of_2(dnx_data_sch.dbal.ps_shaper_quanta_bits_get(unit)) - 1;

    
    define->data = utilex_power_of_2(dnx_data_sch.dbal.ps_shaper_max_burst_bits_get(unit)) - utilex_power_of_2(dnx_data_sch.dbal.ps_shaper_quanta_bits_get(unit)) - 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2p_a0_dnx_data_sch_flow_cl_fq_cl_fq_quartet_set(
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
j2p_a0_dnx_data_sch_flow_flex_quartet_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int feature_index = dnx_data_sch_flow_flex_quartet_supported;
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
j2p_a0_dnx_data_sch_flow_interdigitated_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int feature_index = dnx_data_sch_flow_interdigitated_supported;
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
j2p_a0_dnx_data_sch_flow_nof_hr_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_nof_hr;
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
j2p_a0_dnx_data_sch_flow_nof_cl_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_nof_cl;
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
j2p_a0_dnx_data_sch_flow_nof_se_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_nof_se;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128*1024;

    
    define->data = 128*1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_sch_flow_nof_flows_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_nof_flows;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 384*1024;

    
    define->data = 384*1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_sch_flow_nof_flows_in_quartet_order_group_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_flow;
    int define_index = dnx_data_sch_flow_define_nof_flows_in_quartet_order_group;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8*1024;

    
    define->data = 8*1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2p_a0_dnx_data_sch_se_cl_class_profile_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_se;
    int define_index = dnx_data_sch_se_define_cl_class_profile_nof;
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
j2p_a0_dnx_data_sch_se_quartet_offset_to_cl_num_set(
    int unit)
{
    int quartet_offset_index;
    dnx_data_sch_se_quartet_offset_to_cl_num_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_se;
    int table_index = dnx_data_sch_se_table_quartet_offset_to_cl_num;
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

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sch_se_quartet_offset_to_cl_num_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_sch_se_table_quartet_offset_to_cl_num");

    
    default_data = (dnx_data_sch_se_quartet_offset_to_cl_num_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->cl_num = -1;
    
    for (quartet_offset_index = 0; quartet_offset_index < table->keys[0].size; quartet_offset_index++)
    {
        data = (dnx_data_sch_se_quartet_offset_to_cl_num_t *) dnxc_data_mgmt_table_data_get(unit, table, quartet_offset_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_sch_se_quartet_offset_to_cl_num_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->cl_num = 0;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_sch_se_quartet_offset_to_cl_num_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->cl_num = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2p_a0_dnx_data_sch_interface_non_channelized_calendar_set(
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
j2p_a0_dnx_data_sch_interface_nof_channelized_calendars_set(
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
j2p_a0_dnx_data_sch_interface_nof_sch_interfaces_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_interface;
    int define_index = dnx_data_sch_interface_define_nof_sch_interfaces;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 80;

    
    define->data = 80;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_sch_interface_reserved_set(
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
j2p_a0_dnx_data_sch_interface_shaper_resolution_set(
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
j2p_a0_dnx_data_sch_device_shared_drm_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_device;
    int feature_index = dnx_data_sch_device_shared_drm;
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
j2p_a0_dnx_data_sch_dbal_flow_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_flow_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 19;

    
    define->data = 19;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_sch_dbal_se_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_se_bits;
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
j2p_a0_dnx_data_sch_dbal_cl_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_cl_bits;
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
j2p_a0_dnx_data_sch_dbal_hr_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_hr_bits;
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
j2p_a0_dnx_data_sch_dbal_interface_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sch;
    int submodule_index = dnx_data_sch_submodule_dbal;
    int define_index = dnx_data_sch_dbal_define_interface_bits;
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
j2p_a0_dnx_data_sch_dbal_calendar_bits_set(
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
j2p_a0_dnx_data_sch_dbal_flow_shaper_mant_bits_set(
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
j2p_a0_dnx_data_sch_dbal_ps_shaper_quanta_bits_set(
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
j2p_a0_dnx_data_sch_dbal_ps_shaper_max_burst_bits_set(
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
j2p_a0_dnx_data_sch_features_dlm_set(
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
j2p_a0_data_sch_attach(
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

    
    data_index = dnx_data_sch_general_define_port_priority_propagation_version;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_sch_general_port_priority_propagation_version_set;

    
    data_index = dnx_data_sch_general_fsm_modes_config_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_sch_general_fsm_modes_config_supported_set;
    data_index = dnx_data_sch_general_calrx_rxi_enable_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_sch_general_calrx_rxi_enable_supported_set;

    
    
    submodule_index = dnx_data_sch_submodule_ps;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sch_ps_define_max_burst;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_sch_ps_max_burst_set;

    

    
    
    submodule_index = dnx_data_sch_submodule_flow;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sch_flow_define_nof_hr;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_sch_flow_nof_hr_set;
    data_index = dnx_data_sch_flow_define_nof_cl;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_sch_flow_nof_cl_set;
    data_index = dnx_data_sch_flow_define_nof_se;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_sch_flow_nof_se_set;
    data_index = dnx_data_sch_flow_define_nof_flows;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_sch_flow_nof_flows_set;
    data_index = dnx_data_sch_flow_define_nof_flows_in_quartet_order_group;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_sch_flow_nof_flows_in_quartet_order_group_set;

    
    data_index = dnx_data_sch_flow_cl_fq_cl_fq_quartet;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_sch_flow_cl_fq_cl_fq_quartet_set;
    data_index = dnx_data_sch_flow_flex_quartet_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_sch_flow_flex_quartet_supported_set;
    data_index = dnx_data_sch_flow_interdigitated_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_sch_flow_interdigitated_supported_set;

    
    
    submodule_index = dnx_data_sch_submodule_se;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sch_se_define_cl_class_profile_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_sch_se_cl_class_profile_nof_set;

    

    
    data_index = dnx_data_sch_se_table_quartet_offset_to_cl_num;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_sch_se_quartet_offset_to_cl_num_set;
    
    submodule_index = dnx_data_sch_submodule_interface;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sch_interface_define_nof_channelized_calendars;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_sch_interface_nof_channelized_calendars_set;
    data_index = dnx_data_sch_interface_define_nof_sch_interfaces;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_sch_interface_nof_sch_interfaces_set;
    data_index = dnx_data_sch_interface_define_reserved;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_sch_interface_reserved_set;

    
    data_index = dnx_data_sch_interface_non_channelized_calendar;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_sch_interface_non_channelized_calendar_set;

    
    data_index = dnx_data_sch_interface_table_shaper_resolution;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_sch_interface_shaper_resolution_set;
    
    submodule_index = dnx_data_sch_submodule_device;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_sch_device_shared_drm;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_sch_device_shared_drm_set;

    
    
    submodule_index = dnx_data_sch_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sch_dbal_define_flow_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_sch_dbal_flow_bits_set;
    data_index = dnx_data_sch_dbal_define_se_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_sch_dbal_se_bits_set;
    data_index = dnx_data_sch_dbal_define_cl_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_sch_dbal_cl_bits_set;
    data_index = dnx_data_sch_dbal_define_hr_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_sch_dbal_hr_bits_set;
    data_index = dnx_data_sch_dbal_define_interface_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_sch_dbal_interface_bits_set;
    data_index = dnx_data_sch_dbal_define_calendar_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_sch_dbal_calendar_bits_set;
    data_index = dnx_data_sch_dbal_define_flow_shaper_mant_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_sch_dbal_flow_shaper_mant_bits_set;
    data_index = dnx_data_sch_dbal_define_ps_shaper_quanta_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_sch_dbal_ps_shaper_quanta_bits_set;
    data_index = dnx_data_sch_dbal_define_ps_shaper_max_burst_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_sch_dbal_ps_shaper_max_burst_bits_set;

    

    
    
    submodule_index = dnx_data_sch_submodule_features;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_sch_features_dlm;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_sch_features_dlm_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

