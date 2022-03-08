
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_port.h>



#ifdef BCM_DNXF1_SUPPORT

extern shr_error_e ramon_a0_data_port_attach(
    int unit);

#endif 



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
    submodule_data->features[dnxf_data_port_general_mac_cell_shaper_supported].labels[0] = "data_notrev";
    submodule_data->features[dnxf_data_port_general_mac_cell_shaper_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_port_general_bw_update].name = "bw_update";
    submodule_data->features[dnxf_data_port_general_bw_update].doc = "should serdes bandwidth be updated";
    submodule_data->features[dnxf_data_port_general_bw_update].labels[0] = "data_notrev";
    submodule_data->features[dnxf_data_port_general_bw_update].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_port_general_is_cable_swap_supported].name = "is_cable_swap_supported";
    submodule_data->features[dnxf_data_port_general_is_cable_swap_supported].doc = "Indicates if device supports cable swap";
    submodule_data->features[dnxf_data_port_general_is_cable_swap_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_port_general_linkup_indication_map_supported].name = "linkup_indication_map_supported";
    submodule_data->features[dnxf_data_port_general_linkup_indication_map_supported].doc = "Indicates if link up signal source selection is supported";
    submodule_data->features[dnxf_data_port_general_linkup_indication_map_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_port_general_is_connection_to_repeater_supported].name = "is_connection_to_repeater_supported";
    submodule_data->features[dnxf_data_port_general_is_connection_to_repeater_supported].doc = "Indicates if device can be connected to repeater device(FE3200 for example)";
    submodule_data->features[dnxf_data_port_general_is_connection_to_repeater_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_port_general_fmac_tx_padding_supported].name = "fmac_tx_padding_supported";
    submodule_data->features[dnxf_data_port_general_fmac_tx_padding_supported].doc = "Does device support FMAC TX padding";
    submodule_data->features[dnxf_data_port_general_fmac_tx_padding_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_port_general_link_local_remote_fault_support].name = "link_local_remote_fault_support";
    submodule_data->features[dnxf_data_port_general_link_local_remote_fault_support].doc = "";
    submodule_data->features[dnxf_data_port_general_link_local_remote_fault_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_port_general_fmac_only_disable_supported].name = "fmac_only_disable_supported";
    submodule_data->features[dnxf_data_port_general_fmac_only_disable_supported].doc = "";
    submodule_data->features[dnxf_data_port_general_fmac_only_disable_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_port_general_max_speed_api_supported].name = "max_speed_api_supported";
    submodule_data->features[dnxf_data_port_general_max_speed_api_supported].doc = "";
    submodule_data->features[dnxf_data_port_general_max_speed_api_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_port_general_fec_ber_proj_supported].name = "fec_ber_proj_supported";
    submodule_data->features[dnxf_data_port_general_fec_ber_proj_supported].doc = "";
    submodule_data->features[dnxf_data_port_general_fec_ber_proj_supported].flags |= DNXC_DATA_F_FEATURE;

    
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
    submodule_data->defines[dnxf_data_port_general_define_max_bucket_fill_rate].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_port_general_define_max_bucket_fill_rate].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_general_define_link_mask].name = "link_mask";
    submodule_data->defines[dnxf_data_port_general_define_link_mask].doc = "Mask adjusted to number of links.";
    submodule_data->defines[dnxf_data_port_general_define_link_mask].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_port_general_define_link_mask].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_general_define_aldwp_max_val].name = "aldwp_max_val";
    submodule_data->defines[dnxf_data_port_general_define_aldwp_max_val].doc = "Maximum value of ALDWP.";
    submodule_data->defines[dnxf_data_port_general_define_aldwp_max_val].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_port_general_define_aldwp_max_val].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_general_define_fmac_clock_khz].name = "fmac_clock_khz";
    submodule_data->defines[dnxf_data_port_general_define_fmac_clock_khz].doc = "FMAC clock frequency [KHz]";
    submodule_data->defines[dnxf_data_port_general_define_fmac_clock_khz].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_port_general_define_fmac_clock_khz].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_general_define_speed_max].name = "speed_max";
    submodule_data->defines[dnxf_data_port_general_define_speed_max].doc = "Maximum speed of a port";
    
    submodule_data->defines[dnxf_data_port_general_define_speed_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_general_define_fmac_tx_padding_min_port_speed].name = "fmac_tx_padding_min_port_speed";
    submodule_data->defines[dnxf_data_port_general_define_fmac_tx_padding_min_port_speed].doc = "Minimum port speed requred to enable FMAC Tx Padding";
    
    submodule_data->defines[dnxf_data_port_general_define_fmac_tx_padding_min_port_speed].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_general_define_nof_ber_proj_error_analyzer_counters].name = "nof_ber_proj_error_analyzer_counters";
    submodule_data->defines[dnxf_data_port_general_define_nof_ber_proj_error_analyzer_counters].doc = "Number of histogtam counters for BER Projection";
    
    submodule_data->defines[dnxf_data_port_general_define_nof_ber_proj_error_analyzer_counters].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_general_define_fw_load_method].name = "fw_load_method";
    submodule_data->defines[dnxf_data_port_general_define_fw_load_method].doc = "The method of the firmware load.";
    submodule_data->defines[dnxf_data_port_general_define_fw_load_method].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_port_general_define_fw_load_method].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_port_general_define_fw_crc_check].name = "fw_crc_check";
    submodule_data->defines[dnxf_data_port_general_define_fw_crc_check].doc = "Enable firmware CRC check.";
    submodule_data->defines[dnxf_data_port_general_define_fw_crc_check].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_port_general_define_fw_crc_check].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_port_general_define_fw_load_verify].name = "fw_load_verify";
    submodule_data->defines[dnxf_data_port_general_define_fw_load_verify].doc = "Enable firmware load verification.";
    submodule_data->defines[dnxf_data_port_general_define_fw_load_verify].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_port_general_define_fw_load_verify].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_port_general_define_mac_bucket_fill_rate].name = "mac_bucket_fill_rate";
    submodule_data->defines[dnxf_data_port_general_define_mac_bucket_fill_rate].doc = "Mac bucket fill rate.";
    submodule_data->defines[dnxf_data_port_general_define_mac_bucket_fill_rate].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_port_general_define_mac_bucket_fill_rate].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnxf_data_port_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port general tables");

    
    submodule_data->tables[dnxf_data_port_general_table_supported_lanes].name = "supported_lanes";
    submodule_data->tables[dnxf_data_port_general_table_supported_lanes].doc = "bitmap of supported FMAC lanes in device";
    submodule_data->tables[dnxf_data_port_general_table_supported_lanes].labels[0] = "data_notrev";
    submodule_data->tables[dnxf_data_port_general_table_supported_lanes].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_general_table_supported_lanes].size_of_values = sizeof(dnxf_data_port_general_supported_lanes_t);
    submodule_data->tables[dnxf_data_port_general_table_supported_lanes].entry_get = dnxf_data_port_general_supported_lanes_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_general_table_supported_lanes].nof_keys = 0;

    
    submodule_data->tables[dnxf_data_port_general_table_supported_lanes].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_port_general_table_supported_lanes].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_port_general_table_supported_lanes].nof_values, "_dnxf_data_port_general_table_supported_lanes table values");
    submodule_data->tables[dnxf_data_port_general_table_supported_lanes].values[0].name = "pbmp";
    submodule_data->tables[dnxf_data_port_general_table_supported_lanes].values[0].type = "bcm_pbmp_t";
    submodule_data->tables[dnxf_data_port_general_table_supported_lanes].values[0].doc = "bitmap of supported FMAC lanes in device";
    submodule_data->tables[dnxf_data_port_general_table_supported_lanes].values[0].offset = UTILEX_OFFSETOF(dnxf_data_port_general_supported_lanes_t, pbmp);

    
    submodule_data->tables[dnxf_data_port_general_table_fmac_bus_size].name = "fmac_bus_size";
    submodule_data->tables[dnxf_data_port_general_table_fmac_bus_size].doc = "Fabric MAC bus size [bits]";
    submodule_data->tables[dnxf_data_port_general_table_fmac_bus_size].labels[0] = "data_notrev";
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

    
    submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].name = "supported_interfaces";
    submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].doc = "table with all supported by the device speeds";
    submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].size_of_values = sizeof(dnxf_data_port_general_supported_interfaces_t);
    submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].entry_get = dnxf_data_port_general_supported_interfaces_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].nof_keys = 1;
    submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].keys[0].name = "index";
    submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].keys[0].doc = "entry index";

    
    submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].nof_values, "_dnxf_data_port_general_table_supported_interfaces table values");
    submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].values[0].name = "speed";
    submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].values[0].type = "uint32";
    submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].values[0].doc = "Fabric lane serdes speed.";
    submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].values[0].offset = UTILEX_OFFSETOF(dnxf_data_port_general_supported_interfaces_t, speed);
    submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].values[1].name = "fec_type";
    submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].values[1].type = "bcm_port_phy_fec_t";
    submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].values[1].doc = "fec type";
    submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].values[1].offset = UTILEX_OFFSETOF(dnxf_data_port_general_supported_interfaces_t, fec_type);
    submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].values[2].name = "is_valid";
    submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].values[2].type = "uint32";
    submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].values[2].doc = "Used to filter the valid table entries per device.";
    submodule_data->tables[dnxf_data_port_general_table_supported_interfaces].values[2].offset = UTILEX_OFFSETOF(dnxf_data_port_general_supported_interfaces_t, is_valid);

    
    submodule_data->tables[dnxf_data_port_general_table_default_fec].name = "default_fec";
    submodule_data->tables[dnxf_data_port_general_table_default_fec].doc = "table with the default FEC type per speed";
    submodule_data->tables[dnxf_data_port_general_table_default_fec].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_general_table_default_fec].size_of_values = sizeof(dnxf_data_port_general_default_fec_t);
    submodule_data->tables[dnxf_data_port_general_table_default_fec].entry_get = dnxf_data_port_general_default_fec_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_general_table_default_fec].nof_keys = 1;
    submodule_data->tables[dnxf_data_port_general_table_default_fec].keys[0].name = "speed";
    submodule_data->tables[dnxf_data_port_general_table_default_fec].keys[0].doc = "Fabric lane serdes speed.";

    
    submodule_data->tables[dnxf_data_port_general_table_default_fec].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_port_general_table_default_fec].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_port_general_table_default_fec].nof_values, "_dnxf_data_port_general_table_default_fec table values");
    submodule_data->tables[dnxf_data_port_general_table_default_fec].values[0].name = "fec_type";
    submodule_data->tables[dnxf_data_port_general_table_default_fec].values[0].type = "bcm_port_phy_fec_t";
    submodule_data->tables[dnxf_data_port_general_table_default_fec].values[0].doc = "default fec type of that speed";
    submodule_data->tables[dnxf_data_port_general_table_default_fec].values[0].offset = UTILEX_OFFSETOF(dnxf_data_port_general_default_fec_t, fec_type);

    
    submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].name = "supported_fec_controls";
    submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].doc = "table with supported combinations of FEC type and FEC controls";
    submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].size_of_values = sizeof(dnxf_data_port_general_supported_fec_controls_t);
    submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].entry_get = dnxf_data_port_general_supported_fec_controls_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].nof_keys = 1;
    submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].keys[0].name = "index";
    submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].keys[0].doc = "entry_index";

    
    submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].nof_values, "_dnxf_data_port_general_table_supported_fec_controls table values");
    submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].values[0].name = "fec_type";
    submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].values[0].type = "bcm_port_phy_fec_t";
    submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].values[0].doc = "fec type supported by device";
    submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].values[0].offset = UTILEX_OFFSETOF(dnxf_data_port_general_supported_fec_controls_t, fec_type);
    submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].values[1].name = "control_type";
    submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].values[1].type = "bcm_port_control_t";
    submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].values[1].doc = "control type for the corresponding fec";
    submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].values[1].offset = UTILEX_OFFSETOF(dnxf_data_port_general_supported_fec_controls_t, control_type);
    submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].values[2].name = "is_valid";
    submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].values[2].type = "uint32";
    submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].values[2].doc = "Used to filter the valid table entries per device.";
    submodule_data->tables[dnxf_data_port_general_table_supported_fec_controls].values[2].offset = UTILEX_OFFSETOF(dnxf_data_port_general_supported_fec_controls_t, is_valid);

    
    submodule_data->tables[dnxf_data_port_general_table_supported_fmac_tx_padding_sizes].name = "supported_fmac_tx_padding_sizes";
    submodule_data->tables[dnxf_data_port_general_table_supported_fmac_tx_padding_sizes].doc = "table with all supported FMAC Tx padding sizes";
    submodule_data->tables[dnxf_data_port_general_table_supported_fmac_tx_padding_sizes].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_general_table_supported_fmac_tx_padding_sizes].size_of_values = sizeof(dnxf_data_port_general_supported_fmac_tx_padding_sizes_t);
    submodule_data->tables[dnxf_data_port_general_table_supported_fmac_tx_padding_sizes].entry_get = dnxf_data_port_general_supported_fmac_tx_padding_sizes_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_general_table_supported_fmac_tx_padding_sizes].nof_keys = 1;
    submodule_data->tables[dnxf_data_port_general_table_supported_fmac_tx_padding_sizes].keys[0].name = "index";
    submodule_data->tables[dnxf_data_port_general_table_supported_fmac_tx_padding_sizes].keys[0].doc = "FMAC Tx Padding size";

    
    submodule_data->tables[dnxf_data_port_general_table_supported_fmac_tx_padding_sizes].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_port_general_table_supported_fmac_tx_padding_sizes].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_port_general_table_supported_fmac_tx_padding_sizes].nof_values, "_dnxf_data_port_general_table_supported_fmac_tx_padding_sizes table values");
    submodule_data->tables[dnxf_data_port_general_table_supported_fmac_tx_padding_sizes].values[0].name = "size";
    submodule_data->tables[dnxf_data_port_general_table_supported_fmac_tx_padding_sizes].values[0].type = "uint32";
    submodule_data->tables[dnxf_data_port_general_table_supported_fmac_tx_padding_sizes].values[0].doc = "Supported FMAC Tx Padding sizes";
    submodule_data->tables[dnxf_data_port_general_table_supported_fmac_tx_padding_sizes].values[0].offset = UTILEX_OFFSETOF(dnxf_data_port_general_supported_fmac_tx_padding_sizes_t, size);


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
dnxf_data_port_general_fmac_tx_padding_min_port_speed_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_define_fmac_tx_padding_min_port_speed);
}

uint32
dnxf_data_port_general_nof_ber_proj_error_analyzer_counters_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_define_nof_ber_proj_error_analyzer_counters);
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



const dnxf_data_port_general_supported_lanes_t *
dnxf_data_port_general_supported_lanes_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_supported_lanes);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnxf_data_port_general_supported_lanes_t *) data;

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

const dnxf_data_port_general_supported_interfaces_t *
dnxf_data_port_general_supported_interfaces_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_supported_interfaces);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnxf_data_port_general_supported_interfaces_t *) data;

}

const dnxf_data_port_general_default_fec_t *
dnxf_data_port_general_default_fec_get(
    int unit,
    int speed)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_default_fec);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, speed, 0);
    return (const dnxf_data_port_general_default_fec_t *) data;

}

const dnxf_data_port_general_supported_fec_controls_t *
dnxf_data_port_general_supported_fec_controls_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_supported_fec_controls);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnxf_data_port_general_supported_fec_controls_t *) data;

}

const dnxf_data_port_general_supported_fmac_tx_padding_sizes_t *
dnxf_data_port_general_supported_fmac_tx_padding_sizes_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_supported_fmac_tx_padding_sizes);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnxf_data_port_general_supported_fmac_tx_padding_sizes_t *) data;

}


shr_error_e
dnxf_data_port_general_supported_lanes_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_port_general_supported_lanes_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_supported_lanes);
    data = (const dnxf_data_port_general_supported_lanes_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
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

shr_error_e
dnxf_data_port_general_supported_interfaces_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_port_general_supported_interfaces_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_supported_interfaces);
    data = (const dnxf_data_port_general_supported_interfaces_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->speed);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fec_type);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_port_general_default_fec_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_port_general_default_fec_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_default_fec);
    data = (const dnxf_data_port_general_default_fec_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fec_type);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_port_general_supported_fec_controls_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_port_general_supported_fec_controls_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_supported_fec_controls);
    data = (const dnxf_data_port_general_supported_fec_controls_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fec_type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->control_type);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_port_general_supported_fmac_tx_padding_sizes_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_port_general_supported_fmac_tx_padding_sizes_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_supported_fmac_tx_padding_sizes);
    data = (const dnxf_data_port_general_supported_fmac_tx_padding_sizes_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->size);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnxf_data_port_general_supported_lanes_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_supported_lanes);

}

const dnxc_data_table_info_t *
dnxf_data_port_general_fmac_bus_size_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_fmac_bus_size);

}

const dnxc_data_table_info_t *
dnxf_data_port_general_supported_interfaces_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_supported_interfaces);

}

const dnxc_data_table_info_t *
dnxf_data_port_general_default_fec_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_default_fec);

}

const dnxc_data_table_info_t *
dnxf_data_port_general_supported_fec_controls_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_supported_fec_controls);

}

const dnxc_data_table_info_t *
dnxf_data_port_general_supported_fmac_tx_padding_sizes_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_general, dnxf_data_port_general_table_supported_fmac_tx_padding_sizes);

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
    submodule_data->defines[dnxf_data_port_stat_define_thread_interval].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_port_stat_define_thread_interval].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_port_stat_define_thread_flags].name = "thread_flags";
    submodule_data->defines[dnxf_data_port_stat_define_thread_flags].doc = "Statistics thread flags.";
    submodule_data->defines[dnxf_data_port_stat_define_thread_flags].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_port_stat_define_thread_flags].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_port_stat_define_thread_counter_pri].name = "thread_counter_pri";
    submodule_data->defines[dnxf_data_port_stat_define_thread_counter_pri].doc = "Counter thread priority.";
    submodule_data->defines[dnxf_data_port_stat_define_thread_counter_pri].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_port_stat_define_thread_counter_pri].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_port_stat_define_thread_timeout].name = "thread_timeout";
    submodule_data->defines[dnxf_data_port_stat_define_thread_timeout].doc = "Statistics thread sync operation timeout.";
    submodule_data->defines[dnxf_data_port_stat_define_thread_timeout].labels[0] = "data_notrev";
    
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
dnxf_data_port_stat_thread_flags_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_stat, dnxf_data_port_stat_define_thread_flags);
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

    submodule_data->features[dnxf_data_port_static_add_serdes_tx_taps_pre3_supported_instead_post3].name = "serdes_tx_taps_pre3_supported_instead_post3";
    submodule_data->features[dnxf_data_port_static_add_serdes_tx_taps_pre3_supported_instead_post3].doc = "Indicate if serdes taps parameter pre3 is supported instead of post3";
    submodule_data->features[dnxf_data_port_static_add_serdes_tx_taps_pre3_supported_instead_post3].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnxf_data_port_static_add_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data port static_add defines");

    
    submodule_data->nof_tables = _dnxf_data_port_static_add_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port static_add tables");

    
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].name = "port_info";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].doc = "General port info";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].labels[0] = "data_notrev";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].size_of_values = sizeof(dnxf_data_port_static_add_port_info_t);
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].entry_get = dnxf_data_port_static_add_port_info_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].nof_keys = 1;
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].keys[0].name = "port";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].keys[0].doc = "Link ID.";

    
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].nof_values = 9;
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
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[8].name = "cell_format";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[8].type = "uint32";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[8].doc = "port cell format";
    submodule_data->tables[dnxf_data_port_static_add_table_port_info].values[8].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_port_info_t, cell_format);

    
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].name = "serdes_lane_config";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].doc = "serdes lane configurations, related to firmware";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_lane_config].labels[0] = "data_notrev";
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
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].labels[0] = "data_notrev";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].size_of_values = sizeof(dnxf_data_port_static_add_serdes_tx_taps_t);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].entry_get = dnxf_data_port_static_add_serdes_tx_taps_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].nof_keys = 1;
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].keys[0].name = "port";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].keys[0].doc = "Link ID.";

    
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].nof_values = 9;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].nof_values, "_dnxf_data_port_static_add_table_serdes_tx_taps table values");
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[0].name = "pre3";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[0].type = "int";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[0].doc = "pre3";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[0].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_tx_taps_t, pre3);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[1].name = "pre2";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[1].type = "int";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[1].doc = "pre2";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[1].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_tx_taps_t, pre2);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[2].name = "pre";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[2].type = "int";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[2].doc = "pre";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[2].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_tx_taps_t, pre);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[3].name = "main";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[3].type = "int";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[3].doc = "main";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[3].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_tx_taps_t, main);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[4].name = "post";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[4].type = "int";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[4].doc = "post";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[4].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_tx_taps_t, post);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[5].name = "post2";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[5].type = "int";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[5].doc = "post2";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[5].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_tx_taps_t, post2);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[6].name = "post3";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[6].type = "int";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[6].doc = "post3";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[6].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_tx_taps_t, post3);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[7].name = "tx_tap_mode";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[7].type = "bcm_port_phy_tx_tap_mode_t";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[7].doc = "tx tap mode - 3 or 6 taps";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[7].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_tx_taps_t, tx_tap_mode);
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[8].name = "signalling_mode";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[8].type = "bcm_port_phy_signalling_mode_t";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[8].doc = "signalling mode - pam4 or nrz";
    submodule_data->tables[dnxf_data_port_static_add_table_serdes_tx_taps].values[8].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_serdes_tx_taps_t, signalling_mode);

    
    submodule_data->tables[dnxf_data_port_static_add_table_quad_info].name = "quad_info";
    submodule_data->tables[dnxf_data_port_static_add_table_quad_info].doc = "General quad info";
    submodule_data->tables[dnxf_data_port_static_add_table_quad_info].labels[0] = "data_notrev";
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

    
    submodule_data->tables[dnxf_data_port_static_add_table_cable_swap_info].name = "cable_swap_info";
    submodule_data->tables[dnxf_data_port_static_add_table_cable_swap_info].doc = "Cable swap info";
    submodule_data->tables[dnxf_data_port_static_add_table_cable_swap_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_static_add_table_cable_swap_info].size_of_values = sizeof(dnxf_data_port_static_add_cable_swap_info_t);
    submodule_data->tables[dnxf_data_port_static_add_table_cable_swap_info].entry_get = dnxf_data_port_static_add_cable_swap_info_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_static_add_table_cable_swap_info].nof_keys = 1;
    submodule_data->tables[dnxf_data_port_static_add_table_cable_swap_info].keys[0].name = "pm";
    submodule_data->tables[dnxf_data_port_static_add_table_cable_swap_info].keys[0].doc = "PM ID";

    
    submodule_data->tables[dnxf_data_port_static_add_table_cable_swap_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_port_static_add_table_cable_swap_info].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_port_static_add_table_cable_swap_info].nof_values, "_dnxf_data_port_static_add_table_cable_swap_info table values");
    submodule_data->tables[dnxf_data_port_static_add_table_cable_swap_info].values[0].name = "enable";
    submodule_data->tables[dnxf_data_port_static_add_table_cable_swap_info].values[0].type = "int";
    submodule_data->tables[dnxf_data_port_static_add_table_cable_swap_info].values[0].doc = "Cable swap enable configuration";
    submodule_data->tables[dnxf_data_port_static_add_table_cable_swap_info].values[0].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_cable_swap_info_t, enable);
    submodule_data->tables[dnxf_data_port_static_add_table_cable_swap_info].values[1].name = "master";
    submodule_data->tables[dnxf_data_port_static_add_table_cable_swap_info].values[1].type = "int";
    submodule_data->tables[dnxf_data_port_static_add_table_cable_swap_info].values[1].doc = "Cable swap master/slave configuration";
    submodule_data->tables[dnxf_data_port_static_add_table_cable_swap_info].values[1].offset = UTILEX_OFFSETOF(dnxf_data_port_static_add_cable_swap_info_t, master);


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

const dnxf_data_port_static_add_cable_swap_info_t *
dnxf_data_port_static_add_cable_swap_info_get(
    int unit,
    int pm)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_static_add, dnxf_data_port_static_add_table_cable_swap_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, pm, 0);
    return (const dnxf_data_port_static_add_cable_swap_info_t *) data;

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
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->cell_format);
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
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pre3);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pre2);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pre);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->main);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->post);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->post2);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->post3);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tx_tap_mode);
            break;
        case 8:
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

shr_error_e
dnxf_data_port_static_add_cable_swap_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_port_static_add_cable_swap_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_static_add, dnxf_data_port_static_add_table_cable_swap_info);
    data = (const dnxf_data_port_static_add_cable_swap_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->enable);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->master);
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

const dnxc_data_table_info_t *
dnxf_data_port_static_add_cable_swap_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_static_add, dnxf_data_port_static_add_table_cable_swap_info);

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

    submodule_data->features[dnxf_data_port_lane_map_is_quad_swap_supported].name = "is_quad_swap_supported";
    submodule_data->features[dnxf_data_port_lane_map_is_quad_swap_supported].doc = "Can lanes from FMAL0 connect to phy4-7, and FMAL1 to lanes 0-3";
    submodule_data->features[dnxf_data_port_lane_map_is_quad_swap_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_port_lane_map_is_flexible_link_mapping_supported].name = "is_flexible_link_mapping_supported";
    submodule_data->features[dnxf_data_port_lane_map_is_flexible_link_mapping_supported].doc = "Indicates that the link to FMAC lane mapping is not 1:1";
    submodule_data->features[dnxf_data_port_lane_map_is_flexible_link_mapping_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnxf_data_port_lane_map_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data port lane_map defines");

    submodule_data->defines[dnxf_data_port_lane_map_define_lane_map_bound].name = "lane_map_bound";
    submodule_data->defines[dnxf_data_port_lane_map_define_lane_map_bound].doc = "Lane swapping is allowed within this nof links.";
    
    submodule_data->defines[dnxf_data_port_lane_map_define_lane_map_bound].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnxf_data_port_lane_map_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port lane_map tables");

    
    submodule_data->tables[dnxf_data_port_lane_map_table_info].name = "info";
    submodule_data->tables[dnxf_data_port_lane_map_table_info].doc = "Shows the DB for the mapping between the lanes and the serdeses.";
    submodule_data->tables[dnxf_data_port_lane_map_table_info].labels[0] = "data_notrev";
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
    submodule_data->defines[dnxf_data_port_pll_define_nof_lcpll].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_port_pll_define_nof_lcpll].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_pll_define_nof_port_in_lcpll].name = "nof_port_in_lcpll";
    submodule_data->defines[dnxf_data_port_pll_define_nof_port_in_lcpll].doc = "Number of ports for each LCPLL.";
    submodule_data->defines[dnxf_data_port_pll_define_nof_port_in_lcpll].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_port_pll_define_nof_port_in_lcpll].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_pll_define_fabric_clock_freq_in_default].name = "fabric_clock_freq_in_default";
    submodule_data->defines[dnxf_data_port_pll_define_fabric_clock_freq_in_default].doc = "Fabric Clock freq in default.";
    submodule_data->defines[dnxf_data_port_pll_define_fabric_clock_freq_in_default].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_port_pll_define_fabric_clock_freq_in_default].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_pll_define_fabric_clock_freq_out_default].name = "fabric_clock_freq_out_default";
    submodule_data->defines[dnxf_data_port_pll_define_fabric_clock_freq_out_default].doc = "Fabric Clock freq out default.";
    submodule_data->defines[dnxf_data_port_pll_define_fabric_clock_freq_out_default].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_port_pll_define_fabric_clock_freq_out_default].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_pll_define_nof_plls_per_pm].name = "nof_plls_per_pm";
    submodule_data->defines[dnxf_data_port_pll_define_nof_plls_per_pm].doc = "Number of PLLs per port macro";
    
    submodule_data->defines[dnxf_data_port_pll_define_nof_plls_per_pm].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_pll_define_nof_config_words].name = "nof_config_words";
    submodule_data->defines[dnxf_data_port_pll_define_nof_config_words].doc = "Number of PLL configuration words";
    submodule_data->defines[dnxf_data_port_pll_define_nof_config_words].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_port_pll_define_nof_config_words].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnxf_data_port_pll_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port pll tables");

    
    submodule_data->tables[dnxf_data_port_pll_table_info].name = "info";
    submodule_data->tables[dnxf_data_port_pll_table_info].doc = "General PLL info";
    submodule_data->tables[dnxf_data_port_pll_table_info].labels[0] = "data_notrev";
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

    
    submodule_data->tables[dnxf_data_port_pll_table_force_single_pll].name = "force_single_pll";
    submodule_data->tables[dnxf_data_port_pll_table_force_single_pll].doc = "";
    submodule_data->tables[dnxf_data_port_pll_table_force_single_pll].labels[0] = "data_notrev";
    submodule_data->tables[dnxf_data_port_pll_table_force_single_pll].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_pll_table_force_single_pll].size_of_values = sizeof(dnxf_data_port_pll_force_single_pll_t);
    submodule_data->tables[dnxf_data_port_pll_table_force_single_pll].entry_get = dnxf_data_port_pll_force_single_pll_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_pll_table_force_single_pll].nof_keys = 1;
    submodule_data->tables[dnxf_data_port_pll_table_force_single_pll].keys[0].name = "pm";
    submodule_data->tables[dnxf_data_port_pll_table_force_single_pll].keys[0].doc = "pm id";

    
    submodule_data->tables[dnxf_data_port_pll_table_force_single_pll].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_port_pll_table_force_single_pll].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_port_pll_table_force_single_pll].nof_values, "_dnxf_data_port_pll_table_force_single_pll table values");
    submodule_data->tables[dnxf_data_port_pll_table_force_single_pll].values[0].name = "enable";
    submodule_data->tables[dnxf_data_port_pll_table_force_single_pll].values[0].type = "int";
    submodule_data->tables[dnxf_data_port_pll_table_force_single_pll].values[0].doc = "is enabled";
    submodule_data->tables[dnxf_data_port_pll_table_force_single_pll].values[0].offset = UTILEX_OFFSETOF(dnxf_data_port_pll_force_single_pll_t, enable);

    
    submodule_data->tables[dnxf_data_port_pll_table_config].name = "config";
    submodule_data->tables[dnxf_data_port_pll_table_config].doc = "PLL configuration";
    submodule_data->tables[dnxf_data_port_pll_table_config].labels[0] = "data_notrev";
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

const dnxf_data_port_pll_force_single_pll_t *
dnxf_data_port_pll_force_single_pll_get(
    int unit,
    int pm)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_pll, dnxf_data_port_pll_table_force_single_pll);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, pm, 0);
    return (const dnxf_data_port_pll_force_single_pll_t *) data;

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
dnxf_data_port_pll_force_single_pll_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_port_pll_force_single_pll_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_pll, dnxf_data_port_pll_table_force_single_pll);
    data = (const dnxf_data_port_pll_force_single_pll_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->enable);
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
dnxf_data_port_pll_force_single_pll_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_pll, dnxf_data_port_pll_table_force_single_pll);

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

    submodule_data->defines[dnxf_data_port_synce_define_fabric_div_min].name = "fabric_div_min";
    submodule_data->defines[dnxf_data_port_synce_define_fabric_div_min].doc = "Min value for the fabric synce output clk.";
    
    submodule_data->defines[dnxf_data_port_synce_define_fabric_div_min].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_port_synce_define_fabric_div_max].name = "fabric_div_max";
    submodule_data->defines[dnxf_data_port_synce_define_fabric_div_max].doc = "Max value for the fabric synce output clk.";
    
    submodule_data->defines[dnxf_data_port_synce_define_fabric_div_max].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnxf_data_port_synce_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port synce tables");

    
    submodule_data->tables[dnxf_data_port_synce_table_cfg].name = "cfg";
    submodule_data->tables[dnxf_data_port_synce_table_cfg].doc = "Fabric Sync Ethernet information.";
    submodule_data->tables[dnxf_data_port_synce_table_cfg].labels[0] = "data_notrev";
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

    
    submodule_data->tables[dnxf_data_port_synce_table_pm_to_ipw_mapping].name = "pm_to_ipw_mapping";
    submodule_data->tables[dnxf_data_port_synce_table_pm_to_ipw_mapping].doc = "serdes to ipw mapping";
    submodule_data->tables[dnxf_data_port_synce_table_pm_to_ipw_mapping].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_synce_table_pm_to_ipw_mapping].size_of_values = sizeof(dnxf_data_port_synce_pm_to_ipw_mapping_t);
    submodule_data->tables[dnxf_data_port_synce_table_pm_to_ipw_mapping].entry_get = dnxf_data_port_synce_pm_to_ipw_mapping_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_synce_table_pm_to_ipw_mapping].nof_keys = 1;
    submodule_data->tables[dnxf_data_port_synce_table_pm_to_ipw_mapping].keys[0].name = "pm_idx";
    submodule_data->tables[dnxf_data_port_synce_table_pm_to_ipw_mapping].keys[0].doc = "pm index";

    
    submodule_data->tables[dnxf_data_port_synce_table_pm_to_ipw_mapping].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_port_synce_table_pm_to_ipw_mapping].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_port_synce_table_pm_to_ipw_mapping].nof_values, "_dnxf_data_port_synce_table_pm_to_ipw_mapping table values");
    submodule_data->tables[dnxf_data_port_synce_table_pm_to_ipw_mapping].values[0].name = "ipw";
    submodule_data->tables[dnxf_data_port_synce_table_pm_to_ipw_mapping].values[0].type = "uint32";
    submodule_data->tables[dnxf_data_port_synce_table_pm_to_ipw_mapping].values[0].doc = "ipw index";
    submodule_data->tables[dnxf_data_port_synce_table_pm_to_ipw_mapping].values[0].offset = UTILEX_OFFSETOF(dnxf_data_port_synce_pm_to_ipw_mapping_t, ipw);

    
    submodule_data->tables[dnxf_data_port_synce_table_ipw_info].name = "ipw_info";
    submodule_data->tables[dnxf_data_port_synce_table_ipw_info].doc = "The first pm index for each ipw";
    submodule_data->tables[dnxf_data_port_synce_table_ipw_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_port_synce_table_ipw_info].size_of_values = sizeof(dnxf_data_port_synce_ipw_info_t);
    submodule_data->tables[dnxf_data_port_synce_table_ipw_info].entry_get = dnxf_data_port_synce_ipw_info_entry_str_get;

    
    submodule_data->tables[dnxf_data_port_synce_table_ipw_info].nof_keys = 1;
    submodule_data->tables[dnxf_data_port_synce_table_ipw_info].keys[0].name = "ipw";
    submodule_data->tables[dnxf_data_port_synce_table_ipw_info].keys[0].doc = "ipw index";

    
    submodule_data->tables[dnxf_data_port_synce_table_ipw_info].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_port_synce_table_ipw_info].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_port_synce_table_ipw_info].nof_values, "_dnxf_data_port_synce_table_ipw_info table values");
    submodule_data->tables[dnxf_data_port_synce_table_ipw_info].values[0].name = "first_pm";
    submodule_data->tables[dnxf_data_port_synce_table_ipw_info].values[0].type = "uint32";
    submodule_data->tables[dnxf_data_port_synce_table_ipw_info].values[0].doc = "pm index";
    submodule_data->tables[dnxf_data_port_synce_table_ipw_info].values[0].offset = UTILEX_OFFSETOF(dnxf_data_port_synce_ipw_info_t, first_pm);


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


uint32
dnxf_data_port_synce_fabric_div_min_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_synce, dnxf_data_port_synce_define_fabric_div_min);
}

uint32
dnxf_data_port_synce_fabric_div_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_synce, dnxf_data_port_synce_define_fabric_div_max);
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

const dnxf_data_port_synce_pm_to_ipw_mapping_t *
dnxf_data_port_synce_pm_to_ipw_mapping_get(
    int unit,
    int pm_idx)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_synce, dnxf_data_port_synce_table_pm_to_ipw_mapping);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, pm_idx, 0);
    return (const dnxf_data_port_synce_pm_to_ipw_mapping_t *) data;

}

const dnxf_data_port_synce_ipw_info_t *
dnxf_data_port_synce_ipw_info_get(
    int unit,
    int ipw)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_synce, dnxf_data_port_synce_table_ipw_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ipw, 0);
    return (const dnxf_data_port_synce_ipw_info_t *) data;

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

shr_error_e
dnxf_data_port_synce_pm_to_ipw_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_port_synce_pm_to_ipw_mapping_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_synce, dnxf_data_port_synce_table_pm_to_ipw_mapping);
    data = (const dnxf_data_port_synce_pm_to_ipw_mapping_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ipw);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_port_synce_ipw_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_port_synce_ipw_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_synce, dnxf_data_port_synce_table_ipw_info);
    data = (const dnxf_data_port_synce_ipw_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->first_pm);
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

const dnxc_data_table_info_t *
dnxf_data_port_synce_pm_to_ipw_mapping_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_synce, dnxf_data_port_synce_table_pm_to_ipw_mapping);

}

const dnxc_data_table_info_t *
dnxf_data_port_synce_ipw_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_synce, dnxf_data_port_synce_table_ipw_info);

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
    submodule_data->tables[dnxf_data_port_retimer_table_links_connection].labels[0] = "data_notrev";
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
dnxf_data_port_regression_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "regression";
    submodule_data->doc = "data needed for testing and regression";
    
    submodule_data->nof_features = _dnxf_data_port_regression_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data port regression features");

    submodule_data->features[dnxf_data_port_regression_is_cell_format_set_supported].name = "is_cell_format_set_supported";
    submodule_data->features[dnxf_data_port_regression_is_cell_format_set_supported].doc = "Is setting cell format supported";
    submodule_data->features[dnxf_data_port_regression_is_cell_format_set_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnxf_data_port_regression_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data port regression defines");

    
    submodule_data->nof_tables = _dnxf_data_port_regression_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port regression tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_port_regression_feature_get(
    int unit,
    dnxf_data_port_regression_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_port, dnxf_data_port_submodule_regression, feature);
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
    SHR_IF_ERR_EXIT(dnxf_data_port_regression_init(unit, &module_data->submodules[dnxf_data_port_submodule_regression]));
    
#ifdef BCM_DNXF1_SUPPORT

    if (dnxc_data_mgmt_is_ramon(unit))
    {
        SHR_IF_ERR_EXIT(ramon_a0_data_port_attach(unit));
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

