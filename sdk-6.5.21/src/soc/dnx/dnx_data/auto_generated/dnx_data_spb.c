

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCH

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_spb.h>



extern shr_error_e j2p_a0_data_spb_attach(
    int unit);



static shr_error_e
dnx_data_spb_ocb_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ocb";
    submodule_data->doc = "OCB parameters and defines";
    
    submodule_data->nof_features = _dnx_data_spb_ocb_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data spb ocb features");

    submodule_data->features[dnx_data_spb_ocb_blocks_in_ring].name = "blocks_in_ring";
    submodule_data->features[dnx_data_spb_ocb_blocks_in_ring].doc = "OCB is splitted into blocks connected by a ring";
    submodule_data->features[dnx_data_spb_ocb_blocks_in_ring].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_spb_ocb_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data spb ocb defines");

    submodule_data->defines[dnx_data_spb_ocb_define_nof_blocks].name = "nof_blocks";
    submodule_data->defines[dnx_data_spb_ocb_define_nof_blocks].doc = "Number of ocb blocks";
    
    submodule_data->defines[dnx_data_spb_ocb_define_nof_blocks].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_spb_ocb_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data spb ocb tables");

    
    submodule_data->tables[dnx_data_spb_ocb_table_last_in_chain].name = "last_in_chain";
    submodule_data->tables[dnx_data_spb_ocb_table_last_in_chain].doc = "Holds for each OCB block whether is the last block in the chain";
    submodule_data->tables[dnx_data_spb_ocb_table_last_in_chain].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_spb_ocb_table_last_in_chain].size_of_values = sizeof(dnx_data_spb_ocb_last_in_chain_t);
    submodule_data->tables[dnx_data_spb_ocb_table_last_in_chain].entry_get = dnx_data_spb_ocb_last_in_chain_entry_str_get;

    
    submodule_data->tables[dnx_data_spb_ocb_table_last_in_chain].nof_keys = 1;
    submodule_data->tables[dnx_data_spb_ocb_table_last_in_chain].keys[0].name = "ocb_index";
    submodule_data->tables[dnx_data_spb_ocb_table_last_in_chain].keys[0].doc = "OCB block index";

    
    submodule_data->tables[dnx_data_spb_ocb_table_last_in_chain].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_spb_ocb_table_last_in_chain].values, dnxc_data_value_t, submodule_data->tables[dnx_data_spb_ocb_table_last_in_chain].nof_values, "_dnx_data_spb_ocb_table_last_in_chain table values");
    submodule_data->tables[dnx_data_spb_ocb_table_last_in_chain].values[0].name = "is_last_in_chain";
    submodule_data->tables[dnx_data_spb_ocb_table_last_in_chain].values[0].type = "int";
    submodule_data->tables[dnx_data_spb_ocb_table_last_in_chain].values[0].doc = "Is the OCB block index is the last in the chain";
    submodule_data->tables[dnx_data_spb_ocb_table_last_in_chain].values[0].offset = UTILEX_OFFSETOF(dnx_data_spb_ocb_last_in_chain_t, is_last_in_chain);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_spb_ocb_feature_get(
    int unit,
    dnx_data_spb_ocb_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_spb, dnx_data_spb_submodule_ocb, feature);
}


uint32
dnx_data_spb_ocb_nof_blocks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_spb, dnx_data_spb_submodule_ocb, dnx_data_spb_ocb_define_nof_blocks);
}



const dnx_data_spb_ocb_last_in_chain_t *
dnx_data_spb_ocb_last_in_chain_get(
    int unit,
    int ocb_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_spb, dnx_data_spb_submodule_ocb, dnx_data_spb_ocb_table_last_in_chain);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ocb_index, 0);
    return (const dnx_data_spb_ocb_last_in_chain_t *) data;

}


shr_error_e
dnx_data_spb_ocb_last_in_chain_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_spb_ocb_last_in_chain_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_spb, dnx_data_spb_submodule_ocb, dnx_data_spb_ocb_table_last_in_chain);
    data = (const dnx_data_spb_ocb_last_in_chain_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_last_in_chain);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_spb_ocb_last_in_chain_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_spb, dnx_data_spb_submodule_ocb, dnx_data_spb_ocb_table_last_in_chain);

}



shr_error_e
dnx_data_spb_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "spb";
    module_data->nof_submodules = _dnx_data_spb_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data spb submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_spb_ocb_init(unit, &module_data->submodules[dnx_data_spb_submodule_ocb]));
    
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(j2p_a0_data_spb_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

