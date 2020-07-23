

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_port.h>



extern shr_error_e jr2_a0_data_port_attach(
    int unit);
extern shr_error_e j2c_a0_data_port_attach(
    int unit);
extern shr_error_e q2a_a0_data_port_attach(
    int unit);
extern shr_error_e j2p_a0_data_port_attach(
    int unit);



static shr_error_e
dnx_data_port_static_add_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "static_add";
    submodule_data->doc = "adding port statically";
    
    submodule_data->nof_features = _dnx_data_port_static_add_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data port static_add features");

    
    submodule_data->nof_defines = _dnx_data_port_static_add_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data port static_add defines");

    submodule_data->defines[dnx_data_port_static_add_define_default_speed_for_special_if].name = "default_speed_for_special_if";
    submodule_data->defines[dnx_data_port_static_add_define_default_speed_for_special_if].doc = "default speed in kbps for special interfaces (tm ports excluding NIF)";
    
    submodule_data->defines[dnx_data_port_static_add_define_default_speed_for_special_if].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_static_add_define_fabric_fw_load_method].name = "fabric_fw_load_method";
    submodule_data->defines[dnx_data_port_static_add_define_fabric_fw_load_method].doc = "Method of the Fabric firmware load";
    
    submodule_data->defines[dnx_data_port_static_add_define_fabric_fw_load_method].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_static_add_define_fabric_fw_crc_check].name = "fabric_fw_crc_check";
    submodule_data->defines[dnx_data_port_static_add_define_fabric_fw_crc_check].doc = "Enable fabric firmware CRC check.";
    
    submodule_data->defines[dnx_data_port_static_add_define_fabric_fw_crc_check].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_static_add_define_fabric_fw_load_verify].name = "fabric_fw_load_verify";
    submodule_data->defines[dnx_data_port_static_add_define_fabric_fw_load_verify].doc = "Enable fabric firmware load verification.";
    
    submodule_data->defines[dnx_data_port_static_add_define_fabric_fw_load_verify].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_static_add_define_nif_fw_load_method].name = "nif_fw_load_method";
    submodule_data->defines[dnx_data_port_static_add_define_nif_fw_load_method].doc = "Method of the Nif firmware load";
    
    submodule_data->defines[dnx_data_port_static_add_define_nif_fw_load_method].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_static_add_define_nif_fw_crc_check].name = "nif_fw_crc_check";
    submodule_data->defines[dnx_data_port_static_add_define_nif_fw_crc_check].doc = "Enable NIF firmware CRC check.";
    
    submodule_data->defines[dnx_data_port_static_add_define_nif_fw_crc_check].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_static_add_define_nif_fw_load_verify].name = "nif_fw_load_verify";
    submodule_data->defines[dnx_data_port_static_add_define_nif_fw_load_verify].doc = "Enable NIF firmware load verification.";
    
    submodule_data->defines[dnx_data_port_static_add_define_nif_fw_load_verify].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_port_static_add_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port static_add tables");

    
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].name = "ucode_port";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].doc = "static port configuration";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].size_of_values = sizeof(dnx_data_port_static_add_ucode_port_t);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].entry_get = dnx_data_port_static_add_ucode_port_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].keys[0].doc = "logical port";

    
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].nof_values = 21;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_ucode_port].nof_values, "_dnx_data_port_static_add_table_ucode_port table values");
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[0].name = "interface";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[0].type = "bcm_port_if_t";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[0].doc = "port interface";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, interface);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[1].name = "nof_lanes";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[1].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[1].doc = "port number of lanes";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, nof_lanes);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[2].name = "interface_offset";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[2].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[2].doc = "interface offset";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[2].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, interface_offset);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[3].name = "core";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[3].type = "bcm_core_t";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[3].doc = "core id";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[3].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, core);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[4].name = "tm_port";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[4].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[4].doc = "tm port";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[4].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, tm_port);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[5].name = "channel";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[5].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[5].doc = "channel id";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[5].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, channel);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[6].name = "is_stif";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[6].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[6].doc = "true if it is statistic interface port";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[6].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_stif);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[7].name = "is_stif_data";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[7].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[7].doc = "true if it is statistic interface and data port (mix)";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[7].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_stif_data);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[8].name = "is_kbp";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[8].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[8].doc = "true if it is kbp port";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[8].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_kbp);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[9].name = "is_cross_connect";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[9].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[9].doc = "true if it is cross connect port";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[9].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_cross_connect);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[10].name = "is_flexe_phy";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[10].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[10].doc = "true if it is FlexE physical port";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[10].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_flexe_phy);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[11].name = "is_ingress_interleave";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[11].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[11].doc = "is interface interleaved in ingress";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[11].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_ingress_interleave);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[12].name = "is_egress_interleave";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[12].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[12].doc = "is port contain egress interleaving";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[12].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_egress_interleave);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[13].name = "is_tdm";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[13].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[13].doc = "is TDM port";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[13].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_tdm);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[14].name = "is_if_tdm_only";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[14].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[14].doc = "is interface includes TDM ports only";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[14].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_if_tdm_only);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[15].name = "is_if_tdm_hybrid";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[15].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[15].doc = "is interface includes both TDM ports anf non TDM ports";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[15].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_if_tdm_hybrid);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[16].name = "sch_priority_propagation_en";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[16].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[16].doc = "is sch priority propagation enabled on this port";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[16].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, sch_priority_propagation_en);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[17].name = "num_priorities";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[17].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[17].doc = "number of port priorities";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[17].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, num_priorities);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[18].name = "base_q_pair";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[18].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[18].doc = "should be used just in case the user need to exlictly pick the base queue pair";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[18].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, base_q_pair);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[19].name = "num_sch_priorities";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[19].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[19].doc = "number of sch port priorities";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[19].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, num_sch_priorities);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[20].name = "base_hr";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[20].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[20].doc = "should be used just in case the user need to exlictly pick the base HR";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[20].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, base_hr);

    
    submodule_data->tables[dnx_data_port_static_add_table_speed].name = "speed";
    submodule_data->tables[dnx_data_port_static_add_table_speed].doc = "per port speed";
    submodule_data->tables[dnx_data_port_static_add_table_speed].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_speed].size_of_values = sizeof(dnx_data_port_static_add_speed_t);
    submodule_data->tables[dnx_data_port_static_add_table_speed].entry_get = dnx_data_port_static_add_speed_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_speed].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_speed].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_speed].keys[0].doc = "logical port number";

    
    submodule_data->tables[dnx_data_port_static_add_table_speed].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_speed].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_speed].nof_values, "_dnx_data_port_static_add_table_speed table values");
    submodule_data->tables[dnx_data_port_static_add_table_speed].values[0].name = "val";
    submodule_data->tables[dnx_data_port_static_add_table_speed].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_speed].values[0].doc = "speed value in mbps";
    submodule_data->tables[dnx_data_port_static_add_table_speed].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_speed_t, val);

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].name = "ext_stat_speed";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].doc = "per external kbp stat port speed";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].size_of_values = sizeof(dnx_data_port_static_add_ext_stat_speed_t);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].entry_get = dnx_data_port_static_add_ext_stat_speed_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].keys[0].name = "ext_stat_port";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].keys[0].doc = "external device (kbp) statistics port number";

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].nof_values, "_dnx_data_port_static_add_table_ext_stat_speed table values");
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].values[0].name = "speed";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].values[0].doc = "speed value in mbps";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_speed_t, speed);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].values[1].name = "nof_lanes";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].values[1].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].values[1].doc = "port number of lanes";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_speed_t, nof_lanes);

    
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].name = "eth_padding";
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].doc = "eth ports padding";
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].size_of_values = sizeof(dnx_data_port_static_add_eth_padding_t);
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].entry_get = dnx_data_port_static_add_eth_padding_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].keys[0].doc = "logical port number";

    
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_eth_padding].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_eth_padding].nof_values, "_dnx_data_port_static_add_table_eth_padding table values");
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].values[0].name = "pad_size";
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].values[0].doc = "packets smaller than min size value are padded with this value";
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_eth_padding_t, pad_size);

    
    submodule_data->tables[dnx_data_port_static_add_table_link_training].name = "link_training";
    submodule_data->tables[dnx_data_port_static_add_table_link_training].doc = "per port link training (CL72)";
    submodule_data->tables[dnx_data_port_static_add_table_link_training].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_link_training].size_of_values = sizeof(dnx_data_port_static_add_link_training_t);
    submodule_data->tables[dnx_data_port_static_add_table_link_training].entry_get = dnx_data_port_static_add_link_training_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_link_training].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_link_training].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_link_training].keys[0].doc = "logical port number";

    
    submodule_data->tables[dnx_data_port_static_add_table_link_training].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_link_training].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_link_training].nof_values, "_dnx_data_port_static_add_table_link_training table values");
    submodule_data->tables[dnx_data_port_static_add_table_link_training].values[0].name = "val";
    submodule_data->tables[dnx_data_port_static_add_table_link_training].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_link_training].values[0].doc = "Is CL72 is enabled";
    submodule_data->tables[dnx_data_port_static_add_table_link_training].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_link_training_t, val);

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].name = "ext_stat_link_training";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].doc = "per external kbp stat port link training (CL72)";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].size_of_values = sizeof(dnx_data_port_static_add_ext_stat_link_training_t);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].entry_get = dnx_data_port_static_add_ext_stat_link_training_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].keys[0].name = "ext_stat_port";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].keys[0].doc = "external device (kbp) statistics port number";

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].nof_values, "_dnx_data_port_static_add_table_ext_stat_link_training table values");
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].values[0].name = "val";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].values[0].doc = "Is CL72 is enabled";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_link_training_t, val);

    
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].name = "fec_type";
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].doc = "per port FEC type";
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].size_of_values = sizeof(dnx_data_port_static_add_fec_type_t);
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].entry_get = dnx_data_port_static_add_fec_type_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].keys[0].doc = "logical port number";

    
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_fec_type].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_fec_type].nof_values, "_dnx_data_port_static_add_table_fec_type table values");
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].values[0].name = "val";
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].values[0].type = "bcm_port_phy_fec_t";
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].values[0].doc = "FEC type";
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_fec_type_t, val);

    
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].name = "serdes_lane_config";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].doc = "serdes lane configurations, related to firmware";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].size_of_values = sizeof(dnx_data_port_static_add_serdes_lane_config_t);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].entry_get = dnx_data_port_static_add_serdes_lane_config_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].keys[0].doc = "logical port number";

    
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].nof_values = 6;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].nof_values, "_dnx_data_port_static_add_table_serdes_lane_config table values");
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[0].name = "dfe";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[0].type = "soc_dnxc_port_dfe_mode_t";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[0].doc = "dfe filter";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_lane_config_t, dfe);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[1].name = "media_type";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[1].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[1].doc = "media type";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_lane_config_t, media_type);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[2].name = "unreliable_los";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[2].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[2].doc = "unreliable los";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[2].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_lane_config_t, unreliable_los);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[3].name = "cl72_auto_polarity_enable";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[3].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[3].doc = "cl72 auto polarity";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[3].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_lane_config_t, cl72_auto_polarity_enable);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[4].name = "cl72_restart_timeout_enable";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[4].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[4].doc = "cl72 restart timeout";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[4].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_lane_config_t, cl72_restart_timeout_enable);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[5].name = "channel_mode";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[5].type = "soc_dnxc_port_channel_mode_t";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[5].doc = "force nr or force er";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[5].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_lane_config_t, channel_mode);

    
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].name = "serdes_tx_taps";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].doc = "PHY TX tap configuration";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].size_of_values = sizeof(dnx_data_port_static_add_serdes_tx_taps_t);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].entry_get = dnx_data_port_static_add_serdes_tx_taps_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].keys[0].doc = "logical port number";

    
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].nof_values = 8;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].nof_values, "_dnx_data_port_static_add_table_serdes_tx_taps table values");
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[0].name = "pre";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[0].doc = "Tx fir pre tap";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_tx_taps_t, pre);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[1].name = "main";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[1].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[1].doc = "Tx fir main tap";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_tx_taps_t, main);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[2].name = "post";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[2].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[2].doc = "Tx fir post tap";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[2].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_tx_taps_t, post);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[3].name = "pre2";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[3].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[3].doc = "Tx fir pre2 tap";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[3].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_tx_taps_t, pre2);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[4].name = "post2";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[4].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[4].doc = "Tx fir post2 tap";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[4].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_tx_taps_t, post2);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[5].name = "post3";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[5].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[5].doc = "Tx fir post3 tap";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[5].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_tx_taps_t, post3);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[6].name = "tx_tap_mode";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[6].type = "bcm_port_phy_tx_tap_mode_t";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[6].doc = "Tx fir tap mode";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[6].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_tx_taps_t, tx_tap_mode);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[7].name = "signalling_mode";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[7].type = "bcm_port_phy_signalling_mode_t";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[7].doc = "Signalling Mode, NRZ or PAM4";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[7].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_tx_taps_t, signalling_mode);

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].name = "ext_stat_global_serdes_tx_taps";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].doc = "PHY TX tap configuration";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].size_of_values = sizeof(dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].entry_get = dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].nof_keys = 0;

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].nof_values = 7;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].nof_values, "_dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps table values");
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[0].name = "pre";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[0].doc = "Tx fir pre tap";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t, pre);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[1].name = "main";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[1].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[1].doc = "Tx fir main tap";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t, main);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[2].name = "post";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[2].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[2].doc = "Tx fir post tap";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[2].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t, post);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[3].name = "pre2";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[3].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[3].doc = "Tx fir pre2 tap";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[3].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t, pre2);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[4].name = "post2";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[4].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[4].doc = "Tx fir post2 tap";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[4].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t, post2);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[5].name = "post3";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[5].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[5].doc = "Tx fir post3 tap";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[5].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t, post3);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[6].name = "txfir_tap_enable";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[6].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[6].doc = "TXFIR Tap Enable Enum";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[6].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t, txfir_tap_enable);

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].name = "ext_stat_lane_serdes_tx_taps";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].doc = "PHY TX tap configuration";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].size_of_values = sizeof(dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].entry_get = dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].keys[0].name = "lane";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].keys[0].doc = "physical lane id";

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].nof_values = 7;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].nof_values, "_dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps table values");
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[0].name = "pre";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[0].doc = "Tx fir pre tap";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t, pre);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[1].name = "main";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[1].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[1].doc = "Tx fir main tap";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t, main);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[2].name = "post";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[2].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[2].doc = "Tx fir post tap";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[2].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t, post);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[3].name = "pre2";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[3].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[3].doc = "Tx fir pre2 tap";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[3].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t, pre2);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[4].name = "post2";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[4].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[4].doc = "Tx fir post2 tap";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[4].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t, post2);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[5].name = "post3";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[5].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[5].doc = "Tx fir post3 tap";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[5].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t, post3);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[6].name = "txfir_tap_enable";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[6].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[6].doc = "TXFIR Tap Enable Enum";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[6].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t, txfir_tap_enable);

    
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].name = "tx_pam4_precoder";
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].doc = "is the precoding enabled on TX side";
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].size_of_values = sizeof(dnx_data_port_static_add_tx_pam4_precoder_t);
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].entry_get = dnx_data_port_static_add_tx_pam4_precoder_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].keys[0].doc = "logical port number";

    
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].nof_values, "_dnx_data_port_static_add_table_tx_pam4_precoder table values");
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].values[0].name = "val";
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].values[0].type = "uint32";
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].values[0].doc = "TX PAM4 Precoder";
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_tx_pam4_precoder_t, val);

    
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].name = "lp_tx_precoder";
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].doc = "has the link partner enabled pre-coding on its TX side. In other words - enable the decoding on my RX side.";
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].size_of_values = sizeof(dnx_data_port_static_add_lp_tx_precoder_t);
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].entry_get = dnx_data_port_static_add_lp_tx_precoder_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].keys[0].doc = "logical port number";

    
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].nof_values, "_dnx_data_port_static_add_table_lp_tx_precoder table values");
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].values[0].name = "val";
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].values[0].type = "uint32";
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].values[0].doc = "Link Partner TX Precoder";
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_lp_tx_precoder_t, val);

    
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].name = "fabric_quad_info";
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].doc = "General fabric quad info";
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].size_of_values = sizeof(dnx_data_port_static_add_fabric_quad_info_t);
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].entry_get = dnx_data_port_static_add_fabric_quad_info_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].keys[0].name = "quad";
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].keys[0].doc = "Quad ID";

    
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].nof_values, "_dnx_data_port_static_add_table_fabric_quad_info table values");
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].values[0].name = "quad_enable";
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].values[0].doc = "Is the quad enabled";
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_fabric_quad_info_t, quad_enable);

    
    submodule_data->tables[dnx_data_port_static_add_table_header_type].name = "header_type";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].doc = "header type information";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_header_type].size_of_values = sizeof(dnx_data_port_static_add_header_type_t);
    submodule_data->tables[dnx_data_port_static_add_table_header_type].entry_get = dnx_data_port_static_add_header_type_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_header_type].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_header_type].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].keys[0].doc = "port";

    
    submodule_data->tables[dnx_data_port_static_add_table_header_type].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_header_type].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_header_type].nof_values, "_dnx_data_port_static_add_table_header_type table values");
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[0].name = "header_type_in";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[0].doc = "header type per incoming port";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_header_type_t, header_type_in);
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[1].name = "header_type_out";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[1].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[1].doc = "header type per outgoing port";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_header_type_t, header_type_out);
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[2].name = "header_type";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[2].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[2].doc = "header type per incoming/outgoing port";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[2].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_header_type_t, header_type);

    
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].name = "erp_exist";
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].doc = "Is ERP port defined in config file for the core";
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].size_of_values = sizeof(dnx_data_port_static_add_erp_exist_t);
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].entry_get = dnx_data_port_static_add_erp_exist_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].keys[0].name = "core";
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].keys[0].doc = "core";

    
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_erp_exist].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_erp_exist].nof_values, "_dnx_data_port_static_add_table_erp_exist table values");
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].values[0].name = "exist";
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].values[0].doc = "does epr exist on this core";
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_erp_exist_t, exist);

    
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].name = "base_flexe_instance";
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].doc = "base flexe instance";
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].size_of_values = sizeof(dnx_data_port_static_add_base_flexe_instance_t);
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].entry_get = dnx_data_port_static_add_base_flexe_instance_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].keys[0].doc = "logical port number";

    
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].nof_values, "_dnx_data_port_static_add_table_base_flexe_instance table values");
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].values[0].name = "val";
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].values[0].doc = "base flexe instance";
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_base_flexe_instance_t, val);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_port_static_add_feature_get(
    int unit,
    dnx_data_port_static_add_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, feature);
}


uint32
dnx_data_port_static_add_default_speed_for_special_if_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_define_default_speed_for_special_if);
}

uint32
dnx_data_port_static_add_fabric_fw_load_method_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_define_fabric_fw_load_method);
}

uint32
dnx_data_port_static_add_fabric_fw_crc_check_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_define_fabric_fw_crc_check);
}

uint32
dnx_data_port_static_add_fabric_fw_load_verify_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_define_fabric_fw_load_verify);
}

uint32
dnx_data_port_static_add_nif_fw_load_method_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_define_nif_fw_load_method);
}

uint32
dnx_data_port_static_add_nif_fw_crc_check_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_define_nif_fw_crc_check);
}

uint32
dnx_data_port_static_add_nif_fw_load_verify_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_define_nif_fw_load_verify);
}



const dnx_data_port_static_add_ucode_port_t *
dnx_data_port_static_add_ucode_port_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_ucode_port);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnx_data_port_static_add_ucode_port_t *) data;

}

const dnx_data_port_static_add_speed_t *
dnx_data_port_static_add_speed_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_speed);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnx_data_port_static_add_speed_t *) data;

}

const dnx_data_port_static_add_ext_stat_speed_t *
dnx_data_port_static_add_ext_stat_speed_get(
    int unit,
    int ext_stat_port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_ext_stat_speed);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ext_stat_port, 0);
    return (const dnx_data_port_static_add_ext_stat_speed_t *) data;

}

const dnx_data_port_static_add_eth_padding_t *
dnx_data_port_static_add_eth_padding_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_eth_padding);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnx_data_port_static_add_eth_padding_t *) data;

}

const dnx_data_port_static_add_link_training_t *
dnx_data_port_static_add_link_training_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_link_training);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnx_data_port_static_add_link_training_t *) data;

}

const dnx_data_port_static_add_ext_stat_link_training_t *
dnx_data_port_static_add_ext_stat_link_training_get(
    int unit,
    int ext_stat_port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_ext_stat_link_training);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ext_stat_port, 0);
    return (const dnx_data_port_static_add_ext_stat_link_training_t *) data;

}

const dnx_data_port_static_add_fec_type_t *
dnx_data_port_static_add_fec_type_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_fec_type);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnx_data_port_static_add_fec_type_t *) data;

}

const dnx_data_port_static_add_serdes_lane_config_t *
dnx_data_port_static_add_serdes_lane_config_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_serdes_lane_config);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnx_data_port_static_add_serdes_lane_config_t *) data;

}

const dnx_data_port_static_add_serdes_tx_taps_t *
dnx_data_port_static_add_serdes_tx_taps_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_serdes_tx_taps);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnx_data_port_static_add_serdes_tx_taps_t *) data;

}

const dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t *
dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t *) data;

}

const dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t *
dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_get(
    int unit,
    int lane)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, lane, 0);
    return (const dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t *) data;

}

const dnx_data_port_static_add_tx_pam4_precoder_t *
dnx_data_port_static_add_tx_pam4_precoder_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_tx_pam4_precoder);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnx_data_port_static_add_tx_pam4_precoder_t *) data;

}

const dnx_data_port_static_add_lp_tx_precoder_t *
dnx_data_port_static_add_lp_tx_precoder_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_lp_tx_precoder);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnx_data_port_static_add_lp_tx_precoder_t *) data;

}

const dnx_data_port_static_add_fabric_quad_info_t *
dnx_data_port_static_add_fabric_quad_info_get(
    int unit,
    int quad)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_fabric_quad_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, quad, 0);
    return (const dnx_data_port_static_add_fabric_quad_info_t *) data;

}

const dnx_data_port_static_add_header_type_t *
dnx_data_port_static_add_header_type_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_header_type);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnx_data_port_static_add_header_type_t *) data;

}

const dnx_data_port_static_add_erp_exist_t *
dnx_data_port_static_add_erp_exist_get(
    int unit,
    int core)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_erp_exist);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, core, 0);
    return (const dnx_data_port_static_add_erp_exist_t *) data;

}

const dnx_data_port_static_add_base_flexe_instance_t *
dnx_data_port_static_add_base_flexe_instance_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_base_flexe_instance);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnx_data_port_static_add_base_flexe_instance_t *) data;

}


shr_error_e
dnx_data_port_static_add_ucode_port_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_static_add_ucode_port_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_ucode_port);
    data = (const dnx_data_port_static_add_ucode_port_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->interface);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_lanes);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->interface_offset);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->core);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tm_port);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->channel);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_stif);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_stif_data);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_kbp);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_cross_connect);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_flexe_phy);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_ingress_interleave);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_egress_interleave);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_tdm);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_if_tdm_only);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_if_tdm_hybrid);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->sch_priority_propagation_en);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->num_priorities);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->base_q_pair);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->num_sch_priorities);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->base_hr);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_port_static_add_speed_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_static_add_speed_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_speed);
    data = (const dnx_data_port_static_add_speed_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_port_static_add_ext_stat_speed_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_static_add_ext_stat_speed_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_ext_stat_speed);
    data = (const dnx_data_port_static_add_ext_stat_speed_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->speed);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_lanes);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_port_static_add_eth_padding_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_static_add_eth_padding_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_eth_padding);
    data = (const dnx_data_port_static_add_eth_padding_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pad_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_port_static_add_link_training_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_static_add_link_training_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_link_training);
    data = (const dnx_data_port_static_add_link_training_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_port_static_add_ext_stat_link_training_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_static_add_ext_stat_link_training_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_ext_stat_link_training);
    data = (const dnx_data_port_static_add_ext_stat_link_training_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_port_static_add_fec_type_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_static_add_fec_type_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_fec_type);
    data = (const dnx_data_port_static_add_fec_type_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_port_static_add_serdes_lane_config_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_static_add_serdes_lane_config_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_serdes_lane_config);
    data = (const dnx_data_port_static_add_serdes_lane_config_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
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
dnx_data_port_static_add_serdes_tx_taps_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_static_add_serdes_tx_taps_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_serdes_tx_taps);
    data = (const dnx_data_port_static_add_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pre);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->main);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->post);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pre2);
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
dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps);
    data = (const dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pre);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->main);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->post);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pre2);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->post2);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->post3);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->txfir_tap_enable);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps);
    data = (const dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pre);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->main);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->post);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pre2);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->post2);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->post3);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->txfir_tap_enable);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_port_static_add_tx_pam4_precoder_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_static_add_tx_pam4_precoder_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_tx_pam4_precoder);
    data = (const dnx_data_port_static_add_tx_pam4_precoder_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_port_static_add_lp_tx_precoder_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_static_add_lp_tx_precoder_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_lp_tx_precoder);
    data = (const dnx_data_port_static_add_lp_tx_precoder_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_port_static_add_fabric_quad_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_static_add_fabric_quad_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_fabric_quad_info);
    data = (const dnx_data_port_static_add_fabric_quad_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->quad_enable);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_port_static_add_header_type_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_static_add_header_type_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_header_type);
    data = (const dnx_data_port_static_add_header_type_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->header_type_in);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->header_type_out);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->header_type);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_port_static_add_erp_exist_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_static_add_erp_exist_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_erp_exist);
    data = (const dnx_data_port_static_add_erp_exist_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->exist);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_port_static_add_base_flexe_instance_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_static_add_base_flexe_instance_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_base_flexe_instance);
    data = (const dnx_data_port_static_add_base_flexe_instance_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->val);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_port_static_add_ucode_port_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_ucode_port);

}

const dnxc_data_table_info_t *
dnx_data_port_static_add_speed_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_speed);

}

const dnxc_data_table_info_t *
dnx_data_port_static_add_ext_stat_speed_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_ext_stat_speed);

}

const dnxc_data_table_info_t *
dnx_data_port_static_add_eth_padding_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_eth_padding);

}

const dnxc_data_table_info_t *
dnx_data_port_static_add_link_training_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_link_training);

}

const dnxc_data_table_info_t *
dnx_data_port_static_add_ext_stat_link_training_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_ext_stat_link_training);

}

const dnxc_data_table_info_t *
dnx_data_port_static_add_fec_type_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_fec_type);

}

const dnxc_data_table_info_t *
dnx_data_port_static_add_serdes_lane_config_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_serdes_lane_config);

}

const dnxc_data_table_info_t *
dnx_data_port_static_add_serdes_tx_taps_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_serdes_tx_taps);

}

const dnxc_data_table_info_t *
dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps);

}

const dnxc_data_table_info_t *
dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps);

}

const dnxc_data_table_info_t *
dnx_data_port_static_add_tx_pam4_precoder_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_tx_pam4_precoder);

}

const dnxc_data_table_info_t *
dnx_data_port_static_add_lp_tx_precoder_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_lp_tx_precoder);

}

const dnxc_data_table_info_t *
dnx_data_port_static_add_fabric_quad_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_fabric_quad_info);

}

const dnxc_data_table_info_t *
dnx_data_port_static_add_header_type_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_header_type);

}

const dnxc_data_table_info_t *
dnx_data_port_static_add_erp_exist_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_erp_exist);

}

const dnxc_data_table_info_t *
dnx_data_port_static_add_base_flexe_instance_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_base_flexe_instance);

}






static shr_error_e
dnx_data_port_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "general ports data";
    
    submodule_data->nof_features = _dnx_data_port_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data port general features");

    submodule_data->features[dnx_data_port_general_unrestricted_portmod_pll].name = "unrestricted_portmod_pll";
    submodule_data->features[dnx_data_port_general_unrestricted_portmod_pll].doc = "should pormod PLL restriction be honored";
    submodule_data->features[dnx_data_port_general_unrestricted_portmod_pll].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_port_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data port general defines");

    submodule_data->defines[dnx_data_port_general_define_erp_tm_port].name = "erp_tm_port";
    submodule_data->defines[dnx_data_port_general_define_erp_tm_port].doc = "tm port reserved for ERP";
    
    submodule_data->defines[dnx_data_port_general_define_erp_tm_port].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_max_nof_channels].name = "max_nof_channels";
    submodule_data->defines[dnx_data_port_general_define_max_nof_channels].doc = "max nof channel in an interface";
    
    submodule_data->defines[dnx_data_port_general_define_max_nof_channels].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_fabric_phys_offset].name = "fabric_phys_offset";
    submodule_data->defines[dnx_data_port_general_define_fabric_phys_offset].doc = "Offset for physical fabric ports";
    
    submodule_data->defines[dnx_data_port_general_define_fabric_phys_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_pp_dsp_size].name = "pp_dsp_size";
    submodule_data->defines[dnx_data_port_general_define_pp_dsp_size].doc = "define the size of the pp dsp (in bits)";
    
    submodule_data->defines[dnx_data_port_general_define_pp_dsp_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_ingress_vlan_domain_size].name = "ingress_vlan_domain_size";
    submodule_data->defines[dnx_data_port_general_define_ingress_vlan_domain_size].doc = "define the size of the ingress vlan domain (in bits)";
    
    submodule_data->defines[dnx_data_port_general_define_ingress_vlan_domain_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_vlan_domain_size].name = "vlan_domain_size";
    submodule_data->defines[dnx_data_port_general_define_vlan_domain_size].doc = "define the max size of vlan domain (in bits)";
    
    submodule_data->defines[dnx_data_port_general_define_vlan_domain_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_ffc_instruction_size].name = "ffc_instruction_size";
    submodule_data->defines[dnx_data_port_general_define_ffc_instruction_size].doc = "define the size of the ffc instruction (in bits)";
    
    submodule_data->defines[dnx_data_port_general_define_ffc_instruction_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_max_first_header_size_to_skip].name = "max_first_header_size_to_skip";
    submodule_data->defines[dnx_data_port_general_define_max_first_header_size_to_skip].doc = "max number of bytes that can be skipped";
    
    submodule_data->defines[dnx_data_port_general_define_max_first_header_size_to_skip].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_prt_ffc_width_start_offset].name = "prt_ffc_width_start_offset";
    submodule_data->defines[dnx_data_port_general_define_prt_ffc_width_start_offset].doc = "The offset of ffc width in the PRT FFC builder";
    
    submodule_data->defines[dnx_data_port_general_define_prt_ffc_width_start_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_prt_ffc_start_input_offset].name = "prt_ffc_start_input_offset";
    submodule_data->defines[dnx_data_port_general_define_prt_ffc_start_input_offset].doc = "The offset of ffc input offset in the PRT FFC builder";
    
    submodule_data->defines[dnx_data_port_general_define_prt_ffc_start_input_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_nof_tm_ports].name = "nof_tm_ports";
    submodule_data->defines[dnx_data_port_general_define_nof_tm_ports].doc = "nof tm ports per core";
    
    submodule_data->defines[dnx_data_port_general_define_nof_tm_ports].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_nof_tm_ports_per_fap_id].name = "nof_tm_ports_per_fap_id";
    submodule_data->defines[dnx_data_port_general_define_nof_tm_ports_per_fap_id].doc = "nof tm ports per fap id";
    
    submodule_data->defines[dnx_data_port_general_define_nof_tm_ports_per_fap_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_nof_pp_ports].name = "nof_pp_ports";
    submodule_data->defines[dnx_data_port_general_define_nof_pp_ports].doc = "nof pp ports per core";
    
    submodule_data->defines[dnx_data_port_general_define_nof_pp_ports].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_reserved_tm_port].name = "reserved_tm_port";
    submodule_data->defines[dnx_data_port_general_define_reserved_tm_port].doc = "tm port reserved for redirection of returned credits in LAG SCH";
    
    submodule_data->defines[dnx_data_port_general_define_reserved_tm_port].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_nof_vlan_membership_if].name = "nof_vlan_membership_if";
    submodule_data->defines[dnx_data_port_general_define_nof_vlan_membership_if].doc = "number of vlan_membership_if";
    
    submodule_data->defines[dnx_data_port_general_define_nof_vlan_membership_if].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_max_vlan_membership_if].name = "max_vlan_membership_if";
    submodule_data->defines[dnx_data_port_general_define_max_vlan_membership_if].doc = "max value of vlan_membership_if";
    
    submodule_data->defines[dnx_data_port_general_define_max_vlan_membership_if].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_fabric_port_base].name = "fabric_port_base";
    submodule_data->defines[dnx_data_port_general_define_fabric_port_base].doc = "first fabric logical port";
    
    submodule_data->defines[dnx_data_port_general_define_fabric_port_base].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_pp_port_bits_size].name = "pp_port_bits_size";
    submodule_data->defines[dnx_data_port_general_define_pp_port_bits_size].doc = "pp port size in bits";
    
    submodule_data->defines[dnx_data_port_general_define_pp_port_bits_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_nof_port_bits_in_pp_bus].name = "nof_port_bits_in_pp_bus";
    submodule_data->defines[dnx_data_port_general_define_nof_port_bits_in_pp_bus].doc = "nof port bits in PP bus";
    
    submodule_data->defines[dnx_data_port_general_define_nof_port_bits_in_pp_bus].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_prt_configuration_ptch2_default_index].name = "prt_configuration_ptch2_default_index";
    submodule_data->defines[dnx_data_port_general_define_prt_configuration_ptch2_default_index].doc = "index of ptch2 default configuration in prt_configuration table";
    
    submodule_data->defines[dnx_data_port_general_define_prt_configuration_ptch2_default_index].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_prt_configuration_ptch1_default_index].name = "prt_configuration_ptch1_default_index";
    submodule_data->defines[dnx_data_port_general_define_prt_configuration_ptch1_default_index].doc = "index of ptch1 default configuration in prt_configuration table";
    
    submodule_data->defines[dnx_data_port_general_define_prt_configuration_ptch1_default_index].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_prt_configuration_eth_default_index].name = "prt_configuration_eth_default_index";
    submodule_data->defines[dnx_data_port_general_define_prt_configuration_eth_default_index].doc = "index of eth default configuration in prt_configuration table";
    
    submodule_data->defines[dnx_data_port_general_define_prt_configuration_eth_default_index].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_port_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port general tables");

    
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].name = "prt_configuration";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].doc = "prt configuration information";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].size_of_values = sizeof(dnx_data_port_general_prt_configuration_t);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].entry_get = dnx_data_port_general_prt_configuration_entry_str_get;

    
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].nof_keys = 1;
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].keys[0].name = "index";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].keys[0].doc = "arbitrary index";

    
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].nof_values = 31;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_general_table_prt_configuration].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_general_table_prt_configuration].nof_values, "_dnx_data_port_general_table_prt_configuration table values");
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[0].name = "prt_recycle_profile";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[0].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[0].doc = "prt recycle profile";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, prt_recycle_profile);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[1].name = "port_termination_ptc_profile";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[1].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[1].doc = "port termination ptc profile";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, port_termination_ptc_profile);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[2].name = "prt_qual_mode";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[2].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[2].doc = "prt qual mode";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[2].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, prt_qual_mode);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[3].name = "layer_offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[3].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[3].doc = "prt layer offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[3].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, layer_offset);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[4].name = "context_select";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[4].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[4].doc = "prt context select";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[4].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, context_select);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[5].name = "kbr_valid_bitmap_4";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[5].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[5].doc = "prt kbr valid bitmap 4";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[5].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, kbr_valid_bitmap_4);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[6].name = "kbr_valid_bitmap_3";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[6].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[6].doc = "prt kbr valid bitmap 3";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[6].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, kbr_valid_bitmap_3);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[7].name = "kbr_valid_bitmap_2";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[7].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[7].doc = "prt kbr valid bitmap 2";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[7].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, kbr_valid_bitmap_2);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[8].name = "kbr_valid_bitmap_1";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[8].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[8].doc = "prt kbr valid bitmap 1";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[8].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, kbr_valid_bitmap_1);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[9].name = "kbr_valid_bitmap_0";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[9].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[9].doc = "prt kbr valid bitmap 0";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[9].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, kbr_valid_bitmap_0);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[10].name = "ffc_5_offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[10].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[10].doc = "prt ffc 5 offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[10].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, ffc_5_offset);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[11].name = "ffc_5_width";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[11].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[11].doc = "prt ffc 5 width";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[11].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, ffc_5_width);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[12].name = "ffc_5_input_offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[12].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[12].doc = "prt ffc 5 input offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[12].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, ffc_5_input_offset);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[13].name = "ffc_4_offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[13].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[13].doc = "prt ffc 4 offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[13].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, ffc_4_offset);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[14].name = "ffc_4_width";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[14].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[14].doc = "prt ffc 4 width";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[14].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, ffc_4_width);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[15].name = "ffc_4_input_offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[15].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[15].doc = "prt ffc 4 input offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[15].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, ffc_4_input_offset);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[16].name = "ffc_3_offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[16].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[16].doc = "prt ffc 3 offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[16].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, ffc_3_offset);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[17].name = "ffc_3_width";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[17].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[17].doc = "prt ffc 3 width";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[17].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, ffc_3_width);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[18].name = "ffc_3_input_offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[18].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[18].doc = "prt ffc 3 input offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[18].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, ffc_3_input_offset);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[19].name = "ffc_2_offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[19].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[19].doc = "prt ffc 2 offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[19].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, ffc_2_offset);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[20].name = "ffc_2_width";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[20].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[20].doc = "prt ffc 2 width";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[20].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, ffc_2_width);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[21].name = "ffc_2_input_offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[21].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[21].doc = "prt ffc 2 input offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[21].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, ffc_2_input_offset);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[22].name = "ffc_1_offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[22].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[22].doc = "prt ffc 1 offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[22].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, ffc_1_offset);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[23].name = "ffc_1_width";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[23].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[23].doc = "prt ffc 1 width";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[23].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, ffc_1_width);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[24].name = "ffc_1_input_offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[24].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[24].doc = "prt ffc 1 input offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[24].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, ffc_1_input_offset);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[25].name = "ffc_0_offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[25].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[25].doc = "prt ffc 0 offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[25].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, ffc_0_offset);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[26].name = "ffc_0_width";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[26].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[26].doc = "prt ffc 0 width";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[26].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, ffc_0_width);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[27].name = "ffc_0_input_offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[27].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[27].doc = "prt ffc 0 input offset";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[27].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, ffc_0_input_offset);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[28].name = "src_sys_port_prt_mode";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[28].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[28].doc = "src system port prt mode";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[28].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, src_sys_port_prt_mode);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[29].name = "pp_port_en";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[29].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[29].doc = "pp port enable";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[29].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, pp_port_en);
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[30].name = "tcam_mode_full_en";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[30].type = "int";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[30].doc = "tcam mode full enable";
    submodule_data->tables[dnx_data_port_general_table_prt_configuration].values[30].offset = UTILEX_OFFSETOF(dnx_data_port_general_prt_configuration_t, tcam_mode_full_en);

    
    submodule_data->tables[dnx_data_port_general_table_first_header_size].name = "first_header_size";
    submodule_data->tables[dnx_data_port_general_table_first_header_size].doc = "size of the first header that should be skipped";
    submodule_data->tables[dnx_data_port_general_table_first_header_size].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_general_table_first_header_size].size_of_values = sizeof(dnx_data_port_general_first_header_size_t);
    submodule_data->tables[dnx_data_port_general_table_first_header_size].entry_get = dnx_data_port_general_first_header_size_entry_str_get;

    
    submodule_data->tables[dnx_data_port_general_table_first_header_size].nof_keys = 1;
    submodule_data->tables[dnx_data_port_general_table_first_header_size].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_general_table_first_header_size].keys[0].doc = "logical port";

    
    submodule_data->tables[dnx_data_port_general_table_first_header_size].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_general_table_first_header_size].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_general_table_first_header_size].nof_values, "_dnx_data_port_general_table_first_header_size table values");
    submodule_data->tables[dnx_data_port_general_table_first_header_size].values[0].name = "first_header_size";
    submodule_data->tables[dnx_data_port_general_table_first_header_size].values[0].type = "int";
    submodule_data->tables[dnx_data_port_general_table_first_header_size].values[0].doc = "size of the first header that should be skipped";
    submodule_data->tables[dnx_data_port_general_table_first_header_size].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_general_first_header_size_t, first_header_size);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_port_general_feature_get(
    int unit,
    dnx_data_port_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, feature);
}


uint32
dnx_data_port_general_erp_tm_port_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_erp_tm_port);
}

uint32
dnx_data_port_general_max_nof_channels_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_max_nof_channels);
}

uint32
dnx_data_port_general_fabric_phys_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_fabric_phys_offset);
}

uint32
dnx_data_port_general_pp_dsp_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_pp_dsp_size);
}

uint32
dnx_data_port_general_ingress_vlan_domain_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_ingress_vlan_domain_size);
}

uint32
dnx_data_port_general_vlan_domain_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_vlan_domain_size);
}

uint32
dnx_data_port_general_ffc_instruction_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_ffc_instruction_size);
}

uint32
dnx_data_port_general_max_first_header_size_to_skip_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_max_first_header_size_to_skip);
}

uint32
dnx_data_port_general_prt_ffc_width_start_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_prt_ffc_width_start_offset);
}

uint32
dnx_data_port_general_prt_ffc_start_input_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_prt_ffc_start_input_offset);
}

uint32
dnx_data_port_general_nof_tm_ports_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_nof_tm_ports);
}

uint32
dnx_data_port_general_nof_tm_ports_per_fap_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_nof_tm_ports_per_fap_id);
}

uint32
dnx_data_port_general_nof_pp_ports_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_nof_pp_ports);
}

uint32
dnx_data_port_general_reserved_tm_port_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_reserved_tm_port);
}

uint32
dnx_data_port_general_nof_vlan_membership_if_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_nof_vlan_membership_if);
}

uint32
dnx_data_port_general_max_vlan_membership_if_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_max_vlan_membership_if);
}

uint32
dnx_data_port_general_fabric_port_base_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_fabric_port_base);
}

uint32
dnx_data_port_general_pp_port_bits_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_pp_port_bits_size);
}

uint32
dnx_data_port_general_nof_port_bits_in_pp_bus_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_nof_port_bits_in_pp_bus);
}

uint32
dnx_data_port_general_prt_configuration_ptch2_default_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_prt_configuration_ptch2_default_index);
}

uint32
dnx_data_port_general_prt_configuration_ptch1_default_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_prt_configuration_ptch1_default_index);
}

uint32
dnx_data_port_general_prt_configuration_eth_default_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_prt_configuration_eth_default_index);
}



const dnx_data_port_general_prt_configuration_t *
dnx_data_port_general_prt_configuration_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_table_prt_configuration);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_port_general_prt_configuration_t *) data;

}

const dnx_data_port_general_first_header_size_t *
dnx_data_port_general_first_header_size_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_table_first_header_size);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnx_data_port_general_first_header_size_t *) data;

}


shr_error_e
dnx_data_port_general_prt_configuration_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_general_prt_configuration_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_table_prt_configuration);
    data = (const dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->prt_recycle_profile);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->port_termination_ptc_profile);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->prt_qual_mode);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->layer_offset);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->context_select);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->kbr_valid_bitmap_4);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->kbr_valid_bitmap_3);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->kbr_valid_bitmap_2);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->kbr_valid_bitmap_1);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->kbr_valid_bitmap_0);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ffc_5_offset);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ffc_5_width);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ffc_5_input_offset);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ffc_4_offset);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ffc_4_width);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ffc_4_input_offset);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ffc_3_offset);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ffc_3_width);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ffc_3_input_offset);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ffc_2_offset);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ffc_2_width);
            break;
        case 21:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ffc_2_input_offset);
            break;
        case 22:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ffc_1_offset);
            break;
        case 23:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ffc_1_width);
            break;
        case 24:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ffc_1_input_offset);
            break;
        case 25:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ffc_0_offset);
            break;
        case 26:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ffc_0_width);
            break;
        case 27:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ffc_0_input_offset);
            break;
        case 28:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->src_sys_port_prt_mode);
            break;
        case 29:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pp_port_en);
            break;
        case 30:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tcam_mode_full_en);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_port_general_first_header_size_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_general_first_header_size_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_table_first_header_size);
    data = (const dnx_data_port_general_first_header_size_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->first_header_size);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_port_general_prt_configuration_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_table_prt_configuration);

}

const dnxc_data_table_info_t *
dnx_data_port_general_first_header_size_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_table_first_header_size);

}






static shr_error_e
dnx_data_port_egress_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "egress";
    submodule_data->doc = "egress related data";
    
    submodule_data->nof_features = _dnx_data_port_egress_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data port egress features");

    
    submodule_data->nof_defines = _dnx_data_port_egress_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data port egress defines");

    submodule_data->defines[dnx_data_port_egress_define_nof_ifs].name = "nof_ifs";
    submodule_data->defines[dnx_data_port_egress_define_nof_ifs].doc = "nof egress intrfaces per core";
    
    submodule_data->defines[dnx_data_port_egress_define_nof_ifs].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_port_egress_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port egress tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_port_egress_feature_get(
    int unit,
    dnx_data_port_egress_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_egress, feature);
}


uint32
dnx_data_port_egress_nof_ifs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_egress, dnx_data_port_egress_define_nof_ifs);
}










static shr_error_e
dnx_data_port_imb_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "imb";
    submodule_data->doc = "IMB related data";
    
    submodule_data->nof_features = _dnx_data_port_imb_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data port imb features");

    
    submodule_data->nof_defines = _dnx_data_port_imb_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data port imb defines");

    
    submodule_data->nof_tables = _dnx_data_port_imb_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port imb tables");

    
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].name = "imb_type_info";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].doc = "Information per IMB type";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].size_of_values = sizeof(dnx_data_port_imb_imb_type_info_t);
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].entry_get = dnx_data_port_imb_imb_type_info_entry_str_get;

    
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].nof_keys = 1;
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].keys[0].name = "type";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].keys[0].doc = "IMB type";

    
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].nof_values = 5;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_imb_table_imb_type_info].nof_values, "_dnx_data_port_imb_table_imb_type_info table values");
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[0].name = "nof_pms";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[0].type = "uint32";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[0].doc = "Number of PMs in IMB type";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_imb_imb_type_info_t, nof_pms);
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[1].name = "nof_lanes";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[1].doc = "Number of lanes in IMB type";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_imb_imb_type_info_t, nof_lanes);
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[2].name = "nof_lanes_in_pm";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[2].doc = "Number of lanes in PM in IMB type";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_port_imb_imb_type_info_t, nof_lanes_in_pm);
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[3].name = "nof_memory_entries";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[3].type = "uint32";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[3].doc = "Number of memory entries in IMB type";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_port_imb_imb_type_info_t, nof_memory_entries);
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[4].name = "rate_per_rmc_bit";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[4].type = "uint32";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[4].doc = "Rate per RMC weight bit in IMB type";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[4].offset = UTILEX_OFFSETOF(dnx_data_port_imb_imb_type_info_t, rate_per_rmc_bit);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_port_imb_feature_get(
    int unit,
    dnx_data_port_imb_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_imb, feature);
}




const dnx_data_port_imb_imb_type_info_t *
dnx_data_port_imb_imb_type_info_get(
    int unit,
    int type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_imb, dnx_data_port_imb_table_imb_type_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, type, 0);
    return (const dnx_data_port_imb_imb_type_info_t *) data;

}


shr_error_e
dnx_data_port_imb_imb_type_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_imb_imb_type_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_imb, dnx_data_port_imb_table_imb_type_info);
    data = (const dnx_data_port_imb_imb_type_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_pms);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_lanes);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_lanes_in_pm);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_memory_entries);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->rate_per_rmc_bit);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_port_imb_imb_type_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_imb, dnx_data_port_imb_table_imb_type_info);

}



shr_error_e
dnx_data_port_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "port";
    module_data->nof_submodules = _dnx_data_port_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data port submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_port_static_add_init(unit, &module_data->submodules[dnx_data_port_submodule_static_add]));
    SHR_IF_ERR_EXIT(dnx_data_port_general_init(unit, &module_data->submodules[dnx_data_port_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_port_egress_init(unit, &module_data->submodules[dnx_data_port_submodule_egress]));
    SHR_IF_ERR_EXIT(dnx_data_port_imb_init(unit, &module_data->submodules[dnx_data_port_submodule_imb]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_port_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_port_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_port_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_port_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_port_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_port_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

