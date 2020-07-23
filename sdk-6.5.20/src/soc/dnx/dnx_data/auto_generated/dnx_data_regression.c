

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_regression.h>



extern shr_error_e jr2_a0_data_regression_attach(
    int unit);
extern shr_error_e j2c_a0_data_regression_attach(
    int unit);
extern shr_error_e q2a_a0_data_regression_attach(
    int unit);



static shr_error_e
dnx_data_regression_dvapi_topology_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dvapi_topology";
    submodule_data->doc = "topologies of dvapi";
    
    submodule_data->nof_features = _dnx_data_regression_dvapi_topology_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data regression dvapi_topology features");

    
    submodule_data->nof_defines = _dnx_data_regression_dvapi_topology_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data regression dvapi_topology defines");

    submodule_data->defines[dnx_data_regression_dvapi_topology_define_nof_pm_ext_loops].name = "nof_pm_ext_loops";
    submodule_data->defines[dnx_data_regression_dvapi_topology_define_nof_pm_ext_loops].doc = "number of exp loopbacks";
    
    submodule_data->defines[dnx_data_regression_dvapi_topology_define_nof_pm_ext_loops].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_regression_dvapi_topology_define_nof_pm_p2p_links].name = "nof_pm_p2p_links";
    submodule_data->defines[dnx_data_regression_dvapi_topology_define_nof_pm_p2p_links].doc = "number of p2p links";
    
    submodule_data->defines[dnx_data_regression_dvapi_topology_define_nof_pm_p2p_links].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_regression_dvapi_topology_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data regression dvapi_topology tables");

    
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_ext_loops].name = "pm_ext_loops";
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_ext_loops].doc = "port macros have external loopback";
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_ext_loops].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_ext_loops].size_of_values = sizeof(dnx_data_regression_dvapi_topology_pm_ext_loops_t);
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_ext_loops].entry_get = dnx_data_regression_dvapi_topology_pm_ext_loops_entry_str_get;

    
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_ext_loops].nof_keys = 1;
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_ext_loops].keys[0].name = "index";
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_ext_loops].keys[0].doc = "entry index";

    
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_ext_loops].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_ext_loops].values, dnxc_data_value_t, submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_ext_loops].nof_values, "_dnx_data_regression_dvapi_topology_table_pm_ext_loops table values");
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_ext_loops].values[0].name = "pm_id";
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_ext_loops].values[0].type = "uint32";
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_ext_loops].values[0].doc = "external loopback is enabled on this pm";
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_ext_loops].values[0].offset = UTILEX_OFFSETOF(dnx_data_regression_dvapi_topology_pm_ext_loops_t, pm_id);

    
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_p2p_links].name = "pm_p2p_links";
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_p2p_links].doc = "port macros have p2p links";
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_p2p_links].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_p2p_links].size_of_values = sizeof(dnx_data_regression_dvapi_topology_pm_p2p_links_t);
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_p2p_links].entry_get = dnx_data_regression_dvapi_topology_pm_p2p_links_entry_str_get;

    
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_p2p_links].nof_keys = 1;
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_p2p_links].keys[0].name = "index";
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_p2p_links].keys[0].doc = "entry index";

    
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_p2p_links].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_p2p_links].values, dnxc_data_value_t, submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_p2p_links].nof_values, "_dnx_data_regression_dvapi_topology_table_pm_p2p_links table values");
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_p2p_links].values[0].name = "pm_id";
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_p2p_links].values[0].type = "uint32";
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_p2p_links].values[0].doc = "pm_id id";
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_p2p_links].values[0].offset = UTILEX_OFFSETOF(dnx_data_regression_dvapi_topology_pm_p2p_links_t, pm_id);
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_p2p_links].values[1].name = "peer_pm_id";
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_p2p_links].values[1].type = "uint32";
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_p2p_links].values[1].doc = "peer pm connected with pm_id";
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_p2p_links].values[1].offset = UTILEX_OFFSETOF(dnx_data_regression_dvapi_topology_pm_p2p_links_t, peer_pm_id);

    
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_test_limitations].name = "pm_test_limitations";
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_test_limitations].doc = "Table of PMs with limitations. Used for testing.";
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_test_limitations].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_test_limitations].size_of_values = sizeof(dnx_data_regression_dvapi_topology_pm_test_limitations_t);
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_test_limitations].entry_get = dnx_data_regression_dvapi_topology_pm_test_limitations_entry_str_get;

    
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_test_limitations].nof_keys = 1;
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_test_limitations].keys[0].name = "pm_index";
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_test_limitations].keys[0].doc = "PM instance number.";

    
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_test_limitations].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_test_limitations].values, dnxc_data_value_t, submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_test_limitations].nof_values, "_dnx_data_regression_dvapi_topology_table_pm_test_limitations table values");
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_test_limitations].values[0].name = "max_speed";
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_test_limitations].values[0].type = "int";
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_test_limitations].values[0].doc = "Max speed allowed per PM. Limitation is due to SPF connectors used on board.";
    submodule_data->tables[dnx_data_regression_dvapi_topology_table_pm_test_limitations].values[0].offset = UTILEX_OFFSETOF(dnx_data_regression_dvapi_topology_pm_test_limitations_t, max_speed);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_regression_dvapi_topology_feature_get(
    int unit,
    dnx_data_regression_dvapi_topology_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_dvapi_topology, feature);
}


uint32
dnx_data_regression_dvapi_topology_nof_pm_ext_loops_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_dvapi_topology, dnx_data_regression_dvapi_topology_define_nof_pm_ext_loops);
}

uint32
dnx_data_regression_dvapi_topology_nof_pm_p2p_links_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_dvapi_topology, dnx_data_regression_dvapi_topology_define_nof_pm_p2p_links);
}



const dnx_data_regression_dvapi_topology_pm_ext_loops_t *
dnx_data_regression_dvapi_topology_pm_ext_loops_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_dvapi_topology, dnx_data_regression_dvapi_topology_table_pm_ext_loops);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_regression_dvapi_topology_pm_ext_loops_t *) data;

}

const dnx_data_regression_dvapi_topology_pm_p2p_links_t *
dnx_data_regression_dvapi_topology_pm_p2p_links_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_dvapi_topology, dnx_data_regression_dvapi_topology_table_pm_p2p_links);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_regression_dvapi_topology_pm_p2p_links_t *) data;

}

const dnx_data_regression_dvapi_topology_pm_test_limitations_t *
dnx_data_regression_dvapi_topology_pm_test_limitations_get(
    int unit,
    int pm_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_dvapi_topology, dnx_data_regression_dvapi_topology_table_pm_test_limitations);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, pm_index, 0);
    return (const dnx_data_regression_dvapi_topology_pm_test_limitations_t *) data;

}


shr_error_e
dnx_data_regression_dvapi_topology_pm_ext_loops_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_regression_dvapi_topology_pm_ext_loops_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_dvapi_topology, dnx_data_regression_dvapi_topology_table_pm_ext_loops);
    data = (const dnx_data_regression_dvapi_topology_pm_ext_loops_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pm_id);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_regression_dvapi_topology_pm_p2p_links_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_regression_dvapi_topology_pm_p2p_links_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_dvapi_topology, dnx_data_regression_dvapi_topology_table_pm_p2p_links);
    data = (const dnx_data_regression_dvapi_topology_pm_p2p_links_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pm_id);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->peer_pm_id);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_regression_dvapi_topology_pm_test_limitations_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_regression_dvapi_topology_pm_test_limitations_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_dvapi_topology, dnx_data_regression_dvapi_topology_table_pm_test_limitations);
    data = (const dnx_data_regression_dvapi_topology_pm_test_limitations_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max_speed);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_regression_dvapi_topology_pm_ext_loops_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_dvapi_topology, dnx_data_regression_dvapi_topology_table_pm_ext_loops);

}

const dnxc_data_table_info_t *
dnx_data_regression_dvapi_topology_pm_p2p_links_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_dvapi_topology, dnx_data_regression_dvapi_topology_table_pm_p2p_links);

}

const dnxc_data_table_info_t *
dnx_data_regression_dvapi_topology_pm_test_limitations_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_dvapi_topology, dnx_data_regression_dvapi_topology_table_pm_test_limitations);

}



shr_error_e
dnx_data_regression_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "regression";
    module_data->nof_submodules = _dnx_data_regression_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data regression submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_regression_dvapi_topology_init(unit, &module_data->submodules[dnx_data_regression_submodule_dvapi_topology]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_regression_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_regression_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_regression_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_regression_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_regression_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_regression_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_regression_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_regression_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_regression_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_regression_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_regression_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_regression_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_regression_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_regression_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

