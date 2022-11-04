
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INIT

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_dev_init.h>
#include <bcm_int/dnxf/init.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <../src/appl/reference/dnxf/appl_ref_init_deinit.h>








static shr_error_e
ramon_a0_dnxf_data_dev_init_time_init_total_time_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_time;
    int define_index = dnxf_data_dev_init_time_define_init_total_time;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 14560000;

    
    define->data = 14560000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_dev_init_time_appl_init_total_time_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_time;
    int define_index = dnxf_data_dev_init_time_define_appl_init_total_time;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20000000;

    
    define->data = 20000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_dev_init_time_down_deviation_total_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_time;
    int define_index = dnxf_data_dev_init_time_define_down_deviation_total;
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
ramon_a0_dnxf_data_dev_init_time_up_deviation_total_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_time;
    int define_index = dnxf_data_dev_init_time_define_up_deviation_total;
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
ramon_a0_dnxf_data_dev_init_time_test_sensitivity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_time;
    int define_index = dnxf_data_dev_init_time_define_test_sensitivity;
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
ramon_a0_dnxf_data_dev_init_time_step_time_set(
    int unit)
{
    int step_id_index;
    dnxf_data_dev_init_time_step_time_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_time;
    int table_index = dnxf_data_dev_init_time_table_step_time;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNXF_INIT_STEP_COUNT;
    table->info_get.key_size[0] = DNXF_INIT_STEP_COUNT;

    
    table->values[0].default_val = "DATA(dev_init, time, test_sensitivity)";
    table->values[1].default_val = "10";
    table->values[2].default_val = "10";
    table->values[3].default_val = "10000";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_dev_init_time_step_time_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_dev_init_time_table_step_time");

    
    default_data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->time = dnxf_data_dev_init.time.test_sensitivity_get(unit);
    default_data->down_deviation = 10;
    default_data->up_deviation = 10;
    default_data->non_perc_up_deviation = 10000;
    
    for (step_id_index = 0; step_id_index < table->keys[0].size; step_id_index++)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, step_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNXF_INIT_STEP_MARK_NOT_INITED < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_MARK_NOT_INITED, 0);
        data->time = 20;
        data->down_deviation = 95;
        data->up_deviation = 200;
    }
    if (DNXF_INIT_STEP_DNX_DATA < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_DNX_DATA, 0);
        data->time = 63674;
    }
    if (DNXF_INIT_STEP_HA < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_HA, 0);
        data->time = 45;
        data->down_deviation = 80;
        data->up_deviation = 80;
    }
    if (DNXF_INIT_STEP_SW_STATE < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_SW_STATE, 0);
        data->time = 2396;
    }
    if (DNXF_INIT_STEP_SOC_INFO_LEGACY < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_SOC_INFO_LEGACY, 0);
        data->time = 298;
    }
    if (DNXF_INIT_STEP_DEVICE_RESET < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_DEVICE_RESET, 0);
        data->time = 132596;
    }
    if (DNXF_INIT_STEP_LINK_MAPPING < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_LINK_MAPPING, 0);
        data->time = 95;
        data->down_deviation = 80;
        data->up_deviation = 80;
    }
    if (DNXF_INIT_STEP_BLOCKS_RESET < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_BLOCKS_RESET, 0);
        data->time = 1050000;
    }
    if (DNXF_INIT_STEP_COUNTER_ATTACH < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_COUNTER_ATTACH, 0);
        data->time = 80;
        data->down_deviation = 80;
        data->up_deviation = 80;
    }
    if (DNXF_INIT_STEP_INTERRUPTS < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_INTERRUPTS, 0);
        data->time = 11356;
    }
    if (DNXF_INIT_STEP_SOC_INIT_DONE < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_SOC_INIT_DONE, 0);
        data->time = 48;
        data->down_deviation = 80;
        data->up_deviation = 80;
    }
    if (DNXF_INIT_STEP_PORT < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_PORT, 0);
        data->time = 9100000;
    }
    if (DNXF_INIT_STEP_SYNCE < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_SYNCE, 0);
        data->time = 20;
        data->down_deviation = 95;
        data->up_deviation = 200;
    }
    if (DNXF_INIT_STEP_LINKSCAN < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_LINKSCAN, 0);
        data->time = 251;
        data->down_deviation = 20;
        data->up_deviation = 20;
    }
    if (DNXF_INIT_STEP_STAT < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_STAT, 0);
        data->time = 411;
        data->down_deviation = 20;
        data->up_deviation = 200;
    }
    if (DNXF_INIT_STEP_STACK < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_STACK, 0);
        data->time = 40;
        data->down_deviation = 80;
        data->up_deviation = 80;
    }
    if (DNXF_INIT_STEP_MULTICAST < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_MULTICAST, 0);
        data->time = 38962;
    }
    if (DNXF_INIT_STEP_FABRIC < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_FABRIC, 0);
        data->time = 973821;
    }
    if (DNXF_INIT_STEP_RX < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_RX, 0);
        data->time = 266;
    }
    if (DNXF_INIT_STEP_TUNE < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_TUNE, 0);
        data->time = 11692;
    }
    if (DNXF_INIT_STEP_INIT_DONE < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_INIT_DONE, 0);
        data->time = 46;
        data->down_deviation = 95;
        data->up_deviation = 100;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_dev_init_time_appl_step_time_set(
    int unit)
{
    int step_id_index;
    dnxf_data_dev_init_time_appl_step_time_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_time;
    int table_index = dnxf_data_dev_init_time_table_appl_step_time;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNXF_INIT_STEP_COUNT;
    table->info_get.key_size[0] = DNXF_INIT_STEP_COUNT;

    
    table->values[0].default_val = "DATA(dev_init, time, test_sensitivity)";
    table->values[1].default_val = "10";
    table->values[2].default_val = "10";
    table->values[3].default_val = "10000";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_dev_init_time_appl_step_time_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_dev_init_time_table_appl_step_time");

    
    default_data = (dnxf_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->time = dnxf_data_dev_init.time.test_sensitivity_get(unit);
    default_data->down_deviation = 10;
    default_data->up_deviation = 10;
    default_data->non_perc_up_deviation = 10000;
    
    for (step_id_index = 0; step_id_index < table->keys[0].size; step_id_index++)
    {
        data = (dnxf_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, step_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (APPL_DNXF_STEP_SDK < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNXF_STEP_SDK, 0);
        data->time = 11370000;
    }
    if (APPL_DNXF_STEP_APPS < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNXF_STEP_APPS, 0);
        data->time = 60000;
    }
    if (APPL_DNXF_STEP_DEVICE_PARAMS < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNXF_STEP_DEVICE_PARAMS, 0);
        data->time = 6250;
        data->down_deviation = 95;
        data->up_deviation = 100;
    }
    if (APPL_DNXF_STEP_STK_INIT < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNXF_STEP_STK_INIT, 0);
        data->time = 1200;
        data->down_deviation = 95;
        data->up_deviation = 100;
    }
    if (APPL_DNXF_STEP_RX < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNXF_STEP_RX, 0);
        data->time = 34;
        data->down_deviation = 95;
        data->up_deviation = 100;
    }
    if (APPL_DNXF_STEP_INIT_DONE < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNXF_STEP_INIT_DONE, 0);
        data->time = 37;
        data->down_deviation = 95;
        data->up_deviation = 100;
    }
    if (APPL_DNXF_LINKS_ISOLATE < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNXF_LINKS_ISOLATE, 0);
        data->time = 20;
        data->down_deviation = 95;
        data->up_deviation = 200;
    }
    if (APPL_DNXF_STEP_INTR_INIT < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNXF_STEP_INTR_INIT, 0);
        data->time = 7600;
    }
    if (APPL_DNXF_STEP_SHELL_THREADS < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNXF_STEP_SHELL_THREADS, 0);
        data->time = 20;
        data->down_deviation = 95;
        data->up_deviation = 200;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
ramon_a0_dnxf_data_dev_init_shadow_cache_enable_all_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_shadow;
    int define_index = dnxf_data_dev_init_shadow_define_cache_enable_all;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_MEM_CACHE_ENABLE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "all";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_dev_init_shadow_cache_enable_ecc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_shadow;
    int define_index = dnxf_data_dev_init_shadow_define_cache_enable_ecc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_MEM_CACHE_ENABLE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "ecc";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_dev_init_shadow_cache_enable_parity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_shadow;
    int define_index = dnxf_data_dev_init_shadow_define_cache_enable_parity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_MEM_CACHE_ENABLE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "parity";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_dev_init_shadow_cache_enable_specific_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_shadow;
    int define_index = dnxf_data_dev_init_shadow_define_cache_enable_specific;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_MEM_CACHE_ENABLE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "specific";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_dev_init_shadow_cache_disable_specific_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_shadow;
    int define_index = dnxf_data_dev_init_shadow_define_cache_disable_specific;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_MEM_NOCACHE;
    define->property.doc = 
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
ramon_a0_dnxf_data_dev_init_shadow_uncacheable_mem_set(
    int unit)
{
    int index_index;
    dnxf_data_dev_init_shadow_uncacheable_mem_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_shadow;
    int table_index = dnxf_data_dev_init_shadow_table_uncacheable_mem;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 5;
    table->info_get.key_size[0] = 5;

    
    table->values[0].default_val = "INVALIDm";
    table->values[1].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_dev_init_shadow_uncacheable_mem_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_dev_init_shadow_table_uncacheable_mem");

    
    default_data = (dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->mem = INVALIDm;
    default_data->is_valid = 1;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->mem = RTP_SLSCTm;
        data->is_valid = dnxf_data_fabric.topology.load_balancing_mode_get(unit) == soc_dnxf_load_balancing_mode_balanced_input ? TRUE : FALSE;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->mem = RTP_MULTI_CAST_TABLE_UPDATEm;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->mem = QRH_MCLBTm;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->mem = MCT_MCTm;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->mem = FSRD_FSRD_PROM_MEMm;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}

#ifdef BCM_ACCESS_SUPPORT


static shr_error_e
ramon_a0_dnxf_data_dev_init_shadow_uncacheable_regmem_set(
    int unit)
{
    int index_index;
    dnxf_data_dev_init_shadow_uncacheable_regmem_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_shadow;
    int table_index = dnxf_data_dev_init_shadow_table_uncacheable_regmem;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 0;
    table->info_get.key_size[0] = 0;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_dev_init_shadow_uncacheable_regmem_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_dev_init_shadow_table_uncacheable_regmem");

    
    default_data = (dnxf_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->regmem = -1;
    default_data->is_valid = 1;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnxf_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


#endif 





static shr_error_e
ramon_a0_dnxf_data_dev_init_ha_warmboot_backoff_rate_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_ha;
    int define_index = dnxf_data_dev_init_ha_define_warmboot_backoff_rate;
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
ramon_a0_dnxf_data_dev_init_ha_nof_guaranteed_reboots_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_ha;
    int define_index = dnxf_data_dev_init_ha_define_nof_guaranteed_reboots;
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
ramon_a0_dnxf_data_dev_init_ha_warmboot_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_ha;
    int define_index = dnxf_data_dev_init_ha_define_warmboot_support;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_WARMBOOT_SUPPORT;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "on";
    define->property.mapping[0].val = 1;
    define->property.mapping[1].name = "off";
    define->property.mapping[1].val = 0;
    define->property.mapping[1].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_dev_init_ha_sw_state_max_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_ha;
    int define_index = dnxf_data_dev_init_ha_define_sw_state_max_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_SW_STATE_MAX_SIZE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 1000000000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_dev_init_ha_stable_location_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_ha;
    int define_index = dnxf_data_dev_init_ha_define_stable_location;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_STABLE_LOCATION;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 5;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_dev_init_ha_stable_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_ha;
    int define_index = dnxf_data_dev_init_ha_define_stable_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_STABLE_SIZE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 1000000000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
ramon_a0_dnxf_data_dev_init_ha_stable_filename_set(
    int unit)
{
    dnxf_data_dev_init_ha_stable_filename_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_ha;
    int table_index = dnxf_data_dev_init_ha_table_stable_filename;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "NULL";
    
    DNXC_DATA_ALLOC(table->data, dnxf_data_dev_init_ha_stable_filename_t, (1 + 1  ), "data of dnxf_data_dev_init_ha_table_stable_filename");

    
    default_data = (dnxf_data_dev_init_ha_stable_filename_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = NULL;
    
    data = (dnxf_data_dev_init_ha_stable_filename_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[0].property.name = spn_STABLE_FILENAME;
    table->values[0].property.doc =
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_str;
    table->values[0].property.method_str = "str";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnxf_data_dev_init_ha_stable_filename_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, -1, &data->val));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
ramon_a0_dnxf_data_dev_init_cmc_cmc_num_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_cmc;
    int define_index = dnxf_data_dev_init_cmc_define_cmc_num;
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
ramon_a0_dnxf_data_dev_init_cmc_num_cpu_cosq_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_cmc;
    int define_index = dnxf_data_dev_init_cmc_define_num_cpu_cosq;
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
ramon_a0_dnxf_data_dev_init_cmc_cmc_pci_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_cmc;
    int define_index = dnxf_data_dev_init_cmc_define_cmc_pci;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CMC;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "pci";
    define->property.range_min = 0;
    define->property.range_max = 1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_dev_init_cmc_pci_cmc_num_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_cmc;
    int define_index = dnxf_data_dev_init_cmc_define_pci_cmc_num;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_PCI_CMCS_NUM;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 1;
    define->property.range_max = 2;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_dev_init_cmc_cmc_uc0_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_cmc;
    int define_index = dnxf_data_dev_init_cmc_define_cmc_uc0;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CMC;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "uc0";
    define->property.range_min = 0;
    define->property.range_max = 1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_dev_init_cmc_cmc_uc1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_cmc;
    int define_index = dnxf_data_dev_init_cmc_define_cmc_uc1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CMC;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "uc1";
    define->property.range_min = 0;
    define->property.range_max = 1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_dev_init_cmc_num_queues_pci_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_cmc;
    int define_index = dnxf_data_dev_init_cmc_define_num_queues_pci;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_NUM_QUEUES;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "pci";
    define->property.range_min = 0;
    define->property.range_max = 64;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_dev_init_cmc_num_queues_uc0_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_cmc;
    int define_index = dnxf_data_dev_init_cmc_define_num_queues_uc0;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_NUM_QUEUES;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "uc0";
    define->property.range_min = 0;
    define->property.range_max = 64;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_dev_init_cmc_num_queues_uc1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_cmc;
    int define_index = dnxf_data_dev_init_cmc_define_num_queues_uc1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_NUM_QUEUES;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "uc1";
    define->property.range_min = 0;
    define->property.range_max = 64;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}




shr_error_e
ramon_a0_data_dev_init_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnxf_data_dev_init_submodule_time;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_dev_init_time_define_init_total_time;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_time_init_total_time_set;
    data_index = dnxf_data_dev_init_time_define_appl_init_total_time;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_time_appl_init_total_time_set;
    data_index = dnxf_data_dev_init_time_define_down_deviation_total;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_time_down_deviation_total_set;
    data_index = dnxf_data_dev_init_time_define_up_deviation_total;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_time_up_deviation_total_set;
    data_index = dnxf_data_dev_init_time_define_test_sensitivity;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_time_test_sensitivity_set;

    

    
    data_index = dnxf_data_dev_init_time_table_step_time;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_dev_init_time_step_time_set;
    data_index = dnxf_data_dev_init_time_table_appl_step_time;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_dev_init_time_appl_step_time_set;
    
    submodule_index = dnxf_data_dev_init_submodule_shadow;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_dev_init_shadow_define_cache_enable_all;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_shadow_cache_enable_all_set;
    data_index = dnxf_data_dev_init_shadow_define_cache_enable_ecc;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_shadow_cache_enable_ecc_set;
    data_index = dnxf_data_dev_init_shadow_define_cache_enable_parity;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_shadow_cache_enable_parity_set;
    data_index = dnxf_data_dev_init_shadow_define_cache_enable_specific;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_shadow_cache_enable_specific_set;
    data_index = dnxf_data_dev_init_shadow_define_cache_disable_specific;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_shadow_cache_disable_specific_set;

    

    
    data_index = dnxf_data_dev_init_shadow_table_uncacheable_mem;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_dev_init_shadow_uncacheable_mem_set;
#ifdef BCM_ACCESS_SUPPORT

    data_index = dnxf_data_dev_init_shadow_table_uncacheable_regmem;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_dev_init_shadow_uncacheable_regmem_set;

#endif 
    
    submodule_index = dnxf_data_dev_init_submodule_ha;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_dev_init_ha_define_warmboot_backoff_rate;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_ha_warmboot_backoff_rate_set;
    data_index = dnxf_data_dev_init_ha_define_nof_guaranteed_reboots;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_ha_nof_guaranteed_reboots_set;
    data_index = dnxf_data_dev_init_ha_define_warmboot_support;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_ha_warmboot_support_set;
    data_index = dnxf_data_dev_init_ha_define_sw_state_max_size;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_ha_sw_state_max_size_set;
    data_index = dnxf_data_dev_init_ha_define_stable_location;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_ha_stable_location_set;
    data_index = dnxf_data_dev_init_ha_define_stable_size;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_ha_stable_size_set;

    

    
    data_index = dnxf_data_dev_init_ha_table_stable_filename;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_dev_init_ha_stable_filename_set;
    
    submodule_index = dnxf_data_dev_init_submodule_cmc;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_dev_init_cmc_define_cmc_num;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_cmc_cmc_num_set;
    data_index = dnxf_data_dev_init_cmc_define_num_cpu_cosq;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_cmc_num_cpu_cosq_set;
    data_index = dnxf_data_dev_init_cmc_define_cmc_pci;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_cmc_cmc_pci_set;
    data_index = dnxf_data_dev_init_cmc_define_pci_cmc_num;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_cmc_pci_cmc_num_set;
    data_index = dnxf_data_dev_init_cmc_define_cmc_uc0;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_cmc_cmc_uc0_set;
    data_index = dnxf_data_dev_init_cmc_define_cmc_uc1;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_cmc_cmc_uc1_set;
    data_index = dnxf_data_dev_init_cmc_define_num_queues_pci;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_cmc_num_queues_pci_set;
    data_index = dnxf_data_dev_init_cmc_define_num_queues_uc0;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_cmc_num_queues_uc0_set;
    data_index = dnxf_data_dev_init_cmc_define_num_queues_uc1;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_cmc_num_queues_uc1_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

