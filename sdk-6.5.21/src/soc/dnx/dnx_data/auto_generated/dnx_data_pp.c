

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INIT_PP

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_pp.h>



extern shr_error_e jr2_a0_data_pp_attach(
    int unit);
extern shr_error_e j2c_a0_data_pp_attach(
    int unit);
extern shr_error_e q2a_a0_data_pp_attach(
    int unit);
extern shr_error_e j2p_a0_data_pp_attach(
    int unit);



static shr_error_e
dnx_data_pp_stages_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "stages";
    submodule_data->doc = "Information on PP stages";
    
    submodule_data->nof_features = _dnx_data_pp_stages_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data pp stages features");

    
    submodule_data->nof_defines = _dnx_data_pp_stages_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data pp stages defines");

    
    submodule_data->nof_tables = _dnx_data_pp_stages_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data pp stages tables");

    
    submodule_data->tables[dnx_data_pp_stages_table_params].name = "params";
    submodule_data->tables[dnx_data_pp_stages_table_params].doc = "Per stage properties and characteristics";
    submodule_data->tables[dnx_data_pp_stages_table_params].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_pp_stages_table_params].size_of_values = sizeof(dnx_data_pp_stages_params_t);
    submodule_data->tables[dnx_data_pp_stages_table_params].entry_get = dnx_data_pp_stages_params_entry_str_get;

    
    submodule_data->tables[dnx_data_pp_stages_table_params].nof_keys = 1;
    submodule_data->tables[dnx_data_pp_stages_table_params].keys[0].name = "stage";
    submodule_data->tables[dnx_data_pp_stages_table_params].keys[0].doc = "stage enum";

    
    submodule_data->tables[dnx_data_pp_stages_table_params].nof_values = 6;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_pp_stages_table_params].values, dnxc_data_value_t, submodule_data->tables[dnx_data_pp_stages_table_params].nof_values, "_dnx_data_pp_stages_table_params table values");
    submodule_data->tables[dnx_data_pp_stages_table_params].values[0].name = "valid";
    submodule_data->tables[dnx_data_pp_stages_table_params].values[0].type = "int";
    submodule_data->tables[dnx_data_pp_stages_table_params].values[0].doc = "stage validity flag";
    submodule_data->tables[dnx_data_pp_stages_table_params].values[0].offset = UTILEX_OFFSETOF(dnx_data_pp_stages_params_t, valid);
    submodule_data->tables[dnx_data_pp_stages_table_params].values[1].name = "name";
    submodule_data->tables[dnx_data_pp_stages_table_params].values[1].type = "char *";
    submodule_data->tables[dnx_data_pp_stages_table_params].values[1].doc = "stage name";
    submodule_data->tables[dnx_data_pp_stages_table_params].values[1].offset = UTILEX_OFFSETOF(dnx_data_pp_stages_params_t, name);
    submodule_data->tables[dnx_data_pp_stages_table_params].values[2].name = "block";
    submodule_data->tables[dnx_data_pp_stages_table_params].values[2].type = "char *";
    submodule_data->tables[dnx_data_pp_stages_table_params].values[2].doc = "block name";
    submodule_data->tables[dnx_data_pp_stages_table_params].values[2].offset = UTILEX_OFFSETOF(dnx_data_pp_stages_params_t, block);
    submodule_data->tables[dnx_data_pp_stages_table_params].values[3].name = "is_fp";
    submodule_data->tables[dnx_data_pp_stages_table_params].values[3].type = "int";
    submodule_data->tables[dnx_data_pp_stages_table_params].values[3].doc = "flag indicating support by Filed Processor";
    submodule_data->tables[dnx_data_pp_stages_table_params].values[3].offset = UTILEX_OFFSETOF(dnx_data_pp_stages_params_t, is_fp);
    submodule_data->tables[dnx_data_pp_stages_table_params].values[4].name = "is_kleap";
    submodule_data->tables[dnx_data_pp_stages_table_params].values[4].type = "int";
    submodule_data->tables[dnx_data_pp_stages_table_params].values[4].doc = "flag indicating kleap processing";
    submodule_data->tables[dnx_data_pp_stages_table_params].values[4].offset = UTILEX_OFFSETOF(dnx_data_pp_stages_params_t, is_kleap);
    submodule_data->tables[dnx_data_pp_stages_table_params].values[5].name = "pema_id";
    submodule_data->tables[dnx_data_pp_stages_table_params].values[5].type = "int";
    submodule_data->tables[dnx_data_pp_stages_table_params].values[5].doc = "PEMA Access ID - mapping to PEMA register in DBAL";
    submodule_data->tables[dnx_data_pp_stages_table_params].values[5].offset = UTILEX_OFFSETOF(dnx_data_pp_stages_params_t, pema_id);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_pp_stages_feature_get(
    int unit,
    dnx_data_pp_stages_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_pp, dnx_data_pp_submodule_stages, feature);
}




const dnx_data_pp_stages_params_t *
dnx_data_pp_stages_params_get(
    int unit,
    int stage)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_pp, dnx_data_pp_submodule_stages, dnx_data_pp_stages_table_params);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, stage, 0);
    return (const dnx_data_pp_stages_params_t *) data;

}


shr_error_e
dnx_data_pp_stages_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_pp_stages_params_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_pp, dnx_data_pp_submodule_stages, dnx_data_pp_stages_table_params);
    data = (const dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->name == NULL ? "" : data->name);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->block == NULL ? "" : data->block);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_fp);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_kleap);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pema_id);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_pp_stages_params_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_pp, dnx_data_pp_submodule_stages, dnx_data_pp_stages_table_params);

}






static shr_error_e
dnx_data_pp_PEM_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "PEM";
    submodule_data->doc = "General data about PEM";
    
    submodule_data->nof_features = _dnx_data_pp_PEM_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data pp PEM features");

    submodule_data->features[dnx_data_pp_PEM_clock_power_down].name = "clock_power_down";
    submodule_data->features[dnx_data_pp_PEM_clock_power_down].doc = "Can PEM blocks clock be power down";
    submodule_data->features[dnx_data_pp_PEM_clock_power_down].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_pp_PEM_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data pp PEM defines");

    submodule_data->defines[dnx_data_pp_PEM_define_nof_pem_bits].name = "nof_pem_bits";
    submodule_data->defines[dnx_data_pp_PEM_define_nof_pem_bits].doc = "number of bit needed to represent nof PEM blocks";
    
    submodule_data->defines[dnx_data_pp_PEM_define_nof_pem_bits].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_pp_PEM_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data pp PEM tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_pp_PEM_feature_get(
    int unit,
    dnx_data_pp_PEM_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_pp, dnx_data_pp_submodule_PEM, feature);
}


uint32
dnx_data_pp_PEM_nof_pem_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pp, dnx_data_pp_submodule_PEM, dnx_data_pp_PEM_define_nof_pem_bits);
}










static shr_error_e
dnx_data_pp_application_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "application";
    submodule_data->doc = "Information on PP applications";
    
    submodule_data->nof_features = _dnx_data_pp_application_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data pp application features");

    
    submodule_data->nof_defines = _dnx_data_pp_application_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data pp application defines");

    
    submodule_data->nof_tables = _dnx_data_pp_application_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data pp application tables");

    
    submodule_data->tables[dnx_data_pp_application_table_device_image].name = "device_image";
    submodule_data->tables[dnx_data_pp_application_table_device_image].doc = "device pp image info";
    submodule_data->tables[dnx_data_pp_application_table_device_image].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_pp_application_table_device_image].size_of_values = sizeof(dnx_data_pp_application_device_image_t);
    submodule_data->tables[dnx_data_pp_application_table_device_image].entry_get = dnx_data_pp_application_device_image_entry_str_get;

    
    submodule_data->tables[dnx_data_pp_application_table_device_image].nof_keys = 0;

    
    submodule_data->tables[dnx_data_pp_application_table_device_image].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_pp_application_table_device_image].values, dnxc_data_value_t, submodule_data->tables[dnx_data_pp_application_table_device_image].nof_values, "_dnx_data_pp_application_table_device_image table values");
    submodule_data->tables[dnx_data_pp_application_table_device_image].values[0].name = "standard_image_name";
    submodule_data->tables[dnx_data_pp_application_table_device_image].values[0].type = "char *";
    submodule_data->tables[dnx_data_pp_application_table_device_image].values[0].doc = "standard image name";
    submodule_data->tables[dnx_data_pp_application_table_device_image].values[0].offset = UTILEX_OFFSETOF(dnx_data_pp_application_device_image_t, standard_image_name);
    submodule_data->tables[dnx_data_pp_application_table_device_image].values[1].name = "name";
    submodule_data->tables[dnx_data_pp_application_table_device_image].values[1].type = "char *";
    submodule_data->tables[dnx_data_pp_application_table_device_image].values[1].doc = "image name";
    submodule_data->tables[dnx_data_pp_application_table_device_image].values[1].offset = UTILEX_OFFSETOF(dnx_data_pp_application_device_image_t, name);

    
    submodule_data->tables[dnx_data_pp_application_table_ucode].name = "ucode";
    submodule_data->tables[dnx_data_pp_application_table_ucode].doc = "ucode file information";
    submodule_data->tables[dnx_data_pp_application_table_ucode].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_pp_application_table_ucode].size_of_values = sizeof(dnx_data_pp_application_ucode_t);
    submodule_data->tables[dnx_data_pp_application_table_ucode].entry_get = dnx_data_pp_application_ucode_entry_str_get;

    
    submodule_data->tables[dnx_data_pp_application_table_ucode].nof_keys = 0;

    
    submodule_data->tables[dnx_data_pp_application_table_ucode].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_pp_application_table_ucode].values, dnxc_data_value_t, submodule_data->tables[dnx_data_pp_application_table_ucode].nof_values, "_dnx_data_pp_application_table_ucode table values");
    submodule_data->tables[dnx_data_pp_application_table_ucode].values[0].name = "relative_path";
    submodule_data->tables[dnx_data_pp_application_table_ucode].values[0].type = "char *";
    submodule_data->tables[dnx_data_pp_application_table_ucode].values[0].doc = "pemla ucode relative path. relative to exe_location/pemla/device_name/ucode/";
    submodule_data->tables[dnx_data_pp_application_table_ucode].values[0].offset = UTILEX_OFFSETOF(dnx_data_pp_application_ucode_t, relative_path);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_pp_application_feature_get(
    int unit,
    dnx_data_pp_application_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_pp, dnx_data_pp_submodule_application, feature);
}




const dnx_data_pp_application_device_image_t *
dnx_data_pp_application_device_image_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_pp, dnx_data_pp_submodule_application, dnx_data_pp_application_table_device_image);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_pp_application_device_image_t *) data;

}

const dnx_data_pp_application_ucode_t *
dnx_data_pp_application_ucode_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_pp, dnx_data_pp_submodule_application, dnx_data_pp_application_table_ucode);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_pp_application_ucode_t *) data;

}


shr_error_e
dnx_data_pp_application_device_image_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_pp_application_device_image_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_pp, dnx_data_pp_submodule_application, dnx_data_pp_application_table_device_image);
    data = (const dnx_data_pp_application_device_image_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->standard_image_name == NULL ? "" : data->standard_image_name);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->name == NULL ? "" : data->name);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_pp_application_ucode_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_pp_application_ucode_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_pp, dnx_data_pp_submodule_application, dnx_data_pp_application_table_ucode);
    data = (const dnx_data_pp_application_ucode_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->relative_path == NULL ? "" : data->relative_path);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_pp_application_device_image_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_pp, dnx_data_pp_submodule_application, dnx_data_pp_application_table_device_image);

}

const dnxc_data_table_info_t *
dnx_data_pp_application_ucode_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_pp, dnx_data_pp_submodule_application, dnx_data_pp_application_table_ucode);

}






static shr_error_e
dnx_data_pp_ETPP_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ETPP";
    submodule_data->doc = "ETPP general parameters";
    
    submodule_data->nof_features = _dnx_data_pp_ETPP_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data pp ETPP features");

    
    submodule_data->nof_defines = _dnx_data_pp_ETPP_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data pp ETPP defines");

    submodule_data->defines[dnx_data_pp_ETPP_define_etps_type_size_bits].name = "etps_type_size_bits";
    submodule_data->defines[dnx_data_pp_ETPP_define_etps_type_size_bits].doc = "Size in bits of the ETPS type field";
    
    submodule_data->defines[dnx_data_pp_ETPP_define_etps_type_size_bits].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_pp_ETPP_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data pp ETPP tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_pp_ETPP_feature_get(
    int unit,
    dnx_data_pp_ETPP_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_pp, dnx_data_pp_submodule_ETPP, feature);
}


uint32
dnx_data_pp_ETPP_etps_type_size_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pp, dnx_data_pp_submodule_ETPP, dnx_data_pp_ETPP_define_etps_type_size_bits);
}










static shr_error_e
dnx_data_pp_debug_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "debug";
    submodule_data->doc = "debug proprties and data";
    
    submodule_data->nof_features = _dnx_data_pp_debug_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data pp debug features");

    
    submodule_data->nof_defines = _dnx_data_pp_debug_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data pp debug defines");

    submodule_data->defines[dnx_data_pp_debug_define_packet_header_data].name = "packet_header_data";
    submodule_data->defines[dnx_data_pp_debug_define_packet_header_data].doc = "number of original packet header data";
    
    submodule_data->defines[dnx_data_pp_debug_define_packet_header_data].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pp_debug_define_ptc_size].name = "ptc_size";
    submodule_data->defines[dnx_data_pp_debug_define_ptc_size].doc = "Size of PTC field in debug register";
    
    submodule_data->defines[dnx_data_pp_debug_define_ptc_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pp_debug_define_valid_bytes].name = "valid_bytes";
    submodule_data->defines[dnx_data_pp_debug_define_valid_bytes].doc = "valid bytes";
    
    submodule_data->defines[dnx_data_pp_debug_define_valid_bytes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pp_debug_define_time_stamp].name = "time_stamp";
    submodule_data->defines[dnx_data_pp_debug_define_time_stamp].doc = "Time Stamp (only for packets received from NIF)";
    
    submodule_data->defines[dnx_data_pp_debug_define_time_stamp].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_pp_debug_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data pp debug tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_pp_debug_feature_get(
    int unit,
    dnx_data_pp_debug_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_pp, dnx_data_pp_submodule_debug, feature);
}


uint32
dnx_data_pp_debug_packet_header_data_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pp, dnx_data_pp_submodule_debug, dnx_data_pp_debug_define_packet_header_data);
}

uint32
dnx_data_pp_debug_ptc_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pp, dnx_data_pp_submodule_debug, dnx_data_pp_debug_define_ptc_size);
}

uint32
dnx_data_pp_debug_valid_bytes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pp, dnx_data_pp_submodule_debug, dnx_data_pp_debug_define_valid_bytes);
}

uint32
dnx_data_pp_debug_time_stamp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pp, dnx_data_pp_submodule_debug, dnx_data_pp_debug_define_time_stamp);
}







shr_error_e
dnx_data_pp_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "pp";
    module_data->nof_submodules = _dnx_data_pp_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data pp submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_pp_stages_init(unit, &module_data->submodules[dnx_data_pp_submodule_stages]));
    SHR_IF_ERR_EXIT(dnx_data_pp_PEM_init(unit, &module_data->submodules[dnx_data_pp_submodule_PEM]));
    SHR_IF_ERR_EXIT(dnx_data_pp_application_init(unit, &module_data->submodules[dnx_data_pp_submodule_application]));
    SHR_IF_ERR_EXIT(dnx_data_pp_ETPP_init(unit, &module_data->submodules[dnx_data_pp_submodule_ETPP]));
    SHR_IF_ERR_EXIT(dnx_data_pp_debug_init(unit, &module_data->submodules[dnx_data_pp_submodule_debug]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pp_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pp_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pp_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pp_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_pp_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pp_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_pp_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pp_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_pp_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pp_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_pp_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pp_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_pp_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pp_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_pp_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

