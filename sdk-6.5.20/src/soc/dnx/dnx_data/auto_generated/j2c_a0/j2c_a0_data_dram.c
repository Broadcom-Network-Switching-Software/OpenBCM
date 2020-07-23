

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
#include <soc/shmoo_hbm16.h>
#include <bcm_int/dnx/dram/hbmc/hbmc.h>







static shr_error_e
j2c_a0_dnx_data_dram_hbm_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int feature_index = dnx_data_dram_hbm_is_supported;
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
j2c_a0_dnx_data_dram_hbm_dword_alignment_check_set(
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
j2c_a0_dnx_data_dram_hbm_low_temperature_dram_protction_set(
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
j2c_a0_dnx_data_dram_hbm_is_delete_bdb_supported_set(
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
j2c_a0_dnx_data_dram_hbm_seamless_commands_to_same_bank_control_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int feature_index = dnx_data_dram_hbm_seamless_commands_to_same_bank_control;
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
j2c_a0_dnx_data_dram_hbm_vendor_info_disable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int feature_index = dnx_data_dram_hbm_vendor_info_disable;
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
j2c_a0_dnx_data_dram_hbm_stop_traffic_low_temp_threshold_set(
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
j2c_a0_dnx_data_dram_hbm_restore_traffic_low_temp_threshold_set(
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
j2c_a0_dnx_data_dram_hbm_power_down_low_temp_threshold_set(
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
j2c_a0_dnx_data_dram_hbm_start_disabled_set(
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
j2c_a0_dnx_data_dram_hbm_channel_symmetric_regs_set(
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

    
    table->keys[0].size = 19;
    table->info_get.key_size[0] = 19;

    
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
        data->reg = HBC_DSI_ASYNC_FIFO_CONFIGr;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->reg = HBC_WPBQ_CONFIGr;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->reg = HBC_RPBQ_CONFIGr;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->reg = HBC_HBM_ROW_ACCESS_TIMINGSr;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->reg = HBC_HBM_COLUMN_ACCESS_TIMINGSr;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->reg = HBC_HBM_REFRESH_TIMINGSr;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->reg = HBC_HBM_GENERAL_TIMINGSr;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->reg = HBC_HBM_REFRESH_INTERVALSr;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->reg = HBC_TSM_CONFIGr;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->reg = HBC_TSM_READ_WRITE_SWITCH_CONFIGr;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->reg = HBC_PIPELINES_COMMAND_PIN_MUXr;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->reg = HBC_PIPELINES_BANK_REMAPPINGr;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->reg = HBC_PIPELINES_PHY_RCMDr;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->reg = HBC_HBM_CPU_CONTROLr;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->reg = HBC_PIPELINES_CONFIGr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_dram_hbm_channel_not_symmetric_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_hbm_channel_not_symmetric_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int table_index = dnx_data_dram_hbm_table_channel_not_symmetric_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 0;
    table->info_get.key_size[0] = 0;

    
    table->values[0].default_val = "INVALIDr";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_hbm_channel_not_symmetric_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_hbm_table_channel_not_symmetric_regs");

    
    default_data = (dnx_data_dram_hbm_channel_not_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_hbm_channel_not_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_dram_hbm_controller_symmetric_regs_set(
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
        data->reg = HBMC_HBM_PHY_CHANNEL_CONTROLr;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->reg = TDU_WRITE_REQUEST_PATH_CONFIGr;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->reg = TDU_READ_REQUEST_PATH_CONFIGr;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->reg = TDU_READ_DATA_PATH_CONFIGr;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->reg = TDU_ADDRESS_TRANSLATION_MATRIXr;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->reg = TDU_DRAM_BLOCKED_CONFIGr;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->reg = TDU_ADDRESS_TRANSLATION_CONFIGr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_dram_hbm_controller_not_symmetric_regs_set(
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

    
    table->keys[0].size = 0;
    table->info_get.key_size[0] = 0;

    
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_dram_hbm_hbc_last_in_chain_set(
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

    
    table->values[0].default_val = "5";
    
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
    default_data->is_last_in_chain = 5;
    
    for (dram_bitmap_index = 0; dram_bitmap_index < table->keys[0].size; dram_bitmap_index++)
    {
        for (dram_index_index = 0; dram_index_index < table->keys[1].size; dram_index_index++)
        {
            data = (dnx_data_dram_hbm_hbc_last_in_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, dram_bitmap_index, dram_index_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2c_a0_dnx_data_dram_general_info_max_nof_drams_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_max_nof_drams;
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
j2c_a0_dnx_data_dram_general_info_frequency_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_frequency;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1000;

    
    define->data = 1000;

    
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
j2c_a0_dnx_data_dram_general_info_dram_info_set(
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

    
    table->values[0].default_val = "SHMOO_HBM16_DRAM_TYPE_GEN2";
    table->values[1].default_val = "0x1";
    table->values[2].default_val = "32";
    table->values[3].default_val = "16384";
    table->values[4].default_val = "32";
    table->values[5].default_val = "DATA(dram, general_info, frequency) * 2";
    table->values[6].default_val = "100";
    table->values[7].default_val = "SHMOO_HBM16_CTL_TYPE_1";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_general_info_dram_info_t, (1 + 1  ), "data of dnx_data_dram_general_info_table_dram_info");

    
    default_data = (dnx_data_dram_general_info_dram_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dram_type = SHMOO_HBM16_DRAM_TYPE_GEN2;
    default_data->dram_bitmap = 0x1;
    default_data->nof_columns = 32;
    default_data->nof_rows = 16384;
    default_data->nof_banks = 32;
    default_data->data_rate = dnx_data_dram.general_info.frequency_get(unit) * 2;
    default_data->ref_clock = 100;
    default_data->ctl_type = SHMOO_HBM16_CTL_TYPE_1;
    
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
j2c_a0_dnx_data_dram_address_translation_tdu_map_set(
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_dram_address_translation_matrix_configuration_set(
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
        data->logical_to_physical[2] = 0x0000024;
        data->logical_to_physical[3] = 0x0004008;
        data->logical_to_physical[4] = 0x0000041;
        data->logical_to_physical[5] = 0x0008008;
        data->logical_to_physical[6] = 0x0010004;
        data->logical_to_physical[7] = 0x0020002;
        data->logical_to_physical[8] = 0x0000011;
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
        data->logical_to_physical[24] = 0x0800000;
        data->logical_to_physical[25] = 0x200000;
        data->logical_to_physical[26] = 0x0000000;
        data->logical_to_physical[27] = 0x0000000;
        data->logical_to_physical[28] = 0x0000000;
        data->logical_to_physical[29] = 0x0000000;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2c_a0_dnx_data_dram_dram_block_write_minus_read_leaky_bucket_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int feature_index = dnx_data_dram_dram_block_write_minus_read_leaky_bucket;
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
j2c_a0_dnx_data_dram_dram_block_write_minus_read_leaky_bucket_disable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int feature_index = dnx_data_dram_dram_block_write_minus_read_leaky_bucket_disable;
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
j2c_a0_dnx_data_dram_dram_block_write_plus_read_leaky_bucket_disable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int feature_index = dnx_data_dram_dram_block_write_plus_read_leaky_bucket_disable;
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
j2c_a0_dnx_data_dram_dram_block_average_read_inflights_leaky_bucket_set(
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
j2c_a0_dnx_data_dram_dram_block_average_read_inflights_log2_window_size_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int feature_index = dnx_data_dram_dram_block_average_read_inflights_log2_window_size;
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
j2c_a0_dnx_data_dram_dram_block_leaky_bucket_window_size_set(
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
j2c_a0_dnx_data_dram_dram_block_wmr_reset_on_deassert_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int define_index = dnx_data_dram_dram_block_define_wmr_reset_on_deassert;
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
j2c_a0_dnx_data_dram_dram_block_wmr_full_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int define_index = dnx_data_dram_dram_block_define_wmr_full_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 30;

    
    define->data = 30;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_dram_dram_block_average_read_inflights_assert_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int define_index = dnx_data_dram_dram_block_define_average_read_inflights_assert_threshold;
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
j2c_a0_dnx_data_dram_dram_block_average_read_inflights_full_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int define_index = dnx_data_dram_dram_block_define_average_read_inflights_full_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 44;

    
    define->data = 44;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2c_a0_dnx_data_dram_dbal_average_read_inflights_increment_threshold_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_average_read_inflights_increment_threshold_nof_bits;
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
j2c_a0_dnx_data_dram_dbal_average_read_inflights_decrement_threshold_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_average_read_inflights_decrement_threshold_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20;

    
    define->data = 20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2c_a0_data_dram_attach(
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
    define->set = j2c_a0_dnx_data_dram_hbm_stop_traffic_low_temp_threshold_set;
    data_index = dnx_data_dram_hbm_define_restore_traffic_low_temp_threshold;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_dram_hbm_restore_traffic_low_temp_threshold_set;
    data_index = dnx_data_dram_hbm_define_power_down_low_temp_threshold;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_dram_hbm_power_down_low_temp_threshold_set;
    data_index = dnx_data_dram_hbm_define_start_disabled;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_dram_hbm_start_disabled_set;

    
    data_index = dnx_data_dram_hbm_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_dram_hbm_is_supported_set;
    data_index = dnx_data_dram_hbm_dword_alignment_check;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_dram_hbm_dword_alignment_check_set;
    data_index = dnx_data_dram_hbm_low_temperature_dram_protction;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_dram_hbm_low_temperature_dram_protction_set;
    data_index = dnx_data_dram_hbm_is_delete_bdb_supported;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_dram_hbm_is_delete_bdb_supported_set;
    data_index = dnx_data_dram_hbm_seamless_commands_to_same_bank_control;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_dram_hbm_seamless_commands_to_same_bank_control_set;
    data_index = dnx_data_dram_hbm_vendor_info_disable;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_dram_hbm_vendor_info_disable_set;

    
    data_index = dnx_data_dram_hbm_table_channel_symmetric_regs;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_dram_hbm_channel_symmetric_regs_set;
    data_index = dnx_data_dram_hbm_table_channel_not_symmetric_regs;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_dram_hbm_channel_not_symmetric_regs_set;
    data_index = dnx_data_dram_hbm_table_controller_symmetric_regs;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_dram_hbm_controller_symmetric_regs_set;
    data_index = dnx_data_dram_hbm_table_controller_not_symmetric_regs;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_dram_hbm_controller_not_symmetric_regs_set;
    data_index = dnx_data_dram_hbm_table_hbc_last_in_chain;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_dram_hbm_hbc_last_in_chain_set;
    
    submodule_index = dnx_data_dram_submodule_general_info;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_general_info_define_max_nof_drams;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_dram_general_info_max_nof_drams_set;
    data_index = dnx_data_dram_general_info_define_frequency;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_dram_general_info_frequency_set;

    

    
    data_index = dnx_data_dram_general_info_table_dram_info;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_dram_general_info_dram_info_set;
    
    submodule_index = dnx_data_dram_submodule_address_translation;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_dram_address_translation_table_tdu_map;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_dram_address_translation_tdu_map_set;
    data_index = dnx_data_dram_address_translation_table_matrix_configuration;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_dram_address_translation_matrix_configuration_set;
    
    submodule_index = dnx_data_dram_submodule_dram_block;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_dram_block_define_leaky_bucket_window_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_dram_dram_block_leaky_bucket_window_size_set;
    data_index = dnx_data_dram_dram_block_define_wmr_reset_on_deassert;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_dram_dram_block_wmr_reset_on_deassert_set;
    data_index = dnx_data_dram_dram_block_define_wmr_full_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_dram_dram_block_wmr_full_size_set;
    data_index = dnx_data_dram_dram_block_define_average_read_inflights_assert_threshold;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_dram_dram_block_average_read_inflights_assert_threshold_set;
    data_index = dnx_data_dram_dram_block_define_average_read_inflights_full_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_dram_dram_block_average_read_inflights_full_size_set;

    
    data_index = dnx_data_dram_dram_block_write_minus_read_leaky_bucket;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_dram_dram_block_write_minus_read_leaky_bucket_set;
    data_index = dnx_data_dram_dram_block_write_minus_read_leaky_bucket_disable;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_dram_dram_block_write_minus_read_leaky_bucket_disable_set;
    data_index = dnx_data_dram_dram_block_write_plus_read_leaky_bucket_disable;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_dram_dram_block_write_plus_read_leaky_bucket_disable_set;
    data_index = dnx_data_dram_dram_block_average_read_inflights_leaky_bucket;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_dram_dram_block_average_read_inflights_leaky_bucket_set;
    data_index = dnx_data_dram_dram_block_average_read_inflights_log2_window_size;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_dram_dram_block_average_read_inflights_log2_window_size_set;

    
    
    submodule_index = dnx_data_dram_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_dbal_define_average_read_inflights_increment_threshold_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_dram_dbal_average_read_inflights_increment_threshold_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_average_read_inflights_decrement_threshold_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_dram_dbal_average_read_inflights_decrement_threshold_nof_bits_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

