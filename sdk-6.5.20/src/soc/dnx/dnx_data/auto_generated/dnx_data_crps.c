

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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



static shr_error_e
dnx_data_crps_engine_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "engine";
    submodule_data->doc = "counters engine data";
    
    submodule_data->nof_features = _dnx_data_crps_engine_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data crps engine features");

    submodule_data->features[dnx_data_crps_engine_valid_address_admission_is_supported].name = "valid_address_admission_is_supported";
    submodule_data->features[dnx_data_crps_engine_valid_address_admission_is_supported].doc = "Is correct address admission supported for both double mode and not double mode?";
    submodule_data->features[dnx_data_crps_engine_valid_address_admission_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_engine_last_input_command_is_supported].name = "last_input_command_is_supported";
    submodule_data->features[dnx_data_crps_engine_last_input_command_is_supported].doc = "Is diagnostic presenting last input command towards crps supported?";
    submodule_data->features[dnx_data_crps_engine_last_input_command_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_crps_engine_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data crps engine defines");

    submodule_data->defines[dnx_data_crps_engine_define_nof_engines].name = "nof_engines";
    submodule_data->defines[dnx_data_crps_engine_define_nof_engines].doc = "Number of counter engines per core";
    
    submodule_data->defines[dnx_data_crps_engine_define_nof_engines].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_engine_define_nof_mid_engines].name = "nof_mid_engines";
    submodule_data->defines[dnx_data_crps_engine_define_nof_mid_engines].doc = "Number of mid counter engines per core";
    
    submodule_data->defines[dnx_data_crps_engine_define_nof_mid_engines].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_engine_define_nof_big_engines].name = "nof_big_engines";
    submodule_data->defines[dnx_data_crps_engine_define_nof_big_engines].doc = "Number of big counter engines per core";
    
    submodule_data->defines[dnx_data_crps_engine_define_nof_big_engines].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_engine_define_nof_databases].name = "nof_databases";
    submodule_data->defines[dnx_data_crps_engine_define_nof_databases].doc = "Number of counter databases per core";
    
    submodule_data->defines[dnx_data_crps_engine_define_nof_databases].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_engine_define_data_mapping_table_size].name = "data_mapping_table_size";
    submodule_data->defines[dnx_data_crps_engine_define_data_mapping_table_size].doc = "Number of entries in data mappnig table";
    
    submodule_data->defines[dnx_data_crps_engine_define_data_mapping_table_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_engine_define_max_counter_set_size].name = "max_counter_set_size";
    submodule_data->defines[dnx_data_crps_engine_define_max_counter_set_size].doc = "max size of the counter set";
    
    submodule_data->defines[dnx_data_crps_engine_define_max_counter_set_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_engine_define_oam_counter_set_size].name = "oam_counter_set_size";
    submodule_data->defines[dnx_data_crps_engine_define_oam_counter_set_size].doc = "size of the counter set for OAM sources";
    
    submodule_data->defines[dnx_data_crps_engine_define_oam_counter_set_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_engine_define_last_address_nof_bits].name = "last_address_nof_bits";
    submodule_data->defines[dnx_data_crps_engine_define_last_address_nof_bits].doc = "number of bits of last address admitted per engine";
    
    submodule_data->defines[dnx_data_crps_engine_define_last_address_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_crps_engine_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data crps engine tables");

    
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].name = "engines_info";
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].doc = "size of each counter engine and indication if it is shared with meter";
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].size_of_values = sizeof(dnx_data_crps_engine_engines_info_t);
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].entry_get = dnx_data_crps_engine_engines_info_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].nof_keys = 1;
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].keys[0].name = "engine_id";
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].keys[0].doc = "engine id {0..(nof_counter_engines-1)}";

    
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_engine_table_engines_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_engine_table_engines_info].nof_values, "_dnx_data_crps_engine_table_engines_info table values");
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].values[0].name = "nof_counters";
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].values[0].type = "int";
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].values[0].doc = "number of counters per engine";
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_engine_engines_info_t, nof_counters);
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].values[1].name = "meter_shared";
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].values[1].type = "int";
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].values[1].doc = "is engine is memory shared with meter";
    submodule_data->tables[dnx_data_crps_engine_table_engines_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_crps_engine_engines_info_t, meter_shared);

    
    submodule_data->tables[dnx_data_crps_engine_table_source_base].name = "source_base";
    submodule_data->tables[dnx_data_crps_engine_table_source_base].doc = "defines the base value for the source which is connected to the engine";
    submodule_data->tables[dnx_data_crps_engine_table_source_base].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_engine_table_source_base].size_of_values = sizeof(dnx_data_crps_engine_source_base_t);
    submodule_data->tables[dnx_data_crps_engine_table_source_base].entry_get = dnx_data_crps_engine_source_base_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_engine_table_source_base].nof_keys = 1;
    submodule_data->tables[dnx_data_crps_engine_table_source_base].keys[0].name = "source";
    submodule_data->tables[dnx_data_crps_engine_table_source_base].keys[0].doc = "source id {according to API enum}";

    
    submodule_data->tables[dnx_data_crps_engine_table_source_base].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_engine_table_source_base].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_engine_table_source_base].nof_values, "_dnx_data_crps_engine_table_source_base table values");
    submodule_data->tables[dnx_data_crps_engine_table_source_base].values[0].name = "base_val";
    submodule_data->tables[dnx_data_crps_engine_table_source_base].values[0].type = "int";
    submodule_data->tables[dnx_data_crps_engine_table_source_base].values[0].doc = "the base value for the input source, in order to configure CRPS_ENGINE_SOURCE_SELECT field";
    submodule_data->tables[dnx_data_crps_engine_table_source_base].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_engine_source_base_t, base_val);


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
dnx_data_crps_engine_last_address_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_engine, dnx_data_crps_engine_define_last_address_nof_bits);
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
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_counters);
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






static shr_error_e
dnx_data_crps_eviction_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "eviction";
    submodule_data->doc = "counters eviction parameters";
    
    submodule_data->nof_features = _dnx_data_crps_eviction_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data crps eviction features");

    submodule_data->features[dnx_data_crps_eviction_algorithmic_is_supported].name = "algorithmic_is_supported";
    submodule_data->features[dnx_data_crps_eviction_algorithmic_is_supported].doc = "Is algorithmic scan mode supported?";
    submodule_data->features[dnx_data_crps_eviction_algorithmic_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_eviction_probabilistic_is_supported].name = "probabilistic_is_supported";
    submodule_data->features[dnx_data_crps_eviction_probabilistic_is_supported].doc = "Is probabilistic scan mode supported?";
    submodule_data->features[dnx_data_crps_eviction_probabilistic_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_eviction_conditional_action_is_supported].name = "conditional_action_is_supported";
    submodule_data->features[dnx_data_crps_eviction_conditional_action_is_supported].doc = "Is conditional action scan is supported";
    submodule_data->features[dnx_data_crps_eviction_conditional_action_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_eviction_narrow_counters_is_supported].name = "narrow_counters_is_supported";
    submodule_data->features[dnx_data_crps_eviction_narrow_counters_is_supported].doc = "Is narrow counters eviction supported";
    submodule_data->features[dnx_data_crps_eviction_narrow_counters_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_crps_eviction_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data crps eviction defines");

    submodule_data->defines[dnx_data_crps_eviction_define_counters_fifo_depth].name = "counters_fifo_depth";
    submodule_data->defines[dnx_data_crps_eviction_define_counters_fifo_depth].doc = "Indicates the FIFO depth in the host";
    
    submodule_data->defines[dnx_data_crps_eviction_define_counters_fifo_depth].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_eviction_define_nof_counters_fifo].name = "nof_counters_fifo";
    submodule_data->defines[dnx_data_crps_eviction_define_nof_counters_fifo].doc = "Indicates the number of FIFOs available for CRPS (for each core)";
    
    submodule_data->defines[dnx_data_crps_eviction_define_nof_counters_fifo].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_eviction_define_dma_record_entry_nof_bits].name = "dma_record_entry_nof_bits";
    submodule_data->defines[dnx_data_crps_eviction_define_dma_record_entry_nof_bits].doc = "size (in bits) of one record that is copied by the DMA-FIFO";
    
    submodule_data->defines[dnx_data_crps_eviction_define_dma_record_entry_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_eviction_define_packet_and_bytes_packet_size].name = "packet_and_bytes_packet_size";
    submodule_data->defines[dnx_data_crps_eviction_define_packet_and_bytes_packet_size].doc = "size (in bits) of the packets, if format is packet and bytes";
    
    submodule_data->defines[dnx_data_crps_eviction_define_packet_and_bytes_packet_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_eviction_define_bubble_before_request_timeout].name = "bubble_before_request_timeout";
    submodule_data->defines[dnx_data_crps_eviction_define_bubble_before_request_timeout].doc = "The waiting period in clocks until requesting bubble, if EnginenScanBubbleRequestEnable=1";
    
    submodule_data->defines[dnx_data_crps_eviction_define_bubble_before_request_timeout].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_eviction_define_bubble_after_request_timeout].name = "bubble_after_request_timeout";
    submodule_data->defines[dnx_data_crps_eviction_define_bubble_after_request_timeout].doc = "The waiting period, after bubble reuest before forcing bubble (if force enable)";
    
    submodule_data->defines[dnx_data_crps_eviction_define_bubble_after_request_timeout].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_eviction_define_seq_address_nof_bits].name = "seq_address_nof_bits";
    submodule_data->defines[dnx_data_crps_eviction_define_seq_address_nof_bits].doc = "nof bit for sequential address (start/end)";
    
    submodule_data->defines[dnx_data_crps_eviction_define_seq_address_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_eviction_define_bg_thread_enable].name = "bg_thread_enable";
    submodule_data->defines[dnx_data_crps_eviction_define_bg_thread_enable].doc = "enable/disable counters bg thread operation";
    
    submodule_data->defines[dnx_data_crps_eviction_define_bg_thread_enable].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_crps_eviction_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data crps eviction tables");

    
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].name = "phy_record_format";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].doc = "holds the format of the physical record that is copied by the DMA from CRPS FIFO";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].size_of_values = sizeof(dnx_data_crps_eviction_phy_record_format_t);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].entry_get = dnx_data_crps_eviction_phy_record_format_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].nof_keys = 0;

    
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].nof_values = 18;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].nof_values, "_dnx_data_crps_eviction_table_phy_record_format table values");
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[0].name = "obj_type_size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[0].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[0].doc = "size 'obj_type' field base on its size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, obj_type_size);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[1].name = "obj_type_offset";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[1].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[1].doc = "offset in the record for obj_type field";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[1].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, obj_type_offset);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[2].name = "reserved0_size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[2].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[2].doc = "size 'reserved0' field base on its size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[2].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, reserved0_size);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[3].name = "reserved0_offset";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[3].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[3].doc = "offset in the record for 'reserved0' field";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[3].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, reserved0_offset);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[4].name = "opcode_size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[4].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[4].doc = "'opcode' type field base on its size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[4].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, opcode_size);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[5].name = "opcode_offset";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[5].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[5].doc = "offset in the record for 'opcode' field";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[5].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, opcode_offset);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[6].name = "err_ind_size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[6].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[6].doc = "size 'error indication' field base on its size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[6].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, err_ind_size);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[7].name = "err_ind_offset";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[7].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[7].doc = "offset in the record for 'error indication' field";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[7].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, err_ind_offset);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[8].name = "reserved1_size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[8].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[8].doc = "size 'reserved1' field base on its size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[8].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, reserved1_size);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[9].name = "reserved1_offset";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[9].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[9].doc = "offset in the record for 'reserved1' field";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[9].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, reserved1_offset);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[10].name = "core_id_size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[10].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[10].doc = "size 'core id' field base on its size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[10].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, core_id_size);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[11].name = "core_id_offset";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[11].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[11].doc = "offset in the record for 'core id' field";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[11].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, core_id_offset);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[12].name = "engine_id_size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[12].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[12].doc = "size 'engine id' field base on its size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[12].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, engine_id_size);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[13].name = "engine_id_offset";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[13].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[13].doc = "offset in the record for 'engine id' field";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[13].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, engine_id_offset);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[14].name = "local_ctr_id_size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[14].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[14].doc = "size 'local counter id' field base on its size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[14].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, local_ctr_id_size);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[15].name = "local_ctr_id_offset";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[15].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[15].doc = "offset in the record for 'local counter id' field";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[15].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, local_ctr_id_offset);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[16].name = "ctr_value_size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[16].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[16].doc = "size 'counter value' field base on its size";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[16].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, ctr_value_size);
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[17].name = "ctr_value_offset";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[17].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[17].doc = "offset in the record for 'counter value' field";
    submodule_data->tables[dnx_data_crps_eviction_table_phy_record_format].values[17].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_phy_record_format_t, ctr_value_offset);

    
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].name = "condional_action_valid";
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].doc = "holds which action is valid";
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].size_of_values = sizeof(dnx_data_crps_eviction_condional_action_valid_t);
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].entry_get = dnx_data_crps_eviction_condional_action_valid_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].nof_keys = 1;
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].keys[0].name = "action_idx";
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].keys[0].doc = "action idx flag {according to API flags}";

    
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].nof_values, "_dnx_data_crps_eviction_table_condional_action_valid table values");
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].values[0].name = "valid";
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].values[0].type = "uint32";
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].values[0].doc = "condional action valid";
    submodule_data->tables[dnx_data_crps_eviction_table_condional_action_valid].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_eviction_condional_action_valid_t, valid);


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
dnx_data_crps_eviction_packet_and_bytes_packet_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_eviction, dnx_data_crps_eviction_define_packet_and_bytes_packet_size);
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






static shr_error_e
dnx_data_crps_expansion_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "expansion";
    submodule_data->doc = "counters expansion parameters";
    
    submodule_data->nof_features = _dnx_data_crps_expansion_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data crps expansion features");

    
    submodule_data->nof_defines = _dnx_data_crps_expansion_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data crps expansion defines");

    submodule_data->defines[dnx_data_crps_expansion_define_expansion_size].name = "expansion_size";
    submodule_data->defines[dnx_data_crps_expansion_define_expansion_size].doc = "define the size of the expansion (in bits)";
    
    submodule_data->defines[dnx_data_crps_expansion_define_expansion_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_crps_expansion_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data crps expansion tables");

    
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].name = "irpp_types";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].doc = "gather all the field types that construct the irpp metadata";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].size_of_values = sizeof(dnx_data_crps_expansion_irpp_types_t);
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].entry_get = dnx_data_crps_expansion_irpp_types_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].nof_keys = 0;

    
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].nof_values = 14;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].nof_values, "_dnx_data_crps_expansion_table_irpp_types table values");
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[0].name = "valid_types";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[0].type = "bcm_stat_expansion_types_t[12]";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[0].doc = "valid field types that construct the irpp metadata";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_irpp_types_t, valid_types);
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[1].name = "total_nof_bits";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[1].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[1].doc = "total size of bits for all fields";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[1].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_irpp_types_t, total_nof_bits);
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[2].name = "drop_reason_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[2].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[2].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[2].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_irpp_types_t, drop_reason_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[3].name = "disposition_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[3].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[3].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[3].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_irpp_types_t, disposition_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[4].name = "tc_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[4].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[4].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[4].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_irpp_types_t, tc_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[5].name = "dp0_valid_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[5].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[5].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[5].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_irpp_types_t, dp0_valid_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[6].name = "dp0_value_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[6].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[6].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[6].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_irpp_types_t, dp0_value_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[7].name = "dp1_valid_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[7].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[7].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[7].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_irpp_types_t, dp1_valid_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[8].name = "dp1_value_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[8].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[8].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[8].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_irpp_types_t, dp1_value_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[9].name = "dp2_valid_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[9].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[9].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[9].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_irpp_types_t, dp2_valid_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[10].name = "dp2_value_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[10].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[10].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[10].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_irpp_types_t, dp2_value_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[11].name = "final_dp_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[11].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[11].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[11].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_irpp_types_t, final_dp_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[12].name = "reject_all_copies_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[12].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[12].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[12].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_irpp_types_t, reject_all_copies_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[13].name = "metadata_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[13].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[13].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_irpp_types].values[13].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_irpp_types_t, metadata_offset);

    
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].name = "itm_types";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].doc = "gather all the field types that construct the itm metadata";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].size_of_values = sizeof(dnx_data_crps_expansion_itm_types_t);
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].entry_get = dnx_data_crps_expansion_itm_types_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].nof_keys = 0;

    
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].nof_values = 7;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_expansion_table_itm_types].nof_values, "_dnx_data_crps_expansion_table_itm_types table values");
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[0].name = "valid_types";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[0].type = "bcm_stat_expansion_types_t[6]";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[0].doc = "valid field types that construct the itm metadata";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_itm_types_t, valid_types);
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[1].name = "total_nof_bits";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[1].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[1].doc = "total size of bits for all fields";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[1].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_itm_types_t, total_nof_bits);
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[2].name = "drop_reason_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[2].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[2].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[2].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_itm_types_t, drop_reason_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[3].name = "disposition_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[3].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[3].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[3].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_itm_types_t, disposition_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[4].name = "tc_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[4].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[4].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[4].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_itm_types_t, tc_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[5].name = "final_dp_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[5].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[5].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[5].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_itm_types_t, final_dp_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[6].name = "last_copy_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[6].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[6].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_itm_types].values[6].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_itm_types_t, last_copy_offset);

    
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].name = "itpp_types";
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].doc = "gather all the field types that construct the itpp metadata";
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].size_of_values = sizeof(dnx_data_crps_expansion_itpp_types_t);
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].entry_get = dnx_data_crps_expansion_itpp_types_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].nof_keys = 0;

    
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].nof_values = 6;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].nof_values, "_dnx_data_crps_expansion_table_itpp_types table values");
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[0].name = "valid_types";
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[0].type = "bcm_stat_expansion_types_t[6]";
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[0].doc = "valid field types that construct the itpp metadata";
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_itpp_types_t, valid_types);
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[1].name = "total_nof_bits";
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[1].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[1].doc = "total size of bits for all fields";
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[1].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_itpp_types_t, total_nof_bits);
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[2].name = "latency_bin_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[2].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[2].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[2].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_itpp_types_t, latency_bin_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[3].name = "tc_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[3].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[3].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[3].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_itpp_types_t, tc_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[4].name = "latency_profile_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[4].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[4].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[4].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_itpp_types_t, latency_profile_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[5].name = "mc_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[5].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[5].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_itpp_types].values[5].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_itpp_types_t, mc_offset);

    
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].name = "erpp_types";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].doc = "gather all the field types that construct the erpp metadata";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].size_of_values = sizeof(dnx_data_crps_expansion_erpp_types_t);
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].entry_get = dnx_data_crps_expansion_erpp_types_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].nof_keys = 0;

    
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].nof_values = 8;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].nof_values, "_dnx_data_crps_expansion_table_erpp_types table values");
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[0].name = "valid_types";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[0].type = "bcm_stat_expansion_types_t[7]";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[0].doc = "valid field types that construct the erpp metadata";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_erpp_types_t, valid_types);
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[1].name = "total_nof_bits";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[1].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[1].doc = "total size of bits for all fields";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[1].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_erpp_types_t, total_nof_bits);
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[2].name = "metadata_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[2].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[2].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[2].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_erpp_types_t, metadata_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[3].name = "dp_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[3].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[3].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[3].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_erpp_types_t, dp_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[4].name = "tc_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[4].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[4].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[4].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_erpp_types_t, tc_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[5].name = "mc_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[5].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[5].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[5].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_erpp_types_t, mc_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[6].name = "tm_action_type_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[6].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[6].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[6].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_erpp_types_t, tm_action_type_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[7].name = "pp_drop_reason_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[7].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[7].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_erpp_types].values[7].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_erpp_types_t, pp_drop_reason_offset);

    
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].name = "etm_types";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].doc = "gather all the field types that construct the etm metadata";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].size_of_values = sizeof(dnx_data_crps_expansion_etm_types_t);
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].entry_get = dnx_data_crps_expansion_etm_types_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].nof_keys = 0;

    
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].nof_values = 7;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_expansion_table_etm_types].nof_values, "_dnx_data_crps_expansion_table_etm_types table values");
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[0].name = "valid_types";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[0].type = "bcm_stat_expansion_types_t[7]";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[0].doc = "valid field types that construct the etm metadata";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_etm_types_t, valid_types);
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[1].name = "total_nof_bits";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[1].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[1].doc = "total size of bits for all fields";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[1].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_etm_types_t, total_nof_bits);
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[2].name = "drop_reason_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[2].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[2].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[2].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_etm_types_t, drop_reason_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[3].name = "tm_discard_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[3].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[3].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[3].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_etm_types_t, tm_discard_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[4].name = "pp_discard_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[4].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[4].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[4].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_etm_types_t, pp_discard_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[5].name = "tc_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[5].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[5].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[5].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_etm_types_t, tc_offset);
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[6].name = "dp_offset";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[6].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[6].doc = "field offset in the data sent from the source";
    submodule_data->tables[dnx_data_crps_expansion_table_etm_types].values[6].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_etm_types_t, dp_offset);

    
    submodule_data->tables[dnx_data_crps_expansion_table_etpp_types].name = "etpp_types";
    submodule_data->tables[dnx_data_crps_expansion_table_etpp_types].doc = "gather all the field types that construct the etpp metadata";
    submodule_data->tables[dnx_data_crps_expansion_table_etpp_types].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_expansion_table_etpp_types].size_of_values = sizeof(dnx_data_crps_expansion_etpp_types_t);
    submodule_data->tables[dnx_data_crps_expansion_table_etpp_types].entry_get = dnx_data_crps_expansion_etpp_types_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_expansion_table_etpp_types].nof_keys = 0;

    
    submodule_data->tables[dnx_data_crps_expansion_table_etpp_types].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_expansion_table_etpp_types].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_expansion_table_etpp_types].nof_values, "_dnx_data_crps_expansion_table_etpp_types table values");
    submodule_data->tables[dnx_data_crps_expansion_table_etpp_types].values[0].name = "valid_types";
    submodule_data->tables[dnx_data_crps_expansion_table_etpp_types].values[0].type = "bcm_stat_expansion_types_t[8]";
    submodule_data->tables[dnx_data_crps_expansion_table_etpp_types].values[0].doc = "valid field types that construct the etpp metadata";
    submodule_data->tables[dnx_data_crps_expansion_table_etpp_types].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_etpp_types_t, valid_types);
    submodule_data->tables[dnx_data_crps_expansion_table_etpp_types].values[1].name = "total_nof_bits";
    submodule_data->tables[dnx_data_crps_expansion_table_etpp_types].values[1].type = "uint32";
    submodule_data->tables[dnx_data_crps_expansion_table_etpp_types].values[1].doc = "total size of bits for all fields";
    submodule_data->tables[dnx_data_crps_expansion_table_etpp_types].values[1].offset = UTILEX_OFFSETOF(dnx_data_crps_expansion_etpp_types_t, total_nof_bits);


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



const dnx_data_crps_expansion_irpp_types_t *
dnx_data_crps_expansion_irpp_types_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_irpp_types);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_crps_expansion_irpp_types_t *) data;

}

const dnx_data_crps_expansion_itm_types_t *
dnx_data_crps_expansion_itm_types_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_itm_types);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_crps_expansion_itm_types_t *) data;

}

const dnx_data_crps_expansion_itpp_types_t *
dnx_data_crps_expansion_itpp_types_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_itpp_types);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_crps_expansion_itpp_types_t *) data;

}

const dnx_data_crps_expansion_erpp_types_t *
dnx_data_crps_expansion_erpp_types_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_erpp_types);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_crps_expansion_erpp_types_t *) data;

}

const dnx_data_crps_expansion_etm_types_t *
dnx_data_crps_expansion_etm_types_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_etm_types);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_crps_expansion_etm_types_t *) data;

}

const dnx_data_crps_expansion_etpp_types_t *
dnx_data_crps_expansion_etpp_types_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_etpp_types);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_crps_expansion_etpp_types_t *) data;

}


shr_error_e
dnx_data_crps_expansion_irpp_types_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_crps_expansion_irpp_types_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_irpp_types);
    data = (const dnx_data_crps_expansion_irpp_types_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, 12, data->valid_types);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->total_nof_bits);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->drop_reason_offset);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->disposition_offset);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tc_offset);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dp0_valid_offset);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dp0_value_offset);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dp1_valid_offset);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dp1_value_offset);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dp2_valid_offset);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dp2_value_offset);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->final_dp_offset);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reject_all_copies_offset);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->metadata_offset);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_crps_expansion_itm_types_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_crps_expansion_itm_types_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_itm_types);
    data = (const dnx_data_crps_expansion_itm_types_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, 6, data->valid_types);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->total_nof_bits);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->drop_reason_offset);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->disposition_offset);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tc_offset);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->final_dp_offset);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->last_copy_offset);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_crps_expansion_itpp_types_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_crps_expansion_itpp_types_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_itpp_types);
    data = (const dnx_data_crps_expansion_itpp_types_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, 6, data->valid_types);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->total_nof_bits);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->latency_bin_offset);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tc_offset);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->latency_profile_offset);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mc_offset);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_crps_expansion_erpp_types_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_crps_expansion_erpp_types_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_erpp_types);
    data = (const dnx_data_crps_expansion_erpp_types_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, 7, data->valid_types);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->total_nof_bits);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->metadata_offset);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dp_offset);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tc_offset);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mc_offset);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tm_action_type_offset);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pp_drop_reason_offset);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_crps_expansion_etm_types_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_crps_expansion_etm_types_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_etm_types);
    data = (const dnx_data_crps_expansion_etm_types_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, 7, data->valid_types);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->total_nof_bits);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->drop_reason_offset);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tm_discard_offset);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pp_discard_offset);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tc_offset);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dp_offset);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_crps_expansion_etpp_types_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_crps_expansion_etpp_types_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_etpp_types);
    data = (const dnx_data_crps_expansion_etpp_types_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, 8, data->valid_types);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->total_nof_bits);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_crps_expansion_irpp_types_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_irpp_types);

}

const dnxc_data_table_info_t *
dnx_data_crps_expansion_itm_types_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_itm_types);

}

const dnxc_data_table_info_t *
dnx_data_crps_expansion_itpp_types_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_itpp_types);

}

const dnxc_data_table_info_t *
dnx_data_crps_expansion_erpp_types_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_erpp_types);

}

const dnxc_data_table_info_t *
dnx_data_crps_expansion_etm_types_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_etm_types);

}

const dnxc_data_table_info_t *
dnx_data_crps_expansion_etpp_types_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_expansion, dnx_data_crps_expansion_table_etpp_types);

}






static shr_error_e
dnx_data_crps_src_interface_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "src_interface";
    submodule_data->doc = "source interface";
    
    submodule_data->nof_features = _dnx_data_crps_src_interface_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data crps src_interface features");

    
    submodule_data->nof_defines = _dnx_data_crps_src_interface_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data crps src_interface defines");

    
    submodule_data->nof_tables = _dnx_data_crps_src_interface_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data crps src_interface tables");

    
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].name = "command_id";
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].doc = "command_id data - possible values per source";
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].size_of_values = sizeof(dnx_data_crps_src_interface_command_id_t);
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].entry_get = dnx_data_crps_src_interface_command_id_entry_str_get;

    
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].nof_keys = 1;
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].keys[0].name = "source";
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].keys[0].doc = "interface source";

    
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_crps_src_interface_table_command_id].values, dnxc_data_value_t, submodule_data->tables[dnx_data_crps_src_interface_table_command_id].nof_values, "_dnx_data_crps_src_interface_table_command_id table values");
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].values[0].name = "size";
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].values[0].type = "uint32";
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].values[0].doc = "maximum possible value for command_id for this source";
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].values[0].offset = UTILEX_OFFSETOF(dnx_data_crps_src_interface_command_id_t, size);
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].values[1].name = "base";
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].values[1].type = "uint32";
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].values[1].doc = "the base value - minimum value of the command_id";
    submodule_data->tables[dnx_data_crps_src_interface_table_command_id].values[1].offset = UTILEX_OFFSETOF(dnx_data_crps_src_interface_command_id_t, base);


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


const dnxc_data_table_info_t *
dnx_data_crps_src_interface_command_id_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_crps, dnx_data_crps_submodule_src_interface, dnx_data_crps_src_interface_table_command_id);

}






static shr_error_e
dnx_data_crps_latency_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "latency";
    submodule_data->doc = "latency stat id definitions";
    
    submodule_data->nof_features = _dnx_data_crps_latency_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data crps latency features");

    submodule_data->features[dnx_data_crps_latency_valid_flow_profile].name = "valid_flow_profile";
    submodule_data->features[dnx_data_crps_latency_valid_flow_profile].doc = "can latency be measured per pp flow profile";
    submodule_data->features[dnx_data_crps_latency_valid_flow_profile].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_latency_valid_end_to_end_aqm_profile].name = "valid_end_to_end_aqm_profile";
    submodule_data->features[dnx_data_crps_latency_valid_end_to_end_aqm_profile].doc = "can end to end aqm profile be created ";
    submodule_data->features[dnx_data_crps_latency_valid_end_to_end_aqm_profile].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_crps_latency_probabilistic_drop_mechanism_support].name = "probabilistic_drop_mechanism_support";
    submodule_data->features[dnx_data_crps_latency_probabilistic_drop_mechanism_support].doc = "is the probibalistic mark cni/drop mechanism supported (ingress deqeue latency marking)";
    submodule_data->features[dnx_data_crps_latency_probabilistic_drop_mechanism_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_crps_latency_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data crps latency defines");

    submodule_data->defines[dnx_data_crps_latency_define_etpp_command_id].name = "etpp_command_id";
    submodule_data->defines[dnx_data_crps_latency_define_etpp_command_id].doc = "etpp command id that can be used for latency";
    
    submodule_data->defines[dnx_data_crps_latency_define_etpp_command_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_latency_define_stat_id_port_offset].name = "stat_id_port_offset";
    submodule_data->defines[dnx_data_crps_latency_define_stat_id_port_offset].doc = "offset of the port in the latency stat (flow) id  encoding";
    
    submodule_data->defines[dnx_data_crps_latency_define_stat_id_port_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_latency_define_stat_id_tc_offset].name = "stat_id_tc_offset";
    submodule_data->defines[dnx_data_crps_latency_define_stat_id_tc_offset].doc = "offset of the traffic class in the latency stat (flow) id  encoding";
    
    submodule_data->defines[dnx_data_crps_latency_define_stat_id_tc_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_crps_latency_define_stat_id_multicast_offset].name = "stat_id_multicast_offset";
    submodule_data->defines[dnx_data_crps_latency_define_stat_id_multicast_offset].doc = "offset of the multicast in the latency stat (flow) id  encoding";
    
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
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

