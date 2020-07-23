

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCH

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_switch.h>



extern shr_error_e jr2_a0_data_switch_attach(
    int unit);
extern shr_error_e jr2_b0_data_switch_attach(
    int unit);
extern shr_error_e j2c_a0_data_switch_attach(
    int unit);
extern shr_error_e q2a_a0_data_switch_attach(
    int unit);
extern shr_error_e j2p_a0_data_switch_attach(
    int unit);



static shr_error_e
dnx_data_switch_load_balancing_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "load_balancing";
    submodule_data->doc = "Definitions related to load balancing on L2 and L3.";
    
    submodule_data->nof_features = _dnx_data_switch_load_balancing_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data switch load_balancing features");

    
    submodule_data->nof_defines = _dnx_data_switch_load_balancing_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data switch load_balancing defines");

    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_lb_clients].name = "nof_lb_clients";
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_lb_clients].doc = "Number of load balancing clients (users of CRC functions)";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_lb_clients].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_lb_crc_sel_tcam_entries].name = "nof_lb_crc_sel_tcam_entries";
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_lb_crc_sel_tcam_entries].doc = "Number of entries on IPPB_LOAD_BALANCING_CRC_SELECTION_CAM";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_lb_crc_sel_tcam_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_crc_functions].name = "nof_crc_functions";
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_crc_functions].doc = "Number of crc functions available for 'load balancing'";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_crc_functions].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_label].name = "initial_reserved_label";
    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_label].doc = "Bitmap. Each bit indicates the corresponding reserved label is to be                         included into load balancing calculation";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_label].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_label_force].name = "initial_reserved_label_force";
    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_label_force].doc = "Bitmap. Each bit indicates the corresponding reserved label is to be                         forced into load balancing calculation even if it was terminated";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_label_force].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_next_label].name = "initial_reserved_next_label";
    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_next_label].doc = "Bitmap. Each bit indicates whether label following correponding reserved label                         is to be forced into load balancing calculation even if it was terminated";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_next_label].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_next_label_valid].name = "initial_reserved_next_label_valid";
    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_next_label_valid].doc = "Bitmap. Currently redundant. Should be equal to 'initial_reserved_next_label' ";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_initial_reserved_next_label_valid].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_reserved_next_label_valid].name = "reserved_next_label_valid";
    submodule_data->defines[dnx_data_switch_load_balancing_define_reserved_next_label_valid].doc = "Indicate whether the field reserved_next_label_valid exists";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_reserved_next_label_valid].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_num_valid_mpls_protocols].name = "num_valid_mpls_protocols";
    submodule_data->defines[dnx_data_switch_load_balancing_define_num_valid_mpls_protocols].doc = "Number of supported MPLS protocols in dbal_enum_value_field_current_protocol_type_e";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_num_valid_mpls_protocols].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_layer_records_from_parser].name = "nof_layer_records_from_parser";
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_layer_records_from_parser].doc = "Number of 32-bits layer records, cretaed by parser, which are used for hashing on load balancing";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_layer_records_from_parser].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_seeds_per_crc_function].name = "nof_seeds_per_crc_function";
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_seeds_per_crc_function].doc = "Number of crc seeds that may be selected per crc function on load balancing";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_nof_seeds_per_crc_function].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_hashing_selection_per_layer].name = "hashing_selection_per_layer";
    submodule_data->defines[dnx_data_switch_load_balancing_define_hashing_selection_per_layer].doc = "Indicate whether the functionality of using the LB key in hashing per layer is enabled";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_hashing_selection_per_layer].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_mpls_split_stack_config].name = "mpls_split_stack_config";
    submodule_data->defines[dnx_data_switch_load_balancing_define_mpls_split_stack_config].doc = "Indicate whether there is support separate configuration per each of the two stacks - terminated and non-terminated";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_mpls_split_stack_config].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_load_balancing_define_mpls_cam_next_label_valid_field_exists].name = "mpls_cam_next_label_valid_field_exists";
    submodule_data->defines[dnx_data_switch_load_balancing_define_mpls_cam_next_label_valid_field_exists].doc = "The MPLS force label TCAM contains a field which was later remove for some of the devices.";
    
    submodule_data->defines[dnx_data_switch_load_balancing_define_mpls_cam_next_label_valid_field_exists].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_switch_load_balancing_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data switch load_balancing tables");

    
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].name = "lb_clients";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].doc = "Given hw identifier ('client id') get the corresponding logical client name, from 'bcm_switch_control_t'.";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].size_of_values = sizeof(dnx_data_switch_load_balancing_lb_clients_t);
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].entry_get = dnx_data_switch_load_balancing_lb_clients_entry_str_get;

    
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].nof_keys = 1;
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].keys[0].name = "client_hw_id";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].keys[0].doc = "Hw identifier of load-balancing client. See table LOAD_BALANCING_CRC_SELECTION_CAM (IPPB_LOAD_BALANCING_CRC_SELECTION_CAM)";

    
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].values, dnxc_data_value_t, submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].nof_values, "_dnx_data_switch_load_balancing_table_lb_clients table values");
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].values[0].name = "client_logical_id";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].values[0].type = "int";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].values[0].doc = "Corresponding logical identifier of client (value taken from 'bcm_switch_control_t')";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_clients].values[0].offset = UTILEX_OFFSETOF(dnx_data_switch_load_balancing_lb_clients_t, client_logical_id);

    
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].name = "lb_client_crc";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].doc = "Define default CRC hash function selection (bcm_switch_hash_config_t) based on hw identifier.";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].size_of_values = sizeof(dnx_data_switch_load_balancing_lb_client_crc_t);
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].entry_get = dnx_data_switch_load_balancing_lb_client_crc_entry_str_get;

    
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].nof_keys = 1;
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].keys[0].name = "client_hw_id";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].keys[0].doc = "Hw identifier of load-balancing client. See table LOAD_BALANCING_CRC_SELECTION_CAM (IPPB_LOAD_BALANCING_CRC_SELECTION_CAM)";

    
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].values, dnxc_data_value_t, submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].nof_values, "_dnx_data_switch_load_balancing_table_lb_client_crc table values");
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].values[0].name = "crc_hash_func";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].values[0].type = "int";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].values[0].doc = "Default hashing function for identifier of client";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_client_crc].values[0].offset = UTILEX_OFFSETOF(dnx_data_switch_load_balancing_lb_client_crc_t, crc_hash_func);

    
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].name = "lb_field_enablers";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].doc = "Define the default values for field enablers per header";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].size_of_values = sizeof(dnx_data_switch_load_balancing_lb_field_enablers_t);
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].entry_get = dnx_data_switch_load_balancing_lb_field_enablers_entry_str_get;

    
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].nof_keys = 1;
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].keys[0].name = "header_id";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].keys[0].doc = "the ID of the header based on dbal_enum_value_field_hash_field_enablers_header_e";

    
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].values, dnxc_data_value_t, submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].nof_values, "_dnx_data_switch_load_balancing_table_lb_field_enablers table values");
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].values[0].name = "field_enablers_mask";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].values[0].type = "uint32";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].values[0].doc = "The mask of the fields which are enabled for hashing per header";
    submodule_data->tables[dnx_data_switch_load_balancing_table_lb_field_enablers].values[0].offset = UTILEX_OFFSETOF(dnx_data_switch_load_balancing_lb_field_enablers_t, field_enablers_mask);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_switch_load_balancing_feature_get(
    int unit,
    dnx_data_switch_load_balancing_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, feature);
}


uint32
dnx_data_switch_load_balancing_nof_lb_clients_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_nof_lb_clients);
}

uint32
dnx_data_switch_load_balancing_nof_lb_crc_sel_tcam_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_nof_lb_crc_sel_tcam_entries);
}

uint32
dnx_data_switch_load_balancing_nof_crc_functions_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_nof_crc_functions);
}

uint32
dnx_data_switch_load_balancing_initial_reserved_label_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_initial_reserved_label);
}

uint32
dnx_data_switch_load_balancing_initial_reserved_label_force_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_initial_reserved_label_force);
}

uint32
dnx_data_switch_load_balancing_initial_reserved_next_label_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_initial_reserved_next_label);
}

uint32
dnx_data_switch_load_balancing_initial_reserved_next_label_valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_initial_reserved_next_label_valid);
}

uint32
dnx_data_switch_load_balancing_reserved_next_label_valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_reserved_next_label_valid);
}

uint32
dnx_data_switch_load_balancing_num_valid_mpls_protocols_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_num_valid_mpls_protocols);
}

uint32
dnx_data_switch_load_balancing_nof_layer_records_from_parser_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_nof_layer_records_from_parser);
}

uint32
dnx_data_switch_load_balancing_nof_seeds_per_crc_function_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_nof_seeds_per_crc_function);
}

uint32
dnx_data_switch_load_balancing_hashing_selection_per_layer_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_hashing_selection_per_layer);
}

uint32
dnx_data_switch_load_balancing_mpls_split_stack_config_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_mpls_split_stack_config);
}

uint32
dnx_data_switch_load_balancing_mpls_cam_next_label_valid_field_exists_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_define_mpls_cam_next_label_valid_field_exists);
}



const dnx_data_switch_load_balancing_lb_clients_t *
dnx_data_switch_load_balancing_lb_clients_get(
    int unit,
    int client_hw_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_table_lb_clients);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, client_hw_id, 0);
    return (const dnx_data_switch_load_balancing_lb_clients_t *) data;

}

const dnx_data_switch_load_balancing_lb_client_crc_t *
dnx_data_switch_load_balancing_lb_client_crc_get(
    int unit,
    int client_hw_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_table_lb_client_crc);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, client_hw_id, 0);
    return (const dnx_data_switch_load_balancing_lb_client_crc_t *) data;

}

const dnx_data_switch_load_balancing_lb_field_enablers_t *
dnx_data_switch_load_balancing_lb_field_enablers_get(
    int unit,
    int header_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_table_lb_field_enablers);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, header_id, 0);
    return (const dnx_data_switch_load_balancing_lb_field_enablers_t *) data;

}


shr_error_e
dnx_data_switch_load_balancing_lb_clients_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_switch_load_balancing_lb_clients_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_table_lb_clients);
    data = (const dnx_data_switch_load_balancing_lb_clients_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->client_logical_id);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_switch_load_balancing_lb_client_crc_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_switch_load_balancing_lb_client_crc_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_table_lb_client_crc);
    data = (const dnx_data_switch_load_balancing_lb_client_crc_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->crc_hash_func);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_switch_load_balancing_lb_field_enablers_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_switch_load_balancing_lb_field_enablers_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_table_lb_field_enablers);
    data = (const dnx_data_switch_load_balancing_lb_field_enablers_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->field_enablers_mask);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_switch_load_balancing_lb_clients_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_table_lb_clients);

}

const dnxc_data_table_info_t *
dnx_data_switch_load_balancing_lb_client_crc_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_table_lb_client_crc);

}

const dnxc_data_table_info_t *
dnx_data_switch_load_balancing_lb_field_enablers_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_load_balancing, dnx_data_switch_load_balancing_table_lb_field_enablers);

}






static shr_error_e
dnx_data_switch_svtag_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "svtag";
    submodule_data->doc = "Definitions related to the SVTAG.";
    
    submodule_data->nof_features = _dnx_data_switch_svtag_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data switch svtag features");

    
    submodule_data->nof_defines = _dnx_data_switch_svtag_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data switch svtag defines");

    submodule_data->defines[dnx_data_switch_svtag_define_supported].name = "supported";
    submodule_data->defines[dnx_data_switch_svtag_define_supported].doc = "Indicate if the SVTAG supported on the device";
    
    submodule_data->defines[dnx_data_switch_svtag_define_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_svtag_label_size_bits].name = "svtag_label_size_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_svtag_label_size_bits].doc = "The size in bits of an SVTAG label";
    
    submodule_data->defines[dnx_data_switch_svtag_define_svtag_label_size_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_offset_addr_size_bits].name = "egress_svtag_offset_addr_size_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_offset_addr_size_bits].doc = "The size in bits of the offset addr on the egress svtag ";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_offset_addr_size_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_offset_addr_position_bits].name = "egress_svtag_offset_addr_position_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_offset_addr_position_bits].doc = "The position in bits of the offset addr in the egress svtag ";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_offset_addr_position_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_signature_size_bits].name = "egress_svtag_signature_size_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_signature_size_bits].doc = "The size in bits of the signature on the egress svtag ";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_signature_size_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_signature_position_bits].name = "egress_svtag_signature_position_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_signature_position_bits].doc = "The position in bits of the signature in the egress svtag ";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_signature_position_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_sci_size_bits].name = "egress_svtag_sci_size_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_sci_size_bits].doc = "The size in bits of the SCI on the egress svtag ";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_sci_size_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_sci_position_bits].name = "egress_svtag_sci_position_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_sci_position_bits].doc = "The position in bits of the SCI in the egress svtag ";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_sci_position_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_pkt_type_size_bits].name = "egress_svtag_pkt_type_size_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_pkt_type_size_bits].doc = "The size in bits of the packet type on the egress svtag ";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_pkt_type_size_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_pkt_type_position_bits].name = "egress_svtag_pkt_type_position_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_pkt_type_position_bits].doc = "The position in bits of the packet type in the egress svtag ";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_pkt_type_position_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_ipv6_indication_position_bits].name = "egress_svtag_ipv6_indication_position_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_ipv6_indication_position_bits].doc = "The position in bits of the IPv6 indication in the egress svtag ";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_ipv6_indication_position_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_hw_field_position_bits].name = "egress_svtag_hw_field_position_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_hw_field_position_bits].doc = "The position in bits of the SVTAG in the HW field.";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_hw_field_position_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_accumulation_indication_hw_field_position_bits].name = "egress_svtag_accumulation_indication_hw_field_position_bits";
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_accumulation_indication_hw_field_position_bits].doc = "The HW field position in bits of the SVTAG accumulation indication.";
    
    submodule_data->defines[dnx_data_switch_svtag_define_egress_svtag_accumulation_indication_hw_field_position_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_UDP_dedicated_port].name = "UDP_dedicated_port";
    submodule_data->defines[dnx_data_switch_svtag_define_UDP_dedicated_port].doc = "The source and destination port value in case of a security tag over a UDP tunnel.";
    
    submodule_data->defines[dnx_data_switch_svtag_define_UDP_dedicated_port].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_ingress_svtag_position_bytes].name = "ingress_svtag_position_bytes";
    submodule_data->defines[dnx_data_switch_svtag_define_ingress_svtag_position_bytes].doc = "The ingress SVTAG position in bytes before it swapped to the beginning  of the packet.";
    
    submodule_data->defines[dnx_data_switch_svtag_define_ingress_svtag_position_bytes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_switch_svtag_define_enable_by_defualt].name = "enable_by_defualt";
    submodule_data->defines[dnx_data_switch_svtag_define_enable_by_defualt].doc = "SVTAG is enable by default on all ports";
    
    submodule_data->defines[dnx_data_switch_svtag_define_enable_by_defualt].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_switch_svtag_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data switch svtag tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_switch_svtag_feature_get(
    int unit,
    dnx_data_switch_svtag_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, feature);
}


uint32
dnx_data_switch_svtag_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_supported);
}

uint32
dnx_data_switch_svtag_svtag_label_size_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_svtag_label_size_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_offset_addr_size_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_offset_addr_size_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_offset_addr_position_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_offset_addr_position_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_signature_size_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_signature_size_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_signature_position_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_signature_position_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_sci_size_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_sci_size_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_sci_position_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_sci_position_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_pkt_type_size_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_pkt_type_size_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_pkt_type_position_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_pkt_type_position_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_ipv6_indication_position_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_ipv6_indication_position_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_hw_field_position_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_hw_field_position_bits);
}

uint32
dnx_data_switch_svtag_egress_svtag_accumulation_indication_hw_field_position_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_egress_svtag_accumulation_indication_hw_field_position_bits);
}

uint32
dnx_data_switch_svtag_UDP_dedicated_port_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_UDP_dedicated_port);
}

uint32
dnx_data_switch_svtag_ingress_svtag_position_bytes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_ingress_svtag_position_bytes);
}

uint32
dnx_data_switch_svtag_enable_by_defualt_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_svtag, dnx_data_switch_svtag_define_enable_by_defualt);
}










static shr_error_e
dnx_data_switch_feature_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "feature";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_switch_feature_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data switch feature features");

    submodule_data->features[dnx_data_switch_feature_mpls_labels_included_in_hash].name = "mpls_labels_included_in_hash";
    submodule_data->features[dnx_data_switch_feature_mpls_labels_included_in_hash].doc = "No MPLS labels participate in the build of the LB keys regardless of their termination state.";
    submodule_data->features[dnx_data_switch_feature_mpls_labels_included_in_hash].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_switch_feature_excluded_header_bits_from_hash].name = "excluded_header_bits_from_hash";
    submodule_data->features[dnx_data_switch_feature_excluded_header_bits_from_hash].doc = "The ETH.VID field does not participate in hashing; 4 MSBs of IPV6.Flow_label participate in hashing.";
    submodule_data->features[dnx_data_switch_feature_excluded_header_bits_from_hash].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_switch_feature_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data switch feature defines");

    
    submodule_data->nof_tables = _dnx_data_switch_feature_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data switch feature tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_switch_feature_feature_get(
    int unit,
    dnx_data_switch_feature_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_switch, dnx_data_switch_submodule_feature, feature);
}








shr_error_e
dnx_data_switch_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "switch";
    module_data->nof_submodules = _dnx_data_switch_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data switch submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_switch_load_balancing_init(unit, &module_data->submodules[dnx_data_switch_submodule_load_balancing]));
    SHR_IF_ERR_EXIT(dnx_data_switch_svtag_init(unit, &module_data->submodules[dnx_data_switch_submodule_svtag]));
    SHR_IF_ERR_EXIT(dnx_data_switch_feature_init(unit, &module_data->submodules[dnx_data_switch_submodule_feature]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_switch_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_switch_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_switch_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_switch_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_switch_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_switch_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_switch_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_switch_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_switch_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_switch_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_switch_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_switch_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_switch_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_switch_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_switch_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_switch_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_switch_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

