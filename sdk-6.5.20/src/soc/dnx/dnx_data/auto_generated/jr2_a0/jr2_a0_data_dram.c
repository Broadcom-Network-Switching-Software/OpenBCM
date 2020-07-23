

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
#include <shared/utilex/utilex_integer_arithmetic.h>







static shr_error_e
jr2_a0_dnx_data_dram_hbm_is_supported_set(
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
jr2_a0_dnx_data_dram_hbm_dword_alignment_check_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int feature_index = dnx_data_dram_hbm_dword_alignment_check;
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
jr2_a0_dnx_data_dram_hbm_low_temperature_dram_protction_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int feature_index = dnx_data_dram_hbm_low_temperature_dram_protction;
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
jr2_a0_dnx_data_dram_hbm_is_delete_bdb_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int feature_index = dnx_data_dram_hbm_is_delete_bdb_supported;
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
jr2_a0_dnx_data_dram_hbm_cpu2tap_access_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int feature_index = dnx_data_dram_hbm_cpu2tap_access;
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
jr2_a0_dnx_data_dram_hbm_burst_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int define_index = dnx_data_dram_hbm_define_burst_length;
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
jr2_a0_dnx_data_dram_hbm_stop_traffic_temp_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int define_index = dnx_data_dram_hbm_define_stop_traffic_temp_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 90;

    
    define->data = 90;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_DRAM_TEMPERATURE_THRESHOLD_STOP_TRAFFIC;
    define->property.doc = 
        "\n"
        "This is soc property that holds the temperature threshold, for stop directing traffic to DRAM.\n"
        "Units: degrees Celsius\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = dnx_data_dram.hbm.restore_traffic_temp_threshold_get(unit)+1;
    define->property.range_max = 105;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_hbm_restore_traffic_temp_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int define_index = dnx_data_dram_hbm_define_restore_traffic_temp_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 85;

    
    define->data = 85;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_DRAM_TEMPERATURE_THRESHOLD_RESTORE_TRAFFIC;
    define->property.doc = 
        "\n"
        "This is soc property that holds the temperature threshold, for restore directing traffic to DRAM.\n"
        "Units: degrees Celsius\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 105;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_hbm_usec_between_temp_samples_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int define_index = dnx_data_dram_hbm_define_usec_between_temp_samples;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 500000;

    
    define->data = 500000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_hbm_power_down_temp_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int define_index = dnx_data_dram_hbm_define_power_down_temp_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 95;

    
    define->data = 95;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_DRAM_TEMPERATURE_THRESHOLD_POWER_DOWN;
    define->property.doc = 
        "\n"
        "This is soc property that holds the temperature threshold, for doing DRAM shut down.\n"
        "Units: degrees Celsius\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = dnx_data_dram.hbm.stop_traffic_temp_threshold_get(unit)+1;
    define->property.range_max = 105;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_hbm_dram_temp_monitor_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int define_index = dnx_data_dram_hbm_define_dram_temp_monitor_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_DRAM_TEMPERATURE_MONITOR_ENABLE;
    define->property.doc = 
        "\n"
        "Enable/disable SDK DRAM temperature monitor\n"
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
jr2_a0_dnx_data_dram_hbm_start_disabled_set(
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_hbm_output_enable_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int define_index = dnx_data_dram_hbm_define_output_enable_length;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 + dnx_data_dram.hbm.burst_length_get(unit)/2 + 1;

    
    define->data = 1 + dnx_data_dram.hbm.burst_length_get(unit)/2 + 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_hbm_output_enable_delay_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int define_index = dnx_data_dram_hbm_define_output_enable_delay;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_dram.general_info.write_latency_get(unit) - 1;

    
    define->data = dnx_data_dram.general_info.write_latency_get(unit) - 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_hbm_default_model_part_num_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int define_index = dnx_data_dram_hbm_define_default_model_part_num;
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
jr2_a0_dnx_data_dram_hbm_channel_symmetric_regs_set(
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

    
    table->keys[0].size = 18;
    table->info_get.key_size[0] = 18;

    
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_hbm_channel_not_symmetric_regs_set(
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

    
    table->keys[0].size = 1;
    table->info_get.key_size[0] = 1;

    
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
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_not_symmetric_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = HBC_PIPELINES_CONFIGr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_hbm_controller_symmetric_regs_set(
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

    
    table->keys[0].size = 9;
    table->info_get.key_size[0] = 9;

    
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_hbm_controller_not_symmetric_regs_set(
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

    
    table->keys[0].size = 1;
    table->info_get.key_size[0] = 1;

    
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_hbm_channel_interrupt_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_hbm_channel_interrupt_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int table_index = dnx_data_dram_hbm_table_channel_interrupt_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 5;
    table->info_get.key_size[0] = 5;

    
    table->values[0].default_val = "INVALIDr";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_hbm_channel_interrupt_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_hbm_table_channel_interrupt_regs");

    
    default_data = (dnx_data_dram_hbm_channel_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_hbm_channel_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = HBC_INTERRUPT_REGISTERr;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = HBC_ECC_INTERRUPT_REGISTERr;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->reg = HBC_DSI_INTERRUPT_REGISTERr;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->reg = HBC_RDR_INTERRUPT_REGISTERr;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->reg = HBC_PIPELINES_INTERRUPT_REGISTERr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_hbm_controller_interrupt_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_hbm_controller_interrupt_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int table_index = dnx_data_dram_hbm_table_controller_interrupt_regs;
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

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_hbm_controller_interrupt_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_hbm_table_controller_interrupt_regs");

    
    default_data = (dnx_data_dram_hbm_controller_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_hbm_controller_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = HBMC_INTERRUPT_REGISTERr;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = HBMC_ECC_INTERRUPT_REGISTERr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_hbm_channel_debug_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_hbm_channel_debug_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int table_index = dnx_data_dram_hbm_table_channel_debug_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "INVALIDr";
    table->values[1].default_val = "NULL";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_hbm_channel_debug_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_hbm_table_channel_debug_regs");

    
    default_data = (dnx_data_dram_hbm_channel_debug_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    default_data->reassuring_str = NULL;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_hbm_channel_debug_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_debug_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = HBC_HBM_ERR_COUNTERSr;
        data->reassuring_str = "HBM error counters are cleared";
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_debug_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = HBC_PIPELINES_DEBUGr;
        data->reassuring_str = "Pipeline debug info is cleared";
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_hbm_channel_counter_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_hbm_channel_counter_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int table_index = dnx_data_dram_hbm_table_channel_counter_regs;
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

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_hbm_channel_counter_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_hbm_table_channel_counter_regs");

    
    default_data = (dnx_data_dram_hbm_channel_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_hbm_channel_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = HBC_TSM_DEBUGr;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = HBC_HBM_COMMAND_COUNTERSr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_hbm_channel_type_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_hbm_channel_type_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int table_index = dnx_data_dram_hbm_table_channel_type_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "INVALIDr";
    table->values[1].default_val = "DNX_HBMC_NOF_DIAG_REG_TYPES";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_hbm_channel_type_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_hbm_table_channel_type_regs");

    
    default_data = (dnx_data_dram_hbm_channel_type_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    default_data->type = DNX_HBMC_NOF_DIAG_REG_TYPES;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_hbm_channel_type_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_type_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = HBC_WPBQ_STATUSr;
        data->type = DNX_HBMC_DIAG_REG_HISTOGRAM_32_8;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_channel_type_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = HBC_RPBQ_STATUSr;
        data->type = DNX_HBMC_DIAG_REG_HISTOGRAM_32_8;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_hbm_controller_info_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_hbm_controller_info_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int table_index = dnx_data_dram_hbm_table_controller_info_regs;
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

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_hbm_controller_info_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_hbm_table_controller_info_regs");

    
    default_data = (dnx_data_dram_hbm_controller_info_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_hbm_controller_info_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_info_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = HBMC_HBM_PLL_STATUSr;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_hbm_controller_info_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = HBMC_HBM_STATUSr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_hbm_bist_set(
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

    
    table->values[0].default_val = "0x12345, 0x34755, 0x23003, 0x98654, 0xab342, 0xff43, 0x95847, 0x12587, 0xcdb45, 0x453, 0x958db, 0x37485, 0xadf45, 0x128f, 0x54785, 0xf123b";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_hbm_bist_t, (1 + 1  ), "data of dnx_data_dram_hbm_table_bist");

    
    default_data = (dnx_data_dram_hbm_bist_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    SHR_RANGE_VERIFY(16, 0, HBMC_SHMOO_BIST_NOF_PRBS_DATA_SEEDS, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->prbs_seeds[0] = 0x12345;
    default_data->prbs_seeds[1] = 0x34755;
    default_data->prbs_seeds[2] = 0x23003;
    default_data->prbs_seeds[3] = 0x98654;
    default_data->prbs_seeds[4] = 0xab342;
    default_data->prbs_seeds[5] = 0xff43;
    default_data->prbs_seeds[6] = 0x95847;
    default_data->prbs_seeds[7] = 0x12587;
    default_data->prbs_seeds[8] = 0xcdb45;
    default_data->prbs_seeds[9] = 0x453;
    default_data->prbs_seeds[10] = 0x958db;
    default_data->prbs_seeds[11] = 0x37485;
    default_data->prbs_seeds[12] = 0xadf45;
    default_data->prbs_seeds[13] = 0x128f;
    default_data->prbs_seeds[14] = 0x54785;
    default_data->prbs_seeds[15] = 0xf123b;
    
    data = (dnx_data_dram_hbm_bist_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_hbm_hbc_last_in_chain_set(
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_dram_general_info_is_temperature_reading_supported_set(
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
jr2_a0_dnx_data_dram_general_info_otp_restore_version_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_otp_restore_version;
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
jr2_a0_dnx_data_dram_general_info_max_nof_drams_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_max_nof_drams;
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
jr2_a0_dnx_data_dram_general_info_nof_channels_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_nof_channels;
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
jr2_a0_dnx_data_dram_general_info_mr_mask_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_mr_mask;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xff;

    
    define->data = 0xff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_general_info_nof_mrs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_nof_mrs;
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
jr2_a0_dnx_data_dram_general_info_phy_address_mask_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_phy_address_mask;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xfff;

    
    define->data = 0xfff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_general_info_max_dram_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_max_dram_index;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = shr_bitop_last_bit_get(&dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap, dnx_data_dram.general_info.max_nof_drams_get(unit));

    
    define->data = shr_bitop_last_bit_get(&dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap, dnx_data_dram.general_info.max_nof_drams_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_general_info_min_dram_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_min_dram_index;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = shr_bitop_first_bit_get(&dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap, dnx_data_dram.general_info.max_nof_drams_get(unit));

    
    define->data = shr_bitop_first_bit_get(&dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap, dnx_data_dram.general_info.max_nof_drams_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_general_info_frequency_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_frequency;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 500;

    
    define->data = 500;

    
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
jr2_a0_dnx_data_dram_general_info_buffer_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_buffer_size;
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
jr2_a0_dnx_data_dram_general_info_command_address_parity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_command_address_parity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_RAM_COMMAND_ADDRESS_PARITY;
    define->property.doc = 
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "0 - Command/address parity disabled\n"
        "1 - Command/address parity enabled\n"
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
jr2_a0_dnx_data_dram_general_info_dq_write_parity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_dq_write_parity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "ext_ram_dq_write_parity";
    define->property.doc = 
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "0 - DQ write parity disabled\n"
        "1 - DQ write parity enabled\n"
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
jr2_a0_dnx_data_dram_general_info_dq_read_parity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_dq_read_parity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "ext_ram_dq_read_parity";
    define->property.doc = 
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "0 - DQ read parity disabled\n"
        "1 - DQ read parity enabled\n"
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
jr2_a0_dnx_data_dram_general_info_dbi_read_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_dbi_read;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_RAM_READ_DBI;
    define->property.doc = 
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "0 - Read DBIac disabled\n"
        "1 - Read DBIac enabled\n"
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
jr2_a0_dnx_data_dram_general_info_dbi_write_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_dbi_write;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_RAM_WRITE_DBI;
    define->property.doc = 
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "0 - Write DBIac disabled\n"
        "1 - Write DBIac enabled\n"
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
jr2_a0_dnx_data_dram_general_info_write_latency_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_write_latency;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_RAM_WRITE_LATENCY;
    define->property.doc = 
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "HBM write latency (WR) in tCK units\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 15;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_general_info_read_latency_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_read_latency;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 22;

    
    define->data = 22;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_RAM_READ_LATENCY;
    define->property.doc = 
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "HBM read latency (RD) in tCK units\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 30;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_general_info_parity_latency_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_parity_latency;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_RAM_PARITY_LATENCY;
    define->property.doc = 
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "DQ Parity latency (PL) in tCK units\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 3;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_general_info_actual_parity_latency_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_actual_parity_latency;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_dram.general_info.parity_latency_get(unit) * dnx_data_dram.general_info.dq_read_parity_get(unit);

    
    define->data = dnx_data_dram.general_info.parity_latency_get(unit) * dnx_data_dram.general_info.dq_read_parity_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_general_info_tune_mode_on_init_set(
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
    define->property.nof_mapping = 5;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "RESTORE_TUNE_PARAMETERS_FROM_SOC_PROPERTIES";
    define->property.mapping[0].val = DNX_HBMC_RESTORE_TUNE_PARAMETERS_FROM_SOC_PROPERTIES;
    define->property.mapping[1].name = "RESTORE_TUNE_PARAMETERS_FROM_OTP";
    define->property.mapping[1].val = DNX_HBMC_RESTORE_TUNE_PARAMETERS_FROM_OTP;
    define->property.mapping[2].name = "RUN_TUNE";
    define->property.mapping[2].val = DNX_HBMC_RUN_TUNE;
    define->property.mapping[2].is_default = 1 ;
    define->property.mapping[3].name = "SKIP_TUNE";
    define->property.mapping[3].val = DNX_HBMC_SKIP_TUNE;
    define->property.mapping[4].name = "RESTORE_TUNE_PARAMS_FROM_SOC_PROPS_OR_OTP_OR_TUNE";
    define->property.mapping[4].val = DNX_HBMC_RESTORE_TUNE_PARAMS_FROM_SOC_PROPS_OR_OTP_OR_TUNE;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_dram_general_info_mr_defaults_set(
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
    
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_mr_defaults_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->value = 0x70;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_mr_defaults_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->value = 0xc0;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_mr_defaults_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->value = 0x3;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_general_info_dram_info_set(
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
    table->values[1].default_val = "0x2";
    table->values[2].default_val = "32";
    table->values[3].default_val = "16384";
    table->values[4].default_val = "32";
    table->values[5].default_val = "DATA(dram, general_info, frequency) * 2";
    table->values[6].default_val = "100";
    table->values[7].default_val = "SHMOO_HBM16_CTL_TYPE_1";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_general_info_dram_info_t, (1 + 1  ), "data of dnx_data_dram_general_info_table_dram_info");

    
    default_data = (dnx_data_dram_general_info_dram_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dram_type = SHMOO_HBM16_DRAM_TYPE_GEN2;
    default_data->dram_bitmap = 0x2;
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
jr2_a0_dnx_data_dram_general_info_timing_params_set(
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

    
    table->values[0].default_val = "17";
    table->values[1].default_val = "16";
    table->values[2].default_val = "7";
    table->values[3].default_val = "7";
    table->values[4].default_val = "4";
    table->values[5].default_val = "4";
    table->values[6].default_val = "16";
    table->values[7].default_val = "14";
    table->values[8].default_val = "16";
    table->values[9].default_val = "33";
    table->values[10].default_val = "47";
    table->values[11].default_val = "9";
    table->values[12].default_val = "5";
    table->values[13].default_val = "21";
    table->values[14].default_val = "3";
    table->values[15].default_val = "2";
    table->values[16].default_val = "4";
    table->values[17].default_val = "8";
    table->values[18].default_val = "160";
    table->values[19].default_val = "260";
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
    default_data->twr = 17;
    default_data->trp = 16;
    default_data->trtps = 7;
    default_data->trtpl = 7;
    default_data->trrds = 4;
    default_data->trrdl = 4;
    default_data->tfaw = 16;
    default_data->trcdwr = 14;
    default_data->trcdrd = 16;
    default_data->tras = 33;
    default_data->trc = 47;
    default_data->twtrl = 9;
    default_data->twtrs = 5;
    default_data->trtw = 21;
    default_data->tccdr = 3;
    default_data->tccds = 2;
    default_data->tccdl = 4;
    default_data->trrefd = 8;
    default_data->trfcsb = 160;
    default_data->trfc = 260;
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
jr2_a0_dnx_data_dram_general_info_refresh_intervals_set(
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
    table->values[1].default_val = "60";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_general_info_refresh_intervals_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_general_info_table_refresh_intervals");

    
    default_data = (dnx_data_dram_general_info_refresh_intervals_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->trefi = 3900;
    default_data->trefisb = 60;
    
    for (temp_index_index = 0; temp_index_index < table->keys[0].size; temp_index_index++)
    {
        data = (dnx_data_dram_general_info_refresh_intervals_t *) dnxc_data_mgmt_table_data_get(unit, table, temp_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
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
jr2_a0_dnx_data_dram_address_translation_matrix_column_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_address_translation;
    int define_index = dnx_data_dram_address_translation_define_matrix_column_size;
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
jr2_a0_dnx_data_dram_address_translation_physical_address_transaction_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_address_translation;
    int define_index = dnx_data_dram_address_translation_define_physical_address_transaction_size;
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
jr2_a0_dnx_data_dram_address_translation_nof_tdus_per_dram_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_address_translation;
    int define_index = dnx_data_dram_address_translation_define_nof_tdus_per_dram;
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
jr2_a0_dnx_data_dram_address_translation_tdu_map_set(
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
    if (1 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_dram_address_translation_tdu_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->data_source_id = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_address_translation_matrix_configuration_set(
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
        data->logical_to_physical[25] = 0x1000000;
        data->logical_to_physical[26] = 0x0000000;
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
        data->logical_to_physical[24] = 0x0200000;
        data->logical_to_physical[25] = 0x1000000;
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
jr2_a0_dnx_data_dram_address_translation_interrupt_regs_set(
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

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_address_translation_counter_regs_set(
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

    
    table->keys[0].size = 6;
    table->info_get.key_size[0] = 6;

    
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_dram_buffers_quarantine_buffers_if_tdu_corrected_errors_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_buffers;
    int feature_index = dnx_data_dram_buffers_quarantine_buffers_if_tdu_corrected_errors;
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
jr2_a0_dnx_data_dram_buffers_quarantine_buffers_if_tdu_errors_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_buffers;
    int feature_index = dnx_data_dram_buffers_quarantine_buffers_if_tdu_errors;
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
jr2_a0_dnx_data_dram_buffers_allowed_errors_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_buffers;
    int define_index = dnx_data_dram_buffers_define_allowed_errors;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_QUARANTINE_MECHANISM_ALLOWED_ERRORS;
    define->property.doc = 
        "\n"
        "Define the amount of errors a single buffer can have before the Quarantine Mechanism delete the buffer instead of restoring it\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 100;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_buffers_nof_bdbs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_buffers;
    int define_index = dnx_data_dram_buffers_define_nof_bdbs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 131072;

    
    define->data = 131072;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_buffers_nof_fpc_banks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_buffers;
    int define_index = dnx_data_dram_buffers_define_nof_fpc_banks;
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
jr2_a0_dnx_data_dram_buffers_deleted_buffers_info_set(
    int unit)
{
    dnx_data_dram_buffers_deleted_buffers_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_buffers;
    int table_index = dnx_data_dram_buffers_table_deleted_buffers_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "./deleted_buffers_file";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_buffers_deleted_buffers_info_t, (1 + 1  ), "data of dnx_data_dram_buffers_table_deleted_buffers_info");

    
    default_data = (dnx_data_dram_buffers_deleted_buffers_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->deleted_buffers_file = "./deleted_buffers_file";
    
    data = (dnx_data_dram_buffers_deleted_buffers_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[0].property.name = spn_DELETED_BUFFERS_FILE_PATH;
    table->values[0].property.doc =
        "\n"
        "Path for deleted buffers file, if a file exists it will be used,\n"
        "if not it will be created in this path according to need.\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_str;
    table->values[0].property.method_str = "str";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_dram_buffers_deleted_buffers_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, -1, &data->deleted_buffers_file));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_dram_dram_block_write_minus_read_leaky_bucket_set(
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
jr2_a0_dnx_data_dram_dram_block_leaky_bucket_window_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int define_index = dnx_data_dram_dram_block_define_leaky_bucket_window_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5000;

    
    define->data = 5000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_dram_block_wmr_reset_on_deassert_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int define_index = dnx_data_dram_dram_block_define_wmr_reset_on_deassert;
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
jr2_a0_dnx_data_dram_dram_block_wmr_full_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int define_index = dnx_data_dram_dram_block_define_wmr_full_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 50;

    
    define->data = 50;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_dram_block_wmr_decrement_thr_factor_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int define_index = dnx_data_dram_dram_block_define_wmr_decrement_thr_factor;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8 * dnx_data_dram.dram_block.leaky_bucket_window_size_get(unit) * dnx_data_dram.general_info.frequency_get(unit) / 1000;

    
    define->data = 8 * dnx_data_dram.dram_block.leaky_bucket_window_size_get(unit) * dnx_data_dram.general_info.frequency_get(unit) / 1000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_dram_block_wpr_increment_thr_factor_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int define_index = dnx_data_dram_dram_block_define_wpr_increment_thr_factor;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8 * dnx_data_dram.dram_block.leaky_bucket_window_size_get(unit) * dnx_data_dram.general_info.max_nof_drams_get(unit) * dnx_data_dram.general_info.frequency_get(unit) / 1000;

    
    define->data = 8 * dnx_data_dram.dram_block.leaky_bucket_window_size_get(unit) * dnx_data_dram.general_info.max_nof_drams_get(unit) * dnx_data_dram.general_info.frequency_get(unit) / 1000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_set(
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
        data->threshold = 91 * dnx_data_dram.dram_block.wpr_increment_thr_factor_get(unit) / 100;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->threshold = 93 * dnx_data_dram.dram_block.wpr_increment_thr_factor_get(unit) / 100;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->threshold = 95 * dnx_data_dram.dram_block.wpr_increment_thr_factor_get(unit) / 100;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_dram_dbal_hbm_trc_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_hbm_trc_nof_bits;
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
jr2_a0_dnx_data_dram_dbal_wpr_increment_threshold_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_wpr_increment_threshold_nof_bits;
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
jr2_a0_dnx_data_dram_dbal_hbm_pll_pdiv_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_hbm_pll_pdiv_nof_bits;
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
jr2_a0_dnx_data_dram_dbal_hbm_pll_ch_mdiv_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_hbm_pll_ch_mdiv_nof_bits;
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
jr2_a0_dnx_data_dram_dbal_hbm_pll_frefeff_info_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_hbm_pll_frefeff_info_nof_bits;
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
jr2_a0_dnx_data_dram_dbal_hbm_pll_aux_post_enableb_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_hbm_pll_aux_post_enableb_nof_bits;
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
jr2_a0_dnx_data_dram_dbal_hbm_pll_ch_enableb_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_hbm_pll_ch_enableb_nof_bits;
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
jr2_a0_dnx_data_dram_dbal_hbm_pll_aux_post_diffcmos_en_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_hbm_pll_aux_post_diffcmos_en_nof_bits;
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
jr2_a0_dnx_data_dram_dbal_dram_bist_mode_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_dram_bist_mode_nof_bits;
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
jr2_a0_dnx_data_dram_dbal_hbmc_index_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_hbmc_index_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_dram.general_info.max_nof_drams_get(unit) > 1 ? utilex_log2_round_up(dnx_data_dram.general_info.max_nof_drams_get(unit)) : 1;

    
    define->data = dnx_data_dram.general_info.max_nof_drams_get(unit) > 1 ? utilex_log2_round_up(dnx_data_dram.general_info.max_nof_drams_get(unit)) : 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dram_dbal_hbmc_tdu_index_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_hbmc_tdu_index_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_dram.address_translation.nof_tdus_per_dram_get(unit) > 1 ? utilex_log2_round_up(dnx_data_dram.address_translation.nof_tdus_per_dram_get(unit)) : 1;

    
    define->data = dnx_data_dram.address_translation.nof_tdus_per_dram_get(unit) > 1 ? utilex_log2_round_up(dnx_data_dram.address_translation.nof_tdus_per_dram_get(unit)) : 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_dram_attach(
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

    
    data_index = dnx_data_dram_hbm_define_burst_length;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_hbm_burst_length_set;
    data_index = dnx_data_dram_hbm_define_stop_traffic_temp_threshold;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_hbm_stop_traffic_temp_threshold_set;
    data_index = dnx_data_dram_hbm_define_restore_traffic_temp_threshold;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_hbm_restore_traffic_temp_threshold_set;
    data_index = dnx_data_dram_hbm_define_usec_between_temp_samples;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_hbm_usec_between_temp_samples_set;
    data_index = dnx_data_dram_hbm_define_power_down_temp_threshold;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_hbm_power_down_temp_threshold_set;
    data_index = dnx_data_dram_hbm_define_dram_temp_monitor_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_hbm_dram_temp_monitor_enable_set;
    data_index = dnx_data_dram_hbm_define_start_disabled;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_hbm_start_disabled_set;
    data_index = dnx_data_dram_hbm_define_output_enable_length;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_hbm_output_enable_length_set;
    data_index = dnx_data_dram_hbm_define_output_enable_delay;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_hbm_output_enable_delay_set;
    data_index = dnx_data_dram_hbm_define_default_model_part_num;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_hbm_default_model_part_num_set;

    
    data_index = dnx_data_dram_hbm_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_dram_hbm_is_supported_set;
    data_index = dnx_data_dram_hbm_dword_alignment_check;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_dram_hbm_dword_alignment_check_set;
    data_index = dnx_data_dram_hbm_low_temperature_dram_protction;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_dram_hbm_low_temperature_dram_protction_set;
    data_index = dnx_data_dram_hbm_is_delete_bdb_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_dram_hbm_is_delete_bdb_supported_set;
    data_index = dnx_data_dram_hbm_cpu2tap_access;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_dram_hbm_cpu2tap_access_set;

    
    data_index = dnx_data_dram_hbm_table_channel_symmetric_regs;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_hbm_channel_symmetric_regs_set;
    data_index = dnx_data_dram_hbm_table_channel_not_symmetric_regs;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_hbm_channel_not_symmetric_regs_set;
    data_index = dnx_data_dram_hbm_table_controller_symmetric_regs;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_hbm_controller_symmetric_regs_set;
    data_index = dnx_data_dram_hbm_table_controller_not_symmetric_regs;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_hbm_controller_not_symmetric_regs_set;
    data_index = dnx_data_dram_hbm_table_channel_interrupt_regs;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_hbm_channel_interrupt_regs_set;
    data_index = dnx_data_dram_hbm_table_controller_interrupt_regs;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_hbm_controller_interrupt_regs_set;
    data_index = dnx_data_dram_hbm_table_channel_debug_regs;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_hbm_channel_debug_regs_set;
    data_index = dnx_data_dram_hbm_table_channel_counter_regs;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_hbm_channel_counter_regs_set;
    data_index = dnx_data_dram_hbm_table_channel_type_regs;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_hbm_channel_type_regs_set;
    data_index = dnx_data_dram_hbm_table_controller_info_regs;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_hbm_controller_info_regs_set;
    data_index = dnx_data_dram_hbm_table_bist;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_hbm_bist_set;
    data_index = dnx_data_dram_hbm_table_hbc_last_in_chain;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_hbm_hbc_last_in_chain_set;
    
    submodule_index = dnx_data_dram_submodule_general_info;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_general_info_define_otp_restore_version;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_general_info_otp_restore_version_set;
    data_index = dnx_data_dram_general_info_define_max_nof_drams;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_general_info_max_nof_drams_set;
    data_index = dnx_data_dram_general_info_define_nof_channels;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_general_info_nof_channels_set;
    data_index = dnx_data_dram_general_info_define_mr_mask;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_general_info_mr_mask_set;
    data_index = dnx_data_dram_general_info_define_nof_mrs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_general_info_nof_mrs_set;
    data_index = dnx_data_dram_general_info_define_phy_address_mask;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_general_info_phy_address_mask_set;
    data_index = dnx_data_dram_general_info_define_max_dram_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_general_info_max_dram_index_set;
    data_index = dnx_data_dram_general_info_define_min_dram_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_general_info_min_dram_index_set;
    data_index = dnx_data_dram_general_info_define_frequency;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_general_info_frequency_set;
    data_index = dnx_data_dram_general_info_define_buffer_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_general_info_buffer_size_set;
    data_index = dnx_data_dram_general_info_define_command_address_parity;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_general_info_command_address_parity_set;
    data_index = dnx_data_dram_general_info_define_dq_write_parity;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_general_info_dq_write_parity_set;
    data_index = dnx_data_dram_general_info_define_dq_read_parity;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_general_info_dq_read_parity_set;
    data_index = dnx_data_dram_general_info_define_dbi_read;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_general_info_dbi_read_set;
    data_index = dnx_data_dram_general_info_define_dbi_write;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_general_info_dbi_write_set;
    data_index = dnx_data_dram_general_info_define_write_latency;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_general_info_write_latency_set;
    data_index = dnx_data_dram_general_info_define_read_latency;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_general_info_read_latency_set;
    data_index = dnx_data_dram_general_info_define_parity_latency;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_general_info_parity_latency_set;
    data_index = dnx_data_dram_general_info_define_actual_parity_latency;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_general_info_actual_parity_latency_set;
    data_index = dnx_data_dram_general_info_define_tune_mode_on_init;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_general_info_tune_mode_on_init_set;

    
    data_index = dnx_data_dram_general_info_is_temperature_reading_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_dram_general_info_is_temperature_reading_supported_set;

    
    data_index = dnx_data_dram_general_info_table_mr_defaults;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_general_info_mr_defaults_set;
    data_index = dnx_data_dram_general_info_table_dram_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_general_info_dram_info_set;
    data_index = dnx_data_dram_general_info_table_timing_params;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_general_info_timing_params_set;
    data_index = dnx_data_dram_general_info_table_refresh_intervals;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_general_info_refresh_intervals_set;
    
    submodule_index = dnx_data_dram_submodule_address_translation;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_address_translation_define_matrix_column_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_address_translation_matrix_column_size_set;
    data_index = dnx_data_dram_address_translation_define_physical_address_transaction_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_address_translation_physical_address_transaction_size_set;
    data_index = dnx_data_dram_address_translation_define_nof_tdus_per_dram;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_address_translation_nof_tdus_per_dram_set;

    

    
    data_index = dnx_data_dram_address_translation_table_tdu_map;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_address_translation_tdu_map_set;
    data_index = dnx_data_dram_address_translation_table_matrix_configuration;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_address_translation_matrix_configuration_set;
    data_index = dnx_data_dram_address_translation_table_interrupt_regs;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_address_translation_interrupt_regs_set;
    data_index = dnx_data_dram_address_translation_table_counter_regs;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_address_translation_counter_regs_set;
    
    submodule_index = dnx_data_dram_submodule_buffers;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_buffers_define_allowed_errors;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_buffers_allowed_errors_set;
    data_index = dnx_data_dram_buffers_define_nof_bdbs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_buffers_nof_bdbs_set;
    data_index = dnx_data_dram_buffers_define_nof_fpc_banks;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_buffers_nof_fpc_banks_set;

    
    data_index = dnx_data_dram_buffers_quarantine_buffers_if_tdu_corrected_errors;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_dram_buffers_quarantine_buffers_if_tdu_corrected_errors_set;
    data_index = dnx_data_dram_buffers_quarantine_buffers_if_tdu_errors;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_dram_buffers_quarantine_buffers_if_tdu_errors_set;

    
    data_index = dnx_data_dram_buffers_table_deleted_buffers_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_buffers_deleted_buffers_info_set;
    
    submodule_index = dnx_data_dram_submodule_dram_block;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_dram_block_define_leaky_bucket_window_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_dram_block_leaky_bucket_window_size_set;
    data_index = dnx_data_dram_dram_block_define_wmr_reset_on_deassert;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_dram_block_wmr_reset_on_deassert_set;
    data_index = dnx_data_dram_dram_block_define_wmr_full_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_dram_block_wmr_full_size_set;
    data_index = dnx_data_dram_dram_block_define_wmr_decrement_thr_factor;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_dram_block_wmr_decrement_thr_factor_set;
    data_index = dnx_data_dram_dram_block_define_wpr_increment_thr_factor;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_dram_block_wpr_increment_thr_factor_set;

    
    data_index = dnx_data_dram_dram_block_write_minus_read_leaky_bucket;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_dram_dram_block_write_minus_read_leaky_bucket_set;

    
    data_index = dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_set;
    
    submodule_index = dnx_data_dram_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_dbal_define_hbm_trc_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_dbal_hbm_trc_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_wpr_increment_threshold_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_dbal_wpr_increment_threshold_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_hbm_pll_pdiv_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_dbal_hbm_pll_pdiv_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_hbm_pll_ch_mdiv_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_dbal_hbm_pll_ch_mdiv_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_hbm_pll_frefeff_info_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_dbal_hbm_pll_frefeff_info_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_hbm_pll_aux_post_enableb_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_dbal_hbm_pll_aux_post_enableb_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_hbm_pll_ch_enableb_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_dbal_hbm_pll_ch_enableb_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_hbm_pll_aux_post_diffcmos_en_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_dbal_hbm_pll_aux_post_diffcmos_en_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_dram_bist_mode_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_dbal_dram_bist_mode_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_hbmc_index_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_dbal_hbmc_index_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_hbmc_tdu_index_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dram_dbal_hbmc_tdu_index_nof_bits_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

