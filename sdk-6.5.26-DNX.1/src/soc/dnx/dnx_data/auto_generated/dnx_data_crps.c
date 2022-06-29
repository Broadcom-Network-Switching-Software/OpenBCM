
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_crps.h>




extern shr_error_e jr2_a0_data_crps_attach(
    int unit);


extern shr_error_e jr2_b0_data_crps_attach(
    int unit);


extern shr_error_e j2c_a0_data_crps_attach(
    int unit);


extern shr_error_e q2a_a0_data_crps_attach(
    int unit);


extern shr_error_e j2p_a0_data_crps_attach(
    int unit);


extern shr_error_e j2p_a1_data_crps_attach(
    int unit);


extern shr_error_e j2x_a0_data_crps_attach(
    int unit);




static shr_error_e
dnx_data_crps_engine_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "engine";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_crps_engine_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data crps engine features");

    submodule_data->features[dnx_data_crps_engine_valid_address_admission_is_supported].name = "valid_address_admission_is_supported";
    submodule_data->features[dnx_data_crps_engine_valid_address_admission_is_supported].doc = "";
    submodule_data->features[dnx_data_crps_engine_valid_address_admission_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_engine_last_input_command_is_supported].name = "last_input_command_is_supported";
    submodule_data->features[dnx_data_crps_engine_last_input_command_is_supported].doc = "";
    submodule_data->features[dnx_data_crps_engine_last_input_command_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_engine_multi_sets_is_supported].name = "multi_sets_is_supported";
    submodule_data->features[dnx_data_crps_engine_multi_sets_is_supported].doc = "";
    submodule_data->features[dnx_data_crps_engine_multi_sets_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_engine_dynamic_counter_mode_is_supported].name = "dynamic_counter_mode_is_supported";
    submodule_data->features[dnx_data_crps_engine_dynamic_counter_mode_is_supported].doc = "";
    submodule_data->features[dnx_data_crps_engine_dynamic_counter_mode_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_crps_engine_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data crps engine defines");

    submodule_data->defines[dnx_data_crps_engine_define_nof_engines].name = "nof_engines";
    submodule_data->defines[dnx_data_crps_engine_define_nof_engines].doc = "";
    
    submodule_data->defines[dnx_data_crps_engine_define_nof_engines].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_engine_define_nof_mid_engines].name = "nof_mid_engines";
    submodule_data->defines[dnx_data_crps_engine_define_nof_mid_engines].doc = "";
    
    submodule_data->defines[dnx_data_crps_engine_define_nof_mid_engines].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_engine_define_nof_big_engines].name = "nof_big_engines";
    submodule_data->defines[dnx_data_crps_engine_define_nof_big_engines].doc = "";
    
    submodule_data->defines[dnx_data_crps_engine_define_nof_big_engines].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_engine_define_nof_databases].name = "nof_databases";
    submodule_data->defines[dnx_data_crps_engine_define_nof_databases].doc = "";
    
    submodule_data->defines[dnx_data_crps_engine_define_nof_databases].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_engine_define_data_mapping_table_size].name = "data_mapping_table_size";
    submodule_data->defines[dnx_data_crps_engine_define_data_mapping_table_size].doc = "";
    
    submodule_data->defines[dnx_data_crps_engine_define_data_mapping_table_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_engine_define_max_counter_set_size].name = "max_counter_set_size";
    submodule_data->defines[dnx_data_crps_engine_define_max_counter_set_size].doc = "";
    
    submodule_data->defines[dnx_data_crps_engine_define_max_counter_set_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_engine_define_oam_counter_set_size].name = "oam_counter_set_size";
    submodule_data->defines[dnx_data_crps_engine_define_oam_counter_set_size].doc = "";
    
    submodule_data->defines[dnx_data_crps_engine_define_oam_counter_set_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_engine_define_max_nof_counter_sets].name = "max_nof_counter_sets";
    submodule_data->defines[dnx_data_crps_engine_define_max_nof_counter_sets].doc = "";
    
    submodule_data->defines[dnx_data_crps_engine_define_max_nof_counter_sets].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_engine_define_last_address_nof_bits].name = "last_address_nof_bits";
    submodule_data->defines[dnx_data_crps_engine_define_last_address_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_crps_engine_define_last_address_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_engine_define_counter_format_nof_bits].name = "counter_format_nof_bits";
    submodule_data->defines[dnx_data_crps_engine_define_counter_format_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_crps_engine_define_counter_format_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_engine_define_counter_mode_nof_bits].name = "counter_mode_nof_bits";
    submodule_data->defines[dnx_data_crps_engine_define_counter_mode_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_crps_engine_define_counter_mode_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_engine_define_nof_small_meter_shared_engines].name = "nof_small_meter_shared_engines";
    submodule_data->defines[dnx_data_crps_engine_define_nof_small_meter_shared_engines].doc = "";
    
    submodule_data->defines[dnx_data_crps_engine_define_nof_small_meter_shared_engines].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_engine_define_nof_mid_meter_shared_engines].name = "nof_mid_meter_shared_engines";
    submodule_data->defines[dnx_data_crps_engine_define_nof_mid_meter_shared_engines].doc = "";
    
    submodule_data->defines[dnx_data_crps_engine_define_nof_mid_meter_shared_engines].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_engine_define_nof_big_meter_shared_engines].name = "nof_big_meter_shared_engines";
    submodule_data->defines[dnx_data_crps_engine_define_nof_big_meter_shared_engines].doc = "";
    
    submodule_data->defines[dnx_data_crps_engine_define_nof_big_meter_shared_engines].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_crps_engine_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data crps engine tables");

    
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].name = "engines_info";
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].doc = "";
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].size_of_values = sizeof(dnx_data_crps_engine_engines_info_t);
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].entry_get = dnx_data_crps_engine_engines_info_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].nof_keys = 1;
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].keys[0].name = "engine_id";
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_engine_table_engines_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_engine_table_engines_info].nof_values, "_dnx_data_crps_engine_table_engines_info table values");
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].values[0].name = "nof_entries";
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].values[0].type = "int";
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].values[0].doc = "";
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_engine_engines_info_t, nof_entries);
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].values[1].name = "meter_shared";
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].values[1].type = "int";
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].values[1].doc = "";
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_crps_engine_engines_info_t, meter_shared);

    
    submodule_data->tables[dnx_data_crps_engine_table_source_base].name = "source_base";
    submodule_data->tables[dnx_data_crps_engine_table_source_base].doc = "";
    submodule_data->tables[dnx_data_crps_engine_table_source_base].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_engine_table_source_base].size_of_values = sizeof(dnx_data_crps_engine_source_base_t);
    submodule_data->tables[dnx_data_crps_engine_table_source_base].entry_get = dnx_data_crps_engine_source_base_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_engine_table_source_base].nof_keys = 1;
    submodule_data->tables[dnx_data_crps_engine_table_source_base].keys[0].name = "source";
    submodule_data->tables[dnx_data_crps_engine_table_source_base].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_crps_engine_table_source_base].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_engine_table_source_base].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_engine_table_source_base].nof_values, "_dnx_data_crps_engine_table_source_base table values");
    submodule_data->tables[dnx_data_crps_engine_table_source_base].values[0].name = "base_val";
    submodule_data->tables[dnx_data_crps_engine_table_source_base].values[0].type = "int";
    submodule_data->tables[dnx_data_crps_engine_table_source_base].values[0].doc = "";
    submodule_data->tables[dnx_data_crps_engine_table_source_base].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_engine_source_base_t, base_val);

    
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].name = "counter_format_types";
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].doc = "";
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].size_of_values = sizeof(dnx_data_crps_engine_counter_format_types_t);
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].entry_get = dnx_data_crps_engine_counter_format_types_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].nof_keys = 1;
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].keys[0].name = "format_type";
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].nof_values = 5;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].nof_values, "_dnx_data_crps_engine_table_counter_format_types table values");
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].values[0].name = "valid";
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].values[0].type = "int";
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].values[0].doc = "";
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_engine_counter_format_types_t, valid);
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].values[1].name = "nof_bits_for_packets";
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].values[1].type = "int";
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].values[1].doc = "";
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].values[1].offset = UTILEX_OFFSETOF(dnx_data_crps_engine_counter_format_types_t, nof_bits_for_packets);
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].values[2].name = "nof_bits_for_bytes";
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].values[2].type = "int";
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].values[2].doc = "";
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].values[2].offset = UTILEX_OFFSETOF(dnx_data_crps_engine_counter_format_types_t, nof_bits_for_bytes);
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].values[3].name = "nof_counters_per_entry";
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].values[3].type = "int";
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].values[3].doc = "";
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].values[3].offset = UTILEX_OFFSETOF(dnx_data_crps_engine_counter_format_types_t, nof_counters_per_entry);
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].values[4].name = "valid_eviction_destinations";
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].values[4].type = "bcm_eviction_destination_type_t[DNX_DATA_MAX_CRPS_EVICTION_NOF_EVICTION_DESTINATION_TYPES]";
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].values[4].doc = "";
    submodule_data->tables[dnx_data_crps_engine_table_counter_format_types].values[4].offset = UTILEX_OFFSETOF(dnx_data_crps_engine_counter_format_types_t, valid_eviction_destinations);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_crps_engine_feature_get(
    int unit,
    dnx_data_crps_engine_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, feature);
}


uint32
dnx_data_crps_engine_nof_engines_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_define_nof_engines);
}

uint32
dnx_data_crps_engine_nof_mid_engines_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_define_nof_mid_engines);
}

uint32
dnx_data_crps_engine_nof_big_engines_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_define_nof_big_engines);
}

uint32
dnx_data_crps_engine_nof_databases_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_define_nof_databases);
}

uint32
dnx_data_crps_engine_data_mapping_table_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_define_data_mapping_table_size);
}

uint32
dnx_data_crps_engine_max_counter_set_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_define_max_counter_set_size);
}

uint32
dnx_data_crps_engine_oam_counter_set_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_define_oam_counter_set_size);
}

uint32
dnx_data_crps_engine_max_nof_counter_sets_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_define_max_nof_counter_sets);
}

uint32
dnx_data_crps_engine_last_address_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_define_last_address_nof_bits);
}

uint32
dnx_data_crps_engine_counter_format_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_define_counter_format_nof_bits);
}

uint32
dnx_data_crps_engine_counter_mode_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_define_counter_mode_nof_bits);
}

uint32
dnx_data_crps_engine_nof_small_meter_shared_engines_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_define_nof_small_meter_shared_engines);
}

uint32
dnx_data_crps_engine_nof_mid_meter_shared_engines_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_define_nof_mid_meter_shared_engines);
}

uint32
dnx_data_crps_engine_nof_big_meter_shared_engines_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_define_nof_big_meter_shared_engines);
}



const dnx_data_crps_engine_engines_info_t *
dnx_data_crps_engine_engines_info_get(
    int unit,
    int engine_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_table_engines_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, engine_id, 0);
    return (const dnx_data_crps_engine_engines_info_t *) data;

}

const dnx_data_crps_engine_source_base_t *
dnx_data_crps_engine_source_base_get(
    int unit,
    int source)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_table_source_base);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, source, 0);
    return (const dnx_data_crps_engine_source_base_t *) data;

}

const dnx_data_crps_engine_counter_format_types_t *
dnx_data_crps_engine_counter_format_types_get(
    int unit,
    int format_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_table_counter_format_types);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, format_type, 0);
    return (const dnx_data_crps_engine_counter_format_types_t *) data;

}


shr_error_e
dnx_data_crps_engine_engines_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_crps_engine_engines_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_table_engines_info);
    data = (const dnx_data_crps_engine_engines_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_entries);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->meter_shared);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_crps_engine_source_base_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_crps_engine_source_base_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_table_source_base);
    data = (const dnx_data_crps_engine_source_base_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->base_val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_crps_engine_counter_format_types_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_crps_engine_counter_format_types_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_table_counter_format_types);
    data = (const dnx_data_crps_engine_counter_format_types_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_bits_for_packets);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_bits_for_bytes);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_counters_per_entry);
            break;
        case 4:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_CRPS_EVICTION_NOF_EVICTION_DESTINATION_TYPES, data->valid_eviction_destinations);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_crps_engine_engines_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_table_engines_info);

}

const dnxc_data_table_info_t *
dnx_data_crps_engine_source_base_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_table_source_base);

}

const dnxc_data_table_info_t *
dnx_data_crps_engine_counter_format_types_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_table_counter_format_types);

}






static shr_error_e
dnx_data_crps_eviction_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "eviction";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_crps_eviction_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data crps eviction features");

    submodule_data->features[dnx_data_crps_eviction_algorithmic_is_supported].name = "algorithmic_is_supported";
    submodule_data->features[dnx_data_crps_eviction_algorithmic_is_supported].doc = "";
    submodule_data->features[dnx_data_crps_eviction_algorithmic_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_eviction_probabilistic_is_supported].name = "probabilistic_is_supported";
    submodule_data->features[dnx_data_crps_eviction_probabilistic_is_supported].doc = "";
    submodule_data->features[dnx_data_crps_eviction_probabilistic_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_eviction_low_rate_probabilistic_eviction].name = "low_rate_probabilistic_eviction";
    submodule_data->features[dnx_data_crps_eviction_low_rate_probabilistic_eviction].doc = "";
    submodule_data->features[dnx_data_crps_eviction_low_rate_probabilistic_eviction].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_eviction_conditional_action_is_supported].name = "conditional_action_is_supported";
    submodule_data->features[dnx_data_crps_eviction_conditional_action_is_supported].doc = "";
    submodule_data->features[dnx_data_crps_eviction_conditional_action_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_eviction_eviction_from_memory_is_supported].name = "eviction_from_memory_is_supported";
    submodule_data->features[dnx_data_crps_eviction_eviction_from_memory_is_supported].doc = "";
    submodule_data->features[dnx_data_crps_eviction_eviction_from_memory_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_eviction_sequential_scan_speedup_is_supported].name = "sequential_scan_speedup_is_supported";
    submodule_data->features[dnx_data_crps_eviction_sequential_scan_speedup_is_supported].doc = "";
    submodule_data->features[dnx_data_crps_eviction_sequential_scan_speedup_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_eviction_probabilistic_eviction_thresholds_is_supported].name = "probabilistic_eviction_thresholds_is_supported";
    submodule_data->features[dnx_data_crps_eviction_probabilistic_eviction_thresholds_is_supported].doc = "";
    submodule_data->features[dnx_data_crps_eviction_probabilistic_eviction_thresholds_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_eviction_iteration_interval_time_config_is_supported].name = "iteration_interval_time_config_is_supported";
    submodule_data->features[dnx_data_crps_eviction_iteration_interval_time_config_is_supported].doc = "";
    submodule_data->features[dnx_data_crps_eviction_iteration_interval_time_config_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_crps_eviction_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data crps eviction defines");

    submodule_data->defines[dnx_data_crps_eviction_define_counters_fifo_depth].name = "counters_fifo_depth";
    submodule_data->defines[dnx_data_crps_eviction_define_counters_fifo_depth].doc = "";
    
    submodule_data->defines[dnx_data_crps_eviction_define_counters_fifo_depth].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_eviction_define_nof_counters_fifo].name = "nof_counters_fifo";
    submodule_data->defines[dnx_data_crps_eviction_define_nof_counters_fifo].doc = "";
    
    submodule_data->defines[dnx_data_crps_eviction_define_nof_counters_fifo].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_eviction_define_dma_record_entry_nof_bits].name = "dma_record_entry_nof_bits";
    submodule_data->defines[dnx_data_crps_eviction_define_dma_record_entry_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_crps_eviction_define_dma_record_entry_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_eviction_define_bubble_before_request_timeout].name = "bubble_before_request_timeout";
    submodule_data->defines[dnx_data_crps_eviction_define_bubble_before_request_timeout].doc = "";
    
    submodule_data->defines[dnx_data_crps_eviction_define_bubble_before_request_timeout].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_eviction_define_bubble_after_request_timeout].name = "bubble_after_request_timeout";
    submodule_data->defines[dnx_data_crps_eviction_define_bubble_after_request_timeout].doc = "";
    
    submodule_data->defines[dnx_data_crps_eviction_define_bubble_after_request_timeout].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_eviction_define_seq_address_nof_bits].name = "seq_address_nof_bits";
    submodule_data->defines[dnx_data_crps_eviction_define_seq_address_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_crps_eviction_define_seq_address_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_eviction_define_nof_eviction_destination_types].name = "nof_eviction_destination_types";
    submodule_data->defines[dnx_data_crps_eviction_define_nof_eviction_destination_types].doc = "";
    
    submodule_data->defines[dnx_data_crps_eviction_define_nof_eviction_destination_types].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_eviction_define_nof_crps_network_channels].name = "nof_crps_network_channels";
    submodule_data->defines[dnx_data_crps_eviction_define_nof_crps_network_channels].doc = "";
    
    submodule_data->defines[dnx_data_crps_eviction_define_nof_crps_network_channels].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_eviction_define_bg_thread_enable].name = "bg_thread_enable";
    submodule_data->defines[dnx_data_crps_eviction_define_bg_thread_enable].doc = "";
    
    submodule_data->defines[dnx_data_crps_eviction_define_bg_thread_enable].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_crps_eviction_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data crps eviction tables");

    
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].name = "phy_record_format";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].size_of_values = sizeof(dnx_data_crps_eviction_phy_record_format_t);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].entry_get = dnx_data_crps_eviction_phy_record_format_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].nof_keys = 0;

    
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].nof_values = 20;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].nof_values, "_dnx_data_crps_eviction_table_phy_record_format table values");
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[0].name = "obj_type_size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[0].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[0].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, obj_type_size);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[1].name = "obj_type_offset";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[1].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[1].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[1].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, obj_type_offset);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[2].name = "reserved0_size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[2].type = "int";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[2].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[2].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, reserved0_size);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[3].name = "reserved0_offset";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[3].type = "int";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[3].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[3].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, reserved0_offset);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[4].name = "opcode_size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[4].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[4].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[4].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, opcode_size);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[5].name = "opcode_offset";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[5].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[5].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[5].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, opcode_offset);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[6].name = "err_ind_size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[6].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[6].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[6].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, err_ind_size);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[7].name = "err_ind_offset";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[7].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[7].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[7].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, err_ind_offset);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[8].name = "reserved1_size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[8].type = "int";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[8].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[8].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, reserved1_size);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[9].name = "reserved1_offset";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[9].type = "int";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[9].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[9].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, reserved1_offset);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[10].name = "core_id_size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[10].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[10].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[10].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, core_id_size);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[11].name = "core_id_offset";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[11].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[11].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[11].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, core_id_offset);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[12].name = "engine_id_size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[12].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[12].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[12].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, engine_id_size);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[13].name = "engine_id_offset";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[13].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[13].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[13].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, engine_id_offset);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[14].name = "local_ctr_id_size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[14].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[14].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[14].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, local_ctr_id_size);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[15].name = "local_ctr_id_offset";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[15].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[15].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[15].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, local_ctr_id_offset);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[16].name = "ctr_value_size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[16].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[16].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[16].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, ctr_value_size);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[17].name = "ctr_value_offset";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[17].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[17].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[17].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, ctr_value_offset);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[18].name = "is_probabilistic_size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[18].type = "int";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[18].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[18].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, is_probabilistic_size);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[19].name = "is_probabilistic_offset";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[19].type = "int";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[19].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[19].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, is_probabilistic_offset);

    
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].name = "condional_action_valid";
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].size_of_values = sizeof(dnx_data_crps_eviction_condional_action_valid_t);
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].entry_get = dnx_data_crps_eviction_condional_action_valid_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].nof_keys = 1;
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].keys[0].name = "action_idx";
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].nof_values, "_dnx_data_crps_eviction_table_condional_action_valid table values");
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].values[0].name = "valid";
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].values[0].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].values[0].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_condional_action_valid_t, valid);

    
    submodule_data->tables[dnx_data_crps_eviction_table_supported_eviction_destination].name = "supported_eviction_destination";
    submodule_data->tables[dnx_data_crps_eviction_table_supported_eviction_destination].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_supported_eviction_destination].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_eviction_table_supported_eviction_destination].size_of_values = sizeof(dnx_data_crps_eviction_supported_eviction_destination_t);
    submodule_data->tables[dnx_data_crps_eviction_table_supported_eviction_destination].entry_get = dnx_data_crps_eviction_supported_eviction_destination_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_eviction_table_supported_eviction_destination].nof_keys = 1;
    submodule_data->tables[dnx_data_crps_eviction_table_supported_eviction_destination].keys[0].name = "destination_type";
    submodule_data->tables[dnx_data_crps_eviction_table_supported_eviction_destination].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_crps_eviction_table_supported_eviction_destination].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_eviction_table_supported_eviction_destination].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_eviction_table_supported_eviction_destination].nof_values, "_dnx_data_crps_eviction_table_supported_eviction_destination table values");
    submodule_data->tables[dnx_data_crps_eviction_table_supported_eviction_destination].values[0].name = "valid";
    submodule_data->tables[dnx_data_crps_eviction_table_supported_eviction_destination].values[0].type = "int";
    submodule_data->tables[dnx_data_crps_eviction_table_supported_eviction_destination].values[0].doc = "";
    submodule_data->tables[dnx_data_crps_eviction_table_supported_eviction_destination].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_supported_eviction_destination_t, valid);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_crps_eviction_feature_get(
    int unit,
    dnx_data_crps_eviction_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_eviction, feature);
}


uint32
dnx_data_crps_eviction_counters_fifo_depth_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_eviction, dnx_data_crps_eviction_define_counters_fifo_depth);
}

uint32
dnx_data_crps_eviction_nof_counters_fifo_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_eviction, dnx_data_crps_eviction_define_nof_counters_fifo);
}

uint32
dnx_data_crps_eviction_dma_record_entry_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_eviction, dnx_data_crps_eviction_define_dma_record_entry_nof_bits);
}

uint32
dnx_data_crps_eviction_bubble_before_request_timeout_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_eviction, dnx_data_crps_eviction_define_bubble_before_request_timeout);
}

uint32
dnx_data_crps_eviction_bubble_after_request_timeout_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_eviction, dnx_data_crps_eviction_define_bubble_after_request_timeout);
}

uint32
dnx_data_crps_eviction_seq_address_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_eviction, dnx_data_crps_eviction_define_seq_address_nof_bits);
}

uint32
dnx_data_crps_eviction_nof_eviction_destination_types_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_eviction, dnx_data_crps_eviction_define_nof_eviction_destination_types);
}

uint32
dnx_data_crps_eviction_nof_crps_network_channels_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_eviction, dnx_data_crps_eviction_define_nof_crps_network_channels);
}

uint32
dnx_data_crps_eviction_bg_thread_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_eviction, dnx_data_crps_eviction_define_bg_thread_enable);
}



const dnx_data_crps_eviction_phy_record_format_t *
dnx_data_crps_eviction_phy_record_format_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_eviction, dnx_data_crps_eviction_table_phy_record_format);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_crps_eviction_phy_record_format_t *) data;

}

const dnx_data_crps_eviction_condional_action_valid_t *
dnx_data_crps_eviction_condional_action_valid_get(
    int unit,
    int action_idx)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_eviction, dnx_data_crps_eviction_table_condional_action_valid);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, action_idx, 0);
    return (const dnx_data_crps_eviction_condional_action_valid_t *) data;

}

const dnx_data_crps_eviction_supported_eviction_destination_t *
dnx_data_crps_eviction_supported_eviction_destination_get(
    int unit,
    int destination_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_eviction, dnx_data_crps_eviction_table_supported_eviction_destination);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, destination_type, 0);
    return (const dnx_data_crps_eviction_supported_eviction_destination_t *) data;

}


shr_error_e
dnx_data_crps_eviction_phy_record_format_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_crps_eviction_phy_record_format_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_eviction, dnx_data_crps_eviction_table_phy_record_format);
    data = (const dnx_data_crps_eviction_phy_record_format_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->obj_type_size);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->obj_type_offset);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reserved0_size);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reserved0_offset);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->opcode_size);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->opcode_offset);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->err_ind_size);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->err_ind_offset);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reserved1_size);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reserved1_offset);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->core_id_size);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->core_id_offset);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->engine_id_size);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->engine_id_offset);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->local_ctr_id_size);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->local_ctr_id_offset);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ctr_value_size);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ctr_value_offset);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_probabilistic_size);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_probabilistic_offset);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_crps_eviction_condional_action_valid_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_crps_eviction_condional_action_valid_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_eviction, dnx_data_crps_eviction_table_condional_action_valid);
    data = (const dnx_data_crps_eviction_condional_action_valid_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_crps_eviction_supported_eviction_destination_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_crps_eviction_supported_eviction_destination_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_eviction, dnx_data_crps_eviction_table_supported_eviction_destination);
    data = (const dnx_data_crps_eviction_supported_eviction_destination_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_crps_eviction_phy_record_format_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_eviction, dnx_data_crps_eviction_table_phy_record_format);

}

const dnxc_data_table_info_t *
dnx_data_crps_eviction_condional_action_valid_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_eviction, dnx_data_crps_eviction_table_condional_action_valid);

}

const dnxc_data_table_info_t *
dnx_data_crps_eviction_supported_eviction_destination_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_eviction, dnx_data_crps_eviction_table_supported_eviction_destination);

}






static shr_error_e
dnx_data_crps_expansion_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "expansion";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_crps_expansion_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data crps expansion features");

    
    submodule_data->nof_defines = _dnx_data_crps_expansion_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data crps expansion defines");

    submodule_data->defines[dnx_data_crps_expansion_define_expansion_size].name = "expansion_size";
    submodule_data->defines[dnx_data_crps_expansion_define_expansion_size].doc = "";
    
    submodule_data->defines[dnx_data_crps_expansion_define_expansion_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_crps_expansion_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data crps expansion tables");

    
    submodule_data->tables[dnx_data_crps_expansion_table_source_metadata_types].name = "source_metadata_types";
    submodule_data->tables[dnx_data_crps_expansion_table_source_metadata_types].doc = "";
    submodule_data->tables[dnx_data_crps_expansion_table_source_metadata_types].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_expansion_table_source_metadata_types].size_of_values = sizeof(dnx_data_crps_expansion_source_metadata_types_t);
    submodule_data->tables[dnx_data_crps_expansion_table_source_metadata_types].entry_get = dnx_data_crps_expansion_source_metadata_types_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_expansion_table_source_metadata_types].nof_keys = 2;
    submodule_data->tables[dnx_data_crps_expansion_table_source_metadata_types].keys[0].name = "source";
    submodule_data->tables[dnx_data_crps_expansion_table_source_metadata_types].keys[0].doc = "";
    submodule_data->tables[dnx_data_crps_expansion_table_source_metadata_types].keys[1].name = "metadata_type";
    submodule_data->tables[dnx_data_crps_expansion_table_source_metadata_types].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_crps_expansion_table_source_metadata_types].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_expansion_table_source_metadata_types].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_expansion_table_source_metadata_types].nof_values, "_dnx_data_crps_expansion_table_source_metadata_types table values");
    submodule_data->tables[dnx_data_crps_expansion_table_source_metadata_types].values[0].name = "offset";
    submodule_data->tables[dnx_data_crps_expansion_table_source_metadata_types].values[0].type = "int";
    submodule_data->tables[dnx_data_crps_expansion_table_source_metadata_types].values[0].doc = "";
    submodule_data->tables[dnx_data_crps_expansion_table_source_metadata_types].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_source_metadata_types_t, offset);
    submodule_data->tables[dnx_data_crps_expansion_table_source_metadata_types].values[1].name = "nof_bits";
    submodule_data->tables[dnx_data_crps_expansion_table_source_metadata_types].values[1].type = "int";
    submodule_data->tables[dnx_data_crps_expansion_table_source_metadata_types].values[1].doc = "";
    submodule_data->tables[dnx_data_crps_expansion_table_source_metadata_types].values[1].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_source_metadata_types_t, nof_bits);

    
    submodule_data->tables[dnx_data_crps_expansion_table_source_expansion_info].name = "source_expansion_info";
    submodule_data->tables[dnx_data_crps_expansion_table_source_expansion_info].doc = "";
    submodule_data->tables[dnx_data_crps_expansion_table_source_expansion_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_expansion_table_source_expansion_info].size_of_values = sizeof(dnx_data_crps_expansion_source_expansion_info_t);
    submodule_data->tables[dnx_data_crps_expansion_table_source_expansion_info].entry_get = dnx_data_crps_expansion_source_expansion_info_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_expansion_table_source_expansion_info].nof_keys = 1;
    submodule_data->tables[dnx_data_crps_expansion_table_source_expansion_info].keys[0].name = "source";
    submodule_data->tables[dnx_data_crps_expansion_table_source_expansion_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_crps_expansion_table_source_expansion_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_expansion_table_source_expansion_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_expansion_table_source_expansion_info].nof_values, "_dnx_data_crps_expansion_table_source_expansion_info table values");
    submodule_data->tables[dnx_data_crps_expansion_table_source_expansion_info].values[0].name = "expansion_size";
    submodule_data->tables[dnx_data_crps_expansion_table_source_expansion_info].values[0].type = "int";
    submodule_data->tables[dnx_data_crps_expansion_table_source_expansion_info].values[0].doc = "";
    submodule_data->tables[dnx_data_crps_expansion_table_source_expansion_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_source_expansion_info_t, expansion_size);
    submodule_data->tables[dnx_data_crps_expansion_table_source_expansion_info].values[1].name = "supports_expansion_per_type";
    submodule_data->tables[dnx_data_crps_expansion_table_source_expansion_info].values[1].type = "int";
    submodule_data->tables[dnx_data_crps_expansion_table_source_expansion_info].values[1].doc = "";
    submodule_data->tables[dnx_data_crps_expansion_table_source_expansion_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_source_expansion_info_t, supports_expansion_per_type);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_crps_expansion_feature_get(
    int unit,
    dnx_data_crps_expansion_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, feature);
}


uint32
dnx_data_crps_expansion_expansion_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_define_expansion_size);
}



const dnx_data_crps_expansion_source_metadata_types_t *
dnx_data_crps_expansion_source_metadata_types_get(
    int unit,
    int source,
    int metadata_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_source_metadata_types);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, source, metadata_type);
    return (const dnx_data_crps_expansion_source_metadata_types_t *) data;

}

const dnx_data_crps_expansion_source_expansion_info_t *
dnx_data_crps_expansion_source_expansion_info_get(
    int unit,
    int source)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_source_expansion_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, source, 0);
    return (const dnx_data_crps_expansion_source_expansion_info_t *) data;

}


shr_error_e
dnx_data_crps_expansion_source_metadata_types_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_crps_expansion_source_metadata_types_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_source_metadata_types);
    data = (const dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->offset);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_bits);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_crps_expansion_source_expansion_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_crps_expansion_source_expansion_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_source_expansion_info);
    data = (const dnx_data_crps_expansion_source_expansion_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->expansion_size);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->supports_expansion_per_type);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_crps_expansion_source_metadata_types_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_source_metadata_types);

}

const dnxc_data_table_info_t *
dnx_data_crps_expansion_source_expansion_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_source_expansion_info);

}






static shr_error_e
dnx_data_crps_src_interface_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "src_interface";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_crps_src_interface_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data crps src_interface features");

    submodule_data->features[dnx_data_crps_src_interface_etm_count_pp_port_enable_table_is_supported].name = "etm_count_pp_port_enable_table_is_supported";
    submodule_data->features[dnx_data_crps_src_interface_etm_count_pp_port_enable_table_is_supported].doc = "";
    submodule_data->features[dnx_data_crps_src_interface_etm_count_pp_port_enable_table_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_crps_src_interface_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data crps src_interface defines");

    
    submodule_data->nof_tables = _dnx_data_crps_src_interface_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data crps src_interface tables");

    
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].name = "command_id";
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].doc = "";
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].size_of_values = sizeof(dnx_data_crps_src_interface_command_id_t);
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].entry_get = dnx_data_crps_src_interface_command_id_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].nof_keys = 1;
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].keys[0].name = "source";
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_src_interface_table_command_id].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_src_interface_table_command_id].nof_values, "_dnx_data_crps_src_interface_table_command_id table values");
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].values[0].name = "size";
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].values[0].type = "uint32";
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].values[0].doc = "";
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_src_interface_command_id_t, size);
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].values[1].name = "base";
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].values[1].type = "uint32";
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].values[1].doc = "";
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].values[1].offset = UTILEX_OFFSETOF(dnx_data_crps_src_interface_command_id_t, base);

    
    submodule_data->tables[dnx_data_crps_src_interface_table_crps_counting].name = "crps_counting";
    submodule_data->tables[dnx_data_crps_src_interface_table_crps_counting].doc = "";
    submodule_data->tables[dnx_data_crps_src_interface_table_crps_counting].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_src_interface_table_crps_counting].size_of_values = sizeof(dnx_data_crps_src_interface_crps_counting_t);
    submodule_data->tables[dnx_data_crps_src_interface_table_crps_counting].entry_get = dnx_data_crps_src_interface_crps_counting_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_src_interface_table_crps_counting].nof_keys = 1;
    submodule_data->tables[dnx_data_crps_src_interface_table_crps_counting].keys[0].name = "source";
    submodule_data->tables[dnx_data_crps_src_interface_table_crps_counting].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_crps_src_interface_table_crps_counting].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_src_interface_table_crps_counting].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_src_interface_table_crps_counting].nof_values, "_dnx_data_crps_src_interface_table_crps_counting table values");
    submodule_data->tables[dnx_data_crps_src_interface_table_crps_counting].values[0].name = "valid_for_counting";
    submodule_data->tables[dnx_data_crps_src_interface_table_crps_counting].values[0].type = "int";
    submodule_data->tables[dnx_data_crps_src_interface_table_crps_counting].values[0].doc = "";
    submodule_data->tables[dnx_data_crps_src_interface_table_crps_counting].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_src_interface_crps_counting_t, valid_for_counting);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_crps_src_interface_feature_get(
    int unit,
    dnx_data_crps_src_interface_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_src_interface, feature);
}




const dnx_data_crps_src_interface_command_id_t *
dnx_data_crps_src_interface_command_id_get(
    int unit,
    int source)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_src_interface, dnx_data_crps_src_interface_table_command_id);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, source, 0);
    return (const dnx_data_crps_src_interface_command_id_t *) data;

}

const dnx_data_crps_src_interface_crps_counting_t *
dnx_data_crps_src_interface_crps_counting_get(
    int unit,
    int source)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_src_interface, dnx_data_crps_src_interface_table_crps_counting);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, source, 0);
    return (const dnx_data_crps_src_interface_crps_counting_t *) data;

}


shr_error_e
dnx_data_crps_src_interface_command_id_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_crps_src_interface_command_id_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_src_interface, dnx_data_crps_src_interface_table_command_id);
    data = (const dnx_data_crps_src_interface_command_id_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->size);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->base);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_crps_src_interface_crps_counting_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_crps_src_interface_crps_counting_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_src_interface, dnx_data_crps_src_interface_table_crps_counting);
    data = (const dnx_data_crps_src_interface_crps_counting_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid_for_counting);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_crps_src_interface_command_id_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_src_interface, dnx_data_crps_src_interface_table_command_id);

}

const dnxc_data_table_info_t *
dnx_data_crps_src_interface_crps_counting_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_src_interface, dnx_data_crps_src_interface_table_crps_counting);

}






static shr_error_e
dnx_data_crps_latency_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "latency";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_crps_latency_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data crps latency features");

    submodule_data->features[dnx_data_crps_latency_valid_flow_profile].name = "valid_flow_profile";
    submodule_data->features[dnx_data_crps_latency_valid_flow_profile].doc = "";
    submodule_data->features[dnx_data_crps_latency_valid_flow_profile].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_latency_valid_end_to_end_aqm_profile].name = "valid_end_to_end_aqm_profile";
    submodule_data->features[dnx_data_crps_latency_valid_end_to_end_aqm_profile].doc = "";
    submodule_data->features[dnx_data_crps_latency_valid_end_to_end_aqm_profile].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_latency_drop_probabilistic_mechanism_support].name = "drop_probabilistic_mechanism_support";
    submodule_data->features[dnx_data_crps_latency_drop_probabilistic_mechanism_support].doc = "";
    submodule_data->features[dnx_data_crps_latency_drop_probabilistic_mechanism_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_crps_latency_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data crps latency defines");

    submodule_data->defines[dnx_data_crps_latency_define_etpp_command_id].name = "etpp_command_id";
    submodule_data->defines[dnx_data_crps_latency_define_etpp_command_id].doc = "";
    
    submodule_data->defines[dnx_data_crps_latency_define_etpp_command_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_latency_define_stat_id_port_offset].name = "stat_id_port_offset";
    submodule_data->defines[dnx_data_crps_latency_define_stat_id_port_offset].doc = "";
    
    submodule_data->defines[dnx_data_crps_latency_define_stat_id_port_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_latency_define_stat_id_tc_offset].name = "stat_id_tc_offset";
    submodule_data->defines[dnx_data_crps_latency_define_stat_id_tc_offset].doc = "";
    
    submodule_data->defines[dnx_data_crps_latency_define_stat_id_tc_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_latency_define_stat_id_multicast_offset].name = "stat_id_multicast_offset";
    submodule_data->defines[dnx_data_crps_latency_define_stat_id_multicast_offset].doc = "";
    
    submodule_data->defines[dnx_data_crps_latency_define_stat_id_multicast_offset].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_crps_latency_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data crps latency tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_crps_latency_feature_get(
    int unit,
    dnx_data_crps_latency_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_latency, feature);
}


uint32
dnx_data_crps_latency_etpp_command_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_latency, dnx_data_crps_latency_define_etpp_command_id);
}

uint32
dnx_data_crps_latency_stat_id_port_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_latency, dnx_data_crps_latency_define_stat_id_port_offset);
}

uint32
dnx_data_crps_latency_stat_id_tc_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_latency, dnx_data_crps_latency_define_stat_id_tc_offset);
}

uint32
dnx_data_crps_latency_stat_id_multicast_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_latency, dnx_data_crps_latency_define_stat_id_multicast_offset);
}










static shr_error_e
dnx_data_crps_state_counters_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "state_counters";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_crps_state_counters_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data crps state_counters features");

    
    submodule_data->nof_defines = _dnx_data_crps_state_counters_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data crps state_counters defines");

    submodule_data->defines[dnx_data_crps_state_counters_define_max_wide_state_counters_size].name = "max_wide_state_counters_size";
    submodule_data->defines[dnx_data_crps_state_counters_define_max_wide_state_counters_size].doc = "";
    
    submodule_data->defines[dnx_data_crps_state_counters_define_max_wide_state_counters_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_state_counters_define_max_narrow_state_counters_size].name = "max_narrow_state_counters_size";
    submodule_data->defines[dnx_data_crps_state_counters_define_max_narrow_state_counters_size].doc = "";
    
    submodule_data->defines[dnx_data_crps_state_counters_define_max_narrow_state_counters_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_state_counters_define_state_counter_data_size_per_full_counter].name = "state_counter_data_size_per_full_counter";
    submodule_data->defines[dnx_data_crps_state_counters_define_state_counter_data_size_per_full_counter].doc = "";
    
    submodule_data->defines[dnx_data_crps_state_counters_define_state_counter_data_size_per_full_counter].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_crps_state_counters_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data crps state_counters tables");

    
    submodule_data->tables[dnx_data_crps_state_counters_table_counter_size_per_format_type].name = "counter_size_per_format_type";
    submodule_data->tables[dnx_data_crps_state_counters_table_counter_size_per_format_type].doc = "";
    submodule_data->tables[dnx_data_crps_state_counters_table_counter_size_per_format_type].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_state_counters_table_counter_size_per_format_type].size_of_values = sizeof(dnx_data_crps_state_counters_counter_size_per_format_type_t);
    submodule_data->tables[dnx_data_crps_state_counters_table_counter_size_per_format_type].entry_get = dnx_data_crps_state_counters_counter_size_per_format_type_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_state_counters_table_counter_size_per_format_type].nof_keys = 2;
    submodule_data->tables[dnx_data_crps_state_counters_table_counter_size_per_format_type].keys[0].name = "format_type";
    submodule_data->tables[dnx_data_crps_state_counters_table_counter_size_per_format_type].keys[0].doc = "";
    submodule_data->tables[dnx_data_crps_state_counters_table_counter_size_per_format_type].keys[1].name = "counter_size";
    submodule_data->tables[dnx_data_crps_state_counters_table_counter_size_per_format_type].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_crps_state_counters_table_counter_size_per_format_type].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_state_counters_table_counter_size_per_format_type].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_state_counters_table_counter_size_per_format_type].nof_values, "_dnx_data_crps_state_counters_table_counter_size_per_format_type table values");
    submodule_data->tables[dnx_data_crps_state_counters_table_counter_size_per_format_type].values[0].name = "valid";
    submodule_data->tables[dnx_data_crps_state_counters_table_counter_size_per_format_type].values[0].type = "int";
    submodule_data->tables[dnx_data_crps_state_counters_table_counter_size_per_format_type].values[0].doc = "";
    submodule_data->tables[dnx_data_crps_state_counters_table_counter_size_per_format_type].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_state_counters_counter_size_per_format_type_t, valid);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_crps_state_counters_feature_get(
    int unit,
    dnx_data_crps_state_counters_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_state_counters, feature);
}


uint32
dnx_data_crps_state_counters_max_wide_state_counters_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_state_counters, dnx_data_crps_state_counters_define_max_wide_state_counters_size);
}

uint32
dnx_data_crps_state_counters_max_narrow_state_counters_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_state_counters, dnx_data_crps_state_counters_define_max_narrow_state_counters_size);
}

uint32
dnx_data_crps_state_counters_state_counter_data_size_per_full_counter_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_state_counters, dnx_data_crps_state_counters_define_state_counter_data_size_per_full_counter);
}



const dnx_data_crps_state_counters_counter_size_per_format_type_t *
dnx_data_crps_state_counters_counter_size_per_format_type_get(
    int unit,
    int format_type,
    int counter_size)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_state_counters, dnx_data_crps_state_counters_table_counter_size_per_format_type);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, format_type, counter_size);
    return (const dnx_data_crps_state_counters_counter_size_per_format_type_t *) data;

}


shr_error_e
dnx_data_crps_state_counters_counter_size_per_format_type_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_crps_state_counters_counter_size_per_format_type_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_state_counters, dnx_data_crps_state_counters_table_counter_size_per_format_type);
    data = (const dnx_data_crps_state_counters_counter_size_per_format_type_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_crps_state_counters_counter_size_per_format_type_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_state_counters, dnx_data_crps_state_counters_table_counter_size_per_format_type);

}






static shr_error_e
dnx_data_crps_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_crps_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data crps general features");

    submodule_data->features[dnx_data_crps_general_crps_expansion_support].name = "crps_expansion_support";
    submodule_data->features[dnx_data_crps_general_crps_expansion_support].doc = "";
    submodule_data->features[dnx_data_crps_general_crps_expansion_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_general_count_mc_as_one_support].name = "count_mc_as_one_support";
    submodule_data->features[dnx_data_crps_general_count_mc_as_one_support].doc = "";
    submodule_data->features[dnx_data_crps_general_count_mc_as_one_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_general_crps_compensation_support].name = "crps_compensation_support";
    submodule_data->features[dnx_data_crps_general_crps_compensation_support].doc = "";
    submodule_data->features[dnx_data_crps_general_crps_compensation_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_general_crps_set_eviction_boundaries_support].name = "crps_set_eviction_boundaries_support";
    submodule_data->features[dnx_data_crps_general_crps_set_eviction_boundaries_support].doc = "";
    submodule_data->features[dnx_data_crps_general_crps_set_eviction_boundaries_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_general_is_used].name = "is_used";
    submodule_data->features[dnx_data_crps_general_is_used].doc = "";
    submodule_data->features[dnx_data_crps_general_is_used].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_general_power_down_supported].name = "power_down_supported";
    submodule_data->features[dnx_data_crps_general_power_down_supported].doc = "";
    submodule_data->features[dnx_data_crps_general_power_down_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_general_crps_rate_calculation_support].name = "crps_rate_calculation_support";
    submodule_data->features[dnx_data_crps_general_crps_rate_calculation_support].doc = "";
    submodule_data->features[dnx_data_crps_general_crps_rate_calculation_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_crps_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data crps general defines");

    
    submodule_data->nof_tables = _dnx_data_crps_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data crps general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_crps_general_feature_get(
    int unit,
    dnx_data_crps_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_general, feature);
}








shr_error_e
dnx_data_crps_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "crps";
    module_data->nof_submodules = _dnx_data_crps_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data crps submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_crps_engine_init(unit, &module_data->submodules[dnx_data_crps_submodule_engine]));
    SHR_IF_ERR_EXIT(dnx_data_crps_eviction_init(unit, &module_data->submodules[dnx_data_crps_submodule_eviction]));
    SHR_IF_ERR_EXIT(dnx_data_crps_expansion_init(unit, &module_data->submodules[dnx_data_crps_submodule_expansion]));
    SHR_IF_ERR_EXIT(dnx_data_crps_src_interface_init(unit, &module_data->submodules[dnx_data_crps_submodule_src_interface]));
    SHR_IF_ERR_EXIT(dnx_data_crps_latency_init(unit, &module_data->submodules[dnx_data_crps_submodule_latency]));
    SHR_IF_ERR_EXIT(dnx_data_crps_state_counters_init(unit, &module_data->submodules[dnx_data_crps_submodule_state_counters]));
    SHR_IF_ERR_EXIT(dnx_data_crps_general_init(unit, &module_data->submodules[dnx_data_crps_submodule_general]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_crps_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_crps_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_crps_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_crps_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_crps_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_crps_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_crps_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_crps_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_crps_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_crps_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_crps_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_crps_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_crps_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_crps_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_crps_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_crps_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_crps_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_crps_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_crps_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a1_data_crps_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_crps_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_crps_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_crps_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_crps_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

