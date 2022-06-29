
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MACSEC

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_macsec.h>
#ifdef INCLUDE_XFLOW_MACSEC
#include <xflow_macsec_defs.h>
#endif  







static shr_error_e
j2x_a0_dnx_data_macsec_general_macsec_block_exists_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int feature_index = dnx_data_macsec_general_macsec_block_exists;
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
j2x_a0_dnx_data_macsec_general_is_hw_reset_init_done_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int feature_index = dnx_data_macsec_general_is_hw_reset_init_done_supported;
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
j2x_a0_dnx_data_macsec_general_is_shared_macsec_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int feature_index = dnx_data_macsec_general_is_shared_macsec_supported;
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
j2x_a0_dnx_data_macsec_general_is_macsec_wrapper_main_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int feature_index = dnx_data_macsec_general_is_macsec_wrapper_main_enable;
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
j2x_a0_dnx_data_macsec_general_is_port_based_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int feature_index = dnx_data_macsec_general_is_port_based_supported;
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
j2x_a0_dnx_data_macsec_general_macsec_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int define_index = dnx_data_macsec_general_define_macsec_nof;
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
j2x_a0_dnx_data_macsec_general_macsec_in_core_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int define_index = dnx_data_macsec_general_define_macsec_in_core_nof;
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
j2x_a0_dnx_data_macsec_general_ports_in_macsec_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int define_index = dnx_data_macsec_general_define_ports_in_macsec_nof;
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
j2x_a0_dnx_data_macsec_general_etype_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int define_index = dnx_data_macsec_general_define_etype_nof;
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
j2x_a0_dnx_data_macsec_general_etype_not_zero_verify_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int define_index = dnx_data_macsec_general_define_etype_not_zero_verify;
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
j2x_a0_dnx_data_macsec_general_tx_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int define_index = dnx_data_macsec_general_define_tx_threshold;
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
j2x_a0_dnx_data_macsec_general_macsec_tdm_cal_max_depth_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int define_index = dnx_data_macsec_general_define_macsec_tdm_cal_max_depth;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3232;

    
    define->data = 3232;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_macsec_general_macsec_tdm_cal_hw_depth_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int define_index = dnx_data_macsec_general_define_macsec_tdm_cal_hw_depth;
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
j2x_a0_dnx_data_macsec_general_macsec_gen_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int define_index = dnx_data_macsec_general_define_macsec_gen;
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
j2x_a0_dnx_data_macsec_general_sectag_offset_min_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int define_index = dnx_data_macsec_general_define_sectag_offset_min_value;
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
j2x_a0_dnx_data_macsec_general_supported_entries_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int define_index = dnx_data_macsec_general_define_supported_entries_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = -1;

    
    define->data = -1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_macsec_general_is_macsec_enabled_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int define_index = dnx_data_macsec_general_define_is_macsec_enabled;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "macsec_enabled";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnx_data_property_macsec_general_is_macsec_enabled_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_macsec_general_is_power_optimization_enabled_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int define_index = dnx_data_macsec_general_define_is_power_optimization_enabled;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "macsec_power_optimization_enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2x_a0_dnx_data_macsec_general_macsec_block_mapping_set(
    int unit)
{
    int is_mss_index;
    int block_id_index;
    dnx_data_macsec_general_macsec_block_mapping_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int table_index = dnx_data_macsec_general_table_macsec_block_mapping;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;
    table->keys[1].size = dnx_data_macsec.general.macsec_nof_get(unit);
    table->info_get.key_size[1] = dnx_data_macsec.general.macsec_nof_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_macsec_general_macsec_block_mapping_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_macsec_general_table_macsec_block_mapping");

    
    default_data = (dnx_data_macsec_general_macsec_block_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->instance_id = 0;
    
    for (is_mss_index = 0; is_mss_index < table->keys[0].size; is_mss_index++)
    {
        for (block_id_index = 0; block_id_index < table->keys[1].size; block_id_index++)
        {
            data = (dnx_data_macsec_general_macsec_block_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, is_mss_index, block_id_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_macsec_general_macsec_block_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->instance_id = 0;
    }
    if (0 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_macsec_general_macsec_block_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 1);
        data->instance_id = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}

#ifdef INCLUDE_XFLOW_MACSEC


static shr_error_e
j2x_a0_dnx_data_macsec_general_macsec_control_options_set(
    int unit)
{
    int control_option_index;
    dnx_data_macsec_general_macsec_control_options_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int table_index = dnx_data_macsec_general_table_macsec_control_options;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = xflowMacsecControlCount;
    table->info_get.key_size[0] = xflowMacsecControlCount;

    
    table->values[0].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_macsec_general_macsec_control_options_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_macsec_general_table_macsec_control_options");

    
    default_data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->is_supported = 1;
    
    for (control_option_index = 0; control_option_index < table->keys[0].size; control_option_index++)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, control_option_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (xflowMacsecControlPNThresholdMask0 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlPNThresholdMask0, 0);
        data->is_supported = 0;
    }
    if (xflowMacsecControlPNThresholdMask1 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlPNThresholdMask1, 0);
        data->is_supported = 0;
    }
    if (xflowMacsecControlPNThresholdMask2 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlPNThresholdMask2, 0);
        data->is_supported = 0;
    }
    if (xflowMacsecControlPNThresholdMask3 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlPNThresholdMask3, 0);
        data->is_supported = 0;
    }
    if (xflowMacsecIpsecControlXPNThresholdMask0 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlXPNThresholdMask0, 0);
        data->is_supported = 0;
    }
    if (xflowMacsecIpsecControlXPNThresholdMask1 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlXPNThresholdMask1, 0);
        data->is_supported = 0;
    }
    if (xflowMacsecIpsecControlXPNThresholdMask2 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlXPNThresholdMask2, 0);
        data->is_supported = 0;
    }
    if (xflowMacsecIpsecControlXPNThresholdMask3 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlXPNThresholdMask3, 0);
        data->is_supported = 0;
    }
    if (xflowMacsecIpsecControlPNThresholdMask0 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlPNThresholdMask0, 0);
        data->is_supported = 0;
    }
    if (xflowMacsecIpsecControlPNThresholdMask1 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlPNThresholdMask1, 0);
        data->is_supported = 0;
    }
    if (xflowMacsecIpsecControlPNThresholdMask2 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlPNThresholdMask2, 0);
        data->is_supported = 0;
    }
    if (xflowMacsecIpsecControlPNThresholdMask3 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlPNThresholdMask3, 0);
        data->is_supported = 0;
    }
    if (xflowMacsecIpsecControlNextHopDummy1 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlNextHopDummy1, 0);
        data->is_supported = 0;
    }
    if (xflowMacsecIpsecControlNextHopDummy2 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlNextHopDummy2, 0);
        data->is_supported = 0;
    }
    if (xflowMacsecIpsecControlNextHopDummy3 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlNextHopDummy3, 0);
        data->is_supported = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


#endif 





static shr_error_e
j2x_a0_dnx_data_macsec_ingress_flow_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_ingress;
    int define_index = dnx_data_macsec_ingress_define_flow_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1536;

    
    define->data = 1536;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_macsec_ingress_policy_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_ingress;
    int define_index = dnx_data_macsec_ingress_define_policy_nof;
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
j2x_a0_dnx_data_macsec_ingress_secure_assoc_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_ingress;
    int define_index = dnx_data_macsec_ingress_define_secure_assoc_nof;
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
j2x_a0_dnx_data_macsec_ingress_udf_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_ingress;
    int define_index = dnx_data_macsec_ingress_define_udf_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 440;

    
    define->data = 440;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_macsec_ingress_mgmt_rule_exact_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_ingress;
    int define_index = dnx_data_macsec_ingress_define_mgmt_rule_exact_nof;
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
j2x_a0_dnx_data_macsec_ingress_tpid_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_ingress;
    int define_index = dnx_data_macsec_ingress_define_tpid_nof;
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
j2x_a0_dnx_data_macsec_ingress_cpu_flex_map_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_ingress;
    int define_index = dnx_data_macsec_ingress_define_cpu_flex_map_nof;
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
j2x_a0_dnx_data_macsec_ingress_sc_tcam_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_ingress;
    int define_index = dnx_data_macsec_ingress_define_sc_tcam_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1280;

    
    define->data = 1280;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_macsec_ingress_sa_per_sc_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_ingress;
    int define_index = dnx_data_macsec_ingress_define_sa_per_sc_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "xflow_macsec_secure_chan_to_num_secure_assoc";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    define->property.label = DNXC_DATA_LABEL_MACSEC;
    SHR_IF_ERR_EXIT(dnx_data_property_macsec_ingress_sa_per_sc_nof_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_macsec_ingress_invalidate_sa_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_ingress;
    int define_index = dnx_data_macsec_ingress_define_invalidate_sa;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_XFLOW_MACSEC_DECRYPT_AUTO_SECURE_ASSOC_INVALIDATE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_enable;
    define->property.method_str = "enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    define->property.label = DNXC_DATA_LABEL_MACSEC;
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_macsec_ingress_secure_channel_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_ingress;
    int define_index = dnx_data_macsec_ingress_define_secure_channel_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_macsec.ingress.secure_assoc_nof_get(unit)/dnx_data_macsec.ingress.sa_per_sc_nof_get(unit);

    
    define->data = dnx_data_macsec.ingress.secure_assoc_nof_get(unit)/dnx_data_macsec.ingress.sa_per_sc_nof_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2x_a0_dnx_data_macsec_ingress_udf_set(
    int unit)
{
    int type_index;
    dnx_data_macsec_ingress_udf_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_ingress;
    int table_index = dnx_data_macsec_ingress_table_udf;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 13;
    table->info_get.key_size[0] = 13;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_macsec_ingress_udf_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_macsec_ingress_table_udf");

    
    default_data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->payload_len = 0;
    
    for (type_index = 0; type_index < table->keys[0].size; type_index++)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->payload_len = 264;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->payload_len = 264;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->payload_len = 264;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->payload_len = 280;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->payload_len = 152;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->payload_len = 328;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->payload_len = 248;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->payload_len = 224;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->payload_len = 192;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->payload_len = 192;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->payload_len = 80;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->payload_len = 48;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->payload_len = 48;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2x_a0_dnx_data_macsec_egress_secure_assoc_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_egress;
    int define_index = dnx_data_macsec_egress_define_secure_assoc_nof;
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
j2x_a0_dnx_data_macsec_egress_soft_expiry_threshold_profiles_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_egress;
    int define_index = dnx_data_macsec_egress_define_soft_expiry_threshold_profiles_nof;
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
j2x_a0_dnx_data_macsec_egress_an_bit_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_egress;
    int define_index = dnx_data_macsec_egress_define_an_bit_nof;
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
j2x_a0_dnx_data_macsec_egress_nh_profiles_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_egress;
    int define_index = dnx_data_macsec_egress_define_nh_profiles_nof;
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
j2x_a0_dnx_data_macsec_egress_association_number_actual_range_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_egress;
    int define_index = dnx_data_macsec_egress_define_association_number_actual_range_nof;
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
j2x_a0_dnx_data_macsec_egress_sa_per_sc_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_egress;
    int define_index = dnx_data_macsec_egress_define_sa_per_sc_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "xflow_macsec_secure_chan_to_num_secure_assoc";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    define->property.label = DNXC_DATA_LABEL_MACSEC;
    SHR_IF_ERR_EXIT(dnx_data_property_macsec_egress_sa_per_sc_nof_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_macsec_egress_secure_channel_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_egress;
    int define_index = dnx_data_macsec_egress_define_secure_channel_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_macsec.egress.secure_assoc_nof_get(unit)/dnx_data_macsec.egress.sa_per_sc_nof_get(unit);

    
    define->data = dnx_data_macsec.egress.secure_assoc_nof_get(unit)/dnx_data_macsec.egress.sa_per_sc_nof_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2x_a0_dnx_data_macsec_wrapper_macsec_arb_ports_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_wrapper;
    int define_index = dnx_data_macsec_wrapper_define_macsec_arb_ports_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 384;

    
    define->data = 384;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_macsec_wrapper_per_port_speed_max_mbps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_wrapper;
    int define_index = dnx_data_macsec_wrapper_define_per_port_speed_max_mbps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 400000;

    
    define->data = 400000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_macsec_wrapper_wrapper_speed_max_mbps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_wrapper;
    int define_index = dnx_data_macsec_wrapper_define_wrapper_speed_max_mbps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 800000;

    
    define->data = 800000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_macsec_wrapper_framer_ports_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_wrapper;
    int define_index = dnx_data_macsec_wrapper_define_framer_ports_offset;
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
j2x_a0_dnx_data_macsec_wrapper_tdm_calendar_sbus_slot_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_wrapper;
    int define_index = dnx_data_macsec_wrapper_define_tdm_calendar_sbus_slot;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 33;

    
    define->data = 33;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_macsec_wrapper_tdm_calendar_nof_slots_per_line_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_wrapper;
    int define_index = dnx_data_macsec_wrapper_define_tdm_calendar_nof_slots_per_line;
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
j2x_a0_dnx_data_macsec_wrapper_nof_tags_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_wrapper;
    int define_index = dnx_data_macsec_wrapper_define_nof_tags;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_macsec.general.macsec_nof_get(unit)+1;

    
    define->data = dnx_data_macsec.general.macsec_nof_get(unit)+1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2x_a0_data_macsec_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_macsec;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_macsec_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_macsec_general_define_macsec_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_general_macsec_nof_set;
    data_index = dnx_data_macsec_general_define_macsec_in_core_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_general_macsec_in_core_nof_set;
    data_index = dnx_data_macsec_general_define_ports_in_macsec_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_general_ports_in_macsec_nof_set;
    data_index = dnx_data_macsec_general_define_etype_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_general_etype_nof_set;
    data_index = dnx_data_macsec_general_define_etype_not_zero_verify;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_general_etype_not_zero_verify_set;
    data_index = dnx_data_macsec_general_define_tx_threshold;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_general_tx_threshold_set;
    data_index = dnx_data_macsec_general_define_macsec_tdm_cal_max_depth;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_general_macsec_tdm_cal_max_depth_set;
    data_index = dnx_data_macsec_general_define_macsec_tdm_cal_hw_depth;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_general_macsec_tdm_cal_hw_depth_set;
    data_index = dnx_data_macsec_general_define_macsec_gen;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_general_macsec_gen_set;
    data_index = dnx_data_macsec_general_define_sectag_offset_min_value;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_general_sectag_offset_min_value_set;
    data_index = dnx_data_macsec_general_define_supported_entries_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_general_supported_entries_nof_set;
    data_index = dnx_data_macsec_general_define_is_macsec_enabled;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_general_is_macsec_enabled_set;
    data_index = dnx_data_macsec_general_define_is_power_optimization_enabled;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_general_is_power_optimization_enabled_set;

    
    data_index = dnx_data_macsec_general_macsec_block_exists;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_macsec_general_macsec_block_exists_set;
    data_index = dnx_data_macsec_general_is_hw_reset_init_done_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_macsec_general_is_hw_reset_init_done_supported_set;
    data_index = dnx_data_macsec_general_is_shared_macsec_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_macsec_general_is_shared_macsec_supported_set;
    data_index = dnx_data_macsec_general_is_macsec_wrapper_main_enable;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_macsec_general_is_macsec_wrapper_main_enable_set;
    data_index = dnx_data_macsec_general_is_port_based_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_macsec_general_is_port_based_supported_set;

    
    data_index = dnx_data_macsec_general_table_macsec_block_mapping;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_macsec_general_macsec_block_mapping_set;
#ifdef INCLUDE_XFLOW_MACSEC

    data_index = dnx_data_macsec_general_table_macsec_control_options;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_macsec_general_macsec_control_options_set;

#endif 
    
    submodule_index = dnx_data_macsec_submodule_ingress;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_macsec_ingress_define_flow_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_ingress_flow_nof_set;
    data_index = dnx_data_macsec_ingress_define_policy_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_ingress_policy_nof_set;
    data_index = dnx_data_macsec_ingress_define_secure_assoc_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_ingress_secure_assoc_nof_set;
    data_index = dnx_data_macsec_ingress_define_udf_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_ingress_udf_nof_bits_set;
    data_index = dnx_data_macsec_ingress_define_mgmt_rule_exact_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_ingress_mgmt_rule_exact_nof_set;
    data_index = dnx_data_macsec_ingress_define_tpid_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_ingress_tpid_nof_set;
    data_index = dnx_data_macsec_ingress_define_cpu_flex_map_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_ingress_cpu_flex_map_nof_set;
    data_index = dnx_data_macsec_ingress_define_sc_tcam_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_ingress_sc_tcam_nof_set;
    data_index = dnx_data_macsec_ingress_define_sa_per_sc_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_ingress_sa_per_sc_nof_set;
    data_index = dnx_data_macsec_ingress_define_invalidate_sa;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_ingress_invalidate_sa_set;
    data_index = dnx_data_macsec_ingress_define_secure_channel_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_ingress_secure_channel_nof_set;

    

    
    data_index = dnx_data_macsec_ingress_table_udf;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_macsec_ingress_udf_set;
    
    submodule_index = dnx_data_macsec_submodule_egress;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_macsec_egress_define_secure_assoc_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_egress_secure_assoc_nof_set;
    data_index = dnx_data_macsec_egress_define_soft_expiry_threshold_profiles_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_egress_soft_expiry_threshold_profiles_nof_set;
    data_index = dnx_data_macsec_egress_define_an_bit_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_egress_an_bit_nof_set;
    data_index = dnx_data_macsec_egress_define_nh_profiles_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_egress_nh_profiles_nof_set;
    data_index = dnx_data_macsec_egress_define_association_number_actual_range_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_egress_association_number_actual_range_nof_set;
    data_index = dnx_data_macsec_egress_define_sa_per_sc_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_egress_sa_per_sc_nof_set;
    data_index = dnx_data_macsec_egress_define_secure_channel_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_egress_secure_channel_nof_set;

    

    
    
    submodule_index = dnx_data_macsec_submodule_wrapper;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_macsec_wrapper_define_macsec_arb_ports_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_wrapper_macsec_arb_ports_nof_set;
    data_index = dnx_data_macsec_wrapper_define_per_port_speed_max_mbps;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_wrapper_per_port_speed_max_mbps_set;
    data_index = dnx_data_macsec_wrapper_define_wrapper_speed_max_mbps;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_wrapper_wrapper_speed_max_mbps_set;
    data_index = dnx_data_macsec_wrapper_define_framer_ports_offset;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_wrapper_framer_ports_offset_set;
    data_index = dnx_data_macsec_wrapper_define_tdm_calendar_sbus_slot;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_wrapper_tdm_calendar_sbus_slot_set;
    data_index = dnx_data_macsec_wrapper_define_tdm_calendar_nof_slots_per_line;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_wrapper_tdm_calendar_nof_slots_per_line_set;
    data_index = dnx_data_macsec_wrapper_define_nof_tags;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_macsec_wrapper_nof_tags_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

