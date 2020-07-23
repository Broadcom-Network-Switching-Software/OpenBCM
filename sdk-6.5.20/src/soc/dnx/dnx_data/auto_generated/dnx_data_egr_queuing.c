

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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



static shr_error_e
dnx_data_egr_queuing_params_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "params";
    submodule_data->doc = "default values for egr_queuing module";
    
    submodule_data->nof_features = _dnx_data_egr_queuing_params_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data egr_queuing params features");

    submodule_data->features[dnx_data_egr_queuing_params_conditional_crdt_table_access].name = "conditional_crdt_table_access";
    submodule_data->features[dnx_data_egr_queuing_params_conditional_crdt_table_access].doc = "Indicates whether 'port_read_enable' is required before accessing OTM_CALENDAR_CRDT_TABLE (temporary HW issue)";
    submodule_data->features[dnx_data_egr_queuing_params_conditional_crdt_table_access].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_non_j2b0_counters_are_valid].name = "non_j2b0_counters_are_valid";
    submodule_data->features[dnx_data_egr_queuing_params_non_j2b0_counters_are_valid].doc = "Indicates whether counters, which are not supported by j2b0, are to be referenced. Set to 'invalid' for J2B0.";
    submodule_data->features[dnx_data_egr_queuing_params_non_j2b0_counters_are_valid].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_if_attributes_profile_exist].name = "if_attributes_profile_exist";
    submodule_data->features[dnx_data_egr_queuing_params_if_attributes_profile_exist].doc = "Is egq interface attributes using profile";
    submodule_data->features[dnx_data_egr_queuing_params_if_attributes_profile_exist].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_cos_map_is_per_pp_port].name = "cos_map_is_per_pp_port";
    submodule_data->features[dnx_data_egr_queuing_params_cos_map_is_per_pp_port].doc = "Indicates whether access to cos map is per pp port or per tm port";
    submodule_data->features[dnx_data_egr_queuing_params_cos_map_is_per_pp_port].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_phantom_queues].name = "phantom_queues";
    submodule_data->features[dnx_data_egr_queuing_params_phantom_queues].doc = "support phantom queues feature";
    submodule_data->features[dnx_data_egr_queuing_params_phantom_queues].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_almost_empty_delay].name = "almost_empty_delay";
    submodule_data->features[dnx_data_egr_queuing_params_almost_empty_delay].doc = "support almost empty delay configuration";
    submodule_data->features[dnx_data_egr_queuing_params_almost_empty_delay].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_fqp_calender_set_select].name = "fqp_calender_set_select";
    submodule_data->features[dnx_data_egr_queuing_params_fqp_calender_set_select].doc = "is fqp_calender_set_select field exist in HW";
    submodule_data->features[dnx_data_egr_queuing_params_fqp_calender_set_select].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_sub_calendar].name = "sub_calendar";
    submodule_data->features[dnx_data_egr_queuing_params_sub_calendar].doc = "is sub calendar feature enable";
    submodule_data->features[dnx_data_egr_queuing_params_sub_calendar].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_txq_tdm_irdy_sel_exist].name = "txq_tdm_irdy_sel_exist";
    submodule_data->features[dnx_data_egr_queuing_params_txq_tdm_irdy_sel_exist].doc = "determine if parameter txq_tdm_irdy_sel exist";
    submodule_data->features[dnx_data_egr_queuing_params_txq_tdm_irdy_sel_exist].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_egr_queuing_params_total_shaper_max_burst_exist].name = "total_shaper_max_burst_exist";
    submodule_data->features[dnx_data_egr_queuing_params_total_shaper_max_burst_exist].doc = "determine if parameter total_shaper_max_burst exist";
    submodule_data->features[dnx_data_egr_queuing_params_total_shaper_max_burst_exist].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_egr_queuing_params_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data egr_queuing params defines");

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_tcg_weight].name = "nof_bits_in_tcg_weight";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_tcg_weight].doc = "number of bits in 'weight' representation of a TCG. See EPS_DWM_8P";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_tcg_weight].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_q_pairs_in_ps].name = "nof_bits_in_nof_q_pairs_in_ps";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_q_pairs_in_ps].doc = "number of bits in 'nof_q_pairs_in_ps' representation. Essentially, log2(nof_q_pairs_in_ps)";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_q_pairs_in_ps].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_q_pairs_in_ps].name = "nof_q_pairs_in_ps";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_q_pairs_in_ps].doc = "Number of qpairs per one port scheduler";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_q_pairs_in_ps].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_q_pair].name = "nof_bits_in_q_pair";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_q_pair].doc = "number of bits in 'queue pair' representation. Essentially, log2(nof_q_pairs)";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_q_pair].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_q_pairs].name = "nof_q_pairs";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_q_pairs].doc = "Number of q_pairs per core";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_q_pairs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_port_schedulers].name = "nof_bits_in_nof_port_schedulers";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_port_schedulers].doc = "number of bits in 'nof_port_schedulers' representation. Essentially, log2(nof_port_schedulers)";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_port_schedulers].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_port_schedulers].name = "nof_port_schedulers";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_port_schedulers].doc = "Number of port schedulers per core";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_port_schedulers].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_chan_arb_calendar_size].name = "nof_bits_in_chan_arb_calendar_size";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_chan_arb_calendar_size].doc = "number of bits in 'num. of entries in chan_arb calendar' representation. Essentially, log2(chan_arb_calendar_size)";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_chan_arb_calendar_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_chan_arb_calendar_size].name = "chan_arb_calendar_size";
    submodule_data->defines[dnx_data_egr_queuing_params_define_chan_arb_calendar_size].doc = "number of entries in calendar of 'channelized arbiter' (chan_arb)";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_chan_arb_calendar_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_port_prio_calendar_size].name = "nof_bits_in_port_prio_calendar_size";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_port_prio_calendar_size].doc = "number of bits in 'num. of entries in port_prio calendar' representation. Essentially, log2(port_prio_calendar_size)";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_port_prio_calendar_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_port_prio_calendar_size].name = "port_prio_calendar_size";
    submodule_data->defines[dnx_data_egr_queuing_params_define_port_prio_calendar_size].doc = "number of entries in calendar of 'port priority' (also referred to as TC)";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_port_prio_calendar_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_tcg_calendar_size].name = "nof_bits_in_tcg_calendar_size";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_tcg_calendar_size].doc = "number of bits in 'num. of entries in tcg calendar' representation. Essentially, log2(port_prio_calendar_size)";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_tcg_calendar_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_tcg_calendar_size].name = "tcg_calendar_size";
    submodule_data->defines[dnx_data_egr_queuing_params_define_tcg_calendar_size].doc = "number of entries in calendar of 'tcg' (group of TCs, group of port_prio entries)";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_tcg_calendar_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_calendars].name = "nof_bits_in_nof_calendars";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_calendars].doc = "number of bits in 'egr nof_calendars' representation. Essentially, log2(nof_calendars)";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_calendars].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_calendars].name = "nof_calendars";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_calendars].doc = "Number of calendar-pairs per core. Total number of calendars is twice this value.";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_calendars].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_egr_interface].name = "nof_bits_in_egr_interface";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_egr_interface].doc = "number of bits in 'egr interface' representation. Essentially, log2(nof_egr_interfaces)";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_egr_interface].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_mirror_prio].name = "nof_bits_mirror_prio";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_mirror_prio].doc = "number of bits in mirror priority to check bandwidth";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_mirror_prio].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_egr_ch_interface].name = "nof_bits_in_egr_ch_interface";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_egr_ch_interface].doc = "number of bits in 'egr ch interface' representation. Essentially, log2(nof_egr_ch_interfaces)";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_egr_ch_interface].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_interfaces].name = "nof_egr_interfaces";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_interfaces].doc = "Number of egr interfaces, per core";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_interfaces].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_ch_interfaces].name = "nof_egr_ch_interfaces";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_ch_interfaces].doc = "Number of egr channelized interfaces, per core";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_ch_interfaces].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_reserved_if].name = "reserved_if";
    submodule_data->defines[dnx_data_egr_queuing_params_define_reserved_if].doc = "reserved interface (used for dummy flow control mapping in LAG SCH)";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_reserved_if].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_cpu].name = "egr_if_cpu";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_cpu].doc = "cpu interface at egr";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_cpu].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_sat].name = "egr_if_sat";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_sat].doc = "sat interface at egr";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_sat].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_oam].name = "egr_if_oam";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_oam].doc = "oam interface at egr";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_oam].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_olp].name = "egr_if_olp";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_olp].doc = "olp interface at egr";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_olp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_rcy].name = "egr_if_rcy";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_rcy].doc = "recycle interface at egr";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_rcy].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_txi_rcy].name = "egr_if_txi_rcy";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_txi_rcy].doc = "txi recycle interface at egr";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_txi_rcy].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_eventor].name = "egr_if_eventor";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_eventor].doc = "eventor interface at egr";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_eventor].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_nif_base].name = "egr_if_nif_base";
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_nif_base].doc = "base egress interface for Nif ports";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_egr_if_nif_base].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_res].name = "cal_res";
    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_res].doc = "Calendar resolution. Conversion coefficient from bits to credits for DATA mode.";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_res].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_res_packet_mode].name = "cal_res_packet_mode";
    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_res_packet_mode].doc = "Calendar resolution. Conversion coefficient from bits to credits for PACKET mode.";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_res_packet_mode].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_burst_res].name = "cal_burst_res";
    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_burst_res].doc = "Calendar resolution. Conversion coefficient from bytes to credits. Number of credits per byte. Used for burst size conversion.";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_burst_res].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_cal_cal_len].name = "nof_bits_in_cal_cal_len";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_cal_cal_len].doc = "Number of bits in representation of cal_cal_len (log2)";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_cal_cal_len].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_cal_len].name = "cal_cal_len";
    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_cal_len].doc = "calcal calendar length";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_cal_cal_len].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_pqp].name = "nof_bits_in_nif_cal_len_pqp";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_pqp].doc = "Number of bits in representation of nif_cal_len (log2) for pqp";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_pqp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nif_cal_len_pqp].name = "nif_cal_len_pqp";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nif_cal_len_pqp].doc = "nif calendar length for pqp";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nif_cal_len_pqp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_fqp].name = "nof_bits_in_nif_cal_len_fqp";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_fqp].doc = "Number of bits in representation of nif_cal_len (log2) for fqp";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_fqp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nif_cal_len_fqp].name = "nif_cal_len_fqp";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nif_cal_len_fqp].doc = "nif calendar length for fqp";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nif_cal_len_fqp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_tcg].name = "nof_bits_in_nof_tcg";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_tcg].doc = "Number of bits in representation of nof_tcg (log2)";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_tcg].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_tcg].name = "nof_tcg";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_tcg].doc = "Number of TC groups supported by this system";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_tcg].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_egr_q_prio].name = "nof_bits_in_nof_egr_q_prio";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_egr_q_prio].doc = "Number of bits in representation of nof_egr_q_prio (log2)";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_bits_in_nof_egr_q_prio].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_q_prio].name = "nof_egr_q_prio";
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_q_prio].doc = "Number of egress priorities for unscheduled traffic";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_nof_egr_q_prio].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_tcg_min_priorities].name = "tcg_min_priorities";
    submodule_data->defines[dnx_data_egr_queuing_params_define_tcg_min_priorities].doc = "minimum number of priorities supported by tcg";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_tcg_min_priorities].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_max_credit_number].name = "max_credit_number";
    submodule_data->defines[dnx_data_egr_queuing_params_define_max_credit_number].doc = "the maximum number of credits supported";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_max_credit_number].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_max_gbps_rate_egq].name = "max_gbps_rate_egq";
    submodule_data->defines[dnx_data_egr_queuing_params_define_max_gbps_rate_egq].doc = "EGQ Maximum Gb/s rate. This is the upper boundary, it can be lower depending on the credit size";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_max_gbps_rate_egq].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_txq_iready_th].name = "txq_iready_th";
    submodule_data->defines[dnx_data_egr_queuing_params_define_txq_iready_th].doc = "Define ready TXQ treshold in PDs for non TDM queues, when Q is above this treshold FQP block relevant interface in EPS";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_txq_iready_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_txq_tdm_iready_th].name = "txq_tdm_iready_th";
    submodule_data->defines[dnx_data_egr_queuing_params_define_txq_tdm_iready_th].doc = "Define ready TXQ treshold in PDs for TDM queues, when Q is above this treshold FQP block relevant interface in EPS";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_txq_tdm_iready_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_initial_packet_mode].name = "initial_packet_mode";
    submodule_data->defines[dnx_data_egr_queuing_params_define_initial_packet_mode].doc = "'packet mode' value for a newly added port. See type 'bcmCosqControlShaperPacketMode' on bcm_dnx_cosq_control_set";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_initial_packet_mode].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_sub_calendar_ifc].name = "sub_calendar_ifc";
    submodule_data->defines[dnx_data_egr_queuing_params_define_sub_calendar_ifc].doc = "special ifc id for the sub calendar";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_sub_calendar_ifc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_txq_max_credits].name = "txq_max_credits";
    submodule_data->defines[dnx_data_egr_queuing_params_define_txq_max_credits].doc = "max credits allowed according to ESB capacity";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_txq_max_credits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_pqp_ifc_almost_empty_th].name = "pqp_ifc_almost_empty_th";
    submodule_data->defines[dnx_data_egr_queuing_params_define_pqp_ifc_almost_empty_th].doc = "pqp ifc almost empty threshold.";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_pqp_ifc_almost_empty_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_pqp_port_almost_empty_th].name = "pqp_port_almost_empty_th";
    submodule_data->defines[dnx_data_egr_queuing_params_define_pqp_port_almost_empty_th].doc = "pqp port almost empty threshold.";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_pqp_port_almost_empty_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_pqp_queue_almost_empty_th].name = "pqp_queue_almost_empty_th";
    submodule_data->defines[dnx_data_egr_queuing_params_define_pqp_queue_almost_empty_th].doc = "pqp queue almost empty threshold.";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_pqp_queue_almost_empty_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_if_min_gap_priority_size].name = "if_min_gap_priority_size";
    submodule_data->defines[dnx_data_egr_queuing_params_define_if_min_gap_priority_size].doc = "size in bits of ifc min priority gap";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_if_min_gap_priority_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_params_define_invalid_otm_port].name = "invalid_otm_port";
    submodule_data->defines[dnx_data_egr_queuing_params_define_invalid_otm_port].doc = "tm port that is marked as invalid (per core)";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_invalid_otm_port].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_egr_queuing_params_define_burst_size_below_mtu].name = "burst_size_below_mtu";
    submodule_data->defines[dnx_data_egr_queuing_params_define_burst_size_below_mtu].doc = "indicated whether burst size can be configured bellow MTU";
    
    submodule_data->defines[dnx_data_egr_queuing_params_define_burst_size_below_mtu].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_egr_queuing_params_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data egr_queuing params tables");

    
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].name = "if_speed_params";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].doc = "per port speed thresholds";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].size_of_values = sizeof(dnx_data_egr_queuing_params_if_speed_params_t);
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].entry_get = dnx_data_egr_queuing_params_if_speed_params_entry_str_get;

    
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].nof_keys = 2;
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].keys[0].name = "idx";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].keys[0].doc = "table index";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].keys[1].name = "clock_rate_idx";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params].keys[1].doc = "index to table based on the clock rate";

    
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
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].doc = "emr fifo parameters";
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].size_of_values = sizeof(dnx_data_egr_queuing_params_emr_fifo_t);
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].entry_get = dnx_data_egr_queuing_params_emr_fifo_entry_str_get;

    
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].nof_keys = 2;
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].keys[0].name = "idx";
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].keys[0].doc = "table index";
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].keys[1].name = "tdm";
    submodule_data->tables[dnx_data_egr_queuing_params_table_emr_fifo].keys[1].doc = "tdm exist in the system";

    
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
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].doc = "clock rate thresholds in Khz, used as input to if_speed_params";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].size_of_values = sizeof(dnx_data_egr_queuing_params_if_speed_params_clk_th_t);
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].entry_get = dnx_data_egr_queuing_params_if_speed_params_clk_th_entry_str_get;

    
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].nof_keys = 1;
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].keys[0].name = "idx";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].keys[0].doc = "table index";

    
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].values, dnxc_data_value_t, submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].nof_values, "_dnx_data_egr_queuing_params_table_if_speed_params_clk_th table values");
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].values[0].name = "max_clock_rate_th";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].values[0].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].values[0].doc = "clock rate thresholds in Khz";
    submodule_data->tables[dnx_data_egr_queuing_params_table_if_speed_params_clk_th].values[0].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_params_if_speed_params_clk_th_t, max_clock_rate_th);


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
dnx_data_egr_queuing_params_nof_egr_ch_interfaces_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing, dnx_data_egr_queuing_submodule_params, dnx_data_egr_queuing_params_define_nof_egr_ch_interfaces);
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






static shr_error_e
dnx_data_egr_queuing_common_max_val_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "common_max_val";
    submodule_data->doc = "Common max values over all calendar types";
    
    submodule_data->nof_features = _dnx_data_egr_queuing_common_max_val_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data egr_queuing common_max_val features");

    
    submodule_data->nof_defines = _dnx_data_egr_queuing_common_max_val_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data egr_queuing common_max_val defines");

    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_nof_bits_in_calendar_size].name = "nof_bits_in_calendar_size";
    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_nof_bits_in_calendar_size].doc = "Maximal number of bits representing any calendar size. Essentially, this is log2(calendar_size)";
    
    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_nof_bits_in_calendar_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_calendar_size].name = "calendar_size";
    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_calendar_size].doc = "Maximal size over all calendar sizes. Essentially, this is 2^nof_bits_in_calendar_size";
    
    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_calendar_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_slow_port_speed].name = "slow_port_speed";
    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_slow_port_speed].doc = "Maximal speed of ports that will be added to the sub calendar. units: Mbps";
    
    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_slow_port_speed].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_mux_calendar_resolution].name = "mux_calendar_resolution";
    submodule_data->defines[dnx_data_egr_queuing_common_max_val_define_mux_calendar_resolution].doc = "Resolution of the MUX calendars. units: Mbps";
    
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
    submodule_data->doc = "egress rate measure mechanism. calculates egress rate per EGQ if for various uses";
    
    submodule_data->nof_features = _dnx_data_egr_queuing_rate_measurement_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data egr_queuing rate_measurement features");

    submodule_data->features[dnx_data_egr_queuing_rate_measurement_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_egr_queuing_rate_measurement_is_supported].doc = "support egress rate measure mechanism";
    submodule_data->features[dnx_data_egr_queuing_rate_measurement_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_egr_queuing_rate_measurement_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data egr_queuing rate_measurement defines");

    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_max_interval].name = "max_interval";
    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_max_interval].doc = "Max interval value for egress rate measure [usec]";
    
    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_max_interval].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_promile_factor].name = "promile_factor";
    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_promile_factor].doc = "promile calculation factor for egress rate measure";
    
    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_promile_factor].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_max_granularity].name = "max_granularity";
    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_max_granularity].doc = "max granularity value";
    
    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_max_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_min_bytes_for_granularity].name = "min_bytes_for_granularity";
    submodule_data->defines[dnx_data_egr_queuing_rate_measurement_define_min_bytes_for_granularity].doc = "represent the minimum number of bytes to update the rate counter in a single access. if the calculated promile is lower than this value, granularity should increase";
    
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
    submodule_data->doc = "phantom queues feature related info. phantom queues use egr_rate so the info is only used in devices supporting egr_rate feature.";
    
    submodule_data->nof_features = _dnx_data_egr_queuing_phantom_queues_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data egr_queuing phantom_queues features");

    
    submodule_data->nof_defines = _dnx_data_egr_queuing_phantom_queues_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data egr_queuing phantom_queues defines");

    submodule_data->defines[dnx_data_egr_queuing_phantom_queues_define_nof_threshold_profiles].name = "nof_threshold_profiles";
    submodule_data->defines[dnx_data_egr_queuing_phantom_queues_define_nof_threshold_profiles].doc = "number of phantom queues threshold profiles";
    
    submodule_data->defines[dnx_data_egr_queuing_phantom_queues_define_nof_threshold_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_phantom_queues_define_max_threshold].name = "max_threshold";
    submodule_data->defines[dnx_data_egr_queuing_phantom_queues_define_max_threshold].doc = "max value for phantom queues threshold";
    
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
    submodule_data->doc = "egress reassembly";
    
    submodule_data->nof_features = _dnx_data_egr_queuing_reassembly_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data egr_queuing reassembly features");

    submodule_data->features[dnx_data_egr_queuing_reassembly_prs].name = "prs";
    submodule_data->features[dnx_data_egr_queuing_reassembly_prs].doc = "pairs reassembly stage is part of the pipe line";
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
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

