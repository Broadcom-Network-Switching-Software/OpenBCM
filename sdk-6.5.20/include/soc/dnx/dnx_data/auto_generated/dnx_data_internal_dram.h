

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_DRAM_H_

#define _DNX_DATA_INTERNAL_DRAM_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_dram_submodule_hbm,
    dnx_data_dram_submodule_gddr6,
    dnx_data_dram_submodule_general_info,
    dnx_data_dram_submodule_address_translation,
    dnx_data_dram_submodule_buffers,
    dnx_data_dram_submodule_dram_block,
    dnx_data_dram_submodule_dbal,
    dnx_data_dram_submodule_firmware,
    dnx_data_dram_submodule_apd_phy,

    
    _dnx_data_dram_submodule_nof
} dnx_data_dram_submodule_e;








int dnx_data_dram_hbm_feature_get(
    int unit,
    dnx_data_dram_hbm_feature_e feature);



typedef enum
{
    dnx_data_dram_hbm_define_burst_length,
    dnx_data_dram_hbm_define_stop_traffic_temp_threshold,
    dnx_data_dram_hbm_define_stop_traffic_low_temp_threshold,
    dnx_data_dram_hbm_define_restore_traffic_temp_threshold,
    dnx_data_dram_hbm_define_restore_traffic_low_temp_threshold,
    dnx_data_dram_hbm_define_usec_between_temp_samples,
    dnx_data_dram_hbm_define_power_down_temp_threshold,
    dnx_data_dram_hbm_define_power_down_low_temp_threshold,
    dnx_data_dram_hbm_define_dram_temp_monitor_enable,
    dnx_data_dram_hbm_define_start_disabled,
    dnx_data_dram_hbm_define_output_enable_length,
    dnx_data_dram_hbm_define_output_enable_delay,
    dnx_data_dram_hbm_define_default_model_part_num,
    dnx_data_dram_hbm_define_nof_tdu_per_core,
    dnx_data_dram_hbm_define_wds_size,

    
    _dnx_data_dram_hbm_define_nof
} dnx_data_dram_hbm_define_e;



uint32 dnx_data_dram_hbm_burst_length_get(
    int unit);


uint32 dnx_data_dram_hbm_stop_traffic_temp_threshold_get(
    int unit);


uint32 dnx_data_dram_hbm_stop_traffic_low_temp_threshold_get(
    int unit);


uint32 dnx_data_dram_hbm_restore_traffic_temp_threshold_get(
    int unit);


uint32 dnx_data_dram_hbm_restore_traffic_low_temp_threshold_get(
    int unit);


uint32 dnx_data_dram_hbm_usec_between_temp_samples_get(
    int unit);


uint32 dnx_data_dram_hbm_power_down_temp_threshold_get(
    int unit);


uint32 dnx_data_dram_hbm_power_down_low_temp_threshold_get(
    int unit);


uint32 dnx_data_dram_hbm_dram_temp_monitor_enable_get(
    int unit);


uint32 dnx_data_dram_hbm_start_disabled_get(
    int unit);


uint32 dnx_data_dram_hbm_output_enable_length_get(
    int unit);


uint32 dnx_data_dram_hbm_output_enable_delay_get(
    int unit);


uint32 dnx_data_dram_hbm_default_model_part_num_get(
    int unit);


uint32 dnx_data_dram_hbm_nof_tdu_per_core_get(
    int unit);


uint32 dnx_data_dram_hbm_wds_size_get(
    int unit);



typedef enum
{
    dnx_data_dram_hbm_table_channel_symmetric_regs,
    dnx_data_dram_hbm_table_channel_not_symmetric_regs,
    dnx_data_dram_hbm_table_controller_symmetric_regs,
    dnx_data_dram_hbm_table_controller_not_symmetric_regs,
    dnx_data_dram_hbm_table_channel_interrupt_regs,
    dnx_data_dram_hbm_table_controller_interrupt_regs,
    dnx_data_dram_hbm_table_channel_debug_regs,
    dnx_data_dram_hbm_table_channel_counter_regs,
    dnx_data_dram_hbm_table_channel_type_regs,
    dnx_data_dram_hbm_table_controller_info_regs,
    dnx_data_dram_hbm_table_bist,
    dnx_data_dram_hbm_table_hbc_last_in_chain,

    
    _dnx_data_dram_hbm_table_nof
} dnx_data_dram_hbm_table_e;



const dnx_data_dram_hbm_channel_symmetric_regs_t * dnx_data_dram_hbm_channel_symmetric_regs_get(
    int unit,
    int index);


const dnx_data_dram_hbm_channel_not_symmetric_regs_t * dnx_data_dram_hbm_channel_not_symmetric_regs_get(
    int unit,
    int index);


const dnx_data_dram_hbm_controller_symmetric_regs_t * dnx_data_dram_hbm_controller_symmetric_regs_get(
    int unit,
    int index);


const dnx_data_dram_hbm_controller_not_symmetric_regs_t * dnx_data_dram_hbm_controller_not_symmetric_regs_get(
    int unit,
    int index);


const dnx_data_dram_hbm_channel_interrupt_regs_t * dnx_data_dram_hbm_channel_interrupt_regs_get(
    int unit,
    int index);


const dnx_data_dram_hbm_controller_interrupt_regs_t * dnx_data_dram_hbm_controller_interrupt_regs_get(
    int unit,
    int index);


const dnx_data_dram_hbm_channel_debug_regs_t * dnx_data_dram_hbm_channel_debug_regs_get(
    int unit,
    int index);


const dnx_data_dram_hbm_channel_counter_regs_t * dnx_data_dram_hbm_channel_counter_regs_get(
    int unit,
    int index);


const dnx_data_dram_hbm_channel_type_regs_t * dnx_data_dram_hbm_channel_type_regs_get(
    int unit,
    int index);


const dnx_data_dram_hbm_controller_info_regs_t * dnx_data_dram_hbm_controller_info_regs_get(
    int unit,
    int index);


const dnx_data_dram_hbm_bist_t * dnx_data_dram_hbm_bist_get(
    int unit);


const dnx_data_dram_hbm_hbc_last_in_chain_t * dnx_data_dram_hbm_hbc_last_in_chain_get(
    int unit,
    int dram_bitmap,
    int dram_index);



shr_error_e dnx_data_dram_hbm_channel_symmetric_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_hbm_channel_not_symmetric_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_hbm_controller_symmetric_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_hbm_controller_not_symmetric_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_hbm_channel_interrupt_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_hbm_controller_interrupt_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_hbm_channel_debug_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_hbm_channel_counter_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_hbm_channel_type_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_hbm_controller_info_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_hbm_bist_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_hbm_hbc_last_in_chain_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_dram_hbm_channel_symmetric_regs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_hbm_channel_not_symmetric_regs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_hbm_controller_symmetric_regs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_hbm_controller_not_symmetric_regs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_hbm_channel_interrupt_regs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_hbm_controller_interrupt_regs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_hbm_channel_debug_regs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_hbm_channel_counter_regs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_hbm_channel_type_regs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_hbm_controller_info_regs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_hbm_bist_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_hbm_hbc_last_in_chain_info_get(
    int unit);






int dnx_data_dram_gddr6_feature_get(
    int unit,
    dnx_data_dram_gddr6_feature_e feature);



typedef enum
{
    dnx_data_dram_gddr6_define_nof_ca_bits,
    dnx_data_dram_gddr6_define_bytes_per_channel,
    dnx_data_dram_gddr6_define_training_fifo_depth,
    dnx_data_dram_gddr6_define_readout_to_readout_prd,
    dnx_data_dram_gddr6_define_refresh_to_readout_prd,
    dnx_data_dram_gddr6_define_readout_done_to_done_prd,
    dnx_data_dram_gddr6_define_refresh_mechanism_enable,
    dnx_data_dram_gddr6_define_bist_enable,
    dnx_data_dram_gddr6_define_dynamic_calibration_enable,
    dnx_data_dram_gddr6_define_cdr_enable,
    dnx_data_dram_gddr6_define_write_recovery,
    dnx_data_dram_gddr6_define_cabi,
    dnx_data_dram_gddr6_define_dram_mode,
    dnx_data_dram_gddr6_define_cal_termination,
    dnx_data_dram_gddr6_define_cah_termination,
    dnx_data_dram_gddr6_define_command_pipe_extra_delay,
    dnx_data_dram_gddr6_define_use_11bits_ca,
    dnx_data_dram_gddr6_define_ck_odt,
    dnx_data_dram_gddr6_define_dynamic_calibration_period,

    
    _dnx_data_dram_gddr6_define_nof
} dnx_data_dram_gddr6_define_e;



uint32 dnx_data_dram_gddr6_nof_ca_bits_get(
    int unit);


uint32 dnx_data_dram_gddr6_bytes_per_channel_get(
    int unit);


uint32 dnx_data_dram_gddr6_training_fifo_depth_get(
    int unit);


uint32 dnx_data_dram_gddr6_readout_to_readout_prd_get(
    int unit);


uint32 dnx_data_dram_gddr6_refresh_to_readout_prd_get(
    int unit);


uint32 dnx_data_dram_gddr6_readout_done_to_done_prd_get(
    int unit);


uint32 dnx_data_dram_gddr6_refresh_mechanism_enable_get(
    int unit);


uint32 dnx_data_dram_gddr6_bist_enable_get(
    int unit);


uint32 dnx_data_dram_gddr6_dynamic_calibration_enable_get(
    int unit);


uint32 dnx_data_dram_gddr6_cdr_enable_get(
    int unit);


uint32 dnx_data_dram_gddr6_write_recovery_get(
    int unit);


uint32 dnx_data_dram_gddr6_cabi_get(
    int unit);


uint32 dnx_data_dram_gddr6_dram_mode_get(
    int unit);


uint32 dnx_data_dram_gddr6_cal_termination_get(
    int unit);


uint32 dnx_data_dram_gddr6_cah_termination_get(
    int unit);


uint32 dnx_data_dram_gddr6_command_pipe_extra_delay_get(
    int unit);


uint32 dnx_data_dram_gddr6_use_11bits_ca_get(
    int unit);


uint32 dnx_data_dram_gddr6_ck_odt_get(
    int unit);


uint32 dnx_data_dram_gddr6_dynamic_calibration_period_get(
    int unit);



typedef enum
{
    dnx_data_dram_gddr6_table_refresh_intervals,
    dnx_data_dram_gddr6_table_dq_map,
    dnx_data_dram_gddr6_table_dq_channel_swap,
    dnx_data_dram_gddr6_table_dq_byte_map,
    dnx_data_dram_gddr6_table_ca_map,
    dnx_data_dram_gddr6_table_cadt_byte_map,
    dnx_data_dram_gddr6_table_channel_regs,
    dnx_data_dram_gddr6_table_controller_regs,
    dnx_data_dram_gddr6_table_channel_interrupt_regs,
    dnx_data_dram_gddr6_table_controller_interrupt_regs,
    dnx_data_dram_gddr6_table_channel_debug_regs,
    dnx_data_dram_gddr6_table_channel_counter_regs,
    dnx_data_dram_gddr6_table_channel_type_regs,
    dnx_data_dram_gddr6_table_controller_info_regs,
    dnx_data_dram_gddr6_table_master_phy,

    
    _dnx_data_dram_gddr6_table_nof
} dnx_data_dram_gddr6_table_e;



const dnx_data_dram_gddr6_refresh_intervals_t * dnx_data_dram_gddr6_refresh_intervals_get(
    int unit);


const dnx_data_dram_gddr6_dq_map_t * dnx_data_dram_gddr6_dq_map_get(
    int unit,
    int dram_index,
    int byte);


const dnx_data_dram_gddr6_dq_channel_swap_t * dnx_data_dram_gddr6_dq_channel_swap_get(
    int unit,
    int dram_index);


const dnx_data_dram_gddr6_dq_byte_map_t * dnx_data_dram_gddr6_dq_byte_map_get(
    int unit,
    int dram_index,
    int byte);


const dnx_data_dram_gddr6_ca_map_t * dnx_data_dram_gddr6_ca_map_get(
    int unit,
    int dram_index,
    int channel);


const dnx_data_dram_gddr6_cadt_byte_map_t * dnx_data_dram_gddr6_cadt_byte_map_get(
    int unit,
    int dram_index,
    int channel);


const dnx_data_dram_gddr6_channel_regs_t * dnx_data_dram_gddr6_channel_regs_get(
    int unit,
    int index);


const dnx_data_dram_gddr6_controller_regs_t * dnx_data_dram_gddr6_controller_regs_get(
    int unit,
    int index);


const dnx_data_dram_gddr6_channel_interrupt_regs_t * dnx_data_dram_gddr6_channel_interrupt_regs_get(
    int unit,
    int index);


const dnx_data_dram_gddr6_controller_interrupt_regs_t * dnx_data_dram_gddr6_controller_interrupt_regs_get(
    int unit,
    int index);


const dnx_data_dram_gddr6_channel_debug_regs_t * dnx_data_dram_gddr6_channel_debug_regs_get(
    int unit,
    int index);


const dnx_data_dram_gddr6_channel_counter_regs_t * dnx_data_dram_gddr6_channel_counter_regs_get(
    int unit,
    int index);


const dnx_data_dram_gddr6_channel_type_regs_t * dnx_data_dram_gddr6_channel_type_regs_get(
    int unit,
    int index);


const dnx_data_dram_gddr6_controller_info_regs_t * dnx_data_dram_gddr6_controller_info_regs_get(
    int unit,
    int index);


const dnx_data_dram_gddr6_master_phy_t * dnx_data_dram_gddr6_master_phy_get(
    int unit,
    int dram_index);



shr_error_e dnx_data_dram_gddr6_refresh_intervals_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_gddr6_dq_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_gddr6_dq_channel_swap_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_gddr6_dq_byte_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_gddr6_ca_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_gddr6_cadt_byte_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_gddr6_channel_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_gddr6_controller_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_gddr6_channel_interrupt_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_gddr6_controller_interrupt_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_gddr6_channel_debug_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_gddr6_channel_counter_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_gddr6_channel_type_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_gddr6_controller_info_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_gddr6_master_phy_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_dram_gddr6_refresh_intervals_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_gddr6_dq_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_gddr6_dq_channel_swap_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_gddr6_dq_byte_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_gddr6_ca_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_gddr6_cadt_byte_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_gddr6_channel_regs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_gddr6_controller_regs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_gddr6_channel_interrupt_regs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_gddr6_controller_interrupt_regs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_gddr6_channel_debug_regs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_gddr6_channel_counter_regs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_gddr6_channel_type_regs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_gddr6_controller_info_regs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_gddr6_master_phy_info_get(
    int unit);






int dnx_data_dram_general_info_feature_get(
    int unit,
    dnx_data_dram_general_info_feature_e feature);



typedef enum
{
    dnx_data_dram_general_info_define_otp_restore_version,
    dnx_data_dram_general_info_define_max_nof_drams,
    dnx_data_dram_general_info_define_nof_channels,
    dnx_data_dram_general_info_define_mr_mask,
    dnx_data_dram_general_info_define_nof_mrs,
    dnx_data_dram_general_info_define_phy_address_mask,
    dnx_data_dram_general_info_define_max_dram_index,
    dnx_data_dram_general_info_define_min_dram_index,
    dnx_data_dram_general_info_define_frequency,
    dnx_data_dram_general_info_define_buffer_size,
    dnx_data_dram_general_info_define_command_address_parity,
    dnx_data_dram_general_info_define_dq_write_parity,
    dnx_data_dram_general_info_define_dq_read_parity,
    dnx_data_dram_general_info_define_dbi_read,
    dnx_data_dram_general_info_define_dbi_write,
    dnx_data_dram_general_info_define_write_latency,
    dnx_data_dram_general_info_define_read_latency,
    dnx_data_dram_general_info_define_read_data_enable_length,
    dnx_data_dram_general_info_define_parity_latency,
    dnx_data_dram_general_info_define_actual_parity_latency,
    dnx_data_dram_general_info_define_tune_mode_on_init,
    dnx_data_dram_general_info_define_command_parity_latency,
    dnx_data_dram_general_info_define_crc_write_latency,
    dnx_data_dram_general_info_define_crc_read_latency,
    dnx_data_dram_general_info_define_crc_write,
    dnx_data_dram_general_info_define_crc_read,
    dnx_data_dram_general_info_define_device_size,

    
    _dnx_data_dram_general_info_define_nof
} dnx_data_dram_general_info_define_e;



uint32 dnx_data_dram_general_info_otp_restore_version_get(
    int unit);


uint32 dnx_data_dram_general_info_max_nof_drams_get(
    int unit);


uint32 dnx_data_dram_general_info_nof_channels_get(
    int unit);


uint32 dnx_data_dram_general_info_mr_mask_get(
    int unit);


uint32 dnx_data_dram_general_info_nof_mrs_get(
    int unit);


uint32 dnx_data_dram_general_info_phy_address_mask_get(
    int unit);


uint32 dnx_data_dram_general_info_max_dram_index_get(
    int unit);


uint32 dnx_data_dram_general_info_min_dram_index_get(
    int unit);


uint32 dnx_data_dram_general_info_frequency_get(
    int unit);


uint32 dnx_data_dram_general_info_buffer_size_get(
    int unit);


uint32 dnx_data_dram_general_info_command_address_parity_get(
    int unit);


uint32 dnx_data_dram_general_info_dq_write_parity_get(
    int unit);


uint32 dnx_data_dram_general_info_dq_read_parity_get(
    int unit);


uint32 dnx_data_dram_general_info_dbi_read_get(
    int unit);


uint32 dnx_data_dram_general_info_dbi_write_get(
    int unit);


uint32 dnx_data_dram_general_info_write_latency_get(
    int unit);


uint32 dnx_data_dram_general_info_read_latency_get(
    int unit);


uint32 dnx_data_dram_general_info_read_data_enable_length_get(
    int unit);


uint32 dnx_data_dram_general_info_parity_latency_get(
    int unit);


uint32 dnx_data_dram_general_info_actual_parity_latency_get(
    int unit);


uint32 dnx_data_dram_general_info_tune_mode_on_init_get(
    int unit);


uint32 dnx_data_dram_general_info_command_parity_latency_get(
    int unit);


uint32 dnx_data_dram_general_info_crc_write_latency_get(
    int unit);


uint32 dnx_data_dram_general_info_crc_read_latency_get(
    int unit);


uint32 dnx_data_dram_general_info_crc_write_get(
    int unit);


uint32 dnx_data_dram_general_info_crc_read_get(
    int unit);


uint32 dnx_data_dram_general_info_device_size_get(
    int unit);



typedef enum
{
    dnx_data_dram_general_info_table_mr_defaults,
    dnx_data_dram_general_info_table_dram_info,
    dnx_data_dram_general_info_table_timing_params,
    dnx_data_dram_general_info_table_refresh_intervals,

    
    _dnx_data_dram_general_info_table_nof
} dnx_data_dram_general_info_table_e;



const dnx_data_dram_general_info_mr_defaults_t * dnx_data_dram_general_info_mr_defaults_get(
    int unit,
    int index);


const dnx_data_dram_general_info_dram_info_t * dnx_data_dram_general_info_dram_info_get(
    int unit);


const dnx_data_dram_general_info_timing_params_t * dnx_data_dram_general_info_timing_params_get(
    int unit);


const dnx_data_dram_general_info_refresh_intervals_t * dnx_data_dram_general_info_refresh_intervals_get(
    int unit,
    int temp_index);



shr_error_e dnx_data_dram_general_info_mr_defaults_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_general_info_dram_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_general_info_timing_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_general_info_refresh_intervals_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_dram_general_info_mr_defaults_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_general_info_dram_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_general_info_timing_params_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_general_info_refresh_intervals_info_get(
    int unit);






int dnx_data_dram_address_translation_feature_get(
    int unit,
    dnx_data_dram_address_translation_feature_e feature);



typedef enum
{
    dnx_data_dram_address_translation_define_matrix_column_size,
    dnx_data_dram_address_translation_define_physical_address_transaction_size,
    dnx_data_dram_address_translation_define_nof_atms,
    dnx_data_dram_address_translation_define_nof_tdus_per_dram,
    dnx_data_dram_address_translation_define_nof_tdus,

    
    _dnx_data_dram_address_translation_define_nof
} dnx_data_dram_address_translation_define_e;



uint32 dnx_data_dram_address_translation_matrix_column_size_get(
    int unit);


uint32 dnx_data_dram_address_translation_physical_address_transaction_size_get(
    int unit);


uint32 dnx_data_dram_address_translation_nof_atms_get(
    int unit);


uint32 dnx_data_dram_address_translation_nof_tdus_per_dram_get(
    int unit);


uint32 dnx_data_dram_address_translation_nof_tdus_get(
    int unit);



typedef enum
{
    dnx_data_dram_address_translation_table_tdu_map,
    dnx_data_dram_address_translation_table_matrix_configuration,
    dnx_data_dram_address_translation_table_interrupt_regs,
    dnx_data_dram_address_translation_table_counter_regs,

    
    _dnx_data_dram_address_translation_table_nof
} dnx_data_dram_address_translation_table_e;



const dnx_data_dram_address_translation_tdu_map_t * dnx_data_dram_address_translation_tdu_map_get(
    int unit,
    int dram_index,
    int tdu_index);


const dnx_data_dram_address_translation_matrix_configuration_t * dnx_data_dram_address_translation_matrix_configuration_get(
    int unit,
    int matrix_option);


const dnx_data_dram_address_translation_interrupt_regs_t * dnx_data_dram_address_translation_interrupt_regs_get(
    int unit,
    int index);


const dnx_data_dram_address_translation_counter_regs_t * dnx_data_dram_address_translation_counter_regs_get(
    int unit,
    int index);



shr_error_e dnx_data_dram_address_translation_tdu_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_address_translation_matrix_configuration_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_address_translation_interrupt_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_address_translation_counter_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_dram_address_translation_tdu_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_address_translation_matrix_configuration_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_address_translation_interrupt_regs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_address_translation_counter_regs_info_get(
    int unit);






int dnx_data_dram_buffers_feature_get(
    int unit,
    dnx_data_dram_buffers_feature_e feature);



typedef enum
{
    dnx_data_dram_buffers_define_allowed_errors,
    dnx_data_dram_buffers_define_nof_bdbs,
    dnx_data_dram_buffers_define_nof_fpc_banks,

    
    _dnx_data_dram_buffers_define_nof
} dnx_data_dram_buffers_define_e;



uint32 dnx_data_dram_buffers_allowed_errors_get(
    int unit);


uint32 dnx_data_dram_buffers_nof_bdbs_get(
    int unit);


uint32 dnx_data_dram_buffers_nof_fpc_banks_get(
    int unit);



typedef enum
{
    dnx_data_dram_buffers_table_deleted_buffers_info,

    
    _dnx_data_dram_buffers_table_nof
} dnx_data_dram_buffers_table_e;



const dnx_data_dram_buffers_deleted_buffers_info_t * dnx_data_dram_buffers_deleted_buffers_info_get(
    int unit);



shr_error_e dnx_data_dram_buffers_deleted_buffers_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_dram_buffers_deleted_buffers_info_info_get(
    int unit);






int dnx_data_dram_dram_block_feature_get(
    int unit,
    dnx_data_dram_dram_block_feature_e feature);



typedef enum
{
    dnx_data_dram_dram_block_define_leaky_bucket_window_size,
    dnx_data_dram_dram_block_define_wmr_reset_on_deassert,
    dnx_data_dram_dram_block_define_wmr_full_size,
    dnx_data_dram_dram_block_define_average_read_inflights_assert_threshold,
    dnx_data_dram_dram_block_define_average_read_inflights_full_size,
    dnx_data_dram_dram_block_define_wmr_decrement_thr_factor,
    dnx_data_dram_dram_block_define_wpr_increment_thr_factor,

    
    _dnx_data_dram_dram_block_define_nof
} dnx_data_dram_dram_block_define_e;



uint32 dnx_data_dram_dram_block_leaky_bucket_window_size_get(
    int unit);


uint32 dnx_data_dram_dram_block_wmr_reset_on_deassert_get(
    int unit);


uint32 dnx_data_dram_dram_block_wmr_full_size_get(
    int unit);


uint32 dnx_data_dram_dram_block_average_read_inflights_assert_threshold_get(
    int unit);


uint32 dnx_data_dram_dram_block_average_read_inflights_full_size_get(
    int unit);


uint32 dnx_data_dram_dram_block_wmr_decrement_thr_factor_get(
    int unit);


uint32 dnx_data_dram_dram_block_wpr_increment_thr_factor_get(
    int unit);



typedef enum
{
    dnx_data_dram_dram_block_table_wpr_leaky_bucket_increment_th,
    dnx_data_dram_dram_block_table_write_leaky_bucket_increment_th,

    
    _dnx_data_dram_dram_block_table_nof
} dnx_data_dram_dram_block_table_e;



const dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_t * dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_get(
    int unit,
    int index);


const dnx_data_dram_dram_block_write_leaky_bucket_increment_th_t * dnx_data_dram_dram_block_write_leaky_bucket_increment_th_get(
    int unit,
    int index);



shr_error_e dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_dram_block_write_leaky_bucket_increment_th_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_dram_block_write_leaky_bucket_increment_th_info_get(
    int unit);






int dnx_data_dram_dbal_feature_get(
    int unit,
    dnx_data_dram_dbal_feature_e feature);



typedef enum
{
    dnx_data_dram_dbal_define_hbm_trc_nof_bits,
    dnx_data_dram_dbal_define_wpr_increment_threshold_nof_bits,
    dnx_data_dram_dbal_define_average_read_inflights_increment_threshold_nof_bits,
    dnx_data_dram_dbal_define_average_read_inflights_decrement_threshold_nof_bits,
    dnx_data_dram_dbal_define_hbm_pll_pdiv_nof_bits,
    dnx_data_dram_dbal_define_hbm_pll_ch_mdiv_nof_bits,
    dnx_data_dram_dbal_define_hbm_pll_frefeff_info_nof_bits,
    dnx_data_dram_dbal_define_hbm_pll_aux_post_enableb_nof_bits,
    dnx_data_dram_dbal_define_hbm_pll_ch_enableb_nof_bits,
    dnx_data_dram_dbal_define_hbm_pll_aux_post_diffcmos_en_nof_bits,
    dnx_data_dram_dbal_define_dram_bist_mode_nof_bits,
    dnx_data_dram_dbal_define_hbmc_index_nof_bits,
    dnx_data_dram_dbal_define_hbmc_tdu_index_nof_bits,

    
    _dnx_data_dram_dbal_define_nof
} dnx_data_dram_dbal_define_e;



uint32 dnx_data_dram_dbal_hbm_trc_nof_bits_get(
    int unit);


uint32 dnx_data_dram_dbal_wpr_increment_threshold_nof_bits_get(
    int unit);


uint32 dnx_data_dram_dbal_average_read_inflights_increment_threshold_nof_bits_get(
    int unit);


uint32 dnx_data_dram_dbal_average_read_inflights_decrement_threshold_nof_bits_get(
    int unit);


uint32 dnx_data_dram_dbal_hbm_pll_pdiv_nof_bits_get(
    int unit);


uint32 dnx_data_dram_dbal_hbm_pll_ch_mdiv_nof_bits_get(
    int unit);


uint32 dnx_data_dram_dbal_hbm_pll_frefeff_info_nof_bits_get(
    int unit);


uint32 dnx_data_dram_dbal_hbm_pll_aux_post_enableb_nof_bits_get(
    int unit);


uint32 dnx_data_dram_dbal_hbm_pll_ch_enableb_nof_bits_get(
    int unit);


uint32 dnx_data_dram_dbal_hbm_pll_aux_post_diffcmos_en_nof_bits_get(
    int unit);


uint32 dnx_data_dram_dbal_dram_bist_mode_nof_bits_get(
    int unit);


uint32 dnx_data_dram_dbal_hbmc_index_nof_bits_get(
    int unit);


uint32 dnx_data_dram_dbal_hbmc_tdu_index_nof_bits_get(
    int unit);



typedef enum
{

    
    _dnx_data_dram_dbal_table_nof
} dnx_data_dram_dbal_table_e;









int dnx_data_dram_firmware_feature_get(
    int unit,
    dnx_data_dram_firmware_feature_e feature);



typedef enum
{
    dnx_data_dram_firmware_define_nof_hbm_spare_data_results,
    dnx_data_dram_firmware_define_sbus_clock_divider,
    dnx_data_dram_firmware_define_snap_state_init_done,

    
    _dnx_data_dram_firmware_define_nof
} dnx_data_dram_firmware_define_e;



uint32 dnx_data_dram_firmware_nof_hbm_spare_data_results_get(
    int unit);


uint32 dnx_data_dram_firmware_sbus_clock_divider_get(
    int unit);


uint32 dnx_data_dram_firmware_snap_state_init_done_get(
    int unit);



typedef enum
{
    dnx_data_dram_firmware_table_rom,
    dnx_data_dram_firmware_table_operation_status,
    dnx_data_dram_firmware_table_operation_result_error_code,
    dnx_data_dram_firmware_table_operation_result_last_operation,

    
    _dnx_data_dram_firmware_table_nof
} dnx_data_dram_firmware_table_e;



const dnx_data_dram_firmware_rom_t * dnx_data_dram_firmware_rom_get(
    int unit);


const dnx_data_dram_firmware_operation_status_t * dnx_data_dram_firmware_operation_status_get(
    int unit);


const dnx_data_dram_firmware_operation_result_error_code_t * dnx_data_dram_firmware_operation_result_error_code_get(
    int unit,
    int error_code);


const dnx_data_dram_firmware_operation_result_last_operation_t * dnx_data_dram_firmware_operation_result_last_operation_get(
    int unit,
    int operation_code);



shr_error_e dnx_data_dram_firmware_rom_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_firmware_operation_status_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_firmware_operation_result_error_code_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dram_firmware_operation_result_last_operation_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_dram_firmware_rom_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_firmware_operation_status_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_firmware_operation_result_error_code_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dram_firmware_operation_result_last_operation_info_get(
    int unit);






int dnx_data_dram_apd_phy_feature_get(
    int unit,
    dnx_data_dram_apd_phy_feature_e feature);



typedef enum
{

    
    _dnx_data_dram_apd_phy_define_nof
} dnx_data_dram_apd_phy_define_e;




typedef enum
{
    dnx_data_dram_apd_phy_table_pll,

    
    _dnx_data_dram_apd_phy_table_nof
} dnx_data_dram_apd_phy_table_e;



const dnx_data_dram_apd_phy_pll_t * dnx_data_dram_apd_phy_pll_get(
    int unit);



shr_error_e dnx_data_dram_apd_phy_pll_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_dram_apd_phy_pll_info_get(
    int unit);



shr_error_e dnx_data_dram_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

