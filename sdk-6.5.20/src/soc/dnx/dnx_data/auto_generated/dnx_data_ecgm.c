

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_ecgm.h>



extern shr_error_e jr2_a0_data_ecgm_attach(
    int unit);
extern shr_error_e j2c_a0_data_ecgm_attach(
    int unit);
extern shr_error_e q2a_a0_data_ecgm_attach(
    int unit);
extern shr_error_e j2p_a0_data_ecgm_attach(
    int unit);



static shr_error_e
dnx_data_ecgm_core_resources_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "core_resources";
    submodule_data->doc = "ecgm resources info per core";
    
    submodule_data->nof_features = _dnx_data_ecgm_core_resources_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ecgm core_resources features");

    
    submodule_data->nof_defines = _dnx_data_ecgm_core_resources_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ecgm core_resources defines");

    submodule_data->defines[dnx_data_ecgm_core_resources_define_nof_sp].name = "nof_sp";
    submodule_data->defines[dnx_data_ecgm_core_resources_define_nof_sp].doc = "number of service pools";
    
    submodule_data->defines[dnx_data_ecgm_core_resources_define_nof_sp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ecgm_core_resources_define_nof_interface_profiles].name = "nof_interface_profiles";
    submodule_data->defines[dnx_data_ecgm_core_resources_define_nof_interface_profiles].doc = "total interface profiles per core";
    
    submodule_data->defines[dnx_data_ecgm_core_resources_define_nof_interface_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ecgm_core_resources_define_nof_port_profiles].name = "nof_port_profiles";
    submodule_data->defines[dnx_data_ecgm_core_resources_define_nof_port_profiles].doc = "total port profiles per core";
    
    submodule_data->defines[dnx_data_ecgm_core_resources_define_nof_port_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ecgm_core_resources_define_total_pds].name = "total_pds";
    submodule_data->defines[dnx_data_ecgm_core_resources_define_total_pds].doc = "total packet descriptors per core";
    
    submodule_data->defines[dnx_data_ecgm_core_resources_define_total_pds].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ecgm_core_resources_define_total_pds_nof_bits].name = "total_pds_nof_bits";
    submodule_data->defines[dnx_data_ecgm_core_resources_define_total_pds_nof_bits].doc = "number of bits for total packet descriptors";
    
    submodule_data->defines[dnx_data_ecgm_core_resources_define_total_pds_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ecgm_core_resources_define_total_dbs].name = "total_dbs";
    submodule_data->defines[dnx_data_ecgm_core_resources_define_total_dbs].doc = "total data buffers per core";
    
    submodule_data->defines[dnx_data_ecgm_core_resources_define_total_dbs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ecgm_core_resources_define_total_dbs_nof_bits].name = "total_dbs_nof_bits";
    submodule_data->defines[dnx_data_ecgm_core_resources_define_total_dbs_nof_bits].doc = "number of bits for total data buffers";
    
    submodule_data->defines[dnx_data_ecgm_core_resources_define_total_dbs_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ecgm_core_resources_define_max_alpha_value].name = "max_alpha_value";
    submodule_data->defines[dnx_data_ecgm_core_resources_define_max_alpha_value].doc = "max value for FADT alpha";
    
    submodule_data->defines[dnx_data_ecgm_core_resources_define_max_alpha_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ecgm_core_resources_define_min_alpha_value].name = "min_alpha_value";
    submodule_data->defines[dnx_data_ecgm_core_resources_define_min_alpha_value].doc = "min value for FADT alpha";
    
    submodule_data->defines[dnx_data_ecgm_core_resources_define_min_alpha_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ecgm_core_resources_define_max_core_bandwidth_Mbps].name = "max_core_bandwidth_Mbps";
    submodule_data->defines[dnx_data_ecgm_core_resources_define_max_core_bandwidth_Mbps].doc = "maximum bandwidth pec core in Mbps for ecgm calculation";
    
    submodule_data->defines[dnx_data_ecgm_core_resources_define_max_core_bandwidth_Mbps].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_ecgm_core_resources_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ecgm core_resources tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ecgm_core_resources_feature_get(
    int unit,
    dnx_data_ecgm_core_resources_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_core_resources, feature);
}


uint32
dnx_data_ecgm_core_resources_nof_sp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_core_resources, dnx_data_ecgm_core_resources_define_nof_sp);
}

uint32
dnx_data_ecgm_core_resources_nof_interface_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_core_resources, dnx_data_ecgm_core_resources_define_nof_interface_profiles);
}

uint32
dnx_data_ecgm_core_resources_nof_port_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_core_resources, dnx_data_ecgm_core_resources_define_nof_port_profiles);
}

uint32
dnx_data_ecgm_core_resources_total_pds_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_core_resources, dnx_data_ecgm_core_resources_define_total_pds);
}

uint32
dnx_data_ecgm_core_resources_total_pds_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_core_resources, dnx_data_ecgm_core_resources_define_total_pds_nof_bits);
}

uint32
dnx_data_ecgm_core_resources_total_dbs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_core_resources, dnx_data_ecgm_core_resources_define_total_dbs);
}

uint32
dnx_data_ecgm_core_resources_total_dbs_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_core_resources, dnx_data_ecgm_core_resources_define_total_dbs_nof_bits);
}

uint32
dnx_data_ecgm_core_resources_max_alpha_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_core_resources, dnx_data_ecgm_core_resources_define_max_alpha_value);
}

uint32
dnx_data_ecgm_core_resources_min_alpha_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_core_resources, dnx_data_ecgm_core_resources_define_min_alpha_value);
}

uint32
dnx_data_ecgm_core_resources_max_core_bandwidth_Mbps_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_core_resources, dnx_data_ecgm_core_resources_define_max_core_bandwidth_Mbps);
}










static shr_error_e
dnx_data_ecgm_port_resources_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "port_resources";
    submodule_data->doc = "ecgm resources info per port";
    
    submodule_data->nof_features = _dnx_data_ecgm_port_resources_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ecgm port_resources features");

    
    submodule_data->nof_defines = _dnx_data_ecgm_port_resources_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ecgm port_resources defines");

    submodule_data->defines[dnx_data_ecgm_port_resources_define_max_nof_ports].name = "max_nof_ports";
    submodule_data->defines[dnx_data_ecgm_port_resources_define_max_nof_ports].doc = "maximum num of ports per core for ecgm calculation";
    
    submodule_data->defines[dnx_data_ecgm_port_resources_define_max_nof_ports].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_ecgm_port_resources_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ecgm port_resources tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ecgm_port_resources_feature_get(
    int unit,
    dnx_data_ecgm_port_resources_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_port_resources, feature);
}


uint32
dnx_data_ecgm_port_resources_max_nof_ports_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_port_resources, dnx_data_ecgm_port_resources_define_max_nof_ports);
}










static shr_error_e
dnx_data_ecgm_delete_fifo_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "delete_fifo";
    submodule_data->doc = "default values for ECGM delete FIFOs";
    
    submodule_data->nof_features = _dnx_data_ecgm_delete_fifo_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ecgm delete_fifo features");

    
    submodule_data->nof_defines = _dnx_data_ecgm_delete_fifo_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ecgm delete_fifo defines");

    
    submodule_data->nof_tables = _dnx_data_ecgm_delete_fifo_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ecgm delete_fifo tables");

    
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].name = "thresholds";
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].doc = "Delete FIFO thresholds";
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].size_of_values = sizeof(dnx_data_ecgm_delete_fifo_thresholds_t);
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].entry_get = dnx_data_ecgm_delete_fifo_thresholds_entry_str_get;

    
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].nof_keys = 1;
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].keys[0].name = "fifo_type";
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].keys[0].doc = "fifo type to get thresholds to.";

    
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].values, dnxc_data_value_t, submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].nof_values, "_dnx_data_ecgm_delete_fifo_table_thresholds table values");
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].values[0].name = "fqp_high_priority";
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].values[0].type = "uint32";
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].values[0].doc = "threshold for higher priority in FQP";
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].values[0].offset = UTILEX_OFFSETOF(dnx_data_ecgm_delete_fifo_thresholds_t, fqp_high_priority);
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].values[1].name = "stop_mc_low";
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].values[1].type = "uint32";
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].values[1].doc = "threshold for stop reading multicast low priority FIFO";
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].values[1].offset = UTILEX_OFFSETOF(dnx_data_ecgm_delete_fifo_thresholds_t, stop_mc_low);
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].values[2].name = "stop_mc_high";
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].values[2].type = "uint32";
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].values[2].doc = "threshold for stop reading multicast high priority FIFO";
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].values[2].offset = UTILEX_OFFSETOF(dnx_data_ecgm_delete_fifo_thresholds_t, stop_mc_high);
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].values[3].name = "stop_all";
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].values[3].type = "uint32";
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].values[3].doc = "threshold for stop reading unicast FIFO and stop send packets to ERPP";
    submodule_data->tables[dnx_data_ecgm_delete_fifo_table_thresholds].values[3].offset = UTILEX_OFFSETOF(dnx_data_ecgm_delete_fifo_thresholds_t, stop_all);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ecgm_delete_fifo_feature_get(
    int unit,
    dnx_data_ecgm_delete_fifo_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_delete_fifo, feature);
}




const dnx_data_ecgm_delete_fifo_thresholds_t *
dnx_data_ecgm_delete_fifo_thresholds_get(
    int unit,
    int fifo_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_delete_fifo, dnx_data_ecgm_delete_fifo_table_thresholds);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, fifo_type, 0);
    return (const dnx_data_ecgm_delete_fifo_thresholds_t *) data;

}


shr_error_e
dnx_data_ecgm_delete_fifo_thresholds_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_ecgm_delete_fifo_thresholds_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_delete_fifo, dnx_data_ecgm_delete_fifo_table_thresholds);
    data = (const dnx_data_ecgm_delete_fifo_thresholds_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fqp_high_priority);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->stop_mc_low);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->stop_mc_high);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->stop_all);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_ecgm_delete_fifo_thresholds_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_delete_fifo, dnx_data_ecgm_delete_fifo_table_thresholds);

}






static shr_error_e
dnx_data_ecgm_info_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "info";
    submodule_data->doc = "Egress congestion info";
    
    submodule_data->nof_features = _dnx_data_ecgm_info_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ecgm info features");

    
    submodule_data->nof_defines = _dnx_data_ecgm_info_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ecgm info defines");

    submodule_data->defines[dnx_data_ecgm_info_define_nof_dropped_reasons_rqp].name = "nof_dropped_reasons_rqp";
    submodule_data->defines[dnx_data_ecgm_info_define_nof_dropped_reasons_rqp].doc = "number of reasons for the dropped packets in RQP";
    
    submodule_data->defines[dnx_data_ecgm_info_define_nof_dropped_reasons_rqp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ecgm_info_define_nof_dropped_reasons_pqp].name = "nof_dropped_reasons_pqp";
    submodule_data->defines[dnx_data_ecgm_info_define_nof_dropped_reasons_pqp].doc = "number of reasons for the dropped packets in PQP";
    
    submodule_data->defines[dnx_data_ecgm_info_define_nof_dropped_reasons_pqp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ecgm_info_define_nof_bits_in_pd_count].name = "nof_bits_in_pd_count";
    submodule_data->defines[dnx_data_ecgm_info_define_nof_bits_in_pd_count].doc = "number of bits in PD COUNT";
    
    submodule_data->defines[dnx_data_ecgm_info_define_nof_bits_in_pd_count].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_ecgm_info_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ecgm info tables");

    
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_rqp].name = "dropped_reason_rqp";
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_rqp].doc = "reasons for the dropped and rejected packets in RQP";
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_rqp].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_rqp].size_of_values = sizeof(dnx_data_ecgm_info_dropped_reason_rqp_t);
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_rqp].entry_get = dnx_data_ecgm_info_dropped_reason_rqp_entry_str_get;

    
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_rqp].nof_keys = 1;
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_rqp].keys[0].name = "index";
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_rqp].keys[0].doc = "this index mapped to the bit in RQP_DISCARD_REASONS";

    
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_rqp].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_rqp].values, dnxc_data_value_t, submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_rqp].nof_values, "_dnx_data_ecgm_info_table_dropped_reason_rqp table values");
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_rqp].values[0].name = "reason";
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_rqp].values[0].type = "char *";
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_rqp].values[0].doc = "reason string";
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_rqp].values[0].offset = UTILEX_OFFSETOF(dnx_data_ecgm_info_dropped_reason_rqp_t, reason);

    
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_pqp].name = "dropped_reason_pqp";
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_pqp].doc = "reasons for the dropped and rejected packets in PQP";
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_pqp].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_pqp].size_of_values = sizeof(dnx_data_ecgm_info_dropped_reason_pqp_t);
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_pqp].entry_get = dnx_data_ecgm_info_dropped_reason_pqp_entry_str_get;

    
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_pqp].nof_keys = 1;
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_pqp].keys[0].name = "index";
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_pqp].keys[0].doc = "this index mapped to the bit in PQP_DISCARD_REASONS";

    
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_pqp].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_pqp].values, dnxc_data_value_t, submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_pqp].nof_values, "_dnx_data_ecgm_info_table_dropped_reason_pqp table values");
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_pqp].values[0].name = "reason";
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_pqp].values[0].type = "char *";
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_pqp].values[0].doc = "reason string";
    submodule_data->tables[dnx_data_ecgm_info_table_dropped_reason_pqp].values[0].offset = UTILEX_OFFSETOF(dnx_data_ecgm_info_dropped_reason_pqp_t, reason);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ecgm_info_feature_get(
    int unit,
    dnx_data_ecgm_info_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_info, feature);
}


uint32
dnx_data_ecgm_info_nof_dropped_reasons_rqp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_info, dnx_data_ecgm_info_define_nof_dropped_reasons_rqp);
}

uint32
dnx_data_ecgm_info_nof_dropped_reasons_pqp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_info, dnx_data_ecgm_info_define_nof_dropped_reasons_pqp);
}

uint32
dnx_data_ecgm_info_nof_bits_in_pd_count_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_info, dnx_data_ecgm_info_define_nof_bits_in_pd_count);
}



const dnx_data_ecgm_info_dropped_reason_rqp_t *
dnx_data_ecgm_info_dropped_reason_rqp_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_info, dnx_data_ecgm_info_table_dropped_reason_rqp);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_ecgm_info_dropped_reason_rqp_t *) data;

}

const dnx_data_ecgm_info_dropped_reason_pqp_t *
dnx_data_ecgm_info_dropped_reason_pqp_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_info, dnx_data_ecgm_info_table_dropped_reason_pqp);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_ecgm_info_dropped_reason_pqp_t *) data;

}


shr_error_e
dnx_data_ecgm_info_dropped_reason_rqp_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_ecgm_info_dropped_reason_rqp_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_info, dnx_data_ecgm_info_table_dropped_reason_rqp);
    data = (const dnx_data_ecgm_info_dropped_reason_rqp_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->reason == NULL ? "" : data->reason);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_ecgm_info_dropped_reason_pqp_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_ecgm_info_dropped_reason_pqp_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_info, dnx_data_ecgm_info_table_dropped_reason_pqp);
    data = (const dnx_data_ecgm_info_dropped_reason_pqp_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->reason == NULL ? "" : data->reason);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_ecgm_info_dropped_reason_rqp_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_info, dnx_data_ecgm_info_table_dropped_reason_rqp);

}

const dnxc_data_table_info_t *
dnx_data_ecgm_info_dropped_reason_pqp_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_ecgm, dnx_data_ecgm_submodule_info, dnx_data_ecgm_info_table_dropped_reason_pqp);

}



shr_error_e
dnx_data_ecgm_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "ecgm";
    module_data->nof_submodules = _dnx_data_ecgm_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data ecgm submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_ecgm_core_resources_init(unit, &module_data->submodules[dnx_data_ecgm_submodule_core_resources]));
    SHR_IF_ERR_EXIT(dnx_data_ecgm_port_resources_init(unit, &module_data->submodules[dnx_data_ecgm_submodule_port_resources]));
    SHR_IF_ERR_EXIT(dnx_data_ecgm_delete_fifo_init(unit, &module_data->submodules[dnx_data_ecgm_submodule_delete_fifo]));
    SHR_IF_ERR_EXIT(dnx_data_ecgm_info_init(unit, &module_data->submodules[dnx_data_ecgm_submodule_info]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ecgm_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ecgm_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ecgm_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ecgm_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_ecgm_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ecgm_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_ecgm_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ecgm_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_ecgm_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ecgm_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_ecgm_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ecgm_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_ecgm_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ecgm_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_ecgm_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

