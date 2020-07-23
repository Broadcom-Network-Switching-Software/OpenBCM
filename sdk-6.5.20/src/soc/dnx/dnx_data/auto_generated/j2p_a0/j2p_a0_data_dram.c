

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <shared/utilex/utilex_integer_arithmetic.h>







static shr_error_e
j2p_a0_dnx_data_dram_hbm_dword_alignment_check_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int feature_index = dnx_data_dram_hbm_dword_alignment_check;
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
j2p_a0_dnx_data_dram_hbm_low_temperature_dram_protction_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int feature_index = dnx_data_dram_hbm_low_temperature_dram_protction;
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
j2p_a0_dnx_data_dram_hbm_is_delete_bdb_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int feature_index = dnx_data_dram_hbm_is_delete_bdb_supported;
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
j2p_a0_dnx_data_dram_hbm_cpu2tap_access_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int feature_index = dnx_data_dram_hbm_cpu2tap_access;
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
j2p_a0_dnx_data_dram_hbm_apd_phy_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int feature_index = dnx_data_dram_hbm_apd_phy;
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
j2p_a0_dnx_data_dram_hbm_is_hrc_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int feature_index = dnx_data_dram_hbm_is_hrc_supported;
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
j2p_a0_dnx_data_dram_hbm_channel_wds_inflight_threshold_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int feature_index = dnx_data_dram_hbm_channel_wds_inflight_threshold;
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
j2p_a0_dnx_data_dram_hbm_stop_traffic_low_temp_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int define_index = dnx_data_dram_hbm_define_stop_traffic_low_temp_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_DRAM_LOW_TEMPERATURE_THRESHOLD_STOP_TRAFFIC;
    define->property.doc = 
        "\n"
        "This soc property holds the low temperature threshold.\n"
        "When getting below this temperature, system will stop directing traffic to DRAM.\n"
        "Units: degrees Celsius\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = dnx_data_dram.hbm.restore_traffic_low_temp_threshold_get(unit)-1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_hbm_restore_traffic_low_temp_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int define_index = dnx_data_dram_hbm_define_restore_traffic_low_temp_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_DRAM_LOW_TEMPERATURE_THRESHOLD_RESTORE_TRAFFIC;
    define->property.doc = 
        "\n"
        "This soc property holds temperature threshold.\n"
        "When crossing this threshold from below, system will renew directing traffic to DRAM.\n"
        "Units: degrees Celsius\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = dnx_data_dram.hbm.restore_traffic_temp_threshold_get(unit)-1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_hbm_power_down_low_temp_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int define_index = dnx_data_dram_hbm_define_power_down_low_temp_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_DRAM_LOW_TEMPERATURE_THRESHOLD_POWER_DOWN;
    define->property.doc = 
        "\n"
        "This soc property holds temperature threshold.\n"
        "When crossing this threshold from above, system will power down DRAM.\n"
        "Units: degrees Celsius\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = dnx_data_dram.hbm.stop_traffic_low_temp_threshold_get(unit)-1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_hbm_start_disabled_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int define_index = dnx_data_dram_hbm_define_start_disabled;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_RAM_FREEZE_BOOT;
    define->property.doc = 
        "\n"
        "Do not start DRAM during BCM init\n"
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
j2p_a0_dnx_data_dram_hbm_output_enable_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int define_index = dnx_data_dram_hbm_define_output_enable_length;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_dram.hbm.burst_length_get(unit) / 2;

    
    define->data = dnx_data_dram.hbm.burst_length_get(unit) / 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_hbm_output_enable_delay_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int define_index = dnx_data_dram_hbm_define_output_enable_delay;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_dram.general_info.write_latency_get(unit) + 1;

    
    define->data = dnx_data_dram.general_info.write_latency_get(unit) + 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_hbm_wds_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int define_index = dnx_data_dram_hbm_define_wds_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256;

    
    define->data = 256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2p_a0_dnx_data_dram_hbm_channel_symmetric_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_hbm_channel_symmetric_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int table_index = dnx_data_dram_hbm_table_channel_symmetric_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 16;
    table->info_get.key_size[0] = 16;

    
    table->values[0].default_val = "INVALIDr";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_hbm_channel_symmetric_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_hbm_table_channel_symmetric_regs");

    
    default_data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = HBC_HBM_CONFIGr;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = HBC_HBM_MODE_REGISTERSr;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->reg = HBC_HBM_MODE_REGISTER_MANAGER_CONFIGr;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->reg = HBC_DATA_SOURCE_CONFIGr;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->reg = HBC_WPBQ_CONFIGr;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->reg = HBC_RPBQ_CONFIGr;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->reg = HBC_HBM_ROW_ACCESS_TIMINGSr;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->reg = HBC_HBM_COLUMN_ACCESS_TIMINGSr;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->reg = HBC_HBM_REFRESH_TIMINGSr;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->reg = HBC_HBM_GENERAL_TIMINGSr;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->reg = HBC_HBM_REFRESH_INTERVALSr;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->reg = HBC_TSM_CONFIGr;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->reg = HBC_TSM_READ_WRITE_SWITCH_CONFIGr;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->reg = HBC_PIPELINES_BANK_REMAPPINGr;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->reg = HBC_PIPELINES_PHY_RCMDr;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->reg = HBC_HBM_CPU_CONTROLr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_hbm_controller_symmetric_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_hbm_controller_symmetric_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int table_index = dnx_data_dram_hbm_table_controller_symmetric_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 10;
    table->info_get.key_size[0] = 10;

    
    table->values[0].default_val = "INVALIDr";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_hbm_controller_symmetric_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_hbm_table_controller_symmetric_regs");

    
    default_data = (dnx_data_dram_hbm_controller_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_hbm_controller_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = HBMC_HBM_PLL_CONFIGr;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = HBMC_HBM_RESET_CONTROLr;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->reg = HBMC_HBM_PHY_CONTROLr;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->reg = TDU_WRITE_REQUEST_PATH_CONFIGr;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->reg = TDU_READ_REQUEST_PATH_CONFIGr;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->reg = TDU_READ_DATA_PATH_CONFIGr;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->reg = TDU_DRAM_BLOCKED_CONFIGr;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->reg = HRC_WRITE_REQUEST_PATH_CONFIGr;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->reg = HRC_READ_REQUEST_PATH_CONFIGr;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->reg = HRC_READ_DATA_PATH_CONFIGr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_hbm_controller_not_symmetric_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_hbm_controller_not_symmetric_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int table_index = dnx_data_dram_hbm_table_controller_not_symmetric_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "INVALIDr";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_hbm_controller_not_symmetric_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_hbm_table_controller_not_symmetric_regs");

    
    default_data = (dnx_data_dram_hbm_controller_not_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_hbm_controller_not_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_not_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = TDU_ADDRESS_TRANSLATION_CONFIGr;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_not_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = TDU_ADDRESS_TRANSLATION_MATRIXr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_hbm_bist_set(
    int unit)
{
    dnx_data_dram_hbm_bist_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int table_index = dnx_data_dram_hbm_table_bist;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "0x12345678, 0x23456789, 0x3456789a, 0x456789ab, 0x56789abc, 0x6789abcd, 0x789abcde, 0x89abcdef, 0, 0, 0, 0, 0, 0, 0, 0";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_hbm_bist_t, (1 + 1  ), "data of dnx_data_dram_hbm_table_bist");

    
    default_data = (dnx_data_dram_hbm_bist_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    SHR_RANGE_VERIFY(16, 0, HBMC_SHMOO_BIST_NOF_PRBS_DATA_SEEDS, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->prbs_seeds[0] = 0x12345678;
    default_data->prbs_seeds[1] = 0x23456789;
    default_data->prbs_seeds[2] = 0x3456789a;
    default_data->prbs_seeds[3] = 0x456789ab;
    default_data->prbs_seeds[4] = 0x56789abc;
    default_data->prbs_seeds[5] = 0x6789abcd;
    default_data->prbs_seeds[6] = 0x789abcde;
    default_data->prbs_seeds[7] = 0x89abcdef;
    default_data->prbs_seeds[8] = 0;
    default_data->prbs_seeds[9] = 0;
    default_data->prbs_seeds[10] = 0;
    default_data->prbs_seeds[11] = 0;
    default_data->prbs_seeds[12] = 0;
    default_data->prbs_seeds[13] = 0;
    default_data->prbs_seeds[14] = 0;
    default_data->prbs_seeds[15] = 0;
    
    data = (dnx_data_dram_hbm_bist_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_hbm_hbc_last_in_chain_set(
    int unit)
{
    int dram_bitmap_index;
    int dram_index_index;
    dnx_data_dram_hbm_hbc_last_in_chain_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int table_index = dnx_data_dram_hbm_table_hbc_last_in_chain;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = utilex_power_of_2(dnx_data_dram.general_info.max_nof_drams_get(unit));
    table->info_get.key_size[0] = utilex_power_of_2(dnx_data_dram.general_info.max_nof_drams_get(unit));
    table->keys[1].size = dnx_data_dram.general_info.max_nof_drams_get(unit);
    table->info_get.key_size[1] = dnx_data_dram.general_info.max_nof_drams_get(unit);

    
    table->values[0].default_val = "7";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_hbm_hbc_last_in_chain_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_dram_hbm_table_hbc_last_in_chain");

    
    default_data = (dnx_data_dram_hbm_hbc_last_in_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->is_last_in_chain = 7;
    
    for (dram_bitmap_index = 0; dram_bitmap_index < table->keys[0].size; dram_bitmap_index++)
    {
        for (dram_index_index = 0; dram_index_index < table->keys[1].size; dram_index_index++)
        {
            data = (dnx_data_dram_hbm_hbc_last_in_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, dram_bitmap_index, dram_index_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0x2 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_dram_hbm_hbc_last_in_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x2, 1);
        data->is_last_in_chain = 15;
    }
    if (0x3 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_dram_hbm_hbc_last_in_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x3, 1);
        data->is_last_in_chain = 15;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2p_a0_dnx_data_dram_general_info_is_temperature_reading_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int feature_index = dnx_data_dram_general_info_is_temperature_reading_supported;
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
j2p_a0_dnx_data_dram_general_info_frequency_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_frequency;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1200;

    
    define->data = 1200;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_RAM_FREQ;
    define->property.doc = 
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "dram Frequency in MHZ\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 500;
    define->property.range_max = 1400;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_general_info_read_latency_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_read_latency;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20;

    
    define->data = 20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_general_info_read_data_enable_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_read_data_enable_length;
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
j2p_a0_dnx_data_dram_general_info_tune_mode_on_init_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_tune_mode_on_init;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DNX_HBMC_RUN_TUNE;

    
    define->data = DNX_HBMC_RUN_TUNE;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_DRAM_PHY_TUNE_MODE_ON_INIT;
    define->property.doc = 
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "Choose dram phy tune mode during Init\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 1;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "RUN_TUNE";
    define->property.mapping[0].val = DNX_HBMC_RUN_TUNE;
    define->property.mapping[0].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2p_a0_dnx_data_dram_general_info_mr_defaults_set(
    int unit)
{
    int index_index;
    dnx_data_dram_general_info_mr_defaults_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int table_index = dnx_data_dram_general_info_table_mr_defaults;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_dram.general_info.nof_mrs_get(unit);
    table->info_get.key_size[0] = dnx_data_dram.general_info.nof_mrs_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_general_info_mr_defaults_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_general_info_table_mr_defaults");

    
    default_data = (dnx_data_dram_general_info_mr_defaults_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->value = 0;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_general_info_mr_defaults_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_mr_defaults_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->value = 0x20;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_mr_defaults_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->value = 0x14;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_mr_defaults_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->value = 0x92;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_mr_defaults_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->value = 0xe1;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_mr_defaults_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->value = 0x2;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_mr_defaults_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->value = 0x70;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_mr_defaults_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->value = 0x1;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_mr_defaults_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->value = 0x1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_general_info_dram_info_set(
    int unit)
{
    dnx_data_dram_general_info_dram_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int table_index = dnx_data_dram_general_info_table_dram_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "0x3";
    table->values[2].default_val = "32";
    table->values[3].default_val = "16384";
    table->values[4].default_val = "32";
    table->values[5].default_val = "-1";
    table->values[6].default_val = "-1";
    table->values[7].default_val = "-1";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_general_info_dram_info_t, (1 + 1  ), "data of dnx_data_dram_general_info_table_dram_info");

    
    default_data = (dnx_data_dram_general_info_dram_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dram_type = -1;
    default_data->dram_bitmap = 0x3;
    default_data->nof_columns = 32;
    default_data->nof_rows = 16384;
    default_data->nof_banks = 32;
    default_data->data_rate = -1;
    default_data->ref_clock = -1;
    default_data->ctl_type = -1;
    
    data = (dnx_data_dram_general_info_dram_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[1].property.name = "ext_ram_enabled_bitmap";
    table->values[1].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "A bitmap that indicates which HBMs are used.\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_custom;
    table->values[1].property.method_str = "custom";
    
    table->values[2].property.name = spn_EXT_RAM_COLUMNS;
    table->values[2].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "Number of columns in the dram\n"
        "\n"
    ;
    table->values[2].property.method = dnxc_data_property_method_direct_map;
    table->values[2].property.method_str = "direct_map";
    table->values[2].property.nof_mapping = 1;
    DNXC_DATA_ALLOC(table->values[2].property.mapping, dnxc_data_property_map_t, table->values[2].property.nof_mapping, "dnx_data_dram_general_info_dram_info_t property mapping");

    table->values[2].property.mapping[0].name = "32";
    table->values[2].property.mapping[0].val = 32;
    table->values[2].property.mapping[0].is_default = 1 ;
    
    table->values[3].property.name = spn_EXT_RAM_ROWS;
    table->values[3].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "Number of rows in the dram\n"
        "\n"
    ;
    table->values[3].property.method = dnxc_data_property_method_direct_map;
    table->values[3].property.method_str = "direct_map";
    table->values[3].property.nof_mapping = 1;
    DNXC_DATA_ALLOC(table->values[3].property.mapping, dnxc_data_property_map_t, table->values[3].property.nof_mapping, "dnx_data_dram_general_info_dram_info_t property mapping");

    table->values[3].property.mapping[0].name = "16384";
    table->values[3].property.mapping[0].val = 16384;
    table->values[3].property.mapping[0].is_default = 1 ;
    
    table->values[4].property.name = spn_EXT_RAM_BANKS;
    table->values[4].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "Number of banks in the dram\n"
        "\n"
    ;
    table->values[4].property.method = dnxc_data_property_method_direct_map;
    table->values[4].property.method_str = "direct_map";
    table->values[4].property.nof_mapping = 1;
    DNXC_DATA_ALLOC(table->values[4].property.mapping, dnxc_data_property_map_t, table->values[4].property.nof_mapping, "dnx_data_dram_general_info_dram_info_t property mapping");

    table->values[4].property.mapping[0].name = "32";
    table->values[4].property.mapping[0].val = 32;
    table->values[4].property.mapping[0].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_dram_general_info_dram_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnx_data_property_dram_general_info_dram_info_dram_bitmap_read(unit, &data->dram_bitmap));
    data = (dnx_data_dram_general_info_dram_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[2].property, -1, &data->nof_columns));
    data = (dnx_data_dram_general_info_dram_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[3].property, -1, &data->nof_rows));
    data = (dnx_data_dram_general_info_dram_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[4].property, -1, &data->nof_banks));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);
    table->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_general_info_timing_params_set(
    int unit)
{
    dnx_data_dram_general_info_timing_params_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int table_index = dnx_data_dram_general_info_table_timing_params;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "UTILEX_DIV_ROUND_UP(17 * DATA(dram, general_info, frequency), 1000)";
    table->values[1].default_val = "UTILEX_DIV_ROUND_UP(14 * DATA(dram, general_info, frequency), 1000)";
    table->values[2].default_val = "5";
    table->values[3].default_val = "5";
    table->values[4].default_val = "UTILEX_DIV_ROUND_UP(4 * DATA(dram, general_info, frequency), 1000)";
    table->values[5].default_val = "UTILEX_DIV_ROUND_UP(4 * DATA(dram, general_info, frequency), 1000)";
    table->values[6].default_val = "UTILEX_DIV_ROUND_UP(16 * DATA(dram, general_info, frequency), 1000)";
    table->values[7].default_val = "UTILEX_DIV_ROUND_UP(10 * DATA(dram, general_info, frequency), 1000)";
    table->values[8].default_val = "UTILEX_DIV_ROUND_UP(14 * DATA(dram, general_info, frequency), 1000)";
    table->values[9].default_val = "33";
    table->values[10].default_val = "UTILEX_DIV_ROUND_UP(47 * DATA(dram, general_info, frequency), 1000)";
    table->values[11].default_val = "UTILEX_DIV_ROUND_UP(9 * DATA(dram, general_info, frequency), 1000)";
    table->values[12].default_val = "UTILEX_DIV_ROUND_UP(4 * DATA(dram, general_info, frequency), 1000)";
    table->values[13].default_val = "4";
    table->values[14].default_val = "3";
    table->values[15].default_val = "2";
    table->values[16].default_val = "4";
    table->values[17].default_val = "UTILEX_DIV_ROUND_UP(8 * DATA(dram, general_info, frequency), 1000)";
    table->values[18].default_val = "UTILEX_DIV_ROUND_UP(160 * DATA(dram, general_info, frequency), 1000)";
    table->values[19].default_val = "UTILEX_DIV_ROUND_UP(350 * DATA(dram, general_info, frequency), 1000)";
    table->values[20].default_val = "8";
    table->values[21].default_val = "15";
    table->values[22].default_val = "-1";
    table->values[23].default_val = "-1";
    table->values[24].default_val = "-1";
    table->values[25].default_val = "-1";
    table->values[26].default_val = "-1";
    table->values[27].default_val = "-1";
    table->values[28].default_val = "-1";
    table->values[29].default_val = "-1";
    table->values[30].default_val = "-1";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_general_info_timing_params_t, (1 + 1  ), "data of dnx_data_dram_general_info_table_timing_params");

    
    default_data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->twr = UTILEX_DIV_ROUND_UP(17 * dnx_data_dram.general_info.frequency_get(unit), 1000);
    default_data->trp = UTILEX_DIV_ROUND_UP(14 * dnx_data_dram.general_info.frequency_get(unit), 1000);
    default_data->trtps = 5;
    default_data->trtpl = 5;
    default_data->trrds = UTILEX_DIV_ROUND_UP(4 * dnx_data_dram.general_info.frequency_get(unit), 1000);
    default_data->trrdl = UTILEX_DIV_ROUND_UP(4 * dnx_data_dram.general_info.frequency_get(unit), 1000);
    default_data->tfaw = UTILEX_DIV_ROUND_UP(16 * dnx_data_dram.general_info.frequency_get(unit), 1000);
    default_data->trcdwr = UTILEX_DIV_ROUND_UP(10 * dnx_data_dram.general_info.frequency_get(unit), 1000);
    default_data->trcdrd = UTILEX_DIV_ROUND_UP(14 * dnx_data_dram.general_info.frequency_get(unit), 1000);
    default_data->tras = 33;
    default_data->trc = UTILEX_DIV_ROUND_UP(47 * dnx_data_dram.general_info.frequency_get(unit), 1000);
    default_data->twtrl = UTILEX_DIV_ROUND_UP(9 * dnx_data_dram.general_info.frequency_get(unit), 1000);
    default_data->twtrs = UTILEX_DIV_ROUND_UP(4 * dnx_data_dram.general_info.frequency_get(unit), 1000);
    default_data->trtw = 4;
    default_data->tccdr = 3;
    default_data->tccds = 2;
    default_data->tccdl = 4;
    default_data->trrefd = UTILEX_DIV_ROUND_UP(8 * dnx_data_dram.general_info.frequency_get(unit), 1000);
    default_data->trfcsb = UTILEX_DIV_ROUND_UP(160 * dnx_data_dram.general_info.frequency_get(unit), 1000);
    default_data->trfc = UTILEX_DIV_ROUND_UP(350 * dnx_data_dram.general_info.frequency_get(unit), 1000);
    default_data->tmrd = 8;
    default_data->tmod = 15;
    default_data->twtrtr = -1;
    default_data->twrwtr = -1;
    default_data->treftr = -1;
    default_data->trdtlt = -1;
    default_data->trcdwtr = -1;
    default_data->trcdrtr = -1;
    default_data->trcdltr = -1;
    default_data->tltrtr = -1;
    default_data->tltltr = -1;
    
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[0].property.name = spn_EXT_RAM_T_WR;
    table->values[0].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_range;
    table->values[0].property.method_str = "range";
    table->values[0].property.range_min = 0;
    table->values[0].property.range_max = 63;
    
    table->values[1].property.name = spn_EXT_RAM_T_RP;
    table->values[1].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_range;
    table->values[1].property.method_str = "range";
    table->values[1].property.range_min = 0;
    table->values[1].property.range_max = 63;
    
    table->values[2].property.name = spn_EXT_RAM_T_RTP_S;
    table->values[2].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[2].property.method = dnxc_data_property_method_range;
    table->values[2].property.method_str = "range";
    table->values[2].property.range_min = 0;
    table->values[2].property.range_max = 63;
    
    table->values[3].property.name = spn_EXT_RAM_T_RTP_L;
    table->values[3].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[3].property.method = dnxc_data_property_method_range;
    table->values[3].property.method_str = "range";
    table->values[3].property.range_min = 0;
    table->values[3].property.range_max = 63;
    
    table->values[4].property.name = spn_EXT_RAM_T_RRD_S;
    table->values[4].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[4].property.method = dnxc_data_property_method_range;
    table->values[4].property.method_str = "range";
    table->values[4].property.range_min = 0;
    table->values[4].property.range_max = 63;
    
    table->values[5].property.name = spn_EXT_RAM_T_RRD_L;
    table->values[5].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[5].property.method = dnxc_data_property_method_range;
    table->values[5].property.method_str = "range";
    table->values[5].property.range_min = 0;
    table->values[5].property.range_max = 63;
    
    table->values[6].property.name = spn_EXT_RAM_T_FAW;
    table->values[6].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[6].property.method = dnxc_data_property_method_range;
    table->values[6].property.method_str = "range";
    table->values[6].property.range_min = 0;
    table->values[6].property.range_max = 63;
    
    table->values[7].property.name = spn_EXT_RAM_T_RCD_WR;
    table->values[7].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[7].property.method = dnxc_data_property_method_range;
    table->values[7].property.method_str = "range";
    table->values[7].property.range_min = 0;
    table->values[7].property.range_max = 63;
    
    table->values[8].property.name = spn_EXT_RAM_T_RCD_RD;
    table->values[8].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[8].property.method = dnxc_data_property_method_range;
    table->values[8].property.method_str = "range";
    table->values[8].property.range_min = 0;
    table->values[8].property.range_max = 63;
    
    table->values[9].property.name = spn_EXT_RAM_T_RAS;
    table->values[9].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[9].property.method = dnxc_data_property_method_range;
    table->values[9].property.method_str = "range";
    table->values[9].property.range_min = 0;
    table->values[9].property.range_max = 63;
    
    table->values[10].property.name = spn_EXT_RAM_T_RC;
    table->values[10].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[10].property.method = dnxc_data_property_method_range;
    table->values[10].property.method_str = "range";
    table->values[10].property.range_min = 0;
    table->values[10].property.range_max = 63;
    
    table->values[11].property.name = spn_EXT_RAM_T_WTR_L;
    table->values[11].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[11].property.method = dnxc_data_property_method_range;
    table->values[11].property.method_str = "range";
    table->values[11].property.range_min = 0;
    table->values[11].property.range_max = 63;
    
    table->values[12].property.name = spn_EXT_RAM_T_WTR_S;
    table->values[12].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[12].property.method = dnxc_data_property_method_range;
    table->values[12].property.method_str = "range";
    table->values[12].property.range_min = 0;
    table->values[12].property.range_max = 63;
    
    table->values[13].property.name = spn_EXT_RAM_T_RTW;
    table->values[13].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[13].property.method = dnxc_data_property_method_range;
    table->values[13].property.method_str = "range";
    table->values[13].property.range_min = 0;
    table->values[13].property.range_max = 63;
    
    table->values[14].property.name = spn_EXT_RAM_T_CCD_R;
    table->values[14].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[14].property.method = dnxc_data_property_method_range;
    table->values[14].property.method_str = "range";
    table->values[14].property.range_min = 0;
    table->values[14].property.range_max = 63;
    
    table->values[15].property.name = spn_EXT_RAM_T_CCD_S;
    table->values[15].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[15].property.method = dnxc_data_property_method_range;
    table->values[15].property.method_str = "range";
    table->values[15].property.range_min = 0;
    table->values[15].property.range_max = 63;
    
    table->values[16].property.name = spn_EXT_RAM_T_CCD_L;
    table->values[16].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[16].property.method = dnxc_data_property_method_range;
    table->values[16].property.method_str = "range";
    table->values[16].property.range_min = 0;
    table->values[16].property.range_max = 63;
    
    table->values[17].property.name = "ext_ram_t_rrefd";
    table->values[17].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[17].property.method = dnxc_data_property_method_range;
    table->values[17].property.method_str = "range";
    table->values[17].property.range_min = 0;
    table->values[17].property.range_max = 255;
    
    table->values[18].property.name = "ext_ram_t_rfcsb";
    table->values[18].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[18].property.method = dnxc_data_property_method_range;
    table->values[18].property.method_str = "range";
    table->values[18].property.range_min = 0;
    table->values[18].property.range_max = 1023;
    
    table->values[19].property.name = "ext_ram_t_rfc";
    table->values[19].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[19].property.method = dnxc_data_property_method_range;
    table->values[19].property.method_str = "range";
    table->values[19].property.range_min = 0;
    table->values[19].property.range_max = 1023;
    
    table->values[20].property.name = "ext_ram_t_mrd";
    table->values[20].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[20].property.method = dnxc_data_property_method_range;
    table->values[20].property.method_str = "range";
    table->values[20].property.range_min = 0;
    table->values[20].property.range_max = 63;
    
    table->values[21].property.name = "ext_ram_t_mod";
    table->values[21].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[21].property.method = dnxc_data_property_method_range;
    table->values[21].property.method_str = "range";
    table->values[21].property.range_min = 0;
    table->values[21].property.range_max = 63;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, -1, &data->twr));
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, -1, &data->trp));
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[2].property, -1, &data->trtps));
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[3].property, -1, &data->trtpl));
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[4].property, -1, &data->trrds));
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[5].property, -1, &data->trrdl));
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[6].property, -1, &data->tfaw));
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[7].property, -1, &data->trcdwr));
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[8].property, -1, &data->trcdrd));
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[9].property, -1, &data->tras));
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[10].property, -1, &data->trc));
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[11].property, -1, &data->twtrl));
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[12].property, -1, &data->twtrs));
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[13].property, -1, &data->trtw));
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[14].property, -1, &data->tccdr));
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[15].property, -1, &data->tccds));
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[16].property, -1, &data->tccdl));
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[17].property, -1, &data->trrefd));
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[18].property, -1, &data->trfcsb));
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[19].property, -1, &data->trfc));
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[20].property, -1, &data->tmrd));
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[21].property, -1, &data->tmod));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_general_info_refresh_intervals_set(
    int unit)
{
    int temp_index_index;
    dnx_data_dram_general_info_refresh_intervals_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int table_index = dnx_data_dram_general_info_table_refresh_intervals;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 8;
    table->info_get.key_size[0] = 8;

    
    table->values[0].default_val = "3900";
    table->values[1].default_val = "120";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_general_info_refresh_intervals_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_general_info_table_refresh_intervals");

    
    default_data = (dnx_data_dram_general_info_refresh_intervals_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->trefi = 3900;
    default_data->trefisb = 120;
    
    for (temp_index_index = 0; temp_index_index < table->keys[0].size; temp_index_index++)
    {
        data = (dnx_data_dram_general_info_refresh_intervals_t *) dnxc_data_mgmt_table_data_get(unit, table, temp_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_refresh_intervals_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->trefi = 3900 * 2.1;
        data->trefisb = 120 * 4;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_refresh_intervals_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->trefi = 3900 * 2.2;
        data->trefisb = 120 * 2;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_refresh_intervals_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->trefi = 3900 * 0.5;
        data->trefisb = 120 * 0.5;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_refresh_intervals_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->trefi = 3900 * 1;
        data->trefisb = 120 * 1;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_refresh_intervals_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->trefi = 3900 * 2.3;
        data->trefisb = 120 * 10;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_refresh_intervals_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->trefi = 3900 * 2.4;
        data->trefisb = 120 * 10;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_refresh_intervals_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->trefi = 3900 * 0.25;
        data->trefisb = 120 * 0.25;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_refresh_intervals_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->trefi = 3900 * 2.5;
        data->trefisb = 120 * 10;
    }
    
    table->values[0].property.name = "ext_ram_t_refi";
    table->values[0].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_suffix_range;
    table->values[0].property.method_str = "suffix_range";
    table->values[0].property.suffix = "";
    table->values[0].property.range_min = 0;
    table->values[0].property.range_max = 16383;
    
    table->values[1].property.name = "ext_ram_t_refisb";
    table->values[1].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "For more info regarding this timing parameter check JEDEC\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_suffix_range;
    table->values[1].property.method_str = "suffix_range";
    table->values[1].property.suffix = "";
    table->values[1].property.range_min = 0;
    table->values[1].property.range_max = 2047;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (temp_index_index = 0; temp_index_index < table->keys[0].size; temp_index_index++)
    {
        data = (dnx_data_dram_general_info_refresh_intervals_t *) dnxc_data_mgmt_table_data_get(unit, table, temp_index_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, temp_index_index, &data->trefi));
    }
    for (temp_index_index = 0; temp_index_index < table->keys[0].size; temp_index_index++)
    {
        data = (dnx_data_dram_general_info_refresh_intervals_t *) dnxc_data_mgmt_table_data_get(unit, table, temp_index_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, temp_index_index, &data->trefisb));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2p_a0_dnx_data_dram_address_translation_nof_tdus_per_dram_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_address_translation;
    int define_index = dnx_data_dram_address_translation_define_nof_tdus_per_dram;
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
j2p_a0_dnx_data_dram_address_translation_tdu_map_set(
    int unit)
{
    int dram_index_index;
    int tdu_index_index;
    dnx_data_dram_address_translation_tdu_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_address_translation;
    int table_index = dnx_data_dram_address_translation_table_tdu_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_dram.general_info.max_nof_drams_get(unit);
    table->info_get.key_size[0] = dnx_data_dram.general_info.max_nof_drams_get(unit);
    table->keys[1].size = dnx_data_dram.address_translation.nof_tdus_per_dram_get(unit);
    table->info_get.key_size[1] = dnx_data_dram.address_translation.nof_tdus_per_dram_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_address_translation_tdu_map_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_dram_address_translation_table_tdu_map");

    
    default_data = (dnx_data_dram_address_translation_tdu_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->data_source_id = 0;
    
    for (dram_index_index = 0; dram_index_index < table->keys[0].size; dram_index_index++)
    {
        for (tdu_index_index = 0; tdu_index_index < table->keys[1].size; tdu_index_index++)
        {
            data = (dnx_data_dram_address_translation_tdu_map_t *) dnxc_data_mgmt_table_data_get(unit, table, dram_index_index, tdu_index_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_dram_address_translation_tdu_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->data_source_id = 0;
    }
    if (0 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_dram_address_translation_tdu_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 1);
        data->data_source_id = 1;
    }
    if (1 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_dram_address_translation_tdu_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->data_source_id = 1;
    }
    if (1 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_dram_address_translation_tdu_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 1);
        data->data_source_id = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_address_translation_matrix_configuration_set(
    int unit)
{
    int matrix_option_index;
    dnx_data_dram_address_translation_matrix_configuration_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_address_translation;
    int table_index = dnx_data_dram_address_translation_table_matrix_configuration;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_HBMC_NOF_ATMS;
    table->info_get.key_size[0] = DNX_HBMC_NOF_ATMS;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_address_translation_matrix_configuration_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_address_translation_table_matrix_configuration");

    
    default_data = (dnx_data_dram_address_translation_matrix_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->logical_to_physical[0] = 0;
    
    for (matrix_option_index = 0; matrix_option_index < table->keys[0].size; matrix_option_index++)
    {
        data = (dnx_data_dram_address_translation_matrix_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, matrix_option_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_HBMC_SINGLE_HBM_ATM < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_matrix_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_HBMC_SINGLE_HBM_ATM, 0);
        SHR_RANGE_VERIFY(30, 0, DNX_DATA_MAX_DRAM_ADDRESS_TRANSLATION_MATRIX_COLUMN_SIZE, _SHR_E_INTERNAL, "out of bound access to array")
        data->logical_to_physical[0] = 0x1000000;
        data->logical_to_physical[1] = 0x0400002;
        data->logical_to_physical[2] = 0x0800004;
        data->logical_to_physical[3] = 0x0004008;
        data->logical_to_physical[4] = 0x0010004;
        data->logical_to_physical[5] = 0x0008008;
        data->logical_to_physical[6] = 0x0000012;
        data->logical_to_physical[7] = 0x0020001;
        data->logical_to_physical[8] = 0x0000040;
        data->logical_to_physical[9] = 0x0000080;
        data->logical_to_physical[10] = 0x0000100;
        data->logical_to_physical[11] = 0x0000200;
        data->logical_to_physical[12] = 0x0000400;
        data->logical_to_physical[13] = 0x0000800;
        data->logical_to_physical[14] = 0x0001000;
        data->logical_to_physical[15] = 0x0002000;
        data->logical_to_physical[16] = 0x0000001;
        data->logical_to_physical[17] = 0x0000002;
        data->logical_to_physical[18] = 0x0000004;
        data->logical_to_physical[19] = 0x0000008;
        data->logical_to_physical[20] = 0x0040000;
        data->logical_to_physical[21] = 0x0080000;
        data->logical_to_physical[22] = 0x0100000;
        data->logical_to_physical[23] = 0x0000000;
        data->logical_to_physical[24] = 0x0800020;
        data->logical_to_physical[25] = 0x1000000;
        data->logical_to_physical[26] = 0x0000040;
        data->logical_to_physical[27] = 0x0000000;
        data->logical_to_physical[28] = 0x0000000;
        data->logical_to_physical[29] = 0x0000000;
    }
    if (DNX_HBMC_2_HBMS_ATM < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_matrix_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_HBMC_2_HBMS_ATM, 0);
        SHR_RANGE_VERIFY(30, 0, DNX_DATA_MAX_DRAM_ADDRESS_TRANSLATION_MATRIX_COLUMN_SIZE, _SHR_E_INTERNAL, "out of bound access to array")
        data->logical_to_physical[0] = 0x0000021;
        data->logical_to_physical[1] = 0x0400002;
        data->logical_to_physical[2] = 0x0800004;
        data->logical_to_physical[3] = 0x0004008;
        data->logical_to_physical[4] = 0x0010004;
        data->logical_to_physical[5] = 0x0008008;
        data->logical_to_physical[6] = 0x0000012;
        data->logical_to_physical[7] = 0x0020001;
        data->logical_to_physical[8] = 0x0000040;
        data->logical_to_physical[9] = 0x0000080;
        data->logical_to_physical[10] = 0x0000100;
        data->logical_to_physical[11] = 0x0000200;
        data->logical_to_physical[12] = 0x0000400;
        data->logical_to_physical[13] = 0x0000800;
        data->logical_to_physical[14] = 0x0001000;
        data->logical_to_physical[15] = 0x0002000;
        data->logical_to_physical[16] = 0x0000001;
        data->logical_to_physical[17] = 0x0000002;
        data->logical_to_physical[18] = 0x0000004;
        data->logical_to_physical[19] = 0x0000008;
        data->logical_to_physical[20] = 0x0040000;
        data->logical_to_physical[21] = 0x0080000;
        data->logical_to_physical[22] = 0x0100000;
        data->logical_to_physical[23] = 0x0000000;
        data->logical_to_physical[24] = 0x0200000;
        data->logical_to_physical[25] = 0x1000000;
        data->logical_to_physical[26] = 0x0000040;
        data->logical_to_physical[27] = 0x0000000;
        data->logical_to_physical[28] = 0x0000000;
        data->logical_to_physical[29] = 0x0000000;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_address_translation_interrupt_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_address_translation_interrupt_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_address_translation;
    int table_index = dnx_data_dram_address_translation_table_interrupt_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;

    
    table->values[0].default_val = "INVALIDr";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_address_translation_interrupt_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_address_translation_table_interrupt_regs");

    
    default_data = (dnx_data_dram_address_translation_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_address_translation_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = TDU_INTERRUPT_REGISTERr;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = TDU_ECC_INTERRUPT_REGISTERr;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->reg = HRC_INTERRUPT_REGISTERr;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->reg = HRC_ECC_INTERRUPT_REGISTERr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_address_translation_counter_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_address_translation_counter_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_address_translation;
    int table_index = dnx_data_dram_address_translation_table_counter_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 8;
    table->info_get.key_size[0] = 8;

    
    table->values[0].default_val = "INVALIDr";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_address_translation_counter_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_address_translation_table_counter_regs");

    
    default_data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = TDU_WRITE_REQUEST_PATH_COUNTERSr;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = TDU_WRITE_REQUEST_PER_CHANNEL_COUNTr;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->reg = TDU_READ_REQUEST_PATH_COUNTERSr;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->reg = TDU_READ_REQUEST_PER_CHANNEL_COUNTr;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->reg = TDU_READ_DATA_PATH_COUNTERSr;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->reg = TDU_DRAM_BLOCKED_STATUSr;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->reg = HRC_REQUEST_PATH_COUNTERSr;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->reg = HRC_READ_DATA_PATH_COUNTERSr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2p_a0_dnx_data_dram_buffers_quarantine_buffers_if_tdu_corrected_errors_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_buffers;
    int feature_index = dnx_data_dram_buffers_quarantine_buffers_if_tdu_corrected_errors;
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
j2p_a0_dnx_data_dram_buffers_quarantine_buffers_if_tdu_errors_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_buffers;
    int feature_index = dnx_data_dram_buffers_quarantine_buffers_if_tdu_errors;
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
j2p_a0_dnx_data_dram_buffers_bdb_release_mechanism_usage_counters_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_buffers;
    int feature_index = dnx_data_dram_buffers_bdb_release_mechanism_usage_counters;
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
j2p_a0_dnx_data_dram_dram_block_write_minus_read_leaky_bucket_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int feature_index = dnx_data_dram_dram_block_write_minus_read_leaky_bucket;
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
j2p_a0_dnx_data_dram_dram_block_write_leaky_bucket_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int feature_index = dnx_data_dram_dram_block_write_leaky_bucket;
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
j2p_a0_dnx_data_dram_dram_block_average_read_inflights_leaky_bucket_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int feature_index = dnx_data_dram_dram_block_average_read_inflights_leaky_bucket;
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
j2p_a0_dnx_data_dram_dram_block_average_read_inflights_log2_window_size_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int feature_index = dnx_data_dram_dram_block_average_read_inflights_log2_window_size;
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
j2p_a0_dnx_data_dram_dram_block_leaky_bucket_window_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int define_index = dnx_data_dram_dram_block_define_leaky_bucket_window_size;
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
j2p_a0_dnx_data_dram_dram_block_average_read_inflights_assert_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int define_index = dnx_data_dram_dram_block_define_average_read_inflights_assert_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20;

    
    define->data = 20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_dram_block_average_read_inflights_full_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int define_index = dnx_data_dram_dram_block_define_average_read_inflights_full_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 40;

    
    define->data = 40;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_dram_block_wpr_increment_thr_factor_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int define_index = dnx_data_dram_dram_block_define_wpr_increment_thr_factor;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_DIV_ROUND_UP(8 * dnx_data_dram.dram_block.leaky_bucket_window_size_get(unit) * 1000, (dnx_data_device.general.core_clock_khz_get(unit) / dnx_data_dram.general_info.frequency_get(unit)));

    
    define->data = UTILEX_DIV_ROUND_UP(8 * dnx_data_dram.dram_block.leaky_bucket_window_size_get(unit) * 1000, (dnx_data_device.general.core_clock_khz_get(unit) / dnx_data_dram.general_info.frequency_get(unit)));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2p_a0_dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_set(
    int unit)
{
    int index_index;
    dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int table_index = dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 3;
    table->info_get.key_size[0] = 3;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th");

    
    default_data = (dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->threshold = 0;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->threshold = 152 * dnx_data_dram.dram_block.wpr_increment_thr_factor_get(unit) / 100;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->threshold = 154 * dnx_data_dram.dram_block.wpr_increment_thr_factor_get(unit) / 100;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->threshold = 156 * dnx_data_dram.dram_block.wpr_increment_thr_factor_get(unit) / 100;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_dram_block_write_leaky_bucket_increment_th_set(
    int unit)
{
    int index_index;
    dnx_data_dram_dram_block_write_leaky_bucket_increment_th_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int table_index = dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 3;
    table->info_get.key_size[0] = 3;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_dram_block_write_leaky_bucket_increment_th_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th");

    
    default_data = (dnx_data_dram_dram_block_write_leaky_bucket_increment_th_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->threshold = 0;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_dram_block_write_leaky_bucket_increment_th_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_dram_block_write_leaky_bucket_increment_th_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->threshold = 102 * dnx_data_dram.dram_block.wpr_increment_thr_factor_get(unit) / 100;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_dram_block_write_leaky_bucket_increment_th_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->threshold = 104 * dnx_data_dram.dram_block.wpr_increment_thr_factor_get(unit) / 100;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dram_dram_block_write_leaky_bucket_increment_th_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->threshold = 106 * dnx_data_dram.dram_block.wpr_increment_thr_factor_get(unit) / 100;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2p_a0_dnx_data_dram_dbal_hbm_trc_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_hbm_trc_nof_bits;
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
j2p_a0_dnx_data_dram_dbal_wpr_increment_threshold_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_wpr_increment_threshold_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 26;

    
    define->data = 26;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_dbal_average_read_inflights_increment_threshold_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_average_read_inflights_increment_threshold_nof_bits;
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
j2p_a0_dnx_data_dram_dbal_average_read_inflights_decrement_threshold_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_average_read_inflights_decrement_threshold_nof_bits;
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
j2p_a0_dnx_data_dram_dbal_hbm_pll_pdiv_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_hbm_pll_pdiv_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_dbal_hbm_pll_ch_mdiv_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_hbm_pll_ch_mdiv_nof_bits;
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
j2p_a0_dnx_data_dram_dbal_hbm_pll_frefeff_info_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_hbm_pll_frefeff_info_nof_bits;
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
j2p_a0_dnx_data_dram_dbal_hbm_pll_aux_post_enableb_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_hbm_pll_aux_post_enableb_nof_bits;
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
j2p_a0_dnx_data_dram_dbal_hbm_pll_ch_enableb_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_hbm_pll_ch_enableb_nof_bits;
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
j2p_a0_dnx_data_dram_dbal_hbm_pll_aux_post_diffcmos_en_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_hbm_pll_aux_post_diffcmos_en_nof_bits;
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
j2p_a0_dnx_data_dram_dbal_dram_bist_mode_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_dram_bist_mode_nof_bits;
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
j2p_a0_dnx_data_dram_firmware_nof_hbm_spare_data_results_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_firmware;
    int define_index = dnx_data_dram_firmware_define_nof_hbm_spare_data_results;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_firmware_sbus_clock_divider_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_firmware;
    int define_index = dnx_data_dram_firmware_define_sbus_clock_divider;
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
j2p_a0_dnx_data_dram_firmware_snap_state_init_done_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_firmware;
    int define_index = dnx_data_dram_firmware_define_snap_state_init_done;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x12;

    
    define->data = 0x12;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2p_a0_dnx_data_dram_firmware_rom_set(
    int unit)
{
    dnx_data_dram_firmware_rom_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_firmware;
    int table_index = dnx_data_dram_firmware_table_rom;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "hbmfw/sbus_master.hbm.rom";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_firmware_rom_t, (1 + 1  ), "data of dnx_data_dram_firmware_table_rom");

    
    default_data = (dnx_data_dram_firmware_rom_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->filename = "hbmfw/sbus_master.hbm.rom";
    
    data = (dnx_data_dram_firmware_rom_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_firmware_operation_status_set(
    int unit)
{
    dnx_data_dram_firmware_operation_status_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_firmware;
    int table_index = dnx_data_dram_firmware_table_operation_status;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "0x1";
    table->values[1].default_val = "0x2";
    table->values[2].default_val = "0x4";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_firmware_operation_status_t, (1 + 1  ), "data of dnx_data_dram_firmware_table_operation_status");

    
    default_data = (dnx_data_dram_firmware_operation_status_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->done = 0x1;
    default_data->active = 0x2;
    default_data->errors_detected = 0x4;
    
    data = (dnx_data_dram_firmware_operation_status_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_firmware_operation_result_error_code_set(
    int unit)
{
    int error_code_index;
    dnx_data_dram_firmware_operation_result_error_code_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_firmware;
    int table_index = dnx_data_dram_firmware_table_operation_result_error_code;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 0x21;
    table->info_get.key_size[0] = 0x21;

    
    table->values[0].default_val = "UNDEFINED_ERROR_CODE";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_firmware_operation_result_error_code_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_firmware_table_operation_result_error_code");

    
    default_data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->description = "UNDEFINED_ERROR_CODE";
    
    for (error_code_index = 0; error_code_index < table->keys[0].size; error_code_index++)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, error_code_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0x00 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x00, 0);
        data->description = "PASS";
    }
    if (0x01 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x01, 0);
        data->description = "ERROR_DETECTED";
    }
    if (0x02 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x02, 0);
        data->description = "ERROR_UNEXPECTED_RESET_STATE";
    }
    if (0x03 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x03, 0);
        data->description = "ERROR_ILLEGAL_CHANNEL_NUMBER";
    }
    if (0x04 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x04, 0);
        data->description = "ERROR_TIMEOUT_WAITING_FOR_1500_DONE";
    }
    if (0x05 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x05, 0);
        data->description = "ERROR_TIMEOUT_WAITING_FOR_BIST_DONE";
    }
    if (0x06 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x06, 0);
        data->description = "ERROR_DATA_COMPARE_FAILED";
    }
    if (0x07 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x07, 0);
        data->description = "ERROR_ALL_CHANNEL_NOT_SELECTED_FOR_RESET";
    }
    if (0x08 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x08, 0);
        data->description = "ERROR_REPAIR_LIMIT_EXCEEDED";
    }
    if (0x09 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x09, 0);
        data->description = "ERROR_NON_REPAIRABLE_FAULTS_FOUND";
    }
    if (0x0a < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x0a, 0);
        data->description = "ERROR_MBIST_FAILED";
    }
    if (0x0b < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x0b, 0);
        data->description = "ERROR_EXCEEDED_BANK_REPAIR_LIMIT";
    }
    if (0x0c < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x0c, 0);
        data->description = "ERROR_ALL_CHANNELS_NOT_ENABLED";
    }
    if (0x0d < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x0d, 0);
        data->description = "ERROR_TIMEOUT_WAITING_FOR_PHYUPD_HANDSHAKE";
    }
    if (0x0e < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x0e, 0);
        data->description = "ERROR_CHANNEL_UNREPAIRABLE";
    }
    if (0x0f < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x0f, 0);
        data->description = "ERROR_NO_FUSES_AVAILABLE_FOR_REPAIR";
    }
    if (0x10 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x10, 0);
        data->description = "ERROR_TIMEOUT_WAITING_FOR_VALID_TEMP";
    }
    if (0x11 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x11, 0);
        data->description = "ERROR_CHANNEL_FAILURES_EXIST";
    }
    if (0x12 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x12, 0);
        data->description = "ERROR_UNKNOWN_ERROR";
    }
    if (0x13 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x13, 0);
        data->description = "ERROR_TIMEOUT_WAITING_FOR_NWL_INIT";
    }
    if (0x14 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x14, 0);
        data->description = "UNDEFINED_ERROR_CODE";
    }
    if (0x15 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x15, 0);
        data->description = "UNDEFINED_ERROR_CODE";
    }
    if (0x16 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x16, 0);
        data->description = "UNDEFINED_ERROR_CODE";
    }
    if (0x17 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x17, 0);
        data->description = "ERROR_LANE_ERRORS_DETECTED";
    }
    if (0x18 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x18, 0);
        data->description = "UNDEFINED_ERROR_CODE";
    }
    if (0x19 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x19, 0);
        data->description = "ERROR_UNSUPPORTED_HBM_CONFIGURATION";
    }
    if (0x1a < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x1a, 0);
        data->description = "ERROR_HBM_MISR_PRESET_FAILED";
    }
    if (0x1b < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x1b, 0);
        data->description = "ERROR_PHY_MISR_PRESET_FAILED";
    }
    if (0x1c < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x1c, 0);
        data->description = "ERROR_CORE_POWERON_RST_L_ASSERTED";
    }
    if (0x1d < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x1d, 0);
        data->description = "ERROR_UNSUPPORTED_INTERRUPT";
    }
    if (0x1e < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x1e, 0);
        data->description = "UNDEFINED_ERROR_CODE";
    }
    if (0x1f < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x1f, 0);
        data->description = "ERROR_MODE1_LANE_REPAIR_LIMIT_EXCEEDED";
    }
    if (0x20 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x20, 0);
        data->description = "ERROR_TOTAL_LANE_REPAIR_LIMIT_EXCEEDED";
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dram_firmware_operation_result_last_operation_set(
    int unit)
{
    int operation_code_index;
    dnx_data_dram_firmware_operation_result_last_operation_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_firmware;
    int table_index = dnx_data_dram_firmware_table_operation_result_last_operation;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 0x24;
    table->info_get.key_size[0] = 0x24;

    
    table->values[0].default_val = "UNDEFINED_OP_CODE";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_firmware_operation_result_last_operation_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_firmware_table_operation_result_last_operation");

    
    default_data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->description = "UNDEFINED_OP_CODE";
    
    for (operation_code_index = 0; operation_code_index < table->keys[0].size; operation_code_index++)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, operation_code_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0x00 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x00, 0);
        data->description = "OP_NONE";
    }
    if (0x01 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x01, 0);
        data->description = "OP_BYPASS";
    }
    if (0x02 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x02, 0);
        data->description = "OP_DEVICE_ID";
    }
    if (0x03 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x03, 0);
        data->description = "OP_AWORD";
    }
    if (0x04 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x04, 0);
        data->description = "OP_AERR";
    }
    if (0x05 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x05, 0);
        data->description = "OP_AWORD_ILB";
    }
    if (0x06 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x06, 0);
        data->description = "OP_AERR_ILB";
    }
    if (0x07 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x07, 0);
        data->description = "OP_AERR_INJ_ILB";
    }
    if (0x08 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x08, 0);
        data->description = "OP_DWORD_WRITE";
    }
    if (0x09 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x09, 0);
        data->description = "OP_DWORD_READ";
    }
    if (0x0a < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x0a, 0);
        data->description = "OP_DERR";
    }
    if (0x0b < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x0b, 0);
        data->description = "OP_DWORD_UPPER_ILB";
    }
    if (0x0c < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x0c, 0);
        data->description = "OP_DWORD_LOWER_ILB";
    }
    if (0x0d < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x0d, 0);
        data->description = "OP_DERR_ILB";
    }
    if (0x0e < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x0e, 0);
        data->description = "OP_DERR_INJ_ILB";
    }
    if (0x0f < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x0f, 0);
        data->description = "OP_LANE_REPAIR";
    }
    if (0x10 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x10, 0);
        data->description = "OP_DEVICE_TEMP";
    }
    if (0x11 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x11, 0);
        data->description = "OP_CONNECTIVITY_CHECK";
    }
    if (0x12 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x12, 0);
        data->description = "OP_RESET";
    }
    if (0x13 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x13, 0);
        data->description = "OP_MBIST";
    }
    if (0x14 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x14, 0);
        data->description = "OP_BITCELL_REPAIR";
    }
    if (0x15 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x15, 0);
        data->description = "OP_AWORD_SLB";
    }
    if (0x16 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x16, 0);
        data->description = "OP_AERR_SLB";
    }
    if (0x17 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x17, 0);
        data->description = "OP_AERR_INJ_SLB";
    }
    if (0x18 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x18, 0);
        data->description = "OP_DWORD_UPPER_SLB";
    }
    if (0x19 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x19, 0);
        data->description = "OP_DWORD_LOWER_SLB";
    }
    if (0x1a < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x1a, 0);
        data->description = "OP_DERR_SLB";
    }
    if (0x1b < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x1b, 0);
        data->description = "OP_DERR_INJ_SLB";
    }
    if (0x1c < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x1c, 0);
        data->description = "OP_TMRS";
    }
    if (0x1d < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x1d, 0);
        data->description = "OP_CHIPPING";
    }
    if (0x1e < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x1e, 0);
        data->description = "UNDEFINED_OP_CODE";
    }
    if (0x1f < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x1f, 0);
        data->description = "UNDEFINED_OP_CODE";
    }
    if (0x20 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x20, 0);
        data->description = "OP_APPLY_LANE_REPAIR";
    }
    if (0x21 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x21, 0);
        data->description = "OP_BURN_LANE_REPAIR";
    }
    if (0x22 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x22, 0);
        data->description = "OP_CATTRIP";
    }
    if (0x23 < table->keys[0].size)
    {
        data = (dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_get(unit, table, 0x23, 0);
        data->description = "OP_FUSE_SCAN";
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2p_a0_dnx_data_dram_apd_phy_pll_set(
    int unit)
{
    dnx_data_dram_apd_phy_pll_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_apd_phy;
    int table_index = dnx_data_dram_apd_phy_table_pll;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "80*DATA(dram, general_info, frequency)/1000";
    table->values[1].default_val = "80*DATA(dram, general_info, frequency)/1000";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_apd_phy_pll_t, (1 + 1  ), "data of dnx_data_dram_apd_phy_table_pll");

    
    default_data = (dnx_data_dram_apd_phy_pll_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->ndiv_int = 80*dnx_data_dram.general_info.frequency_get(unit)/1000;
    default_data->ch1_mdiv = 80*dnx_data_dram.general_info.frequency_get(unit)/1000;
    
    data = (dnx_data_dram_apd_phy_pll_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
j2p_a0_data_dram_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_dram;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_dram_submodule_hbm;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_hbm_define_stop_traffic_low_temp_threshold;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_hbm_stop_traffic_low_temp_threshold_set;
    data_index = dnx_data_dram_hbm_define_restore_traffic_low_temp_threshold;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_hbm_restore_traffic_low_temp_threshold_set;
    data_index = dnx_data_dram_hbm_define_power_down_low_temp_threshold;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_hbm_power_down_low_temp_threshold_set;
    data_index = dnx_data_dram_hbm_define_start_disabled;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_hbm_start_disabled_set;
    data_index = dnx_data_dram_hbm_define_output_enable_length;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_hbm_output_enable_length_set;
    data_index = dnx_data_dram_hbm_define_output_enable_delay;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_hbm_output_enable_delay_set;
    data_index = dnx_data_dram_hbm_define_wds_size;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_hbm_wds_size_set;

    
    data_index = dnx_data_dram_hbm_dword_alignment_check;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dram_hbm_dword_alignment_check_set;
    data_index = dnx_data_dram_hbm_low_temperature_dram_protction;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dram_hbm_low_temperature_dram_protction_set;
    data_index = dnx_data_dram_hbm_is_delete_bdb_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dram_hbm_is_delete_bdb_supported_set;
    data_index = dnx_data_dram_hbm_cpu2tap_access;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dram_hbm_cpu2tap_access_set;
    data_index = dnx_data_dram_hbm_apd_phy;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dram_hbm_apd_phy_set;
    data_index = dnx_data_dram_hbm_is_hrc_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dram_hbm_is_hrc_supported_set;
    data_index = dnx_data_dram_hbm_channel_wds_inflight_threshold;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dram_hbm_channel_wds_inflight_threshold_set;

    
    data_index = dnx_data_dram_hbm_table_channel_symmetric_regs;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dram_hbm_channel_symmetric_regs_set;
    data_index = dnx_data_dram_hbm_table_controller_symmetric_regs;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dram_hbm_controller_symmetric_regs_set;
    data_index = dnx_data_dram_hbm_table_controller_not_symmetric_regs;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dram_hbm_controller_not_symmetric_regs_set;
    data_index = dnx_data_dram_hbm_table_bist;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dram_hbm_bist_set;
    data_index = dnx_data_dram_hbm_table_hbc_last_in_chain;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dram_hbm_hbc_last_in_chain_set;
    
    submodule_index = dnx_data_dram_submodule_general_info;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_general_info_define_frequency;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_general_info_frequency_set;
    data_index = dnx_data_dram_general_info_define_read_latency;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_general_info_read_latency_set;
    data_index = dnx_data_dram_general_info_define_read_data_enable_length;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_general_info_read_data_enable_length_set;
    data_index = dnx_data_dram_general_info_define_tune_mode_on_init;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_general_info_tune_mode_on_init_set;

    
    data_index = dnx_data_dram_general_info_is_temperature_reading_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dram_general_info_is_temperature_reading_supported_set;

    
    data_index = dnx_data_dram_general_info_table_mr_defaults;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dram_general_info_mr_defaults_set;
    data_index = dnx_data_dram_general_info_table_dram_info;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dram_general_info_dram_info_set;
    data_index = dnx_data_dram_general_info_table_timing_params;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dram_general_info_timing_params_set;
    data_index = dnx_data_dram_general_info_table_refresh_intervals;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dram_general_info_refresh_intervals_set;
    
    submodule_index = dnx_data_dram_submodule_address_translation;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_address_translation_define_nof_tdus_per_dram;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_address_translation_nof_tdus_per_dram_set;

    

    
    data_index = dnx_data_dram_address_translation_table_tdu_map;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dram_address_translation_tdu_map_set;
    data_index = dnx_data_dram_address_translation_table_matrix_configuration;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dram_address_translation_matrix_configuration_set;
    data_index = dnx_data_dram_address_translation_table_interrupt_regs;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dram_address_translation_interrupt_regs_set;
    data_index = dnx_data_dram_address_translation_table_counter_regs;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dram_address_translation_counter_regs_set;
    
    submodule_index = dnx_data_dram_submodule_buffers;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_dram_buffers_quarantine_buffers_if_tdu_corrected_errors;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dram_buffers_quarantine_buffers_if_tdu_corrected_errors_set;
    data_index = dnx_data_dram_buffers_quarantine_buffers_if_tdu_errors;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dram_buffers_quarantine_buffers_if_tdu_errors_set;
    data_index = dnx_data_dram_buffers_bdb_release_mechanism_usage_counters;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dram_buffers_bdb_release_mechanism_usage_counters_set;

    
    
    submodule_index = dnx_data_dram_submodule_dram_block;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_dram_block_define_leaky_bucket_window_size;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_dram_block_leaky_bucket_window_size_set;
    data_index = dnx_data_dram_dram_block_define_average_read_inflights_assert_threshold;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_dram_block_average_read_inflights_assert_threshold_set;
    data_index = dnx_data_dram_dram_block_define_average_read_inflights_full_size;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_dram_block_average_read_inflights_full_size_set;
    data_index = dnx_data_dram_dram_block_define_wpr_increment_thr_factor;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_dram_block_wpr_increment_thr_factor_set;

    
    data_index = dnx_data_dram_dram_block_write_minus_read_leaky_bucket;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dram_dram_block_write_minus_read_leaky_bucket_set;
    data_index = dnx_data_dram_dram_block_write_leaky_bucket;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dram_dram_block_write_leaky_bucket_set;
    data_index = dnx_data_dram_dram_block_average_read_inflights_leaky_bucket;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dram_dram_block_average_read_inflights_leaky_bucket_set;
    data_index = dnx_data_dram_dram_block_average_read_inflights_log2_window_size;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dram_dram_block_average_read_inflights_log2_window_size_set;

    
    data_index = dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_set;
    data_index = dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dram_dram_block_write_leaky_bucket_increment_th_set;
    
    submodule_index = dnx_data_dram_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_dbal_define_hbm_trc_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_dbal_hbm_trc_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_wpr_increment_threshold_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_dbal_wpr_increment_threshold_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_average_read_inflights_increment_threshold_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_dbal_average_read_inflights_increment_threshold_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_average_read_inflights_decrement_threshold_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_dbal_average_read_inflights_decrement_threshold_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_hbm_pll_pdiv_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_dbal_hbm_pll_pdiv_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_hbm_pll_ch_mdiv_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_dbal_hbm_pll_ch_mdiv_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_hbm_pll_frefeff_info_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_dbal_hbm_pll_frefeff_info_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_hbm_pll_aux_post_enableb_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_dbal_hbm_pll_aux_post_enableb_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_hbm_pll_ch_enableb_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_dbal_hbm_pll_ch_enableb_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_hbm_pll_aux_post_diffcmos_en_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_dbal_hbm_pll_aux_post_diffcmos_en_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_dram_bist_mode_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_dbal_dram_bist_mode_nof_bits_set;

    

    
    
    submodule_index = dnx_data_dram_submodule_firmware;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_firmware_define_nof_hbm_spare_data_results;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_firmware_nof_hbm_spare_data_results_set;
    data_index = dnx_data_dram_firmware_define_sbus_clock_divider;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_firmware_sbus_clock_divider_set;
    data_index = dnx_data_dram_firmware_define_snap_state_init_done;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dram_firmware_snap_state_init_done_set;

    

    
    data_index = dnx_data_dram_firmware_table_rom;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dram_firmware_rom_set;
    data_index = dnx_data_dram_firmware_table_operation_status;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dram_firmware_operation_status_set;
    data_index = dnx_data_dram_firmware_table_operation_result_error_code;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dram_firmware_operation_result_error_code_set;
    data_index = dnx_data_dram_firmware_table_operation_result_last_operation;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dram_firmware_operation_result_last_operation_set;
    
    submodule_index = dnx_data_dram_submodule_apd_phy;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_dram_apd_phy_table_pll;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dram_apd_phy_pll_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

