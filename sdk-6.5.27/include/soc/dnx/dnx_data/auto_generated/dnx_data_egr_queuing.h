
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_EGR_QUEUING_H_

#define _DNX_DATA_EGR_QUEUING_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_egr_queuing.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_egr_queuing_init(
    int unit);






typedef struct
{
    int speed;
    int if_id;
    int crdt_size;
    int irdy_thr;
    int txq_max_bytes;
    int min_gap_hp;
    int fqp_min_gap;
} dnx_data_egr_queuing_params_if_speed_params_t;


typedef struct
{
    int depth;
    int almost_full;
    int full;
} dnx_data_egr_queuing_params_emr_fifo_t;


typedef struct
{
    int max_clock_rate_th;
} dnx_data_egr_queuing_params_if_speed_params_clk_th_t;


typedef struct
{
    int delay;
} dnx_data_egr_queuing_params_fqp_dbf_additional_latency_t;



typedef enum
{
    dnx_data_egr_queuing_params_conditional_crdt_table_access,
    dnx_data_egr_queuing_params_non_j2b0_counters_are_valid,
    dnx_data_egr_queuing_params_if_attributes_profile_exist,
    dnx_data_egr_queuing_params_cos_map_is_per_pp_port,
    dnx_data_egr_queuing_params_cos_map_per_pp_or_tm_selector,
    dnx_data_egr_queuing_params_phantom_queues,
    dnx_data_egr_queuing_params_phantom_queues_prob_thresh,
    dnx_data_egr_queuing_params_almost_empty_delay,
    dnx_data_egr_queuing_params_fqp_calender_set_select,
    dnx_data_egr_queuing_params_txq_max_byte_dedicated_th,
    dnx_data_egr_queuing_params_sub_calendar,
    dnx_data_egr_queuing_params_txq_tdm_irdy_sel_exist,
    dnx_data_egr_queuing_params_total_shaper_max_burst_exist,
    dnx_data_egr_queuing_params_tcg,
    dnx_data_egr_queuing_params_tcg_mapping,
    dnx_data_egr_queuing_params_credits_above_max_burst_for_negative_comp,
    dnx_data_egr_queuing_params_txq_reduce_max_credits,
    dnx_data_egr_queuing_params_reserved_if_fc_config,
    dnx_data_egr_queuing_params_credit_init_mode,
    dnx_data_egr_queuing_params_set_select_needs_to_be_set,
    dnx_data_egr_queuing_params_shared_hp_tdm_config_is_supported,
    dnx_data_egr_queuing_params_enhanced_nif_calendar,
    dnx_data_egr_queuing_params_unicast_fifo_depth_not_supported,
    dnx_data_egr_queuing_params_wfq_mode,
    dnx_data_egr_queuing_params_rqp_max_pkt_limit,
    dnx_data_egr_queuing_params_cal_read_en_is_supported,
    dnx_data_egr_queuing_params_cal_windows_is_supported,
    dnx_data_egr_queuing_params_channel_id_mapping_is_supported,
    dnx_data_egr_queuing_params_calendar_low_range_config,
    dnx_data_egr_queuing_params_allow_aloc_ae_ifc_support,

    
    _dnx_data_egr_queuing_params_feature_nof
} dnx_data_egr_queuing_params_feature_e;



typedef int(
    *dnx_data_egr_queuing_params_feature_get_f) (
    int unit,
    dnx_data_egr_queuing_params_feature_e feature);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_bits_in_tcg_weight_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_bits_in_nof_q_pairs_in_ps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_q_pairs_in_ps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_bits_in_q_pair_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_q_pairs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_bits_in_nof_port_schedulers_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_port_schedulers_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_bits_in_channel_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_bits_in_chan_arb_calendar_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_chan_arb_calendar_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_bits_in_port_prio_calendar_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_port_prio_calendar_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_bits_in_tcg_calendar_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_tcg_calendar_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_bits_in_nof_calendars_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_calendars_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_bits_in_egr_interface_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_bits_mirror_prio_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_bits_in_egr_ch_interface_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_egr_interfaces_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_first_lp_egr_interface_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_egr_ch_interfaces_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_egr_rcy_interfaces_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_egr_olp_interfaces_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_reserved_if_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_egr_if_cpu_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_egr_if_sat_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_egr_if_oam_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_egr_if_olp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_egr_if_rcy_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_egr_if_txi_rcy_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_egr_if_eventor_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_egr_if_nif_base_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_cal_res_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_cal_res_packet_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_cal_burst_res_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_bits_in_cal_cal_len_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_cal_cal_len_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_bits_in_nif_cal_len_pqp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nif_cal_len_pqp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_bits_in_nif_cal_len_fqp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nif_cal_len_fqp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_bits_in_nof_tcg_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_tcg_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_bits_in_nof_egr_q_prio_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_egr_q_prio_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_tcg_min_priorities_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_max_credit_number_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_max_gbps_rate_egq_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_txq_iready_th_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_txq_tdm_iready_th_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_initial_packet_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_sub_calendar_ifc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_txq_max_credits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_txq_max_credits_reduce_factor_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_pqp_ifc_almost_empty_th_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_pqp_port_almost_empty_th_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_pqp_queue_almost_empty_th_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_if_min_gap_priority_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_egr_if_flexe_base_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_dedicated_flexe_interfaces_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_egr_interfaces_wo_flexe_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_egr_if_nif_alloc_start_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_egr_if_nif_alloc_end_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_egr_if_ilkn_alloc_start_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_egr_if_ilkn_alloc_end_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_egq_shaper_max_mbps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_nof_fqp_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_min_gap_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_bubble_pulse_width_for_polling_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_invalid_otm_port_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_burst_size_below_mtu_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_params_egress_bubble_delay_get_f) (
    int unit);


typedef const dnx_data_egr_queuing_params_if_speed_params_t *(
    *dnx_data_egr_queuing_params_if_speed_params_get_f) (
    int unit,
    int idx,
    int clock_rate_idx);


typedef const dnx_data_egr_queuing_params_emr_fifo_t *(
    *dnx_data_egr_queuing_params_emr_fifo_get_f) (
    int unit,
    int idx,
    int tdm);


typedef const dnx_data_egr_queuing_params_if_speed_params_clk_th_t *(
    *dnx_data_egr_queuing_params_if_speed_params_clk_th_get_f) (
    int unit,
    int idx);


typedef const dnx_data_egr_queuing_params_fqp_dbf_additional_latency_t *(
    *dnx_data_egr_queuing_params_fqp_dbf_additional_latency_get_f) (
    int unit,
    int core_index);



typedef struct
{
    
    dnx_data_egr_queuing_params_feature_get_f feature_get;
    
    dnx_data_egr_queuing_params_nof_bits_in_tcg_weight_get_f nof_bits_in_tcg_weight_get;
    
    dnx_data_egr_queuing_params_nof_bits_in_nof_q_pairs_in_ps_get_f nof_bits_in_nof_q_pairs_in_ps_get;
    
    dnx_data_egr_queuing_params_nof_q_pairs_in_ps_get_f nof_q_pairs_in_ps_get;
    
    dnx_data_egr_queuing_params_nof_bits_in_q_pair_get_f nof_bits_in_q_pair_get;
    
    dnx_data_egr_queuing_params_nof_q_pairs_get_f nof_q_pairs_get;
    
    dnx_data_egr_queuing_params_nof_bits_in_nof_port_schedulers_get_f nof_bits_in_nof_port_schedulers_get;
    
    dnx_data_egr_queuing_params_nof_port_schedulers_get_f nof_port_schedulers_get;
    
    dnx_data_egr_queuing_params_nof_bits_in_channel_get_f nof_bits_in_channel_get;
    
    dnx_data_egr_queuing_params_nof_bits_in_chan_arb_calendar_size_get_f nof_bits_in_chan_arb_calendar_size_get;
    
    dnx_data_egr_queuing_params_chan_arb_calendar_size_get_f chan_arb_calendar_size_get;
    
    dnx_data_egr_queuing_params_nof_bits_in_port_prio_calendar_size_get_f nof_bits_in_port_prio_calendar_size_get;
    
    dnx_data_egr_queuing_params_port_prio_calendar_size_get_f port_prio_calendar_size_get;
    
    dnx_data_egr_queuing_params_nof_bits_in_tcg_calendar_size_get_f nof_bits_in_tcg_calendar_size_get;
    
    dnx_data_egr_queuing_params_tcg_calendar_size_get_f tcg_calendar_size_get;
    
    dnx_data_egr_queuing_params_nof_bits_in_nof_calendars_get_f nof_bits_in_nof_calendars_get;
    
    dnx_data_egr_queuing_params_nof_calendars_get_f nof_calendars_get;
    
    dnx_data_egr_queuing_params_nof_bits_in_egr_interface_get_f nof_bits_in_egr_interface_get;
    
    dnx_data_egr_queuing_params_nof_bits_mirror_prio_get_f nof_bits_mirror_prio_get;
    
    dnx_data_egr_queuing_params_nof_bits_in_egr_ch_interface_get_f nof_bits_in_egr_ch_interface_get;
    
    dnx_data_egr_queuing_params_nof_egr_interfaces_get_f nof_egr_interfaces_get;
    
    dnx_data_egr_queuing_params_first_lp_egr_interface_get_f first_lp_egr_interface_get;
    
    dnx_data_egr_queuing_params_nof_egr_ch_interfaces_get_f nof_egr_ch_interfaces_get;
    
    dnx_data_egr_queuing_params_nof_egr_rcy_interfaces_get_f nof_egr_rcy_interfaces_get;
    
    dnx_data_egr_queuing_params_nof_egr_olp_interfaces_get_f nof_egr_olp_interfaces_get;
    
    dnx_data_egr_queuing_params_reserved_if_get_f reserved_if_get;
    
    dnx_data_egr_queuing_params_egr_if_cpu_get_f egr_if_cpu_get;
    
    dnx_data_egr_queuing_params_egr_if_sat_get_f egr_if_sat_get;
    
    dnx_data_egr_queuing_params_egr_if_oam_get_f egr_if_oam_get;
    
    dnx_data_egr_queuing_params_egr_if_olp_get_f egr_if_olp_get;
    
    dnx_data_egr_queuing_params_egr_if_rcy_get_f egr_if_rcy_get;
    
    dnx_data_egr_queuing_params_egr_if_txi_rcy_get_f egr_if_txi_rcy_get;
    
    dnx_data_egr_queuing_params_egr_if_eventor_get_f egr_if_eventor_get;
    
    dnx_data_egr_queuing_params_egr_if_nif_base_get_f egr_if_nif_base_get;
    
    dnx_data_egr_queuing_params_cal_res_get_f cal_res_get;
    
    dnx_data_egr_queuing_params_cal_res_packet_mode_get_f cal_res_packet_mode_get;
    
    dnx_data_egr_queuing_params_cal_burst_res_get_f cal_burst_res_get;
    
    dnx_data_egr_queuing_params_nof_bits_in_cal_cal_len_get_f nof_bits_in_cal_cal_len_get;
    
    dnx_data_egr_queuing_params_cal_cal_len_get_f cal_cal_len_get;
    
    dnx_data_egr_queuing_params_nof_bits_in_nif_cal_len_pqp_get_f nof_bits_in_nif_cal_len_pqp_get;
    
    dnx_data_egr_queuing_params_nif_cal_len_pqp_get_f nif_cal_len_pqp_get;
    
    dnx_data_egr_queuing_params_nof_bits_in_nif_cal_len_fqp_get_f nof_bits_in_nif_cal_len_fqp_get;
    
    dnx_data_egr_queuing_params_nif_cal_len_fqp_get_f nif_cal_len_fqp_get;
    
    dnx_data_egr_queuing_params_nof_bits_in_nof_tcg_get_f nof_bits_in_nof_tcg_get;
    
    dnx_data_egr_queuing_params_nof_tcg_get_f nof_tcg_get;
    
    dnx_data_egr_queuing_params_nof_bits_in_nof_egr_q_prio_get_f nof_bits_in_nof_egr_q_prio_get;
    
    dnx_data_egr_queuing_params_nof_egr_q_prio_get_f nof_egr_q_prio_get;
    
    dnx_data_egr_queuing_params_tcg_min_priorities_get_f tcg_min_priorities_get;
    
    dnx_data_egr_queuing_params_max_credit_number_get_f max_credit_number_get;
    
    dnx_data_egr_queuing_params_max_gbps_rate_egq_get_f max_gbps_rate_egq_get;
    
    dnx_data_egr_queuing_params_txq_iready_th_get_f txq_iready_th_get;
    
    dnx_data_egr_queuing_params_txq_tdm_iready_th_get_f txq_tdm_iready_th_get;
    
    dnx_data_egr_queuing_params_initial_packet_mode_get_f initial_packet_mode_get;
    
    dnx_data_egr_queuing_params_sub_calendar_ifc_get_f sub_calendar_ifc_get;
    
    dnx_data_egr_queuing_params_txq_max_credits_get_f txq_max_credits_get;
    
    dnx_data_egr_queuing_params_txq_max_credits_reduce_factor_get_f txq_max_credits_reduce_factor_get;
    
    dnx_data_egr_queuing_params_pqp_ifc_almost_empty_th_get_f pqp_ifc_almost_empty_th_get;
    
    dnx_data_egr_queuing_params_pqp_port_almost_empty_th_get_f pqp_port_almost_empty_th_get;
    
    dnx_data_egr_queuing_params_pqp_queue_almost_empty_th_get_f pqp_queue_almost_empty_th_get;
    
    dnx_data_egr_queuing_params_if_min_gap_priority_size_get_f if_min_gap_priority_size_get;
    
    dnx_data_egr_queuing_params_egr_if_flexe_base_get_f egr_if_flexe_base_get;
    
    dnx_data_egr_queuing_params_nof_dedicated_flexe_interfaces_get_f nof_dedicated_flexe_interfaces_get;
    
    dnx_data_egr_queuing_params_nof_egr_interfaces_wo_flexe_get_f nof_egr_interfaces_wo_flexe_get;
    
    dnx_data_egr_queuing_params_egr_if_nif_alloc_start_get_f egr_if_nif_alloc_start_get;
    
    dnx_data_egr_queuing_params_egr_if_nif_alloc_end_get_f egr_if_nif_alloc_end_get;
    
    dnx_data_egr_queuing_params_egr_if_ilkn_alloc_start_get_f egr_if_ilkn_alloc_start_get;
    
    dnx_data_egr_queuing_params_egr_if_ilkn_alloc_end_get_f egr_if_ilkn_alloc_end_get;
    
    dnx_data_egr_queuing_params_egq_shaper_max_mbps_get_f egq_shaper_max_mbps_get;
    
    dnx_data_egr_queuing_params_nof_fqp_profiles_get_f nof_fqp_profiles_get;
    
    dnx_data_egr_queuing_params_min_gap_bits_get_f min_gap_bits_get;
    
    dnx_data_egr_queuing_params_bubble_pulse_width_for_polling_get_f bubble_pulse_width_for_polling_get;
    
    dnx_data_egr_queuing_params_invalid_otm_port_get_f invalid_otm_port_get;
    
    dnx_data_egr_queuing_params_burst_size_below_mtu_get_f burst_size_below_mtu_get;
    
    dnx_data_egr_queuing_params_egress_bubble_delay_get_f egress_bubble_delay_get;
    
    dnx_data_egr_queuing_params_if_speed_params_get_f if_speed_params_get;
    
    dnxc_data_table_info_get_f if_speed_params_info_get;
    
    dnx_data_egr_queuing_params_emr_fifo_get_f emr_fifo_get;
    
    dnxc_data_table_info_get_f emr_fifo_info_get;
    
    dnx_data_egr_queuing_params_if_speed_params_clk_th_get_f if_speed_params_clk_th_get;
    
    dnxc_data_table_info_get_f if_speed_params_clk_th_info_get;
    
    dnx_data_egr_queuing_params_fqp_dbf_additional_latency_get_f fqp_dbf_additional_latency_get;
    
    dnxc_data_table_info_get_f fqp_dbf_additional_latency_info_get;
} dnx_data_if_egr_queuing_params_t;







typedef enum
{

    
    _dnx_data_egr_queuing_common_max_val_feature_nof
} dnx_data_egr_queuing_common_max_val_feature_e;



typedef int(
    *dnx_data_egr_queuing_common_max_val_feature_get_f) (
    int unit,
    dnx_data_egr_queuing_common_max_val_feature_e feature);


typedef uint32(
    *dnx_data_egr_queuing_common_max_val_nof_bits_in_calendar_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_common_max_val_calendar_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_common_max_val_slow_port_speed_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_common_max_val_mux_calendar_resolution_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_egr_queuing_common_max_val_feature_get_f feature_get;
    
    dnx_data_egr_queuing_common_max_val_nof_bits_in_calendar_size_get_f nof_bits_in_calendar_size_get;
    
    dnx_data_egr_queuing_common_max_val_calendar_size_get_f calendar_size_get;
    
    dnx_data_egr_queuing_common_max_val_slow_port_speed_get_f slow_port_speed_get;
    
    dnx_data_egr_queuing_common_max_val_mux_calendar_resolution_get_f mux_calendar_resolution_get;
} dnx_data_if_egr_queuing_common_max_val_t;







typedef enum
{
    dnx_data_egr_queuing_rate_measurement_is_supported,

    
    _dnx_data_egr_queuing_rate_measurement_feature_nof
} dnx_data_egr_queuing_rate_measurement_feature_e;



typedef int(
    *dnx_data_egr_queuing_rate_measurement_feature_get_f) (
    int unit,
    dnx_data_egr_queuing_rate_measurement_feature_e feature);


typedef uint32(
    *dnx_data_egr_queuing_rate_measurement_max_interval_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_rate_measurement_promile_factor_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_rate_measurement_max_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_rate_measurement_min_bytes_for_granularity_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_egr_queuing_rate_measurement_feature_get_f feature_get;
    
    dnx_data_egr_queuing_rate_measurement_max_interval_get_f max_interval_get;
    
    dnx_data_egr_queuing_rate_measurement_promile_factor_get_f promile_factor_get;
    
    dnx_data_egr_queuing_rate_measurement_max_granularity_get_f max_granularity_get;
    
    dnx_data_egr_queuing_rate_measurement_min_bytes_for_granularity_get_f min_bytes_for_granularity_get;
} dnx_data_if_egr_queuing_rate_measurement_t;







typedef enum
{

    
    _dnx_data_egr_queuing_phantom_queues_feature_nof
} dnx_data_egr_queuing_phantom_queues_feature_e;



typedef int(
    *dnx_data_egr_queuing_phantom_queues_feature_get_f) (
    int unit,
    dnx_data_egr_queuing_phantom_queues_feature_e feature);


typedef uint32(
    *dnx_data_egr_queuing_phantom_queues_nof_threshold_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_egr_queuing_phantom_queues_max_threshold_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_egr_queuing_phantom_queues_feature_get_f feature_get;
    
    dnx_data_egr_queuing_phantom_queues_nof_threshold_profiles_get_f nof_threshold_profiles_get;
    
    dnx_data_egr_queuing_phantom_queues_max_threshold_get_f max_threshold_get;
} dnx_data_if_egr_queuing_phantom_queues_t;







typedef enum
{
    dnx_data_egr_queuing_reassembly_prs,

    
    _dnx_data_egr_queuing_reassembly_feature_nof
} dnx_data_egr_queuing_reassembly_feature_e;



typedef int(
    *dnx_data_egr_queuing_reassembly_feature_get_f) (
    int unit,
    dnx_data_egr_queuing_reassembly_feature_e feature);



typedef struct
{
    
    dnx_data_egr_queuing_reassembly_feature_get_f feature_get;
} dnx_data_if_egr_queuing_reassembly_t;







typedef enum
{
    dnx_data_egr_queuing_general_is_used,
    dnx_data_egr_queuing_general_power_down_supported,

    
    _dnx_data_egr_queuing_general_feature_nof
} dnx_data_egr_queuing_general_feature_e;



typedef int(
    *dnx_data_egr_queuing_general_feature_get_f) (
    int unit,
    dnx_data_egr_queuing_general_feature_e feature);



typedef struct
{
    
    dnx_data_egr_queuing_general_feature_get_f feature_get;
} dnx_data_if_egr_queuing_general_t;





typedef struct
{
    
    dnx_data_if_egr_queuing_params_t params;
    
    dnx_data_if_egr_queuing_common_max_val_t common_max_val;
    
    dnx_data_if_egr_queuing_rate_measurement_t rate_measurement;
    
    dnx_data_if_egr_queuing_phantom_queues_t phantom_queues;
    
    dnx_data_if_egr_queuing_reassembly_t reassembly;
    
    dnx_data_if_egr_queuing_general_t general;
} dnx_data_if_egr_queuing_t;




extern dnx_data_if_egr_queuing_t dnx_data_egr_queuing;


#endif 

