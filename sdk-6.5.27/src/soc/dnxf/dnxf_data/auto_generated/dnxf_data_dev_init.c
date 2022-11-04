
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



#ifdef BCM_DNXF1_SUPPORT

extern shr_error_e ramon_a0_data_dev_init_attach(
    int unit);

#endif 



static shr_error_e
dnxf_data_dev_init_time_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "time";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnxf_data_dev_init_time_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dev_init time features");

    
    submodule_data->nof_defines = _dnxf_data_dev_init_time_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dev_init time defines");

    submodule_data->defines[dnxf_data_dev_init_time_define_init_total_time].name = "init_total_time";
    submodule_data->defines[dnxf_data_dev_init_time_define_init_total_time].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_time_define_init_total_time].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_dev_init_time_define_appl_init_total_time].name = "appl_init_total_time";
    submodule_data->defines[dnxf_data_dev_init_time_define_appl_init_total_time].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_time_define_appl_init_total_time].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_dev_init_time_define_down_deviation_total].name = "down_deviation_total";
    submodule_data->defines[dnxf_data_dev_init_time_define_down_deviation_total].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_time_define_down_deviation_total].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_dev_init_time_define_up_deviation_total].name = "up_deviation_total";
    submodule_data->defines[dnxf_data_dev_init_time_define_up_deviation_total].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_time_define_up_deviation_total].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_dev_init_time_define_test_sensitivity].name = "test_sensitivity";
    submodule_data->defines[dnxf_data_dev_init_time_define_test_sensitivity].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_time_define_test_sensitivity].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnxf_data_dev_init_time_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dev_init time tables");

    
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].name = "step_time";
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].doc = "";
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].size_of_values = sizeof(dnxf_data_dev_init_time_step_time_t);
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].entry_get = dnxf_data_dev_init_time_step_time_entry_str_get;

    
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].nof_keys = 1;
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].keys[0].name = "step_id";
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].keys[0].doc = "";

    
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_dev_init_time_table_step_time].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_dev_init_time_table_step_time].nof_values, "_dnxf_data_dev_init_time_table_step_time table values");
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].values[0].name = "time";
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].values[0].type = "uint32";
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].values[0].doc = "";
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].values[0].offset = UTILEX_OFFSETOF(dnxf_data_dev_init_time_step_time_t, time);
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].values[1].name = "down_deviation";
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].values[1].type = "uint8";
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].values[1].doc = "";
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].values[1].offset = UTILEX_OFFSETOF(dnxf_data_dev_init_time_step_time_t, down_deviation);
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].values[2].name = "up_deviation";
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].values[2].type = "uint8";
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].values[2].doc = "";
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].values[2].offset = UTILEX_OFFSETOF(dnxf_data_dev_init_time_step_time_t, up_deviation);
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].values[3].name = "non_perc_up_deviation";
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].values[3].type = "uint32";
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].values[3].doc = "";
    submodule_data->tables[dnxf_data_dev_init_time_table_step_time].values[3].offset = UTILEX_OFFSETOF(dnxf_data_dev_init_time_step_time_t, non_perc_up_deviation);

    
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].name = "appl_step_time";
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].doc = "";
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].size_of_values = sizeof(dnxf_data_dev_init_time_appl_step_time_t);
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].entry_get = dnxf_data_dev_init_time_appl_step_time_entry_str_get;

    
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].nof_keys = 1;
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].keys[0].name = "step_id";
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].keys[0].doc = "";

    
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].nof_values, "_dnxf_data_dev_init_time_table_appl_step_time table values");
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].values[0].name = "time";
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].values[0].type = "uint32";
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].values[0].doc = "";
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].values[0].offset = UTILEX_OFFSETOF(dnxf_data_dev_init_time_appl_step_time_t, time);
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].values[1].name = "down_deviation";
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].values[1].type = "uint8";
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].values[1].doc = "";
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].values[1].offset = UTILEX_OFFSETOF(dnxf_data_dev_init_time_appl_step_time_t, down_deviation);
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].values[2].name = "up_deviation";
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].values[2].type = "uint8";
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].values[2].doc = "";
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].values[2].offset = UTILEX_OFFSETOF(dnxf_data_dev_init_time_appl_step_time_t, up_deviation);
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].values[3].name = "non_perc_up_deviation";
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].values[3].type = "uint32";
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].values[3].doc = "";
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_time].values[3].offset = UTILEX_OFFSETOF(dnxf_data_dev_init_time_appl_step_time_t, non_perc_up_deviation);


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_dev_init_time_feature_get(
    int unit,
    dnxf_data_dev_init_time_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_time, feature);
}


uint32
dnxf_data_dev_init_time_init_total_time_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_time, dnxf_data_dev_init_time_define_init_total_time);
}

uint32
dnxf_data_dev_init_time_appl_init_total_time_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_time, dnxf_data_dev_init_time_define_appl_init_total_time);
}

uint32
dnxf_data_dev_init_time_down_deviation_total_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_time, dnxf_data_dev_init_time_define_down_deviation_total);
}

uint32
dnxf_data_dev_init_time_up_deviation_total_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_time, dnxf_data_dev_init_time_define_up_deviation_total);
}

uint32
dnxf_data_dev_init_time_test_sensitivity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_time, dnxf_data_dev_init_time_define_test_sensitivity);
}



const dnxf_data_dev_init_time_step_time_t *
dnxf_data_dev_init_time_step_time_get(
    int unit,
    int step_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_time, dnxf_data_dev_init_time_table_step_time);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, step_id, 0);
    return (const dnxf_data_dev_init_time_step_time_t *) data;

}

const dnxf_data_dev_init_time_appl_step_time_t *
dnxf_data_dev_init_time_appl_step_time_get(
    int unit,
    int step_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_time, dnxf_data_dev_init_time_table_appl_step_time);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, step_id, 0);
    return (const dnxf_data_dev_init_time_appl_step_time_t *) data;

}


shr_error_e
dnxf_data_dev_init_time_step_time_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_dev_init_time_step_time_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_time, dnxf_data_dev_init_time_table_step_time);
    data = (const dnxf_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->time);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->down_deviation);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->up_deviation);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->non_perc_up_deviation);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_dev_init_time_appl_step_time_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_dev_init_time_appl_step_time_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_time, dnxf_data_dev_init_time_table_appl_step_time);
    data = (const dnxf_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->time);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->down_deviation);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->up_deviation);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->non_perc_up_deviation);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnxf_data_dev_init_time_step_time_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_time, dnxf_data_dev_init_time_table_step_time);

}

const dnxc_data_table_info_t *
dnxf_data_dev_init_time_appl_step_time_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_time, dnxf_data_dev_init_time_table_appl_step_time);

}






static shr_error_e
dnxf_data_dev_init_shadow_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "shadow";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnxf_data_dev_init_shadow_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dev_init shadow features");

    
    submodule_data->nof_defines = _dnxf_data_dev_init_shadow_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dev_init shadow defines");

    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_all].name = "cache_enable_all";
    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_all].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_all].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_ecc].name = "cache_enable_ecc";
    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_ecc].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_ecc].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_parity].name = "cache_enable_parity";
    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_parity].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_parity].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_specific].name = "cache_enable_specific";
    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_specific].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_specific].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_disable_specific].name = "cache_disable_specific";
    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_disable_specific].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_disable_specific].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnxf_data_dev_init_shadow_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dev_init shadow tables");

    
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].name = "uncacheable_mem";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].doc = "";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].size_of_values = sizeof(dnxf_data_dev_init_shadow_uncacheable_mem_t);
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].entry_get = dnxf_data_dev_init_shadow_uncacheable_mem_entry_str_get;

    
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].nof_keys = 1;
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].keys[0].name = "index";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].keys[0].doc = "";

    
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].nof_values, "_dnxf_data_dev_init_shadow_table_uncacheable_mem table values");
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].values[0].name = "mem";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].values[0].type = "soc_mem_t";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].values[0].doc = "";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].values[0].offset = UTILEX_OFFSETOF(dnxf_data_dev_init_shadow_uncacheable_mem_t, mem);
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].values[1].name = "is_valid";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].values[1].type = "int";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].values[1].doc = "";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].values[1].offset = UTILEX_OFFSETOF(dnxf_data_dev_init_shadow_uncacheable_mem_t, is_valid);

#ifdef BCM_ACCESS_SUPPORT

    
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_regmem].name = "uncacheable_regmem";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_regmem].doc = "";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_regmem].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_regmem].size_of_values = sizeof(dnxf_data_dev_init_shadow_uncacheable_regmem_t);
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_regmem].entry_get = dnxf_data_dev_init_shadow_uncacheable_regmem_entry_str_get;

    
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_regmem].nof_keys = 1;
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_regmem].keys[0].name = "index";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_regmem].keys[0].doc = "";

    
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_regmem].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_regmem].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_regmem].nof_values, "_dnxf_data_dev_init_shadow_table_uncacheable_regmem table values");
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_regmem].values[0].name = "regmem";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_regmem].values[0].type = "uint32";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_regmem].values[0].doc = "";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_regmem].values[0].offset = UTILEX_OFFSETOF(dnxf_data_dev_init_shadow_uncacheable_regmem_t, regmem);
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_regmem].values[1].name = "is_valid";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_regmem].values[1].type = "int";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_regmem].values[1].doc = "";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_regmem].values[1].offset = UTILEX_OFFSETOF(dnxf_data_dev_init_shadow_uncacheable_regmem_t, is_valid);

#endif 

exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_dev_init_shadow_feature_get(
    int unit,
    dnxf_data_dev_init_shadow_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_shadow, feature);
}


uint32
dnxf_data_dev_init_shadow_cache_enable_all_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_shadow, dnxf_data_dev_init_shadow_define_cache_enable_all);
}

uint32
dnxf_data_dev_init_shadow_cache_enable_ecc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_shadow, dnxf_data_dev_init_shadow_define_cache_enable_ecc);
}

uint32
dnxf_data_dev_init_shadow_cache_enable_parity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_shadow, dnxf_data_dev_init_shadow_define_cache_enable_parity);
}

uint32
dnxf_data_dev_init_shadow_cache_enable_specific_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_shadow, dnxf_data_dev_init_shadow_define_cache_enable_specific);
}

uint32
dnxf_data_dev_init_shadow_cache_disable_specific_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_shadow, dnxf_data_dev_init_shadow_define_cache_disable_specific);
}



const dnxf_data_dev_init_shadow_uncacheable_mem_t *
dnxf_data_dev_init_shadow_uncacheable_mem_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_shadow, dnxf_data_dev_init_shadow_table_uncacheable_mem);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnxf_data_dev_init_shadow_uncacheable_mem_t *) data;

}

#ifdef BCM_ACCESS_SUPPORT

const dnxf_data_dev_init_shadow_uncacheable_regmem_t *
dnxf_data_dev_init_shadow_uncacheable_regmem_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_shadow, dnxf_data_dev_init_shadow_table_uncacheable_regmem);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnxf_data_dev_init_shadow_uncacheable_regmem_t *) data;

}


#endif 

shr_error_e
dnxf_data_dev_init_shadow_uncacheable_mem_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_dev_init_shadow_uncacheable_mem_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_shadow, dnxf_data_dev_init_shadow_table_uncacheable_mem);
    data = (const dnxf_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mem);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

#ifdef BCM_ACCESS_SUPPORT

shr_error_e
dnxf_data_dev_init_shadow_uncacheable_regmem_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_dev_init_shadow_uncacheable_regmem_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_shadow, dnxf_data_dev_init_shadow_table_uncacheable_regmem);
    data = (const dnxf_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->regmem);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_valid);
            break;
    }

    SHR_FUNC_EXIT;
}


#endif 

const dnxc_data_table_info_t *
dnxf_data_dev_init_shadow_uncacheable_mem_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_shadow, dnxf_data_dev_init_shadow_table_uncacheable_mem);

}

#ifdef BCM_ACCESS_SUPPORT

const dnxc_data_table_info_t *
dnxf_data_dev_init_shadow_uncacheable_regmem_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_shadow, dnxf_data_dev_init_shadow_table_uncacheable_regmem);

}


#endif 





static shr_error_e
dnxf_data_dev_init_ha_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ha";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnxf_data_dev_init_ha_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dev_init ha features");

    
    submodule_data->nof_defines = _dnxf_data_dev_init_ha_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dev_init ha defines");

    submodule_data->defines[dnxf_data_dev_init_ha_define_warmboot_backoff_rate].name = "warmboot_backoff_rate";
    submodule_data->defines[dnxf_data_dev_init_ha_define_warmboot_backoff_rate].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_ha_define_warmboot_backoff_rate].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_ha_define_nof_guaranteed_reboots].name = "nof_guaranteed_reboots";
    submodule_data->defines[dnxf_data_dev_init_ha_define_nof_guaranteed_reboots].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_ha_define_nof_guaranteed_reboots].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_ha_define_warmboot_support].name = "warmboot_support";
    submodule_data->defines[dnxf_data_dev_init_ha_define_warmboot_support].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_ha_define_warmboot_support].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_ha_define_sw_state_max_size].name = "sw_state_max_size";
    submodule_data->defines[dnxf_data_dev_init_ha_define_sw_state_max_size].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_ha_define_sw_state_max_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_ha_define_stable_location].name = "stable_location";
    submodule_data->defines[dnxf_data_dev_init_ha_define_stable_location].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_ha_define_stable_location].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_ha_define_stable_size].name = "stable_size";
    submodule_data->defines[dnxf_data_dev_init_ha_define_stable_size].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_ha_define_stable_size].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnxf_data_dev_init_ha_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dev_init ha tables");

    
    submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].name = "stable_filename";
    submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].doc = "";
    submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].size_of_values = sizeof(dnxf_data_dev_init_ha_stable_filename_t);
    submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].entry_get = dnxf_data_dev_init_ha_stable_filename_entry_str_get;

    
    submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].nof_keys = 0;

    
    submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].nof_values, "_dnxf_data_dev_init_ha_table_stable_filename table values");
    submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].values[0].name = "val";
    submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].values[0].type = "char *";
    submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].values[0].doc = "";
    submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].values[0].offset = UTILEX_OFFSETOF(dnxf_data_dev_init_ha_stable_filename_t, val);


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_dev_init_ha_feature_get(
    int unit,
    dnxf_data_dev_init_ha_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_ha, feature);
}


uint32
dnxf_data_dev_init_ha_warmboot_backoff_rate_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_ha, dnxf_data_dev_init_ha_define_warmboot_backoff_rate);
}

uint32
dnxf_data_dev_init_ha_nof_guaranteed_reboots_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_ha, dnxf_data_dev_init_ha_define_nof_guaranteed_reboots);
}

uint32
dnxf_data_dev_init_ha_warmboot_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_ha, dnxf_data_dev_init_ha_define_warmboot_support);
}

uint32
dnxf_data_dev_init_ha_sw_state_max_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_ha, dnxf_data_dev_init_ha_define_sw_state_max_size);
}

uint32
dnxf_data_dev_init_ha_stable_location_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_ha, dnxf_data_dev_init_ha_define_stable_location);
}

uint32
dnxf_data_dev_init_ha_stable_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_ha, dnxf_data_dev_init_ha_define_stable_size);
}



const dnxf_data_dev_init_ha_stable_filename_t *
dnxf_data_dev_init_ha_stable_filename_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_ha, dnxf_data_dev_init_ha_table_stable_filename);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnxf_data_dev_init_ha_stable_filename_t *) data;

}


shr_error_e
dnxf_data_dev_init_ha_stable_filename_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_dev_init_ha_stable_filename_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_ha, dnxf_data_dev_init_ha_table_stable_filename);
    data = (const dnxf_data_dev_init_ha_stable_filename_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->val == NULL ? "" : data->val);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnxf_data_dev_init_ha_stable_filename_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_ha, dnxf_data_dev_init_ha_table_stable_filename);

}






static shr_error_e
dnxf_data_dev_init_cmc_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "cmc";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnxf_data_dev_init_cmc_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dev_init cmc features");

    
    submodule_data->nof_defines = _dnxf_data_dev_init_cmc_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dev_init cmc defines");

    submodule_data->defines[dnxf_data_dev_init_cmc_define_cmc_num].name = "cmc_num";
    submodule_data->defines[dnxf_data_dev_init_cmc_define_cmc_num].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_cmc_define_cmc_num].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_dev_init_cmc_define_num_cpu_cosq].name = "num_cpu_cosq";
    submodule_data->defines[dnxf_data_dev_init_cmc_define_num_cpu_cosq].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_cmc_define_num_cpu_cosq].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_dev_init_cmc_define_cmc_pci].name = "cmc_pci";
    submodule_data->defines[dnxf_data_dev_init_cmc_define_cmc_pci].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_cmc_define_cmc_pci].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_cmc_define_pci_cmc_num].name = "pci_cmc_num";
    submodule_data->defines[dnxf_data_dev_init_cmc_define_pci_cmc_num].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_cmc_define_pci_cmc_num].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_cmc_define_cmc_uc0].name = "cmc_uc0";
    submodule_data->defines[dnxf_data_dev_init_cmc_define_cmc_uc0].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_cmc_define_cmc_uc0].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_cmc_define_cmc_uc1].name = "cmc_uc1";
    submodule_data->defines[dnxf_data_dev_init_cmc_define_cmc_uc1].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_cmc_define_cmc_uc1].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_cmc_define_num_queues_pci].name = "num_queues_pci";
    submodule_data->defines[dnxf_data_dev_init_cmc_define_num_queues_pci].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_cmc_define_num_queues_pci].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_cmc_define_num_queues_uc0].name = "num_queues_uc0";
    submodule_data->defines[dnxf_data_dev_init_cmc_define_num_queues_uc0].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_cmc_define_num_queues_uc0].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_cmc_define_num_queues_uc1].name = "num_queues_uc1";
    submodule_data->defines[dnxf_data_dev_init_cmc_define_num_queues_uc1].doc = "";
    
    submodule_data->defines[dnxf_data_dev_init_cmc_define_num_queues_uc1].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnxf_data_dev_init_cmc_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dev_init cmc tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_dev_init_cmc_feature_get(
    int unit,
    dnxf_data_dev_init_cmc_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_cmc, feature);
}


uint32
dnxf_data_dev_init_cmc_cmc_num_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_cmc, dnxf_data_dev_init_cmc_define_cmc_num);
}

uint32
dnxf_data_dev_init_cmc_num_cpu_cosq_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_cmc, dnxf_data_dev_init_cmc_define_num_cpu_cosq);
}

uint32
dnxf_data_dev_init_cmc_cmc_pci_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_cmc, dnxf_data_dev_init_cmc_define_cmc_pci);
}

uint32
dnxf_data_dev_init_cmc_pci_cmc_num_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_cmc, dnxf_data_dev_init_cmc_define_pci_cmc_num);
}

uint32
dnxf_data_dev_init_cmc_cmc_uc0_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_cmc, dnxf_data_dev_init_cmc_define_cmc_uc0);
}

uint32
dnxf_data_dev_init_cmc_cmc_uc1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_cmc, dnxf_data_dev_init_cmc_define_cmc_uc1);
}

uint32
dnxf_data_dev_init_cmc_num_queues_pci_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_cmc, dnxf_data_dev_init_cmc_define_num_queues_pci);
}

uint32
dnxf_data_dev_init_cmc_num_queues_uc0_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_cmc, dnxf_data_dev_init_cmc_define_num_queues_uc0);
}

uint32
dnxf_data_dev_init_cmc_num_queues_uc1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_cmc, dnxf_data_dev_init_cmc_define_num_queues_uc1);
}







shr_error_e
dnxf_data_dev_init_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "dev_init";
    module_data->nof_submodules = _dnxf_data_dev_init_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data dev_init submodules");

    
    SHR_IF_ERR_EXIT(dnxf_data_dev_init_time_init(unit, &module_data->submodules[dnxf_data_dev_init_submodule_time]));
    SHR_IF_ERR_EXIT(dnxf_data_dev_init_shadow_init(unit, &module_data->submodules[dnxf_data_dev_init_submodule_shadow]));
    SHR_IF_ERR_EXIT(dnxf_data_dev_init_ha_init(unit, &module_data->submodules[dnxf_data_dev_init_submodule_ha]));
    SHR_IF_ERR_EXIT(dnxf_data_dev_init_cmc_init(unit, &module_data->submodules[dnxf_data_dev_init_submodule_cmc]));
    
#ifdef BCM_DNXF1_SUPPORT

    if (dnxc_data_mgmt_is_ramon(unit))
    {
        SHR_IF_ERR_EXIT(ramon_a0_data_dev_init_attach(unit));
    }
    else

#endif 
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

