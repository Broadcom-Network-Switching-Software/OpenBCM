

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COMMON

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_aod_sizes.h>



extern shr_error_e jr2_a0_data_aod_sizes_attach(
    int unit);
extern shr_error_e jr2_b0_data_aod_sizes_attach(
    int unit);
extern shr_error_e j2c_a0_data_aod_sizes_attach(
    int unit);
extern shr_error_e q2a_a0_data_aod_sizes_attach(
    int unit);
extern shr_error_e q2a_b0_data_aod_sizes_attach(
    int unit);
extern shr_error_e j2p_a0_data_aod_sizes_attach(
    int unit);



static shr_error_e
dnx_data_aod_sizes_AOD_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "AOD";
    submodule_data->doc = "AOD table sizes";
    
    submodule_data->nof_features = _dnx_data_aod_sizes_AOD_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data aod_sizes AOD features");

    
    submodule_data->nof_defines = _dnx_data_aod_sizes_AOD_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data aod_sizes AOD defines");

    submodule_data->defines[dnx_data_aod_sizes_AOD_define_count].name = "count";
    submodule_data->defines[dnx_data_aod_sizes_AOD_define_count].doc = "None";
    
    submodule_data->defines[dnx_data_aod_sizes_AOD_define_count].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_aod_sizes_AOD_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data aod_sizes AOD tables");

    
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_sizes].name = "sizes";
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_sizes].doc = "AOD table sizes";
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_sizes].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_sizes].size_of_values = sizeof(dnx_data_aod_sizes_AOD_sizes_t);
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_sizes].entry_get = dnx_data_aod_sizes_AOD_sizes_entry_str_get;

    
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_sizes].nof_keys = 1;
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_sizes].keys[0].name = "index";
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_sizes].keys[0].doc = "None";

    
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_sizes].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_aod_sizes_AOD_table_sizes].values, dnxc_data_value_t, submodule_data->tables[dnx_data_aod_sizes_AOD_table_sizes].nof_values, "_dnx_data_aod_sizes_AOD_table_sizes table values");
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_sizes].values[0].name = "value";
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_sizes].values[0].type = "uint32";
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_sizes].values[0].doc = "None";
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_sizes].values[0].offset = UTILEX_OFFSETOF(dnx_data_aod_sizes_AOD_sizes_t, value);

    
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_dynamic_sizes].name = "dynamic_sizes";
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_dynamic_sizes].doc = "AOD table sizes";
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_dynamic_sizes].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_dynamic_sizes].size_of_values = sizeof(dnx_data_aod_sizes_AOD_dynamic_sizes_t);
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_dynamic_sizes].entry_get = dnx_data_aod_sizes_AOD_dynamic_sizes_entry_str_get;

    
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_dynamic_sizes].nof_keys = 1;
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_dynamic_sizes].keys[0].name = "index";
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_dynamic_sizes].keys[0].doc = "None";

    
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_dynamic_sizes].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_aod_sizes_AOD_table_dynamic_sizes].values, dnxc_data_value_t, submodule_data->tables[dnx_data_aod_sizes_AOD_table_dynamic_sizes].nof_values, "_dnx_data_aod_sizes_AOD_table_dynamic_sizes table values");
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_dynamic_sizes].values[0].name = "value";
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_dynamic_sizes].values[0].type = "uint32";
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_dynamic_sizes].values[0].doc = "None";
    submodule_data->tables[dnx_data_aod_sizes_AOD_table_dynamic_sizes].values[0].offset = UTILEX_OFFSETOF(dnx_data_aod_sizes_AOD_dynamic_sizes_t, value);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_aod_sizes_AOD_feature_get(
    int unit,
    dnx_data_aod_sizes_AOD_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_aod_sizes, dnx_data_aod_sizes_submodule_AOD, feature);
}


uint32
dnx_data_aod_sizes_AOD_count_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_aod_sizes, dnx_data_aod_sizes_submodule_AOD, dnx_data_aod_sizes_AOD_define_count);
}



const dnx_data_aod_sizes_AOD_sizes_t *
dnx_data_aod_sizes_AOD_sizes_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod_sizes, dnx_data_aod_sizes_submodule_AOD, dnx_data_aod_sizes_AOD_table_sizes);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_aod_sizes_AOD_sizes_t *) data;

}

const dnx_data_aod_sizes_AOD_dynamic_sizes_t *
dnx_data_aod_sizes_AOD_dynamic_sizes_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod_sizes, dnx_data_aod_sizes_submodule_AOD, dnx_data_aod_sizes_AOD_table_dynamic_sizes);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_aod_sizes_AOD_dynamic_sizes_t *) data;

}


shr_error_e
dnx_data_aod_sizes_AOD_sizes_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_aod_sizes_AOD_sizes_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod_sizes, dnx_data_aod_sizes_submodule_AOD, dnx_data_aod_sizes_AOD_table_sizes);
    data = (const dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->value);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_aod_sizes_AOD_dynamic_sizes_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_aod_sizes_AOD_dynamic_sizes_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod_sizes, dnx_data_aod_sizes_submodule_AOD, dnx_data_aod_sizes_AOD_table_dynamic_sizes);
    data = (const dnx_data_aod_sizes_AOD_dynamic_sizes_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->value);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_aod_sizes_AOD_sizes_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_aod_sizes, dnx_data_aod_sizes_submodule_AOD, dnx_data_aod_sizes_AOD_table_sizes);

}

const dnxc_data_table_info_t *
dnx_data_aod_sizes_AOD_dynamic_sizes_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_aod_sizes, dnx_data_aod_sizes_submodule_AOD, dnx_data_aod_sizes_AOD_table_dynamic_sizes);

}



shr_error_e
dnx_data_aod_sizes_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "aod_sizes";
    module_data->nof_submodules = _dnx_data_aod_sizes_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data aod_sizes submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_aod_sizes_AOD_init(unit, &module_data->submodules[dnx_data_aod_sizes_submodule_AOD]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_aod_sizes_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_aod_sizes_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_aod_sizes_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_aod_sizes_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_aod_sizes_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_aod_sizes_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_aod_sizes_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_aod_sizes_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_aod_sizes_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

