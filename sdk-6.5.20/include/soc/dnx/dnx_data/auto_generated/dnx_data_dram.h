

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_DRAM_H_

#define _DNX_DATA_DRAM_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm_int/dnx/dram/hbmc/hbmc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_dram.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_dram_init(
    int unit);






typedef struct
{
    
    soc_reg_t reg;
} dnx_data_dram_hbm_channel_symmetric_regs_t;


typedef struct
{
    
    soc_reg_t reg;
} dnx_data_dram_hbm_channel_not_symmetric_regs_t;


typedef struct
{
    
    soc_reg_t reg;
} dnx_data_dram_hbm_controller_symmetric_regs_t;


typedef struct
{
    
    soc_reg_t reg;
} dnx_data_dram_hbm_controller_not_symmetric_regs_t;


typedef struct
{
    
    soc_reg_t reg;
} dnx_data_dram_hbm_channel_interrupt_regs_t;


typedef struct
{
    
    soc_reg_t reg;
} dnx_data_dram_hbm_controller_interrupt_regs_t;


typedef struct
{
    
    soc_reg_t reg;
    
    char *reassuring_str;
} dnx_data_dram_hbm_channel_debug_regs_t;


typedef struct
{
    
    soc_reg_t reg;
} dnx_data_dram_hbm_channel_counter_regs_t;


typedef struct
{
    
    soc_reg_t reg;
    
    dnx_hbmc_diag_registers_type_t type;
} dnx_data_dram_hbm_channel_type_regs_t;


typedef struct
{
    
    soc_reg_t reg;
} dnx_data_dram_hbm_controller_info_regs_t;


typedef struct
{
    
    uint32 prbs_seeds[HBMC_SHMOO_BIST_NOF_PRBS_DATA_SEEDS];
} dnx_data_dram_hbm_bist_t;


typedef struct
{
    
    uint32 is_last_in_chain;
} dnx_data_dram_hbm_hbc_last_in_chain_t;



typedef enum
{
    
    dnx_data_dram_hbm_is_supported,
    
    dnx_data_dram_hbm_dword_alignment_check,
    
    dnx_data_dram_hbm_low_temperature_dram_protction,
    
    dnx_data_dram_hbm_is_delete_bdb_supported,
    
    dnx_data_dram_hbm_cpu2tap_access,
    
    dnx_data_dram_hbm_seamless_commands_to_same_bank_control,
    
    dnx_data_dram_hbm_vendor_info_disable,
    
    dnx_data_dram_hbm_apd_phy,
    
    dnx_data_dram_hbm_is_hrc_supported,
    
    dnx_data_dram_hbm_channel_wds_inflight_threshold,

    
    _dnx_data_dram_hbm_feature_nof
} dnx_data_dram_hbm_feature_e;



typedef int(
    *dnx_data_dram_hbm_feature_get_f) (
    int unit,
    dnx_data_dram_hbm_feature_e feature);


typedef uint32(
    *dnx_data_dram_hbm_burst_length_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_hbm_stop_traffic_temp_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_hbm_stop_traffic_low_temp_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_hbm_restore_traffic_temp_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_hbm_restore_traffic_low_temp_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_hbm_usec_between_temp_samples_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_hbm_power_down_temp_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_hbm_power_down_low_temp_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_hbm_dram_temp_monitor_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_hbm_start_disabled_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_hbm_output_enable_length_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_hbm_output_enable_delay_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_hbm_default_model_part_num_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_hbm_nof_tdu_per_core_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_hbm_wds_size_get_f) (
    int unit);


typedef const dnx_data_dram_hbm_channel_symmetric_regs_t *(
    *dnx_data_dram_hbm_channel_symmetric_regs_get_f) (
    int unit,
    int index);


typedef const dnx_data_dram_hbm_channel_not_symmetric_regs_t *(
    *dnx_data_dram_hbm_channel_not_symmetric_regs_get_f) (
    int unit,
    int index);


typedef const dnx_data_dram_hbm_controller_symmetric_regs_t *(
    *dnx_data_dram_hbm_controller_symmetric_regs_get_f) (
    int unit,
    int index);


typedef const dnx_data_dram_hbm_controller_not_symmetric_regs_t *(
    *dnx_data_dram_hbm_controller_not_symmetric_regs_get_f) (
    int unit,
    int index);


typedef const dnx_data_dram_hbm_channel_interrupt_regs_t *(
    *dnx_data_dram_hbm_channel_interrupt_regs_get_f) (
    int unit,
    int index);


typedef const dnx_data_dram_hbm_controller_interrupt_regs_t *(
    *dnx_data_dram_hbm_controller_interrupt_regs_get_f) (
    int unit,
    int index);


typedef const dnx_data_dram_hbm_channel_debug_regs_t *(
    *dnx_data_dram_hbm_channel_debug_regs_get_f) (
    int unit,
    int index);


typedef const dnx_data_dram_hbm_channel_counter_regs_t *(
    *dnx_data_dram_hbm_channel_counter_regs_get_f) (
    int unit,
    int index);


typedef const dnx_data_dram_hbm_channel_type_regs_t *(
    *dnx_data_dram_hbm_channel_type_regs_get_f) (
    int unit,
    int index);


typedef const dnx_data_dram_hbm_controller_info_regs_t *(
    *dnx_data_dram_hbm_controller_info_regs_get_f) (
    int unit,
    int index);


typedef const dnx_data_dram_hbm_bist_t *(
    *dnx_data_dram_hbm_bist_get_f) (
    int unit);


typedef const dnx_data_dram_hbm_hbc_last_in_chain_t *(
    *dnx_data_dram_hbm_hbc_last_in_chain_get_f) (
    int unit,
    int dram_bitmap,
    int dram_index);



typedef struct
{
    
    dnx_data_dram_hbm_feature_get_f feature_get;
    
    dnx_data_dram_hbm_burst_length_get_f burst_length_get;
    
    dnx_data_dram_hbm_stop_traffic_temp_threshold_get_f stop_traffic_temp_threshold_get;
    
    dnx_data_dram_hbm_stop_traffic_low_temp_threshold_get_f stop_traffic_low_temp_threshold_get;
    
    dnx_data_dram_hbm_restore_traffic_temp_threshold_get_f restore_traffic_temp_threshold_get;
    
    dnx_data_dram_hbm_restore_traffic_low_temp_threshold_get_f restore_traffic_low_temp_threshold_get;
    
    dnx_data_dram_hbm_usec_between_temp_samples_get_f usec_between_temp_samples_get;
    
    dnx_data_dram_hbm_power_down_temp_threshold_get_f power_down_temp_threshold_get;
    
    dnx_data_dram_hbm_power_down_low_temp_threshold_get_f power_down_low_temp_threshold_get;
    
    dnx_data_dram_hbm_dram_temp_monitor_enable_get_f dram_temp_monitor_enable_get;
    
    dnx_data_dram_hbm_start_disabled_get_f start_disabled_get;
    
    dnx_data_dram_hbm_output_enable_length_get_f output_enable_length_get;
    
    dnx_data_dram_hbm_output_enable_delay_get_f output_enable_delay_get;
    
    dnx_data_dram_hbm_default_model_part_num_get_f default_model_part_num_get;
    
    dnx_data_dram_hbm_nof_tdu_per_core_get_f nof_tdu_per_core_get;
    
    dnx_data_dram_hbm_wds_size_get_f wds_size_get;
    
    dnx_data_dram_hbm_channel_symmetric_regs_get_f channel_symmetric_regs_get;
    
    dnxc_data_table_info_get_f channel_symmetric_regs_info_get;
    
    dnx_data_dram_hbm_channel_not_symmetric_regs_get_f channel_not_symmetric_regs_get;
    
    dnxc_data_table_info_get_f channel_not_symmetric_regs_info_get;
    
    dnx_data_dram_hbm_controller_symmetric_regs_get_f controller_symmetric_regs_get;
    
    dnxc_data_table_info_get_f controller_symmetric_regs_info_get;
    
    dnx_data_dram_hbm_controller_not_symmetric_regs_get_f controller_not_symmetric_regs_get;
    
    dnxc_data_table_info_get_f controller_not_symmetric_regs_info_get;
    
    dnx_data_dram_hbm_channel_interrupt_regs_get_f channel_interrupt_regs_get;
    
    dnxc_data_table_info_get_f channel_interrupt_regs_info_get;
    
    dnx_data_dram_hbm_controller_interrupt_regs_get_f controller_interrupt_regs_get;
    
    dnxc_data_table_info_get_f controller_interrupt_regs_info_get;
    
    dnx_data_dram_hbm_channel_debug_regs_get_f channel_debug_regs_get;
    
    dnxc_data_table_info_get_f channel_debug_regs_info_get;
    
    dnx_data_dram_hbm_channel_counter_regs_get_f channel_counter_regs_get;
    
    dnxc_data_table_info_get_f channel_counter_regs_info_get;
    
    dnx_data_dram_hbm_channel_type_regs_get_f channel_type_regs_get;
    
    dnxc_data_table_info_get_f channel_type_regs_info_get;
    
    dnx_data_dram_hbm_controller_info_regs_get_f controller_info_regs_get;
    
    dnxc_data_table_info_get_f controller_info_regs_info_get;
    
    dnx_data_dram_hbm_bist_get_f bist_get;
    
    dnxc_data_table_info_get_f bist_info_get;
    
    dnx_data_dram_hbm_hbc_last_in_chain_get_f hbc_last_in_chain_get;
    
    dnxc_data_table_info_get_f hbc_last_in_chain_info_get;
} dnx_data_if_dram_hbm_t;






typedef struct
{
    
    uint32 trefiab;
    
    uint32 trefiab_acc;
    
    uint32 trefisb;
} dnx_data_dram_gddr6_refresh_intervals_t;


typedef struct
{
    
    uint8 dq_map[8];
} dnx_data_dram_gddr6_dq_map_t;


typedef struct
{
    
    uint8 dq_channel_swap;
} dnx_data_dram_gddr6_dq_channel_swap_t;


typedef struct
{
    
    uint8 dq_byte_map;
} dnx_data_dram_gddr6_dq_byte_map_t;


typedef struct
{
    
    uint8 ca_map[12];
} dnx_data_dram_gddr6_ca_map_t;


typedef struct
{
    
    uint8 cadt_byte_map;
} dnx_data_dram_gddr6_cadt_byte_map_t;


typedef struct
{
    
    soc_reg_t reg;
} dnx_data_dram_gddr6_channel_regs_t;


typedef struct
{
    
    soc_reg_t reg;
} dnx_data_dram_gddr6_controller_regs_t;


typedef struct
{
    
    soc_reg_t reg;
} dnx_data_dram_gddr6_channel_interrupt_regs_t;


typedef struct
{
    
    soc_reg_t reg;
} dnx_data_dram_gddr6_controller_interrupt_regs_t;


typedef struct
{
    
    soc_reg_t reg;
    
    char *reassuring_str;
} dnx_data_dram_gddr6_channel_debug_regs_t;


typedef struct
{
    
    soc_reg_t reg;
} dnx_data_dram_gddr6_channel_counter_regs_t;


typedef struct
{
    
    soc_reg_t reg;
    
    dnx_hbmc_diag_registers_type_t type;
} dnx_data_dram_gddr6_channel_type_regs_t;


typedef struct
{
    
    soc_reg_t reg;
} dnx_data_dram_gddr6_controller_info_regs_t;


typedef struct
{
    
    uint32 master_dram_index;
} dnx_data_dram_gddr6_master_phy_t;



typedef enum
{
    
    dnx_data_dram_gddr6_is_supported,
    
    dnx_data_dram_gddr6_allow_disable_read_crc,
    
    dnx_data_dram_gddr6_interleaved_refresh_cycles,
    
    dnx_data_dram_gddr6_refresh_pend_switch,
    
    dnx_data_dram_gddr6_cadt_exit_mode_when_done,
    
    dnx_data_dram_gddr6_periodic_temp_readout,
    dnx_data_dram_gddr6_controller_coherency_handling_mode,

    
    _dnx_data_dram_gddr6_feature_nof
} dnx_data_dram_gddr6_feature_e;



typedef int(
    *dnx_data_dram_gddr6_feature_get_f) (
    int unit,
    dnx_data_dram_gddr6_feature_e feature);


typedef uint32(
    *dnx_data_dram_gddr6_nof_ca_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_gddr6_bytes_per_channel_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_gddr6_training_fifo_depth_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_gddr6_readout_to_readout_prd_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_gddr6_refresh_to_readout_prd_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_gddr6_readout_done_to_done_prd_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_gddr6_refresh_mechanism_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_gddr6_bist_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_gddr6_dynamic_calibration_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_gddr6_cdr_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_gddr6_write_recovery_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_gddr6_cabi_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_gddr6_dram_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_gddr6_cal_termination_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_gddr6_cah_termination_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_gddr6_command_pipe_extra_delay_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_gddr6_use_11bits_ca_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_gddr6_ck_odt_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_gddr6_dynamic_calibration_period_get_f) (
    int unit);


typedef const dnx_data_dram_gddr6_refresh_intervals_t *(
    *dnx_data_dram_gddr6_refresh_intervals_get_f) (
    int unit);


typedef const dnx_data_dram_gddr6_dq_map_t *(
    *dnx_data_dram_gddr6_dq_map_get_f) (
    int unit,
    int dram_index,
    int byte);


typedef const dnx_data_dram_gddr6_dq_channel_swap_t *(
    *dnx_data_dram_gddr6_dq_channel_swap_get_f) (
    int unit,
    int dram_index);


typedef const dnx_data_dram_gddr6_dq_byte_map_t *(
    *dnx_data_dram_gddr6_dq_byte_map_get_f) (
    int unit,
    int dram_index,
    int byte);


typedef const dnx_data_dram_gddr6_ca_map_t *(
    *dnx_data_dram_gddr6_ca_map_get_f) (
    int unit,
    int dram_index,
    int channel);


typedef const dnx_data_dram_gddr6_cadt_byte_map_t *(
    *dnx_data_dram_gddr6_cadt_byte_map_get_f) (
    int unit,
    int dram_index,
    int channel);


typedef const dnx_data_dram_gddr6_channel_regs_t *(
    *dnx_data_dram_gddr6_channel_regs_get_f) (
    int unit,
    int index);


typedef const dnx_data_dram_gddr6_controller_regs_t *(
    *dnx_data_dram_gddr6_controller_regs_get_f) (
    int unit,
    int index);


typedef const dnx_data_dram_gddr6_channel_interrupt_regs_t *(
    *dnx_data_dram_gddr6_channel_interrupt_regs_get_f) (
    int unit,
    int index);


typedef const dnx_data_dram_gddr6_controller_interrupt_regs_t *(
    *dnx_data_dram_gddr6_controller_interrupt_regs_get_f) (
    int unit,
    int index);


typedef const dnx_data_dram_gddr6_channel_debug_regs_t *(
    *dnx_data_dram_gddr6_channel_debug_regs_get_f) (
    int unit,
    int index);


typedef const dnx_data_dram_gddr6_channel_counter_regs_t *(
    *dnx_data_dram_gddr6_channel_counter_regs_get_f) (
    int unit,
    int index);


typedef const dnx_data_dram_gddr6_channel_type_regs_t *(
    *dnx_data_dram_gddr6_channel_type_regs_get_f) (
    int unit,
    int index);


typedef const dnx_data_dram_gddr6_controller_info_regs_t *(
    *dnx_data_dram_gddr6_controller_info_regs_get_f) (
    int unit,
    int index);


typedef const dnx_data_dram_gddr6_master_phy_t *(
    *dnx_data_dram_gddr6_master_phy_get_f) (
    int unit,
    int dram_index);



typedef struct
{
    
    dnx_data_dram_gddr6_feature_get_f feature_get;
    
    dnx_data_dram_gddr6_nof_ca_bits_get_f nof_ca_bits_get;
    
    dnx_data_dram_gddr6_bytes_per_channel_get_f bytes_per_channel_get;
    
    dnx_data_dram_gddr6_training_fifo_depth_get_f training_fifo_depth_get;
    
    dnx_data_dram_gddr6_readout_to_readout_prd_get_f readout_to_readout_prd_get;
    
    dnx_data_dram_gddr6_refresh_to_readout_prd_get_f refresh_to_readout_prd_get;
    
    dnx_data_dram_gddr6_readout_done_to_done_prd_get_f readout_done_to_done_prd_get;
    
    dnx_data_dram_gddr6_refresh_mechanism_enable_get_f refresh_mechanism_enable_get;
    
    dnx_data_dram_gddr6_bist_enable_get_f bist_enable_get;
    
    dnx_data_dram_gddr6_dynamic_calibration_enable_get_f dynamic_calibration_enable_get;
    
    dnx_data_dram_gddr6_cdr_enable_get_f cdr_enable_get;
    
    dnx_data_dram_gddr6_write_recovery_get_f write_recovery_get;
    
    dnx_data_dram_gddr6_cabi_get_f cabi_get;
    
    dnx_data_dram_gddr6_dram_mode_get_f dram_mode_get;
    
    dnx_data_dram_gddr6_cal_termination_get_f cal_termination_get;
    
    dnx_data_dram_gddr6_cah_termination_get_f cah_termination_get;
    
    dnx_data_dram_gddr6_command_pipe_extra_delay_get_f command_pipe_extra_delay_get;
    
    dnx_data_dram_gddr6_use_11bits_ca_get_f use_11bits_ca_get;
    
    dnx_data_dram_gddr6_ck_odt_get_f ck_odt_get;
    
    dnx_data_dram_gddr6_dynamic_calibration_period_get_f dynamic_calibration_period_get;
    
    dnx_data_dram_gddr6_refresh_intervals_get_f refresh_intervals_get;
    
    dnxc_data_table_info_get_f refresh_intervals_info_get;
    
    dnx_data_dram_gddr6_dq_map_get_f dq_map_get;
    
    dnxc_data_table_info_get_f dq_map_info_get;
    
    dnx_data_dram_gddr6_dq_channel_swap_get_f dq_channel_swap_get;
    
    dnxc_data_table_info_get_f dq_channel_swap_info_get;
    
    dnx_data_dram_gddr6_dq_byte_map_get_f dq_byte_map_get;
    
    dnxc_data_table_info_get_f dq_byte_map_info_get;
    
    dnx_data_dram_gddr6_ca_map_get_f ca_map_get;
    
    dnxc_data_table_info_get_f ca_map_info_get;
    
    dnx_data_dram_gddr6_cadt_byte_map_get_f cadt_byte_map_get;
    
    dnxc_data_table_info_get_f cadt_byte_map_info_get;
    
    dnx_data_dram_gddr6_channel_regs_get_f channel_regs_get;
    
    dnxc_data_table_info_get_f channel_regs_info_get;
    
    dnx_data_dram_gddr6_controller_regs_get_f controller_regs_get;
    
    dnxc_data_table_info_get_f controller_regs_info_get;
    
    dnx_data_dram_gddr6_channel_interrupt_regs_get_f channel_interrupt_regs_get;
    
    dnxc_data_table_info_get_f channel_interrupt_regs_info_get;
    
    dnx_data_dram_gddr6_controller_interrupt_regs_get_f controller_interrupt_regs_get;
    
    dnxc_data_table_info_get_f controller_interrupt_regs_info_get;
    
    dnx_data_dram_gddr6_channel_debug_regs_get_f channel_debug_regs_get;
    
    dnxc_data_table_info_get_f channel_debug_regs_info_get;
    
    dnx_data_dram_gddr6_channel_counter_regs_get_f channel_counter_regs_get;
    
    dnxc_data_table_info_get_f channel_counter_regs_info_get;
    
    dnx_data_dram_gddr6_channel_type_regs_get_f channel_type_regs_get;
    
    dnxc_data_table_info_get_f channel_type_regs_info_get;
    
    dnx_data_dram_gddr6_controller_info_regs_get_f controller_info_regs_get;
    
    dnxc_data_table_info_get_f controller_info_regs_info_get;
    
    dnx_data_dram_gddr6_master_phy_get_f master_phy_get;
    
    dnxc_data_table_info_get_f master_phy_info_get;
} dnx_data_if_dram_gddr6_t;






typedef struct
{
    
    uint32 value;
} dnx_data_dram_general_info_mr_defaults_t;


typedef struct
{
    
    uint32 dram_type;
    
    uint32 dram_bitmap;
    
    uint32 nof_columns;
    
    uint32 nof_rows;
    
    uint32 nof_banks;
    
    uint32 data_rate;
    
    uint32 ref_clock;
    
    uint32 ctl_type;
} dnx_data_dram_general_info_dram_info_t;


typedef struct
{
    
    uint32 twr;
    
    uint32 trp;
    
    uint32 trtps;
    
    uint32 trtpl;
    
    uint32 trrds;
    
    uint32 trrdl;
    
    uint32 tfaw;
    
    uint32 trcdwr;
    
    uint32 trcdrd;
    
    uint32 tras;
    
    uint32 trc;
    
    uint32 twtrl;
    
    uint32 twtrs;
    
    uint32 trtw;
    
    uint32 tccdr;
    
    uint32 tccds;
    
    uint32 tccdl;
    
    uint32 trrefd;
    
    uint32 trfcsb;
    
    uint32 trfc;
    
    uint32 tmrd;
    
    uint32 tmod;
    
    uint32 twtrtr;
    
    uint32 twrwtr;
    
    uint32 treftr;
    
    uint32 trdtlt;
    
    uint32 trcdwtr;
    
    uint32 trcdrtr;
    
    uint32 trcdltr;
    
    uint32 tltrtr;
    
    uint32 tltltr;
} dnx_data_dram_general_info_timing_params_t;


typedef struct
{
    
    uint32 trefi;
    
    uint32 trefisb;
} dnx_data_dram_general_info_refresh_intervals_t;



typedef enum
{
    
    dnx_data_dram_general_info_is_temperature_reading_supported,

    
    _dnx_data_dram_general_info_feature_nof
} dnx_data_dram_general_info_feature_e;



typedef int(
    *dnx_data_dram_general_info_feature_get_f) (
    int unit,
    dnx_data_dram_general_info_feature_e feature);


typedef uint32(
    *dnx_data_dram_general_info_otp_restore_version_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_max_nof_drams_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_nof_channels_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_mr_mask_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_nof_mrs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_phy_address_mask_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_max_dram_index_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_min_dram_index_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_frequency_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_buffer_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_command_address_parity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_dq_write_parity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_dq_read_parity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_dbi_read_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_dbi_write_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_write_latency_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_read_latency_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_read_data_enable_length_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_parity_latency_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_actual_parity_latency_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_tune_mode_on_init_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_command_parity_latency_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_crc_write_latency_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_crc_read_latency_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_crc_write_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_crc_read_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_general_info_device_size_get_f) (
    int unit);


typedef const dnx_data_dram_general_info_mr_defaults_t *(
    *dnx_data_dram_general_info_mr_defaults_get_f) (
    int unit,
    int index);


typedef const dnx_data_dram_general_info_dram_info_t *(
    *dnx_data_dram_general_info_dram_info_get_f) (
    int unit);


typedef const dnx_data_dram_general_info_timing_params_t *(
    *dnx_data_dram_general_info_timing_params_get_f) (
    int unit);


typedef const dnx_data_dram_general_info_refresh_intervals_t *(
    *dnx_data_dram_general_info_refresh_intervals_get_f) (
    int unit,
    int temp_index);



typedef struct
{
    
    dnx_data_dram_general_info_feature_get_f feature_get;
    
    dnx_data_dram_general_info_otp_restore_version_get_f otp_restore_version_get;
    
    dnx_data_dram_general_info_max_nof_drams_get_f max_nof_drams_get;
    
    dnx_data_dram_general_info_nof_channels_get_f nof_channels_get;
    
    dnx_data_dram_general_info_mr_mask_get_f mr_mask_get;
    
    dnx_data_dram_general_info_nof_mrs_get_f nof_mrs_get;
    
    dnx_data_dram_general_info_phy_address_mask_get_f phy_address_mask_get;
    
    dnx_data_dram_general_info_max_dram_index_get_f max_dram_index_get;
    
    dnx_data_dram_general_info_min_dram_index_get_f min_dram_index_get;
    
    dnx_data_dram_general_info_frequency_get_f frequency_get;
    
    dnx_data_dram_general_info_buffer_size_get_f buffer_size_get;
    
    dnx_data_dram_general_info_command_address_parity_get_f command_address_parity_get;
    
    dnx_data_dram_general_info_dq_write_parity_get_f dq_write_parity_get;
    
    dnx_data_dram_general_info_dq_read_parity_get_f dq_read_parity_get;
    
    dnx_data_dram_general_info_dbi_read_get_f dbi_read_get;
    
    dnx_data_dram_general_info_dbi_write_get_f dbi_write_get;
    
    dnx_data_dram_general_info_write_latency_get_f write_latency_get;
    
    dnx_data_dram_general_info_read_latency_get_f read_latency_get;
    
    dnx_data_dram_general_info_read_data_enable_length_get_f read_data_enable_length_get;
    
    dnx_data_dram_general_info_parity_latency_get_f parity_latency_get;
    
    dnx_data_dram_general_info_actual_parity_latency_get_f actual_parity_latency_get;
    
    dnx_data_dram_general_info_tune_mode_on_init_get_f tune_mode_on_init_get;
    
    dnx_data_dram_general_info_command_parity_latency_get_f command_parity_latency_get;
    
    dnx_data_dram_general_info_crc_write_latency_get_f crc_write_latency_get;
    
    dnx_data_dram_general_info_crc_read_latency_get_f crc_read_latency_get;
    
    dnx_data_dram_general_info_crc_write_get_f crc_write_get;
    
    dnx_data_dram_general_info_crc_read_get_f crc_read_get;
    
    dnx_data_dram_general_info_device_size_get_f device_size_get;
    
    dnx_data_dram_general_info_mr_defaults_get_f mr_defaults_get;
    
    dnxc_data_table_info_get_f mr_defaults_info_get;
    
    dnx_data_dram_general_info_dram_info_get_f dram_info_get;
    
    dnxc_data_table_info_get_f dram_info_info_get;
    
    dnx_data_dram_general_info_timing_params_get_f timing_params_get;
    
    dnxc_data_table_info_get_f timing_params_info_get;
    
    dnx_data_dram_general_info_refresh_intervals_get_f refresh_intervals_get;
    
    dnxc_data_table_info_get_f refresh_intervals_info_get;
} dnx_data_if_dram_general_info_t;






typedef struct
{
    
    uint32 data_source_id;
} dnx_data_dram_address_translation_tdu_map_t;


typedef struct
{
    
    uint32 logical_to_physical[DNX_DATA_MAX_DRAM_ADDRESS_TRANSLATION_MATRIX_COLUMN_SIZE];
} dnx_data_dram_address_translation_matrix_configuration_t;


typedef struct
{
    
    soc_reg_t reg;
} dnx_data_dram_address_translation_interrupt_regs_t;


typedef struct
{
    
    soc_reg_t reg;
} dnx_data_dram_address_translation_counter_regs_t;



typedef enum
{

    
    _dnx_data_dram_address_translation_feature_nof
} dnx_data_dram_address_translation_feature_e;



typedef int(
    *dnx_data_dram_address_translation_feature_get_f) (
    int unit,
    dnx_data_dram_address_translation_feature_e feature);


typedef uint32(
    *dnx_data_dram_address_translation_matrix_column_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_address_translation_physical_address_transaction_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_address_translation_nof_atms_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_address_translation_nof_tdus_per_dram_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_address_translation_nof_tdus_get_f) (
    int unit);


typedef const dnx_data_dram_address_translation_tdu_map_t *(
    *dnx_data_dram_address_translation_tdu_map_get_f) (
    int unit,
    int dram_index,
    int tdu_index);


typedef const dnx_data_dram_address_translation_matrix_configuration_t *(
    *dnx_data_dram_address_translation_matrix_configuration_get_f) (
    int unit,
    int matrix_option);


typedef const dnx_data_dram_address_translation_interrupt_regs_t *(
    *dnx_data_dram_address_translation_interrupt_regs_get_f) (
    int unit,
    int index);


typedef const dnx_data_dram_address_translation_counter_regs_t *(
    *dnx_data_dram_address_translation_counter_regs_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnx_data_dram_address_translation_feature_get_f feature_get;
    
    dnx_data_dram_address_translation_matrix_column_size_get_f matrix_column_size_get;
    
    dnx_data_dram_address_translation_physical_address_transaction_size_get_f physical_address_transaction_size_get;
    
    dnx_data_dram_address_translation_nof_atms_get_f nof_atms_get;
    
    dnx_data_dram_address_translation_nof_tdus_per_dram_get_f nof_tdus_per_dram_get;
    
    dnx_data_dram_address_translation_nof_tdus_get_f nof_tdus_get;
    
    dnx_data_dram_address_translation_tdu_map_get_f tdu_map_get;
    
    dnxc_data_table_info_get_f tdu_map_info_get;
    
    dnx_data_dram_address_translation_matrix_configuration_get_f matrix_configuration_get;
    
    dnxc_data_table_info_get_f matrix_configuration_info_get;
    
    dnx_data_dram_address_translation_interrupt_regs_get_f interrupt_regs_get;
    
    dnxc_data_table_info_get_f interrupt_regs_info_get;
    
    dnx_data_dram_address_translation_counter_regs_get_f counter_regs_get;
    
    dnxc_data_table_info_get_f counter_regs_info_get;
} dnx_data_if_dram_address_translation_t;






typedef struct
{
    
    char *deleted_buffers_file;
} dnx_data_dram_buffers_deleted_buffers_info_t;



typedef enum
{
    
    dnx_data_dram_buffers_quarantine_buffers_if_tdu_corrected_errors,
    
    dnx_data_dram_buffers_quarantine_buffers_if_tdu_errors,
    
    dnx_data_dram_buffers_bdb_release_mechanism_usage_counters,

    
    _dnx_data_dram_buffers_feature_nof
} dnx_data_dram_buffers_feature_e;



typedef int(
    *dnx_data_dram_buffers_feature_get_f) (
    int unit,
    dnx_data_dram_buffers_feature_e feature);


typedef uint32(
    *dnx_data_dram_buffers_allowed_errors_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_buffers_nof_bdbs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_buffers_nof_fpc_banks_get_f) (
    int unit);


typedef const dnx_data_dram_buffers_deleted_buffers_info_t *(
    *dnx_data_dram_buffers_deleted_buffers_info_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_dram_buffers_feature_get_f feature_get;
    
    dnx_data_dram_buffers_allowed_errors_get_f allowed_errors_get;
    
    dnx_data_dram_buffers_nof_bdbs_get_f nof_bdbs_get;
    
    dnx_data_dram_buffers_nof_fpc_banks_get_f nof_fpc_banks_get;
    
    dnx_data_dram_buffers_deleted_buffers_info_get_f deleted_buffers_info_get;
    
    dnxc_data_table_info_get_f deleted_buffers_info_info_get;
} dnx_data_if_dram_buffers_t;






typedef struct
{
    
    uint32 threshold;
} dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_t;


typedef struct
{
    
    uint32 threshold;
} dnx_data_dram_dram_block_write_leaky_bucket_increment_th_t;



typedef enum
{
    
    dnx_data_dram_dram_block_write_minus_read_leaky_bucket,
    
    dnx_data_dram_dram_block_write_leaky_bucket,
    
    dnx_data_dram_dram_block_write_minus_read_leaky_bucket_disable,
    
    dnx_data_dram_dram_block_write_plus_read_leaky_bucket_disable,
    
    dnx_data_dram_dram_block_average_read_inflights_leaky_bucket,
    
    dnx_data_dram_dram_block_average_read_inflights_log2_window_size,

    
    _dnx_data_dram_dram_block_feature_nof
} dnx_data_dram_dram_block_feature_e;



typedef int(
    *dnx_data_dram_dram_block_feature_get_f) (
    int unit,
    dnx_data_dram_dram_block_feature_e feature);


typedef uint32(
    *dnx_data_dram_dram_block_leaky_bucket_window_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_dram_block_wmr_reset_on_deassert_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_dram_block_wmr_full_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_dram_block_average_read_inflights_assert_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_dram_block_average_read_inflights_full_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_dram_block_wmr_decrement_thr_factor_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_dram_block_wpr_increment_thr_factor_get_f) (
    int unit);


typedef const dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_t *(
    *dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_get_f) (
    int unit,
    int index);


typedef const dnx_data_dram_dram_block_write_leaky_bucket_increment_th_t *(
    *dnx_data_dram_dram_block_write_leaky_bucket_increment_th_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnx_data_dram_dram_block_feature_get_f feature_get;
    
    dnx_data_dram_dram_block_leaky_bucket_window_size_get_f leaky_bucket_window_size_get;
    
    dnx_data_dram_dram_block_wmr_reset_on_deassert_get_f wmr_reset_on_deassert_get;
    
    dnx_data_dram_dram_block_wmr_full_size_get_f wmr_full_size_get;
    
    dnx_data_dram_dram_block_average_read_inflights_assert_threshold_get_f average_read_inflights_assert_threshold_get;
    
    dnx_data_dram_dram_block_average_read_inflights_full_size_get_f average_read_inflights_full_size_get;
    
    dnx_data_dram_dram_block_wmr_decrement_thr_factor_get_f wmr_decrement_thr_factor_get;
    
    dnx_data_dram_dram_block_wpr_increment_thr_factor_get_f wpr_increment_thr_factor_get;
    
    dnx_data_dram_dram_block_wpr_leaky_bucket_increment_th_get_f wpr_leaky_bucket_increment_th_get;
    
    dnxc_data_table_info_get_f wpr_leaky_bucket_increment_th_info_get;
    
    dnx_data_dram_dram_block_write_leaky_bucket_increment_th_get_f write_leaky_bucket_increment_th_get;
    
    dnxc_data_table_info_get_f write_leaky_bucket_increment_th_info_get;
} dnx_data_if_dram_dram_block_t;







typedef enum
{

    
    _dnx_data_dram_dbal_feature_nof
} dnx_data_dram_dbal_feature_e;



typedef int(
    *dnx_data_dram_dbal_feature_get_f) (
    int unit,
    dnx_data_dram_dbal_feature_e feature);


typedef uint32(
    *dnx_data_dram_dbal_hbm_trc_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_dbal_wpr_increment_threshold_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_dbal_average_read_inflights_increment_threshold_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_dbal_average_read_inflights_decrement_threshold_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_dbal_hbm_pll_pdiv_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_dbal_hbm_pll_ch_mdiv_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_dbal_hbm_pll_frefeff_info_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_dbal_hbm_pll_aux_post_enableb_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_dbal_hbm_pll_ch_enableb_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_dbal_hbm_pll_aux_post_diffcmos_en_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_dbal_dram_bist_mode_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_dbal_hbmc_index_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_dbal_hbmc_tdu_index_nof_bits_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_dram_dbal_feature_get_f feature_get;
    
    dnx_data_dram_dbal_hbm_trc_nof_bits_get_f hbm_trc_nof_bits_get;
    
    dnx_data_dram_dbal_wpr_increment_threshold_nof_bits_get_f wpr_increment_threshold_nof_bits_get;
    
    dnx_data_dram_dbal_average_read_inflights_increment_threshold_nof_bits_get_f average_read_inflights_increment_threshold_nof_bits_get;
    
    dnx_data_dram_dbal_average_read_inflights_decrement_threshold_nof_bits_get_f average_read_inflights_decrement_threshold_nof_bits_get;
    
    dnx_data_dram_dbal_hbm_pll_pdiv_nof_bits_get_f hbm_pll_pdiv_nof_bits_get;
    
    dnx_data_dram_dbal_hbm_pll_ch_mdiv_nof_bits_get_f hbm_pll_ch_mdiv_nof_bits_get;
    
    dnx_data_dram_dbal_hbm_pll_frefeff_info_nof_bits_get_f hbm_pll_frefeff_info_nof_bits_get;
    
    dnx_data_dram_dbal_hbm_pll_aux_post_enableb_nof_bits_get_f hbm_pll_aux_post_enableb_nof_bits_get;
    
    dnx_data_dram_dbal_hbm_pll_ch_enableb_nof_bits_get_f hbm_pll_ch_enableb_nof_bits_get;
    
    dnx_data_dram_dbal_hbm_pll_aux_post_diffcmos_en_nof_bits_get_f hbm_pll_aux_post_diffcmos_en_nof_bits_get;
    
    dnx_data_dram_dbal_dram_bist_mode_nof_bits_get_f dram_bist_mode_nof_bits_get;
    
    dnx_data_dram_dbal_hbmc_index_nof_bits_get_f hbmc_index_nof_bits_get;
    
    dnx_data_dram_dbal_hbmc_tdu_index_nof_bits_get_f hbmc_tdu_index_nof_bits_get;
} dnx_data_if_dram_dbal_t;






typedef struct
{
    
    char *filename;
} dnx_data_dram_firmware_rom_t;


typedef struct
{
    
    uint32 done;
    
    uint32 active;
    
    uint32 errors_detected;
} dnx_data_dram_firmware_operation_status_t;


typedef struct
{
    
    char *description;
} dnx_data_dram_firmware_operation_result_error_code_t;


typedef struct
{
    
    char *description;
} dnx_data_dram_firmware_operation_result_last_operation_t;



typedef enum
{

    
    _dnx_data_dram_firmware_feature_nof
} dnx_data_dram_firmware_feature_e;



typedef int(
    *dnx_data_dram_firmware_feature_get_f) (
    int unit,
    dnx_data_dram_firmware_feature_e feature);


typedef uint32(
    *dnx_data_dram_firmware_nof_hbm_spare_data_results_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_firmware_sbus_clock_divider_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dram_firmware_snap_state_init_done_get_f) (
    int unit);


typedef const dnx_data_dram_firmware_rom_t *(
    *dnx_data_dram_firmware_rom_get_f) (
    int unit);


typedef const dnx_data_dram_firmware_operation_status_t *(
    *dnx_data_dram_firmware_operation_status_get_f) (
    int unit);


typedef const dnx_data_dram_firmware_operation_result_error_code_t *(
    *dnx_data_dram_firmware_operation_result_error_code_get_f) (
    int unit,
    int error_code);


typedef const dnx_data_dram_firmware_operation_result_last_operation_t *(
    *dnx_data_dram_firmware_operation_result_last_operation_get_f) (
    int unit,
    int operation_code);



typedef struct
{
    
    dnx_data_dram_firmware_feature_get_f feature_get;
    
    dnx_data_dram_firmware_nof_hbm_spare_data_results_get_f nof_hbm_spare_data_results_get;
    
    dnx_data_dram_firmware_sbus_clock_divider_get_f sbus_clock_divider_get;
    
    dnx_data_dram_firmware_snap_state_init_done_get_f snap_state_init_done_get;
    
    dnx_data_dram_firmware_rom_get_f rom_get;
    
    dnxc_data_table_info_get_f rom_info_get;
    
    dnx_data_dram_firmware_operation_status_get_f operation_status_get;
    
    dnxc_data_table_info_get_f operation_status_info_get;
    
    dnx_data_dram_firmware_operation_result_error_code_get_f operation_result_error_code_get;
    
    dnxc_data_table_info_get_f operation_result_error_code_info_get;
    
    dnx_data_dram_firmware_operation_result_last_operation_get_f operation_result_last_operation_get;
    
    dnxc_data_table_info_get_f operation_result_last_operation_info_get;
} dnx_data_if_dram_firmware_t;






typedef struct
{
    
    uint32 ndiv_int;
    
    uint32 ch1_mdiv;
} dnx_data_dram_apd_phy_pll_t;



typedef enum
{

    
    _dnx_data_dram_apd_phy_feature_nof
} dnx_data_dram_apd_phy_feature_e;



typedef int(
    *dnx_data_dram_apd_phy_feature_get_f) (
    int unit,
    dnx_data_dram_apd_phy_feature_e feature);


typedef const dnx_data_dram_apd_phy_pll_t *(
    *dnx_data_dram_apd_phy_pll_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_dram_apd_phy_feature_get_f feature_get;
    
    dnx_data_dram_apd_phy_pll_get_f pll_get;
    
    dnxc_data_table_info_get_f pll_info_get;
} dnx_data_if_dram_apd_phy_t;





typedef struct
{
    
    dnx_data_if_dram_hbm_t hbm;
    
    dnx_data_if_dram_gddr6_t gddr6;
    
    dnx_data_if_dram_general_info_t general_info;
    
    dnx_data_if_dram_address_translation_t address_translation;
    
    dnx_data_if_dram_buffers_t buffers;
    
    dnx_data_if_dram_dram_block_t dram_block;
    
    dnx_data_if_dram_dbal_t dbal;
    
    dnx_data_if_dram_firmware_t firmware;
    
    dnx_data_if_dram_apd_phy_t apd_phy;
} dnx_data_if_dram_t;




extern dnx_data_if_dram_t dnx_data_dram;


#endif 

