

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dram.h>



extern shr_error_e jr2_a0_data_dram_attach(
    int unit);
extern shr_error_e jr2_b0_data_dram_attach(
    int unit);
extern shr_error_e j2c_a0_data_dram_attach(
    int unit);
extern shr_error_e q2a_a0_data_dram_attach(
    int unit);
extern shr_error_e q2a_b0_data_dram_attach(
    int unit);
extern shr_error_e q2a_b1_data_dram_attach(
    int unit);
extern shr_error_e j2p_a0_data_dram_attach(
    int unit);



static shr_error_e
dnx_data_dram_hbm_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "hbm";
    submodule_data->doc = "HBM parameters and defines";
    
    submodule_data->nof_features = _dnx_data_dram_hbm_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dram hbm features");

    submodule_data->features[dnx_data_dram_hbm_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_dram_hbm_is_supported].doc = "hbm supported indication";
    submodule_data->features[dnx_data_dram_hbm_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_dword_alignment_check].name = "dword_alignment_check";
    submodule_data->features[dnx_data_dram_hbm_dword_alignment_check].doc = "indication if dword alignemnt check is required for this architecture";
    submodule_data->features[dnx_data_dram_hbm_dword_alignment_check].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_low_temperature_dram_protction].name = "low_temperature_dram_protction";
    submodule_data->features[dnx_data_dram_hbm_low_temperature_dram_protction].doc = "Indicates whether there is a 'low-temperature' forbidden range for HBM";
    submodule_data->features[dnx_data_dram_hbm_low_temperature_dram_protction].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_is_delete_bdb_supported].name = "is_delete_bdb_supported";
    submodule_data->features[dnx_data_dram_hbm_is_delete_bdb_supported].doc = "Deleting dram buffers (marking them as faulty) is possible during 'init sequence'";
    submodule_data->features[dnx_data_dram_hbm_is_delete_bdb_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_cpu2tap_access].name = "cpu2tap_access";
    submodule_data->features[dnx_data_dram_hbm_cpu2tap_access].doc = "Indication whether cpu2tap access to the HBM is supported";
    submodule_data->features[dnx_data_dram_hbm_cpu2tap_access].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_seamless_commands_to_same_bank_control].name = "seamless_commands_to_same_bank_control";
    submodule_data->features[dnx_data_dram_hbm_seamless_commands_to_same_bank_control].doc = "Indication whether seamless commands to same bank control is supported";
    submodule_data->features[dnx_data_dram_hbm_seamless_commands_to_same_bank_control].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_vendor_info_disable].name = "vendor_info_disable";
    submodule_data->features[dnx_data_dram_hbm_vendor_info_disable].doc = "Reading vendor info is not supported since it can cause cpu2tap problems";
    submodule_data->features[dnx_data_dram_hbm_vendor_info_disable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_apd_phy].name = "apd_phy";
    submodule_data->features[dnx_data_dram_hbm_apd_phy].doc = "The HBM PHY was provided by the APD group in Broadcom";
    submodule_data->features[dnx_data_dram_hbm_apd_phy].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_is_hrc_supported].name = "is_hrc_supported";
    submodule_data->features[dnx_data_dram_hbm_is_hrc_supported].doc = "The channels can be reached by the 'remote controller' mechanism (which includes HRC blocks and 'fake' TDU blocks)";
    submodule_data->features[dnx_data_dram_hbm_is_hrc_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_channel_wds_inflight_threshold].name = "channel_wds_inflight_threshold";
    submodule_data->features[dnx_data_dram_hbm_channel_wds_inflight_threshold].doc = "A threshold needs to be set upon the Write Data Source memory in order to prevent WDS overflow";
    submodule_data->features[dnx_data_dram_hbm_channel_wds_inflight_threshold].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_dram_hbm_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dram hbm defines");

    submodule_data->defines[dnx_data_dram_hbm_define_burst_length].name = "burst_length";
    submodule_data->defines[dnx_data_dram_hbm_define_burst_length].doc = "burst length";
    
    submodule_data->defines[dnx_data_dram_hbm_define_burst_length].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_stop_traffic_temp_threshold].name = "stop_traffic_temp_threshold";
    submodule_data->defines[dnx_data_dram_hbm_define_stop_traffic_temp_threshold].doc = "temperature threshold (C degrees) above which the HBM should not recieve packets anymore";
    
    submodule_data->defines[dnx_data_dram_hbm_define_stop_traffic_temp_threshold].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_stop_traffic_low_temp_threshold].name = "stop_traffic_low_temp_threshold";
    submodule_data->defines[dnx_data_dram_hbm_define_stop_traffic_low_temp_threshold].doc = "Low temperature threshold (C degrees) below which the HBM should not recieve packets anymore";
    
    submodule_data->defines[dnx_data_dram_hbm_define_stop_traffic_low_temp_threshold].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_restore_traffic_temp_threshold].name = "restore_traffic_temp_threshold";
    submodule_data->defines[dnx_data_dram_hbm_define_restore_traffic_temp_threshold].doc = "temperature threshold (C degrees) below which the HBM should be reassigned to usage in the traffic flow";
    
    submodule_data->defines[dnx_data_dram_hbm_define_restore_traffic_temp_threshold].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_restore_traffic_low_temp_threshold].name = "restore_traffic_low_temp_threshold";
    submodule_data->defines[dnx_data_dram_hbm_define_restore_traffic_low_temp_threshold].doc = "Low temperature threshold (C degrees) above which the HBM should be reassigned to usage in the traffic flow";
    
    submodule_data->defines[dnx_data_dram_hbm_define_restore_traffic_low_temp_threshold].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_usec_between_temp_samples].name = "usec_between_temp_samples";
    submodule_data->defines[dnx_data_dram_hbm_define_usec_between_temp_samples].doc = "time in usecs to wait between HBM temperature sampling";
    
    submodule_data->defines[dnx_data_dram_hbm_define_usec_between_temp_samples].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_power_down_temp_threshold].name = "power_down_temp_threshold";
    submodule_data->defines[dnx_data_dram_hbm_define_power_down_temp_threshold].doc = "temperature threshold, for doing DRAM shut down";
    
    submodule_data->defines[dnx_data_dram_hbm_define_power_down_temp_threshold].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_power_down_low_temp_threshold].name = "power_down_low_temp_threshold";
    submodule_data->defines[dnx_data_dram_hbm_define_power_down_low_temp_threshold].doc = "Low temperature threshold, for doing DRAM shut down";
    
    submodule_data->defines[dnx_data_dram_hbm_define_power_down_low_temp_threshold].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_dram_temp_monitor_enable].name = "dram_temp_monitor_enable";
    submodule_data->defines[dnx_data_dram_hbm_define_dram_temp_monitor_enable].doc = "enable/disable driver temperature monitoring";
    
    submodule_data->defines[dnx_data_dram_hbm_define_dram_temp_monitor_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_start_disabled].name = "start_disabled";
    submodule_data->defines[dnx_data_dram_hbm_define_start_disabled].doc = "do not start DRAM during BCM init";
    
    submodule_data->defines[dnx_data_dram_hbm_define_start_disabled].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_output_enable_length].name = "output_enable_length";
    submodule_data->defines[dnx_data_dram_hbm_define_output_enable_length].doc = "Output enable length pipeline param";
    
    submodule_data->defines[dnx_data_dram_hbm_define_output_enable_length].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_output_enable_delay].name = "output_enable_delay";
    submodule_data->defines[dnx_data_dram_hbm_define_output_enable_delay].doc = "Output enable delay pipeline param";
    
    submodule_data->defines[dnx_data_dram_hbm_define_output_enable_delay].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_default_model_part_num].name = "default_model_part_num";
    submodule_data->defines[dnx_data_dram_hbm_define_default_model_part_num].doc = "default model part number";
    
    submodule_data->defines[dnx_data_dram_hbm_define_default_model_part_num].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_hbm_define_nof_tdu_per_core].name = "nof_tdu_per_core";
    submodule_data->defines[dnx_data_dram_hbm_define_nof_tdu_per_core].doc = "Number of TDU blocks per core";
    
    submodule_data->defines[dnx_data_dram_hbm_define_nof_tdu_per_core].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_hbm_define_wds_size].name = "wds_size";
    submodule_data->defines[dnx_data_dram_hbm_define_wds_size].doc = "Size of the Write Data Source memory";
    
    submodule_data->defines[dnx_data_dram_hbm_define_wds_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_dram_hbm_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dram hbm tables");

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].name = "channel_symmetric_regs";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].doc = "per channel list of symmetric registers";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].size_of_values = sizeof(dnx_data_dram_hbm_channel_symmetric_regs_t);
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].entry_get = dnx_data_dram_hbm_channel_symmetric_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].nof_values, "_dnx_data_dram_hbm_table_channel_symmetric_regs table values");
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].values[0].doc = "register";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_channel_symmetric_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].name = "channel_not_symmetric_regs";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].doc = "per channel list of not symmetric registers";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].size_of_values = sizeof(dnx_data_dram_hbm_channel_not_symmetric_regs_t);
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].entry_get = dnx_data_dram_hbm_channel_not_symmetric_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].nof_values, "_dnx_data_dram_hbm_table_channel_not_symmetric_regs table values");
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].values[0].doc = "register";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_channel_not_symmetric_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].name = "controller_symmetric_regs";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].doc = "per controller list of symmetric registers";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].size_of_values = sizeof(dnx_data_dram_hbm_controller_symmetric_regs_t);
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].entry_get = dnx_data_dram_hbm_controller_symmetric_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].nof_values, "_dnx_data_dram_hbm_table_controller_symmetric_regs table values");
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].values[0].doc = "register";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_controller_symmetric_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].name = "controller_not_symmetric_regs";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].doc = "per controller list of not symmetric registers";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].size_of_values = sizeof(dnx_data_dram_hbm_controller_not_symmetric_regs_t);
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].entry_get = dnx_data_dram_hbm_controller_not_symmetric_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].nof_values, "_dnx_data_dram_hbm_table_controller_not_symmetric_regs table values");
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].values[0].doc = "register";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_controller_not_symmetric_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].name = "channel_interrupt_regs";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].doc = "per channel list of interrupt registers";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].size_of_values = sizeof(dnx_data_dram_hbm_channel_interrupt_regs_t);
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].entry_get = dnx_data_dram_hbm_channel_interrupt_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].nof_values, "_dnx_data_dram_hbm_table_channel_interrupt_regs table values");
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].values[0].doc = "register";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_channel_interrupt_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].name = "controller_interrupt_regs";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].doc = "per controller list of interrupt registers";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].size_of_values = sizeof(dnx_data_dram_hbm_controller_interrupt_regs_t);
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].entry_get = dnx_data_dram_hbm_controller_interrupt_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].nof_values, "_dnx_data_dram_hbm_table_controller_interrupt_regs table values");
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].values[0].doc = "register";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_controller_interrupt_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].name = "channel_debug_regs";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].doc = "per channel list of debug registers - should be preented if value is not reset value";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].size_of_values = sizeof(dnx_data_dram_hbm_channel_debug_regs_t);
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].entry_get = dnx_data_dram_hbm_channel_debug_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].nof_values, "_dnx_data_dram_hbm_table_channel_debug_regs table values");
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].values[0].doc = "register";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_channel_debug_regs_t, reg);
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].values[1].name = "reassuring_str";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].values[1].type = "char *";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].values[1].doc = "string to explain that this register's value is as expected (reset value)";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_channel_debug_regs_t, reassuring_str);

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].name = "channel_counter_regs";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].doc = "per channel list of counter registers";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].size_of_values = sizeof(dnx_data_dram_hbm_channel_counter_regs_t);
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].entry_get = dnx_data_dram_hbm_channel_counter_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].nof_values, "_dnx_data_dram_hbm_table_channel_counter_regs table values");
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].values[0].doc = "register";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_channel_counter_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].name = "channel_type_regs";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].doc = "per channel list of registers that require per type printing";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].size_of_values = sizeof(dnx_data_dram_hbm_channel_type_regs_t);
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].entry_get = dnx_data_dram_hbm_channel_type_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].nof_values, "_dnx_data_dram_hbm_table_channel_type_regs table values");
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].values[0].doc = "register";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_channel_type_regs_t, reg);
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].values[1].name = "type";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].values[1].type = "dnx_hbmc_diag_registers_type_t";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].values[1].doc = "printing type";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_channel_type_regs_t, type);

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].name = "controller_info_regs";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].doc = "per contorller list of info registers";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].size_of_values = sizeof(dnx_data_dram_hbm_controller_info_regs_t);
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].entry_get = dnx_data_dram_hbm_controller_info_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].nof_values, "_dnx_data_dram_hbm_table_controller_info_regs table values");
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].values[0].doc = "register";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_controller_info_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_hbm_table_bist].name = "bist";
    submodule_data->tables[dnx_data_dram_hbm_table_bist].doc = "BIST configuration";
    submodule_data->tables[dnx_data_dram_hbm_table_bist].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_bist].size_of_values = sizeof(dnx_data_dram_hbm_bist_t);
    submodule_data->tables[dnx_data_dram_hbm_table_bist].entry_get = dnx_data_dram_hbm_bist_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_bist].nof_keys = 0;

    
    submodule_data->tables[dnx_data_dram_hbm_table_bist].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_bist].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_bist].nof_values, "_dnx_data_dram_hbm_table_bist table values");
    submodule_data->tables[dnx_data_dram_hbm_table_bist].values[0].name = "prbs_seeds";
    submodule_data->tables[dnx_data_dram_hbm_table_bist].values[0].type = "uint32[HBMC_SHMOO_BIST_NOF_PRBS_DATA_SEEDS]";
    submodule_data->tables[dnx_data_dram_hbm_table_bist].values[0].doc = "The seed of the PRBS";
    submodule_data->tables[dnx_data_dram_hbm_table_bist].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_bist_t, prbs_seeds);

    
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].name = "hbc_last_in_chain";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].doc = "Mark the last HBC block in SBUS chain";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].size_of_values = sizeof(dnx_data_dram_hbm_hbc_last_in_chain_t);
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].entry_get = dnx_data_dram_hbm_hbc_last_in_chain_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].nof_keys = 2;
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].keys[0].name = "dram_bitmap";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].keys[0].doc = "active drams bitmap";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].keys[1].name = "dram_index";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].keys[1].doc = "dram index";

    
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].nof_values, "_dnx_data_dram_hbm_table_hbc_last_in_chain table values");
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].values[0].name = "is_last_in_chain";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].values[0].doc = "is last in chain";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_hbc_last_in_chain_t, is_last_in_chain);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dram_hbm_feature_get(
    int unit,
    dnx_data_dram_hbm_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, feature);
}


uint32
dnx_data_dram_hbm_burst_length_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_define_burst_length);
}

uint32
dnx_data_dram_hbm_stop_traffic_temp_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_define_stop_traffic_temp_threshold);
}

uint32
dnx_data_dram_hbm_stop_traffic_low_temp_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_define_stop_traffic_low_temp_threshold);
}

uint32
dnx_data_dram_hbm_restore_traffic_temp_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_define_restore_traffic_temp_threshold);
}

uint32
dnx_data_dram_hbm_restore_traffic_low_temp_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_define_restore_traffic_low_temp_threshold);
}

uint32
dnx_data_dram_hbm_usec_between_temp_samples_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_define_usec_between_temp_samples);
}

uint32
dnx_data_dram_hbm_power_down_temp_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_define_power_down_temp_threshold);
}

uint32
dnx_data_dram_hbm_power_down_low_temp_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_define_power_down_low_temp_threshold);
}

uint32
dnx_data_dram_hbm_dram_temp_monitor_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_define_dram_temp_monitor_enable);
}

uint32
dnx_data_dram_hbm_start_disabled_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_define_start_disabled);
}

uint32
dnx_data_dram_hbm_output_enable_length_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_define_output_enable_length);
}

uint32
dnx_data_dram_hbm_output_enable_delay_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_define_output_enable_delay);
}

uint32
dnx_data_dram_hbm_default_model_part_num_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_define_default_model_part_num);
}

uint32
dnx_data_dram_hbm_nof_tdu_per_core_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_define_nof_tdu_per_core);
}

uint32
dnx_data_dram_hbm_wds_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_define_wds_size);
}



const dnx_data_dram_hbm_channel_symmetric_regs_t *
dnx_data_dram_hbm_channel_symmetric_regs_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_channel_symmetric_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_hbm_channel_symmetric_regs_t *) data;

}

const dnx_data_dram_hbm_channel_not_symmetric_regs_t *
dnx_data_dram_hbm_channel_not_symmetric_regs_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_channel_not_symmetric_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_hbm_channel_not_symmetric_regs_t *) data;

}

const dnx_data_dram_hbm_controller_symmetric_regs_t *
dnx_data_dram_hbm_controller_symmetric_regs_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_controller_symmetric_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_hbm_controller_symmetric_regs_t *) data;

}

const dnx_data_dram_hbm_controller_not_symmetric_regs_t *
dnx_data_dram_hbm_controller_not_symmetric_regs_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_controller_not_symmetric_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_hbm_controller_not_symmetric_regs_t *) data;

}

const dnx_data_dram_hbm_channel_interrupt_regs_t *
dnx_data_dram_hbm_channel_interrupt_regs_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_channel_interrupt_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_hbm_channel_interrupt_regs_t *) data;

}

const dnx_data_dram_hbm_controller_interrupt_regs_t *
dnx_data_dram_hbm_controller_interrupt_regs_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_controller_interrupt_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_hbm_controller_interrupt_regs_t *) data;

}

const dnx_data_dram_hbm_channel_debug_regs_t *
dnx_data_dram_hbm_channel_debug_regs_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_channel_debug_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_hbm_channel_debug_regs_t *) data;

}

const dnx_data_dram_hbm_channel_counter_regs_t *
dnx_data_dram_hbm_channel_counter_regs_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_channel_counter_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_hbm_channel_counter_regs_t *) data;

}

const dnx_data_dram_hbm_channel_type_regs_t *
dnx_data_dram_hbm_channel_type_regs_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_channel_type_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_hbm_channel_type_regs_t *) data;

}

const dnx_data_dram_hbm_controller_info_regs_t *
dnx_data_dram_hbm_controller_info_regs_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_controller_info_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_hbm_controller_info_regs_t *) data;

}

const dnx_data_dram_hbm_bist_t *
dnx_data_dram_hbm_bist_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_bist);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_dram_hbm_bist_t *) data;

}

const dnx_data_dram_hbm_hbc_last_in_chain_t *
dnx_data_dram_hbm_hbc_last_in_chain_get(
    int unit,
    int dram_bitmap,
    int dram_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_hbc_last_in_chain);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dram_bitmap, dram_index);
    return (const dnx_data_dram_hbm_hbc_last_in_chain_t *) data;

}


shr_error_e
dnx_data_dram_hbm_channel_symmetric_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_hbm_channel_symmetric_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_channel_symmetric_regs);
    data = (const dnx_data_dram_hbm_channel_symmetric_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_hbm_channel_not_symmetric_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_hbm_channel_not_symmetric_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_channel_not_symmetric_regs);
    data = (const dnx_data_dram_hbm_channel_not_symmetric_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_hbm_controller_symmetric_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_hbm_controller_symmetric_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_controller_symmetric_regs);
    data = (const dnx_data_dram_hbm_controller_symmetric_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_hbm_controller_not_symmetric_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_hbm_controller_not_symmetric_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_controller_not_symmetric_regs);
    data = (const dnx_data_dram_hbm_controller_not_symmetric_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_hbm_channel_interrupt_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_hbm_channel_interrupt_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_channel_interrupt_regs);
    data = (const dnx_data_dram_hbm_channel_interrupt_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_hbm_controller_interrupt_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_hbm_controller_interrupt_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_controller_interrupt_regs);
    data = (const dnx_data_dram_hbm_controller_interrupt_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_hbm_channel_debug_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_hbm_channel_debug_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_channel_debug_regs);
    data = (const dnx_data_dram_hbm_channel_debug_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->reassuring_str == NULL ? "" : data->reassuring_str);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_hbm_channel_counter_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_hbm_channel_counter_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_channel_counter_regs);
    data = (const dnx_data_dram_hbm_channel_counter_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_hbm_channel_type_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_hbm_channel_type_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_channel_type_regs);
    data = (const dnx_data_dram_hbm_channel_type_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->type);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_hbm_controller_info_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_hbm_controller_info_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_controller_info_regs);
    data = (const dnx_data_dram_hbm_controller_info_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_hbm_bist_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_hbm_bist_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_bist);
    data = (const dnx_data_dram_hbm_bist_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, HBMC_SHMOO_BIST_NOF_PRBS_DATA_SEEDS, data->prbs_seeds);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_hbm_hbc_last_in_chain_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_hbm_hbc_last_in_chain_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_hbc_last_in_chain);
    data = (const dnx_data_dram_hbm_hbc_last_in_chain_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_last_in_chain);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_dram_hbm_channel_symmetric_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_channel_symmetric_regs);

}

const dnxc_data_table_info_t *
dnx_data_dram_hbm_channel_not_symmetric_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_channel_not_symmetric_regs);

}

const dnxc_data_table_info_t *
dnx_data_dram_hbm_controller_symmetric_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_controller_symmetric_regs);

}

const dnxc_data_table_info_t *
dnx_data_dram_hbm_controller_not_symmetric_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_controller_not_symmetric_regs);

}

const dnxc_data_table_info_t *
dnx_data_dram_hbm_channel_interrupt_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_channel_interrupt_regs);

}

const dnxc_data_table_info_t *
dnx_data_dram_hbm_controller_interrupt_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_controller_interrupt_regs);

}

const dnxc_data_table_info_t *
dnx_data_dram_hbm_channel_debug_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_channel_debug_regs);

}

const dnxc_data_table_info_t *
dnx_data_dram_hbm_channel_counter_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_channel_counter_regs);

}

const dnxc_data_table_info_t *
dnx_data_dram_hbm_channel_type_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_channel_type_regs);

}

const dnxc_data_table_info_t *
dnx_data_dram_hbm_controller_info_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_controller_info_regs);

}

const dnxc_data_table_info_t *
dnx_data_dram_hbm_bist_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_bist);

}

const dnxc_data_table_info_t *
dnx_data_dram_hbm_hbc_last_in_chain_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_hbc_last_in_chain);

}






static shr_error_e
dnx_data_dram_gddr6_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "gddr6";
    submodule_data->doc = "GDDR6 parameters and defines";
    
    submodule_data->nof_features = _dnx_data_dram_gddr6_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dram gddr6 features");

    submodule_data->features[dnx_data_dram_gddr6_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_dram_gddr6_is_supported].doc = "gddr supported indication";
    submodule_data->features[dnx_data_dram_gddr6_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_gddr6_allow_disable_read_crc].name = "allow_disable_read_crc";
    submodule_data->features[dnx_data_dram_gddr6_allow_disable_read_crc].doc = "allow disabling read crc";
    submodule_data->features[dnx_data_dram_gddr6_allow_disable_read_crc].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_gddr6_interleaved_refresh_cycles].name = "interleaved_refresh_cycles";
    submodule_data->features[dnx_data_dram_gddr6_interleaved_refresh_cycles].doc = "allow per bank and all banks refresh to be interleaved";
    submodule_data->features[dnx_data_dram_gddr6_interleaved_refresh_cycles].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_gddr6_refresh_pend_switch].name = "refresh_pend_switch";
    submodule_data->features[dnx_data_dram_gddr6_refresh_pend_switch].doc = "allow wait for Wr/Rd swith to perform AB refresh";
    submodule_data->features[dnx_data_dram_gddr6_refresh_pend_switch].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_gddr6_cadt_exit_mode_when_done].name = "cadt_exit_mode_when_done";
    submodule_data->features[dnx_data_dram_gddr6_cadt_exit_mode_when_done].doc = "perform MRS to exit CADT mode at the end of CADT";
    submodule_data->features[dnx_data_dram_gddr6_cadt_exit_mode_when_done].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_gddr6_periodic_temp_readout].name = "periodic_temp_readout";
    submodule_data->features[dnx_data_dram_gddr6_periodic_temp_readout].doc = "Periodic temperature readout";
    submodule_data->features[dnx_data_dram_gddr6_periodic_temp_readout].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_gddr6_controller_coherency_handling_mode].name = "controller_coherency_handling_mode";
    submodule_data->features[dnx_data_dram_gddr6_controller_coherency_handling_mode].doc = "";
    submodule_data->features[dnx_data_dram_gddr6_controller_coherency_handling_mode].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_dram_gddr6_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dram gddr6 defines");

    submodule_data->defines[dnx_data_dram_gddr6_define_nof_ca_bits].name = "nof_ca_bits";
    submodule_data->defines[dnx_data_dram_gddr6_define_nof_ca_bits].doc = "number of command address bits";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_nof_ca_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_gddr6_define_bytes_per_channel].name = "bytes_per_channel";
    submodule_data->defines[dnx_data_dram_gddr6_define_bytes_per_channel].doc = "bytes per channel in GDDR6 dram";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_bytes_per_channel].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_gddr6_define_training_fifo_depth].name = "training_fifo_depth";
    submodule_data->defines[dnx_data_dram_gddr6_define_training_fifo_depth].doc = "the depth of the training fifo in GDDR6 dram";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_training_fifo_depth].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_gddr6_define_readout_to_readout_prd].name = "readout_to_readout_prd";
    submodule_data->defines[dnx_data_dram_gddr6_define_readout_to_readout_prd].doc = "Number of AB (all-banks) refresh to wait between consecutive temperature readout iterations";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_readout_to_readout_prd].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_gddr6_define_refresh_to_readout_prd].name = "refresh_to_readout_prd";
    submodule_data->defines[dnx_data_dram_gddr6_define_refresh_to_readout_prd].doc = "Period between AB (all-banks) refresh to MRS command triggering the temperature readout";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_refresh_to_readout_prd].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_gddr6_define_readout_done_to_done_prd].name = "readout_done_to_done_prd";
    submodule_data->defines[dnx_data_dram_gddr6_define_readout_done_to_done_prd].doc = "Period between vendor_id engine done till readout engine done";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_readout_done_to_done_prd].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_gddr6_define_refresh_mechanism_enable].name = "refresh_mechanism_enable";
    submodule_data->defines[dnx_data_dram_gddr6_define_refresh_mechanism_enable].doc = "TSM refresh mechanism enable";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_refresh_mechanism_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_bist_enable].name = "bist_enable";
    submodule_data->defines[dnx_data_dram_gddr6_define_bist_enable].doc = "DRAM bist enable";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_bist_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_dynamic_calibration_enable].name = "dynamic_calibration_enable";
    submodule_data->defines[dnx_data_dram_gddr6_define_dynamic_calibration_enable].doc = "Enable dynamic calibration on init";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_dynamic_calibration_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_cdr_enable].name = "cdr_enable";
    submodule_data->defines[dnx_data_dram_gddr6_define_cdr_enable].doc = "Enable CDR step when running dram tune on init";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_cdr_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_write_recovery].name = "write_recovery";
    submodule_data->defines[dnx_data_dram_gddr6_define_write_recovery].doc = "write recovery";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_write_recovery].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_cabi].name = "cabi";
    submodule_data->defines[dnx_data_dram_gddr6_define_cabi].doc = "enable cabi";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_cabi].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_dram_mode].name = "dram_mode";
    submodule_data->defines[dnx_data_dram_gddr6_define_dram_mode].doc = "dram mode";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_dram_mode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_cal_termination].name = "cal_termination";
    submodule_data->defines[dnx_data_dram_gddr6_define_cal_termination].doc = "CAL Termination";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_cal_termination].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_cah_termination].name = "cah_termination";
    submodule_data->defines[dnx_data_dram_gddr6_define_cah_termination].doc = "CAH Termination";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_cah_termination].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_command_pipe_extra_delay].name = "command_pipe_extra_delay";
    submodule_data->defines[dnx_data_dram_gddr6_define_command_pipe_extra_delay].doc = "command pipe extra delay";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_command_pipe_extra_delay].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_use_11bits_ca].name = "use_11bits_ca";
    submodule_data->defines[dnx_data_dram_gddr6_define_use_11bits_ca].doc = "use 11bits for command address";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_use_11bits_ca].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_ck_odt].name = "ck_odt";
    submodule_data->defines[dnx_data_dram_gddr6_define_ck_odt].doc = "CK ODT";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_ck_odt].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_dynamic_calibration_period].name = "dynamic_calibration_period";
    submodule_data->defines[dnx_data_dram_gddr6_define_dynamic_calibration_period].doc = "time between 2 iterations of dynamic calibration";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_dynamic_calibration_period].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_dram_gddr6_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dram gddr6 tables");

    
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].name = "refresh_intervals";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].doc = "refresh intervals timing parameters";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].size_of_values = sizeof(dnx_data_dram_gddr6_refresh_intervals_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].entry_get = dnx_data_dram_gddr6_refresh_intervals_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].nof_keys = 0;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].nof_values, "_dnx_data_dram_gddr6_table_refresh_intervals table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[0].name = "trefiab";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[0].doc = "trefiab timing parameters";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_refresh_intervals_t, trefiab);
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[1].name = "trefiab_acc";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[1].type = "uint32";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[1].doc = "trefiab_acc timing parameters";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_refresh_intervals_t, trefiab_acc);
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[2].name = "trefisb";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[2].type = "uint32";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[2].doc = "trefisb timing parameters";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[2].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_refresh_intervals_t, trefisb);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].name = "dq_map";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].doc = "DQ swap mapping on boards";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].size_of_values = sizeof(dnx_data_dram_gddr6_dq_map_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].entry_get = dnx_data_dram_gddr6_dq_map_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].nof_keys = 2;
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].keys[0].name = "dram_index";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].keys[0].doc = "dram index";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].keys[1].name = "byte";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].keys[1].doc = "bit on dram";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].nof_values, "_dnx_data_dram_gddr6_table_dq_map table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].values[0].name = "dq_map";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].values[0].type = "uint8[8]";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].values[0].doc = "dq map";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_dq_map_t, dq_map);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].name = "dq_channel_swap";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].doc = "DQ channel swap mapping on boards";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].size_of_values = sizeof(dnx_data_dram_gddr6_dq_channel_swap_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].entry_get = dnx_data_dram_gddr6_dq_channel_swap_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].keys[0].name = "dram_index";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].keys[0].doc = "dram index";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].nof_values, "_dnx_data_dram_gddr6_table_dq_channel_swap table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].values[0].name = "dq_channel_swap";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].values[0].type = "uint8";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].values[0].doc = "dq map";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_dq_channel_swap_t, dq_channel_swap);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].name = "dq_byte_map";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].doc = "DQ bytes swap mapping on boards";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].size_of_values = sizeof(dnx_data_dram_gddr6_dq_byte_map_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].entry_get = dnx_data_dram_gddr6_dq_byte_map_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].nof_keys = 2;
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].keys[0].name = "dram_index";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].keys[0].doc = "dram index";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].keys[1].name = "byte";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].keys[1].doc = "byte on dram";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].nof_values, "_dnx_data_dram_gddr6_table_dq_byte_map table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].values[0].name = "dq_byte_map";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].values[0].type = "uint8";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].values[0].doc = "dq byte map";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_dq_byte_map_t, dq_byte_map);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].name = "ca_map";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].doc = "command address bit swap mapping on boards";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].size_of_values = sizeof(dnx_data_dram_gddr6_ca_map_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].entry_get = dnx_data_dram_gddr6_ca_map_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].nof_keys = 2;
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].keys[0].name = "dram_index";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].keys[0].doc = "dram index";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].keys[1].name = "channel";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].keys[1].doc = "command address bit";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].nof_values, "_dnx_data_dram_gddr6_table_ca_map table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].values[0].name = "ca_map";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].values[0].type = "uint8[12]";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].values[0].doc = "ca map";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_ca_map_t, ca_map);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].name = "cadt_byte_map";
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].doc = "bytes swapping on boards";
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].size_of_values = sizeof(dnx_data_dram_gddr6_cadt_byte_map_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].entry_get = dnx_data_dram_gddr6_cadt_byte_map_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].nof_keys = 2;
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].keys[0].name = "dram_index";
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].keys[0].doc = "dram index";
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].keys[1].name = "channel";
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].keys[1].doc = "dram controller channel";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].nof_values, "_dnx_data_dram_gddr6_table_cadt_byte_map table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].values[0].name = "cadt_byte_map";
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].values[0].type = "uint8";
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].values[0].doc = "cadt byte map";
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_cadt_byte_map_t, cadt_byte_map);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].name = "channel_regs";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].doc = "per channel list of registers";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].size_of_values = sizeof(dnx_data_dram_gddr6_channel_regs_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].entry_get = dnx_data_dram_gddr6_channel_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].nof_values, "_dnx_data_dram_gddr6_table_channel_regs table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].values[0].doc = "register";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_channel_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].name = "controller_regs";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].doc = "per phy controller list of registers";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].size_of_values = sizeof(dnx_data_dram_gddr6_controller_regs_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].entry_get = dnx_data_dram_gddr6_controller_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].nof_values, "_dnx_data_dram_gddr6_table_controller_regs table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].values[0].doc = "register";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_controller_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].name = "channel_interrupt_regs";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].doc = "per channel list of interrupt registers";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].size_of_values = sizeof(dnx_data_dram_gddr6_channel_interrupt_regs_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].entry_get = dnx_data_dram_gddr6_channel_interrupt_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].nof_values, "_dnx_data_dram_gddr6_table_channel_interrupt_regs table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].values[0].doc = "register";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_channel_interrupt_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].name = "controller_interrupt_regs";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].doc = "per phy list of interrupt registers";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].size_of_values = sizeof(dnx_data_dram_gddr6_controller_interrupt_regs_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].entry_get = dnx_data_dram_gddr6_controller_interrupt_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].nof_values, "_dnx_data_dram_gddr6_table_controller_interrupt_regs table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].values[0].doc = "register";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_controller_interrupt_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].name = "channel_debug_regs";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].doc = "per channel list of debug registers - should be preented if value is not reset value";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].size_of_values = sizeof(dnx_data_dram_gddr6_channel_debug_regs_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].entry_get = dnx_data_dram_gddr6_channel_debug_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].nof_values, "_dnx_data_dram_gddr6_table_channel_debug_regs table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].values[0].doc = "register";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_channel_debug_regs_t, reg);
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].values[1].name = "reassuring_str";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].values[1].type = "char *";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].values[1].doc = "string to explain that this register's value is as expected (reset value)";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_channel_debug_regs_t, reassuring_str);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].name = "channel_counter_regs";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].doc = "per channel list of counter registers";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].size_of_values = sizeof(dnx_data_dram_gddr6_channel_counter_regs_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].entry_get = dnx_data_dram_gddr6_channel_counter_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].nof_values, "_dnx_data_dram_gddr6_table_channel_counter_regs table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].values[0].doc = "register";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_channel_counter_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].name = "channel_type_regs";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].doc = "per channel list of registers that require per type printing";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].size_of_values = sizeof(dnx_data_dram_gddr6_channel_type_regs_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].entry_get = dnx_data_dram_gddr6_channel_type_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].nof_values, "_dnx_data_dram_gddr6_table_channel_type_regs table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].values[0].doc = "register";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_channel_type_regs_t, reg);
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].values[1].name = "type";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].values[1].type = "dnx_hbmc_diag_registers_type_t";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].values[1].doc = "printing type";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_channel_type_regs_t, type);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].name = "controller_info_regs";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].doc = "per contorller list of info registers";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].size_of_values = sizeof(dnx_data_dram_gddr6_controller_info_regs_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].entry_get = dnx_data_dram_gddr6_controller_info_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].nof_values, "_dnx_data_dram_gddr6_table_controller_info_regs table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].values[0].doc = "register";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_controller_info_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].name = "master_phy";
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].doc = "The resistors for ZQ calibration are connected only to one of the drams, which we denote as master phy. The other dram is denoted as slave phy";
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].size_of_values = sizeof(dnx_data_dram_gddr6_master_phy_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].entry_get = dnx_data_dram_gddr6_master_phy_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].keys[0].name = "dram_index";
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].keys[0].doc = "index of dram";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].nof_values, "_dnx_data_dram_gddr6_table_master_phy table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].values[0].name = "master_dram_index";
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].values[0].doc = "The index of the master dram";
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_master_phy_t, master_dram_index);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dram_gddr6_feature_get(
    int unit,
    dnx_data_dram_gddr6_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, feature);
}


uint32
dnx_data_dram_gddr6_nof_ca_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_nof_ca_bits);
}

uint32
dnx_data_dram_gddr6_bytes_per_channel_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_bytes_per_channel);
}

uint32
dnx_data_dram_gddr6_training_fifo_depth_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_training_fifo_depth);
}

uint32
dnx_data_dram_gddr6_readout_to_readout_prd_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_readout_to_readout_prd);
}

uint32
dnx_data_dram_gddr6_refresh_to_readout_prd_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_refresh_to_readout_prd);
}

uint32
dnx_data_dram_gddr6_readout_done_to_done_prd_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_readout_done_to_done_prd);
}

uint32
dnx_data_dram_gddr6_refresh_mechanism_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_refresh_mechanism_enable);
}

uint32
dnx_data_dram_gddr6_bist_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_bist_enable);
}

uint32
dnx_data_dram_gddr6_dynamic_calibration_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_dynamic_calibration_enable);
}

uint32
dnx_data_dram_gddr6_cdr_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_cdr_enable);
}

uint32
dnx_data_dram_gddr6_write_recovery_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_write_recovery);
}

uint32
dnx_data_dram_gddr6_cabi_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_cabi);
}

uint32
dnx_data_dram_gddr6_dram_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_dram_mode);
}

uint32
dnx_data_dram_gddr6_cal_termination_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_cal_termination);
}

uint32
dnx_data_dram_gddr6_cah_termination_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_cah_termination);
}

uint32
dnx_data_dram_gddr6_command_pipe_extra_delay_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_command_pipe_extra_delay);
}

uint32
dnx_data_dram_gddr6_use_11bits_ca_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_use_11bits_ca);
}

uint32
dnx_data_dram_gddr6_ck_odt_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_ck_odt);
}

uint32
dnx_data_dram_gddr6_dynamic_calibration_period_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_dynamic_calibration_period);
}



const dnx_data_dram_gddr6_refresh_intervals_t *
dnx_data_dram_gddr6_refresh_intervals_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_refresh_intervals);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_dram_gddr6_refresh_intervals_t *) data;

}

const dnx_data_dram_gddr6_dq_map_t *
dnx_data_dram_gddr6_dq_map_get(
    int unit,
    int dram_index,
    int byte)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_dq_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dram_index, byte);
    return (const dnx_data_dram_gddr6_dq_map_t *) data;

}

const dnx_data_dram_gddr6_dq_channel_swap_t *
dnx_data_dram_gddr6_dq_channel_swap_get(
    int unit,
    int dram_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_dq_channel_swap);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dram_index, 0);
    return (const dnx_data_dram_gddr6_dq_channel_swap_t *) data;

}

const dnx_data_dram_gddr6_dq_byte_map_t *
dnx_data_dram_gddr6_dq_byte_map_get(
    int unit,
    int dram_index,
    int byte)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_dq_byte_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dram_index, byte);
    return (const dnx_data_dram_gddr6_dq_byte_map_t *) data;

}

const dnx_data_dram_gddr6_ca_map_t *
dnx_data_dram_gddr6_ca_map_get(
    int unit,
    int dram_index,
    int channel)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_ca_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dram_index, channel);
    return (const dnx_data_dram_gddr6_ca_map_t *) data;

}

const dnx_data_dram_gddr6_cadt_byte_map_t *
dnx_data_dram_gddr6_cadt_byte_map_get(
    int unit,
    int dram_index,
    int channel)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_cadt_byte_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dram_index, channel);
    return (const dnx_data_dram_gddr6_cadt_byte_map_t *) data;

}

const dnx_data_dram_gddr6_channel_regs_t *
dnx_data_dram_gddr6_channel_regs_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_channel_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_gddr6_channel_regs_t *) data;

}

const dnx_data_dram_gddr6_controller_regs_t *
dnx_data_dram_gddr6_controller_regs_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_controller_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_gddr6_controller_regs_t *) data;

}

const dnx_data_dram_gddr6_channel_interrupt_regs_t *
dnx_data_dram_gddr6_channel_interrupt_regs_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_channel_interrupt_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_gddr6_channel_interrupt_regs_t *) data;

}

const dnx_data_dram_gddr6_controller_interrupt_regs_t *
dnx_data_dram_gddr6_controller_interrupt_regs_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_controller_interrupt_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_gddr6_controller_interrupt_regs_t *) data;

}

const dnx_data_dram_gddr6_channel_debug_regs_t *
dnx_data_dram_gddr6_channel_debug_regs_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_channel_debug_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_gddr6_channel_debug_regs_t *) data;

}

const dnx_data_dram_gddr6_channel_counter_regs_t *
dnx_data_dram_gddr6_channel_counter_regs_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_channel_counter_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_gddr6_channel_counter_regs_t *) data;

}

const dnx_data_dram_gddr6_channel_type_regs_t *
dnx_data_dram_gddr6_channel_type_regs_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_channel_type_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_gddr6_channel_type_regs_t *) data;

}

const dnx_data_dram_gddr6_controller_info_regs_t *
dnx_data_dram_gddr6_controller_info_regs_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_controller_info_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_gddr6_controller_info_regs_t *) data;

}

const dnx_data_dram_gddr6_master_phy_t *
dnx_data_dram_gddr6_master_phy_get(
    int unit,
    int dram_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_master_phy);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dram_index, 0);
    return (const dnx_data_dram_gddr6_master_phy_t *) data;

}


shr_error_e
dnx_data_dram_gddr6_refresh_intervals_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_gddr6_refresh_intervals_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_refresh_intervals);
    data = (const dnx_data_dram_gddr6_refresh_intervals_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trefiab);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trefiab_acc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trefisb);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_gddr6_dq_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_gddr6_dq_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_dq_map);
    data = (const dnx_data_dram_gddr6_dq_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, 8, data->dq_map);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_gddr6_dq_channel_swap_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_gddr6_dq_channel_swap_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_dq_channel_swap);
    data = (const dnx_data_dram_gddr6_dq_channel_swap_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dq_channel_swap);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_gddr6_dq_byte_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_gddr6_dq_byte_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_dq_byte_map);
    data = (const dnx_data_dram_gddr6_dq_byte_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dq_byte_map);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_gddr6_ca_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_gddr6_ca_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_ca_map);
    data = (const dnx_data_dram_gddr6_ca_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, 12, data->ca_map);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_gddr6_cadt_byte_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_gddr6_cadt_byte_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_cadt_byte_map);
    data = (const dnx_data_dram_gddr6_cadt_byte_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->cadt_byte_map);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_gddr6_channel_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_gddr6_channel_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_channel_regs);
    data = (const dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_gddr6_controller_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_gddr6_controller_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_controller_regs);
    data = (const dnx_data_dram_gddr6_controller_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_gddr6_channel_interrupt_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_gddr6_channel_interrupt_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_channel_interrupt_regs);
    data = (const dnx_data_dram_gddr6_channel_interrupt_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_gddr6_controller_interrupt_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_gddr6_controller_interrupt_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_controller_interrupt_regs);
    data = (const dnx_data_dram_gddr6_controller_interrupt_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_gddr6_channel_debug_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_gddr6_channel_debug_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_channel_debug_regs);
    data = (const dnx_data_dram_gddr6_channel_debug_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->reassuring_str == NULL ? "" : data->reassuring_str);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_gddr6_channel_counter_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_gddr6_channel_counter_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_channel_counter_regs);
    data = (const dnx_data_dram_gddr6_channel_counter_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_gddr6_channel_type_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_gddr6_channel_type_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_channel_type_regs);
    data = (const dnx_data_dram_gddr6_channel_type_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->type);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_gddr6_controller_info_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_gddr6_controller_info_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_controller_info_regs);
    data = (const dnx_data_dram_gddr6_controller_info_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_gddr6_master_phy_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_gddr6_master_phy_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_master_phy);
    data = (const dnx_data_dram_gddr6_master_phy_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->master_dram_index);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_dram_gddr6_refresh_intervals_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_refresh_intervals);

}

const dnxc_data_table_info_t *
dnx_data_dram_gddr6_dq_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_dq_map);

}

const dnxc_data_table_info_t *
dnx_data_dram_gddr6_dq_channel_swap_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_dq_channel_swap);

}

const dnxc_data_table_info_t *
dnx_data_dram_gddr6_dq_byte_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_dq_byte_map);

}

const dnxc_data_table_info_t *
dnx_data_dram_gddr6_ca_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_ca_map);

}

const dnxc_data_table_info_t *
dnx_data_dram_gddr6_cadt_byte_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_cadt_byte_map);

}

const dnxc_data_table_info_t *
dnx_data_dram_gddr6_channel_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_channel_regs);

}

const dnxc_data_table_info_t *
dnx_data_dram_gddr6_controller_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_controller_regs);

}

const dnxc_data_table_info_t *
dnx_data_dram_gddr6_channel_interrupt_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_channel_interrupt_regs);

}

const dnxc_data_table_info_t *
dnx_data_dram_gddr6_controller_interrupt_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_controller_interrupt_regs);

}

const dnxc_data_table_info_t *
dnx_data_dram_gddr6_channel_debug_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_channel_debug_regs);

}

const dnxc_data_table_info_t *
dnx_data_dram_gddr6_channel_counter_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_channel_counter_regs);

}

const dnxc_data_table_info_t *
dnx_data_dram_gddr6_channel_type_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_channel_type_regs);

}

const dnxc_data_table_info_t *
dnx_data_dram_gddr6_controller_info_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_controller_info_regs);

}

const dnxc_data_table_info_t *
dnx_data_dram_gddr6_master_phy_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_master_phy);

}






static shr_error_e
dnx_data_dram_general_info_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general_info";
    submodule_data->doc = "general info derived from defaults or soc properties which is relevant also for other types of drams, not only to HBM";
    
    submodule_data->nof_features = _dnx_data_dram_general_info_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dram general_info features");

    submodule_data->features[dnx_data_dram_general_info_is_temperature_reading_supported].name = "is_temperature_reading_supported";
    submodule_data->features[dnx_data_dram_general_info_is_temperature_reading_supported].doc = "Is reading the dram temperature is supported";
    submodule_data->features[dnx_data_dram_general_info_is_temperature_reading_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_dram_general_info_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dram general_info defines");

    submodule_data->defines[dnx_data_dram_general_info_define_otp_restore_version].name = "otp_restore_version";
    submodule_data->defines[dnx_data_dram_general_info_define_otp_restore_version].doc = "when restoring from OTP the tune data, do it according to the given version format";
    
    submodule_data->defines[dnx_data_dram_general_info_define_otp_restore_version].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_general_info_define_max_nof_drams].name = "max_nof_drams";
    submodule_data->defines[dnx_data_dram_general_info_define_max_nof_drams].doc = "max number of drams supported for this device - in other words the size of the dram bitmap";
    
    submodule_data->defines[dnx_data_dram_general_info_define_max_nof_drams].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_general_info_define_nof_channels].name = "nof_channels";
    submodule_data->defines[dnx_data_dram_general_info_define_nof_channels].doc = "max number of dram channels supported for this device";
    
    submodule_data->defines[dnx_data_dram_general_info_define_nof_channels].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_general_info_define_mr_mask].name = "mr_mask";
    submodule_data->defines[dnx_data_dram_general_info_define_mr_mask].doc = "mode register bit mask";
    
    submodule_data->defines[dnx_data_dram_general_info_define_mr_mask].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_general_info_define_nof_mrs].name = "nof_mrs";
    submodule_data->defines[dnx_data_dram_general_info_define_nof_mrs].doc = "number of mode registers";
    
    submodule_data->defines[dnx_data_dram_general_info_define_nof_mrs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_general_info_define_phy_address_mask].name = "phy_address_mask";
    submodule_data->defines[dnx_data_dram_general_info_define_phy_address_mask].doc = "bit mask to describe the relevant bits in phy register address";
    
    submodule_data->defines[dnx_data_dram_general_info_define_phy_address_mask].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_general_info_define_max_dram_index].name = "max_dram_index";
    submodule_data->defines[dnx_data_dram_general_info_define_max_dram_index].doc = "max dram index from bitmap";
    
    submodule_data->defines[dnx_data_dram_general_info_define_max_dram_index].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_min_dram_index].name = "min_dram_index";
    submodule_data->defines[dnx_data_dram_general_info_define_min_dram_index].doc = "min dram index from bitmap";
    
    submodule_data->defines[dnx_data_dram_general_info_define_min_dram_index].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_frequency].name = "frequency";
    submodule_data->defines[dnx_data_dram_general_info_define_frequency].doc = "dram frequency (in MHZ), used to calculated data_rate";
    
    submodule_data->defines[dnx_data_dram_general_info_define_frequency].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_buffer_size].name = "buffer_size";
    submodule_data->defines[dnx_data_dram_general_info_define_buffer_size].doc = "dram buffer size in bytes";
    
    submodule_data->defines[dnx_data_dram_general_info_define_buffer_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_command_address_parity].name = "command_address_parity";
    submodule_data->defines[dnx_data_dram_general_info_define_command_address_parity].doc = "command address parity";
    
    submodule_data->defines[dnx_data_dram_general_info_define_command_address_parity].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_dq_write_parity].name = "dq_write_parity";
    submodule_data->defines[dnx_data_dram_general_info_define_dq_write_parity].doc = "dq write parity";
    
    submodule_data->defines[dnx_data_dram_general_info_define_dq_write_parity].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_dq_read_parity].name = "dq_read_parity";
    submodule_data->defines[dnx_data_dram_general_info_define_dq_read_parity].doc = "dq read parity";
    
    submodule_data->defines[dnx_data_dram_general_info_define_dq_read_parity].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_dbi_read].name = "dbi_read";
    submodule_data->defines[dnx_data_dram_general_info_define_dbi_read].doc = "dbi read";
    
    submodule_data->defines[dnx_data_dram_general_info_define_dbi_read].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_dbi_write].name = "dbi_write";
    submodule_data->defines[dnx_data_dram_general_info_define_dbi_write].doc = "dbi write";
    
    submodule_data->defines[dnx_data_dram_general_info_define_dbi_write].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_write_latency].name = "write_latency";
    submodule_data->defines[dnx_data_dram_general_info_define_write_latency].doc = "write latency";
    
    submodule_data->defines[dnx_data_dram_general_info_define_write_latency].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_read_latency].name = "read_latency";
    submodule_data->defines[dnx_data_dram_general_info_define_read_latency].doc = "read latency";
    
    submodule_data->defines[dnx_data_dram_general_info_define_read_latency].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_read_data_enable_length].name = "read_data_enable_length";
    submodule_data->defines[dnx_data_dram_general_info_define_read_data_enable_length].doc = "read data enable length";
    
    submodule_data->defines[dnx_data_dram_general_info_define_read_data_enable_length].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_parity_latency].name = "parity_latency";
    submodule_data->defines[dnx_data_dram_general_info_define_parity_latency].doc = "parity latency";
    
    submodule_data->defines[dnx_data_dram_general_info_define_parity_latency].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_actual_parity_latency].name = "actual_parity_latency";
    submodule_data->defines[dnx_data_dram_general_info_define_actual_parity_latency].doc = "parity latency to be used after factoring in the effect of other numerics";
    
    submodule_data->defines[dnx_data_dram_general_info_define_actual_parity_latency].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_tune_mode_on_init].name = "tune_mode_on_init";
    submodule_data->defines[dnx_data_dram_general_info_define_tune_mode_on_init].doc = "tune mode during init sequence";
    
    submodule_data->defines[dnx_data_dram_general_info_define_tune_mode_on_init].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_command_parity_latency].name = "command_parity_latency";
    submodule_data->defines[dnx_data_dram_general_info_define_command_parity_latency].doc = "command parity latency";
    
    submodule_data->defines[dnx_data_dram_general_info_define_command_parity_latency].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_crc_write_latency].name = "crc_write_latency";
    submodule_data->defines[dnx_data_dram_general_info_define_crc_write_latency].doc = "crc write latency";
    
    submodule_data->defines[dnx_data_dram_general_info_define_crc_write_latency].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_crc_read_latency].name = "crc_read_latency";
    submodule_data->defines[dnx_data_dram_general_info_define_crc_read_latency].doc = "crc read latency";
    
    submodule_data->defines[dnx_data_dram_general_info_define_crc_read_latency].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_crc_write].name = "crc_write";
    submodule_data->defines[dnx_data_dram_general_info_define_crc_write].doc = "write crc";
    
    submodule_data->defines[dnx_data_dram_general_info_define_crc_write].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_crc_read].name = "crc_read";
    submodule_data->defines[dnx_data_dram_general_info_define_crc_read].doc = "read crc";
    
    submodule_data->defines[dnx_data_dram_general_info_define_crc_read].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_device_size].name = "device_size";
    submodule_data->defines[dnx_data_dram_general_info_define_device_size].doc = "device size for each dram die";
    
    submodule_data->defines[dnx_data_dram_general_info_define_device_size].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_dram_general_info_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dram general_info tables");

    
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].name = "mr_defaults";
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].doc = "mode registers default values";
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].size_of_values = sizeof(dnx_data_dram_general_info_mr_defaults_t);
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].entry_get = dnx_data_dram_general_info_mr_defaults_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].keys[0].doc = "mode register index";

    
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].nof_values, "_dnx_data_dram_general_info_table_mr_defaults table values");
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].values[0].name = "value";
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].values[0].doc = "default value for mode register";
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_mr_defaults_t, value);

    
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].name = "dram_info";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].doc = "general information regarding the dram in the system";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].size_of_values = sizeof(dnx_data_dram_general_info_dram_info_t);
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].entry_get = dnx_data_dram_general_info_dram_info_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].nof_keys = 0;

    
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].nof_values = 8;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_general_info_table_dram_info].nof_values, "_dnx_data_dram_general_info_table_dram_info table values");
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[0].name = "dram_type";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[0].doc = "dram type as defined by tuning infrastructure";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_dram_info_t, dram_type);
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[1].name = "dram_bitmap";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[1].doc = "bitmap of currently available drams";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_dram_info_t, dram_bitmap);
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[2].name = "nof_columns";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[2].doc = "number of columns in the dram";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_dram_info_t, nof_columns);
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[3].name = "nof_rows";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[3].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[3].doc = "number of rows in the dram";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_dram_info_t, nof_rows);
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[4].name = "nof_banks";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[4].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[4].doc = "number of banks in the dram";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[4].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_dram_info_t, nof_banks);
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[5].name = "data_rate";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[5].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[5].doc = "(in MHZ)data rate in mbps, in DDRs equal 2 * frequency";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[5].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_dram_info_t, data_rate);
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[6].name = "ref_clock";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[6].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[6].doc = "ref clock for the dram phy";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[6].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_dram_info_t, ref_clock);
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[7].name = "ctl_type";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[7].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[7].doc = "shmoo input - which actions to perform";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[7].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_dram_info_t, ctl_type);

    
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].name = "timing_params";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].doc = "timing parameters";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].size_of_values = sizeof(dnx_data_dram_general_info_timing_params_t);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].entry_get = dnx_data_dram_general_info_timing_params_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].nof_keys = 0;

    
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].nof_values = 31;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_general_info_table_timing_params].nof_values, "_dnx_data_dram_general_info_table_timing_params table values");
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[0].name = "twr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[0].doc = "twr timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, twr);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[1].name = "trp";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[1].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[1].doc = "trp timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trp);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[2].name = "trtps";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[2].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[2].doc = "trtps timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[2].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trtps);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[3].name = "trtpl";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[3].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[3].doc = "trtpl timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[3].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trtpl);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[4].name = "trrds";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[4].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[4].doc = "trrds timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[4].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trrds);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[5].name = "trrdl";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[5].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[5].doc = "trrdl timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[5].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trrdl);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[6].name = "tfaw";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[6].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[6].doc = "tfaw timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[6].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, tfaw);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[7].name = "trcdwr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[7].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[7].doc = "trcdwr timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[7].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trcdwr);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[8].name = "trcdrd";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[8].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[8].doc = "trcdrd timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[8].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trcdrd);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[9].name = "tras";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[9].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[9].doc = "tras timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[9].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, tras);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[10].name = "trc";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[10].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[10].doc = "trc timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[10].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trc);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[11].name = "twtrl";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[11].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[11].doc = "twtrl timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[11].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, twtrl);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[12].name = "twtrs";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[12].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[12].doc = "twtrs timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[12].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, twtrs);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[13].name = "trtw";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[13].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[13].doc = "trtw timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[13].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trtw);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[14].name = "tccdr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[14].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[14].doc = "tccdr timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[14].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, tccdr);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[15].name = "tccds";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[15].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[15].doc = "tccds timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[15].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, tccds);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[16].name = "tccdl";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[16].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[16].doc = "tccdl timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[16].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, tccdl);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[17].name = "trrefd";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[17].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[17].doc = "trrefd timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[17].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trrefd);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[18].name = "trfcsb";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[18].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[18].doc = "trfcsb timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[18].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trfcsb);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[19].name = "trfc";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[19].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[19].doc = "trfc timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[19].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trfc);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[20].name = "tmrd";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[20].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[20].doc = "tmrd timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[20].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, tmrd);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[21].name = "tmod";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[21].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[21].doc = "tmod timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[21].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, tmod);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[22].name = "twtrtr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[22].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[22].doc = "twtrtr timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[22].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, twtrtr);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[23].name = "twrwtr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[23].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[23].doc = "twrwtr timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[23].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, twrwtr);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[24].name = "treftr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[24].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[24].doc = "treftr timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[24].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, treftr);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[25].name = "trdtlt";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[25].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[25].doc = "trdtlt timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[25].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trdtlt);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[26].name = "trcdwtr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[26].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[26].doc = "trcdwtr timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[26].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trcdwtr);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[27].name = "trcdrtr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[27].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[27].doc = "trcdrtr timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[27].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trcdrtr);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[28].name = "trcdltr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[28].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[28].doc = "trcdltr timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[28].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trcdltr);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[29].name = "tltrtr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[29].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[29].doc = "tltrtr timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[29].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, tltrtr);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[30].name = "tltltr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[30].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[30].doc = "tltltr timing parameter";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[30].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, tltltr);

    
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].name = "refresh_intervals";
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].doc = "refresh intervals timing parameters";
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].size_of_values = sizeof(dnx_data_dram_general_info_refresh_intervals_t);
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].entry_get = dnx_data_dram_general_info_refresh_intervals_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].keys[0].name = "temp_index";
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].keys[0].doc = "temperature index";

    
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].nof_values, "_dnx_data_dram_general_info_table_refresh_intervals table values");
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].values[0].name = "trefi";
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].values[0].doc = "trefi timing parameters";
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_refresh_intervals_t, trefi);
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].values[1].name = "trefisb";
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].values[1].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].values[1].doc = "trefisb timing parameters";
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_refresh_intervals_t, trefisb);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dram_general_info_feature_get(
    int unit,
    dnx_data_dram_general_info_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, feature);
}


uint32
dnx_data_dram_general_info_otp_restore_version_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_otp_restore_version);
}

uint32
dnx_data_dram_general_info_max_nof_drams_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_max_nof_drams);
}

uint32
dnx_data_dram_general_info_nof_channels_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_nof_channels);
}

uint32
dnx_data_dram_general_info_mr_mask_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_mr_mask);
}

uint32
dnx_data_dram_general_info_nof_mrs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_nof_mrs);
}

uint32
dnx_data_dram_general_info_phy_address_mask_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_phy_address_mask);
}

uint32
dnx_data_dram_general_info_max_dram_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_max_dram_index);
}

uint32
dnx_data_dram_general_info_min_dram_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_min_dram_index);
}

uint32
dnx_data_dram_general_info_frequency_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_frequency);
}

uint32
dnx_data_dram_general_info_buffer_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_buffer_size);
}

uint32
dnx_data_dram_general_info_command_address_parity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_command_address_parity);
}

uint32
dnx_data_dram_general_info_dq_write_parity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_dq_write_parity);
}

uint32
dnx_data_dram_general_info_dq_read_parity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_dq_read_parity);
}

uint32
dnx_data_dram_general_info_dbi_read_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_dbi_read);
}

uint32
dnx_data_dram_general_info_dbi_write_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_dbi_write);
}

uint32
dnx_data_dram_general_info_write_latency_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_write_latency);
}

uint32
dnx_data_dram_general_info_read_latency_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_read_latency);
}

uint32
dnx_data_dram_general_info_read_data_enable_length_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_read_data_enable_length);
}

uint32
dnx_data_dram_general_info_parity_latency_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_parity_latency);
}

uint32
dnx_data_dram_general_info_actual_parity_latency_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_actual_parity_latency);
}

uint32
dnx_data_dram_general_info_tune_mode_on_init_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_tune_mode_on_init);
}

uint32
dnx_data_dram_general_info_command_parity_latency_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_command_parity_latency);
}

uint32
dnx_data_dram_general_info_crc_write_latency_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_crc_write_latency);
}

uint32
dnx_data_dram_general_info_crc_read_latency_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_crc_read_latency);
}

uint32
dnx_data_dram_general_info_crc_write_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_crc_write);
}

uint32
dnx_data_dram_general_info_crc_read_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_crc_read);
}

uint32
dnx_data_dram_general_info_device_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_device_size);
}



const dnx_data_dram_general_info_mr_defaults_t *
dnx_data_dram_general_info_mr_defaults_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_table_mr_defaults);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_general_info_mr_defaults_t *) data;

}

const dnx_data_dram_general_info_dram_info_t *
dnx_data_dram_general_info_dram_info_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_table_dram_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_dram_general_info_dram_info_t *) data;

}

const dnx_data_dram_general_info_timing_params_t *
dnx_data_dram_general_info_timing_params_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_table_timing_params);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_dram_general_info_timing_params_t *) data;

}

const dnx_data_dram_general_info_refresh_intervals_t *
dnx_data_dram_general_info_refresh_intervals_get(
    int unit,
    int temp_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_table_refresh_intervals);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, temp_index, 0);
    return (const dnx_data_dram_general_info_refresh_intervals_t *) data;

}


shr_error_e
dnx_data_dram_general_info_mr_defaults_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_general_info_mr_defaults_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_table_mr_defaults);
    data = (const dnx_data_dram_general_info_mr_defaults_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->value);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_general_info_dram_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_general_info_dram_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_table_dram_info);
    data = (const dnx_data_dram_general_info_dram_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dram_type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dram_bitmap);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_columns);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_rows);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_banks);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->data_rate);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ref_clock);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ctl_type);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_general_info_timing_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_general_info_timing_params_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_table_timing_params);
    data = (const dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->twr);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trp);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trtps);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trtpl);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trrds);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trrdl);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tfaw);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trcdwr);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trcdrd);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tras);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trc);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->twtrl);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->twtrs);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trtw);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tccdr);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tccds);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tccdl);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trrefd);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trfcsb);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trfc);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tmrd);
            break;
        case 21:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tmod);
            break;
        case 22:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->twtrtr);
            break;
        case 23:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->twrwtr);
            break;
        case 24:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->treftr);
            break;
        case 25:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trdtlt);
            break;
        case 26:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trcdwtr);
            break;
        case 27:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trcdrtr);
            break;
        case 28:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trcdltr);
            break;
        case 29:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tltrtr);
            break;
        case 30:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tltltr);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_general_info_refresh_intervals_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_general_info_refresh_intervals_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_table_refresh_intervals);
    data = (const dnx_data_dram_general_info_refresh_intervals_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trefi);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trefisb);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_dram_general_info_mr_defaults_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_table_mr_defaults);

}

const dnxc_data_table_info_t *
dnx_data_dram_general_info_dram_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_table_dram_info);

}

const dnxc_data_table_info_t *
dnx_data_dram_general_info_timing_params_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_table_timing_params);

}

const dnxc_data_table_info_t *
dnx_data_dram_general_info_refresh_intervals_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_table_refresh_intervals);

}






static shr_error_e
dnx_data_dram_address_translation_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "address_translation";
    submodule_data->doc = "information regarding the address translation scheme between logical to physical buffer";
    
    submodule_data->nof_features = _dnx_data_dram_address_translation_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dram address_translation features");

    
    submodule_data->nof_defines = _dnx_data_dram_address_translation_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dram address_translation defines");

    submodule_data->defines[dnx_data_dram_address_translation_define_matrix_column_size].name = "matrix_column_size";
    submodule_data->defines[dnx_data_dram_address_translation_define_matrix_column_size].doc = "the number of rows in the ATM";
    
    submodule_data->defines[dnx_data_dram_address_translation_define_matrix_column_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_address_translation_define_physical_address_transaction_size].name = "physical_address_transaction_size";
    submodule_data->defines[dnx_data_dram_address_translation_define_physical_address_transaction_size].doc = "nof bytes in each read/write from/to physical address";
    
    submodule_data->defines[dnx_data_dram_address_translation_define_physical_address_transaction_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_address_translation_define_nof_atms].name = "nof_atms";
    submodule_data->defines[dnx_data_dram_address_translation_define_nof_atms].doc = "number of address translation matrix";
    
    submodule_data->defines[dnx_data_dram_address_translation_define_nof_atms].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_address_translation_define_nof_tdus_per_dram].name = "nof_tdus_per_dram";
    submodule_data->defines[dnx_data_dram_address_translation_define_nof_tdus_per_dram].doc = "number of TDU blocks per DRAM";
    
    submodule_data->defines[dnx_data_dram_address_translation_define_nof_tdus_per_dram].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_address_translation_define_nof_tdus].name = "nof_tdus";
    submodule_data->defines[dnx_data_dram_address_translation_define_nof_tdus].doc = "number of TDU blocks";
    
    submodule_data->defines[dnx_data_dram_address_translation_define_nof_tdus].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_dram_address_translation_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dram address_translation tables");

    
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].name = "tdu_map";
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].doc = "Map between TDU and its data source";
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].size_of_values = sizeof(dnx_data_dram_address_translation_tdu_map_t);
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].entry_get = dnx_data_dram_address_translation_tdu_map_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].nof_keys = 2;
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].keys[0].name = "dram_index";
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].keys[0].doc = "DRAM index";
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].keys[1].name = "tdu_index";
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].keys[1].doc = "TDU index";

    
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].nof_values, "_dnx_data_dram_address_translation_table_tdu_map table values");
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].values[0].name = "data_source_id";
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].values[0].doc = "The source (core) from which the packets arrive at the TDU";
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_address_translation_tdu_map_t, data_source_id);

    
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].name = "matrix_configuration";
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].doc = "address translation matrix(ATM) configuration options";
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].size_of_values = sizeof(dnx_data_dram_address_translation_matrix_configuration_t);
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].entry_get = dnx_data_dram_address_translation_matrix_configuration_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].keys[0].name = "matrix_option";
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].keys[0].doc = "matrix selector";

    
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].nof_values, "_dnx_data_dram_address_translation_table_matrix_configuration table values");
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].values[0].name = "logical_to_physical";
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].values[0].type = "uint32[DNX_DATA_MAX_DRAM_ADDRESS_TRANSLATION_MATRIX_COLUMN_SIZE]";
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].values[0].doc = "the values of the ATM";
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_address_translation_matrix_configuration_t, logical_to_physical);

    
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].name = "interrupt_regs";
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].doc = "per address translation unit list of interrupt registers";
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].size_of_values = sizeof(dnx_data_dram_address_translation_interrupt_regs_t);
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].entry_get = dnx_data_dram_address_translation_interrupt_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].nof_values, "_dnx_data_dram_address_translation_table_interrupt_regs table values");
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].values[0].doc = "register";
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_address_translation_interrupt_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].name = "counter_regs";
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].doc = "per address translation unit list of counter registers";
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].size_of_values = sizeof(dnx_data_dram_address_translation_counter_regs_t);
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].entry_get = dnx_data_dram_address_translation_counter_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].nof_values, "_dnx_data_dram_address_translation_table_counter_regs table values");
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].values[0].doc = "register";
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_address_translation_counter_regs_t, reg);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dram_address_translation_feature_get(
    int unit,
    dnx_data_dram_address_translation_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_address_translation, feature);
}


uint32
dnx_data_dram_address_translation_matrix_column_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_address_translation, dnx_data_dram_address_translation_define_matrix_column_size);
}

uint32
dnx_data_dram_address_translation_physical_address_transaction_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_address_translation, dnx_data_dram_address_translation_define_physical_address_transaction_size);
}

uint32
dnx_data_dram_address_translation_nof_atms_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_address_translation, dnx_data_dram_address_translation_define_nof_atms);
}

uint32
dnx_data_dram_address_translation_nof_tdus_per_dram_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_address_translation, dnx_data_dram_address_translation_define_nof_tdus_per_dram);
}

uint32
dnx_data_dram_address_translation_nof_tdus_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_address_translation, dnx_data_dram_address_translation_define_nof_tdus);
}



const dnx_data_dram_address_translation_tdu_map_t *
dnx_data_dram_address_translation_tdu_map_get(
    int unit,
    int dram_index,
    int tdu_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_address_translation, dnx_data_dram_address_translation_table_tdu_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dram_index, tdu_index);
    return (const dnx_data_dram_address_translation_tdu_map_t *) data;

}

const dnx_data_dram_address_translation_matrix_configuration_t *
dnx_data_dram_address_translation_matrix_configuration_get(
    int unit,
    int matrix_option)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_address_translation, dnx_data_dram_address_translation_table_matrix_configuration);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, matrix_option, 0);
    return (const dnx_data_dram_address_translation_matrix_configuration_t *) data;

}

const dnx_data_dram_address_translation_interrupt_regs_t *
dnx_data_dram_address_translation_interrupt_regs_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_address_translation, dnx_data_dram_address_translation_table_interrupt_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_address_translation_interrupt_regs_t *) data;

}

const dnx_data_dram_address_translation_counter_regs_t *
dnx_data_dram_address_translation_counter_regs_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_address_translation, dnx_data_dram_address_translation_table_counter_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_address_translation_counter_regs_t *) data;

}


shr_error_e
dnx_data_dram_address_translation_tdu_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_address_translation_tdu_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_address_translation, dnx_data_dram_address_translation_table_tdu_map);
    data = (const dnx_data_dram_address_translation_tdu_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->data_source_id);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_address_translation_matrix_configuration_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_address_translation_matrix_configuration_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_address_translation, dnx_data_dram_address_translation_table_matrix_configuration);
    data = (const dnx_data_dram_address_translation_matrix_configuration_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_DRAM_ADDRESS_TRANSLATION_MATRIX_COLUMN_SIZE, data->logical_to_physical);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_address_translation_interrupt_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_address_translation_interrupt_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_address_translation, dnx_data_dram_address_translation_table_interrupt_regs);
    data = (const dnx_data_dram_address_translation_interrupt_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_address_translation_counter_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_address_translation_counter_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_address_translation, dnx_data_dram_address_translation_table_counter_regs);
    data = (const dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_dram_address_translation_tdu_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_address_translation, dnx_data_dram_address_translation_table_tdu_map);

}

const dnxc_data_table_info_t *
dnx_data_dram_address_translation_matrix_configuration_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_address_translation, dnx_data_dram_address_translation_table_matrix_configuration);

}

const dnxc_data_table_info_t *
dnx_data_dram_address_translation_interrupt_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_address_translation, dnx_data_dram_address_translation_table_interrupt_regs);

}

const dnxc_data_table_info_t *
dnx_data_dram_address_translation_counter_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_address_translation, dnx_data_dram_address_translation_table_counter_regs);

}






static shr_error_e
dnx_data_dram_buffers_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "buffers";
    submodule_data->doc = "information regarding BDs and BDBs";
    
    submodule_data->nof_features = _dnx_data_dram_buffers_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dram buffers features");

    submodule_data->features[dnx_data_dram_buffers_quarantine_buffers_if_tdu_corrected_errors].name = "quarantine_buffers_if_tdu_corrected_errors";
    submodule_data->features[dnx_data_dram_buffers_quarantine_buffers_if_tdu_corrected_errors].doc = "this feature represents sending buffers to quarantine after tdu corrected error event has happend";
    submodule_data->features[dnx_data_dram_buffers_quarantine_buffers_if_tdu_corrected_errors].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_buffers_quarantine_buffers_if_tdu_errors].name = "quarantine_buffers_if_tdu_errors";
    submodule_data->features[dnx_data_dram_buffers_quarantine_buffers_if_tdu_errors].doc = "this feature represents sending buffers to quarantine after tdu error event has happend";
    submodule_data->features[dnx_data_dram_buffers_quarantine_buffers_if_tdu_errors].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_buffers_bdb_release_mechanism_usage_counters].name = "bdb_release_mechanism_usage_counters";
    submodule_data->features[dnx_data_dram_buffers_bdb_release_mechanism_usage_counters].doc = "The DDP stores a usage counter per BDB, and release the BDB only when the counter is 0";
    submodule_data->features[dnx_data_dram_buffers_bdb_release_mechanism_usage_counters].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_dram_buffers_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dram buffers defines");

    submodule_data->defines[dnx_data_dram_buffers_define_allowed_errors].name = "allowed_errors";
    submodule_data->defines[dnx_data_dram_buffers_define_allowed_errors].doc = "number of times a buffer is allowed into the quarantine FIFO before it is deleted instead of restored";
    
    submodule_data->defines[dnx_data_dram_buffers_define_allowed_errors].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_buffers_define_nof_bdbs].name = "nof_bdbs";
    submodule_data->defines[dnx_data_dram_buffers_define_nof_bdbs].doc = "number of BDBs per core";
    
    submodule_data->defines[dnx_data_dram_buffers_define_nof_bdbs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_buffers_define_nof_fpc_banks].name = "nof_fpc_banks";
    submodule_data->defines[dnx_data_dram_buffers_define_nof_fpc_banks].doc = "number of free pointer controler banks";
    
    submodule_data->defines[dnx_data_dram_buffers_define_nof_fpc_banks].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_dram_buffers_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dram buffers tables");

    
    submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].name = "deleted_buffers_info";
    submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].doc = "deleted buffers info";
    submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].size_of_values = sizeof(dnx_data_dram_buffers_deleted_buffers_info_t);
    submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].entry_get = dnx_data_dram_buffers_deleted_buffers_info_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].nof_keys = 0;

    
    submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].nof_values, "_dnx_data_dram_buffers_table_deleted_buffers_info table values");
    submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].values[0].name = "deleted_buffers_file";
    submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].values[0].type = "char *";
    submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].values[0].doc = "full path to file containing deleted buffers for this unit";
    submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_buffers_deleted_buffers_info_t, deleted_buffers_file);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dram_buffers_feature_get(
    int unit,
    dnx_data_dram_buffers_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_buffers, feature);
}


uint32
dnx_data_dram_buffers_allowed_errors_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_buffers, dnx_data_dram_buffers_define_allowed_errors);
}

uint32
dnx_data_dram_buffers_nof_bdbs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_buffers, dnx_data_dram_buffers_define_nof_bdbs);
}

uint32
dnx_data_dram_buffers_nof_fpc_banks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_buffers, dnx_data_dram_buffers_define_nof_fpc_banks);
}



const dnx_data_dram_buffers_deleted_buffers_info_t *
dnx_data_dram_buffers_deleted_buffers_info_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_buffers, dnx_data_dram_buffers_table_deleted_buffers_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_dram_buffers_deleted_buffers_info_t *) data;

}


shr_error_e
dnx_data_dram_buffers_deleted_buffers_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_buffers_deleted_buffers_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_buffers, dnx_data_dram_buffers_table_deleted_buffers_info);
    data = (const dnx_data_dram_buffers_deleted_buffers_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->deleted_buffers_file == NULL ? "" : data->deleted_buffers_file);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_dram_buffers_deleted_buffers_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_buffers, dnx_data_dram_buffers_table_deleted_buffers_info);

}






static shr_error_e
dnx_data_dram_dram_block_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dram_block";
    submodule_data->doc = "information regarding DRAM blocked state.";
    
    submodule_data->nof_features = _dnx_data_dram_dram_block_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dram dram_block features");

    submodule_data->features[dnx_data_dram_dram_block_write_minus_read_leaky_bucket].name = "write_minus_read_leaky_bucket";
    submodule_data->features[dnx_data_dram_dram_block_write_minus_read_leaky_bucket].doc = "indication whether the write minus read leaky bucket is supported";
    submodule_data->features[dnx_data_dram_dram_block_write_minus_read_leaky_bucket].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_dram_block_write_leaky_bucket].name = "write_leaky_bucket";
    submodule_data->features[dnx_data_dram_dram_block_write_leaky_bucket].doc = "indication whether the write leaky bucket is supported";
    submodule_data->features[dnx_data_dram_dram_block_write_leaky_bucket].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_dram_block_write_minus_read_leaky_bucket_disable].name = "write_minus_read_leaky_bucket_disable";
    submodule_data->features[dnx_data_dram_dram_block_write_minus_read_leaky_bucket_disable].doc = "indication whether disabling the write minus read leaky bucket is supported";
    submodule_data->features[dnx_data_dram_dram_block_write_minus_read_leaky_bucket_disable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_dram_block_write_plus_read_leaky_bucket_disable].name = "write_plus_read_leaky_bucket_disable";
    submodule_data->features[dnx_data_dram_dram_block_write_plus_read_leaky_bucket_disable].doc = "indication whether disabling the write plus read leaky bucket is supported";
    submodule_data->features[dnx_data_dram_dram_block_write_plus_read_leaky_bucket_disable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_dram_block_average_read_inflights_leaky_bucket].name = "average_read_inflights_leaky_bucket";
    submodule_data->features[dnx_data_dram_dram_block_average_read_inflights_leaky_bucket].doc = "indication whether average read inflights leaky bucket is supported";
    submodule_data->features[dnx_data_dram_dram_block_average_read_inflights_leaky_bucket].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_dram_block_average_read_inflights_log2_window_size].name = "average_read_inflights_log2_window_size";
    submodule_data->features[dnx_data_dram_dram_block_average_read_inflights_log2_window_size].doc = "average-read-inflight uses a separate log2 based window size";
    submodule_data->features[dnx_data_dram_dram_block_average_read_inflights_log2_window_size].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_dram_dram_block_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dram dram_block defines");

    submodule_data->defines[dnx_data_dram_dram_block_define_leaky_bucket_window_size].name = "leaky_bucket_window_size";
    submodule_data->defines[dnx_data_dram_dram_block_define_leaky_bucket_window_size].doc = "window size for DRAM block calculation";
    
    submodule_data->defines[dnx_data_dram_dram_block_define_leaky_bucket_window_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dram_block_define_wmr_reset_on_deassert].name = "wmr_reset_on_deassert";
    submodule_data->defines[dnx_data_dram_dram_block_define_wmr_reset_on_deassert].doc = "indication whether to reset on deassert of write-minus-read leaky bucket";
    
    submodule_data->defines[dnx_data_dram_dram_block_define_wmr_reset_on_deassert].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dram_block_define_wmr_full_size].name = "wmr_full_size";
    submodule_data->defines[dnx_data_dram_dram_block_define_wmr_full_size].doc = "write-minus-read leaky bucket full size";
    
    submodule_data->defines[dnx_data_dram_dram_block_define_wmr_full_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dram_block_define_average_read_inflights_assert_threshold].name = "average_read_inflights_assert_threshold";
    submodule_data->defines[dnx_data_dram_dram_block_define_average_read_inflights_assert_threshold].doc = "average-read-inflights leaky bucket assert threshold";
    
    submodule_data->defines[dnx_data_dram_dram_block_define_average_read_inflights_assert_threshold].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dram_block_define_average_read_inflights_full_size].name = "average_read_inflights_full_size";
    submodule_data->defines[dnx_data_dram_dram_block_define_average_read_inflights_full_size].doc = "average-read-inflights leaky bucket full size";
    
    submodule_data->defines[dnx_data_dram_dram_block_define_average_read_inflights_full_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dram_block_define_wmr_decrement_thr_factor].name = "wmr_decrement_thr_factor";
    submodule_data->defines[dnx_data_dram_dram_block_define_wmr_decrement_thr_factor].doc = "factor for calculating decrement threshold for write_minus_read leaky bucket";
    
    submodule_data->defines[dnx_data_dram_dram_block_define_wmr_decrement_thr_factor].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_dram_block_define_wpr_increment_thr_factor].name = "wpr_increment_thr_factor";
    submodule_data->defines[dnx_data_dram_dram_block_define_wpr_increment_thr_factor].doc = "factor for calculating increment threshold for write-plus-read leaky bucket";
    
    submodule_data->defines[dnx_data_dram_dram_block_define_wpr_increment_thr_factor].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_dram_dram_block_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dram dram_block tables");

    
    submodule_data->tables[dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th].name = "wpr_leaky_bucket_increment_th";
    submodule_data->tables[dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th].doc = "Write-plus-read leaky bucket increment thresholds";
    submodule_data->tables[dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th].size_of_values = sizeof(dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_t);
    submodule_data->tables[dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th].entry_get = dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th].keys[0].doc = "threshold index";

    
    submodule_data->tables[dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th].nof_values, "_dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th table values");
    submodule_data->tables[dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th].values[0].name = "threshold";
    submodule_data->tables[dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th].values[0].doc = "Threshold value";
    submodule_data->tables[dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_t, threshold);

    
    submodule_data->tables[dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th].name = "write_leaky_bucket_increment_th";
    submodule_data->tables[dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th].doc = "Write leaky bucket increment thresholds";
    submodule_data->tables[dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th].size_of_values = sizeof(dnx_data_dram_dram_block_write_leaky_bucket_increment_th_t);
    submodule_data->tables[dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th].entry_get = dnx_data_dram_dram_block_write_leaky_bucket_increment_th_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th].keys[0].doc = "threshold index";

    
    submodule_data->tables[dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th].nof_values, "_dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th table values");
    submodule_data->tables[dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th].values[0].name = "threshold";
    submodule_data->tables[dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th].values[0].doc = "Threshold value";
    submodule_data->tables[dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_write_leaky_bucket_increment_th_t, threshold);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dram_dram_block_feature_get(
    int unit,
    dnx_data_dram_dram_block_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dram_block, feature);
}


uint32
dnx_data_dram_dram_block_leaky_bucket_window_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dram_block, dnx_data_dram_dram_block_define_leaky_bucket_window_size);
}

uint32
dnx_data_dram_dram_block_wmr_reset_on_deassert_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dram_block, dnx_data_dram_dram_block_define_wmr_reset_on_deassert);
}

uint32
dnx_data_dram_dram_block_wmr_full_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dram_block, dnx_data_dram_dram_block_define_wmr_full_size);
}

uint32
dnx_data_dram_dram_block_average_read_inflights_assert_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dram_block, dnx_data_dram_dram_block_define_average_read_inflights_assert_threshold);
}

uint32
dnx_data_dram_dram_block_average_read_inflights_full_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dram_block, dnx_data_dram_dram_block_define_average_read_inflights_full_size);
}

uint32
dnx_data_dram_dram_block_wmr_decrement_thr_factor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dram_block, dnx_data_dram_dram_block_define_wmr_decrement_thr_factor);
}

uint32
dnx_data_dram_dram_block_wpr_increment_thr_factor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dram_block, dnx_data_dram_dram_block_define_wpr_increment_thr_factor);
}



const dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_t *
dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dram_block, dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_t *) data;

}

const dnx_data_dram_dram_block_write_leaky_bucket_increment_th_t *
dnx_data_dram_dram_block_write_leaky_bucket_increment_th_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dram_block, dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_dram_block_write_leaky_bucket_increment_th_t *) data;

}


shr_error_e
dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dram_block, dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th);
    data = (const dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->threshold);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_dram_block_write_leaky_bucket_increment_th_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_dram_block_write_leaky_bucket_increment_th_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dram_block, dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th);
    data = (const dnx_data_dram_dram_block_write_leaky_bucket_increment_th_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->threshold);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dram_block, dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th);

}

const dnxc_data_table_info_t *
dnx_data_dram_dram_block_write_leaky_bucket_increment_th_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dram_block, dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th);

}






static shr_error_e
dnx_data_dram_dbal_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dbal";
    submodule_data->doc = "information regarding DRAM dbal.";
    
    submodule_data->nof_features = _dnx_data_dram_dbal_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dram dbal features");

    
    submodule_data->nof_defines = _dnx_data_dram_dbal_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dram dbal defines");

    submodule_data->defines[dnx_data_dram_dbal_define_hbm_trc_nof_bits].name = "hbm_trc_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_trc_nof_bits].doc = "Number of bits in HBM_TRC field";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_trc_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_wpr_increment_threshold_nof_bits].name = "wpr_increment_threshold_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_wpr_increment_threshold_nof_bits].doc = "Number of bits in DRAM_BLOCKED_WRITE_PLUS_READ_LEAKY_BUCKET_INCREMENT_SIZE fields";
    
    submodule_data->defines[dnx_data_dram_dbal_define_wpr_increment_threshold_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_average_read_inflights_increment_threshold_nof_bits].name = "average_read_inflights_increment_threshold_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_average_read_inflights_increment_threshold_nof_bits].doc = "Number of bits in DRAM_BLOCKED_AVERAGE_READ_INFLIGHTS_LEAKY_BUCKET_INCREMENT_SIZE fields";
    
    submodule_data->defines[dnx_data_dram_dbal_define_average_read_inflights_increment_threshold_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_average_read_inflights_decrement_threshold_nof_bits].name = "average_read_inflights_decrement_threshold_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_average_read_inflights_decrement_threshold_nof_bits].doc = "Number of bits in DRAM_BLOCKED_AVERAGE_READ_INFLIGHTS_LEAKY_BUCKET_DECREMENT_SIZE fields";
    
    submodule_data->defines[dnx_data_dram_dbal_define_average_read_inflights_decrement_threshold_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_pdiv_nof_bits].name = "hbm_pll_pdiv_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_pdiv_nof_bits].doc = "Number of bits in HBM_PLL_PDIV field";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_pdiv_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_ch_mdiv_nof_bits].name = "hbm_pll_ch_mdiv_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_ch_mdiv_nof_bits].doc = "Number of bits in HBM_PLL_CH_0_MDIV field";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_ch_mdiv_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_frefeff_info_nof_bits].name = "hbm_pll_frefeff_info_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_frefeff_info_nof_bits].doc = "Number of bits in HBM_PLL_FREFEFF_INFO field";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_frefeff_info_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_aux_post_enableb_nof_bits].name = "hbm_pll_aux_post_enableb_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_aux_post_enableb_nof_bits].doc = "Number of bits in HBM_PLL_AUX_POST_ENABLEB field";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_aux_post_enableb_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_ch_enableb_nof_bits].name = "hbm_pll_ch_enableb_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_ch_enableb_nof_bits].doc = "Number of bits in HBM_PLL_CH_ENABLEB field";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_ch_enableb_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_aux_post_diffcmos_en_nof_bits].name = "hbm_pll_aux_post_diffcmos_en_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_aux_post_diffcmos_en_nof_bits].doc = "Number of bits in HBM_PLL_AUX_POST_DIFFCMOS_EN field";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_aux_post_diffcmos_en_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_dram_bist_mode_nof_bits].name = "dram_bist_mode_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_dram_bist_mode_nof_bits].doc = "Number of bits in dram bist DATA_MODE field";
    
    submodule_data->defines[dnx_data_dram_dbal_define_dram_bist_mode_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_hbmc_index_nof_bits].name = "hbmc_index_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbmc_index_nof_bits].doc = "The size (number of bits) of HBMC_INDEX dbal field";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbmc_index_nof_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_dbal_define_hbmc_tdu_index_nof_bits].name = "hbmc_tdu_index_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbmc_tdu_index_nof_bits].doc = "The size (number of bits) of HBMC_TDU_INDEX dbal field";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbmc_tdu_index_nof_bits].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_dram_dbal_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dram dbal tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dram_dbal_feature_get(
    int unit,
    dnx_data_dram_dbal_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dbal, feature);
}


uint32
dnx_data_dram_dbal_hbm_trc_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dbal, dnx_data_dram_dbal_define_hbm_trc_nof_bits);
}

uint32
dnx_data_dram_dbal_wpr_increment_threshold_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dbal, dnx_data_dram_dbal_define_wpr_increment_threshold_nof_bits);
}

uint32
dnx_data_dram_dbal_average_read_inflights_increment_threshold_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dbal, dnx_data_dram_dbal_define_average_read_inflights_increment_threshold_nof_bits);
}

uint32
dnx_data_dram_dbal_average_read_inflights_decrement_threshold_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dbal, dnx_data_dram_dbal_define_average_read_inflights_decrement_threshold_nof_bits);
}

uint32
dnx_data_dram_dbal_hbm_pll_pdiv_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dbal, dnx_data_dram_dbal_define_hbm_pll_pdiv_nof_bits);
}

uint32
dnx_data_dram_dbal_hbm_pll_ch_mdiv_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dbal, dnx_data_dram_dbal_define_hbm_pll_ch_mdiv_nof_bits);
}

uint32
dnx_data_dram_dbal_hbm_pll_frefeff_info_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dbal, dnx_data_dram_dbal_define_hbm_pll_frefeff_info_nof_bits);
}

uint32
dnx_data_dram_dbal_hbm_pll_aux_post_enableb_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dbal, dnx_data_dram_dbal_define_hbm_pll_aux_post_enableb_nof_bits);
}

uint32
dnx_data_dram_dbal_hbm_pll_ch_enableb_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dbal, dnx_data_dram_dbal_define_hbm_pll_ch_enableb_nof_bits);
}

uint32
dnx_data_dram_dbal_hbm_pll_aux_post_diffcmos_en_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dbal, dnx_data_dram_dbal_define_hbm_pll_aux_post_diffcmos_en_nof_bits);
}

uint32
dnx_data_dram_dbal_dram_bist_mode_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dbal, dnx_data_dram_dbal_define_dram_bist_mode_nof_bits);
}

uint32
dnx_data_dram_dbal_hbmc_index_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dbal, dnx_data_dram_dbal_define_hbmc_index_nof_bits);
}

uint32
dnx_data_dram_dbal_hbmc_tdu_index_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dbal, dnx_data_dram_dbal_define_hbmc_tdu_index_nof_bits);
}










static shr_error_e
dnx_data_dram_firmware_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "firmware";
    submodule_data->doc = "information regarding DRAM PHY firmware.";
    
    submodule_data->nof_features = _dnx_data_dram_firmware_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dram firmware features");

    
    submodule_data->nof_defines = _dnx_data_dram_firmware_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dram firmware defines");

    submodule_data->defines[dnx_data_dram_firmware_define_nof_hbm_spare_data_results].name = "nof_hbm_spare_data_results";
    submodule_data->defines[dnx_data_dram_firmware_define_nof_hbm_spare_data_results].doc = "Number of HBM spare data results";
    
    submodule_data->defines[dnx_data_dram_firmware_define_nof_hbm_spare_data_results].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_firmware_define_sbus_clock_divider].name = "sbus_clock_divider";
    submodule_data->defines[dnx_data_dram_firmware_define_sbus_clock_divider].doc = "Divide sbus_ctrl_clk with this value to create the SBus clock. Max clock after division should be 200MHz.";
    
    submodule_data->defines[dnx_data_dram_firmware_define_sbus_clock_divider].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_firmware_define_snap_state_init_done].name = "snap_state_init_done";
    submodule_data->defines[dnx_data_dram_firmware_define_snap_state_init_done].doc = "The value which indicates that the SNaP initialization is done";
    
    submodule_data->defines[dnx_data_dram_firmware_define_snap_state_init_done].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_dram_firmware_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dram firmware tables");

    
    submodule_data->tables[dnx_data_dram_firmware_table_rom].name = "rom";
    submodule_data->tables[dnx_data_dram_firmware_table_rom].doc = "Firmaware ROM info";
    submodule_data->tables[dnx_data_dram_firmware_table_rom].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_firmware_table_rom].size_of_values = sizeof(dnx_data_dram_firmware_rom_t);
    submodule_data->tables[dnx_data_dram_firmware_table_rom].entry_get = dnx_data_dram_firmware_rom_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_firmware_table_rom].nof_keys = 0;

    
    submodule_data->tables[dnx_data_dram_firmware_table_rom].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_firmware_table_rom].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_firmware_table_rom].nof_values, "_dnx_data_dram_firmware_table_rom table values");
    submodule_data->tables[dnx_data_dram_firmware_table_rom].values[0].name = "filename";
    submodule_data->tables[dnx_data_dram_firmware_table_rom].values[0].type = "char *";
    submodule_data->tables[dnx_data_dram_firmware_table_rom].values[0].doc = "path to the HBM PHY Firmware ROM";
    submodule_data->tables[dnx_data_dram_firmware_table_rom].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_firmware_rom_t, filename);

    
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].name = "operation_status";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].doc = "Indications on the firmware operation status. The values are taken from the HBM PHY Firmware programming guide.";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].size_of_values = sizeof(dnx_data_dram_firmware_operation_status_t);
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].entry_get = dnx_data_dram_firmware_operation_status_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].nof_keys = 0;

    
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_firmware_table_operation_status].nof_values, "_dnx_data_dram_firmware_table_operation_status table values");
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[0].name = "done";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[0].doc = "operation done";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_firmware_operation_status_t, done);
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[1].name = "active";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[1].type = "uint32";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[1].doc = "operation is still active";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_firmware_operation_status_t, active);
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[2].name = "errors_detected";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[2].type = "uint32";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[2].doc = "operation resulted with an error";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[2].offset = UTILEX_OFFSETOF(dnx_data_dram_firmware_operation_status_t, errors_detected);

    
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].name = "operation_result_error_code";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].doc = "Error codes that can be encountered when using the Get Operation Result interrupt code";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].size_of_values = sizeof(dnx_data_dram_firmware_operation_result_error_code_t);
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].entry_get = dnx_data_dram_firmware_operation_result_error_code_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].keys[0].name = "error_code";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].keys[0].doc = "the resulted error code";

    
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].nof_values, "_dnx_data_dram_firmware_table_operation_result_error_code table values");
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].values[0].name = "description";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].values[0].type = "char *";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].values[0].doc = "description of the error code";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_firmware_operation_result_error_code_t, description);

    
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].name = "operation_result_last_operation";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].doc = "When operation resulted with an error, the operation is saved. This table shows the meaning of the failing operation code.";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].size_of_values = sizeof(dnx_data_dram_firmware_operation_result_last_operation_t);
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].entry_get = dnx_data_dram_firmware_operation_result_last_operation_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].keys[0].name = "operation_code";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].keys[0].doc = "the operation code";

    
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].nof_values, "_dnx_data_dram_firmware_table_operation_result_last_operation table values");
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].values[0].name = "description";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].values[0].type = "char *";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].values[0].doc = "description of the operation code";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_firmware_operation_result_last_operation_t, description);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dram_firmware_feature_get(
    int unit,
    dnx_data_dram_firmware_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_firmware, feature);
}


uint32
dnx_data_dram_firmware_nof_hbm_spare_data_results_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_firmware, dnx_data_dram_firmware_define_nof_hbm_spare_data_results);
}

uint32
dnx_data_dram_firmware_sbus_clock_divider_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_firmware, dnx_data_dram_firmware_define_sbus_clock_divider);
}

uint32
dnx_data_dram_firmware_snap_state_init_done_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_firmware, dnx_data_dram_firmware_define_snap_state_init_done);
}



const dnx_data_dram_firmware_rom_t *
dnx_data_dram_firmware_rom_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_firmware, dnx_data_dram_firmware_table_rom);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_dram_firmware_rom_t *) data;

}

const dnx_data_dram_firmware_operation_status_t *
dnx_data_dram_firmware_operation_status_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_firmware, dnx_data_dram_firmware_table_operation_status);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_dram_firmware_operation_status_t *) data;

}

const dnx_data_dram_firmware_operation_result_error_code_t *
dnx_data_dram_firmware_operation_result_error_code_get(
    int unit,
    int error_code)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_firmware, dnx_data_dram_firmware_table_operation_result_error_code);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, error_code, 0);
    return (const dnx_data_dram_firmware_operation_result_error_code_t *) data;

}

const dnx_data_dram_firmware_operation_result_last_operation_t *
dnx_data_dram_firmware_operation_result_last_operation_get(
    int unit,
    int operation_code)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_firmware, dnx_data_dram_firmware_table_operation_result_last_operation);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, operation_code, 0);
    return (const dnx_data_dram_firmware_operation_result_last_operation_t *) data;

}


shr_error_e
dnx_data_dram_firmware_rom_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_firmware_rom_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_firmware, dnx_data_dram_firmware_table_rom);
    data = (const dnx_data_dram_firmware_rom_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->filename == NULL ? "" : data->filename);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_firmware_operation_status_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_firmware_operation_status_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_firmware, dnx_data_dram_firmware_table_operation_status);
    data = (const dnx_data_dram_firmware_operation_status_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->done);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->active);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->errors_detected);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_firmware_operation_result_error_code_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_firmware_operation_result_error_code_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_firmware, dnx_data_dram_firmware_table_operation_result_error_code);
    data = (const dnx_data_dram_firmware_operation_result_error_code_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->description == NULL ? "" : data->description);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_firmware_operation_result_last_operation_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_firmware_operation_result_last_operation_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_firmware, dnx_data_dram_firmware_table_operation_result_last_operation);
    data = (const dnx_data_dram_firmware_operation_result_last_operation_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->description == NULL ? "" : data->description);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_dram_firmware_rom_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_firmware, dnx_data_dram_firmware_table_rom);

}

const dnxc_data_table_info_t *
dnx_data_dram_firmware_operation_status_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_firmware, dnx_data_dram_firmware_table_operation_status);

}

const dnxc_data_table_info_t *
dnx_data_dram_firmware_operation_result_error_code_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_firmware, dnx_data_dram_firmware_table_operation_result_error_code);

}

const dnxc_data_table_info_t *
dnx_data_dram_firmware_operation_result_last_operation_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_firmware, dnx_data_dram_firmware_table_operation_result_last_operation);

}






static shr_error_e
dnx_data_dram_apd_phy_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "apd_phy";
    submodule_data->doc = "Configurations for APD PHY";
    
    submodule_data->nof_features = _dnx_data_dram_apd_phy_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dram apd_phy features");

    
    submodule_data->nof_defines = _dnx_data_dram_apd_phy_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dram apd_phy defines");

    
    submodule_data->nof_tables = _dnx_data_dram_apd_phy_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dram apd_phy tables");

    
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].name = "pll";
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].doc = "PLL parameters";
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].size_of_values = sizeof(dnx_data_dram_apd_phy_pll_t);
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].entry_get = dnx_data_dram_apd_phy_pll_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].nof_keys = 0;

    
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_apd_phy_table_pll].nof_values, "_dnx_data_dram_apd_phy_table_pll table values");
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values[0].name = "ndiv_int";
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values[0].doc = "N divider of the PLL";
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_apd_phy_pll_t, ndiv_int);
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values[1].name = "ch1_mdiv";
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values[1].type = "uint32";
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values[1].doc = "M-divider for channel 1 of the PLL";
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_apd_phy_pll_t, ch1_mdiv);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dram_apd_phy_feature_get(
    int unit,
    dnx_data_dram_apd_phy_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_apd_phy, feature);
}




const dnx_data_dram_apd_phy_pll_t *
dnx_data_dram_apd_phy_pll_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_apd_phy, dnx_data_dram_apd_phy_table_pll);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_dram_apd_phy_pll_t *) data;

}


shr_error_e
dnx_data_dram_apd_phy_pll_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_apd_phy_pll_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_apd_phy, dnx_data_dram_apd_phy_table_pll);
    data = (const dnx_data_dram_apd_phy_pll_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ndiv_int);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ch1_mdiv);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_dram_apd_phy_pll_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_apd_phy, dnx_data_dram_apd_phy_table_pll);

}



shr_error_e
dnx_data_dram_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "dram";
    module_data->nof_submodules = _dnx_data_dram_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data dram submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_dram_hbm_init(unit, &module_data->submodules[dnx_data_dram_submodule_hbm]));
    SHR_IF_ERR_EXIT(dnx_data_dram_gddr6_init(unit, &module_data->submodules[dnx_data_dram_submodule_gddr6]));
    SHR_IF_ERR_EXIT(dnx_data_dram_general_info_init(unit, &module_data->submodules[dnx_data_dram_submodule_general_info]));
    SHR_IF_ERR_EXIT(dnx_data_dram_address_translation_init(unit, &module_data->submodules[dnx_data_dram_submodule_address_translation]));
    SHR_IF_ERR_EXIT(dnx_data_dram_buffers_init(unit, &module_data->submodules[dnx_data_dram_submodule_buffers]));
    SHR_IF_ERR_EXIT(dnx_data_dram_dram_block_init(unit, &module_data->submodules[dnx_data_dram_submodule_dram_block]));
    SHR_IF_ERR_EXIT(dnx_data_dram_dbal_init(unit, &module_data->submodules[dnx_data_dram_submodule_dbal]));
    SHR_IF_ERR_EXIT(dnx_data_dram_firmware_init(unit, &module_data->submodules[dnx_data_dram_submodule_firmware]));
    SHR_IF_ERR_EXIT(dnx_data_dram_apd_phy_init(unit, &module_data->submodules[dnx_data_dram_submodule_apd_phy]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dram_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dram_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_dram_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dram_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_dram_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dram_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_dram_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dram_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_dram_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dram_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_dram_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dram_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_dram_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_dram_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dram_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_dram_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_dram_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b1_data_dram_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dram_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_dram_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

