

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MACSEC

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_macsec.h>



extern shr_error_e jr2_a0_data_macsec_attach(
    int unit);
extern shr_error_e j2p_a0_data_macsec_attach(
    int unit);



static shr_error_e
dnx_data_macsec_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_macsec_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data macsec general features");

    submodule_data->features[dnx_data_macsec_general_is_macsec_supported].name = "is_macsec_supported";
    submodule_data->features[dnx_data_macsec_general_is_macsec_supported].doc = "";
    submodule_data->features[dnx_data_macsec_general_is_macsec_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_macsec_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data macsec general defines");

    submodule_data->defines[dnx_data_macsec_general_define_macsec_nof].name = "macsec_nof";
    submodule_data->defines[dnx_data_macsec_general_define_macsec_nof].doc = "";
    
    submodule_data->defines[dnx_data_macsec_general_define_macsec_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_macsec_general_define_macsec_nof_bits].name = "macsec_nof_bits";
    submodule_data->defines[dnx_data_macsec_general_define_macsec_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_macsec_general_define_macsec_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_macsec_general_define_ports_in_macsec_nof].name = "ports_in_macsec_nof";
    submodule_data->defines[dnx_data_macsec_general_define_ports_in_macsec_nof].doc = "";
    
    submodule_data->defines[dnx_data_macsec_general_define_ports_in_macsec_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_macsec_general_define_ports_in_macsec_nof_bits].name = "ports_in_macsec_nof_bits";
    submodule_data->defines[dnx_data_macsec_general_define_ports_in_macsec_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_macsec_general_define_ports_in_macsec_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_macsec_general_define_etype_nof].name = "etype_nof";
    submodule_data->defines[dnx_data_macsec_general_define_etype_nof].doc = "";
    
    submodule_data->defines[dnx_data_macsec_general_define_etype_nof].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_macsec_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data macsec general tables");

    
    submodule_data->tables[dnx_data_macsec_general_table_pm_to_macsec].name = "pm_to_macsec";
    submodule_data->tables[dnx_data_macsec_general_table_pm_to_macsec].doc = "";
    submodule_data->tables[dnx_data_macsec_general_table_pm_to_macsec].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_macsec_general_table_pm_to_macsec].size_of_values = sizeof(dnx_data_macsec_general_pm_to_macsec_t);
    submodule_data->tables[dnx_data_macsec_general_table_pm_to_macsec].entry_get = dnx_data_macsec_general_pm_to_macsec_entry_str_get;

    
    submodule_data->tables[dnx_data_macsec_general_table_pm_to_macsec].nof_keys = 1;
    submodule_data->tables[dnx_data_macsec_general_table_pm_to_macsec].keys[0].name = "ethu_index";
    submodule_data->tables[dnx_data_macsec_general_table_pm_to_macsec].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_macsec_general_table_pm_to_macsec].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_macsec_general_table_pm_to_macsec].values, dnxc_data_value_t, submodule_data->tables[dnx_data_macsec_general_table_pm_to_macsec].nof_values, "_dnx_data_macsec_general_table_pm_to_macsec table values");
    submodule_data->tables[dnx_data_macsec_general_table_pm_to_macsec].values[0].name = "macsec_id";
    submodule_data->tables[dnx_data_macsec_general_table_pm_to_macsec].values[0].type = "uint32";
    submodule_data->tables[dnx_data_macsec_general_table_pm_to_macsec].values[0].doc = "";
    submodule_data->tables[dnx_data_macsec_general_table_pm_to_macsec].values[0].offset = UTILEX_OFFSETOF(dnx_data_macsec_general_pm_to_macsec_t, macsec_id);

    
    submodule_data->tables[dnx_data_macsec_general_table_macsec_instances].name = "macsec_instances";
    submodule_data->tables[dnx_data_macsec_general_table_macsec_instances].doc = "";
    submodule_data->tables[dnx_data_macsec_general_table_macsec_instances].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_macsec_general_table_macsec_instances].size_of_values = sizeof(dnx_data_macsec_general_macsec_instances_t);
    submodule_data->tables[dnx_data_macsec_general_table_macsec_instances].entry_get = dnx_data_macsec_general_macsec_instances_entry_str_get;

    
    submodule_data->tables[dnx_data_macsec_general_table_macsec_instances].nof_keys = 1;
    submodule_data->tables[dnx_data_macsec_general_table_macsec_instances].keys[0].name = "macsec_id";
    submodule_data->tables[dnx_data_macsec_general_table_macsec_instances].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_macsec_general_table_macsec_instances].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_macsec_general_table_macsec_instances].values, dnxc_data_value_t, submodule_data->tables[dnx_data_macsec_general_table_macsec_instances].nof_values, "_dnx_data_macsec_general_table_macsec_instances table values");
    submodule_data->tables[dnx_data_macsec_general_table_macsec_instances].values[0].name = "nof_pms";
    submodule_data->tables[dnx_data_macsec_general_table_macsec_instances].values[0].type = "uint32";
    submodule_data->tables[dnx_data_macsec_general_table_macsec_instances].values[0].doc = "";
    submodule_data->tables[dnx_data_macsec_general_table_macsec_instances].values[0].offset = UTILEX_OFFSETOF(dnx_data_macsec_general_macsec_instances_t, nof_pms);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_macsec_general_feature_get(
    int unit,
    dnx_data_macsec_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_general, feature);
}


uint32
dnx_data_macsec_general_macsec_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_general, dnx_data_macsec_general_define_macsec_nof);
}

uint32
dnx_data_macsec_general_macsec_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_general, dnx_data_macsec_general_define_macsec_nof_bits);
}

uint32
dnx_data_macsec_general_ports_in_macsec_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_general, dnx_data_macsec_general_define_ports_in_macsec_nof);
}

uint32
dnx_data_macsec_general_ports_in_macsec_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_general, dnx_data_macsec_general_define_ports_in_macsec_nof_bits);
}

uint32
dnx_data_macsec_general_etype_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_general, dnx_data_macsec_general_define_etype_nof);
}



const dnx_data_macsec_general_pm_to_macsec_t *
dnx_data_macsec_general_pm_to_macsec_get(
    int unit,
    int ethu_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_general, dnx_data_macsec_general_table_pm_to_macsec);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ethu_index, 0);
    return (const dnx_data_macsec_general_pm_to_macsec_t *) data;

}

const dnx_data_macsec_general_macsec_instances_t *
dnx_data_macsec_general_macsec_instances_get(
    int unit,
    int macsec_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_general, dnx_data_macsec_general_table_macsec_instances);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, macsec_id, 0);
    return (const dnx_data_macsec_general_macsec_instances_t *) data;

}


shr_error_e
dnx_data_macsec_general_pm_to_macsec_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_macsec_general_pm_to_macsec_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_general, dnx_data_macsec_general_table_pm_to_macsec);
    data = (const dnx_data_macsec_general_pm_to_macsec_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->macsec_id);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_macsec_general_macsec_instances_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_macsec_general_macsec_instances_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_general, dnx_data_macsec_general_table_macsec_instances);
    data = (const dnx_data_macsec_general_macsec_instances_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_pms);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_macsec_general_pm_to_macsec_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_general, dnx_data_macsec_general_table_pm_to_macsec);

}

const dnxc_data_table_info_t *
dnx_data_macsec_general_macsec_instances_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_general, dnx_data_macsec_general_table_macsec_instances);

}






static shr_error_e
dnx_data_macsec_ingress_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ingress";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_macsec_ingress_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data macsec ingress features");

    
    submodule_data->nof_defines = _dnx_data_macsec_ingress_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data macsec ingress defines");

    submodule_data->defines[dnx_data_macsec_ingress_define_flow_nof].name = "flow_nof";
    submodule_data->defines[dnx_data_macsec_ingress_define_flow_nof].doc = "";
    
    submodule_data->defines[dnx_data_macsec_ingress_define_flow_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_macsec_ingress_define_policy_nof].name = "policy_nof";
    submodule_data->defines[dnx_data_macsec_ingress_define_policy_nof].doc = "";
    
    submodule_data->defines[dnx_data_macsec_ingress_define_policy_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_macsec_ingress_define_secure_channel_nof].name = "secure_channel_nof";
    submodule_data->defines[dnx_data_macsec_ingress_define_secure_channel_nof].doc = "";
    
    submodule_data->defines[dnx_data_macsec_ingress_define_secure_channel_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_macsec_ingress_define_secure_assoc_nof].name = "secure_assoc_nof";
    submodule_data->defines[dnx_data_macsec_ingress_define_secure_assoc_nof].doc = "";
    
    submodule_data->defines[dnx_data_macsec_ingress_define_secure_assoc_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_macsec_ingress_define_udf_nof_bits].name = "udf_nof_bits";
    submodule_data->defines[dnx_data_macsec_ingress_define_udf_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_macsec_ingress_define_udf_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_macsec_ingress_define_mgmt_rule_exact_nof].name = "mgmt_rule_exact_nof";
    submodule_data->defines[dnx_data_macsec_ingress_define_mgmt_rule_exact_nof].doc = "";
    
    submodule_data->defines[dnx_data_macsec_ingress_define_mgmt_rule_exact_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_macsec_ingress_define_tpid_nof].name = "tpid_nof";
    submodule_data->defines[dnx_data_macsec_ingress_define_tpid_nof].doc = "";
    
    submodule_data->defines[dnx_data_macsec_ingress_define_tpid_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_macsec_ingress_define_cpu_flex_map_nof].name = "cpu_flex_map_nof";
    submodule_data->defines[dnx_data_macsec_ingress_define_cpu_flex_map_nof].doc = "";
    
    submodule_data->defines[dnx_data_macsec_ingress_define_cpu_flex_map_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_macsec_ingress_define_sc_tcam_nof].name = "sc_tcam_nof";
    submodule_data->defines[dnx_data_macsec_ingress_define_sc_tcam_nof].doc = "";
    
    submodule_data->defines[dnx_data_macsec_ingress_define_sc_tcam_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_macsec_ingress_define_sa_per_sc_nof].name = "sa_per_sc_nof";
    submodule_data->defines[dnx_data_macsec_ingress_define_sa_per_sc_nof].doc = "";
    
    submodule_data->defines[dnx_data_macsec_ingress_define_sa_per_sc_nof].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_macsec_ingress_define_invalidate_sa].name = "invalidate_sa";
    submodule_data->defines[dnx_data_macsec_ingress_define_invalidate_sa].doc = "";
    
    submodule_data->defines[dnx_data_macsec_ingress_define_invalidate_sa].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_macsec_ingress_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data macsec ingress tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_macsec_ingress_feature_get(
    int unit,
    dnx_data_macsec_ingress_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_ingress, feature);
}


uint32
dnx_data_macsec_ingress_flow_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_ingress, dnx_data_macsec_ingress_define_flow_nof);
}

uint32
dnx_data_macsec_ingress_policy_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_ingress, dnx_data_macsec_ingress_define_policy_nof);
}

uint32
dnx_data_macsec_ingress_secure_channel_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_ingress, dnx_data_macsec_ingress_define_secure_channel_nof);
}

uint32
dnx_data_macsec_ingress_secure_assoc_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_ingress, dnx_data_macsec_ingress_define_secure_assoc_nof);
}

uint32
dnx_data_macsec_ingress_udf_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_ingress, dnx_data_macsec_ingress_define_udf_nof_bits);
}

uint32
dnx_data_macsec_ingress_mgmt_rule_exact_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_ingress, dnx_data_macsec_ingress_define_mgmt_rule_exact_nof);
}

uint32
dnx_data_macsec_ingress_tpid_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_ingress, dnx_data_macsec_ingress_define_tpid_nof);
}

uint32
dnx_data_macsec_ingress_cpu_flex_map_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_ingress, dnx_data_macsec_ingress_define_cpu_flex_map_nof);
}

uint32
dnx_data_macsec_ingress_sc_tcam_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_ingress, dnx_data_macsec_ingress_define_sc_tcam_nof);
}

uint32
dnx_data_macsec_ingress_sa_per_sc_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_ingress, dnx_data_macsec_ingress_define_sa_per_sc_nof);
}

uint32
dnx_data_macsec_ingress_invalidate_sa_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_ingress, dnx_data_macsec_ingress_define_invalidate_sa);
}










static shr_error_e
dnx_data_macsec_egress_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "egress";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_macsec_egress_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data macsec egress features");

    
    submodule_data->nof_defines = _dnx_data_macsec_egress_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data macsec egress defines");

    submodule_data->defines[dnx_data_macsec_egress_define_secure_channel_nof].name = "secure_channel_nof";
    submodule_data->defines[dnx_data_macsec_egress_define_secure_channel_nof].doc = "";
    
    submodule_data->defines[dnx_data_macsec_egress_define_secure_channel_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_macsec_egress_define_secure_assoc_nof].name = "secure_assoc_nof";
    submodule_data->defines[dnx_data_macsec_egress_define_secure_assoc_nof].doc = "";
    
    submodule_data->defines[dnx_data_macsec_egress_define_secure_assoc_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_macsec_egress_define_soft_expiry_threshold_profiles_nof].name = "soft_expiry_threshold_profiles_nof";
    submodule_data->defines[dnx_data_macsec_egress_define_soft_expiry_threshold_profiles_nof].doc = "";
    
    submodule_data->defines[dnx_data_macsec_egress_define_soft_expiry_threshold_profiles_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_macsec_egress_define_sa_per_sc_nof].name = "sa_per_sc_nof";
    submodule_data->defines[dnx_data_macsec_egress_define_sa_per_sc_nof].doc = "";
    
    submodule_data->defines[dnx_data_macsec_egress_define_sa_per_sc_nof].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_macsec_egress_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data macsec egress tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_macsec_egress_feature_get(
    int unit,
    dnx_data_macsec_egress_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_egress, feature);
}


uint32
dnx_data_macsec_egress_secure_channel_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_egress, dnx_data_macsec_egress_define_secure_channel_nof);
}

uint32
dnx_data_macsec_egress_secure_assoc_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_egress, dnx_data_macsec_egress_define_secure_assoc_nof);
}

uint32
dnx_data_macsec_egress_soft_expiry_threshold_profiles_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_egress, dnx_data_macsec_egress_define_soft_expiry_threshold_profiles_nof);
}

uint32
dnx_data_macsec_egress_sa_per_sc_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_macsec, dnx_data_macsec_submodule_egress, dnx_data_macsec_egress_define_sa_per_sc_nof);
}







shr_error_e
dnx_data_macsec_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "macsec";
    module_data->nof_submodules = _dnx_data_macsec_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data macsec submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_macsec_general_init(unit, &module_data->submodules[dnx_data_macsec_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_macsec_ingress_init(unit, &module_data->submodules[dnx_data_macsec_submodule_ingress]));
    SHR_IF_ERR_EXIT(dnx_data_macsec_egress_init(unit, &module_data->submodules[dnx_data_macsec_submodule_egress]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_macsec_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_macsec_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_macsec_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_macsec_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_macsec_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_macsec_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_macsec_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_macsec_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_macsec_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_macsec_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

