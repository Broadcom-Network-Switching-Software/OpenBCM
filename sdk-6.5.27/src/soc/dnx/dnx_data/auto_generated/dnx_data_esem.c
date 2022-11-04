
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_ESEM

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_esem.h>




extern shr_error_e jr2_a0_data_esem_attach(
    int unit);


extern shr_error_e jr2_b0_data_esem_attach(
    int unit);


extern shr_error_e j2c_a0_data_esem_attach(
    int unit);


extern shr_error_e q2a_a0_data_esem_attach(
    int unit);


extern shr_error_e j2p_a0_data_esem_attach(
    int unit);


extern shr_error_e j2x_a0_data_esem_attach(
    int unit);




static shr_error_e
dnx_data_esem_access_cmd_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "access_cmd";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_esem_access_cmd_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data esem access_cmd features");

    
    submodule_data->nof_defines = _dnx_data_esem_access_cmd_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data esem access_cmd defines");

    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_esem_accesses].name = "nof_esem_accesses";
    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_esem_accesses].doc = "";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_esem_accesses].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_app_di_id_size_in_bits].name = "app_di_id_size_in_bits";
    submodule_data->defines[dnx_data_esem_access_cmd_define_app_di_id_size_in_bits].doc = "";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_app_di_id_size_in_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_default_native].name = "default_native";
    submodule_data->defines[dnx_data_esem_access_cmd_define_default_native].doc = "";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_default_native].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_sflow_sample_interface].name = "sflow_sample_interface";
    submodule_data->defines[dnx_data_esem_access_cmd_define_sflow_sample_interface].doc = "";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_sflow_sample_interface].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_ip_tunnel_no_action].name = "ip_tunnel_no_action";
    submodule_data->defines[dnx_data_esem_access_cmd_define_ip_tunnel_no_action].doc = "";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_ip_tunnel_no_action].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_port_svtag].name = "port_svtag";
    submodule_data->defines[dnx_data_esem_access_cmd_define_port_svtag].doc = "";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_port_svtag].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_esem_cmd_default_profiles].name = "nof_esem_cmd_default_profiles";
    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_esem_cmd_default_profiles].doc = "";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_esem_cmd_default_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_cmds_size_in_bits].name = "nof_cmds_size_in_bits";
    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_cmds_size_in_bits].doc = "";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_cmds_size_in_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_cmds].name = "nof_cmds";
    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_cmds].doc = "";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_cmds].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_min_size_for_esem_cmd].name = "min_size_for_esem_cmd";
    submodule_data->defines[dnx_data_esem_access_cmd_define_min_size_for_esem_cmd].doc = "";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_min_size_for_esem_cmd].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_gre_any_ipv4].name = "esem_cmd_arr_prefix_gre_any_ipv4";
    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_gre_any_ipv4].doc = "";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_gre_any_ipv4].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_vxlan_gpe].name = "esem_cmd_arr_prefix_vxlan_gpe";
    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_vxlan_gpe].doc = "";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_vxlan_gpe].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_ip_tunnel_no_action].name = "esem_cmd_arr_prefix_ip_tunnel_no_action";
    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_ip_tunnel_no_action].doc = "";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_ip_tunnel_no_action].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_arp_plus_ac].name = "esem_cmd_arr_prefix_arp_plus_ac";
    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_arp_plus_ac].doc = "";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_arp_plus_ac].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_predefine_allocations].name = "esem_cmd_predefine_allocations";
    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_predefine_allocations].doc = "";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_predefine_allocations].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_no_action].name = "no_action";
    submodule_data->defines[dnx_data_esem_access_cmd_define_no_action].doc = "";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_no_action].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_esem_access_cmd_define_default_ac].name = "default_ac";
    submodule_data->defines[dnx_data_esem_access_cmd_define_default_ac].doc = "";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_default_ac].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_esem_access_cmd_define_allocation_mngr_default].name = "allocation_mngr_default";
    submodule_data->defines[dnx_data_esem_access_cmd_define_allocation_mngr_default].doc = "";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_allocation_mngr_default].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_esem_access_cmd_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data esem access_cmd tables");

    
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].name = "esem_cmd_table_map";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].doc = "";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].size_of_values = sizeof(dnx_data_esem_access_cmd_esem_cmd_table_map_t);
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].entry_get = dnx_data_esem_access_cmd_esem_cmd_table_map_entry_str_get;

    
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].nof_keys = 0;

    
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].nof_values = 6;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].nof_values, "_dnx_data_esem_access_cmd_table_esem_cmd_table_map table values");
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[0].name = "table_name";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[0].doc = "";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_esem_access_cmd_esem_cmd_table_map_t, table_name);
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[1].name = "esem_cmd";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[1].type = "dbal_fields_e";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[1].doc = "";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[1].offset = UTILEX_OFFSETOF(dnx_data_esem_access_cmd_esem_cmd_table_map_t, esem_cmd);
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[2].name = "access_valid";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[2].type = "dbal_fields_e[DNX_DATA_MAX_ESEM_ACCESS_CMD_NOF_ESEM_ACCESSES]";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[2].doc = "";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[2].offset = UTILEX_OFFSETOF(dnx_data_esem_access_cmd_esem_cmd_table_map_t, access_valid);
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[3].name = "access_program";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[3].type = "dbal_fields_e[DNX_DATA_MAX_ESEM_ACCESS_CMD_NOF_ESEM_ACCESSES]";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[3].doc = "";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[3].offset = UTILEX_OFFSETOF(dnx_data_esem_access_cmd_esem_cmd_table_map_t, access_program);
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[4].name = "access_designated_offset";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[4].type = "dbal_fields_e[DNX_DATA_MAX_ESEM_ACCESS_CMD_NOF_ESEM_ACCESSES]";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[4].doc = "";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[4].offset = UTILEX_OFFSETOF(dnx_data_esem_access_cmd_esem_cmd_table_map_t, access_designated_offset);
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[5].name = "access_default_result";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[5].type = "dbal_fields_e[DNX_DATA_MAX_ESEM_ACCESS_CMD_NOF_ESEM_ACCESSES]";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[5].doc = "";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[5].offset = UTILEX_OFFSETOF(dnx_data_esem_access_cmd_esem_cmd_table_map_t, access_default_result);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_esem_access_cmd_feature_get(
    int unit,
    dnx_data_esem_access_cmd_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, feature);
}


uint32
dnx_data_esem_access_cmd_nof_esem_accesses_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_nof_esem_accesses);
}

uint32
dnx_data_esem_access_cmd_app_di_id_size_in_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_app_di_id_size_in_bits);
}

uint32
dnx_data_esem_access_cmd_default_native_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_default_native);
}

uint32
dnx_data_esem_access_cmd_sflow_sample_interface_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_sflow_sample_interface);
}

uint32
dnx_data_esem_access_cmd_ip_tunnel_no_action_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_ip_tunnel_no_action);
}

uint32
dnx_data_esem_access_cmd_port_svtag_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_port_svtag);
}

uint32
dnx_data_esem_access_cmd_nof_esem_cmd_default_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_nof_esem_cmd_default_profiles);
}

uint32
dnx_data_esem_access_cmd_nof_cmds_size_in_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_nof_cmds_size_in_bits);
}

uint32
dnx_data_esem_access_cmd_nof_cmds_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_nof_cmds);
}

uint32
dnx_data_esem_access_cmd_min_size_for_esem_cmd_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_min_size_for_esem_cmd);
}

uint32
dnx_data_esem_access_cmd_esem_cmd_arr_prefix_gre_any_ipv4_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_gre_any_ipv4);
}

uint32
dnx_data_esem_access_cmd_esem_cmd_arr_prefix_vxlan_gpe_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_vxlan_gpe);
}

uint32
dnx_data_esem_access_cmd_esem_cmd_arr_prefix_ip_tunnel_no_action_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_ip_tunnel_no_action);
}

uint32
dnx_data_esem_access_cmd_esem_cmd_arr_prefix_arp_plus_ac_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_arp_plus_ac);
}

uint32
dnx_data_esem_access_cmd_esem_cmd_predefine_allocations_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_esem_cmd_predefine_allocations);
}

uint32
dnx_data_esem_access_cmd_no_action_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_no_action);
}

uint32
dnx_data_esem_access_cmd_default_ac_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_default_ac);
}

uint32
dnx_data_esem_access_cmd_allocation_mngr_default_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_allocation_mngr_default);
}



const dnx_data_esem_access_cmd_esem_cmd_table_map_t *
dnx_data_esem_access_cmd_esem_cmd_table_map_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_table_esem_cmd_table_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_esem_access_cmd_esem_cmd_table_map_t *) data;

}


shr_error_e
dnx_data_esem_access_cmd_esem_cmd_table_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_esem_access_cmd_esem_cmd_table_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_table_esem_cmd_table_map);
    data = (const dnx_data_esem_access_cmd_esem_cmd_table_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->table_name);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->esem_cmd);
            break;
        case 2:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_ESEM_ACCESS_CMD_NOF_ESEM_ACCESSES, data->access_valid);
            break;
        case 3:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_ESEM_ACCESS_CMD_NOF_ESEM_ACCESSES, data->access_program);
            break;
        case 4:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_ESEM_ACCESS_CMD_NOF_ESEM_ACCESSES, data->access_designated_offset);
            break;
        case 5:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_ESEM_ACCESS_CMD_NOF_ESEM_ACCESSES, data->access_default_result);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_esem_access_cmd_esem_cmd_table_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_table_esem_cmd_table_map);

}






static shr_error_e
dnx_data_esem_default_result_profile_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "default_result_profile";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_esem_default_result_profile_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data esem default_result_profile features");

    submodule_data->features[dnx_data_esem_default_result_profile_default_ac_2tag].name = "default_ac_2tag";
    submodule_data->features[dnx_data_esem_default_result_profile_default_ac_2tag].doc = "";
    submodule_data->features[dnx_data_esem_default_result_profile_default_ac_2tag].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_esem_default_result_profile_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data esem default_result_profile defines");

    submodule_data->defines[dnx_data_esem_default_result_profile_define_default_native].name = "default_native";
    submodule_data->defines[dnx_data_esem_default_result_profile_define_default_native].doc = "";
    
    submodule_data->defines[dnx_data_esem_default_result_profile_define_default_native].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_esem_default_result_profile_define_default_ac].name = "default_ac";
    submodule_data->defines[dnx_data_esem_default_result_profile_define_default_ac].doc = "";
    
    submodule_data->defines[dnx_data_esem_default_result_profile_define_default_ac].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_esem_default_result_profile_define_sflow_sample_interface].name = "sflow_sample_interface";
    submodule_data->defines[dnx_data_esem_default_result_profile_define_sflow_sample_interface].doc = "";
    
    submodule_data->defines[dnx_data_esem_default_result_profile_define_sflow_sample_interface].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_esem_default_result_profile_define_default_dual_homing].name = "default_dual_homing";
    submodule_data->defines[dnx_data_esem_default_result_profile_define_default_dual_homing].doc = "";
    
    submodule_data->defines[dnx_data_esem_default_result_profile_define_default_dual_homing].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_esem_default_result_profile_define_vxlan].name = "vxlan";
    submodule_data->defines[dnx_data_esem_default_result_profile_define_vxlan].doc = "";
    
    submodule_data->defines[dnx_data_esem_default_result_profile_define_vxlan].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_esem_default_result_profile_define_nof_profiles].name = "nof_profiles";
    submodule_data->defines[dnx_data_esem_default_result_profile_define_nof_profiles].doc = "";
    
    submodule_data->defines[dnx_data_esem_default_result_profile_define_nof_profiles].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_esem_default_result_profile_define_nof_allocable_profiles].name = "nof_allocable_profiles";
    submodule_data->defines[dnx_data_esem_default_result_profile_define_nof_allocable_profiles].doc = "";
    
    submodule_data->defines[dnx_data_esem_default_result_profile_define_nof_allocable_profiles].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_esem_default_result_profile_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data esem default_result_profile tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_esem_default_result_profile_feature_get(
    int unit,
    dnx_data_esem_default_result_profile_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_default_result_profile, feature);
}


uint32
dnx_data_esem_default_result_profile_default_native_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_default_result_profile, dnx_data_esem_default_result_profile_define_default_native);
}

uint32
dnx_data_esem_default_result_profile_default_ac_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_default_result_profile, dnx_data_esem_default_result_profile_define_default_ac);
}

uint32
dnx_data_esem_default_result_profile_sflow_sample_interface_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_default_result_profile, dnx_data_esem_default_result_profile_define_sflow_sample_interface);
}

uint32
dnx_data_esem_default_result_profile_default_dual_homing_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_default_result_profile, dnx_data_esem_default_result_profile_define_default_dual_homing);
}

uint32
dnx_data_esem_default_result_profile_vxlan_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_default_result_profile, dnx_data_esem_default_result_profile_define_vxlan);
}

uint32
dnx_data_esem_default_result_profile_nof_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_default_result_profile, dnx_data_esem_default_result_profile_define_nof_profiles);
}

uint32
dnx_data_esem_default_result_profile_nof_allocable_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_default_result_profile, dnx_data_esem_default_result_profile_define_nof_allocable_profiles);
}










static shr_error_e
dnx_data_esem_feature_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "feature";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_esem_feature_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data esem feature features");

    submodule_data->features[dnx_data_esem_feature_etps_properties_assignment].name = "etps_properties_assignment";
    submodule_data->features[dnx_data_esem_feature_etps_properties_assignment].doc = "";
    submodule_data->features[dnx_data_esem_feature_etps_properties_assignment].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_esem_feature_key_with_sspa_contains_member_id].name = "key_with_sspa_contains_member_id";
    submodule_data->features[dnx_data_esem_feature_key_with_sspa_contains_member_id].doc = "";
    submodule_data->features[dnx_data_esem_feature_key_with_sspa_contains_member_id].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_esem_feature_esem_is_dpc].name = "esem_is_dpc";
    submodule_data->features[dnx_data_esem_feature_esem_is_dpc].doc = "";
    submodule_data->features[dnx_data_esem_feature_esem_is_dpc].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_esem_feature_esem_sw_managment].name = "esem_sw_managment";
    submodule_data->features[dnx_data_esem_feature_esem_sw_managment].doc = "";
    submodule_data->features[dnx_data_esem_feature_esem_sw_managment].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_esem_feature_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data esem feature defines");

    submodule_data->defines[dnx_data_esem_feature_define_esem_name_space_nof_bits].name = "esem_name_space_nof_bits";
    submodule_data->defines[dnx_data_esem_feature_define_esem_name_space_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_esem_feature_define_esem_name_space_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_esem_feature_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data esem feature tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_esem_feature_feature_get(
    int unit,
    dnx_data_esem_feature_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_feature, feature);
}


uint32
dnx_data_esem_feature_esem_name_space_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_feature, dnx_data_esem_feature_define_esem_name_space_nof_bits);
}







shr_error_e
dnx_data_esem_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "esem";
    module_data->nof_submodules = _dnx_data_esem_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data esem submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_esem_access_cmd_init(unit, &module_data->submodules[dnx_data_esem_submodule_access_cmd]));
    SHR_IF_ERR_EXIT(dnx_data_esem_default_result_profile_init(unit, &module_data->submodules[dnx_data_esem_submodule_default_result_profile]));
    SHR_IF_ERR_EXIT(dnx_data_esem_feature_init(unit, &module_data->submodules[dnx_data_esem_submodule_feature]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_esem_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_esem_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_esem_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_esem_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_esem_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_esem_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_esem_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_esem_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_esem_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_esem_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_esem_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

