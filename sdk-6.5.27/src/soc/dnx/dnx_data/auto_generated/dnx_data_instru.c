
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INSTRU

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_instru.h>




extern shr_error_e jr2_a0_data_instru_attach(
    int unit);


extern shr_error_e jr2_b0_data_instru_attach(
    int unit);


extern shr_error_e j2c_a0_data_instru_attach(
    int unit);


extern shr_error_e j2c_a1_data_instru_attach(
    int unit);


extern shr_error_e q2a_a0_data_instru_attach(
    int unit);


extern shr_error_e j2p_a0_data_instru_attach(
    int unit);


extern shr_error_e j2x_a0_data_instru_attach(
    int unit);




static shr_error_e
dnx_data_instru_ipt_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ipt";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_instru_ipt_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data instru ipt features");

    submodule_data->features[dnx_data_instru_ipt_advanced_ipt].name = "advanced_ipt";
    submodule_data->features[dnx_data_instru_ipt_advanced_ipt].doc = "";
    submodule_data->features[dnx_data_instru_ipt_advanced_ipt].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_instru_ipt_ipt_mapping_v2].name = "ipt_mapping_v2";
    submodule_data->features[dnx_data_instru_ipt_ipt_mapping_v2].doc = "";
    submodule_data->features[dnx_data_instru_ipt_ipt_mapping_v2].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_instru_ipt_estimated_encap_size].name = "estimated_encap_size";
    submodule_data->features[dnx_data_instru_ipt_estimated_encap_size].doc = "";
    submodule_data->features[dnx_data_instru_ipt_estimated_encap_size].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_instru_ipt_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data instru ipt defines");

    submodule_data->defines[dnx_data_instru_ipt_define_profile_size].name = "profile_size";
    submodule_data->defines[dnx_data_instru_ipt_define_profile_size].doc = "";
    
    submodule_data->defines[dnx_data_instru_ipt_define_profile_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_ipt_define_metadata_bitmap_size].name = "metadata_bitmap_size";
    submodule_data->defines[dnx_data_instru_ipt_define_metadata_bitmap_size].doc = "";
    
    submodule_data->defines[dnx_data_instru_ipt_define_metadata_bitmap_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_ipt_define_metadata_edit_size].name = "metadata_edit_size";
    submodule_data->defines[dnx_data_instru_ipt_define_metadata_edit_size].doc = "";
    
    submodule_data->defines[dnx_data_instru_ipt_define_metadata_edit_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_ipt_define_node_id_padding_size].name = "node_id_padding_size";
    submodule_data->defines[dnx_data_instru_ipt_define_node_id_padding_size].doc = "";
    
    submodule_data->defines[dnx_data_instru_ipt_define_node_id_padding_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_ipt_define_telemetry_queue_occ_check].name = "telemetry_queue_occ_check";
    submodule_data->defines[dnx_data_instru_ipt_define_telemetry_queue_occ_check].doc = "";
    
    submodule_data->defines[dnx_data_instru_ipt_define_telemetry_queue_occ_check].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_ipt_define_telemetry_queue_occ_max].name = "telemetry_queue_occ_max";
    submodule_data->defines[dnx_data_instru_ipt_define_telemetry_queue_occ_max].doc = "";
    
    submodule_data->defines[dnx_data_instru_ipt_define_telemetry_queue_occ_max].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_instru_ipt_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data instru ipt tables");

    
    submodule_data->tables[dnx_data_instru_ipt_table_profile_info].name = "profile_info";
    submodule_data->tables[dnx_data_instru_ipt_table_profile_info].doc = "";
    submodule_data->tables[dnx_data_instru_ipt_table_profile_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_instru_ipt_table_profile_info].size_of_values = sizeof(dnx_data_instru_ipt_profile_info_t);
    submodule_data->tables[dnx_data_instru_ipt_table_profile_info].entry_get = dnx_data_instru_ipt_profile_info_entry_str_get;

    
    submodule_data->tables[dnx_data_instru_ipt_table_profile_info].nof_keys = 1;
    submodule_data->tables[dnx_data_instru_ipt_table_profile_info].keys[0].name = "profile_id";
    submodule_data->tables[dnx_data_instru_ipt_table_profile_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_instru_ipt_table_profile_info].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_instru_ipt_table_profile_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_instru_ipt_table_profile_info].nof_values, "_dnx_data_instru_ipt_table_profile_info table values");
    submodule_data->tables[dnx_data_instru_ipt_table_profile_info].values[0].name = "type";
    submodule_data->tables[dnx_data_instru_ipt_table_profile_info].values[0].type = "bcm_instru_ipt_node_type_t";
    submodule_data->tables[dnx_data_instru_ipt_table_profile_info].values[0].doc = "";
    submodule_data->tables[dnx_data_instru_ipt_table_profile_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_instru_ipt_profile_info_t, type);

    
    submodule_data->tables[dnx_data_instru_ipt_table_metadata].name = "metadata";
    submodule_data->tables[dnx_data_instru_ipt_table_metadata].doc = "";
    submodule_data->tables[dnx_data_instru_ipt_table_metadata].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_instru_ipt_table_metadata].size_of_values = sizeof(dnx_data_instru_ipt_metadata_t);
    submodule_data->tables[dnx_data_instru_ipt_table_metadata].entry_get = dnx_data_instru_ipt_metadata_entry_str_get;

    
    submodule_data->tables[dnx_data_instru_ipt_table_metadata].nof_keys = 1;
    submodule_data->tables[dnx_data_instru_ipt_table_metadata].keys[0].name = "flag_id";
    submodule_data->tables[dnx_data_instru_ipt_table_metadata].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_instru_ipt_table_metadata].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_instru_ipt_table_metadata].values, dnxc_data_value_t, submodule_data->tables[dnx_data_instru_ipt_table_metadata].nof_values, "_dnx_data_instru_ipt_table_metadata table values");
    submodule_data->tables[dnx_data_instru_ipt_table_metadata].values[0].name = "bit";
    submodule_data->tables[dnx_data_instru_ipt_table_metadata].values[0].type = "uint32";
    submodule_data->tables[dnx_data_instru_ipt_table_metadata].values[0].doc = "";
    submodule_data->tables[dnx_data_instru_ipt_table_metadata].values[0].offset = UTILEX_OFFSETOF(dnx_data_instru_ipt_metadata_t, bit);
    submodule_data->tables[dnx_data_instru_ipt_table_metadata].values[1].name = "size";
    submodule_data->tables[dnx_data_instru_ipt_table_metadata].values[1].type = "uint32";
    submodule_data->tables[dnx_data_instru_ipt_table_metadata].values[1].doc = "";
    submodule_data->tables[dnx_data_instru_ipt_table_metadata].values[1].offset = UTILEX_OFFSETOF(dnx_data_instru_ipt_metadata_t, size);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_instru_ipt_feature_get(
    int unit,
    dnx_data_instru_ipt_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_ipt, feature);
}


uint32
dnx_data_instru_ipt_profile_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_ipt, dnx_data_instru_ipt_define_profile_size);
}

uint32
dnx_data_instru_ipt_metadata_bitmap_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_ipt, dnx_data_instru_ipt_define_metadata_bitmap_size);
}

uint32
dnx_data_instru_ipt_metadata_edit_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_ipt, dnx_data_instru_ipt_define_metadata_edit_size);
}

uint32
dnx_data_instru_ipt_node_id_padding_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_ipt, dnx_data_instru_ipt_define_node_id_padding_size);
}

uint32
dnx_data_instru_ipt_telemetry_queue_occ_check_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_ipt, dnx_data_instru_ipt_define_telemetry_queue_occ_check);
}

uint32
dnx_data_instru_ipt_telemetry_queue_occ_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_ipt, dnx_data_instru_ipt_define_telemetry_queue_occ_max);
}



const dnx_data_instru_ipt_profile_info_t *
dnx_data_instru_ipt_profile_info_get(
    int unit,
    int profile_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_ipt, dnx_data_instru_ipt_table_profile_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, profile_id, 0);
    return (const dnx_data_instru_ipt_profile_info_t *) data;

}

const dnx_data_instru_ipt_metadata_t *
dnx_data_instru_ipt_metadata_get(
    int unit,
    int flag_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_ipt, dnx_data_instru_ipt_table_metadata);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, flag_id, 0);
    return (const dnx_data_instru_ipt_metadata_t *) data;

}


shr_error_e
dnx_data_instru_ipt_profile_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_instru_ipt_profile_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_ipt, dnx_data_instru_ipt_table_profile_info);
    data = (const dnx_data_instru_ipt_profile_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->type);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_instru_ipt_metadata_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_instru_ipt_metadata_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_ipt, dnx_data_instru_ipt_table_metadata);
    data = (const dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->bit);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->size);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_instru_ipt_profile_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_ipt, dnx_data_instru_ipt_table_profile_info);

}

const dnxc_data_table_info_t *
dnx_data_instru_ipt_metadata_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_ipt, dnx_data_instru_ipt_table_metadata);

}






static shr_error_e
dnx_data_instru_sflow_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "sflow";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_instru_sflow_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data instru sflow features");

    submodule_data->features[dnx_data_instru_sflow_hard_logic_supported].name = "hard_logic_supported";
    submodule_data->features[dnx_data_instru_sflow_hard_logic_supported].doc = "";
    submodule_data->features[dnx_data_instru_sflow_hard_logic_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_instru_sflow_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data instru sflow defines");

    submodule_data->defines[dnx_data_instru_sflow_define_max_nof_sflow_encaps].name = "max_nof_sflow_encaps";
    submodule_data->defines[dnx_data_instru_sflow_define_max_nof_sflow_encaps].doc = "";
    
    submodule_data->defines[dnx_data_instru_sflow_define_max_nof_sflow_encaps].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_sflow_define_nof_sflow_raw_entries_per_stack].name = "nof_sflow_raw_entries_per_stack";
    submodule_data->defines[dnx_data_instru_sflow_define_nof_sflow_raw_entries_per_stack].doc = "";
    
    submodule_data->defines[dnx_data_instru_sflow_define_nof_sflow_raw_entries_per_stack].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_instru_sflow_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data instru sflow tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_instru_sflow_feature_get(
    int unit,
    dnx_data_instru_sflow_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_sflow, feature);
}


uint32
dnx_data_instru_sflow_max_nof_sflow_encaps_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_sflow, dnx_data_instru_sflow_define_max_nof_sflow_encaps);
}

uint32
dnx_data_instru_sflow_nof_sflow_raw_entries_per_stack_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_sflow, dnx_data_instru_sflow_define_nof_sflow_raw_entries_per_stack);
}










static shr_error_e
dnx_data_instru_ifa_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ifa";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_instru_ifa_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data instru ifa features");

    
    submodule_data->nof_defines = _dnx_data_instru_ifa_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data instru ifa defines");

    submodule_data->defines[dnx_data_instru_ifa_define_ingress_tod_feature].name = "ingress_tod_feature";
    submodule_data->defines[dnx_data_instru_ifa_define_ingress_tod_feature].doc = "";
    
    submodule_data->defines[dnx_data_instru_ifa_define_ingress_tod_feature].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_ifa_define_max_lenght_check_for_ifa2].name = "max_lenght_check_for_ifa2";
    submodule_data->defines[dnx_data_instru_ifa_define_max_lenght_check_for_ifa2].doc = "";
    
    submodule_data->defines[dnx_data_instru_ifa_define_max_lenght_check_for_ifa2].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_ifa_define_ifa2_is_in_hl].name = "ifa2_is_in_hl";
    submodule_data->defines[dnx_data_instru_ifa_define_ifa2_is_in_hl].doc = "";
    
    submodule_data->defines[dnx_data_instru_ifa_define_ifa2_is_in_hl].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_ifa_define_ifa2_is_supported].name = "ifa2_is_supported";
    submodule_data->defines[dnx_data_instru_ifa_define_ifa2_is_supported].doc = "";
    
    submodule_data->defines[dnx_data_instru_ifa_define_ifa2_is_supported].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_instru_ifa_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data instru ifa tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_instru_ifa_feature_get(
    int unit,
    dnx_data_instru_ifa_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_ifa, feature);
}


uint32
dnx_data_instru_ifa_ingress_tod_feature_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_ifa, dnx_data_instru_ifa_define_ingress_tod_feature);
}

uint32
dnx_data_instru_ifa_max_lenght_check_for_ifa2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_ifa, dnx_data_instru_ifa_define_max_lenght_check_for_ifa2);
}

uint32
dnx_data_instru_ifa_ifa2_is_in_hl_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_ifa, dnx_data_instru_ifa_define_ifa2_is_in_hl);
}

uint32
dnx_data_instru_ifa_ifa2_is_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_ifa, dnx_data_instru_ifa_define_ifa2_is_supported);
}










static shr_error_e
dnx_data_instru_ipfix_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ipfix";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_instru_ipfix_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data instru ipfix features");

    
    submodule_data->nof_defines = _dnx_data_instru_ipfix_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data instru ipfix defines");

    submodule_data->defines[dnx_data_instru_ipfix_define_ipfix_v2].name = "ipfix_v2";
    submodule_data->defines[dnx_data_instru_ipfix_define_ipfix_v2].doc = "";
    
    submodule_data->defines[dnx_data_instru_ipfix_define_ipfix_v2].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_instru_ipfix_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data instru ipfix tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_instru_ipfix_feature_get(
    int unit,
    dnx_data_instru_ipfix_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_ipfix, feature);
}


uint32
dnx_data_instru_ipfix_ipfix_v2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_ipfix, dnx_data_instru_ipfix_define_ipfix_v2);
}










static shr_error_e
dnx_data_instru_hbh_pt_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "hbh_pt";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_instru_hbh_pt_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data instru hbh_pt features");

    
    submodule_data->nof_defines = _dnx_data_instru_hbh_pt_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data instru hbh_pt defines");

    submodule_data->defines[dnx_data_instru_hbh_pt_define_Ingress_1588_ToD_LSB_size].name = "Ingress_1588_ToD_LSB_size";
    submodule_data->defines[dnx_data_instru_hbh_pt_define_Ingress_1588_ToD_LSB_size].doc = "";
    
    submodule_data->defines[dnx_data_instru_hbh_pt_define_Ingress_1588_ToD_LSB_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_instru_hbh_pt_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data instru hbh_pt tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_instru_hbh_pt_feature_get(
    int unit,
    dnx_data_instru_hbh_pt_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_hbh_pt, feature);
}


uint32
dnx_data_instru_hbh_pt_Ingress_1588_ToD_LSB_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_hbh_pt, dnx_data_instru_hbh_pt_define_Ingress_1588_ToD_LSB_size);
}










static shr_error_e
dnx_data_instru_eventor_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "eventor";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_instru_eventor_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data instru eventor features");

    submodule_data->features[dnx_data_instru_eventor_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_instru_eventor_is_supported].doc = "";
    submodule_data->features[dnx_data_instru_eventor_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_instru_eventor_is_periodic_events_supported].name = "is_periodic_events_supported";
    submodule_data->features[dnx_data_instru_eventor_is_periodic_events_supported].doc = "";
    submodule_data->features[dnx_data_instru_eventor_is_periodic_events_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_instru_eventor_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data instru eventor defines");

    submodule_data->defines[dnx_data_instru_eventor_define_rx_evpck_header_size].name = "rx_evpck_header_size";
    submodule_data->defines[dnx_data_instru_eventor_define_rx_evpck_header_size].doc = "";
    
    submodule_data->defines[dnx_data_instru_eventor_define_rx_evpck_header_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_eventor_define_tx_builder_max_header_size].name = "tx_builder_max_header_size";
    submodule_data->defines[dnx_data_instru_eventor_define_tx_builder_max_header_size].doc = "";
    
    submodule_data->defines[dnx_data_instru_eventor_define_tx_builder_max_header_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_eventor_define_nof_contexts].name = "nof_contexts";
    submodule_data->defines[dnx_data_instru_eventor_define_nof_contexts].doc = "";
    
    submodule_data->defines[dnx_data_instru_eventor_define_nof_contexts].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_eventor_define_nof_contexts_bits].name = "nof_contexts_bits";
    submodule_data->defines[dnx_data_instru_eventor_define_nof_contexts_bits].doc = "";
    
    submodule_data->defines[dnx_data_instru_eventor_define_nof_contexts_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_eventor_define_nof_builders].name = "nof_builders";
    submodule_data->defines[dnx_data_instru_eventor_define_nof_builders].doc = "";
    
    submodule_data->defines[dnx_data_instru_eventor_define_nof_builders].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_eventor_define_nof_builders_bits].name = "nof_builders_bits";
    submodule_data->defines[dnx_data_instru_eventor_define_nof_builders_bits].doc = "";
    
    submodule_data->defines[dnx_data_instru_eventor_define_nof_builders_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_eventor_define_nof_sram_banks].name = "nof_sram_banks";
    submodule_data->defines[dnx_data_instru_eventor_define_nof_sram_banks].doc = "";
    
    submodule_data->defines[dnx_data_instru_eventor_define_nof_sram_banks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_eventor_define_sram_bank_bits].name = "sram_bank_bits";
    submodule_data->defines[dnx_data_instru_eventor_define_sram_bank_bits].doc = "";
    
    submodule_data->defines[dnx_data_instru_eventor_define_sram_bank_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_eventor_define_sram_banks_full_size].name = "sram_banks_full_size";
    submodule_data->defines[dnx_data_instru_eventor_define_sram_banks_full_size].doc = "";
    
    submodule_data->defines[dnx_data_instru_eventor_define_sram_banks_full_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_eventor_define_sram_banks_net_size].name = "sram_banks_net_size";
    submodule_data->defines[dnx_data_instru_eventor_define_sram_banks_net_size].doc = "";
    
    submodule_data->defines[dnx_data_instru_eventor_define_sram_banks_net_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_eventor_define_builder_max_buffer_size].name = "builder_max_buffer_size";
    submodule_data->defines[dnx_data_instru_eventor_define_builder_max_buffer_size].doc = "";
    
    submodule_data->defines[dnx_data_instru_eventor_define_builder_max_buffer_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_eventor_define_rx_max_buffer_size].name = "rx_max_buffer_size";
    submodule_data->defines[dnx_data_instru_eventor_define_rx_max_buffer_size].doc = "";
    
    submodule_data->defines[dnx_data_instru_eventor_define_rx_max_buffer_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_eventor_define_axi_sram_offset].name = "axi_sram_offset";
    submodule_data->defines[dnx_data_instru_eventor_define_axi_sram_offset].doc = "";
    
    submodule_data->defines[dnx_data_instru_eventor_define_axi_sram_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_eventor_define_is_extra_words_copy].name = "is_extra_words_copy";
    submodule_data->defines[dnx_data_instru_eventor_define_is_extra_words_copy].doc = "";
    
    submodule_data->defines[dnx_data_instru_eventor_define_is_extra_words_copy].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_eventor_define_is_rx_en].name = "is_rx_en";
    submodule_data->defines[dnx_data_instru_eventor_define_is_rx_en].doc = "";
    
    submodule_data->defines[dnx_data_instru_eventor_define_is_rx_en].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_eventor_define_tx_builder_limitation_1].name = "tx_builder_limitation_1";
    submodule_data->defines[dnx_data_instru_eventor_define_tx_builder_limitation_1].doc = "";
    
    submodule_data->defines[dnx_data_instru_eventor_define_tx_builder_limitation_1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_eventor_define_builder_max_timeout_ms].name = "builder_max_timeout_ms";
    submodule_data->defines[dnx_data_instru_eventor_define_builder_max_timeout_ms].doc = "";
    
    submodule_data->defines[dnx_data_instru_eventor_define_builder_max_timeout_ms].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_instru_eventor_define_sram_bank_mask].name = "sram_bank_mask";
    submodule_data->defines[dnx_data_instru_eventor_define_sram_bank_mask].doc = "";
    
    submodule_data->defines[dnx_data_instru_eventor_define_sram_bank_mask].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_instru_eventor_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data instru eventor tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_instru_eventor_feature_get(
    int unit,
    dnx_data_instru_eventor_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_eventor, feature);
}


uint32
dnx_data_instru_eventor_rx_evpck_header_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_eventor, dnx_data_instru_eventor_define_rx_evpck_header_size);
}

uint32
dnx_data_instru_eventor_tx_builder_max_header_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_eventor, dnx_data_instru_eventor_define_tx_builder_max_header_size);
}

uint32
dnx_data_instru_eventor_nof_contexts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_eventor, dnx_data_instru_eventor_define_nof_contexts);
}

uint32
dnx_data_instru_eventor_nof_contexts_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_eventor, dnx_data_instru_eventor_define_nof_contexts_bits);
}

uint32
dnx_data_instru_eventor_nof_builders_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_eventor, dnx_data_instru_eventor_define_nof_builders);
}

uint32
dnx_data_instru_eventor_nof_builders_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_eventor, dnx_data_instru_eventor_define_nof_builders_bits);
}

uint32
dnx_data_instru_eventor_nof_sram_banks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_eventor, dnx_data_instru_eventor_define_nof_sram_banks);
}

uint32
dnx_data_instru_eventor_sram_bank_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_eventor, dnx_data_instru_eventor_define_sram_bank_bits);
}

uint32
dnx_data_instru_eventor_sram_banks_full_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_eventor, dnx_data_instru_eventor_define_sram_banks_full_size);
}

uint32
dnx_data_instru_eventor_sram_banks_net_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_eventor, dnx_data_instru_eventor_define_sram_banks_net_size);
}

uint32
dnx_data_instru_eventor_builder_max_buffer_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_eventor, dnx_data_instru_eventor_define_builder_max_buffer_size);
}

uint32
dnx_data_instru_eventor_rx_max_buffer_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_eventor, dnx_data_instru_eventor_define_rx_max_buffer_size);
}

uint32
dnx_data_instru_eventor_axi_sram_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_eventor, dnx_data_instru_eventor_define_axi_sram_offset);
}

uint32
dnx_data_instru_eventor_is_extra_words_copy_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_eventor, dnx_data_instru_eventor_define_is_extra_words_copy);
}

uint32
dnx_data_instru_eventor_is_rx_en_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_eventor, dnx_data_instru_eventor_define_is_rx_en);
}

uint32
dnx_data_instru_eventor_tx_builder_limitation_1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_eventor, dnx_data_instru_eventor_define_tx_builder_limitation_1);
}

uint32
dnx_data_instru_eventor_builder_max_timeout_ms_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_eventor, dnx_data_instru_eventor_define_builder_max_timeout_ms);
}

uint32
dnx_data_instru_eventor_sram_bank_mask_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_eventor, dnx_data_instru_eventor_define_sram_bank_mask);
}










static shr_error_e
dnx_data_instru_synced_triggers_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "synced_triggers";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_instru_synced_triggers_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data instru synced_triggers features");

    
    submodule_data->nof_defines = _dnx_data_instru_synced_triggers_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data instru synced_triggers defines");

    submodule_data->defines[dnx_data_instru_synced_triggers_define_synced_trigger_version].name = "synced_trigger_version";
    submodule_data->defines[dnx_data_instru_synced_triggers_define_synced_trigger_version].doc = "";
    
    submodule_data->defines[dnx_data_instru_synced_triggers_define_synced_trigger_version].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_instru_synced_triggers_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data instru synced_triggers tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_instru_synced_triggers_feature_get(
    int unit,
    dnx_data_instru_synced_triggers_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_triggers, feature);
}


uint32
dnx_data_instru_synced_triggers_synced_trigger_version_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_triggers, dnx_data_instru_synced_triggers_define_synced_trigger_version);
}










static shr_error_e
dnx_data_instru_synced_counters_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "synced_counters";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_instru_synced_counters_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data instru synced_counters features");

    submodule_data->features[dnx_data_instru_synced_counters_limited_ports_for_pfc].name = "limited_ports_for_pfc";
    submodule_data->features[dnx_data_instru_synced_counters_limited_ports_for_pfc].doc = "";
    submodule_data->features[dnx_data_instru_synced_counters_limited_ports_for_pfc].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_instru_synced_counters_is_implemented].name = "is_implemented";
    submodule_data->features[dnx_data_instru_synced_counters_is_implemented].doc = "";
    submodule_data->features[dnx_data_instru_synced_counters_is_implemented].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_instru_synced_counters_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data instru synced_counters defines");

    submodule_data->defines[dnx_data_instru_synced_counters_define_icgm_type].name = "icgm_type";
    submodule_data->defines[dnx_data_instru_synced_counters_define_icgm_type].doc = "";
    
    submodule_data->defines[dnx_data_instru_synced_counters_define_icgm_type].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_instru_synced_counters_define_nif_type].name = "nif_type";
    submodule_data->defines[dnx_data_instru_synced_counters_define_nif_type].doc = "";
    
    submodule_data->defines[dnx_data_instru_synced_counters_define_nif_type].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_instru_synced_counters_define_nof_types].name = "nof_types";
    submodule_data->defines[dnx_data_instru_synced_counters_define_nof_types].doc = "";
    
    submodule_data->defines[dnx_data_instru_synced_counters_define_nof_types].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_instru_synced_counters_define_period_adjusted_to_ns_through_time_ratio].name = "period_adjusted_to_ns_through_time_ratio";
    submodule_data->defines[dnx_data_instru_synced_counters_define_period_adjusted_to_ns_through_time_ratio].doc = "";
    
    submodule_data->defines[dnx_data_instru_synced_counters_define_period_adjusted_to_ns_through_time_ratio].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_synced_counters_define_period_adjusting_clk_bits].name = "period_adjusting_clk_bits";
    submodule_data->defines[dnx_data_instru_synced_counters_define_period_adjusting_clk_bits].doc = "";
    
    submodule_data->defines[dnx_data_instru_synced_counters_define_period_adjusting_clk_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_synced_counters_define_period_adjusting_ns_bits].name = "period_adjusting_ns_bits";
    submodule_data->defines[dnx_data_instru_synced_counters_define_period_adjusting_ns_bits].doc = "";
    
    submodule_data->defines[dnx_data_instru_synced_counters_define_period_adjusting_ns_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_synced_counters_define_interval_period_size_hw].name = "interval_period_size_hw";
    submodule_data->defines[dnx_data_instru_synced_counters_define_interval_period_size_hw].doc = "";
    
    submodule_data->defines[dnx_data_instru_synced_counters_define_interval_period_size_hw].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_synced_counters_define_max_interval_period_size].name = "max_interval_period_size";
    submodule_data->defines[dnx_data_instru_synced_counters_define_max_interval_period_size].doc = "";
    
    submodule_data->defines[dnx_data_instru_synced_counters_define_max_interval_period_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_synced_counters_define_cgm_max_nof_intervals].name = "cgm_max_nof_intervals";
    submodule_data->defines[dnx_data_instru_synced_counters_define_cgm_max_nof_intervals].doc = "";
    
    submodule_data->defines[dnx_data_instru_synced_counters_define_cgm_max_nof_intervals].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_synced_counters_define_nif_max_nof_intervals].name = "nif_max_nof_intervals";
    submodule_data->defines[dnx_data_instru_synced_counters_define_nif_max_nof_intervals].doc = "";
    
    submodule_data->defines[dnx_data_instru_synced_counters_define_nif_max_nof_intervals].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_synced_counters_define_cgm_write_counter_wrap_around].name = "cgm_write_counter_wrap_around";
    submodule_data->defines[dnx_data_instru_synced_counters_define_cgm_write_counter_wrap_around].doc = "";
    
    submodule_data->defines[dnx_data_instru_synced_counters_define_cgm_write_counter_wrap_around].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_synced_counters_define_nif_write_counter_wrap_around].name = "nif_write_counter_wrap_around";
    submodule_data->defines[dnx_data_instru_synced_counters_define_nif_write_counter_wrap_around].doc = "";
    
    submodule_data->defines[dnx_data_instru_synced_counters_define_nif_write_counter_wrap_around].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_synced_counters_define_bytes_in_word].name = "bytes_in_word";
    submodule_data->defines[dnx_data_instru_synced_counters_define_bytes_in_word].doc = "";
    
    submodule_data->defines[dnx_data_instru_synced_counters_define_bytes_in_word].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_synced_counters_define_nof_pms_for_pfc].name = "nof_pms_for_pfc";
    submodule_data->defines[dnx_data_instru_synced_counters_define_nof_pms_for_pfc].doc = "";
    
    submodule_data->defines[dnx_data_instru_synced_counters_define_nof_pms_for_pfc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_synced_counters_define_nof_ports_for_pfc].name = "nof_ports_for_pfc";
    submodule_data->defines[dnx_data_instru_synced_counters_define_nof_ports_for_pfc].doc = "";
    
    submodule_data->defines[dnx_data_instru_synced_counters_define_nof_ports_for_pfc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_instru_synced_counters_define_nof_tcs_per_port].name = "nof_tcs_per_port";
    submodule_data->defines[dnx_data_instru_synced_counters_define_nof_tcs_per_port].doc = "";
    
    submodule_data->defines[dnx_data_instru_synced_counters_define_nof_tcs_per_port].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_instru_synced_counters_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data instru synced_counters tables");

    
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].name = "counters_info";
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].doc = "";
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].size_of_values = sizeof(dnx_data_instru_synced_counters_counters_info_t);
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].entry_get = dnx_data_instru_synced_counters_counters_info_entry_str_get;

    
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].nof_keys = 2;
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].keys[0].name = "entry_index";
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].keys[0].doc = "";
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].keys[1].name = "source_type";
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].nof_values = 5;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].nof_values, "_dnx_data_instru_synced_counters_table_counters_info table values");
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].values[0].name = "valid";
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].values[0].type = "int";
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].values[0].doc = "";
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_instru_synced_counters_counters_info_t, valid);
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].values[1].name = "bcm_counter_type";
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].values[1].type = "int";
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].values[1].doc = "";
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_instru_synced_counters_counters_info_t, bcm_counter_type);
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].values[2].name = "counter_index";
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].values[2].type = "int";
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].values[2].doc = "";
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_instru_synced_counters_counters_info_t, counter_index);
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].values[3].name = "dbal_field";
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].values[3].type = "int";
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].values[3].doc = "";
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_instru_synced_counters_counters_info_t, dbal_field);
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].values[4].name = "words_to_bytes_conv_needed";
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].values[4].type = "int";
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].values[4].doc = "";
    submodule_data->tables[dnx_data_instru_synced_counters_table_counters_info].values[4].offset = UTILEX_OFFSETOF(dnx_data_instru_synced_counters_counters_info_t, words_to_bytes_conv_needed);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_instru_synced_counters_feature_get(
    int unit,
    dnx_data_instru_synced_counters_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_counters, feature);
}


uint32
dnx_data_instru_synced_counters_icgm_type_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_counters, dnx_data_instru_synced_counters_define_icgm_type);
}

uint32
dnx_data_instru_synced_counters_nif_type_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_counters, dnx_data_instru_synced_counters_define_nif_type);
}

uint32
dnx_data_instru_synced_counters_nof_types_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_counters, dnx_data_instru_synced_counters_define_nof_types);
}

uint32
dnx_data_instru_synced_counters_period_adjusted_to_ns_through_time_ratio_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_counters, dnx_data_instru_synced_counters_define_period_adjusted_to_ns_through_time_ratio);
}

uint32
dnx_data_instru_synced_counters_period_adjusting_clk_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_counters, dnx_data_instru_synced_counters_define_period_adjusting_clk_bits);
}

uint32
dnx_data_instru_synced_counters_period_adjusting_ns_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_counters, dnx_data_instru_synced_counters_define_period_adjusting_ns_bits);
}

uint32
dnx_data_instru_synced_counters_interval_period_size_hw_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_counters, dnx_data_instru_synced_counters_define_interval_period_size_hw);
}

uint32
dnx_data_instru_synced_counters_max_interval_period_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_counters, dnx_data_instru_synced_counters_define_max_interval_period_size);
}

uint32
dnx_data_instru_synced_counters_cgm_max_nof_intervals_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_counters, dnx_data_instru_synced_counters_define_cgm_max_nof_intervals);
}

uint32
dnx_data_instru_synced_counters_nif_max_nof_intervals_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_counters, dnx_data_instru_synced_counters_define_nif_max_nof_intervals);
}

uint32
dnx_data_instru_synced_counters_cgm_write_counter_wrap_around_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_counters, dnx_data_instru_synced_counters_define_cgm_write_counter_wrap_around);
}

uint32
dnx_data_instru_synced_counters_nif_write_counter_wrap_around_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_counters, dnx_data_instru_synced_counters_define_nif_write_counter_wrap_around);
}

uint32
dnx_data_instru_synced_counters_bytes_in_word_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_counters, dnx_data_instru_synced_counters_define_bytes_in_word);
}

uint32
dnx_data_instru_synced_counters_nof_pms_for_pfc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_counters, dnx_data_instru_synced_counters_define_nof_pms_for_pfc);
}

uint32
dnx_data_instru_synced_counters_nof_ports_for_pfc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_counters, dnx_data_instru_synced_counters_define_nof_ports_for_pfc);
}

uint32
dnx_data_instru_synced_counters_nof_tcs_per_port_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_counters, dnx_data_instru_synced_counters_define_nof_tcs_per_port);
}



const dnx_data_instru_synced_counters_counters_info_t *
dnx_data_instru_synced_counters_counters_info_get(
    int unit,
    int entry_index,
    int source_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_counters, dnx_data_instru_synced_counters_table_counters_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, entry_index, source_type);
    return (const dnx_data_instru_synced_counters_counters_info_t *) data;

}


shr_error_e
dnx_data_instru_synced_counters_counters_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_instru_synced_counters_counters_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_counters, dnx_data_instru_synced_counters_table_counters_info);
    data = (const dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->bcm_counter_type);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->counter_index);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_field);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->words_to_bytes_conv_needed);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_instru_synced_counters_counters_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_synced_counters, dnx_data_instru_synced_counters_table_counters_info);

}






static shr_error_e
dnx_data_instru_alternate_marking_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "alternate_marking";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_instru_alternate_marking_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data instru alternate_marking features");

    submodule_data->features[dnx_data_instru_alternate_marking_L_bit_profile_mapping].name = "L_bit_profile_mapping";
    submodule_data->features[dnx_data_instru_alternate_marking_L_bit_profile_mapping].doc = "";
    submodule_data->features[dnx_data_instru_alternate_marking_L_bit_profile_mapping].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_instru_alternate_marking_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data instru alternate_marking defines");

    submodule_data->defines[dnx_data_instru_alternate_marking_define_nof_L_bit_period_profile].name = "nof_L_bit_period_profile";
    submodule_data->defines[dnx_data_instru_alternate_marking_define_nof_L_bit_period_profile].doc = "";
    
    submodule_data->defines[dnx_data_instru_alternate_marking_define_nof_L_bit_period_profile].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_instru_alternate_marking_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data instru alternate_marking tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_instru_alternate_marking_feature_get(
    int unit,
    dnx_data_instru_alternate_marking_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_alternate_marking, feature);
}


uint32
dnx_data_instru_alternate_marking_nof_L_bit_period_profile_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_instru, dnx_data_instru_submodule_alternate_marking, dnx_data_instru_alternate_marking_define_nof_L_bit_period_profile);
}







shr_error_e
dnx_data_instru_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "instru";
    module_data->nof_submodules = _dnx_data_instru_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data instru submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_instru_ipt_init(unit, &module_data->submodules[dnx_data_instru_submodule_ipt]));
    SHR_IF_ERR_EXIT(dnx_data_instru_sflow_init(unit, &module_data->submodules[dnx_data_instru_submodule_sflow]));
    SHR_IF_ERR_EXIT(dnx_data_instru_ifa_init(unit, &module_data->submodules[dnx_data_instru_submodule_ifa]));
    SHR_IF_ERR_EXIT(dnx_data_instru_ipfix_init(unit, &module_data->submodules[dnx_data_instru_submodule_ipfix]));
    SHR_IF_ERR_EXIT(dnx_data_instru_hbh_pt_init(unit, &module_data->submodules[dnx_data_instru_submodule_hbh_pt]));
    SHR_IF_ERR_EXIT(dnx_data_instru_eventor_init(unit, &module_data->submodules[dnx_data_instru_submodule_eventor]));
    SHR_IF_ERR_EXIT(dnx_data_instru_synced_triggers_init(unit, &module_data->submodules[dnx_data_instru_submodule_synced_triggers]));
    SHR_IF_ERR_EXIT(dnx_data_instru_synced_counters_init(unit, &module_data->submodules[dnx_data_instru_submodule_synced_counters]));
    SHR_IF_ERR_EXIT(dnx_data_instru_alternate_marking_init(unit, &module_data->submodules[dnx_data_instru_submodule_alternate_marking]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_instru_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_instru_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_instru_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_instru_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_instru_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_instru_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_instru_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_instru_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_instru_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a1_data_instru_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_instru_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_instru_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_instru_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_instru_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_instru_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_instru_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_instru_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_instru_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_instru_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_instru_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_instru_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_instru_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_instru_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_instru_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

