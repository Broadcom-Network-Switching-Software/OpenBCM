
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

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_port.h>




extern shr_error_e jr2_a0_data_port_attach(
    int unit);


extern shr_error_e j2c_a0_data_port_attach(
    int unit);


extern shr_error_e q2a_a0_data_port_attach(
    int unit);


extern shr_error_e j2p_a0_data_port_attach(
    int unit);


extern shr_error_e j2x_a0_data_port_attach(
    int unit);




static shr_error_e
dnx_data_port_static_add_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "static_add";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_port_static_add_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data port static_add features");

    submodule_data->features[dnx_data_port_static_add_base_flexe_instance_exists].name = "base_flexe_instance_exists";
    submodule_data->features[dnx_data_port_static_add_base_flexe_instance_exists].doc = "";
    submodule_data->features[dnx_data_port_static_add_base_flexe_instance_exists].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_port_static_add_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data port static_add defines");

    submodule_data->defines[dnx_data_port_static_add_define_default_speed_for_special_if].name = "default_speed_for_special_if";
    submodule_data->defines[dnx_data_port_static_add_define_default_speed_for_special_if].doc = "";
    
    submodule_data->defines[dnx_data_port_static_add_define_default_speed_for_special_if].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_static_add_define_fabric_fw_load_method].name = "fabric_fw_load_method";
    submodule_data->defines[dnx_data_port_static_add_define_fabric_fw_load_method].doc = "";
    
    submodule_data->defines[dnx_data_port_static_add_define_fabric_fw_load_method].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_static_add_define_fabric_fw_crc_check].name = "fabric_fw_crc_check";
    submodule_data->defines[dnx_data_port_static_add_define_fabric_fw_crc_check].doc = "";
    
    submodule_data->defines[dnx_data_port_static_add_define_fabric_fw_crc_check].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_static_add_define_fabric_fw_load_verify].name = "fabric_fw_load_verify";
    submodule_data->defines[dnx_data_port_static_add_define_fabric_fw_load_verify].doc = "";
    
    submodule_data->defines[dnx_data_port_static_add_define_fabric_fw_load_verify].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_static_add_define_nif_fw_load_method].name = "nif_fw_load_method";
    submodule_data->defines[dnx_data_port_static_add_define_nif_fw_load_method].doc = "";
    
    submodule_data->defines[dnx_data_port_static_add_define_nif_fw_load_method].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_static_add_define_nif_fw_crc_check].name = "nif_fw_crc_check";
    submodule_data->defines[dnx_data_port_static_add_define_nif_fw_crc_check].doc = "";
    
    submodule_data->defines[dnx_data_port_static_add_define_nif_fw_crc_check].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_static_add_define_nif_fw_load_verify].name = "nif_fw_load_verify";
    submodule_data->defines[dnx_data_port_static_add_define_nif_fw_load_verify].doc = "";
    
    submodule_data->defines[dnx_data_port_static_add_define_nif_fw_load_verify].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_port_static_add_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port static_add tables");

    
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].name = "ucode_port";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].size_of_values = sizeof(dnx_data_port_static_add_ucode_port_t);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].entry_get = dnx_data_port_static_add_ucode_port_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].nof_values = 25;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_ucode_port].nof_values, "_dnx_data_port_static_add_table_ucode_port table values");
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[0].name = "interface";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[0].type = "bcm_port_if_t";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[0].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, interface);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[1].name = "nof_lanes";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[1].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[1].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, nof_lanes);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[2].name = "interface_offset";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[2].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[2].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[2].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, interface_offset);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[3].name = "core";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[3].type = "bcm_core_t";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[3].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[3].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, core);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[4].name = "tm_port";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[4].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[4].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[4].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, tm_port);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[5].name = "out_tm_port";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[5].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[5].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[5].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, out_tm_port);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[6].name = "channel";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[6].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[6].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[6].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, channel);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[7].name = "is_stif";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[7].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[7].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[7].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_stif);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[8].name = "is_stif_data";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[8].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[8].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[8].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_stif_data);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[9].name = "is_mc";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[9].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[9].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[9].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_mc);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[10].name = "is_kbp";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[10].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[10].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[10].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_kbp);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[11].name = "is_cross_connect";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[11].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[11].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[11].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_cross_connect);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[12].name = "is_flexe_phy";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[12].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[12].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[12].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_flexe_phy);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[13].name = "is_flexe_unaware";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[13].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[13].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[13].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_flexe_unaware);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[14].name = "is_ingress_interleave";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[14].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[14].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[14].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_ingress_interleave);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[15].name = "is_egress_interleave";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[15].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[15].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[15].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_egress_interleave);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[16].name = "is_tdm";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[16].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[16].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[16].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_tdm);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[17].name = "is_if_tdm_only";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[17].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[17].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[17].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_if_tdm_only);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[18].name = "is_if_tdm_hybrid";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[18].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[18].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[18].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, is_if_tdm_hybrid);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[19].name = "sch_priority_propagation_en";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[19].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[19].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[19].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, sch_priority_propagation_en);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[20].name = "frame_preemptable";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[20].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[20].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[20].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, frame_preemptable);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[21].name = "num_priorities";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[21].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[21].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[21].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, num_priorities);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[22].name = "base_q_pair";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[22].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[22].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[22].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, base_q_pair);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[23].name = "num_sch_priorities";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[23].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[23].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[23].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, num_sch_priorities);
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[24].name = "base_hr";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[24].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[24].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ucode_port].values[24].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ucode_port_t, base_hr);

    
    submodule_data->tables[dnx_data_port_static_add_table_speed].name = "speed";
    submodule_data->tables[dnx_data_port_static_add_table_speed].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_speed].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_speed].size_of_values = sizeof(dnx_data_port_static_add_speed_t);
    submodule_data->tables[dnx_data_port_static_add_table_speed].entry_get = dnx_data_port_static_add_speed_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_speed].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_speed].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_speed].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_static_add_table_speed].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_speed].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_speed].nof_values, "_dnx_data_port_static_add_table_speed table values");
    submodule_data->tables[dnx_data_port_static_add_table_speed].values[0].name = "val";
    submodule_data->tables[dnx_data_port_static_add_table_speed].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_speed].values[0].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_speed].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_speed_t, val);

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].name = "ext_stat_speed";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].size_of_values = sizeof(dnx_data_port_static_add_ext_stat_speed_t);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].entry_get = dnx_data_port_static_add_ext_stat_speed_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].keys[0].name = "ext_stat_port";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].nof_values, "_dnx_data_port_static_add_table_ext_stat_speed table values");
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].values[0].name = "speed";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].values[0].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_speed_t, speed);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].values[1].name = "nof_lanes";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].values[1].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].values[1].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_speed].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_speed_t, nof_lanes);

    
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].name = "eth_padding";
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].size_of_values = sizeof(dnx_data_port_static_add_eth_padding_t);
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].entry_get = dnx_data_port_static_add_eth_padding_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_eth_padding].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_eth_padding].nof_values, "_dnx_data_port_static_add_table_eth_padding table values");
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].values[0].name = "pad_size";
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].values[0].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_eth_padding].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_eth_padding_t, pad_size);

    
    submodule_data->tables[dnx_data_port_static_add_table_link_training].name = "link_training";
    submodule_data->tables[dnx_data_port_static_add_table_link_training].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_link_training].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_link_training].size_of_values = sizeof(dnx_data_port_static_add_link_training_t);
    submodule_data->tables[dnx_data_port_static_add_table_link_training].entry_get = dnx_data_port_static_add_link_training_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_link_training].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_link_training].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_link_training].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_static_add_table_link_training].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_link_training].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_link_training].nof_values, "_dnx_data_port_static_add_table_link_training table values");
    submodule_data->tables[dnx_data_port_static_add_table_link_training].values[0].name = "val";
    submodule_data->tables[dnx_data_port_static_add_table_link_training].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_link_training].values[0].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_link_training].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_link_training_t, val);

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].name = "ext_stat_link_training";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].size_of_values = sizeof(dnx_data_port_static_add_ext_stat_link_training_t);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].entry_get = dnx_data_port_static_add_ext_stat_link_training_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].keys[0].name = "ext_stat_port";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].nof_values, "_dnx_data_port_static_add_table_ext_stat_link_training table values");
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].values[0].name = "val";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].values[0].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_link_training].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_link_training_t, val);

    
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].name = "fec_type";
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].size_of_values = sizeof(dnx_data_port_static_add_fec_type_t);
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].entry_get = dnx_data_port_static_add_fec_type_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_fec_type].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_fec_type].nof_values, "_dnx_data_port_static_add_table_fec_type table values");
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].values[0].name = "val";
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].values[0].type = "bcm_port_phy_fec_t";
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].values[0].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_fec_type].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_fec_type_t, val);

    
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].name = "serdes_lane_config";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].size_of_values = sizeof(dnx_data_port_static_add_serdes_lane_config_t);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].entry_get = dnx_data_port_static_add_serdes_lane_config_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].nof_values = 6;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].nof_values, "_dnx_data_port_static_add_table_serdes_lane_config table values");
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[0].name = "dfe";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[0].type = "soc_dnxc_port_dfe_mode_t";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[0].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_lane_config_t, dfe);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[1].name = "media_type";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[1].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[1].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_lane_config_t, media_type);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[2].name = "unreliable_los";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[2].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[2].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[2].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_lane_config_t, unreliable_los);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[3].name = "cl72_auto_polarity_enable";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[3].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[3].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[3].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_lane_config_t, cl72_auto_polarity_enable);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[4].name = "cl72_restart_timeout_enable";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[4].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[4].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[4].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_lane_config_t, cl72_restart_timeout_enable);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[5].name = "channel_mode";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[5].type = "soc_dnxc_port_channel_mode_t";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[5].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_lane_config].values[5].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_lane_config_t, channel_mode);

    
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].name = "serdes_tx_taps";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].size_of_values = sizeof(dnx_data_port_static_add_serdes_tx_taps_t);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].entry_get = dnx_data_port_static_add_serdes_tx_taps_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].nof_values = 9;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].nof_values, "_dnx_data_port_static_add_table_serdes_tx_taps table values");
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[0].name = "pre";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[0].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_tx_taps_t, pre);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[1].name = "main";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[1].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[1].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_tx_taps_t, main);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[2].name = "post";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[2].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[2].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[2].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_tx_taps_t, post);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[3].name = "pre2";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[3].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[3].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[3].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_tx_taps_t, pre2);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[4].name = "post2";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[4].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[4].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[4].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_tx_taps_t, post2);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[5].name = "pre3";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[5].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[5].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[5].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_tx_taps_t, pre3);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[6].name = "post3";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[6].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[6].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[6].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_tx_taps_t, post3);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[7].name = "tx_tap_mode";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[7].type = "bcm_port_phy_tx_tap_mode_t";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[7].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[7].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_tx_taps_t, tx_tap_mode);
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[8].name = "signalling_mode";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[8].type = "bcm_port_phy_signalling_mode_t";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[8].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_serdes_tx_taps].values[8].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_serdes_tx_taps_t, signalling_mode);

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].name = "ext_stat_global_serdes_tx_taps";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].size_of_values = sizeof(dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].entry_get = dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].nof_keys = 0;

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].nof_values = 7;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].nof_values, "_dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps table values");
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[0].name = "pre";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[0].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t, pre);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[1].name = "main";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[1].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[1].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t, main);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[2].name = "post";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[2].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[2].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[2].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t, post);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[3].name = "pre2";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[3].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[3].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[3].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t, pre2);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[4].name = "post2";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[4].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[4].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[4].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t, post2);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[5].name = "post3";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[5].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[5].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[5].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t, post3);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[6].name = "txfir_tap_enable";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[6].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[6].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps].values[6].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t, txfir_tap_enable);

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].name = "ext_stat_lane_serdes_tx_taps";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].size_of_values = sizeof(dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].entry_get = dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].keys[0].name = "lane";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].nof_values = 7;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].nof_values, "_dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps table values");
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[0].name = "pre";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[0].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t, pre);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[1].name = "main";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[1].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[1].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t, main);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[2].name = "post";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[2].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[2].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[2].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t, post);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[3].name = "pre2";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[3].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[3].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[3].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t, pre2);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[4].name = "post2";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[4].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[4].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[4].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t, post2);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[5].name = "post3";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[5].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[5].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[5].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t, post3);
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[6].name = "txfir_tap_enable";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[6].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[6].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps].values[6].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t, txfir_tap_enable);

    
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].name = "tx_pam4_precoder";
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].size_of_values = sizeof(dnx_data_port_static_add_tx_pam4_precoder_t);
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].entry_get = dnx_data_port_static_add_tx_pam4_precoder_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].nof_values, "_dnx_data_port_static_add_table_tx_pam4_precoder table values");
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].values[0].name = "val";
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].values[0].type = "uint32";
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].values[0].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_tx_pam4_precoder].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_tx_pam4_precoder_t, val);

    
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].name = "lp_tx_precoder";
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].size_of_values = sizeof(dnx_data_port_static_add_lp_tx_precoder_t);
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].entry_get = dnx_data_port_static_add_lp_tx_precoder_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].nof_values, "_dnx_data_port_static_add_table_lp_tx_precoder table values");
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].values[0].name = "val";
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].values[0].type = "uint32";
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].values[0].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_lp_tx_precoder].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_lp_tx_precoder_t, val);

    
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].name = "fabric_quad_info";
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].size_of_values = sizeof(dnx_data_port_static_add_fabric_quad_info_t);
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].entry_get = dnx_data_port_static_add_fabric_quad_info_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].keys[0].name = "quad";
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].nof_values, "_dnx_data_port_static_add_table_fabric_quad_info table values");
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].values[0].name = "quad_enable";
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].values[0].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_fabric_quad_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_fabric_quad_info_t, quad_enable);

    
    submodule_data->tables[dnx_data_port_static_add_table_header_type].name = "header_type";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_header_type].size_of_values = sizeof(dnx_data_port_static_add_header_type_t);
    submodule_data->tables[dnx_data_port_static_add_table_header_type].entry_get = dnx_data_port_static_add_header_type_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_header_type].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_header_type].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_static_add_table_header_type].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_header_type].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_header_type].nof_values, "_dnx_data_port_static_add_table_header_type table values");
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[0].name = "header_type_in";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[0].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_header_type_t, header_type_in);
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[1].name = "header_type_out";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[1].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[1].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_header_type_t, header_type_out);
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[2].name = "header_type";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[2].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[2].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_header_type].values[2].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_header_type_t, header_type);

    
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].name = "erp_exist";
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].size_of_values = sizeof(dnx_data_port_static_add_erp_exist_t);
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].entry_get = dnx_data_port_static_add_erp_exist_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].keys[0].name = "core";
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_erp_exist].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_erp_exist].nof_values, "_dnx_data_port_static_add_table_erp_exist table values");
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].values[0].name = "exist";
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].values[0].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_erp_exist].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_erp_exist_t, exist);

    
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].name = "base_flexe_instance";
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].size_of_values = sizeof(dnx_data_port_static_add_base_flexe_instance_t);
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].entry_get = dnx_data_port_static_add_base_flexe_instance_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].nof_values, "_dnx_data_port_static_add_table_base_flexe_instance table values");
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].values[0].name = "val";
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].values[0].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_base_flexe_instance].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_base_flexe_instance_t, val);

    
    submodule_data->tables[dnx_data_port_static_add_table_cable_swap_info].name = "cable_swap_info";
    submodule_data->tables[dnx_data_port_static_add_table_cable_swap_info].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_cable_swap_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_cable_swap_info].size_of_values = sizeof(dnx_data_port_static_add_cable_swap_info_t);
    submodule_data->tables[dnx_data_port_static_add_table_cable_swap_info].entry_get = dnx_data_port_static_add_cable_swap_info_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_cable_swap_info].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_cable_swap_info].keys[0].name = "pm";
    submodule_data->tables[dnx_data_port_static_add_table_cable_swap_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_static_add_table_cable_swap_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_cable_swap_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_cable_swap_info].nof_values, "_dnx_data_port_static_add_table_cable_swap_info table values");
    submodule_data->tables[dnx_data_port_static_add_table_cable_swap_info].values[0].name = "enable";
    submodule_data->tables[dnx_data_port_static_add_table_cable_swap_info].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_cable_swap_info].values[0].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_cable_swap_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_cable_swap_info_t, enable);
    submodule_data->tables[dnx_data_port_static_add_table_cable_swap_info].values[1].name = "master";
    submodule_data->tables[dnx_data_port_static_add_table_cable_swap_info].values[1].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_cable_swap_info].values[1].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_cable_swap_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_cable_swap_info_t, master);

    
    submodule_data->tables[dnx_data_port_static_add_table_flr_config].name = "flr_config";
    submodule_data->tables[dnx_data_port_static_add_table_flr_config].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_flr_config].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_flr_config].size_of_values = sizeof(dnx_data_port_static_add_flr_config_t);
    submodule_data->tables[dnx_data_port_static_add_table_flr_config].entry_get = dnx_data_port_static_add_flr_config_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_flr_config].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_flr_config].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_flr_config].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_static_add_table_flr_config].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_flr_config].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_flr_config].nof_values, "_dnx_data_port_static_add_table_flr_config table values");
    submodule_data->tables[dnx_data_port_static_add_table_flr_config].values[0].name = "flr_fw_support";
    submodule_data->tables[dnx_data_port_static_add_table_flr_config].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_flr_config].values[0].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_flr_config].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_flr_config_t, flr_fw_support);
    submodule_data->tables[dnx_data_port_static_add_table_flr_config].values[1].name = "timeout";
    submodule_data->tables[dnx_data_port_static_add_table_flr_config].values[1].type = "bcm_port_flr_timeout_t";
    submodule_data->tables[dnx_data_port_static_add_table_flr_config].values[1].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_flr_config].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_flr_config_t, timeout);

    
    submodule_data->tables[dnx_data_port_static_add_table_timestamp_phy_init].name = "timestamp_phy_init";
    submodule_data->tables[dnx_data_port_static_add_table_timestamp_phy_init].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_timestamp_phy_init].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_timestamp_phy_init].size_of_values = sizeof(dnx_data_port_static_add_timestamp_phy_init_t);
    submodule_data->tables[dnx_data_port_static_add_table_timestamp_phy_init].entry_get = dnx_data_port_static_add_timestamp_phy_init_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_timestamp_phy_init].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_timestamp_phy_init].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_timestamp_phy_init].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_static_add_table_timestamp_phy_init].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_timestamp_phy_init].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_timestamp_phy_init].nof_values, "_dnx_data_port_static_add_table_timestamp_phy_init table values");
    submodule_data->tables[dnx_data_port_static_add_table_timestamp_phy_init].values[0].name = "timestamp_enable";
    submodule_data->tables[dnx_data_port_static_add_table_timestamp_phy_init].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_timestamp_phy_init].values[0].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_timestamp_phy_init].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_timestamp_phy_init_t, timestamp_enable);
    submodule_data->tables[dnx_data_port_static_add_table_timestamp_phy_init].values[1].name = "one_step_enable";
    submodule_data->tables[dnx_data_port_static_add_table_timestamp_phy_init].values[1].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_timestamp_phy_init].values[1].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_timestamp_phy_init].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_timestamp_phy_init_t, one_step_enable);

    
    submodule_data->tables[dnx_data_port_static_add_table_txpi_mode].name = "txpi_mode";
    submodule_data->tables[dnx_data_port_static_add_table_txpi_mode].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_txpi_mode].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_static_add_table_txpi_mode].size_of_values = sizeof(dnx_data_port_static_add_txpi_mode_t);
    submodule_data->tables[dnx_data_port_static_add_table_txpi_mode].entry_get = dnx_data_port_static_add_txpi_mode_entry_str_get;

    
    submodule_data->tables[dnx_data_port_static_add_table_txpi_mode].nof_keys = 1;
    submodule_data->tables[dnx_data_port_static_add_table_txpi_mode].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_static_add_table_txpi_mode].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_static_add_table_txpi_mode].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_static_add_table_txpi_mode].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_static_add_table_txpi_mode].nof_values, "_dnx_data_port_static_add_table_txpi_mode table values");
    submodule_data->tables[dnx_data_port_static_add_table_txpi_mode].values[0].name = "val";
    submodule_data->tables[dnx_data_port_static_add_table_txpi_mode].values[0].type = "int";
    submodule_data->tables[dnx_data_port_static_add_table_txpi_mode].values[0].doc = "";
    submodule_data->tables[dnx_data_port_static_add_table_txpi_mode].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_static_add_txpi_mode_t, val);


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

const dnx_data_port_static_add_cable_swap_info_t *
dnx_data_port_static_add_cable_swap_info_get(
    int unit,
    int pm)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_cable_swap_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, pm, 0);
    return (const dnx_data_port_static_add_cable_swap_info_t *) data;

}

const dnx_data_port_static_add_flr_config_t *
dnx_data_port_static_add_flr_config_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_flr_config);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnx_data_port_static_add_flr_config_t *) data;

}

const dnx_data_port_static_add_timestamp_phy_init_t *
dnx_data_port_static_add_timestamp_phy_init_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_timestamp_phy_init);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnx_data_port_static_add_timestamp_phy_init_t *) data;

}

const dnx_data_port_static_add_txpi_mode_t *
dnx_data_port_static_add_txpi_mode_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_txpi_mode);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnx_data_port_static_add_txpi_mode_t *) data;

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
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->out_tm_port);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->channel);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_stif);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_stif_data);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_mc);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_kbp);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_cross_connect);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_flexe_phy);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_flexe_unaware);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_ingress_interleave);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_egress_interleave);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_tdm);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_if_tdm_only);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_if_tdm_hybrid);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->sch_priority_propagation_en);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->frame_preemptable);
            break;
        case 21:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->num_priorities);
            break;
        case 22:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->base_q_pair);
            break;
        case 23:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->num_sch_priorities);
            break;
        case 24:
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
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pre3);
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

shr_error_e
dnx_data_port_static_add_cable_swap_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_static_add_cable_swap_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_cable_swap_info);
    data = (const dnx_data_port_static_add_cable_swap_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
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

shr_error_e
dnx_data_port_static_add_flr_config_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_static_add_flr_config_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_flr_config);
    data = (const dnx_data_port_static_add_flr_config_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->flr_fw_support);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->timeout);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_port_static_add_timestamp_phy_init_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_static_add_timestamp_phy_init_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_timestamp_phy_init);
    data = (const dnx_data_port_static_add_timestamp_phy_init_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->timestamp_enable);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->one_step_enable);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_port_static_add_txpi_mode_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_static_add_txpi_mode_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_txpi_mode);
    data = (const dnx_data_port_static_add_txpi_mode_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
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

const dnxc_data_table_info_t *
dnx_data_port_static_add_cable_swap_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_cable_swap_info);

}

const dnxc_data_table_info_t *
dnx_data_port_static_add_flr_config_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_flr_config);

}

const dnxc_data_table_info_t *
dnx_data_port_static_add_timestamp_phy_init_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_timestamp_phy_init);

}

const dnxc_data_table_info_t *
dnx_data_port_static_add_txpi_mode_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_static_add, dnx_data_port_static_add_table_txpi_mode);

}






static shr_error_e
dnx_data_port_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_port_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data port general features");

    submodule_data->features[dnx_data_port_general_unrestricted_portmod_pll].name = "unrestricted_portmod_pll";
    submodule_data->features[dnx_data_port_general_unrestricted_portmod_pll].doc = "";
    submodule_data->features[dnx_data_port_general_unrestricted_portmod_pll].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_port_general_use_portmod_speed_when_not_enabled].name = "use_portmod_speed_when_not_enabled";
    submodule_data->features[dnx_data_port_general_use_portmod_speed_when_not_enabled].doc = "";
    submodule_data->features[dnx_data_port_general_use_portmod_speed_when_not_enabled].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_port_general_is_cable_swap_supported].name = "is_cable_swap_supported";
    submodule_data->features[dnx_data_port_general_is_cable_swap_supported].doc = "";
    submodule_data->features[dnx_data_port_general_is_cable_swap_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_port_general_is_header_type_rch_srv6_usp_psp_supported].name = "is_header_type_rch_srv6_usp_psp_supported";
    submodule_data->features[dnx_data_port_general_is_header_type_rch_srv6_usp_psp_supported].doc = "";
    submodule_data->features[dnx_data_port_general_is_header_type_rch_srv6_usp_psp_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_port_general_is_rch_header_offset_supported].name = "is_rch_header_offset_supported";
    submodule_data->features[dnx_data_port_general_is_rch_header_offset_supported].doc = "";
    submodule_data->features[dnx_data_port_general_is_rch_header_offset_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_port_general_is_phy_rx_taps_supported].name = "is_phy_rx_taps_supported";
    submodule_data->features[dnx_data_port_general_is_phy_rx_taps_supported].doc = "";
    submodule_data->features[dnx_data_port_general_is_phy_rx_taps_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_port_general_is_serdes_tx_soc_prop_dict_format_supported].name = "is_serdes_tx_soc_prop_dict_format_supported";
    submodule_data->features[dnx_data_port_general_is_serdes_tx_soc_prop_dict_format_supported].doc = "";
    submodule_data->features[dnx_data_port_general_is_serdes_tx_soc_prop_dict_format_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_port_general_is_fabric_signal_quality_supported].name = "is_fabric_signal_quality_supported";
    submodule_data->features[dnx_data_port_general_is_fabric_signal_quality_supported].doc = "";
    submodule_data->features[dnx_data_port_general_is_fabric_signal_quality_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_port_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data port general defines");

    submodule_data->defines[dnx_data_port_general_define_erp_pp_dsp].name = "erp_pp_dsp";
    submodule_data->defines[dnx_data_port_general_define_erp_pp_dsp].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_erp_pp_dsp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_max_nof_channels].name = "max_nof_channels";
    submodule_data->defines[dnx_data_port_general_define_max_nof_channels].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_max_nof_channels].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_fabric_phys_offset].name = "fabric_phys_offset";
    submodule_data->defines[dnx_data_port_general_define_fabric_phys_offset].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_fabric_phys_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_pp_dsp_size].name = "pp_dsp_size";
    submodule_data->defines[dnx_data_port_general_define_pp_dsp_size].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_pp_dsp_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_ingress_vlan_domain_size].name = "ingress_vlan_domain_size";
    submodule_data->defines[dnx_data_port_general_define_ingress_vlan_domain_size].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_ingress_vlan_domain_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_vlan_domain_size].name = "vlan_domain_size";
    submodule_data->defines[dnx_data_port_general_define_vlan_domain_size].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_vlan_domain_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_ffc_instruction_size].name = "ffc_instruction_size";
    submodule_data->defines[dnx_data_port_general_define_ffc_instruction_size].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_ffc_instruction_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_ffc_instruction_offset_size].name = "ffc_instruction_offset_size";
    submodule_data->defines[dnx_data_port_general_define_ffc_instruction_offset_size].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_ffc_instruction_offset_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_ffc_instruction_width_size].name = "ffc_instruction_width_size";
    submodule_data->defines[dnx_data_port_general_define_ffc_instruction_width_size].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_ffc_instruction_width_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_ffc_instruction_input_offset_size].name = "ffc_instruction_input_offset_size";
    submodule_data->defines[dnx_data_port_general_define_ffc_instruction_input_offset_size].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_ffc_instruction_input_offset_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_max_first_header_size_to_skip].name = "max_first_header_size_to_skip";
    submodule_data->defines[dnx_data_port_general_define_max_first_header_size_to_skip].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_max_first_header_size_to_skip].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_max_first_header_size_to_skip_before].name = "max_first_header_size_to_skip_before";
    submodule_data->defines[dnx_data_port_general_define_max_first_header_size_to_skip_before].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_max_first_header_size_to_skip_before].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_max_first_header_size_to_skip_after].name = "max_first_header_size_to_skip_after";
    submodule_data->defines[dnx_data_port_general_define_max_first_header_size_to_skip_after].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_max_first_header_size_to_skip_after].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_prt_ffc_width_start_offset].name = "prt_ffc_width_start_offset";
    submodule_data->defines[dnx_data_port_general_define_prt_ffc_width_start_offset].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_prt_ffc_width_start_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_prt_ffc_start_input_offset].name = "prt_ffc_start_input_offset";
    submodule_data->defines[dnx_data_port_general_define_prt_ffc_start_input_offset].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_prt_ffc_start_input_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_max_phys_per_pm].name = "max_phys_per_pm";
    submodule_data->defines[dnx_data_port_general_define_max_phys_per_pm].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_max_phys_per_pm].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_max_core_access_per_port].name = "max_core_access_per_port";
    submodule_data->defines[dnx_data_port_general_define_max_core_access_per_port].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_max_core_access_per_port].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_general_define_nof_pp_dsp_per_fap_id].name = "nof_pp_dsp_per_fap_id";
    submodule_data->defines[dnx_data_port_general_define_nof_pp_dsp_per_fap_id].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_nof_pp_dsp_per_fap_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_nof_ptc].name = "nof_ptc";
    submodule_data->defines[dnx_data_port_general_define_nof_ptc].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_nof_ptc].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_nof_in_tm_ports].name = "nof_in_tm_ports";
    submodule_data->defines[dnx_data_port_general_define_nof_in_tm_ports].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_nof_in_tm_ports].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_nof_out_tm_ports].name = "nof_out_tm_ports";
    submodule_data->defines[dnx_data_port_general_define_nof_out_tm_ports].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_nof_out_tm_ports].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_nof_pp_dsp].name = "nof_pp_dsp";
    submodule_data->defines[dnx_data_port_general_define_nof_pp_dsp].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_nof_pp_dsp].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_nof_pp_ports].name = "nof_pp_ports";
    submodule_data->defines[dnx_data_port_general_define_nof_pp_ports].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_nof_pp_ports].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_reserved_pp_dsp].name = "reserved_pp_dsp";
    submodule_data->defines[dnx_data_port_general_define_reserved_pp_dsp].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_reserved_pp_dsp].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_nof_vlan_membership_if].name = "nof_vlan_membership_if";
    submodule_data->defines[dnx_data_port_general_define_nof_vlan_membership_if].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_nof_vlan_membership_if].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_max_vlan_membership_if].name = "max_vlan_membership_if";
    submodule_data->defines[dnx_data_port_general_define_max_vlan_membership_if].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_max_vlan_membership_if].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_fabric_port_base].name = "fabric_port_base";
    submodule_data->defines[dnx_data_port_general_define_fabric_port_base].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_fabric_port_base].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_pp_port_bits_size].name = "pp_port_bits_size";
    submodule_data->defines[dnx_data_port_general_define_pp_port_bits_size].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_pp_port_bits_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_nof_port_bits_in_pp_bus].name = "nof_port_bits_in_pp_bus";
    submodule_data->defines[dnx_data_port_general_define_nof_port_bits_in_pp_bus].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_nof_port_bits_in_pp_bus].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_port_general_define_nof_bits_egress_vlan_membership_if].name = "nof_bits_egress_vlan_membership_if";
    submodule_data->defines[dnx_data_port_general_define_nof_bits_egress_vlan_membership_if].doc = "";
    
    submodule_data->defines[dnx_data_port_general_define_nof_bits_egress_vlan_membership_if].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_port_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port general tables");

    
    submodule_data->tables[dnx_data_port_general_table_first_header_size].name = "first_header_size";
    submodule_data->tables[dnx_data_port_general_table_first_header_size].doc = "";
    submodule_data->tables[dnx_data_port_general_table_first_header_size].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_general_table_first_header_size].size_of_values = sizeof(dnx_data_port_general_first_header_size_t);
    submodule_data->tables[dnx_data_port_general_table_first_header_size].entry_get = dnx_data_port_general_first_header_size_entry_str_get;

    
    submodule_data->tables[dnx_data_port_general_table_first_header_size].nof_keys = 1;
    submodule_data->tables[dnx_data_port_general_table_first_header_size].keys[0].name = "port";
    submodule_data->tables[dnx_data_port_general_table_first_header_size].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_general_table_first_header_size].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_general_table_first_header_size].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_general_table_first_header_size].nof_values, "_dnx_data_port_general_table_first_header_size table values");
    submodule_data->tables[dnx_data_port_general_table_first_header_size].values[0].name = "first_header_size";
    submodule_data->tables[dnx_data_port_general_table_first_header_size].values[0].type = "int";
    submodule_data->tables[dnx_data_port_general_table_first_header_size].values[0].doc = "";
    submodule_data->tables[dnx_data_port_general_table_first_header_size].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_general_first_header_size_t, first_header_size);

    
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].name = "fec_compensation_info";
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].doc = "";
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].size_of_values = sizeof(dnx_data_port_general_fec_compensation_info_t);
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].entry_get = dnx_data_port_general_fec_compensation_info_entry_str_get;

    
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].nof_keys = 1;
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].keys[0].name = "fec_type";
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].nof_values, "_dnx_data_port_general_table_fec_compensation_info table values");
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].values[0].name = "counted_bits";
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].values[0].type = "uint32";
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].values[0].doc = "";
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_general_fec_compensation_info_t, counted_bits);
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].values[1].name = "total_bits";
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].values[1].doc = "";
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_general_fec_compensation_info_t, total_bits);
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].values[2].name = "max_nof_correctable_symbols";
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].values[2].doc = "";
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_port_general_fec_compensation_info_t, max_nof_correctable_symbols);
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].values[3].name = "is_valid";
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].values[3].type = "uint32";
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].values[3].doc = "";
    submodule_data->tables[dnx_data_port_general_table_fec_compensation_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_port_general_fec_compensation_info_t, is_valid);

    
    submodule_data->tables[dnx_data_port_general_table_pm_info].name = "pm_info";
    submodule_data->tables[dnx_data_port_general_table_pm_info].doc = "";
    submodule_data->tables[dnx_data_port_general_table_pm_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_general_table_pm_info].size_of_values = sizeof(dnx_data_port_general_pm_info_t);
    submodule_data->tables[dnx_data_port_general_table_pm_info].entry_get = dnx_data_port_general_pm_info_entry_str_get;

    
    submodule_data->tables[dnx_data_port_general_table_pm_info].nof_keys = 1;
    submodule_data->tables[dnx_data_port_general_table_pm_info].keys[0].name = "pm_type";
    submodule_data->tables[dnx_data_port_general_table_pm_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_general_table_pm_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_general_table_pm_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_general_table_pm_info].nof_values, "_dnx_data_port_general_table_pm_info table values");
    submodule_data->tables[dnx_data_port_general_table_pm_info].values[0].name = "is_serdes_tx_pre3_supported";
    submodule_data->tables[dnx_data_port_general_table_pm_info].values[0].type = "int";
    submodule_data->tables[dnx_data_port_general_table_pm_info].values[0].doc = "";
    submodule_data->tables[dnx_data_port_general_table_pm_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_general_pm_info_t, is_serdes_tx_pre3_supported);
    submodule_data->tables[dnx_data_port_general_table_pm_info].values[1].name = "is_scrambling_disable_supported";
    submodule_data->tables[dnx_data_port_general_table_pm_info].values[1].type = "int";
    submodule_data->tables[dnx_data_port_general_table_pm_info].values[1].doc = "";
    submodule_data->tables[dnx_data_port_general_table_pm_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_general_pm_info_t, is_scrambling_disable_supported);


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
dnx_data_port_general_erp_pp_dsp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_erp_pp_dsp);
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
dnx_data_port_general_ffc_instruction_offset_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_ffc_instruction_offset_size);
}

uint32
dnx_data_port_general_ffc_instruction_width_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_ffc_instruction_width_size);
}

uint32
dnx_data_port_general_ffc_instruction_input_offset_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_ffc_instruction_input_offset_size);
}

uint32
dnx_data_port_general_max_first_header_size_to_skip_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_max_first_header_size_to_skip);
}

uint32
dnx_data_port_general_max_first_header_size_to_skip_before_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_max_first_header_size_to_skip_before);
}

uint32
dnx_data_port_general_max_first_header_size_to_skip_after_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_max_first_header_size_to_skip_after);
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
dnx_data_port_general_max_phys_per_pm_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_max_phys_per_pm);
}

uint32
dnx_data_port_general_max_core_access_per_port_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_max_core_access_per_port);
}

uint32
dnx_data_port_general_nof_pp_dsp_per_fap_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_nof_pp_dsp_per_fap_id);
}

uint32
dnx_data_port_general_nof_ptc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_nof_ptc);
}

uint32
dnx_data_port_general_nof_in_tm_ports_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_nof_in_tm_ports);
}

uint32
dnx_data_port_general_nof_out_tm_ports_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_nof_out_tm_ports);
}

uint32
dnx_data_port_general_nof_pp_dsp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_nof_pp_dsp);
}

uint32
dnx_data_port_general_nof_pp_ports_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_nof_pp_ports);
}

uint32
dnx_data_port_general_reserved_pp_dsp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_reserved_pp_dsp);
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
dnx_data_port_general_nof_bits_egress_vlan_membership_if_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_define_nof_bits_egress_vlan_membership_if);
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

const dnx_data_port_general_fec_compensation_info_t *
dnx_data_port_general_fec_compensation_info_get(
    int unit,
    int fec_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_table_fec_compensation_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, fec_type, 0);
    return (const dnx_data_port_general_fec_compensation_info_t *) data;

}

const dnx_data_port_general_pm_info_t *
dnx_data_port_general_pm_info_get(
    int unit,
    int pm_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_table_pm_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, pm_type, 0);
    return (const dnx_data_port_general_pm_info_t *) data;

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

shr_error_e
dnx_data_port_general_fec_compensation_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_general_fec_compensation_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_table_fec_compensation_info);
    data = (const dnx_data_port_general_fec_compensation_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->counted_bits);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->total_bits);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max_nof_correctable_symbols);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_port_general_pm_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_port_general_pm_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_table_pm_info);
    data = (const dnx_data_port_general_pm_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_serdes_tx_pre3_supported);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_scrambling_disable_supported);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_port_general_first_header_size_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_table_first_header_size);

}

const dnxc_data_table_info_t *
dnx_data_port_general_fec_compensation_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_table_fec_compensation_info);

}

const dnxc_data_table_info_t *
dnx_data_port_general_pm_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_port, dnx_data_port_submodule_general, dnx_data_port_general_table_pm_info);

}






static shr_error_e
dnx_data_port_imb_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "imb";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_port_imb_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data port imb features");

    
    submodule_data->nof_defines = _dnx_data_port_imb_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data port imb defines");

    
    submodule_data->nof_tables = _dnx_data_port_imb_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port imb tables");

    
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].name = "imb_type_info";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].doc = "";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].size_of_values = sizeof(dnx_data_port_imb_imb_type_info_t);
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].entry_get = dnx_data_port_imb_imb_type_info_entry_str_get;

    
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].nof_keys = 1;
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].keys[0].name = "type";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].nof_values = 8;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_port_imb_table_imb_type_info].nof_values, "_dnx_data_port_imb_table_imb_type_info table values");
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[0].name = "pm_type";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[0].type = "portmod_dispatch_type_t";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[0].doc = "";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_port_imb_imb_type_info_t, pm_type);
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[1].name = "nof_pms";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[1].doc = "";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_port_imb_imb_type_info_t, nof_pms);
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[2].name = "nof_lanes";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[2].doc = "";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_port_imb_imb_type_info_t, nof_lanes);
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[3].name = "nof_lanes_in_pm";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[3].type = "uint32";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[3].doc = "";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_port_imb_imb_type_info_t, nof_lanes_in_pm);
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[4].name = "nof_memory_entries";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[4].type = "uint32";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[4].doc = "";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[4].offset = UTILEX_OFFSETOF(dnx_data_port_imb_imb_type_info_t, nof_memory_entries);
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[5].name = "rate_per_rmc_bit";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[5].type = "uint32";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[5].doc = "";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[5].offset = UTILEX_OFFSETOF(dnx_data_port_imb_imb_type_info_t, rate_per_rmc_bit);
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[6].name = "precoder_supported";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[6].type = "uint32";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[6].doc = "";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[6].offset = UTILEX_OFFSETOF(dnx_data_port_imb_imb_type_info_t, precoder_supported);
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[7].name = "mib_type";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[7].type = "dnx_algo_port_mib_counter_set_type_e";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[7].doc = "";
    submodule_data->tables[dnx_data_port_imb_table_imb_type_info].values[7].offset = UTILEX_OFFSETOF(dnx_data_port_imb_imb_type_info_t, mib_type);


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
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pm_type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_pms);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_lanes);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_lanes_in_pm);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_memory_entries);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->rate_per_rmc_bit);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->precoder_supported);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mib_type);
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






static shr_error_e
dnx_data_port_fabric_signal_quality_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "fabric_signal_quality";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_port_fabric_signal_quality_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data port fabric_signal_quality features");

    
    submodule_data->nof_defines = _dnx_data_port_fabric_signal_quality_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data port fabric_signal_quality defines");

    submodule_data->defines[dnx_data_port_fabric_signal_quality_define_max_interval_in_cycles].name = "max_interval_in_cycles";
    submodule_data->defines[dnx_data_port_fabric_signal_quality_define_max_interval_in_cycles].doc = "";
    
    submodule_data->defines[dnx_data_port_fabric_signal_quality_define_max_interval_in_cycles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_fabric_signal_quality_define_max_degrade_threshold].name = "max_degrade_threshold";
    submodule_data->defines[dnx_data_port_fabric_signal_quality_define_max_degrade_threshold].doc = "";
    
    submodule_data->defines[dnx_data_port_fabric_signal_quality_define_max_degrade_threshold].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_fabric_signal_quality_define_min_degrade_threshold].name = "min_degrade_threshold";
    submodule_data->defines[dnx_data_port_fabric_signal_quality_define_min_degrade_threshold].doc = "";
    
    submodule_data->defines[dnx_data_port_fabric_signal_quality_define_min_degrade_threshold].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_fabric_signal_quality_define_max_fail_threshold].name = "max_fail_threshold";
    submodule_data->defines[dnx_data_port_fabric_signal_quality_define_max_fail_threshold].doc = "";
    
    submodule_data->defines[dnx_data_port_fabric_signal_quality_define_max_fail_threshold].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_fabric_signal_quality_define_min_fail_threshold].name = "min_fail_threshold";
    submodule_data->defines[dnx_data_port_fabric_signal_quality_define_min_fail_threshold].doc = "";
    
    submodule_data->defines[dnx_data_port_fabric_signal_quality_define_min_fail_threshold].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_fabric_signal_quality_define_max_interval].name = "max_interval";
    submodule_data->defines[dnx_data_port_fabric_signal_quality_define_max_interval].doc = "";
    
    submodule_data->defines[dnx_data_port_fabric_signal_quality_define_max_interval].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_fabric_signal_quality_define_min_interval].name = "min_interval";
    submodule_data->defines[dnx_data_port_fabric_signal_quality_define_min_interval].doc = "";
    
    submodule_data->defines[dnx_data_port_fabric_signal_quality_define_min_interval].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_port_fabric_signal_quality_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port fabric_signal_quality tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_port_fabric_signal_quality_feature_get(
    int unit,
    dnx_data_port_fabric_signal_quality_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_fabric_signal_quality, feature);
}


uint32
dnx_data_port_fabric_signal_quality_max_interval_in_cycles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_fabric_signal_quality, dnx_data_port_fabric_signal_quality_define_max_interval_in_cycles);
}

uint32
dnx_data_port_fabric_signal_quality_max_degrade_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_fabric_signal_quality, dnx_data_port_fabric_signal_quality_define_max_degrade_threshold);
}

uint32
dnx_data_port_fabric_signal_quality_min_degrade_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_fabric_signal_quality, dnx_data_port_fabric_signal_quality_define_min_degrade_threshold);
}

uint32
dnx_data_port_fabric_signal_quality_max_fail_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_fabric_signal_quality, dnx_data_port_fabric_signal_quality_define_max_fail_threshold);
}

uint32
dnx_data_port_fabric_signal_quality_min_fail_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_fabric_signal_quality, dnx_data_port_fabric_signal_quality_define_min_fail_threshold);
}

uint32
dnx_data_port_fabric_signal_quality_max_interval_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_fabric_signal_quality, dnx_data_port_fabric_signal_quality_define_max_interval);
}

uint32
dnx_data_port_fabric_signal_quality_min_interval_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port, dnx_data_port_submodule_fabric_signal_quality, dnx_data_port_fabric_signal_quality_define_min_interval);
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
    SHR_IF_ERR_EXIT(dnx_data_port_imb_init(unit, &module_data->submodules[dnx_data_port_submodule_imb]));
    SHR_IF_ERR_EXIT(dnx_data_port_fabric_signal_quality_init(unit, &module_data->submodules[dnx_data_port_submodule_fabric_signal_quality]));
    

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


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_port_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_port_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_port_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

