

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LINKSCANDNX

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_linkscan.h>



extern shr_error_e ramon_a0_data_linkscan_attach(
    int unit);



static shr_error_e
dnxf_data_linkscan_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "general data";
    
    submodule_data->nof_features = _dnxf_data_linkscan_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data linkscan general features");

    
    submodule_data->nof_defines = _dnxf_data_linkscan_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data linkscan general defines");

    submodule_data->defines[dnxf_data_linkscan_general_define_interval].name = "interval";
    submodule_data->defines[dnxf_data_linkscan_general_define_interval].doc = "linkscan interval in microseconds";
    
    submodule_data->defines[dnxf_data_linkscan_general_define_interval].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_linkscan_general_define_error_delay].name = "error_delay";
    submodule_data->defines[dnxf_data_linkscan_general_define_error_delay].doc = "the amount of time in microseconds for which the bcm_linkscan module will suspend a port from further update processing after 'max_error' errors are detected.  After this delay, the error state for the port is cleared and normal linkscan processing resumes on the port";
    
    submodule_data->defines[dnxf_data_linkscan_general_define_error_delay].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_linkscan_general_define_max_error].name = "max_error";
    submodule_data->defines[dnxf_data_linkscan_general_define_max_error].doc = "The number of port update errors which will cause the bcm_linkscan module to suspend a port from update processing for the period of time set in 'error_delay'";
    
    submodule_data->defines[dnxf_data_linkscan_general_define_max_error].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_linkscan_general_define_thread_priority].name = "thread_priority";
    submodule_data->defines[dnxf_data_linkscan_general_define_thread_priority].doc = "specifies the priority of the BCM Linkscan thread";
    
    submodule_data->defines[dnxf_data_linkscan_general_define_thread_priority].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_linkscan_general_define_m0_pause_enable].name = "m0_pause_enable";
    submodule_data->defines[dnxf_data_linkscan_general_define_m0_pause_enable].doc = "specifies the pause m0 firmware at Linkscan thread";
    
    submodule_data->defines[dnxf_data_linkscan_general_define_m0_pause_enable].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnxf_data_linkscan_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data linkscan general tables");

    
    submodule_data->tables[dnxf_data_linkscan_general_table_pbmp].name = "pbmp";
    submodule_data->tables[dnxf_data_linkscan_general_table_pbmp].doc = "linkscan pbmp";
    submodule_data->tables[dnxf_data_linkscan_general_table_pbmp].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_linkscan_general_table_pbmp].size_of_values = sizeof(dnxf_data_linkscan_general_pbmp_t);
    submodule_data->tables[dnxf_data_linkscan_general_table_pbmp].entry_get = dnxf_data_linkscan_general_pbmp_entry_str_get;

    
    submodule_data->tables[dnxf_data_linkscan_general_table_pbmp].nof_keys = 0;

    
    submodule_data->tables[dnxf_data_linkscan_general_table_pbmp].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_linkscan_general_table_pbmp].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_linkscan_general_table_pbmp].nof_values, "_dnxf_data_linkscan_general_table_pbmp table values");
    submodule_data->tables[dnxf_data_linkscan_general_table_pbmp].values[0].name = "sw";
    submodule_data->tables[dnxf_data_linkscan_general_table_pbmp].values[0].type = "bcm_pbmp_t";
    submodule_data->tables[dnxf_data_linkscan_general_table_pbmp].values[0].doc = "bitmap of sw linkscan";
    submodule_data->tables[dnxf_data_linkscan_general_table_pbmp].values[0].offset = UTILEX_OFFSETOF(dnxf_data_linkscan_general_pbmp_t, sw);
    submodule_data->tables[dnxf_data_linkscan_general_table_pbmp].values[1].name = "hw";
    submodule_data->tables[dnxf_data_linkscan_general_table_pbmp].values[1].type = "bcm_pbmp_t";
    submodule_data->tables[dnxf_data_linkscan_general_table_pbmp].values[1].doc = "bitmap of hw linkscan";
    submodule_data->tables[dnxf_data_linkscan_general_table_pbmp].values[1].offset = UTILEX_OFFSETOF(dnxf_data_linkscan_general_pbmp_t, hw);


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_linkscan_general_feature_get(
    int unit,
    dnxf_data_linkscan_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_linkscan, dnxf_data_linkscan_submodule_general, feature);
}


uint32
dnxf_data_linkscan_general_interval_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_linkscan, dnxf_data_linkscan_submodule_general, dnxf_data_linkscan_general_define_interval);
}

uint32
dnxf_data_linkscan_general_error_delay_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_linkscan, dnxf_data_linkscan_submodule_general, dnxf_data_linkscan_general_define_error_delay);
}

uint32
dnxf_data_linkscan_general_max_error_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_linkscan, dnxf_data_linkscan_submodule_general, dnxf_data_linkscan_general_define_max_error);
}

uint32
dnxf_data_linkscan_general_thread_priority_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_linkscan, dnxf_data_linkscan_submodule_general, dnxf_data_linkscan_general_define_thread_priority);
}

uint32
dnxf_data_linkscan_general_m0_pause_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_linkscan, dnxf_data_linkscan_submodule_general, dnxf_data_linkscan_general_define_m0_pause_enable);
}



const dnxf_data_linkscan_general_pbmp_t *
dnxf_data_linkscan_general_pbmp_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_linkscan, dnxf_data_linkscan_submodule_general, dnxf_data_linkscan_general_table_pbmp);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnxf_data_linkscan_general_pbmp_t *) data;

}


shr_error_e
dnxf_data_linkscan_general_pbmp_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_linkscan_general_pbmp_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_linkscan, dnxf_data_linkscan_submodule_general, dnxf_data_linkscan_general_table_pbmp);
    data = (const dnxf_data_linkscan_general_pbmp_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_PBMP_STR(buffer, data->sw);
            break;
        case 1:
            DNXC_DATA_MGMT_PBMP_STR(buffer, data->hw);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnxf_data_linkscan_general_pbmp_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_linkscan, dnxf_data_linkscan_submodule_general, dnxf_data_linkscan_general_table_pbmp);

}



shr_error_e
dnxf_data_linkscan_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "linkscan";
    module_data->nof_submodules = _dnxf_data_linkscan_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data linkscan submodules");

    
    SHR_IF_ERR_EXIT(dnxf_data_linkscan_general_init(unit, &module_data->submodules[dnxf_data_linkscan_submodule_general]));
    
    if (dnxc_data_mgmt_is_ramon(unit))
    {
        SHR_IF_ERR_EXIT(ramon_a0_data_linkscan_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

