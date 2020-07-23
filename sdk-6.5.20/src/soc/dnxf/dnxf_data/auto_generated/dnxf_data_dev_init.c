

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



extern shr_error_e ramon_a0_data_dev_init_attach(
    int unit);



static shr_error_e
dnxf_data_dev_init_time_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "time";
    submodule_data->doc = "time relevant for init";
    
    submodule_data->nof_features = _dnxf_data_dev_init_time_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dev_init time features");

    
    submodule_data->nof_defines = _dnxf_data_dev_init_time_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dev_init time defines");

    submodule_data->defines[dnxf_data_dev_init_time_define_init_total_thresh].name = "init_total_thresh";
    submodule_data->defines[dnxf_data_dev_init_time_define_init_total_thresh].doc = "Default time threshold (in microseconds) for the overall time that the BCM Init takes";
    
    submodule_data->defines[dnxf_data_dev_init_time_define_init_total_thresh].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_dev_init_time_define_appl_init_total_thresh].name = "appl_init_total_thresh";
    submodule_data->defines[dnxf_data_dev_init_time_define_appl_init_total_thresh].doc = "Default time threshold (in microseconds) for the overall time that the APPL Init takes";
    
    submodule_data->defines[dnxf_data_dev_init_time_define_appl_init_total_thresh].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnxf_data_dev_init_time_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dev_init time tables");

    
    submodule_data->tables[dnxf_data_dev_init_time_table_step_thresh].name = "step_thresh";
    submodule_data->tables[dnxf_data_dev_init_time_table_step_thresh].doc = "Time threshold (in microseconds) for the time each step is taking during BCM init";
    submodule_data->tables[dnxf_data_dev_init_time_table_step_thresh].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_dev_init_time_table_step_thresh].size_of_values = sizeof(dnxf_data_dev_init_time_step_thresh_t);
    submodule_data->tables[dnxf_data_dev_init_time_table_step_thresh].entry_get = dnxf_data_dev_init_time_step_thresh_entry_str_get;

    
    submodule_data->tables[dnxf_data_dev_init_time_table_step_thresh].nof_keys = 1;
    submodule_data->tables[dnxf_data_dev_init_time_table_step_thresh].keys[0].name = "step_id";
    submodule_data->tables[dnxf_data_dev_init_time_table_step_thresh].keys[0].doc = "IDs for the different steps and sub-steps of the Init.";

    
    submodule_data->tables[dnxf_data_dev_init_time_table_step_thresh].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_dev_init_time_table_step_thresh].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_dev_init_time_table_step_thresh].nof_values, "_dnxf_data_dev_init_time_table_step_thresh table values");
    submodule_data->tables[dnxf_data_dev_init_time_table_step_thresh].values[0].name = "value";
    submodule_data->tables[dnxf_data_dev_init_time_table_step_thresh].values[0].type = "uint32";
    submodule_data->tables[dnxf_data_dev_init_time_table_step_thresh].values[0].doc = "Time threshold in microseconds. The default value was chosen because the majority of the steps are below it. For steps that are expected to be above the default, a specific threshold can be set";
    submodule_data->tables[dnxf_data_dev_init_time_table_step_thresh].values[0].offset = UTILEX_OFFSETOF(dnxf_data_dev_init_time_step_thresh_t, value);

    
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_thresh].name = "appl_step_thresh";
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_thresh].doc = "Time threshold (in microseconds) for the time each step is taking during APPL init";
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_thresh].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_thresh].size_of_values = sizeof(dnxf_data_dev_init_time_appl_step_thresh_t);
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_thresh].entry_get = dnxf_data_dev_init_time_appl_step_thresh_entry_str_get;

    
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_thresh].nof_keys = 1;
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_thresh].keys[0].name = "step_id";
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_thresh].keys[0].doc = "IDs for the different steps and sub-steps of the Init.";

    
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_thresh].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_thresh].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_thresh].nof_values, "_dnxf_data_dev_init_time_table_appl_step_thresh table values");
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_thresh].values[0].name = "value";
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_thresh].values[0].type = "uint32";
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_thresh].values[0].doc = "Time threshold in microseconds. The default value was chosen because the majority of the steps are below it. For steps that are expected to be above the default, a specific threshold can be set";
    submodule_data->tables[dnxf_data_dev_init_time_table_appl_step_thresh].values[0].offset = UTILEX_OFFSETOF(dnxf_data_dev_init_time_appl_step_thresh_t, value);


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
dnxf_data_dev_init_time_init_total_thresh_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_time, dnxf_data_dev_init_time_define_init_total_thresh);
}

uint32
dnxf_data_dev_init_time_appl_init_total_thresh_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_time, dnxf_data_dev_init_time_define_appl_init_total_thresh);
}



const dnxf_data_dev_init_time_step_thresh_t *
dnxf_data_dev_init_time_step_thresh_get(
    int unit,
    int step_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_time, dnxf_data_dev_init_time_table_step_thresh);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, step_id, 0);
    return (const dnxf_data_dev_init_time_step_thresh_t *) data;

}

const dnxf_data_dev_init_time_appl_step_thresh_t *
dnxf_data_dev_init_time_appl_step_thresh_get(
    int unit,
    int step_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_time, dnxf_data_dev_init_time_table_appl_step_thresh);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, step_id, 0);
    return (const dnxf_data_dev_init_time_appl_step_thresh_t *) data;

}


shr_error_e
dnxf_data_dev_init_time_step_thresh_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_dev_init_time_step_thresh_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_time, dnxf_data_dev_init_time_table_step_thresh);
    data = (const dnxf_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->value);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_dev_init_time_appl_step_thresh_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_dev_init_time_appl_step_thresh_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_time, dnxf_data_dev_init_time_table_appl_step_thresh);
    data = (const dnxf_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->value);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnxf_data_dev_init_time_step_thresh_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_time, dnxf_data_dev_init_time_table_step_thresh);

}

const dnxc_data_table_info_t *
dnxf_data_dev_init_time_appl_step_thresh_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_time, dnxf_data_dev_init_time_table_appl_step_thresh);

}






static shr_error_e
dnxf_data_dev_init_shadow_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "shadow";
    submodule_data->doc = "shadow memory for init";
    
    submodule_data->nof_features = _dnxf_data_dev_init_shadow_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dev_init shadow features");

    
    submodule_data->nof_defines = _dnxf_data_dev_init_shadow_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dev_init shadow defines");

    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_all].name = "cache_enable_all";
    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_all].doc = "if true shadow all cacheable memory";
    
    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_all].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_ecc].name = "cache_enable_ecc";
    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_ecc].doc = "if true shadow all cacheable memory which contain ecc field";
    
    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_ecc].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_parity].name = "cache_enable_parity";
    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_parity].doc = "if true shadow all cacheable memory which contain parity field";
    
    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_parity].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_specific].name = "cache_enable_specific";
    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_specific].doc = "if true, some specific memory be shadowed";
    
    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_enable_specific].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_disable_specific].name = "cache_disable_specific";
    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_disable_specific].doc = "if true, some specific memory won't be shadowed";
    
    submodule_data->defines[dnxf_data_dev_init_shadow_define_cache_disable_specific].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnxf_data_dev_init_shadow_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dev_init shadow tables");

    
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].name = "uncacheable_mem";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].doc = "uncache memory";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].size_of_values = sizeof(dnxf_data_dev_init_shadow_uncacheable_mem_t);
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].entry_get = dnxf_data_dev_init_shadow_uncacheable_mem_entry_str_get;

    
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].nof_keys = 1;
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].keys[0].name = "index";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].keys[0].doc = "uncacheable memory index";

    
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].nof_values, "_dnxf_data_dev_init_shadow_table_uncacheable_mem table values");
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].values[0].name = "mem";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].values[0].type = "soc_mem_t";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].values[0].doc = "memory";
    submodule_data->tables[dnxf_data_dev_init_shadow_table_uncacheable_mem].values[0].offset = UTILEX_OFFSETOF(dnxf_data_dev_init_shadow_uncacheable_mem_t, mem);


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
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnxf_data_dev_init_shadow_uncacheable_mem_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_dev_init, dnxf_data_dev_init_submodule_shadow, dnxf_data_dev_init_shadow_table_uncacheable_mem);

}






static shr_error_e
dnxf_data_dev_init_ha_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ha";
    submodule_data->doc = "values for sw state and high availability init";
    
    submodule_data->nof_features = _dnxf_data_dev_init_ha_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dev_init ha features");

    
    submodule_data->nof_defines = _dnxf_data_dev_init_ha_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dev_init ha defines");

    submodule_data->defines[dnxf_data_dev_init_ha_define_warmboot_support].name = "warmboot_support";
    submodule_data->defines[dnxf_data_dev_init_ha_define_warmboot_support].doc = "specify if unit should support wb";
    
    submodule_data->defines[dnxf_data_dev_init_ha_define_warmboot_support].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_ha_define_sw_state_max_size].name = "sw_state_max_size";
    submodule_data->defines[dnxf_data_dev_init_ha_define_sw_state_max_size].doc = "specify the amount of memory (in bytes) preallocated for sw state";
    
    submodule_data->defines[dnxf_data_dev_init_ha_define_sw_state_max_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_ha_define_stable_location].name = "stable_location";
    submodule_data->defines[dnxf_data_dev_init_ha_define_stable_location].doc = "Specify the stable cache option for Warm Boot operations";
    
    submodule_data->defines[dnxf_data_dev_init_ha_define_stable_location].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_dev_init_ha_define_stable_size].name = "stable_size";
    submodule_data->defines[dnxf_data_dev_init_ha_define_stable_size].doc = "Specify the stable cache size in bytes used for Warm boot operations";
    
    submodule_data->defines[dnxf_data_dev_init_ha_define_stable_size].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnxf_data_dev_init_ha_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dev_init ha tables");

    
    submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].name = "stable_filename";
    submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].doc = "If the stable cache location is BCM_SWITCH_STABLE_APPLICATION, the local file system will be used to save the stable cache data with this filename";
    submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].size_of_values = sizeof(dnxf_data_dev_init_ha_stable_filename_t);
    submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].entry_get = dnxf_data_dev_init_ha_stable_filename_entry_str_get;

    
    submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].nof_keys = 0;

    
    submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].nof_values, "_dnxf_data_dev_init_ha_table_stable_filename table values");
    submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].values[0].name = "val";
    submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].values[0].type = "char *";
    submodule_data->tables[dnxf_data_dev_init_ha_table_stable_filename].values[0].doc = "stable filename";
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
    
    if (dnxc_data_mgmt_is_ramon(unit))
    {
        SHR_IF_ERR_EXIT(ramon_a0_data_dev_init_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

