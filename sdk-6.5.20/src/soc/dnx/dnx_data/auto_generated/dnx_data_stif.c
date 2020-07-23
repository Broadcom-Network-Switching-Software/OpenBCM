

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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



static shr_error_e
dnx_data_stif_config_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "config";
    submodule_data->doc = "configure STIF via soc properties";
    
    submodule_data->nof_features = _dnx_data_stif_config_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data stif config features");

    submodule_data->features[dnx_data_stif_config_stif_enable].name = "stif_enable";
    submodule_data->features[dnx_data_stif_config_stif_enable].doc = "Enable/disable the statistic interface.";
    submodule_data->features[dnx_data_stif_config_stif_enable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_stif_config_stif_report_mc_single_copy].name = "stif_report_mc_single_copy";
    submodule_data->features[dnx_data_stif_config_stif_report_mc_single_copy].doc = "Billing - count all ingress MC copies as one.";
    submodule_data->features[dnx_data_stif_config_stif_report_mc_single_copy].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_stif_config_mixing_ports_limitation].name = "mixing_ports_limitation";
    submodule_data->features[dnx_data_stif_config_mixing_ports_limitation].doc = "activate limition of Mixing STIF and none STIF ports in one port macro";
    submodule_data->features[dnx_data_stif_config_mixing_ports_limitation].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_stif_config_map_source_to_two_stif_ports].name = "map_source_to_two_stif_ports";
    submodule_data->features[dnx_data_stif_config_map_source_to_two_stif_ports].doc = "It indicates if it is allowed the source mapping to be split into two STIF ports or source mapping must be 1:1";
    submodule_data->features[dnx_data_stif_config_map_source_to_two_stif_ports].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_stif_config_only_billing_mode_support].name = "only_billing_mode_support";
    submodule_data->features[dnx_data_stif_config_only_billing_mode_support].doc = "It indicates if only billing mode is supported or there is support for ";
    submodule_data->features[dnx_data_stif_config_only_billing_mode_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_stif_config_is_stif_record_updated].name = "is_stif_record_updated";
    submodule_data->features[dnx_data_stif_config_is_stif_record_updated].doc = "It indicates if stif record is updated";
    submodule_data->features[dnx_data_stif_config_is_stif_record_updated].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_stif_config_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data stif config defines");

    submodule_data->defines[dnx_data_stif_config_define_sram_buffers_resolution].name = "sram_buffers_resolution";
    submodule_data->defines[dnx_data_stif_config_define_sram_buffers_resolution].doc = "scrubber sram buffers threshold resolution";
    
    submodule_data->defines[dnx_data_stif_config_define_sram_buffers_resolution].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_config_define_sram_pdbs_resolution].name = "sram_pdbs_resolution";
    submodule_data->defines[dnx_data_stif_config_define_sram_pdbs_resolution].doc = "scrubber sram pdbs threshold resolution";
    
    submodule_data->defines[dnx_data_stif_config_define_sram_pdbs_resolution].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_config_define_dram_bdb_resolution].name = "dram_bdb_resolution";
    submodule_data->defines[dnx_data_stif_config_define_dram_bdb_resolution].doc = "scrubber dram bdb threshold resolution";
    
    submodule_data->defines[dnx_data_stif_config_define_dram_bdb_resolution].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_config_define_stif_packet_size].name = "stif_packet_size";
    submodule_data->defines[dnx_data_stif_config_define_stif_packet_size].doc = "Configure the packet size - for all report modes";
    
    submodule_data->defines[dnx_data_stif_config_define_stif_packet_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_queue_min].name = "stif_scrubber_queue_min";
    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_queue_min].doc = "Scrubber queue minimum - for QSIZE";
    
    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_queue_min].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_queue_max].name = "stif_scrubber_queue_max";
    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_queue_max].doc = "Scrubber queue maximum - for QSIZE";
    
    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_queue_max].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_rate_min].name = "stif_scrubber_rate_min";
    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_rate_min].doc = "The minimal period between two conscutive scrubber reports in nanoseconds - for QSIZE";
    
    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_rate_min].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_rate_max].name = "stif_scrubber_rate_max";
    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_rate_max].doc = "It indicates the maximal period between two consecutive scrubber reports in nanoseconds - for QSIZE";
    
    submodule_data->defines[dnx_data_stif_config_define_stif_scrubber_rate_max].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_stif_config_define_stif_report_mode].name = "stif_report_mode";
    submodule_data->defines[dnx_data_stif_config_define_stif_report_mode].doc = "stif report mode value - qsize/billing/ billing ingress";
    
    submodule_data->defines[dnx_data_stif_config_define_stif_report_mode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_stif_config_define_stif_report_size_ingress].name = "stif_report_size_ingress";
    submodule_data->defines[dnx_data_stif_config_define_stif_report_size_ingress].doc = "ingress stif report size in bytes";
    
    submodule_data->defines[dnx_data_stif_config_define_stif_report_size_ingress].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_stif_config_define_stif_report_size_egress].name = "stif_report_size_egress";
    submodule_data->defines[dnx_data_stif_config_define_stif_report_size_egress].doc = "egress stif report size in bytes";
    
    submodule_data->defines[dnx_data_stif_config_define_stif_report_size_egress].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_stif_config_define_stif_idle_report].name = "stif_idle_report";
    submodule_data->defines[dnx_data_stif_config_define_stif_idle_report].doc = "Configure if empty reports will be sent; if they will - the period will be 256";
    
    submodule_data->defines[dnx_data_stif_config_define_stif_idle_report].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_stif_config_define_nof_valid_sources].name = "nof_valid_sources";
    submodule_data->defines[dnx_data_stif_config_define_nof_valid_sources].doc = "qsize valid sources inq/deq/scrubber; billing valid sources - inq/deq/egr ";
    
    submodule_data->defines[dnx_data_stif_config_define_nof_valid_sources].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_stif_config_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data stif config tables");

    
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].name = "scrubber_buffers";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].doc = "scrubber buffer limits";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].size_of_values = sizeof(dnx_data_stif_config_scrubber_buffers_t);
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].entry_get = dnx_data_stif_config_scrubber_buffers_entry_str_get;

    
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].nof_keys = 1;
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].keys[0].name = "thresh_id";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].keys[0].doc = "threshold id";

    
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values, dnxc_data_value_t, submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].nof_values, "_dnx_data_stif_config_table_scrubber_buffers table values");
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[0].name = "sram_buffers";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[0].type = "uint32";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[0].doc = "scrubber sram buffers threshold";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[0].offset = UTILEX_OFFSETOF(dnx_data_stif_config_scrubber_buffers_t, sram_buffers);
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[1].name = "sram_pdbs";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[1].type = "uint32";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[1].doc = "scrubber sram pdbs threshold";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[1].offset = UTILEX_OFFSETOF(dnx_data_stif_config_scrubber_buffers_t, sram_pdbs);
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[2].name = "dram_bdb";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[2].type = "uint32";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[2].doc = "scrubber dram bdb threshold";
    submodule_data->tables[dnx_data_stif_config_table_scrubber_buffers].values[2].offset = UTILEX_OFFSETOF(dnx_data_stif_config_scrubber_buffers_t, dram_bdb);

    
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].name = "selective_report_range";
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].doc = "select the range of queues for qsize report - 2 ranges";
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].size_of_values = sizeof(dnx_data_stif_config_selective_report_range_t);
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].entry_get = dnx_data_stif_config_selective_report_range_entry_str_get;

    
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].nof_keys = 1;
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].keys[0].name = "range_id";
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].keys[0].doc = "range id - 0/1";

    
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_stif_config_table_selective_report_range].values, dnxc_data_value_t, submodule_data->tables[dnx_data_stif_config_table_selective_report_range].nof_values, "_dnx_data_stif_config_table_selective_report_range table values");
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].values[0].name = "min";
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].values[0].type = "uint32";
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].values[0].doc = "min value of the range";
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].values[0].offset = UTILEX_OFFSETOF(dnx_data_stif_config_selective_report_range_t, min);
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].values[1].name = "max";
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].values[1].type = "uint32";
    submodule_data->tables[dnx_data_stif_config_table_selective_report_range].values[1].doc = "max value of the range";
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
    submodule_data->doc = "stif - nif port instance values";
    
    submodule_data->nof_features = _dnx_data_stif_port_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data stif port features");

    
    submodule_data->nof_defines = _dnx_data_stif_port_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data stif port defines");

    submodule_data->defines[dnx_data_stif_port_define_max_nof_instances].name = "max_nof_instances";
    submodule_data->defines[dnx_data_stif_port_define_max_nof_instances].doc = "stif-nif port instances";
    
    submodule_data->defines[dnx_data_stif_port_define_max_nof_instances].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_port_define_stif_ethu_select_size].name = "stif_ethu_select_size";
    submodule_data->defines[dnx_data_stif_port_define_stif_ethu_select_size].doc = "used for dbal - size of stif ethu select in bits";
    
    submodule_data->defines[dnx_data_stif_port_define_stif_ethu_select_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_port_define_stif_lane_mapping_size].name = "stif_lane_mapping_size";
    submodule_data->defines[dnx_data_stif_port_define_stif_lane_mapping_size].doc = "used for dbal - size of stif to lane bitmap in bits";
    
    submodule_data->defines[dnx_data_stif_port_define_stif_lane_mapping_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_stif_port_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data stif port tables");

    
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].name = "ethu_id_stif_valid";
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].doc = "stif-nif valid ethu ids for stif and their according hw value to be set";
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].size_of_values = sizeof(dnx_data_stif_port_ethu_id_stif_valid_t);
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].entry_get = dnx_data_stif_port_ethu_id_stif_valid_entry_str_get;

    
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].nof_keys = 1;
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].keys[0].name = "ethu_id";
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].keys[0].doc = "ethu id";

    
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].values, dnxc_data_value_t, submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].nof_values, "_dnx_data_stif_port_table_ethu_id_stif_valid table values");
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].values[0].name = "valid";
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].values[0].type = "uint32";
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].values[0].doc = "valid ethu id for stif - true/false";
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].values[0].offset = UTILEX_OFFSETOF(dnx_data_stif_port_ethu_id_stif_valid_t, valid);
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].values[1].name = "hw_select_value";
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].values[1].type = "uint32";
    submodule_data->tables[dnx_data_stif_port_table_ethu_id_stif_valid].values[1].doc = "hw select value to realize stif-nif mapping";
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
    submodule_data->doc = "stif values related to the report";
    
    submodule_data->nof_features = _dnx_data_stif_report_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data stif report features");

    submodule_data->features[dnx_data_stif_report_etpp_core_in_portmetadata].name = "etpp_core_in_portmetadata";
    submodule_data->features[dnx_data_stif_report_etpp_core_in_portmetadata].doc = "";
    submodule_data->features[dnx_data_stif_report_etpp_core_in_portmetadata].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_stif_report_stif_record_is_last_copy].name = "stif_record_is_last_copy";
    submodule_data->features[dnx_data_stif_report_stif_record_is_last_copy].doc = "indicates if record contain MC is last copy field";
    submodule_data->features[dnx_data_stif_report_stif_record_is_last_copy].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_stif_report_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data stif report defines");

    submodule_data->defines[dnx_data_stif_report_define_qsize_idle_report_size].name = "qsize_idle_report_size";
    submodule_data->defines[dnx_data_stif_report_define_qsize_idle_report_size].doc = "default value for qsize idle report size in bytes";
    
    submodule_data->defines[dnx_data_stif_report_define_qsize_idle_report_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_qsize_idle_report_period].name = "qsize_idle_report_period";
    submodule_data->defines[dnx_data_stif_report_define_qsize_idle_report_period].doc = "default value for the QSIZE report period";
    
    submodule_data->defines[dnx_data_stif_report_define_qsize_idle_report_period].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_qsize_idle_report_period_invalid].name = "qsize_idle_report_period_invalid";
    submodule_data->defines[dnx_data_stif_report_define_qsize_idle_report_period_invalid].doc = "disable the idle report generation";
    
    submodule_data->defines[dnx_data_stif_report_define_qsize_idle_report_period_invalid].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_qsize_scrubber_report_cycles_unit].name = "qsize_scrubber_report_cycles_unit";
    submodule_data->defines[dnx_data_stif_report_define_qsize_scrubber_report_cycles_unit].doc = "Period between consecutive scrubber records is in units - unit = defined cycle periods";
    
    submodule_data->defines[dnx_data_stif_report_define_qsize_scrubber_report_cycles_unit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_max_thresholds_per_buffer].name = "max_thresholds_per_buffer";
    submodule_data->defines[dnx_data_stif_report_define_max_thresholds_per_buffer].doc = "scrubber buffer thresholds - the thresholds define 16 ranges of buffer occupancy";
    
    submodule_data->defines[dnx_data_stif_report_define_max_thresholds_per_buffer].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_incoming_tc].name = "incoming_tc";
    submodule_data->defines[dnx_data_stif_report_define_incoming_tc].doc = "STIF uses incoming TC (tc reported by PP) and cannot be changed by the user - hw value";
    
    submodule_data->defines[dnx_data_stif_report_define_incoming_tc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_record_two_msb].name = "record_two_msb";
    submodule_data->defines[dnx_data_stif_report_define_record_two_msb].doc = "two MSB of the record should be 2'b11";
    
    submodule_data->defines[dnx_data_stif_report_define_record_two_msb].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_nof_source_types].name = "nof_source_types";
    submodule_data->defines[dnx_data_stif_report_define_nof_source_types].doc = "number of source types - ingress enq/deq/scrubber, egress - regarding bcm_stat_stif_source_type_t struct";
    
    submodule_data->defines[dnx_data_stif_report_define_nof_source_types].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_stat_id_max_count].name = "stat_id_max_count";
    submodule_data->defines[dnx_data_stif_report_define_stat_id_max_count].doc = "max count of possible statistics ids";
    
    submodule_data->defines[dnx_data_stif_report_define_stat_id_max_count].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_stat_object_size].name = "stat_object_size";
    submodule_data->defines[dnx_data_stif_report_define_stat_object_size].doc = "the size of a statistic object (0,1,2,3) - size in bits";
    
    submodule_data->defines[dnx_data_stif_report_define_stat_object_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_billing_format_element_packet_size].name = "billing_format_element_packet_size";
    submodule_data->defines[dnx_data_stif_report_define_billing_format_element_packet_size].doc = "get size of the  billing element for packet size - size in bits";
    
    submodule_data->defines[dnx_data_stif_report_define_billing_format_element_packet_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_billing_format_msb_opcode_size].name = "billing_format_msb_opcode_size";
    submodule_data->defines[dnx_data_stif_report_define_billing_format_msb_opcode_size].doc = "get size of the  billing element opcode which the user cannot change - size in bits";
    
    submodule_data->defines[dnx_data_stif_report_define_billing_format_msb_opcode_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_billing_format_etpp_metadata_size].name = "billing_format_etpp_metadata_size";
    submodule_data->defines[dnx_data_stif_report_define_billing_format_etpp_metadata_size].doc = "billing record format ETPP metadata size - size in bits";
    
    submodule_data->defines[dnx_data_stif_report_define_billing_format_etpp_metadata_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_one_type_mask].name = "billing_opsize_use_one_type_mask";
    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_one_type_mask].doc = "billing record format opsize mask to use one type";
    
    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_one_type_mask].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_two_types_mask].name = "billing_opsize_use_two_types_mask";
    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_two_types_mask].doc = "billing record format mask to use two types";
    
    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_two_types_mask].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_three_types_mask].name = "billing_opsize_use_three_types_mask";
    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_three_types_mask].doc = "billing record format mask to use three types";
    
    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_three_types_mask].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_four_types_mask].name = "billing_opsize_use_four_types_mask";
    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_four_types_mask].doc = "billing record format mask to use four types";
    
    submodule_data->defines[dnx_data_stif_report_define_billing_opsize_use_four_types_mask].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_report_define_billing_enq_ing_size_shift].name = "billing_enq_ing_size_shift";
    submodule_data->defines[dnx_data_stif_report_define_billing_enq_ing_size_shift].doc = "billing record ingress enqueu size=96. need to reduce shift value from each element sgift value";
    
    submodule_data->defines[dnx_data_stif_report_define_billing_enq_ing_size_shift].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_stif_report_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data stif report tables");

    
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].name = "etpp_metadata";
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].doc = "etpp metadata elements and their size in bits";
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].size_of_values = sizeof(dnx_data_stif_report_etpp_metadata_t);
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].entry_get = dnx_data_stif_report_etpp_metadata_entry_str_get;

    
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].nof_keys = 1;
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].keys[0].name = "element";
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].keys[0].doc = "metadata element type";

    
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].values, dnxc_data_value_t, submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].nof_values, "_dnx_data_stif_report_table_etpp_metadata table values");
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].values[0].name = "size";
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].values[0].type = "uint32";
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].values[0].doc = "size of the element in bits";
    submodule_data->tables[dnx_data_stif_report_table_etpp_metadata].values[0].offset = UTILEX_OFFSETOF(dnx_data_stif_report_etpp_metadata_t, size);

    
    submodule_data->tables[dnx_data_stif_report_table_report_size].name = "report_size";
    submodule_data->tables[dnx_data_stif_report_table_report_size].doc = "connect the soc property mapped value of the report size to the real value in bits";
    submodule_data->tables[dnx_data_stif_report_table_report_size].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_stif_report_table_report_size].size_of_values = sizeof(dnx_data_stif_report_report_size_t);
    submodule_data->tables[dnx_data_stif_report_table_report_size].entry_get = dnx_data_stif_report_report_size_entry_str_get;

    
    submodule_data->tables[dnx_data_stif_report_table_report_size].nof_keys = 1;
    submodule_data->tables[dnx_data_stif_report_table_report_size].keys[0].name = "dbal_enum";
    submodule_data->tables[dnx_data_stif_report_table_report_size].keys[0].doc = "the value that will get from the soc property";

    
    submodule_data->tables[dnx_data_stif_report_table_report_size].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_stif_report_table_report_size].values, dnxc_data_value_t, submodule_data->tables[dnx_data_stif_report_table_report_size].nof_values, "_dnx_data_stif_report_table_report_size table values");
    submodule_data->tables[dnx_data_stif_report_table_report_size].values[0].name = "size";
    submodule_data->tables[dnx_data_stif_report_table_report_size].values[0].type = "uint32";
    submodule_data->tables[dnx_data_stif_report_table_report_size].values[0].doc = "size of the report corresponding to the soc property value";
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
    submodule_data->doc = "stif values related to the low and high flow control calculations";
    
    submodule_data->nof_features = _dnx_data_stif_flow_control_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data stif flow_control features");

    submodule_data->features[dnx_data_stif_flow_control_instance_0_must_be_on].name = "instance_0_must_be_on";
    submodule_data->features[dnx_data_stif_flow_control_instance_0_must_be_on].doc = "";
    submodule_data->features[dnx_data_stif_flow_control_instance_0_must_be_on].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_stif_flow_control_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data stif flow_control defines");

    submodule_data->defines[dnx_data_stif_flow_control_define_etpp_pipe_length].name = "etpp_pipe_length";
    submodule_data->defines[dnx_data_stif_flow_control_define_etpp_pipe_length].doc = "pipe length size in clocks";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_etpp_pipe_length].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_etpp_deq_size_of_fifo].name = "etpp_deq_size_of_fifo";
    submodule_data->defines[dnx_data_stif_flow_control_define_etpp_deq_size_of_fifo].doc = "etpp dequeue size of fifo - reports";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_etpp_deq_size_of_fifo].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_cgm_pipe_length].name = "cgm_pipe_length";
    submodule_data->defines[dnx_data_stif_flow_control_define_cgm_pipe_length].doc = "cgm pipe length in clocks";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_cgm_pipe_length].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_cgm_size_of_fifo].name = "cgm_size_of_fifo";
    submodule_data->defines[dnx_data_stif_flow_control_define_cgm_size_of_fifo].doc = "cgm size of fifo - reports";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_cgm_size_of_fifo].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_billing_level_sampling].name = "billing_level_sampling";
    submodule_data->defines[dnx_data_stif_flow_control_define_billing_level_sampling].doc = "level sampling value - used for billing flow control calculation";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_billing_level_sampling].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_fc_disable_value].name = "fc_disable_value";
    submodule_data->defines[dnx_data_stif_flow_control_define_fc_disable_value].doc = "disable flow control value";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_fc_disable_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_billing_etpp_flow_control_high].name = "billing_etpp_flow_control_high";
    submodule_data->defines[dnx_data_stif_flow_control_define_billing_etpp_flow_control_high].doc = "billing etpp flow control max value - calculated with formula given from the architecture";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_billing_etpp_flow_control_high].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_billing_etpp_flow_control_low].name = "billing_etpp_flow_control_low";
    submodule_data->defines[dnx_data_stif_flow_control_define_billing_etpp_flow_control_low].doc = "billing etpp flow control min value - calculated with formula given from the architecture";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_billing_etpp_flow_control_low].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_billing_cgm_flow_control_high].name = "billing_cgm_flow_control_high";
    submodule_data->defines[dnx_data_stif_flow_control_define_billing_cgm_flow_control_high].doc = "billing cgm flow control max value - calculated with formula given from the architecture";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_billing_cgm_flow_control_high].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_billing_cgm_flow_control_low].name = "billing_cgm_flow_control_low";
    submodule_data->defines[dnx_data_stif_flow_control_define_billing_cgm_flow_control_low].doc = "billing cgm flow control min value - calculated with formula given from the architecture";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_billing_cgm_flow_control_low].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_qsize_cgm_flow_control_high].name = "qsize_cgm_flow_control_high";
    submodule_data->defines[dnx_data_stif_flow_control_define_qsize_cgm_flow_control_high].doc = "qsize cgm flow control max value - calculated with formula given from the architecture";
    
    submodule_data->defines[dnx_data_stif_flow_control_define_qsize_cgm_flow_control_high].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stif_flow_control_define_qsize_cgm_flow_control_low].name = "qsize_cgm_flow_control_low";
    submodule_data->defines[dnx_data_stif_flow_control_define_qsize_cgm_flow_control_low].doc = "qsize cgm flow control min value - calculated with formula given from the architecture";
    
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







shr_error_e
dnx_data_stif_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "stif";
    module_data->nof_submodules = _dnx_data_stif_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data stif submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_stif_config_init(unit, &module_data->submodules[dnx_data_stif_submodule_config]));
    SHR_IF_ERR_EXIT(dnx_data_stif_port_init(unit, &module_data->submodules[dnx_data_stif_submodule_port]));
    SHR_IF_ERR_EXIT(dnx_data_stif_report_init(unit, &module_data->submodules[dnx_data_stif_submodule_report]));
    SHR_IF_ERR_EXIT(dnx_data_stif_flow_control_init(unit, &module_data->submodules[dnx_data_stif_submodule_flow_control]));
    
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
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

