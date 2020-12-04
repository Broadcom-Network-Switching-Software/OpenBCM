

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_fifodma.h>



extern shr_error_e jr2_a0_data_fifodma_attach(
    int unit);



static shr_error_e
dnx_data_fifodma_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "fifodma general data";
    
    submodule_data->nof_features = _dnx_data_fifodma_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fifodma general features");

    
    submodule_data->nof_defines = _dnx_data_fifodma_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fifodma general defines");

    submodule_data->defines[dnx_data_fifodma_general_define_nof_fifodma_channels].name = "nof_fifodma_channels";
    submodule_data->defines[dnx_data_fifodma_general_define_nof_fifodma_channels].doc = "number of fifodma channels supported";
    
    submodule_data->defines[dnx_data_fifodma_general_define_nof_fifodma_channels].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fifodma_general_define_min_nof_host_entries].name = "min_nof_host_entries";
    submodule_data->defines[dnx_data_fifodma_general_define_min_nof_host_entries].doc = "minimum number of host entries supported";
    
    submodule_data->defines[dnx_data_fifodma_general_define_min_nof_host_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fifodma_general_define_max_nof_host_entries].name = "max_nof_host_entries";
    submodule_data->defines[dnx_data_fifodma_general_define_max_nof_host_entries].doc = "maximum number of host entries supported";
    
    submodule_data->defines[dnx_data_fifodma_general_define_max_nof_host_entries].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_fifodma_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fifodma general tables");

    
    submodule_data->tables[dnx_data_fifodma_general_table_fifodma_map].name = "fifodma_map";
    submodule_data->tables[dnx_data_fifodma_general_table_fifodma_map].doc = "1-1 map between fifodma channel id and fifodma source";
    submodule_data->tables[dnx_data_fifodma_general_table_fifodma_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fifodma_general_table_fifodma_map].size_of_values = sizeof(dnx_data_fifodma_general_fifodma_map_t);
    submodule_data->tables[dnx_data_fifodma_general_table_fifodma_map].entry_get = dnx_data_fifodma_general_fifodma_map_entry_str_get;

    
    submodule_data->tables[dnx_data_fifodma_general_table_fifodma_map].nof_keys = 1;
    submodule_data->tables[dnx_data_fifodma_general_table_fifodma_map].keys[0].name = "fifodma_source";
    submodule_data->tables[dnx_data_fifodma_general_table_fifodma_map].keys[0].doc = "fifodma source";

    
    submodule_data->tables[dnx_data_fifodma_general_table_fifodma_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fifodma_general_table_fifodma_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fifodma_general_table_fifodma_map].nof_values, "_dnx_data_fifodma_general_table_fifodma_map table values");
    submodule_data->tables[dnx_data_fifodma_general_table_fifodma_map].values[0].name = "fifodma_channel";
    submodule_data->tables[dnx_data_fifodma_general_table_fifodma_map].values[0].type = "int";
    submodule_data->tables[dnx_data_fifodma_general_table_fifodma_map].values[0].doc = "fifodma channel id";
    submodule_data->tables[dnx_data_fifodma_general_table_fifodma_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_fifodma_general_fifodma_map_t, fifodma_channel);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fifodma_general_feature_get(
    int unit,
    dnx_data_fifodma_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fifodma, dnx_data_fifodma_submodule_general, feature);
}


uint32
dnx_data_fifodma_general_nof_fifodma_channels_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fifodma, dnx_data_fifodma_submodule_general, dnx_data_fifodma_general_define_nof_fifodma_channels);
}

uint32
dnx_data_fifodma_general_min_nof_host_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fifodma, dnx_data_fifodma_submodule_general, dnx_data_fifodma_general_define_min_nof_host_entries);
}

uint32
dnx_data_fifodma_general_max_nof_host_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fifodma, dnx_data_fifodma_submodule_general, dnx_data_fifodma_general_define_max_nof_host_entries);
}



const dnx_data_fifodma_general_fifodma_map_t *
dnx_data_fifodma_general_fifodma_map_get(
    int unit,
    int fifodma_source)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fifodma, dnx_data_fifodma_submodule_general, dnx_data_fifodma_general_table_fifodma_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, fifodma_source, 0);
    return (const dnx_data_fifodma_general_fifodma_map_t *) data;

}


shr_error_e
dnx_data_fifodma_general_fifodma_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fifodma_general_fifodma_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fifodma, dnx_data_fifodma_submodule_general, dnx_data_fifodma_general_table_fifodma_map);
    data = (const dnx_data_fifodma_general_fifodma_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fifodma_channel);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_fifodma_general_fifodma_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fifodma, dnx_data_fifodma_submodule_general, dnx_data_fifodma_general_table_fifodma_map);

}



shr_error_e
dnx_data_fifodma_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "fifodma";
    module_data->nof_submodules = _dnx_data_fifodma_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data fifodma submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_fifodma_general_init(unit, &module_data->submodules[dnx_data_fifodma_submodule_general]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fifodma_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fifodma_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fifodma_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fifodma_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fifodma_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fifodma_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fifodma_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fifodma_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fifodma_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

