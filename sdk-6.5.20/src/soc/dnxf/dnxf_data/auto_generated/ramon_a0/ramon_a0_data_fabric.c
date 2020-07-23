

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FABRIC

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_property.h>
#include <bcm/types.h>
#include <bcm_int/common/rx.h>
#include <appl/diag/dnxc/diag_dnxc_fabric.h>
#include <soc/dnxc/fabric.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_drv.h>







static shr_error_e
ramon_a0_dnxf_data_fabric_general_is_multi_stage_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_general;
    int feature_index = dnxf_data_fabric_general_is_multi_stage_supported;
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
ramon_a0_dnxf_data_fabric_general_max_nof_pipes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_general;
    int define_index = dnxf_data_fabric_general_define_max_nof_pipes;
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
ramon_a0_dnxf_data_fabric_general_device_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_general;
    int define_index = dnxf_data_fabric_general_define_device_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = soc_dnxf_fabric_device_mode_single_stage_fe2;

    
    define->data = soc_dnxf_fabric_device_mode_single_stage_fe2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FABRIC_DEVICE_MODE;
    define->property.doc = 
        "\n"
        "Sets the operationl mode of the device\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 3;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "SINGLE_STAGE_FE2";
    define->property.mapping[0].val = soc_dnxf_fabric_device_mode_single_stage_fe2;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "MULTI_STAGE_FE2";
    define->property.mapping[1].val = soc_dnxf_fabric_device_mode_multi_stage_fe2;
    define->property.mapping[2].name = "MULTI_STAGE_FE13";
    define->property.mapping[2].val = soc_dnxf_fabric_device_mode_multi_stage_fe13;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_general_local_routing_enable_uc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_general;
    int define_index = dnxf_data_fabric_general_define_local_routing_enable_uc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FABRIC_LOCAL_ROUTING_ENABLE;
    define->property.doc = 
        "\n"
        "Enables unicast local routing from FE1 to FE3\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "uc";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_general_local_routing_enable_mc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_general;
    int define_index = dnxf_data_fabric_general_define_local_routing_enable_mc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FABRIC_LOCAL_ROUTING_ENABLE;
    define->property.doc = 
        "\n"
        "Enables multicast local routing from FE1 to FE3\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "mc";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
ramon_a0_dnxf_data_fabric_pipes_max_nof_pipes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_pipes;
    int define_index = dnxf_data_fabric_pipes_define_max_nof_pipes;
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
ramon_a0_dnxf_data_fabric_pipes_nof_valid_mapping_options_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_pipes;
    int define_index = dnxf_data_fabric_pipes_define_nof_valid_mapping_options;
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
ramon_a0_dnxf_data_fabric_pipes_nof_pipes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_pipes;
    int define_index = dnxf_data_fabric_pipes_define_nof_pipes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FABRIC_NUM_PIPES;
    define->property.doc = 
        "\n"
        "Configure number of fabric pipes\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 1;
    define->property.range_max = 3;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_pipes_system_contains_multiple_pipe_device_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_pipes;
    int define_index = dnxf_data_fabric_pipes_define_system_contains_multiple_pipe_device;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_SYSTEM_CONTAINS_MULTIPLE_PIPE_DEVICE;
    define->property.doc = 
        "\n"
        "Determines if there is a multiple pipe device in the system\n"
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
ramon_a0_dnxf_data_fabric_pipes_system_tdm_priority_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_pipes;
    int define_index = dnxf_data_fabric_pipes_define_system_tdm_priority;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FABRIC_TDM_PRIORITY_MIN;
    define->property.doc = 
        "\n"
        "Mark a CGM fabric priority as TDM (should be globally configured over the system).\n"
        "'NONE' represents a system that does not support TDM and all the priorities can be used as regular priorities.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "NONE";
    define->property.mapping[0].val = SOC_DNXF_FABRIC_TDM_PRIORITY_NONE;
    define->property.mapping[1].name = "3";
    define->property.mapping[1].val = soc_dnxf_fabric_priority_3;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
ramon_a0_dnxf_data_fabric_pipes_map_set(
    int unit)
{
    dnxf_data_fabric_pipes_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_pipes;
    int table_index = dnxf_data_fabric_pipes_table_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "0, 0, 0, 0";
    table->values[1].default_val = "0, 0, 0, 0";
    table->values[2].default_val = "SINGLE PIPE";
    table->values[3].default_val = "soc_dnxc_fabric_pipe_map_single";
    table->values[4].default_val = "-1";
    
    DNXC_DATA_ALLOC(table->data, dnxf_data_fabric_pipes_map_t, (1 + 1  ), "data of dnxf_data_fabric_pipes_table_map");

    
    default_data = (dnxf_data_fabric_pipes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    SHR_RANGE_VERIFY(4, 0, DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->uc[0] = 0;
    default_data->uc[1] = 0;
    default_data->uc[2] = 0;
    default_data->uc[3] = 0;
    SHR_RANGE_VERIFY(4, 0, DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->mc[0] = 0;
    default_data->mc[1] = 0;
    default_data->mc[2] = 0;
    default_data->mc[3] = 0;
    default_data->name = "SINGLE PIPE";
    default_data->type = soc_dnxc_fabric_pipe_map_single;
    default_data->min_hp_mc = -1;
    
    data = (dnxf_data_fabric_pipes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->property.name = "fabric_pipe_map";
    table->property.doc =
        "\n"
        "Assign traffic cast (unicast/multicast) and priority (0/1/2/3) to a pipe.\n"
        "fabric_pipe_map=0/1/2\n"
        "Assigning a specific traffic cast is done by a suffix uc/mc and a number which represents the cell priority.\n"
        "Not all the options supported, please find the supported options in UM\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnxf_data_fabric_pipes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxf_data_property_fabric_pipes_map_read(unit, data));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_pipes_valid_map_config_set(
    int unit)
{
    int mapping_option_idx_index;
    dnxf_data_fabric_pipes_valid_map_config_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_pipes;
    int table_index = dnxf_data_fabric_pipes_table_valid_map_config;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnxf_data_fabric.pipes.nof_valid_mapping_options_get(unit);
    table->info_get.key_size[0] = dnxf_data_fabric.pipes.nof_valid_mapping_options_get(unit);

    
    table->values[0].default_val = "1";
    table->values[1].default_val = "0, 0, 0, 0";
    table->values[2].default_val = "0, 0, 0, 0";
    table->values[3].default_val = "SINGLE PIPE";
    table->values[4].default_val = "soc_dnxc_fabric_pipe_map_single";
    table->values[5].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_fabric_pipes_valid_map_config_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_fabric_pipes_table_valid_map_config");

    
    default_data = (dnxf_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nof_pipes = 1;
    SHR_RANGE_VERIFY(4, 0, DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->uc[0] = 0;
    default_data->uc[1] = 0;
    default_data->uc[2] = 0;
    default_data->uc[3] = 0;
    SHR_RANGE_VERIFY(4, 0, DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->mc[0] = 0;
    default_data->mc[1] = 0;
    default_data->mc[2] = 0;
    default_data->mc[3] = 0;
    default_data->name = "SINGLE PIPE";
    default_data->type = soc_dnxc_fabric_pipe_map_single;
    default_data->min_hp_mc = -1;
    
    for (mapping_option_idx_index = 0; mapping_option_idx_index < table->keys[0].size; mapping_option_idx_index++)
    {
        data = (dnxf_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, mapping_option_idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnxf_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->nof_pipes = 1;
        SHR_RANGE_VERIFY(4, 0, DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->uc[0] = 0;
        data->uc[1] = 0;
        data->uc[2] = 0;
        data->uc[3] = 0;
        SHR_RANGE_VERIFY(4, 0, DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->mc[0] = 0;
        data->mc[1] = 0;
        data->mc[2] = 0;
        data->mc[3] = 0;
        data->name = "SINGLE PIPE";
        data->type = soc_dnxc_fabric_pipe_map_single;
        data->min_hp_mc = -1;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnxf_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->nof_pipes = 2;
        SHR_RANGE_VERIFY(4, 0, DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->uc[0] = 0;
        data->uc[1] = 0;
        data->uc[2] = 0;
        data->uc[3] = 0;
        SHR_RANGE_VERIFY(4, 0, DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->mc[0] = 1;
        data->mc[1] = 1;
        data->mc[2] = 1;
        data->mc[3] = 1;
        data->name = "UC,MC";
        data->type = soc_dnxc_fabric_pipe_map_dual_uc_mc;
        data->min_hp_mc = -1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnxf_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->nof_pipes = 2;
        SHR_RANGE_VERIFY(4, 0, DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->uc[0] = 0;
        data->uc[1] = 0;
        data->uc[2] = 0;
        data->uc[3] = 1;
        SHR_RANGE_VERIFY(4, 0, DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->mc[0] = 0;
        data->mc[1] = 0;
        data->mc[2] = 0;
        data->mc[3] = 1;
        data->name = "Non-TDM,TDM";
        data->type = soc_dnxc_fabric_pipe_map_dual_tdm_non_tdm;
        data->min_hp_mc = -1;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnxf_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->nof_pipes = 3;
        SHR_RANGE_VERIFY(4, 0, DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->uc[0] = 0;
        data->uc[1] = 0;
        data->uc[2] = 0;
        data->uc[3] = 0;
        SHR_RANGE_VERIFY(4, 0, DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->mc[0] = 2;
        data->mc[1] = 1;
        data->mc[2] = 1;
        data->mc[3] = 1;
        data->name = "UC,HP MC (1,2,3),LP MC";
        data->type = soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc;
        data->min_hp_mc = 1;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnxf_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->nof_pipes = 3;
        SHR_RANGE_VERIFY(4, 0, DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->uc[0] = 0;
        data->uc[1] = 0;
        data->uc[2] = 0;
        data->uc[3] = 0;
        SHR_RANGE_VERIFY(4, 0, DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->mc[0] = 2;
        data->mc[1] = 2;
        data->mc[2] = 1;
        data->mc[3] = 1;
        data->name = "UC,HP MC (2,3),LP MC";
        data->type = soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc;
        data->min_hp_mc = 2;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnxf_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->nof_pipes = 3;
        SHR_RANGE_VERIFY(4, 0, DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->uc[0] = 0;
        data->uc[1] = 0;
        data->uc[2] = 0;
        data->uc[3] = 0;
        SHR_RANGE_VERIFY(4, 0, DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->mc[0] = 2;
        data->mc[1] = 2;
        data->mc[2] = 2;
        data->mc[3] = 1;
        data->name = "UC,HP MC (3),LP MC";
        data->type = soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc;
        data->min_hp_mc = 3;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnxf_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->nof_pipes = 3;
        SHR_RANGE_VERIFY(4, 0, DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->uc[0] = 0;
        data->uc[1] = 0;
        data->uc[2] = 0;
        data->uc[3] = 2;
        SHR_RANGE_VERIFY(4, 0, DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->mc[0] = 1;
        data->mc[1] = 1;
        data->mc[2] = 1;
        data->mc[3] = 2;
        data->name = "UC,MC,TDM";
        data->type = soc_dnxc_fabric_pipe_map_triple_uc_mc_tdm;
        data->min_hp_mc = -1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
ramon_a0_dnxf_data_fabric_multicast_table_row_size_in_uint32_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_multicast;
    int define_index = dnxf_data_fabric_multicast_define_table_row_size_in_uint32;
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
ramon_a0_dnxf_data_fabric_multicast_id_range_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_multicast;
    int define_index = dnxf_data_fabric_multicast_define_id_range;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = soc_dnxf_multicast_table_mode_256k_half;

    
    define->data = soc_dnxf_multicast_table_mode_256k_half;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FE_MC_ID_RANGE;
    define->property.doc = 
        "\n"
        "Specifies the MC IDs range.\n"
        "1.  64K - Supports eight queries at each clock, maximum of 64K multicast IDs, and up to 192 next-stage devices.\n"
        "2.  128K_HALF - Supports eight queries at each clock, maximum of 128K multicast IDs, and up to 96 next- stage devices.\n"
        "3.  128K - Supports four queries at each clock, maximum of 128K multicast IDs, and up to 192 next-stage devices.\n"
        "4.  256K_HALF - Supports four queries at each clock, maximum of 256K multicast IDs, and up to 96 next- stage devices.\n"
        "5.  256K - Supports two queries at each clock, maximum of 256K multicast IDs, and up to 192 next-stage devices.\n"
        "6.  512K_HALF - Supports two queries at each clock, maximum of 512K multicast IDs, and up to 96 next- stage devices.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 6;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "64K";
    define->property.mapping[0].val = soc_dnxf_multicast_table_mode_64k;
    define->property.mapping[1].name = "128K_HALF";
    define->property.mapping[1].val = soc_dnxf_multicast_table_mode_128k_half;
    define->property.mapping[2].name = "128K";
    define->property.mapping[2].val = soc_dnxf_multicast_table_mode_128k;
    define->property.mapping[3].name = "256K_HALF";
    define->property.mapping[3].val = soc_dnxf_multicast_table_mode_256k_half;
    define->property.mapping[3].is_default = 1 ;
    define->property.mapping[4].name = "256K";
    define->property.mapping[4].val = soc_dnxf_multicast_table_mode_256k;
    define->property.mapping[5].name = "512K_HALF";
    define->property.mapping[5].val = soc_dnxf_multicast_table_mode_512k_half;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_multicast_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_multicast;
    int define_index = dnxf_data_fabric_multicast_define_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = soc_dnxf_multicast_mode_direct;

    
    define->data = soc_dnxf_multicast_mode_direct;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FABRIC_MULTICAST_MODE;
    define->property.doc = 
        "\n"
        "Specifies the MC mode.\n"
        "Direct multicast is valid in a single-stage systems with a highest FAP-ID less then 192/96 (according to MC table mode).\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "DIRECT";
    define->property.mapping[0].val = soc_dnxf_multicast_mode_direct;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "INDIRECT";
    define->property.mapping[1].val = soc_dnxf_multicast_mode_indirect;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_multicast_priority_map_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_multicast;
    int define_index = dnxf_data_fabric_multicast_define_priority_map_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FE_MC_PRIORITY_MAP_ENABLE;
    define->property.doc = 
        "\n"
        "Enable / Disable mapping internal multicast priority according to MC-ID ranges.\n"
        "1: Map MC-ID to fabric priority.\n"
        "0: Extract MC cell priority from cell header.\n"
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
ramon_a0_dnxf_data_fabric_fifos_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_fifos;
    int define_index = dnxf_data_fabric_fifos_define_granularity;
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
ramon_a0_dnxf_data_fabric_fifos_min_depth_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_fifos;
    int define_index = dnxf_data_fabric_fifos_define_min_depth;
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
ramon_a0_dnxf_data_fabric_fifos_max_unused_resources_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_fifos;
    int define_index = dnxf_data_fabric_fifos_define_max_unused_resources;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 36;

    
    define->data = 36;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_fifos_rx_resources_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_fifos;
    int define_index = dnxf_data_fabric_fifos_define_rx_resources;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 768;

    
    define->data = 768;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_fifos_mid_resources_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_fifos;
    int define_index = dnxf_data_fabric_fifos_define_mid_resources;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4608;

    
    define->data = 4608;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_fifos_tx_resources_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_fifos;
    int define_index = dnxf_data_fabric_fifos_define_tx_resources;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4096;

    
    define->data = 4096;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_fifos_rx_full_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_fifos;
    int define_index = dnxf_data_fabric_fifos_define_rx_full_offset;
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
ramon_a0_dnxf_data_fabric_fifos_mid_full_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_fifos;
    int define_index = dnxf_data_fabric_fifos_define_mid_full_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12;

    
    define->data = 12;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_fifos_tx_full_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_fifos;
    int define_index = dnxf_data_fabric_fifos_define_tx_full_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12;

    
    define->data = 12;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
ramon_a0_dnxf_data_fabric_fifos_rx_depth_per_pipe_set(
    int unit)
{
    int pipe_id_index;
    dnxf_data_fabric_fifos_rx_depth_per_pipe_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_fifos;
    int table_index = dnxf_data_fabric_fifos_table_rx_depth_per_pipe;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnxf_data_fabric.pipes.nof_pipes_get(unit);
    table->info_get.key_size[0] = dnxf_data_fabric.pipes.nof_pipes_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_fabric_fifos_rx_depth_per_pipe_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_fabric_fifos_table_rx_depth_per_pipe");

    
    default_data = (dnxf_data_fabric_fifos_rx_depth_per_pipe_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->fe1 = 0;
    default_data->fe3 = 0;
    
    for (pipe_id_index = 0; pipe_id_index < table->keys[0].size; pipe_id_index++)
    {
        data = (dnxf_data_fabric_fifos_rx_depth_per_pipe_t *) dnxc_data_mgmt_table_data_get(unit, table, pipe_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "fabric_link_fifo_size_rx";
    table->property.doc =
        "\n"
        "Get RX FIFO depth per pipe\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (pipe_id_index = 0; pipe_id_index < table->keys[0].size; pipe_id_index++)
    {
        data = (dnxf_data_fabric_fifos_rx_depth_per_pipe_t *) dnxc_data_mgmt_table_data_get(unit, table, pipe_id_index, 0);
        SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_rx_depth_per_pipe_read(unit, pipe_id_index, data));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_fifos_mid_depth_per_pipe_set(
    int unit)
{
    int pipe_id_index;
    dnxf_data_fabric_fifos_mid_depth_per_pipe_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_fifos;
    int table_index = dnxf_data_fabric_fifos_table_mid_depth_per_pipe;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnxf_data_fabric.pipes.nof_pipes_get(unit);
    table->info_get.key_size[0] = dnxf_data_fabric.pipes.nof_pipes_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_fabric_fifos_mid_depth_per_pipe_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_fabric_fifos_table_mid_depth_per_pipe");

    
    default_data = (dnxf_data_fabric_fifos_mid_depth_per_pipe_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->fe1_nlr = 0;
    default_data->fe3_nlr = 0;
    default_data->fe1_lr = 0;
    default_data->fe3_lr = 0;
    
    for (pipe_id_index = 0; pipe_id_index < table->keys[0].size; pipe_id_index++)
    {
        data = (dnxf_data_fabric_fifos_mid_depth_per_pipe_t *) dnxc_data_mgmt_table_data_get(unit, table, pipe_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "fabric_link_fifo_size_mid";
    table->property.doc =
        "\n"
        "Get MID FIFO depth per pipe\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (pipe_id_index = 0; pipe_id_index < table->keys[0].size; pipe_id_index++)
    {
        data = (dnxf_data_fabric_fifos_mid_depth_per_pipe_t *) dnxc_data_mgmt_table_data_get(unit, table, pipe_id_index, 0);
        SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_mid_depth_per_pipe_read(unit, pipe_id_index, data));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_fifos_tx_depth_per_pipe_set(
    int unit)
{
    int pipe_id_index;
    dnxf_data_fabric_fifos_tx_depth_per_pipe_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_fifos;
    int table_index = dnxf_data_fabric_fifos_table_tx_depth_per_pipe;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnxf_data_fabric.pipes.nof_pipes_get(unit);
    table->info_get.key_size[0] = dnxf_data_fabric.pipes.nof_pipes_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_fabric_fifos_tx_depth_per_pipe_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_fabric_fifos_table_tx_depth_per_pipe");

    
    default_data = (dnxf_data_fabric_fifos_tx_depth_per_pipe_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->fe1_nlr = 0;
    default_data->fe3_nlr = 0;
    default_data->fe1_lr = 0;
    default_data->fe3_lr = 0;
    
    for (pipe_id_index = 0; pipe_id_index < table->keys[0].size; pipe_id_index++)
    {
        data = (dnxf_data_fabric_fifos_tx_depth_per_pipe_t *) dnxc_data_mgmt_table_data_get(unit, table, pipe_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "fabric_link_fifo_size_tx";
    table->property.doc =
        "\n"
        "Get TX FIFO depth per pipe\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (pipe_id_index = 0; pipe_id_index < table->keys[0].size; pipe_id_index++)
    {
        data = (dnxf_data_fabric_fifos_tx_depth_per_pipe_t *) dnxc_data_mgmt_table_data_get(unit, table, pipe_id_index, 0);
        SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_tx_depth_per_pipe_read(unit, pipe_id_index, data));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
ramon_a0_dnxf_data_fabric_congestion_nof_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_congestion;
    int define_index = dnxf_data_fabric_congestion_define_nof_profiles;
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
ramon_a0_dnxf_data_fabric_congestion_nof_threshold_priorities_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_congestion;
    int define_index = dnxf_data_fabric_congestion_define_nof_threshold_priorities;
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
ramon_a0_dnxf_data_fabric_congestion_nof_threshold_levels_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_congestion;
    int define_index = dnxf_data_fabric_congestion_define_nof_threshold_levels;
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
ramon_a0_dnxf_data_fabric_congestion_nof_threshold_index_dimensions_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_congestion;
    int define_index = dnxf_data_fabric_congestion_define_nof_threshold_index_dimensions;
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
ramon_a0_dnxf_data_fabric_congestion_nof_thresholds_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_congestion;
    int define_index = dnxf_data_fabric_congestion_define_nof_thresholds;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 37;

    
    define->data = 37;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_congestion_nof_dfl_banks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_congestion;
    int define_index = dnxf_data_fabric_congestion_define_nof_dfl_banks;
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
ramon_a0_dnxf_data_fabric_congestion_nof_dfl_sub_banks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_congestion;
    int define_index = dnxf_data_fabric_congestion_define_nof_dfl_sub_banks;
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
ramon_a0_dnxf_data_fabric_congestion_nof_dfl_bank_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_congestion;
    int define_index = dnxf_data_fabric_congestion_define_nof_dfl_bank_entries;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3600;

    
    define->data = 3600;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_congestion_nof_rci_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_congestion;
    int define_index = dnxf_data_fabric_congestion_define_nof_rci_bits;
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
ramon_a0_dnxf_data_fabric_congestion_thresholds_info_set(
    int unit)
{
    int threshold_id_index;
    dnxf_data_fabric_congestion_thresholds_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_congestion;
    int table_index = dnxf_data_fabric_congestion_table_thresholds_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnxf_data_fabric.congestion.nof_thresholds_get(unit);
    table->info_get.key_size[0] = dnxf_data_fabric.congestion.nof_thresholds_get(unit);

    
    table->values[0].default_val = "MID TAG DROP";
    table->values[1].default_val = "soc_dnxf_threshold_group_drop_option";
    table->values[2].default_val = "soc_dnxf_threshold_stage_rx_option";
    table->values[3].default_val = "_shr_dnxf_cgm_index_type_pipe";
    table->values[4].default_val = "_SHR_FABRIC_TH_FLAG_SUPPORT_ALL";
    table->values[5].default_val = "1";
    table->values[6].default_val = "_SHR_FABRIC_CGM_VALIDITY_NONE";
    table->values[7].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_fabric_congestion_thresholds_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_fabric_congestion_table_thresholds_info");

    
    default_data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->threshold_str = "MID TAG DROP";
    default_data->threshold_group = soc_dnxf_threshold_group_drop_option;
    default_data->threshold_stage = soc_dnxf_threshold_stage_rx_option;
    default_data->index_type = _shr_dnxf_cgm_index_type_pipe;
    default_data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_ALL;
    default_data->is_tx = 1;
    default_data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
    default_data->is_to_be_displayed = 1;
    
    for (threshold_id_index = 0; threshold_id_index < table->keys[0].size; threshold_id_index++)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, threshold_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (_SHR_FABRIC_MID_TAG_DROP_CLASS_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_MID_TAG_DROP_CLASS_TH, 0);
        data->threshold_str = "MID TAG DROP";
        data->threshold_group = soc_dnxf_threshold_group_drop_option;
        data->threshold_stage = soc_dnxf_threshold_stage_middle_option;
        data->index_type = _shr_dnxf_cgm_index_type_cast_prio;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_TX_LINK_LOAD_DROP_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_TX_LINK_LOAD_DROP_PIPE_TH, 0);
        data->threshold_str = "TX LINK LOAD DROP";
        data->threshold_group = soc_dnxf_threshold_group_drop_option;
        data->threshold_stage = soc_dnxf_threshold_stage_tx_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_TX_FRAG_GUARANTEED_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_TX_FRAG_GUARANTEED_PIPE_TH, 0);
        data->threshold_str = "TX FRAG GUARANTEED";
        data->threshold_group = soc_dnxf_threshold_group_drop_option;
        data->threshold_stage = soc_dnxf_threshold_stage_tx_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_SHARED_DROP_CLASS_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_SHARED_DROP_CLASS_TH, 0);
        data->threshold_str = "SHARED DROP";
        data->threshold_group = soc_dnxf_threshold_group_drop_option;
        data->threshold_stage = soc_dnxf_threshold_stage_shared_option;
        data->index_type = _shr_dnxf_cgm_index_type_cast_prio;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NOT_PROFILE_DEPENDENT;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_SHARED_MC_COPIES_DROP_PRIO_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_SHARED_MC_COPIES_DROP_PRIO_TH, 0);
        data->threshold_str = "SHARED MC COPIES DROP";
        data->threshold_group = soc_dnxf_threshold_group_drop_option;
        data->threshold_stage = soc_dnxf_threshold_stage_shared_option;
        data->index_type = _shr_dnxf_cgm_index_type_priority;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_MID_FRAG_DROP_CLASS_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_MID_FRAG_DROP_CLASS_TH, 0);
        data->threshold_str = "MID FRAG DROP";
        data->threshold_group = soc_dnxf_threshold_group_drop_option;
        data->threshold_stage = soc_dnxf_threshold_stage_middle_option;
        data->index_type = _shr_dnxf_cgm_index_type_cast_prio;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_SHARED_BANK_MC_DROP_PRIO_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_SHARED_BANK_MC_DROP_PRIO_TH, 0);
        data->threshold_str = "SHARED BANK MC DROP";
        data->threshold_group = soc_dnxf_threshold_group_drop_option;
        data->threshold_stage = soc_dnxf_threshold_stage_shared_option;
        data->index_type = _shr_dnxf_cgm_index_type_priority;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NOT_PROFILE_DEPENDENT;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_TX_TAG_DROP_CLASS_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_TX_TAG_DROP_CLASS_TH, 0);
        data->threshold_str = "TX TAG DROP";
        data->threshold_group = soc_dnxf_threshold_group_drop_option;
        data->threshold_stage = soc_dnxf_threshold_stage_tx_option;
        data->index_type = _shr_dnxf_cgm_index_type_cast_prio;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_ALL;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_TX_FRAG_DROP_CLASS_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_TX_FRAG_DROP_CLASS_TH, 0);
        data->threshold_str = "TX FRAG DROP";
        data->threshold_group = soc_dnxf_threshold_group_drop_option;
        data->threshold_stage = soc_dnxf_threshold_stage_tx_option;
        data->index_type = _shr_dnxf_cgm_index_type_cast_prio;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_ALL;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_TX_TAG_GCI_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_TX_TAG_GCI_PIPE_TH, 0);
        data->threshold_str = "TX TAG CGI";
        data->threshold_group = soc_dnxf_threshold_group_gci_option;
        data->threshold_stage = soc_dnxf_threshold_stage_tx_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe_level;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_ALL;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_TX_FRAG_GCI_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_TX_FRAG_GCI_PIPE_TH, 0);
        data->threshold_str = "TX FRAG GCI";
        data->threshold_group = soc_dnxf_threshold_group_gci_option;
        data->threshold_stage = soc_dnxf_threshold_stage_tx_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe_level;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_ALL;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_MID_TAG_GCI_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_MID_TAG_GCI_PIPE_TH, 0);
        data->threshold_str = "MID TAG GCI";
        data->threshold_group = soc_dnxf_threshold_group_gci_option;
        data->threshold_stage = soc_dnxf_threshold_stage_middle_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe_level;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_MID_FRAG_GCI_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_MID_FRAG_GCI_PIPE_TH, 0);
        data->threshold_str = "MID FRAG GCI";
        data->threshold_group = soc_dnxf_threshold_group_gci_option;
        data->threshold_stage = soc_dnxf_threshold_stage_middle_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe_level;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_SHARED_GCI_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_SHARED_GCI_PIPE_TH, 0);
        data->threshold_str = "SHARED GCI";
        data->threshold_group = soc_dnxf_threshold_group_gci_option;
        data->threshold_stage = soc_dnxf_threshold_stage_shared_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe_level;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NOT_PROFILE_DEPENDENT;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_SHARED_FRAG_COPIES_GCI_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_SHARED_FRAG_COPIES_GCI_PIPE_TH, 0);
        data->threshold_str = "SHARED FRAG COPIES GCI";
        data->threshold_group = soc_dnxf_threshold_group_gci_option;
        data->threshold_stage = soc_dnxf_threshold_stage_shared_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe_level;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NOT_PROFILE_DEPENDENT;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_TX_FRAG_GUARANTEED_RCI_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_TX_FRAG_GUARANTEED_RCI_PIPE_TH, 0);
        data->threshold_str = "TX FRAG GUARANTEED RCI";
        data->threshold_group = soc_dnxf_threshold_group_rci_option;
        data->threshold_stage = soc_dnxf_threshold_stage_tx_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe_level;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_TX_LINK_LOAD_RCI_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_TX_LINK_LOAD_RCI_PIPE_TH, 0);
        data->threshold_str = "TX LINK LOAD RCI";
        data->threshold_group = soc_dnxf_threshold_group_rci_option;
        data->threshold_stage = soc_dnxf_threshold_stage_tx_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe_level;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_TX_TAG_RCI_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_TX_TAG_RCI_PIPE_TH, 0);
        data->threshold_str = "TX TAG RCI";
        data->threshold_group = soc_dnxf_threshold_group_rci_option;
        data->threshold_stage = soc_dnxf_threshold_stage_tx_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe_level;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_ALL;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_TX_FRAG_RCI_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_TX_FRAG_RCI_PIPE_TH, 0);
        data->threshold_str = "TX FRAG RCI";
        data->threshold_group = soc_dnxf_threshold_group_rci_option;
        data->threshold_stage = soc_dnxf_threshold_stage_tx_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe_level;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_ALL;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_MID_TAG_RCI_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_MID_TAG_RCI_PIPE_TH, 0);
        data->threshold_str = "MID TAG RCI";
        data->threshold_group = soc_dnxf_threshold_group_rci_option;
        data->threshold_stage = soc_dnxf_threshold_stage_middle_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe_level;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_MID_FRAG_RCI_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_MID_FRAG_RCI_PIPE_TH, 0);
        data->threshold_str = "MID FRAG RCI";
        data->threshold_group = soc_dnxf_threshold_group_rci_option;
        data->threshold_stage = soc_dnxf_threshold_stage_middle_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe_level;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_SHARED_RCI_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_SHARED_RCI_PIPE_TH, 0);
        data->threshold_str = "SHARED RCI";
        data->threshold_group = soc_dnxf_threshold_group_rci_option;
        data->threshold_stage = soc_dnxf_threshold_stage_shared_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe_level;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NOT_PROFILE_DEPENDENT;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_MID_MC_COPIES_FC_PRIO_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_MID_MC_COPIES_FC_PRIO_TH, 0);
        data->threshold_str = "MID MC COPIES FC";
        data->threshold_group = soc_dnxf_threshold_group_fc_option;
        data->threshold_stage = soc_dnxf_threshold_stage_middle_option;
        data->index_type = _shr_dnxf_cgm_index_type_priority;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NOT_PROFILE_DEPENDENT;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_MID_TAG_FC_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_MID_TAG_FC_PIPE_TH, 0);
        data->threshold_str = "MID TAG FC";
        data->threshold_group = soc_dnxf_threshold_group_fc_option;
        data->threshold_stage = soc_dnxf_threshold_stage_middle_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_MID_FRAG_FC_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_MID_FRAG_FC_PIPE_TH, 0);
        data->threshold_str = "MID FRAG FC";
        data->threshold_group = soc_dnxf_threshold_group_fc_option;
        data->threshold_stage = soc_dnxf_threshold_stage_middle_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_SHARED_FC_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_SHARED_FC_PIPE_TH, 0);
        data->threshold_str = "SHARED FC";
        data->threshold_group = soc_dnxf_threshold_group_fc_option;
        data->threshold_stage = soc_dnxf_threshold_stage_shared_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NOT_PROFILE_DEPENDENT;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_TX_TAG_FC_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_TX_TAG_FC_PIPE_TH, 0);
        data->threshold_str = "TX TAG FC";
        data->threshold_group = soc_dnxf_threshold_group_fc_option;
        data->threshold_stage = soc_dnxf_threshold_stage_tx_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_ALL;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_TX_FRAG_FC_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_TX_FRAG_FC_PIPE_TH, 0);
        data->threshold_str = "TX FRAG FC";
        data->threshold_group = soc_dnxf_threshold_group_fc_option;
        data->threshold_stage = soc_dnxf_threshold_stage_tx_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_ALL;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_MID_MASK_FC_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_MID_MASK_FC_PIPE_TH, 0);
        data->threshold_str = "MID MASK FC";
        data->threshold_group = soc_dnxf_threshold_group_fc_option;
        data->threshold_stage = soc_dnxf_threshold_stage_middle_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_TX_FE1_BYPASS_LLFCFC_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_TX_FE1_BYPASS_LLFCFC_PIPE_TH, 0);
        data->threshold_str = "TX FE1 BYPASS LLFCFC";
        data->threshold_group = soc_dnxf_threshold_group_fc_option;
        data->threshold_stage = soc_dnxf_threshold_stage_tx_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_RX_FRAG_DROP_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_RX_FRAG_DROP_PIPE_TH, 0);
        data->threshold_str = "RX FRAG DROP";
        data->threshold_group = soc_dnxf_threshold_group_drop_option;
        data->threshold_stage = soc_dnxf_threshold_stage_rx_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 0;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NOT_PROFILE_DEPENDENT;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_RX_MC_LOW_PRIO_DROP_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_RX_MC_LOW_PRIO_DROP_PIPE_TH, 0);
        data->threshold_str = "RX MC LOW PRIO DROP";
        data->threshold_group = soc_dnxf_threshold_group_drop_option;
        data->threshold_stage = soc_dnxf_threshold_stage_rx_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 0;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NOT_PROFILE_DEPENDENT;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_RX_FRAG_GCI_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_RX_FRAG_GCI_PIPE_TH, 0);
        data->threshold_str = "RX FRAG GCI";
        data->threshold_group = soc_dnxf_threshold_group_gci_option;
        data->threshold_stage = soc_dnxf_threshold_stage_rx_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe_level;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 0;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_RX_LLFCFC_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_RX_LLFCFC_PIPE_TH, 0);
        data->threshold_str = "RX LLFCFC";
        data->threshold_group = soc_dnxf_threshold_group_fc_option;
        data->threshold_stage = soc_dnxf_threshold_stage_rx_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 0;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NONE;
        data->is_to_be_displayed = 1;
    }
    if (_SHR_FABRIC_RX_WFQ_DYNAMIC_WEIGHT_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_RX_WFQ_DYNAMIC_WEIGHT_PIPE_TH, 0);
        data->threshold_str = "RX WFQ DYNAMIC WEIGHT";
        data->threshold_group = soc_dnxf_threshold_group_dynamic_weight_option;
        data->threshold_stage = soc_dnxf_threshold_stage_rx_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_FE1_FE3;
        data->is_tx = 0;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NOT_PROFILE_DEPENDENT;
        data->is_to_be_displayed = 0;
    }
    if (_SHR_FABRIC_MID_WFQ_DYNAMIC_WEIGHT_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_MID_WFQ_DYNAMIC_WEIGHT_PIPE_TH, 0);
        data->threshold_str = "MID WFQ DYNAMIC WEIGHT";
        data->threshold_group = soc_dnxf_threshold_group_dynamic_weight_option;
        data->threshold_stage = soc_dnxf_threshold_stage_middle_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_ALL;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NOT_PROFILE_DEPENDENT;
        data->is_to_be_displayed = 0;
    }
    if (_SHR_FABRIC_TX_WFQ_DYNAMIC_WEIGHT_PIPE_TH < table->keys[0].size)
    {
        data = (dnxf_data_fabric_congestion_thresholds_info_t *) dnxc_data_mgmt_table_data_get(unit, table, _SHR_FABRIC_TX_WFQ_DYNAMIC_WEIGHT_PIPE_TH, 0);
        data->threshold_str = "TX WFQ DYNAMIC WEIGHT";
        data->threshold_group = soc_dnxf_threshold_group_dynamic_weight_option;
        data->threshold_stage = soc_dnxf_threshold_stage_tx_option;
        data->index_type = _shr_dnxf_cgm_index_type_pipe;
        data->supported_flags = _SHR_FABRIC_TH_FLAG_SUPPORT_ALL;
        data->is_tx = 1;
        data->th_validity_bmp = _SHR_FABRIC_CGM_VALIDITY_NOT_PROFILE_DEPENDENT;
        data->is_to_be_displayed = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
ramon_a0_dnxf_data_fabric_cell_fifo_dma_fabric_cell_nof_entries_per_cell_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_cell;
    int define_index = dnxf_data_fabric_cell_define_fifo_dma_fabric_cell_nof_entries_per_cell;
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
ramon_a0_dnxf_data_fabric_cell_rx_cpu_cell_max_payload_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_cell;
    int define_index = dnxf_data_fabric_cell_define_rx_cpu_cell_max_payload_size;
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
ramon_a0_dnxf_data_fabric_cell_rx_max_nof_cpu_buffers_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_cell;
    int define_index = dnxf_data_fabric_cell_define_rx_max_nof_cpu_buffers;
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
ramon_a0_dnxf_data_fabric_cell_source_routed_cells_header_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_cell;
    int define_index = dnxf_data_fabric_cell_define_source_routed_cells_header_offset;
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
ramon_a0_dnxf_data_fabric_cell_fifo_dma_nof_channels_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_cell;
    int define_index = dnxf_data_fabric_cell_define_fifo_dma_nof_channels;
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
ramon_a0_dnxf_data_fabric_cell_fifo_dma_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_cell;
    int define_index = dnxf_data_fabric_cell_define_fifo_dma_entry_size;
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
ramon_a0_dnxf_data_fabric_cell_fifo_dma_max_nof_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_cell;
    int define_index = dnxf_data_fabric_cell_define_fifo_dma_max_nof_entries;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x4000;

    
    define->data = 0x4000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_cell_fifo_dma_min_nof_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_cell;
    int define_index = dnxf_data_fabric_cell_define_fifo_dma_min_nof_entries;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x40;

    
    define->data = 0x40;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_cell_fifo_dma_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_cell;
    int define_index = dnxf_data_fabric_cell_define_fifo_dma_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FABRIC_CELL_FIFO_DMA_ENABLE;
    define->property.doc = 
        "\n"
        "Deterimins if to use or not to use FIFO DMA.\n"
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
ramon_a0_dnxf_data_fabric_cell_fifo_dma_buffer_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_cell;
    int define_index = dnxf_data_fabric_cell_define_fifo_dma_buffer_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20480;

    
    define->data = 20480;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FABRIC_CELL_FIFO_DMA_BUFFER_SIZE;
    define->property.doc = 
        "\n"
        "Size of the host memory stored allocated by the CPU.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 64;
    define->property.range_max = 32768;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_cell_fifo_dma_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_cell;
    int define_index = dnxf_data_fabric_cell_define_fifo_dma_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FABRIC_CELL_FIFO_DMA_THRESHOLD;
    define->property.doc = 
        "\n"
        "The number of writes by the DMA until a threshold based interrupt is triggered.\n"
        "The value 0 disables the number of write-based interrupts.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 16;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_cell_fifo_dma_timeout_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_cell;
    int define_index = dnxf_data_fabric_cell_define_fifo_dma_timeout;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = SOC_DNXF_PROPERTY_UNAVAIL;

    
    define->data = SOC_DNXF_PROPERTY_UNAVAIL;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FABRIC_CELL_FIFO_DMA_TIMEOUT;
    define->property.doc = 
        "\n"
        "The amount of time in microseconds that passes from the first write by the DMA until a timeout based interrupt is triggered.\n"
        "Value 0 disables timeout based interrupts.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 10000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_cell_rx_thread_pri_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_cell;
    int define_index = dnxf_data_fabric_cell_define_rx_thread_pri;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = RX_THREAD_PRI_DFLT;

    
    define->data = RX_THREAD_PRI_DFLT;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_BCM_RX_THREAD_PRI;
    define->property.doc = 
        "\n"
        "RX thread priority.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 10000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
ramon_a0_dnxf_data_fabric_topology_max_link_score_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_topology;
    int define_index = dnxf_data_fabric_topology_define_max_link_score;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 120;

    
    define->data = 120;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_topology_nof_local_modid_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_topology;
    int define_index = dnxf_data_fabric_topology_define_nof_local_modid;
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
ramon_a0_dnxf_data_fabric_topology_nof_local_modid_fe13_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_topology;
    int define_index = dnxf_data_fabric_topology_define_nof_local_modid_fe13;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 96;

    
    define->data = 96;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_topology_load_balancing_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_topology;
    int define_index = dnxf_data_fabric_topology_define_load_balancing_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = soc_dnxf_load_balancing_mode_normal;

    
    define->data = soc_dnxf_load_balancing_mode_normal;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FABRIC_LOAD_BALANCING_MODE;
    define->property.doc = 
        "\n"
        "Sets the load balancing method.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 3;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "NORMAL_LOAD_BALANCE";
    define->property.mapping[0].val = soc_dnxf_load_balancing_mode_normal;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "DESTINATION_UNREACHABLE";
    define->property.mapping[1].val = soc_dnxf_load_balancing_mode_destination_unreachable;
    define->property.mapping[2].name = "BALANCED_INPUT";
    define->property.mapping[2].val = soc_dnxf_load_balancing_mode_balanced_input;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
ramon_a0_dnxf_data_fabric_gpd_in_time_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_gpd;
    int define_index = dnxf_data_fabric_gpd_define_in_time;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 60;

    
    define->data = 60;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_gpd_out_time_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_gpd;
    int define_index = dnxf_data_fabric_gpd_define_out_time;
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
ramon_a0_dnxf_data_fabric_reachability_table_row_size_in_uint32_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_reachability;
    int define_index = dnxf_data_fabric_reachability_define_table_row_size_in_uint32;
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
ramon_a0_dnxf_data_fabric_reachability_table_group_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_reachability;
    int define_index = dnxf_data_fabric_reachability_define_table_group_size;
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
ramon_a0_dnxf_data_fabric_reachability_rmgr_units_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_reachability;
    int define_index = dnxf_data_fabric_reachability_define_rmgr_units;
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
ramon_a0_dnxf_data_fabric_reachability_rmgr_nof_links_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_reachability;
    int define_index = dnxf_data_fabric_reachability_define_rmgr_nof_links;
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
ramon_a0_dnxf_data_fabric_reachability_gpd_rmgr_time_factor_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_reachability;
    int define_index = dnxf_data_fabric_reachability_define_gpd_rmgr_time_factor;
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
ramon_a0_dnxf_data_fabric_reachability_gen_rate_link_delta_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_reachability;
    int define_index = dnxf_data_fabric_reachability_define_gen_rate_link_delta;
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
ramon_a0_dnxf_data_fabric_reachability_gen_rate_full_cycle_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_reachability;
    int define_index = dnxf_data_fabric_reachability_define_gen_rate_full_cycle;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 44800;

    
    define->data = 44800;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_reachability_gpd_gen_rate_full_cycle_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_reachability;
    int define_index = dnxf_data_fabric_reachability_define_gpd_gen_rate_full_cycle;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10000;

    
    define->data = 10000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_fabric_reachability_watchdog_rate_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_reachability;
    int define_index = dnxf_data_fabric_reachability_define_watchdog_rate;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 100000;

    
    define->data = 100000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
ramon_a0_dnxf_data_fabric_hw_snake_is_sw_config_required_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = dnxf_data_fabric_submodule_hw_snake;
    int feature_index = dnxf_data_fabric_hw_snake_is_sw_config_required;
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
ramon_a0_data_fabric_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnxf_data_module_fabric;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnxf_data_fabric_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_fabric_general_define_max_nof_pipes;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_general_max_nof_pipes_set;
    data_index = dnxf_data_fabric_general_define_device_mode;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_general_device_mode_set;
    data_index = dnxf_data_fabric_general_define_local_routing_enable_uc;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_general_local_routing_enable_uc_set;
    data_index = dnxf_data_fabric_general_define_local_routing_enable_mc;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_general_local_routing_enable_mc_set;

    
    data_index = dnxf_data_fabric_general_is_multi_stage_supported;
    feature = &submodule->features[data_index];
    feature->set = ramon_a0_dnxf_data_fabric_general_is_multi_stage_supported_set;

    
    
    submodule_index = dnxf_data_fabric_submodule_pipes;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_fabric_pipes_define_max_nof_pipes;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_pipes_max_nof_pipes_set;
    data_index = dnxf_data_fabric_pipes_define_nof_valid_mapping_options;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_pipes_nof_valid_mapping_options_set;
    data_index = dnxf_data_fabric_pipes_define_nof_pipes;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_pipes_nof_pipes_set;
    data_index = dnxf_data_fabric_pipes_define_system_contains_multiple_pipe_device;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_pipes_system_contains_multiple_pipe_device_set;
    data_index = dnxf_data_fabric_pipes_define_system_tdm_priority;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_pipes_system_tdm_priority_set;

    

    
    data_index = dnxf_data_fabric_pipes_table_map;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_fabric_pipes_map_set;
    data_index = dnxf_data_fabric_pipes_table_valid_map_config;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_fabric_pipes_valid_map_config_set;
    
    submodule_index = dnxf_data_fabric_submodule_multicast;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_fabric_multicast_define_table_row_size_in_uint32;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_multicast_table_row_size_in_uint32_set;
    data_index = dnxf_data_fabric_multicast_define_id_range;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_multicast_id_range_set;
    data_index = dnxf_data_fabric_multicast_define_mode;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_multicast_mode_set;
    data_index = dnxf_data_fabric_multicast_define_priority_map_enable;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_multicast_priority_map_enable_set;

    

    
    
    submodule_index = dnxf_data_fabric_submodule_fifos;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_fabric_fifos_define_granularity;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_fifos_granularity_set;
    data_index = dnxf_data_fabric_fifos_define_min_depth;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_fifos_min_depth_set;
    data_index = dnxf_data_fabric_fifos_define_max_unused_resources;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_fifos_max_unused_resources_set;
    data_index = dnxf_data_fabric_fifos_define_rx_resources;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_fifos_rx_resources_set;
    data_index = dnxf_data_fabric_fifos_define_mid_resources;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_fifos_mid_resources_set;
    data_index = dnxf_data_fabric_fifos_define_tx_resources;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_fifos_tx_resources_set;
    data_index = dnxf_data_fabric_fifos_define_rx_full_offset;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_fifos_rx_full_offset_set;
    data_index = dnxf_data_fabric_fifos_define_mid_full_offset;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_fifos_mid_full_offset_set;
    data_index = dnxf_data_fabric_fifos_define_tx_full_offset;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_fifos_tx_full_offset_set;

    

    
    data_index = dnxf_data_fabric_fifos_table_rx_depth_per_pipe;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_fabric_fifos_rx_depth_per_pipe_set;
    data_index = dnxf_data_fabric_fifos_table_mid_depth_per_pipe;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_fabric_fifos_mid_depth_per_pipe_set;
    data_index = dnxf_data_fabric_fifos_table_tx_depth_per_pipe;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_fabric_fifos_tx_depth_per_pipe_set;
    
    submodule_index = dnxf_data_fabric_submodule_congestion;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_fabric_congestion_define_nof_profiles;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_congestion_nof_profiles_set;
    data_index = dnxf_data_fabric_congestion_define_nof_threshold_priorities;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_congestion_nof_threshold_priorities_set;
    data_index = dnxf_data_fabric_congestion_define_nof_threshold_levels;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_congestion_nof_threshold_levels_set;
    data_index = dnxf_data_fabric_congestion_define_nof_threshold_index_dimensions;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_congestion_nof_threshold_index_dimensions_set;
    data_index = dnxf_data_fabric_congestion_define_nof_thresholds;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_congestion_nof_thresholds_set;
    data_index = dnxf_data_fabric_congestion_define_nof_dfl_banks;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_congestion_nof_dfl_banks_set;
    data_index = dnxf_data_fabric_congestion_define_nof_dfl_sub_banks;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_congestion_nof_dfl_sub_banks_set;
    data_index = dnxf_data_fabric_congestion_define_nof_dfl_bank_entries;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_congestion_nof_dfl_bank_entries_set;
    data_index = dnxf_data_fabric_congestion_define_nof_rci_bits;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_congestion_nof_rci_bits_set;

    

    
    data_index = dnxf_data_fabric_congestion_table_thresholds_info;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_fabric_congestion_thresholds_info_set;
    
    submodule_index = dnxf_data_fabric_submodule_cell;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_fabric_cell_define_fifo_dma_fabric_cell_nof_entries_per_cell;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_cell_fifo_dma_fabric_cell_nof_entries_per_cell_set;
    data_index = dnxf_data_fabric_cell_define_rx_cpu_cell_max_payload_size;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_cell_rx_cpu_cell_max_payload_size_set;
    data_index = dnxf_data_fabric_cell_define_rx_max_nof_cpu_buffers;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_cell_rx_max_nof_cpu_buffers_set;
    data_index = dnxf_data_fabric_cell_define_source_routed_cells_header_offset;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_cell_source_routed_cells_header_offset_set;
    data_index = dnxf_data_fabric_cell_define_fifo_dma_nof_channels;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_cell_fifo_dma_nof_channels_set;
    data_index = dnxf_data_fabric_cell_define_fifo_dma_entry_size;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_cell_fifo_dma_entry_size_set;
    data_index = dnxf_data_fabric_cell_define_fifo_dma_max_nof_entries;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_cell_fifo_dma_max_nof_entries_set;
    data_index = dnxf_data_fabric_cell_define_fifo_dma_min_nof_entries;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_cell_fifo_dma_min_nof_entries_set;
    data_index = dnxf_data_fabric_cell_define_fifo_dma_enable;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_cell_fifo_dma_enable_set;
    data_index = dnxf_data_fabric_cell_define_fifo_dma_buffer_size;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_cell_fifo_dma_buffer_size_set;
    data_index = dnxf_data_fabric_cell_define_fifo_dma_threshold;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_cell_fifo_dma_threshold_set;
    data_index = dnxf_data_fabric_cell_define_fifo_dma_timeout;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_cell_fifo_dma_timeout_set;
    data_index = dnxf_data_fabric_cell_define_rx_thread_pri;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_cell_rx_thread_pri_set;

    

    
    
    submodule_index = dnxf_data_fabric_submodule_topology;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_fabric_topology_define_max_link_score;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_topology_max_link_score_set;
    data_index = dnxf_data_fabric_topology_define_nof_local_modid;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_topology_nof_local_modid_set;
    data_index = dnxf_data_fabric_topology_define_nof_local_modid_fe13;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_topology_nof_local_modid_fe13_set;
    data_index = dnxf_data_fabric_topology_define_load_balancing_mode;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_topology_load_balancing_mode_set;

    

    
    
    submodule_index = dnxf_data_fabric_submodule_gpd;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_fabric_gpd_define_in_time;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_gpd_in_time_set;
    data_index = dnxf_data_fabric_gpd_define_out_time;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_gpd_out_time_set;

    

    
    
    submodule_index = dnxf_data_fabric_submodule_reachability;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_fabric_reachability_define_table_row_size_in_uint32;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_reachability_table_row_size_in_uint32_set;
    data_index = dnxf_data_fabric_reachability_define_table_group_size;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_reachability_table_group_size_set;
    data_index = dnxf_data_fabric_reachability_define_rmgr_units;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_reachability_rmgr_units_set;
    data_index = dnxf_data_fabric_reachability_define_rmgr_nof_links;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_reachability_rmgr_nof_links_set;
    data_index = dnxf_data_fabric_reachability_define_gpd_rmgr_time_factor;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_reachability_gpd_rmgr_time_factor_set;
    data_index = dnxf_data_fabric_reachability_define_gen_rate_link_delta;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_reachability_gen_rate_link_delta_set;
    data_index = dnxf_data_fabric_reachability_define_gen_rate_full_cycle;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_reachability_gen_rate_full_cycle_set;
    data_index = dnxf_data_fabric_reachability_define_gpd_gen_rate_full_cycle;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_reachability_gpd_gen_rate_full_cycle_set;
    data_index = dnxf_data_fabric_reachability_define_watchdog_rate;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_fabric_reachability_watchdog_rate_set;

    

    
    
    submodule_index = dnxf_data_fabric_submodule_hw_snake;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnxf_data_fabric_hw_snake_is_sw_config_required;
    feature = &submodule->features[data_index];
    feature->set = ramon_a0_dnxf_data_fabric_hw_snake_is_sw_config_required_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

