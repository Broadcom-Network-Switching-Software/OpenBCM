
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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


extern shr_error_e j2p_a1_data_dram_attach(
    int unit);


extern shr_error_e j2x_a0_data_dram_attach(
    int unit);




static shr_error_e
dnx_data_dram_hbm_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "hbm";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_dram_hbm_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dram hbm features");

    submodule_data->features[dnx_data_dram_hbm_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_dram_hbm_is_supported].doc = "";
    submodule_data->features[dnx_data_dram_hbm_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_dword_alignment_check].name = "dword_alignment_check";
    submodule_data->features[dnx_data_dram_hbm_dword_alignment_check].doc = "";
    submodule_data->features[dnx_data_dram_hbm_dword_alignment_check].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_low_temperature_dram_protction].name = "low_temperature_dram_protction";
    submodule_data->features[dnx_data_dram_hbm_low_temperature_dram_protction].doc = "";
    submodule_data->features[dnx_data_dram_hbm_low_temperature_dram_protction].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_is_delete_bdb_supported].name = "is_delete_bdb_supported";
    submodule_data->features[dnx_data_dram_hbm_is_delete_bdb_supported].doc = "";
    submodule_data->features[dnx_data_dram_hbm_is_delete_bdb_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_cpu2tap_access].name = "cpu2tap_access";
    submodule_data->features[dnx_data_dram_hbm_cpu2tap_access].doc = "";
    submodule_data->features[dnx_data_dram_hbm_cpu2tap_access].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_seamless_commands_to_same_bank_control].name = "seamless_commands_to_same_bank_control";
    submodule_data->features[dnx_data_dram_hbm_seamless_commands_to_same_bank_control].doc = "";
    submodule_data->features[dnx_data_dram_hbm_seamless_commands_to_same_bank_control].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_vendor_info_disable].name = "vendor_info_disable";
    submodule_data->features[dnx_data_dram_hbm_vendor_info_disable].doc = "";
    submodule_data->features[dnx_data_dram_hbm_vendor_info_disable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_apd_phy].name = "apd_phy";
    submodule_data->features[dnx_data_dram_hbm_apd_phy].doc = "";
    submodule_data->features[dnx_data_dram_hbm_apd_phy].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_is_hrc_supported].name = "is_hrc_supported";
    submodule_data->features[dnx_data_dram_hbm_is_hrc_supported].doc = "";
    submodule_data->features[dnx_data_dram_hbm_is_hrc_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_channel_wds_inflight_threshold].name = "channel_wds_inflight_threshold";
    submodule_data->features[dnx_data_dram_hbm_channel_wds_inflight_threshold].doc = "";
    submodule_data->features[dnx_data_dram_hbm_channel_wds_inflight_threshold].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_is_freeze_boot_supported].name = "is_freeze_boot_supported";
    submodule_data->features[dnx_data_dram_hbm_is_freeze_boot_supported].doc = "";
    submodule_data->features[dnx_data_dram_hbm_is_freeze_boot_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_hbm2e_support].name = "hbm2e_support";
    submodule_data->features[dnx_data_dram_hbm_hbm2e_support].doc = "";
    submodule_data->features[dnx_data_dram_hbm_hbm2e_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_is_coherency_enforcer_supported].name = "is_coherency_enforcer_supported";
    submodule_data->features[dnx_data_dram_hbm_is_coherency_enforcer_supported].doc = "";
    submodule_data->features[dnx_data_dram_hbm_is_coherency_enforcer_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_hbm_sbus_execute_delay].name = "sbus_execute_delay";
    submodule_data->features[dnx_data_dram_hbm_sbus_execute_delay].doc = "";
    submodule_data->features[dnx_data_dram_hbm_sbus_execute_delay].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_dram_hbm_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dram hbm defines");

    submodule_data->defines[dnx_data_dram_hbm_define_burst_length].name = "burst_length";
    submodule_data->defines[dnx_data_dram_hbm_define_burst_length].doc = "";
    
    submodule_data->defines[dnx_data_dram_hbm_define_burst_length].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_stop_traffic_temp_threshold].name = "stop_traffic_temp_threshold";
    submodule_data->defines[dnx_data_dram_hbm_define_stop_traffic_temp_threshold].doc = "";
    
    submodule_data->defines[dnx_data_dram_hbm_define_stop_traffic_temp_threshold].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_stop_traffic_low_temp_threshold].name = "stop_traffic_low_temp_threshold";
    submodule_data->defines[dnx_data_dram_hbm_define_stop_traffic_low_temp_threshold].doc = "";
    
    submodule_data->defines[dnx_data_dram_hbm_define_stop_traffic_low_temp_threshold].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_restore_traffic_temp_threshold].name = "restore_traffic_temp_threshold";
    submodule_data->defines[dnx_data_dram_hbm_define_restore_traffic_temp_threshold].doc = "";
    
    submodule_data->defines[dnx_data_dram_hbm_define_restore_traffic_temp_threshold].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_restore_traffic_low_temp_threshold].name = "restore_traffic_low_temp_threshold";
    submodule_data->defines[dnx_data_dram_hbm_define_restore_traffic_low_temp_threshold].doc = "";
    
    submodule_data->defines[dnx_data_dram_hbm_define_restore_traffic_low_temp_threshold].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_usec_between_temp_samples].name = "usec_between_temp_samples";
    submodule_data->defines[dnx_data_dram_hbm_define_usec_between_temp_samples].doc = "";
    
    submodule_data->defines[dnx_data_dram_hbm_define_usec_between_temp_samples].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_power_down_temp_threshold].name = "power_down_temp_threshold";
    submodule_data->defines[dnx_data_dram_hbm_define_power_down_temp_threshold].doc = "";
    
    submodule_data->defines[dnx_data_dram_hbm_define_power_down_temp_threshold].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_power_down_low_temp_threshold].name = "power_down_low_temp_threshold";
    submodule_data->defines[dnx_data_dram_hbm_define_power_down_low_temp_threshold].doc = "";
    
    submodule_data->defines[dnx_data_dram_hbm_define_power_down_low_temp_threshold].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_dram_temp_monitor_enable].name = "dram_temp_monitor_enable";
    submodule_data->defines[dnx_data_dram_hbm_define_dram_temp_monitor_enable].doc = "";
    
    submodule_data->defines[dnx_data_dram_hbm_define_dram_temp_monitor_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_start_disabled].name = "start_disabled";
    submodule_data->defines[dnx_data_dram_hbm_define_start_disabled].doc = "";
    
    submodule_data->defines[dnx_data_dram_hbm_define_start_disabled].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_output_enable_length].name = "output_enable_length";
    submodule_data->defines[dnx_data_dram_hbm_define_output_enable_length].doc = "";
    
    submodule_data->defines[dnx_data_dram_hbm_define_output_enable_length].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_output_enable_delay].name = "output_enable_delay";
    submodule_data->defines[dnx_data_dram_hbm_define_output_enable_delay].doc = "";
    
    submodule_data->defines[dnx_data_dram_hbm_define_output_enable_delay].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_driver_strength].name = "driver_strength";
    submodule_data->defines[dnx_data_dram_hbm_define_driver_strength].doc = "";
    
    submodule_data->defines[dnx_data_dram_hbm_define_driver_strength].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_t_rdlat_offset].name = "t_rdlat_offset";
    submodule_data->defines[dnx_data_dram_hbm_define_t_rdlat_offset].doc = "";
    
    submodule_data->defines[dnx_data_dram_hbm_define_t_rdlat_offset].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_sbus_execute_delay_default].name = "sbus_execute_delay_default";
    submodule_data->defines[dnx_data_dram_hbm_define_sbus_execute_delay_default].doc = "";
    
    submodule_data->defines[dnx_data_dram_hbm_define_sbus_execute_delay_default].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_enable_dram_bist_on_init].name = "enable_dram_bist_on_init";
    submodule_data->defines[dnx_data_dram_hbm_define_enable_dram_bist_on_init].doc = "";
    
    submodule_data->defines[dnx_data_dram_hbm_define_enable_dram_bist_on_init].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_hbm_define_default_model_part_num].name = "default_model_part_num";
    submodule_data->defines[dnx_data_dram_hbm_define_default_model_part_num].doc = "";
    
    submodule_data->defines[dnx_data_dram_hbm_define_default_model_part_num].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_hbm_define_nof_channel_dwords].name = "nof_channel_dwords";
    submodule_data->defines[dnx_data_dram_hbm_define_nof_channel_dwords].doc = "";
    
    submodule_data->defines[dnx_data_dram_hbm_define_nof_channel_dwords].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_hbm_define_wds_size].name = "wds_size";
    submodule_data->defines[dnx_data_dram_hbm_define_wds_size].doc = "";
    
    submodule_data->defines[dnx_data_dram_hbm_define_wds_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_dram_hbm_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dram hbm tables");

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].name = "channel_symmetric_regs";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].size_of_values = sizeof(dnx_data_dram_hbm_channel_symmetric_regs_t);
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].entry_get = dnx_data_dram_hbm_channel_symmetric_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].nof_values, "_dnx_data_dram_hbm_table_channel_symmetric_regs table values");
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_symmetric_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_channel_symmetric_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].name = "channel_not_symmetric_regs";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].size_of_values = sizeof(dnx_data_dram_hbm_channel_not_symmetric_regs_t);
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].entry_get = dnx_data_dram_hbm_channel_not_symmetric_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].nof_values, "_dnx_data_dram_hbm_table_channel_not_symmetric_regs table values");
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_not_symmetric_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_channel_not_symmetric_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].name = "controller_symmetric_regs";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].size_of_values = sizeof(dnx_data_dram_hbm_controller_symmetric_regs_t);
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].entry_get = dnx_data_dram_hbm_controller_symmetric_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].nof_values, "_dnx_data_dram_hbm_table_controller_symmetric_regs table values");
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_symmetric_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_controller_symmetric_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].name = "controller_not_symmetric_regs";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].size_of_values = sizeof(dnx_data_dram_hbm_controller_not_symmetric_regs_t);
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].entry_get = dnx_data_dram_hbm_controller_not_symmetric_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].nof_values, "_dnx_data_dram_hbm_table_controller_not_symmetric_regs table values");
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_not_symmetric_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_controller_not_symmetric_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].name = "channel_interrupt_regs";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].size_of_values = sizeof(dnx_data_dram_hbm_channel_interrupt_regs_t);
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].entry_get = dnx_data_dram_hbm_channel_interrupt_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].nof_values, "_dnx_data_dram_hbm_table_channel_interrupt_regs table values");
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_interrupt_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_channel_interrupt_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].name = "controller_interrupt_regs";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].size_of_values = sizeof(dnx_data_dram_hbm_controller_interrupt_regs_t);
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].entry_get = dnx_data_dram_hbm_controller_interrupt_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].nof_values, "_dnx_data_dram_hbm_table_controller_interrupt_regs table values");
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_interrupt_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_controller_interrupt_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].name = "channel_debug_regs";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].size_of_values = sizeof(dnx_data_dram_hbm_channel_debug_regs_t);
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].entry_get = dnx_data_dram_hbm_channel_debug_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].nof_values, "_dnx_data_dram_hbm_table_channel_debug_regs table values");
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_channel_debug_regs_t, reg);
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].values[1].name = "reassuring_str";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].values[1].type = "char *";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].values[1].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_debug_regs].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_channel_debug_regs_t, reassuring_str);

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].name = "channel_counter_regs";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].size_of_values = sizeof(dnx_data_dram_hbm_channel_counter_regs_t);
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].entry_get = dnx_data_dram_hbm_channel_counter_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].nof_values, "_dnx_data_dram_hbm_table_channel_counter_regs table values");
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_counter_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_channel_counter_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].name = "channel_type_regs";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].size_of_values = sizeof(dnx_data_dram_hbm_channel_type_regs_t);
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].entry_get = dnx_data_dram_hbm_channel_type_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].nof_values, "_dnx_data_dram_hbm_table_channel_type_regs table values");
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_channel_type_regs_t, reg);
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].values[1].name = "type";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].values[1].type = "dnx_hbmc_diag_registers_type_t";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].values[1].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_channel_type_regs].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_channel_type_regs_t, type);

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].name = "controller_info_regs";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].size_of_values = sizeof(dnx_data_dram_hbm_controller_info_regs_t);
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].entry_get = dnx_data_dram_hbm_controller_info_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].nof_values, "_dnx_data_dram_hbm_table_controller_info_regs table values");
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_controller_info_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_controller_info_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_hbm_table_bist].name = "bist";
    submodule_data->tables[dnx_data_dram_hbm_table_bist].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_bist].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_bist].size_of_values = sizeof(dnx_data_dram_hbm_bist_t);
    submodule_data->tables[dnx_data_dram_hbm_table_bist].entry_get = dnx_data_dram_hbm_bist_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_bist].nof_keys = 0;

    
    submodule_data->tables[dnx_data_dram_hbm_table_bist].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_bist].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_bist].nof_values, "_dnx_data_dram_hbm_table_bist table values");
    submodule_data->tables[dnx_data_dram_hbm_table_bist].values[0].name = "prbs_seeds";
    submodule_data->tables[dnx_data_dram_hbm_table_bist].values[0].type = "uint32[HBMC_SHMOO_BIST_NOF_PRBS_DATA_SEEDS]";
    submodule_data->tables[dnx_data_dram_hbm_table_bist].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_bist].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_bist_t, prbs_seeds);
    submodule_data->tables[dnx_data_dram_hbm_table_bist].values[1].name = "same_row_commands";
    submodule_data->tables[dnx_data_dram_hbm_table_bist].values[1].type = "uint32";
    submodule_data->tables[dnx_data_dram_hbm_table_bist].values[1].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_bist].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_bist_t, same_row_commands);

    
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].name = "hbc_last_in_chain";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].size_of_values = sizeof(dnx_data_dram_hbm_hbc_last_in_chain_t);
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].entry_get = dnx_data_dram_hbm_hbc_last_in_chain_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].nof_keys = 2;
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].keys[0].name = "dram_bitmap";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].keys[0].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].keys[1].name = "dram_index";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].nof_values, "_dnx_data_dram_hbm_table_hbc_last_in_chain table values");
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].values[0].name = "is_last_in_chain";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_last_in_chain].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_hbc_last_in_chain_t, is_last_in_chain);

    
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_sbus_chain].name = "hbc_sbus_chain";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_sbus_chain].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_sbus_chain].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_sbus_chain].size_of_values = sizeof(dnx_data_dram_hbm_hbc_sbus_chain_t);
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_sbus_chain].entry_get = dnx_data_dram_hbm_hbc_sbus_chain_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_sbus_chain].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_sbus_chain].keys[0].name = "channel";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_sbus_chain].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_sbus_chain].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_hbc_sbus_chain].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_hbc_sbus_chain].nof_values, "_dnx_data_dram_hbm_table_hbc_sbus_chain table values");
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_sbus_chain].values[0].name = "sbus_chain";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_sbus_chain].values[0].type = "int";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_sbus_chain].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_hbc_sbus_chain].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_hbc_sbus_chain_t, sbus_chain);

    
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].name = "dll_and_qc_conf_vals_per_freq_range";
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].size_of_values = sizeof(dnx_data_dram_hbm_dll_and_qc_conf_vals_per_freq_range_t);
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].entry_get = dnx_data_dram_hbm_dll_and_qc_conf_vals_per_freq_range_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].nof_values = 5;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].nof_values, "_dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range table values");
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].values[0].name = "freq_x2_range_upper_limit";
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].values[0].type = "int";
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_dll_and_qc_conf_vals_per_freq_range_t, freq_x2_range_upper_limit);
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].values[1].name = "dll_config_val";
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].values[1].type = "uint32";
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].values[1].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_dll_and_qc_conf_vals_per_freq_range_t, dll_config_val);
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].values[2].name = "qc_ck_delay_config_val";
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].values[2].type = "uint32";
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].values[2].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].values[2].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_dll_and_qc_conf_vals_per_freq_range_t, qc_ck_delay_config_val);
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].values[3].name = "qc_wr_delay_config_val";
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].values[3].type = "uint32";
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].values[3].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].values[3].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_dll_and_qc_conf_vals_per_freq_range_t, qc_wr_delay_config_val);
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].values[4].name = "qc_rd_delay_config_val";
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].values[4].type = "uint32";
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].values[4].doc = "";
    submodule_data->tables[dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range].values[4].offset = UTILEX_OFFSETOF(dnx_data_dram_hbm_dll_and_qc_conf_vals_per_freq_range_t, qc_rd_delay_config_val);


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
dnx_data_dram_hbm_driver_strength_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_define_driver_strength);
}

uint32
dnx_data_dram_hbm_t_rdlat_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_define_t_rdlat_offset);
}

uint32
dnx_data_dram_hbm_sbus_execute_delay_default_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_define_sbus_execute_delay_default);
}

uint32
dnx_data_dram_hbm_enable_dram_bist_on_init_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_define_enable_dram_bist_on_init);
}

uint32
dnx_data_dram_hbm_default_model_part_num_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_define_default_model_part_num);
}

uint32
dnx_data_dram_hbm_nof_channel_dwords_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_define_nof_channel_dwords);
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

const dnx_data_dram_hbm_hbc_sbus_chain_t *
dnx_data_dram_hbm_hbc_sbus_chain_get(
    int unit,
    int channel)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_hbc_sbus_chain);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, channel, 0);
    return (const dnx_data_dram_hbm_hbc_sbus_chain_t *) data;

}

const dnx_data_dram_hbm_dll_and_qc_conf_vals_per_freq_range_t *
dnx_data_dram_hbm_dll_and_qc_conf_vals_per_freq_range_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_hbm_dll_and_qc_conf_vals_per_freq_range_t *) data;

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
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->same_row_commands);
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

shr_error_e
dnx_data_dram_hbm_hbc_sbus_chain_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_hbm_hbc_sbus_chain_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_hbc_sbus_chain);
    data = (const dnx_data_dram_hbm_hbc_sbus_chain_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->sbus_chain);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dram_hbm_dll_and_qc_conf_vals_per_freq_range_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_hbm_dll_and_qc_conf_vals_per_freq_range_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range);
    data = (const dnx_data_dram_hbm_dll_and_qc_conf_vals_per_freq_range_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->freq_x2_range_upper_limit);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dll_config_val);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->qc_ck_delay_config_val);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->qc_wr_delay_config_val);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->qc_rd_delay_config_val);
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

const dnxc_data_table_info_t *
dnx_data_dram_hbm_hbc_sbus_chain_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_hbc_sbus_chain);

}

const dnxc_data_table_info_t *
dnx_data_dram_hbm_dll_and_qc_conf_vals_per_freq_range_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_hbm, dnx_data_dram_hbm_table_dll_and_qc_conf_vals_per_freq_range);

}






static shr_error_e
dnx_data_dram_gddr6_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "gddr6";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_dram_gddr6_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dram gddr6 features");

    submodule_data->features[dnx_data_dram_gddr6_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_dram_gddr6_is_supported].doc = "";
    submodule_data->features[dnx_data_dram_gddr6_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_gddr6_allow_disable_read_crc].name = "allow_disable_read_crc";
    submodule_data->features[dnx_data_dram_gddr6_allow_disable_read_crc].doc = "";
    submodule_data->features[dnx_data_dram_gddr6_allow_disable_read_crc].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_gddr6_interleaved_refresh_cycles].name = "interleaved_refresh_cycles";
    submodule_data->features[dnx_data_dram_gddr6_interleaved_refresh_cycles].doc = "";
    submodule_data->features[dnx_data_dram_gddr6_interleaved_refresh_cycles].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_gddr6_refresh_pend_switch].name = "refresh_pend_switch";
    submodule_data->features[dnx_data_dram_gddr6_refresh_pend_switch].doc = "";
    submodule_data->features[dnx_data_dram_gddr6_refresh_pend_switch].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_gddr6_cadt_exit_mode_when_done].name = "cadt_exit_mode_when_done";
    submodule_data->features[dnx_data_dram_gddr6_cadt_exit_mode_when_done].doc = "";
    submodule_data->features[dnx_data_dram_gddr6_cadt_exit_mode_when_done].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_gddr6_periodic_temp_readout].name = "periodic_temp_readout";
    submodule_data->features[dnx_data_dram_gddr6_periodic_temp_readout].doc = "";
    submodule_data->features[dnx_data_dram_gddr6_periodic_temp_readout].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_gddr6_controller_coherency_handling_mode].name = "controller_coherency_handling_mode";
    submodule_data->features[dnx_data_dram_gddr6_controller_coherency_handling_mode].doc = "";
    submodule_data->features[dnx_data_dram_gddr6_controller_coherency_handling_mode].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_gddr6_enable_temp_read_samsung].name = "enable_temp_read_samsung";
    submodule_data->features[dnx_data_dram_gddr6_enable_temp_read_samsung].doc = "";
    submodule_data->features[dnx_data_dram_gddr6_enable_temp_read_samsung].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_gddr6_enable_temp_read_micron].name = "enable_temp_read_micron";
    submodule_data->features[dnx_data_dram_gddr6_enable_temp_read_micron].doc = "";
    submodule_data->features[dnx_data_dram_gddr6_enable_temp_read_micron].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_dram_gddr6_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dram gddr6 defines");

    submodule_data->defines[dnx_data_dram_gddr6_define_nof_ca_bits].name = "nof_ca_bits";
    submodule_data->defines[dnx_data_dram_gddr6_define_nof_ca_bits].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_nof_ca_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_gddr6_define_bytes_per_channel].name = "bytes_per_channel";
    submodule_data->defines[dnx_data_dram_gddr6_define_bytes_per_channel].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_bytes_per_channel].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_gddr6_define_training_fifo_depth].name = "training_fifo_depth";
    submodule_data->defines[dnx_data_dram_gddr6_define_training_fifo_depth].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_training_fifo_depth].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_gddr6_define_readout_to_readout_prd].name = "readout_to_readout_prd";
    submodule_data->defines[dnx_data_dram_gddr6_define_readout_to_readout_prd].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_readout_to_readout_prd].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_gddr6_define_refresh_to_readout_prd].name = "refresh_to_readout_prd";
    submodule_data->defines[dnx_data_dram_gddr6_define_refresh_to_readout_prd].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_refresh_to_readout_prd].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_gddr6_define_readout_done_to_done_prd].name = "readout_done_to_done_prd";
    submodule_data->defines[dnx_data_dram_gddr6_define_readout_done_to_done_prd].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_readout_done_to_done_prd].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_gddr6_define_refresh_mechanism_enable].name = "refresh_mechanism_enable";
    submodule_data->defines[dnx_data_dram_gddr6_define_refresh_mechanism_enable].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_refresh_mechanism_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_bist_enable].name = "bist_enable";
    submodule_data->defines[dnx_data_dram_gddr6_define_bist_enable].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_bist_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_dynamic_calibration_enable].name = "dynamic_calibration_enable";
    submodule_data->defines[dnx_data_dram_gddr6_define_dynamic_calibration_enable].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_dynamic_calibration_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_cdr_enable].name = "cdr_enable";
    submodule_data->defines[dnx_data_dram_gddr6_define_cdr_enable].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_cdr_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_write_recovery].name = "write_recovery";
    submodule_data->defines[dnx_data_dram_gddr6_define_write_recovery].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_write_recovery].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_cabi].name = "cabi";
    submodule_data->defines[dnx_data_dram_gddr6_define_cabi].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_cabi].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_dram_mode].name = "dram_mode";
    submodule_data->defines[dnx_data_dram_gddr6_define_dram_mode].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_dram_mode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_command_pipe_extra_delay].name = "command_pipe_extra_delay";
    submodule_data->defines[dnx_data_dram_gddr6_define_command_pipe_extra_delay].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_command_pipe_extra_delay].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_use_11bits_ca].name = "use_11bits_ca";
    submodule_data->defines[dnx_data_dram_gddr6_define_use_11bits_ca].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_use_11bits_ca].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_ck_odt].name = "ck_odt";
    submodule_data->defines[dnx_data_dram_gddr6_define_ck_odt].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_ck_odt].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_dynamic_calibration_period].name = "dynamic_calibration_period";
    submodule_data->defines[dnx_data_dram_gddr6_define_dynamic_calibration_period].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_dynamic_calibration_period].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_ck_termination_mr_conf_val].name = "ck_termination_mr_conf_val";
    submodule_data->defines[dnx_data_dram_gddr6_define_ck_termination_mr_conf_val].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_ck_termination_mr_conf_val].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_ck_termination_ca_conf_val].name = "ck_termination_ca_conf_val";
    submodule_data->defines[dnx_data_dram_gddr6_define_ck_termination_ca_conf_val].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_ck_termination_ca_conf_val].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_wck_granularity].name = "wck_granularity";
    submodule_data->defines[dnx_data_dram_gddr6_define_wck_granularity].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_wck_granularity].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_ref_clk_bitmap].name = "ref_clk_bitmap";
    submodule_data->defines[dnx_data_dram_gddr6_define_ref_clk_bitmap].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_ref_clk_bitmap].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_gddr6_define_ca_termination_conf_val].name = "ca_termination_conf_val";
    submodule_data->defines[dnx_data_dram_gddr6_define_ca_termination_conf_val].doc = "";
    
    submodule_data->defines[dnx_data_dram_gddr6_define_ca_termination_conf_val].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_dram_gddr6_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dram gddr6 tables");

    
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].name = "refresh_intervals";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].size_of_values = sizeof(dnx_data_dram_gddr6_refresh_intervals_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].entry_get = dnx_data_dram_gddr6_refresh_intervals_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].nof_keys = 0;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].nof_values, "_dnx_data_dram_gddr6_table_refresh_intervals table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[0].name = "trefiab";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_refresh_intervals_t, trefiab);
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[1].name = "trefiab_acc";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[1].type = "uint32";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[1].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_refresh_intervals_t, trefiab_acc);
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[2].name = "trefisb";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[2].type = "uint32";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[2].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_refresh_intervals].values[2].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_refresh_intervals_t, trefisb);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].name = "dq_map";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].size_of_values = sizeof(dnx_data_dram_gddr6_dq_map_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].entry_get = dnx_data_dram_gddr6_dq_map_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].nof_keys = 2;
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].keys[0].name = "dram_index";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].keys[0].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].keys[1].name = "byte";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].nof_values, "_dnx_data_dram_gddr6_table_dq_map table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].values[0].name = "dq_map";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].values[0].type = "uint8[8]";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_dq_map_t, dq_map);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].name = "dq_channel_swap";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].size_of_values = sizeof(dnx_data_dram_gddr6_dq_channel_swap_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].entry_get = dnx_data_dram_gddr6_dq_channel_swap_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].keys[0].name = "dram_index";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].nof_values, "_dnx_data_dram_gddr6_table_dq_channel_swap table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].values[0].name = "dq_channel_swap";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].values[0].type = "uint8";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_channel_swap].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_dq_channel_swap_t, dq_channel_swap);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].name = "dq_byte_map";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].size_of_values = sizeof(dnx_data_dram_gddr6_dq_byte_map_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].entry_get = dnx_data_dram_gddr6_dq_byte_map_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].nof_keys = 2;
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].keys[0].name = "dram_index";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].keys[0].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].keys[1].name = "byte";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].nof_values, "_dnx_data_dram_gddr6_table_dq_byte_map table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].values[0].name = "dq_byte_map";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].values[0].type = "uint8";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_dq_byte_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_dq_byte_map_t, dq_byte_map);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].name = "ca_map";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].size_of_values = sizeof(dnx_data_dram_gddr6_ca_map_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].entry_get = dnx_data_dram_gddr6_ca_map_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].nof_keys = 2;
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].keys[0].name = "dram_index";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].keys[0].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].keys[1].name = "channel";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].nof_values, "_dnx_data_dram_gddr6_table_ca_map table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].values[0].name = "ca_map";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].values[0].type = "uint8[12]";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_ca_map_t, ca_map);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].name = "cadt_byte_map";
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].size_of_values = sizeof(dnx_data_dram_gddr6_cadt_byte_map_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].entry_get = dnx_data_dram_gddr6_cadt_byte_map_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].nof_keys = 2;
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].keys[0].name = "dram_index";
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].keys[0].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].keys[1].name = "channel";
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].nof_values, "_dnx_data_dram_gddr6_table_cadt_byte_map table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].values[0].name = "cadt_byte_map";
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].values[0].type = "uint8";
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_cadt_byte_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_cadt_byte_map_t, cadt_byte_map);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].name = "channel_regs";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].size_of_values = sizeof(dnx_data_dram_gddr6_channel_regs_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].entry_get = dnx_data_dram_gddr6_channel_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].nof_values, "_dnx_data_dram_gddr6_table_channel_regs table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_channel_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].name = "controller_regs";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].size_of_values = sizeof(dnx_data_dram_gddr6_controller_regs_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].entry_get = dnx_data_dram_gddr6_controller_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].nof_values, "_dnx_data_dram_gddr6_table_controller_regs table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_controller_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].name = "channel_interrupt_regs";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].size_of_values = sizeof(dnx_data_dram_gddr6_channel_interrupt_regs_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].entry_get = dnx_data_dram_gddr6_channel_interrupt_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].nof_values, "_dnx_data_dram_gddr6_table_channel_interrupt_regs table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_interrupt_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_channel_interrupt_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].name = "controller_interrupt_regs";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].size_of_values = sizeof(dnx_data_dram_gddr6_controller_interrupt_regs_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].entry_get = dnx_data_dram_gddr6_controller_interrupt_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].nof_values, "_dnx_data_dram_gddr6_table_controller_interrupt_regs table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_interrupt_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_controller_interrupt_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].name = "channel_debug_regs";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].size_of_values = sizeof(dnx_data_dram_gddr6_channel_debug_regs_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].entry_get = dnx_data_dram_gddr6_channel_debug_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].nof_values, "_dnx_data_dram_gddr6_table_channel_debug_regs table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_channel_debug_regs_t, reg);
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].values[1].name = "reassuring_str";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].values[1].type = "char *";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].values[1].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_debug_regs].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_channel_debug_regs_t, reassuring_str);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].name = "channel_counter_regs";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].size_of_values = sizeof(dnx_data_dram_gddr6_channel_counter_regs_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].entry_get = dnx_data_dram_gddr6_channel_counter_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].nof_values, "_dnx_data_dram_gddr6_table_channel_counter_regs table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_counter_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_channel_counter_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].name = "channel_type_regs";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].size_of_values = sizeof(dnx_data_dram_gddr6_channel_type_regs_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].entry_get = dnx_data_dram_gddr6_channel_type_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].nof_values, "_dnx_data_dram_gddr6_table_channel_type_regs table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_channel_type_regs_t, reg);
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].values[1].name = "type";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].values[1].type = "dnx_hbmc_diag_registers_type_t";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].values[1].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_channel_type_regs].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_channel_type_regs_t, type);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].name = "controller_info_regs";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].size_of_values = sizeof(dnx_data_dram_gddr6_controller_info_regs_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].entry_get = dnx_data_dram_gddr6_controller_info_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].nof_values, "_dnx_data_dram_gddr6_table_controller_info_regs table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_controller_info_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_controller_info_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].name = "master_phy";
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].size_of_values = sizeof(dnx_data_dram_gddr6_master_phy_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].entry_get = dnx_data_dram_gddr6_master_phy_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].keys[0].name = "dram_index";
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].nof_values, "_dnx_data_dram_gddr6_table_master_phy table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].values[0].name = "master_dram_index";
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_master_phy].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_master_phy_t, master_dram_index);

    
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].name = "ca_system_config_fields";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].size_of_values = sizeof(dnx_data_dram_gddr6_ca_system_config_fields_t);
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].entry_get = dnx_data_dram_gddr6_ca_system_config_fields_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].nof_values, "_dnx_data_dram_gddr6_table_ca_system_config_fields table values");
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].values[0].name = "ch_a_start_bit";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_ca_system_config_fields_t, ch_a_start_bit);
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].values[1].name = "ch_b_start_bit";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].values[1].type = "uint32";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].values[1].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_ca_system_config_fields_t, ch_b_start_bit);
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].values[2].name = "field_val";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].values[2].type = "uint32";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].values[2].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].values[2].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_ca_system_config_fields_t, field_val);
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].values[3].name = "nof_bits";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].values[3].type = "uint32";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].values[3].doc = "";
    submodule_data->tables[dnx_data_dram_gddr6_table_ca_system_config_fields].values[3].offset = UTILEX_OFFSETOF(dnx_data_dram_gddr6_ca_system_config_fields_t, nof_bits);


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

uint32
dnx_data_dram_gddr6_ck_termination_mr_conf_val_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_ck_termination_mr_conf_val);
}

uint32
dnx_data_dram_gddr6_ck_termination_ca_conf_val_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_ck_termination_ca_conf_val);
}

uint32
dnx_data_dram_gddr6_wck_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_wck_granularity);
}

uint32
dnx_data_dram_gddr6_ref_clk_bitmap_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_ref_clk_bitmap);
}

uint32
dnx_data_dram_gddr6_ca_termination_conf_val_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_define_ca_termination_conf_val);
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

const dnx_data_dram_gddr6_ca_system_config_fields_t *
dnx_data_dram_gddr6_ca_system_config_fields_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_ca_system_config_fields);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dram_gddr6_ca_system_config_fields_t *) data;

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

shr_error_e
dnx_data_dram_gddr6_ca_system_config_fields_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_gddr6_ca_system_config_fields_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_ca_system_config_fields);
    data = (const dnx_data_dram_gddr6_ca_system_config_fields_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ch_a_start_bit);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ch_b_start_bit);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->field_val);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_bits);
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

const dnxc_data_table_info_t *
dnx_data_dram_gddr6_ca_system_config_fields_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_gddr6, dnx_data_dram_gddr6_table_ca_system_config_fields);

}






static shr_error_e
dnx_data_dram_general_info_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general_info";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_dram_general_info_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dram general_info features");

    submodule_data->features[dnx_data_dram_general_info_is_temperature_reading_supported].name = "is_temperature_reading_supported";
    submodule_data->features[dnx_data_dram_general_info_is_temperature_reading_supported].doc = "";
    submodule_data->features[dnx_data_dram_general_info_is_temperature_reading_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_general_info_is_power_optimization_supported].name = "is_power_optimization_supported";
    submodule_data->features[dnx_data_dram_general_info_is_power_optimization_supported].doc = "";
    submodule_data->features[dnx_data_dram_general_info_is_power_optimization_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_dram_general_info_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dram general_info defines");

    submodule_data->defines[dnx_data_dram_general_info_define_otp_restore_version].name = "otp_restore_version";
    submodule_data->defines[dnx_data_dram_general_info_define_otp_restore_version].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_otp_restore_version].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_general_info_define_max_nof_drams].name = "max_nof_drams";
    submodule_data->defines[dnx_data_dram_general_info_define_max_nof_drams].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_max_nof_drams].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_general_info_define_nof_channels].name = "nof_channels";
    submodule_data->defines[dnx_data_dram_general_info_define_nof_channels].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_nof_channels].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_general_info_define_mr_mask].name = "mr_mask";
    submodule_data->defines[dnx_data_dram_general_info_define_mr_mask].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_mr_mask].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_general_info_define_nof_mrs].name = "nof_mrs";
    submodule_data->defines[dnx_data_dram_general_info_define_nof_mrs].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_nof_mrs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_general_info_define_phy_address_mask].name = "phy_address_mask";
    submodule_data->defines[dnx_data_dram_general_info_define_phy_address_mask].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_phy_address_mask].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_general_info_define_max_dram_index].name = "max_dram_index";
    submodule_data->defines[dnx_data_dram_general_info_define_max_dram_index].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_max_dram_index].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_min_dram_index].name = "min_dram_index";
    submodule_data->defines[dnx_data_dram_general_info_define_min_dram_index].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_min_dram_index].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_frequency].name = "frequency";
    submodule_data->defines[dnx_data_dram_general_info_define_frequency].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_frequency].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_buffer_size].name = "buffer_size";
    submodule_data->defines[dnx_data_dram_general_info_define_buffer_size].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_buffer_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_command_address_parity].name = "command_address_parity";
    submodule_data->defines[dnx_data_dram_general_info_define_command_address_parity].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_command_address_parity].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_dq_write_parity].name = "dq_write_parity";
    submodule_data->defines[dnx_data_dram_general_info_define_dq_write_parity].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_dq_write_parity].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_dq_read_parity].name = "dq_read_parity";
    submodule_data->defines[dnx_data_dram_general_info_define_dq_read_parity].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_dq_read_parity].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_dbi_read].name = "dbi_read";
    submodule_data->defines[dnx_data_dram_general_info_define_dbi_read].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_dbi_read].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_dbi_write].name = "dbi_write";
    submodule_data->defines[dnx_data_dram_general_info_define_dbi_write].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_dbi_write].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_write_latency].name = "write_latency";
    submodule_data->defines[dnx_data_dram_general_info_define_write_latency].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_write_latency].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_write_latency_hbm2e].name = "write_latency_hbm2e";
    submodule_data->defines[dnx_data_dram_general_info_define_write_latency_hbm2e].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_write_latency_hbm2e].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_read_latency].name = "read_latency";
    submodule_data->defines[dnx_data_dram_general_info_define_read_latency].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_read_latency].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_read_latency_hbm2e].name = "read_latency_hbm2e";
    submodule_data->defines[dnx_data_dram_general_info_define_read_latency_hbm2e].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_read_latency_hbm2e].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_read_data_enable_delay].name = "read_data_enable_delay";
    submodule_data->defines[dnx_data_dram_general_info_define_read_data_enable_delay].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_read_data_enable_delay].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_read_data_enable_length].name = "read_data_enable_length";
    submodule_data->defines[dnx_data_dram_general_info_define_read_data_enable_length].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_read_data_enable_length].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_parity_latency].name = "parity_latency";
    submodule_data->defines[dnx_data_dram_general_info_define_parity_latency].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_parity_latency].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_actual_parity_latency].name = "actual_parity_latency";
    submodule_data->defines[dnx_data_dram_general_info_define_actual_parity_latency].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_actual_parity_latency].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_tune_mode_on_init].name = "tune_mode_on_init";
    submodule_data->defines[dnx_data_dram_general_info_define_tune_mode_on_init].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_tune_mode_on_init].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_command_parity_latency].name = "command_parity_latency";
    submodule_data->defines[dnx_data_dram_general_info_define_command_parity_latency].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_command_parity_latency].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_crc_write_latency].name = "crc_write_latency";
    submodule_data->defines[dnx_data_dram_general_info_define_crc_write_latency].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_crc_write_latency].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_crc_read_latency].name = "crc_read_latency";
    submodule_data->defines[dnx_data_dram_general_info_define_crc_read_latency].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_crc_read_latency].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_crc_write].name = "crc_write";
    submodule_data->defines[dnx_data_dram_general_info_define_crc_write].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_crc_write].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_crc_read].name = "crc_read";
    submodule_data->defines[dnx_data_dram_general_info_define_crc_read].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_crc_read].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_device_size].name = "device_size";
    submodule_data->defines[dnx_data_dram_general_info_define_device_size].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_device_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_supported_dram_bitmap].name = "supported_dram_bitmap";
    submodule_data->defines[dnx_data_dram_general_info_define_supported_dram_bitmap].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_supported_dram_bitmap].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_dram_bitmap_internal].name = "dram_bitmap_internal";
    submodule_data->defines[dnx_data_dram_general_info_define_dram_bitmap_internal].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_dram_bitmap_internal].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_tertw].name = "tertw";
    submodule_data->defines[dnx_data_dram_general_info_define_tertw].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_tertw].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_general_info_define_dram_blocks_in_power_down].name = "dram_blocks_in_power_down";
    submodule_data->defines[dnx_data_dram_general_info_define_dram_blocks_in_power_down].doc = "";
    
    submodule_data->defines[dnx_data_dram_general_info_define_dram_blocks_in_power_down].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_dram_general_info_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dram general_info tables");

    
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].name = "mr_defaults";
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].size_of_values = sizeof(dnx_data_dram_general_info_mr_defaults_t);
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].entry_get = dnx_data_dram_general_info_mr_defaults_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].nof_values, "_dnx_data_dram_general_info_table_mr_defaults table values");
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].values[0].name = "value";
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_mr_defaults].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_mr_defaults_t, value);

    
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].name = "dram_info";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].size_of_values = sizeof(dnx_data_dram_general_info_dram_info_t);
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].entry_get = dnx_data_dram_general_info_dram_info_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].nof_keys = 0;

    
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].nof_values = 8;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_general_info_table_dram_info].nof_values, "_dnx_data_dram_general_info_table_dram_info table values");
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[0].name = "dram_type";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_dram_info_t, dram_type);
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[1].name = "dram_bitmap";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[1].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_dram_info_t, dram_bitmap);
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[2].name = "nof_columns";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[2].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_dram_info_t, nof_columns);
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[3].name = "nof_rows";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[3].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[3].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_dram_info_t, nof_rows);
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[4].name = "nof_banks";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[4].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[4].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[4].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_dram_info_t, nof_banks);
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[5].name = "data_rate";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[5].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[5].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[5].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_dram_info_t, data_rate);
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[6].name = "ref_clock";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[6].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[6].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[6].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_dram_info_t, ref_clock);
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[7].name = "ctl_type";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[7].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[7].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_dram_info].values[7].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_dram_info_t, ctl_type);

    
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].name = "timing_params";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].size_of_values = sizeof(dnx_data_dram_general_info_timing_params_t);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].entry_get = dnx_data_dram_general_info_timing_params_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].nof_keys = 0;

    
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].nof_values = 32;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_general_info_table_timing_params].nof_values, "_dnx_data_dram_general_info_table_timing_params table values");
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[0].name = "twr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, twr);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[1].name = "trp";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[1].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[1].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trp);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[2].name = "trtps";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[2].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[2].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[2].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trtps);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[3].name = "trtpl";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[3].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[3].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[3].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trtpl);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[4].name = "trrds";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[4].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[4].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[4].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trrds);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[5].name = "trrdl";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[5].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[5].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[5].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trrdl);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[6].name = "tfaw";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[6].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[6].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[6].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, tfaw);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[7].name = "trcdwr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[7].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[7].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[7].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trcdwr);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[8].name = "trcdrd";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[8].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[8].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[8].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trcdrd);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[9].name = "tras";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[9].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[9].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[9].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, tras);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[10].name = "tras_hbm2e";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[10].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[10].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[10].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, tras_hbm2e);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[11].name = "trc";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[11].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[11].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[11].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trc);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[12].name = "twtrl";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[12].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[12].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[12].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, twtrl);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[13].name = "twtrs";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[13].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[13].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[13].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, twtrs);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[14].name = "trtw";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[14].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[14].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[14].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trtw);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[15].name = "tccdr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[15].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[15].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[15].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, tccdr);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[16].name = "tccds";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[16].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[16].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[16].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, tccds);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[17].name = "tccdl";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[17].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[17].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[17].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, tccdl);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[18].name = "trrefd";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[18].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[18].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[18].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trrefd);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[19].name = "trfcsb";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[19].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[19].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[19].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trfcsb);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[20].name = "trfc";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[20].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[20].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[20].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trfc);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[21].name = "tmrd";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[21].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[21].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[21].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, tmrd);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[22].name = "tmod";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[22].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[22].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[22].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, tmod);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[23].name = "twtrtr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[23].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[23].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[23].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, twtrtr);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[24].name = "twrwtr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[24].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[24].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[24].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, twrwtr);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[25].name = "treftr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[25].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[25].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[25].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, treftr);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[26].name = "trdtlt";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[26].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[26].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[26].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trdtlt);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[27].name = "trcdwtr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[27].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[27].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[27].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trcdwtr);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[28].name = "trcdrtr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[28].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[28].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[28].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trcdrtr);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[29].name = "trcdltr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[29].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[29].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[29].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, trcdltr);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[30].name = "tltrtr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[30].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[30].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[30].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, tltrtr);
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[31].name = "tltltr";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[31].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[31].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_timing_params].values[31].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_timing_params_t, tltltr);

    
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].name = "refresh_intervals";
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].size_of_values = sizeof(dnx_data_dram_general_info_refresh_intervals_t);
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].entry_get = dnx_data_dram_general_info_refresh_intervals_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].keys[0].name = "temp_index";
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].nof_values, "_dnx_data_dram_general_info_table_refresh_intervals table values");
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].values[0].name = "trefi";
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_general_info_refresh_intervals_t, trefi);
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].values[1].name = "trefisb";
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].values[1].type = "uint32";
    submodule_data->tables[dnx_data_dram_general_info_table_refresh_intervals].values[1].doc = "";
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
dnx_data_dram_general_info_write_latency_hbm2e_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_write_latency_hbm2e);
}

uint32
dnx_data_dram_general_info_read_latency_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_read_latency);
}

uint32
dnx_data_dram_general_info_read_latency_hbm2e_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_read_latency_hbm2e);
}

uint32
dnx_data_dram_general_info_read_data_enable_delay_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_read_data_enable_delay);
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

uint32
dnx_data_dram_general_info_supported_dram_bitmap_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_supported_dram_bitmap);
}

uint32
dnx_data_dram_general_info_dram_bitmap_internal_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_dram_bitmap_internal);
}

uint32
dnx_data_dram_general_info_tertw_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_tertw);
}

uint32
dnx_data_dram_general_info_dram_blocks_in_power_down_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_general_info, dnx_data_dram_general_info_define_dram_blocks_in_power_down);
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
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tras_hbm2e);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trc);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->twtrl);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->twtrs);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trtw);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tccdr);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tccds);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tccdl);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trrefd);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trfcsb);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trfc);
            break;
        case 21:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tmrd);
            break;
        case 22:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tmod);
            break;
        case 23:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->twtrtr);
            break;
        case 24:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->twrwtr);
            break;
        case 25:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->treftr);
            break;
        case 26:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trdtlt);
            break;
        case 27:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trcdwtr);
            break;
        case 28:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trcdrtr);
            break;
        case 29:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->trcdltr);
            break;
        case 30:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tltrtr);
            break;
        case 31:
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
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_dram_address_translation_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dram address_translation features");

    
    submodule_data->nof_defines = _dnx_data_dram_address_translation_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dram address_translation defines");

    submodule_data->defines[dnx_data_dram_address_translation_define_matrix_column_size].name = "matrix_column_size";
    submodule_data->defines[dnx_data_dram_address_translation_define_matrix_column_size].doc = "";
    
    submodule_data->defines[dnx_data_dram_address_translation_define_matrix_column_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_address_translation_define_physical_address_transaction_size].name = "physical_address_transaction_size";
    submodule_data->defines[dnx_data_dram_address_translation_define_physical_address_transaction_size].doc = "";
    
    submodule_data->defines[dnx_data_dram_address_translation_define_physical_address_transaction_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_address_translation_define_nof_atms].name = "nof_atms";
    submodule_data->defines[dnx_data_dram_address_translation_define_nof_atms].doc = "";
    
    submodule_data->defines[dnx_data_dram_address_translation_define_nof_atms].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_address_translation_define_nof_tdus_per_dram].name = "nof_tdus_per_dram";
    submodule_data->defines[dnx_data_dram_address_translation_define_nof_tdus_per_dram].doc = "";
    
    submodule_data->defines[dnx_data_dram_address_translation_define_nof_tdus_per_dram].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_address_translation_define_max_tdu_index].name = "max_tdu_index";
    submodule_data->defines[dnx_data_dram_address_translation_define_max_tdu_index].doc = "";
    
    submodule_data->defines[dnx_data_dram_address_translation_define_max_tdu_index].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_dram_address_translation_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dram address_translation tables");

    
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].name = "tdu_map";
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].doc = "";
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].size_of_values = sizeof(dnx_data_dram_address_translation_tdu_map_t);
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].entry_get = dnx_data_dram_address_translation_tdu_map_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].nof_keys = 2;
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].keys[0].name = "dram_index";
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].keys[0].doc = "";
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].keys[1].name = "tdu_index";
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].nof_values, "_dnx_data_dram_address_translation_table_tdu_map table values");
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].values[0].name = "data_source_id";
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_address_translation_table_tdu_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_address_translation_tdu_map_t, data_source_id);

    
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].name = "matrix_configuration";
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].doc = "";
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].size_of_values = sizeof(dnx_data_dram_address_translation_matrix_configuration_t);
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].entry_get = dnx_data_dram_address_translation_matrix_configuration_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].keys[0].name = "matrix_option";
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].nof_values, "_dnx_data_dram_address_translation_table_matrix_configuration table values");
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].values[0].name = "logical_to_physical";
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].values[0].type = "uint32[DNX_DATA_MAX_DRAM_ADDRESS_TRANSLATION_MATRIX_COLUMN_SIZE]";
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_address_translation_table_matrix_configuration].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_address_translation_matrix_configuration_t, logical_to_physical);

    
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].name = "interrupt_regs";
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].doc = "";
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].size_of_values = sizeof(dnx_data_dram_address_translation_interrupt_regs_t);
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].entry_get = dnx_data_dram_address_translation_interrupt_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].nof_values, "_dnx_data_dram_address_translation_table_interrupt_regs table values");
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_address_translation_table_interrupt_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_address_translation_interrupt_regs_t, reg);

    
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].name = "counter_regs";
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].doc = "";
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].size_of_values = sizeof(dnx_data_dram_address_translation_counter_regs_t);
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].entry_get = dnx_data_dram_address_translation_counter_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].nof_values, "_dnx_data_dram_address_translation_table_counter_regs table values");
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dram_address_translation_table_counter_regs].values[0].doc = "";
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
dnx_data_dram_address_translation_max_tdu_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_address_translation, dnx_data_dram_address_translation_define_max_tdu_index);
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
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_dram_buffers_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dram buffers features");

    submodule_data->features[dnx_data_dram_buffers_quarantine_buffers_if_tdu_corrected_errors].name = "quarantine_buffers_if_tdu_corrected_errors";
    submodule_data->features[dnx_data_dram_buffers_quarantine_buffers_if_tdu_corrected_errors].doc = "";
    submodule_data->features[dnx_data_dram_buffers_quarantine_buffers_if_tdu_corrected_errors].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_buffers_quarantine_buffers_if_tdu_errors].name = "quarantine_buffers_if_tdu_errors";
    submodule_data->features[dnx_data_dram_buffers_quarantine_buffers_if_tdu_errors].doc = "";
    submodule_data->features[dnx_data_dram_buffers_quarantine_buffers_if_tdu_errors].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_buffers_bdb_release_mechanism_usage_counters].name = "bdb_release_mechanism_usage_counters";
    submodule_data->features[dnx_data_dram_buffers_bdb_release_mechanism_usage_counters].doc = "";
    submodule_data->features[dnx_data_dram_buffers_bdb_release_mechanism_usage_counters].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_dram_buffers_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dram buffers defines");

    submodule_data->defines[dnx_data_dram_buffers_define_allowed_errors].name = "allowed_errors";
    submodule_data->defines[dnx_data_dram_buffers_define_allowed_errors].doc = "";
    
    submodule_data->defines[dnx_data_dram_buffers_define_allowed_errors].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_buffers_define_nof_bdbs].name = "nof_bdbs";
    submodule_data->defines[dnx_data_dram_buffers_define_nof_bdbs].doc = "";
    
    submodule_data->defines[dnx_data_dram_buffers_define_nof_bdbs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_buffers_define_nof_fpc_banks].name = "nof_fpc_banks";
    submodule_data->defines[dnx_data_dram_buffers_define_nof_fpc_banks].doc = "";
    
    submodule_data->defines[dnx_data_dram_buffers_define_nof_fpc_banks].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_dram_buffers_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dram buffers tables");

    
    submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].name = "deleted_buffers_info";
    submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].doc = "";
    submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].size_of_values = sizeof(dnx_data_dram_buffers_deleted_buffers_info_t);
    submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].entry_get = dnx_data_dram_buffers_deleted_buffers_info_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].nof_keys = 0;

    
    submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].nof_values, "_dnx_data_dram_buffers_table_deleted_buffers_info table values");
    submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].values[0].name = "deleted_buffers_file";
    submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].values[0].type = "char *";
    submodule_data->tables[dnx_data_dram_buffers_table_deleted_buffers_info].values[0].doc = "";
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
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_dram_dram_block_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dram dram_block features");

    submodule_data->features[dnx_data_dram_dram_block_write_minus_read_leaky_bucket].name = "write_minus_read_leaky_bucket";
    submodule_data->features[dnx_data_dram_dram_block_write_minus_read_leaky_bucket].doc = "";
    submodule_data->features[dnx_data_dram_dram_block_write_minus_read_leaky_bucket].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_dram_block_write_plus_read_leaky_bucket].name = "write_plus_read_leaky_bucket";
    submodule_data->features[dnx_data_dram_dram_block_write_plus_read_leaky_bucket].doc = "";
    submodule_data->features[dnx_data_dram_dram_block_write_plus_read_leaky_bucket].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_dram_block_write_leaky_bucket].name = "write_leaky_bucket";
    submodule_data->features[dnx_data_dram_dram_block_write_leaky_bucket].doc = "";
    submodule_data->features[dnx_data_dram_dram_block_write_leaky_bucket].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_dram_block_average_read_inflights_leaky_bucket].name = "average_read_inflights_leaky_bucket";
    submodule_data->features[dnx_data_dram_dram_block_average_read_inflights_leaky_bucket].doc = "";
    submodule_data->features[dnx_data_dram_dram_block_average_read_inflights_leaky_bucket].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_dram_block_write_minus_read_leaky_bucket_disable].name = "write_minus_read_leaky_bucket_disable";
    submodule_data->features[dnx_data_dram_dram_block_write_minus_read_leaky_bucket_disable].doc = "";
    submodule_data->features[dnx_data_dram_dram_block_write_minus_read_leaky_bucket_disable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_dram_block_write_plus_read_leaky_bucket_disable].name = "write_plus_read_leaky_bucket_disable";
    submodule_data->features[dnx_data_dram_dram_block_write_plus_read_leaky_bucket_disable].doc = "";
    submodule_data->features[dnx_data_dram_dram_block_write_plus_read_leaky_bucket_disable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dram_dram_block_average_read_inflights_log2_window_size].name = "average_read_inflights_log2_window_size";
    submodule_data->features[dnx_data_dram_dram_block_average_read_inflights_log2_window_size].doc = "";
    submodule_data->features[dnx_data_dram_dram_block_average_read_inflights_log2_window_size].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_dram_dram_block_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dram dram_block defines");

    submodule_data->defines[dnx_data_dram_dram_block_define_leaky_bucket_window_size].name = "leaky_bucket_window_size";
    submodule_data->defines[dnx_data_dram_dram_block_define_leaky_bucket_window_size].doc = "";
    
    submodule_data->defines[dnx_data_dram_dram_block_define_leaky_bucket_window_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dram_block_define_wmr_decrement_thr_factor].name = "wmr_decrement_thr_factor";
    submodule_data->defines[dnx_data_dram_dram_block_define_wmr_decrement_thr_factor].doc = "";
    
    submodule_data->defines[dnx_data_dram_dram_block_define_wmr_decrement_thr_factor].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_dram_block_define_wpr_increment_thr_factor].name = "wpr_increment_thr_factor";
    submodule_data->defines[dnx_data_dram_dram_block_define_wpr_increment_thr_factor].doc = "";
    
    submodule_data->defines[dnx_data_dram_dram_block_define_wpr_increment_thr_factor].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_dram_dram_block_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dram dram_block tables");

    
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].name = "dram_block_params";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].size_of_values = sizeof(dnx_data_dram_dram_block_dram_block_params_t);
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].entry_get = dnx_data_dram_dram_block_dram_block_params_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].keys[0].name = "mechanism";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].nof_values = 21;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].nof_values, "_dnx_data_dram_dram_block_table_dram_block_params table values");
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[0].name = "is_valid";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_dram_block_params_t, is_valid);
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[1].name = "is_used";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[1].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[1].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_dram_block_params_t, is_used);
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[2].name = "is_disable_supported";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[2].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[2].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[2].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_dram_block_params_t, is_disable_supported);
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[3].name = "dbal_table";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[3].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[3].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[3].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_dram_block_params_t, dbal_table);
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[4].name = "increment_threshold_0";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[4].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[4].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[4].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_dram_block_params_t, increment_threshold_0);
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[5].name = "increment_threshold_1";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[5].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[5].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[5].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_dram_block_params_t, increment_threshold_1);
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[6].name = "increment_threshold_2";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[6].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[6].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[6].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_dram_block_params_t, increment_threshold_2);
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[7].name = "decrement_threshold_0";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[7].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[7].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[7].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_dram_block_params_t, decrement_threshold_0);
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[8].name = "decrement_threshold_1";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[8].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[8].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[8].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_dram_block_params_t, decrement_threshold_1);
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[9].name = "decrement_threshold_2";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[9].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[9].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[9].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_dram_block_params_t, decrement_threshold_2);
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[10].name = "increment_size_0";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[10].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[10].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[10].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_dram_block_params_t, increment_size_0);
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[11].name = "increment_size_1";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[11].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[11].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[11].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_dram_block_params_t, increment_size_1);
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[12].name = "increment_size_2";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[12].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[12].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[12].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_dram_block_params_t, increment_size_2);
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[13].name = "decrement_size_0";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[13].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[13].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[13].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_dram_block_params_t, decrement_size_0);
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[14].name = "decrement_size_1";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[14].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[14].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[14].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_dram_block_params_t, decrement_size_1);
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[15].name = "decrement_size_2";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[15].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[15].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[15].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_dram_block_params_t, decrement_size_2);
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[16].name = "bucket_full_size";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[16].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[16].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[16].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_dram_block_params_t, bucket_full_size);
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[17].name = "bucket_assert_threshold";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[17].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[17].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[17].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_dram_block_params_t, bucket_assert_threshold);
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[18].name = "bucket_deassert_threshold";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[18].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[18].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[18].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_dram_block_params_t, bucket_deassert_threshold);
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[19].name = "log2_window_size";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[19].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[19].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[19].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_dram_block_params_t, log2_window_size);
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[20].name = "reset_on_deassert";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[20].type = "uint32";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[20].doc = "";
    submodule_data->tables[dnx_data_dram_dram_block_table_dram_block_params].values[20].offset = UTILEX_OFFSETOF(dnx_data_dram_dram_block_dram_block_params_t, reset_on_deassert);


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



const dnx_data_dram_dram_block_dram_block_params_t *
dnx_data_dram_dram_block_dram_block_params_get(
    int unit,
    int mechanism)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dram_block, dnx_data_dram_dram_block_table_dram_block_params);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mechanism, 0);
    return (const dnx_data_dram_dram_block_dram_block_params_t *) data;

}


shr_error_e
dnx_data_dram_dram_block_dram_block_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_dram_block_dram_block_params_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dram_block, dnx_data_dram_dram_block_table_dram_block_params);
    data = (const dnx_data_dram_dram_block_dram_block_params_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_used);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_disable_supported);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->increment_threshold_0);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->increment_threshold_1);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->increment_threshold_2);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->decrement_threshold_0);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->decrement_threshold_1);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->decrement_threshold_2);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->increment_size_0);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->increment_size_1);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->increment_size_2);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->decrement_size_0);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->decrement_size_1);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->decrement_size_2);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->bucket_full_size);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->bucket_assert_threshold);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->bucket_deassert_threshold);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->log2_window_size);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reset_on_deassert);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_dram_dram_block_dram_block_params_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dram_block, dnx_data_dram_dram_block_table_dram_block_params);

}






static shr_error_e
dnx_data_dram_dbal_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dbal";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_dram_dbal_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dram dbal features");

    
    submodule_data->nof_defines = _dnx_data_dram_dbal_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dram dbal defines");

    submodule_data->defines[dnx_data_dram_dbal_define_hbm_trc_nof_bits].name = "hbm_trc_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_trc_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_trc_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_wpr_increment_threshold_nof_bits].name = "wpr_increment_threshold_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_wpr_increment_threshold_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_dram_dbal_define_wpr_increment_threshold_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_average_read_inflights_increment_threshold_nof_bits].name = "average_read_inflights_increment_threshold_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_average_read_inflights_increment_threshold_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_dram_dbal_define_average_read_inflights_increment_threshold_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_average_read_inflights_decrement_threshold_nof_bits].name = "average_read_inflights_decrement_threshold_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_average_read_inflights_decrement_threshold_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_dram_dbal_define_average_read_inflights_decrement_threshold_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_pdiv_nof_bits].name = "hbm_pll_pdiv_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_pdiv_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_pdiv_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_ch_mdiv_nof_bits].name = "hbm_pll_ch_mdiv_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_ch_mdiv_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_ch_mdiv_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_frefeff_info_nof_bits].name = "hbm_pll_frefeff_info_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_frefeff_info_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_frefeff_info_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_aux_post_enableb_nof_bits].name = "hbm_pll_aux_post_enableb_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_aux_post_enableb_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_aux_post_enableb_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_ch_enableb_nof_bits].name = "hbm_pll_ch_enableb_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_ch_enableb_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_ch_enableb_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_aux_post_diffcmos_en_nof_bits].name = "hbm_pll_aux_post_diffcmos_en_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_aux_post_diffcmos_en_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_pll_aux_post_diffcmos_en_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_dram_bist_mode_nof_bits].name = "dram_bist_mode_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_dram_bist_mode_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_dram_dbal_define_dram_bist_mode_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_hbm_write_latency_nof_bits].name = "hbm_write_latency_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_write_latency_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_write_latency_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_hbm_output_enable_delay_nof_bits].name = "hbm_output_enable_delay_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_output_enable_delay_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_output_enable_delay_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_hbm_rd_data_en_delay_nof_bits].name = "hbm_rd_data_en_delay_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_rd_data_en_delay_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_rd_data_en_delay_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_hbm_c_u_nof_bits].name = "hbm_c_u_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_c_u_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_c_u_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_hbm_r_u_nof_bits].name = "hbm_r_u_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_r_u_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbm_r_u_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_dbal_define_hbmc_index_nof_bits].name = "hbmc_index_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbmc_index_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_dram_dbal_define_hbmc_index_nof_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dram_dbal_define_hbmc_tdu_index_nof_bits].name = "hbmc_tdu_index_nof_bits";
    submodule_data->defines[dnx_data_dram_dbal_define_hbmc_tdu_index_nof_bits].doc = "";
    
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
dnx_data_dram_dbal_hbm_write_latency_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dbal, dnx_data_dram_dbal_define_hbm_write_latency_nof_bits);
}

uint32
dnx_data_dram_dbal_hbm_output_enable_delay_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dbal, dnx_data_dram_dbal_define_hbm_output_enable_delay_nof_bits);
}

uint32
dnx_data_dram_dbal_hbm_rd_data_en_delay_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dbal, dnx_data_dram_dbal_define_hbm_rd_data_en_delay_nof_bits);
}

uint32
dnx_data_dram_dbal_hbm_c_u_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dbal, dnx_data_dram_dbal_define_hbm_c_u_nof_bits);
}

uint32
dnx_data_dram_dbal_hbm_r_u_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_dbal, dnx_data_dram_dbal_define_hbm_r_u_nof_bits);
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
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_dram_firmware_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dram firmware features");

    
    submodule_data->nof_defines = _dnx_data_dram_firmware_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dram firmware defines");

    submodule_data->defines[dnx_data_dram_firmware_define_nof_hbm_spare_data_results].name = "nof_hbm_spare_data_results";
    submodule_data->defines[dnx_data_dram_firmware_define_nof_hbm_spare_data_results].doc = "";
    
    submodule_data->defines[dnx_data_dram_firmware_define_nof_hbm_spare_data_results].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_firmware_define_sbus_clock_divider].name = "sbus_clock_divider";
    submodule_data->defines[dnx_data_dram_firmware_define_sbus_clock_divider].doc = "";
    
    submodule_data->defines[dnx_data_dram_firmware_define_sbus_clock_divider].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_firmware_define_snap_state_init_done].name = "snap_state_init_done";
    submodule_data->defines[dnx_data_dram_firmware_define_snap_state_init_done].doc = "";
    
    submodule_data->defines[dnx_data_dram_firmware_define_snap_state_init_done].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dram_firmware_define_max_timeout].name = "max_timeout";
    submodule_data->defines[dnx_data_dram_firmware_define_max_timeout].doc = "";
    
    submodule_data->defines[dnx_data_dram_firmware_define_max_timeout].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_dram_firmware_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dram firmware tables");

    
    submodule_data->tables[dnx_data_dram_firmware_table_rom].name = "rom";
    submodule_data->tables[dnx_data_dram_firmware_table_rom].doc = "";
    submodule_data->tables[dnx_data_dram_firmware_table_rom].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_firmware_table_rom].size_of_values = sizeof(dnx_data_dram_firmware_rom_t);
    submodule_data->tables[dnx_data_dram_firmware_table_rom].entry_get = dnx_data_dram_firmware_rom_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_firmware_table_rom].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_firmware_table_rom].keys[0].name = "hbm_generation";
    submodule_data->tables[dnx_data_dram_firmware_table_rom].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_firmware_table_rom].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_firmware_table_rom].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_firmware_table_rom].nof_values, "_dnx_data_dram_firmware_table_rom table values");
    submodule_data->tables[dnx_data_dram_firmware_table_rom].values[0].name = "filename";
    submodule_data->tables[dnx_data_dram_firmware_table_rom].values[0].type = "char *";
    submodule_data->tables[dnx_data_dram_firmware_table_rom].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_firmware_table_rom].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_firmware_rom_t, filename);

    
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].name = "operation_status";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].doc = "";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].size_of_values = sizeof(dnx_data_dram_firmware_operation_status_t);
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].entry_get = dnx_data_dram_firmware_operation_status_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].nof_keys = 0;

    
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_firmware_table_operation_status].nof_values, "_dnx_data_dram_firmware_table_operation_status table values");
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[0].name = "done";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_firmware_operation_status_t, done);
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[1].name = "active";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[1].type = "uint32";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[1].doc = "";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_firmware_operation_status_t, active);
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[2].name = "errors_detected";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[2].type = "uint32";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[2].doc = "";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_status].values[2].offset = UTILEX_OFFSETOF(dnx_data_dram_firmware_operation_status_t, errors_detected);

    
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].name = "operation_result_error_code";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].doc = "";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].size_of_values = sizeof(dnx_data_dram_firmware_operation_result_error_code_t);
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].entry_get = dnx_data_dram_firmware_operation_result_error_code_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].keys[0].name = "error_code";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].nof_values, "_dnx_data_dram_firmware_table_operation_result_error_code table values");
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].values[0].name = "description";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].values[0].type = "char *";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_error_code].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_firmware_operation_result_error_code_t, description);

    
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].name = "operation_result_last_operation";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].doc = "";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].size_of_values = sizeof(dnx_data_dram_firmware_operation_result_last_operation_t);
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].entry_get = dnx_data_dram_firmware_operation_result_last_operation_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].nof_keys = 1;
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].keys[0].name = "operation_code";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].nof_values, "_dnx_data_dram_firmware_table_operation_result_last_operation table values");
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].values[0].name = "description";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].values[0].type = "char *";
    submodule_data->tables[dnx_data_dram_firmware_table_operation_result_last_operation].values[0].doc = "";
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

uint32
dnx_data_dram_firmware_max_timeout_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_firmware, dnx_data_dram_firmware_define_max_timeout);
}



const dnx_data_dram_firmware_rom_t *
dnx_data_dram_firmware_rom_get(
    int unit,
    int hbm_generation)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_firmware, dnx_data_dram_firmware_table_rom);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, hbm_generation, 0);
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
    data = (const dnx_data_dram_firmware_rom_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
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
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_dram_apd_phy_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dram apd_phy features");

    
    submodule_data->nof_defines = _dnx_data_dram_apd_phy_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dram apd_phy defines");

    
    submodule_data->nof_tables = _dnx_data_dram_apd_phy_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dram apd_phy tables");

    
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].name = "pll";
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].doc = "";
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].size_of_values = sizeof(dnx_data_dram_apd_phy_pll_t);
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].entry_get = dnx_data_dram_apd_phy_pll_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].nof_keys = 0;

    
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_apd_phy_table_pll].nof_values, "_dnx_data_dram_apd_phy_table_pll table values");
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values[0].name = "ndiv_int";
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_apd_phy_pll_t, ndiv_int);
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values[1].name = "ch1_mdiv";
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values[1].type = "uint32";
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values[1].doc = "";
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_apd_phy_pll_t, ch1_mdiv);
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values[2].name = "ch_enable_bitmap";
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values[2].type = "uint32";
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values[2].doc = "";
    submodule_data->tables[dnx_data_dram_apd_phy_table_pll].values[2].offset = UTILEX_OFFSETOF(dnx_data_dram_apd_phy_pll_t, ch_enable_bitmap);


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
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ch_enable_bitmap);
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






static shr_error_e
dnx_data_dram_ctests_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ctests";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_dram_ctests_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dram ctests features");

    
    submodule_data->nof_defines = _dnx_data_dram_ctests_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dram ctests defines");

    
    submodule_data->nof_tables = _dnx_data_dram_ctests_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dram ctests tables");

    
    submodule_data->tables[dnx_data_dram_ctests_table_rate].name = "rate";
    submodule_data->tables[dnx_data_dram_ctests_table_rate].doc = "";
    submodule_data->tables[dnx_data_dram_ctests_table_rate].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dram_ctests_table_rate].size_of_values = sizeof(dnx_data_dram_ctests_rate_t);
    submodule_data->tables[dnx_data_dram_ctests_table_rate].entry_get = dnx_data_dram_ctests_rate_entry_str_get;

    
    submodule_data->tables[dnx_data_dram_ctests_table_rate].nof_keys = 0;

    
    submodule_data->tables[dnx_data_dram_ctests_table_rate].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dram_ctests_table_rate].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dram_ctests_table_rate].nof_values, "_dnx_data_dram_ctests_table_rate table values");
    submodule_data->tables[dnx_data_dram_ctests_table_rate].values[0].name = "snake_ports_type";
    submodule_data->tables[dnx_data_dram_ctests_table_rate].values[0].type = "char *";
    submodule_data->tables[dnx_data_dram_ctests_table_rate].values[0].doc = "";
    submodule_data->tables[dnx_data_dram_ctests_table_rate].values[0].offset = UTILEX_OFFSETOF(dnx_data_dram_ctests_rate_t, snake_ports_type);
    submodule_data->tables[dnx_data_dram_ctests_table_rate].values[1].name = "nof_lanes_in_snake";
    submodule_data->tables[dnx_data_dram_ctests_table_rate].values[1].type = "uint32";
    submodule_data->tables[dnx_data_dram_ctests_table_rate].values[1].doc = "";
    submodule_data->tables[dnx_data_dram_ctests_table_rate].values[1].offset = UTILEX_OFFSETOF(dnx_data_dram_ctests_rate_t, nof_lanes_in_snake);
    submodule_data->tables[dnx_data_dram_ctests_table_rate].values[2].name = "expected_rate_in_mbps";
    submodule_data->tables[dnx_data_dram_ctests_table_rate].values[2].type = "uint32";
    submodule_data->tables[dnx_data_dram_ctests_table_rate].values[2].doc = "";
    submodule_data->tables[dnx_data_dram_ctests_table_rate].values[2].offset = UTILEX_OFFSETOF(dnx_data_dram_ctests_rate_t, expected_rate_in_mbps);
    submodule_data->tables[dnx_data_dram_ctests_table_rate].values[3].name = "expected_dram_rate_in_mbps";
    submodule_data->tables[dnx_data_dram_ctests_table_rate].values[3].type = "uint32";
    submodule_data->tables[dnx_data_dram_ctests_table_rate].values[3].doc = "";
    submodule_data->tables[dnx_data_dram_ctests_table_rate].values[3].offset = UTILEX_OFFSETOF(dnx_data_dram_ctests_rate_t, expected_dram_rate_in_mbps);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dram_ctests_feature_get(
    int unit,
    dnx_data_dram_ctests_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_ctests, feature);
}




const dnx_data_dram_ctests_rate_t *
dnx_data_dram_ctests_rate_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_ctests, dnx_data_dram_ctests_table_rate);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_dram_ctests_rate_t *) data;

}


shr_error_e
dnx_data_dram_ctests_rate_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dram_ctests_rate_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_ctests, dnx_data_dram_ctests_table_rate);
    data = (const dnx_data_dram_ctests_rate_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->snake_ports_type == NULL ? "" : data->snake_ports_type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_lanes_in_snake);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->expected_rate_in_mbps);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->expected_dram_rate_in_mbps);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_dram_ctests_rate_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dram, dnx_data_dram_submodule_ctests, dnx_data_dram_ctests_table_rate);

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
    SHR_IF_ERR_EXIT(dnx_data_dram_ctests_init(unit, &module_data->submodules[dnx_data_dram_submodule_ctests]));
    

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


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dram_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_dram_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a1_data_dram_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dram_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_dram_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a1_data_dram_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dram_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_dram_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

