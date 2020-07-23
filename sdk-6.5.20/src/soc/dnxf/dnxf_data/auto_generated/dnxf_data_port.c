

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_port.h>



extern shr_error_e ramon_a0_data_port_attach(
    int unit);



static shr_error_e
dnxf_data_port_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "General attributes";
    
    submodule_data->nof_features = _dnxf_data_port_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data port general features");

    submodule_data->features[dnxf_data_port_general_mac_cell_shaper_supported].name = "mac_cell_shaper_supported";
    submodule_data->features[dnxf_data_port_general_mac_cell_shaper_supported].doc = "Indicates if MAC cell shaper is supported for the given device";
    submodule_data->features[dnxf_data_port_general_mac_cell_shaper_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnxf_data_port_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data port general defines");

    submodule_data->defines[dnxf_data_port_general_define_nof_links].name = "nof_links";
    submodule_data->defines[dnxf_data_port_general_define_nof_links].doc = "Number of links.";
    
    submodule_data->defines[dnxf_data_port_general_define_nof_links].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_general_define_nof_pms].name = "nof_pms";
    submodule_data->defines[dnxf_data_port_general_define_nof_pms].doc = "Number of port macros.";
    
    submodule_data->defines[dnxf_data_port_general_define_nof_pms].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_general_define_max_bucket_fill_rate].name = "max_bucket_fill_rate";
    submodule_data->defines[dnxf_data_port_general_define_max_bucket_fill_rate].doc = "Max bucket fill rate.";
    
    submodule_data->defines[dnxf_data_port_general_define_max_bucket_fill_rate].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_general_define_link_mask].name = "link_mask";
    submodule_data->defines[dnxf_data_port_general_define_link_mask].doc = "Mask adjusted to number of links.";
    
    submodule_data->defines[dnxf_data_port_general_define_link_mask].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_general_define_aldwp_max_val].name = "aldwp_max_val";
    submodule_data->defines[dnxf_data_port_general_define_aldwp_max_val].doc = "Maximum value of ALDWP.";
    
    submodule_data->defines[dnxf_data_port_general_define_aldwp_max_val].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_general_define_fmac_clock_khz].name = "fmac_clock_khz";
    submodule_data->defines[dnxf_data_port_general_define_fmac_clock_khz].doc = "FMAC clock frequency [KHz]";
    
    submodule_data->defines[dnxf_data_port_general_define_fmac_clock_khz].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_general_define_speed_max].name = "speed_max";
    submodule_data->defines[dnxf_data_port_general_define_speed_max].doc = "Maximum speed of a port";
    
    submodule_data->defines[dnxf_data_port_general_define_speed_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_general_define_fw_load_method].name = "fw_load_method";
    submodule_data->defines[dnxf_data_port_general_define_fw_load_method].doc = "The method of the firmware load.";
    
    submodule_data->defines[dnxf_data_port_general_define_fw_load_method].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_port_general_define_fw_crc_check].name = "fw_crc_check";
    submodule_data->defines[dnxf_data_port_general_define_fw_crc_check].doc = "Enable firmware CRC check.";
    
    submodule_data->defines[dnxf_data_port_general_define_fw_crc_check].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_port_general_define_fw_load_verify].name = "fw_load_verify";
    submodule_data->defines[dnxf_data_port_general_define_fw_load_verify].doc = "Enable firmware load verification.";
    
    submodule_data->defines[dnxf_data_port_general_define_fw_load_verify].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_port_general_define_mac_bucket_fill_rate].name = "mac_bucket_fill_rate";
    submodule_data->defines[dnxf_data_port_general_define_mac_bucket_fill_rate].doc = "Mac bucket fill rate.";
    
    submodule_data->defines[dnxf_data_port_general_define_mac_bucket_fill_rate].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnxf_data_port_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port general tables");

    
    submodule_data->tables[dnxf_data_port_general_table_supported_phys].name = "supported_phys";
    submodule_data->tables[dnxf_data_port_general_table_supported_phys].doc = "general data about phys";
    submodule_data->tables[dnxf_data_port_general_table_supported_phys].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_general_table_supported_phys].size_of_values = sizeof(dnxf_data_port_general_supported_phys_t);
    submodule_data->tables[dnxf_data_port_general_table_supported_phys].entry_get = dnxf_data_port_general_supported_phys_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_general_table_supported_phys].nof_keys = 0;

    
    submodule_data->tables[dnxf_data_port_general_table_supported_phys].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_port_general_table_supported_phys].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_port_general_table_supported_phys].nof_values, "_dnxf_data_port_general_table_supported_phys table values");
    submodule_data->tables[dnxf_data_port_general_table_supported_phys].values[0].name = "pbmp";
    submodule_data->tables[dnxf_data_port_general_table_supported_phys].values[0].type = "bcm_pbmp_t";
    submodule_data->tables[dnxf_data_port_general_table_supported_phys].values[0].doc = "bitmap of supported phys.";
    submodule_data->tables[dnxf_data_port_general_table_supported_phys].values[0].offset = UTILEX_OFFSETOF(dnxf_data_port_general_supported_phys_t, pbmp);

    
    submodule_data->tables[dnxf_data_port_general_table_fmac_bus_size].name = "fmac_bus_size";
    submodule_data->tables[dnxf_data_port_general_table_fmac_bus_size].doc = "Fabric MAC bus size [bits]";
    submodule_data->tables[dnxf_data_port_general_table_fmac_bus_size].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_general_table_fmac_bus_size].size_of_values = sizeof(dnxf_data_port_general_fmac_bus_size_t);
    submodule_data->tables[dnxf_data_port_general_table_fmac_bus_size].entry_get = dnxf_data_port_general_fmac_bus_size_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_general_table_fmac_bus_size].nof_keys = 1;
    submodule_data->tables[dnxf_data_port_general_table_fmac_bus_size].keys[0].name = "mode";
    submodule_data->tables[dnxf_data_port_general_table_fmac_bus_size].keys[0].doc = "Fabric MAC mode (FEC type)";

    
    submodule_data->tables[dnxf_data_port_general_table_fmac_bus_size].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_port_general_table_fmac_bus_size].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_port_general_table_fmac_bus_size].nof_values, "_dnxf_data_port_general_table_fmac_bus_size table values");
    submodule_data->tables[dnxf_data_port_general_table_fmac_bus_size].values[0].name = "size";
    submodule_data->tables[dnxf_data_port_general_table_fmac_bus_size].values[0].type = "int";
    submodule_data->tables[dnxf_data_port_general_table_fmac_bus_size].values[0].doc = "The bus size [bits]";
    submodule_data->tables[dnxf_data_port_general_table_fmac_bus_size].values[0].offset = UTILEX_OFFSETOF(dnxf_data_port_general_fmac_bus_size_t, size);


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_port_general_feature_get(
    int unit,
    dnxf_data_port_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, feature);
}


uint32
dnxf_data_port_general_nof_links_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_define_nof_links);
}

uint32
dnxf_data_port_general_nof_pms_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_define_nof_pms);
}

uint32
dnxf_data_port_general_max_bucket_fill_rate_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_define_max_bucket_fill_rate);
}

uint32
dnxf_data_port_general_link_mask_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_define_link_mask);
}

uint32
dnxf_data_port_general_aldwp_max_val_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_define_aldwp_max_val);
}

uint32
dnxf_data_port_general_fmac_clock_khz_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_define_fmac_clock_khz);
}

uint32
dnxf_data_port_general_speed_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_define_speed_max);
}

uint32
dnxf_data_port_general_fw_load_method_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_define_fw_load_method);
}

uint32
dnxf_data_port_general_fw_crc_check_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_define_fw_crc_check);
}

uint32
dnxf_data_port_general_fw_load_verify_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_define_fw_load_verify);
}

uint32
dnxf_data_port_general_mac_bucket_fill_rate_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_define_mac_bucket_fill_rate);
}



const dnxf_data_port_general_supported_phys_t *
dnxf_data_port_general_supported_phys_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_supported_phys);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnxf_data_port_general_supported_phys_t *) data;

}

const dnxf_data_port_general_fmac_bus_size_t *
dnxf_data_port_general_fmac_bus_size_get(
    int unit,
    int mode)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_fmac_bus_size);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mode, 0);
    return (const dnxf_data_port_general_fmac_bus_size_t *) data;

}


shr_error_e
dnxf_data_port_general_supported_phys_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_port_general_supported_phys_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_supported_phys);
    data = (const dnxf_data_port_general_supported_phys_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_PBMP_STR(buffer, data->pbmp);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_port_general_fmac_bus_size_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_port_general_fmac_bus_size_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_fmac_bus_size);
    data = (const dnxf_data_port_general_fmac_bus_size_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->size);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnxf_data_port_general_supported_phys_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_supported_phys);

}

const dnxc_data_table_info_t *
dnxf_data_port_general_fmac_bus_size_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_fmac_bus_size);

}






static shr_error_e
dnxf_data_port_stat_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "stat";
    submodule_data->doc = "Port Statistics information";
    
    submodule_data->nof_features = _dnxf_data_port_stat_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data port stat features");

    
    submodule_data->nof_defines = _dnxf_data_port_stat_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data port stat defines");

    submodule_data->defines[dnxf_data_port_stat_define_thread_interval].name = "thread_interval";
    submodule_data->defines[dnxf_data_port_stat_define_thread_interval].doc = "Statistics thread interval in microseconds.";
    
    submodule_data->defines[dnxf_data_port_stat_define_thread_interval].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_port_stat_define_thread_counter_pri].name = "thread_counter_pri";
    submodule_data->defines[dnxf_data_port_stat_define_thread_counter_pri].doc = "Counter thread priority.";
    
    submodule_data->defines[dnxf_data_port_stat_define_thread_counter_pri].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_port_stat_define_thread_timeout].name = "thread_timeout";
    submodule_data->defines[dnxf_data_port_stat_define_thread_timeout].doc = "Statistics thread sync operation timeout.";
    
    submodule_data->defines[dnxf_data_port_stat_define_thread_timeout].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnxf_data_port_stat_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port stat tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_port_stat_feature_get(
    int unit,
    dnxf_data_port_stat_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_stat, feature);
}


uint32
dnxf_data_port_stat_thread_interval_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_stat, dnxf_data_port_stat_define_thread_interval);
}

uint32
dnxf_data_port_stat_thread_counter_pri_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_stat, dnxf_data_port_stat_define_thread_counter_pri);
}

uint32
dnxf_data_port_stat_thread_timeout_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_stat, dnxf_data_port_stat_define_thread_timeout);
}










static shr_error_e
dnxf_data_port_static_add_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "static_add";
    submodule_data->doc = "General static add information";
    
    submodule_data->nof_features = _dnxf_data_port_static_add_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data port static_add features");

    
    submodule_data->nof_defines = _dnxf_data_port_static_add_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data port static_add defines");

    
    submodule_data->nof_tables = _dnxf_data_port_static_add_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port static_add tables");

    
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].name = "port_info";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].doc = "General port info";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].size_of_values = sizeof(dnxf_data_port_static_add_port_info_t);
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].entry_get = dnxf_data_port_static_add_port_info_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].nof_keys = 1;
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].keys[0].name = "port";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].keys[0].doc = "Link ID.";

    
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].nof_values = 8;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_port_static_add_table_port_info].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_port_static_add_table_port_info].nof_values, "_dnxf_data_port_static_add_table_port_info table values");
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[0].name = "speed";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[0].type = "int";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[0].doc = "Sets the data transfer speed with which the port is initialized";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[0].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_port_info_t, speed);
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[1].name = "link_training";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[1].type = "uint32";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[1].doc = "Does the port has its link training enabled";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[1].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_port_info_t, link_training);
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[2].name = "fec_type";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[2].type = "bcm_port_phy_fec_t";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[2].doc = "Specifies the FEC type";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[2].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_port_info_t, fec_type);
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[3].name = "tx_polarity";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[3].type = "uint32";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[3].doc = "Switch TX phy polarity";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[3].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_port_info_t, tx_polarity);
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[4].name = "rx_polarity";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[4].type = "uint32";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[4].doc = "Switch RX phy polarity";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[4].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_port_info_t, rx_polarity);
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[5].name = "connected_to_repeater";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[5].type = "uint32";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[5].doc = "Shows if the link is connected to repeater device";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[5].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_port_info_t, connected_to_repeater);
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[6].name = "tx_pam4_precoder";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[6].type = "uint32";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[6].doc = "is the precoding enabled on TX side";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[6].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_port_info_t, tx_pam4_precoder);
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[7].name = "lp_tx_precoder";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[7].type = "uint32";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[7].doc = "has the link partner enabled pre-coding on its TX side. In other words - enable the decoding on my RX side.";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[7].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_port_info_t, lp_tx_precoder);

    
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].name = "serdes_lane_config";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].doc = "serdes lane configurations, related to firmware";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].size_of_values = sizeof(dnxf_data_port_static_add_serdes_lane_config_t);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].entry_get = dnxf_data_port_static_add_serdes_lane_config_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].nof_keys = 1;
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].keys[0].name = "port";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].keys[0].doc = "Link ID.";

    
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].nof_values = 6;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].nof_values, "_dnxf_data_port_static_add_table_serdes_lane_config table values");
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[0].name = "dfe";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[0].type = "soc_dnxc_port_dfe_mode_t";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[0].doc = "dfe filter";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[0].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_lane_config_t, dfe);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[1].name = "media_type";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[1].type = "int";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[1].doc = "media type";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[1].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_lane_config_t, media_type);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[2].name = "unreliable_los";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[2].type = "int";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[2].doc = "unreliable los";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[2].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_lane_config_t, unreliable_los);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[3].name = "cl72_auto_polarity_enable";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[3].type = "int";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[3].doc = "cl72 auto polarity";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[3].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_lane_config_t, cl72_auto_polarity_enable);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[4].name = "cl72_restart_timeout_enable";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[4].type = "int";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[4].doc = "cl72 restart timeout";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[4].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_lane_config_t, cl72_restart_timeout_enable);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[5].name = "channel_mode";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[5].type = "soc_dnxc_port_channel_mode_t";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[5].doc = "force nr or force er";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].values[5].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_lane_config_t, channel_mode);

    
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].name = "serdes_tx_taps";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].doc = "TX FIR parameters";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].size_of_values = sizeof(dnxf_data_port_static_add_serdes_tx_taps_t);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].entry_get = dnxf_data_port_static_add_serdes_tx_taps_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].nof_keys = 1;
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].keys[0].name = "port";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].keys[0].doc = "Link ID.";

    
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].nof_values = 8;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].nof_values, "_dnxf_data_port_static_add_table_serdes_tx_taps table values");
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[0].name = "pre2";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[0].type = "int";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[0].doc = "pre2";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[0].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_tx_taps_t, pre2);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[1].name = "pre";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[1].type = "int";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[1].doc = "pre";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[1].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_tx_taps_t, pre);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[2].name = "main";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[2].type = "int";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[2].doc = "main";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[2].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_tx_taps_t, main);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[3].name = "post";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[3].type = "int";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[3].doc = "post";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[3].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_tx_taps_t, post);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[4].name = "post2";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[4].type = "int";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[4].doc = "post2";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[4].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_tx_taps_t, post2);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[5].name = "post3";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[5].type = "int";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[5].doc = "post3";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[5].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_tx_taps_t, post3);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[6].name = "tx_tap_mode";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[6].type = "bcm_port_phy_tx_tap_mode_t";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[6].doc = "tx tap mode - 3 or 6 taps";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[6].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_tx_taps_t, tx_tap_mode);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[7].name = "signalling_mode";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[7].type = "bcm_port_phy_signalling_mode_t";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[7].doc = "signalling mode - pam4 or nrz";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[7].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_tx_taps_t, signalling_mode);

    
    submodule_data->tables[dnxf_data_port_static_add_table_quad_info].name = "quad_info";
    submodule_data->tables[dnxf_data_port_static_add_table_quad_info].doc = "General quad info";
    submodule_data->tables[dnxf_data_port_static_add_table_quad_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_static_add_table_quad_info].size_of_values = sizeof(dnxf_data_port_static_add_quad_info_t);
    submodule_data->tables[dnxf_data_port_static_add_table_quad_info].entry_get = dnxf_data_port_static_add_quad_info_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_static_add_table_quad_info].nof_keys = 1;
    submodule_data->tables[dnxf_data_port_static_add_table_quad_info].keys[0].name = "quad";
    submodule_data->tables[dnxf_data_port_static_add_table_quad_info].keys[0].doc = "Quad ID.";

    
    submodule_data->tables[dnxf_data_port_static_add_table_quad_info].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_port_static_add_table_quad_info].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_port_static_add_table_quad_info].nof_values, "_dnxf_data_port_static_add_table_quad_info table values");
    submodule_data->tables[dnxf_data_port_static_add_table_quad_info].values[0].name = "quad_enable";
    submodule_data->tables[dnxf_data_port_static_add_table_quad_info].values[0].type = "int";
    submodule_data->tables[dnxf_data_port_static_add_table_quad_info].values[0].doc = "Is the quad enabled.";
    submodule_data->tables[dnxf_data_port_static_add_table_quad_info].values[0].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_quad_info_t, quad_enable);


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_port_static_add_feature_get(
    int unit,
    dnxf_data_port_static_add_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_static_add, feature);
}




const dnxf_data_port_static_add_port_info_t *
dnxf_data_port_static_add_port_info_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_static_add, dnxf_data_port_static_add_table_port_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnxf_data_port_static_add_port_info_t *) data;

}

const dnxf_data_port_static_add_serdes_lane_config_t *
dnxf_data_port_static_add_serdes_lane_config_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_static_add, dnxf_data_port_static_add_table_serdes_lane_config);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnxf_data_port_static_add_serdes_lane_config_t *) data;

}

const dnxf_data_port_static_add_serdes_tx_taps_t *
dnxf_data_port_static_add_serdes_tx_taps_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_static_add, dnxf_data_port_static_add_table_serdes_tx_taps);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnxf_data_port_static_add_serdes_tx_taps_t *) data;

}

const dnxf_data_port_static_add_quad_info_t *
dnxf_data_port_static_add_quad_info_get(
    int unit,
    int quad)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_static_add, dnxf_data_port_static_add_table_quad_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, quad, 0);
    return (const dnxf_data_port_static_add_quad_info_t *) data;

}


shr_error_e
dnxf_data_port_static_add_port_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_port_static_add_port_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_static_add, dnxf_data_port_static_add_table_port_info);
    data = (const dnxf_data_port_static_add_port_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->speed);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->link_training);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fec_type);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tx_polarity);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->rx_polarity);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->connected_to_repeater);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tx_pam4_precoder);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->lp_tx_precoder);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_port_static_add_serdes_lane_config_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_port_static_add_serdes_lane_config_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_static_add, dnxf_data_port_static_add_table_serdes_lane_config);
    data = (const dnxf_data_port_static_add_serdes_lane_config_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dfe);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->media_type);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->unreliable_los);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->cl72_auto_polarity_enable);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->cl72_restart_timeout_enable);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->channel_mode);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_port_static_add_serdes_tx_taps_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_port_static_add_serdes_tx_taps_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_static_add, dnxf_data_port_static_add_table_serdes_tx_taps);
    data = (const dnxf_data_port_static_add_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pre2);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pre);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->main);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->post);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->post2);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->post3);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tx_tap_mode);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->signalling_mode);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_port_static_add_quad_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_port_static_add_quad_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_static_add, dnxf_data_port_static_add_table_quad_info);
    data = (const dnxf_data_port_static_add_quad_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->quad_enable);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnxf_data_port_static_add_port_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_static_add, dnxf_data_port_static_add_table_port_info);

}

const dnxc_data_table_info_t *
dnxf_data_port_static_add_serdes_lane_config_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_static_add, dnxf_data_port_static_add_table_serdes_lane_config);

}

const dnxc_data_table_info_t *
dnxf_data_port_static_add_serdes_tx_taps_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_static_add, dnxf_data_port_static_add_table_serdes_tx_taps);

}

const dnxc_data_table_info_t *
dnxf_data_port_static_add_quad_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_static_add, dnxf_data_port_static_add_table_quad_info);

}






static shr_error_e
dnxf_data_port_lane_map_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "lane_map";
    submodule_data->doc = "General lane map attributes";
    
    submodule_data->nof_features = _dnxf_data_port_lane_map_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data port lane_map features");

    
    submodule_data->nof_defines = _dnxf_data_port_lane_map_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data port lane_map defines");

    submodule_data->defines[dnxf_data_port_lane_map_define_lane_map_bound].name = "lane_map_bound";
    submodule_data->defines[dnxf_data_port_lane_map_define_lane_map_bound].doc = "Lane swapping is allowed within this nof links.";
    
    submodule_data->defines[dnxf_data_port_lane_map_define_lane_map_bound].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnxf_data_port_lane_map_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port lane_map tables");

    
    submodule_data->tables[dnxf_data_port_lane_map_table_info].name = "info";
    submodule_data->tables[dnxf_data_port_lane_map_table_info].doc = "Shows the DB for the mapping between the lanes and the serdeses.";
    submodule_data->tables[dnxf_data_port_lane_map_table_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_lane_map_table_info].size_of_values = sizeof(dnxf_data_port_lane_map_info_t);
    submodule_data->tables[dnxf_data_port_lane_map_table_info].entry_get = dnxf_data_port_lane_map_info_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_lane_map_table_info].nof_keys = 1;
    submodule_data->tables[dnxf_data_port_lane_map_table_info].keys[0].name = "lane";
    submodule_data->tables[dnxf_data_port_lane_map_table_info].keys[0].doc = "Use lane as a key to return its serdes mapping values";

    
    submodule_data->tables[dnxf_data_port_lane_map_table_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_port_lane_map_table_info].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_port_lane_map_table_info].nof_values, "_dnxf_data_port_lane_map_table_info table values");
    submodule_data->tables[dnxf_data_port_lane_map_table_info].values[0].name = "serdes_rx_id";
    submodule_data->tables[dnxf_data_port_lane_map_table_info].values[0].type = "uint32";
    submodule_data->tables[dnxf_data_port_lane_map_table_info].values[0].doc = "The connected serdes rx id.";
    submodule_data->tables[dnxf_data_port_lane_map_table_info].values[0].offset = UTILEX_OFFSETOF(dnxf_data_port_lane_map_info_t, serdes_rx_id);
    submodule_data->tables[dnxf_data_port_lane_map_table_info].values[1].name = "serdes_tx_id";
    submodule_data->tables[dnxf_data_port_lane_map_table_info].values[1].type = "uint32";
    submodule_data->tables[dnxf_data_port_lane_map_table_info].values[1].doc = "The connected serdes tx id.";
    submodule_data->tables[dnxf_data_port_lane_map_table_info].values[1].offset = UTILEX_OFFSETOF(dnxf_data_port_lane_map_info_t, serdes_tx_id);


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_port_lane_map_feature_get(
    int unit,
    dnxf_data_port_lane_map_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_lane_map, feature);
}


uint32
dnxf_data_port_lane_map_lane_map_bound_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_lane_map, dnxf_data_port_lane_map_define_lane_map_bound);
}



const dnxf_data_port_lane_map_info_t *
dnxf_data_port_lane_map_info_get(
    int unit,
    int lane)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_lane_map, dnxf_data_port_lane_map_table_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, lane, 0);
    return (const dnxf_data_port_lane_map_info_t *) data;

}


shr_error_e
dnxf_data_port_lane_map_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_port_lane_map_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_lane_map, dnxf_data_port_lane_map_table_info);
    data = (const dnxf_data_port_lane_map_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->serdes_rx_id);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->serdes_tx_id);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnxf_data_port_lane_map_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_lane_map, dnxf_data_port_lane_map_table_info);

}






static shr_error_e
dnxf_data_port_pll_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "pll";
    submodule_data->doc = "General PLL attributes";
    
    submodule_data->nof_features = _dnxf_data_port_pll_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data port pll features");

    
    submodule_data->nof_defines = _dnxf_data_port_pll_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data port pll defines");

    submodule_data->defines[dnxf_data_port_pll_define_nof_lcpll].name = "nof_lcpll";
    submodule_data->defines[dnxf_data_port_pll_define_nof_lcpll].doc = "Number of LCPLL.";
    
    submodule_data->defines[dnxf_data_port_pll_define_nof_lcpll].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_pll_define_nof_port_in_lcpll].name = "nof_port_in_lcpll";
    submodule_data->defines[dnxf_data_port_pll_define_nof_port_in_lcpll].doc = "Number of ports for each LCPLL.";
    
    submodule_data->defines[dnxf_data_port_pll_define_nof_port_in_lcpll].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_pll_define_fabric_clock_freq_in_default].name = "fabric_clock_freq_in_default";
    submodule_data->defines[dnxf_data_port_pll_define_fabric_clock_freq_in_default].doc = "Fabric Clock freq in default.";
    
    submodule_data->defines[dnxf_data_port_pll_define_fabric_clock_freq_in_default].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_pll_define_fabric_clock_freq_out_default].name = "fabric_clock_freq_out_default";
    submodule_data->defines[dnxf_data_port_pll_define_fabric_clock_freq_out_default].doc = "Fabric Clock freq out default.";
    
    submodule_data->defines[dnxf_data_port_pll_define_fabric_clock_freq_out_default].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_pll_define_nof_plls_per_pm].name = "nof_plls_per_pm";
    submodule_data->defines[dnxf_data_port_pll_define_nof_plls_per_pm].doc = "Number of PLLs per port macro";
    
    submodule_data->defines[dnxf_data_port_pll_define_nof_plls_per_pm].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_pll_define_nof_config_words].name = "nof_config_words";
    submodule_data->defines[dnxf_data_port_pll_define_nof_config_words].doc = "Number of PLL configuration words";
    
    submodule_data->defines[dnxf_data_port_pll_define_nof_config_words].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnxf_data_port_pll_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port pll tables");

    
    submodule_data->tables[dnxf_data_port_pll_table_info].name = "info";
    submodule_data->tables[dnxf_data_port_pll_table_info].doc = "General PLL info";
    submodule_data->tables[dnxf_data_port_pll_table_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_pll_table_info].size_of_values = sizeof(dnxf_data_port_pll_info_t);
    submodule_data->tables[dnxf_data_port_pll_table_info].entry_get = dnxf_data_port_pll_info_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_pll_table_info].nof_keys = 1;
    submodule_data->tables[dnxf_data_port_pll_table_info].keys[0].name = "pll";
    submodule_data->tables[dnxf_data_port_pll_table_info].keys[0].doc = "PLL ID.";

    
    submodule_data->tables[dnxf_data_port_pll_table_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_port_pll_table_info].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_port_pll_table_info].nof_values, "_dnxf_data_port_pll_table_info table values");
    submodule_data->tables[dnxf_data_port_pll_table_info].values[0].name = "fabric_ref_clk_in";
    submodule_data->tables[dnxf_data_port_pll_table_info].values[0].type = "uint32";
    submodule_data->tables[dnxf_data_port_pll_table_info].values[0].doc = "Shows the value for the fabric PLL input reference clock.";
    submodule_data->tables[dnxf_data_port_pll_table_info].values[0].offset = UTILEX_OFFSETOF(dnxf_data_port_pll_info_t, fabric_ref_clk_in);
    submodule_data->tables[dnxf_data_port_pll_table_info].values[1].name = "fabric_ref_clk_out";
    submodule_data->tables[dnxf_data_port_pll_table_info].values[1].type = "uint32";
    submodule_data->tables[dnxf_data_port_pll_table_info].values[1].doc = "Shows the value for the fabric PLL output reference clock.";
    submodule_data->tables[dnxf_data_port_pll_table_info].values[1].offset = UTILEX_OFFSETOF(dnxf_data_port_pll_info_t, fabric_ref_clk_out);

    
    submodule_data->tables[dnxf_data_port_pll_table_config].name = "config";
    submodule_data->tables[dnxf_data_port_pll_table_config].doc = "PLL configuration";
    submodule_data->tables[dnxf_data_port_pll_table_config].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_pll_table_config].size_of_values = sizeof(dnxf_data_port_pll_config_t);
    submodule_data->tables[dnxf_data_port_pll_table_config].entry_get = dnxf_data_port_pll_config_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_pll_table_config].nof_keys = 2;
    submodule_data->tables[dnxf_data_port_pll_table_config].keys[0].name = "fabric_ref_clk_in";
    submodule_data->tables[dnxf_data_port_pll_table_config].keys[0].doc = "PLL Input Reference Clock";
    submodule_data->tables[dnxf_data_port_pll_table_config].keys[1].name = "fabric_ref_clk_out";
    submodule_data->tables[dnxf_data_port_pll_table_config].keys[1].doc = "PLL Output Reference Clock";

    
    submodule_data->tables[dnxf_data_port_pll_table_config].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_port_pll_table_config].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_port_pll_table_config].nof_values, "_dnxf_data_port_pll_table_config table values");
    submodule_data->tables[dnxf_data_port_pll_table_config].values[0].name = "data";
    submodule_data->tables[dnxf_data_port_pll_table_config].values[0].type = "uint32[DNXF_DATA_MAX_PORT_PLL_NOF_CONFIG_WORDS]";
    submodule_data->tables[dnxf_data_port_pll_table_config].values[0].doc = "PLL configuration words";
    submodule_data->tables[dnxf_data_port_pll_table_config].values[0].offset = UTILEX_OFFSETOF(dnxf_data_port_pll_config_t, data);
    submodule_data->tables[dnxf_data_port_pll_table_config].values[1].name = "valid";
    submodule_data->tables[dnxf_data_port_pll_table_config].values[1].type = "uint32";
    submodule_data->tables[dnxf_data_port_pll_table_config].values[1].doc = "Is configuration valid (boolean)";
    submodule_data->tables[dnxf_data_port_pll_table_config].values[1].offset = UTILEX_OFFSETOF(dnxf_data_port_pll_config_t, valid);


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_port_pll_feature_get(
    int unit,
    dnxf_data_port_pll_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_pll, feature);
}


uint32
dnxf_data_port_pll_nof_lcpll_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_pll, dnxf_data_port_pll_define_nof_lcpll);
}

uint32
dnxf_data_port_pll_nof_port_in_lcpll_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_pll, dnxf_data_port_pll_define_nof_port_in_lcpll);
}

uint32
dnxf_data_port_pll_fabric_clock_freq_in_default_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_pll, dnxf_data_port_pll_define_fabric_clock_freq_in_default);
}

uint32
dnxf_data_port_pll_fabric_clock_freq_out_default_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_pll, dnxf_data_port_pll_define_fabric_clock_freq_out_default);
}

uint32
dnxf_data_port_pll_nof_plls_per_pm_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_pll, dnxf_data_port_pll_define_nof_plls_per_pm);
}

uint32
dnxf_data_port_pll_nof_config_words_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_pll, dnxf_data_port_pll_define_nof_config_words);
}



const dnxf_data_port_pll_info_t *
dnxf_data_port_pll_info_get(
    int unit,
    int pll)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_pll, dnxf_data_port_pll_table_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, pll, 0);
    return (const dnxf_data_port_pll_info_t *) data;

}

const dnxf_data_port_pll_config_t *
dnxf_data_port_pll_config_get(
    int unit,
    int fabric_ref_clk_in,
    int fabric_ref_clk_out)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_pll, dnxf_data_port_pll_table_config);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, fabric_ref_clk_in, fabric_ref_clk_out);
    return (const dnxf_data_port_pll_config_t *) data;

}


shr_error_e
dnxf_data_port_pll_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_port_pll_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_pll, dnxf_data_port_pll_table_info);
    data = (const dnxf_data_port_pll_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fabric_ref_clk_in);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fabric_ref_clk_out);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_port_pll_config_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_port_pll_config_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_pll, dnxf_data_port_pll_table_config);
    data = (const dnxf_data_port_pll_config_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNXF_DATA_MAX_PORT_PLL_NOF_CONFIG_WORDS, data->data);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnxf_data_port_pll_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_pll, dnxf_data_port_pll_table_info);

}

const dnxc_data_table_info_t *
dnxf_data_port_pll_config_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_pll, dnxf_data_port_pll_table_config);

}






static shr_error_e
dnxf_data_port_synce_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "synce";
    submodule_data->doc = "General SYNCE attributes.";
    
    submodule_data->nof_features = _dnxf_data_port_synce_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data port synce features");

    
    submodule_data->nof_defines = _dnxf_data_port_synce_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data port synce defines");

    
    submodule_data->nof_tables = _dnxf_data_port_synce_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port synce tables");

    
    submodule_data->tables[dnxf_data_port_synce_table_cfg].name = "cfg";
    submodule_data->tables[dnxf_data_port_synce_table_cfg].doc = "Fabric Sync Ethernet information.";
    submodule_data->tables[dnxf_data_port_synce_table_cfg].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_synce_table_cfg].size_of_values = sizeof(dnxf_data_port_synce_cfg_t);
    submodule_data->tables[dnxf_data_port_synce_table_cfg].entry_get = dnxf_data_port_synce_cfg_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_synce_table_cfg].nof_keys = 1;
    submodule_data->tables[dnxf_data_port_synce_table_cfg].keys[0].name = "synce_index";
    submodule_data->tables[dnxf_data_port_synce_table_cfg].keys[0].doc = "SYNCE clock index.";

    
    submodule_data->tables[dnxf_data_port_synce_table_cfg].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_port_synce_table_cfg].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_port_synce_table_cfg].nof_values, "_dnxf_data_port_synce_table_cfg table values");
    submodule_data->tables[dnxf_data_port_synce_table_cfg].values[0].name = "source_clock_port";
    submodule_data->tables[dnxf_data_port_synce_table_cfg].values[0].type = "int";
    submodule_data->tables[dnxf_data_port_synce_table_cfg].values[0].doc = "Specifies which is the source clock port";
    submodule_data->tables[dnxf_data_port_synce_table_cfg].values[0].offset = UTILEX_OFFSETOF(dnxf_data_port_synce_cfg_t, source_clock_port);
    submodule_data->tables[dnxf_data_port_synce_table_cfg].values[1].name = "source_clock_divider";
    submodule_data->tables[dnxf_data_port_synce_table_cfg].values[1].type = "uint32";
    submodule_data->tables[dnxf_data_port_synce_table_cfg].values[1].doc = "Specifiers the clock divider";
    submodule_data->tables[dnxf_data_port_synce_table_cfg].values[1].offset = UTILEX_OFFSETOF(dnxf_data_port_synce_cfg_t, source_clock_divider);


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_port_synce_feature_get(
    int unit,
    dnxf_data_port_synce_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_synce, feature);
}




const dnxf_data_port_synce_cfg_t *
dnxf_data_port_synce_cfg_get(
    int unit,
    int synce_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_synce, dnxf_data_port_synce_table_cfg);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, synce_index, 0);
    return (const dnxf_data_port_synce_cfg_t *) data;

}


shr_error_e
dnxf_data_port_synce_cfg_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_port_synce_cfg_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_synce, dnxf_data_port_synce_table_cfg);
    data = (const dnxf_data_port_synce_cfg_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->source_clock_port);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->source_clock_divider);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnxf_data_port_synce_cfg_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_synce, dnxf_data_port_synce_table_cfg);

}






static shr_error_e
dnxf_data_port_retimer_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "retimer";
    submodule_data->doc = "retimer database";
    
    submodule_data->nof_features = _dnxf_data_port_retimer_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data port retimer features");

    submodule_data->features[dnxf_data_port_retimer_is_supported].name = "is_supported";
    submodule_data->features[dnxf_data_port_retimer_is_supported].doc = "Shows if retimer is supported";
    submodule_data->features[dnxf_data_port_retimer_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnxf_data_port_retimer_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data port retimer defines");

    
    submodule_data->nof_tables = _dnxf_data_port_retimer_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port retimer tables");

    
    submodule_data->tables[dnxf_data_port_retimer_table_links_connection].name = "links_connection";
    submodule_data->tables[dnxf_data_port_retimer_table_links_connection].doc = "Link pairs for pass-through retimer";
    submodule_data->tables[dnxf_data_port_retimer_table_links_connection].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_retimer_table_links_connection].size_of_values = sizeof(dnxf_data_port_retimer_links_connection_t);
    submodule_data->tables[dnxf_data_port_retimer_table_links_connection].entry_get = dnxf_data_port_retimer_links_connection_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_retimer_table_links_connection].nof_keys = 1;
    submodule_data->tables[dnxf_data_port_retimer_table_links_connection].keys[0].name = "link";
    submodule_data->tables[dnxf_data_port_retimer_table_links_connection].keys[0].doc = "link id";

    
    submodule_data->tables[dnxf_data_port_retimer_table_links_connection].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_port_retimer_table_links_connection].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_port_retimer_table_links_connection].nof_values, "_dnxf_data_port_retimer_table_links_connection table values");
    submodule_data->tables[dnxf_data_port_retimer_table_links_connection].values[0].name = "connected_link";
    submodule_data->tables[dnxf_data_port_retimer_table_links_connection].values[0].type = "soc_port_t";
    submodule_data->tables[dnxf_data_port_retimer_table_links_connection].values[0].doc = "the connected link id";
    submodule_data->tables[dnxf_data_port_retimer_table_links_connection].values[0].offset = UTILEX_OFFSETOF(dnxf_data_port_retimer_links_connection_t, connected_link);


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_port_retimer_feature_get(
    int unit,
    dnxf_data_port_retimer_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_retimer, feature);
}




const dnxf_data_port_retimer_links_connection_t *
dnxf_data_port_retimer_links_connection_get(
    int unit,
    int link)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_retimer, dnxf_data_port_retimer_table_links_connection);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, link, 0);
    return (const dnxf_data_port_retimer_links_connection_t *) data;

}


shr_error_e
dnxf_data_port_retimer_links_connection_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_port_retimer_links_connection_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_retimer, dnxf_data_port_retimer_table_links_connection);
    data = (const dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->connected_link);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnxf_data_port_retimer_links_connection_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_retimer, dnxf_data_port_retimer_table_links_connection);

}






static shr_error_e
dnxf_data_port_features_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "features";
    submodule_data->doc = "Per devices features";
    
    submodule_data->nof_features = _dnxf_data_port_features_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data port features features");

    submodule_data->features[dnxf_data_port_features_isolation_needed_before_disable].name = "isolation_needed_before_disable";
    submodule_data->features[dnxf_data_port_features_isolation_needed_before_disable].doc = "";
    submodule_data->features[dnxf_data_port_features_isolation_needed_before_disable].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnxf_data_port_features_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data port features defines");

    
    submodule_data->nof_tables = _dnxf_data_port_features_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port features tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_port_features_feature_get(
    int unit,
    dnxf_data_port_features_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_features, feature);
}








shr_error_e
dnxf_data_port_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "port";
    module_data->nof_submodules = _dnxf_data_port_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data port submodules");

    
    SHR_IF_ERR_EXIT(dnxf_data_port_general_init(unit, &module_data->submodules[dnxf_data_port_submodule_general]));
    SHR_IF_ERR_EXIT(dnxf_data_port_stat_init(unit, &module_data->submodules[dnxf_data_port_submodule_stat]));
    SHR_IF_ERR_EXIT(dnxf_data_port_static_add_init(unit, &module_data->submodules[dnxf_data_port_submodule_static_add]));
    SHR_IF_ERR_EXIT(dnxf_data_port_lane_map_init(unit, &module_data->submodules[dnxf_data_port_submodule_lane_map]));
    SHR_IF_ERR_EXIT(dnxf_data_port_pll_init(unit, &module_data->submodules[dnxf_data_port_submodule_pll]));
    SHR_IF_ERR_EXIT(dnxf_data_port_synce_init(unit, &module_data->submodules[dnxf_data_port_submodule_synce]));
    SHR_IF_ERR_EXIT(dnxf_data_port_retimer_init(unit, &module_data->submodules[dnxf_data_port_submodule_retimer]));
    SHR_IF_ERR_EXIT(dnxf_data_port_features_init(unit, &module_data->submodules[dnxf_data_port_submodule_features]));
    
    if (dnxc_data_mgmt_is_ramon(unit))
    {
        SHR_IF_ERR_EXIT(ramon_a0_data_port_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

