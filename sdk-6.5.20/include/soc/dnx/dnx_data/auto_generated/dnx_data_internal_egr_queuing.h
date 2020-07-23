

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_EGR_QUEUING_H_

#define _DNX_DATA_INTERNAL_EGR_QUEUING_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_egr_queuing_submodule_params,
    dnx_data_egr_queuing_submodule_common_max_val,
    dnx_data_egr_queuing_submodule_rate_measurement,
    dnx_data_egr_queuing_submodule_phantom_queues,
    dnx_data_egr_queuing_submodule_reassembly,

    
    _dnx_data_egr_queuing_submodule_nof
} dnx_data_egr_queuing_submodule_e;








int dnx_data_egr_queuing_params_feature_get(
    int unit,
    dnx_data_egr_queuing_params_feature_e feature);



typedef enum
{
    dnx_data_egr_queuing_params_define_nof_bits_in_tcg_weight,
    dnx_data_egr_queuing_params_define_nof_bits_in_nof_q_pairs_in_ps,
    dnx_data_egr_queuing_params_define_nof_q_pairs_in_ps,
    dnx_data_egr_queuing_params_define_nof_bits_in_q_pair,
    dnx_data_egr_queuing_params_define_nof_q_pairs,
    dnx_data_egr_queuing_params_define_nof_bits_in_nof_port_schedulers,
    dnx_data_egr_queuing_params_define_nof_port_schedulers,
    dnx_data_egr_queuing_params_define_nof_bits_in_chan_arb_calendar_size,
    dnx_data_egr_queuing_params_define_chan_arb_calendar_size,
    dnx_data_egr_queuing_params_define_nof_bits_in_port_prio_calendar_size,
    dnx_data_egr_queuing_params_define_port_prio_calendar_size,
    dnx_data_egr_queuing_params_define_nof_bits_in_tcg_calendar_size,
    dnx_data_egr_queuing_params_define_tcg_calendar_size,
    dnx_data_egr_queuing_params_define_nof_bits_in_nof_calendars,
    dnx_data_egr_queuing_params_define_nof_calendars,
    dnx_data_egr_queuing_params_define_nof_bits_in_egr_interface,
    dnx_data_egr_queuing_params_define_nof_bits_mirror_prio,
    dnx_data_egr_queuing_params_define_nof_bits_in_egr_ch_interface,
    dnx_data_egr_queuing_params_define_nof_egr_interfaces,
    dnx_data_egr_queuing_params_define_nof_egr_ch_interfaces,
    dnx_data_egr_queuing_params_define_reserved_if,
    dnx_data_egr_queuing_params_define_egr_if_cpu,
    dnx_data_egr_queuing_params_define_egr_if_sat,
    dnx_data_egr_queuing_params_define_egr_if_oam,
    dnx_data_egr_queuing_params_define_egr_if_olp,
    dnx_data_egr_queuing_params_define_egr_if_rcy,
    dnx_data_egr_queuing_params_define_egr_if_txi_rcy,
    dnx_data_egr_queuing_params_define_egr_if_eventor,
    dnx_data_egr_queuing_params_define_egr_if_nif_base,
    dnx_data_egr_queuing_params_define_cal_res,
    dnx_data_egr_queuing_params_define_cal_res_packet_mode,
    dnx_data_egr_queuing_params_define_cal_burst_res,
    dnx_data_egr_queuing_params_define_nof_bits_in_cal_cal_len,
    dnx_data_egr_queuing_params_define_cal_cal_len,
    dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_pqp,
    dnx_data_egr_queuing_params_define_nif_cal_len_pqp,
    dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_fqp,
    dnx_data_egr_queuing_params_define_nif_cal_len_fqp,
    dnx_data_egr_queuing_params_define_nof_bits_in_nof_tcg,
    dnx_data_egr_queuing_params_define_nof_tcg,
    dnx_data_egr_queuing_params_define_nof_bits_in_nof_egr_q_prio,
    dnx_data_egr_queuing_params_define_nof_egr_q_prio,
    dnx_data_egr_queuing_params_define_tcg_min_priorities,
    dnx_data_egr_queuing_params_define_max_credit_number,
    dnx_data_egr_queuing_params_define_max_gbps_rate_egq,
    dnx_data_egr_queuing_params_define_txq_iready_th,
    dnx_data_egr_queuing_params_define_txq_tdm_iready_th,
    dnx_data_egr_queuing_params_define_initial_packet_mode,
    dnx_data_egr_queuing_params_define_sub_calendar_ifc,
    dnx_data_egr_queuing_params_define_txq_max_credits,
    dnx_data_egr_queuing_params_define_pqp_ifc_almost_empty_th,
    dnx_data_egr_queuing_params_define_pqp_port_almost_empty_th,
    dnx_data_egr_queuing_params_define_pqp_queue_almost_empty_th,
    dnx_data_egr_queuing_params_define_if_min_gap_priority_size,
    dnx_data_egr_queuing_params_define_invalid_otm_port,
    dnx_data_egr_queuing_params_define_burst_size_below_mtu,

    
    _dnx_data_egr_queuing_params_define_nof
} dnx_data_egr_queuing_params_define_e;



uint32 dnx_data_egr_queuing_params_nof_bits_in_tcg_weight_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_bits_in_nof_q_pairs_in_ps_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_q_pairs_in_ps_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_bits_in_q_pair_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_q_pairs_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_bits_in_nof_port_schedulers_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_port_schedulers_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_bits_in_chan_arb_calendar_size_get(
    int unit);


uint32 dnx_data_egr_queuing_params_chan_arb_calendar_size_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_bits_in_port_prio_calendar_size_get(
    int unit);


uint32 dnx_data_egr_queuing_params_port_prio_calendar_size_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_bits_in_tcg_calendar_size_get(
    int unit);


uint32 dnx_data_egr_queuing_params_tcg_calendar_size_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_bits_in_nof_calendars_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_calendars_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_bits_in_egr_interface_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_bits_mirror_prio_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_bits_in_egr_ch_interface_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_egr_interfaces_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_egr_ch_interfaces_get(
    int unit);


uint32 dnx_data_egr_queuing_params_reserved_if_get(
    int unit);


uint32 dnx_data_egr_queuing_params_egr_if_cpu_get(
    int unit);


uint32 dnx_data_egr_queuing_params_egr_if_sat_get(
    int unit);


uint32 dnx_data_egr_queuing_params_egr_if_oam_get(
    int unit);


uint32 dnx_data_egr_queuing_params_egr_if_olp_get(
    int unit);


uint32 dnx_data_egr_queuing_params_egr_if_rcy_get(
    int unit);


uint32 dnx_data_egr_queuing_params_egr_if_txi_rcy_get(
    int unit);


uint32 dnx_data_egr_queuing_params_egr_if_eventor_get(
    int unit);


uint32 dnx_data_egr_queuing_params_egr_if_nif_base_get(
    int unit);


uint32 dnx_data_egr_queuing_params_cal_res_get(
    int unit);


uint32 dnx_data_egr_queuing_params_cal_res_packet_mode_get(
    int unit);


uint32 dnx_data_egr_queuing_params_cal_burst_res_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_bits_in_cal_cal_len_get(
    int unit);


uint32 dnx_data_egr_queuing_params_cal_cal_len_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_bits_in_nif_cal_len_pqp_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nif_cal_len_pqp_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_bits_in_nif_cal_len_fqp_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nif_cal_len_fqp_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_bits_in_nof_tcg_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_tcg_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_bits_in_nof_egr_q_prio_get(
    int unit);


uint32 dnx_data_egr_queuing_params_nof_egr_q_prio_get(
    int unit);


uint32 dnx_data_egr_queuing_params_tcg_min_priorities_get(
    int unit);


uint32 dnx_data_egr_queuing_params_max_credit_number_get(
    int unit);


uint32 dnx_data_egr_queuing_params_max_gbps_rate_egq_get(
    int unit);


uint32 dnx_data_egr_queuing_params_txq_iready_th_get(
    int unit);


uint32 dnx_data_egr_queuing_params_txq_tdm_iready_th_get(
    int unit);


uint32 dnx_data_egr_queuing_params_initial_packet_mode_get(
    int unit);


uint32 dnx_data_egr_queuing_params_sub_calendar_ifc_get(
    int unit);


uint32 dnx_data_egr_queuing_params_txq_max_credits_get(
    int unit);


uint32 dnx_data_egr_queuing_params_pqp_ifc_almost_empty_th_get(
    int unit);


uint32 dnx_data_egr_queuing_params_pqp_port_almost_empty_th_get(
    int unit);


uint32 dnx_data_egr_queuing_params_pqp_queue_almost_empty_th_get(
    int unit);


uint32 dnx_data_egr_queuing_params_if_min_gap_priority_size_get(
    int unit);


uint32 dnx_data_egr_queuing_params_invalid_otm_port_get(
    int unit);


uint32 dnx_data_egr_queuing_params_burst_size_below_mtu_get(
    int unit);



typedef enum
{
    dnx_data_egr_queuing_params_table_if_speed_params,
    dnx_data_egr_queuing_params_table_emr_fifo,
    dnx_data_egr_queuing_params_table_if_speed_params_clk_th,

    
    _dnx_data_egr_queuing_params_table_nof
} dnx_data_egr_queuing_params_table_e;



const dnx_data_egr_queuing_params_if_speed_params_t * dnx_data_egr_queuing_params_if_speed_params_get(
    int unit,
    int idx,
    int clock_rate_idx);


const dnx_data_egr_queuing_params_emr_fifo_t * dnx_data_egr_queuing_params_emr_fifo_get(
    int unit,
    int idx,
    int tdm);


const dnx_data_egr_queuing_params_if_speed_params_clk_th_t * dnx_data_egr_queuing_params_if_speed_params_clk_th_get(
    int unit,
    int idx);



shr_error_e dnx_data_egr_queuing_params_if_speed_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_egr_queuing_params_emr_fifo_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_egr_queuing_params_if_speed_params_clk_th_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_egr_queuing_params_if_speed_params_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_egr_queuing_params_emr_fifo_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_egr_queuing_params_if_speed_params_clk_th_info_get(
    int unit);






int dnx_data_egr_queuing_common_max_val_feature_get(
    int unit,
    dnx_data_egr_queuing_common_max_val_feature_e feature);



typedef enum
{
    dnx_data_egr_queuing_common_max_val_define_nof_bits_in_calendar_size,
    dnx_data_egr_queuing_common_max_val_define_calendar_size,
    dnx_data_egr_queuing_common_max_val_define_slow_port_speed,
    dnx_data_egr_queuing_common_max_val_define_mux_calendar_resolution,

    
    _dnx_data_egr_queuing_common_max_val_define_nof
} dnx_data_egr_queuing_common_max_val_define_e;



uint32 dnx_data_egr_queuing_common_max_val_nof_bits_in_calendar_size_get(
    int unit);


uint32 dnx_data_egr_queuing_common_max_val_calendar_size_get(
    int unit);


uint32 dnx_data_egr_queuing_common_max_val_slow_port_speed_get(
    int unit);


uint32 dnx_data_egr_queuing_common_max_val_mux_calendar_resolution_get(
    int unit);



typedef enum
{

    
    _dnx_data_egr_queuing_common_max_val_table_nof
} dnx_data_egr_queuing_common_max_val_table_e;









int dnx_data_egr_queuing_rate_measurement_feature_get(
    int unit,
    dnx_data_egr_queuing_rate_measurement_feature_e feature);



typedef enum
{
    dnx_data_egr_queuing_rate_measurement_define_max_interval,
    dnx_data_egr_queuing_rate_measurement_define_promile_factor,
    dnx_data_egr_queuing_rate_measurement_define_max_granularity,
    dnx_data_egr_queuing_rate_measurement_define_min_bytes_for_granularity,

    
    _dnx_data_egr_queuing_rate_measurement_define_nof
} dnx_data_egr_queuing_rate_measurement_define_e;



uint32 dnx_data_egr_queuing_rate_measurement_max_interval_get(
    int unit);


uint32 dnx_data_egr_queuing_rate_measurement_promile_factor_get(
    int unit);


uint32 dnx_data_egr_queuing_rate_measurement_max_granularity_get(
    int unit);


uint32 dnx_data_egr_queuing_rate_measurement_min_bytes_for_granularity_get(
    int unit);



typedef enum
{

    
    _dnx_data_egr_queuing_rate_measurement_table_nof
} dnx_data_egr_queuing_rate_measurement_table_e;









int dnx_data_egr_queuing_phantom_queues_feature_get(
    int unit,
    dnx_data_egr_queuing_phantom_queues_feature_e feature);



typedef enum
{
    dnx_data_egr_queuing_phantom_queues_define_nof_threshold_profiles,
    dnx_data_egr_queuing_phantom_queues_define_max_threshold,

    
    _dnx_data_egr_queuing_phantom_queues_define_nof
} dnx_data_egr_queuing_phantom_queues_define_e;



uint32 dnx_data_egr_queuing_phantom_queues_nof_threshold_profiles_get(
    int unit);


uint32 dnx_data_egr_queuing_phantom_queues_max_threshold_get(
    int unit);



typedef enum
{

    
    _dnx_data_egr_queuing_phantom_queues_table_nof
} dnx_data_egr_queuing_phantom_queues_table_e;









int dnx_data_egr_queuing_reassembly_feature_get(
    int unit,
    dnx_data_egr_queuing_reassembly_feature_e feature);



typedef enum
{

    
    _dnx_data_egr_queuing_reassembly_define_nof
} dnx_data_egr_queuing_reassembly_define_e;




typedef enum
{

    
    _dnx_data_egr_queuing_reassembly_table_nof
} dnx_data_egr_queuing_reassembly_table_e;






shr_error_e dnx_data_egr_queuing_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

