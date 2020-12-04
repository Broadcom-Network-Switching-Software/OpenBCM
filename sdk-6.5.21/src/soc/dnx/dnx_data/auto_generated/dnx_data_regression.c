

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
extern shr_error_e j2p_a0_data_regression_attach(
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






static shr_error_e
dnx_data_regression_ser_parameters_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ser_parameters";
    submodule_data->doc = "Entry idx used for ser dvapi";
    
    submodule_data->nof_features = _dnx_data_regression_ser_parameters_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data regression ser_parameters features");

    
    submodule_data->nof_defines = _dnx_data_regression_ser_parameters_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data regression ser_parameters defines");

    submodule_data->defines[dnx_data_regression_ser_parameters_define_cgm_voq_profiles].name = "cgm_voq_profiles";
    submodule_data->defines[dnx_data_regression_ser_parameters_define_cgm_voq_profiles].doc = "entry index of the table. Used for testing.";
    
    submodule_data->defines[dnx_data_regression_ser_parameters_define_cgm_voq_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_regression_ser_parameters_define_ecgm_qqst_table].name = "ecgm_qqst_table";
    submodule_data->defines[dnx_data_regression_ser_parameters_define_ecgm_qqst_table].doc = "entry index of the table. Used for testing.";
    
    submodule_data->defines[dnx_data_regression_ser_parameters_define_ecgm_qqst_table].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_regression_ser_parameters_define_ecgm_qdct_table].name = "ecgm_qdct_table";
    submodule_data->defines[dnx_data_regression_ser_parameters_define_ecgm_qdct_table].doc = "entry index of the table. Used for testing.";
    
    submodule_data->defines[dnx_data_regression_ser_parameters_define_ecgm_qdct_table].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_regression_ser_parameters_define_erpp_per_pp_port_table].name = "erpp_per_pp_port_table";
    submodule_data->defines[dnx_data_regression_ser_parameters_define_erpp_per_pp_port_table].doc = "entry index of the table. Used for testing.";
    
    submodule_data->defines[dnx_data_regression_ser_parameters_define_erpp_per_pp_port_table].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_regression_ser_parameters_define_erpp_per_pp_port_table_2].name = "erpp_per_pp_port_table_2";
    submodule_data->defines[dnx_data_regression_ser_parameters_define_erpp_per_pp_port_table_2].doc = "entry index of the table. Used for testing of special sku.";
    
    submodule_data->defines[dnx_data_regression_ser_parameters_define_erpp_per_pp_port_table_2].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_regression_ser_parameters_define_mdb_ivsi].name = "mdb_ivsi";
    submodule_data->defines[dnx_data_regression_ser_parameters_define_mdb_ivsi].doc = "entry index of the table. Used for testing.";
    
    submodule_data->defines[dnx_data_regression_ser_parameters_define_mdb_ivsi].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_regression_ser_parameters_define_mdb_evsi].name = "mdb_evsi";
    submodule_data->defines[dnx_data_regression_ser_parameters_define_mdb_evsi].doc = "entry index of the table. Used for testing.";
    
    submodule_data->defines[dnx_data_regression_ser_parameters_define_mdb_evsi].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_regression_ser_parameters_define_mdb_glem].name = "mdb_glem";
    submodule_data->defines[dnx_data_regression_ser_parameters_define_mdb_glem].doc = "entry index of the table. Used for testing.";
    
    submodule_data->defines[dnx_data_regression_ser_parameters_define_mdb_glem].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_regression_ser_parameters_define_mdb_exem_1].name = "mdb_exem_1";
    submodule_data->defines[dnx_data_regression_ser_parameters_define_mdb_exem_1].doc = "entry index of the table. Used for testing.";
    
    submodule_data->defines[dnx_data_regression_ser_parameters_define_mdb_exem_1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_regression_ser_parameters_define_mdb_fec_1].name = "mdb_fec_1";
    submodule_data->defines[dnx_data_regression_ser_parameters_define_mdb_fec_1].doc = "entry index of the table. Used for testing.";
    
    submodule_data->defines[dnx_data_regression_ser_parameters_define_mdb_fec_1].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_regression_ser_parameters_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data regression ser_parameters tables");

    
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_ipp_map].name = "cgm_ipp_map";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_ipp_map].doc = "entry index used for ser testing.";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_ipp_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_ipp_map].size_of_values = sizeof(dnx_data_regression_ser_parameters_cgm_ipp_map_t);
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_ipp_map].entry_get = dnx_data_regression_ser_parameters_cgm_ipp_map_entry_str_get;

    
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_ipp_map].nof_keys = 1;
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_ipp_map].keys[0].name = "index";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_ipp_map].keys[0].doc = "entry index";

    
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_ipp_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_ipp_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_ipp_map].nof_values, "_dnx_data_regression_ser_parameters_table_cgm_ipp_map table values");
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_ipp_map].values[0].name = "entry_index";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_ipp_map].values[0].type = "int";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_ipp_map].values[0].doc = "entry index for ser testing.";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_ipp_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_regression_ser_parameters_cgm_ipp_map_t, entry_index);

    
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms].name = "cgm_voq_dram_bound_prms";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms].doc = "entry index used for ser testing.";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms].size_of_values = sizeof(dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_t);
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms].entry_get = dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_entry_str_get;

    
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms].nof_keys = 1;
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms].keys[0].name = "index";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms].keys[0].doc = "entry index";

    
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms].values, dnxc_data_value_t, submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms].nof_values, "_dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms table values");
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms].values[0].name = "entry_index";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms].values[0].type = "int";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms].values[0].doc = "entry index for ser testing.";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms].values[0].offset = UTILEX_OFFSETOF(dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_t, entry_index);

    
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms_2].name = "cgm_voq_dram_bound_prms_2";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms_2].doc = "entry index used for ser testing of special sku.";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms_2].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms_2].size_of_values = sizeof(dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_2_t);
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms_2].entry_get = dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_2_entry_str_get;

    
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms_2].nof_keys = 1;
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms_2].keys[0].name = "index";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms_2].keys[0].doc = "entry index";

    
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms_2].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms_2].values, dnxc_data_value_t, submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms_2].nof_values, "_dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms_2 table values");
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms_2].values[0].name = "entry_index";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms_2].values[0].type = "int";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms_2].values[0].doc = "entry index for ser testing.";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms_2].values[0].offset = UTILEX_OFFSETOF(dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_2_t, entry_index);

    
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms].name = "cgm_voq_dram_recovery_prms";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms].doc = "entry index used for ser testing.";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms].size_of_values = sizeof(dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_t);
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms].entry_get = dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_entry_str_get;

    
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms].nof_keys = 1;
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms].keys[0].name = "index";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms].keys[0].doc = "entry index";

    
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms].values, dnxc_data_value_t, submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms].nof_values, "_dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms table values");
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms].values[0].name = "entry_index";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms].values[0].type = "int";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms].values[0].doc = "entry index for ser testing.";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms].values[0].offset = UTILEX_OFFSETOF(dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_t, entry_index);

    
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms_2].name = "cgm_voq_dram_recovery_prms_2";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms_2].doc = "entry index used for ser testing of special sku.";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms_2].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms_2].size_of_values = sizeof(dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_2_t);
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms_2].entry_get = dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_2_entry_str_get;

    
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms_2].nof_keys = 1;
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms_2].keys[0].name = "index";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms_2].keys[0].doc = "entry index";

    
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms_2].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms_2].values, dnxc_data_value_t, submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms_2].nof_values, "_dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms_2 table values");
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms_2].values[0].name = "entry_index";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms_2].values[0].type = "int";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms_2].values[0].doc = "entry index for ser testing.";
    submodule_data->tables[dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms_2].values[0].offset = UTILEX_OFFSETOF(dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_2_t, entry_index);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_regression_ser_parameters_feature_get(
    int unit,
    dnx_data_regression_ser_parameters_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, feature);
}


uint32
dnx_data_regression_ser_parameters_cgm_voq_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_define_cgm_voq_profiles);
}

uint32
dnx_data_regression_ser_parameters_ecgm_qqst_table_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_define_ecgm_qqst_table);
}

uint32
dnx_data_regression_ser_parameters_ecgm_qdct_table_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_define_ecgm_qdct_table);
}

uint32
dnx_data_regression_ser_parameters_erpp_per_pp_port_table_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_define_erpp_per_pp_port_table);
}

uint32
dnx_data_regression_ser_parameters_erpp_per_pp_port_table_2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_define_erpp_per_pp_port_table_2);
}

uint32
dnx_data_regression_ser_parameters_mdb_ivsi_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_define_mdb_ivsi);
}

uint32
dnx_data_regression_ser_parameters_mdb_evsi_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_define_mdb_evsi);
}

uint32
dnx_data_regression_ser_parameters_mdb_glem_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_define_mdb_glem);
}

uint32
dnx_data_regression_ser_parameters_mdb_exem_1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_define_mdb_exem_1);
}

uint32
dnx_data_regression_ser_parameters_mdb_fec_1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_define_mdb_fec_1);
}



const dnx_data_regression_ser_parameters_cgm_ipp_map_t *
dnx_data_regression_ser_parameters_cgm_ipp_map_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_table_cgm_ipp_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_regression_ser_parameters_cgm_ipp_map_t *) data;

}

const dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_t *
dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_t *) data;

}

const dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_2_t *
dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_2_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms_2);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_2_t *) data;

}

const dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_t *
dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_t *) data;

}

const dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_2_t *
dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_2_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms_2);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_2_t *) data;

}


shr_error_e
dnx_data_regression_ser_parameters_cgm_ipp_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_regression_ser_parameters_cgm_ipp_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_table_cgm_ipp_map);
    data = (const dnx_data_regression_ser_parameters_cgm_ipp_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->entry_index);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms);
    data = (const dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->entry_index);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_2_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_2_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms_2);
    data = (const dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_2_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->entry_index);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms);
    data = (const dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->entry_index);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_2_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_2_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms_2);
    data = (const dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_2_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->entry_index);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_regression_ser_parameters_cgm_ipp_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_table_cgm_ipp_map);

}

const dnxc_data_table_info_t *
dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms);

}

const dnxc_data_table_info_t *
dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_2_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms_2);

}

const dnxc_data_table_info_t *
dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms);

}

const dnxc_data_table_info_t *
dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_2_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_ser_parameters, dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms_2);

}






static shr_error_e
dnx_data_regression_access_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "access";
    submodule_data->doc = "access for dvapi";
    
    submodule_data->nof_features = _dnx_data_regression_access_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data regression access features");

    
    submodule_data->nof_defines = _dnx_data_regression_access_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data regression access defines");

    submodule_data->defines[dnx_data_regression_access_define_access_mode].name = "access_mode";
    submodule_data->defines[dnx_data_regression_access_define_access_mode].doc = "Checking the device access mode for access dvapi.";
    
    submodule_data->defines[dnx_data_regression_access_define_access_mode].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_regression_access_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data regression access tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_regression_access_feature_get(
    int unit,
    dnx_data_regression_access_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_access, feature);
}


uint32
dnx_data_regression_access_access_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_regression, dnx_data_regression_submodule_access, dnx_data_regression_access_define_access_mode);
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
    SHR_IF_ERR_EXIT(dnx_data_regression_ser_parameters_init(unit, &module_data->submodules[dnx_data_regression_submodule_ser_parameters]));
    SHR_IF_ERR_EXIT(dnx_data_regression_access_init(unit, &module_data->submodules[dnx_data_regression_submodule_access]));
    
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
        SHR_IF_ERR_EXIT(j2p_a0_data_regression_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

