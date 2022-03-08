
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



#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_a0_data_esem_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_b0_data_esem_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2c_a0_data_esem_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e q2a_a0_data_esem_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2p_a0_data_esem_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2x_a0_data_esem_attach(
    int unit);

#endif 



static shr_error_e
dnx_data_esem_access_cmd_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "access_cmd";
    submodule_data->doc = "data for egress sem access command";
    
    submodule_data->nof_features = _dnx_data_esem_access_cmd_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data esem access_cmd features");

    
    submodule_data->nof_defines = _dnx_data_esem_access_cmd_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data esem access_cmd defines");

    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_esem_accesses].name = "nof_esem_accesses";
    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_esem_accesses].doc = "The number of accesses that can be preformed into the ESEM/EXEM.";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_esem_accesses].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_app_di_id_size_in_bits].name = "app_di_id_size_in_bits";
    submodule_data->defines[dnx_data_esem_access_cmd_define_app_di_id_size_in_bits].doc = "NOF bits for the APP DB ID.";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_app_di_id_size_in_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_no_action].name = "no_action";
    submodule_data->defines[dnx_data_esem_access_cmd_define_no_action].doc = "Undefined access command index";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_no_action].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_default_native].name = "default_native";
    submodule_data->defines[dnx_data_esem_access_cmd_define_default_native].doc = "Default access command for native AC";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_default_native].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_default_ac].name = "default_ac";
    submodule_data->defines[dnx_data_esem_access_cmd_define_default_ac].doc = "Default-access command index for outer AC";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_default_ac].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_sflow_sample_interface].name = "sflow_sample_interface";
    submodule_data->defines[dnx_data_esem_access_cmd_define_sflow_sample_interface].doc = "Default-access command index for sflow sample interface";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_sflow_sample_interface].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_ip_tunnel_no_action].name = "ip_tunnel_no_action";
    submodule_data->defines[dnx_data_esem_access_cmd_define_ip_tunnel_no_action].doc = "No lookup access command for ip tunnel cases";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_ip_tunnel_no_action].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_esem_cmd_default_profiles].name = "nof_esem_cmd_default_profiles";
    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_esem_cmd_default_profiles].doc = "Number of esem cmd default profiles";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_esem_cmd_default_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_cmds_size_in_bits].name = "nof_cmds_size_in_bits";
    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_cmds_size_in_bits].doc = "Number of bits for the esem command";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_cmds_size_in_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_cmds].name = "nof_cmds";
    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_cmds].doc = "Number of esem access commands";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_nof_cmds].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_min_size_for_esem_cmd].name = "min_size_for_esem_cmd";
    submodule_data->defines[dnx_data_esem_access_cmd_define_min_size_for_esem_cmd].doc = "The minimal size in bits that is required for a result type to have a holding of ESEM CMD";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_min_size_for_esem_cmd].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_gre_any_ipv4].name = "esem_cmd_arr_prefix_gre_any_ipv4";
    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_gre_any_ipv4].doc = "The ESEM command ARR prefix of an GRE ANY IPv4";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_gre_any_ipv4].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_vxlan_gpe].name = "esem_cmd_arr_prefix_vxlan_gpe";
    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_vxlan_gpe].doc = "The ESEM command ARR prefix of VXLAN GPE";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_vxlan_gpe].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_ip_tunnel_no_action].name = "esem_cmd_arr_prefix_ip_tunnel_no_action";
    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_ip_tunnel_no_action].doc = "The ESEM command ARR prefix of ip tunnel no action";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_ip_tunnel_no_action].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_predefine_allocations].name = "esem_cmd_predefine_allocations";
    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_predefine_allocations].doc = "Indicates whether the predefined default ESEM CMDs are allocated automatically";
    
    submodule_data->defines[dnx_data_esem_access_cmd_define_esem_cmd_predefine_allocations].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_esem_access_cmd_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data esem access_cmd tables");

    
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].name = "esem_cmd_table_map";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].doc = "ESEM CMD table mapping";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].size_of_values = sizeof(dnx_data_esem_access_cmd_esem_cmd_table_map_t);
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].entry_get = dnx_data_esem_access_cmd_esem_cmd_table_map_entry_str_get;

    
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].nof_keys = 0;

    
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].nof_values = 6;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].nof_values, "_dnx_data_esem_access_cmd_table_esem_cmd_table_map table values");
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[0].name = "table_name";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[0].doc = "ESEM CMD table name";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_esem_access_cmd_esem_cmd_table_map_t, table_name);
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[1].name = "esem_cmd";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[1].type = "dbal_fields_e";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[1].doc = "ESEM CMD profile filed value";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[1].offset = UTILEX_OFFSETOF(dnx_data_esem_access_cmd_esem_cmd_table_map_t, esem_cmd);
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[2].name = "access_valid";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[2].type = "dbal_fields_e[DNX_DATA_MAX_ESEM_ACCESS_CMD_NOF_ESEM_ACCESSES]";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[2].doc = "array of ESEM CMD valid filed value for eache ESEM access";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[2].offset = UTILEX_OFFSETOF(dnx_data_esem_access_cmd_esem_cmd_table_map_t, access_valid);
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[3].name = "access_program";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[3].type = "dbal_fields_e[DNX_DATA_MAX_ESEM_ACCESS_CMD_NOF_ESEM_ACCESSES]";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[3].doc = "array of ESEM CMD program filed value for eache ESEM access";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[3].offset = UTILEX_OFFSETOF(dnx_data_esem_access_cmd_esem_cmd_table_map_t, access_program);
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[4].name = "access_designated_offset";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[4].type = "dbal_fields_e[DNX_DATA_MAX_ESEM_ACCESS_CMD_NOF_ESEM_ACCESSES]";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[4].doc = "array of ESEM CMD designated offset filed value for eache ESEM access";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[4].offset = UTILEX_OFFSETOF(dnx_data_esem_access_cmd_esem_cmd_table_map_t, access_designated_offset);
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[5].name = "access_default_result";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[5].type = "dbal_fields_e[DNX_DATA_MAX_ESEM_ACCESS_CMD_NOF_ESEM_ACCESSES]";
    submodule_data->tables[dnx_data_esem_access_cmd_table_esem_cmd_table_map].values[5].doc = "array of ESEM CMD default result filed value for eache ESEM access";
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
dnx_data_esem_access_cmd_no_action_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_no_action);
}

uint32
dnx_data_esem_access_cmd_default_native_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_default_native);
}

uint32
dnx_data_esem_access_cmd_default_ac_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_default_ac);
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
dnx_data_esem_access_cmd_esem_cmd_predefine_allocations_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_esem, dnx_data_esem_submodule_access_cmd, dnx_data_esem_access_cmd_define_esem_cmd_predefine_allocations);
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
    submodule_data->doc = "default results in case of mismatch in esem lookup";
    
    submodule_data->nof_features = _dnx_data_esem_default_result_profile_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data esem default_result_profile features");

    submodule_data->features[dnx_data_esem_default_result_profile_default_ac_2tag].name = "default_ac_2tag";
    submodule_data->features[dnx_data_esem_default_result_profile_default_ac_2tag].doc = "True if ESEM default_result_profile for AC has two tags";
    submodule_data->features[dnx_data_esem_default_result_profile_default_ac_2tag].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_esem_default_result_profile_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data esem default_result_profile defines");

    submodule_data->defines[dnx_data_esem_default_result_profile_define_default_native].name = "default_native";
    submodule_data->defines[dnx_data_esem_default_result_profile_define_default_native].doc = "Profile for Esem default result for default native";
    
    submodule_data->defines[dnx_data_esem_default_result_profile_define_default_native].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_default_result_profile_define_default_ac].name = "default_ac";
    submodule_data->defines[dnx_data_esem_default_result_profile_define_default_ac].doc = "Profile for Esem default result for default AC";
    
    submodule_data->defines[dnx_data_esem_default_result_profile_define_default_ac].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_default_result_profile_define_sflow_sample_interface].name = "sflow_sample_interface";
    submodule_data->defines[dnx_data_esem_default_result_profile_define_sflow_sample_interface].doc = "Profile for Esem default result for sflow sample interface";
    
    submodule_data->defines[dnx_data_esem_default_result_profile_define_sflow_sample_interface].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_default_result_profile_define_default_dual_homing].name = "default_dual_homing";
    submodule_data->defines[dnx_data_esem_default_result_profile_define_default_dual_homing].doc = "Profile for Esem default result for dual homing db";
    
    submodule_data->defines[dnx_data_esem_default_result_profile_define_default_dual_homing].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_default_result_profile_define_vxlan].name = "vxlan";
    submodule_data->defines[dnx_data_esem_default_result_profile_define_vxlan].doc = "Profile for Esem default result for vxlan";
    
    submodule_data->defines[dnx_data_esem_default_result_profile_define_vxlan].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_default_result_profile_define_nof_profiles].name = "nof_profiles";
    submodule_data->defines[dnx_data_esem_default_result_profile_define_nof_profiles].doc = "Number of esem default result profiles";
    
    submodule_data->defines[dnx_data_esem_default_result_profile_define_nof_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_esem_default_result_profile_define_nof_allocable_profiles].name = "nof_allocable_profiles";
    submodule_data->defines[dnx_data_esem_default_result_profile_define_nof_allocable_profiles].doc = "Number of esem default result profiles exposed to users";
    
    submodule_data->defines[dnx_data_esem_default_result_profile_define_nof_allocable_profiles].flags |= DNXC_DATA_F_DEFINE;

    
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
    submodule_data->features[dnx_data_esem_feature_etps_properties_assignment].doc = "Properties are taken from ETPS TOS entry, instead of EES TOS entry";
    submodule_data->features[dnx_data_esem_feature_etps_properties_assignment].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_esem_feature_key_with_sspa_contains_member_id].name = "key_with_sspa_contains_member_id";
    submodule_data->features[dnx_data_esem_feature_key_with_sspa_contains_member_id].doc = "Does the constructed key contain member-id in case of trunk system port aggregate";
    submodule_data->features[dnx_data_esem_feature_key_with_sspa_contains_member_id].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_esem_feature_esem_is_dpc].name = "esem_is_dpc";
    submodule_data->features[dnx_data_esem_feature_esem_is_dpc].doc = "True if ESEM is DPC";
    submodule_data->features[dnx_data_esem_feature_esem_is_dpc].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_esem_feature_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data esem feature defines");

    
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
    
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_esem_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_esem_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_esem_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_esem_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_esem_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_esem_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_esem_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_esem_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_esem_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_esem_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_esem_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_esem_attach(unit));
    }
    else

#endif 
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

