

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_lane_map.h>



extern shr_error_e jr2_a0_data_lane_map_attach(
    int unit);



static shr_error_e
dnx_data_lane_map_nif_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "nif";
    submodule_data->doc = "lane mapping attributes for NIF";
    
    submodule_data->nof_features = _dnx_data_lane_map_nif_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data lane_map nif features");

    
    submodule_data->nof_defines = _dnx_data_lane_map_nif_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data lane_map nif defines");

    
    submodule_data->nof_tables = _dnx_data_lane_map_nif_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data lane_map nif tables");

    
    submodule_data->tables[dnx_data_lane_map_nif_table_mapping].name = "mapping";
    submodule_data->tables[dnx_data_lane_map_nif_table_mapping].doc = "lane to serdes mapping info for NIF ports";
    submodule_data->tables[dnx_data_lane_map_nif_table_mapping].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_lane_map_nif_table_mapping].size_of_values = sizeof(dnx_data_lane_map_nif_mapping_t);
    submodule_data->tables[dnx_data_lane_map_nif_table_mapping].entry_get = dnx_data_lane_map_nif_mapping_entry_str_get;

    
    submodule_data->tables[dnx_data_lane_map_nif_table_mapping].nof_keys = 1;
    submodule_data->tables[dnx_data_lane_map_nif_table_mapping].keys[0].name = "lane_id";
    submodule_data->tables[dnx_data_lane_map_nif_table_mapping].keys[0].doc = "logical lane id";

    
    submodule_data->tables[dnx_data_lane_map_nif_table_mapping].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_lane_map_nif_table_mapping].values, dnxc_data_value_t, submodule_data->tables[dnx_data_lane_map_nif_table_mapping].nof_values, "_dnx_data_lane_map_nif_table_mapping table values");
    submodule_data->tables[dnx_data_lane_map_nif_table_mapping].values[0].name = "serdes_rx_id";
    submodule_data->tables[dnx_data_lane_map_nif_table_mapping].values[0].type = "int";
    submodule_data->tables[dnx_data_lane_map_nif_table_mapping].values[0].doc = "Rx Serdes lane id";
    submodule_data->tables[dnx_data_lane_map_nif_table_mapping].values[0].offset = UTILEX_OFFSETOF(dnx_data_lane_map_nif_mapping_t, serdes_rx_id);
    submodule_data->tables[dnx_data_lane_map_nif_table_mapping].values[1].name = "serdes_tx_id";
    submodule_data->tables[dnx_data_lane_map_nif_table_mapping].values[1].type = "int";
    submodule_data->tables[dnx_data_lane_map_nif_table_mapping].values[1].doc = "Tx Serdes lane id";
    submodule_data->tables[dnx_data_lane_map_nif_table_mapping].values[1].offset = UTILEX_OFFSETOF(dnx_data_lane_map_nif_mapping_t, serdes_tx_id);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_lane_map_nif_feature_get(
    int unit,
    dnx_data_lane_map_nif_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_lane_map, dnx_data_lane_map_submodule_nif, feature);
}




const dnx_data_lane_map_nif_mapping_t *
dnx_data_lane_map_nif_mapping_get(
    int unit,
    int lane_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_lane_map, dnx_data_lane_map_submodule_nif, dnx_data_lane_map_nif_table_mapping);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, lane_id, 0);
    return (const dnx_data_lane_map_nif_mapping_t *) data;

}


shr_error_e
dnx_data_lane_map_nif_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_lane_map_nif_mapping_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_lane_map, dnx_data_lane_map_submodule_nif, dnx_data_lane_map_nif_table_mapping);
    data = (const dnx_data_lane_map_nif_mapping_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->serdes_rx_id);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->serdes_tx_id);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_lane_map_nif_mapping_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_lane_map, dnx_data_lane_map_submodule_nif, dnx_data_lane_map_nif_table_mapping);

}






static shr_error_e
dnx_data_lane_map_fabric_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "fabric";
    submodule_data->doc = "lane mapping attributes for Farbic";
    
    submodule_data->nof_features = _dnx_data_lane_map_fabric_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data lane_map fabric features");

    
    submodule_data->nof_defines = _dnx_data_lane_map_fabric_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data lane_map fabric defines");

    submodule_data->defines[dnx_data_lane_map_fabric_define_resolution].name = "resolution";
    submodule_data->defines[dnx_data_lane_map_fabric_define_resolution].doc = "Nof links which fabric lane mapping is allowed.";
    
    submodule_data->defines[dnx_data_lane_map_fabric_define_resolution].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_lane_map_fabric_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data lane_map fabric tables");

    
    submodule_data->tables[dnx_data_lane_map_fabric_table_mapping].name = "mapping";
    submodule_data->tables[dnx_data_lane_map_fabric_table_mapping].doc = "lane to serdes mapping info for Fabric links";
    submodule_data->tables[dnx_data_lane_map_fabric_table_mapping].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_lane_map_fabric_table_mapping].size_of_values = sizeof(dnx_data_lane_map_fabric_mapping_t);
    submodule_data->tables[dnx_data_lane_map_fabric_table_mapping].entry_get = dnx_data_lane_map_fabric_mapping_entry_str_get;

    
    submodule_data->tables[dnx_data_lane_map_fabric_table_mapping].nof_keys = 1;
    submodule_data->tables[dnx_data_lane_map_fabric_table_mapping].keys[0].name = "lane_id";
    submodule_data->tables[dnx_data_lane_map_fabric_table_mapping].keys[0].doc = "logical lane id";

    
    submodule_data->tables[dnx_data_lane_map_fabric_table_mapping].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_lane_map_fabric_table_mapping].values, dnxc_data_value_t, submodule_data->tables[dnx_data_lane_map_fabric_table_mapping].nof_values, "_dnx_data_lane_map_fabric_table_mapping table values");
    submodule_data->tables[dnx_data_lane_map_fabric_table_mapping].values[0].name = "serdes_rx_id";
    submodule_data->tables[dnx_data_lane_map_fabric_table_mapping].values[0].type = "int";
    submodule_data->tables[dnx_data_lane_map_fabric_table_mapping].values[0].doc = "Rx Serdes lane id";
    submodule_data->tables[dnx_data_lane_map_fabric_table_mapping].values[0].offset = UTILEX_OFFSETOF(dnx_data_lane_map_fabric_mapping_t, serdes_rx_id);
    submodule_data->tables[dnx_data_lane_map_fabric_table_mapping].values[1].name = "serdes_tx_id";
    submodule_data->tables[dnx_data_lane_map_fabric_table_mapping].values[1].type = "int";
    submodule_data->tables[dnx_data_lane_map_fabric_table_mapping].values[1].doc = "Tx Serdes lane id";
    submodule_data->tables[dnx_data_lane_map_fabric_table_mapping].values[1].offset = UTILEX_OFFSETOF(dnx_data_lane_map_fabric_mapping_t, serdes_tx_id);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_lane_map_fabric_feature_get(
    int unit,
    dnx_data_lane_map_fabric_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_lane_map, dnx_data_lane_map_submodule_fabric, feature);
}


uint32
dnx_data_lane_map_fabric_resolution_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lane_map, dnx_data_lane_map_submodule_fabric, dnx_data_lane_map_fabric_define_resolution);
}



const dnx_data_lane_map_fabric_mapping_t *
dnx_data_lane_map_fabric_mapping_get(
    int unit,
    int lane_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_lane_map, dnx_data_lane_map_submodule_fabric, dnx_data_lane_map_fabric_table_mapping);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, lane_id, 0);
    return (const dnx_data_lane_map_fabric_mapping_t *) data;

}


shr_error_e
dnx_data_lane_map_fabric_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_lane_map_fabric_mapping_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_lane_map, dnx_data_lane_map_submodule_fabric, dnx_data_lane_map_fabric_table_mapping);
    data = (const dnx_data_lane_map_fabric_mapping_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->serdes_rx_id);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->serdes_tx_id);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_lane_map_fabric_mapping_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_lane_map, dnx_data_lane_map_submodule_fabric, dnx_data_lane_map_fabric_table_mapping);

}






static shr_error_e
dnx_data_lane_map_ilkn_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ilkn";
    submodule_data->doc = "logical lane re-mapping attributes for ILKN";
    
    submodule_data->nof_features = _dnx_data_lane_map_ilkn_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data lane_map ilkn features");

    
    submodule_data->nof_defines = _dnx_data_lane_map_ilkn_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data lane_map ilkn defines");

    
    submodule_data->nof_tables = _dnx_data_lane_map_ilkn_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data lane_map ilkn tables");

    
    submodule_data->tables[dnx_data_lane_map_ilkn_table_remapping].name = "remapping";
    submodule_data->tables[dnx_data_lane_map_ilkn_table_remapping].doc = "Ilkn remapping info";
    submodule_data->tables[dnx_data_lane_map_ilkn_table_remapping].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_lane_map_ilkn_table_remapping].size_of_values = sizeof(dnx_data_lane_map_ilkn_remapping_t);
    submodule_data->tables[dnx_data_lane_map_ilkn_table_remapping].entry_get = dnx_data_lane_map_ilkn_remapping_entry_str_get;

    
    submodule_data->tables[dnx_data_lane_map_ilkn_table_remapping].nof_keys = 2;
    submodule_data->tables[dnx_data_lane_map_ilkn_table_remapping].keys[0].name = "lane_id";
    submodule_data->tables[dnx_data_lane_map_ilkn_table_remapping].keys[0].doc = "logical lane id";
    submodule_data->tables[dnx_data_lane_map_ilkn_table_remapping].keys[1].name = "ilkn_id";
    submodule_data->tables[dnx_data_lane_map_ilkn_table_remapping].keys[1].doc = "ILKN port ID";

    
    submodule_data->tables[dnx_data_lane_map_ilkn_table_remapping].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_lane_map_ilkn_table_remapping].values, dnxc_data_value_t, submodule_data->tables[dnx_data_lane_map_ilkn_table_remapping].nof_values, "_dnx_data_lane_map_ilkn_table_remapping table values");
    submodule_data->tables[dnx_data_lane_map_ilkn_table_remapping].values[0].name = "ilkn_lane_id";
    submodule_data->tables[dnx_data_lane_map_ilkn_table_remapping].values[0].type = "int";
    submodule_data->tables[dnx_data_lane_map_ilkn_table_remapping].values[0].doc = "Ilkn lane ID after remapping.";
    submodule_data->tables[dnx_data_lane_map_ilkn_table_remapping].values[0].offset = UTILEX_OFFSETOF(dnx_data_lane_map_ilkn_remapping_t, ilkn_lane_id);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_lane_map_ilkn_feature_get(
    int unit,
    dnx_data_lane_map_ilkn_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_lane_map, dnx_data_lane_map_submodule_ilkn, feature);
}




const dnx_data_lane_map_ilkn_remapping_t *
dnx_data_lane_map_ilkn_remapping_get(
    int unit,
    int lane_id,
    int ilkn_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_lane_map, dnx_data_lane_map_submodule_ilkn, dnx_data_lane_map_ilkn_table_remapping);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, lane_id, ilkn_id);
    return (const dnx_data_lane_map_ilkn_remapping_t *) data;

}


shr_error_e
dnx_data_lane_map_ilkn_remapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_lane_map_ilkn_remapping_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_lane_map, dnx_data_lane_map_submodule_ilkn, dnx_data_lane_map_ilkn_table_remapping);
    data = (const dnx_data_lane_map_ilkn_remapping_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ilkn_lane_id);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_lane_map_ilkn_remapping_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_lane_map, dnx_data_lane_map_submodule_ilkn, dnx_data_lane_map_ilkn_table_remapping);

}



shr_error_e
dnx_data_lane_map_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "lane_map";
    module_data->nof_submodules = _dnx_data_lane_map_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data lane_map submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_lane_map_nif_init(unit, &module_data->submodules[dnx_data_lane_map_submodule_nif]));
    SHR_IF_ERR_EXIT(dnx_data_lane_map_fabric_init(unit, &module_data->submodules[dnx_data_lane_map_submodule_fabric]));
    SHR_IF_ERR_EXIT(dnx_data_lane_map_ilkn_init(unit, &module_data->submodules[dnx_data_lane_map_submodule_ilkn]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_lane_map_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_lane_map_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_lane_map_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_lane_map_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_lane_map_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_lane_map_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_lane_map_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_lane_map_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_lane_map_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

