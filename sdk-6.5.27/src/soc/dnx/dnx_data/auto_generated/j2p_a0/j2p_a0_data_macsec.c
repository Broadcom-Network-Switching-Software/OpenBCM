
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
#ifdef INCLUDE_XFLOW_MACSEC
#include <dnx/dnx_sec.h>
#endif  
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_ACCESS_SUPPORT) 
#include <soc/access/auto_generated/common_enum.h>
#endif  







static shr_error_e
j2p_a0_dnx_data_macsec_general_macsec_block_exists_set(
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
j2p_a0_dnx_data_macsec_general_is_hw_reset_init_done_supported_set(
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
j2p_a0_dnx_data_macsec_general_is_port_based_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int feature_index = dnx_data_macsec_general_is_port_based_supported;
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
j2p_a0_dnx_data_macsec_general_an_full_range_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int feature_index = dnx_data_macsec_general_an_full_range_supported;
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
j2p_a0_dnx_data_macsec_general_rfc_6054_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int feature_index = dnx_data_macsec_general_rfc_6054_supported;
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
j2p_a0_dnx_data_macsec_general_multiple_pms_per_macsec_instance_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int feature_index = dnx_data_macsec_general_multiple_pms_per_macsec_instance;
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
j2p_a0_dnx_data_macsec_general_macsec_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int define_index = dnx_data_macsec_general_define_macsec_nof;
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
j2p_a0_dnx_data_macsec_general_macsec_in_core_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int define_index = dnx_data_macsec_general_define_macsec_in_core_nof;
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
j2p_a0_dnx_data_macsec_general_ports_in_macsec_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int define_index = dnx_data_macsec_general_define_ports_in_macsec_nof;
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
j2p_a0_dnx_data_macsec_general_etype_nof_set(
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
j2p_a0_dnx_data_macsec_general_etype_not_zero_verify_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int define_index = dnx_data_macsec_general_define_etype_not_zero_verify;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_macsec_general_tx_threshold_set(
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
j2p_a0_dnx_data_macsec_general_macsec_tdm_cal_max_depth_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int define_index = dnx_data_macsec_general_define_macsec_tdm_cal_max_depth;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 129;

    
    define->data = 129;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_macsec_general_macsec_tdm_cal_hw_depth_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int define_index = dnx_data_macsec_general_define_macsec_tdm_cal_hw_depth;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 129;

    
    define->data = 129;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_macsec_general_association_number_full_range_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int define_index = dnx_data_macsec_general_define_association_number_full_range_nof;
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
j2p_a0_dnx_data_macsec_general_macsec_gen_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int define_index = dnx_data_macsec_general_define_macsec_gen;
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
j2p_a0_dnx_data_macsec_general_sectag_offset_min_value_set(
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
j2p_a0_dnx_data_macsec_general_supported_entries_nof_set(
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
j2p_a0_dnx_data_macsec_general_is_macsec_enabled_set(
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
j2p_a0_dnx_data_macsec_general_is_power_optimization_enabled_set(
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
j2p_a0_dnx_data_macsec_general_pm_to_macsec_set(
    int unit)
{
    int ethu_index_index;
    dnx_data_macsec_general_pm_to_macsec_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int table_index = dnx_data_macsec_general_table_pm_to_macsec;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_nif.eth.ethu_nof_get(unit);
    table->info_get.key_size[0] = dnx_data_nif.eth.ethu_nof_get(unit);

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_macsec_general_pm_to_macsec_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_macsec_general_table_pm_to_macsec");

    
    default_data = (dnx_data_macsec_general_pm_to_macsec_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->macsec_id = -1;
    
    for (ethu_index_index = 0; ethu_index_index < table->keys[0].size; ethu_index_index++)
    {
        data = (dnx_data_macsec_general_pm_to_macsec_t *) dnxc_data_mgmt_table_data_get(unit, table, ethu_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_pm_to_macsec_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->macsec_id = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_pm_to_macsec_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->macsec_id = 1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_pm_to_macsec_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->macsec_id = 1;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_pm_to_macsec_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->macsec_id = 2;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_pm_to_macsec_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->macsec_id = 2;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_pm_to_macsec_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->macsec_id = 3;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_pm_to_macsec_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->macsec_id = 3;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_pm_to_macsec_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->macsec_id = 4;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_pm_to_macsec_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->macsec_id = 4;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_pm_to_macsec_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->macsec_id = 5;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_pm_to_macsec_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->macsec_id = 6;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_pm_to_macsec_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->macsec_id = 6;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_pm_to_macsec_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->macsec_id = 7;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_pm_to_macsec_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->macsec_id = 7;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_pm_to_macsec_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->macsec_id = 8;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_pm_to_macsec_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->macsec_id = 8;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_pm_to_macsec_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->macsec_id = 9;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_pm_to_macsec_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->macsec_id = 9;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_macsec_general_macsec_instances_set(
    int unit)
{
    int macsec_id_index;
    dnx_data_macsec_general_macsec_instances_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int table_index = dnx_data_macsec_general_table_macsec_instances;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_macsec.general.macsec_nof_get(unit);
    table->info_get.key_size[0] = dnx_data_macsec.general.macsec_nof_get(unit);

    
    table->values[0].default_val = "2";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_macsec_general_macsec_instances_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_macsec_general_table_macsec_instances");

    
    default_data = (dnx_data_macsec_general_macsec_instances_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nof_pms = 2;
    
    for (macsec_id_index = 0; macsec_id_index < table->keys[0].size; macsec_id_index++)
    {
        data = (dnx_data_macsec_general_macsec_instances_t *) dnxc_data_mgmt_table_data_get(unit, table, macsec_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_instances_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->nof_pms = 1;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_instances_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->nof_pms = 2;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_instances_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->nof_pms = 2;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_instances_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->nof_pms = 2;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_instances_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->nof_pms = 2;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_instances_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->nof_pms = 1;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_instances_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->nof_pms = 2;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_instances_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->nof_pms = 2;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_instances_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->nof_pms = 2;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_instances_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->nof_pms = 2;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_macsec_general_macsec_block_mapping_set(
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
    
    if (1 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_macsec_general_macsec_block_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->instance_id = 0;
    }
    if (0 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_macsec_general_macsec_block_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->instance_id = 1;
    }
    if (0 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_macsec_general_macsec_block_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 1);
        data->instance_id = 2;
    }
    if (0 < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_macsec_general_macsec_block_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 2);
        data->instance_id = 3;
    }
    if (0 < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_macsec_general_macsec_block_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 3);
        data->instance_id = 4;
    }
    if (1 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_macsec_general_macsec_block_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 1);
        data->instance_id = 5;
    }
    if (0 < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_macsec_general_macsec_block_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 4);
        data->instance_id = 6;
    }
    if (0 < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_macsec_general_macsec_block_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 5);
        data->instance_id = 7;
    }
    if (0 < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_macsec_general_macsec_block_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 6);
        data->instance_id = 8;
    }
    if (0 < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_macsec_general_macsec_block_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 7);
        data->instance_id = 9;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}

#ifdef INCLUDE_XFLOW_MACSEC


static shr_error_e
j2p_a0_dnx_data_macsec_general_macsec_control_options_set(
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

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_macsec_general_macsec_control_options_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_macsec_general_table_macsec_control_options");

    
    default_data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->is_supported = 0;
    
    for (control_option_index = 0; control_option_index < table->keys[0].size; control_option_index++)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, control_option_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (xflowMacsecControlPNThreshold < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlPNThreshold, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlXPNThreshold < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlXPNThreshold, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlEncryptFailCopyToCpu < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlEncryptFailCopyToCpu, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlDecryptFailCopyToCpu < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlDecryptFailCopyToCpu, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlEncryptFailDrop < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlEncryptFailDrop, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlDecryptFailDrop < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlDecryptFailDrop, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlMgmtMTU < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlMgmtMTU, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlSVTagEnable < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlSVTagEnable, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlPTPDestPortGeneral < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlPTPDestPortGeneral, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlPTPDestPortEvent < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlPTPDestPortEvent, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlPbbTpidBTag < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlPbbTpidBTag, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlPbbTpidITag < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlPbbTpidITag, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlEtypeNIV < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlEtypeNIV, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlEtypePE < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlEtypePE, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlEtypeMgmt0 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlEtypeMgmt0, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlEtypeMgmt1 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlEtypeMgmt1, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlEtypeMgmt2 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlEtypeMgmt2, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlEtypeMgmt3 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlEtypeMgmt3, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlEtypeMgmt4 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlEtypeMgmt4, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlEtypeMgmt5 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlEtypeMgmt5, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlEtypeMgmt6 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlEtypeMgmt6, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlEtypeMgmt7 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlEtypeMgmt7, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlVxLANSecDestPort < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlVxLANSecDestPort, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlOutDestPort < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlOutDestPort, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlMplsEtype0 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlMplsEtype0, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlMplsEtype1 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlMplsEtype1, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlMplsEtype2 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlMplsEtype2, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlMplsEtype3 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlMplsEtype3, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlSVTagTPIDEtype < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlSVTagTPIDEtype, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlSelIpInfoAfterMplsBos < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlSelIpInfoAfterMplsBos, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlSvtagSopErrorDrop < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlSvtagSopErrorDrop, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlXPNThresholdMask0 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlXPNThresholdMask0, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlXPNThresholdMask1 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlXPNThresholdMask1, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlXPNThresholdMask2 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlXPNThresholdMask2, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlXPNThresholdMask3 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlXPNThresholdMask3, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecIpsecControlUdpDstPortWithNonEsp < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlUdpDstPortWithNonEsp, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecIpsecControlUdpDstPortWithoutNonEsp < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlUdpDstPortWithoutNonEsp, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecIpsecControlNatUdpDstPort < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlNatUdpDstPort, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecIpsecControlNatUdpSrcPort < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlNatUdpSrcPort, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecIpsecControlESPUdpDstPort < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlESPUdpDstPort, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecIpsecControlESPUdpSrcPort < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlESPUdpSrcPort, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecIpsecControlIncrementPadBytes < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlIncrementPadBytes, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecIpsecControlNextHopDummy < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlNextHopDummy, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlUdpDestPort < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlUdpDestPort, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlTcpDestPort < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlTcpDestPort, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlSectagE0C1Error < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlSectagE0C1Error, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlMibOctetMode < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlMibOctetMode, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecIpsecControlEspTrailPadChk < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlEspTrailPadChk, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecIpsecControlIncrByteForEspTrailPadChk < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlIncrByteForEspTrailPadChk, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlSecurePktIpLengthChk < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlSecurePktIpLengthChk, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecIpsecControlSecurePktIpLengthChk < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlSecurePktIpLengthChk, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecIpsecControlDummyPktStrictChk < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlDummyPktStrictChk, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecIpsecControlPNThreshold < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlPNThreshold, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecIpsecControlXPNThreshold < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIpsecControlXPNThreshold, 0);
        data->is_supported = 1;
    }
    if (xflowMacsecControlEncryptMaxNofAnValues < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_macsec_control_options_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecControlEncryptMaxNofAnValues, 0);
        data->is_supported = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


#endif 
#ifdef INCLUDE_XFLOW_MACSEC


static shr_error_e
j2p_a0_dnx_data_macsec_general_stat_special_map_set(
    int unit)
{
    int index_index;
    dnx_data_macsec_general_stat_special_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int table_index = dnx_data_macsec_general_table_stat_special_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 8;
    table->info_get.key_size[0] = 8;

    
    table->values[0].default_val = "xflowMacsecStatTypeCount";
    table->values[1].default_val = "xflowMacsecStatTypeCount";
    table->values[2].default_val = "XFLOW_MACSEC_ENCRYPT_DECRYPT_NONE";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_macsec_general_stat_special_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_macsec_general_table_stat_special_map");

    
    default_data = (dnx_data_macsec_general_stat_special_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->stat_from = xflowMacsecStatTypeCount;
    default_data->stat_to = xflowMacsecStatTypeCount;
    default_data->oper = XFLOW_MACSEC_ENCRYPT_DECRYPT_NONE;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_macsec_general_stat_special_map_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_special_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->stat_from = xflowMacsecSecyTxSCStatsProtectedPkts;
        data->stat_to = xflowMacsecSecyTxSAStatsProtectedPkts;
        data->oper = XFLOW_MACSEC_ENCRYPT;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_special_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->stat_from = xflowMacsecSecyTxSCStatsEncryptedPkts;
        data->stat_to = xflowMacsecSecyTxSAStatsEncryptedPkts;
        data->oper = XFLOW_MACSEC_ENCRYPT;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_special_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->stat_from = xflowMacsecSecyRxSCStatsUnusedSAPkts;
        data->stat_to = xflowMacsecSecyRxSAStatsUnusedSAPkts;
        data->oper = XFLOW_MACSEC_DECRYPT;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_special_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->stat_from = xflowMacsecSecyRxSCStatsNotUsingSAPkts;
        data->stat_to = xflowMacsecSecyRxSAStatsNotUsingSAPkts;
        data->oper = XFLOW_MACSEC_DECRYPT;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_special_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->stat_from = xflowMacsecSecyRxSCStatsNotValidPkts;
        data->stat_to = xflowMacsecSecyRxSAStatsNotValidPkts;
        data->oper = XFLOW_MACSEC_DECRYPT;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_special_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->stat_from = xflowMacsecSecyRxSCStatsInvalidPkts;
        data->stat_to = xflowMacsecSecyRxSAStatsInvalidPkts;
        data->oper = XFLOW_MACSEC_DECRYPT;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_special_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->stat_from = xflowMacsecSecyRxSCStatsOKPkts;
        data->stat_to = xflowMacsecSecyRxSAStatsOKPkts;
        data->oper = XFLOW_MACSEC_DECRYPT;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_special_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->stat_from = xflowMacsecBadOlpHdrCntr;
        data->stat_to = xflowMacsecBadOlpHdrCntr;
        data->oper = XFLOW_MACSEC_ENCRYPT_DECRYPT_NONE;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


#endif 
#ifdef INCLUDE_XFLOW_MACSEC


static shr_error_e
j2p_a0_dnx_data_macsec_general_counter_map_set(
    int unit)
{
    int counter_id_index;
    dnx_data_macsec_general_counter_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int table_index = dnx_data_macsec_general_table_counter_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = (DNX_XFLOW_MACSEC_CID_SPECIAL+1);
    table->info_get.key_size[0] = (DNX_XFLOW_MACSEC_CID_SPECIAL+1);

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "N/A";
    table->values[2].default_val = "0";
    table->values[3].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_macsec_general_counter_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_macsec_general_table_counter_map");

    
    default_data = (dnx_data_macsec_general_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->hw_access = -1;
    default_data->counter_name = "N/A";
    default_data->is_reg = 0;
    default_data->valid = 1;
    
    for (counter_id_index = 0; counter_id_index < table->keys[0].size; counter_id_index++)
    {
        data = (dnx_data_macsec_general_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, counter_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_XFLOW_MACSEC_CID_0 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_0, 0);
        data->hw_access = MACSEC_ISEC_MIB_SP_UNCTRLm;
        data->counter_name = "xflowMacsecCid0";
    }
    if (DNX_XFLOW_MACSEC_CID_1 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_1, 0);
        data->hw_access = MACSEC_ISEC_MIB_SP_CTRL_1m;
        data->counter_name = "xflowMacsecCid1";
    }
    if (DNX_XFLOW_MACSEC_CID_2 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_2, 0);
        data->hw_access = MACSEC_ISEC_MIB_SP_CTRL_2m;
        data->counter_name = "xflowMacsecCid2";
    }
    if (DNX_XFLOW_MACSEC_CID_3 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_3, 0);
        data->hw_access = MACSEC_ISEC_MIB_SCm;
        data->counter_name = "xflowMacsecCid3";
    }
    if (DNX_XFLOW_MACSEC_CID_4 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_4, 0);
        data->hw_access = MACSEC_ISEC_MIB_SAm;
        data->counter_name = "xflowMacsecCid4";
    }
    if (DNX_XFLOW_MACSEC_CID_5 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_5, 0);
        data->hw_access = MACSEC_ESEC_MIB_MISCm;
        data->counter_name = "xflowMacsecCid5";
    }
    if (DNX_XFLOW_MACSEC_CID_6 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_6, 0);
        data->hw_access = MACSEC_ESEC_MIB_SC_UN_CTRLm;
        data->counter_name = "xflowMacsecCid6";
    }
    if (DNX_XFLOW_MACSEC_CID_7 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_7, 0);
        data->hw_access = MACSEC_ESEC_MIB_SC_CTRLm;
        data->counter_name = "xflowMacsecCid7";
    }
    if (DNX_XFLOW_MACSEC_CID_8 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_8, 0);
        data->hw_access = MACSEC_ESEC_MIB_SCm;
        data->counter_name = "xflowMacsecCid8";
    }
    if (DNX_XFLOW_MACSEC_CID_9 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_9, 0);
        data->hw_access = MACSEC_ESEC_MIB_SAm;
        data->counter_name = "xflowMacsecCid9";
    }
    if (DNX_XFLOW_MACSEC_CID_10 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_10, 0);
        data->hw_access = MACSEC_ISEC_SPTCAM_HIT_COUNTm;
        data->counter_name = "xflowMacsecCid10";
    }
    if (DNX_XFLOW_MACSEC_CID_11 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_11, 0);
        data->hw_access = MACSEC_ISEC_PORT_COUNTERSm;
        data->counter_name = "xflowMacsecCid11";
    }
    if (DNX_XFLOW_MACSEC_CID_12 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_12, 0);
        data->hw_access = MACSEC_ISEC_SCTCAM_HIT_COUNTm;
        data->counter_name = "xflowMacsecCid12";
    }
    if (DNX_XFLOW_MACSEC_CID_13 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_13, 0);
        data->hw_access = MACSEC_ISEC_AES_ICV_FAIL_CNTr;
        data->counter_name = "xflowMacsecCid13";
        data->is_reg = 1;
    }
    if (DNX_XFLOW_MACSEC_CID_14 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_14, 0);
        data->hw_access = MACSEC_ISEC_MTU_FAIL_CNTr;
        data->counter_name = "xflowMacsecCid14";
        data->is_reg = 1;
    }
    if (DNX_XFLOW_MACSEC_CID_COUNT < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_COUNT, 0);
        data->counter_name = "xflowMacsecCidCount";
        data->valid = 0;
    }
    if (DNX_XFLOW_MACSEC_CID_SPECIAL < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_SPECIAL, 0);
        data->counter_name = "xflowMacsecCidSpecial";
        data->valid = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


#endif 
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_ACCESS_SUPPORT) 


static shr_error_e
j2p_a0_dnx_data_macsec_general_access_counter_map_set(
    int unit)
{
    int counter_id_index;
    dnx_data_macsec_general_access_counter_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int table_index = dnx_data_macsec_general_table_access_counter_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = (DNX_XFLOW_MACSEC_CID_SPECIAL+1);
    table->info_get.key_size[0] = (DNX_XFLOW_MACSEC_CID_SPECIAL+1);

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "N/A";
    table->values[2].default_val = "0";
    table->values[3].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_macsec_general_access_counter_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_macsec_general_table_access_counter_map");

    
    default_data = (dnx_data_macsec_general_access_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->hw_access = -1;
    default_data->counter_name = "N/A";
    default_data->is_reg = 0;
    default_data->valid = 1;
    
    for (counter_id_index = 0; counter_id_index < table->keys[0].size; counter_id_index++)
    {
        data = (dnx_data_macsec_general_access_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, counter_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_XFLOW_MACSEC_CID_0 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_0, 0);
        data->hw_access = mMACSEC_ISEC_MIB_SP_UNCTRL;
        data->counter_name = "xflowMacsecCid0";
    }
    if (DNX_XFLOW_MACSEC_CID_1 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_1, 0);
        data->hw_access = mMACSEC_ISEC_MIB_SP_CTRL_1;
        data->counter_name = "xflowMacsecCid1";
    }
    if (DNX_XFLOW_MACSEC_CID_2 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_2, 0);
        data->hw_access = mMACSEC_ISEC_MIB_SP_CTRL_2;
        data->counter_name = "xflowMacsecCid2";
    }
    if (DNX_XFLOW_MACSEC_CID_3 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_3, 0);
        data->hw_access = mMACSEC_ISEC_MIB_SC;
        data->counter_name = "xflowMacsecCid3";
    }
    if (DNX_XFLOW_MACSEC_CID_4 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_4, 0);
        data->hw_access = mMACSEC_ISEC_MIB_SA;
        data->counter_name = "xflowMacsecCid4";
    }
    if (DNX_XFLOW_MACSEC_CID_5 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_5, 0);
        data->hw_access = mMACSEC_ESEC_MIB_MISC;
        data->counter_name = "xflowMacsecCid5";
    }
    if (DNX_XFLOW_MACSEC_CID_6 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_6, 0);
        data->hw_access = mMACSEC_ESEC_MIB_SC_UN_CTRL;
        data->counter_name = "xflowMacsecCid6";
    }
    if (DNX_XFLOW_MACSEC_CID_7 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_7, 0);
        data->hw_access = mMACSEC_ESEC_MIB_SC_CTRL;
        data->counter_name = "xflowMacsecCid7";
    }
    if (DNX_XFLOW_MACSEC_CID_8 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_8, 0);
        data->hw_access = mMACSEC_ESEC_MIB_SC;
        data->counter_name = "xflowMacsecCid8";
    }
    if (DNX_XFLOW_MACSEC_CID_9 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_9, 0);
        data->hw_access = mMACSEC_ESEC_MIB_SA;
        data->counter_name = "xflowMacsecCid9";
    }
    if (DNX_XFLOW_MACSEC_CID_10 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_10, 0);
        data->hw_access = mMACSEC_ISEC_SPTCAM_HIT_COUNT;
        data->counter_name = "xflowMacsecCid10";
    }
    if (DNX_XFLOW_MACSEC_CID_11 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_11, 0);
        data->hw_access = mMACSEC_ISEC_PORT_COUNTERS;
        data->counter_name = "xflowMacsecCid11";
    }
    if (DNX_XFLOW_MACSEC_CID_12 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_12, 0);
        data->hw_access = mMACSEC_ISEC_SCTCAM_HIT_COUNT;
        data->counter_name = "xflowMacsecCid12";
    }
    if (DNX_XFLOW_MACSEC_CID_13 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_13, 0);
        data->hw_access = rMACSEC_ISEC_AES_ICV_FAIL_CNT;
        data->counter_name = "xflowMacsecCid13";
        data->is_reg = 1;
    }
    if (DNX_XFLOW_MACSEC_CID_14 < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_14, 0);
        data->hw_access = rMACSEC_ISEC_MTU_FAIL_CNT;
        data->counter_name = "xflowMacsecCid14";
        data->is_reg = 1;
    }
    if (DNX_XFLOW_MACSEC_CID_COUNT < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_COUNT, 0);
        data->counter_name = "xflowMacsecCidCount";
        data->valid = 0;
    }
    if (DNX_XFLOW_MACSEC_CID_SPECIAL < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_counter_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_XFLOW_MACSEC_CID_SPECIAL, 0);
        data->counter_name = "xflowMacsecCidSpecial";
        data->valid = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


#endif 
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_ACCESS_SUPPORT) 


static shr_error_e
j2p_a0_dnx_data_macsec_general_access_stat_map_set(
    int unit)
{
    int stat_type_index;
    dnx_data_macsec_general_access_stat_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int table_index = dnx_data_macsec_general_table_access_stat_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = xflowMacsecStatTypeCount;
    table->info_get.key_size[0] = xflowMacsecStatTypeCount;

    
    table->values[0].default_val = "DNX_XFLOW_MACSEC_CID_INVALID";
    table->values[1].default_val = "INVALIDf";
    table->values[2].default_val = "XFLOW_MACSEC_ENCRYPT_DECRYPT_NONE";
    table->values[3].default_val = "xflowMacsecIdTypeCount";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    table->values[6].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_macsec_general_access_stat_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_macsec_general_table_access_stat_map");

    
    default_data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->counter_id = DNX_XFLOW_MACSEC_CID_INVALID;
    default_data->field = INVALIDf;
    default_data->direction = XFLOW_MACSEC_ENCRYPT_DECRYPT_NONE;
    default_data->id_type = xflowMacsecIdTypeCount;
    default_data->macsec_supported = 0;
    default_data->ipsec_supported = 0;
    default_data->valid = 1;
    
    for (stat_type_index = 0; stat_type_index < table->keys[0].size; stat_type_index++)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, stat_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (xflowMacsecStatTypeInvalid < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecStatTypeInvalid, 0);
        data->valid = 0;
    }
    if (xflowMacsecUnctrlPortInOctets < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortInOctets, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_0;
        data->field = mMACSEC_ISEC_MIB_SP_UNCTRL_fIN_OCTETS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecUnctrlPortInUcastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortInUcastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_0;
        data->field = mMACSEC_ISEC_MIB_SP_UNCTRL_fIN_UCAST_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecUnctrlPortInMulticastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortInMulticastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_0;
        data->field = mMACSEC_ISEC_MIB_SP_UNCTRL_fIN_MULTICAST_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecUnctrlPortInBroadcastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortInBroadcastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_0;
        data->field = mMACSEC_ISEC_MIB_SP_UNCTRL_fIN_BROADCAST_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecUnctrlPortInDiscards < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortInDiscards, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_COUNT;
        data->field = ACCESS_INVALID_FIELD_ID;
        data->direction = XFLOW_MACSEC_ENCRYPT_DECRYPT_NONE;
        data->id_type = xflowMacsecIdTypeInvalid;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecUnctrlPortOutOctets < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortOutOctets, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_6;
        data->field = mMACSEC_ESEC_MIB_SC_UN_CTRL_fOUT_OCTETS;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecUnctrlPortOutUcastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortOutUcastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_6;
        data->field = mMACSEC_ESEC_MIB_SC_UN_CTRL_fOUT_UCAST_PKTS;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecUnctrlPortOutMulticastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortOutMulticastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_6;
        data->field = mMACSEC_ESEC_MIB_SC_UN_CTRL_fOUT_MULTICAST_PKTS;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecUnctrlPortOutBroadcastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortOutBroadcastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_6;
        data->field = mMACSEC_ESEC_MIB_SC_UN_CTRL_fOUT_BROADCAST_PKTS;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecUnctrlPortOutErrors < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortOutErrors, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_6;
        data->field = mMACSEC_ESEC_MIB_SC_UN_CTRL_fOUT_ERRORS;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecCtrlPortInOctets < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortInOctets, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_2;
        data->field = mMACSEC_ISEC_MIB_SP_CTRL_2_fIN_OCTETS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecCtrlPortInUcastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortInUcastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_2;
        data->field = mMACSEC_ISEC_MIB_SP_CTRL_2_fIN_UCAST_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecCtrlPortInMulticastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortInMulticastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_2;
        data->field = mMACSEC_ISEC_MIB_SP_CTRL_2_fIN_MULTICAST_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecCtrlPortInBroadcastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortInBroadcastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_2;
        data->field = mMACSEC_ISEC_MIB_SP_CTRL_2_fIN_BROADCAST_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecCtrlPortInDiscards < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortInDiscards, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_2;
        data->field = mMACSEC_ISEC_MIB_SP_CTRL_2_fIN_DISCARDS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecCtrlPortInErrors < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortInErrors, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_2;
        data->field = mMACSEC_ISEC_MIB_SP_CTRL_2_fIN_ERRORS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecCtrlPortOutOctets < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortOutOctets, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_7;
        data->field = mMACSEC_ESEC_MIB_SC_CTRL_fOUT_OCTETS;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecCtrlPortOutUcastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortOutUcastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_7;
        data->field = mMACSEC_ESEC_MIB_SC_CTRL_fOUT_UCAST_PKTS;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecCtrlPortOutMulticastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortOutMulticastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_7;
        data->field = mMACSEC_ESEC_MIB_SC_CTRL_fOUT_MULTICAST_PKTS;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecCtrlPortOutBroadcastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortOutBroadcastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_7;
        data->field = mMACSEC_ESEC_MIB_SC_CTRL_fOUT_BROADCAST_PKTS;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecCtrlPortOutErrors < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortOutErrors, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_8;
        data->field = mMACSEC_ESEC_MIB_SC_fOUT_ERRORS;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyStatsTxUntaggedPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyStatsTxUntaggedPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_8;
        data->field = mMACSEC_ESEC_MIB_SC_fSECY_STATS_TX_UNTAGGED_PKTS;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyStatsTxTooLongPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyStatsTxTooLongPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_8;
        data->field = mMACSEC_ESEC_MIB_SC_fSECY_STATS_TX_TOO_LONG_PKTS;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecSecyStatsRxUntaggedPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyStatsRxUntaggedPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_1;
        data->field = mMACSEC_ISEC_MIB_SP_CTRL_1_fSECY_STATS_RX_UNTAGGED_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecSecyStatsRxNoTagPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyStatsRxNoTagPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_1;
        data->field = mMACSEC_ISEC_MIB_SP_CTRL_1_fSECY_STATS_RX_NO_TAG_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyStatsRxBadTagPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyStatsRxBadTagPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_1;
        data->field = mMACSEC_ISEC_MIB_SP_CTRL_1_fSECY_STATS_RX_BAD_TAG_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyStatsRxUnknownSCIPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyStatsRxUnknownSCIPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_1;
        data->field = mMACSEC_ISEC_MIB_SP_CTRL_1_fSECY_STATS_RX_UNKNOWN_SCI_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecSecyStatsRxNoSCIPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyStatsRxNoSCIPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_1;
        data->field = mMACSEC_ISEC_MIB_SP_CTRL_1_fSECY_STATS_RX_NO_SCI_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecSecyStatsRxOverrunPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyStatsRxOverrunPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_SPECIAL;
        data->field = ACCESS_INVALID_FIELD_ID;
        data->direction = XFLOW_MACSEC_ENCRYPT_DECRYPT_NONE;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyTxSCStatsProtectedPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyTxSCStatsProtectedPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_SPECIAL;
        data->field = ACCESS_INVALID_FIELD_ID;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyTxSCStatsEncryptedPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyTxSCStatsEncryptedPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_SPECIAL;
        data->field = ACCESS_INVALID_FIELD_ID;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyTxSCStatsOctetsProtected < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyTxSCStatsOctetsProtected, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_8;
        data->field = mMACSEC_ESEC_MIB_SC_fSECY_TX_SC_STATS_OCTETS_PROTECTED;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyTxSCStatsOctetsEncrypted < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyTxSCStatsOctetsEncrypted, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_8;
        data->field = mMACSEC_ESEC_MIB_SC_fSECY_TX_SC_STATS_OCTETS_ENCRYPTED;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyRxSCStatsUnusedSAPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSCStatsUnusedSAPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_SPECIAL;
        data->field = ACCESS_INVALID_FIELD_ID;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecSecyRxSCStatsNotUsingSAPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSCStatsNotUsingSAPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_SPECIAL;
        data->field = ACCESS_INVALID_FIELD_ID;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecSecyRxSCStatsLatePkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSCStatsLatePkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_3;
        data->field = mMACSEC_ISEC_MIB_SC_fSECY_RX_SC_STATS_LATE_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyRxSCStatsNotValidPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSCStatsNotValidPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_SPECIAL;
        data->field = ACCESS_INVALID_FIELD_ID;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyRxSCStatsInvalidPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSCStatsInvalidPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_SPECIAL;
        data->field = ACCESS_INVALID_FIELD_ID;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecSecyRxSCStatsDelayedPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSCStatsDelayedPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_3;
        data->field = mMACSEC_ISEC_MIB_SC_fSECY_RX_SC_STATS_DELAYED_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyRxSCStatsUncheckedPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSCStatsUncheckedPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_3;
        data->field = mMACSEC_ISEC_MIB_SC_fSECY_RX_SC_STATS_UNCHECKED_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyRxSCStatsOKPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSCStatsOKPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_SPECIAL;
        data->field = ACCESS_INVALID_FIELD_ID;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyRxSCStatsOctetsValidated < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSCStatsOctetsValidated, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_3;
        data->field = mMACSEC_ISEC_MIB_SC_fSECY_RX_SC_STATS_OCTETS_VALIDATED;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyRxSCStatsOctetsDecrypted < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSCStatsOctetsDecrypted, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_3;
        data->field = mMACSEC_ISEC_MIB_SC_fSECY_RX_SC_STATS_OCTETS_DECRYPTED;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyTxSAStatsProtectedPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyTxSAStatsProtectedPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_9;
        data->field = mMACSEC_ESEC_MIB_SA_fSECY_TX_SA_STATS_PROTECTED_PKTS;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSecureAssoc;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyTxSAStatsEncryptedPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyTxSAStatsEncryptedPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_9;
        data->field = mMACSEC_ESEC_MIB_SA_fSECY_TX_SA_STATS_ENCRYPTED_PKTS;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSecureAssoc;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyRxSAStatsUnusedSAPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSAStatsUnusedSAPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_4;
        data->field = mMACSEC_ISEC_MIB_SA_fSECY_RX_SA_STATS_UNUSED_SA_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureAssoc;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecSecyRxSAStatsNotUsingSAPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSAStatsNotUsingSAPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_4;
        data->field = mMACSEC_ISEC_MIB_SA_fSECY_RX_SA_STATS_NOT_USING_SA_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureAssoc;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecSecyRxSAStatsNotValidPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSAStatsNotValidPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_4;
        data->field = mMACSEC_ISEC_MIB_SA_fSECY_RX_SA_STATS_NOT_VALID_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureAssoc;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyRxSAStatsInvalidPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSAStatsInvalidPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_4;
        data->field = mMACSEC_ISEC_MIB_SA_fSECY_RX_SA_STATS_INVALID_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureAssoc;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecSecyRxSAStatsOKPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSAStatsOKPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_4;
        data->field = mMACSEC_ISEC_MIB_SA_fSECY_RX_SA_STATS_OK_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureAssoc;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecInMgmtPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecInMgmtPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_0;
        data->field = mMACSEC_ISEC_MIB_SP_UNCTRL_fIN_MGMT_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecFlowTcamHitCntr < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecFlowTcamHitCntr, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_10;
        data->field = mMACSEC_ISEC_SPTCAM_HIT_COUNT_fSPTCAM_HIT;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeFlow;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecFlowTcamMissCntr < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecFlowTcamMissCntr, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_11;
        data->field = mMACSEC_ISEC_PORT_COUNTERS_fSPTCAM_MISS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypePort;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecScTcamHitCntr < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecScTcamHitCntr, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_12;
        data->field = mMACSEC_ISEC_SCTCAM_HIT_COUNT_fSCTCAM_HIT;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecScTcamMissCntr < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecScTcamMissCntr, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_11;
        data->field = mMACSEC_ISEC_PORT_COUNTERS_fSCTCAM_MISS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypePort;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecOutMgmtPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecOutMgmtPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_6;
        data->field = mMACSEC_ESEC_MIB_SC_UN_CTRL_fOUT_MGMT_PKTS;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecInPacketDropCntr < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecInPacketDropCntr, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_11;
        data->field = mMACSEC_ISEC_PORT_COUNTERS_fPKTDROP;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypePort;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecOutPacketDropCntr < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecOutPacketDropCntr, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_5;
        data->field = mMACSEC_ESEC_MIB_MISC_fPKT_DROP;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypePort;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecBadOlpHdrCntr < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecBadOlpHdrCntr, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_COUNT;
        data->field = ACCESS_INVALID_FIELD_ID;
        data->direction = XFLOW_MACSEC_ENCRYPT_DECRYPT_NONE;
        data->id_type = xflowMacsecIdTypeInvalid;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecBadSvtagHdrCntr < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecBadSvtagHdrCntr, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_5;
        data->field = mMACSEC_ESEC_MIB_MISC_fBAD_CUSTOM_HDR;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypePort;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecUnctrlPortInKayPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortInKayPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_0;
        data->field = mMACSEC_ISEC_MIB_SP_UNCTRL_fIN_KA_Y_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecIcvFailPktsCntr < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIcvFailPktsCntr, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_COUNT;
        data->field = ACCESS_INVALID_FIELD_ID;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypePort;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecMtuFailPktsCntr < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecMtuFailPktsCntr, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_COUNT;
        data->field = ACCESS_INVALID_FIELD_ID;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypePort;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecIPsecCtrlPortDummyPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIPsecCtrlPortDummyPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_7;
        data->field = mMACSEC_ESEC_MIB_SC_CTRL_fDUMMY_PKTS;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecIPsecCtrlPortIPLengthMismatch < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIPsecCtrlPortIPLengthMismatch, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_7;
        data->field = mMACSEC_ESEC_MIB_SC_CTRL_fIP_LENGTH_MISMATCH;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 0;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecIPsecTxOutErrors < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIPsecTxOutErrors, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_8;
        data->field = mMACSEC_ESEC_MIB_SC_fOUT_ERRORS;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 0;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecIPsecUnctrlPortInIkePkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIPsecUnctrlPortInIkePkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_0;
        data->field = mMACSEC_ISEC_MIB_SP_UNCTRL_fIN_IKE_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 0;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecIPsecSecyRxNoSPIPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIPsecSecyRxNoSPIPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_1;
        data->field = mMACSEC_ISEC_MIB_SP_CTRL_1_fSECY_IP_SEC_RX_NO_SPI_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 0;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecIPsecSecyRxIPFragmentsSetPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIPsecSecyRxIPFragmentsSetPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_1;
        data->field = mMACSEC_ISEC_MIB_SP_CTRL_1_fSECY_IP_SEC_RX_IP_FRAGMENTS_SET_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 0;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecIPSecSecyRxIllegalNxtHdrPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIPSecSecyRxIllegalNxtHdrPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_1;
        data->field = mMACSEC_ISEC_MIB_SP_CTRL_1_fSECY_STATS_IP_SEC_RX_ILLEGAL_NXT_HDR_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 0;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecIPSecRxNoSAPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIPSecRxNoSAPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_4;
        data->field = mMACSEC_ISEC_MIB_SA_fSECY_IP_SEC_RX_NO_SA_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureAssoc;
        data->macsec_supported = 0;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecIPSecRxSADummyPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIPSecRxSADummyPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_4;
        data->field = mMACSEC_ISEC_MIB_SA_fSECY_IP_SEC_RX_SA_DUMMY_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureAssoc;
        data->macsec_supported = 0;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecIPSecRxSAPadMismatchPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_access_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIPSecRxSAPadMismatchPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_4;
        data->field = mMACSEC_ISEC_MIB_SA_fSECY_IP_SEC_RX_SA_PAD_MISMATCH_PKTS;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureAssoc;
        data->macsec_supported = 0;
        data->ipsec_supported = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


#endif 
#ifdef INCLUDE_XFLOW_MACSEC


static shr_error_e
j2p_a0_dnx_data_macsec_general_stat_map_set(
    int unit)
{
    int stat_type_index;
    dnx_data_macsec_general_stat_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_general;
    int table_index = dnx_data_macsec_general_table_stat_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = xflowMacsecStatTypeCount;
    table->info_get.key_size[0] = xflowMacsecStatTypeCount;

    
    table->values[0].default_val = "DNX_XFLOW_MACSEC_CID_INVALID";
    table->values[1].default_val = "INVALIDf";
    table->values[2].default_val = "XFLOW_MACSEC_ENCRYPT_DECRYPT_NONE";
    table->values[3].default_val = "xflowMacsecIdTypeCount";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    table->values[6].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_macsec_general_stat_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_macsec_general_table_stat_map");

    
    default_data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->counter_id = DNX_XFLOW_MACSEC_CID_INVALID;
    default_data->field = INVALIDf;
    default_data->direction = XFLOW_MACSEC_ENCRYPT_DECRYPT_NONE;
    default_data->id_type = xflowMacsecIdTypeCount;
    default_data->macsec_supported = 0;
    default_data->ipsec_supported = 0;
    default_data->valid = 1;
    
    for (stat_type_index = 0; stat_type_index < table->keys[0].size; stat_type_index++)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, stat_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (xflowMacsecStatTypeInvalid < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecStatTypeInvalid, 0);
        data->valid = 0;
    }
    if (xflowMacsecUnctrlPortInOctets < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortInOctets, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_0;
        data->field = IN_OCTETSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecUnctrlPortInUcastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortInUcastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_0;
        data->field = IN_UCAST_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecUnctrlPortInMulticastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortInMulticastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_0;
        data->field = IN_MULTICAST_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecUnctrlPortInBroadcastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortInBroadcastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_0;
        data->field = IN_BROADCAST_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecUnctrlPortInDiscards < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortInDiscards, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_COUNT;
        data->field = INVALIDf;
        data->direction = XFLOW_MACSEC_ENCRYPT_DECRYPT_NONE;
        data->id_type = xflowMacsecIdTypeInvalid;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecUnctrlPortOutOctets < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortOutOctets, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_6;
        data->field = OUT_OCTETSf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecUnctrlPortOutUcastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortOutUcastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_6;
        data->field = OUT_UCAST_PKTSf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecUnctrlPortOutMulticastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortOutMulticastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_6;
        data->field = OUT_MULTICAST_PKTSf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecUnctrlPortOutBroadcastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortOutBroadcastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_6;
        data->field = OUT_BROADCAST_PKTSf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecUnctrlPortOutErrors < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortOutErrors, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_6;
        data->field = OUT_ERRORSf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecCtrlPortInOctets < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortInOctets, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_2;
        data->field = IN_OCTETSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecCtrlPortInUcastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortInUcastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_2;
        data->field = IN_UCAST_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecCtrlPortInMulticastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortInMulticastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_2;
        data->field = IN_MULTICAST_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecCtrlPortInBroadcastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortInBroadcastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_2;
        data->field = IN_BROADCAST_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecCtrlPortInDiscards < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortInDiscards, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_2;
        data->field = IN_DISCARDSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecCtrlPortInErrors < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortInErrors, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_2;
        data->field = IN_ERRORSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecCtrlPortOutOctets < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortOutOctets, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_7;
        data->field = OUT_OCTETSf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecCtrlPortOutUcastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortOutUcastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_7;
        data->field = OUT_UCAST_PKTSf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecCtrlPortOutMulticastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortOutMulticastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_7;
        data->field = OUT_MULTICAST_PKTSf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecCtrlPortOutBroadcastPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortOutBroadcastPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_7;
        data->field = OUT_BROADCAST_PKTSf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecCtrlPortOutErrors < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecCtrlPortOutErrors, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_8;
        data->field = OUT_ERRORSf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyStatsTxUntaggedPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyStatsTxUntaggedPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_8;
        data->field = SECY_STATS_TX_UNTAGGED_PKTSf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyStatsTxTooLongPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyStatsTxTooLongPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_8;
        data->field = SECY_STATS_TX_TOO_LONG_PKTSf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecSecyStatsRxUntaggedPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyStatsRxUntaggedPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_1;
        data->field = SECY_STATS_RX_UNTAGGED_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecSecyStatsRxNoTagPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyStatsRxNoTagPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_1;
        data->field = SECY_STATS_RX_NO_TAG_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyStatsRxBadTagPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyStatsRxBadTagPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_1;
        data->field = SECY_STATS_RX_BAD_TAG_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyStatsRxUnknownSCIPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyStatsRxUnknownSCIPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_1;
        data->field = SECY_STATS_RX_UNKNOWN_SCI_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecSecyStatsRxNoSCIPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyStatsRxNoSCIPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_1;
        data->field = SECY_STATS_RX_NO_SCI_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecSecyStatsRxOverrunPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyStatsRxOverrunPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_SPECIAL;
        data->field = INVALIDf;
        data->direction = XFLOW_MACSEC_ENCRYPT_DECRYPT_NONE;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyTxSCStatsProtectedPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyTxSCStatsProtectedPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_SPECIAL;
        data->field = SECY_TX_SA_STATS_PROTECTED_PKTSf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyTxSCStatsEncryptedPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyTxSCStatsEncryptedPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_SPECIAL;
        data->field = SECY_TX_SA_STATS_ENCRYPTED_PKTSf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyTxSCStatsOctetsProtected < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyTxSCStatsOctetsProtected, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_8;
        data->field = SECY_TX_SC_STATS_OCTETS_PROTECTEDf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyTxSCStatsOctetsEncrypted < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyTxSCStatsOctetsEncrypted, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_8;
        data->field = SECY_TX_SC_STATS_OCTETS_ENCRYPTEDf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyRxSCStatsUnusedSAPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSCStatsUnusedSAPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_SPECIAL;
        data->field = SECY_RX_SA_STATS_UNUSED_SA_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecSecyRxSCStatsNotUsingSAPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSCStatsNotUsingSAPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_SPECIAL;
        data->field = SECY_RX_SA_STATS_NOT_USING_SA_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecSecyRxSCStatsLatePkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSCStatsLatePkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_3;
        data->field = SECY_RX_SC_STATS_LATE_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyRxSCStatsNotValidPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSCStatsNotValidPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_SPECIAL;
        data->field = SECY_RX_SA_STATS_NOT_VALID_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyRxSCStatsInvalidPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSCStatsInvalidPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_SPECIAL;
        data->field = SECY_RX_SA_STATS_INVALID_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecSecyRxSCStatsDelayedPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSCStatsDelayedPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_3;
        data->field = SECY_RX_SC_STATS_DELAYED_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyRxSCStatsUncheckedPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSCStatsUncheckedPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_3;
        data->field = SECY_RX_SC_STATS_UNCHECKED_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyRxSCStatsOKPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSCStatsOKPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_SPECIAL;
        data->field = SECY_RX_SA_STATS_OK_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyRxSCStatsOctetsValidated < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSCStatsOctetsValidated, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_3;
        data->field = SECY_RX_SC_STATS_OCTETS_VALIDATEDf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyRxSCStatsOctetsDecrypted < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSCStatsOctetsDecrypted, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_3;
        data->field = SECY_RX_SC_STATS_OCTETS_DECRYPTEDf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyTxSAStatsProtectedPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyTxSAStatsProtectedPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_9;
        data->field = SECY_TX_SA_STATS_PROTECTED_PKTSf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSecureAssoc;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyTxSAStatsEncryptedPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyTxSAStatsEncryptedPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_9;
        data->field = SECY_TX_SA_STATS_ENCRYPTED_PKTSf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSecureAssoc;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyRxSAStatsUnusedSAPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSAStatsUnusedSAPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_4;
        data->field = SECY_RX_SA_STATS_UNUSED_SA_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureAssoc;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecSecyRxSAStatsNotUsingSAPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSAStatsNotUsingSAPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_4;
        data->field = SECY_RX_SA_STATS_NOT_USING_SA_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureAssoc;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecSecyRxSAStatsNotValidPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSAStatsNotValidPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_4;
        data->field = SECY_RX_SA_STATS_NOT_VALID_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureAssoc;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecSecyRxSAStatsInvalidPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSAStatsInvalidPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_4;
        data->field = SECY_RX_SA_STATS_INVALID_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureAssoc;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecSecyRxSAStatsOKPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecSecyRxSAStatsOKPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_4;
        data->field = SECY_RX_SA_STATS_OK_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureAssoc;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecInMgmtPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecInMgmtPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_0;
        data->field = IN_MGMT_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecFlowTcamHitCntr < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecFlowTcamHitCntr, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_10;
        data->field = SPTCAM_HITf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeFlow;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecFlowTcamMissCntr < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecFlowTcamMissCntr, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_11;
        data->field = SPTCAM_MISSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypePort;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecScTcamHitCntr < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecScTcamHitCntr, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_12;
        data->field = SCTCAM_HITf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureChan;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecScTcamMissCntr < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecScTcamMissCntr, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_11;
        data->field = SCTCAM_MISSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypePort;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecOutMgmtPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecOutMgmtPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_6;
        data->field = OUT_MGMT_PKTSf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecInPacketDropCntr < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecInPacketDropCntr, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_11;
        data->field = PKTDROPf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypePort;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecOutPacketDropCntr < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecOutPacketDropCntr, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_5;
        data->field = PKT_DROPf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypePort;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecBadOlpHdrCntr < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecBadOlpHdrCntr, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_COUNT;
        data->field = INVALIDf;
        data->direction = XFLOW_MACSEC_ENCRYPT_DECRYPT_NONE;
        data->id_type = xflowMacsecIdTypeInvalid;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecBadSvtagHdrCntr < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecBadSvtagHdrCntr, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_5;
        data->field = BAD_CUSTOM_HDRf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypePort;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecUnctrlPortInKayPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecUnctrlPortInKayPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_0;
        data->field = IN_KA_Y_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecIcvFailPktsCntr < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIcvFailPktsCntr, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_COUNT;
        data->field = ICV_FAIL_CNT_Nf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypePort;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecMtuFailPktsCntr < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecMtuFailPktsCntr, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_COUNT;
        data->field = MTU_FAIL_CNT_Nf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypePort;
        data->macsec_supported = 1;
        data->ipsec_supported = 0;
    }
    if (xflowMacsecIPsecCtrlPortDummyPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIPsecCtrlPortDummyPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_7;
        data->field = DUMMY_PKTSf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 1;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecIPsecCtrlPortIPLengthMismatch < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIPsecCtrlPortIPLengthMismatch, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_7;
        data->field = IP_LENGTH_MISMATCHf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 0;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecIPsecTxOutErrors < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIPsecTxOutErrors, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_8;
        data->field = OUT_ERRORSf;
        data->direction = XFLOW_MACSEC_ENCRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 0;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecIPsecUnctrlPortInIkePkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIPsecUnctrlPortInIkePkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_0;
        data->field = IN_IKE_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 0;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecIPsecSecyRxNoSPIPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIPsecSecyRxNoSPIPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_1;
        data->field = SECY_IP_SEC_RX_NO_SPI_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 0;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecIPsecSecyRxIPFragmentsSetPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIPsecSecyRxIPFragmentsSetPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_1;
        data->field = SECY_IP_SEC_RX_IP_FRAGMENTS_SET_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 0;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecIPSecSecyRxIllegalNxtHdrPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIPSecSecyRxIllegalNxtHdrPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_1;
        data->field = SECY_STATS_IP_SEC_RX_ILLEGAL_NXT_HDR_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSubportNum;
        data->macsec_supported = 0;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecIPSecRxNoSAPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIPSecRxNoSAPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_4;
        data->field = SECY_IP_SEC_RX_NO_SA_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureAssoc;
        data->macsec_supported = 0;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecIPSecRxSADummyPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIPSecRxSADummyPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_4;
        data->field = SECY_IP_SEC_RX_SA_DUMMY_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureAssoc;
        data->macsec_supported = 0;
        data->ipsec_supported = 1;
    }
    if (xflowMacsecIPSecRxSAPadMismatchPkts < table->keys[0].size)
    {
        data = (dnx_data_macsec_general_stat_map_t *) dnxc_data_mgmt_table_data_get(unit, table, xflowMacsecIPSecRxSAPadMismatchPkts, 0);
        data->counter_id = DNX_XFLOW_MACSEC_CID_4;
        data->field = SECY_IP_SEC_RX_SA_PAD_MISMATCH_PKTSf;
        data->direction = XFLOW_MACSEC_DECRYPT;
        data->id_type = xflowMacsecIdTypeSecureAssoc;
        data->macsec_supported = 0;
        data->ipsec_supported = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


#endif 




static shr_error_e
j2p_a0_dnx_data_macsec_ingress_restricted_hw_ids_exist_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_ingress;
    int feature_index = dnx_data_macsec_ingress_restricted_hw_ids_exist;
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
j2p_a0_dnx_data_macsec_ingress_flow_nof_set(
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
j2p_a0_dnx_data_macsec_ingress_policy_nof_set(
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
j2p_a0_dnx_data_macsec_ingress_secure_assoc_nof_set(
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
j2p_a0_dnx_data_macsec_ingress_udf_nof_bits_set(
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
j2p_a0_dnx_data_macsec_ingress_mgmt_rule_exact_nof_set(
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
j2p_a0_dnx_data_macsec_ingress_tpid_nof_set(
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
j2p_a0_dnx_data_macsec_ingress_cpu_flex_map_nof_set(
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
j2p_a0_dnx_data_macsec_ingress_sc_tcam_nof_set(
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
j2p_a0_dnx_data_macsec_ingress_sa_per_sc_nof_set(
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
j2p_a0_dnx_data_macsec_ingress_invalidate_sa_set(
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
j2p_a0_dnx_data_macsec_ingress_secure_channel_nof_set(
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
j2p_a0_dnx_data_macsec_ingress_udf_set(
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
        data->payload_len = 280;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->payload_len = 280;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->payload_len = 280;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->payload_len = 296;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->payload_len = 168;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->payload_len = 344;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->payload_len = 264;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->payload_len = 240;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->payload_len = 208;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->payload_len = 208;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->payload_len = 96;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->payload_len = 64;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_udf_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->payload_len = 64;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_macsec_ingress_restricted_sc_tcam_id_set(
    int unit)
{
    int key_index_index;
    dnx_data_macsec_ingress_restricted_sc_tcam_id_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_ingress;
    int table_index = dnx_data_macsec_ingress_table_restricted_sc_tcam_id;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 1;
    table->info_get.key_size[0] = 1;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_macsec_ingress_restricted_sc_tcam_id_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_macsec_ingress_table_restricted_sc_tcam_id");

    
    default_data = (dnx_data_macsec_ingress_restricted_sc_tcam_id_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->hw_id = -1;
    
    for (key_index_index = 0; key_index_index < table->keys[0].size; key_index_index++)
    {
        data = (dnx_data_macsec_ingress_restricted_sc_tcam_id_t *) dnxc_data_mgmt_table_data_get(unit, table, key_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_restricted_sc_tcam_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->hw_id = 255;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_macsec_ingress_restricted_sp_tcam_id_set(
    int unit)
{
    int key_index_index;
    dnx_data_macsec_ingress_restricted_sp_tcam_id_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_ingress;
    int table_index = dnx_data_macsec_ingress_table_restricted_sp_tcam_id;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 1;
    table->info_get.key_size[0] = 1;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_macsec_ingress_restricted_sp_tcam_id_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_macsec_ingress_table_restricted_sp_tcam_id");

    
    default_data = (dnx_data_macsec_ingress_restricted_sp_tcam_id_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->hw_id = -1;
    
    for (key_index_index = 0; key_index_index < table->keys[0].size; key_index_index++)
    {
        data = (dnx_data_macsec_ingress_restricted_sp_tcam_id_t *) dnxc_data_mgmt_table_data_get(unit, table, key_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_macsec_ingress_restricted_sp_tcam_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->hw_id = 511;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2p_a0_dnx_data_macsec_egress_secure_assoc_nof_set(
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
j2p_a0_dnx_data_macsec_egress_soft_expiry_threshold_profiles_nof_set(
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
j2p_a0_dnx_data_macsec_egress_an_bit_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_egress;
    int define_index = dnx_data_macsec_egress_define_an_bit_nof;
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
j2p_a0_dnx_data_macsec_egress_nh_profiles_nof_set(
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
j2p_a0_dnx_data_macsec_egress_association_number_actual_range_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_egress;
    int define_index = dnx_data_macsec_egress_define_association_number_actual_range_nof;
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
j2p_a0_dnx_data_macsec_egress_sa_per_sc_nof_set(
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
j2p_a0_dnx_data_macsec_egress_secure_channel_nof_set(
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
j2p_a0_dnx_data_macsec_wrapper_macsec_arb_ports_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_wrapper;
    int define_index = dnx_data_macsec_wrapper_define_macsec_arb_ports_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_macsec_wrapper_nof_tags_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_macsec;
    int submodule_index = dnx_data_macsec_submodule_wrapper;
    int define_index = dnx_data_macsec_wrapper_define_nof_tags;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2p_a0_data_macsec_attach(
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
    define->set = j2p_a0_dnx_data_macsec_general_macsec_nof_set;
    data_index = dnx_data_macsec_general_define_macsec_in_core_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_general_macsec_in_core_nof_set;
    data_index = dnx_data_macsec_general_define_ports_in_macsec_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_general_ports_in_macsec_nof_set;
    data_index = dnx_data_macsec_general_define_etype_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_general_etype_nof_set;
    data_index = dnx_data_macsec_general_define_etype_not_zero_verify;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_general_etype_not_zero_verify_set;
    data_index = dnx_data_macsec_general_define_tx_threshold;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_general_tx_threshold_set;
    data_index = dnx_data_macsec_general_define_macsec_tdm_cal_max_depth;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_general_macsec_tdm_cal_max_depth_set;
    data_index = dnx_data_macsec_general_define_macsec_tdm_cal_hw_depth;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_general_macsec_tdm_cal_hw_depth_set;
    data_index = dnx_data_macsec_general_define_association_number_full_range_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_general_association_number_full_range_nof_set;
    data_index = dnx_data_macsec_general_define_macsec_gen;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_general_macsec_gen_set;
    data_index = dnx_data_macsec_general_define_sectag_offset_min_value;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_general_sectag_offset_min_value_set;
    data_index = dnx_data_macsec_general_define_supported_entries_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_general_supported_entries_nof_set;
    data_index = dnx_data_macsec_general_define_is_macsec_enabled;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_general_is_macsec_enabled_set;
    data_index = dnx_data_macsec_general_define_is_power_optimization_enabled;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_general_is_power_optimization_enabled_set;

    
    data_index = dnx_data_macsec_general_macsec_block_exists;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_macsec_general_macsec_block_exists_set;
    data_index = dnx_data_macsec_general_is_hw_reset_init_done_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_macsec_general_is_hw_reset_init_done_supported_set;
    data_index = dnx_data_macsec_general_is_port_based_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_macsec_general_is_port_based_supported_set;
    data_index = dnx_data_macsec_general_an_full_range_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_macsec_general_an_full_range_supported_set;
    data_index = dnx_data_macsec_general_rfc_6054_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_macsec_general_rfc_6054_supported_set;
    data_index = dnx_data_macsec_general_multiple_pms_per_macsec_instance;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_macsec_general_multiple_pms_per_macsec_instance_set;

    
    data_index = dnx_data_macsec_general_table_pm_to_macsec;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_macsec_general_pm_to_macsec_set;
    data_index = dnx_data_macsec_general_table_macsec_instances;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_macsec_general_macsec_instances_set;
    data_index = dnx_data_macsec_general_table_macsec_block_mapping;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_macsec_general_macsec_block_mapping_set;
#ifdef INCLUDE_XFLOW_MACSEC

    data_index = dnx_data_macsec_general_table_macsec_control_options;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_macsec_general_macsec_control_options_set;

#endif 
#ifdef INCLUDE_XFLOW_MACSEC

    data_index = dnx_data_macsec_general_table_stat_special_map;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_macsec_general_stat_special_map_set;

#endif 
#ifdef INCLUDE_XFLOW_MACSEC

    data_index = dnx_data_macsec_general_table_counter_map;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_macsec_general_counter_map_set;

#endif 
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_ACCESS_SUPPORT) 

    data_index = dnx_data_macsec_general_table_access_counter_map;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_macsec_general_access_counter_map_set;

#endif 
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_ACCESS_SUPPORT) 

    data_index = dnx_data_macsec_general_table_access_stat_map;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_macsec_general_access_stat_map_set;

#endif 
#ifdef INCLUDE_XFLOW_MACSEC

    data_index = dnx_data_macsec_general_table_stat_map;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_macsec_general_stat_map_set;

#endif 
    
    submodule_index = dnx_data_macsec_submodule_ingress;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_macsec_ingress_define_flow_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_ingress_flow_nof_set;
    data_index = dnx_data_macsec_ingress_define_policy_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_ingress_policy_nof_set;
    data_index = dnx_data_macsec_ingress_define_secure_assoc_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_ingress_secure_assoc_nof_set;
    data_index = dnx_data_macsec_ingress_define_udf_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_ingress_udf_nof_bits_set;
    data_index = dnx_data_macsec_ingress_define_mgmt_rule_exact_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_ingress_mgmt_rule_exact_nof_set;
    data_index = dnx_data_macsec_ingress_define_tpid_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_ingress_tpid_nof_set;
    data_index = dnx_data_macsec_ingress_define_cpu_flex_map_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_ingress_cpu_flex_map_nof_set;
    data_index = dnx_data_macsec_ingress_define_sc_tcam_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_ingress_sc_tcam_nof_set;
    data_index = dnx_data_macsec_ingress_define_sa_per_sc_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_ingress_sa_per_sc_nof_set;
    data_index = dnx_data_macsec_ingress_define_invalidate_sa;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_ingress_invalidate_sa_set;
    data_index = dnx_data_macsec_ingress_define_secure_channel_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_ingress_secure_channel_nof_set;

    
    data_index = dnx_data_macsec_ingress_restricted_hw_ids_exist;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_macsec_ingress_restricted_hw_ids_exist_set;

    
    data_index = dnx_data_macsec_ingress_table_udf;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_macsec_ingress_udf_set;
    data_index = dnx_data_macsec_ingress_table_restricted_sc_tcam_id;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_macsec_ingress_restricted_sc_tcam_id_set;
    data_index = dnx_data_macsec_ingress_table_restricted_sp_tcam_id;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_macsec_ingress_restricted_sp_tcam_id_set;
    
    submodule_index = dnx_data_macsec_submodule_egress;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_macsec_egress_define_secure_assoc_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_egress_secure_assoc_nof_set;
    data_index = dnx_data_macsec_egress_define_soft_expiry_threshold_profiles_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_egress_soft_expiry_threshold_profiles_nof_set;
    data_index = dnx_data_macsec_egress_define_an_bit_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_egress_an_bit_nof_set;
    data_index = dnx_data_macsec_egress_define_nh_profiles_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_egress_nh_profiles_nof_set;
    data_index = dnx_data_macsec_egress_define_association_number_actual_range_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_egress_association_number_actual_range_nof_set;
    data_index = dnx_data_macsec_egress_define_sa_per_sc_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_egress_sa_per_sc_nof_set;
    data_index = dnx_data_macsec_egress_define_secure_channel_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_egress_secure_channel_nof_set;

    

    
    
    submodule_index = dnx_data_macsec_submodule_wrapper;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_macsec_wrapper_define_macsec_arb_ports_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_wrapper_macsec_arb_ports_nof_set;
    data_index = dnx_data_macsec_wrapper_define_nof_tags;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_macsec_wrapper_nof_tags_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

