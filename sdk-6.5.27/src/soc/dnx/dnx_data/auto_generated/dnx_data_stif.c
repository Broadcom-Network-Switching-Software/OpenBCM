
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_STAT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_stif.h>




extern shr_error_e jr2_a0_data_stif_attach(
    int unit);


extern shr_error_e jr2_b0_data_stif_attach(
    int unit);


extern shr_error_e j2c_a0_data_stif_attach(
    int unit);


extern shr_error_e q2a_a0_data_stif_attach(
    int unit);


extern shr_error_e q2a_b0_data_stif_attach(
    int unit);


extern shr_error_e j2p_a0_data_stif_attach(
    int unit);


extern shr_error_e j2x_a0_data_stif_attach(
    int unit);




static shr_error_e
dnx_data_stif_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_stif_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data stif general features");

    submodule_data->features[dnx_data_stif_general_is_used].name = "is_used";
    submodule_data->features[dnx_data_stif_general_is_used].doc = "";
    submodule_data->features[dnx_data_stif_general_is_used].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_stif_general_power_down_supported].name = "power_down_supported";
    submodule_data->features[dnx_data_stif_general_power_down_supported].doc = "";
    submodule_data->features[dnx_data_stif_general_power_down_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_stif_general_is_tap_supported].name = "is_tap_supported";
    submodule_data->features[dnx_data_stif_general_is_tap_supported].doc = "";
    submodule_data->features[dnx_data_stif_general_is_tap_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_stif_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data stif general defines");

    
    submodule_data->nof_tables = _dnx_data_stif_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data stif general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_stif_general_feature_get(
    int unit,
    dnx_data_stif_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_general, feature);
}











static shr_error_e
dnx_data_stif_config_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "config";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_stif_config_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data stif config features");

    submodule_data->features[dnx_data_stif_config_stif_enable].name = "stif_enable";
    submodule_data->features[dnx_data_stif_config_stif_enable].doc = "";
    submodule_data->features[dnx_data_stif_config_stif_enable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_stif_config_stif_report_mc_single_copy].name = "stif_report_mc_single_copy";
    submodule_data->features[dnx_data_stif_config_stif_report_mc_single_copy].doc = "";
    submodule_data->features[dnx_data_stif_config_stif_report_mc_single_copy].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_stif_config_stif_report_disp_is_drop_with_mc_single_copy].name = "stif_report_disp_is_drop_with_mc_single_copy";
    submodule_data->features[dnx_data_stif_config_stif_report_disp_is_drop_with_mc_single_copy].doc = "";
    submodule_data->features[dnx_data_stif_config_stif_report_disp_is_drop_with_mc_single_copy].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_stif_config_mixing_ports_limitation].name = "mixing_ports_limitation";
    submodule_data->features[dnx_data_stif_config_mixing_ports_limitation].doc = "";
    submodule_data->features[dnx_data_stif_config_mixing_ports_limitation].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_stif_config_map_source_to_two_stif_ports].name = "map_source_to_two_stif_ports";
    submodule_data->features[dnx_data_stif_config_map_source_to_two_stif_ports].doc = "";
    submodule_data->features[dnx_data_stif_config_map_source_to_two_stif_ports].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_stif_config_only_billing_mode_support].name = "only_billing_mode_support";
    submodule_data->features[dnx_data_stif_config_only_billing_mode_support].doc = "";
    submodule_data->features[dnx_data_stif_config_only_billing_mode_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_stif_config_is_stif_record_updated].name = "is_stif_record_updated";
    submodule_data->features[dnx_data_stif_config_is_stif_record_updated].doc = "";
    submodule_data->features[dnx_data_stif_config_is_stif_record_updated].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_stif_config_is_stif_hard_wire_to_nif].name = "is_stif_hard_wire_to_nif";
    submodule_data->features[dnx_data_stif_config_is_stif_hard_wire_to_nif].doc = "";
    submodule_data->features[dnx_data_stif_config_is_stif_hard_wire_to_nif].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_stif_config_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data stif config defines");

    submodule_data->defines[dnx_data_stif_config_define_sram_buffers_resolution].name = "sram_buffers_resolution";
    submodule_data->defines[dnx_data_stif_config_define_sram_buffers_resolution].doc = "";
    
    submodule_data->defines[dnx_data_stif_config_define_sram_buffers_resolution].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_config_define_sram_pdbs_resolution].name = "sram_pdbs_resolution";
    submodule_data->defines[dnx_data_stif_config_define_sram_pdbs_resolution].doc = "";
    
    submodule_data->defines[dnx_data_stif_config_define_sram_pdbs_resolution].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_config_define_dram_bdb_resolution].name = "dram_bdb_resolution";
    submodule_data->defines[dnx_data_stif_config_define_dram_bdb_resolution].doc = "";
    
    submodule_data->defines[dnx_data_stif_config_define_dram_bdb_resolution].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_config_define_stif_packet_size].name = "stif_packet_size";
    submodule_data->defines[dnx_data_stif_config_define_stif_packet_size].doc = "";
    
    submodule_data->defines[dnx_data_stif_config_define_stif_packet_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_queue_min].name = "stif_scrubber_queue_min";
    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_queue_min].doc = "";
    
    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_queue_min].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_queue_max].name = "stif_scrubber_queue_max";
    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_queue_max].doc = "";
    
    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_queue_max].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_rate_min].name = "stif_scrubber_rate_min";
    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_rate_min].doc = "";
    
    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_rate_min].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_rate_max].name = "stif_scrubber_rate_max";
    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_rate_max].doc = "";
    
    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_rate_max].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_stif_config_define_stif_report_mode].name = "stif_report_mode";
    submodule_data->defines[dnx_data_stif_config_define_stif_report_mode].doc = "";
    
    submodule_data->defines[dnx_data_stif_config_define_stif_report_mode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_stif_config_define_stif_report_size_ingress].name = "stif_report_size_ingress";
    submodule_data->defines[dnx_data_stif_config_define_stif_report_size_ingress].doc = "";
    
    submodule_data->defines[dnx_data_stif_config_define_stif_report_size_ingress].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_stif_config_define_stif_report_size_egress].name = "stif_report_size_egress";
    submodule_data->defines[dnx_data_stif_config_define_stif_report_size_egress].doc = "";
    
    submodule_data->defines[dnx_data_stif_config_define_stif_report_size_egress].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_stif_config_define_stif_idle_report].name = "stif_idle_report";
    submodule_data->defines[dnx_data_stif_config_define_stif_idle_report].doc = "";
    
    submodule_data->defines[dnx_data_stif_config_define_stif_idle_report].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_stif_config_define_nof_valid_sources].name = "nof_valid_sources";
    submodule_data->defines[dnx_data_stif_config_define_nof_valid_sources].doc = "";
    
    submodule_data->defines[dnx_data_stif_config_define_nof_valid_sources].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_stif_config_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data stif config tables");

    
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].name = "scrubber_buffers";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].doc = "";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].size_of_values = sizeof(dnx_data_stif_config_scrubber_buffers_t);
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].entry_get = dnx_data_stif_config_scrubber_buffers_entry_str_get;

    
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].nof_keys = 1;
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].keys[0].name = "thresh_id";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values, dnxc_data_value_t, submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].nof_values, "_dnx_data_stif_config_table_scrubber_buffers table values");
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[0].name = "sram_buffers";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[0].type = "uint32";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[0].doc = "";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[0].offset = UTILEX_OFFSETOF(dnx_data_stif_config_scrubber_buffers_t, sram_buffers);
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[1].name = "sram_pdbs";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[1].type = "uint32";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[1].doc = "";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[1].offset = UTILEX_OFFSETOF(dnx_data_stif_config_scrubber_buffers_t, sram_pdbs);
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[2].name = "dram_bdb";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[2].type = "uint32";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[2].doc = "";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[2].offset = UTILEX_OFFSETOF(dnx_data_stif_config_scrubber_buffers_t, dram_bdb);

    
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].name = "selective_report_range";
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].doc = "";
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].size_of_values = sizeof(dnx_data_stif_config_selective_report_range_t);
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].entry_get = dnx_data_stif_config_selective_report_range_entry_str_get;

    
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].nof_keys = 1;
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].keys[0].name = "range_id";
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_stif_config_table_selective_report_range].values, dnxc_data_value_t, submodule_data->tables[dnx_data_stif_config_table_selective_report_range].nof_values, "_dnx_data_stif_config_table_selective_report_range table values");
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].values[0].name = "min";
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].values[0].type = "uint32";
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].values[0].doc = "";
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].values[0].offset = UTILEX_OFFSETOF(dnx_data_stif_config_selective_report_range_t, min);
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].values[1].name = "max";
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].values[1].type = "uint32";
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].values[1].doc = "";
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].values[1].offset = UTILEX_OFFSETOF(dnx_data_stif_config_selective_report_range_t, max);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_stif_config_feature_get(
    int unit,
    dnx_data_stif_config_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_config, feature);
}


uint32
dnx_data_stif_config_sram_buffers_resolution_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_config, dnx_data_stif_config_define_sram_buffers_resolution);
}

uint32
dnx_data_stif_config_sram_pdbs_resolution_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_config, dnx_data_stif_config_define_sram_pdbs_resolution);
}

uint32
dnx_data_stif_config_dram_bdb_resolution_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_config, dnx_data_stif_config_define_dram_bdb_resolution);
}

uint32
dnx_data_stif_config_stif_packet_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_config, dnx_data_stif_config_define_stif_packet_size);
}

uint32
dnx_data_stif_config_stif_scrubber_queue_min_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_config, dnx_data_stif_config_define_stif_scrubber_queue_min);
}

uint32
dnx_data_stif_config_stif_scrubber_queue_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_config, dnx_data_stif_config_define_stif_scrubber_queue_max);
}

uint32
dnx_data_stif_config_stif_scrubber_rate_min_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_config, dnx_data_stif_config_define_stif_scrubber_rate_min);
}

uint32
dnx_data_stif_config_stif_scrubber_rate_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_config, dnx_data_stif_config_define_stif_scrubber_rate_max);
}

uint32
dnx_data_stif_config_stif_report_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_config, dnx_data_stif_config_define_stif_report_mode);
}

uint32
dnx_data_stif_config_stif_report_size_ingress_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_config, dnx_data_stif_config_define_stif_report_size_ingress);
}

uint32
dnx_data_stif_config_stif_report_size_egress_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_config, dnx_data_stif_config_define_stif_report_size_egress);
}

uint32
dnx_data_stif_config_stif_idle_report_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_config, dnx_data_stif_config_define_stif_idle_report);
}

uint32
dnx_data_stif_config_nof_valid_sources_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_config, dnx_data_stif_config_define_nof_valid_sources);
}



const dnx_data_stif_config_scrubber_buffers_t *
dnx_data_stif_config_scrubber_buffers_get(
    int unit,
    int thresh_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_config, dnx_data_stif_config_table_scrubber_buffers);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, thresh_id, 0);
    return (const dnx_data_stif_config_scrubber_buffers_t *) data;

}

const dnx_data_stif_config_selective_report_range_t *
dnx_data_stif_config_selective_report_range_get(
    int unit,
    int range_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_config, dnx_data_stif_config_table_selective_report_range);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, range_id, 0);
    return (const dnx_data_stif_config_selective_report_range_t *) data;

}


shr_error_e
dnx_data_stif_config_scrubber_buffers_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_stif_config_scrubber_buffers_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_config, dnx_data_stif_config_table_scrubber_buffers);
    data = (const dnx_data_stif_config_scrubber_buffers_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->sram_buffers);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->sram_pdbs);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dram_bdb);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_stif_config_selective_report_range_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_stif_config_selective_report_range_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_config, dnx_data_stif_config_table_selective_report_range);
    data = (const dnx_data_stif_config_selective_report_range_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->min);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_stif_config_scrubber_buffers_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_config, dnx_data_stif_config_table_scrubber_buffers);

}

const dnxc_data_table_info_t *
dnx_data_stif_config_selective_report_range_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_config, dnx_data_stif_config_table_selective_report_range);

}






static shr_error_e
dnx_data_stif_port_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "port";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_stif_port_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data stif port features");

    
    submodule_data->nof_defines = _dnx_data_stif_port_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data stif port defines");

    submodule_data->defines[dnx_data_stif_port_define_max_nof_instances].name = "max_nof_instances";
    submodule_data->defines[dnx_data_stif_port_define_max_nof_instances].doc = "";
    
    submodule_data->defines[dnx_data_stif_port_define_max_nof_instances].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_port_define_stif_ethu_select_size].name = "stif_ethu_select_size";
    submodule_data->defines[dnx_data_stif_port_define_stif_ethu_select_size].doc = "";
    
    submodule_data->defines[dnx_data_stif_port_define_stif_ethu_select_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_port_define_stif_lane_mapping_size].name = "stif_lane_mapping_size";
    submodule_data->defines[dnx_data_stif_port_define_stif_lane_mapping_size].doc = "";
    
    submodule_data->defines[dnx_data_stif_port_define_stif_lane_mapping_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_port_define_ethu_id].name = "ethu_id";
    submodule_data->defines[dnx_data_stif_port_define_ethu_id].doc = "";
    
    submodule_data->defines[dnx_data_stif_port_define_ethu_id].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_stif_port_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data stif port tables");

    
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].name = "ethu_id_stif_valid";
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].doc = "";
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].size_of_values = sizeof(dnx_data_stif_port_ethu_id_stif_valid_t);
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].entry_get = dnx_data_stif_port_ethu_id_stif_valid_entry_str_get;

    
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].nof_keys = 1;
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].keys[0].name = "ethu_id";
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].values, dnxc_data_value_t, submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].nof_values, "_dnx_data_stif_port_table_ethu_id_stif_valid table values");
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].values[0].name = "valid";
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].values[0].type = "uint32";
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].values[0].doc = "";
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].values[0].offset = UTILEX_OFFSETOF(dnx_data_stif_port_ethu_id_stif_valid_t, valid);
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].values[1].name = "hw_select_value";
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].values[1].type = "uint32";
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].values[1].doc = "";
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].values[1].offset = UTILEX_OFFSETOF(dnx_data_stif_port_ethu_id_stif_valid_t, hw_select_value);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_stif_port_feature_get(
    int unit,
    dnx_data_stif_port_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_port, feature);
}


uint32
dnx_data_stif_port_max_nof_instances_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_port, dnx_data_stif_port_define_max_nof_instances);
}

uint32
dnx_data_stif_port_stif_ethu_select_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_port, dnx_data_stif_port_define_stif_ethu_select_size);
}

uint32
dnx_data_stif_port_stif_lane_mapping_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_port, dnx_data_stif_port_define_stif_lane_mapping_size);
}

uint32
dnx_data_stif_port_ethu_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_port, dnx_data_stif_port_define_ethu_id);
}



const dnx_data_stif_port_ethu_id_stif_valid_t *
dnx_data_stif_port_ethu_id_stif_valid_get(
    int unit,
    int ethu_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_port, dnx_data_stif_port_table_ethu_id_stif_valid);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ethu_id, 0);
    return (const dnx_data_stif_port_ethu_id_stif_valid_t *) data;

}


shr_error_e
dnx_data_stif_port_ethu_id_stif_valid_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_stif_port_ethu_id_stif_valid_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_port, dnx_data_stif_port_table_ethu_id_stif_valid);
    data = (const dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->hw_select_value);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_stif_port_ethu_id_stif_valid_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_port, dnx_data_stif_port_table_ethu_id_stif_valid);

}






static shr_error_e
dnx_data_stif_report_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "report";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_stif_report_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data stif report features");

    submodule_data->features[dnx_data_stif_report_etpp_core_in_portmetadata].name = "etpp_core_in_portmetadata";
    submodule_data->features[dnx_data_stif_report_etpp_core_in_portmetadata].doc = "";
    submodule_data->features[dnx_data_stif_report_etpp_core_in_portmetadata].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_stif_report_stif_record_is_last_copy].name = "stif_record_is_last_copy";
    submodule_data->features[dnx_data_stif_report_stif_record_is_last_copy].doc = "";
    submodule_data->features[dnx_data_stif_report_stif_record_is_last_copy].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_stif_report_stif_record_nif_port_rate_bin].name = "stif_record_nif_port_rate_bin";
    submodule_data->features[dnx_data_stif_report_stif_record_nif_port_rate_bin].doc = "";
    submodule_data->features[dnx_data_stif_report_stif_record_nif_port_rate_bin].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_stif_report_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data stif report defines");

    submodule_data->defines[dnx_data_stif_report_define_qsize_idle_report_size].name = "qsize_idle_report_size";
    submodule_data->defines[dnx_data_stif_report_define_qsize_idle_report_size].doc = "";
    
    submodule_data->defines[dnx_data_stif_report_define_qsize_idle_report_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_qsize_idle_report_period].name = "qsize_idle_report_period";
    submodule_data->defines[dnx_data_stif_report_define_qsize_idle_report_period].doc = "";
    
    submodule_data->defines[dnx_data_stif_report_define_qsize_idle_report_period].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_qsize_idle_report_period_invalid].name = "qsize_idle_report_period_invalid";
    submodule_data->defines[dnx_data_stif_report_define_qsize_idle_report_period_invalid].doc = "";
    
    submodule_data->defines[dnx_data_stif_report_define_qsize_idle_report_period_invalid].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_qsize_scrubber_report_cycles_unit].name = "qsize_scrubber_report_cycles_unit";
    submodule_data->defines[dnx_data_stif_report_define_qsize_scrubber_report_cycles_unit].doc = "";
    
    submodule_data->defines[dnx_data_stif_report_define_qsize_scrubber_report_cycles_unit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_max_thresholds_per_buffer].name = "max_thresholds_per_buffer";
    submodule_data->defines[dnx_data_stif_report_define_max_thresholds_per_buffer].doc = "";
    
    submodule_data->defines[dnx_data_stif_report_define_max_thresholds_per_buffer].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_incoming_tc].name = "incoming_tc";
    submodule_data->defines[dnx_data_stif_report_define_incoming_tc].doc = "";
    
    submodule_data->defines[dnx_data_stif_report_define_incoming_tc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_record_two_msb].name = "record_two_msb";
    submodule_data->defines[dnx_data_stif_report_define_record_two_msb].doc = "";
    
    submodule_data->defines[dnx_data_stif_report_define_record_two_msb].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_nof_source_types].name = "nof_source_types";
    submodule_data->defines[dnx_data_stif_report_define_nof_source_types].doc = "";
    
    submodule_data->defines[dnx_data_stif_report_define_nof_source_types].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_stat_id_max_count].name = "stat_id_max_count";
    submodule_data->defines[dnx_data_stif_report_define_stat_id_max_count].doc = "";
    
    submodule_data->defines[dnx_data_stif_report_define_stat_id_max_count].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_stat_object_size].name = "stat_object_size";
    submodule_data->defines[dnx_data_stif_report_define_stat_object_size].doc = "";
    
    submodule_data->defines[dnx_data_stif_report_define_stat_object_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_billing_format_element_packet_size].name = "billing_format_element_packet_size";
    submodule_data->defines[dnx_data_stif_report_define_billing_format_element_packet_size].doc = "";
    
    submodule_data->defines[dnx_data_stif_report_define_billing_format_element_packet_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_billing_format_msb_opcode_size].name = "billing_format_msb_opcode_size";
    submodule_data->defines[dnx_data_stif_report_define_billing_format_msb_opcode_size].doc = "";
    
    submodule_data->defines[dnx_data_stif_report_define_billing_format_msb_opcode_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_billing_format_etpp_metadata_size].name = "billing_format_etpp_metadata_size";
    submodule_data->defines[dnx_data_stif_report_define_billing_format_etpp_metadata_size].doc = "";
    
    submodule_data->defines[dnx_data_stif_report_define_billing_format_etpp_metadata_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_one_type_mask].name = "billing_opsize_use_one_type_mask";
    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_one_type_mask].doc = "";
    
    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_one_type_mask].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_two_types_mask].name = "billing_opsize_use_two_types_mask";
    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_two_types_mask].doc = "";
    
    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_two_types_mask].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_three_types_mask].name = "billing_opsize_use_three_types_mask";
    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_three_types_mask].doc = "";
    
    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_three_types_mask].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_four_types_mask].name = "billing_opsize_use_four_types_mask";
    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_four_types_mask].doc = "";
    
    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_four_types_mask].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_billing_enq_ing_size_shift].name = "billing_enq_ing_size_shift";
    submodule_data->defines[dnx_data_stif_report_define_billing_enq_ing_size_shift].doc = "";
    
    submodule_data->defines[dnx_data_stif_report_define_billing_enq_ing_size_shift].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_stif_report_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data stif report tables");

    
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].name = "etpp_metadata";
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].doc = "";
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].size_of_values = sizeof(dnx_data_stif_report_etpp_metadata_t);
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].entry_get = dnx_data_stif_report_etpp_metadata_entry_str_get;

    
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].nof_keys = 1;
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].keys[0].name = "element";
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].values, dnxc_data_value_t, submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].nof_values, "_dnx_data_stif_report_table_etpp_metadata table values");
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].values[0].name = "size";
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].values[0].type = "uint32";
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].values[0].doc = "";
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].values[0].offset = UTILEX_OFFSETOF(dnx_data_stif_report_etpp_metadata_t, size);

    
    submodule_data->tables[dnx_data_stif_report_table_report_size].name = "report_size";
    submodule_data->tables[dnx_data_stif_report_table_report_size].doc = "";
    submodule_data->tables[dnx_data_stif_report_table_report_size].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_stif_report_table_report_size].size_of_values = sizeof(dnx_data_stif_report_report_size_t);
    submodule_data->tables[dnx_data_stif_report_table_report_size].entry_get = dnx_data_stif_report_report_size_entry_str_get;

    
    submodule_data->tables[dnx_data_stif_report_table_report_size].nof_keys = 1;
    submodule_data->tables[dnx_data_stif_report_table_report_size].keys[0].name = "dbal_enum";
    submodule_data->tables[dnx_data_stif_report_table_report_size].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_stif_report_table_report_size].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_stif_report_table_report_size].values, dnxc_data_value_t, submodule_data->tables[dnx_data_stif_report_table_report_size].nof_values, "_dnx_data_stif_report_table_report_size table values");
    submodule_data->tables[dnx_data_stif_report_table_report_size].values[0].name = "size";
    submodule_data->tables[dnx_data_stif_report_table_report_size].values[0].type = "uint32";
    submodule_data->tables[dnx_data_stif_report_table_report_size].values[0].doc = "";
    submodule_data->tables[dnx_data_stif_report_table_report_size].values[0].offset = UTILEX_OFFSETOF(dnx_data_stif_report_report_size_t, size);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_stif_report_feature_get(
    int unit,
    dnx_data_stif_report_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, feature);
}


uint32
dnx_data_stif_report_qsize_idle_report_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_define_qsize_idle_report_size);
}

uint32
dnx_data_stif_report_qsize_idle_report_period_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_define_qsize_idle_report_period);
}

uint32
dnx_data_stif_report_qsize_idle_report_period_invalid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_define_qsize_idle_report_period_invalid);
}

uint32
dnx_data_stif_report_qsize_scrubber_report_cycles_unit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_define_qsize_scrubber_report_cycles_unit);
}

uint32
dnx_data_stif_report_max_thresholds_per_buffer_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_define_max_thresholds_per_buffer);
}

uint32
dnx_data_stif_report_incoming_tc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_define_incoming_tc);
}

uint32
dnx_data_stif_report_record_two_msb_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_define_record_two_msb);
}

uint32
dnx_data_stif_report_nof_source_types_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_define_nof_source_types);
}

uint32
dnx_data_stif_report_stat_id_max_count_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_define_stat_id_max_count);
}

uint32
dnx_data_stif_report_stat_object_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_define_stat_object_size);
}

uint32
dnx_data_stif_report_billing_format_element_packet_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_define_billing_format_element_packet_size);
}

uint32
dnx_data_stif_report_billing_format_msb_opcode_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_define_billing_format_msb_opcode_size);
}

uint32
dnx_data_stif_report_billing_format_etpp_metadata_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_define_billing_format_etpp_metadata_size);
}

uint32
dnx_data_stif_report_billing_opsize_use_one_type_mask_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_define_billing_opsize_use_one_type_mask);
}

uint32
dnx_data_stif_report_billing_opsize_use_two_types_mask_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_define_billing_opsize_use_two_types_mask);
}

uint32
dnx_data_stif_report_billing_opsize_use_three_types_mask_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_define_billing_opsize_use_three_types_mask);
}

uint32
dnx_data_stif_report_billing_opsize_use_four_types_mask_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_define_billing_opsize_use_four_types_mask);
}

uint32
dnx_data_stif_report_billing_enq_ing_size_shift_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_define_billing_enq_ing_size_shift);
}



const dnx_data_stif_report_etpp_metadata_t *
dnx_data_stif_report_etpp_metadata_get(
    int unit,
    int element)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_table_etpp_metadata);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, element, 0);
    return (const dnx_data_stif_report_etpp_metadata_t *) data;

}

const dnx_data_stif_report_report_size_t *
dnx_data_stif_report_report_size_get(
    int unit,
    int dbal_enum)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_table_report_size);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_enum, 0);
    return (const dnx_data_stif_report_report_size_t *) data;

}


shr_error_e
dnx_data_stif_report_etpp_metadata_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_stif_report_etpp_metadata_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_table_etpp_metadata);
    data = (const dnx_data_stif_report_etpp_metadata_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_stif_report_report_size_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_stif_report_report_size_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_table_report_size);
    data = (const dnx_data_stif_report_report_size_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->size);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_stif_report_etpp_metadata_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_table_etpp_metadata);

}

const dnxc_data_table_info_t *
dnx_data_stif_report_report_size_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_report, dnx_data_stif_report_table_report_size);

}






static shr_error_e
dnx_data_stif_flow_control_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "flow_control";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_stif_flow_control_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data stif flow_control features");

    submodule_data->features[dnx_data_stif_flow_control_instance_0_must_be_on].name = "instance_0_must_be_on";
    submodule_data->features[dnx_data_stif_flow_control_instance_0_must_be_on].doc = "";
    submodule_data->features[dnx_data_stif_flow_control_instance_0_must_be_on].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_stif_flow_control_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data stif flow_control defines");

    submodule_data->defines[dnx_data_stif_flow_control_define_etpp_pipe_length].name = "etpp_pipe_length";
    submodule_data->defines[dnx_data_stif_flow_control_define_etpp_pipe_length].doc = "";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_etpp_pipe_length].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_etpp_deq_size_of_fifo].name = "etpp_deq_size_of_fifo";
    submodule_data->defines[dnx_data_stif_flow_control_define_etpp_deq_size_of_fifo].doc = "";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_etpp_deq_size_of_fifo].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_cgm_pipe_length].name = "cgm_pipe_length";
    submodule_data->defines[dnx_data_stif_flow_control_define_cgm_pipe_length].doc = "";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_cgm_pipe_length].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_cgm_size_of_fifo].name = "cgm_size_of_fifo";
    submodule_data->defines[dnx_data_stif_flow_control_define_cgm_size_of_fifo].doc = "";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_cgm_size_of_fifo].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_billing_level_sampling].name = "billing_level_sampling";
    submodule_data->defines[dnx_data_stif_flow_control_define_billing_level_sampling].doc = "";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_billing_level_sampling].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_fc_disable_value].name = "fc_disable_value";
    submodule_data->defines[dnx_data_stif_flow_control_define_fc_disable_value].doc = "";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_fc_disable_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_billing_etpp_flow_control_high].name = "billing_etpp_flow_control_high";
    submodule_data->defines[dnx_data_stif_flow_control_define_billing_etpp_flow_control_high].doc = "";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_billing_etpp_flow_control_high].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_billing_etpp_flow_control_low].name = "billing_etpp_flow_control_low";
    submodule_data->defines[dnx_data_stif_flow_control_define_billing_etpp_flow_control_low].doc = "";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_billing_etpp_flow_control_low].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_billing_cgm_flow_control_high].name = "billing_cgm_flow_control_high";
    submodule_data->defines[dnx_data_stif_flow_control_define_billing_cgm_flow_control_high].doc = "";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_billing_cgm_flow_control_high].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_billing_cgm_flow_control_low].name = "billing_cgm_flow_control_low";
    submodule_data->defines[dnx_data_stif_flow_control_define_billing_cgm_flow_control_low].doc = "";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_billing_cgm_flow_control_low].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_qsize_cgm_flow_control_high].name = "qsize_cgm_flow_control_high";
    submodule_data->defines[dnx_data_stif_flow_control_define_qsize_cgm_flow_control_high].doc = "";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_qsize_cgm_flow_control_high].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_qsize_cgm_flow_control_low].name = "qsize_cgm_flow_control_low";
    submodule_data->defines[dnx_data_stif_flow_control_define_qsize_cgm_flow_control_low].doc = "";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_qsize_cgm_flow_control_low].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_stif_flow_control_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data stif flow_control tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_stif_flow_control_feature_get(
    int unit,
    dnx_data_stif_flow_control_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_flow_control, feature);
}


uint32
dnx_data_stif_flow_control_etpp_pipe_length_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_flow_control, dnx_data_stif_flow_control_define_etpp_pipe_length);
}

uint32
dnx_data_stif_flow_control_etpp_deq_size_of_fifo_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_flow_control, dnx_data_stif_flow_control_define_etpp_deq_size_of_fifo);
}

uint32
dnx_data_stif_flow_control_cgm_pipe_length_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_flow_control, dnx_data_stif_flow_control_define_cgm_pipe_length);
}

uint32
dnx_data_stif_flow_control_cgm_size_of_fifo_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_flow_control, dnx_data_stif_flow_control_define_cgm_size_of_fifo);
}

uint32
dnx_data_stif_flow_control_billing_level_sampling_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_flow_control, dnx_data_stif_flow_control_define_billing_level_sampling);
}

uint32
dnx_data_stif_flow_control_fc_disable_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_flow_control, dnx_data_stif_flow_control_define_fc_disable_value);
}

uint32
dnx_data_stif_flow_control_billing_etpp_flow_control_high_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_flow_control, dnx_data_stif_flow_control_define_billing_etpp_flow_control_high);
}

uint32
dnx_data_stif_flow_control_billing_etpp_flow_control_low_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_flow_control, dnx_data_stif_flow_control_define_billing_etpp_flow_control_low);
}

uint32
dnx_data_stif_flow_control_billing_cgm_flow_control_high_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_flow_control, dnx_data_stif_flow_control_define_billing_cgm_flow_control_high);
}

uint32
dnx_data_stif_flow_control_billing_cgm_flow_control_low_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_flow_control, dnx_data_stif_flow_control_define_billing_cgm_flow_control_low);
}

uint32
dnx_data_stif_flow_control_qsize_cgm_flow_control_high_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_flow_control, dnx_data_stif_flow_control_define_qsize_cgm_flow_control_high);
}

uint32
dnx_data_stif_flow_control_qsize_cgm_flow_control_low_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_flow_control, dnx_data_stif_flow_control_define_qsize_cgm_flow_control_low);
}










static shr_error_e
dnx_data_stif_regression_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "regression";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_stif_regression_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data stif regression features");

    
    submodule_data->nof_defines = _dnx_data_stif_regression_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data stif regression defines");

    submodule_data->defines[dnx_data_stif_regression_define_last_stif_data_length].name = "last_stif_data_length";
    submodule_data->defines[dnx_data_stif_regression_define_last_stif_data_length].doc = "";
    
    submodule_data->defines[dnx_data_stif_regression_define_last_stif_data_length].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_stif_regression_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data stif regression tables");

    
    submodule_data->tables[dnx_data_stif_regression_table_supported_modes].name = "supported_modes";
    submodule_data->tables[dnx_data_stif_regression_table_supported_modes].doc = "";
    submodule_data->tables[dnx_data_stif_regression_table_supported_modes].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_stif_regression_table_supported_modes].size_of_values = sizeof(dnx_data_stif_regression_supported_modes_t);
    submodule_data->tables[dnx_data_stif_regression_table_supported_modes].entry_get = dnx_data_stif_regression_supported_modes_entry_str_get;

    
    submodule_data->tables[dnx_data_stif_regression_table_supported_modes].nof_keys = 2;
    submodule_data->tables[dnx_data_stif_regression_table_supported_modes].keys[0].name = "mode";
    submodule_data->tables[dnx_data_stif_regression_table_supported_modes].keys[0].doc = "";
    submodule_data->tables[dnx_data_stif_regression_table_supported_modes].keys[1].name = "speed";
    submodule_data->tables[dnx_data_stif_regression_table_supported_modes].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_stif_regression_table_supported_modes].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_stif_regression_table_supported_modes].values, dnxc_data_value_t, submodule_data->tables[dnx_data_stif_regression_table_supported_modes].nof_values, "_dnx_data_stif_regression_table_supported_modes table values");
    submodule_data->tables[dnx_data_stif_regression_table_supported_modes].values[0].name = "valid";
    submodule_data->tables[dnx_data_stif_regression_table_supported_modes].values[0].type = "uint32";
    submodule_data->tables[dnx_data_stif_regression_table_supported_modes].values[0].doc = "";
    submodule_data->tables[dnx_data_stif_regression_table_supported_modes].values[0].offset = UTILEX_OFFSETOF(dnx_data_stif_regression_supported_modes_t, valid);
    submodule_data->tables[dnx_data_stif_regression_table_supported_modes].values[1].name = "ports_type";
    submodule_data->tables[dnx_data_stif_regression_table_supported_modes].values[1].type = "char *";
    submodule_data->tables[dnx_data_stif_regression_table_supported_modes].values[1].doc = "";
    submodule_data->tables[dnx_data_stif_regression_table_supported_modes].values[1].offset = UTILEX_OFFSETOF(dnx_data_stif_regression_supported_modes_t, ports_type);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_stif_regression_feature_get(
    int unit,
    dnx_data_stif_regression_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_regression, feature);
}


uint32
dnx_data_stif_regression_last_stif_data_length_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_regression, dnx_data_stif_regression_define_last_stif_data_length);
}



const dnx_data_stif_regression_supported_modes_t *
dnx_data_stif_regression_supported_modes_get(
    int unit,
    int mode,
    int speed)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_regression, dnx_data_stif_regression_table_supported_modes);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mode, speed);
    return (const dnx_data_stif_regression_supported_modes_t *) data;

}


shr_error_e
dnx_data_stif_regression_supported_modes_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_stif_regression_supported_modes_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_regression, dnx_data_stif_regression_table_supported_modes);
    data = (const dnx_data_stif_regression_supported_modes_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->ports_type == NULL ? "" : data->ports_type);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_stif_regression_supported_modes_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_stif, dnx_data_stif_submodule_regression, dnx_data_stif_regression_table_supported_modes);

}



shr_error_e
dnx_data_stif_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "stif";
    module_data->nof_submodules = _dnx_data_stif_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data stif submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_stif_general_init(unit, &module_data->submodules[dnx_data_stif_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_stif_config_init(unit, &module_data->submodules[dnx_data_stif_submodule_config]));
    SHR_IF_ERR_EXIT(dnx_data_stif_port_init(unit, &module_data->submodules[dnx_data_stif_submodule_port]));
    SHR_IF_ERR_EXIT(dnx_data_stif_report_init(unit, &module_data->submodules[dnx_data_stif_submodule_report]));
    SHR_IF_ERR_EXIT(dnx_data_stif_flow_control_init(unit, &module_data->submodules[dnx_data_stif_submodule_flow_control]));
    SHR_IF_ERR_EXIT(dnx_data_stif_regression_init(unit, &module_data->submodules[dnx_data_stif_submodule_regression]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stif_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_stif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stif_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_stif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stif_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_stif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stif_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_stif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stif_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_stif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stif_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_stif_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_stif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stif_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_stif_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_stif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stif_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_stif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stif_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_stif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stif_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_stif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stif_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_stif_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

