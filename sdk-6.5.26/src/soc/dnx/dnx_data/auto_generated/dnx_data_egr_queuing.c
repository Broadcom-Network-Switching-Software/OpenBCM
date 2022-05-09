
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_EGRESSDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_egr_queuing.h>




extern shr_error_e jr2_a0_data_egr_queuing_attach(
    int unit);


extern shr_error_e jr2_b0_data_egr_queuing_attach(
    int unit);


extern shr_error_e j2c_a0_data_egr_queuing_attach(
    int unit);


extern shr_error_e q2a_a0_data_egr_queuing_attach(
    int unit);


extern shr_error_e q2a_b1_data_egr_queuing_attach(
    int unit);


extern shr_error_e j2p_a0_data_egr_queuing_attach(
    int unit);


extern shr_error_e j2p_a2_data_egr_queuing_attach(
    int unit);


extern shr_error_e j2x_a0_data_egr_queuing_attach(
    int unit);




static shr_error_e
dnx_data_egr_queuing_params_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "params";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_egr_queuing_params_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data egr_queuing params features");

    submodule_data->features[dnx_data_egr_queuing_params_conditional_crdt_table_access].name = "conditional_crdt_table_access";
    submodule_data->features[dnx_data_egr_queuing_params_conditional_crdt_table_access].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_conditional_crdt_table_access].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_non_j2b0_counters_are_valid].name = "non_j2b0_counters_are_valid";
    submodule_data->features[dnx_data_egr_queuing_params_non_j2b0_counters_are_valid].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_non_j2b0_counters_are_valid].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_if_attributes_profile_exist].name = "if_attributes_profile_exist";
    submodule_data->features[dnx_data_egr_queuing_params_if_attributes_profile_exist].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_if_attributes_profile_exist].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_cos_map_is_per_pp_port].name = "cos_map_is_per_pp_port";
    submodule_data->features[dnx_data_egr_queuing_params_cos_map_is_per_pp_port].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_cos_map_is_per_pp_port].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_cos_map_per_pp_or_tm_selector].name = "cos_map_per_pp_or_tm_selector";
    submodule_data->features[dnx_data_egr_queuing_params_cos_map_per_pp_or_tm_selector].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_cos_map_per_pp_or_tm_selector].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_phantom_queues].name = "phantom_queues";
    submodule_data->features[dnx_data_egr_queuing_params_phantom_queues].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_phantom_queues].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_phantom_queues_prob_thresh].name = "phantom_queues_prob_thresh";
    submodule_data->features[dnx_data_egr_queuing_params_phantom_queues_prob_thresh].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_phantom_queues_prob_thresh].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_almost_empty_delay].name = "almost_empty_delay";
    submodule_data->features[dnx_data_egr_queuing_params_almost_empty_delay].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_almost_empty_delay].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_fqp_calender_set_select].name = "fqp_calender_set_select";
    submodule_data->features[dnx_data_egr_queuing_params_fqp_calender_set_select].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_fqp_calender_set_select].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_txq_max_byte_dedicated_th].name = "txq_max_byte_dedicated_th";
    submodule_data->features[dnx_data_egr_queuing_params_txq_max_byte_dedicated_th].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_txq_max_byte_dedicated_th].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_sub_calendar].name = "sub_calendar";
    submodule_data->features[dnx_data_egr_queuing_params_sub_calendar].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_sub_calendar].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_txq_tdm_irdy_sel_exist].name = "txq_tdm_irdy_sel_exist";
    submodule_data->features[dnx_data_egr_queuing_params_txq_tdm_irdy_sel_exist].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_txq_tdm_irdy_sel_exist].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_total_shaper_max_burst_exist].name = "total_shaper_max_burst_exist";
    submodule_data->features[dnx_data_egr_queuing_params_total_shaper_max_burst_exist].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_total_shaper_max_burst_exist].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_tcg].name = "tcg";
    submodule_data->features[dnx_data_egr_queuing_params_tcg].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_tcg].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_tcg_mapping].name = "tcg_mapping";
    submodule_data->features[dnx_data_egr_queuing_params_tcg_mapping].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_tcg_mapping].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_const_rqp_fqp_bubbles_disabled].name = "const_rqp_fqp_bubbles_disabled";
    submodule_data->features[dnx_data_egr_queuing_params_const_rqp_fqp_bubbles_disabled].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_const_rqp_fqp_bubbles_disabled].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_credits_above_max_burst_for_negative_comp].name = "credits_above_max_burst_for_negative_comp";
    submodule_data->features[dnx_data_egr_queuing_params_credits_above_max_burst_for_negative_comp].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_credits_above_max_burst_for_negative_comp].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_txq_reduce_max_credits].name = "txq_reduce_max_credits";
    submodule_data->features[dnx_data_egr_queuing_params_txq_reduce_max_credits].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_txq_reduce_max_credits].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_reserved_if_fc_config].name = "reserved_if_fc_config";
    submodule_data->features[dnx_data_egr_queuing_params_reserved_if_fc_config].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_reserved_if_fc_config].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_credit_init_mode].name = "credit_init_mode";
    submodule_data->features[dnx_data_egr_queuing_params_credit_init_mode].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_credit_init_mode].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_set_select_needs_to_be_set].name = "set_select_needs_to_be_set";
    submodule_data->features[dnx_data_egr_queuing_params_set_select_needs_to_be_set].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_set_select_needs_to_be_set].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_shared_hp_tdm_config_is_supported].name = "shared_hp_tdm_config_is_supported";
    submodule_data->features[dnx_data_egr_queuing_params_shared_hp_tdm_config_is_supported].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_shared_hp_tdm_config_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_enhanced_nif_calendar].name = "enhanced_nif_calendar";
    submodule_data->features[dnx_data_egr_queuing_params_enhanced_nif_calendar].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_enhanced_nif_calendar].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_unicast_fifo_depth_not_supported].name = "unicast_fifo_depth_not_supported";
    submodule_data->features[dnx_data_egr_queuing_params_unicast_fifo_depth_not_supported].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_unicast_fifo_depth_not_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_wfq_mode].name = "wfq_mode";
    submodule_data->features[dnx_data_egr_queuing_params_wfq_mode].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_wfq_mode].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_rqp_max_pkt_limit].name = "rqp_max_pkt_limit";
    submodule_data->features[dnx_data_egr_queuing_params_rqp_max_pkt_limit].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_rqp_max_pkt_limit].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_cal_read_en_is_supported].name = "cal_read_en_is_supported";
    submodule_data->features[dnx_data_egr_queuing_params_cal_read_en_is_supported].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_cal_read_en_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_cal_windows_is_supported].name = "cal_windows_is_supported";
    submodule_data->features[dnx_data_egr_queuing_params_cal_windows_is_supported].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_cal_windows_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_channel_id_mapping_is_supported].name = "channel_id_mapping_is_supported";
    submodule_data->features[dnx_data_egr_queuing_params_channel_id_mapping_is_supported].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_channel_id_mapping_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_calendar_low_range_config].name = "calendar_low_range_config";
    submodule_data->features[dnx_data_egr_queuing_params_calendar_low_range_config].doc = "";
    submodule_data->features[dnx_data_egr_queuing_params_calendar_low_range_config].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_egr_queuing_params_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data egr_queuing params defines");

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_tcg_weight].name = "nof_bits_in_tcg_weight";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_tcg_weight].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_tcg_weight].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_q_pairs_in_ps].name = "nof_bits_in_nof_q_pairs_in_ps";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_q_pairs_in_ps].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_q_pairs_in_ps].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_q_pairs_in_ps].name = "nof_q_pairs_in_ps";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_q_pairs_in_ps].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_q_pairs_in_ps].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_q_pair].name = "nof_bits_in_q_pair";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_q_pair].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_q_pair].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_q_pairs].name = "nof_q_pairs";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_q_pairs].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_q_pairs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_port_schedulers].name = "nof_bits_in_nof_port_schedulers";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_port_schedulers].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_port_schedulers].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_port_schedulers].name = "nof_port_schedulers";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_port_schedulers].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_port_schedulers].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_channel].name = "nof_bits_in_channel";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_channel].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_channel].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_chan_arb_calendar_size].name = "nof_bits_in_chan_arb_calendar_size";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_chan_arb_calendar_size].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_chan_arb_calendar_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_chan_arb_calendar_size].name = "chan_arb_calendar_size";
    submodule_data->defines[dnx_data_egr_queuing_params_define_chan_arb_calendar_size].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_chan_arb_calendar_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_port_prio_calendar_size].name = "nof_bits_in_port_prio_calendar_size";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_port_prio_calendar_size].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_port_prio_calendar_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_port_prio_calendar_size].name = "port_prio_calendar_size";
    submodule_data->defines[dnx_data_egr_queuing_params_define_port_prio_calendar_size].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_port_prio_calendar_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_tcg_calendar_size].name = "nof_bits_in_tcg_calendar_size";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_tcg_calendar_size].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_tcg_calendar_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_tcg_calendar_size].name = "tcg_calendar_size";
    submodule_data->defines[dnx_data_egr_queuing_params_define_tcg_calendar_size].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_tcg_calendar_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_calendars].name = "nof_bits_in_nof_calendars";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_calendars].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_calendars].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_calendars].name = "nof_calendars";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_calendars].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_calendars].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_egr_interface].name = "nof_bits_in_egr_interface";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_egr_interface].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_egr_interface].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_mirror_prio].name = "nof_bits_mirror_prio";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_mirror_prio].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_mirror_prio].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_egr_ch_interface].name = "nof_bits_in_egr_ch_interface";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_egr_ch_interface].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_egr_ch_interface].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_interfaces].name = "nof_egr_interfaces";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_interfaces].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_interfaces].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_first_lp_egr_interface].name = "first_lp_egr_interface";
    submodule_data->defines[dnx_data_egr_queuing_params_define_first_lp_egr_interface].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_first_lp_egr_interface].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_ch_interfaces].name = "nof_egr_ch_interfaces";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_ch_interfaces].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_ch_interfaces].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_rcy_interfaces].name = "nof_egr_rcy_interfaces";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_rcy_interfaces].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_rcy_interfaces].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_olp_interfaces].name = "nof_egr_olp_interfaces";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_olp_interfaces].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_olp_interfaces].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_reserved_if].name = "reserved_if";
    submodule_data->defines[dnx_data_egr_queuing_params_define_reserved_if].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_reserved_if].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_cpu].name = "egr_if_cpu";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_cpu].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_cpu].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_sat].name = "egr_if_sat";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_sat].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_sat].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_oam].name = "egr_if_oam";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_oam].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_oam].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_olp].name = "egr_if_olp";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_olp].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_olp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_rcy].name = "egr_if_rcy";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_rcy].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_rcy].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_txi_rcy].name = "egr_if_txi_rcy";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_txi_rcy].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_txi_rcy].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_eventor].name = "egr_if_eventor";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_eventor].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_eventor].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_nif_base].name = "egr_if_nif_base";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_nif_base].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_nif_base].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_res].name = "cal_res";
    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_res].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_res].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_res_packet_mode].name = "cal_res_packet_mode";
    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_res_packet_mode].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_res_packet_mode].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_burst_res].name = "cal_burst_res";
    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_burst_res].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_burst_res].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_cal_cal_len].name = "nof_bits_in_cal_cal_len";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_cal_cal_len].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_cal_cal_len].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_cal_len].name = "cal_cal_len";
    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_cal_len].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_cal_len].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_pqp].name = "nof_bits_in_nif_cal_len_pqp";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_pqp].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_pqp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nif_cal_len_pqp].name = "nif_cal_len_pqp";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nif_cal_len_pqp].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nif_cal_len_pqp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_fqp].name = "nof_bits_in_nif_cal_len_fqp";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_fqp].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_fqp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nif_cal_len_fqp].name = "nif_cal_len_fqp";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nif_cal_len_fqp].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nif_cal_len_fqp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_tcg].name = "nof_bits_in_nof_tcg";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_tcg].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_tcg].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_tcg].name = "nof_tcg";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_tcg].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_tcg].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_egr_q_prio].name = "nof_bits_in_nof_egr_q_prio";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_egr_q_prio].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_egr_q_prio].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_q_prio].name = "nof_egr_q_prio";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_q_prio].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_q_prio].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_tcg_min_priorities].name = "tcg_min_priorities";
    submodule_data->defines[dnx_data_egr_queuing_params_define_tcg_min_priorities].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_tcg_min_priorities].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_max_credit_number].name = "max_credit_number";
    submodule_data->defines[dnx_data_egr_queuing_params_define_max_credit_number].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_max_credit_number].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_max_gbps_rate_egq].name = "max_gbps_rate_egq";
    submodule_data->defines[dnx_data_egr_queuing_params_define_max_gbps_rate_egq].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_max_gbps_rate_egq].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_txq_iready_th].name = "txq_iready_th";
    submodule_data->defines[dnx_data_egr_queuing_params_define_txq_iready_th].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_txq_iready_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_txq_tdm_iready_th].name = "txq_tdm_iready_th";
    submodule_data->defines[dnx_data_egr_queuing_params_define_txq_tdm_iready_th].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_txq_tdm_iready_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_initial_packet_mode].name = "initial_packet_mode";
    submodule_data->defines[dnx_data_egr_queuing_params_define_initial_packet_mode].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_initial_packet_mode].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_sub_calendar_ifc].name = "sub_calendar_ifc";
    submodule_data->defines[dnx_data_egr_queuing_params_define_sub_calendar_ifc].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_sub_calendar_ifc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_txq_max_credits].name = "txq_max_credits";
    submodule_data->defines[dnx_data_egr_queuing_params_define_txq_max_credits].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_txq_max_credits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_txq_max_credits_reduce_factor].name = "txq_max_credits_reduce_factor";
    submodule_data->defines[dnx_data_egr_queuing_params_define_txq_max_credits_reduce_factor].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_txq_max_credits_reduce_factor].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_pqp_ifc_almost_empty_th].name = "pqp_ifc_almost_empty_th";
    submodule_data->defines[dnx_data_egr_queuing_params_define_pqp_ifc_almost_empty_th].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_pqp_ifc_almost_empty_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_pqp_port_almost_empty_th].name = "pqp_port_almost_empty_th";
    submodule_data->defines[dnx_data_egr_queuing_params_define_pqp_port_almost_empty_th].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_pqp_port_almost_empty_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_pqp_queue_almost_empty_th].name = "pqp_queue_almost_empty_th";
    submodule_data->defines[dnx_data_egr_queuing_params_define_pqp_queue_almost_empty_th].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_pqp_queue_almost_empty_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_if_min_gap_priority_size].name = "if_min_gap_priority_size";
    submodule_data->defines[dnx_data_egr_queuing_params_define_if_min_gap_priority_size].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_if_min_gap_priority_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_flexe_base].name = "egr_if_flexe_base";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_flexe_base].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_flexe_base].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_dedicated_flexe_interfaces].name = "nof_dedicated_flexe_interfaces";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_dedicated_flexe_interfaces].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_dedicated_flexe_interfaces].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_interfaces_wo_flexe].name = "nof_egr_interfaces_wo_flexe";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_interfaces_wo_flexe].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_interfaces_wo_flexe].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_nif_alloc_start].name = "egr_if_nif_alloc_start";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_nif_alloc_start].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_nif_alloc_start].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_nif_alloc_end].name = "egr_if_nif_alloc_end";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_nif_alloc_end].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_nif_alloc_end].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_ilkn_alloc_start].name = "egr_if_ilkn_alloc_start";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_ilkn_alloc_start].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_ilkn_alloc_start].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_ilkn_alloc_end].name = "egr_if_ilkn_alloc_end";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_ilkn_alloc_end].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_ilkn_alloc_end].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egq_shaper_max_mbps].name = "egq_shaper_max_mbps";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egq_shaper_max_mbps].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egq_shaper_max_mbps].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_fqp_profiles].name = "nof_fqp_profiles";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_fqp_profiles].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_fqp_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_min_gap_bits].name = "min_gap_bits";
    submodule_data->defines[dnx_data_egr_queuing_params_define_min_gap_bits].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_min_gap_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_invalid_otm_port].name = "invalid_otm_port";
    submodule_data->defines[dnx_data_egr_queuing_params_define_invalid_otm_port].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_invalid_otm_port].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_egr_queuing_params_define_burst_size_below_mtu].name = "burst_size_below_mtu";
    submodule_data->defines[dnx_data_egr_queuing_params_define_burst_size_below_mtu].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_burst_size_below_mtu].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egress_bubble_delay].name = "egress_bubble_delay";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egress_bubble_delay].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egress_bubble_delay].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_egr_queuing_params_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data egr_queuing params tables");

    
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].name = "if_speed_params";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].doc = "";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].size_of_values = sizeof(dnx_data_egr_queuing_params_if_speed_params_t);
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].entry_get = dnx_data_egr_queuing_params_if_speed_params_entry_str_get;

    
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].nof_keys = 2;
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].keys[0].name = "idx";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].keys[0].doc = "";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].keys[1].name = "clock_rate_idx";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].nof_values = 7;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values, dnxc_data_value_t, submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].nof_values, "_dnx_data_egr_queuing_params_table_if_speed_params table values");
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[0].name = "speed";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[0].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[0].doc = "port speed in GHz";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[0].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_params_if_speed_params_t, speed);
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[1].name = "if_id";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[1].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[1].doc = "port egq interface ID";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[1].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_params_if_speed_params_t, if_id);
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[2].name = "crdt_size";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[2].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[2].doc = "TXI credits";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[2].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_params_if_speed_params_t, crdt_size);
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[3].name = "irdy_thr";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[3].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[3].doc = "NRDY threshold";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[3].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_params_if_speed_params_t, irdy_thr);
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[4].name = "txq_max_bytes";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[4].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[4].doc = "TXQ max bytes threshold";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[4].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_params_if_speed_params_t, txq_max_bytes);
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[5].name = "min_gap_hp";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[5].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[5].doc = "minimum gap between consecutive access to same interface if serving HP Q";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[5].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_params_if_speed_params_t, min_gap_hp);
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[6].name = "fqp_min_gap";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[6].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[6].doc = "fqp min gap";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].values[6].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_params_if_speed_params_t, fqp_min_gap);

    
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].name = "emr_fifo";
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].doc = "";
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].size_of_values = sizeof(dnx_data_egr_queuing_params_emr_fifo_t);
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].entry_get = dnx_data_egr_queuing_params_emr_fifo_entry_str_get;

    
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].nof_keys = 2;
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].keys[0].name = "idx";
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].keys[0].doc = "";
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].keys[1].name = "tdm";
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].values, dnxc_data_value_t, submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].nof_values, "_dnx_data_egr_queuing_params_table_emr_fifo table values");
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].values[0].name = "depth";
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].values[0].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].values[0].doc = "depth configuration for each FIFO";
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].values[0].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_params_emr_fifo_t, depth);
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].values[1].name = "almost_full";
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].values[1].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].values[1].doc = "configuration of almost full threshold";
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].values[1].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_params_emr_fifo_t, almost_full);
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].values[2].name = "full";
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].values[2].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].values[2].doc = "configuration of full threshold";
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].values[2].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_params_emr_fifo_t, full);

    
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].name = "if_speed_params_clk_th";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].doc = "";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].size_of_values = sizeof(dnx_data_egr_queuing_params_if_speed_params_clk_th_t);
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].entry_get = dnx_data_egr_queuing_params_if_speed_params_clk_th_entry_str_get;

    
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].nof_keys = 1;
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].keys[0].name = "idx";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].values, dnxc_data_value_t, submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].nof_values, "_dnx_data_egr_queuing_params_table_if_speed_params_clk_th table values");
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].values[0].name = "max_clock_rate_th";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].values[0].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].values[0].doc = "clock rate thresholds in Khz";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].values[0].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_params_if_speed_params_clk_th_t, max_clock_rate_th);

    
    submodule_data->tables[dnx_data_egr_queuing_params_table_fqp_dbf_additional_latency].name = "fqp_dbf_additional_latency";
    submodule_data->tables[dnx_data_egr_queuing_params_table_fqp_dbf_additional_latency].doc = "";
    submodule_data->tables[dnx_data_egr_queuing_params_table_fqp_dbf_additional_latency].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_egr_queuing_params_table_fqp_dbf_additional_latency].size_of_values = sizeof(dnx_data_egr_queuing_params_fqp_dbf_additional_latency_t);
    submodule_data->tables[dnx_data_egr_queuing_params_table_fqp_dbf_additional_latency].entry_get = dnx_data_egr_queuing_params_fqp_dbf_additional_latency_entry_str_get;

    
    submodule_data->tables[dnx_data_egr_queuing_params_table_fqp_dbf_additional_latency].nof_keys = 1;
    submodule_data->tables[dnx_data_egr_queuing_params_table_fqp_dbf_additional_latency].keys[0].name = "core_index";
    submodule_data->tables[dnx_data_egr_queuing_params_table_fqp_dbf_additional_latency].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_egr_queuing_params_table_fqp_dbf_additional_latency].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_egr_queuing_params_table_fqp_dbf_additional_latency].values, dnxc_data_value_t, submodule_data->tables[dnx_data_egr_queuing_params_table_fqp_dbf_additional_latency].nof_values, "_dnx_data_egr_queuing_params_table_fqp_dbf_additional_latency table values");
    submodule_data->tables[dnx_data_egr_queuing_params_table_fqp_dbf_additional_latency].values[0].name = "delay";
    submodule_data->tables[dnx_data_egr_queuing_params_table_fqp_dbf_additional_latency].values[0].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_params_table_fqp_dbf_additional_latency].values[0].doc = "fqp_dbf_additional_latency of the CORE.";
    submodule_data->tables[dnx_data_egr_queuing_params_table_fqp_dbf_additional_latency].values[0].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_params_fqp_dbf_additional_latency_t, delay);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_egr_queuing_params_feature_get(
    int unit,
    dnx_data_egr_queuing_params_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, feature);
}


uint32
dnx_data_egr_queuing_params_nof_bits_in_tcg_weight_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_bits_in_tcg_weight);
}

uint32
dnx_data_egr_queuing_params_nof_bits_in_nof_q_pairs_in_ps_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_bits_in_nof_q_pairs_in_ps);
}

uint32
dnx_data_egr_queuing_params_nof_q_pairs_in_ps_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_q_pairs_in_ps);
}

uint32
dnx_data_egr_queuing_params_nof_bits_in_q_pair_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_bits_in_q_pair);
}

uint32
dnx_data_egr_queuing_params_nof_q_pairs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_q_pairs);
}

uint32
dnx_data_egr_queuing_params_nof_bits_in_nof_port_schedulers_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_bits_in_nof_port_schedulers);
}

uint32
dnx_data_egr_queuing_params_nof_port_schedulers_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_port_schedulers);
}

uint32
dnx_data_egr_queuing_params_nof_bits_in_channel_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_bits_in_channel);
}

uint32
dnx_data_egr_queuing_params_nof_bits_in_chan_arb_calendar_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_bits_in_chan_arb_calendar_size);
}

uint32
dnx_data_egr_queuing_params_chan_arb_calendar_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_chan_arb_calendar_size);
}

uint32
dnx_data_egr_queuing_params_nof_bits_in_port_prio_calendar_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_bits_in_port_prio_calendar_size);
}

uint32
dnx_data_egr_queuing_params_port_prio_calendar_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_port_prio_calendar_size);
}

uint32
dnx_data_egr_queuing_params_nof_bits_in_tcg_calendar_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_bits_in_tcg_calendar_size);
}

uint32
dnx_data_egr_queuing_params_tcg_calendar_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_tcg_calendar_size);
}

uint32
dnx_data_egr_queuing_params_nof_bits_in_nof_calendars_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_bits_in_nof_calendars);
}

uint32
dnx_data_egr_queuing_params_nof_calendars_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_calendars);
}

uint32
dnx_data_egr_queuing_params_nof_bits_in_egr_interface_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_bits_in_egr_interface);
}

uint32
dnx_data_egr_queuing_params_nof_bits_mirror_prio_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_bits_mirror_prio);
}

uint32
dnx_data_egr_queuing_params_nof_bits_in_egr_ch_interface_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_bits_in_egr_ch_interface);
}

uint32
dnx_data_egr_queuing_params_nof_egr_interfaces_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_egr_interfaces);
}

uint32
dnx_data_egr_queuing_params_first_lp_egr_interface_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_first_lp_egr_interface);
}

uint32
dnx_data_egr_queuing_params_nof_egr_ch_interfaces_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_egr_ch_interfaces);
}

uint32
dnx_data_egr_queuing_params_nof_egr_rcy_interfaces_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_egr_rcy_interfaces);
}

uint32
dnx_data_egr_queuing_params_nof_egr_olp_interfaces_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_egr_olp_interfaces);
}

uint32
dnx_data_egr_queuing_params_reserved_if_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_reserved_if);
}

uint32
dnx_data_egr_queuing_params_egr_if_cpu_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_egr_if_cpu);
}

uint32
dnx_data_egr_queuing_params_egr_if_sat_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_egr_if_sat);
}

uint32
dnx_data_egr_queuing_params_egr_if_oam_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_egr_if_oam);
}

uint32
dnx_data_egr_queuing_params_egr_if_olp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_egr_if_olp);
}

uint32
dnx_data_egr_queuing_params_egr_if_rcy_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_egr_if_rcy);
}

uint32
dnx_data_egr_queuing_params_egr_if_txi_rcy_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_egr_if_txi_rcy);
}

uint32
dnx_data_egr_queuing_params_egr_if_eventor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_egr_if_eventor);
}

uint32
dnx_data_egr_queuing_params_egr_if_nif_base_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_egr_if_nif_base);
}

uint32
dnx_data_egr_queuing_params_cal_res_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_cal_res);
}

uint32
dnx_data_egr_queuing_params_cal_res_packet_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_cal_res_packet_mode);
}

uint32
dnx_data_egr_queuing_params_cal_burst_res_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_cal_burst_res);
}

uint32
dnx_data_egr_queuing_params_nof_bits_in_cal_cal_len_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_bits_in_cal_cal_len);
}

uint32
dnx_data_egr_queuing_params_cal_cal_len_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_cal_cal_len);
}

uint32
dnx_data_egr_queuing_params_nof_bits_in_nif_cal_len_pqp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_pqp);
}

uint32
dnx_data_egr_queuing_params_nif_cal_len_pqp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nif_cal_len_pqp);
}

uint32
dnx_data_egr_queuing_params_nof_bits_in_nif_cal_len_fqp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_fqp);
}

uint32
dnx_data_egr_queuing_params_nif_cal_len_fqp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nif_cal_len_fqp);
}

uint32
dnx_data_egr_queuing_params_nof_bits_in_nof_tcg_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_bits_in_nof_tcg);
}

uint32
dnx_data_egr_queuing_params_nof_tcg_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_tcg);
}

uint32
dnx_data_egr_queuing_params_nof_bits_in_nof_egr_q_prio_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_bits_in_nof_egr_q_prio);
}

uint32
dnx_data_egr_queuing_params_nof_egr_q_prio_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_egr_q_prio);
}

uint32
dnx_data_egr_queuing_params_tcg_min_priorities_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_tcg_min_priorities);
}

uint32
dnx_data_egr_queuing_params_max_credit_number_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_max_credit_number);
}

uint32
dnx_data_egr_queuing_params_max_gbps_rate_egq_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_max_gbps_rate_egq);
}

uint32
dnx_data_egr_queuing_params_txq_iready_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_txq_iready_th);
}

uint32
dnx_data_egr_queuing_params_txq_tdm_iready_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_txq_tdm_iready_th);
}

uint32
dnx_data_egr_queuing_params_initial_packet_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_initial_packet_mode);
}

uint32
dnx_data_egr_queuing_params_sub_calendar_ifc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_sub_calendar_ifc);
}

uint32
dnx_data_egr_queuing_params_txq_max_credits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_txq_max_credits);
}

uint32
dnx_data_egr_queuing_params_txq_max_credits_reduce_factor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_txq_max_credits_reduce_factor);
}

uint32
dnx_data_egr_queuing_params_pqp_ifc_almost_empty_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_pqp_ifc_almost_empty_th);
}

uint32
dnx_data_egr_queuing_params_pqp_port_almost_empty_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_pqp_port_almost_empty_th);
}

uint32
dnx_data_egr_queuing_params_pqp_queue_almost_empty_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_pqp_queue_almost_empty_th);
}

uint32
dnx_data_egr_queuing_params_if_min_gap_priority_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_if_min_gap_priority_size);
}

uint32
dnx_data_egr_queuing_params_egr_if_flexe_base_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_egr_if_flexe_base);
}

uint32
dnx_data_egr_queuing_params_nof_dedicated_flexe_interfaces_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_dedicated_flexe_interfaces);
}

uint32
dnx_data_egr_queuing_params_nof_egr_interfaces_wo_flexe_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_egr_interfaces_wo_flexe);
}

uint32
dnx_data_egr_queuing_params_egr_if_nif_alloc_start_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_egr_if_nif_alloc_start);
}

uint32
dnx_data_egr_queuing_params_egr_if_nif_alloc_end_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_egr_if_nif_alloc_end);
}

uint32
dnx_data_egr_queuing_params_egr_if_ilkn_alloc_start_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_egr_if_ilkn_alloc_start);
}

uint32
dnx_data_egr_queuing_params_egr_if_ilkn_alloc_end_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_egr_if_ilkn_alloc_end);
}

uint32
dnx_data_egr_queuing_params_egq_shaper_max_mbps_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_egq_shaper_max_mbps);
}

uint32
dnx_data_egr_queuing_params_nof_fqp_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_fqp_profiles);
}

uint32
dnx_data_egr_queuing_params_min_gap_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_min_gap_bits);
}

uint32
dnx_data_egr_queuing_params_invalid_otm_port_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_invalid_otm_port);
}

uint32
dnx_data_egr_queuing_params_burst_size_below_mtu_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_burst_size_below_mtu);
}

uint32
dnx_data_egr_queuing_params_egress_bubble_delay_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_egress_bubble_delay);
}



const dnx_data_egr_queuing_params_if_speed_params_t *
dnx_data_egr_queuing_params_if_speed_params_get(
    int unit,
    int idx,
    int clock_rate_idx)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_table_if_speed_params);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, idx, clock_rate_idx);
    return (const dnx_data_egr_queuing_params_if_speed_params_t *) data;

}

const dnx_data_egr_queuing_params_emr_fifo_t *
dnx_data_egr_queuing_params_emr_fifo_get(
    int unit,
    int idx,
    int tdm)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_table_emr_fifo);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, idx, tdm);
    return (const dnx_data_egr_queuing_params_emr_fifo_t *) data;

}

const dnx_data_egr_queuing_params_if_speed_params_clk_th_t *
dnx_data_egr_queuing_params_if_speed_params_clk_th_get(
    int unit,
    int idx)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_table_if_speed_params_clk_th);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, idx, 0);
    return (const dnx_data_egr_queuing_params_if_speed_params_clk_th_t *) data;

}

const dnx_data_egr_queuing_params_fqp_dbf_additional_latency_t *
dnx_data_egr_queuing_params_fqp_dbf_additional_latency_get(
    int unit,
    int core_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_table_fqp_dbf_additional_latency);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, core_index, 0);
    return (const dnx_data_egr_queuing_params_fqp_dbf_additional_latency_t *) data;

}


shr_error_e
dnx_data_egr_queuing_params_if_speed_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_egr_queuing_params_if_speed_params_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_table_if_speed_params);
    data = (const dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->speed);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->if_id);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->crdt_size);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->irdy_thr);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->txq_max_bytes);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->min_gap_hp);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fqp_min_gap);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_egr_queuing_params_emr_fifo_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_egr_queuing_params_emr_fifo_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_table_emr_fifo);
    data = (const dnx_data_egr_queuing_params_emr_fifo_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->depth);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->almost_full);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->full);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_egr_queuing_params_if_speed_params_clk_th_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_egr_queuing_params_if_speed_params_clk_th_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_table_if_speed_params_clk_th);
    data = (const dnx_data_egr_queuing_params_if_speed_params_clk_th_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max_clock_rate_th);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_egr_queuing_params_fqp_dbf_additional_latency_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_egr_queuing_params_fqp_dbf_additional_latency_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_table_fqp_dbf_additional_latency);
    data = (const dnx_data_egr_queuing_params_fqp_dbf_additional_latency_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->delay);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_egr_queuing_params_if_speed_params_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_table_if_speed_params);

}

const dnxc_data_table_info_t *
dnx_data_egr_queuing_params_emr_fifo_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_table_emr_fifo);

}

const dnxc_data_table_info_t *
dnx_data_egr_queuing_params_if_speed_params_clk_th_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_table_if_speed_params_clk_th);

}

const dnxc_data_table_info_t *
dnx_data_egr_queuing_params_fqp_dbf_additional_latency_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_table_fqp_dbf_additional_latency);

}






static shr_error_e
dnx_data_egr_queuing_common_max_val_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "common_max_val";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_egr_queuing_common_max_val_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data egr_queuing common_max_val features");

    
    submodule_data->nof_defines = _dnx_data_egr_queuing_common_max_val_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data egr_queuing common_max_val defines");

    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_nof_bits_in_calendar_size].name = "nof_bits_in_calendar_size";
    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_nof_bits_in_calendar_size].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_nof_bits_in_calendar_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_calendar_size].name = "calendar_size";
    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_calendar_size].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_calendar_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_slow_port_speed].name = "slow_port_speed";
    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_slow_port_speed].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_slow_port_speed].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_mux_calendar_resolution].name = "mux_calendar_resolution";
    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_mux_calendar_resolution].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_mux_calendar_resolution].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_egr_queuing_common_max_val_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data egr_queuing common_max_val tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_egr_queuing_common_max_val_feature_get(
    int unit,
    dnx_data_egr_queuing_common_max_val_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_common_max_val, feature);
}


uint32
dnx_data_egr_queuing_common_max_val_nof_bits_in_calendar_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_common_max_val, dnx_data_egr_queuing_common_max_val_define_nof_bits_in_calendar_size);
}

uint32
dnx_data_egr_queuing_common_max_val_calendar_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_common_max_val, dnx_data_egr_queuing_common_max_val_define_calendar_size);
}

uint32
dnx_data_egr_queuing_common_max_val_slow_port_speed_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_common_max_val, dnx_data_egr_queuing_common_max_val_define_slow_port_speed);
}

uint32
dnx_data_egr_queuing_common_max_val_mux_calendar_resolution_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_common_max_val, dnx_data_egr_queuing_common_max_val_define_mux_calendar_resolution);
}










static shr_error_e
dnx_data_egr_queuing_rate_measurement_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "rate_measurement";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_egr_queuing_rate_measurement_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data egr_queuing rate_measurement features");

    submodule_data->features[dnx_data_egr_queuing_rate_measurement_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_egr_queuing_rate_measurement_is_supported].doc = "";
    submodule_data->features[dnx_data_egr_queuing_rate_measurement_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_egr_queuing_rate_measurement_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data egr_queuing rate_measurement defines");

    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_max_interval].name = "max_interval";
    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_max_interval].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_max_interval].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_promile_factor].name = "promile_factor";
    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_promile_factor].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_promile_factor].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_max_granularity].name = "max_granularity";
    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_max_granularity].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_max_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_min_bytes_for_granularity].name = "min_bytes_for_granularity";
    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_min_bytes_for_granularity].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_min_bytes_for_granularity].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_egr_queuing_rate_measurement_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data egr_queuing rate_measurement tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_egr_queuing_rate_measurement_feature_get(
    int unit,
    dnx_data_egr_queuing_rate_measurement_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_rate_measurement, feature);
}


uint32
dnx_data_egr_queuing_rate_measurement_max_interval_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_rate_measurement, dnx_data_egr_queuing_rate_measurement_define_max_interval);
}

uint32
dnx_data_egr_queuing_rate_measurement_promile_factor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_rate_measurement, dnx_data_egr_queuing_rate_measurement_define_promile_factor);
}

uint32
dnx_data_egr_queuing_rate_measurement_max_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_rate_measurement, dnx_data_egr_queuing_rate_measurement_define_max_granularity);
}

uint32
dnx_data_egr_queuing_rate_measurement_min_bytes_for_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_rate_measurement, dnx_data_egr_queuing_rate_measurement_define_min_bytes_for_granularity);
}










static shr_error_e
dnx_data_egr_queuing_phantom_queues_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "phantom_queues";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_egr_queuing_phantom_queues_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data egr_queuing phantom_queues features");

    
    submodule_data->nof_defines = _dnx_data_egr_queuing_phantom_queues_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data egr_queuing phantom_queues defines");

    submodule_data->defines[dnx_data_egr_queuing_phantom_queues_define_nof_threshold_profiles].name = "nof_threshold_profiles";
    submodule_data->defines[dnx_data_egr_queuing_phantom_queues_define_nof_threshold_profiles].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_phantom_queues_define_nof_threshold_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_phantom_queues_define_max_threshold].name = "max_threshold";
    submodule_data->defines[dnx_data_egr_queuing_phantom_queues_define_max_threshold].doc = "";
    
    submodule_data->defines[dnx_data_egr_queuing_phantom_queues_define_max_threshold].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_egr_queuing_phantom_queues_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data egr_queuing phantom_queues tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_egr_queuing_phantom_queues_feature_get(
    int unit,
    dnx_data_egr_queuing_phantom_queues_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_phantom_queues, feature);
}


uint32
dnx_data_egr_queuing_phantom_queues_nof_threshold_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_phantom_queues, dnx_data_egr_queuing_phantom_queues_define_nof_threshold_profiles);
}

uint32
dnx_data_egr_queuing_phantom_queues_max_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_phantom_queues, dnx_data_egr_queuing_phantom_queues_define_max_threshold);
}










static shr_error_e
dnx_data_egr_queuing_reassembly_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "reassembly";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_egr_queuing_reassembly_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data egr_queuing reassembly features");

    submodule_data->features[dnx_data_egr_queuing_reassembly_prs].name = "prs";
    submodule_data->features[dnx_data_egr_queuing_reassembly_prs].doc = "";
    submodule_data->features[dnx_data_egr_queuing_reassembly_prs].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_egr_queuing_reassembly_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data egr_queuing reassembly defines");

    
    submodule_data->nof_tables = _dnx_data_egr_queuing_reassembly_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data egr_queuing reassembly tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_egr_queuing_reassembly_feature_get(
    int unit,
    dnx_data_egr_queuing_reassembly_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_reassembly, feature);
}








shr_error_e
dnx_data_egr_queuing_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "egr_queuing";
    module_data->nof_submodules = _dnx_data_egr_queuing_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data egr_queuing submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_egr_queuing_params_init(unit, &module_data->submodules[dnx_data_egr_queuing_submodule_params]));
    SHR_IF_ERR_EXIT(dnx_data_egr_queuing_common_max_val_init(unit, &module_data->submodules[dnx_data_egr_queuing_submodule_common_max_val]));
    SHR_IF_ERR_EXIT(dnx_data_egr_queuing_rate_measurement_init(unit, &module_data->submodules[dnx_data_egr_queuing_submodule_rate_measurement]));
    SHR_IF_ERR_EXIT(dnx_data_egr_queuing_phantom_queues_init(unit, &module_data->submodules[dnx_data_egr_queuing_submodule_phantom_queues]));
    SHR_IF_ERR_EXIT(dnx_data_egr_queuing_reassembly_init(unit, &module_data->submodules[dnx_data_egr_queuing_submodule_reassembly]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_egr_queuing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_egr_queuing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_egr_queuing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_egr_queuing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_egr_queuing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_egr_queuing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_egr_queuing_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b1_data_egr_queuing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_egr_queuing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_egr_queuing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_egr_queuing_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a2_data_egr_queuing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_egr_queuing_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

