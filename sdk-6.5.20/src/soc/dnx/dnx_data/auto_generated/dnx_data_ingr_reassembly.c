

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_ingr_reassembly.h>



extern shr_error_e jr2_a0_data_ingr_reassembly_attach(
    int unit);
extern shr_error_e j2c_a0_data_ingr_reassembly_attach(
    int unit);
extern shr_error_e q2a_a0_data_ingr_reassembly_attach(
    int unit);
extern shr_error_e j2p_a0_data_ingr_reassembly_attach(
    int unit);



static shr_error_e
dnx_data_ingr_reassembly_context_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "context";
    submodule_data->doc = "Ingress context data";
    
    submodule_data->nof_features = _dnx_data_ingr_reassembly_context_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingr_reassembly context features");

    submodule_data->features[dnx_data_ingr_reassembly_context_rcy_interleaving_between_priorities].name = "rcy_interleaving_between_priorities";
    submodule_data->features[dnx_data_ingr_reassembly_context_rcy_interleaving_between_priorities].doc = "RCY interleaving between priorities";
    submodule_data->features[dnx_data_ingr_reassembly_context_rcy_interleaving_between_priorities].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_ingr_reassembly_context_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingr_reassembly context defines");

    submodule_data->defines[dnx_data_ingr_reassembly_context_define_nof_contexts].name = "nof_contexts";
    submodule_data->defines[dnx_data_ingr_reassembly_context_define_nof_contexts].doc = "Number of ingress reassembly contexts per core";
    
    submodule_data->defines[dnx_data_ingr_reassembly_context_define_nof_contexts].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_context_define_invalid_context].name = "invalid_context";
    submodule_data->defines[dnx_data_ingr_reassembly_context_define_invalid_context].doc = "Invalid reassembly context";
    
    submodule_data->defines[dnx_data_ingr_reassembly_context_define_invalid_context].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_ingr_reassembly_context_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingr_reassembly context tables");

    
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].name = "context_map";
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].doc = "Context Map configuration information per port type ";
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].size_of_values = sizeof(dnx_data_ingr_reassembly_context_context_map_t);
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].entry_get = dnx_data_ingr_reassembly_context_context_map_entry_str_get;

    
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].nof_keys = 1;
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].keys[0].name = "port_type";
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].keys[0].doc = "interface type";

    
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].nof_values, "_dnx_data_ingr_reassembly_context_table_context_map table values");
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].values[0].name = "start_index";
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].values[0].type = "uint32";
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].values[0].doc = "index of start of specific interface type in reassembly table";
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_ingr_reassembly_context_context_map_t, start_index);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ingr_reassembly_context_feature_get(
    int unit,
    dnx_data_ingr_reassembly_context_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_context, feature);
}


uint32
dnx_data_ingr_reassembly_context_nof_contexts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_context, dnx_data_ingr_reassembly_context_define_nof_contexts);
}

uint32
dnx_data_ingr_reassembly_context_invalid_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_context, dnx_data_ingr_reassembly_context_define_invalid_context);
}



const dnx_data_ingr_reassembly_context_context_map_t *
dnx_data_ingr_reassembly_context_context_map_get(
    int unit,
    int port_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_context, dnx_data_ingr_reassembly_context_table_context_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port_type, 0);
    return (const dnx_data_ingr_reassembly_context_context_map_t *) data;

}


shr_error_e
dnx_data_ingr_reassembly_context_context_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_ingr_reassembly_context_context_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_context, dnx_data_ingr_reassembly_context_table_context_map);
    data = (const dnx_data_ingr_reassembly_context_context_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->start_index);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_ingr_reassembly_context_context_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_context, dnx_data_ingr_reassembly_context_table_context_map);

}






static shr_error_e
dnx_data_ingr_reassembly_ilkn_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ilkn";
    submodule_data->doc = "Interlaken data";
    
    submodule_data->nof_features = _dnx_data_ingr_reassembly_ilkn_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingr_reassembly ilkn features");

    submodule_data->features[dnx_data_ingr_reassembly_ilkn_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_ingr_reassembly_ilkn_is_supported].doc = "set if device supports ILKN ports (not including ELK)";
    submodule_data->features[dnx_data_ingr_reassembly_ilkn_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_ingr_reassembly_ilkn_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingr_reassembly ilkn defines");

    
    submodule_data->nof_tables = _dnx_data_ingr_reassembly_ilkn_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingr_reassembly ilkn tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ingr_reassembly_ilkn_feature_get(
    int unit,
    dnx_data_ingr_reassembly_ilkn_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_ilkn, feature);
}











static shr_error_e
dnx_data_ingr_reassembly_rcy_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "rcy";
    submodule_data->doc = "Recycle data";
    
    submodule_data->nof_features = _dnx_data_ingr_reassembly_rcy_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingr_reassembly rcy features");

    
    submodule_data->nof_defines = _dnx_data_ingr_reassembly_rcy_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingr_reassembly rcy defines");

    submodule_data->defines[dnx_data_ingr_reassembly_rcy_define_nof_interfaces].name = "nof_interfaces";
    submodule_data->defines[dnx_data_ingr_reassembly_rcy_define_nof_interfaces].doc = "Number of RCY interfaces per core";
    
    submodule_data->defines[dnx_data_ingr_reassembly_rcy_define_nof_interfaces].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_ingr_reassembly_rcy_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingr_reassembly rcy tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ingr_reassembly_rcy_feature_get(
    int unit,
    dnx_data_ingr_reassembly_rcy_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_rcy, feature);
}


uint32
dnx_data_ingr_reassembly_rcy_nof_interfaces_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_rcy, dnx_data_ingr_reassembly_rcy_define_nof_interfaces);
}










static shr_error_e
dnx_data_ingr_reassembly_priority_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "priority";
    submodule_data->doc = "information about priorities";
    
    submodule_data->nof_features = _dnx_data_ingr_reassembly_priority_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingr_reassembly priority features");

    
    submodule_data->nof_defines = _dnx_data_ingr_reassembly_priority_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingr_reassembly priority defines");

    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_nif_eth_priorities_nof].name = "nif_eth_priorities_nof";
    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_nif_eth_priorities_nof].doc = "Number of priorities for NIF ETH port";
    
    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_nif_eth_priorities_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_mirror_priorities_nof].name = "mirror_priorities_nof";
    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_mirror_priorities_nof].doc = "Number of priorities for mirror channel";
    
    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_mirror_priorities_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_rcy_priorities_nof].name = "rcy_priorities_nof";
    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_rcy_priorities_nof].doc = "Number of priorities (groups) for RCY interface";
    
    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_rcy_priorities_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_priorities_nof].name = "priorities_nof";
    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_priorities_nof].doc = "Number of priorities (groups) for any interface type";
    
    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_priorities_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_cgm_priorities_nof].name = "cgm_priorities_nof";
    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_cgm_priorities_nof].doc = "Number of priorities (groups) for any interface type as considered by CGM module";
    
    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_cgm_priorities_nof].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_ingr_reassembly_priority_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingr_reassembly priority tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ingr_reassembly_priority_feature_get(
    int unit,
    dnx_data_ingr_reassembly_priority_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_priority, feature);
}


uint32
dnx_data_ingr_reassembly_priority_nif_eth_priorities_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_priority, dnx_data_ingr_reassembly_priority_define_nif_eth_priorities_nof);
}

uint32
dnx_data_ingr_reassembly_priority_mirror_priorities_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_priority, dnx_data_ingr_reassembly_priority_define_mirror_priorities_nof);
}

uint32
dnx_data_ingr_reassembly_priority_rcy_priorities_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_priority, dnx_data_ingr_reassembly_priority_define_rcy_priorities_nof);
}

uint32
dnx_data_ingr_reassembly_priority_priorities_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_priority, dnx_data_ingr_reassembly_priority_define_priorities_nof);
}

uint32
dnx_data_ingr_reassembly_priority_cgm_priorities_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_priority, dnx_data_ingr_reassembly_priority_define_cgm_priorities_nof);
}










static shr_error_e
dnx_data_ingr_reassembly_dbal_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dbal";
    submodule_data->doc = "information required for dbal";
    
    submodule_data->nof_features = _dnx_data_ingr_reassembly_dbal_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingr_reassembly dbal features");

    
    submodule_data->nof_defines = _dnx_data_ingr_reassembly_dbal_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingr_reassembly dbal defines");

    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_reassembly_context_bits].name = "reassembly_context_bits";
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_reassembly_context_bits].doc = "number of bits in reassembly context";
    
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_reassembly_context_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interface_bits].name = "interface_bits";
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interface_bits].doc = "number of bits in ingress reassembly interface ID";
    
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interface_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interface_max].name = "interface_max";
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interface_max].doc = "maximal ingress reassembly interface ID";
    
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interface_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_context_map_base_address_bits].name = "context_map_base_address_bits";
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_context_map_base_address_bits].doc = "number of bits in context_map_base_address";
    
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_context_map_base_address_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_context_map_base_address_max].name = "context_map_base_address_max";
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_context_map_base_address_max].doc = "maximal value of context_map_base_address";
    
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_context_map_base_address_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_context_map_base_offest_ext_max].name = "context_map_base_offest_ext_max";
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_context_map_base_offest_ext_max].doc = "maximal value of offset ext field";
    
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_context_map_base_offest_ext_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interleaved_interface_max].name = "interleaved_interface_max";
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interleaved_interface_max].doc = "maximal interleaved interface ID";
    
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interleaved_interface_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interleaved_interface_bits].name = "interleaved_interface_bits";
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interleaved_interface_bits].doc = "number of bits in interleaved interface ID";
    
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interleaved_interface_bits].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_ingr_reassembly_dbal_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingr_reassembly dbal tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ingr_reassembly_dbal_feature_get(
    int unit,
    dnx_data_ingr_reassembly_dbal_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_dbal, feature);
}


uint32
dnx_data_ingr_reassembly_dbal_reassembly_context_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_dbal, dnx_data_ingr_reassembly_dbal_define_reassembly_context_bits);
}

uint32
dnx_data_ingr_reassembly_dbal_interface_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_dbal, dnx_data_ingr_reassembly_dbal_define_interface_bits);
}

uint32
dnx_data_ingr_reassembly_dbal_interface_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_dbal, dnx_data_ingr_reassembly_dbal_define_interface_max);
}

uint32
dnx_data_ingr_reassembly_dbal_context_map_base_address_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_dbal, dnx_data_ingr_reassembly_dbal_define_context_map_base_address_bits);
}

uint32
dnx_data_ingr_reassembly_dbal_context_map_base_address_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_dbal, dnx_data_ingr_reassembly_dbal_define_context_map_base_address_max);
}

uint32
dnx_data_ingr_reassembly_dbal_context_map_base_offest_ext_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_dbal, dnx_data_ingr_reassembly_dbal_define_context_map_base_offest_ext_max);
}

uint32
dnx_data_ingr_reassembly_dbal_interleaved_interface_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_dbal, dnx_data_ingr_reassembly_dbal_define_interleaved_interface_max);
}

uint32
dnx_data_ingr_reassembly_dbal_interleaved_interface_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_dbal, dnx_data_ingr_reassembly_dbal_define_interleaved_interface_bits);
}







shr_error_e
dnx_data_ingr_reassembly_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "ingr_reassembly";
    module_data->nof_submodules = _dnx_data_ingr_reassembly_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data ingr_reassembly submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_ingr_reassembly_context_init(unit, &module_data->submodules[dnx_data_ingr_reassembly_submodule_context]));
    SHR_IF_ERR_EXIT(dnx_data_ingr_reassembly_ilkn_init(unit, &module_data->submodules[dnx_data_ingr_reassembly_submodule_ilkn]));
    SHR_IF_ERR_EXIT(dnx_data_ingr_reassembly_rcy_init(unit, &module_data->submodules[dnx_data_ingr_reassembly_submodule_rcy]));
    SHR_IF_ERR_EXIT(dnx_data_ingr_reassembly_priority_init(unit, &module_data->submodules[dnx_data_ingr_reassembly_submodule_priority]));
    SHR_IF_ERR_EXIT(dnx_data_ingr_reassembly_dbal_init(unit, &module_data->submodules[dnx_data_ingr_reassembly_submodule_dbal]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_reassembly_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_reassembly_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_reassembly_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_reassembly_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_ingr_reassembly_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_reassembly_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_ingr_reassembly_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_reassembly_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_ingr_reassembly_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_reassembly_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_ingr_reassembly_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_reassembly_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_ingr_reassembly_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_reassembly_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_ingr_reassembly_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

