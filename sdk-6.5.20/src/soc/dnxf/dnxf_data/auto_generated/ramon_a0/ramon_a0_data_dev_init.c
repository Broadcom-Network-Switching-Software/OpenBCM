

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INIT

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_dev_init.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_property.h>
#include <bcm_int/dnxf/init.h>
#include <../src/appl/reference/dnxf/appl_ref_init_deinit.h>








static shr_error_e
ramon_a0_dnxf_data_dev_init_time_init_total_thresh_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_time;
    int define_index = dnxf_data_dev_init_time_define_init_total_thresh;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 13912484;

    
    define->data = 13912484;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_dev_init_time_appl_init_total_thresh_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_time;
    int define_index = dnxf_data_dev_init_time_define_appl_init_total_thresh;
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
ramon_a0_dnxf_data_dev_init_time_step_thresh_set(
    int unit)
{
    int step_id_index;
    dnxf_data_dev_init_time_step_thresh_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_time;
    int table_index = dnxf_data_dev_init_time_table_step_thresh;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNXF_INIT_STEP_COUNT;
    table->info_get.key_size[0] = DNXF_INIT_STEP_COUNT;

    
    table->values[0].default_val = "1000000";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_dev_init_time_step_thresh_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_dev_init_time_table_step_thresh");

    
    default_data = (dnxf_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->value = 1000000;
    
    for (step_id_index = 0; step_id_index < table->keys[0].size; step_id_index++)
    {
        data = (dnxf_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, step_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNXF_INIT_STEP_SOC < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_SOC, 0);
        data->value = 2000000;
    }
    if (DNXF_INIT_STEP_PORT < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_PORT, 0);
        data->value = 11000000;
    }
    if (DNXF_INIT_STEP_FABRIC < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNXF_INIT_STEP_FABRIC, 0);
        data->value = 1900000;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_dev_init_time_appl_step_thresh_set(
    int unit)
{
    int step_id_index;
    dnxf_data_dev_init_time_appl_step_thresh_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_dev_init;
    int submodule_index = dnxf_data_dev_init_submodule_time;
    int table_index = dnxf_data_dev_init_time_table_appl_step_thresh;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNXF_INIT_STEP_COUNT;
    table->info_get.key_size[0] = DNXF_INIT_STEP_COUNT;

    
    table->values[0].default_val = "APPL_DNXF_INIT_STEP_TIME_THRESH_DEFAULT";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_dev_init_time_appl_step_thresh_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_dev_init_time_table_appl_step_thresh");

    
    default_data = (dnxf_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->value = APPL_DNXF_INIT_STEP_TIME_THRESH_DEFAULT;
    
    for (step_id_index = 0; step_id_index < table->keys[0].size; step_id_index++)
    {
        data = (dnxf_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, step_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (APPL_DNXF_STEP_SDK < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNXF_STEP_SDK, 0);
        data->value = 13899822;
    }
    if (APPL_DNXF_STEP_APPS < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNXF_STEP_APPS, 0);
        data->value = 60000;
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
        "Shadow all memory or not.\n"
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
        "Shadow all memory with ecc field or not.\n"
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
        "Shadow all memory with parity field or not.\n"
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
        "Enable memory shadow for specific table.\n"
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
        "Disable memory shadow for specific table.\n"
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

    
    table->keys[0].size = 12;
    table->info_get.key_size[0] = 12;

    
    table->values[0].default_val = "INVALIDm";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_dev_init_shadow_uncacheable_mem_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_dev_init_shadow_table_uncacheable_mem");

    
    default_data = (dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->mem = INVALIDm;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->mem = QRH_MCLBTm;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->mem = MCT_MCTm;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->mem = FSRD_FSRD_WL_EXT_MEMm;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->mem = FSRD_FSRD_PROM_MEMm;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->mem = SER_ACC_TYPE_MAPm;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->mem = SER_MEMORYm;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->mem = SER_RESULT_0m;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->mem = SER_RESULT_1m;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->mem = SER_RESULT_DATA_0m;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->mem = SER_RESULT_DATA_1m;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->mem = SER_RESULT_EXPECTED_0m;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->mem = SER_RESULT_EXPECTED_1m;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


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
        "Specify if WarmBoot is supported\n"
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
        "Configures the size of memory to be pre allocated for sw state.\n"
        "The valid range is 0-1000000000 (i.e up to 1TB).\n"
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
        "Specify the stable cache option for Warm Boot operations.\n"
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
        "Specify the stable cache size in bytes used for Warm boot operations.\n"
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
        "file system will be used to save the stable cache data with this filename\n"
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

    
    data_index = dnxf_data_dev_init_time_define_init_total_thresh;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_time_init_total_thresh_set;
    data_index = dnxf_data_dev_init_time_define_appl_init_total_thresh;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_dev_init_time_appl_init_total_thresh_set;

    

    
    data_index = dnxf_data_dev_init_time_table_step_thresh;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_dev_init_time_step_thresh_set;
    data_index = dnxf_data_dev_init_time_table_appl_step_thresh;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_dev_init_time_appl_step_thresh_set;
    
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
    
    submodule_index = dnxf_data_dev_init_submodule_ha;
    submodule = &module->submodules[submodule_index];

    
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

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

