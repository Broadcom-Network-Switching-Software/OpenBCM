
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

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dev_init.h>
#include <bcm_int/dnx/spb/spb.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/init/init_custom_funcs.h>
#include <shared/utilex/utilex_time_analyzer.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fc.h>
#include <appl/reference/dnx/appl_ref_init_deinit.h>








static shr_error_e
q2a_b0_dnx_data_dev_init_time_init_total_time_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_time;
    int define_index = dnx_data_dev_init_time_define_init_total_time;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10849986;

    
    define->data = 10849986;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_b0_dnx_data_dev_init_time_appl_init_total_time_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_time;
    int define_index = dnx_data_dev_init_time_define_appl_init_total_time;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 19380000;

    
    define->data = 19380000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_b0_dnx_data_dev_init_time_down_deviation_total_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_time;
    int define_index = dnx_data_dev_init_time_define_down_deviation_total;
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
q2a_b0_dnx_data_dev_init_time_up_deviation_total_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_time;
    int define_index = dnx_data_dev_init_time_define_up_deviation_total;
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
q2a_b0_dnx_data_dev_init_time_test_sensitivity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_time;
    int define_index = dnx_data_dev_init_time_define_test_sensitivity;
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
q2a_b0_dnx_data_dev_init_time_step_time_set(
    int unit)
{
    int step_id_index;
    dnx_data_dev_init_time_step_time_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_time;
    int table_index = dnx_data_dev_init_time_table_step_time;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_INIT_STEP_COUNT;
    table->info_get.key_size[0] = DNX_INIT_STEP_COUNT;

    
    table->values[0].default_val = "DATA(dev_init, time, test_sensitivity)";
    table->values[1].default_val = "10";
    table->values[2].default_val = "10";
    table->values[3].default_val = "10000";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dev_init_time_step_time_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dev_init_time_table_step_time");

    
    default_data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->time = dnx_data_dev_init.time.test_sensitivity_get(unit);
    default_data->down_deviation = 10;
    default_data->up_deviation = 10;
    default_data->non_perc_up_deviation = 10000;
    
    for (step_id_index = 0; step_id_index < table->keys[0].size; step_id_index++)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, step_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_INIT_STEP_TUNE < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_TUNE, 0);
        data->time = 360469;
    }
    if (DNX_INIT_STEP_TM_MODULES < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_TM_MODULES, 0);
        data->time = 198949;
    }
    if (DNX_INIT_STEP_ALGO_TUNNEL < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_ALGO_TUNNEL, 0);
        data->time = 445766;
    }
    if (DNX_INIT_MDB < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_MDB, 0);
        data->time = 172728;
    }
    if (DNX_INIT_STEP_PEMLA < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_PEMLA, 0);
        data->time = 202392;
    }
    if (DNX_INIT_STEP_MEM_DEFAULTS < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_MEM_DEFAULTS, 0);
        data->time = 133816;
    }
    if (DNX_INIT_STEP_SOFT_RESET < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_SOFT_RESET, 0);
        data->time = 125926;
    }
    if (DNX_INIT_STEP_PP_MODULES < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_PP_MODULES, 0);
        data->time = 194881;
    }
    if (DNX_INIT_STEP_AUX_ACCESS < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_AUX_ACCESS, 0);
        data->time = 144685;
        data->down_deviation = 30;
    }
    if (DNX_INIT_STEP_ACCESS < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_ACCESS, 0);
        data->time = 128013;
    }
    if (DNX_INIT_STEP_PRE_SOFT_INIT < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_PRE_SOFT_INIT, 0);
        data->time = 3152400;
    }
    if (DNX_INIT_STEP_APPL_PROPERTIES < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_APPL_PROPERTIES, 0);
        data->time = 742170;
    }
    if (DNX_INIT_STEP_COMMON_MODULES < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_COMMON_MODULES, 0);
        data->time = 336844;
    }
    if (DNX_INIT_STEP_DYN_PORT_ADD < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_DYN_PORT_ADD, 0);
        data->time = 654468;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_b0_dnx_data_dev_init_time_appl_step_time_set(
    int unit)
{
    int step_id_index;
    dnx_data_dev_init_time_appl_step_time_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_time;
    int table_index = dnx_data_dev_init_time_table_appl_step_time;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = APPL_DNX_INIT_STEP_COUNT;
    table->info_get.key_size[0] = APPL_DNX_INIT_STEP_COUNT;

    
    table->values[0].default_val = "DATA(dev_init, time, test_sensitivity)";
    table->values[1].default_val = "10";
    table->values[2].default_val = "10";
    table->values[3].default_val = "10000";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dev_init_time_appl_step_time_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dev_init_time_table_appl_step_time");

    
    default_data = (dnx_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->time = dnx_data_dev_init.time.test_sensitivity_get(unit);
    default_data->down_deviation = 10;
    default_data->up_deviation = 10;
    default_data->non_perc_up_deviation = 10000;
    
    for (step_id_index = 0; step_id_index < table->keys[0].size; step_id_index++)
    {
        data = (dnx_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, step_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (APPL_DNX_STEP_VLAN < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_VLAN, 0);
        data->time = 101320;
    }
    if (APPL_DNX_STEP_SDK < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_SDK, 0);
        data->time = 3297413;
    }
    if (APPL_DNX_STEP_APPS < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_APPS, 0);
        data->time = 623953;
    }
    if (APPL_DNX_STEP_ITMH_PPH < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_ITMH_PPH, 0);
        data->time = 241449;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
q2a_b0_dnx_data_dev_init_general_network_header_termination_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_network_header_termination;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = dnx_data_headers.system_headers.system_headers_mode_get(unit)==dnx_data_headers.system_headers.system_headers_mode_jericho2_get(unit)?1:0;

    
    feature->data = dnx_data_headers.system_headers.system_headers_mode_get(unit)==dnx_data_headers.system_headers.system_headers_mode_jericho2_get(unit)?1:0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}





shr_error_e
q2a_b0_data_dev_init_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_dev_init_submodule_time;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dev_init_time_define_init_total_time;
    define = &submodule->defines[data_index];
    define->set = q2a_b0_dnx_data_dev_init_time_init_total_time_set;
    data_index = dnx_data_dev_init_time_define_appl_init_total_time;
    define = &submodule->defines[data_index];
    define->set = q2a_b0_dnx_data_dev_init_time_appl_init_total_time_set;
    data_index = dnx_data_dev_init_time_define_down_deviation_total;
    define = &submodule->defines[data_index];
    define->set = q2a_b0_dnx_data_dev_init_time_down_deviation_total_set;
    data_index = dnx_data_dev_init_time_define_up_deviation_total;
    define = &submodule->defines[data_index];
    define->set = q2a_b0_dnx_data_dev_init_time_up_deviation_total_set;
    data_index = dnx_data_dev_init_time_define_test_sensitivity;
    define = &submodule->defines[data_index];
    define->set = q2a_b0_dnx_data_dev_init_time_test_sensitivity_set;

    

    
    data_index = dnx_data_dev_init_time_table_step_time;
    table = &submodule->tables[data_index];
    table->set = q2a_b0_dnx_data_dev_init_time_step_time_set;
    data_index = dnx_data_dev_init_time_table_appl_step_time;
    table = &submodule->tables[data_index];
    table->set = q2a_b0_dnx_data_dev_init_time_appl_step_time_set;
    
    submodule_index = dnx_data_dev_init_submodule_general;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_dev_init_general_network_header_termination;
    feature = &submodule->features[data_index];
    feature->set = q2a_b0_dnx_data_dev_init_general_network_header_termination_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

